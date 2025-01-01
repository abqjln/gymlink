#ifndef GL_BLE_DIS_H
#define GL_BLE_DIS_H

#include <stdint.h>

#include "characteristic.h"
#include "application.h"


//**********************************************************************
// DEVICE INFORMATION SERVICE
#define DIS_SERVICE_UUID		"0000180a-0000-1000-8000-00805f9b34fb"
#define DIS_SYSTEM_ID_UUID		"00002a23-0000-1000-8000-00805f9b34fb"
#define DIS_MODEL_UUID			"00002a24-0000-1000-8000-00805f9b34fb"
#define DIS_SERIAL_UUID			"00002a25-0000-1000-8000-00805f9b34fb"
#define DIS_FIRMWARE_UUID		"00002a26-0000-1000-8000-00805f9b34fb"
#define DIS_HARDWARE_UUID		"00002a27-0000-1000-8000-00805f9b34fb"
#define DIS_SOFTWARE_UUID		"00002a28-0000-1000-8000-00805f9b34fb"
#define DIS_MANUFACTURER_UUID	"00002a29-0000-1000-8000-00805f9b34fb"
#define DIS_PnP_ID_UUID			"00002a50-0000-1000-8000-00805f9b34fb"

struct dis_struct{
	uint64_t system_id;
	char *model;
	char *serial;
	char *hardware;
	char *firmware;
	char *software;
	char *manufacturer;
	struct {
		uint8_t vendor_id_source;
		uint16_t vendor_id;
		uint16_t product_id;
		uint16_t product_version;
	} pnp_id;
};


//**********************************************************************
int gl_ble_dis_init( Application *application, struct dis_struct *p_dis_s );
void gl_ble_dis_inspect( struct dis_struct *p_dis_s );
void gl_ble_dis_free( struct dis_struct *p_dis_s );

#endif
