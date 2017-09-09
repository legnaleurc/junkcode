import os
import re
from setuptools import setup


setup(
    name='wcpan.model',
    description='Declarative dictionary-based model classes for Python',
    version='1.0.0.dev1',
    packages=[
        'wcpan.model',
    ],
    url='https://github.com/legnaleurc/wcpan.model',
    author='Wei-Cheng Pan',
    author_email='legnaleurc@gmail.com',
    classifiers = [
        'Programming Language :: Python :: 3 :: Only',
        'Programming Language :: Python :: 3.3',
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'Operating System :: OS Independent',
        'Topic :: Software Development :: Libraries :: Python Modules',
    ],
)
