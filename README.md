Gymlink is a C implementation of Bluetooth Low Energy (BLE) communication between fitness peripherals/servers and centrals/clients using Raspberry Pis. Specific applications are being developed as the author has devices.

Gymlink has been intentionally pedantic in style and commenting to provide a learning tool. The author knew nothing about BLE or fitness machine communication prior to summer of 2024, and quite likely will forget them soon, so an attempt was made to write everything down. The intent is to help others learn and the author remember--it should not be this difficult! This code was enabled by the excellent C library bluez_inc from @weliem ([bluez_inc](https://github.com/weliem/bluez_inc)), which provides a C library to abstract from the dBus-based ([bluez](https://github.com/bluez/bluez))

I found other clues scattered around the not-well-documented world of BLE fitness included the Pelomon project [https://github.com/ihaque/pelomon](url).

As will become evident, the author has little experience using GitHub, and @weliem in particular has been quite kind in correcting errors. The best evidence of usefulness is continuing critiques and corrections; please let me know if anything could be corrected or improved.

# k2cps
Converts the propietary, pubished BLE broadcast (not connectable) BLE beacon advertising data from a Keiser M3i spin bike into the (connectable) BLE Cycling Power Service. Testing has been done on Garmin Fenix8 watch with software ranging from 12.19 through the 13.12 beta. It was also tested using kinomap. It is intended that it will also work with other clients that communicate using Bluetooth Low Energy for Cycling Power and Speed.


<p align="center">Garmin Fenix8 v13.12 (client) <--BLE-CPS--> Gymlink (server/client) <-- Keiser M3i bike (beacon)</p>

<p align="center">kinomap ios18.2 (client) <--BLE-CPS--> Gymlink (server/client) <-- Keiser M3i bike (beacon)</p>


Note 1: There is a Keiser Bluetooth accessory that receives the broadcast packets and converts them into BLE CPS, Heart Rate Service, and Cycling Speed and Cadence Service data. While accessory supports the BLE specification, but the Fenix8 seems to only recognize one service per MAC ID, resulting in only receiving the accessory heart rate data. The Keiser accessory heart rate data is a rebroadcast of data received by the Keiser beacon from an like a Polar H10, so it is better anyway to pair the heart rate monitor directly to the watch rather than going through the beacon->accessory_->watch chain.

Note 2: The Gymnasticon image worked for me but only if the RPi was never updated since it relies on specific, and very old, libraries. I don't speak NODE.js so could not cmake it. It does not seem to be maintained. https://github.com/ptx2/gymnasticon

Note 3: https://k2pi.company.site/ is a proprietary commercial product, but was not tested.

## Installation
1) Start with a headless RPi installation using RPi Imager. (Google how to do this if new to RPis)
2) (Optional?) Install latest bluez (see note below)
3) (Optional?) Set communication specs to meet Apple recommentations
4) Clone bluez_inc (do not need to cmake)
5) Clone gymlink
6) cd <gymlink/k2cps dir>
7) make
8) ./k2cps

k2cps will configure the BT adapter, and begin advertising the connectable BLE-CPS service.

k2cps will scan for the Keiser M3i advertisement, convert the data into CPS format, and serve it to the client.

# Keiser M3i Notes
The Keiser M3i bike itself broadcasts a nonconnectable BLE advertisement containing the bike data. The format is here  [https://dev.keiser.com/mseries/direct/#data-parse-example](url)

The BLE signal from the M3i is not particularly strong, so place the RPi close by.

Upon stopping a workout, the bike beacon has an annoying behavior of sending the last non-zero data for over a minute until it times out; Gymlink will detect this and send zeros, so you won't get credit for spinning that extra minute.<br/>

# Garmin Fenix8 Notes

## Cycling Power Sensor
Once Gymlink is running, put the watch in sensor pairing mode and when Gymlink is detected the watch will show a PWR-[xx:xx:xx:xx:xx], i.e., the last 5 values in the MAC address in reverse order. The Fenix8 does not make use of the Gymlink advertised name (or bluez adapter name), but will The watch will display other Gymlink information in the About section. If you like, after pairing you can rename the sensor on the watch. Other clients may auto-populate with the advertised name.

When starting an activity that uses CPS, the Fenix8 will connect, enable indications on the CPS-CyclingPowerControl characteristic <1818:2a66>, read the DIS <0x0180a:xxxx> values, enable notifications on the Measurement Characteristic (CPS-MC) <1818:2a63>, and begin sending notifications at 500ms intervals. The Fenix8 will show zeros for the cadence and power until the CPS data source, in this case the M3i, starts sending.<br/>

The Fenix8 not enable BatteryService-BatteryLevelCharacteristic <xxx;xxx. notifications, however it does read the value every 30 seconds, likely as a keep-alive function.<br/>

After about 30 seconds of non-use, the Fenix8 will disconnect.

## Heart Rate Sensor:
Any external Heart Rate Sensor (HRS) service will override the integrated watch heart rate sensor. The watch will connect to an external HRS in the Sensors menu, but will timeout in ~30s if not used in a workout, then disconnect and the Garmin will return to using the integrated sensor.

## bluetooth stack<br/>
I found connecting with the Fenix8 stack challenging as the software does odd things.

Used the kitchen-sink method of reverse engineering Garmin--enable all features and see what it asks for. Set the CPS Feature Characteristic to Distributed System Not Supported since single measurement for power. The Fenix8 ignored where the sensor location was set. (while kinomap properly doubles the value if set to "left crank" and does not double when set to "spider"

Set BLE speed: Connectivity->phone, then press light 5 times (not fast enough to turn on flashlight). Will give BLE menu to set connection speed. [https://www.reddit.com/r/GarminFenix/comments/1ac6kne/bluetooth_keeps_disconnecting/](url)

Seems to only use one service per MAC address; multi-service adapters, such as the Keiser adapter above, will only have one service recognized.

(different topic) The NPE Runn treadmill adapter provides an ANT+ footpod and a BLE Fitness Machine Service (FTMS) with the advertising flags properly set to specify treadmill present. Unfortunately, the Fenix 8 detects only the FTMS and improperly reads it as a "Trainer", improperly configuring it as an indoor bike trainer.


# bluez Notes

## Installation
The RPi OS distro did not include the latest bluez version. I always just updated to the latest latest version--currently 5.79. I could not get the bluez README to work, and eventually developed `install_bluez.sh` that does the job.

Gymlink was tested with Edimax BT-8500 bluetooth USB adapters in addition to the RPi onboard adapter. There were some issues with the RPi5 not auto-detecting these adapters in bluez 5.66. This has apparently been corrected by 5.78.

## Configuration

After any changes to bluez settings, you must reload and restart the daemon<br/>

`sudo systemctl daemon-reload && sudo systemctl restart bluetooth`

## bluetooth.service
bluez installs the battery plugin by default, resulting in each server having their battery notification state to 'true' on connecting and 'false' on disconnecting. This results in undesired communication clogging logs and having other effects that may not be supported (links below). Gymlink lets the connecting client control this behavior, so omits this plugin by first finding the file

`systemctl show -p FragmentPath bluetooth`<br/>

and editing the file to add the flag<br/>

`ExecStart=/usr/libexec/bluetooth/bluetoothd -P battery`<br/>

[https://stackoverflow.com/questions/66690927/ios-central-with-bluez-peripheral-disconnect-due-to-insufficient-auth](url)
[https://lore.kernel.org/linux-bluetooth/CAG-pW8Ev_ntv1=o0ndd_2FXTdbsvHJ=FVsB-YXUucVtyKSO0Sg@mail.gmail.com/t/](url)


## Using kernel configuration files /sys/kernel/debug/bluetooth/hcix/*
Gymlink focused on a gym environment with fixed machines and wall power using RPis, so the author used the most aggressive, reliable, fast, and power-hungry radio configurations. The central targets were Garmin Fenix8 and iOS devices. These both would provide the bridges to WiFi and cloud-based enterprises. The Apple communication recommendations were chosen performance. Because of the issues noted below with main.conf, the configuration settings were also implemented in<br/>

`bluez_ios.sh`


### Using /etc/bluetooth/main.conf

bluez main.conf does not restore default kernel settings once they have been modified, e.g., commenting out a previously uncommented setting does not restore the original. Once you uncomment and change a value, commenting it out will have no effect; you must explicitly change it back to the setting you like (even if default) This has been reported. [https://github.com/bluez/bluez/issues/1039](url)


`main.conf` comments & edits<br\>

#DeviceID = bluetooth:1234:5678:abcd <br/>
The default install publishes the GATT Device Information Service (DIS) using internally defined parameters. Because Gymlink controls those values, to avoid DIS UUID <0x180a> read failures one must disable this by changing this line to <br/>
DeviceID = false

Optional notes <br/>
#Name = BlueZ <br/>
This is true only if -P hostname is used when starting the bluetooth.service to exclude the hostname plugin. Otherwise bluez will name adapters "hostname" for last adapter installed, and hostname, hostname #1, hostname #2, ... for additional adapters. It does this in reverse order, meaning that for a Raspberry Pi for one USB adapter installed, the on-board controller is becomes hostname #1, and the USB adapter becomes hostname. [https://github.com/bluez/bluez/blob/master/plugins/hostname.c](url)

#Class = 0x000100 <br/>
This implements the device class flags implemented per https://www.ampedrftech.com/cod.htm The BLE spec provides for Information, Health, Undefined as 0x800900, but hciconfig hcix class 0x800900 gives an Invalid Device Class error. So this was set Service=Information, Major=Miscellaneous, Minor=None or <br/>
Class = 0x800000

There is a lot of unnecessary BLE communication checking for service classes related to the installed protocols. Class is said to determine power, but from btmon, bluez picks this up and looks for characteristics associated with the class, leading to many transactions that return unknown. On bluez 5.78 on RPi seems to default to 0x6c0000 Rendering, Capturing, Audio, Telephony and all that extra RF traffic contributes to dropouts. One can presumably disable plugin protocols using the -P flag on starting bluetooth.service, but I never saw any performance difference. I tried to disable everything on this list [https://en.wikipedia.org/wiki/List_of_Bluetooth_profiles](url) using <br\>

```ExecStart=/usr/libexec/bluetooth/bluetoothd -P battery -P battery -P avrcp -P a2dp -P bap -P hfp -P pbap -P bap -P hsp -P gavdp -Pvdp -Pvcp -P hdp -P map -P pbap -P vdp -P tap```<br/>

but only the battery flag seemed to work. It seems protocols can be installed by other software independent of these bluez configuration settings? I gave up trying.

https://raspberrypi.stackexchange.com/questions/85960/unable-to-change-bluetooth-service-class
















# Bluetooth Low Energy (BLE) Discussion
The Bluetooth spec documentation does exist online but is (unnecessarily) painfully difficult to pull together. I am not sure why. It seems the standards committee changed their approach before I started this. Here's what I understand.

## Central-Peripheral Roles
Central and Peripheral are connect roles defined based on BLE connectivity
Central--scans for devices and initiates outgoing connection request
Peripheral--advertises and accepts incoming connection request
Agents manage pairing both client and server
Bluetooth 4.1 allows dual-topology and multi-central devices simultaneously
https://docs.silabs.com/bluetooth/7.2.0/bluetooth-fundamentals-connections/

## Client-Server Roles
Once paired, the GATT Client and GATT Server are data roles defined based on storage and flow of data
Clients access remote resources over BLE using GATT. Servers have local database to provide resources to remote client.
https://docs.silabs.com/bluetooth/7.2.0/bluetooth-gatt/gatt-server-client-roles Commonly the central acts as a client and the peripheral acts as a server, but this is not necessary.

## Specifications
The various Bluetooth Low Energy / Bluetooth Smart / BLE services can be found here
[https://www.bluetooth.com/specifications/specs/](url)


Unfortunately the implementation details are not in the same document. The GATT services and characteristics complete with read/write/notify/indicate requirements and data types, formats, bit fields are in

[https://github.com/oesmith/gatt-xml](url)


## Organizationally Unique Identifier (OUI) and Company Assigned Information (CAI)
Companies can apply for their own organizationally unique identifier (OUI) consisting of 6 unique hex values. These values form the first part of a BLE adapter MAC address. The last 6 values are assigned by the organization and form the Company Assigned Information (CAI), which makes up the second part of the MAC. Most of the time, the Device_Information_Service->System_ID_Characteristic is made up of these two values with a 0xFFEE padded in between to give the full 64 bits.

If the OUI is AB:CD:EF and the CAI is UV:WX:YZ,

BT Adapter MAC : AB:CD:EF:UV:WX:YZ
DIS->SystemID: AB:CD:EF:FF:EE:UV:WX:YZ


## Cycling Power Service (CPS)
Gymlink was developed using BLE/Cycling Power Service v1.1.1.
[https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=585857](url)

gl_ble_cps.c/h contains the characteristic information. This is certainly redundant with libraries all over.
The cadence and speed values are based on counters and timers that would be easiest to implement on a battery-operated device. However this meant more work in converting speed or cadence to wheel/crank revolutions and wheel/crank times.


## Fitness Machine Service (FTMS)
## Running Speed and Cadence Service (RSCS)
## Cycling Speed and Cadence Service (CSCS)
## Heart Rate Service (HRS)


# bluez_inc Notes
An incredibly useful debugging technique was to modify the library to have each function name reported in the logs, versus the default source file name. To accomplish this, Gymlink compiles this library from source. In the binc directory, run <br/>
```sed -i '/static const char \*const TAG/c\#define TAG __func__'*.c```


If you pass a GByteArray pointer into a binc function, the function will free the GByteArray when it is finished. Freeing it yourself will result in improper behavior.

bluez will automatically creates Client Characteristic Change Descriptors (CCCD) when Notify or Indicate are specified.
Do not create them separately.



# Additional things the author poorly understands and possibly incorrect

For multiple advertisements on single adapter, need unique DBus object path
https://stackoverflow.com/questions/63661259/how-to-have-2-advertisements-in-blebluetooth-low-energy

One advertisement per dBus connection
Multiple dBus connections per adapter??

Use addresses to find identities--they do not change while advertised name, adapter name seem to get mixed up by bluez.


https://punchthrough.com/creating-a-ble-peripheral-with-bluez/
DBus communications bus
adapters are objects--you can act on them
/org/bluez/hci0 is a remote bluez object meaning it is managed by bluez
proxy objects are local reference to the remote bluez object
Adapter object implements interfaces (dot form) methods, signals, and properties
		, e.g. org.bluez.Adapter1, org.freedesktop.DBus.Properties, org.bluez.GattManager1, org.bluez.LEAdvertisingManager1




//**********************************************************************






MAC address of ethernet d8:3a:dd:47:f5:54
MAC of Wifi d8:3a:dd:47:f5:55
MAC of BT  D8:3A:DD:47:F5:57

Bt Management API
https://github.com/bluez/bluez/blob/5b6ffe0381e80dcc980f0e73ff0621c2a6033be3/doc/mgmt-api.txt


+ 1 = MAC of Wifi
MAC Wifi + 2 = MAC bluetooth



Garmin supports shoe-base pods to measure cadence, speed, and stride, and combines with GPS data from watch. They can get
elevation changes from GPS. Prehaps this is why they do not implement Fitness Machine service with treadmill.

Garmin implements Running Speed and Cadence service <00001814>. Upon connecting it reads the
Feature characteristic.


instantaneous speed--determined by counting belt movement
instantaneous cadence--detemined by accelerometer detecting pulses due to footfalls?
Don't need to include stride length or total distance features
Still does not do elevation changes for incline.



## bluez Notes


bluez uses /var/bluetooth/lib/bluetooth to store adapter info
`sudo ls /var/bluetooth/lib/bluetooth` view connected devices by MAC address
`sudo rm -rf /var/bluetooth/lib/bluetooth` to clean state

pi:bluetoothctl list
shows device addresses and names
pi: hcitool dev
shows device paths and addresses (hcitool is deprecated but so useful it's not going away soon)

btmon -p debug to generate logs of what bluez is doing



bluez defaults to naming adapters <hostname>, then external adapters are named <hostname #1>, then <hostname #2>
Change the base name with cat PRETTY_HOSTNAME=<newname> >> /etc/machine-info



bluez discovery filters do
https://git.kernel.org/pub/scm/bluetooth/bluez.git/tree/doc/adapter-api.txt?h=5.58#n49







sudo btmon -w /mnt/code/gymlink/filename

hciconfig hci0 class

List running services
sudo systemctl status


sudo apt purge pulseaudio
sudo apt purge pipewire




sudo apt purge pulseaudio-utils // No effect
sudo systemctl daemon-reload && sudo systemctl restart bluetooth


sudo apt purge pipewire
sudo apt purge pipewire-bin
sudo apt purge pipewire-libcamera
sudo apt purge libpipewire-0.3-*
No effect

No endpoints registered in bluez
bluetootctl menu endpoint list produces nothing

bluetoothctl menu endpoint unregister <1850> does not stop it from being seen on scan






Cannot control endpoints registered by pipewire as the latter uses a different D-Bus interface, and bluetoothd won't allow different connections to
control the same transport



Scan rate determined by kernel
https://bbs.archlinux.org/viewtopic.php?id=279872
https://stackoverflow.com/questions/24945620/excessive-bluetooth-le-timeouts-on-linux

Deduplicating scan
https://stackoverflow.com/questions/55336017/disable-filter-duplicates-setting-for-le-set-scan-enable-command
https://github.com/bluez/bluez/issues/406


btmon -p debug -w


Pairing
https://community.nxp.com/t5/Wireless-MCU/Bluetooth-Low-Energy-SMP-Pairing/m-p/376931



one agent per adapter, not per server

# Installation
Install glib
sudo apt install -y libglib2.0-dev

Install bluez (to get latest version)
script

Install bc for beacon simulator

Install bluez_inc

