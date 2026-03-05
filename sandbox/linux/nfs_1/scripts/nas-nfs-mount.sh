#!/usr/bin/env bash
# NAS NFS mount script: start = mount (NFS then bindfs), stop = unmount (bindfs then NFS) + cleanup.
# Requires: environment file with NAS_HOST, NFS_EXPORT, MOUNT_POINT, STAGING_DIR, BINDFS_UID_MAP, etc.
# Usage: nas-nfs-mount.sh start | stop
# Called by systemd with ExecStart=/path/to/nas-nfs-mount.sh start, ExecStop=/path/to/nas-nfs-mount.sh stop.

set -e

# Default env file (override with NAS_NFS_ENV or pass as first arg before start/stop)
NAS_NFS_ENV="${NAS_NFS_ENV:-/etc/default/nas-nfs}"

# If first argument looks like a path to env file, use it and shift
if [[ -f "${1:-}" && "${1:-}" != start && "${1:-}" != stop ]]; then
  NAS_NFS_ENV="$1"
  shift
fi

CMD="${1:-}"

if [[ "$CMD" != start && "$CMD" != stop ]]; then
  echo "Usage: $0 [path/to/nas-nfs.env] start|stop" >&2
  exit 1
fi

# Load env file if present (systemd may already set variables via EnvironmentFile=)
if [[ -f "$NAS_NFS_ENV" ]]; then
  # shellcheck source=/dev/null
  source "$NAS_NFS_ENV"
fi

# Require essential variables
for v in NAS_HOST NFS_EXPORT MOUNT_POINT STAGING_DIR BINDFS_UID_MAP MOUNT_UID MOUNT_GID; do
  if [[ -z "${!v:-}" ]]; then
    echo "Missing required variable: $v" >&2
    exit 1
  fi
done

NFS_OPTS="${NFS_OPTS:-noresvport,soft,tcp}"

# Check if a path is a mount point
is_mounted() {
  mountpoint -q "$1" 2>/dev/null
}

do_start() {
  if is_mounted "$MOUNT_POINT"; then
    echo "Already mounted: $MOUNT_POINT"
    return 0
  fi

  mkdir -p "$STAGING_DIR"
  if ! is_mounted "$STAGING_DIR"; then
    echo "Mounting NFS $NAS_HOST:$NFS_EXPORT to $STAGING_DIR"
    mount -t nfs -o "$NFS_OPTS" "$NAS_HOST:$NFS_EXPORT" "$STAGING_DIR"
  fi

  mkdir -p "$MOUNT_POINT"
  chown "$MOUNT_UID:$MOUNT_GID" "$MOUNT_POINT"

  if ! is_mounted "$MOUNT_POINT"; then
    BINDFS_ARGS=(--map="$BINDFS_UID_MAP" "$STAGING_DIR" "$MOUNT_POINT")
    if [[ -n "${BINDFS_GID_MAP:-}" ]]; then
      # bindfs group map: @gid_src/@gid_dst
      GID_FROM="${BINDFS_GID_MAP%%:*}"
      GID_TO="${BINDFS_GID_MAP##*:}"
      BINDFS_ARGS=(--map="$BINDFS_UID_MAP" --map="@${GID_FROM}/@${GID_TO}" "$STAGING_DIR" "$MOUNT_POINT")
    fi
    echo "Mounting bindfs to $MOUNT_POINT"
    bindfs "${BINDFS_ARGS[@]}"
  fi

  echo "Mount complete: $MOUNT_POINT"
}

do_stop() {
  # Unmount bindfs first, then NFS; then cleanup staging dir (best effort)
  if is_mounted "$MOUNT_POINT"; then
    echo "Unmounting bindfs: $MOUNT_POINT"
    umount "$MOUNT_POINT" || true
  fi

  if is_mounted "$STAGING_DIR"; then
    echo "Unmounting NFS: $STAGING_DIR"
    umount "$STAGING_DIR" || true
  fi

  if [[ -d "$STAGING_DIR" ]]; then
    rmdir "$STAGING_DIR" 2>/dev/null || true
  fi

  echo "Unmount complete"
}

case "$CMD" in
  start) do_start ;;
  stop)  do_stop ;;
  *)     echo "Usage: $0 [path/to/nas-nfs.env] start|stop" >&2; exit 1 ;;
esac
