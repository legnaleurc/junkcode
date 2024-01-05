from argparse import ArgumentParser
from pathlib import Path
import asyncio
import re
import sys

from ..lib import create_default_drive


async def main(args: list[str]) -> int:
    parser = ArgumentParser("listeh")
    parser.add_argument("path", type=str)
    kwargs = parser.parse_args(args)
    path = Path(kwargs.path)

    async with create_default_drive() as drive:
        parent = await drive.get_node_by_path(path)
        children = await drive.get_children(parent)
        g = ((to_eid(_.name), _.name) for _ in children if not _.is_trashed)
        pair_list = [(eid, name) for eid, name in g if eid is not None]
        pair_list.sort(reverse=True)
        name_list = (name for _eid, name in pair_list)
        for name in name_list:
            print(name)

    return 0


def to_eid(name: str) -> int | None:
    m = re.search(r"\[(\d+)\]\.7z$", name)
    if not m:
        return None
    eid = int(m.group(1))
    return eid


if __name__ == "__main__":
    sys.exit(asyncio.run(main(sys.argv[1:])))
