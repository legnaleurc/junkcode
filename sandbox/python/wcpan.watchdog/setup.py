# import os.path as op

from setuptools import setup


# with open(op.join(op.dirname(__file__), './README.md')) as fin:
#     long_description = fin.read()


setup(
        name='wcpan.watchdog',
        version='1.0.0',
        author='Wei-Cheng Pan',
        author_email='legnaleurc@gmail.com',
        description='Asynchronous watchdog.',
#        long_description=long_description,
#        long_description_content_type='text/markdown',
        url='https://github.com/legnaleurc/wcpan.watchdog',
        packages=[
            'wcpan.watchdog',
        ],
        python_requires='>= 3.7',
        classifiers=[
            'Programming Language :: Python :: 3 :: Only',
            'Programming Language :: Python :: 3.7',
        ])
