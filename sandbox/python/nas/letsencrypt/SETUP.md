# Simple Setup Guide

This guide walks through the simplified 3-step workflow for automatic Let's Encrypt certificates on Synology.

## Overview

1. **First time**: Manually issue certificate
2. **Manual import**: Import to Synology to create certificate ID
3. **Configure**: Set up automatic renewal and service restart

## Prerequisites

- Docker and Docker Compose installed on Synology
- Cloudflare DNS for your domain
- Cloudflare API Token with DNS edit permissions
- SSH access to your Synology NAS

## Step 0: Initial Setup

### 0.1 Configure Environment

```bash
cd /volume1/docker/letsencrypt
cp .env.example .env
nano .env
```

Set your Cloudflare credentials:
```env
CF_TOKEN=your_cloudflare_api_token
CF_ACCOUNT_ID=your_account_id
CF_ZONE_ID=your_zone_id
```

### 0.2 Start acme.sh Container

```bash
docker-compose up -d
```

Verify it's running:
```bash
docker ps | grep letsencrypt
```

### 0.3 Setup SSH (for automatic service restart)

**On Synology:**
```bash
# Enable SSH if not already enabled
# Control Panel > Terminal & SNMP > Enable SSH service

# Make restart script executable
chmod +x /volume1/docker/letsencrypt/restart-services.sh
```

**In Container:**
```bash
# Generate SSH key
docker exec -it letsencrypt-acme sh
mkdir -p /acme.sh/.ssh
ssh-keygen -t ed25519 -f /acme.sh/.ssh/id_ed25519 -N ""
cat /acme.sh/.ssh/id_ed25519.pub
exit
```

**Copy the public key, then on Synology:**
```bash
# Add public key to root's authorized_keys
sudo mkdir -p /root/.ssh
sudo nano /root/.ssh/authorized_keys
# Paste the public key, save and exit

sudo chmod 700 /root/.ssh
sudo chmod 600 /root/.ssh/authorized_keys
```

**Test SSH connection:**
```bash
docker exec letsencrypt-acme ssh root@172.17.0.1 "echo SSH works"
```

If you see "SSH works", you're ready!

## Step 1: Issue Certificate (First Time)

Run the first-time certificate issuance:

```bash
cd /volume1/docker/letsencrypt
chmod +x 1-initial-cert.sh
./1-initial-cert.sh example.com '*.example.com'
```

This will:
- Issue certificate via Let's Encrypt DNS-01 challenge
- Save certificate files to `/volume1/docker/letsencrypt/certs/`

**Output files:**
- `example.com.key` - Private key
- `example.com.crt` - Full certificate chain

## Step 2: Manual Import to Synology

### 2.1 Import Certificate

1. Open DSM: **Control Panel > Security > Certificate**
2. Click **Add** > **Import certificate**
3. Fill in the form:
   - **Private Key**: Browse and select `example.com.key`
   - **Certificate**: Browse and select `example.com.crt`
   - **Description**: "Let's Encrypt - example.com"
4. Click **OK**

### 2.2 Find Certificate ID

SSH to your Synology and find the certificate ID:

```bash
ssh admin@your-nas.local
sudo ls -la /usr/syno/etc/certificate/_archive/
```

You'll see directories with IDs like:
```
drwxr-xr-x 2 root root 4096 Jan 15 10:00 CKeLNv
drwxr-xr-x 2 root root 4096 Jan 14 08:30 AbCd12
```

The newest one (check timestamps) is your certificate. Note the ID (e.g., `CKeLNv`).

**To find the default certificate:**
```bash
sudo ls -la /usr/syno/etc/certificate/system/default
```

This shows which certificate is set as default.

## Step 3: Configure Auto-Renewal

Now configure automatic renewal to update Synology directly:

```bash
cd /volume1/docker/letsencrypt
chmod +x 2-configure-auto-renewal.sh

# Using specific certificate ID
./2-configure-auto-renewal.sh example.com CKeLNv

# OR using 'default' for the default system certificate
./2-configure-auto-renewal.sh example.com default
```

This configures acme.sh to:
- **Automatically renew** when certificate is within 60 days of expiry
- **Install directly** to Synology certificate folder
- **Restart services** automatically after renewal

## Done!

Your certificate will now:
- ✅ Automatically renew before expiry
- ✅ Update Synology Certificate Manager
- ✅ Restart all services (nginx, SMB, SSH, etc.)
- ✅ All without manual intervention

## Verification

### Check Certificate Status

```bash
docker exec letsencrypt-acme --list
```

### Test Manual Renewal

Force a renewal to test everything works:

```bash
docker exec letsencrypt-acme --renew -d example.com --force
```

Watch the logs:
```bash
docker logs -f letsencrypt-acme
```

### Check Renewal Log

```bash
cat /volume1/docker/letsencrypt/renewal.log
```

### Verify Certificate in Synology

1. **Control Panel > Security > Certificate**
2. Check the expiry date - should be ~90 days from now
3. Click **Configure** to assign to services if needed

## Troubleshooting

### "Permission denied" when writing to certificate folder

Make sure the Synology certificate folder is mounted in docker-compose.yml:
```yaml
volumes:
  - /usr/syno/etc/certificate:/usr/syno/etc/certificate
```

Restart container:
```bash
docker-compose down && docker-compose up -d
```

### SSH connection fails

1. Verify SSH is enabled on Synology
2. Check firewall allows SSH from Docker network (172.17.0.1)
3. Verify public key is in `/root/.ssh/authorized_keys`
4. Test: `docker exec letsencrypt-acme ssh root@172.17.0.1 "ls"`

### Certificate not updating in Synology UI

1. Check certificate ID is correct:
   ```bash
   sudo ls -la /usr/syno/etc/certificate/_archive/YOUR_CERT_ID/
   ```
2. Verify files were updated (check timestamps)
3. Manually reload nginx:
   ```bash
   sudo synosystemctl reload nginx
   ```

### Services not using new certificate

Some services need restart instead of reload:
```bash
sudo synosystemctl restart smbd nmbd
sudo synosystemctl restart sshd
```

## Maintenance

### View Renewal Logs

```bash
# Container logs
docker logs letsencrypt-acme

# Service restart logs
cat /volume1/docker/letsencrypt/renewal.log
```

### Force Renewal

```bash
docker exec letsencrypt-acme --renew -d example.com --force
```

### Add More Domains

To add additional domains to the same certificate:

```bash
# Issue new certificate with additional domains
./1-initial-cert.sh example.com '*.example.com' 'subdomain.example.com'

# Import manually to Synology (will update existing certificate)

# No need to reconfigure - auto-renewal is already set up
```

### Update Certificate Assignment

After renewal, you can assign the certificate to specific services:

1. **Control Panel > Security > Certificate**
2. Click **Configure**
3. Select services and assign certificate
4. Click **OK**

## Alternative: Without SSH (Scheduled Task Method)

If you prefer not to use SSH, you can use a Synology scheduled task to check for cert changes:

**Create `/volume1/docker/letsencrypt/check-and-restart.sh`:**

```bash
#!/bin/bash
CERT_FILE="/usr/syno/etc/certificate/_archive/CKeLNv/fullchain.pem"
TIMESTAMP_FILE="/volume1/docker/letsencrypt/.last-restart"

if [ "$CERT_FILE" -nt "$TIMESTAMP_FILE" ]; then
    echo "$(date): New certificate detected" >> /volume1/docker/letsencrypt/renewal.log
    /volume1/docker/letsencrypt/restart-services.sh
    touch "$TIMESTAMP_FILE"
fi
```

**Create Synology Scheduled Task:**
1. Control Panel > Task Scheduler
2. Create > Scheduled Task > User-defined script
3. General: Task name "Check Certificate Updates", User: root
4. Schedule: Run every 1 hour
5. Task Settings: `/bin/bash /volume1/docker/letsencrypt/check-and-restart.sh`

This checks every hour if the certificate was updated and restarts services.

## Security Notes

- Private keys stored with 600 permissions
- SSH key auth only (no password)
- Container runs in bridge network (isolated)
- Cloudflare API token should have minimal permissions (only DNS edit)
- Regular security updates: `docker pull neilpang/acme.sh:latest && docker-compose up -d`
