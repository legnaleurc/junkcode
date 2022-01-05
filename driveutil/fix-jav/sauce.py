import asyncio
import functools

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
        if not title:
            return None
        return normalize_title(title.text)


SOURCE_LIST = [
    fetch_jav_data_from_javbus,
    fetch_jav_data_from_javlibrary,
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
