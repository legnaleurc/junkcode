#! /usr/bin/env python
#-*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import unicode_literals

import sys


def sum_(s):
    a = map(int, s)
    return sum(a)


def main(args=None):
    if args is None:
        args = sys.argv

    line = raw_input().strip()
    odd_numbers = line[0::2]
    even_numbers = line[1::2]
    a = sum_(odd_numbers)
    b = sum_(even_numbers)
    if abs(a - b) % 11 == 0:
        print('{0} 之奇偶數差為 11 的倍數'.format(line))
    else:
        print('{0} 之奇偶數差非 11 的倍數'.format(line))

    return 0


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
