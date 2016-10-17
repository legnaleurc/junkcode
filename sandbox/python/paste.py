#! /usr/bin/env python

import re
import sys

import requests


def main(args=None):
    if args is None:
        args = sys.argv

    while True:
        url = from_user_input()
        title = fetch_title(url)
        md = '[{0}]({1})'.format(title, url)
        print(md)

    return 0


def from_user_input():
    line = sys.stdin.readline()
    return line.rstrip()


def fetch_title(url):
    r = requests.get(url)
    r.encoding = 'utf-8'
    html = r.text
    m = re.search(r'<title>(.*)</title>', html)
    if not m:
        return None
    return m.group(1)


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
