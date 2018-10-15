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


async def root(request):
    drive = request.app['drive']
    root = await drive.get_root_node()
    return json_response(root.id_)


async def list_(request):
    id_ = request.match_info.get('id', None)
    if not id_:
        return aw.Response(status=400)

    drive = request.app['drive']
    node = await get_node(drive, id_)
    if not node:
        return aw.Response(status=404)

    children = await drive.get_children(node)
    children = [dict_from_node(_) for _ in children]
    return json_response(children)


async def info(request):
    id_ = request.match_info.get('id', None)
    if not id_:
        return aw.Response(status=400)

    drive = request.app['drive']
    node = await get_node(drive, id_)
    if not node:
        return aw.Response(status=404)

    node = dict_from_node(node)
    return json_response(node)


async def file_(request):
    id_ = request.match_info.get('id', None)
    if not id_:
        return aw.Response(status=404)

    drive = request.app['drive']
    node = await drive.get_node_by_id(id_)
    if not node:
        return aw.Response(status=404)
    if node.is_folder:
        return aw.Response(status=404)

    range_ = request.http_range
    offset = 0 if range_.start is None else range_.start
    length = node.size - offset if not range_.stop else range_.stop + 1

    response = aw.StreamResponse(status=206)
    response.headers['Accept-Ranges'] = 'bytes'
    response.headers['Content-Range'] = f'bytes {offset}-{offset + length - 1}/{node.size}'
    response.content_length = length
    response.content_type = node.mime_type
    try:
        await response.prepare(request)
        async with await drive.download(node) as stream:
            await stream.seek(offset)
            async for chunk in stream:
                await response.write(chunk)
    finally:
        await response.write_eof()
    return response


async def sync(request):
    drive = request.app['drive']
    sockets = request.app['changes']
    lock = request.app['sync_lock']
    loop = asyncio.get_event_loop()
    loop.create_task(broadcast_changes(drive, lock, sockets))
    return aw.Response(
        status=204,
        headers={
            'Access-Control-Allow-Origin': '*',
        })


def json_response(data):
    data = json.dumps(data)
    return aw.Response(
        content_type='application/json',
        text=data + '\n',
        headers={
            'Access-Control-Allow-Origin': '*',
        })


def dict_from_node(node):
    return {
        'id': node.id_,
        'parent_id': node.parent_id,
        'name': node.name,
        'trashed': node.trashed,
        'is_folder': node.is_folder,
        'created': node.created.isoformat(),
        'modified': node.modified.isoformat(),
        'mime_type': node.mime_type,
        'md5': node.md5,
        'size': node.size,
    }


async def get_node(drive, id_or_path):
    if id_or_path.startswith('/'):
        return await drive.get_node_by_path(id_or_path)
    else:
        return await drive.get_node_by_id(id_or_path)


async def broadcast_changes(drive, lock, socket_set):
    async with lock:
        async for changes in drive.sync():
            data = json.dumps(changes)
            fs = [ws.send_str(data + '\n') for ws in socket_set]
            if fs:
                done, pending = await asyncio.wait(fs)
