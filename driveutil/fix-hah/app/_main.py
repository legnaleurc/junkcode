from asyncio import sleep
from concurrent.futures import ProcessPoolExecutor, Executor
from logging.config import dictConfig
from pathlib import Path
from shutil import rmtree
from tempfile import TemporaryDirectory

from wcpan.drive.core.types import Drive, Node
from wcpan.drive.cli.lib import create_drive_from_config
from wcpan.logging import ConfigBuilder

from ._arg import parse_args
from ._download import download
from ._archive import archive
from ._upload import upload


async def main(args: list[str]) -> int:
    kwargs = parse_args(args)

    dictConfig(ConfigBuilder().add("wcpan", level="D").to_dict())

    async with create_drive_from_config(kwargs.config_path) as drive:
        async for _ in drive.sync():
            pass

        with TemporaryDirectory() as tmp, ProcessPoolExecutor() as pool:
            work_path = Path(tmp)

            for path in kwargs.path_list:
                node = await drive.get_node_by_path(path)
                children = await drive.get_children(node)
                children = (_ for _ in children if not _.is_trashed)
                children = (_ for _ in children if _.is_directory)
                children = (_ for _ in children if _is_hah_name(_.name))

                for child in children:
                    await _migrate(child, drive=drive, pool=pool, tmp=work_path, parent=node)
                    await sleep(60)
    return 0


def _is_hah_name(name: str) -> bool:
    import re

    rv = re.search(r"\[\d+\]$", name)
    return rv is not None


async def _migrate(src: Node, /, *, drive: Drive, pool: Executor, tmp: Path, parent: Node) -> None:
    src_path = await download(drive, src, tmp, pool)
    tmp_path = await archive(src_path, tmp)
    await upload(drive, tmp_path, parent, pool)
    await _cleanup(src, tmp_path, src_path, drive=drive)


async def _cleanup(src: Node, ar: Path, tmp: Path, /, *, drive: Drive):
    rmtree(ar)
    print(f"remove {ar}")
    rmtree(tmp)
    print(f"remove {tmp}")
    await drive.move(src, trashed=True)
    print(f"remove {src.name}")
