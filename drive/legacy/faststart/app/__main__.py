import asyncio
import sys

from .main import main

sys.exit(asyncio.run(main()))
