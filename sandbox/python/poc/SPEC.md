We need a POC to verify wcpan.drive.core v5 is ready for release.

We usually use a script like ./bin/wdcli to use the drive.

Initialize an environment at this folder to test wcpan.drive.core v5.
It should install the following packages as local editable:
- ../wcpan.drive.cli
- ../wcpan.drive.core
- ../wcpan.drive.crypt
- ../wcpan.drive.google
- ../wcpan.drive.sqlite

You may simply use uvx, or initialize a uv project.
If you need to fix above packages, just edit them.

All packages are
- using uv to manage project
- use make test for testing
- use make format for format
- use make lint for lint
- does not use mypy and pytest
