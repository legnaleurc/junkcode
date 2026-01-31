# wcpan.drive.synology

Synology Drive Web API integration for wcpan.drive framework.

## Features

- Async file operations (upload, download, move, delete)
- Session-based authentication with Synology DSM
- SQLite-based local cache for offline operations
- Media metadata support (images, videos)
- Automatic retry and rate limiting

## Installation

```bash
pip install wcpan-drive-synology
```

## Usage

```python
from wcpan.drive.synology import create_file_service, create_snapshot_service

# Create file service (remote API)
async with create_file_service(
    username="your_username",
    password="your_password",
    base_url="https://your-nas-url:5001",
) as file_service:
    root = await file_service.get_root()
    print(f"Root: {root}")

# Create snapshot service (local cache)
async with create_snapshot_service("./cache.db") as snapshot:
    root = await snapshot.get_root()
    print(f"Cached root: {root}")
```

## Requirements

- Python >= 3.12
- aiohttp >= 3.13.0
- wcpan-drive-core >= 4.1.0

## Architecture Highlights

### GIL-Free Database Operations
Following the `wcpan.drive.sqlite` pattern, this package uses **ProcessPoolExecutor** (not ThreadPoolExecutor) for all SQLite operations. This completely avoids Python's GIL and provides true parallelism for database operations.

```python
# Database operations run in separate processes
async with create_snapshot_service("./cache.db") as snapshot:
    # This runs in a separate process - no GIL contention!
    root = await snapshot.get_root()
```

### Other Features

- **Session-based authentication**: Username/password login with optional 2FA
- **Streaming downloads**: Range request support for efficient seeking
- **Buffered uploads**: Memory-efficient file uploads
- **Automatic retry**: Exponential backoff for rate limiting and transient errors
- **Type-safe**: Full type hints with `py.typed` marker

## License

MIT
