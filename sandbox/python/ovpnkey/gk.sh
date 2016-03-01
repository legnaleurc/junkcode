#! /bin/sh

cd /etc/openvpn/easy-rsa/2.0
. ./vars
export KEY_EMAIL="$1"
./pkitool "$2"
