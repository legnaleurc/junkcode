#!/bin/bash
# Helper script to issue and install SSL certificates

set -e

SYNOLOGY_AUTO_DEPLOY=false
SYNOLOGY_CERT_ID=""

# Parse flags
while [[ $# -gt 0 ]]; do
    case $1 in
        --synology-cert-id)
            SYNOLOGY_AUTO_DEPLOY=true
            SYNOLOGY_CERT_ID="$2"
            shift 2
            ;;
        --synology-cert-id=*)
            SYNOLOGY_AUTO_DEPLOY=true
            SYNOLOGY_CERT_ID="${1#*=}"
            shift
            ;;
        *)
            break
            ;;
    esac
done

if [ -z "$1" ]; then
    echo "Usage: ./issue-cert.sh [OPTIONS] yourdomain.com [additional-domains...]"
    echo ""
    echo "Options:"
    echo "  --synology-cert-id=ID    Auto-deploy to Synology Certificate Manager"
    echo "                           (use 'default' for default cert, or specific ID)"
    echo ""
    echo "Examples:"
    echo "  ./issue-cert.sh example.com"
    echo "  ./issue-cert.sh example.com '*.example.com'"
    echo "  ./issue-cert.sh --synology-cert-id=default example.com"
    echo "  ./issue-cert.sh --synology-cert-id=CKeLNv example.com '*.example.com'"
    exit 1
fi

MAIN_DOMAIN=$1
shift
ADDITIONAL_DOMAINS=("$@")

echo "==================================="
echo "Issuing certificate for:"
echo "  Main: $MAIN_DOMAIN"
if [ ${#ADDITIONAL_DOMAINS[@]} -gt 0 ]; then
    echo "  Additional:"
    for domain in "${ADDITIONAL_DOMAINS[@]}"; do
        echo "    - $domain"
    done
fi
echo "==================================="

# Build the issue command
ISSUE_CMD="--issue --dns dns_cf -d $MAIN_DOMAIN"
for domain in "${ADDITIONAL_DOMAINS[@]}"; do
    ISSUE_CMD="$ISSUE_CMD -d $domain"
done

# Issue the certificate
echo ""
echo "Step 1: Issuing certificate..."
docker exec letsencrypt-acme $ISSUE_CMD

# Install the certificate
echo ""
echo "Step 2: Installing certificate to /certs..."
docker exec letsencrypt-acme --install-cert \
    -d "$MAIN_DOMAIN" \
    --key-file "/certs/${MAIN_DOMAIN}.key" \
    --fullchain-file "/certs/${MAIN_DOMAIN}.crt" \
    --reloadcmd "echo 'Certificate installed successfully'"

echo ""
echo "==================================="
echo "Certificate issued and installed!"
echo "==================================="
echo ""
echo "Certificate files:"
echo "  Certificate: /certs/${MAIN_DOMAIN}.crt"
echo "  Private Key: /certs/${MAIN_DOMAIN}.key"
echo ""
echo "On your Synology NAS, find them at:"
echo "  \$OUTPUT_PATH/${MAIN_DOMAIN}.crt"
echo "  \$OUTPUT_PATH/${MAIN_DOMAIN}.key"
echo ""

# Auto-deploy to Synology if requested
if [ "$SYNOLOGY_AUTO_DEPLOY" = true ]; then
    echo "==================================="
    echo "Auto-deploying to Synology..."
    echo "==================================="

    if [ ! -f "./deploy-to-synology.sh" ]; then
        echo "Error: deploy-to-synology.sh not found"
        echo "Make sure you're running this script from the letsencrypt directory"
        exit 1
    fi

    echo "Running: sudo ./deploy-to-synology.sh $MAIN_DOMAIN $SYNOLOGY_CERT_ID"
    sudo ./deploy-to-synology.sh "$MAIN_DOMAIN" "$SYNOLOGY_CERT_ID"

    echo ""
    echo "Certificate has been imported to Synology Certificate Manager!"
    echo "Go to Control Panel > Security > Certificate to verify."
fi
echo ""
