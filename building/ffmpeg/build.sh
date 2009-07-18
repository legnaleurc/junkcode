#! /bin/sh

LAME='--enable-libmp3lame'
#SHARED='--disable-static --enable-shared --disable-stripping'
OPT='--disable-optimizations'

../configure \
--cc='gcc-3.4' \
--prefix='/usr/local' \
--enable-gpl \
$SHARED \
$LAME \
$OPT \
--disable-ffplay \
--disable-ffserver \
--disable-network \
--disable-ipv6 \
;
