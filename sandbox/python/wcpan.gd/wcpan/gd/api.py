import json
from typing import Awaitable, Callable, Dict, Tuple

from pydrive.auth import GoogleAuth
from wcpan.logger import EXCEPTION, DEBUG

from .network import Network, Response, NetworkError
from .util import FOLDER_MIME_TYPE


API_ROOT = 'https://www.googleapis.com/drive/v3'
EMPTY_STRING = ''

Consumer = Callable[[bytes], None]
Producer = Callable[[Callable[[bytes], Awaitable[None]]], Awaitable[None]]
# producer consumer


class Client(object):

    def __init__(self, settings):
        self._settings = settings
        self._oauth = None
        self._network = None
        self._api = None

    def initialize(self):
        if self._oauth:
            return

        s = self._settings
        args = {
            'client_config_backend': s['client_config_backend'],
            'client_config_file': s['client_config_file'],
            'get_refresh_token': s['get_refresh_token'],
            'save_credentials': s['save_credentials'],
            'save_credentials_backend': s['save_credentials_backend'],
            'save_credentials_file': s['save_credentials_file'],
        }
        self._oauth = GoogleAuth()
        # HACK undocumented behavior
        self._oauth.settings.update(args)
        self._oauth.CommandLineAuth()
        # HACK undocumented behavior
        self._network = Network(self._oauth.credentials.access_token)
        self._api = {
            'changes': Changes(self),
            'files': Files(self),
        }

    @property
    def authorized(self):
        return self._oauth is not None

    @property
    def changes(self):
        return self._api['changes']

    @property
    def files(self):
        return self._api['files']

    def _refresh_token(self):
        self._oauth.Refresh()

    async def _get(self, *args, **kwargs):
        while True:
            try:
                rv = await self._network.get(*args, **kwargs)
                break
            except NetworkError as e:
                if e.status != '401':
                    raise
            DEBUG('wcpan.gd') << 'refresh token'
            self._refresh_token()
        return rv

    async def _post(self, *args, **kwargs):
        while True:
            try:
                rv = await self._network.post(*args, **kwargs)
                break
            except NetworkError as e:
                if e.status != '401':
                    raise
            DEBUG('wcpan.gd') << 'refresh token'
            self._refresh_token()
        return rv

    async def _put(self, *args, **kwargs):
        while True:
            try:
                rv = await self._network.put(*args, **kwargs)
                break
            except NetworkError as e:
                if e.status != '401':
                    raise
            DEBUG('wcpan.gd') << 'refresh token'
            self._refresh_token()
        return rv


class Changes(object):

    def __init__(self, client: Client) -> None:
        self._client = client
        self._root = API_ROOT + '/changes'

    async def get_start_page_token(self, supports_team_drives: bool = None,
                                   team_drive_id: str = None) -> Response:
        args = {}
        if supports_team_drives is not None:
            args['supportsTeamDrives'] = supports_team_drives
        if team_drive_id is not None:
            args['teamDriveId'] = team_drive_id

        rv = await self._client._get(self._root + '/startPageToken', args)
        return rv

    async def list_(self, page_token: str, include_corpus_removals: bool = None,
                    include_removed: bool = None,
                    include_team_drive_items: bool = None,
                    page_size: int = None, restrict_to_my_drive: bool = None,
                    spaces: str = None, supports_team_drives: bool = None,
                    team_drive_id: str = None, fields: str = None) -> Response:
        args = {
            'pageToken': page_token,
        }
        if include_corpus_removals is not None:
            args['includeCorpusRemovals'] = include_corpus_removals
        if include_removed is not None:
            args['includeRemoved'] = include_removed
        if include_team_drive_items is not None:
            args['includeTeamDriveItems'] = include_team_drive_items
        if page_size is not None:
            args['pageSize'] = page_size
        if restrict_to_my_drive is not None:
            args['restrictToMyDrive'] = restrict_to_my_drive
        if spaces is not None:
            args['spaces'] = spaces
        if supports_team_drives is not None:
            args['supportsTeamDrives'] = supports_team_drives
        if team_drive_id is not None:
            args['teamDriveId'] = team_drive_id
        if fields is not None:
            args['fields'] = fields

        rv = await self._client._get(self._root, args)
        return rv


class Files(object):

    def __init__(self, client: Client) -> None:
        self._client = client
        self._root = API_ROOT + '/files'
        self._upload_uri = 'https://www.googleapis.com/upload/drive/v3/files'

    # only for metadata
    async def get(self, file_id: str, supports_team_drives: bool = None,
                  fields: str = None) -> Response:
        args = {}
        if supports_team_drives is not None:
            args['supportsTeamDrives'] = supports_team_drives
        if fields is not None:
            args['fields'] = fields

        rv = await self._client._get(self._root + '/' + file_id, args)
        return rv

    async def list_(self, corpora: str = None, corpus: str = None,
                    include_team_drive_items: bool = None, order_by: str = None,
                    page_size: int = None, page_token: str = None,
                    q: str = None, spaces: str = None,
                    supports_team_drives: bool = None,
                    team_drive_id: str = None, fields: str = None) -> Response:
        args = {}
        if corpora is not None:
            args['corpora'] = corpora
        if corpus is not None:
            args['corpus'] = corpus
        if include_team_drive_items is not None:
            args['includeTeamDriveItems'] = include_team_drive_items
        if order_by is not None:
            args['orderBy'] = order_by
        if page_size is not None:
            args['pageSize'] = page_size
        if page_token is not None:
            args['pageToken'] = page_token
        if q is not None:
            args['q'] = q
        if spaces is not None:
            args['spaces'] = spaces
        if supports_team_drives is not None:
            args['supportsTeamDrives'] = supports_team_drives
        if team_drive_id is not None:
            args['teamDriveId'] = team_drive_id
        if fields is not None:
            args['fields'] = fields

        rv = await self._client._get(self._root, args)
        return rv

    # download and send to streaming_callback
    async def download(self, file_id: str, range_: Tuple[int, int],
                       consumer: Consumer, acknowledge_abuse: bool = None,
                       supports_team_drives: bool = None) -> Response:
        args = {
            'alt': 'media',
        }
        if acknowledge_abuse is not None:
            args['acknowledgeAbuse'] = acknowledge_abuse
        if supports_team_drives is not None:
            args['supportsTeamDrives'] = supports_team_drives

        headers = {
            'Range': 'bytes={0}-{1}'.format(*range_),
        }

        rv = await self._client._get(self._root + '/' + file_id, args,
                                     headers=headers, consumer=consumer)
        return rv

    async def initiate_uploading(self, file_name: str, total_file_size: int,
                                 parent_id: str = None,
                                 mime_type: str = None) -> Response:
        metadata = {
            'name': file_name,
        }
        if parent_id is not None:
            metadata['parents'] = [parent_id]
        metadata = json.dumps(metadata)
        metadata = metadata.encode('utf-8')
        headers = {
            'X-Upload-Content-Length': total_file_size,
            'Content-Type': 'application/json; charset=UTF-8',
            'Content-Length': len(metadata),
        }
        if mime_type is not None:
            headers['X-Upload-Content-Type'] = mime_type

        args = {
            'uploadType': 'resumable',
        }

        rv = await self._client._post(self._upload_uri, args, headers=headers,
                                      body=metadata)
        return rv

    async def upload(self, uri: str, producer: Producer, offset: int,
                     total_file_size: int, mime_type: str = None) -> Response:
        last_position = total_file_size - 1
        headers = {
            'Content-Length': total_file_size - offset,
            'Content-Range': 'bytes {0}-{1}/{2}'.format(offset, last_position,
                                                        total_file_size),
        }
        if mime_type is not None:
            headers['Content-Type'] = mime_type

        # upload usually need more time to complete
        rv = await self._client._put(uri, headers=headers, body=producer,
                                     timeout=3600.0)
        return rv

    async def get_upload_status(self, uri: str,
                                total_file_size: int) -> Response:
        headers = {
            'Content-Length': 0,
            'Content-Range': 'bytes */{0}'.format(total_file_size),
        }
        rv = await self._client._put(uri, headers=headers)
        return rv

    async def create_folder(self, folder_name: str,
                            parent_id: str = None) -> Response:
        metadata = {
            'name': folder_name,
            'mimeType': FOLDER_MIME_TYPE,
        }
        if parent_id is not None:
            metadata['parents'] = [parent_id]
        metadata = json.dumps(metadata)
        metadata = metadata.encode('utf-8')
        headers = {
            'Content-Type': 'application/json; charset=UTF-8',
            'Content-Length': len(metadata),
        }

        rv = await self._client._post(self._root, headers=headers,
                                      body=metadata)
        return rv
