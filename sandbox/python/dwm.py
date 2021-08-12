#! /usr/bin/env python3

import asyncio
import datetime
import sys


async def main():
    while True:
        footprint = await get_dwm_footprint()
        now = datetime.datetime.now(datetime.timezone.utc).astimezone()
        print(now.strftime('%Y/%m/%d-%H:%M'), footprint)
        await asyncio.sleep(60 * 1000)
    return 0


async def get_dwm_footprint():
    p = await asyncio.create_subprocess_shell(
        'tasklist.exe | grep dwm',
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.PIPE,
    )
    out, err = await p.communicate()
    err = out.decode('utf-8')
    err = err.split()
    err = f'{err[4]}{err[5]}'
    return err


if __name__ == '__main__':
    sys.exit(asyncio.run(main()))
