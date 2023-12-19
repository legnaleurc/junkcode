from datetime import datetime, UTC, timedelta
from pathlib import Path

from wcpan.drive.core.types import Drive
from wcpan.drive.sqlite.lib import get_uploaded_size


async def get_daily_usage(drive: Drive) -> int:
    now = datetime.now(UTC)
    yesterday = now - timedelta(days=1)
    dsn = _get_dsn(drive)
    return await get_uploaded_size(
        dsn, int(yesterday.timestamp()), int(now.timestamp())
    )


def _get_dsn(drive: Drive) -> str:
    return drive._ss._dsn  # type: ignore


def get_default_config_path() -> Path:
    path = Path("~/.config")
    path = path.expanduser()
    path = path / "wcpan.drive"
    return path


def get_default_data_path() -> Path:
    path = Path("~/.local/share")
    path = path.expanduser()
    path = path / "wcpan.drive"
    return path
