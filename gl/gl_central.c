/***********************************************************************
 *
 * gl_central.c
 *
*/

#define VERBOSE (1)
#define PCOLOR CGRN

#include <stdio.h>
#include <string.h>

#include "adapter.h"

#include "gl_colors.h"
#include "gl_utilities.h"
#include "gl_structures.h"
#include "gl_config.h"
#include "gl_on_central.h"
#include "gl_central.h"

static char s_prefix[100];


/***********************************************************************
 *
 * Prefix (pattern) for matching peripherals
 */
int gl_central_set_filter_prefix( char *prefix )
{
	strncpy( s_prefix, prefix, sizeof(s_prefix) );

	return( 0 );
}


char *gl_central_get_filter_prefix( void )
{
	return( s_prefix );
}


/***********************************************************************
 *
 * Starts discovery on adapter using UUIDs and/or advertised pattern
 */
int gl_central_init_discovery( Adapter *adapter, GPtrArray *adv_uuids, char *pattern,
		AdapterDiscoveryResultCallback result_cb )
{
	binc_adapter_set_discovery_cb( adapter, result_cb );
	binc_adapter_set_discovery_state_cb( adapter, &gl_on_central_discovery_state_changed );

	// If filter on non-null kfilter_adv_uuids, do not also get pattern match (for beacons like M3)
	// The uuid and adv_pattern arguments are filtered at the OS level. And they are not OR'd
	// If pattern is specified, does not also give uuids.
	// https://github.com/hbldh/bleak/issues/230
	// https://github.com/weliem/bluez_inc/issues/60
	// If this changes, should implement to reduce filtering in userspace
	binc_adapter_set_discovery_filter( adapter, -100, adv_uuids, pattern );

	// Initialize using this; afterwards use gl_central_start_discovery_nodedup( adapter );
	binc_adapter_start_discovery( adapter );

	gl_log( GL_LOG_INFO, "[%s]: Started discovery on adapter '%s', bluez/kernel filters={UUID=[%s], pattern='%s'}, gl_scan_filter='%s'\n",
			__func__, binc_adapter_get_path( adapter ), gl_get_uuid_string(adv_uuids), pattern, gl_central_get_filter_prefix() );

	return( 0 );
}


/***********************************************************************
 *
 * gl_central_start_discovery_nodedup
 *
 * Performs discovery without deduplication that is unstoppable in bluez
 *
 *
 * https://github.com/hbldh/bleak/issues/235
 *
 * First, disable ongoing scan enabled by bluetoothctl - if this is not executed
 * then next command to enable scanning will result in Command Disallowed (0x0c)
 * status. Fortunatelly, bluetoothctl seems not to be bothered by execution of
 * this commands.
 *
 * hcitool cmd 0x08 0x000C 0x00 0x00
 * This results in
 * < HCI Command: LE Set Scan Enable (0x08|0x000c) plen 2
 *         Scanning: Disabled (0x00)
 *         Filter duplicates: Disabled (0x00)
 * > HCI Event: Command Complete (0x0e) plen 4
 *        LE Set Scan Enable (0x08|0x000c) ncmd 1
 *        Status: Success (0x00)
 *
 * Now, enable scanning with duplicate filtering disabled
 * hcitool cmd 0x08 0x000C 0x01 0x00
 * This results in
 * < HCI Command: LE Set Scan Enable (0x08|0x000c) plen 2
 *         Scanning: Enabled (0x01)
 *         Filter duplicates: Disabled (0x00)
 * > HCI Event: Command Complete (0x0e) plen 4
 *       LE Set Scan Enable (0x08|0x000c) ncmd 1
 *         Status: Success (0x00)
*/
void gl_central_start_discovery_nodedup( Adapter *adapter )
{
	char cmd[60], *hcx;
	const char *hcx_path;

	hcx_path = binc_adapter_get_path( adapter );
	hcx = strstr( hcx_path, "hci" );

	snprintf( cmd, sizeof(cmd), "sudo hcitool -i %s cmd 0x08 0x000C 0x00 0x00 > /dev/null", hcx );
	system( cmd );

	snprintf( cmd, sizeof(cmd), "sudo hcitool -i %s cmd 0x08 0x000C 0x01 0x00 > /dev/null", hcx );
	system( cmd );

	return;
}

