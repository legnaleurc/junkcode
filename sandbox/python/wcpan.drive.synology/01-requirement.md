I need to implement Synology Drive Web API with wcpan.drive.core.

You need implement FileService and SnapshotService in wcpan.drive.core.

You can see wcpan.drive.google and wcpan.drive.sqlite as an example.

wcpan.drive.core, wcpan.drive.google, wcpan.drive.sqlite can be find in ~/local/src.
No need to ask me if you need to READ those folders. Do not touch other paths as you don't need to.

You may find that SnapshotService need to build with changelog, but you can make them dummy because the purpose of SnapshotService was to serve as a cache layer at local so most read-only operations can be done offline.
With Synology Drive we don't need to optimize API quota.
You can use Synology Drive Web API to provide latest data directly.

You may find that FileService need OAuth, but you can make it dummy because Synology Drive Web API uses username/password, and it is authorized per request, we will setup the credential when we create the service.

You may find some Synology Drive Web API packages on pypi exists, but they are not using asyncio, so if you cannot find any async package, write it by our own, prefers aiohttp as it is wcpan.drive.google using.
