import asyncio

import tornado.platform.asyncio as tpa
import aioftp


loop = tpa.AsyncIOMainLoop()
loop.install()

server = aioftp.Server()
f = server.start('0.0.0.0', 2121)

f = asyncio.ensure_future(f)
f = tpa.to_tornado_future(f)
loop.add_future(f, lambda _: print(_))

loop.start()
loop.close()
