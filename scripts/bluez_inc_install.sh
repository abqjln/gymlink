#!/bin/bash -e
#
# Get bluez_source--Do not need to cmake as we will use source

date
if [ -z "$GYMLINK_LOCAL_LIB_DIR" ]; then
	printf "Please 'export GYMLINK_LOCAL_LIB_DIR=<path>' and rerun\n"
	exit 1
fi

# Remove any previous clone
cd ${GYMLINK_LOCAL_LIB_DIR}
rm -rf bluez_inc
git clone https://github.com/weliem/bluez_inc
date


