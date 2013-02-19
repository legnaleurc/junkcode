#! /bin/bash

REAL_ROOT=
REAL_HOME="${REAL_ROOT}/home/legnaleurc"
FAKE_ROOT=/root/precise-i386
FAKE_HOME="${FAKE_ROOT}/home/steam"

mount --bind "${REAL_HOME}/.local/share/Steam" "${FAKE_HOME}/.local/share/Steam"
mount --bind "${REAL_ROOT}/dev/shm" "${FAKE_ROOT}/dev/shm"
mount --bind "${REAL_ROOT}/dev/snd" "${FAKE_ROOT}/dev/snd"
mount --bind "${REAL_ROOT}/var/run/dbus" "${FAKE_ROOT}/var/run/dbus"

sandbox enter "${FAKE_ROOT}"

umount "${FAKE_ROOT}/var/run/dbus"
umount "${FAKE_ROOT}/dev/snd"
umount "${FAKE_ROOT}/dev/shm"
umount "${FAKE_HOME}/.local/share/Steam"
