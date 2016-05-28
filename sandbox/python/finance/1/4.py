#! /usr/bin/env python
#-*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import unicode_literals

import sys


def input_matrix(name):
    print('請輸入 {0} 陣列：'.format(name))
    m = []
    for i in range(3):
        line = raw_input().strip()
        ns = line.split()
        ns = list(map(int, ns))
        m.append(ns)
    return m


def add_matrix(a, b):
    c = []
    for i in range(len(a)):
        cc = []
        for j in range(len(a[i])):
            cc.append(a[i][j] + b[i][j])
        c.append(cc)
    return c


def subtract_matrix(a, b):
    c = []
    for i in range(len(a)):
        cc = []
        for j in range(len(a[i])):
            cc.append(a[i][j] - b[i][j])
        c.append(cc)
    return c


def multiplex_matrix(a, b):
    c = []
    for i in range(len(a)):
        cc = []
        for j in range(len(a[i])):
            cc.append(a[i][j] * b[i][j])
        c.append(cc)
    return c


def divide_matrix(a, b):
    c = []
    for i in range(len(a)):
        cc = []
        for j in range(len(a[i])):
            cc.append(float(a[i][j]) / float(b[i][j]))
        c.append(cc)
    return c


def print_matrix(m):
    for i in m:
        i = map(str, i)
        print(' '.join(i))


def main(args=None):
    if args is None:
        args = sys.argv

    a = input_matrix('A')
    b = input_matrix('B')

    print('相加：')
    print_matrix(add_matrix(a, b))

    print('相減：')
    print_matrix(subtract_matrix(a, b))

    print('相乘：')
    print_matrix(multiplex_matrix(a, b))

    print('相除：')
    print_matrix(divide_matrix(a, b))

    return 0


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
