import json

import aiohttp.web as aw


async def list_(request):
    path = request.query.get('path', None)
    if not path:
        return aw.Response(status=400)

    drive = request.app['drive']
    node = await drive.get_node_by_path(path)
    children = await drive.get_children(node)
    children = [dict_from_node(_) for _ in children]
    return json_response(children)


def json_response(data):
    data = json.dumps(data)
    return aw.Response(text=data + '\n')


def dict_from_node(node):
    return {
        'name': node.name,
        'is_folder': node.is_folder,
        'mime_type': node.mime_type,
        'size': node.size,
    }
