#! /usr/bin/env python3


import contextlib
import time
import os

import requests
from selenium import webdriver


USERSCRIPT = 'https://adsbypasser.github.io/releases/adsbypasser.user.js'
GM_URL = 'https://addons.mozilla.org/firefox/downloads/file/282084/greasemonkey-2.3-fx.xpi'
GM_PATH = '/tmp/gm.xpi'


def download(remote, local):
    r = requests.get(remote, stream=True)
    size = 0
    with open(local, 'wb') as fout:
        for chunk in r.iter_content(chunk_size=8192):
            if chunk:
                fout.write(chunk)
                size += len(chunk)
                f.flush()
    return size


@contextlib.contextmanager
def quiting(thing):
    try:
        yield thing
    finally:
        thing.quit()


def main(args=None):
    if not os.path.exists(GM_PATH):
        download(GM_URL, GM_PATH)

    profile = webdriver.FirefoxProfile()
    profile.add_extension(extension=GM_PATH)

    with quiting(webdriver.Firefox(firefox_profile=profile)) as driver:
        try:
            driver.get('https://adsbypasser.github.io/releases/adsbypasser.user.js')
        except Exception as e:
            # expected exception: UI thread locked by modal dialog
            pass
        # wait for the dialog disappear
        time.sleep(10)

        # ready
