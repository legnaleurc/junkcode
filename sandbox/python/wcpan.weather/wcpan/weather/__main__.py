import asyncio
import argparse
import signal
import sys

from aiohttp import web as aw
import aiohttp_jinja2 as aj
import jinja2
from wcpan.logger import setup as setup_logger, EXCEPTION


class Daemon(object):

    def __init__(self, args):
        # args = parse_args(args)
        setup_logger((
            'aiohttp',
            'wcpan.weather',
        ))

        self._loop = asyncio.get_event_loop()
        self._finished = asyncio.Event()

    def __call__(self):
        self._loop.create_task(self._guard())
        self._loop.add_signal_handler(signal.SIGINT, self._close_from_signal)
        self._loop.run_forever()
        self._loop.close()
        return 0

    async def _guard(self):
        try:
            return await self._main()
        except Exception as e:
            EXCEPTION('wcpan.weather', e)
        finally:
            self._loop.stop()
        return 1

    async def _main(self):
        app = aw.Application()

        setup_static_and_view(app)
        setup_api_path(app)

        async with ServerContext(app):
            await self._wait_for_finished()

        return 0

    def _close_from_signal(self):
        self._finished.set()

    async def _wait_for_finished(self):
        await self._finished.wait()


class ServerContext(object):

    def __init__(self, app):
        self._runner = aw.AppRunner(app)

    async def __aenter__(self):
        await self._runner.setup()
        site = aw.TCPSite(self._runner, host='127.0.0.1', port=8000)
        await site.start()
        return self._runner

    async def __aexit__(self, exc_type, exc, tb):
        await self._runner.cleanup()


# def parse_args(args):
#     parser = argparse.ArgumentParser(prog='duld',
#         formatter_class=argparse.ArgumentDefaultsHelpFormatter)
#     parser.add_argument('-s', '--settings', default='duld.yaml', type=str,
#                         help='settings file name')
#     args = parser.parse_args(args[1:])
#     return args


def setup_static_and_view(app):
    root = op.dirname(__file__)
    static_path = op.join(root, 'static')
    template_path = op.join(root, 'templates')

    app.router.add_static('/static/', path=static_path, name='static')
    app['static_root_url'] = '/static'

    aj.setup(app, loader=jinja2.FileSystemLoader(template_path))

    app.router.add_view(r'/', view.IndexHandler)


def setup_api_path(app):
    app.router.add_view(r'/api/v1/nodes', api.NodesHandler)
    app.router.add_view(r'/api/v1/nodes/{id:[a-zA-Z0-9\-_]+}', api.NodesHandler)
    app.router.add_view(r'/api/v1/cache', api.CacheHandler)
    app.router.add_view(r'/api/v1/cache/{id:[a-zA-Z0-9\-_]+}', api.CacheHandler)
    app.router.add_view(r'/api/v1/log', api.LogHandler)


main = Daemon(sys.argv)
exit_code = main()
sys.exit(exit_code)
