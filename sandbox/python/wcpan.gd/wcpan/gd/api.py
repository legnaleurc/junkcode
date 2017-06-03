from pydrive.auth import GoogleAuth

from .network import Network


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
        http = self._oauth.Get_Http_Object()
        self._network = Network(http)
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

    def getStartPageToken(self):
        rv = self._network.get(self._root + '/startPageToken')
        return rv.json_

    def list_(self, pageToken, includeCorpusRemovals=False,
              includeRemoved=True, includeTeamDriveItems=False, pageSize=100,
              restrictToMyDrive=False, spaces=None, supportsTeamDrives=False,
              teamDriveId=None, fields=None):
        args = {
            'pageToken': pageToken,
            'includeCorpusRemovals': includeCorpusRemovals,
            'includeRemoved': includeRemoved,
            'pageSize': pageSize,
            'restrictToMyDrive': restrictToMyDrive,
            'supportsTeamDrives': supportsTeamDrives,
        }
        if spaces:
            args['spaces'] = spaces
        if teamDriveId:
            args['teamDriveId'] = teamDriveId
            args['includeTeamDriveItems'] = includeTeamDriveItems
        if fields:
            args['fields'] = fields
        rv = self._network.get(self._root, args)
        return rv.json_


class Files(object):

    def __init__(self, network):
        self._network = network
        self._root = API_ROOT + '/files'

    def get(self, fileId, acknowledgeAbuse=False, supportsTeamDrives=False,
            fields=None):
        args = {
            'fileId': fileId,
            'acknowledgeAbuse': acknowledgeAbuse,
            'supportsTeamDrives': supportsTeamDrives,
        }
        if fields:
            args['fields'] = fields
        rv = self._network.get(self._root + '/' + fileId, args)
        return rv.json_
