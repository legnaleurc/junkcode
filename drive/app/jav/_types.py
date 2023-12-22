from dataclasses import dataclass
from typing import TypedDict


@dataclass(frozen=True, kw_only=True)
class SauceData:
    sauce: str
    query: str


@dataclass(frozen=True, kw_only=True)
class JavData:
    name: str
    sauce_list: list[SauceData]


class ManifestDict(TypedDict):
    id: str
    jav_id: str
    name: str
    title: dict[str, str | None]
