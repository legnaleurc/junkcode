#! /usr/bin/env python3

import asyncio
import unicodedata
import sys

from ..lib import create_default_drive


async def main():
    FORM = "NFC"

    async with create_default_drive() as drive:
        root_node = await drive.get_root()
        async for _root, folders, files in drive.walk(root_node):
            node_list = folders + files
            for child in node_list:
                if unicodedata.is_normalized(FORM, child.name):
                    continue
                new_name = unicodedata.normalize(FORM, child.name)
                print(child.id)
                print(child.name)
                print(new_name)
                await drive.move(child, new_parent=None, new_name=new_name)

    return 0


if __name__ == "__main__":
    sys.exit(asyncio.run(main()))
