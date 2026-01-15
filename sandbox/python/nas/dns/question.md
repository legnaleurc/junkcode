I have a Synology NAS at home, a VPS which has openvpn server. The NAS connects VPN to the VPS.
I want when I am at home, all devices in the LAN can solve a domain name (e.g. nas.example.com) to the NAS (e.g. 192.168.0.x which has been fixed by the router).
And when I am outside, devices connect to the vps through openvpn can also solve the domain (nas.example.com) to the NAS (e.g. 10.8.0.x which also fixed by openvpn server).

How should I setup dns for this?

On VPS, should I use bind9 or dnsmasq?

At home, my router cannot setup domain name record or run local DNS.
