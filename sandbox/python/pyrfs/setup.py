import os.path as op

from setuptools import setup, Extension


unpack = Extension(
    '_unpack',
    sources=[
        'unpack/glue.c',
        'unpack/unpack.cpp',
    ],
    libraries=[
        'archive',
    ],
)


setup(
    name='pyrfs',
    version='1.0.0',
    author='Wei-Cheng Pan',
    author_email='legnaleurc@gmail.com',
    description='pyrfs',
    url='https://github.com/legnaleurc/pyrfs',
    packages=[
        'server',
    ],
    python_requires='>= 3.7',
    install_requires=[
        'aiohttp ~= 3.4.4',
        'wcpan.logger ~= 1.2.3',
        'wcpan.drive.google ~= 3.0.0',
    ],
    ext_modules=[unpack],
    classifiers=[
        'Programming Language :: Python :: 3 :: Only',
        'Programming Language :: Python :: 3.7',
    ])
