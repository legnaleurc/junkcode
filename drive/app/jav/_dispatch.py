import re

from ._types import SauceData, JavData


def match_jav_1(name: str) -> JavData | None:
    m = re.search(r"(\w{2,6})[-_](\d{2,4}\w?)", name)
    if not m:
        return None
    name = f"{m.group(1)}-{m.group(2)}"
    name = name.upper()
    return JavData(
        name=name,
        sauce_list=[
            SauceData(name="javbus", query=name),
            SauceData(name="javlibrary", query=name),
        ],
    )


def match_jav_2(name: str) -> JavData | None:
    m = re.search(r"(\d{3,4})(\w{3,6})[-_](\d{3,4}\w?)", name)
    if not m:
        return None
    name = f"{m.group(1)}{m.group(2)}-{m.group(3)}"
    name = name.upper()
    return JavData(
        name=name,
        sauce_list=[
            SauceData(name="javbus", query=name),
            SauceData(name="javlibrary", query=name),
        ],
    )


def match_fc2(name: str) -> JavData | None:
    m = re.search(r"fc2[-_]ppv[-_](\d+)", name, re.I)
    if not m:
        return None
    name = f"FC2-PPV-{m.group(1)}"
    return JavData(
        name=name,
        sauce_list=[
            SauceData(name="javbee", query=name),
            SauceData(name="javtorrent", query=name),
        ],
    )


def match_heydouga(name: str) -> JavData | None:
    m = re.search(r"hey(douga)?[-_ ]?(\d+)[-_](\d+)", name, re.I)
    if not m:
        return None
    a = m.group(2)
    b = m.group(3)
    name = f"HEYDOUGA-{a}-{b}"
    return JavData(
        name=name,
        sauce_list=[
            SauceData(name="heydouga", query=f"{a}/{b}"),
        ],
    )


def match_caribpr(name: str) -> JavData | None:
    m = re.search(r"(\d{6})[-_](\d{3})-CARIBPR", name, re.I)
    if not m:
        return None
    a = m.group(1)
    b = m.group(2)
    name = f"CARIBPR {a}-{b}"
    return JavData(
        name=name,
        sauce_list=[SauceData(name="caribpr", query=f"{a}-{b}")],
    )


def match_carib(name: str) -> JavData | None:
    m = re.search(r"(\d{6})[-_](\d{3})-CARIB", name, re.I)
    if not m:
        return None
    a = m.group(1)
    b = m.group(2)
    name = f"CARIB {a}-{b}"
    return JavData(
        name=name,
        sauce_list=[SauceData(name="carib", query=f"{a}-{b}")],
    )


def match_1pondo(name: str) -> JavData | None:
    m = re.search(r"(\d{6})[-_](\d{3})-1PON", name, re.I)
    if not m:
        return None
    a = m.group(1)
    b = m.group(2)
    name = f"1PONDO {a}-{b}"
    return JavData(
        name=name,
        sauce_list=[SauceData(name="1pondo", query=f"{a}-{b}")],
    )


def match_unknown(name: str) -> JavData | None:
    m = re.search(r"(\d{6})[-_](\d{3})", name, re.I)
    if not m:
        return None
    a = m.group(1)
    b = m.group(2)
    name = f"{a}-{b}"
    return JavData(
        name=name,
        sauce_list=[
            SauceData(name="carib", query=f"{a}-{b}"),
            SauceData(name="caribpr", query=f"{a}-{b}"),
            SauceData(name="1pondo", query=f"{a}-{b}"),
        ],
    )


def match_mesubuta(name: str) -> JavData | None:
    m = re.search(r"(\d{6})[-_](\d{3})[-_](\d{2})", name, re.I)
    if not m:
        return None
    name = f"{m.group(1)}_{m.group(2)}_{m.group(3)}"
    return JavData(
        name=name,
        sauce_list=[],
    )


def match_10musume(name: str) -> JavData | None:
    m = re.search(r"(\d{6})[-_](\d{2})-10MU", name, re.I)
    if not m:
        return None
    a = m.group(1)
    b = m.group(2)
    name = f"10MU {a}_{b}"
    return JavData(
        name=name,
        sauce_list=[
            SauceData(name="10musume", query=f"{a}_{b}"),
        ],
    )


EXCLUDE_LIST = [
    match_mesubuta,
]


INCLUDE_LIST = [
    match_10musume,
    match_caribpr,
    match_carib,
    match_1pondo,
    match_heydouga,
    match_fc2,
    match_jav_1,
    match_jav_2,
    match_unknown,
]


def get_jav_query(name: str) -> JavData | None:
    for ex in EXCLUDE_LIST:
        rv = ex(name)
        if rv:
            return None
    for in_ in INCLUDE_LIST:
        rv = in_(name)
        if rv:
            return rv
    return None
