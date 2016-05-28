#! /usr/bin/env python
#-*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import unicode_literals

import sys


def main(args=None):
    if args is None:
        args = sys.argv

    while True:
        print('請輸入成績：')
        line = raw_input().strip()
        if not line:
            print('空行，結束')
            break
        line = int(line)
        if 90 <= line <= 100:
            print('甲等')
        elif 80 <= line < 90:
            print('乙等')
        elif 70 <= line < 80:
            print('丙等')
        elif 60 <= line < 70:
            print('丁等')
        elif 0 <= line < 60:
            print('戊等')
        else:
            print('輸入錯誤')

    return 0


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
