#ifndef GL_STRUCTURES_H
#define GL_STRUCTURES_H

#include <stdint.h>

#include <glib.h>

#include "adapter.h"
#include "application.h"
#include "advertisement.h"
#include "device.h"

#include "gl_ble_dis.h"
#include "gl_ble_bs.h"
#include "gl_ble_cps.h"
#include "gl_ble_cscs.h"
#include "gl_ble_hrs.h"
#include "gl_ble_rscs.h"
#include "gl_ble_ftms.h"
#include "gl_service_m3i_beacon.h"


/***********************************************************************
 * Curated data to serve remote clients
 *
 */
struct gl_local_server_struct{
	GDBusConnection *dbusConnection;
	Adapter *adapter;

	// Peripheral role
	Device *central_device;
	ConnectionState connection_state;
	Application *app;
	Advertisement *advertisement;
	Agent *agent;
	char adv_name[1024];
	GPtrArray *adv_service_uuids;
	uint16_t adv_manufacturer_id;
	uint8_t adv_manufacturer_data[23]; // (31 BLE AD max - 3 flags/adtype - 2 mfr adtype - 2 manufacturer id - 1 len) = 23
	uint32_t adv_min_interval;
	uint32_t adv_max_interval;
	uint32_t tx_power; // added to binc 12/14/2024; untested

	// Server role--only BLE services
	struct dis_struct dis;
	struct bs_struct bs;
	struct cps_struct cps;
	struct cscs_struct cscs;
	struct hrs_struct hrs;
	struct rscs_struct rscs;
	struct ftms_struct ftms;
};
typedef struct gl_local_server_struct GL_LServer;


/***********************************************************************
 * Data collected from remote/edge servers
 *
 */struct gl_local_client_struct{
	GDBusConnection *dbusConnection;
	Adapter *adapter;

	// Central role
	Agent *agent;

	// Client role--BLE and other data sources
	struct dis_struct dis;
	struct bs_struct bs;
	struct hrs_struct hrs;
	struct cps_struct cps;
	struct cscs_struct cscs;
	struct rscs_struct rscs;
	struct ftms_struct ftms;
	struct m3i_struct m3i;
};
typedef struct gl_local_client_struct GL_LClient;


#endif
