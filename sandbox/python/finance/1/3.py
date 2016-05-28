#! /usr/bin/env python
#-*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import unicode_literals

import sys


def when(target, money, money_rate):
    money = money * 12
    money_rate = 1 + money_rate / 100.
    years = 1
    while True:
        the_money = (money * years) * money_rate ** years
        if the_money >= target:
            break
        years += 1
    return years


def main(args=None):
    if args is None:
        args = sys.argv

    years = when(3000000, 10000, 12)
    print('需要 {0} 年'.format(years))
    return 0


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
