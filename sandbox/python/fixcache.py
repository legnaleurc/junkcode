#! /usr/bin/env python3

import asyncio
import sys

from wcpan.drive.core.drive import DriveFactory
from wcpan.drive.crypt.util import encrypt_name
from wcpan.logger import setup as setup_logger, INFO


async def main():
    setup_logger((
        'wcpan.drive',
        'fixcache',
    ), '/tmp/fixcache.log')

    factory = DriveFactory()
    factory.load_config()
    async with factory() as drive:
        async for change in drive.sync():
            INFO('fixcache') << change

        node = await drive.get_node_by_path('/old')
        async for root, dummy_folders, dummy_files in drive.walk(node):
            root_path = await drive.get_path(root)
            INFO('fixcache') << f'working on {root_path}'

            local_children = await drive.get_children(root)
            remote_children = await drive._remote._driver._fetch_children(root.id_)
            local_dict = {_.id_: _ for _ in local_children}
            remote_dict = {_.id_: _ for _ in remote_children}
            local_set = set((_.id_ for _ in local_children))
            remote_set = set((_.id_ for _ in remote_children))
            common_set = local_set.intersection(remote_set)
            hidden_set = remote_set.difference(common_set)
            orphan_set = local_set.difference(common_set)

            if not hidden_set and not orphan_set:
                INFO('fixcache') << f'no error in {root_path}'
                continue

            for orphan_id in orphan_set:
                orphan_node = local_dict[orphan_id]
                INFO('fixcache') << f'found orphan file {orphan_node.name}'
            assert not orphan_set

            for hidden_id in hidden_set:
                hidden_node = remote_dict[hidden_id]
                await drive._remote._driver._force_update_by_id(hidden_id)
                INFO('fixcache') << f'force updated {hidden_node.name}'

    return 0


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))
