import asyncio
import sys

from ._main import main


sys.exit(asyncio.run(main(sys.argv[1:])))
