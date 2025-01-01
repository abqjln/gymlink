#ifndef GL_BLE_BS_H
#define GL_BLE_BS_H

#include <stdint.h>

#include "application.h"

#include "gl_service_simvals.h"


//**********************************************************************
// BATTERY SERVICE
#define BATTERY_SERVICE_UUID			"0000180f-0000-1000-8000-00805f9b34fb"

//**********
#define BS_BATTERY_LEVEL_UUID			"00002a19-0000-1000-8000-00805f9b34fb" // CCCD auto-attached
// Mandatory, Read, Notify optional
struct bs_blc_struct{
	uint8_t percent;
	unsigned int notify_ms;
};


//**********************************************************************
// Battery service structure
struct bs_struct{
	struct bs_blc_struct blc;
};


//**********************************************************************
int gl_ble_bs_init( Application *application, struct bs_struct *p_bs  );

void gl_ble_bs_free( struct bs_struct *p_bs );

GByteArray* gl_ble_bs_blc_struct_to_gba( struct bs_blc_struct *p_bs_blc );

gboolean gl_ble_bs_blc_send_notify( gpointer gptr );

int gl_ble_bs_blc_gba_to_struct( const GByteArray *byteArray, struct bs_blc_struct *p_blc );

int gl_ble_bs_blc_receive_notify( const GByteArray *byteArray,  struct bs_blc_struct *p_blc );

void gl_ble_bs_blc_simulate( struct bs_blc_struct *p_blc );

#endif
