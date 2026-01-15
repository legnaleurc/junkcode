# Automated Let's Encrypt SSL Certificate Management for Synology NAS

This solution automates Let's Encrypt SSL certificate management on Synology NAS (DSM 7) using acme.sh in a Docker container with Cloudflare DNS-01 challenge.

## Features

- ✅ Automated certificate renewal (daily checks)
- ✅ DNS-01 challenge (works behind NAT, no port 80/443 exposure needed)
- ✅ Cloudflare DNS API integration
- ✅ Automatic deployment to Synology certificate directories
- ✅ Intelligent service restart (only restarts running services)
- ✅ Zero manual intervention after initial setup
- ✅ Comprehensive logging

## Prerequisites

- Synology NAS running DSM 7
- Docker/Container Manager installed
- Domain name managed on Cloudflare
- Cloudflare API token with DNS permissions

## Quick Start

### 1. Cloudflare API Token Setup

1. Login to Cloudflare Dashboard
2. Go to Profile > API Tokens > Create Token
3. Create Custom Token with:
   - **Permissions**: Zone:DNS:Edit, Zone:Zone:Read
   - **Zone Resources**: Include your specific zone
4. Copy the token

### 2. Initial Configuration

1. Copy the example environment file:
   ```bash
   cp .env.example .env
   ```

2. Edit `.env` file with your details:
   ```bash
   CF_TOKEN=your_cloudflare_api_token_here
   CF_ACCOUNT_ID=your_cloudflare_account_id_here
   DOMAIN=example.com
   WILDCARD_DOMAIN=*.example.com
   EMAIL=your-email@example.com
   ```

3. Keep `SYNO_CERT_ID` empty for now (will set after manual import)

### 3. Start the Container

```bash
docker-compose up -d
```

Verify it's running:
```bash
docker ps
```

### 4. Generate Initial Certificate

Run the initial certificate generation script (this is the ONLY script you need to run manually):

```bash
docker exec acme-letsencrypt /scripts/initial-cert.sh
```

This will:
- Validate your Cloudflare API access
- Issue a certificate via DNS-01 challenge
- Display certificate location and next steps

### 5. Manual Certificate Import

1. Login to DSM: Control Panel > Security > Certificate
2. Click "Add" > "Add a new certificate"
3. Select "Import certificate"
4. Upload the certificate files from the container:
   - Private Key: `{domain}.key`
   - Certificate: `{domain}.cer`
   - Intermediate Certificate: `ca.cer`
5. Set as "Default certificate"

### 6. Configure Auto-Renewal

1. Find your certificate ID:
   ```bash
   cat /usr/syno/etc/certificate/_archive/DEFAULT
   ```

2. Update `.env` file with the certificate ID:
   ```bash
   SYNO_CERT_ID=aBc123  # Replace with your actual ID
   ```

3. Restart the container:
   ```bash
   docker-compose restart
   ```

### 7. Done!

The container will now automatically:
- Check for certificate renewal daily
- Renew certificates 30 days before expiry
- Deploy renewed certificates to Synology
- Restart services that use the certificate

No further manual intervention needed!

## Monitoring & Maintenance

### Check Certificate Status

```bash
docker exec acme-letsencrypt acme.sh --list
```

### View Container Logs

```bash
docker logs -f acme-letsencrypt
```

### View acme.sh Logs

```bash
tail -f ./logs/acme.log
```

### Force Certificate Renewal (Testing)

```bash
docker exec acme-letsencrypt acme.sh --renew -d example.com --force
```

### Test Service Restart (Dry Run)

```bash
docker exec acme-letsencrypt /scripts/restart-services.sh --dry-run
```

## File Structure

```
/volume1/docker/letsencrypt/
├── .env.example                  # Environment configuration template
├── .env                          # Your actual configuration (create from .env.example)
├── .gitignore                    # Git ignore file (protects secrets)
├── docker-compose.yml            # Container definition
├── scripts/
│   # USER-FACING SCRIPTS (run manually)
│   ├── initial-cert.sh          # First-run certificate generation
│
│   # AUTOMATIC SCRIPTS (run by container)
│   ├── startup.sh               # Container entrypoint (auto-config)
│   ├── deploy-to-synology.sh    # Deploy hook (called by acme.sh)
│
│   # HELPER SCRIPTS (used by other scripts)
│   ├── restart-services.sh      # Service restart logic
│   └── utils.sh                 # Shared utility functions
├── logs/
│   └── acme.log                 # Centralized logging
└── acme-data/                   # Certificate storage (volume mount)
```

## Troubleshooting

### Certificate Not Generating

1. Check Cloudflare API access:
   ```bash
   docker exec acme-letsencrypt acme.sh --test-dns dns_cf -d example.com
   ```

2. Verify CF_TOKEN permissions in Cloudflare Dashboard

3. Check logs for errors:
   ```bash
   tail -f ./logs/acme.log
   ```

### Certificate Not Deploying

1. Verify SYNO_CERT_ID is set correctly:
   ```bash
   cat /usr/syno/etc/certificate/_archive/DEFAULT
   ```

2. Check that the certificate directory exists:
   ```bash
   ls -la /usr/syno/etc/certificate/_archive/$SYNO_CERT_ID/
   ```

3. Manually test deploy hook:
   ```bash
   docker exec acme-letsencrypt /scripts/deploy-to-synology.sh
   ```

### Services Not Restarting

1. Check which services are running:
   ```bash
   systemctl status nginx
   ```

2. Test service restart script:
   ```bash
   docker exec acme-letsencrypt /scripts/restart-services.sh --dry-run
   ```

3. Manually restart nginx:
   ```bash
   systemctl reload nginx
   ```

## Security Notes

- **.env file**: Contains sensitive API tokens
  - Create from `.env.example` (never commit `.env` to git)
  - Protected by `.gitignore` to prevent accidental commits
  - Set to chmod 600 automatically
- **Private keys**: Automatically set to chmod 600 during deployment
- **Cloudflare API Token**: Limited to DNS permissions only, no account-wide access
- **Certificate backups**: Created before each deployment (last 5 kept)
- **.gitignore**: Protects secrets, logs, and certificate data from being committed

## Advanced Configuration

### Custom Renewal Check Interval

Edit `.env`:
```bash
RENEW_CHECK_DAYS=20  # Renew if less than 20 days until expiry
```

### Use 4096-bit Keys

Edit `.env`:
```bash
CERT_KEY_LENGTH=4096
```

### Add Additional Services

Edit `scripts/restart-services.sh` and add to the service arrays:
```bash
OPTIONAL_SERVICES=(
    "your-custom-service"
)
```

## Support

For issues or questions:
1. Check logs: `tail -f ./logs/acme.log`
2. Check Docker logs: `docker logs acme-letsencrypt`
3. Review the plan file for detailed implementation info

## License

This setup script is provided as-is for use with your Synology NAS.
