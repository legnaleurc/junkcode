#! /usr/bin/env python
#-*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import unicode_literals

import sys


TABLE_NAME = ['產品 A', '產品 B', '產品 C', '產品 D', '產品 F']
TABLE = [12, 16, 10, 14, 15]
SALES = {
    '銷售員 1': [33, 32, 56, 45, 33],
    '銷售員 2': [77, 33, 68, 45, 23],
    '銷售員 3': [43, 55, 43, 67, 65],
}


def calc_A():
    best = ''
    highest = 0
    for name, sale in SALES.items():
        sum_ = 0
        for i, n in enumerate(sale):
            sum_ += TABLE[i] * n
        print('{0} 銷售總金額 {1}'.format(name, sum_))
        if sum_ > highest:
            highest = sum_
            best = name
    return best


def calc_B():
    best = ''
    highest = 0
    for i in range(len(TABLE)):
        sum_ = 0
        for name, sale in SALES.items():
            sum_ = sale[i] * TABLE[i]
        print('{0} 銷售總金額 {1}'.format(TABLE_NAME[i], sum_))
        if sum_ > highest:
            highest = sum_
            best = TABLE_NAME[i]
    return best


def main(args=None):
    if args is None:
        args = sys.argv

    c = calc_A()
    d = calc_B()
    print('最好業績：' + c)
    print('最高產品：' + d)

    return 0


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
