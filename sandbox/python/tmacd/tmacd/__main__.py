import datetime
import logging
import os
import re
import shutil
import subprocess
import sys
import yaml

from tornado import ioloop, process, web, log, options, util, gen, iostream
import transmissionrpc


OPTS = None


class RootHandler(web.RequestHandler):

    def get(self):
        torrent_root = self.get_argument('torrent_root')
        torrent_id = self.get_argument('torrent_id')
        loop = ioloop.IOLoop.current()
        loop.add_callback(process_torrent, torrent_root, torrent_id)


def main(args=None):
    if not args:
        args = sys.argv

    setup_logger()

    options.define('config', default=None, type=str, help='config file path', metavar='tmacd.yaml', callback=parse_config)
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

    logger = logging.getLogger('tmacd')
    logger.propagate = False
    logger.setLevel(logging.DEBUG)
    # rotate on Sunday
    handler = logging.handlers.TimedRotatingFileHandler('/tmp/tmacd.log', when='w6', atTime=datetime.time())
    handler.setLevel(logging.DEBUG)
    formatter = logging.Formatter('{asctime}|{levelname:_<8}|{message}', style='{')
    handler.setFormatter(formatter)
    logger.addHandler(handler)

    logger = logging.getLogger('acd')
    logger.propagate = False
    logger.setLevel(logging.DEBUG)
    # rotate on Sunday
    handler = logging.handlers.TimedRotatingFileHandler('/tmp/acd.log', when='w6', atTime=datetime.time())
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

    logger.info('remove torrent')
    root_items = remove_torrent(torrent_id)

    logger.info('preprocess')
    root_items = strip_files(torrent_root, root_items)

    logger.info('upload')
    yield upload(torrent_root, root_items)

    logger.info('remove items', root_items)
    for item in root_items:
        root = os.path.join(torrent_root, item)
        logger.debug(root)
        if os.path.isdir(root):
            shutil.rmtree(root)
        else:
            os.remove(root)


def remove_torrent(torrent_id):
    '''
    Returns root items.
    '''
    logger = logging.getLogger('tmacd')
    opt = OPTS['transmission']
    client = transmissionrpc.Client(opt['host'], port=opt['port'], user=opt['username'], password=opt['password'])
    torrent = client.get_torrent(torrent_id)
    files = torrent.files()
    common = set()
    for fid, item in files.items():
        if not item['selected']:
            continue
        parts = split_all(item['name'])
        common.add(parts[0])
    common = list(common)
    client.remove_torrent(torrent_id)
    logger.info('root items: {0}'.format(common))
    return common


def split_all(path):
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


def strip_files(torrent_root, root_items):
    logger = logging.getLogger('tmacd')

    to_be_deleted = []
    for root_item in root_items:
        root = os.path.join(torrent_root, root_item)
        if os.path.isdir(root):
            for path, dirs, files in os.walk(root):
                match_files(to_be_deleted, files)
        else:
            match_files(to_be_deleted, [root])

    logger.info('to be deleted: {0}'.format(to_be_deleted))

    for f in to_be_deleted:
        os.remove(f)

    return filter(lambda __: os.path.exists(os.path.join(torrent_root, __)), root_items)


def match_files(matched_list, files):
    patterns = (
        r'(girlcelly|NemuAndHaruka).*(txt|png)$',
        r'padding_file.*BitComet',
    )
    matched = filter(lambda f: any(map(lambda p: re.match(p, f) is not None, patterns)), files)
    matched_list.extend(matched)


@gen.coroutine
def upload(torrent_root, root_items):
    logger = logging.getLogger('acd')
    cmd = ['acdcli', '-v', 'upload', '-d']
    cmd.extend(map(lambda __: os.path.join(torrent_root, __), root_items))
    cmd.append('/tmp')
    logging.getLogger('tmacd').info('acdcli command: {0}'.format(cmd))
    p = process.Subprocess(cmd, stdout=subprocess.DEVNULL, stderr=process.Subprocess.STREAM)
    while True:
        try:
            chunk = yield p.stderr.read_bytes(65536, partial=True)
            logger.info(chunk.decode('utf-8').strip())
        except iostream.StreamClosedError as e:
            break
    exit_code = yield p.wait_for_exit()
    return exit_code


if __name__ == '__main__':
    exit_code = main()
    sys.exit(exit_code)
