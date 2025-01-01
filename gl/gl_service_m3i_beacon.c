/***********************************************************************
 *
 * gl_service_m3i_beacon.c
 *
*/

#define VERBOSE (0)

#include <stdio.h>

#include "device.h"

#include "gl_structures.h"
#include "gl_service_m3i_beacon.h"


/***********************************************************************
 *
 * Extracts data from manufacturer-specific data
*/
int gl_service_m3i_beacon_to_struct( Device *server, struct m3i_struct *p_m3i )
{
	unsigned int i, j;

	const char mfr_data_start[] = "-> "; // Defined in devices.c
	const char *m3i_beacon_data_string = strstr( binc_device_to_string(server), mfr_data_start ) + sizeof( mfr_data_start ) - 1;

	j = 0;
	if( VERBOSE ) printf( "[%s]: Raw packet: ", __func__ );
	for( i = 0; i < sizeof(p_m3i->data.values) - sizeof(p_m3i->data.values.compiler_padding); i++ ){
		sscanf( m3i_beacon_data_string + j, "%02hhx", &p_m3i->data.bytes[i] );
		if( VERBOSE ) printf( "%02X ", p_m3i->data.bytes[i] );
		j = j + 2;
	}
	if( VERBOSE ) printf( "\n" );

	// Scale values, rest are unscaled
	p_m3i->data.values.cadence = (uint16_t) ((double) p_m3i->data.values.cadence / 10.0);
	p_m3i->data.values.bpm = (uint16_t) ((double) p_m3i->data.values.bpm / 10.0);
	p_m3i->data.values.distance = (uint16_t) ((double) p_m3i->data.values.distance / 10.0); // in 0.1 miles

	return( 0 );
}
