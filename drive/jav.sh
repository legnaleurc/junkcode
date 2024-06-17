#! /bin/sh

OPENSSL_CONF=./assets/openssl.cnf exec poetry run -- python3 -m app.jav "$@"
