/***********************************************************************
 *
 * gl_on_peripheral.c
 *
*/

#include <time.h>

#include "adapter.h"
#include "device.h"

#include "gl_colors.h"
#include "gl_utilities.h"
#include "gl_structures.h"
#include "gl_config.h"
#include "gl_adapter.h"
#include "gl_server.h"
#include "gl_on_peripheral.h"


/***********************************************************************
 *
 * Callback for peripheral when connection state with central changes
 *
*/
void gl_on_peripheral_central_connection_state_changed( Adapter *peripheral, Device *central )
{
//	time_t unix_seconds;
//	struct tm * local;

	// Timestamp for checking dropouts
//	time( &unix_seconds );
//	local = localtime( &unix_seconds );

	// Save global connection state to mask notifies
	g_p_lserver->connection_state = binc_device_get_connection_state( central );

	if( g_p_lserver->connection_state == BINC_CONNECTED ){
		gl_log( GL_LOG_ALERT, "[%s]: Central '%s' [%s] and peripheral '%s' [%s] CONNECTED\n", __func__,
				binc_device_get_name( central ), binc_device_get_address( central ),
				binc_adapter_get_name( peripheral ), binc_adapter_get_address( peripheral) );
		binc_adapter_stop_advertising( peripheral,  binc_adapter_get_advertisement( peripheral ) );
	}
	else if( g_p_lserver->connection_state == BINC_DISCONNECTED ){
		gl_log( GL_LOG_ALERT, "[%s]: Central '%s' [%s] and peripheral '%s' [%s] DISCONNECTED\n", __func__,
				binc_device_get_name( central ), binc_device_get_address( central ),
				binc_adapter_get_name( peripheral ), binc_adapter_get_address( peripheral) );
		binc_adapter_start_advertising( peripheral, binc_adapter_get_advertisement( peripheral ) );
	}
	else if( g_p_lserver->connection_state == BINC_CONNECTING ){
		gl_log( GL_LOG_ALERT, "[%s]: Central '%s' [%s] and peripheral '%s' [%s] connecting\n", __func__,
				binc_device_get_name( central ), binc_device_get_address( central ),
				binc_adapter_get_name( peripheral ), binc_adapter_get_address( peripheral) );
	}
	else {
		gl_log( GL_LOG_ERROR, "[%s] Error: Central '%s' [%s] connection state to peripheral  '%s' [%s] INDETERMINATE\n", __func__,
				binc_device_get_name( central ), binc_device_get_address( central ),
				binc_adapter_get_name( peripheral ), binc_adapter_get_address( peripheral ) );
		// Disconnect and restart
		binc_device_disconnect( central );
		binc_adapter_start_advertising( peripheral, binc_adapter_get_advertisement( peripheral ) );
 	}
}

