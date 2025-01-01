#ifndef GL_BLE_CSCS_H
#define GL_BLE_CSCS_H

#include <stdint.h>

#include "application.h"

#include "gl_service_simvals.h"


//**********************************************************************
extern int gl_cscs_mc_source;


// CYCLING SPEED AND CADENCE SERVICE
#define CYCLING_SPEED_AND_CADENCE_SERVICE_UUID				"00001816-0000-1000-8000-00805f9b34fb"

//**********************************************************************
// Mandatory, Notify,
#define CSCS_MC_UUID		"00002a5b-0000-1000-8000-00805f9b34fb"
// Mandatory, Notify, CCCD auto-generated
struct cscs_mc_struct{
	uint8_t flags;
	uint32_t cumulative_wheel_revs; // revolutions
	uint16_t last_wheel_event_time; // 1/1024 sec
	uint16_t cumulative_crank_revs; // revolutions
	uint16_t last_crank_event_time; // 1/1024 sec
	// Not in GATT spec, but save what we sent to client
	uint16_t prev_wheel_event_time;
	uint32_t prev_cumulative_wheel_revs;
	uint16_t prev_crank_event_time;
	uint16_t prev_cumulative_crank_revs;
	// Not in GATT spec, but our simulated calculations
	double dspeed;
	double dcadence;
	unsigned int notify_ms;
};
#define CSCS_MC_WHEEL_REVS_PRESENT			(0x01)
#define CSCS_MC_CRANK_REVS_PRESENT			(0x02)
#define CSCS_MC_WHEEL_EVENT_TIME_RESOLUTION	(1024)
#define CSCS_MC_CRANK_EVENT_TIME_RESOLUTION	(1024)


//**********************************************************************
#define CSCS_FC_UUID			"00002a5c-0000-1000-8000-00805f9b34fb"
// Mandatory, Read
struct cscs_fc_struct{
	uint16_t flags;
};
#define CSCS_FC_WHEEL_REVS_SUPPORTED		0x01
#define CSCS_FC_CRANK_REVS_SUPPORTED		0x02
#define CSCS_FC_MULTISENSOR_LOC_SUPPORTED	0x04


//**********************************************************************
#define CSCS_SLC_UUID			"00002a5d-0000-1000-8000-00805f9b34fb"
// Mandatory if multiple sensor locations supported, Read
struct cscs_slc_struct{
	uint8_t flags;
};
#define CSCS_SLC_OTHER			(0)
#define CSCS_SLC_TOP_OF_SHOE	(1)
#define CSCS_SLC_IN_SHOE		(2)
#define CSCS_SLC_HIP			(3)
#define CSCS_SLC_FRONT_WHEEL	(4)
#define CSCS_SLC_LEFT_CRANK		(5)
#define CSCS_SLC_RIGHT_CRANK	(6)
#define CSCS_SLC_LEFT_PEDAL		(7)
#define CSCS_SLC_RIGHT_PEDAL	(8)
#define CSCS_SLC_FRONT_HUB		(9)
#define CSCS_SLC_REAR_DROPOUT	(10)
#define CSCS_SLC_CHAINSTAY		(11)
#define CSCS_SLC_REAR_WHEEL		(12)
#define CSCS_SLC_REAR_HUB		(13)
#define CSCS_SLC_CHEST			(14)
#define CSCS_SLC_SPIDER			(15)
#define CSCS_SLC_CHAIN_RING		(16)


//**********************************************************************
#define CSCS_CPC_UUID	"00002a55-0000-1000-8000-00805f9b34fb"
// Write, Indicate
// Mandatory if wheel revolution data or multiple sensor locations, Write, Indicate
struct cscs_cpc_struct{
	uint8_t opcode;
	uint32_t cumulative_value;
	uint8_t sensor_location;
	uint8_t request_op_code;
	uint8_t response_value;
	uint32_t response_parameter; // variable length to allow for multiple sensor locations (set to 4 bytes here)
	unsigned int notify_ms;
};
#define CSCS_CPC_OPCODE_RESERVED				(0x00)
#define CSCS_CPC_OPCODE_SET_CUMULATIVE_VALUE	(0x01)
#define CSCS_CPC_OPCODE_START_CALIBRATION		(0x02) // Not used in the version of ble spec
#define CSCS_CPC_OPCODE_UPDATE_SENSOR_LOCATION	(0x03)
#define CSCS_CPC_OPCODE_REQUEST_SENSOR_LOCATION	(0x04)
#define CSCS_CPC_OPCODE_RESPONSE_CODE			(0x10)
#define CSCS_CPC_RESPONSE_SUCCESS				(1) // Sensor location returns lis in response_parameter
#define CSCS_CPC_RESPONSE_UNSUPPORTED			(2)
#define CSCS_CPC_RESPONSE_INVALID_PARAMETER		(3)
#define CSCS_CPC_RESPONSE_FAILED				(4)


//**********************************************************************
// CSCS service structure
struct cscs_struct{
		struct cscs_mc_struct mc;
		struct cscs_fc_struct fc;
		struct cscs_slc_struct slc;
		struct cscs_cpc_struct cpc;
};


//**********************************************************************
int gl_ble_cscs_init( Application *application, struct cscs_struct *p_cscs );
void gl_ble_cscs_free( struct cscs_struct *p_cscs );

GByteArray* gl_ble_cscs_mc_struct_to_gba( struct cscs_mc_struct *p_mc );
gboolean gl_ble_cscs_mc_send_notify( gpointer gptr );

int gl_ble_cscs_mc_receive_notify( const GByteArray *byteArray,  struct cscs_mc_struct *p_mc );
int gl_ble_cscs_mc_gba_to_struct( const GByteArray *byteArray, struct cscs_mc_struct *p_mc );

GByteArray* gl_ble_cscs_cpc_struct_to_gba( struct cscs_cpc_struct *p_cpc );
int gl_ble_cscs_cpc_send_indicate( void *p_gl_server ); // Need to change to GL_Server
int gl_ble_cscs_cpc_gba_process( const GByteArray *byteArray, struct cscs_struct *p_cscs );

void gl_ble_cscs_mc_update( struct cscs_mc_struct *p_mc );

#endif
