import json

from pydrive.auth import GoogleAuth

from .network import Network
from . import util as u


API_ROOT = 'https://www.googleapis.com/drive/v3'
EMPTY_STRING = ''


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
            'changes': Changes(self._network),
            'files': Files(self._network),
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


class Changes(object):

    def __init__(self, network):
        self._network = network
        self._root = API_ROOT + '/changes'

    async def getStartPageToken(self):
        rv = await self._network.get(self._root + '/startPageToken')
        return rv.json_

    @u.optional(
        'includeCorpusRemovals',
        'includeRemoved',
        'includeTeamDriveItems',
        'pageSize',
        'restrictToMyDrive',
        'spaces',
        'supportsTeamDrives',
        'teamDriveId',
        'fields',
    )
    async def list_(self, pageToken, **kwargs):
        args = {
            'pageToken': pageToken,
        }
        args.update(kwargs)
        rv = await self._network.get(self._root, args)
        return rv


class Files(object):

    def __init__(self, network):
        self._network = network
        self._root = API_ROOT + '/files'
        self._upload_uri = 'https://www.googleapis.com/upload/drive/v3/files'

    # only for metadata
    @u.optional(
        'supportsTeamDrives',
        'fields',
    )
    async def get(self, fileId, **kwargs):
        args = kwargs
        rv = await self._network.get(self._root + '/' + fileId, args)
        return rv

    @u.optional(
        'corpora',
        'corpus',
        'includeTeamDriveItems',
        'orderBy',
        'pageSize',
        'pageToken',
        'q',
        'spaces',
        'supportsTeamDrives',
        'teamDriveId',
    )
    async def list_(self, **kwargs):
        args = kwargs
        rv = await self._network.get(self._root, args)
        return rv

    # download and send to streamingCallback
    @u.optional(
        'acknowledgeAbuse',
        'supportsTeamDrives',
    )
    async def download(self, fileId, range_, streamingCallback, **kwargs):
        args = {
            'alt': 'media',
        }
        args.update(**kwargs)
        headers = {
            'Range': 'bytes={0}-{1}'.format(*range_),
        }
        rv = await self._network.get(self._root + '/' + fileId, args,
                                     headers=headers,
                                     streaming_callback=streamingCallback)
        return rv

    @u.optional('mimeType')
    async def initiateUploading(self, fileName, totalFileSize, *kwargs):
        metadata = {
            'name': fileName,
        }
        metadata = json.dumps(metadata)
        metadata = metadata.encode('utf-8')
        args = {
            'uploadType': 'resumable',
        }
        headers = {
            'X-Upload-Content-Length': totalFileSize,
            'Content-Type': 'application/json; charset=UTF-8',
            'Content-Length': len(metadata),
        }
        mime_type = kwargs.get('mimeType', None)
        if mime_type is not None:
            headers['X-Upload-Content-Type'] = mime_type
        rv = await self._network.post(self._upload_uri, args, headers=headers,
                                      body=metadata)
        return rv

    @u.optional('mimeType')
    async def upload(self, uri, bodyProducer, offset, bodySize):
        headers = {
            'Content-Length': bodySize - offset,
            'Content-Range': 'bytes {0}-{1}/{2}'.format(offset, bodySize - 1,
                                                        bodySize),
        }
        mime_type = kwargs.get('mimeType', None)
        if mime_type is not None:
            headers['Content-Type'] = mime_type
        rv = await self._network.put(uri, headers=headers,
                                     body_producer=bodyProducer)
        return rv

    async def getUploadStatus(self, uri, totalFileSize):
        headers = {
            'Content-Length': 0,
            'Content-Range': 'bytes */{0}'.format(totalFileSize),
        }
        rv = await self._network.put(uri, headers=headers)
        return rv
