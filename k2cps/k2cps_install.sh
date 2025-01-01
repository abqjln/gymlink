#!/bin/bash -xe

date
set -e

if [ -z "$GYMLINK_LOCAL_LIB_DIR" ]; then
	printf "Please 'export GYMLINK_LOCAL_LIB_DIR=<path>' and rerun\n"
	exit 1
fi

mkdir -p ${GYMLINK_LOCAL_LIB_DIR}

#
apt update && apt -y full-upgrade

# Clone gymlink (need to fix auth or have it be public)
cd ${HOME}
#git clone https://github.com/abqjln/gymlink
# OR use Git desktop

# Install bluez
sudo -E ./gymlink/scripts/bluez_install.sh

# Configure bluez comm settings based on aggressive ios
sudo -E ./gymlink/scripts/bluez_config_ios.sh

# Disable automatic registration of device information service (DIS)
sed -i 's\#DeviceID.*\DeviceID = false\g' /etc/bluetooth/main.conf

# Disable battery service plugin
systemctl stop bluetooth
SERVICE=$(systemctl show -p FragmentPath bluetooth | sed -n -e  's/^.*=//p')
sed -i 's\ExecStart=/usr/libexec/bluetooth/bluetoothd\ExecStart=/usr/libexec/bluetooth/bluetoothd -P battery\g' $SERVICE

# Restart bluetooth service
systemctl daemon-reload && systemctl restart bluetooth && systemctl status bluetooth

# Get bluez_inc source (do not need to cmake library)
sudo -E ./gymlink/scripts/bluez_inc_install.sh

date
