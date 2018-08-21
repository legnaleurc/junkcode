import argparse
import asyncio
import logging
import os.path as op
import signal
import sys

from aiohttp import web as aw
import wcpan.drive.google as wdg
from wcpan.logger import setup as setup_logger, INFO, EXCEPTION

from . import api


class Daemon(object):

    def __init__(self, args):
        self._kwargs = parse_args(args[1:])
        self._loop = asyncio.get_event_loop()
        self._finished = asyncio.Event()
        self._loggers = setup_logger((
            'aiohttp',
            'wcpan.drive.google',
            'pym',
        ), '/tmp/pym.log')

    def __call__(self):
        self._loop.create_task(self._guard())
        self._loop.add_signal_handler(signal.SIGINT, self._close)
        self._loop.run_forever()
        self._loop.close()

        return 0

    async def _guard(self):
        try:
            return await self._main()
        except Exception as e:
            EXCEPTION('pym', e)
        finally:
            self._loop.stop()
        return 1

    async def _main(self):
        app = aw.Application()

        setup_api_path(app)

        async with wdg.Drive() as drive, \
                   ServerContext(app, self._kwargs.listen):
            app['drive'] = drive
            await self._until_finished()

        return 0

    async def _until_finished(self):
        await self._finished.wait()

    def _close(self):
        self._finished.set()


class ServerContext(object):

    def __init__(self, app, port):
        log_format = '%s %r (%b) %Tfs'
        self._runner = aw.AppRunner(app, access_log_format=log_format)
        self._port = port

    async def __aenter__(self):
        await self._runner.setup()
        site = aw.TCPSite(self._runner, port=self._port)
        await site.start()
        return self._runner

    async def __aexit__(self, exc_type, exc, tb):
        await self._runner.cleanup()


def parse_args(args):
    parser = argparse.ArgumentParser('ddld')

    parser.add_argument('-l', '--listen', required=True, type=int)

    args = parser.parse_args(args)

    return args


def setup_api_path(app):
    app.router.add_view(r'/api/v1/list', api.list_)


main = Daemon(sys.argv)
exit_code = main()
sys.exit(exit_code)
