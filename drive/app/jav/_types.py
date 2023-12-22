from dataclasses import dataclass
from typing import TypedDict


@dataclass(frozen=True, kw_only=True)
class SauceData:
    sauce: str
    name: str
    query: str


@dataclass(frozen=True, kw_only=True)
class JavData:
    sauce_list: list[SauceData]


class ManifestDict(TypedDict):
    id: str
    name: str
    title: dict[str, str | None]
