import asyncio
import functools as ft
import json
import re

import aiohttp.web as aw
from wcpan.logger import EXCEPTION

from . import util as u


class NotFoundError(Exception):

    pass


class ListViewMixin(object):

    async def list_(self):
        raise NotImplementedError


class CreateViewMixin(object):

    async def create(self):
        raise NotImplementedError


class RetrieveViewMixin(object):

    async def retrieve(self):
        raise NotImplementedError


class APIView(object):

    async def get_list(self):
        return []

    async def get_object(self):
        return None


class ListView(ListViewMixin, APIView):

    async def get(self):
        try:
            list_ = await self.get_list()
        except NotFoundError:
            return aw.Response(status=404)
        return json_response(list_)


class RetriveView(RetrieveViewMixin, APIView):

    async def get(self):
        try:
            object_ = await self.get_object()
        except NotFoundError:
            return aw.Response(status=404)
        return json_response(object_)


def raise_404(fn):
    @ft.wraps(fn)
    async def wrapper(self):
        try:
            return await fn(self)
        except NotFoundError:
            return aw.Response(status=404)
    return wrapper


class NodeObjectMixin(object):

    async def get_object(self):
        id_ = self.request.match_info.get('id', None)
        if not id_:
            raise NotFoundError

        drive = self.request.app['drive']
        node = await get_node(drive, id_)
        if not node:
            raise NotFoundError

        return node


class NodeRandomAccessMixin(object):

    async def create_response(self):
        range_ = self.request.http_range
        if range_.start is None and range_.stop is None:
            return aw.StreamResponse(status=200)
        return aw.StreamResponse(status=206)

    async def feed(self, response, node):
        range_ = self.request.http_range
        offset = 0 if range_.start is None else range_.start
        length = node.size - offset if not range_.stop else range_.stop + 1

        if range_.start is not None or range_.stop is not None:
            response.headers['Content-Range'] = f'bytes {offset}-{offset + length - 1}/{node.size}'

        response.content_length = length

        response.headers['Accept-Ranges'] = 'bytes'
        response.content_type = node.mime_type

        drive = self.request.app['drive']

        try:
            await response.prepare(self.request)
            async with await drive.download(node) as stream:
                await stream.seek(offset)
                async for chunk in stream:
                    await response.write(chunk)
        finally:
            await response.write_eof()


class NodeView(NodeObjectMixin, aw.View):

    @raise_404
    async def get(self):
        node = await self.get_object()
        node = dict_from_node(node)
        return json_response(node)

    @raise_404
    async def patch(self):
        node = await self.get_object()
        drive = self.request.app['drive']
        kwargs = await self.request.json()
        if 'parent_id' in kwargs:
            path = await drive.get_path_by_id(kwargs['parent_id'])
            await drive.rename_node(node, path)
        return aw.Response(
            status=204,
            headers={
                'Access-Control-Allow-Origin': '*',
            })

    @raise_404
    async def delete(self):
        node = await self.get_object()
        drive = self.request.app['drive']
        se = self.request.app['se']
        path = await drive.get_path(node)
        se.drop_value(path)
        await drive.trash_node(node)
        return aw.Response(
            status=204,
            headers={
                'Access-Control-Allow-Origin': '*',
            })

    async def options(self):
        return aw.Response(
            status=204,
            headers={
                'Access-Control-Allow-Origin': '*',
                'Access-Control-Allow-Methods': ', '.join([
                    'GET',
                    'PATCH',
                    'OPTIONS',
                ]),
            })


class NodeListView(aw.View):

    async def get(self):
        name_filter = self.request.query.get('name', None)
        if not name_filter:
            return aw.Response(status=400)

        se = self.request.app['se']
        try:
            nodes = await search_by_name(se, name_filter)
        except u.InvalidPatternError:
            return aw.Response(status=400)
        except u.SearchFailedError:
            return aw.Response(status=503)

        nodes = [{'id': k, 'path': v} for k, v in nodes.items()]
        nodes = sorted(nodes, key=lambda _: _['path'])
        return json_response(nodes)


class NodeChildrenView(NodeObjectMixin, aw.View):

    @raise_404
    async def get(self):
        node = await self.get_object()
        drive = self.request.app['drive']
        children = await drive.get_children(node)
        children = [dict_from_node(_) for _ in children]
        return json_response(children)


class NodeStreamView(NodeObjectMixin, NodeRandomAccessMixin, aw.View):

    @raise_404
    async def get(self):
        node = await self.get_object()
        if node.is_folder:
            return aw.Response(status=400)

        response = await self.create_response()
        await self.feed(response, node)
        return response


class NodeDownloadView(NodeObjectMixin, NodeRandomAccessMixin, aw.View):

    @raise_404
    async def get(self):
        node = await self.get_object()
        if node.is_folder:
            return aw.Response(status=400)

        response = await self.create_response()
        response.headers['Content-Disposition'] = f'attachment; filename="{node.name}"'
        await self.feed(response, node)
        return response


class NodeImageListView(NodeObjectMixin, aw.View):

    @raise_404
    async def get(self):
        node = await self.get_object()
        if node.is_folder:
            # TODO support folder
            return aw.Response(status=404)

        ue = self.request.app['ue']
        try:
            manifest = await ue.get_manifest(node.id_)
        except u.InvalidPatternError:
            return aw.Response(status=400)
        except u.SearchFailedError:
            return aw.Response(status=503)

        return json_response(manifest)


class ChangesView(aw.View):

    async def post(self):
        drive = self.request.app['drive']
        se = self.request.app['se']
        await se.clear_cache()
        changes = [_ async for _ in drive.sync()]
        return json_response(changes)


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


async def get_node(drive, id_or_root):
    if id_or_root == 'root':
        return await drive.get_root_node()
    else:
        return await drive.get_node_by_id(id_or_root)


async def search_by_name(search_engine, pattern):
    real_pattern = u.normalize_search_pattern(pattern)
    try:
        re.compile(real_pattern)
    except Exception as e:
        EXCEPTION('server', e) << real_pattern
        raise u.InvalidPatternError(real_pattern)

    se = search_engine
    nodes = await se.get_nodes_by_regex(real_pattern)
    return nodes
