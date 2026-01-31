"""
Synology File Station folder operations API wrappers.
"""

from typing import Any

from .._lib import SynologyFileDict
from .._network import Network


async def create_folder(
    network: Network,
    name: str,
    parent_path: str,
) -> SynologyFileDict:
    """
    Create a new folder.

    Args:
        network: Network instance
        name: Folder name
        parent_path: Parent folder path

    Returns:
        Created folder data

    Reference:
        SYNO.FileStation.CreateFolder API
    """
    base_url = network._auth.base_url
    url = f"{base_url}/webapi/entry.cgi"

    # Construct full folder path
    if parent_path == "/":
        folder_path = f"/{name}"
    else:
        folder_path = f"{parent_path.rstrip('/')}/{name}"

    query = {
        "api": "SYNO.FileStation.CreateFolder",
        "version": "2",
        "method": "create",
        "folder_path": parent_path,
        "name": name,
    }

    async with network.fetch("GET", url, query=query) as response:
        data = await response.json()

        if not data.get("success"):
            error = data.get("error", {})
            raise Exception(f"Failed to create folder: {error}")

        # Get folder info from response
        result_data = data.get("data", {})
        folders = result_data.get("folders", [])

        if folders:
            folder_info = folders[0]
            return _convert_folder_info(folder_info, parent_path)

        # If no folder info in response, construct it
        from datetime import UTC, datetime

        now = datetime.now(UTC)
        return SynologyFileDict(
            id=folder_path,
            name=name,
            parent_id=parent_path,
            is_folder=True,
            trashed=False,
            created_time=now.isoformat(),
            modified_time=now.isoformat(),
            mime_type="application/x-folder",
            size=0,
        )


async def get_folder_info(
    network: Network,
    folder_path: str,
) -> SynologyFileDict:
    """
    Get information about a specific folder.

    Args:
        network: Network instance
        folder_path: Path to the folder

    Returns:
        Folder data

    Reference:
        SYNO.FileStation.List API (get info for specific path)
    """
    base_url = network._auth.base_url
    url = f"{base_url}/webapi/entry.cgi"

    query = {
        "api": "SYNO.FileStation.List",
        "version": "2",
        "method": "getinfo",
        "path": folder_path,
        "additional": '["time","size","owner","real_path","type"]',
    }

    async with network.fetch("GET", url, query=query) as response:
        data = await response.json()

        if not data.get("success"):
            error = data.get("error", {})
            raise Exception(f"Failed to get folder info: {error}")

        result_data = data.get("data", {})
        files = result_data.get("files", [])

        if not files:
            raise Exception(f"Folder not found: {folder_path}")

        # Extract parent path from full path
        parent_path = "/".join(folder_path.rstrip("/").split("/")[:-1]) or "/"

        return _convert_folder_info(files[0], parent_path)


async def list_folder(
    network: Network,
    folder_path: str,
    offset: int = 0,
    limit: int = 1000,
) -> dict[str, Any]:
    """
    List contents of a folder.

    Args:
        network: Network instance
        folder_path: Path to the folder
        offset: Pagination offset
        limit: Number of items per page

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
        "offset": offset,
        "limit": limit,
        "additional": '["time","size","owner","real_path","type"]',
    }

    async with network.fetch("GET", url, query=query) as response:
        data = await response.json()

        if not data.get("success"):
            error = data.get("error", {})
            raise Exception(f"Failed to list folder: {error}")

        return data.get("data", {})


async def rename_folder(
    network: Network,
    folder_path: str,
    new_name: str,
) -> SynologyFileDict:
    """
    Rename a folder.

    Args:
        network: Network instance
        folder_path: Current folder path
        new_name: New folder name

    Returns:
        Updated folder data

    Reference:
        SYNO.FileStation.Rename API
    """
    base_url = network._auth.base_url
    url = f"{base_url}/webapi/entry.cgi"

    query = {
        "api": "SYNO.FileStation.Rename",
        "version": "2",
        "method": "rename",
        "path": folder_path,
        "name": new_name,
    }

    async with network.fetch("GET", url, query=query) as response:
        data = await response.json()

        if not data.get("success"):
            error = data.get("error", {})
            raise Exception(f"Failed to rename folder: {error}")

        # Get updated folder info
        parent_path = "/".join(folder_path.rstrip("/").split("/")[:-1]) or "/"
        new_path = f"{parent_path.rstrip('/')}/{new_name}"

        return await get_folder_info(network, new_path)


async def delete_folder(
    network: Network,
    folder_path: str,
    recursive: bool = True,
) -> None:
    """
    Delete a folder.

    Args:
        network: Network instance
        folder_path: Path to the folder to delete
        recursive: Whether to delete recursively (default: True)

    Reference:
        SYNO.FileStation.Delete API
    """
    base_url = network._auth.base_url
    url = f"{base_url}/webapi/entry.cgi"

    query = {
        "api": "SYNO.FileStation.Delete",
        "version": "2",
        "method": "delete",
        "path": folder_path,
        "recursive": "true" if recursive else "false",
    }

    async with network.fetch("GET", url, query=query) as response:
        data = await response.json()

        if not data.get("success"):
            error = data.get("error", {})
            raise Exception(f"Failed to delete folder: {error}")


def _convert_folder_info(folder_info: dict[str, Any], parent_path: str) -> SynologyFileDict:
    """
    Convert Synology folder info to SynologyFileDict.

    Args:
        folder_info: Raw folder info from API
        parent_path: Parent folder path

    Returns:
        Standardized folder dict
    """
    from datetime import UTC, datetime

    path = folder_info.get("path", "")
    name = folder_info.get("name", "")

    # Extract time info
    additional = folder_info.get("additional", {})
    time_info = additional.get("time", {})
    created_time = time_info.get("crtime", 0)
    modified_time = time_info.get("mtime", 0)

    # Convert timestamps to ISO format
    created_dt = datetime.fromtimestamp(created_time, UTC) if created_time else datetime.now(UTC)
    modified_dt = datetime.fromtimestamp(modified_time, UTC) if modified_time else datetime.now(UTC)

    return SynologyFileDict(
        id=path,
        name=name,
        parent_id=parent_path,
        is_folder=True,
        trashed=False,
        created_time=created_dt.isoformat(),
        modified_time=modified_dt.isoformat(),
        mime_type="application/x-folder",
        size=0,
    )
