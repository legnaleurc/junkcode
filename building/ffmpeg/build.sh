#! /bin/sh

../configure \
--prefix='/usr/local' \
--disable-static \
--enable-shared \
--enable-gpl \
--disable-ffmpeg \
--disable-ffplay \
--disable-ffserver \
--disable-stripping \
;
