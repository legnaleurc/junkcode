"""
Synology File Station listing API wrappers.
"""

from collections.abc import AsyncIterator
from typing import Any

from .._lib import SynologyFileDict
from .._network import Network


async def get_root(network: Network) -> SynologyFileDict:
    """
    Get the root folder information.

    Args:
        network: Network instance

    Returns:
        Root folder data

    Reference:
        SYNO.FileStation.List API - list with folder_path="/"
    """
    base_url = network._auth.base_url
    url = f"{base_url}/webapi/entry.cgi"

    query = {
        "api": "SYNO.FileStation.List",
        "version": "2",
        "method": "list",
        "folder_path": "/",
        "limit": 1,
        "offset": 0,
        "additional": '["time","size","owner","real_path"]',
    }

    async with network.fetch("GET", url, query=query) as response:
        data = await response.json()

        if not data.get("success"):
            error = data.get("error", {})
            raise Exception(f"Failed to get root: {error}")

        # Return root folder info
        # For root, we'll construct a special entry
        return SynologyFileDict(
            id="/",
            name="",
            parent_id=None,
            is_folder=True,
            trashed=False,
            created_time="1970-01-01T00:00:00Z",
            modified_time="1970-01-01T00:00:00Z",
            mime_type="application/x-folder",
            size=0,
        )


async def list_all_files(
    network: Network,
    folder_path: str = "/",
    limit: int = 1000,
) -> AsyncIterator[list[SynologyFileDict]]:
    """
    List all files recursively starting from a folder.

    Args:
        network: Network instance
        folder_path: Starting folder path (default: root "/")
        limit: Number of items per page

    Yields:
        Lists of file/folder data

    Reference:
        SYNO.FileStation.List API with recursive listing
    """
    base_url = network._auth.base_url
    url = f"{base_url}/webapi/entry.cgi"

    offset = 0

    while True:
        query = {
            "api": "SYNO.FileStation.List",
            "version": "2",
            "method": "list",
            "folder_path": folder_path,
            "limit": limit,
            "offset": offset,
            "additional": '["time","size","owner","real_path","type"]',
        }

        async with network.fetch("GET", url, query=query) as response:
            data = await response.json()

            if not data.get("success"):
                error = data.get("error", {})
                error_code = error.get("code")
                # Error code 408 means no more files
                if error_code == 408:
                    break
                raise Exception(f"Failed to list files: {error}")

            result_data = data.get("data", {})
            files = result_data.get("files", [])

            if not files:
                break

            # Convert to SynologyFileDict format
            converted_files: list[SynologyFileDict] = []
            for file_info in files:
                converted = _convert_file_info(file_info, folder_path)
                converted_files.append(converted)

                # If it's a folder, recursively list its contents
                if converted["is_folder"]:
                    file_path = file_info.get("path", "")
                    async for sub_files in list_all_files(network, file_path, limit):
                        yield sub_files

            if converted_files:
                yield converted_files

            # Check if there are more files
            total = result_data.get("total", 0)
            offset += limit
            if offset >= total:
                break


async def list_folder(
    network: Network,
    folder_path: str,
    offset: int = 0,
    limit: int = 1000,
) -> dict[str, Any]:
    """
    List contents of a specific folder (non-recursive).

    Args:
        network: Network instance
        folder_path: Folder path to list
        offset: Pagination offset
        limit: Number of items to return

    Returns:
        Response data with files list

    Reference:
        SYNO.FileStation.List API
    """
    base_url = network._auth.base_url
    url = f"{base_url}/webapi/entry.cgi"

    query = {
        "api": "SYNO.FileStation.List",
        "version": "2",
        "method": "list",
        "folder_path": folder_path,
        "limit": limit,
        "offset": offset,
        "additional": '["time","size","owner","real_path","type"]',
    }

    async with network.fetch("GET", url, query=query) as response:
        data = await response.json()

        if not data.get("success"):
            error = data.get("error", {})
            raise Exception(f"Failed to list folder: {error}")

        return data.get("data", {})


async def get_trash(network: Network) -> list[SynologyFileDict]:
    """
    Get trashed files.

    Note: Synology File Station doesn't have a built-in trash API.
    This is a placeholder that returns an empty list.
    Trashed files might be in a #recycle folder.

    Args:
        network: Network instance

    Returns:
        List of trashed files
    """
    # Synology doesn't have a standard trash API
    # Typically trashed files are in #recycle folder
    # For now, return empty list
    return []


def _convert_file_info(file_info: dict[str, Any], parent_path: str) -> SynologyFileDict:
    """
    Convert Synology File Station file info to SynologyFileDict.

    Args:
        file_info: Raw file info from API
        parent_path: Parent folder path

    Returns:
        Standardized file dict
    """
    # Extract basic info
    path = file_info.get("path", "")
    name = file_info.get("name", "")
    is_folder = file_info.get("isdir", False)

    # Extract time info
    additional = file_info.get("additional", {})
    time_info = additional.get("time", {})
    created_time = time_info.get("crtime", 0)
    modified_time = time_info.get("mtime", 0)

    # Convert timestamps to ISO format
    from datetime import UTC, datetime

    created_dt = datetime.fromtimestamp(created_time, UTC) if created_time else datetime.now(UTC)
    modified_dt = datetime.fromtimestamp(modified_time, UTC) if modified_time else datetime.now(UTC)

    # Extract size
    size = file_info.get("additional", {}).get("size", 0)

    # Determine mime type
    mime_type = "application/x-folder" if is_folder else file_info.get("type", "application/octet-stream")

    return SynologyFileDict(
        id=path,  # Use path as ID
        name=name,
        parent_id=parent_path if parent_path != path else None,
        is_folder=is_folder,
        trashed=False,  # Synology doesn't track this in standard API
        created_time=created_dt.isoformat(),
        modified_time=modified_dt.isoformat(),
        mime_type=mime_type,
        size=size,
    )
