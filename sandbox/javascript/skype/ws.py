#! /usr/bin/env python3

import sys
import json

import tornado.ioloop as ti
import tornado.web as tw
import tornado.websocket as tws

_counter = 0
_party = set()


class SocketHandler(tws.WebSocketHandler):

    def check_origin(self, origin):
        print('check_origin', origin)
        return True

    def open(self):
        # FIXME global?
        global _counter
        global _party

        self._id = _counter
        _party.add(self)
        _counter += 1

        self.unicast({
            'action': 'login',
            'args': {
                'id': self._id,
            },
        })
        # notify other users
        self.broadcast({
            'action': 'new-member',
            'args': {
                'id': self._id,
            },
        })

    def on_message(self, message):
        rv = json.loads(message)
        if rv['action'] == 'audio-offer':
            for socket in _party:
                if socket._id == rv['args']['receiver']:
                    socket.unicast({
                        'action': 'audio-offer',
                        'args': {
                            'sender': self._id,
                            'sdp': rv['args']['sdp'],
                        },
                    })
            return
        if rv['action'] == 'audio-answer':
            for socket in _party:
                if socket._id == rv['args']['receiver']:
                    socket.unicast({
                        'action': 'audio-answer',
                        'args': {
                            'sender': self._id,
                            'sdp': rv['args']['sdp'],
                        },
                    })
            return
        if rv['action'] == 'new-ice-candidate':
            for socket in _party:
                if socket._id == rv['args']['receiver']:
                    socket.unicast({
                        'action': 'new-ice-candidate',
                        'args': {
                            'sender': self._id,
                            'candidate': rv['args']['candidate'],
                        },
                    })
            return

    def on_close(self):
        global _party
        _party.remove(self)

    def broadcast(self, data):
        for socket in _party:
            if socket is not self:
                data = json.dump(data)
                socket.write_message(data)

    def unicast(self, data):
        data = json.dumps(data)
        self.write_message(data)


class RootHandler(tw.RequestHandler):

    def get(self):
        self.render('./index.html')


class JSHandler(tw.RequestHandler):

    def get(self):
        self.render('./signal-socket.js')


def main(args=None):
    if args is None:
        args = sys.argv

    loop = ti.IOLoop.instance()

    app = tw.Application([
        (r'^/$', RootHandler),
        (r'^/signal-socket\.js$', JSHandler),
        (r'^/socket$', SocketHandler),
    ])
    app.listen(8000)

    loop.start()
    loop.close()

    return 0


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
