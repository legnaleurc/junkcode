#! /bin/sh

../configure \
--prefix='/usr/local' \
--disable-static \
--enable-shared \
--enable-gpl \
--disable-ffplay \
--disable-ffserver \
--disable-network \
--disable-ipv6 \
--disable-stripping \
;
