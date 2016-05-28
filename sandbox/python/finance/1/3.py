#! /usr/bin/env python
#-*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import unicode_literals

import sys


def when(target, money, money_rate):
    money_rate = 1 + money_rate / 12. / 100.
    monthes = 1
    while True:
        the_money = money * money_rate ** monthes
        monthes += 1
        if the_money >= target:
            break
    return monthes


def main(args=None):
    if args is None:
        args = sys.argv

    monthes = when(3000000, 10000, 12)
    print(monthes)
    return 0


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
