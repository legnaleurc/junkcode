import re


PATTERN = r'([^.\s]{1,256}\.){0,2}[^.\s]{1,256}\.[^.\s]{1,256}'


def extractURLs(text):
    m = re.finditer(PATTERN, text)
    m = [_.group(0) for _ in m]
    return m
