#!/bin/sh
# Shared utility functions for acme.sh automation scripts

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Log levels
LOG_LEVEL_DEBUG=0
LOG_LEVEL_INFO=1
LOG_LEVEL_WARN=2
LOG_LEVEL_ERROR=3

# Get current log level from environment or default to INFO
get_log_level() {
    case "${LOG_LEVEL:-info}" in
        debug) echo $LOG_LEVEL_DEBUG ;;
        info)  echo $LOG_LEVEL_INFO ;;
        warn)  echo $LOG_LEVEL_WARN ;;
        error) echo $LOG_LEVEL_ERROR ;;
        *)     echo $LOG_LEVEL_INFO ;;
    esac
}

CURRENT_LOG_LEVEL=$(get_log_level)

# log() - Structured logging with timestamps
# Usage: log "info" "message"
log() {
    level=$1
    shift
    message="$*"
    timestamp=$(date '+%Y-%m-%d %H:%M:%S')
    log_file="${LOG_FILE:-/logs/acme.log}"

    # Determine log level number
    case "$level" in
        debug) level_num=$LOG_LEVEL_DEBUG; color=$BLUE ;;
        info)  level_num=$LOG_LEVEL_INFO;  color=$GREEN ;;
        warn)  level_num=$LOG_LEVEL_WARN;  color=$YELLOW ;;
        error) level_num=$LOG_LEVEL_ERROR; color=$RED ;;
        *)     level_num=$LOG_LEVEL_INFO;  color=$NC ;;
    esac

    # Only log if level is high enough (use arithmetic expansion safely)
    if [ "${level_num:-1}" -ge "${CURRENT_LOG_LEVEL:-1}" ] 2>/dev/null; then
        # Convert level to uppercase (POSIX way)
        level_upper=$(echo "$level" | tr '[:lower:]' '[:upper:]')

        # Format log message
        log_msg="[$timestamp] [$level_upper] $message"

        # Output to console with color
        printf "%b\n" "${color}${log_msg}${NC}"

        # Append to log file (without color codes)
        echo "$log_msg" >> "$log_file"
    fi
}

# validate_env() - Check required environment variables
# Usage: validate_env VAR1 VAR2 VAR3
validate_env() {
    missing=""

    for var in "$@"; do
        eval value=\$$var
        if [ -z "$value" ]; then
            if [ -z "$missing" ]; then
                missing="$var"
            else
                missing="$missing $var"
            fi
        fi
    done

    if [ -n "$missing" ]; then
        log "error" "Missing required environment variables: $missing"
        return 1
    fi

    return 0
}

# get_cert_expiry() - Parse certificate expiration date
# Usage: expiry=$(get_cert_expiry /path/to/cert.pem)
get_cert_expiry() {
    cert_file=$1

    if [ ! -f "$cert_file" ]; then
        log "error" "Certificate file not found: $cert_file"
        return 1
    fi

    openssl x509 -in "$cert_file" -noout -enddate 2>/dev/null | cut -d= -f2
}

# check_cert_needs_renewal() - Compare against RENEW_CHECK_DAYS
# Usage: if check_cert_needs_renewal /path/to/cert.pem; then ...
check_cert_needs_renewal() {
    cert_file=$1
    check_days=${RENEW_CHECK_DAYS:-30}

    expiry_date=$(get_cert_expiry "$cert_file")
    if [ -z "$expiry_date" ]; then
        return 1
    fi

    expiry_epoch=$(date -d "$expiry_date" +%s 2>/dev/null || date -j -f "%b %d %H:%M:%S %Y %Z" "$expiry_date" +%s 2>/dev/null)
    if [ -z "$expiry_epoch" ]; then
        log "error" "Could not parse certificate expiry date"
        return 1
    fi

    current_epoch=$(date +%s)
    days_until_expiry=$(( (expiry_epoch - current_epoch) / 86400 ))

    log "info" "Certificate expires in $days_until_expiry days"

    if [ "$days_until_expiry" -le "$check_days" ]; then
        log "warn" "Certificate needs renewal (expires in $days_until_expiry days, threshold: $check_days days)"
        return 0
    fi

    return 1
}

# backup_cert() - Create backup before deployment
# Usage: backup_cert /path/to/cert/directory
backup_cert() {
    cert_dir=$1
    backup_dir="${cert_dir}.backup.$(date +%Y%m%d_%H%M%S)"

    if [ ! -d "$cert_dir" ]; then
        log "warn" "Certificate directory not found, skipping backup: $cert_dir"
        return 0
    fi

    log "info" "Creating backup: $backup_dir"
    cp -r "$cert_dir" "$backup_dir" 2>/dev/null

    if [ $? -eq 0 ]; then
        log "info" "Backup created successfully"

        # Keep only last 5 backups
        backup_parent=$(dirname "$cert_dir")
        backup_pattern=$(basename "$cert_dir").backup.*
        ls -dt "$backup_parent/$backup_pattern" 2>/dev/null | tail -n +6 | xargs rm -rf 2>/dev/null

        return 0
    else
        log "error" "Failed to create backup"
        return 1
    fi
}

# verify_cert_files() - Validate certificate file integrity
# Usage: if verify_cert_files /path/to/cert/dir; then ...
verify_cert_files() {
    cert_dir=$1
    required_files="privkey.pem cert.pem fullchain.pem chain.pem"
    errors=0

    log "info" "Verifying certificate files in $cert_dir"

    for file in $required_files; do
        file_path="$cert_dir/$file"

        if [ ! -f "$file_path" ]; then
            log "error" "Missing file: $file"
            errors=$((errors + 1))
            continue
        fi

        file_size=$(stat -f%z "$file_path" 2>/dev/null || stat -c%s "$file_path" 2>/dev/null)
        if [ -z "$file_size" ] || [ "$file_size" = "0" ]; then
            log "error" "Empty file: $file"
            errors=$((errors + 1))
            continue
        fi

        log "debug" "File OK: $file (${file_size} bytes)"
    done

    if [ $errors -gt 0 ]; then
        log "error" "Certificate verification failed: $errors error(s)"
        return 1
    fi

    log "info" "Certificate files verified successfully"
    return 0
}

# Test function (called when script is run with 'test' argument)
test_utils() {
    log "info" "Testing utility functions..."
    log "debug" "This is a debug message"
    log "info" "This is an info message"
    log "warn" "This is a warning message"
    log "error" "This is an error message"
    echo ""
    log "info" "Utility tests completed"
}

# If script is run directly with 'test' argument, run tests
if [ "$1" == "test" ]; then
    test_utils
fi
