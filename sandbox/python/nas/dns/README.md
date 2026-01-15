# DNS Server for Synology NAS - Split-Horizon DNS

This Docker Compose project sets up dnsmasq for local DNS resolution with split-horizon capability.

## Purpose

- At home: Resolves `nas.example.com` to local IP (192.168.0.x)
- On VPN: VPS DNS resolves `nas.example.com` to VPN IP (10.8.0.x)

## Setup Instructions

### 1. Edit Configuration

Edit `dnsmasq.conf` and replace the placeholder:
```bash
address=/nas.example.com/192.168.0.x
```

Replace:
- `nas.example.com` with your actual domain
- `192.168.0.x` with your NAS's actual local IP address

### 2. Deploy on Synology NAS

```bash
# Start the DNS server
docker-compose up -d

# Check if it's running
docker-compose ps

# View logs
docker-compose logs -f
```

### 3. Configure Your Router

Set the DHCP DNS server to your NAS IP address (192.168.0.x) so all home devices use this DNS server.

If your router doesn't support custom DHCP DNS settings, configure each device manually.

### 4. Test

From a device on your home network:
```bash
nslookup nas.example.com
# Should return: 192.168.0.x

nslookup google.com
# Should return: public Google IP (verifying upstream DNS works)
```

## Management Commands

```bash
# Start
docker-compose up -d

# Stop
docker-compose down

# Restart (after config changes)
docker-compose restart

# View logs
docker-compose logs -f dnsmasq

# Update image
docker-compose pull
docker-compose up -d
```

## Troubleshooting

### Port 53 already in use
If Synology's DNS Server package is installed, it may be using port 53. Either:
1. Stop/uninstall the DNS Server package
2. Change the port mapping in docker-compose.yml

### DNS not resolving
1. Check if container is running: `docker-compose ps`
2. Check logs: `docker-compose logs dnsmasq`
3. Verify firewall allows UDP/TCP port 53
4. Ensure devices are pointing to the correct DNS server

## VPS Setup (for remote access)

On your VPS, set up similar dnsmasq configuration:
```
address=/nas.example.com/10.8.0.x
```

And configure OpenVPN to push the VPS as DNS server to clients.
