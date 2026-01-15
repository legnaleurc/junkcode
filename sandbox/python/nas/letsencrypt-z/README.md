# Let's Encrypt SSL Certificate with DNS-01 Challenge (Cloudflare)

This project provides automated SSL certificate generation and renewal using acme.sh with Cloudflare DNS-01 challenge for Synology NAS.

## Features

- Automatic SSL certificate issuance via Let's Encrypt
- DNS-01 challenge using Cloudflare API (works behind firewall, no port 80/443 needed)
- Built-in automatic renewal (no Synology Task Scheduler needed)
- Runs as a persistent Docker service
- **Automatic import to Synology Certificate Manager** (optional)
- Helper scripts for easy certificate management

## Prerequisites

1. Domain name managed by Cloudflare
2. Cloudflare API Token with DNS edit permissions
3. Docker and Docker Compose installed on Synology NAS

## Setup

### 1. Get Cloudflare API Token

**Option A: API Token (Recommended)**
1. Go to https://dash.cloudflare.com/profile/api-tokens
2. Click "Create Token"
3. Use "Edit zone DNS" template or create custom token with:
   - Permissions: `Zone > DNS > Edit`
   - Zone Resources: `Include > Specific zone > your-domain.com`
4. Copy the token

To find your Account ID and Zone ID:
- Go to your domain overview in Cloudflare dashboard
- Scroll down to see Account ID and Zone ID in the right sidebar

**Option B: Global API Key (Simpler but less secure)**
1. Go to https://dash.cloudflare.com/profile/api-tokens
2. View "Global API Key"
3. Use your Cloudflare email and this key

### 2. Configure Environment

```bash
cd letsencrypt
cp .env.example .env
nano .env
```

Edit the `.env` file with your credentials and paths:
- Set your Cloudflare API token/credentials
- Adjust `CERT_PATH` and `OUTPUT_PATH` to match your Synology volume structure
  - Default: `/volume1/docker/letsencrypt/...`
  - Adjust if using a different volume (e.g., `/volume2/...`)

### 3. Create Required Directories

SSH into your Synology and create the directories:

```bash
sudo mkdir -p /volume1/docker/letsencrypt/acme
sudo mkdir -p /volume1/docker/letsencrypt/certs
```

### 4. Start the Service

```bash
docker-compose up -d
```

### 5. Issue Your First Certificate

**Easy way (recommended):** Use the helper script:

```bash
./issue-cert.sh yourdomain.com '*.yourdomain.com'
```

**Manual way:** Connect to the running container:

```bash
# Issue the certificate
docker exec letsencrypt-acme --issue \
  --dns dns_cf \
  -d yourdomain.com \
  -d '*.yourdomain.com'

# Install to output directory
docker exec letsencrypt-acme --install-cert \
  -d yourdomain.com \
  --key-file /certs/yourdomain.com.key \
  --fullchain-file /certs/yourdomain.com.crt \
  --reloadcmd "echo 'Certificate renewed'"
```

## Scheduling & Renewal

### Do I need Synology Task Scheduler?

**No!** The acme.sh container runs in `daemon` mode, which:
- Automatically checks certificates daily
- Renews certificates when they're within 60 days of expiry
- Runs as a persistent service (restart: unless-stopped)

You do NOT need to set up any tasks in Synology's Task Scheduler. The container handles everything automatically.

## Certificate Location & Permissions

### Where are the SSL certificates?

Certificates are stored in two locations:

1. **CERT_PATH** (`/volume1/docker/letsencrypt/acme`): Internal acme.sh data
   - Contains account info, certificate history, and metadata
   - Don't manually edit files here

2. **OUTPUT_PATH** (`/volume1/docker/letsencrypt/certs`): Your actual certificates
   - `yourdomain.com.crt` - Full certificate chain
   - `yourdomain.com.key` - Private key
   - Point your services (Nginx, Apache, etc.) to these files

### Permission Issues?

Yes, Docker runs as root by default, which can cause permission issues on Synology.

**Solutions:**

**Option 1: Use sudo when accessing certificates**
```bash
sudo cp /volume1/docker/letsencrypt/certs/*.crt /your/service/location/
```

**Option 2: Change ownership after renewal**
Add a reload command when installing the certificate:
```bash
docker exec letsencrypt-acme --install-cert \
  -d yourdomain.com \
  --key-file /certs/yourdomain.com.key \
  --fullchain-file /certs/yourdomain.com.crt \
  --reloadcmd "chown 1000:1000 /certs/*.* && echo 'Permissions updated'"
```

**Option 3: Use ACL permissions on Synology**
```bash
sudo chmod -R 644 /volume1/docker/letsencrypt/certs/*.crt
sudo chmod -R 600 /volume1/docker/letsencrypt/certs/*.key
```

**Option 4: Synology Certificate Import**
If using certificates for Synology's built-in services (DSM, reverse proxy), see the section below for automatic import.

## Synology Certificate Manager Integration

Want to use your Let's Encrypt certificates with DSM, Synology Photos, or other built-in services? You can automatically import them to Synology's Certificate Manager.

### Quick Start

Issue and automatically deploy to Synology:

```bash
./issue-cert.sh --synology-cert-id=default yourdomain.com '*.yourdomain.com'
```

This will:
1. Issue the certificate via Let's Encrypt
2. Install it to `/certs/`
3. Deploy it to Synology's Certificate Manager
4. Reload nginx and services

### Find Your Certificate ID

To use automatic deployment, you need to know your certificate ID:

```bash
# List all certificate IDs
sudo ls -la /usr/syno/etc/certificate/_archive/

# Find the default certificate ID
sudo ls -la /usr/syno/etc/certificate/system/default
```

You'll see IDs like `CKeLNv`, `AbCd12`, etc. Use `default` to automatically find and update the default certificate.

### Manual Deployment

You can also manually deploy certificates after issuance:

```bash
sudo ./deploy-to-synology.sh yourdomain.com default
```

### Full Documentation

For complete instructions on Synology integration, including:
- Automatic deployment on renewal
- Multiple deployment methods
- Troubleshooting
- Security best practices

See **[SYNOLOGY-INTEGRATION.md](SYNOLOGY-INTEGRATION.md)**

## Usage

### Helper Scripts

The project includes helper scripts to make certificate management easier:

**Issue a new certificate:**
```bash
./issue-cert.sh yourdomain.com '*.yourdomain.com'
./issue-cert.sh --synology-cert-id=default yourdomain.com  # With Synology auto-deploy
```

**Manage certificates:**
```bash
./manage-certs.sh list               # List all certificates
./manage-certs.sh info example.com   # Show certificate info
./manage-certs.sh renew example.com  # Force renewal
./manage-certs.sh logs               # View logs
./manage-certs.sh shell              # Open container shell
```

### Manual Commands

**Check certificate status:**
```bash
docker exec letsencrypt-acme --list
```

**Force renewal:**
```bash
docker exec letsencrypt-acme --renew -d yourdomain.com --force
```

**View logs:**
```bash
docker logs letsencrypt-acme
docker logs -f letsencrypt-acme  # Follow logs
```

**Container management:**
```bash
docker-compose restart  # Restart
docker-compose down     # Stop and remove
docker-compose up -d    # Start
```

## Troubleshooting

### DNS propagation issues
If you get DNS validation errors:
- Wait a few minutes for DNS changes to propagate
- Verify your Cloudflare API token has correct permissions
- Check that the domain is active on Cloudflare (orange cloud or DNS only)

### Permission denied errors
```bash
sudo chown -R root:root /volume1/docker/letsencrypt/acme
sudo chmod -R 755 /volume1/docker/letsencrypt/acme
```

### Container keeps restarting
Check logs:
```bash
docker logs letsencrypt-acme
```

Common issues:
- Invalid Cloudflare credentials
- Network connectivity problems
- Insufficient disk space

## Certificate Renewal Flow

1. Container runs in daemon mode 24/7
2. Daily check: acme.sh checks all certificates
3. If certificate expires in < 60 days: automatic renewal
4. Renewal process:
   - Creates DNS TXT record via Cloudflare API
   - Waits for Let's Encrypt validation
   - Downloads new certificate
   - Runs reload command (if configured)
   - Removes DNS TXT record

## Security Notes

- API tokens are stored in environment variables (not in code)
- Use `.env` file (not committed to git) for secrets
- Restrict Cloudflare API token to minimum required permissions
- Keep private keys secure (600 permissions)
- Regularly update the acme.sh Docker image

## Using Certificates with Other Services

### Nginx Reverse Proxy
```nginx
ssl_certificate /volume1/docker/letsencrypt/certs/yourdomain.com.crt;
ssl_certificate_key /volume1/docker/letsencrypt/certs/yourdomain.com.key;
```

### Synology Reverse Proxy
1. Control Panel > Security > Certificate
2. Import certificate
3. Select the .crt and .key files from OUTPUT_PATH

### Docker services
Mount the OUTPUT_PATH in other containers:
```yaml
volumes:
  - /volume1/docker/letsencrypt/certs:/certs:ro
```

## Additional Resources

- [acme.sh Documentation](https://github.com/acmesh-official/acme.sh)
- [Cloudflare API Tokens](https://developers.cloudflare.com/api/tokens/create)
- [Let's Encrypt Documentation](https://letsencrypt.org/docs/)
