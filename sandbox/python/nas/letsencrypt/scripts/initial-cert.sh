#!/bin/sh
# Initial certificate generation script
# This is the ONLY script users need to run manually (once)

set -e  # Exit on error

# Source utility functions
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
. "$SCRIPT_DIR/utils.sh"

log "info" "========================================="
log "info" "Let's Encrypt Initial Certificate Setup"
log "info" "========================================="
echo ""

# Validate required environment variables
log "info" "Step 1: Validating environment configuration..."
if ! validate_env CF_Token DOMAIN EMAIL; then
    log "error" "Please configure the following in your .env file:"
    log "error" "  - CF_Token: Cloudflare API token"
    log "error" "  - DOMAIN: Your primary domain"
    log "error" "  - EMAIL: Email for Let's Encrypt notifications"
    exit 1
fi

log "info" "Configuration valid:"
log "info" "  Domain: $DOMAIN"
log "info" "  Wildcard: ${WILDCARD_DOMAIN:-not set}"
log "info" "  Email: $EMAIL"
log "info" "  Key Length: ${CERT_KEY_LENGTH:-2048}"
echo ""

# Test Cloudflare API access
log "info" "Step 2: Testing Cloudflare API access..."
if acme.sh --test-dns dns_cf -d "$DOMAIN" 2>&1 | grep -q "Success"; then
    log "info" "Cloudflare API access verified"
else
    log "warn" "Could not verify Cloudflare API access"
    log "warn" "Proceeding anyway, but you may encounter issues"
fi
echo ""

# Check if certificate already exists
log "info" "Step 3: Checking for existing certificates..."
if acme.sh --list | grep -q "$DOMAIN"; then
    log "warn" "Certificate already exists for $DOMAIN"
    log "warn" "If you want to regenerate, first remove it with:"
    log "warn" "  acme.sh --remove -d $DOMAIN"
    echo ""
    printf "Do you want to continue and regenerate the certificate? (y/N): "
    read -r REPLY
    echo ""
    case "$REPLY" in
        [Yy]|[Yy][Ee][Ss]) ;;
        *)
            log "info" "Certificate generation cancelled"
            exit 0
            ;;
    esac
    log "info" "Removing existing certificate..."
    acme.sh --remove -d "$DOMAIN"
fi
echo ""

# Issue certificate
log "info" "Step 4: Issuing certificate from Let's Encrypt..."
log "info" "This may take 2-5 minutes depending on DNS propagation..."
echo ""

ISSUE_CMD="acme.sh --issue --dns dns_cf -d \"$DOMAIN\""

# Add wildcard domain if specified
if [ -n "$WILDCARD_DOMAIN" ]; then
    ISSUE_CMD="$ISSUE_CMD -d \"$WILDCARD_DOMAIN\""
fi

# Add key length and log file
ISSUE_CMD="$ISSUE_CMD --keylength ${CERT_KEY_LENGTH:-2048} --log \"${LOG_FILE:-/logs/acme.log}\""

log "info" "Running: $ISSUE_CMD"
echo ""

# Execute the command
eval $ISSUE_CMD

if [ $? -eq 0 ]; then
    log "info" "✓ Certificate issued successfully!"
else
    log "error" "✗ Certificate issuance failed"
    log "error" "Check logs at: ${LOG_FILE:-/logs/acme.log}"
    exit 1
fi
echo ""

# Display certificate location
CERT_DIR="/acme.sh/$DOMAIN"
log "info" "========================================="
log "info" "Certificate Location:"
log "info" "========================================="
log "info" "Certificate directory: $CERT_DIR"
log "info" ""
log "info" "Files:"
log "info" "  - Private Key: $CERT_DIR/${DOMAIN}.key"
log "info" "  - Certificate: $CERT_DIR/${DOMAIN}.cer"
log "info" "  - CA Chain: $CERT_DIR/ca.cer"
log "info" "  - Full Chain: $CERT_DIR/fullchain.cer"
echo ""

# Provide manual import instructions
log "info" "========================================="
log "info" "NEXT STEPS - Manual Import Required"
log "info" "========================================="
echo ""
log "info" "1. Login to Synology DSM"
log "info" "   Go to: Control Panel > Security > Certificate"
echo ""
log "info" "2. Click 'Add' > 'Add a new certificate'"
echo ""
log "info" "3. Select 'Import certificate' and upload:"
log "info" "   - Private Key: ${DOMAIN}.key"
log "info" "   - Certificate: ${DOMAIN}.cer"
log "info" "   - Intermediate Certificate: ca.cer"
echo ""
log "info" "4. Set as 'Default certificate'"
echo ""
log "info" "5. Find your certificate ID by running this command on your NAS:"
log "info" "   cat /usr/syno/etc/certificate/_archive/DEFAULT"
echo ""
log "info" "6. Update your .env file with the certificate ID:"
log "info" "   SYNO_CERT_ID=<value-from-step-5>"
echo ""
log "info" "7. Restart the container:"
log "info" "   docker-compose restart"
echo ""
log "info" "8. After restart, the container will automatically:"
log "info" "   - Register the deploy hook"
log "info" "   - Start daemon mode for auto-renewal"
log "info" "   - Handle certificate deployment on renewal"
echo ""
log "info" "========================================="
log "info" "Setup Complete!"
log "info" "========================================="
log "info" "Certificate files are ready for import."
log "info" "Follow the steps above to complete the setup."
echo ""
