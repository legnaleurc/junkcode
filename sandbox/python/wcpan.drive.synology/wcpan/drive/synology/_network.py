"""
Network layer with retry and backoff logic for Synology API.
"""

import asyncio
import math
import random
from collections.abc import AsyncIterable, AsyncIterator
from contextlib import asynccontextmanager
from logging import getLogger
from typing import Any, NotRequired, TypedDict

from aiohttp import ClientResponse, ClientSession, ContentTypeError

from .exceptions import (
    SynologyApiError,
    SynologyNetworkError,
    SynologyRateLimitError,
    SynologySessionExpiredError,
)


_BACKOFF_FACTOR = 2
_BACKOFF_MAX_TIMEOUT = 60


type QueryDict = dict[str, int | bool | str]
type ReadableContent = bytes | AsyncIterable[bytes]


class _FetchParams(TypedDict):
    url: str
    method: str
    headers: dict[str, str]
    params: NotRequired[dict[str, Any]]
    data: NotRequired[ReadableContent]
    timeout: NotRequired[None]


class Network:
    """
    Network layer that handles:
    1. Session token management
    2. Automatic retry on errors
    3. Exponential backoff on rate limits
    """

    def __init__(self, session: ClientSession, auth_manager: Any) -> None:
        """
        Initialize the network layer.

        Args:
            session: aiohttp ClientSession
            auth_manager: AuthManager instance for session token management
        """
        from ._auth import AuthManager

        self._session = session
        self._auth: AuthManager = auth_manager
        self._backoff = BackoffController()

    @asynccontextmanager
    async def fetch(
        self,
        method: str,
        url: str,
        *,
        query: QueryDict | None = None,
        headers: dict[str, str] | None = None,
        body: ReadableContent | None = None,
        timeout: bool = True,
    ) -> AsyncIterator[ClientResponse]:
        """
        Perform HTTP request with retry logic.

        Args:
            method: HTTP method (GET, POST, etc.)
            url: Request URL
            query: Query parameters
            headers: HTTP headers
            body: Request body
            timeout: Whether to use timeout (False for uploads/downloads)

        Yields:
            ClientResponse object
        """
        kwargs = _prepare_kwargs(
            method=method,
            url=url,
            query=query,
            headers=headers,
            body=body,
            timeout=timeout,
        )

        async with self._retry_fetch(kwargs) as response:
            yield response

    @asynccontextmanager
    async def _retry_fetch(self, kwargs: _FetchParams) -> AsyncIterator[ClientResponse]:
        """
        Send request with retry logic for:
        1. Session token expiration (401)
        2. Rate limiting (429, some 403)
        3. Server errors (5xx)
        """
        while True:
            # Wait for backoff if needed
            await self._backoff.wait()

            # Add session token to request
            await self._add_session_token(kwargs)

            try:
                async with self._session.request(**kwargs) as response:
                    # Success (2xx, 3xx)
                    if response.status < 400:
                        self._backoff.decrease()
                        yield response
                        return

                    # Server errors (5xx) - retry with backoff
                    if response.status >= 500:
                        await _handle_5xx(response)
                        self._backoff.increase()
                        continue

                    # Unauthorized (401) - session expired, re-login
                    if response.status == 401:
                        getLogger(__name__).warning("Session expired, re-authenticating")
                        await self._auth.login(self._session)
                        continue

                    # Forbidden (403) or Too Many Requests (429) - rate limit
                    if response.status in (403, 429):
                        await _handle_rate_limit(response)
                        self._backoff.increase()
                        continue

                    # Other client errors (4xx) - don't retry
                    await _handle_4xx(response)
                    return

            except asyncio.TimeoutError as e:
                getLogger(__name__).error("Request timeout")
                raise SynologyNetworkError("Request timeout", e)
            except Exception as e:
                getLogger(__name__).error(f"Network error: {e}")
                raise SynologyNetworkError(f"Network error: {e}", e)

    async def _add_session_token(self, kwargs: _FetchParams) -> None:
        """
        Add session token to request parameters.
        Synology uses _sid cookie or query parameter for authentication.
        """
        try:
            token = self._auth.get_session_token()
            # Add as query parameter
            if "params" not in kwargs:
                kwargs["params"] = {}
            kwargs["params"]["_sid"] = token
        except SynologySessionExpiredError:
            # Will trigger re-login in retry logic
            pass


class BackoffController:
    """Exponential backoff controller for rate limiting."""

    def __init__(self) -> None:
        self._level = 0

    async def wait(self) -> None:
        """Wait for backoff duration."""
        if self._level <= 0:
            return

        # Exponential backoff with jitter
        seed = random.random()
        power = 2**self._level
        s_delay = math.floor(seed * power * _BACKOFF_FACTOR)
        s_delay = min(_BACKOFF_MAX_TIMEOUT, s_delay)

        getLogger(__name__).debug(f"Backing off for {s_delay} seconds")
        await asyncio.sleep(s_delay)

    def increase(self) -> None:
        """Increase backoff level."""
        self._level = min(self._level + 2, 10)

    def decrease(self) -> None:
        """Decrease backoff level on successful request."""
        self._level = max(self._level - 1, 0)


def _prepare_kwargs(
    *,
    method: str,
    url: str,
    query: QueryDict | None,
    headers: dict[str, str] | None,
    body: ReadableContent | None,
    timeout: bool,
) -> _FetchParams:
    """Prepare request parameters."""
    kwargs: _FetchParams = {
        "method": method,
        "url": url,
        "headers": {} if headers is None else headers,
    }

    if query is not None:
        kwargs["params"] = _normalize_query_dict(query)

    if body is not None:
        kwargs["data"] = body

    # Disable timeout for uploads/downloads
    if not timeout:
        kwargs["timeout"] = None

    return kwargs


def _normalize_query_dict(qs: QueryDict) -> dict[str, str]:
    """Convert query dict values to strings."""
    result: dict[str, str] = {}
    for key, value in qs.items():
        if isinstance(value, bool):
            result[key] = "true" if value else "false"
        elif isinstance(value, int):
            result[key] = str(value)
        else:
            result[key] = value
    return result


async def _handle_rate_limit(response: ClientResponse) -> None:
    """Handle rate limit errors."""
    getLogger(__name__).warning(f"Rate limit hit (status {response.status})")

    # Try to parse retry-after header
    retry_after = response.headers.get("Retry-After")
    if retry_after:
        try:
            retry_seconds = int(retry_after)
            getLogger(__name__).info(f"Retry after {retry_seconds} seconds")
        except ValueError:
            pass

    # Check if it's actually a rate limit error
    try:
        data = await response.json()
        error = data.get("error", {})
        error_code = error.get("code")

        if error_code:
            getLogger(__name__).error(f"API error code: {error_code}")

        # If it's not a rate limit error, raise exception
        # Common Synology error codes for rate limiting: 119 (too many requests)
        if error_code and error_code not in (119, 429):
            raise SynologyApiError(
                f"API error: {error.get('message', 'Unknown error')}",
                error_code=error_code,
            )
    except ContentTypeError:
        # Not JSON response, treat as rate limit
        pass


async def _handle_4xx(response: ClientResponse) -> None:
    """Handle client errors (4xx)."""
    getLogger(__name__).error(f"Client error: {response.status}")
    await _report_error(response)
    response.raise_for_status()


async def _handle_5xx(response: ClientResponse) -> None:
    """Handle server errors (5xx)."""
    getLogger(__name__).error(f"Server error: {response.status}")
    await _report_error(response)


async def _report_error(response: ClientResponse) -> None:
    """Log error details from response."""
    try:
        data = await response.json()
        getLogger(__name__).error(f"Error response: {data}")

        # Parse Synology error format
        if "error" in data:
            error = data["error"]
            error_code = error.get("code")
            error_message = error.get("message", "Unknown error")
            getLogger(__name__).error(f"Synology error {error_code}: {error_message}")

    except ContentTypeError:
        # Not JSON, try text
        try:
            text = await response.text()
            getLogger(__name__).error(f"Error response (text): {text}")
        except Exception as e:
            getLogger(__name__).error(f"Could not parse error response: {e}")
