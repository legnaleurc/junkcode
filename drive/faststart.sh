#! /bin/sh

exec poetry run -- python3 -m app.faststart --data-path="~/.local/share/wcpan.drive/_faststart" "$@"
