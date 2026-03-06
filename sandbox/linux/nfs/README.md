# NFS Mount Scripts for WSL2

Mount/unmount a Synology NAS NFS share on WSL2 using bindfs for uid/gid remapping.

## Dependencies

```bash
sudo apt install nfs-common bindfs
```

## Usage

```bash
cp scripts/example scripts/myshare
# Edit NAS_HOST, NFS_EXPORT, MOUNT_POINT, NAS_UID, NAS_GID, WSL_UID, WSL_GID

sudo bash scripts/mount.sh myshare
sudo bash scripts/umount.sh myshare
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
