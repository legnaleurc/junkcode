import functools as ft
import mimetypes
import os.path as op
import re

from .api import Client
from .database import Database, Node
from .network import NetworkError
from .util import Settings, GoogleDriveError


FILE_FIELDS = 'id,name,mimeType,trashed,parents,createdTime,modifiedTime,md5Checksum,size'


class Drive(object):

    def __init__(self, settings_path=None):
        self._settings = Settings(settings_path)
        self._client = Client(self._settings)
        self._db = Database(self._settings)

    def close(self):
        self._db.close()

    def initialize(self):
        self._client.initialize()
        self._db.initialize()

    async def sync(self):
        assert self._client.authorized

        try:
            check_point = self._db.metadata['check_point']
        except KeyError:
            check_point = '1'

        # first time, get root node
        if check_point == '1':
            rv = await self._client.files.get('root', fields=FILE_FIELDS)
            rv = rv.json_
            rv['name'] = None
            rv['parents'] = []
            node = Node.from_api(rv)
            self._db.insert_node(node)

        new_start_page_token = None
        fields = 'nextPageToken,newStartPageToken,changes(fileId,removed,file({0}))'.format(FILE_FIELDS)
        changes_list_args = {
            'page_token': check_point,
            'page_size': 1000,
            'restrict_to_my_drive': True,
            'fields': fields,
        }

        while new_start_page_token is None:
            rv = await self._client.changes.list_(**changes_list_args)
            rv = rv.json_
            next_page_token = rv.get('nextPageToken', None)
            new_start_page_token = rv.get('newStartPageToken', None)
            changes = rv['changes']

            check_point = next_page_token if next_page_token is not None else new_start_page_token

            self._db.apply_changes(changes, check_point)
            changes_list_args['page_token'] = check_point

    @property
    def root_node(self):
        return self._db.root_node

    def get_node_by_id(self, node_id):
        return self._db.get_node_by_id(node_id)

    def get_node_by_path(self, path):
        return self._db.get_node_by_path(path)

    def get_path_by_id(self, node_id):
        return self._db.get_path_by_id(node_id)

    def get_child_by_id(self, node_id, name):
        return self._db.get_child_by_id(node_id, name)

    def get_children_by_id(self, node_id):
        return self._db.get_children_by_id(node_id)

    def find_duplicate_nodes(self):
        return self._db.find_duplicate_nodes()

    async def download_file_by_id(self, node_id, path):
        node = self.get_node_by_id(node_id)
        return await self.download(node, path)

    async def download_file(self, node, path):
        # sanity check
        if not node:
            return False
        if node.is_folder:
            return False
        if not op.exists(path):
            return False

        # resume download
        tmp_path = op.join(path, node.name + '.__tmp__')
        if op.exists(tmp_path):
            offset = op.getsize(tmp_path)
        else:
            offset = 0
        range_ = (offset, node.size)

        with open(tmp_path, 'ab') as fout:
            def writer(chunk):
                fout.write(chunk)

            api = self._client.files
            rv = await api.download(file_id=node_id, range_=range_,
                                    consumer=writer)

        # TODO rename it back if completed

    async def upload_file(self, file_path, parent_node):
        # sanity check
        if not parent_node:
            return None
        if not parent_node.is_folder:
            return None
        if not op.isfile(file_path):
            return None

        files_api = self._client.files
        file_name = op.basename(file_path)

        # do not upload if remote exists a same file
        node = await self.fetch_child_by_id(parent_node.id_, file_name)
        if node:
            return node

        total_file_size = op.getsize(file_path)
        mt, e = mimetypes.guess_type(file_path)
        rv = await files_api.initiate_uploading(file_name=file_name,
                                                total_file_size=total_file_size,
                                                parent_id=parent_node.id_,
                                                mime_type=mt)

        url = rv.get_header('Location')

        with open(file_path, 'rb') as fin:
            reader = ft.partial(file_producer, fin)
            offset = 0
            uploader = ft.partial(self._inner_try_upload_file,
                                  url=url, producer=reader,
                                  total_file_size=total_file_size, mime_type=mt)

            while True:
                ok, rv = await uploader(offset=offset)
                if ok:
                    break
                offset = rv

        rv = rv.json_
        node = await self.fetch_node_by_id(rv['id'])
        return node

    async def fetch_child_by_id(self, node_id, name):
        safe_name = re.sub(r"[\\']", r"\\\g<0>", name)
        query = "'{0}' in parents and name = '{1}'".format(node_id,
                                                           name)
        fields = 'files({0})'.format(FILE_FIELDS)
        rv = await self._client.files.list_(q=query, fields=fields)
        if rv.status != '200':
            raise Exception('list failed')

        rv = rv.json_
        files = rv['files']
        if not files:
            return None

        node = Node.from_api(files[0])
        self._db.insert_node(node)
        return node

    async def fetch_node_by_id(self, node_id):
        rv = await self._client.files.get(node_id, fields=FILE_FIELDS)
        rv = rv.json_
        node = Node.from_api(rv)
        self._db.insert_node(node)
        return node

    async def _inner_try_upload_file(self, url, producer, offset,
                                     total_file_size, mime_type):
        api = self._client.files

        try:
            rv = await api.upload(url, producer=producer, offset=offset,
                                  total_file_size=total_file_size,
                                  mime_type=mime_type)
            return True, rv
        except NetworkError as e:
            if e.status == '404':
                raise UploadError('the upload session has been expired')
            if e.fatal:
                raise

        while True:
            try:
                rv = await api.get_upload_status(url, total_file_size)
                break
            except NetworkError as e:
                if e.fatal:
                    raise

        if rv.status != '308':
            raise UploadError('invalid upload status')
        rv = rv.get_header('Range')
        if not rv:
            raise UploadError('invalid upload status')
        rv = re.match(r'bytes=(\d+)-(\d+)', rv)
        if not rv:
            raise UploadError('invalid upload status')
        rv = int(rv.group(2))

        return False, rv


class UploadError(GoogleDriveError):

    def __init__(self, message):
        self._message = message

    def __str__(self):
        return self._message


async def file_producer(fin, write):
    chunk = fin.read(65536)
    await write(chunk)
