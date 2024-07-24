#! /bin/sh

exec poetry run -- python3 -m app.lfs "$@"
