#!/bin/bash
# Deploy certificates to Synology Certificate Manager
# This script runs on the Synology host (not in container)

set -e

if [ -z "$1" ] || [ -z "$2" ]; then
    echo "Usage: sudo ./deploy-to-synology.sh <domain> <cert-id>"
    echo ""
    echo "  <domain>   : Your domain name (e.g., example.com)"
    echo "  <cert-id>  : Certificate ID in Synology (e.g., CKeLNv or 'default')"
    echo ""
    echo "To find certificate IDs:"
    echo "  ls -la /usr/syno/etc/certificate/_archive/"
    echo ""
    echo "Or use 'default' to update the default system certificate"
    exit 1
fi

DOMAIN=$1
CERT_ID=$2
CERT_SOURCE_DIR="/volume1/docker/letsencrypt/certs"
SYNOLOGY_CERT_BASE="/usr/syno/etc/certificate"

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo "Error: This script must be run as root (use sudo)"
    exit 1
fi

# Verify source certificates exist
if [ ! -f "${CERT_SOURCE_DIR}/${DOMAIN}.crt" ]; then
    echo "Error: Certificate file not found: ${CERT_SOURCE_DIR}/${DOMAIN}.crt"
    exit 1
fi

if [ ! -f "${CERT_SOURCE_DIR}/${DOMAIN}.key" ]; then
    echo "Error: Private key file not found: ${CERT_SOURCE_DIR}/${DOMAIN}.key"
    exit 1
fi

echo "==================================="
echo "Deploying certificate to Synology"
echo "==================================="
echo "Domain: $DOMAIN"
echo "Certificate ID: $CERT_ID"
echo ""

# Determine target directory
if [ "$CERT_ID" = "default" ]; then
    # Find the default certificate ID
    if [ -L "${SYNOLOGY_CERT_BASE}/system/default" ]; then
        DEFAULT_ID=$(basename $(readlink ${SYNOLOGY_CERT_BASE}/system/default))
        TARGET_DIR="${SYNOLOGY_CERT_BASE}/_archive/${DEFAULT_ID}"
        echo "Found default certificate ID: $DEFAULT_ID"
    else
        echo "Error: Could not find default certificate"
        exit 1
    fi
else
    TARGET_DIR="${SYNOLOGY_CERT_BASE}/_archive/${CERT_ID}"
fi

# Check if target directory exists
if [ ! -d "$TARGET_DIR" ]; then
    echo "Error: Certificate directory not found: $TARGET_DIR"
    echo ""
    echo "Available certificate IDs:"
    ls -1 ${SYNOLOGY_CERT_BASE}/_archive/
    exit 1
fi

# Backup existing certificates
BACKUP_DIR="${TARGET_DIR}/backup_$(date +%Y%m%d_%H%M%S)"
mkdir -p "$BACKUP_DIR"
cp ${TARGET_DIR}/cert.pem "$BACKUP_DIR/" 2>/dev/null || true
cp ${TARGET_DIR}/privkey.pem "$BACKUP_DIR/" 2>/dev/null || true
cp ${TARGET_DIR}/chain.pem "$BACKUP_DIR/" 2>/dev/null || true
cp ${TARGET_DIR}/fullchain.pem "$BACKUP_DIR/" 2>/dev/null || true
echo "Backed up existing certificates to: $BACKUP_DIR"

# Deploy new certificates
echo "Copying new certificates..."

# Copy fullchain as cert.pem and fullchain.pem
cp "${CERT_SOURCE_DIR}/${DOMAIN}.crt" "${TARGET_DIR}/fullchain.pem"
cp "${CERT_SOURCE_DIR}/${DOMAIN}.crt" "${TARGET_DIR}/cert.pem"

# Copy private key
cp "${CERT_SOURCE_DIR}/${DOMAIN}.key" "${TARGET_DIR}/privkey.pem"

# Extract certificate and chain
# The fullchain typically contains: certificate + intermediate CA + root CA
# We need to split it for chain.pem
openssl x509 -in "${CERT_SOURCE_DIR}/${DOMAIN}.crt" -out "${TARGET_DIR}/cert.pem"
sed -n '/-----BEGIN CERTIFICATE-----/,$p' "${CERT_SOURCE_DIR}/${DOMAIN}.crt" | \
    sed '1,/-----END CERTIFICATE-----/d' > "${TARGET_DIR}/chain.pem"

# Set correct permissions
chmod 600 ${TARGET_DIR}/*.pem
chown root:root ${TARGET_DIR}/*.pem

echo "Certificates deployed successfully!"
echo ""
echo "Reloading Synology services..."

# Helper function to check if service is running and restart it
restart_service_if_running() {
    local service=$1
    local action=${2:-restart}  # Default to restart, can pass "reload"

    if command -v synosystemctl &> /dev/null; then
        # Check if service is active
        if synosystemctl is-active "$service" &> /dev/null; then
            synosystemctl $action "$service"
            echo "✓ $service ${action}ed"
            return 0
        fi
    else
        # Fallback to old synoservicectl
        if /usr/syno/sbin/synoservicectl --status "$service" | grep -q "is running"; then
            /usr/syno/sbin/synoservicectl --$action "$service"
            echo "✓ $service ${action}ed"
            return 0
        fi
    fi
    return 1
}

# Always reload nginx (DSM web interface)
restart_service_if_running nginx reload

# List of services that may use certificates
# Only restart if they are running
CERT_SERVICES=(
    "smbd"           # SMB/CIFS file sharing
    "nmbd"           # NetBIOS name service
    "afpd"           # AFP file sharing (Mac)
    "avahi"          # Bonjour/mDNS service discovery
    "ftpd"           # FTP server (if FTPS enabled)
    "sshd"           # SSH server
    "vpnserver"      # VPN Server
    "MailServer"     # Mail Server
    "CalendarServer" # Calendar/CardDAV
    "ContactServer"  # Contacts/CardDAV
    "ldap-server"    # LDAP Server
)

echo ""
echo "Checking and restarting services that use certificates..."
for service in "${CERT_SERVICES[@]}"; do
    restart_service_if_running "$service" || true
done

echo ""
echo "==================================="
echo "Certificate deployment complete!"
echo "==================================="
echo ""
echo "The certificate has been imported to Synology."
echo "Go to Control Panel > Security > Certificate to verify."
