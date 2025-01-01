/***********************************************************************
 *
 * gl_client.c
 *
*/

#define PCOLOR CGRN

#include <stdio.h>

#include "agent.h"

#include "gl_client.h"

 // Define pointer used in lib for cb
GL_LClient *g_p_lclient;


/***********************************************************************
 *
 * Disconnects from any connected servers and shuts down the rest
*/
int gl_client_close( gpointer gptr )
{
	GDBusConnection *dbconn;
	Adapter *client_adapter;
	GList *conn_list;
	Device *conn_device;
	unsigned int i;

	printf( "\n[%s]: Closing...\n", __func__ );

	// Disconnect our client from any server connections
	client_adapter = binc_agent_get_adapter( g_p_lclient->agent );
	conn_list = binc_adapter_get_connected_devices( client_adapter );
	for( i = 0; i < g_list_length(conn_list); i++ ){
		conn_device = g_list_nth_data( conn_list, i );
		printf( "\tdisconnecting server '%s' [%s] from client %s...\n",
				binc_device_get_name( conn_device ), binc_device_get_address( conn_device ), binc_adapter_get_name( client_adapter ));
		binc_device_disconnect( conn_device );
	}

	sleep(5); // Allow disconnects to complete

	printf( "\tfreeing agent '%s'...\n", binc_agent_get_path( g_p_lclient->agent ) );
	binc_agent_free( g_p_lclient->agent );
	g_p_lclient->agent = NULL;

	printf( "\tfreeing adapter '%s'...\n", binc_adapter_get_name( client_adapter ));
	dbconn = binc_adapter_get_dbus_connection( client_adapter );
	binc_adapter_free( client_adapter );
	client_adapter = NULL;

	printf( "\tdisconnecting from dbus %p...\n", dbconn );
	g_dbus_connection_close_sync( dbconn, NULL, NULL );
	g_object_unref( dbconn );

	// Free our data structures
	gl_ble_bs_free( &g_p_lclient->bs );
	gl_ble_dis_free( &g_p_lclient->dis );
	gl_ble_hrs_free( &g_p_lclient->hrs );
	gl_ble_cps_free( &g_p_lclient->cps );
	gl_ble_rscs_free( &g_p_lclient->rscs );
	gl_ble_cscs_free( &g_p_lclient->cscs );

	printf( "\tdone.\n" );
	return( 0 );
}

