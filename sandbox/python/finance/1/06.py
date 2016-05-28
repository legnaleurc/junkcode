#! /usr/bin/env python
#-*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import unicode_literals

import sys


def main(args=None):
    if args is None:
        args = sys.argv

    print('請輸入頭的總數目：')
    line = raw_input().strip()
    n = int(line)
    print('請輸入腳的總數目：')
    line = raw_input().strip()
    m = int(line)

    x = (4 * n - m) / 2.
    y = (m - 2 * n) / 2.

    print('聖誕老人共有 {0} 位'.format(x))
    print('馴鹿共有 {0} 位'.format(y))

    return 0


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
