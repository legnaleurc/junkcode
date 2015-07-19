#! /bin/sh

curl -q -G \
    --data-urlencode "torrent_root=$TR_TORRENT_DIR" \
    --data-urlencode "torrent_id=$TR_TORRENT_ID" \
    'http://127.0.0.1:1234/'
