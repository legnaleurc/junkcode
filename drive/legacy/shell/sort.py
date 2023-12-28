#! /usr/bin/env python3

import json
import sys
import re


def main():
    name_list: list[tuple[int, str]] = []
    data: dict[str, str] = json.load(sys.stdin)

    for _k, name in data.items():
        m = re.search(r'\[(\d+)\]\.7z$', name)
        if not m:
            continue
        eid = int(m.group(1))
        name_list.append((eid, name))

    name_list.sort(reverse=True)

    for eid, name in name_list:
        print(name)

    return 0


if __name__ == "__main__":
    sys.exit(main())
