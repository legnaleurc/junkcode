#! /usr/bin/env python3
# ex: ts=4 sts=4 sw=4 et

import contextlib
import datetime
import grp
import os
import pwd
import re
import subprocess
import sys
import tempfile
import zipfile

from tornado import ioloop, web, httpserver, netutil, log, gen, process


EASY_RSA_ROOT = '/etc/openvpn/easy-rsa/2.0'
OPENVPN_HOST = 'wcpan.me'
OPENVPN_PORT = 1194


class IndexHandler(web.RequestHandler):

    def get(self):
        self.render('./index.html')


class OpenVPNHandler(web.RequestHandler):

    @gen.coroutine
    def get(self):
        email = self.get_argument('email', None)
        if not email:
            self.set_status(400)
            return

        timestamp = datetime.datetime.utcnow().strftime('%Y%m%d_%H%M%S')
        name = '{0}_{1}'.format(email, timestamp)
        if not check_name(name):
            self.set_status(400)
            return

        with open(os.devnull, 'w') as null:
            p = process.Subprocess(['./gk.sh', EASY_RSA_ROOT, email, name], stdout=null, stderr=null)
            exit_code = yield gen.Task(p.set_exit_callback)

        if exit_code != 0:
            self.set_status(400)
            return

        prefix = name
        with tempfile.TemporaryFile() as fout:
            with zipfile.ZipFile(fout, 'w') as zout:
                with open('./client.conf', 'r') as tpl:
                    zout.writestr('{0}/client.conf'.format(prefix), tpl.read().format(host=OPENVPN_HOST, port=OPENVPN_PORT, name=name))
                zout.write('{0}/keys/ca.crt'.format(EASY_RSA_ROOT), '{0}/ca.crt'.format(prefix))
                zout.write('{0}/keys/{1}.crt'.format(EASY_RSA_ROOT, name), '{0}/{1}.crt'.format(prefix, name))
                zout.write('{0}/keys/{1}.key'.format(EASY_RSA_ROOT, name), '{0}/{1}.key'.format(prefix, name))
            fout.seek(0, 0)

            self.set_header('Content-Type', 'application/octet-stream')
            self.set_header('Content-Disposition', 'attachment; filename="{0}.zip"'.format(name))
            self.write(fout.read())
            yield self.flush()


def check_name(name):
    m = re.match(r'^[a-zA-Z0-9_@.\-]+$', name)
    return bool(m)


@contextlib.contextmanager
def unix_socket(path, u, g):
    uid = pwd.getpwnam(u).pw_uid
    gid = grp.getgrnam(g).gr_gid
    with netutil.bind_unix_socket(path) as socket:
        os.chown(path, uid, gid)
        try:
            yield socket
        finally:
            os.remove(path)


def main(args=None):
    if args is None:
        args = sys.argv

    log.enable_pretty_logging()
    main_loop = ioloop.IOLoop()

    application = web.Application([
        (r'/', IndexHandler),
        (r'/key', OpenVPNHandler),
    ])
    server = httpserver.HTTPServer(application)
    with unix_socket('/tmp/ovpnkey.socket', 'www-data', 'www-data') as socket:
        server.add_socket(socket)
        main_loop.start()

    return 0


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
