# Sync Implementation Analysis

## Summary

**Answer: NO, the sync() method does NOT use Synology Drive's index database.**

Instead, it performs a **full recursive file tree scan** via Synology's File Station API on every sync call because Synology doesn't provide a changelog/delta API.

## Architecture

### Sync Flow

```
FileService.sync()
  ↓
get_changes() [_service.py:110-143]
  ↓
listing.list_all_files() [Full recursive scan via API]
  ↓
SYNO.FileStation.List API calls (paginated, limit=1000)
  ↓
Recursively descend into every folder
  ↓
Convert to change actions (all marked as "updates")
  ↓
SnapshotService.apply_changes() [Write to local SQLite cache]
  ↓
Yield changes with incremented cursor
```

### Key Components

#### 1. FileService (_service.py)
- `get_changes(cursor)` - Main sync method
- Returns all files as "updates" since no true changelog exists
- Cursor is just a simple counter (starts at "0", increments by 1)

#### 2. API Layer (_api/listing.py)
- `list_all_files()` - Recursive directory traversal
- Makes paginated API calls to `SYNO.FileStation.List`
- Pagination: 1000 items per request
- For each folder found, recursively calls itself

#### 3. Local SQLite Database (_snapshot_service.py, _db.py, _sql.py)
- **Purpose**: Local cache for offline access and queries
- **NOT used for sync operations** - only stores results
- Uses ProcessPoolExecutor to avoid Python GIL
- Schema:
  - `nodes` table: file/folder metadata (id, parent_id, name, timestamps, size, hash)
  - `metadata` table: cursor, root_id
- Indexed by: parent_id, name, is_trashed, is_directory

## Performance Implications

### Critical Issues

| Issue | Impact | Severity |
|-------|--------|----------|
| **Full scan every sync** | Must traverse entire file tree on every call | 🔴 HIGH |
| **No delta/changelog API** | Cannot detect only changed files | 🔴 HIGH |
| **1000 item pagination** | Large folders = many API calls | 🟡 MEDIUM |
| **Recursive traversal required** | Deep folder trees are expensive | 🟡 MEDIUM |
| **All files marked as "updates"** | No differentiation of actual changes | 🟡 MEDIUM |
| **Cannot resume interrupted syncs** | Cursor is just a counter, not a timestamp | 🟡 MEDIUM |

### Performance Estimates

For a Synology Drive with:
- **10,000 files** across 500 folders
- Average depth: 4 levels

Expected sync operations:
- **500+ API calls** (one per folder, paginated if >1000 items)
- **Minutes of scanning time** for initial sync
- **Same time for every subsequent sync** (no optimization)

### Comparison to True Changelog APIs

| Feature | Synology Implementation | Ideal Changelog API |
|---------|------------------------|---------------------|
| Detects changes | Full scan | Delta only |
| API calls per sync | O(folders + files/1000) | O(changes) |
| Scales with | Total files | Changed files only |
| Resume support | No | Yes (cursor = timestamp) |
| First sync time | Slow | Slow |
| Subsequent syncs | **Still slow** | **Fast** |

## API Details

### Endpoint Used
```
GET /webapi/entry.cgi
```

### Parameters
```
api=SYNO.FileStation.List
version=2
method=list
folder_path=<path>
limit=1000
offset=<offset>
additional=["time","size","owner","real_path","type"]
```

### Authentication
- Uses username/password login via `SYNO.API.Auth`
- Session ID stored and reused
- Auto-retry on 401 (re-authenticate)

## Code References

- **Sync entry point**: `_service.py:110-143` - `get_changes()`
- **Recursive scanning**: `_api/listing.py:60-132` - `list_all_files()`
- **API client**: `_network.py` - HTTP client with retry logic
- **Local cache**: `_snapshot_service.py` - SQLite operations
- **Database schema**: `_sql.py` - Table definitions and queries
- **Authentication**: `_auth.py` - Login/logout operations

## Optimization Opportunities

### Current State
❌ Full recursive scan on every sync
❌ No change detection optimization
❌ All files treated as updates

### Potential Improvements

1. **Client-side change detection**:
   - Compare fetched files against local SQLite cache
   - Only yield actual changes (new, modified, deleted files)
   - Requires comparing: modified_time, size, hash

2. **Incremental scanning with timestamps**:
   - Store last_sync timestamp
   - Filter files client-side by modified_time > last_sync
   - Still requires full scan, but can skip unchanged files earlier

3. **Directory-level caching**:
   - Hash entire directory contents
   - Skip scanning directories with unchanged hash
   - Requires storing directory-level metadata

4. **Parallel API calls**:
   - Use asyncio.gather() to scan multiple folders concurrently
   - Currently sequential folder-by-folder

### Limitation
Synology File Station API fundamentally doesn't support:
- Changelog/delta queries
- "Modified since timestamp" filtering
- Server-side change tracking

Any optimization must work around these API limitations by doing client-side filtering.

## Conclusion

**The sync() method does NOT rely on Synology Drive's index database for performance.**

Instead, it performs a complete file system traversal via API calls on every sync. The local SQLite database is only a cache for storing results, not for optimizing the sync process itself.

For large file systems (>10k files), this approach will be **slow and resource-intensive**. The performance depends entirely on:
1. Synology's API response time
2. Network latency
3. Total number of files and folders
4. Folder depth

There is no way to make this faster without Synology adding a proper changelog API to their File Station service.
