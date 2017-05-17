#! /usr/bin/env python3
# ex: et sts=4 ts=4 sw=4

import os
import os.path as op
import sys
import subprocess as sp
import multiprocessing.dummy as mt


TN = '/home/wcpan/local/src/junkcode/sandbox/c/thumbnail/tn'


def shell_call(cmd):
    sp.run(cmd)


def walk(path):
    ext = ('.mp4', '.mkv')
    for root, dns, fns in os.walk(path):
        vfs = filter(lambda _: op.splitext(_)[1] in ext, fns)
        for vf in vfs:
            p = op.join(root, vf)
            yield [
                TN, p, '/tmp/snapshots/{0}_%02d.png'.format(p.replace('/', '__'))
            ]

for cmd in walk(sys.argv[1]):
    shell_call(cmd)
#with mt.Pool() as pool:
    #pool.map(shell_call, walk(sys.argv[1]))
