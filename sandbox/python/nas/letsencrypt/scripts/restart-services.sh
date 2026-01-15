#!/bin/sh
# Service detection and restart script
# Called by deploy-to-synology.sh after certificate deployment

# Source utility functions
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
. "$SCRIPT_DIR/utils.sh"

log "info" "Starting service restart process..."
echo ""

# Define common services that use certificates (space-separated)
# Core services
CORE_SERVICES="nginx"

# Optional services (may not be installed/running)
OPTIONAL_SERVICES="smbdav ftpd sshd synoscgi avahi-daemon"

# Package services (dynamically detected)
PACKAGE_SERVICES="pkgctl-WebDAVServer pkgctl-VPNCenter pkgctl-MailServer pkgctl-CloudStationShareSync"

# Dry run mode
DRY_RUN=false
if [ "$1" = "--dry-run" ]; then
    DRY_RUN=true
    log "info" "DRY RUN MODE - No services will be restarted"
    echo ""
fi

# Function: check_service_exists()
check_service_exists() {
    service=$1
    systemctl list-unit-files 2>/dev/null | grep -q "^${service}.service" || \
    systemctl list-units 2>/dev/null | grep -q "${service}.service"
}

# Function: check_service_running()
check_service_running() {
    service=$1
    systemctl is-active --quiet "$service" 2>/dev/null
}

# Function: restart_service()
restart_service() {
    service=$1
    action=$2

    if [ "$DRY_RUN" =true ]; then
        log "info" "[DRY RUN] Would $action: $service"
        return 0
    fi

    log "info" "Attempting to $action: $service"

    if [ "$action" ="reload" ]; then
        if systemctl reload "$service" 2>/dev/null; then
            log "info" "  ✓ Reloaded successfully"
            return 0
        else
            log "warn" "  ✗ Reload failed, trying restart..."
            action="restart"
        fi
    fi

    if [ "$action" ="restart" ]; then
        if systemctl restart "$service" 2>/dev/null; then
            log "info" "  ✓ Restarted successfully"
            return 0
        else
            log "error" "  ✗ Restart failed"
            return 1
        fi
    fi
}

# Counter for statistics
total_services=0
restarted_services=0
skipped_services=0
failed_services=0

log "info" "Checking core services..."
for service in "$CORE_SERVICES"; do
    total_services=$((total_services + 1))

    if ! check_service_exists "$service"; then
        log "debug" "Service not found: $service"
        skipped_services=$((skipped_services + 1))
        continue
    fi

    if ! check_service_running "$service"; then
        log "debug" "Service not running: $service (skipping)"
        skipped_services=$((skipped_services + 1))
        continue
    fi

    # Special handling for nginx - use reload instead of restart
    if [ "$service" ="nginx" ]; then
        if restart_service "$service" "reload"; then
            restarted_services=$((restarted_services + 1))
        else
            failed_services=$((failed_services + 1))
        fi
    else
        if restart_service "$service" "restart"; then
            restarted_services=$((restarted_services + 1))
        else
            failed_services=$((failed_services + 1))
        fi
    fi
done
echo ""

log "info" "Checking optional services..."
for service in "$OPTIONAL_SERVICES"; do
    total_services=$((total_services + 1))

    if ! check_service_exists "$service"; then
        log "debug" "Service not installed: $service"
        skipped_services=$((skipped_services + 1))
        continue
    fi

    if ! check_service_running "$service"; then
        log "debug" "Service not running: $service (skipping)"
        skipped_services=$((skipped_services + 1))
        continue
    fi

    if restart_service "$service" "restart"; then
        restarted_services=$((restarted_services + 1))
    else
        failed_services=$((failed_services + 1))
    fi
done
echo ""

log "info" "Checking package services..."
for service in "$PACKAGE_SERVICES"; do
    total_services=$((total_services + 1))

    if ! check_service_exists "$service"; then
        log "debug" "Package service not installed: $service"
        skipped_services=$((skipped_services + 1))
        continue
    fi

    if ! check_service_running "$service"; then
        log "debug" "Package service not running: $service (skipping)"
        skipped_services=$((skipped_services + 1))
        continue
    fi

    if restart_service "$service" "restart"; then
        restarted_services=$((restarted_services + 1))
    else
        failed_services=$((failed_services + 1))
    fi
done
echo ""

# Check certificate INFO file for additional services
log "info" "Checking certificate INFO file for additional services..."
SYNO_CERT_ARCHIVE="${SYNO_CERT_ARCHIVE:-/usr/syno/etc/certificate/_archive}"
INFO_FILE="$SYNO_CERT_ARCHIVE/INFO"

if [ -f "$INFO_FILE" ] && command -v jq &> /dev/null; then
    # Parse INFO file to find services using this certificate
    CERT_ID="${SYNO_CERT_ID}"
    if [ -n "$CERT_ID" ]; then
        SERVICES_IN_INFO=$(jq -r ".\"$CERT_ID\".services[]? // empty" "$INFO_FILE" 2>/dev/null || true)
        if [ -n "$SERVICES_IN_INFO" ]; then
            log "info" "Found services in INFO file for cert $CERT_ID"
            while IFS= read -r service; do
                total_services=$((total_services + 1))
                if check_service_exists "$service" && check_service_running "$service"; then
                    if restart_service "$service" "restart"; then
                        restarted_services=$((restarted_services + 1))
                    else
                        failed_services=$((failed_services + 1))
                    fi
                else
                    skipped_services=$((skipped_services + 1))
                fi
            done <<< "$SERVICES_IN_INFO"
        fi
    fi
else
    log "debug" "INFO file not found or jq not available: $INFO_FILE"
fi
echo ""

# Summary
log "info" "========================================="
log "info" "Service Restart Summary"
log "info" "========================================="
log "info" "Total services checked: $total_services"
log "info" "Successfully restarted: $restarted_services"
log "info" "Skipped (not running): $skipped_services"
log "info" "Failed to restart: $failed_services"
echo ""

if [ $failed_services -gt 0 ]; then
    log "warn" "Some services failed to restart"
    log "warn" "Please check logs and restart manually if needed"
    exit 1
fi

if [ $restarted_services -eq 0 ]; then
    log "warn" "No services were restarted"
    log "warn" "This may indicate a configuration issue"
fi

log "info" "Service restart process completed"
exit 0
