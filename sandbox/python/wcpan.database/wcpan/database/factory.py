import functools as ft
import urllib.parse as ulp


def inner_connect(drivers, dsn):
    rv = ulp.urlparse(dsn)
    connector = drivers.get(rv.scheme, None)
    if not connector:
        # TODO raise error
        return None
    return connector(rv)


def sqlite_connector(dsn):
    from .sqlite import AsyncConnection
    return AsyncConnection(dsn.path[1:])


connect = ft.partial(inner_connect, {
    'sqlite': sqlite_connector,
})
