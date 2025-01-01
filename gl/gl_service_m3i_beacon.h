#ifndef GL_SERVICE_BEACON_M3I_H
#define GL_SERVICE_BEACON_M3I_H

/***********************************************************************
 * The Keiser M3i bike itself functions as manufacturer-specified data advertising beacon
 * Rate:	(0x0237/1600Hz_clock)=0.354375 Seconds - For bikes > 6.30
 *			(0x0C80/1600Hz_clock)=2.0 Seconds - For bikes < 6.30
 *
 * https://dev.keiser.com/mseries/direct/#data-parse-example
 * Keiser M3i advertising Data example
 * 03		09			4D 'M'	33 '3'	|	0x02	0x01	0x04	|	0x14	0xFF			0x0201	0x06	0x30	0x00	0x38	0x3803	0x4605	0x7300	0x0D00	0x04	0x27	0x0100	0x0A
 * Length	Datatype	Category		| 	Length	Dtype  	Flag	|	Plen	MfrSpecData		Co ID	Major	Minor	Dtype	Eq ID	Cadence	HR		Pwr		kCal	Min		Sec		Dist	Gear
 * 			Appearance					|			Flags			|	Length	Dtype
 *
 * Inconsistently sometimes cadence does not go to zero when stopped. Seems to be in the 5xx range.
 *
 *
 * Original bash simulator at https://github.com/KeiserCorp/Keiser.M3i.BLE-HCI-Simulator/blob/master/M3i_Sim.sh
 * *****************
 * The M3i token adapter with battery has these characteristics (a subset of the M3 advertisement above)
 * bluetoothctl
 * scan le
 * info <mac of M3i token>
 *
 * Device E6:14:C4:0E:FA:17 (random)
 * Name: M3i#000
 * Alias: M3i#000
 * Paired: no
 * Bonded: no
 * Trusted: no
 * Blocked: no
 * Connected: no
 * LegacyPairing: no
 * UUID: Device Information        (0000180a-0000-1000-8000-00805f9b34fb)
 * UUID: Cycling Power             (00001818-0000-1000-8000-00805f9b34fb)
 * UUID: Cycling Speed and Cadence (00001816-0000-1000-8000-00805f9b34fb)
 * UUID: Heart Rate                (0000180d-0000-1000-8000-00805f9b34fb)
*/

#include <stdint.h>

#define M3I_WHEEL_CIRCUMFERENCE_MILES ( (3.14159 * 16.0) / (5280.0*12.0) )
#define M3I_CADENCE_SCALE (1.0/10.0)
#define M3I_DISTANCE_SCALE (1.0/10.0)
#define M3I_BPM_SCALE (1.0/10.0)
#define M3I_CRANK_REV_TO_INCHES (8.0 * (16.0 * 3.14259) + 14) // 1 crank rev = 8 wheel revs of 16" plus 14" = 416.124"
#define M3I_CRANK_REV_TO_KM (M3I_CRANK_REV_TO_INCHES / 39370.)
#define M3I_GEAR_RATIO (8. + 14./(16*3.14159)) // 8.27 Way too high for any real bike

//**********************************************************************
// M3 beacon manufacturer specific data structure
struct m3i_mfr_data_struct{
	uint8_t major;
	uint8_t minor;
	uint8_t live;
	uint8_t bikeid;
	uint16_t cadence; // resolution 0.1
	uint16_t bpm; // resolution 0.1
	uint16_t watts;
	uint16_t kCal;
	uint8_t min;
	uint8_t sec;
	uint16_t distance; // miles or km bike setting, resolution 0.1
	uint8_t gear;
	uint8_t compiler_padding;
};

union m3i_mfr_data_union{
	struct m3i_mfr_data_struct values;
	unsigned char bytes[sizeof(struct m3i_mfr_data_struct)];
};


//**********************************************************************
// M3i data structure
struct m3i_struct{
	union m3i_mfr_data_union data;
	uint16_t updated;
};


//**********************************************************************
int gl_service_m3i_beacon_to_struct( Device *server, struct m3i_struct *p_m3i );

#endif
