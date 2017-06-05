import os.path as op
import mimetypes

from .api import Client
from .database import Database, Node
from .util import Settings


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
            rv = await self._client.files.get('root', fields='id,name,mimeType,trashed,parents,createdTime,modifiedTime')
            rv = rv.json_
            rv['name'] = None
            rv['parents'] = []
            node = Node.from_api(rv)
            self._db.insert_node(node)

        new_start_page_token = None
        changes_list_args = {
            'pageToken': check_point,
            'pageSize': 1000,
            'restrictToMyDrive': True,
            'fields': 'nextPageToken,newStartPageToken,changes(fileId,removed,file(id,name,mimeType,trashed,parents,createdTime,modifiedTime,md5Checksum,size))',
        }

        while new_start_page_token is None:
            rv = await self._client.changes.list_(**changes_list_args)
            rv = rv.json_
            next_page_token = rv.get('nextPageToken', None)
            new_start_page_token = rv.get('newStartPageToken', None)
            changes = rv['changes']

            check_point = next_page_token if next_page_token is not None else new_start_page_token

            self._db.apply_changes(changes, check_point)
            changes_list_args['pageToken'] = check_point

    @property
    def root_node(self):
        return self._db.root_node

    def get_node_by_id(self, node_id):
        return self._db.get_node_by_id(node_id)

    def get_node_by_path(self, path):
        return self._db.get_node_by_path(path)

    def get_path_by_id(self, node_id):
        return self._db.get_path_by_id(node_id)

    def get_children_by_id(self, node_id):
        return self._db.get_children_by_id(node_id)

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
                print(len(chunk))
                fout.write(chunk)

            api = self._client.files
            rv = await api.download(fileId=node_id, range_=range_,
                                    streamingCallback=writer)

        # TODO rename it back if completed

    async def upload_file(self, file_path, node):
        # sanity check
        if not node:
            return None
        if not node.is_folder:
            return None
        if not op.isfile(file_path):
            return None

        files_api = self._client.files
        file_name = op.basename(file_path)
        total_file_size = op.getsize(file_path)
        mt, e = mimetypes.guess_type(file_path)
        rv = await files_api.initiateUploading(fileName=file_name,
                                               totalFileSize=total_file_size,
                                               mimeType=mt)
        # TODO handle errors, assuming 200
        if rv.status != '200':
            print('first', rv.json_)
            raise Exception('test')
        url = rv.get_header('Location')

        with open(file_path, 'rb') as fin:
            async def reader(write):
                chunk = fin.read(65536)
                await write(chunk)

            rv = await files_api.upload(url, bodyProducer=reader, offset=0,
                                        bodySize=total_file_size, mimeType=mt)
            if rv.status != '200':
                print('second', rv.json_)
                raise Exception('test')
            # TODO handle errors, assuming 200

        rv = rv.json_
        return rv
