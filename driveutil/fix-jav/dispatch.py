import re


def include_jav_4_3(name: str):
    m = re.search(r'(\w{3,5})[-_](\d{3})', name)
    if not m:
        return ''
    rv = f'{m.group(1)}-{m.group(2)}'
    rv = rv.upper()
    return rv


EXCLUDE_LIST = [
]


INCLUDE_LIST = [
    include_jav_4_3,
]


def get_jav_id(name: str) -> str:
    for ex in EXCLUDE_LIST:
        rv = ex(name)
        if rv:
            return ''
    for in_ in INCLUDE_LIST:
        rv = in_(name)
        if rv:
            return rv
    return ''
