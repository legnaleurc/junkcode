#! /bin/sh

PID=`pidof -s ksnapshot`

if [ -z ${PID} ] ; then
	`which ksnapshot` -caption KSnapshot &
	exit 0
fi

DBUS_SERVICENAME="org.kde.ksnapshot-${PID}"
DBUS_PATH='/KSnapshot'
DBUS_METHOD='slotGrab'

qdbus ${DBUS_SERVICENAME} ${DBUS_PATH} ${DBUS_METHOD}
