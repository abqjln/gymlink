#! /bin/bash
# jln-Commented version https://github.com/KeiserCorp/Keiser.M3i.BLE-HCI-Simulator/blob/master/M3i_Sim.sh
# --------------------------------------------------------
# Script simulates a Keiser M3i bike using HCI device 0.
# --------------------------------------------------------
# Two broadcast rates are available, depending on the
# version of bike that is to be simulated.
# --------------------------------------------------------
#
# Need
# sudo apt install bc # to avoid 08, 09 interpretation as octal


ADNAME="4D 33" # M3
ADNAME_LEN_INC_FLAG="03"
ADNAME_TYPE="09"

#ADNAME="70 69 2D 4D 33" # pi-M3
#ADNAME_LEN_INC_FLAG="06"
#ADNAME_TYPE="09"


trap cntl_c INT

#RATE="80 0C" # (0x0C80/1600Hz_clock)=2.0 Seconds - For bikes < 6.30
RATE="37 02" # (0x0237/1600Hz_clock)=0.354375 Seconds - For bikes > 6.30

MAJOR="06"
MINOR="40"
DATATYPE="00"
M3_CADENCE="00 00"
HR="00 00"
M3_POWER="00 00"
KCAL="00 00"
MINUTES="00"
SECS="00"
TRIP="00 00"
GEAR="01"

BIKEID="00"

POWER=0
CADENCE=128

function cntl_c {
    # Disable advertising
    printf "\nDisabling advertising\n"
    hcitool -i hci0 cmd 0x08 0x000A 00 > /dev/null;
    sudo systemctl restart bluetooth
    exit
}

function init {
    # Clear controller
    hciconfig hci0 down > /dev/null;
    hciconfig hci0 up > /dev/null;
	sleep 1

    # Disable advertising
    hcitool -i hci0 cmd 0x08 0x000A 00 > /dev/null;

    # Set advertising rate
    hcitool -i hci0 cmd 0x08 0x0006 $RATE  $RATE  03  00  00  00 00 00 00 00 00 07 00 > /dev/null;

    # Set advertising message
    set_broadcast;

    # Enable advertising
    hcitool -i hci0 cmd 0x08 0x000A 01 > /dev/null;
}

function set_broadcast {
	MINUTES=$(date +%M)
	SECONDS=$(date +%S)
	# Hex
	HMIN=$(printf "%02X\n" $(( 10#$MINUTES )))
	HSEC=$(printf "%02X\n" $(( 10#$SECONDS )))

    # Set advertising message
	MFR_DATA=$(printf "${MAJOR} ${MINOR} ${DATATYPE} ${BIKEID} ${M3_CADENCE} ${M3_HR} ${M3_POWER} ${KCAL} ${HMIN} ${HSEC} ${M3_TRIP} ${GEAR}")
	hcitool -i hci0 cmd 0x08 0x0008 1C ${ADNAME_LEN_INC_FLAG} ${ADNAME_TYPE} ${ADNAME} 02 01 04 14 FF 02 01 $MFR_DATA > /dev/null;
}

function run {
    init

	HIMASK=0xFF00
	LOMASK=0x00FF

	POWERMIN=40
	POWERMAX=60
	CADENCEMIN=110
	CADENCEMAX=130
	DISTANCEMIN=0
	DISTANCEMAX=50 # 0.1 resolution
	HRMIN=60
	HRMAX=80

	POWER=$POWERMIN
	CADENCE=$CADENCEMIN
	DISTANCE=$DISTANCEMIN
	HR=$HRMIN

    for ((i = 0 ; i < 5000 ; i++ )); do
        sleep 0.3

        POWER=$[$POWER + 1]
        if [ "$POWER" -ge $POWERMAX ]; then
            POWER=$POWERMIN;
        fi
		M3_POWER="$(printf "%02x %02x\n" "$(( $LOMASK & $POWER ))" "$(( $HIMASK & $POWER ))")"

        CADENCE=$[$CADENCE + 1]
        if [ "$CADENCE" -ge $CADENCEMAX ]; then
            CADENCE=$CADENCEMIN;
        fi
        # M3 wants cadence in 0.1 RPM
		M3_CADENCE="$(printf "%02x %02x\n" "$[$LOMASK & ($CADENCE*10)]" "$[($HIMASK & ($CADENCE*10))>>8]")"

        HR=$[$HR + 1]
        if [ "$HR" -ge $HRMAX ]; then
            HR=$HRMIN;
        fi
        # M3 wants HR in 0.1 BPM
		M3_HR="$(printf "%02x %02x\n" "$[$LOMASK & ($HR*10)]" "$[($HIMASK & ($HR*10))>>8]")"

        DISTANCE=$[$DISTANCE + 1]
        if [ "$DISTANCE" -ge $DISTANCEMAX ]; then
            DISTANCE=$DISTANCEMIN;
        fi
        # M3 wants distance in 0.1 km or miles
		M3_TRIP="$(printf "%02x %02x\n" "$[$LOMASK & ($DISTANCE*10)]" "$[($HIMASK & ($DISTANCE*10))>>8]")"

        set_broadcast
        printf "Cadence=%4d, Power=%4d, Distance=%5.1f, HR=%4d, Time= %02d:%02d [${MAJOR} : ${MINOR} : ${DATATYPE} : ${BIKEID} : ${M3_CADENCE} : ${M3_HR} : ${M3_POWER} : $KCAL : ${HMIN} : ${HSEC} : ${M3_TRIP} : ${GEAR}]\r" \
			${CADENCE} ${POWER} $(echo "scale=2; $DISTANCE/10" | bc) ${HR} $(echo "$MINUTES" | bc) $(echo "$SECONDS" | bc)
    done

    sudo systemctl restart bluetooth
}

run;

printf "\n"
