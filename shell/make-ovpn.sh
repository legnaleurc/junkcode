#!/bin/bash

# Script to generate OpenVPN client configuration file
# Usage: ./make-ovpn.sh <client-name> > client-name.ovpn

set -e

CLIENT_NAME="$1"

# Paths
OPENVPN_DIR="/etc/openvpn"
EASYRSA_PKI="${OPENVPN_DIR}/easy-rsa/pki"
CLIENT_TEMPLATE="${OPENVPN_DIR}/client-template.txt"
CA_CERT="${OPENVPN_DIR}/ca.crt"
TLS_CRYPT_KEY="${OPENVPN_DIR}/tls-crypt.key"
CLIENT_CERT="${EASYRSA_PKI}/issued/${CLIENT_NAME}.crt"
CLIENT_KEY="${EASYRSA_PKI}/private/${CLIENT_NAME}.key"

# Check if client name is provided
if [ -z "$CLIENT_NAME" ]; then
    echo "Error: Client name is required" >&2
    echo "Usage: $0 <client-name> > client-name.ovpn" >&2
    exit 1
fi

# Check if template file exists
if [ ! -f "$CLIENT_TEMPLATE" ]; then
    echo "Error: Client template file not found at $CLIENT_TEMPLATE" >&2
    exit 1
fi

# Check if CA certificate exists
if [ ! -f "$CA_CERT" ]; then
    echo "Error: CA certificate not found at $CA_CERT" >&2
    exit 1
fi

# Check if client certificate exists
if [ ! -f "$CLIENT_CERT" ]; then
    echo "Error: Client certificate not found at $CLIENT_CERT" >&2
    echo "Make sure you have generated the client certificate using easyrsa" >&2
    exit 1
fi

# Check if client key exists
if [ ! -f "$CLIENT_KEY" ]; then
    echo "Error: Client key not found at $CLIENT_KEY" >&2
    exit 1
fi

# Check if tls-crypt key exists
if [ ! -f "$TLS_CRYPT_KEY" ]; then
    echo "Error: TLS-Crypt key not found at $TLS_CRYPT_KEY" >&2
    exit 1
fi

# Output the client template
cat "$CLIENT_TEMPLATE"
echo ""

# Embed CA certificate
echo "<ca>"
cat "$CA_CERT"
echo "</ca>"
echo ""

# Embed client certificate (strip header/footer and any text before cert)
echo "<cert>"
sed -n '/BEGIN CERTIFICATE/,/END CERTIFICATE/p' "$CLIENT_CERT"
echo "</cert>"
echo ""

# Embed client key
echo "<key>"
cat "$CLIENT_KEY"
echo "</key>"
echo ""

# Embed tls-crypt key
echo "<tls-crypt>"
cat "$TLS_CRYPT_KEY"
echo "</tls-crypt>"
