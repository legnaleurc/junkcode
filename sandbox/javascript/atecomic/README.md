# comic server

## need few API

### GET /updates

Get latest updated comics. Returns a list of [comic object].

#### Optional Parameters

* offset
    * Start point of the list. Default is 0.
* length
    * The list length. Default is 30.

### GET /comics/{comic_id}

Get a [comic object] by the given ID.

### GET /comics/{comic_id}/episodes

Get a [episode list] by the given ID.

### GET /comics/{comic_id}/episodes/{episode_id}/pages

Get a [page list] by the given ID.

## need few tasks

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

## fields needed in each comic

* ID
* title
* cover image URL
* the latest update time-stamp
* the latest episode ID
* full episode list
* author
* publisher

## fields needed in each episode

* ID
* comic ID
* page image URL list

## tables

* refresh_queue
    * id
    * comic_id
    * url
    * dirty
