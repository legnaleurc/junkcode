# NAS NFS Mount to VPS (Kernel-Space)

Mount a Synology NAS NFS export to a directory under the main operator's home on a VPS, using kernel NFS and bindfs for uid/gid mapping. A single systemd service handles mount on start and unmount (with cleanup) on stop.

## Prerequisites

- **NFS client:** `nfs-common` (Debian/Ubuntu) or equivalent (e.g. `nfs-utils` on RHEL/Fedora).
- **bindfs:** Install e.g. `bindfs` (Debian/Ubuntu) or `bindfs` from your distro.

```bash
# Debian/Ubuntu
sudo apt-get install nfs-common bindfs
```

## Configuration variables

Copy the environment file and edit as needed:

```bash
sudo cp etc/nas-nfs.env /etc/default/nas-nfs
sudo edit /etc/default/nas-nfs
```

| Variable | Description | Example |
|----------|-------------|---------|
| `NAS_HOST` | NAS hostname or IP (e.g. via OpenVPN) | `nas.wcpan.me` |
| `NFS_EXPORT` | NFS export path on NAS | `/volume1/download` |
| `MOUNT_POINT` | Final path under operator home | `/home/operator/nas-download` |
| `STAGING_DIR` | Staging path for NFS (e.g. under `/var/tmp`) | `/var/tmp/nas-nfs-staging` |
| `BINDFS_UID_MAP` | bindfs uid map (nas_uid:operator_uid) | `1026:1000` |
| `BINDFS_GID_MAP` | bindfs gid map (optional) | `100:100` |
| `MOUNT_UID` / `MOUNT_GID` | Owner of the final mount point | `1000` / `100` |
| `NFS_OPTS` | NFS mount options (must include `noresvport` for NAS that allow only ports ≥ 1024) | `noresvport,soft,tcp` |

## Install and enable

1. **Install the script** (must be executable):

   ```bash
   sudo cp scripts/nas-nfs-mount.sh /usr/local/bin/nas-nfs-mount.sh
   sudo chmod +x /usr/local/bin/nas-nfs-mount.sh
   ```

   If you use a different path, edit the systemd unit `ExecStart` and `ExecStop` accordingly.

2. **Install the systemd unit and env file:**

   ```bash
   sudo cp etc/nas-nfs.env /etc/default/nas-nfs
   sudo cp systemd/nas-nfs.service /etc/systemd/system/nas-nfs.service
   sudo systemctl daemon-reload
   ```

3. **Edit** `/etc/default/nas-nfs` and set at least `NAS_HOST`, `NFS_EXPORT`, `MOUNT_POINT`, `STAGING_DIR`, `BINDFS_UID_MAP`, `MOUNT_UID`, `MOUNT_GID`.

4. **Enable and start** (mount runs on start, unmount on stop):

   ```bash
   sudo systemctl enable nas-nfs.service
   sudo systemctl start nas-nfs.service   # mount
   # ...
   sudo systemctl stop nas-nfs.service   # unmount + cleanup
   ```

## Manual run

With the env file in place:

```bash
sudo /usr/local/bin/nas-nfs-mount.sh /etc/default/nas-nfs start
sudo /usr/local/bin/nas-nfs-mount.sh /etc/default/nas-nfs stop
```

Or set `NAS_NFS_ENV` and run:

```bash
sudo NAS_NFS_ENV=/etc/default/nas-nfs /usr/local/bin/nas-nfs-mount.sh start
sudo NAS_NFS_ENV=/etc/default/nas-nfs /usr/local/bin/nas-nfs-mount.sh stop
```

## Behaviour

- **Start:** Creates staging dir, mounts NFS to staging (with `noresvport,soft,tcp` by default), then bindfs from staging to the final mount point with uid/gid mapping. Idempotent: if already mounted, exits successfully.
- **Stop:** Unmounts bindfs at the final path, then NFS at staging, then removes the staging directory (cleanup). Best-effort: exits 0 even if already unmounted.
- The systemd unit uses `PrivateTmp=yes`, so the staging path is in a private `/var/tmp` that systemd removes when the service stops. The script also runs `rmdir` on the staging dir when not using PrivateTmp.

## NAS requirements

- NFS export with no squash, allowed ports ≥ 1024, sub-folders allowed.
- Mount option `noresvport` is required on the client (included in default `NFS_OPTS`).

## Troubleshooting

- **Mount hangs:** Ensure NAS is reachable (e.g. OpenVPN up) and NFS export is correct. The `soft` option avoids indefinite hang on server disconnect.
- **Permission denied:** Check `MOUNT_UID`/`MOUNT_GID` and bindfs maps match the operator and NAS uids/gids.
- **Staging dir left behind:** If not using `PrivateTmp=yes`, the script removes the staging dir on stop; if it is not empty (e.g. unmount failed), remove it manually after unmounting.
