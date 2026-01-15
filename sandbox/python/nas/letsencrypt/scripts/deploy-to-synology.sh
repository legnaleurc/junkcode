#!/bin/bash
# Custom deploy hook for Synology NAS
# Called automatically by acme.sh after certificate renewal

set -e  # Exit on error

# Source utility functions
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/utils.sh"

log "info" "========================================="
log "info" "Deploying Certificate to Synology NAS"
log "info" "========================================="
echo ""

# Validate SYNO_CERT_ID is set
if [ -z "$SYNO_CERT_ID" ]; then
    log "error" "SYNO_CERT_ID is not set in environment"
    log "error" "Please update .env file with your certificate ID"
    log "error" "Find it by running: cat /usr/syno/etc/certificate/_archive/DEFAULT"
    exit 1
fi

# Validate required environment variables from acme.sh
if ! validate_env CERT_PATH CERT_KEY_PATH CA_CERT_PATH CERT_FULLCHAIN_PATH; then
    log "error" "Missing certificate path variables from acme.sh"
    log "error" "This script should be called by acme.sh deploy hook"
    exit 1
fi

log "info" "Certificate source paths:"
log "info" "  Private Key: $CERT_KEY_PATH"
log "info" "  Certificate: $CERT_PATH"
log "info" "  CA Chain: $CA_CERT_PATH"
log "info" "  Full Chain: $CERT_FULLCHAIN_PATH"
echo ""

# Determine target directories
SYNO_CERT_ARCHIVE="${SYNO_CERT_ARCHIVE:-/usr/syno/etc/certificate/_archive}"
SYNO_CERT_DEFAULT="${SYNO_CERT_DEFAULT:-/usr/syno/etc/certificate/system/default}"

TARGET_DIR="$SYNO_CERT_ARCHIVE/$SYNO_CERT_ID"
log "info" "Target directory: $TARGET_DIR"
echo ""

# Verify target directory exists
if [ ! -d "$TARGET_DIR" ]; then
    log "error" "Target directory does not exist: $TARGET_DIR"
    log "error" "Please verify SYNO_CERT_ID is correct: $SYNO_CERT_ID"
    exit 1
fi

# Create backup
log "info" "Step 1: Creating backup of existing certificates..."
if backup_cert "$TARGET_DIR"; then
    log "info" "✓ Backup created successfully"
else
    log "warn" "✗ Backup failed, continuing anyway"
fi
echo ""

# Copy certificates
log "info" "Step 2: Copying renewed certificates..."

# Private key
log "info" "Copying private key..."
cp "$CERT_KEY_PATH" "$TARGET_DIR/privkey.pem"
chmod 600 "$TARGET_DIR/privkey.pem"
log "debug" "  privkey.pem (chmod 600)"

# Certificate
log "info" "Copying certificate..."
cp "$CERT_PATH" "$TARGET_DIR/cert.pem"
chmod 644 "$TARGET_DIR/cert.pem"
log "debug" "  cert.pem (chmod 644)"

# Full chain
log "info" "Copying full chain..."
cp "$CERT_FULLCHAIN_PATH" "$TARGET_DIR/fullchain.pem"
chmod 644 "$TARGET_DIR/fullchain.pem"
log "debug" "  fullchain.pem (chmod 644)"

# CA chain
log "info" "Copying CA chain..."
cp "$CA_CERT_PATH" "$TARGET_DIR/chain.pem"
chmod 644 "$TARGET_DIR/chain.pem"
log "debug" "  chain.pem (chmod 644)"

log "info" "✓ Certificates copied successfully"
echo ""

# Also update system default location
log "info" "Step 3: Updating system default certificate location..."
if [ -d "$SYNO_CERT_DEFAULT" ]; then
    cp "$CERT_KEY_PATH" "$SYNO_CERT_DEFAULT/privkey.pem"
    cp "$CERT_PATH" "$SYNO_CERT_DEFAULT/cert.pem"
    cp "$CERT_FULLCHAIN_PATH" "$SYNO_CERT_DEFAULT/fullchain.pem"
    cp "$CA_CERT_PATH" "$SYNO_CERT_DEFAULT/chain.pem"
    chmod 600 "$SYNO_CERT_DEFAULT/privkey.pem"
    chmod 644 "$SYNO_CERT_DEFAULT/cert.pem"
    chmod 644 "$SYNO_CERT_DEFAULT/fullchain.pem"
    chmod 644 "$SYNO_CERT_DEFAULT/chain.pem"
    log "info" "✓ System default updated"
else
    log "warn" "System default directory not found: $SYNO_CERT_DEFAULT"
fi
echo ""

# Verify certificate files
log "info" "Step 4: Verifying certificate integrity..."
if verify_cert_files "$TARGET_DIR"; then
    log "info" "✓ Certificate files verified"
else
    log "error" "✗ Certificate verification failed"
    log "error" "Deployment may be incomplete"
    exit 1
fi
echo ""

# Restart services
log "info" "Step 5: Restarting services..."
if [ -f "$SCRIPT_DIR/restart-services.sh" ]; then
    bash "$SCRIPT_DIR/restart-services.sh"
    if [ $? -eq 0 ]; then
        log "info" "✓ Services restarted successfully"
    else
        log "warn" "✗ Some services failed to restart"
        log "warn" "Check logs for details"
    fi
else
    log "warn" "restart-services.sh not found, skipping service restart"
fi
echo ""

# Display certificate expiry
log "info" "Step 6: Certificate information..."
EXPIRY_DATE=$(get_cert_expiry "$TARGET_DIR/cert.pem")
if [ -n "$EXPIRY_DATE" ]; then
    log "info" "Certificate expires: $EXPIRY_DATE"
else
    log "warn" "Could not determine expiry date"
fi
echo ""

log "info" "========================================="
log "info" "Deployment Complete!"
log "info" "========================================="
log "info" "Certificate deployed successfully to Synology NAS"
log "info" "Target: $TARGET_DIR"
echo ""
