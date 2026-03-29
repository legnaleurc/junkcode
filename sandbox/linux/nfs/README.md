# NFS Mount Scripts for WSL2

Mount/unmount a Synology NAS NFS share on WSL2 using bindfs for uid/gid remapping.

## Dependencies

```bash
sudo apt install nfs-common bindfs
```

## Usage

```bash
# Install the script
cp scripts/nfs ~/.local/bin/nfs
chmod +x ~/.local/bin/nfs

# Create a profile
cp scripts/example ~/.config/nfs/myshare
# Edit NAS_HOST, NFS_EXPORT, MOUNT_POINT, NAS_UID, NAS_GID, WSL_UID, WSL_GID

nfs start myshare
nfs stop myshare
```

## Systemd Integration

A template unit `scripts/nfs@.service` is provided for automatic mounting at login.

### Setup

```bash
# Allow the script to run with sudo without a password.
# SETENV is required so --preserve-env=HOME works, keeping profile
# lookup in the user's config dir rather than /root/.config/nfs/.
sudo visudo -f /etc/sudoers.d/nfs-mount
# Add: <user> ALL=(root) NOPASSWD,SETENV: /home/<user>/.local/bin/nfs start *
# Add: <user> ALL=(root) NOPASSWD,SETENV: /home/<user>/.local/bin/nfs stop *

# Install the unit
mkdir -p ~/.config/systemd/user
cp scripts/nfs@.service ~/.config/systemd/user/
systemctl --user daemon-reload
```

### Profile selection

Enable the profiles you want to start at login:

```bash
systemctl --user enable nfs@myshare.service
systemctl --user start nfs@myshare.service
```

### Dependencies

Use drop-in files to configure ordering between profiles or other services.

```bash
mkdir -p ~/.config/systemd/user/nfs@myshare.service.d
```

**Depend on another profile:**

```ini
# ~/.config/systemd/user/nfs@gallery.service.d/after-drive.conf
[Unit]
After=nfs@drive.service
Wants=nfs@drive.service
```

**Add a startup delay (e.g. for flaky network):**

```ini
# ~/.config/systemd/user/nfs@myshare.service.d/delay.conf
[Service]
ExecStartPre=/bin/sleep 10
```

## Profile Variables

| Variable | Description |
|---|---|
| `NAS_HOST` | NAS hostname or IP |
| `NFS_EXPORT` | NFS export path on NAS |
| `MOUNT_POINT` | Final mount point (created/removed by scripts) |
| `NAS_UID` | UID that owns files on the NAS |
| `NAS_GID` | GID that owns files on the NAS |
| `WSL_UID` | Local WSL2 UID to map to |
| `WSL_GID` | Local WSL2 GID to map to |
| `NFS_OPTS` | NFS mount options (default: `nfsvers=4.1,noresvport,soft,tcp`) |

## Notes

- `noresvport` is required for WSL2
- NFS server: NFSv4.1, `sec=sys`, no squash — UIDs pass through numerically
- `uid=`/`gid=` are not valid Linux NFS mount options; bindfs handles remapping
- Staging dir is auto-derived as `/tmp/nfs-staging-<profile_name>`
