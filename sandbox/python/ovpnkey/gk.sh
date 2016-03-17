#! /bin/sh

cd "$1"
. ./vars
export KEY_EMAIL="$2"
./pkitool "$3"
