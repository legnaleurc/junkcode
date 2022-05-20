import re


def include_jav(name: str):
    m = re.search(r'(\w{2,6})[-_](\d{2,4}\w?)', name)
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


def include_fc2(name: str):
    m = re.search(r'fc2[-_]ppv[-_](\d+)', name, re.I)
    if not m:
        return ''
    rv = f'FC2-PPV-{m.group(1)}'
    return rv


def include_heydouga(name: str):
    m = re.search(r'hey(douga)?[-_ ]?(\d+)[-_](\d+)', name, re.I)
    if not m:
        return ''
    rv = f'HEYDOUGA-{m.group(2)}-{m.group(3)}'
    return rv


def include_carib(name: str):
    m = re.search(r'(\d{6})[-_](\d{3})', name, re.I)
    if not m:
        return ''
    rv = f'{m.group(1)}_{m.group(2)}'
    return rv

def include_mesubuta(name: str):
    m = re.search(r'(\d{6})[-_](\d{3})[-_](\d{2})', name, re.I)
    if not m:
        return ''
    rv = f'{m.group(1)}_{m.group(2)}_{m.group(3)}'
    return rv


EXCLUDE_LIST = [
    include_mesubuta,
]


INCLUDE_LIST = [
    include_carib,
    include_heydouga,
    include_fc2,
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
