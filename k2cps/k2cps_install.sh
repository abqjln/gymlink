#!/bin/bash -e
#
# sudo -E ${GYMLINK_ROOT}/k2cps/k2cps_install.sh

date
if [ -z "$GYMLINK_ROOT" ]; then
	printf "Please 'export GYMLINK_ROOT=<path>' and rerun\n"
	exit 1
fi

mkdir -p ${GYMLINK_ROOT}

#
apt update && apt -y full-upgrade
apt install bc

# Clone gymlink (need to have it be public)
# git clone https://github.com/abqjln/gymlink
# ABQJLN--clone with Git desktop

# Install bluez if less than 5.79
VER=$(bluetoothctl -v | awk '{print $2}')
if [ 1 -eq "$(echo "${VER} < 5.79" | bc)" ]; then
	sudo -E ${GYMLINK_ROOT}/gymlink/scripts/bluez_install.sh
fi

# Configure bluez comm settings based on aggressive ios
sudo -E ${GYMLINK_ROOT}/gymlink/scripts/bluez_config_ios.sh

# Disable automatic registration of device information service (DIS)
sed -i 's\#DeviceID.*\DeviceID = false\g' /etc/bluetooth/main.conf

# Disable battery service plugin
systemctl stop bluetooth
SERVICE=$(systemctl show -p FragmentPath bluetooth | sed -n -e  's/^.*=//p')
sed -i 's\ExecStart=/usr/libexec/bluetooth/bluetoothd\ExecStart=/usr/libexec/bluetooth/bluetoothd -P battery\g' $SERVICE

# Restart bluetooth service
systemctl daemon-reload && systemctl restart bluetooth && systemctl status bluetooth

# Get bluez_inc source (do not need to cmake library)
sudo -E ${GYMLINK_ROOT}/gymlink/scripts/bluez_inc_install.sh

date
