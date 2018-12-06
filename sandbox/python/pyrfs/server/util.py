import asyncio
import re

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
