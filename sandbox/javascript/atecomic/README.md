# comic server

need few tasks:

```python
def from_latest_update():
    for updated_comics in fetch_latest_updates():
        with WriteLock(global_comic_queue):
            for comic_id in updated_comics:
                global_comic_queue.push(comic_id)


def process_comic_queue():
    with WriteLock(global_comic_queue):
        comic_id = global_comic_queue.pop()
    comic_data = fetch_and_parse_comic_page(comic_id)
    with WriteLock(global_data_base):
        global_data_base.add(comic_data)
```

fields needed in each comic:

* ID
* title
* cover image URL
* the latest update time-stamp
* the latest episode ID
* full episode list
* author
* publisher

fields needed in each episode:

* ID
* comic ID
* page image URL list
