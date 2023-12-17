#! /bin/sh

poetry run -- python3 -m app.faststart --drive-path="~/.config/wcpan.drive/cli.yaml" --data-path="~/.local/share/wcpan.drive/_faststart" "$@"
