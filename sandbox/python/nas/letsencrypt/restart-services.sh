#!/bin/bash
# Restart Synology services after certificate renewal
# This script runs on the Synology HOST (not in container)

echo "$(date): Restarting Synology services after certificate renewal" >> /volume1/docker/letsencrypt/renewal.log

# Helper function to check if service is running and restart it
restart_service_if_running() {
    local service=$1
    local action=${2:-restart}

    if command -v synosystemctl &> /dev/null; then
        if synosystemctl is-active "$service" &> /dev/null 2>&1; then
            synosystemctl $action "$service"
            echo "$(date): ✓ $service ${action}ed" >> /volume1/docker/letsencrypt/renewal.log
            return 0
        fi
    else
        if /usr/syno/sbin/synoservicectl --status "$service" 2>/dev/null | grep -q "is running"; then
            /usr/syno/sbin/synoservicectl --$action "$service"
            echo "$(date): ✓ $service ${action}ed" >> /volume1/docker/letsencrypt/renewal.log
            return 0
        fi
    fi
    return 1
}

# Always reload nginx (DSM web interface)
restart_service_if_running nginx reload

# List of services that may use certificates
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

# Restart services that are running
for service in "${CERT_SERVICES[@]}"; do
    restart_service_if_running "$service" || true
done

echo "$(date): Certificate renewal complete" >> /volume1/docker/letsencrypt/renewal.log
