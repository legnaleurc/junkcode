"""
Hash calculator for file integrity checking.
"""

import hashlib
from collections.abc import Buffer
from typing import Protocol, Self, override

from wcpan.drive.core.types import Hasher


class BuiltinHasher(Protocol):
    """Protocol for Python's built-in hashlib hashers."""

    def update(self, data: Buffer, /) -> None: ...

    def hexdigest(self) -> str: ...

    def digest(self) -> bytes: ...

    def copy(self) -> Self: ...


async def create_hasher() -> Hasher:
    """
    Create a hasher instance.

    Returns:
        MD5 hasher for Synology compatibility

    Note:
        Synology File Station uses MD5 for file checksums.
        Could be changed to SHA256 if needed.
    """
    return Md5Hasher(hashlib.md5())


class Md5Hasher(Hasher):
    """MD5 hash calculator."""

    def __init__(self, hasher: BuiltinHasher) -> None:
        self._hasher = hasher

    @override
    async def update(self, data: bytes) -> None:
        """Update the hash with new data."""
        self._hasher.update(data)

    @override
    async def hexdigest(self) -> str:
        """Get the hex digest of the hash."""
        return self._hasher.hexdigest()

    @override
    async def digest(self) -> bytes:
        """Get the binary digest of the hash."""
        return self._hasher.digest()

    @override
    async def copy(self) -> Self:
        """Create a copy of the hasher."""
        hasher = self._hasher.copy()
        return self.__class__(hasher)
