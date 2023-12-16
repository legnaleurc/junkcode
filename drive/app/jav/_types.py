from dataclasses import dataclass


@dataclass(frozen=True, kw_only=True)
class SauceData:
    name: str
    query: str


@dataclass(frozen=True, kw_only=True)
class JavData:
    name: str
    sauce_list: list[SauceData]
