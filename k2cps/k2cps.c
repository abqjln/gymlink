/***********************************************************************
 *
 * k2cps.c
 *
 * Keiser to BLE CPS
 *
 * Uses simultaneous central and peripheral mode on single BT adapter
 * Creates a BLE central that scans for Keiser M3i beacon and process advertised data.
 * Creates a BLE Cycling Power and Speed connectable BLE server.
 * Tested with Garmin Fenix8 v.13.12
 *
 * bluez--need to change DeviceID = false in /etc/bluetooth/main.conf
 *
 * Issue: On Fenix8, if enable CPS features requiring indication, watch disconnects after sending the
 * indication confirmation. (Fenix stopped interacting with CPV in v13.12 beta software--might still be broken)
 *
 * Issue: bluez seems to act on connection/disconnection commands but does not always trigger state changes to cbs
*/

#define BT_TIMEOUT (10)

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <glib.h>

#include "adapter.h"
#include "logger.h"
#include "utility.h"

#include "gl_utilities.h"
#include "gl_adapter.h"
#include "gl_agent.h"
#include "gl_server.h"
#include "gl_on_server.h"
#include "gl_client.h"
#include "gl_central.h"
#include "gl_on_central.h"
#include "gl_ble_cps.h"

#include "k2cps.h"

static GL_LServer s_lserver; // Local server data to send to remote client
static GL_LClient s_lclient; // Local client data received from edge servers and beacons

static GMainLoop *s_loop = NULL;
static void s_cleanup_handler( int signo );


//**********************************************************************
int main( int argc, char *argv[] )
{
	char hostname[HOST_NAME_MAX];
	int bt_timeout;

	if( ( argc != 1 )){
		gl_log( GL_LOG_ERROR, "Usage: %s\n", argv[0] );
		exit( -1 );
	}

	if( system( "grep \"^DeviceID = false\" /etc/bluetooth/main.conf > /dev/null" ) ){
		gl_log( GL_LOG_ERROR, "Set DeviceID = false in /etc/bluetooth/main.conf\n" );
		exit( -1 );
	}

	// For use globally
	g_p_lserver = &s_lserver;
	g_p_lclient = &s_lclient;

	// Setup handler
	if (signal(SIGINT, s_cleanup_handler) == SIG_ERR) {
		gl_log( GL_LOG_ERROR, "[%s] Error: Can't catch SIGINT\n", __func__ );
		exit( -1 );
	}

	// Set debug level
	log_enabled( TRUE ); // binc
	log_set_level( LOG_ERROR ); // binc
	gl_set_log_level( GL_LOG_DEBUG );

	// Ensure bluetooth is running without errors
	bt_timeout = 0;
	if( system( "sudo systemctl status bluetooth > /dev/null" ) ){
		// Try to reset bluetooth
		system( "sudo rfkill unblock bluetooth && sudo systemctl daemon-reload && sudo systemctl restart bluetooth" );
		while( (system( "sudo systemctl status bluetooth > /dev/null" )) && (bt_timeout++ < BT_TIMEOUT) ) sleep( 1 );
		if( bt_timeout >= BT_TIMEOUT ){
			gl_log( GL_LOG_ERROR, "[%s]: Bluetooth not ready\n", __func__ );
			exit( -1 );
		}
	}


	//******************************************************************
	// Discover, initialize, and assign adapters
	GPtrArray *p_adapter_list;
	if( gl_adapter_init( &p_adapter_list ) <= 0 ){
		gl_log( GL_LOG_ERROR, "[%s] Error: Failed to initialize %d adapter(s) found\n", __func__, (int) p_adapter_list->len );
		return( -1 );
	}

	s_lclient.adapter = g_ptr_array_index( p_adapter_list, 0 );
	s_lserver.adapter = g_ptr_array_index( p_adapter_list, 0 );
	gl_log( GL_LOG_INFO, "[%s]: Initialized %d adapters (Client='%s', Server='%s')\n", __func__,
			(int) p_adapter_list->len, binc_adapter_get_path( s_lclient.adapter ), binc_adapter_get_path( s_lserver.adapter) );
	g_ptr_array_unref( p_adapter_list );

	//******************************************************************
	// Central
	// Scans and discovers unconnectable K2CPS_M3I_BEACON_NAME and collects data
	// Null string disables connectable client mode since no servers match
	gl_central_set_filter_prefix( "" );
	if( gl_central_init_discovery( s_lclient.adapter, NULL, NULL, &gl_on_central_process_M3i_scan_result ) ){
		gl_log( GL_LOG_ERROR, "[%s] Error: Failed to start discovery\n", __func__ );
		exit( -1 );
	}

	//******************************************************************
	// Server
	// Configure agent for s_lserver server bonding to remote central
	if( (s_lserver.agent = gl_agent_init( s_lserver.adapter, NO_INPUT_NO_OUTPUT )) == NULL ){
		gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add agent\n", __func__ );
		return( -1 );
	}

	// Configure s_lserver server
	s_lserver.dbusConnection = binc_adapter_get_dbus_connection( s_lserver.adapter );
	s_lserver.app = NULL;
	s_lserver.central_device = NULL;
	s_lserver.adv_name[0] = '\0';
	s_lserver.adv_service_uuids = g_ptr_array_new();
	g_ptr_array_add( s_lserver.adv_service_uuids, CYCLING_POWER_SERVICE_UUID );

	gethostname(hostname, 8);
	s_lserver.dis.model = malloc( strlen(hostname) + 5 );
	sprintf( s_lserver.dis.model, "%8s-%3s", hostname, "CPS" );
	s_lserver.adv_manufacturer_id = K2CPS_ADV_MANUFACTURER_ID;
	snprintf( s_lserver.adv_name, sizeof(s_lserver.adv_name), "%s-%s-%4s", hostname, s_lserver.dis.model, random_string( 4 ) );
	s_lserver.adv_min_interval = 100;
	s_lserver.adv_max_interval = 300;
	s_lserver.bs.blc.notify_ms = K2CPS_BS_BLC_NOTIFY_MS;
	s_lserver.bs.blc.percent = 100; // Plugged into wall
	s_lserver.dis.serial = K2CPS_DIS_SERIAL_NUMBER;
	s_lserver.dis.hardware = K2CPS_DIS_HARDWARE_REV;
	s_lserver.dis.firmware = K2CPS_DIS_FIRMWARE_REV;
	s_lserver.dis.software = K2CPS_DIS_SOFTWARE_REV;
	s_lserver.dis.manufacturer = K2CPS_DIS_MANUFACTURER;
	s_lserver.cps.fc.flags = CPS_FC_CRANK_REVOLUTION_DATA_SUPPORTED | CPS_FC_WHEEL_REVOLUTION_DATA_SUPPORTED | CPS_FC_DISTRIBUTED_SYS_SUPPORT_FALSE;
	s_lserver.cps.slc.flags = CPS_SLC_SPIDER; // Spider on M3i is total power, not bilateral
	s_lserver.cps.mc.notify_ms = K2CPS_CPS_CPC_NOTIFY_MS;
	s_lserver.cps.mc.gear_ratio = K2CPS_KONA_GEAR_RATIO;
	s_lserver.cps.mc.tire_circumference_km = K2CPS_KONA_TIRE_650B_CIRCUMFERENCE_KM;

	gl_cps_mc_simulate = FALSE;

	// Start application
	if( gl_server_start_application( &s_lserver,
			&gl_on_server_charact_read_request,
			&gl_on_server_charact_write_request,
			&gl_on_server_charact_updated,
			&gl_on_server_desc_read_request,
			&gl_on_server_desc_write_request,
			&gl_on_server_enable_notify,
			&gl_on_server_disable_notify) ){
		gl_log( GL_LOG_ERROR, "[%s] Error: Failed to start application\n", __func__ );
		return( -1 );
	}

	// Configure services
	if( gl_server_add_services( &s_lserver ) ){
		gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add services\n", __func__ );
		return( -1 );
	}

	// Start advertising
	if( gl_server_start_advertising( &s_lserver ) ){
		gl_log( GL_LOG_ERROR, "[%s] Error: Failed to start advertising\n", __func__ );
		return( -1 );
	}


	//******************************************************************
	// Setup mainloop
	s_loop = g_main_loop_new( NULL, FALSE );

	// Main loop--blocks
	g_main_loop_run( s_loop );

	exit( 0 );
}


static void s_cleanup_handler( int signo )
{
	if( signo == SIGINT )
	{
		gl_log( GL_LOG_INFO, "[%s]: Received SIGINT, shutting down\n", __func__ );
		gl_server_close( g_p_lserver );

		if( s_loop != NULL ){
			g_main_loop_quit( s_loop );
			g_main_loop_unref( s_loop );
		}

		gl_log( GL_LOG_INFO, "[%s]: Restarting bluetooth\n", __func__ );
		system( "{ sudo systemctl daemon-reload; sudo systemctl restart bluetooth; } &" );

		exit( 0 );
	}
}
