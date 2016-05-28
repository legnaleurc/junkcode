#! /usr/bin/env python
#-*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import unicode_literals

import sys


def buy_z(n, x, y):
    i = 1
    while True:
        z = i * 2
        total = x * 5 + y * 6 + z * 10
        if total < 100:
            i += 1
        elif total <= n:
            print('x={0}, y={1}, z={2}'.format(x, y, z))
            i += 1
        else:
            break


def buy_y(n, x):
    i = 1
    while True:
        y = i * 3
        total = x * 5 + y * 6 + 2 * 10
        if total > n:
            break
        buy_z(n, x, y)
        i += 1


def buy_x(n):
    i = 1
    while True:
        x = i * 5
        total = x * 5 + 3 * 6 + 2 * 10
        if total > n:
            break
        buy_y(n, x)
        i += 1


def buy(n):
    buy_x(n)


def main(args=None):
    if args is None:
        args = sys.argv

    print('輸入 N：')
    n = raw_input().strip()
    n = int(n)
    buy(n)

    return 0


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
