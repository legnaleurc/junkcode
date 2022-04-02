import re


def include_jav(name: str):
    m = re.search(r'(\w{3,6})[-_](\d{3,4}\w?)', name)
    if not m:
        return ''
    rv = f'{m.group(1)}-{m.group(2)}'
    rv = rv.upper()
    return rv


def include_jav2(name: str):
    m = re.search(r'(\d{3,4})(\w{3,6})[-_](\d{3,4}\w?)', name)
    if not m:
        return ''
    rv = f'{m.group(1)}{m.group(2)}-{m.group(3)}'
    rv = rv.upper()
    return rv


def exclude_fc2(name: str):
    m = re.search(r'fc2[-_]ppv', name, re.I)
    return m is not None


EXCLUDE_LIST = [
    exclude_fc2,
]


INCLUDE_LIST = [
    include_jav2,
    include_jav,
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
