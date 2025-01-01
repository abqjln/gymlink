#ifndef GL_BLE_RSCS_H
#define GL_BLE_RSCS_H

#include <stdint.h>

#include "application.h"

#include "gl_service_simvals.h"


//**********************************************************************
// RUNNING SPEED AND CADENCE SERVICE
#define RUNNING_SPEED_AND_CADENCE_SERVICE_UUID				"00001814-0000-1000-8000-00805f9b34fb"

//*********
// Mandatory, notify, CCCD auto-generated
#define RSCS_MC_UUID		"00002a53-0000-1000-8000-00805f9b34fb"
struct rscs_mc_struct{
	uint8_t flags;
	uint16_t instantaneous_speed; // 1/256 m/s.
	uint8_t instantaneous_cadence; // Stride cadence (step cadence is 2x this) per min
	uint16_t instantaneous_stride_length; // 1/100 m
	uint32_t total_distance; // 1/10 m
	unsigned int notify_ms;
};
#define RSCS_MC_INSTANTANEOUS_STRIDE_LENGTH_PRESENT	(0x01)
#define RSCS_MC_TOTAL_DISTANCE_PRESENT				(0x02)
#define RSCS_MC_WALKING_RUNNING_STATUS				(0x04) // 0 walking, 1 running
#define RSCS_MC_SPEED_SCALE_MPS (1.0/256.0) // Units 1/256 m/s
#define RSCS_MC_SPEED_SCALE_MPH (RSCS_MC_SPEED_SCALE_MPS * (3600.0/1.0) * (1.0/1609.34))
#define RSCS_MC_CADENCE_SCALE (1) // Units 1/min
#define RSCS_MC_STRIDE_LENGTH_SCALE (100) // 1/100 m
#define RSCS_MC_DISTANCE_SCALE_M (1.0/10.0) // 1/10 m
#define RSCS_MC_DISTANCE_SCALE_MILES (RSCS_MC_DISTANCE_SCALE_M * (1.0/1609.34))

//*********
// Mandatory, read
#define RSCS_FC_UUID		"00002a54-0000-1000-8000-00805f9b34fb"
struct rscs_fc_struct{
	uint16_t flags;
};
#define RSCS_FC_INSTANTANEOUS_STRIDE_LENGTH_SUPPORTED	(0x01)
#define RSCS_FC_TOTAL_DISTANCE_SUPPORTED				(0x02)
#define RSCS_FC_WALKING0_RUNNING1_STATUS_SUPPORTED		(0x04)
#define RSCS_FC_CALIBRATION_SUPPORTED					(0x08)
#define RSCS_FC_MULTIPLE_SENSOR_LOCATIONS_SUPPORTED		(0x10)


//*********
#define RSCS_SLC_UUID			"00002a5d-0000-1000-8000-00805f9b34fb"
// Mandatory if multiple sensor locations supported, Read
struct rscs_slc_struct{
	uint8_t flags;
};
#define RSCS_SLC_OTHER			(0)
#define RSCS_SLC_TOP_OF_SHOE	(1)
#define RSCS_SLC_IN_SHOE		(2)
#define RSCS_SLC_HIP			(3)
#define RSCS_SLC_FRONT_WHEEL	(4)
#define RSCS_SLC_LEFT_CRANK		(5)
#define RSCS_SLC_RIGHT_CRANK	(6)
#define RSCS_SLC_LEFT_PEDAL		(7)
#define RSCS_SLC_RIGHT_PEDAL	(8)
#define RSCS_SLC_FRONT_HUB		(9)
#define RSCS_SLC_REAR_DROPOUT	(10)
#define RSCS_SLC_CHAINSTAY		(11)
#define RSCS_SLC_REAR_WHEEL		(12)
#define RSCS_SLC_REAR_HUB		(13)
#define RSCS_SLC_CHEST			(14)
#define RSCS_SLC_SPIDER			(15)
#define RSCS_SLC_CHAIN_RING		(16)


//*********
#define RSCS_CPC_UUID	"00002a55-0000-1000-8000-00805f9b34fb"  // Write, Indicate
// Mandatory if calibration, total distance, or multiple sensor locations supported, Write, Indicate
struct rscs_cpc_struct{
	uint8_t opcode;
	uint32_t cumulative_value;
	uint8_t sensor_location;
	uint8_t request_op_code;
	uint8_t response_value;
	uint32_t response_parameter; // variable to allow for multiple sensor locations (set to 4 bytes here)

};
#define RSCS_CPC_OPCODE_RESERVED					(0)
#define RSCS_CPC_OPCODE_SET_CUMULATIVE_VALUE	(1)
#define RSCS_CPC_OPCODE_START_CALIBRATION		(2) // Not used in the version of ble spec
#define RSCS_CPC_OPCODE_UPDATE_SENSOR_LOCATION	(3)
#define RSCS_CPC_OPCODE_REQUEST_SENSOR_LOCATION	(4)
#define RSCS_CPC_SUCCESS							(1) // Except for sensor location which returns a list


//**********************************************************************
// RSCS service structure
struct rscs_struct{
		struct rscs_mc_struct mc;
		struct rscs_fc_struct fc;
		struct rscs_slc_struct slc;
		struct rscs_cpc_struct cpc;
};


//**********************************************************************
int gl_ble_rscs_init( Application *application, struct rscs_struct *p_rscs );
void gl_ble_rscs_free( struct rscs_struct *p_rscs );

GByteArray* gl_ble_rscs_mc_struct_to_gba( struct rscs_mc_struct *p_mc );
gboolean gl_ble_rscs_mc_send_notify( gpointer gptr );

int gl_ble_rscs_mc_receive_notify( const GByteArray *byteArray,  struct rscs_mc_struct *p_mc );
int gl_ble_rscs_mc_gba_to_struct( const GByteArray *byteArray, struct rscs_mc_struct *p_mc );

gboolean gl_ble_rscs_mc_simulate( gpointer gptr );

//GByteArray* gl_ble_rscs_mc_struct_to_gba( struct rscs_mc_struct *p_mc );
//int gl_service_notify_rscs_mc_update( const GByteArray *byteArray,  struct rscs_mc_struct *p_mc );


//int cscs_cp_gba_to_struct( const GByteArray *byteArray, struct cscs_struct *p_cscs );


#endif
