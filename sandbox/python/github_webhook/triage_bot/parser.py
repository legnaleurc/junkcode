import re


PATTERN = r'([^.]{1,256}\.){0,2}[^.]{1,256}\.[^.]{1,256}'


def extractURLs(text):
    m = re.finditer(PATTERN, text)
    m = [_.group(0) for _ in m]
    return m
