import re
from asyncio import as_completed
from collections.abc import Awaitable

from aiohttp import ClientSession
from bs4 import BeautifulSoup

from ._types import JavData


async def fetch_jav_data_from_javbus(session: ClientSession, jav_id: str, query: str):
    async with session.get(f"https://www.javbus.com/ja/{query}") as response:
        if response.status != 200:
            return None

        html = await response.text(errors="ignore")
        soup = BeautifulSoup(html, "html.parser")
        title = soup.select_one(".container > h3")
        if not title:
            return None
        return normalize_title(title.text)


async def fetch_jav_data_from_javlibrary(
    session: ClientSession, jav_id: str, query: str
):
    async with session.get(
        f"http://www.javlibrary.com/ja/vl_searchbyid.php",
        params={
            "keyword": query,
        },
    ) as response:
        if response.status != 200:
            return None

        html = await response.text(errors="ignore")
        soup = BeautifulSoup(html, "html.parser")
        title = soup.select_one("#video_title .post-title")
        if title:
            return normalize_title(title.text)

        videos = soup.select(".videos .video")
        for div in videos:
            id_ = div.select_one(".id")
            if not id_:
                continue
            if id_.text != query:
                continue
            a = div.select_one("a")
            if a:
                title = a.get("title")
                if not title:
                    continue
                if title.find("（ブルーレイディスク）") >= 0:
                    continue
                return normalize_title(title)

        return None


async def fetch_jav_data_from_javbee(session: ClientSession, jav_id: str, query: str):
    async with session.get(
        "https://javbee.org/search",
        params={
            "keyword": query,
        },
    ) as response:
        if response.status != 200:
            return None

        html = await response.text(errors="ignore")
        soup = BeautifulSoup(html, "html.parser")
        title = soup.select_one(".title > a")
        if not title:
            return None
        return normalize_title(title.text)


async def fetch_jav_data_from_heydouga(session: ClientSession, jav_id: str, query: str):
    async with session.get(
        f"https://www.heydouga.com/moviepages/{query}/index.html",
    ) as response:
        if response.status != 200:
            return None

        html = await response.text(errors="ignore")
        soup = BeautifulSoup(html, "html.parser")
        title = soup.select_one("#title-bg > h1")
        if not title:
            return None
        for span in title.find_all("span"):
            span.decompose()
        return f"{jav_id} {normalize_title(title.text)}"


async def fetch_jav_data_from_carib(session: ClientSession, jav_id: str, query: str):
    async with session.get(
        f"https://www.caribbeancom.com/moviepages/{query}/index.html",
    ) as response:
        if response.status != 200:
            return None

        html = await response.text(errors="ignore")
        soup = BeautifulSoup(html, "html.parser")

        title = soup.select_one("h1[itemprop=name]")
        if not title:
            return None
        title = normalize_title(title.text)

        actor = soup.select_one(".movie-spec a[itemprop=actor] > span[itemprop=name]")
        if not actor:
            return f"{jav_id} {title}"

        return f"{jav_id} {title} {normalize_title(actor.text)}"


async def fetch_jav_data_from_caribpr(session: ClientSession, jav_id: str, query: str):
    async with session.get(
        f"https://www.caribbeancompr.com/moviepages/{query}/index.html",
    ) as response:
        if response.status != 200:
            return None

        html = await response.text(errors="ignore")
        soup = BeautifulSoup(html, "html.parser")

        title = soup.select_one(".movie-info .heading")
        if not title:
            return None
        title = normalize_title(title.text)

        actor = soup.select_one(".movie-spec .spec-content > .spec-item")
        if not actor:
            return f"{jav_id} {title}"

        return f"{jav_id} {title} {normalize_title(actor.text)}"


async def fetch_jav_data_from_1pondo(session: ClientSession, jav_id: str, query: str):
    m = re.match(r"\d{6}_\d{3}", jav_id)
    if not m:
        return None

    async with session.get(
        f"https://www.1pondo.tv/dyn/phpauto/movie_details/movie_id/{query}.json",
    ) as response:
        if response.status != 200:
            return None

        data = await response.json()
        title = normalize_title(data["Title"])
        actor = normalize_title(data["Actor"])

        return f"{jav_id} {title} {actor}"


async def fetch_jav_data_from_10musume(session: ClientSession, jav_id: str, query: str):
    async with session.get(
        f"https://www.10musume.com/dyn/phpauto/movie_details/movie_id/{jav_id}.json",
    ) as response:
        if response.status != 200:
            return None

        data = await response.json()
        title = normalize_title(data["Title"])
        actor = normalize_title(data["Actor"])

        return f"10MU {jav_id} {title} {actor}"


SAUCE_DICT = {
    "javbus": fetch_jav_data_from_javbus,
    "javlibrary": fetch_jav_data_from_javlibrary,
    "javbee": fetch_jav_data_from_javbee,
    "javtorrent": fetch_jav_data_from_javbee,
    "heydouga": fetch_jav_data_from_heydouga,
    "carib": fetch_jav_data_from_carib,
    "caribpr": fetch_jav_data_from_caribpr,
    "1pondo": fetch_jav_data_from_1pondo,
    "10musume": fetch_jav_data_from_10musume,
}


async def fetch_jav_data(session: ClientSession, jav_query: JavData):
    queries = (
        await _
        for _ in as_completed(
            as_kv(_.name, SAUCE_DICT[_.name](session, jav_query.name, _.query))
            for _ in jav_query.sauce_list
        )
    )
    rv = {k: v async for k, v in queries if v}
    return rv


async def as_kv[K, V](key: K, value: Awaitable[V]) -> tuple[K, V]:
    return key, await value


def normalize_title(title: str) -> str:
    title = title.strip()
    title = title.replace("/", "／")
    title = title.replace("\n", "")
    title = title.replace("\r", "")
    return title
