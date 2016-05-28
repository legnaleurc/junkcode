#! /usr/bin/env python
#-*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import unicode_literals

import sys


def buy(n):
    i = 1
    j = 1
    k = 1
    while True:
        x = i * 5
        y = j * 3
        z = k * 2
        total = x * 5 + y * 6 + z * 10
        if total < 100:
            i += 1
        elif total <= n:
            print(x, 3, 2)
            i += 1
        else:
            break


def main(args=None):
    if args is None:
        args = sys.argv

    buy(300)

    return 0


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
