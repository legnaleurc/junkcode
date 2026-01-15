I have a Synology NAS, the OS is DSM 7.
I want to configure ssl cert for it.
I have domain name and setup nameserver on cloudflare.
Synology has built-in letsencrypt to renew ssl, but DNS-01 only allowed to use Synology DDNS.
If using custom domain, only HTTP-01 is supported.
But I don't want to expose my NAS, as it is behind NAT.
To use DNS-01, I suppose we need to run a docker service in Container Manager.

I want to setup the ssl cert by following step:

1. Run a script to generate ssl cert from letsencrypt.
2. then I manually import cert to synology certificate page, to create entry under /usr/syno/etc/certificate
3. start acme in daemon mode to automatically update cert, replace cert, restart services.

You can put api tokens or volume paths in env file.
There maybe many services which need to restart, before restarting check if it is running.
Container Manager run as root, so I suppose we don't need to chown, but maybe chmod is needed?
