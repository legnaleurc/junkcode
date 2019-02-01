import asyncio as aio
import sys

from .main import main

rv = main()
rv = aio.run(rv)
sys.exit(rv)
