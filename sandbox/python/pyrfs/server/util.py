import asyncio
import contextlib as cl
import itertools as it
import mimetypes as mt
import os
import os.path as op
import re
import tempfile

from PIL import Image
from wcpan.logger import EXCEPTION


class InvalidPatternError(Exception):

    def __init__(self, message):
        self._message = message

    def __str__(self):
        return self._message


class SearchFailedError(Exception):

    def __init__(self, message):
        self._message = message

    def __str__(self):
        return self._message


class UnpackFailedError(Exception):

    def __init__(self, message):
        self._message = message

    def __str__(self):
        return self._message


class SearchEngine(object):

    def __init__(self, drive):
        super(SearchEngine, self).__init__()
        # NOTE only takes a reference, not owning
        self._drive = drive
        self._loop = asyncio.get_event_loop()
        self._cache = {}
        self._searching = {}

    async def get_nodes_by_regex(self, pattern):
        nodes = self._cache.get(pattern, None)
        if nodes is not None:
            return nodes

        if pattern in self._searching:
            lock = self._searching[pattern]
            return await self._wait_for_result(lock, pattern)

        lock = asyncio.Condition()
        self._searching[pattern] = lock
        self._loop.create_task(self._search(pattern))
        return await self._wait_for_result(lock, pattern)

    async def clear_cache(self):
        while len(self._searching) > 0:
            pattern, lock = next(iter(self._searching.items()))
            async with lock:
                await lock.wait()
        self._cache = {}

    def drop_value(self, value):
        keys = list(self._cache.keys())
        for k in keys:
            if re.search(k, value, re.I):
                del self._cache[k]

    async def _search(self, pattern):
        lock = self._searching[pattern]
        try:
            nodes = await self._drive.find_nodes_by_regex(pattern)
            nodes = {_.id_: self._drive.get_path(_)
                     for _ in nodes if not _.trashed}
            nodes = await async_dict(nodes)
            self._cache[pattern] = nodes
        except Exception as e:
            EXCEPTION('server', e) << 'search failed, abort'
            raise SearchFailedError(str(e))
        finally:
            del self._searching[pattern]
            async with lock:
                lock.notify_all()

    async def _wait_for_result(self, lock, pattern):
        async with lock:
            await lock.wait()
        try:
            return self._cache[pattern]
        except KeyError:
            raise SearchFailedError(f'{pattern} canceled search')


class UnpackEngine(object):

    def __init__(self, drive, port):
        super(UnpackEngine, self).__init__()
        self._loop = asyncio.get_event_loop()
        # NOTE only takes a reference, not owning
        self._drive = drive
        self._port = port
        self._cache = {}
        self._unpacking = {}
        self._tmp = None
        self._raii = None

    async def __aenter__(self):
        async with cl.AsyncExitStack() as stack:
            self._tmp = stack.enter_context(tempfile.TemporaryDirectory())
            self._raii = stack.pop_all()
        return self

    async def __aexit__(self, exc, type_, tb):
        await self._raii.aclose()
        self._raii = None
        self._tmp = None

    async def get_manifest(self, node):
        manifest = self._cache.get(node.id_, None)
        if manifest is not None:
            return manifest

        if node.id_ in self._unpacking:
            lock = self._unpacking[node.id_]
            return await self._wait_for_result(lock, node.id_)

        lock = asyncio.Condition()
        self._unpacking[node.id_] = lock
        self._loop.create_task(self._unpack(node))
        return await self._wait_for_result(lock, node.id_)

    async def _unpack(self, node):
        lock = self._unpacking[node.id_]
        try:
            if node.is_folder:
                await self._unpack_remote(node)
            else:
                await self._unpack_local(node.id_)
        except UnpackFailedError:
            raise
        except Exception as e:
            EXCEPTION('server', e) << 'unpack failed, abort'
            raise UnpackFailedError(str(e))
        finally:
            del self._unpacking[node.id_]
            async with lock:
                lock.notify_all()

    async def _wait_for_result(self, lock, node_id):
        async with lock:
            await lock.wait()
        try:
            return self._cache[node_id]
        except KeyError:
            raise UnpackFailedError(f'{node_id} canceled unpack')

    async def _unpack_local(self, node_id):
        p = await asyncio.create_subprocess_exec('unpack',
                                                 str(self._port),
                                                 node_id,
                                                 self._tmp)
        out, err = await p.communicate()
        if p.returncode != 0:
            raise UnpackFailedError(f'unpack failed code: {p.returncode}')
        self._cache[node_id] = self._scan_local(node_id)

    def _scan_local(self, node_id):
        rv = []
        top = op.join(self._tmp, node_id)
        for dirpath, dirnames, filenames in os.walk(top):
            dirnames.sort(key=FuzzyName)
            filenames.sort(key=FuzzyName)
            for filename in filenames:
                path = op.join(dirpath, filename)
                type_, encoding = mt.guess_type(path)
                if type_ is None:
                    continue
                if not type_.startswith('image/'):
                    continue
                image = Image.open(path)
                width, height = image.size
                rv.append({
                    'path': path,
                    'type': type_,
                    'size': op.getsize(path),
                    'width': width,
                    'height': height,
                })
        return rv

    async def _unpack_remote(self, node):
        self._cache[node.id_] = await self._scan_remote(node)

    async def _scan_remote(self, node):
        rv = []
        async for root, folders, files in self._drive.walk(node):
            folders.sort(key=lambda _: FuzzyName(_.name))
            files.sort(key=lambda _: FuzzyName(_.name))
            for f in files:
                if f.is_image:
                    rv.append({
                        'path': f.id_,
                        'type': f.mime_type,
                        'size': f.size,
                        'width': f.image_width,
                        'height': f.image_height,
                    })
        return rv


class FuzzyName(object):

    def __init__(self, name):
        seg_list = re.findall(r'\d+|\D+', name)
        seg_list = [int(_) if _.isdigit() else _ for _ in seg_list]
        self._seg_list = seg_list

    def __lt__(self, that):
        for l, r in it.zip_longest(self._seg_list, that._seg_list):
            # compare length: shorter first
            if l is None:
                return True
            if r is None:
                return False

            # compare type: number first
            if type(l) != type(r):
                return not isinstance(l, int)

            # compare value: lower first
            if l != r:
                return l < r
        return False


def normalize_search_pattern(raw):
    rv = re.match(r'(.+?)\s*\((.+)\)', raw)
    if rv:
        rv = rv.groups()
    else:
        rv = (raw,)
    rv = map(inner_normalize_search_pattern, rv)
    rv = '|'.join(rv)
    rv = f'.*({rv}).*'
    return rv


def inner_normalize_search_pattern(raw):
    rv = re.split(r'(?:\s|-)+', raw)
    rv = map(re.escape, rv)
    rv = '.*'.join(rv)
    return rv


async def async_dict(dict_):
    dict_ = (wait_for_value(k, v) for k, v in dict_.items())
    dict_ = await asyncio.gather(*dict_)
    return dict(dict_)


async def wait_for_value(k, v):
    return k, await v
