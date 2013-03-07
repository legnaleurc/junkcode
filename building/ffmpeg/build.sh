#! /bin/sh

LAME='--enable-libmp3lame'
SHARED='--disable-static --enable-shared'
OPT='--disable-optimizations --disable-stripping'

../configure \
--extra-cflags='-pipe' \
--prefix='/usr/local' \
--enable-gpl \
--enable-version3 \
$SHARED \
$LAME \
$OPT \
--disable-ffplay \
--disable-ffserver \
--disable-ffprobe \
--enable-pic \
--arch=amd64 \
;
