from tornado import ioloop, gen
import pyquery as pq

from .util import HTTPSession


_API_PATHS = {
    'login': '/ap/oa',
}

_CLIENT_ID = ''
_EMAIL = ''
_PASSWORD = ''


class Session(object):

    def __init__(self):
        pass

    @gen.coroutine
    def login(self):
        curl = HTTPSession()
        html = yield curl.get('https://amazon.com/ap/oa', {
            'client_id': _CLIENT_ID,
            'scope': 'clouddrive:read_all',
            #'scope': 'clouddrive:write',
            #'scope': 'clouddrive:read clouddrive:write',
            'response_type': 'token',
            #'response_type': 'code',
            'redirect_uri': 'http://127.0.0.1:8000/',
        })

        q = pq.PyQuery(html)
        form = q('#ap_signin_form')
        inputs = form('input')

        action = form.attr.action
        data = {}
        for i in inputs:
            a = i.attrib
            if 'name' in a and 'value' in a:
                data[a['name']] = a['value']
            elif 'name' in a:
                data[a['name']] = None
            else:
                print('!!!!!!{0}'.format(i))
        data['email'] = _EMAIL
        data['password'] = _PASSWORD

        html = yield curl.post(action, data)

        q = pq.PyQuery(html)
        form = q('form[name="consent-form"]')
        inputs = form('input')

        action = form.attr.action
        data = {}
        for i in inputs:
            a = i.attrib
            if 'name' in a and 'value' in a:
                data[a['name']] = a['value']
            elif 'name' in a:
                data[a['name']] = None
            else:
                print('!!!!!!{0}'.format(i))
        #data['consentDenied'] = ''
        data['consentApproved'] = ''

        html = yield curl.get(action, data)

        return html
