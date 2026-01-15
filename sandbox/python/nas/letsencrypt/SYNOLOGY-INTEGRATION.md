# Synology Certificate Manager Integration

This guide explains how to automatically import Let's Encrypt certificates into Synology's Certificate Manager for use with DSM and other Synology services.

## Overview

Synology stores certificates in `/usr/syno/etc/certificate/` with this structure:

```
/usr/syno/etc/certificate/
├── _archive/
│   ├── CKeLNv/          # Each certificate has a unique ID
│   │   ├── cert.pem     # Server certificate
│   │   ├── chain.pem    # CA chain
│   │   ├── fullchain.pem # Full chain
│   │   ├── privkey.pem  # Private key
│   │   └── info         # Certificate metadata
│   └── AbCd12/          # Another certificate
│       └── ...
├── system/
│   └── default -> ../_archive/CKeLNv/  # Symlink to default cert
└── [service-name]/      # Service-specific certificates
    └── -> ../_archive/CKeLNv/
```

## Methods for Auto-Import

There are three approaches to automatically import certificates:

### Method 1: Manual Import with Script (Recommended for Initial Setup)

Use the provided `deploy-to-synology.sh` script to manually deploy certificates after issuance.

**Step 1: Find your certificate ID**

```bash
ssh admin@your-nas.local
sudo ls -la /usr/syno/etc/certificate/_archive/
```

You'll see directories with IDs like `CKeLNv`, `AbCd12`, etc.

To find which one is the default:
```bash
sudo ls -la /usr/syno/etc/certificate/system/default
```

**Step 2: Issue certificate with auto-deploy**

```bash
./issue-cert.sh --synology-cert-id=default yourdomain.com '*.yourdomain.com'
```

Or for a specific certificate ID:
```bash
./issue-cert.sh --synology-cert-id=CKeLNv yourdomain.com '*.yourdomain.com'
```

The script will automatically:
1. Issue the certificate via Let's Encrypt
2. Install it to the output directory
3. Deploy it to Synology's certificate directory
4. Reload nginx and other services

### Method 2: Automatic Deployment on Renewal

For fully automatic deployment on renewal, you need to configure acme.sh's reload command.

**Step 1: Create a renewal script**

Create `/volume1/docker/letsencrypt/renew-and-deploy.sh`:

```bash
#!/bin/bash
DOMAIN="$1"
CERT_ID="$2"

# This script runs on the Synology host
# It's triggered by acme.sh after successful renewal

if [ -z "$DOMAIN" ] || [ -z "$CERT_ID" ]; then
    echo "Usage: renew-and-deploy.sh <domain> <cert-id>"
    exit 1
fi

# Deploy to Synology
/volume1/docker/letsencrypt/letsencrypt/deploy-to-synology.sh "$DOMAIN" "$CERT_ID"
```

Make it executable:
```bash
chmod +x /volume1/docker/letsencrypt/renew-and-deploy.sh
```

**Step 2: Configure acme.sh with the reload command**

When issuing the certificate, set the reload command:

```bash
docker exec letsencrypt-acme --install-cert \
  -d yourdomain.com \
  --key-file /certs/yourdomain.com.key \
  --fullchain-file /certs/yourdomain.com.crt \
  --reloadcmd "/bin/sh -c 'ssh root@172.17.0.1 /volume1/docker/letsencrypt/renew-and-deploy.sh yourdomain.com default'"
```

**Note:** This requires SSH access from container to host. See "SSH Setup" section below.

### Method 3: Scheduled Task (Alternative)

If you prefer not to use SSH, you can create a Synology Task Scheduler job to periodically check for new certificates.

**Step 1: Create check script**

Create `/volume1/docker/letsencrypt/check-and-deploy.sh`:

```bash
#!/bin/bash
DOMAIN="yourdomain.com"
CERT_ID="default"
SOURCE_CERT="/volume1/docker/letsencrypt/certs/${DOMAIN}.crt"
TARGET_DIR="/usr/syno/etc/certificate/_archive"

# Find the target certificate directory
if [ "$CERT_ID" = "default" ]; then
    CERT_DIR=$(readlink /usr/syno/etc/certificate/system/default)
    TARGET_CERT="${CERT_DIR}/fullchain.pem"
else
    TARGET_CERT="${TARGET_DIR}/${CERT_ID}/fullchain.pem"
fi

# Check if source is newer than target
if [ "$SOURCE_CERT" -nt "$TARGET_CERT" ]; then
    echo "New certificate detected, deploying..."
    /volume1/docker/letsencrypt/letsencrypt/deploy-to-synology.sh "$DOMAIN" "$CERT_ID"
else
    echo "Certificate is up to date"
fi
```

**Step 2: Create Task Scheduler job**

1. Go to Control Panel > Task Scheduler
2. Create > Scheduled Task > User-defined script
3. General:
   - Task: Deploy Let's Encrypt Certificate
   - User: root
4. Schedule: Daily at 3:00 AM (or your preferred time)
5. Task Settings:
   - Run command: `/bin/bash /volume1/docker/letsencrypt/check-and-deploy.sh`
6. Save and enable the task

## SSH Setup (for Method 2)

To allow the container to trigger deployment on the host:

**Step 1: Enable SSH on Synology**

1. Control Panel > Terminal & SNMP
2. Enable SSH service
3. Note the port (default: 22)

**Step 2: Setup SSH keys**

```bash
# Generate SSH key in container
docker exec -it letsencrypt-acme sh
ssh-keygen -t ed25519 -f /acme.sh/.ssh/id_ed25519 -N ""
cat /acme.sh/.ssh/id_ed25519.pub
exit

# Copy the public key and add to Synology
ssh admin@your-nas.local
sudo mkdir -p /root/.ssh
sudo nano /root/.ssh/authorized_keys
# Paste the public key
sudo chmod 700 /root/.ssh
sudo chmod 600 /root/.ssh/authorized_keys
```

**Step 3: Test SSH access**

```bash
docker exec letsencrypt-acme ssh root@172.17.0.1 "echo 'SSH works'"
```

## Using Certificates with Synology Services

After deploying certificates to Synology's Certificate Manager, you can assign them to services:

### DSM (Web Interface)

1. Control Panel > Security > Certificate
2. Click "Configure"
3. Select your service (e.g., "System Default" for DSM)
4. Choose your newly imported certificate
5. Click "OK"

### Reverse Proxy

1. Control Panel > Application Portal > Reverse Proxy
2. Edit your reverse proxy rule
3. Under HTTPS settings, select your certificate
4. Save

### Other Services

The certificate can be used with:
- Synology Drive
- Photo Station / Synology Photos
- Download Station
- Mail Server
- VPN Server
- Any other service that supports HTTPS

## Troubleshooting

### "Permission denied" errors

Make sure you're running the deploy script with sudo:
```bash
sudo ./deploy-to-synology.sh yourdomain.com default
```

### Certificate not showing in Certificate Manager

1. Check that the certificate was copied correctly:
   ```bash
   sudo ls -la /usr/syno/etc/certificate/_archive/[YOUR_CERT_ID]/
   ```

2. Verify file permissions:
   ```bash
   sudo chmod 600 /usr/syno/etc/certificate/_archive/[YOUR_CERT_ID]/*.pem
   ```

3. Reload nginx:
   ```bash
   sudo synosystemctl reload nginx
   ```

### Services not using new certificate

After deploying, you may need to restart services:
```bash
sudo synosystemctl restart nginx
sudo synosystemctl restart nmbd    # For SMB
sudo synosystemctl restart avahi   # For Bonjour
```

### Certificate validation fails

Make sure the certificate chain is complete:
```bash
openssl verify -CAfile /volume1/docker/letsencrypt/certs/yourdomain.com.crt \
  /volume1/docker/letsencrypt/certs/yourdomain.com.crt
```

## Best Practices

1. **Use default certificate ID** for automatic assignment to new services
2. **Test manually first** before setting up automatic renewal
3. **Backup existing certificates** before replacing (script does this automatically)
4. **Monitor renewal logs** to ensure automatic deployment works:
   ```bash
   docker logs letsencrypt-acme | grep -A 10 "Renew"
   ```
5. **Keep SSH keys secure** if using SSH-based deployment
6. **Document your certificate ID** in your deployment notes

## Example: Complete Automatic Setup

Here's a complete example for fully automatic certificate management:

```bash
# 1. Issue initial certificate with Synology deployment
./issue-cert.sh --synology-cert-id=default example.com '*.example.com'

# 2. Configure automatic renewal with deployment
docker exec letsencrypt-acme --install-cert \
  -d example.com \
  --key-file /certs/example.com.key \
  --fullchain-file /certs/example.com.crt \
  --reloadcmd "/volume1/docker/letsencrypt/renew-and-deploy.sh example.com default"

# 3. Verify in Synology
# Go to Control Panel > Security > Certificate

# 4. Monitor logs
docker logs -f letsencrypt-acme
```

Now your certificate will:
- Automatically renew when it's within 60 days of expiry
- Automatically deploy to Synology Certificate Manager
- Automatically reload nginx and other services

All without any manual intervention!

## Security Considerations

1. **Protect private keys**: Certificate private keys have 600 permissions
2. **Limit SSH access**: If using SSH method, use key-based auth only
3. **Regular backups**: Backup `/usr/syno/etc/certificate/_archive/`
4. **Monitor access**: Check who has access to certificate directories
5. **Use API tokens**: Never use Cloudflare Global API Key; use scoped tokens

## References

- [Synology Certificate Structure](https://kb.synology.com/en-us/DSM/tutorial/How_to_import_or_export_certificate_chain)
- [acme.sh Documentation](https://github.com/acmesh-official/acme.sh)
- [Let's Encrypt Best Practices](https://letsencrypt.org/docs/integration-guide/)
