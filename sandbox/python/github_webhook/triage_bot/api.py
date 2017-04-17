from tornado import web as tw
from wcpan.listen import DEBUG


class IndexHandler(tw.RequestHandler):

    def post(self):
        payload = self.request.body
        payload = payload.decode('utf-8')
        DEBUG('triage_bot') << payload
