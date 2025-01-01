/***********************************************************************
 *
 * gl_adapter.c
 *
*/


#include <stdio.h>

#include "adapter.h"

#include "gl_colors.h"
#include "gl_utilities.h"
#include "gl_on_adapter.h"
#include "gl_adapter.h"


/***********************************************************************
 *
 * Uses DBus to find and initialize all adapters and power on.
 * Returns a g_ptr_array list of adapters.
 * Highest number in the list is the onboard for RPi
*/
int gl_adapter_init( GPtrArray **pp_adapter_list )
{
	GDBusConnection *dbconnection;
	Adapter *adapter;
	int i;

	// Get DBus connection (common for all adapters)
	if( ( dbconnection = g_bus_get_sync( G_BUS_TYPE_SYSTEM, NULL, NULL )) == NULL ){
		gl_log( GL_LOG_ERROR, "%s] Error: Failed to get DBus connection", __func__ );
		return( -1 );
	}

	// Enumerate adapters, if any
	*pp_adapter_list = binc_adapter_find_all( dbconnection );
	gl_log( GL_LOG_INFO, "[%s]: Found %d adapters\n", __func__, (int) (*pp_adapter_list)->len );

	// Run through all adapters
	for( i = 0; i < (int) (*pp_adapter_list)->len; i++ ){
		adapter = g_ptr_array_index( *pp_adapter_list, i );

		// Turn all adapters on
		binc_adapter_set_powered_state_cb( adapter , &gl_on_adapter_powered_state_changed );
		if( !binc_adapter_get_powered_state( adapter ) ){
			gl_log( GL_LOG_DEBUG, "[%s]: Adapter '%s' is off\n", __func__, binc_adapter_get_path( adapter ) );
			binc_adapter_power_on( adapter );
			gl_log( GL_LOG_DEBUG, "[%s]: Powering on adapter '%s'\n", __func__, binc_adapter_get_path( adapter ) );
			while( !binc_adapter_get_powered_state( adapter ) ) gl_log( GL_LOG_DEBUG, "Waiting for adapter power on\r" );
		}

		gl_log( GL_LOG_INFO, "[%s]: Configured adapter '%s' [%s]\n", __func__,
				binc_adapter_get_path( adapter ), binc_adapter_get_address ( adapter ) );
	}

	return( (int) (*pp_adapter_list)->len );
}


