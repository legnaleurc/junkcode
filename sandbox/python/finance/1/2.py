#! /usr/bin/env python
#-*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import unicode_literals

import sys


def calc(age_begin, age_end, gain, gain_rate, paid, paid_rate):
    # 換算年利率
    gain_rate = 1 + (gain_rate / 100.)
    paid_rate = 1 + (paid_rate / 100.)
    # 總存款
    total_gain = 0
    # 總支出
    total_paid = 0
    for i in range(1, age_end - age_begin + 1):
        # 年度薪資
        year_gain = gain * 12 * (gain_rate ** i)
        # 年度支出
        year_paid = paid * 12 * (paid_rate ** i)

        total_gain += year_gain
        total_paid += year_paid

    return year_gain / 12, year_paid / 12, total_gain - total_paid


def main(args=None):
    if args is None:
        args = sys.argv

    gain, paid, total = calc(25, 28, 30000, 3, 19000, 2)
    print('薪質所得 {0} 元'.format(int(round(gain))))
    print('支出額 {0} 元'.format(int(round(paid))))
    print('存款餘額 {0} 元'.format(int(round(total))))

    return 0


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
