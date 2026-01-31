"""
Synology Drive Web API integration for wcpan.drive.

This package provides:
- FileService: Remote file operations via Synology Drive API
- SnapshotService: Local SQLite-based cache for offline access

Example usage:

    from wcpan.drive.synology import create_file_service, create_snapshot_service

    # Remote file service
    async with create_file_service(
        username="your_username",
        password="your_password",
        base_url="https://nas.example.com:5001",
    ) as file_service:
        root = await file_service.get_root()
        print(f"Root: {root}")

    # Local snapshot service
    async with create_snapshot_service("./cache.db") as snapshot:
        root = await snapshot.get_root()
        print(f"Cached root: {root}")
"""

from ._service import create_file_service as create_file_service
from ._snapshot_service import create_snapshot_service as create_snapshot_service

__version__ = "1.0.0"

__all__ = (
    "create_file_service",
    "create_snapshot_service",
    "__version__",
)
