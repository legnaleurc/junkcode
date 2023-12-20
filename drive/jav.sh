#! /bin/sh

OPENSSL_CONF=./assets/openssl.cnf poetry run -- python3 -m app.jav  "$@"
