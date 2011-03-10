#! /bin/sh

LAME='--enable-libmp3lame'
SHARED='--disable-static --enable-shared'
OPT='--disable-optimizations --disable-stripping'
CC='gcc'

../configure \
--cc="$CC" \
--extra-cflags='-pipe' \
--prefix='/usr/local' \
--enable-gpl \
--enable-version3 \
$SHARED \
$LAME \
$OPT \
--disable-ffplay \
--disable-ffserver \
--enable-pic \
--arch=amd64 \
;
