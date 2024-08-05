import re
from asyncio import as_completed
from collections.abc import Awaitable
from typing import Any

from aiohttp import ClientSession
from bs4 import BeautifulSoup

from ._types import JavData


async def _get_html(
    session: ClientSession,
    url: str,
    *,
    query: dict[str, str] | None = None,
    cookies: dict[str, str] | None = None,
) -> BeautifulSoup | None:
    async with session.get(url, params=query, cookies=cookies) as response:
        if response.status != 200:
            return None

        html = await response.text(errors="ignore")

    return BeautifulSoup(html, "html.parser")


async def _get_json(session: ClientSession, url: str) -> Any | None:
    async with session.get(url) as response:
        if response.status != 200:
            return None

        return await response.json()


async def _fetch_from_fanza(
    session: ClientSession, jav_id: str, query: str
) -> str | None:
    soup = await _get_html(
        session,
        f"https://www.dmm.co.jp/search/=/searchstr={query}/limit=30/sort=rankprofile/",
    )
    if not soup:
        return None

    title = soup.select_one(".txt > a:nth-child(1)")
    if not title:
        return None

    title = _normalize_name(title.text)
    return f"{jav_id} {title}"


async def _fetch_from_mgs(
    session: ClientSession, jav_id: str, query: str
) -> str | None:
    soup = await _get_html(
        session,
        f"https://www.mgstage.com/product/product_detail/{query}/",
        cookies={
            "adc": "1",
        },
    )
    if not soup:
        return None

    title = soup.select_one(".tag")
    if not title:
        return None

    title = _normalize_name(title.text)
    return f"{jav_id} {title}"


async def _fetch_from_heydouga(
    session: ClientSession, jav_id: str, query: str
) -> str | None:
    soup = await _get_html(
        session, f"https://www.heydouga.com/moviepages/{query}/index.html"
    )
    if not soup:
        return None

    title = soup.select_one("#title-bg > h1")
    if not title:
        return None

    for span in title.find_all("span"):
        span.decompose()

    return _normalize_name(title.text)


async def _fetch_from_heyzo(
    session: ClientSession, jav_id: str, query: str
) -> str | None:
    soup = await _get_html(
        session, f"https://www.heyzo.com/moviepages/{query}/index.html"
    )
    if not soup:
        return None

    title = soup.select_one("#movie > h1")
    if not title:
        return None

    title = _normalize_name(title.text)
    title = re.sub(r"\t+", " ", title)
    return f"{jav_id} {title}"


async def _fetch_from_carib(
    session: ClientSession, jav_id: str, query: str
) -> str | None:
    soup = await _get_html(
        session, f"https://www.caribbeancom.com/moviepages/{query}/index.html"
    )
    if not soup:
        return None

    title = soup.select_one("h1[itemprop=name]")
    if not title:
        return None
    title = _normalize_name(title.text)

    actor = soup.select_one(".movie-spec a[itemprop=actor] > span[itemprop=name]")
    if not actor:
        return f"{jav_id} {title}"
    actor = _normalize_name(actor.text)

    return f"{jav_id} {title} {actor}"


async def _fetch_from_caribpr(
    session: ClientSession, jav_id: str, query: str
) -> str | None:
    soup = await _get_html(
        session, f"https://www.caribbeancompr.com/moviepages/{query}/index.html"
    )
    if not soup:
        return None

    title = soup.select_one(".movie-info .heading")
    if not title:
        return None
    title = _normalize_name(title.text)

    actor = soup.select_one(".movie-spec .spec-content > .spec-item")
    if not actor:
        return f"{jav_id} {title}"
    actor = _normalize_name(actor.text)

    return f"{jav_id} {title} {actor}"


async def _fetch_from_1pondo(
    session: ClientSession, jav_id: str, query: str
) -> str | None:
    data = await _get_json(
        session,
        f"https://www.1pondo.tv/dyn/phpauto/movie_details/movie_id/{query}.json",
    )
    if not data:
        return None

    title = _normalize_name(data["Title"])
    actor = _normalize_name(data["Actor"])

    return f"{jav_id} {title} {actor}"


async def _fetch_from_10musume(
    session: ClientSession, jav_id: str, query: str
) -> str | None:
    data = await _get_json(
        session,
        f"https://www.10musume.com/dyn/phpauto/movie_details/movie_id/{query}.json",
    )
    if not data:
        return None

    title = _normalize_name(data["Title"])
    actor = _normalize_name(data["Actor"])

    return f"{jav_id} {title} {actor}"


async def _fetch_from_javbee(
    session: ClientSession, jav_id: str, query: str
) -> str | None:
    soup = await _get_html(
        session,
        "https://javbee.org/search",
        query={
            "keyword": query,
        },
    )
    if not soup:
        return None

    title = soup.select_one(".title > a")
    if not title:
        return None
    return _normalize_name(title.text)


async def _fetch_from_javtorrent(
    session: ClientSession, jav_id: str, query: str
) -> str | None:
    soup = await _get_html(
        session,
        "https://jav-torrent.org/search",
        query={
            "keyword": query,
        },
    )
    if not soup:
        return None

    title = soup.select_one(".title > a")
    if not title:
        return None
    return _normalize_name(title.text)


_SAUCE_DICT = {
    "heydouga": _fetch_from_heydouga,
    "carib": _fetch_from_carib,
    "caribpr": _fetch_from_caribpr,
    "1pondo": _fetch_from_1pondo,
    "10musume": _fetch_from_10musume,
    "heyzo": _fetch_from_heyzo,
    "fanza": _fetch_from_fanza,
    "mgs": _fetch_from_mgs,
    "javbee": _fetch_from_javbee,
    "javtorrent": _fetch_from_javtorrent,
}


async def fetch_jav_data(session: ClientSession, jav_query: JavData):
    queries = (
        await _
        for _ in as_completed(
            _as_kv(_.sauce, _SAUCE_DICT[_.sauce](session, _.name, _.query))
            for _ in jav_query.sauce_list
        )
    )
    rv = {k: v async for k, v in queries}
    return rv


async def _as_kv[K, V](key: K, value: Awaitable[V]) -> tuple[K, V]:
    return key, await value


def _normalize_name(name: str) -> str:
    name = name.strip()
    name = name.replace("/", "／")
    name = name.replace("\n", "")
    name = name.replace("\r", "")
    return name
