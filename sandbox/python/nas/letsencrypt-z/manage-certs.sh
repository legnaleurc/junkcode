#!/bin/bash
# Helper script for certificate management operations

set -e

CONTAINER_NAME="letsencrypt-acme"

show_help() {
    echo "Certificate Management Helper"
    echo ""
    echo "Usage: ./manage-certs.sh <command>"
    echo ""
    echo "Commands:"
    echo "  list           - List all certificates"
    echo "  info <domain>  - Show certificate info"
    echo "  renew <domain> - Force renew a certificate"
    echo "  logs           - Show container logs"
    echo "  logs-follow    - Follow container logs"
    echo "  shell          - Open shell in container"
    echo ""
    echo "Examples:"
    echo "  ./manage-certs.sh list"
    echo "  ./manage-certs.sh info example.com"
    echo "  ./manage-certs.sh renew example.com"
}

check_container() {
    if ! docker ps --format '{{.Names}}' | grep -q "^${CONTAINER_NAME}$"; then
        echo "Error: Container '$CONTAINER_NAME' is not running"
        echo "Start it with: docker-compose up -d"
        exit 1
    fi
}

case "$1" in
    list)
        check_container
        echo "Listing all certificates..."
        docker exec $CONTAINER_NAME --list
        ;;

    info)
        if [ -z "$2" ]; then
            echo "Error: Domain name required"
            echo "Usage: ./manage-certs.sh info <domain>"
            exit 1
        fi
        check_container
        echo "Certificate info for $2:"
        docker exec $CONTAINER_NAME --info -d "$2"
        ;;

    renew)
        if [ -z "$2" ]; then
            echo "Error: Domain name required"
            echo "Usage: ./manage-certs.sh renew <domain>"
            exit 1
        fi
        check_container
        echo "Force renewing certificate for $2..."
        docker exec $CONTAINER_NAME --renew -d "$2" --force
        ;;

    logs)
        echo "Showing container logs..."
        docker logs $CONTAINER_NAME
        ;;

    logs-follow)
        echo "Following container logs (Ctrl+C to exit)..."
        docker logs -f $CONTAINER_NAME
        ;;

    shell)
        check_container
        echo "Opening shell in container..."
        docker exec -it $CONTAINER_NAME sh
        ;;

    help|--help|-h)
        show_help
        ;;

    *)
        echo "Error: Unknown command '$1'"
        echo ""
        show_help
        exit 1
        ;;
esac
