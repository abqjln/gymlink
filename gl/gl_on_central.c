/***********************************************************************
 *
 * gl_on_central.c
 *
 * Callbacks for central connection state
*/

#include <signal.h>
#include <stdint.h>

#include "adapter.h"
#include "device.h"

#include "gl_utilities.h"
#include "gl_config.h"
#include "gl_agent.h"
#include "gl_central.h"
#include "gl_server.h"
#include "gl_client.h"
#include "gl_on_client.h"
#include "gl_on_central.h"


/***********************************************************************
 *
 * Manages connect/disconnect/re-connect behavior and keeps discovery going
 * so can continue to detect unconnectable beacons
*/
void gl_on_central_peripheral_connection_state_changed( Device *periph, ConnectionState state, const GError *error )
{
	const char *periph_name = binc_device_get_name( periph );
	const char *periph_address = binc_device_get_address( periph );
	Adapter *adapter = binc_device_get_adapter( periph );
	const char *adapter_address = binc_adapter_get_address( adapter );

	if( error != NULL ){
		gl_log( GL_LOG_ERROR, "[%s] Error: '%s' [%s] %s (%d)\n",
				__func__, periph_name, periph_address, error->message, error->code );
		// Error 36: GDBus.Error:org.bluez.Error.Failed: le-connection-abort-by-local
		// Typically weak signal or interference (WiFi 2.4GHz)
		// https://stackoverflow.com/questions/63812191/connection-via-bluetooth-le-fail-on-android-when-device-is-not-in-pairing-mode

		// Error 24: timeout --what is that due to??

		// Make sure we are marked as disconnected
		binc_device_disconnect( periph );

		// Remove devices immediately of they are not bonded
		// DOES THIS trigger service resoluton upon next connection, otherwise uses cache????
		if( binc_device_get_bonding_state( periph ) != BINC_BONDED ){
			binc_adapter_remove_device( adapter, periph );
		}

		gl_central_start_discovery_nodedup( adapter );
		//binc_adapter_start_discovery( adapter ); // de-duplicates, wiping out beacon data
	}

	//**********
	else if( state == BINC_DISCONNECTED ){
		gl_log( GL_LOG_ALERT, "[%s]: Adapter [%s] and server '%s' [%s] disconnected\n",
				__func__, adapter_address, periph_name, periph_address );

		// Remove devices immediately of they are not bonded
		// DOES THIS trigger service resoluton upon next connection, otherwise uses cache????
		if( binc_device_get_bonding_state( periph ) != BINC_BONDED ){
			binc_adapter_remove_device( adapter, periph );
		}

		//binc_adapter_start_discovery( adapter ); // de-duplicates, wiping out beacon data
		gl_central_start_discovery_nodedup( adapter );
	}

	//**********
	else if( state == BINC_CONNECTED ){
		gl_log( GL_LOG_ALERT, "[%s]: Adapter [%s] and server '%s' [%s] connected with rssi %d\n",
				__func__, adapter_address, periph_name, periph_address, binc_device_get_rssi( periph) );

		//binc_adapter_start_discovery( adapter ); // de-duplicates, wiping out beacon data
		gl_central_start_discovery_nodedup( adapter );
	}

	//**********
	else if( state == BINC_CONNECTING ){
		gl_log( GL_LOG_ALERT, "[%s]: Adapter [%s] and server '%s' [%s] connecting\n",
				__func__, adapter_address, periph_name, periph_address );
	}

	//**********
	else if( state == BINC_DISCONNECTING ){
		gl_log( GL_LOG_ALERT, "[%s]: Adapter [%s] and server '%s' [%s] disconnecting\n",
				__func__, adapter_address, periph_name, periph_address );
	}

	//**********
	else{
		gl_log( GL_LOG_ERROR, "[%s] Error: '%s' [%s] indeterminate state; retrying\n",
				__func__, periph_name, periph_address);

		binc_device_disconnect( periph ); // Make sure

		//binc_adapter_start_discovery( adapter ); // de-duplicates, wiping out beacon data
		gl_central_start_discovery_nodedup( adapter );
	}
}


/***********************************************************************
 *
 * Filters devices found in discovery scans and processes. bluez scans must
 * be left wide open (NULL uuid, and NULL pattern) as they do not "or" the
 * filtered results.
 *
 *
 * Connectable devices have their cb set and connection initiated.
 * Unconnectable devices have the data in their advertising packet
 * processed uniquely so we need to hardcode the unique identifier, typically
 * a name to cover random address usage.
 *
 * This is where the Device naming transitions
 * from periph (connection state to central) to server (data flow state to client)
 *
 *
*/
void gl_on_central_process_M3i_scan_result( Adapter *adapter, Device *peripheral )
{
	const char *peripheral_name = binc_device_get_name( peripheral );
	Device *server, *beacon; // Is peripheral a server (connectable) or beacon (not connectable)

	// Remember bluez uses kernel filters that don't work for us, so we specify NULL, NULL meaning
	// there is no filtering before this

	// Exclude null-named peripherals
	if( peripheral_name == NULL ) return;

	// Beacon name exact match (non connectable, special handling, no GATT services)
	if( g_strcmp0( peripheral_name, GYMLINK_M3I_BEACON_NAME ) == 0 ){
		//gl_log( GL_LOG_DEBUG, "[%s]:\tMatched peripheral %s with beacon pattern %s\n", __func__, peripheral_name, GYMLINK_M3I_BEACON_NAME );
		beacon = peripheral;
		g_p_lclient->m3i.updated = 1;
		gl_service_m3i_beacon_to_struct( beacon, &(g_p_lclient->m3i) );
		//gl_log( GL_LOG_DEBUG, "[%s]: '%s': %3d RPM, %3d Watts, %5.1f mi, %4d BPM\n",
		//		__func__, GYMLINK_M3I_BEACON_NAME, g_p_lclient->m3i.data.values.cadence,
		//		g_p_lclient->m3i.data.values.watts, (double) (g_p_lclient->m3i.data.values.distance)/10.0, g_p_lclient->m3i.data.values.bpm );
		return;
	}

	// If null prefix, do not connect with anything
	if( !strcmp( "", gl_central_get_filter_prefix() ) ){
		return;
	}
	else{
		// Connect to device(s) matching prefix (connect to GATT services)
		if( g_str_has_prefix( peripheral_name, gl_central_get_filter_prefix() ) == TRUE ){
			gl_log( GL_LOG_ALERT, "[%s]: Connecting to %s\n", __func__, binc_device_to_string( peripheral ) );
			server = peripheral; // Call it a server
			binc_device_set_connection_state_change_cb( server, &gl_on_central_peripheral_connection_state_changed );
			binc_device_set_bonding_state_changed_cb( server, &gl_on_central_bonding_state_changed );
			binc_device_set_services_resolved_cb( server, &gl_on_client_services_resolved );
			binc_device_set_read_char_cb( server, &gl_on_client_received_char_read );
			binc_device_set_write_char_cb( server, &gl_on_client_char_write );
			binc_device_set_notify_char_cb( server, &gl_on_client_received_notification );
			binc_device_set_notify_state_cb( server, &gl_on_client_server_notification_state_changed );
			binc_device_set_read_desc_cb( server, &gl_on_client_desc_read );
			binc_device_connect( server );
			return;
		}
	}

	return;
}


/***********************************************************************
 *
*/
void gl_on_central_discovery_state_changed(Adapter *adapter, DiscoveryState state, const GError *error) {
    if (error != NULL) {
		gl_log( GL_LOG_ERROR, "[%s] Error: Adapter '%s' error %s (%d)\n",
				__func__, binc_adapter_get_path(adapter), error->message, error->code);
		return;
	}

	gl_log( GL_LOG_INFO, "[%s]: Adapter '%s' new discovery state is '%s' (%d)\n",
			__func__, binc_adapter_get_path( adapter ),
			binc_adapter_get_discovery_state_name( adapter ),
			binc_adapter_get_discovery_state( adapter ) );
}


/***********************************************************************
 *
*/
void gl_on_central_device_removal( Adapter *adapter, Device *device )
{
	// Very noisy
	//gl_log( GL_LOG_DEBUG, "[%s]: Removed '%s' [%s] from hash table on adapter [%s]\n",
	//		__func__, binc_device_get_name( device ), binc_device_get_address( device ), binc_adapter_get_address( adapter ) );
	return;
}


/***********************************************************************
 *
 *
*/
void gl_on_central_bonding_state_changed(Device *periph, BondingState new_state, BondingState old_state, const GError *error)
{
	gl_log( GL_LOG_ALERT, "[%s]: Peripheral '%s' [%s] bonding state changed from %d to %d",
			__func__, binc_device_get_name( periph ),  binc_device_get_address( periph ), old_state, new_state);
}


/***********************************************************************
 *
*/
gboolean gl_on_central_request_authorization(Device *periph)
{
	gl_log( GL_LOG_ALERT, "[%s]: Peripheral '%s' [%s] requesting authorization\n",
			__func__, binc_device_get_name( periph ), binc_device_get_address( periph ) );
    return( TRUE );
}


