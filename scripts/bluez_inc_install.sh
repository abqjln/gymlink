#!/bin/bash -e
#
# Get bluez_source--Do not need to cmake as we will use source

if [ -z "$GYMLINK_ROOT" ]; then
	printf "Please 'export GYMLINK_ROOT=<path>' and rerun\n"
	exit 1
fi


# Remove any previous clone
cd ${GYMLINK_ROOT}
rm -rf bluez_inc
git clone https://github.com/weliem/bluez_inc


