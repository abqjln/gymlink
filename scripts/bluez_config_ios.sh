#!/bin/bash -e

# https://developer.apple.com/library/archive/qa/qa1931/_index.html
# https://developer.apple.com/accessories/Accessory-Design-Guidelines.pdf

# Ignore power considerations
# conn_min_interval 15ms (was 24)
#echo 15 > /sys/kernel/debug/bluetooth/hci0/conn_min_interval
# conn_max_interval (= interval_min or interval_min+15) (was 40)
#echo 15 > /sys/kernel/debug/bluetooth/hci0/conn_max_interval
# 2s <= supervision_timeout <= 6s (was 42)
#echo 4 > /sys/kernel/debug/bluetooth/hci0/supervision_timeout
# conn_latency (same as slave_latency??) <= 30
# conn_latency

#cat /sys/kernel/debug/bluetooth/hci0/conn_min_interval
#24
#cat /sys/kernel/debug/bluetooth/hci0/conn_max_interval
#40
#cat /sys/kernel/debug/bluetooth/hci0/conn_latency
#0
#cat /sys/kernel/debug/bluetooth/hci0/supervision_timeout
#42


# 15ms <= conn_min_interval (multiples of 15ms) <= 2000ms
# (conn_max_interval = conn_min_interval = 15ms) OR (conn_max_interval = conn_min_interval + 15ms)
# conn_latency (same as slave_latency or peripheral_latency??) <= 30
# conn_max_interval * (conn_latency + ) <= 6000ms
# or conn_latency <= (6000/conn_max_interval) - 1
#
# supervision_timeout_min = (conn_max_interval * (conn_latency + 1) * 3) + 1

INTERVAL_MIN=15 # ms
INTERVAL_MAX=15 # ms
PERIPHERAL_LATENCY=0 # Connection intervals; 0 means don't skip any to sleep longer
SUPERVISION_TIMEOUT_MIN=$(($((${INTERVAL_MAX} * (${PERIPHERAL_LATENCY} + 1)*3))+1))

printf "min=%d, max=%d, latency=%d, timeout_min=%d\n" ${INTERVAL_MIN} ${INTERVAL_MAX} ${PERIPHERAL_LATENCY} ${SUPERVISION_TIMEOUT_MIN}

BT_DIR=/sys/kernel/debug/bluetooth/hci0
SAVE_DIR=/etc/bluetooth/kernel
mkdir -p ${SAVE_DIR}

FILE=conn_min_interval
if [ ! -f ${SAVE_DIR}/${FILE}.orig ]; then
	cp ${BT_DIR}/${FILE} ${SAVE_DIR}/${FILE}.orig
fi
echo ${INTERVAL_MIN} > ${BT_DIR}/${FILE}

FILE=conn_max_interval
if [ ! -f ${SAVE_DIR}/${FILE}.orig ]; then
	cp ${BT_DIR}/${FILE} ${SAVE_DIR}/${FILE}.orig
fi
echo ${INTERVAL_MAX} > ${BT_DIR}/${FILE}

FILE=conn_latency
if [ ! -f ${SAVE_DIR}/${FILE}.orig ]; then
	cp ${BT_DIR}/${FILE} ${SAVE_DIR}/${FILE}.orig
fi
echo ${PERIPHERAL_LATENCY} > ${BT_DIR}/${FILE}

FILE=supervision_timeout
if [ ! -f ${SAVE_DIR}/${FILE}.orig ]; then
	cp ${BT_DIR}/${FILE} ${SAVE_DIR}/${FILE}.orig
fi
echo ${SUPERVISION_TIMEOUT_MIN} > ${BT_DIR}/${FILE}


