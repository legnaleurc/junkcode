#! /usr/bin/env python
#-*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import unicode_literals

import sys


def calc(a, b):
    sum_ = 0
    for n in range(0, 10):
        sum_ += a[n] * b[9 - n]
    return sum_


def main(args=None):
    if args is None:
        args = sys.argv

    a = [18, -51, 23, 35, 10, 9, -3, 52, 81, 69]
    b = [28, 32, -35, 40, 73, 17, 92, 32, 13, 29]
    sum_ = calc(a, b)
    print(sum_)

    return 0


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
