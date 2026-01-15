#!/bin/sh
# Auto-deploy hook script for acme.sh
# This runs inside the container after certificate renewal
# It copies certificates and triggers Synology reload

DOMAIN="$1"

if [ -z "$DOMAIN" ]; then
    echo "Error: Domain not specified"
    exit 1
fi

echo "Auto-deploy hook triggered for domain: $DOMAIN"

# Copy certificates to output directory (already done by --install-cert)
# But we can add additional logic here if needed

# If the trigger file is mounted, touch it to signal the host
if [ -f "/trigger/deploy-needed" ]; then
    echo "$DOMAIN $(date +%s)" >> /trigger/deploy-needed
    echo "Deployment trigger file updated"
fi

# Set appropriate permissions on certificate files
if [ -d "/certs" ]; then
    chmod 644 /certs/*.crt 2>/dev/null || true
    chmod 600 /certs/*.key 2>/dev/null || true
    echo "Certificate permissions updated"
fi

echo "Auto-deploy hook completed"
