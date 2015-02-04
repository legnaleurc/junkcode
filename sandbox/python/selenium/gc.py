#! /usr/bin/env python3


import contextlib
import time

from selenium import webdriver
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.keys import Keys


@contextlib.contextmanager
def quiting(thing):
    try:
        yield thing
    finally:
        thing.quit()


profile = webdriver.ChromeOptions()
profile.add_extension(extension='/tmp/Tampermonkey_v3.9.202.crx')


with quiting(webdriver.Chrome(chrome_options=profile)) as driver:
    time.sleep(5)
    driver.get('https://adsbypasser.github.io/releases/adsbypasser.user.js')
    time.sleep(5)
    handles = driver.window_handles

    for handle in handles:
        driver.switch_to.window(handle)
        if driver.current_url.startswith('chrome-extension://'):
            install = driver.find_element_by_css_selector('input.install[value$=nstall]')
            install.click()
    time.sleep(10)
