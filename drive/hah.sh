#! /bin/sh

poetry run -- python3 -m app.hah -c "~/.config/wcpan.drive/cli.yaml" "$@"
