#! /bin/sh

poetry run -- python3 -m app.hah --drive-path="~/.config/wcpan.drive/cli.yaml" "$@"
