#! /usr/bin/env python
#-*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import unicode_literals

import sys


def main(args=None):
    if args is None:
        args = sys.argv

    # t = x / (v1 + v2)
    v1 = 1
    v2 = 30 * 2.54 / 100
    print('請輸入距離（公尺）：')
    line = raw_input().strip()
    x = int(line)
    t = int(round(x / (v1 + v2)))
    m = t / 60
    s = t % 60
    if m > 0:
        print('兩人在 {0} 分 {1} 秒會相遇'.format(m, s))
    else:
        print('兩人在 {0} 秒會相遇'.format(s))

    return 0


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
