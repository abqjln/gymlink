Gymlink is a C implementation of Bluetooth Low Energy (BLE) communication between fitness peripherals/servers and centrals/clients using Raspberry Pis. Specific applications are being developed as the author has devices.

Gymlink coding has been intentionally pedantic in style and commenting to provide a learning tool. All the possible compiler warnings are turned on. I knew nothing about BLE or fitness machine communication prior to summer of 2024, and quite likely will forget them soon, so an attempt was made to write everything down. The intent is to help the author remember--and others learn. Bluetooth using C should not be this difficult!

This code was enabled by the excellent C library bluez_inc from @weliem https://github.com/weliem/bluez_inc, which abstracts from the dBus-based https://github.com/bluez/bluez

I found other clues scattered around the not-well-documented world of BLE fitness including the Pelomon project https://github.com/ihaque/pelomon

As will become evident, I have little experience using GitHub, and @weliem in particular has been quite kind in correcting errors. The best evidence of usefulness is continuing critiques and corrections; please let me know if anything could be corrected or improved.

# k2cps
Converts the propietary, pubished BLE broadcast (not connectable) BLE beacon advertising data from a Keiser M3i spin bike (purchased circa 2000) into the (connectable) BLE Cycling Power Service (CPS). Testing has been done on Garmin Fenix8 watch with software ranging from 12.19 through the 13.12 beta. It was also tested using kinomap. It is intended that it will also work with other clients that communicate using Bluetooth Low Energy for Cycling Power and Speed.

<p align="center">Garmin Fenix8 v13.12 (client) <--BLE-CPS--> Gymlink (server/client) <-- Keiser M3i bike (beacon)</p>

<p align="center">kinomap ios18.2 (client) <--BLE-CPS--> Gymlink (server/client) <-- Keiser M3i bike (beacon)</p>


Note 1: There is a Keiser Bluetooth accessory that receives the broadcast packets and converts them into BLE CPS, Heart Rate Service, and Cycling Speed and Cadence Service data. While accessory supports the BLE specification, but the Fenix8 seems to only recognize one service per MAC ID, resulting in only receiving the accessory heart rate data. The Keiser accessory heart rate data is a rebroadcast of data received by the Keiser beacon from an ANT+ sensor like a Polar H10. The author has heard that the Keiser accessory pairs and works with Peloton.

Note 2: Gymnasticon is another project that provides the same functionality using NODE.js. https://github.com/ptx2/gymnasticon. The Gymnasticon image itself worked for me on a RPI Zero W but only if the RPi was never updated since it relies on specific, and very old, libraries. I don't speak NODE.js so could not compile/update it. It does not seem to be maintained.

Note 3: A commercial product k2pi is available but was not tested https://k2pi.company.site/

Note4: Apparently newer Keiser M3i bikes have BLE server capability built-in (mine was circa 2020).

## Installation
1) Start with a headless RPi installation using RPi Imager. (Google how to do this if new to RPis)
2) export GYMLINK_ROOT=\<insert directory here\
3) mkdir -p ${GYMLINK_ROOT}
4) cd ${GYLMINK_ROOT}
5) git clone https://github.com/abqjln/gymlink

So I remember what is going on, I created a script to install the required packages. I use sudo. You might want to inspect the script before running it. It will install bluez v5.79 (takes a while), change default bluez configuration, change some kernel bluetooth configuration, and clone bluez_inc.

5) chmod 755 ${GYMLINK_ROOT}/gymlink/k2cps/k2cps_install.sh
6) chmod 755 ${GYMLINK_ROOT}/gymlink/scripts/*.sh
7) sudo -E ${GYMLINK_ROOT}/gymlink/k2cps/k2cps_install.sh
8) cd ${GYMLINK_ROOT}/gymlink/k2cps
9) make
10) \${GYMLINK_ROOT}/gymlink/${HOSTNAME}/k2cps

The BLE signal from the M3i is not particularly strong, so place the RPi close to the M3i head unit (and your watch).

k2cps will configure the BT adapter, and begin advertising the connectable BLE-CPS service using the RPi hostname.

k2cps will scan for the Keiser M3i advertisement, convert the data into CPS format, and serve it to the client.

## Garmin Fenix 8 as Client

Once k2cps is running, put the watch in sensor pairing mode and when k2cps is detected the watch will show a PWR-[xx:xx:xx:xx:xx] the last 5 values in the MAC address in reverse order. The Fenix8 does not make use of the k2cps advertised name (hostname or bluez adapter name), but will display ${HOSTNAME}-CPS as the model in the About section. If you like, after pairing you can rename the sensor on the watch. Other clients may auto-populate with the advertised name.

When starting an activity that uses CPS, the Fenix8 will connect, enable indications on the CPS-CyclingPowerControl characteristic <1818:2a66>, read the DIS <0x0180a:xxxx> values, enable notifications on the Measurement Characteristic (CPS-MC) <1818:2a63>, and begin sending notifications at 500ms intervals. The Fenix8 will show zeros for the cadence and power until the CPS data source, in this case the M3i, starts sending.<br/>

The Fenix8 not enable BatteryService-BatteryLevelCharacteristic <xxx;xxx. notifications, however it does read the value every 30 seconds, likely as a keep-alive function.<br/>

For simulation purposes, k2cps sends a speed value that is derived from a specified gear ratio in k2cps and tire diameter specified on the Fenix 8. I used a gear ratio of 2:1 and 2096 mm, respectively.

After about 30 seconds of non-use, the Fenix8 will disconnect.

One caveat in testing with the Fenix 8. In early versions I also transmitted a simulated heart rate. If you test using an Activity, even if you delete the Activity, the Active Minutes remain. I chatted with Garmin support about this and there is no way of deleting the Active Minutes associated with a deleted Activity. So you might want to disconnect if not specifically testing if your heart races while sitting at a keyboard.

I sometimes observe dropouts across power, speed, and cadence, indicating a connection error, roughly three times (need ~10s to reconnect) in a 24 minute period. Still debugging.

I also observe odd dropouts only in speed, while cadence and power are fine. I suspect a Garmin BLE stack issue. Since speed is simulated with a stationary bike under CPS, not too worried. The Garmin stack changes frequently and without notice.

## Keiser M3i Notes
The Keiser M3i bike itself broadcasts a nonconnectable BLE advertisement containing the bike data. The specification is here  https://dev.keiser.com/mseries/direct/#data-parse-example

Gymlink includes a bash simulator `M3i_beacon.sh` derived from 
https://github.com/KeiserCorp/Keiser.M3i.BLE-HCI-Simulator/blob/master/M3i_Sim.sh

Upon stopping a workout, the bike beacon has an annoying behavior of sending the last non-zero data for over a minute until it times out; k2cps will detect this and send zeros, so you won't get credit for spinning that extra minute.<br/>

`M3i_beacon.sh` is an improved M3i simulator that can be run on a different RPi for testing.

# Next up
The gl library contains other BLE services but with much cruft and only partially working.

## BLE FTMS treadmill to improve cadence and speed measurements

## BLE FTMS indoor bike for M3i
(actually more appropriate than CPS, but couldn't get Garmin to recognize)

## If I had a BLE rowing machine

