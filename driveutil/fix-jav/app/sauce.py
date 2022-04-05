import asyncio
import functools
import re

from aiohttp import ClientSession
from bs4 import BeautifulSoup


def named_fetch(name: str):
    def decorator(fn):
        @functools.wraps(fn)
        async def wrapper(*args, **kwargs):
            rv = await fn(*args, **kwargs)
            return name, rv
        return wrapper
    return decorator


@named_fetch('javbus')
async def fetch_jav_data_from_javbus(session: ClientSession, jav_id: str):
    if jav_id.startswith('FC2-PPV'):
        return None

    async with session.get(f'https://www.javbus.com/ja/{jav_id}') as response:
        if response.status != 200:
            return None

        html = await response.text(errors='ignore')
        soup = BeautifulSoup(html, 'html.parser')
        title = soup.select_one('.container > h3')
        if not title:
            return None
        return normalize_title(title.text)


@named_fetch('javlib')
async def fetch_jav_data_from_javlibrary(session: ClientSession, jav_id: str):
    if jav_id.startswith('FC2-PPV'):
        return None

    async with session.get(
        f'http://www.javlibrary.com/ja/vl_searchbyid.php',
        params={
            'keyword': jav_id,
        },
    ) as response:
        if response.status != 200:
            return None

        html = await response.text(errors='ignore')
        soup = BeautifulSoup(html, 'html.parser')
        title = soup.select_one('#video_title .post-title')
        if title:
            return normalize_title(title.text)

        videos = soup.select('.videos .video')
        for div in videos:
            id_ = div.select_one('.id')
            if id_.text != jav_id:
                continue
            a = div.select_one('a')
            if a:
                title = a.get('title')
                if title.find('（ブルーレイディスク）') >= 0:
                    continue
                return normalize_title(title)

        return None


@named_fetch('javtor')
async def fetch_jav_data_from_javtorrent(session: ClientSession, jav_id: str):
    if not jav_id.startswith('FC2-PPV'):
        return None

    async with session.get('https://jav-torrent.org/search', params={
        'keyword': jav_id,
    }) as response:
        if response.status != 200:
            return None

        html = await response.text(errors='ignore')
        soup = BeautifulSoup(html, 'html.parser')
        title = soup.select_one('.title > a')
        if not title:
            return None
        return normalize_title(title.text)


@named_fetch('heydouga')
async def fetch_jav_data_from_heydouga(session: ClientSession, jav_id: str):
    m = re.match(r'HEYDOUGA-(\d+)-(\d+)', jav_id)
    if not m:
        return None

    p1 = m.group(1)
    p2 = m.group(2)
    async with session.get(f'https://www.heydouga.com/moviepages/{p1}/{p2}/index.html') as response:
        if response.status != 200:
            return None

        html = await response.text(errors='ignore')
        soup = BeautifulSoup(html, 'html.parser')
        title = soup.select_one('#title-bg > h1')
        if not title:
            return None
        for span in title.find_all('span'):
            span.decompose()
        return f'{jav_id} ' + normalize_title(title.text)


SOURCE_LIST = [
    fetch_jav_data_from_javbus,
    fetch_jav_data_from_javlibrary,
    fetch_jav_data_from_javtorrent,
    fetch_jav_data_from_heydouga,
]


async def fetch_jav_data(session: ClientSession, jav_id: str):
    tasks = [source(session, jav_id) for source in SOURCE_LIST]
    tmp = await asyncio.gather(*tasks)
    rv = {name: value for name, value in tmp}
    return rv


def normalize_title(title: str) -> str:
    title = title.strip()
    title = title.replace('/', '／')
    title = title.replace('\n', '')
    title = title.replace('\r', '')
    return title


async def fetch(jav_id: str):
    async with ClientSession() as session:
        rv = await fetch_jav_data_from_javlibrary(session, jav_id)
        return rv


async def main():
    import sys
    args = sys.argv
    rv = await fetch(args[1])
    print(rv)


if __name__ == '__main__':
    asyncio.run(main())