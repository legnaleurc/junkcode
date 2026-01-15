#!/bin/sh
# Container startup script
# Auto-configures deploy hook and starts daemon mode
# This is the container entrypoint

set -e  # Exit on error

# Source utility functions
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
. "$SCRIPT_DIR/utils.sh"

log "info" "========================================="
log "info" "acme.sh Container Startup"
log "info" "========================================="
echo ""

# Check if DOMAIN is set
if [ -z "$DOMAIN" ]; then
    log "error" "DOMAIN is not set in environment"
    log "error" "Please configure DOMAIN in .env file"
    exit 1
fi

log "info" "Domain: $DOMAIN"
log "info" "Wildcard: ${WILDCARD_DOMAIN:-not set}"
echo ""

# Check if certificate exists in acme.sh
log "info" "Step 1: Checking for existing certificate..."
if acme.sh --list 2>/dev/null | grep -q "$DOMAIN"; then
    log "info" "✓ Certificate found for $DOMAIN"
    CERT_EXISTS=true
else
    log "warn" "✗ Certificate not found for $DOMAIN"
    log "warn" ""
    log "warn" "Initial setup required!"
    log "warn" "Run the initial certificate generation script:"
    log "warn" "  docker exec acme /scripts/initial-cert.sh"
    log "warn" ""
    log "warn" "Container will stay alive in idle mode."
    echo ""

    # Keep container alive but idle
    log "info" "Container is ready. Waiting for certificate generation..."
    exec tail -f /dev/null
    exit 0
fi
echo ""

# Check if SYNO_CERT_ID is set
log "info" "Step 2: Checking Synology certificate configuration..."
if [ -z "$SYNO_CERT_ID" ]; then
    log "warn" "✗ SYNO_CERT_ID is not set in environment"
    log "warn" ""
    log "warn" "This is normal if you haven't completed the initial setup yet."
    log "warn" "After generating and manually importing the certificate:"
    log "warn" "  1. Find your certificate ID:"
    log "warn" "     cat /usr/syno/etc/certificate/_archive/DEFAULT"
    log "warn" "  2. Update .env file with SYNO_CERT_ID=<value>"
    log "warn" "  3. Restart container: docker-compose restart"
    log "warn" ""
    log "warn" "Skipping auto-configuration."
    log "warn" ""
    log "warn" "Container will stay alive in idle mode."
    log "warn" "Run the initial certificate generation script:"
    log "warn" "  docker exec acme /scripts/initial-cert.sh"
    echo ""

    # Keep container alive but idle
    log "info" "Container is ready. Waiting for initial setup..."
    exec tail -f /dev/null
    exit 0
fi

log "info" "✓ SYNO_CERT_ID is set: $SYNO_CERT_ID"

# Verify certificate directory exists
SYNO_CERT_ARCHIVE="${SYNO_CERT_ARCHIVE:-/usr/syno/etc/certificate/_archive}"
CERT_DIR="$SYNO_CERT_ARCHIVE/$SYNO_CERT_ID"

if [ -d "$CERT_DIR" ]; then
    log "info" "✓ Certificate directory exists: $CERT_DIR"
else
    log "warn" "✗ Certificate directory not found: $CERT_DIR"
    log "warn" "Please verify SYNO_CERT_ID is correct"
fi
echo ""

# Check if deploy hook is already registered
log "info" "Step 3: Checking deploy hook registration..."
if [ "$CERT_EXISTS" == true ]; then
    CERT_INFO=$(acme.sh --info -d "$DOMAIN" 2>/dev/null || true)

    if echo "$CERT_INFO" | grep -q "Le_DeployHook"; then
        CURRENT_HOOK=$(echo "$CERT_INFO" | grep "Le_DeployHook" | cut -d"'" -f2)
        log "info" "✓ Deploy hook already registered: $CURRENT_HOOK"

        # Check if it's our hook
        if [ "$CURRENT_HOOK" == "/scripts/deploy-to-synology.sh" ]; then
            log "info" "✓ Using correct deploy hook"
        else
            log "warn" "✗ Deploy hook is different, updating..."
            log "info" "Registering deploy hook: /scripts/deploy-to-synology.sh"
            acme.sh --deploy -d "$DOMAIN" --deploy-hook "/scripts/deploy-to-synology.sh"
            if [ $? -eq 0 ]; then
                log "info" "✓ Deploy hook registered successfully"
            else
                log "error" "✗ Failed to register deploy hook"
            fi
        fi
    else
        log "warn" "✗ Deploy hook not registered"
        log "info" "Registering deploy hook: /scripts/deploy-to-synology.sh"

        # Register deploy hook
        acme.sh --deploy -d "$DOMAIN" --deploy-hook "/scripts/deploy-to-synology.sh"

        if [ $? -eq 0 ]; then
            log "info" "✓ Deploy hook registered successfully"
        else
            log "error" "✗ Failed to register deploy hook"
            log "error" "Certificate renewal will not deploy automatically"
        fi
    fi
else
    log "warn" "Certificate doesn't exist yet, skipping deploy hook registration"
fi
echo ""

# Display certificate info if it exists
if [ "$CERT_EXISTS" == true ]; then
    log "info" "Step 4: Certificate information..."

    # Get certificate details
    CERT_FILE="/acme.sh/$DOMAIN/$DOMAIN.cer"
    if [ -f "$CERT_FILE" ]; then
        EXPIRY_DATE=$(get_cert_expiry "$CERT_FILE")
        if [ -n "$EXPIRY_DATE" ]; then
            log "info" "Certificate expires: $EXPIRY_DATE"

            # Check if renewal is needed
            if check_cert_needs_renewal "$CERT_FILE" 2>/dev/null; then
                log "warn" "Certificate is due for renewal soon"
            fi
        fi
    fi
    echo ""
fi

# Start daemon mode
log "info" "========================================="
log "info" "Starting acme.sh Daemon Mode"
log "info" "========================================="
log "info" "Daemon will check for renewal daily"
log "info" "Certificates will be renewed 30 days before expiry"
log "info" ""
log "info" "To check certificate status:"
log "info" "  docker exec acme acme.sh --list"
log "info" ""
log "info" "To force renewal:"
log "info" "  docker exec acme acme.sh --renew -d $DOMAIN --force"
log "info" ""
log "info" "To view logs:"
log "info" "  docker logs -f acme"
log "info" "  tail -f ./logs/acme.log"
echo ""

# Execute daemon mode
log "info" "Starting daemon..."
exec /entry.sh daemon
