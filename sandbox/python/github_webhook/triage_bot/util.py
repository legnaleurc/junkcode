import sys

from tornado import ioloop as ti, web as tw, httpserver as ths
import wcpan.listen as wls
import wcpan.logger as wlg
from wcpan.logger import DEBUG

from . import api


def main(args=None):
    if args is None:
        args = sys.argv

    wlg.setup([
        'triage_bot',
    ])

    application = tw.Application([
        (r'/api/v1/.*', api.IndexHandler),
    ])

    server = ths.HTTPServer(application)

    loop = ti.IOLoop.instance()

    opts = [8000]

    with wls.create_sockets(opts) as sockets:
        server.add_sockets(sockets)
        loop.start()
        loop.close()

    return 0


def triage(payload):
    # TODO extract repository name
    if payload['repository']['full_name'] != 'legnaleurc/junkcode':
        return

    # ignored: closed, labeled, reopened, edited, unlabeled
    # if you need to handle labeled/unlabeled, watch out loop triggering
    if payload['action'] == 'opened':
        pass

    issue_number = payload['issue']['number']
    issue_title = payload['issue']['title']
    issue_body = payload['issue']['body']

    DEBUG('triage_bot') << json.dumps(payload, indent=2)
