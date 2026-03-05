#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR=$(dirname "$(realpath "$0")")

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <profile>" >&2
    exit 1
fi

PROFILE=$1
source "$SCRIPT_DIR/$PROFILE"

if [[ -z "${NAS_HOST:-}" || -z "${NFS_EXPORT:-}" || -z "${MOUNT_POINT:-}" ]]; then
    echo "Error: NAS_HOST, NFS_EXPORT, and MOUNT_POINT must be set in profile" >&2
    exit 1
fi

NFS_OPTS=${NFS_OPTS:-nfsvers=4.1,noresvport,soft,tcp}
STAGING_DIR=/tmp/nfs-staging-$PROFILE

if mountpoint -q "$MOUNT_POINT" 2>/dev/null; then
    echo "$MOUNT_POINT is already mounted" >&2
    exit 0
fi

mkdir -p "$STAGING_DIR"

if ! mountpoint -q "$STAGING_DIR" 2>/dev/null; then
    mount -t nfs -o "$NFS_OPTS" "$NAS_HOST:$NFS_EXPORT" "$STAGING_DIR"
fi

mkdir -p "$MOUNT_POINT"

MAP_OPT=
if [[ -n "${NAS_UID:-}" && -n "${WSL_UID:-}" ]]; then
    MAP_OPT="$NAS_UID/$WSL_UID"
fi
if [[ -n "${NAS_GID:-}" && -n "${WSL_GID:-}" ]]; then
    if [[ -n "$MAP_OPT" ]]; then
        MAP_OPT="$MAP_OPT:@$NAS_GID/@$WSL_GID"
    else
        MAP_OPT="@$NAS_GID/@$WSL_GID"
    fi
fi

BINDFS_ARGS=()
if [[ -n "$MAP_OPT" ]]; then
    BINDFS_ARGS+=(--map="$MAP_OPT")
fi

bindfs "${BINDFS_ARGS[@]}" "$STAGING_DIR" "$MOUNT_POINT"
