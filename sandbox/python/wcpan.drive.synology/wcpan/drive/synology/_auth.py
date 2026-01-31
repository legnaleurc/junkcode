"""
Authentication manager for Synology DSM.

This module is self-contained and makes HTTP requests directly
(not via Network) to avoid circular dependencies:
- Network depends on AuthManager
- AuthManager needs to login before Network can be used

This follows the same pattern as OAuth2Manager in wcpan.drive.google.
"""

import asyncio
from typing import Any

from aiohttp import ClientSession

from .exceptions import SynologyAuthenticationError, SynologySessionExpiredError


class AuthManager:
    """
    Manages authentication with Synology DSM.
    Handles login, logout, and session token management.
    """

    def __init__(
        self,
        username: str,
        password: str,
        base_url: str,
        otp_code: str | None = None,
    ) -> None:
        """
        Initialize the authentication manager.

        Args:
            username: Synology DSM username
            password: Synology DSM password
            base_url: Base URL of the Synology NAS (e.g., "https://nas.example.com:5001")
            otp_code: Optional OTP code for 2FA
        """
        self._username = username
        self._password = password
        self._base_url = base_url.rstrip("/")
        self._otp_code = otp_code
        self._session_token: str | None = None
        self._lock = asyncio.Lock()

    async def login(self, session: ClientSession) -> None:
        """
        Login to Synology DSM and obtain a session token.

        Args:
            session: aiohttp ClientSession

        Raises:
            SynologyAuthenticationError: If login fails
        """
        async with self._lock:
            # Build login request
            url = f"{self._base_url}/webapi/auth.cgi"
            params: dict[str, Any] = {
                "api": "SYNO.API.Auth",
                "version": "3",
                "method": "login",
                "account": self._username,
                "passwd": self._password,
                "session": "FileStation",
                "format": "cookie",
            }

            if self._otp_code:
                params["otp_code"] = self._otp_code

            # Make request
            async with session.get(url, params=params) as response:
                if response.status != 200:
                    raise SynologyAuthenticationError(
                        f"Login request failed with status {response.status}"
                    )

                data = await response.json()

                # Check for API errors
                if not data.get("success", False):
                    error = data.get("error", {})
                    error_code = error.get("code", "unknown")
                    raise SynologyAuthenticationError(
                        f"Login failed with error code: {error_code}"
                    )

                # Extract session token from response data
                session_data = data.get("data", {})
                self._session_token = session_data.get("sid")

                if not self._session_token:
                    raise SynologyAuthenticationError("No session token in response")

    async def logout(self, session: ClientSession) -> None:
        """
        Logout from Synology DSM and invalidate the session token.

        Args:
            session: aiohttp ClientSession
        """
        async with self._lock:
            if not self._session_token:
                return

            url = f"{self._base_url}/webapi/auth.cgi"
            params = {
                "api": "SYNO.API.Auth",
                "version": "3",
                "method": "logout",
                "session": "FileStation",
            }

            try:
                async with session.get(url, params=params) as response:
                    # Ignore errors during logout
                    pass
            finally:
                self._session_token = None

    def get_session_token(self) -> str:
        """
        Get the current session token.

        Returns:
            The session token

        Raises:
            SynologySessionExpiredError: If no session token is available
        """
        if not self._session_token:
            raise SynologySessionExpiredError("No session token available")
        return self._session_token

    async def ensure_authenticated(self, session: ClientSession) -> None:
        """
        Ensure that we have a valid session token.
        If not, perform login.

        Args:
            session: aiohttp ClientSession
        """
        if not self._session_token:
            await self.login(session)

    def is_authenticated(self) -> bool:
        """
        Check if we have a session token.

        Returns:
            True if authenticated, False otherwise
        """
        return self._session_token is not None

    @property
    def base_url(self) -> str:
        """Get the base URL."""
        return self._base_url
