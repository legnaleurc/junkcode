import functools as ft
import urllib.parse as ulp


async def inner_connect(drivers, dsn):
    rv = ulp.urlparse(dsn)
    connector = drivers.get(rv.scheme, None)
    if not connector:
        # TODO raise error
        return None
    return await connector(rv)


async def sqlite_connector(dsn):
    return AsyncSqliteConnection(dsn.path[1:])


connect = ft.partial(inner_connect, {
    'sqlite': sqlite_connector,
})
