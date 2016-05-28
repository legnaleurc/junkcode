#! /usr/bin/env python
#-*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import unicode_literals

import sys


def get_interval_in_minute(begin_hour, begin_minute, end_hour, end_minute):
    begin_time = begin_hour * 60 + begin_minute
    end_time = end_hour * 60 + end_minute
    return end_time - begin_time


def calc(total_minute):
    second_level = 2 * 60
    third_level = 4 * 60

    sum_ = 0

    if total_minute >= third_level:
        m = total_minute - third_level
        sum_ += m / 30 * 60
    total_minute = min(total_minute, third_level)

    if total_minute >= second_level:
        m = total_minute - second_level
        sum_ += m / 30 * 40
    total_minute = min(total_minute, second_level)

    m = total_minute
    sum_ += m / 30 * 30

    return sum_


def main(args=None):
    if args is None:
        args = sys.argv

    print('開始停車時間：')
    line = raw_input().strip()
    bh, bm = map(int, line.split())
    print('結束停車時間：')
    line = raw_input().strip()
    eh, em = map(int, line.split())

    fee = calc(get_interval_in_minute(bh, bm, eh, em))
    print('停車費用：{0} 元'.format(fee))

    return 0


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
