#! /usr/bin/env python3

# import cgi
import sys

import requests


# form = cgi.FieldStorage()
# mac = form.getfirst('mac')

# r = requests.get('http://robin.skywatch24.com/GetTunnel.php', params={
#     'MAC': mac,
# })

r = requests.get('https://beta.skywatch24.com/api/v2/cameras/1/rtmpstream', cookies={
    'api_key': '4d2b4574fefbe072ec53523535f368aa',
})

sys.stdout.write('Content-Type: text/plain; encoding=utf-8\r\n')
sys.stdout.write('\r\n')
sys.stdout.write(r.text)
