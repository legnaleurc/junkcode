import asyncio
import json

import aiohttp
import aiohttp.web as aw


class ChangesChannel(object):

    def __init__(self, app):
        self._app = app

    async def __aenter__(self):
        self._app['changes'] = set()
        self._queue = asyncio.Queue()
        return self

    async def __aexit__(self, type_, exc, tb):
        wss = set(self._app['changes'])
        fs = [ws.close(code=aiohttp.WSCloseCode.GOING_AWAY) for ws in wss]
        if fs:
            done, pending = await asyncio.wait(fs)
        del self._app['changes']

    async def __call__(self, request):
        ws = aw.WebSocketResponse()
        await ws.prepare(request)
        request.app['changes'].add(ws)

        try:
            async for message in ws:
                pass
        finally:
            request.app['changes'].discard(ws)

        return ws


async def list_(request):
    id_or_path = request.query.get('id_or_path', None)
    if not id_or_path:
        return aw.Response(status=400)

    drive = request.app['drive']
    node = await get_node(drive, id_or_path)
    if not node:
        return aw.Response(status=404)

    children = await drive.get_children(node)
    children = [dict_from_node(_) for _ in children]
    return json_response(children)


async def info(request):
    id_or_path = request.query.get('id_or_path', None)
    if not id_or_path:
        return aw.Response(status=400)

    drive = request.app['drive']
    node = await get_node(drive, id_or_path)
    if not node:
        return aw.Response(status=404)

    node = dict_from_node(node)
    return json_response(node)


async def sync(request):
    drive = request.app['drive']
    async for changes in drive.sync():
        data = json.dumps(changes)
        fs = [ws.send_str(data + '\n') for ws in request.app['changes']]
        if fs:
            done, pending = await asyncio.wait(fs)
    return aw.Response()


def json_response(data):
    data = json.dumps(data)
    return aw.Response(content_type='application/json',
                       text=data + '\n',
                       headers={
                           'Access-Control-Allow-Origin': '*',
                       })


def dict_from_node(node):
    return {
        'id': node.id_,
        'name': node.name,
        'is_folder': node.is_folder,
        'ctime': node.created.timestamp,
        'mtime': node.modified.timestamp,
        'mime_type': node.mime_type,
        'size': node.size,
    }


async def get_node(drive, id_or_path):
    if id_or_path.startswith('/'):
        return await drive.get_node_by_path(id_or_path)
    else:
        return await drive.get_node_by_id(id_or_path)
