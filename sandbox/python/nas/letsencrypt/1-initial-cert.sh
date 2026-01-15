#!/bin/bash
# Step 1: Issue certificate for the first time
# Usage: ./1-initial-cert.sh example.com '*.example.com'

set -e

if [ -z "$1" ]; then
    echo "Usage: ./1-initial-cert.sh <domain> [additional-domains...]"
    echo ""
    echo "Example:"
    echo "  ./1-initial-cert.sh example.com"
    echo "  ./1-initial-cert.sh example.com '*.example.com'"
    exit 1
fi

DOMAIN=$1
shift
ADDITIONAL_DOMAINS="$@"

echo "==================================="
echo "Issuing Certificate - First Time"
echo "==================================="
echo "Primary domain: $DOMAIN"
echo "Additional domains: $ADDITIONAL_DOMAINS"
echo ""

# Build domain arguments
DOMAIN_ARGS="-d $DOMAIN"
for extra in $ADDITIONAL_DOMAINS; do
    DOMAIN_ARGS="$DOMAIN_ARGS -d $extra"
done

# Issue certificate using Cloudflare DNS
echo "Issuing certificate via Let's Encrypt (DNS-01 challenge)..."
docker exec letsencrypt-acme --issue \
    $DOMAIN_ARGS \
    --dns dns_cf \
    --force

# Install to /certs directory
echo ""
echo "Installing certificate to /certs directory..."
docker exec letsencrypt-acme --install-cert -d $DOMAIN \
    --key-file /certs/${DOMAIN}.key \
    --fullchain-file /certs/${DOMAIN}.crt

echo ""
echo "==================================="
echo "Certificate Issued Successfully!"
echo "==================================="
echo ""
echo "Certificate files saved to:"
echo "  - /volume1/docker/letsencrypt/certs/${DOMAIN}.key"
echo "  - /volume1/docker/letsencrypt/certs/${DOMAIN}.crt"
echo ""
echo "NEXT STEP:"
echo "1. Go to Synology: Control Panel > Security > Certificate"
echo "2. Click 'Add' > Import certificate"
echo "3. Upload the certificate files:"
echo "   - Private Key: /volume1/docker/letsencrypt/certs/${DOMAIN}.key"
echo "   - Certificate: /volume1/docker/letsencrypt/certs/${DOMAIN}.crt"
echo "4. After import, find the certificate ID:"
echo "   ssh admin@your-nas"
echo "   sudo ls -la /usr/syno/etc/certificate/_archive/"
echo "5. Run: ./2-configure-auto-renewal.sh $DOMAIN <CERT-ID>"
echo ""
