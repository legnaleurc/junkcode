#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR=$(dirname "$(realpath "$0")")

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <profile>" >&2
    exit 1
fi

PROFILE=$1
source "$SCRIPT_DIR/$PROFILE"

STAGING_DIR=/tmp/nfs-staging-$PROFILE

if mountpoint -q "$MOUNT_POINT" 2>/dev/null; then
    umount "$MOUNT_POINT"
fi

if [[ -d "$MOUNT_POINT" ]] && [[ -z "$(ls -A "$MOUNT_POINT")" ]]; then
    rmdir "$MOUNT_POINT"
fi

if mountpoint -q "$STAGING_DIR" 2>/dev/null; then
    umount "$STAGING_DIR"
fi

if [[ -d "$STAGING_DIR" ]] && [[ -z "$(ls -A "$STAGING_DIR")" ]]; then
    rmdir "$STAGING_DIR"
fi
