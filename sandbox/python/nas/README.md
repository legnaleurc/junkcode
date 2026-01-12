# NAS Docker Compose Project

Docker Compose setup for running `wdcli` on Synology NAS Container Manager.

## Features

- Python 3.14 (slim-trixie) base image
- uv package manager installed
- libmediainfo0v5 system library for pymediainfo
- wdcli command available in container
- Three volume mounts for config, data, and storage

## Setup

1. Copy the environment file and configure your paths:
   ```bash
   cp .env.example .env
   ```

2. Edit `.env` to set your actual Synology paths:
   - `CONFIG_PATH`: Location of your config files (read-only)
   - `DATA_PATH`: Location of your data files (read-write)
   - `STORAGE_PATH`: Location of your storage files (read-write)
   - `TZ`: Your timezone (optional)

3. Build the image:
   ```bash
   docker-compose build
   ```

## Usage

Execute commands on-demand:
```bash
docker-compose run --rm shell wdcli --help
```

Or enter the shell:
```bash
docker-compose run --rm shell bash
```

The container will stop automatically after the command completes.

## Volume Mounts

- `/mnt/config` - Configuration files (read-only)
- `/mnt/data` - Data files (read-write)
- `/mnt/storage` - Storage files (read-write)

## Notes

- The wdcli script uses `uvx` to run wcpan.drive.cli with dependencies
- Config file is expected at `/mnt/config/cli.yaml`
- Container runs with `tail -f /dev/null` to stay alive
