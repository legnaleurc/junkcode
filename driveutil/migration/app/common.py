import contextlib
import pathlib
import sqlite3

from wcpan.drive.core.drive import DriveFactory


def humanize(n: int) -> str:
    unit_list = ['', 'KiB', 'MiB', 'GiB', 'TiB', 'PiB']
    e = 0
    while n >= 1024:
        n = n // 1024
        e += 1
    return f'{n}{unit_list[e]}'


def get_src_drive():
    print('get_src_drive')
    factory = DriveFactory()
    factory.data_path = get_data_path('data/src')
    factory.load_config()
    return factory


def get_dst_drive():
    print('get_dst_drive')
    factory = DriveFactory()
    factory.data_path = get_data_path('data/dst')
    factory.load_config()
    return factory


def get_data_path(root: str):
    path = pathlib.Path(f'./{root}/local/share')
    path = path.expanduser()
    path = path / 'wcpan.drive'
    return path


@contextlib.contextmanager
def migration_cache():
    with sqlite3.connect('./data/_migrated.sqlite') as db, \
         contextlib.closing(db.cursor()) as query:
        yield query
