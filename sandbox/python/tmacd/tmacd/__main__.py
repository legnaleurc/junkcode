import datetime
import logging
import os
import subprocess
import sys

from tornado import ioloop, process, web, log, options, util, gen, iostream
import transmissionrpc
import yaml


# user-defined settings in the YAML
OPTS = None


class RootHandler(web.RequestHandler):

    def get(self):
        # the directory that contains downloaded files
        torrent_root = self.get_argument('torrent_root')
        torrent_id = self.get_argument('torrent_id')
        loop = ioloop.IOLoop.current()
        loop.add_callback(process_torrent, torrent_root, torrent_id)


def main(args=None):
    if not args:
        args = sys.argv

    setup_logger()

    options.define('config', default=None, type=str, help='config file path',
                   metavar='tmacd.yaml', callback=parse_config)
    remains = options.parse_command_line(args)

    main_loop = ioloop.IOLoop.instance()

    application = web.Application([
        (r'^/$', RootHandler),
    ])
    application.listen(OPTS['port'])

    main_loop.start()
    main_loop.close()

    return 0


def setup_logger():
    # tornado
    log.enable_pretty_logging()

    # the logger used in this daemon
    logger = logging.getLogger('tmacd')
    logger.propagate = False
    logger.setLevel(logging.DEBUG)
    # rotate on Sunday
    handler = logging.handlers.TimedRotatingFileHandler('/tmp/tmacd.log',
                                                        when='w6',
                                                        atTime=datetime.time())
    handler.setLevel(logging.DEBUG)
    # align columns for my eyes
    formatter = logging.Formatter('{asctime}|{levelname:_<8}|{message}',
                                  style='{')
    handler.setFormatter(formatter)
    logger.addHandler(handler)

    # the logger used for acd_cli
    logger = logging.getLogger('acd')
    logger.propagate = False
    logger.setLevel(logging.DEBUG)
    # rotate on Sunday
    handler = logging.handlers.TimedRotatingFileHandler('/tmp/acd.log',
                                                        when='w6',
                                                        atTime=datetime.time())
    handler.setLevel(logging.DEBUG)
    # the message already has timestamp
    formatter = logging.Formatter('%(message)s')
    handler.setFormatter(formatter)
    logger.addHandler(handler)


def parse_config(path):
    with open(path, 'r') as fin:
        data = yaml.safe_load(fin)
    global OPTS
    OPTS = data


@gen.coroutine
def process_torrent(torrent_root, torrent_id):
    logger = logging.getLogger('tmacd')

    root_items = get_root_items(torrent_id)
    logger.info('root times: {0}'.format(root_items))

    logger.info('upload')
    # upload files to Amazon Cloud Drive
    yield upload(torrent_root, root_items)

    logger.info('remove torrent')
    # remove the task from Transmission first
    remove_torrent(torrent_id)


def get_root_items(torrent_id):
    client = connect_transmission()
    torrent = client.get_torrent(torrent_id)
    files = torrent.files()

    common = set()
    for fid, item in files.items():
        if not item['selected']:
            continue
        parts = split_all(item['name'])
        common.add(parts[0])
    common = list(common)

    return common


def remove_torrent(torrent_id):
    client = connect_transmission()
    client.remove_torrent(torrent_id, delete_data=True)


def split_all(path):
    '''
    Returns path parts by directories.
    '''
    allparts = []
    while True:
        parts = os.path.split(path)
        if parts[0] == path:  # sentinel for absolute paths
            allparts.insert(0, parts[0])
            break
        elif parts[1] == path: # sentinel for relative paths
            allparts.insert(0, parts[1])
            break
        else:
            path = parts[0]
            allparts.insert(0, parts[1])
    return allparts


@gen.coroutine
def upload(torrent_root, root_items):
    logger = logging.getLogger('acd')

    cmd = ['acdcli', '--verbose', 'upload', '--deduplicate']
    # exclude pattern
    exclude = OPTS['exclude']
    for p in exclude:
        cmd.extend(('--exclude-regex', p))
    # files/directories to be upload
    cmd.extend(map(lambda __: os.path.join(torrent_root, __), root_items))
    # upload destination
    cmd.append(OPTS['upload_to'])

    logging.getLogger('tmacd').info('acdcli command: {0}'.format(cmd))

    # call the external process
    p = process.Subprocess(cmd, stdout=subprocess.DEVNULL,
                           stderr=process.Subprocess.STREAM)
    # tee log
    while True:
        try:
            chunk = yield p.stderr.read_bytes(65536, partial=True)
            logger.info(chunk.decode('utf-8').strip())
        except iostream.StreamClosedError as e:
            break
    exit_code = yield p.wait_for_exit()
    return exit_code


def connect_transmission():
    opt = OPTS['transmission']
    client = transmissionrpc.Client(opt['host'], port=opt['port'],
                                    user=opt['username'],
                                    password=opt['password'])
    return client


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
