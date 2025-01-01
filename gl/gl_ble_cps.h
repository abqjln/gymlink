#ifndef GL_BLE_CPS_H
#define GL_BLE_CPS_H

#include <stdint.h>

#include "application.h"

extern int gl_cps_mc_simulate; // Simulate data flag


//**********************************************************************
// CYCLING POWER SERVICE (CPS)
#define CYCLING_POWER_SERVICE_UUID "00001818-0000-1000-8000-00805f9b34fb"

//**********
// Measurement characteristic
#define CPS_MC_UUID	"00002a63-0000-1000-8000-00805f9b34fb"
// Mandatory, Notify, LE Broadcast optional
struct cps_mc_struct{
	uint16_t flags;
	int16_t instantaneous_power; // Watts (why is this signed in spec?)
	uint8_t pedal_power_balance; // 1/2 %
	uint16_t accumulated_torque; // 1/32 N*m
	uint32_t cumulative_wheel_revs; // revs
	uint16_t last_wheel_event_time; // 1/2048 seconds
	uint16_t cumulative_crank_revs; // revs
	uint16_t last_crank_event_time; // 1/1024 seconds
	uint16_t maximum_force_magnitude; // N
	uint16_t minimum_force_magnitude; // N
	uint16_t maximum_torque_magnitude; // 1/32 N*m
	uint16_t minimum_torque_magnitude; // 1/32 N*m
	uint8_t max_min_angle[3]; // degrees 2x12bits
	uint16_t top_dead_spot_angle; // degrees
	uint16_t bottom_dead_spot_angle; // degrees
	uint16_t accumulated_energy; // kJ
	// Not in GATT spec, but save what we sent to client
	uint16_t prev_wheel_event_time;
	uint32_t prev_cumulative_wheel_revs;
	uint16_t prev_crank_event_time;
	uint16_t prev_cumulative_crank_revs;
	// Not in GATT spec but easier to use
	double wheel_cadence; // wheel revs/min
	double gear_ratio; // Send notify--crank to wheel revs (client sets tire circumference)
	double tire_circumference_km; // Receive notify wheel revs to km
	uint16_t cadence; // crank revs/min
	unsigned int notify_ms;
};
//#define instantaneous_power always present
#define CPS_MC_PEDAL_POWER_BAL_PRESENT			(0x0001)
#define CPS_MC_PEDAL_POWER_BAL_REFERENCE		(0x0002) // 0=unknown, 1=left
#define CPS_MC_ACCUMULATED_TORQUE_PRESENT		(0x0004)
#define CPS_MC_ACCUMULATED_TORQUE_SOURCE		(0x0008) // 0=wheel, 1=crank
#define CPS_MC_WHEEL_REVOLUTION_DATA_PRESENT	(0x0010)
#define CPS_MC_CRANK_REVOLUTION_DATA_PRESENT	(0x0020)
#define CPS_MC_EXTREME_FORCE_MAGNITUDES_PRESENT	(0x0040)
#define CPS_MC_EXTREME_TORQUE_MAGNITUDES_PRESENT (0x0080)
#define CPS_MC_EXTREME_ANGLES_PRESENT			(0x0100)
#define CPS_MC_TOP_DEAD_SPOT_ANGLE_PRESENT		(0x0200)
#define CPS_MC_BOTTOM_DEAD_SPOT_ANGLE_PRESENT	(0x0400)
#define CPS_MC_ACCUMULATED_ENERGY_PRESENT		(0x0800)
#define CPS_MC_OFFSET_COMPENSATION_INDICATOR	(0x1000)
#define CPS_MC_WHEEL_EVENT_TIME_RESOLUTION		(2048)
#define CPS_MC_CRANK_EVENT_TIME_RESOLUTION		(1024)


//**********
// Feature characteristic
#define CPS_FC_UUID		"00002a65-0000-1000-8000-00805f9b34fb"
// Mandatory, Read, Indicate if can change over lifetime
struct cps_fc_struct{
	uint32_t flags;
};
#define CPS_FC_PEDAL_POWER_BALANCE_SUPPORTED				0x00000001
#define CPS_FC_ACCUMULATED_TORQUE_SUPPORTED					0x00000002
#define CPS_FC_WHEEL_REVOLUTION_DATA_SUPPORTED				0x00000004
#define CPS_FC_CRANK_REVOLUTION_DATA_SUPPORTED				0x00000008
#define CPS_FC_EXTREME_MAGNITUDES_SUPPORTED					0x00000010
#define CPS_FC_EXTREME_ANGLES_SUPPORTED						0x00000020
#define CPS_FC_TOP_AND_BOTTOM_DEAD_SPOT_ANGLES_SUPPORTED	0x00000040
#define CPS_FC_ACCUMULATED_ENERGY_SUPPORTED					0x00000080
#define CPS_FC_OFFSET_COMPENSATION_INDICATOR_SUPPORTED		0x00000100
#define CPS_FC_OFFSET_COMPENSATION_SUPPORTED				0x00000200
#define CPS_FC_CPS_CPMC_CONTENT_MASKING_SUPPORTED			0x00000400
#define CPS_FC_MULTIPLE_SENSOR_LOCATIONS_SUPPORTED			0x00000800
#define CPS_FC_CRANK_LENGTH_ADJUSTMENT_SUPPORTED			0x00001000
#define CPS_FC_CHAIN_LENGTH_ADJUSTMENT_SUPPORTED			0x00002000
#define CPS_FC_CHAIN_WEIGHT_ADJUSTMENT_SUPPORTED			0x00004000
#define CPS_FC_SPAN_LENGTH_ADJUSTMENT_SUPPORTED				0x00008000
#define CPS_FC_SENSOR_MEASUREMENT_CONTEXT					0x00010000
#define CPS_FC_INSTANTANEOUS_MEASUREMENT_DIRECTION_SUPPORTED 0x00020000
#define CPS_FC_FACTORY_CALIBRATION_DATE_SUPPORTED			0x00040000
#define CPS_FC_ENHANCED_OFFSET_COMPENSATION_SUPPORTED		0x00080000
#define CPS_FC_DISTRIBUTED_SYS_SUPPORT_LEGACY_UNSPECIFIED	0x00000000
#define CPS_FC_DISTRIBUTED_SYS_SUPPORT_FALSE				0x00100000
#define CPS_FC_DISTRIBUTED_SYS_SUPPORT_TRUE					0x00200000
#define CPS_FC_DISTRIBUTED_SYS_SUPPORT_RFU					0x00300000

//**********
// Cycling Power Control Point characteristic
#define CPS_CPC_UUID			"00002a66-0000-1000-8000-00805f9b34fb"
// Mandatory if wheel revolution data or multiple sensor location features present
// Write, Indicate
struct cps_cpc_struct{
	uint8_t request_opcode;
	uint32_t cumulative_wheel_revs_value;
	uint8_t sensor_location;
	uint16_t crank_length; // 0.5mm (Fenix8 sets to 345 or 172.5mm)
	uint16_t chain_length; // 1.0mm (Fenix8 sets to 1463 mm)
	uint16_t chain_weight; // 1g
	uint16_t span_length; // 1.0mm
	int16_t offset_compensation; // 1/32 Newton (0xFFFF = Not Available)
	uint16_t mask_mc_content;
	//something_t factory_calibration_date; //??
	uint8_t sampling_rate; // Hz, >= 1 Hz
	uint8_t response_opcode;
	uint8_t response_value;
	uint32_t response_parameter;
	unsigned int notify_ms;
	};
#define CPS_CPC_OPCODE_SET_CUMULATIVE_VALUE		(1)
#define CPS_CPC_OPCODE_UPDATE_SENSOR_LOCATION	(2)
#define CPS_CPC_OPCODE_REQUEST_SENSOR_LOCATION	(3)
#define CPS_CPC_OPCODE_SET_CRANK_LENGTH			(4)
#define CPS_CPC_OPCODE_REQUEST_CRANK_LENGTH		(5)
#define CPS_CPC_OPCODE_SET_CHAIN_LENGTH			(6)
#define CPS_CPC_OPCODE_REQUEST_CHAIN_LENGTH		(7)
#define CPS_CPC_OPCODE_SET_CHAIN_WEIGHT			(8)
#define CPS_CPC_OPCODE_REQUEST_CHAIN_WEIGHT		(9)
#define CPS_CPC_OPCODE_SET_SPAN_LENGTH			(10)
#define CPS_CPS_OPCODE_REQUEST_SPAN_LENGTH		(11)
#define CPS_CPC_OPCODE_START_OFFSET_COMP		(12)
#define CPS_CPC_OPCODE_MASK_CPMCC				(13)
#define CPS_CPC_OPCODE_REQUEST_SAMPLING_RATE	(14)
#define CPS_CPC_OPCODE_REQUEST_FACTORY_CAL_DATE	(15)
#define CPS_CPC_OPCODE_START_ENH_OFFSET_COMP	(16)
#define CPS_CPC_OPCODE_RESPONSE_CODE			(32)
#define CPS_CPC_RESPONSE_SUCCESS				(1)
#define CPS_CPC_RESPONSE_UNSUPPORTED			(2)
#define CPS_CPC_RESPONSE_INVALID_PARAMETER		(3)
#define CPS_CPC_RESPONSE_FAILED					(4)
#define CPS_CPC_NOTIFY_MS (1000) // BLE spec says typically 1 second

//**********
// Sensor location characteristic
#define CPS_SLC_UUID			"00002a5d-0000-1000-8000-00805f9b34fb"
// Mandatory, Read
struct cps_slc_struct{
	uint8_t flags;
};
#define CPS_SLC_OTHER			(0)
#define CPS_SLC_TOP_OF_SHOE		(1)
#define CPS_SLC_IN_SHOE			(2)
#define CPS_SLC_HIP				(3)
#define CPS_SLC_FRONT_WHEEL		(4)
#define CPS_SLC_LEFT_CRANK		(5)
#define CPS_SLC_RIGHT_CRANK		(6)
#define CPS_SLC_LEFT_PEDAL		(7)
#define CPS_SLC_RIGHT_PEDAL		(8)
#define CPS_SLC_FRONT_HUB		(9)
#define CPS_SLC_REAR_DROPOUT		(10)
#define CPS_SLC_CHAINSTAY		(11)
#define CPS_SLC_REAR_WHEEL		(12)
#define CPS_SLC_REAR_HUB			(13)
#define CPS_SLC_CHEST			(14)
#define CPS_SLC_SPIDER			(15)
#define CPS_SLC_CHAIN_RING		(16)


//**********
// Cycling power vector characteristic
#define CPS_CPV_UUID			"00002a64-0000-1000-8000-00805f9b34fb"
// Optional, notify
struct cps_cpv_struct{
	uint8_t flags;
	uint16_t cumulative_crank_revs;
	uint16_t last_crank_event_time;
	uint16_t first_crank_measurement_angle;
	int16_t instantaneous_force_magnitude_array;
	int16_t instantaneous_torque_magnitude_array;
};

#define CPS_CPV_CRANK_REVOLUTION_DATA_PRESENT			(0x01)
#define CPS_CPV_FIRST_CRANK_MEASUREMENT_ANGLE_PRESENT	(0x02)
#define CPS_CPV_INSTANTANEOUS_FORCE_MAG_ARRAY_PRESENT	(0x04)
#define CPS_CPV_INSTANTANEOUS_TORQUE_MAG_ARRAY_PRESENT	(0x08)
#define CPS_CPV_INSTANTANEOUS_MEASUREMENT_DIRECTION		(0x10)

//**********
// CPS service structure
struct cps_struct{
	struct cps_mc_struct mc;
	struct cps_fc_struct fc;
	struct cps_slc_struct slc;
	struct cps_cpc_struct cpc;
	struct cps_cpv_struct cpv;
};


//**********************************************************************
int gl_ble_cps_init( Application *application, struct cps_struct *p_cps );
void gl_ble_cps_free( struct cps_struct *p_cps );

GByteArray* gl_ble_cps_mc_struct_to_gba( struct cps_mc_struct *p_mc );
gboolean gl_ble_cps_mc_send_notify( gpointer gptr );

int gl_ble_cps_mc_receive_notify( const GByteArray *byteArray,  struct cps_mc_struct *p_mc );
int gl_ble_cps_mc_gba_to_struct( const GByteArray *byteArray, struct cps_mc_struct *p_mc );

//GByteArray* gl_ble_cps_cpc_struct_to_gba( struct cps_cpc_struct *p_cpc );
int gl_ble_cps_cpc_process_indicate( const GByteArray *opcodeArray, void *gptr );

int gl_ble_cps_mc_update( struct cps_mc_struct *p_mc );

#endif
