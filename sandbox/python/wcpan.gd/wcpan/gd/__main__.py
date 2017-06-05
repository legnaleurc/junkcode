import os.path as op

from tornado import ioloop as ti

from .drive import Drive


def traverse_node(drive, node, level):
    if node.is_root:
        print_node('/', level)
    else:
        print_node(node.name, level)

    if node.is_folder:
        children = drive.get_children_by_id(node.id_)
        for child in children:
            traverse_node(drive, child, level + 1)



def print_node(name, level):
    level = ' ' * level
    print(level + name)


async def main(args=None):
    path = op.expanduser('~/.cache/wcpan/gd')
    drive = Drive(path)
    drive.initialize()
    await drive.sync()

    drive.close()

    return 0


main_loop = ti.IOLoop.instance()
main_loop.run_sync(main)
main_loop.close()

