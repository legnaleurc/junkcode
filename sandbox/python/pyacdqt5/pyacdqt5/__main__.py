import sys

from . import shell


exit_code = shell.main()
sys.exit(exit_code)
