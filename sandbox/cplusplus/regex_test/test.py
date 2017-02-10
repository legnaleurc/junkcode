#! /usr/bin/env python3

import ctypes
import re
import sys
import timeit

sys.path.append('./build')
import regex_import


DLL = ctypes.CDLL('./build/libregex_ctypes.so')
FN = DLL.c_regex_match


def main():
    with open('./test.html') as fin:
        ls = fin.readlines()
    pattern = r'.*fuck.*'

    a = timeit.timeit(lambda: grep(pattern, ls, python), number=1000)
    b = timeit.timeit(lambda: grep(pattern, ls, wrapper), number=1000)
    c = timeit.timeit(lambda: grep(pattern, ls, native), number=1000)

    print(a)
    print(b)
    print(c)


def grep(pattern, rows, fn):
    for row in rows:
        fn(pattern, row)


def python(pattern, cell):
    if cell is None:
        return False
    return re.match(pattern, cell) is not None


def wrapper(pattern, cell):
    if cell is None:
        return False
    return FN(pattern.encode('utf-8'), cell.encode('utf-8')) != 0


def native(pattern, cell):
    if cell is None:
        return False
    return regex_import.c_regex_match(pattern, cell)


main()
