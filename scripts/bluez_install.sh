#!/bin/bash -e
#
# Install bluez
# Readme at https://github.com/bluez/bluez is incomplete. This worked as of 12/30/2024. 54 minutes on RPi Zero W

date
if [ -z "$GYMLINK_LOCAL_LIB_DIR" ]; then
	printf "$0: Please 'export GYMLINK_LOCAL_LIB_DIR=<path>' and rerun\n"
	exit 1
fi

apt install -y git libtool libdbus-1-dev libdbus-glib-1-dev libical-dev libreadline-dev libudev-dev docutils-common

# Remove any previous and get current version
cd ${GYMLINK_LOCAL_LIB_DIR}
rm -rf bluez
git clone https://github.com/bluez/bluez

cd ${GYMLINK_LOCAL_LIB_DIR}/bluez
autoreconf -i
./configure --prefix=/usr --mandir=/usr/share/man --sysconfdir=/etc --localstatedir=/var
make && make install
bluetoothctl -v
date
