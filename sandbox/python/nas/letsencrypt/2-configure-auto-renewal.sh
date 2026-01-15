#!/bin/bash
# Step 2: Configure automatic renewal after manual import
# Usage: ./2-configure-auto-renewal.sh example.com CKeLNv

set -e

if [ -z "$1" ] || [ -z "$2" ]; then
    echo "Usage: ./2-configure-auto-renewal.sh <domain> <cert-id>"
    echo ""
    echo "  <domain>   : Your domain name (e.g., example.com)"
    echo "  <cert-id>  : Certificate ID from Synology (e.g., CKeLNv or 'default')"
    echo ""
    echo "To find certificate ID:"
    echo "  ssh admin@your-nas"
    echo "  sudo ls -la /usr/syno/etc/certificate/_archive/"
    echo ""
    echo "Or use 'default' to update the default system certificate"
    exit 1
fi

DOMAIN=$1
CERT_ID=$2
SYNOLOGY_CERT_BASE="/usr/syno/etc/certificate"

echo "==================================="
echo "Configuring Automatic Renewal"
echo "==================================="
echo "Domain: $DOMAIN"
echo "Certificate ID: $CERT_ID"
echo ""

# Determine target directory
if [ "$CERT_ID" = "default" ]; then
    echo "Resolving default certificate ID..."
    DEFAULT_LINK=$(docker exec letsencrypt-acme readlink ${SYNOLOGY_CERT_BASE}/system/default 2>/dev/null || echo "")
    if [ -z "$DEFAULT_LINK" ]; then
        echo "Error: Could not resolve default certificate."
        echo "Please run on Synology host:"
        echo "  sudo ls -la ${SYNOLOGY_CERT_BASE}/system/default"
        exit 1
    fi
    RESOLVED_ID=$(basename $DEFAULT_LINK)
    TARGET_DIR="${SYNOLOGY_CERT_BASE}/_archive/${RESOLVED_ID}"
    echo "Default certificate ID: $RESOLVED_ID"
else
    TARGET_DIR="${SYNOLOGY_CERT_BASE}/_archive/${CERT_ID}"
    RESOLVED_ID=$CERT_ID
fi

# Check if target directory exists (in container)
if ! docker exec letsencrypt-acme test -d "$TARGET_DIR"; then
    echo "Error: Certificate directory not found in container: $TARGET_DIR"
    echo ""
    echo "Make sure docker-compose.yml has the Synology certificate folder mounted."
    echo "Add this to volumes:"
    echo "  - /usr/syno/etc/certificate:/usr/syno/etc/certificate"
    exit 1
fi

echo "Target directory: $TARGET_DIR"
echo ""
echo "Configuring acme.sh to:"
echo "  1. Auto-renew certificate every 60 days"
echo "  2. Install directly to Synology certificate folder"
echo "  3. Restart services after renewal"
echo ""

# Configure acme.sh --install-cert with direct installation to Synology
docker exec letsencrypt-acme --install-cert -d "$DOMAIN" \
    --key-file "${TARGET_DIR}/privkey.pem" \
    --fullchain-file "${TARGET_DIR}/fullchain.pem" \
    --cert-file "${TARGET_DIR}/cert.pem" \
    --ca-file "${TARGET_DIR}/chain.pem" \
    --reloadcmd "/synology-scripts/restart-services.sh"

echo ""
echo "==================================="
echo "Auto-Renewal Configured!"
echo "==================================="
echo ""
echo "Certificate will automatically:"
echo "  - Renew when within 60 days of expiry"
echo "  - Install to: $TARGET_DIR"
echo "  - Restart Synology services"
echo ""
echo "To test renewal manually:"
echo "  docker exec letsencrypt-acme --renew -d $DOMAIN --force"
echo ""
echo "To check renewal status:"
echo "  docker exec letsencrypt-acme --list"
echo ""
