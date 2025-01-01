/***********************************************************************
 *
 * gl_agent.c
 *
*/

#define PCOLOR CCYN
#define VERBOSE (0)

#include <stdio.h>
#include <stdint.h>

#include "agent.h"
#include "device.h"
#include "utility.h"

#include "gl_colors.h"
#include "gl_utilities.h"
#include "gl_on_central.h"
#include "gl_agent.h"


/***********************************************************************
 *
 * Creates an unique agent using specified adapter and authorizations
 *
 *  Use for both central and peripheral sides
*/
Agent *gl_agent_init( Adapter *adapter, IoCapability io_capability )
{
	char *agent_path, *rstring;
	Agent *agent = NULL;

	rstring = random_string(4);
	agent_path = g_strdup_printf( "/org/bluez/BincAgent_%s", rstring );
	g_free( rstring );

	agent = binc_agent_create( adapter, agent_path, io_capability );
	g_free( agent_path );

	binc_agent_set_request_authorization_cb( agent, &gl_on_central_request_authorization );
	binc_agent_set_request_passkey_cb( agent, &gl_on_central_request_passkey );

	binc_adapter_set_device_removal_cb( adapter, gl_on_central_device_removal );

	gl_log( GL_LOG_INFO, "[%s]: Initialized agent '%s' for adapter %s\n", __func__, binc_agent_get_path( agent ), binc_adapter_get_name( adapter) );

	return( agent );
}

