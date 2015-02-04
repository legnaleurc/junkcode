#! /usr/bin/env python3


import contextlib
import time
from urllib.parse import urlparse
import os

import requests
from selenium import webdriver


CHROME_STORE_URL = 'https://clients2.google.com/service/update2/crx?response=redirect&prodversion=40.0&x=id%3D{id}%26installsource%3Dondemand%26uc'
REQUEST_HEADERS = {
    'User-Agent': 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/40.0.2214.91 Safari/537.36',
    'Referer': 'https://chrome.google.com',
}
TM_ID = 'dhdgffkkebhmkfjojejmpbldmpobfkfo'
TM_PATH = '/tmp/tampermonkey.crx'


@contextlib.contextmanager
def quiting(thing):
    try:
        yield thing
    finally:
        thing.quit()


def download(remote, local):
    r = requests.get(url=remote, headers=REQUEST_HEADERS, stream=True)
    size = 0
    with open(local, 'wb') as fout:
        for chunk in r.iter_content(8192):
            fout.write(chunk)
            size += len(chunk)
            fout.flush()
    return size


def main(args=None):
    if not os.path.exists(TM_PATH):
        download(CHROME_STORE_URL.format(id=TM_ID), TM_PATH)

    profile = webdriver.ChromeOptions()
    profile.add_extension(extension=TM_PATH)

    with quiting(webdriver.Chrome(chrome_options=profile)) as driver:
        # Tampermonkey may not ready yet
        time.sleep(5)
        driver.get('https://adsbypasser.github.io/releases/adsbypasser.user.js')
        time.sleep(5)

        # find the confirm tab
        # TODO close tabs
        handles = driver.window_handles
        for handle in handles:
            driver.switch_to.window(handle)
            if driver.current_url.startswith('chrome-extension://'):
                install = driver.find_element_by_css_selector('input.install[value$=nstall]')
                install.click()
                break

        # ready

main()
