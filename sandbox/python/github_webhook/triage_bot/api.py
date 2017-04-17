import json

from tornado import web as tw
from wcpan.logger import DEBUG


class IndexHandler(tw.RequestHandler):

    def post(self):
        payload = self.request.body
        payload = payload.decode('utf-8')
        payload = json.loads(payload)
        DEBUG('triage_bot') << json.dumps(payload, indent=2)
