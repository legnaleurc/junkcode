#! /usr/bin/env python

import cgi
import json
import sys

import cgitb
cgitb.enable(display=0, logdir='/tmp/nopicads')

from network import perform


def main(argv):
    print 'Content-Type: text/plain'
    print

    args = cgi.FieldStorage()
    payload = args.getvalue('payload')
    payload = json.loads(payload)
    ref = payload['ref']
    if ref.startswith(u'refs/tags/'):
        rev = payload['head_commit']['id']
        perform(rev)
    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv))

# ex: ts=4 sts=4 sw=4 et
