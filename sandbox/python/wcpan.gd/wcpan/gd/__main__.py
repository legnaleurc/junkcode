import os.path as op

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

path = op.expanduser('~/.cache/wcpan/gd')
drive = Drive(path)
drive.initialize()
drive.sync()

drive.close()
