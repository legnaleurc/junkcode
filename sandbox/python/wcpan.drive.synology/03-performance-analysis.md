# Performance Analysis: Is Synology's Index Fast Enough?

## Corrected Understanding

**Yes, Synology's File Station API almost certainly uses an index database on the server side.** Each individual API call is likely fast because it queries an indexed database rather than doing a live filesystem scan.

## The Real Performance Bottleneck

The performance issue isn't the **speed of each API call**, but rather the **number of API calls required**.

### API Call Pattern Analysis

Looking at `wcpan/drive/synology/_api/listing.py:118-122`:

```python
# If it's a folder, recursively list its contents
if converted["is_folder"]:
    file_path = file_info.get("path", "")
    async for sub_files in list_all_files(network, file_path, limit):
        yield sub_files
```

**Key observation**: The code makes **one API call per folder** (plus pagination for large folders).

### Performance Calculation

For a file system with:
- **500 folders**
- **10,000 total files**
- Average 20 files per folder

#### Scenario A: Index-backed API (LIKELY)
- Each API call: ~50ms (fast database query)
- Network round-trip latency: ~20ms
- **Total per call: ~70ms**

**Total sync time:**
- 500 folders × 70ms = **35 seconds**
- Plus ~10 paginated calls for large folders = **36 seconds total**

#### Scenario B: Filesystem scan API (UNLIKELY)
- Each API call: ~200ms (filesystem I/O)
- Network latency: ~20ms
- **Total per call: ~220ms**

**Total sync time:**
- 500 folders × 220ms = **110 seconds (nearly 2 minutes)**

## Conclusion

**Synology's index is probably fast enough** for each individual query, but:

1. **The architecture requires many sequential API calls** (one per folder)
2. **Cumulative network latency** becomes significant (500 × 20ms = 10 seconds)
3. **Even with a fast index, 500 API calls take time** (500 × 50ms = 25 seconds)

### Performance Table

| File System Size | Folders | API Calls | Estimated Sync Time (with index) |
|------------------|---------|-----------|----------------------------------|
| Small (1K files) | 50 | ~50 | 3-5 seconds |
| Medium (10K files) | 500 | ~500 | 30-40 seconds |
| Large (100K files) | 2000 | ~2000 | 2-3 minutes |
| Very Large (500K files) | 5000 | ~5000 | 5-7 minutes |

## The Fundamental Limitation

Even if Synology's index database is **instantaneous**, the sync is still limited by:

1. **Network latency**: Cannot be eliminated (20-50ms per call)
2. **Serial API calls**: Current code is sequential (one folder at a time)
3. **API call overhead**: HTTP request/response parsing

### What Would Make It Fast

A **true changelog API** that returns only changes:
```
GET /api/changes?since=2026-01-20T10:00:00Z
→ Returns only files modified after timestamp
→ Single API call, regardless of total file count
```

With a changelog API:
- First sync: Still slow (must return everything)
- Subsequent syncs: **Fast** (only returns changed files)
  - 10 changed files = 1 API call returning 10 items = **<1 second**

## Practical Answer

**For your use case:**

✅ **Synology's index is fast enough** for moderate file systems (<10K files)
- Expected sync time: 30-60 seconds
- Each individual API call is quick

⚠️ **May become slow** for large file systems (>50K files)
- Expected sync time: Several minutes
- Not because the index is slow, but because of the number of API calls

🔴 **Fundamentally inefficient** compared to changelog-based APIs
- Every sync takes the same time (no optimization for unchanged files)
- Scales with total file count, not change count

## Optimization Opportunities

### 1. Parallel API Calls (Recommended)
Currently, folder scanning is serial. Making parallel API calls would significantly reduce total time:

```python
# Current: Serial (70ms × 500 folders = 35 seconds)
for folder in folders:
    await list_folder(folder)  # One at a time

# Optimized: Parallel (70ms for all 500 folders = ~1-2 seconds)
await asyncio.gather(*[list_folder(f) for f in folders])
```

**Potential speedup: 10-30x** for large folder trees

### 2. Client-Side Change Detection
After fetching all files, compare against local SQLite cache:
- Filter out files with same modified_time and size
- Only yield actual changes
- Reduces downstream processing

**Benefit**: Faster sync application, less data transfer to consumers

### 3. Smart Caching
Cache folder listings with TTL:
- If folder's mtime hasn't changed, reuse cached listing
- Requires checking folder mtime first (1 API call vs many)

**Potential speedup: 5-10x** for mostly static file trees

## Code Reference

**Current bottleneck**: `listing.py:118-122` - Sequential folder recursion

```python
# This happens serially for every folder:
async for sub_files in list_all_files(network, file_path, limit):
    yield sub_files
```

**Impact**: With 500 folders, this creates a 500-deep chain of sequential API calls.
