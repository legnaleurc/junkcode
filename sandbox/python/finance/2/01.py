#! /usr/bin/env python
#-*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import unicode_literals

import sys


def calc(n):
	sum_ = 0
	for i in range(1, n + 1):
		sum_ += 1. / 2 ** i
	return sum_


def main(args=None):
    if args is None:
        args = sys.argv

    line = raw_input('n = ').strip()
    n = int(line)
    result = calc(n)
    print('result = {0}'.format(result))

    return 0


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
