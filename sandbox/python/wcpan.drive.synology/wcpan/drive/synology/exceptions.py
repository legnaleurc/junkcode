"""
Custom exceptions for Synology Drive integration.
"""

from wcpan.drive.core.exceptions import DriveError


__all__ = (
    "SynologyAuthenticationError",
    "SynologySessionExpiredError",
    "SynologyApiError",
    "SynologyUploadError",
    "SynologyRateLimitError",
    "SynologyNetworkError",
)


class SynologyAuthenticationError(DriveError):
    """Raised when authentication with Synology DSM fails."""

    def __init__(self, message: str = "Authentication failed") -> None:
        super().__init__(message)


class SynologySessionExpiredError(DriveError):
    """Raised when the session token has expired."""

    def __init__(self, message: str = "Session expired") -> None:
        super().__init__(message)


class SynologyApiError(DriveError):
    """Raised when Synology API returns an error."""

    def __init__(
        self, message: str, error_code: int | None = None, error_data: dict | None = None
    ) -> None:
        super().__init__(message)
        self.error_code = error_code
        self.error_data = error_data


class SynologyUploadError(DriveError):
    """Raised when file upload fails."""

    def __init__(self, message: str, file_name: str | None = None) -> None:
        super().__init__(message)
        self.file_name = file_name


class SynologyRateLimitError(DriveError):
    """Raised when rate limit is exceeded."""

    def __init__(self, message: str = "Rate limit exceeded", retry_after: int | None = None) -> None:
        super().__init__(message)
        self.retry_after = retry_after


class SynologyNetworkError(DriveError):
    """Raised when a network error occurs."""

    def __init__(self, message: str, original_error: Exception | None = None) -> None:
        super().__init__(message)
        self.original_error = original_error
