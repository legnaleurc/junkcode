"""
Synology File Station file operations API wrappers.
"""

from collections.abc import AsyncIterable, AsyncIterator
from typing import Any

from aiohttp import ClientResponse, FormData

from .._lib import SynologyFileDict
from .._network import Network


async def get_file_info(
    network: Network,
    file_path: str,
) -> SynologyFileDict:
    """
    Get information about a specific file.

    Args:
        network: Network instance
        file_path: Path to the file

    Returns:
        File data

    Reference:
        SYNO.FileStation.List API (getinfo method)
    """
    base_url = network._auth.base_url
    url = f"{base_url}/webapi/entry.cgi"

    query = {
        "api": "SYNO.FileStation.List",
        "version": "2",
        "method": "getinfo",
        "path": file_path,
        "additional": '["time","size","owner","real_path","type"]',
    }

    async with network.fetch("GET", url, query=query) as response:
        data = await response.json()

        if not data.get("success"):
            error = data.get("error", {})
            raise Exception(f"Failed to get file info: {error}")

        result_data = data.get("data", {})
        files = result_data.get("files", [])

        if not files:
            raise Exception(f"File not found: {file_path}")

        # Extract parent path from full path
        parent_path = "/".join(file_path.rstrip("/").split("/")[:-1]) or "/"

        return _convert_file_info(files[0], parent_path)


async def download(
    network: Network,
    file_path: str,
    range_: tuple[int, int] | None = None,
) -> AsyncIterator[ClientResponse]:
    """
    Download a file.

    Args:
        network: Network instance
        file_path: Path to the file to download
        range_: Optional byte range (start, end)

    Yields:
        ClientResponse for streaming download

    Reference:
        SYNO.FileStation.Download API
    """
    base_url = network._auth.base_url
    url = f"{base_url}/webapi/entry.cgi"

    query = {
        "api": "SYNO.FileStation.Download",
        "version": "2",
        "method": "download",
        "path": file_path,
        "mode": "download",
    }

    headers = {}
    if range_:
        start, end = range_
        headers["Range"] = f"bytes={start}-{end}"

    async with network.fetch("GET", url, query=query, headers=headers, timeout=False) as response:
        if not response.ok:
            raise Exception(f"Failed to download file: status {response.status}")

        yield response


async def upload_file(
    network: Network,
    parent_path: str,
    name: str,
    data: AsyncIterable[bytes],
    size: int,
    mime_type: str | None = None,
) -> SynologyFileDict:
    """
    Upload a file.

    Args:
        network: Network instance
        parent_path: Parent folder path
        name: File name
        data: File data as async iterable
        size: File size in bytes
        mime_type: Optional MIME type

    Returns:
        Uploaded file data

    Reference:
        SYNO.FileStation.Upload API
    """
    base_url = network._auth.base_url
    url = f"{base_url}/webapi/entry.cgi"

    # Build form data
    form = FormData()
    form.add_field("api", "SYNO.FileStation.Upload")
    form.add_field("version", "2")
    form.add_field("method", "upload")
    form.add_field("path", parent_path)
    form.add_field("create_parents", "true")
    form.add_field("overwrite", "true")

    # Add file field
    # For streaming, we need to collect data first
    # TODO: Optimize for streaming upload if Synology supports it
    file_content = b""
    async for chunk in data:
        file_content += chunk

    content_type = mime_type or "application/octet-stream"
    form.add_field(
        "file",
        file_content,
        filename=name,
        content_type=content_type,
    )

    async with network.fetch("POST", url, body=form, timeout=False) as response:
        result = await response.json()

        if not result.get("success"):
            error = result.get("error", {})
            raise Exception(f"Failed to upload file: {error}")

        # Get uploaded file info
        file_path = f"{parent_path.rstrip('/')}/{name}"
        return await get_file_info(network, file_path)


async def delete_file(
    network: Network,
    file_path: str,
    permanent: bool = True,
) -> None:
    """
    Delete a file.

    Args:
        network: Network instance
        file_path: Path to the file to delete
        permanent: Whether to delete permanently (default: True)
                  Note: Synology may move to #recycle instead

    Reference:
        SYNO.FileStation.Delete API
    """
    base_url = network._auth.base_url
    url = f"{base_url}/webapi/entry.cgi"

    query = {
        "api": "SYNO.FileStation.Delete",
        "version": "2",
        "method": "delete",
        "path": file_path,
    }

    # Note: Synology File Station doesn't have a trash/untrash concept like Google Drive
    # Files are either deleted or moved to #recycle folder
    # The "permanent" parameter is kept for API consistency but may not work as expected

    async with network.fetch("GET", url, query=query) as response:
        data = await response.json()

        if not data.get("success"):
            error = data.get("error", {})
            raise Exception(f"Failed to delete file: {error}")


async def update_file(
    network: Network,
    file_path: str,
    new_name: str | None = None,
    new_parent_path: str | None = None,
    trashed: bool | None = None,
) -> SynologyFileDict:
    """
    Update file properties (rename, move, etc.).

    Args:
        network: Network instance
        file_path: Current file path
        new_name: New file name (optional)
        new_parent_path: New parent path for moving (optional)
        trashed: Trash/untrash status (not fully supported)

    Returns:
        Updated file data

    Reference:
        SYNO.FileStation.Rename and SYNO.FileStation.CopyMove APIs
    """
    current_path = file_path
    updated_path = file_path

    # Handle rename
    if new_name:
        parent_path = "/".join(current_path.rstrip("/").split("/")[:-1]) or "/"
        updated_path = await _rename_file(network, current_path, new_name)
        current_path = updated_path

    # Handle move
    if new_parent_path:
        file_name = current_path.split("/")[-1]
        updated_path = await _move_file(network, current_path, new_parent_path)
        current_path = updated_path

    # Handle trash (not fully supported in Synology)
    if trashed is not None:
        if trashed:
            # Move to trash (not standard in Synology)
            # This is a placeholder
            pass
        else:
            # Restore from trash (not standard in Synology)
            pass

    # Get updated file info
    return await get_file_info(network, updated_path)


async def _rename_file(
    network: Network,
    file_path: str,
    new_name: str,
) -> str:
    """
    Rename a file.

    Args:
        network: Network instance
        file_path: Current file path
        new_name: New file name

    Returns:
        New file path
    """
    base_url = network._auth.base_url
    url = f"{base_url}/webapi/entry.cgi"

    query = {
        "api": "SYNO.FileStation.Rename",
        "version": "2",
        "method": "rename",
        "path": file_path,
        "name": new_name,
    }

    async with network.fetch("GET", url, query=query) as response:
        data = await response.json()

        if not data.get("success"):
            error = data.get("error", {})
            raise Exception(f"Failed to rename file: {error}")

    # Return new path
    parent_path = "/".join(file_path.rstrip("/").split("/")[:-1]) or "/"
    return f"{parent_path.rstrip('/')}/{new_name}"


async def _move_file(
    network: Network,
    file_path: str,
    dest_folder_path: str,
) -> str:
    """
    Move a file to a different folder.

    Args:
        network: Network instance
        file_path: Current file path
        dest_folder_path: Destination folder path

    Returns:
        New file path
    """
    base_url = network._auth.base_url
    url = f"{base_url}/webapi/entry.cgi"

    query = {
        "api": "SYNO.FileStation.CopyMove",
        "version": "3",
        "method": "start",
        "path": file_path,
        "dest_folder_path": dest_folder_path,
        "overwrite": "true",
        "remove_src": "true",  # Move (not copy)
    }

    async with network.fetch("GET", url, query=query) as response:
        data = await response.json()

        if not data.get("success"):
            error = data.get("error", {})
            raise Exception(f"Failed to move file: {error}")

    # Return new path
    file_name = file_path.split("/")[-1]
    return f"{dest_folder_path.rstrip('/')}/{file_name}"


def _convert_file_info(file_info: dict[str, Any], parent_path: str) -> SynologyFileDict:
    """
    Convert Synology file info to SynologyFileDict.

    Args:
        file_info: Raw file info from API
        parent_path: Parent folder path

    Returns:
        Standardized file dict
    """
    from datetime import UTC, datetime

    path = file_info.get("path", "")
    name = file_info.get("name", "")
    is_folder = file_info.get("isdir", False)

    # Extract time info
    additional = file_info.get("additional", {})
    time_info = additional.get("time", {})
    created_time = time_info.get("crtime", 0)
    modified_time = time_info.get("mtime", 0)

    # Convert timestamps to ISO format
    created_dt = datetime.fromtimestamp(created_time, UTC) if created_time else datetime.now(UTC)
    modified_dt = datetime.fromtimestamp(modified_time, UTC) if modified_time else datetime.now(UTC)

    # Extract size and type
    size = additional.get("size", 0)
    mime_type = file_info.get("type", "application/octet-stream")

    return SynologyFileDict(
        id=path,
        name=name,
        parent_id=parent_path,
        is_folder=is_folder,
        trashed=False,
        created_time=created_dt.isoformat(),
        modified_time=modified_dt.isoformat(),
        mime_type=mime_type,
        size=size,
    )
