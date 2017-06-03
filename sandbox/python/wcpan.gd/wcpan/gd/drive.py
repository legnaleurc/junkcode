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

    def sync(self):
        assert self._client.authorized

        try:
            check_point = self._db.metadata['check_point']
        except KeyError:
            check_point = '1'

        # first time, get root node
        if check_point == '1':
            rv = self._client.files.get('root', fields='id,name,mimeType,trashed,parents,createdTime,modifiedTime')
            rv['name'] = None
            rv['parents'] = []
            node = Node.from_api(rv)
            self._db.insert_node(node)

        new_start_page_token = None
        changes_list_args = {
            'pageToken': check_point,
            'pageSize': 1000,
            'restrictToMyDrive': True,
            'fields': 'nextPageToken,newStartPageToken,changes(file(id,name,mimeType,trashed,parents,createdTime,modifiedTime,md5Checksum,size))',
        }

        while new_start_page_token is None:
            rv = self._client.changes.list_(**changes_list_args)
            next_page_token = rv.get('nextPageToken', None)
            new_start_page_token = rv.get('newStartPageToken', None)
            changes = rv['changes']

            print('next_page_token: {0}, new_start_page_token: {1}, changes: {2}'.format(next_page_token, new_start_page_token, len(changes)))
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
