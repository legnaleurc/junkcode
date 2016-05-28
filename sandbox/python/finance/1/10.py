#! /usr/bin/env python
#-*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import unicode_literals

import sys


def is_jun(year):
    return year % 400 == 0 or (year % 4 == 0 and year % 100 != 0)


def main(args=None):
    if args is None:
        args = sys.argv

    print('請輸入西歷年：')
    line = raw_input().strip()
    year = int(line)

    ok = is_jun(year)
    if ok:
        print('{0} 年是閏年'.format(year))
    else:
        print('{0} 年不是閏年'.format(year))

    return 0


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
