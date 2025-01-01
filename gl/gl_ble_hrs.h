#ifndef GL_BLE_HRS_H
#define GL_BLE_HRS_H

#include <stdint.h>

#include "application.h"

#include "gl_service_simvals.h"


//**********************************************************************
// HEART RATE SERVICE
#define HEART_RATE_SERVICE_UUID	"0000180d-0000-1000-8000-00805f9b34fb"

//**********
#define HRS_MC_UUID			"00002a37-0000-1000-8000-00805f9b34fb"
// Mandatory, Notify, CCCD auto-attached
struct hrs_mc_struct{
	uint8_t flags;
	uint16_t heart_rate_measurement; // two bytes based on flag
	uint16_t energy_expended;
	unsigned int notify_ms;
	uint16_t r_r_interval[]; // 1/1024 seconds
};

union hrs_mc_union{
	struct hrs_mc_struct data;
	unsigned char bytes[sizeof(struct hrs_mc_struct)];
};
// Flags
#define HRS_MC_HRMV_16BITS					0x01
#define HRS_MC_CONTACT_SUPPORT_NO_CONTACT	0x02
#define HRS_MC_CONTACT_SUPPORT_W_CONTACT	0x06
#define HRS_MC_ENG_EXP_PRESENT				0x08
#define HRS_MC_RR_PRESENT					0x10


//**********
#define HRS_BSLC_UUID		"00002a38-0000-1000-8000-00805f9b34fb"
// Optional, Read
struct hrs_bslc_struct{
	uint8_t location;
};
#define HRS_BSLC_OTHER					0x00
#define HRS_BSLC_CHEST					0x01
#define HRS_BSLC_WRIST					0x02
#define HRS_BSLC_FINGER					0x03
#define HRS_BSLC_HAND					0x04
#define HRS_BSLC_EAR_LOBE				0x05
#define HRS_BSLC_FOOT					0x06


//**********
#define HRS_CPC_UUID		"00002a39-0000-1000-8000-00805f9b34fb"
// Mandatory, Write if energy expenditure feature supported, otherwise excluded
struct hrs_cpc_struct{
	uint8_t control_point;
};
#define HRS_CPC_RESET_ENERGY_EXP		0x01


//**********************************************************************
// HR Service structure
struct hrs_struct{
	struct hrs_mc_struct mc;
	struct hrs_bslc_struct bslc;
	struct hrs_cpc_struct cpc;
};


//**********************************************************************
int gl_ble_hrs_init( Application *application, struct hrs_struct *p_hrs );

GByteArray* gl_ble_hrs_mc_struct_to_gba( struct hrs_mc_struct *p_mc );

int gl_ble_hrs_mc_gba_to_struct( const GByteArray *byteArray, struct hrs_mc_struct *p_mc );

gboolean gl_ble_hrs_mc_send_notify( gpointer gptr );

gboolean gl_ble_hrs_mc_simulate( gpointer gptr );

int gl_ble_hrs_mc_receive_notify( const GByteArray *byteArray,  struct hrs_mc_struct *p_mc );

void gl_ble_hrs_free( struct hrs_struct *p_hrs );


#endif
