#! /usr/bin/env python3

import contextlib as cl
import sqlite3 as sql
import sys
import os.path as op

from acdcli.api.client import ACDClient


ROOT_NODE_ID = 'Z7lzX39JTQak6hubzS4nEQ'


def main(args=None):
    if args is None:
        args = sys.argv

    dbf = op.expanduser('~/.cache/acd_cli/nodes.db')
    client = ACDClient(op.expanduser('~/.cache/acd_cli'))
    with sql.connect(dbf) as db:
        for node_id, in get_files(db):
            rv = get_orphan_ancestor(db, node_id)
            if not rv:
                continue
            rv = move_to_orphan(db, client, rv)
            print(rv)

    return 0


def get_files(db):
    with cl.closing(db.cursor()) as query:
        query.execute('''
            SELECT `files`.`id`
            FROM `files`
            LEFT JOIN `nodes`
                ON `nodes`.`id` = `files`.`id`
            WHERE `status` = 'TRASH'
        ;''')
        while True:
            rv = query.fetchone()
            if not rv:
                return
            yield rv


def get_orphan_ancestor(db, node_id):
    path = []
    id_ = node_id
    while id_ != ROOT_NODE_ID:
        path.append(id_)
        parent = get_parent(db, id_)
        if parent is None:
            return path
        id_ = parent[0]
    return None


def move_to_orphan(db, client, node_id_list):
    last_node_id = None
    for node_id in node_id_list:
        with cl.closing(db.cursor()) as query:
            query.execute('SELECT `name`, `status` FROM `nodes` WHERE `id` = ?;', (node_id, ))
            rv = query.fetchone()
            if rv:
                last_node_id = node_id
            else:
                break

    # rv = last_node_id
    # rv = client.move_to_trash(last_node_id)
    parent_id, = get_parent(db, last_node_id)
    rv = client.remove_child(parent_id, last_node_id)
    return rv


def get_parent(db, node_id):
    with cl.closing(db.cursor()) as query:
        query.execute('SELECT `parent` FROM `parentage` WHERE `child` = ?;', (node_id, ))
        rv = query.fetchone()
        return rv


if __name__ == '__main__':
    rv = main()
    sys.exit(rv)
