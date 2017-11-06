import json

from tornado import web as tw
from wcpan.logger import DEBUG

from . import util


class IssueHandler(tw.RequestHandler):

    def post(self):
        payload = self.request.body
        payload = payload.decode('utf-8')
        payload = json.loads(payload)

        util.triage(payload)
