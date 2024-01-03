from collections.abc import AsyncIterator
from contextlib import asynccontextmanager
from datetime import datetime, UTC, timedelta
from pathlib import Path

from wcpan.drive.core.types import Drive
from wcpan.drive.sqlite.lib import get_uploaded_size
from wcpan.drive.cli.lib import create_drive_from_config


def get_daily_usage(drive: Drive) -> int:
    now = datetime.now(UTC)
    yesterday = now - timedelta(days=1)
    dsn = _get_dsn(drive)
    return get_uploaded_size(dsn, yesterday, now)


def _get_dsn(drive: Drive) -> str:
    return drive._ss._bg._dsn  # type: ignore


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


@asynccontextmanager
async def create_default_drive() -> AsyncIterator[Drive]:
    config_path = get_default_config_path()
    drive_path = config_path / "cli.yaml"
    async with create_drive_from_config(drive_path) as drive:
        yield drive
