"""
Synology File Station API wrappers.

Note: Authentication is handled by AuthManager in _auth.py, not here.
The auth module was removed to avoid circular dependencies since
Network depends on AuthManager but API wrappers depend on Network.
"""

from . import files, folders, listing

__all__ = (
    "files",
    "folders",
    "listing",
)
