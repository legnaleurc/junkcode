#! /bin/sh

poetry run -- python3 -m app.faststart -c "~/.config/wcpan.drive/cli.yaml" --data-path="~/.local/share/wcpan.drive/_faststart" "$@"
