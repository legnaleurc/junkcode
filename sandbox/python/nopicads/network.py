import urllib2

import mechanize

from parser import Pack


USERNAME = ''
PASSWORD = ''


def gh_open(rev, path):
    url = u'https://raw.github.com/legnaleurc/nopicads/{rev}{path}'
    url = url.format(rev=rev, path=path)
    link = urllib2.urlopen(url)
    return link


def get_changelog(rev):
    nin = gh_open(rev, u'/CHANGELOG.md')
    cl = nin.read()
    nin.close()
    cl = cl.decode('utf-8')
    return cl


def get_readme(rev):
    nin = gh_open(rev, u'/README.md')
    rm = nin.read()
    nin.close()
    rm = rm.decode('utf-8')
    return rm


def change(cl, rm):
    b = mechanize.Browser()

    b.open('https://userscripts.org/login')
    b.select_form(nr=0)
    b['login'] = USERNAME
    b['password'] = PASSWORD
    b.submit()

    b.open('https://userscripts.org/scripts/edit/154858')
    b.select_form(nr=0)
    uso = b['script[description_extended]']
    pack = Pack(cl, rm, uso)
    uso = pack.getResult()
    b['script[description_extended]'] = uso
    b.submit()

    b.close()


def perform(rev):
    cl = get_changelog(rev)
    rm = get_readme(rev)
    change(cl, rm)
