/***********************************************************************
 *
 * gl_ble_hrs.c
 *
*/

#define VERBOSE (1)
#define PCOLOR CCYN

#include <stdio.h>
#include <stdint.h>

#include "application.h"
#include "adapter.h"
#include "characteristic.h"
#include "parser.h"

#include "gl_colors.h"
#include "gl_structures.h"
#include "gl_ble_hrs.h"


/***********************************************************************
 *
 * Initialize HRS (Heart Rate Service)
*/
int gl_ble_hrs_init( Application *application, struct hrs_struct *p_hrs )
{
	if (binc_application_add_service(application, HEART_RATE_SERVICE_UUID) != BINC_OK) {
		fprintf( stderr, CRED"[%s] Error: Failed to add service %s\n"CNRM, __func__, HEART_RATE_SERVICE_UUID );
		return( -1 );
	}
	else{

		if( binc_application_add_characteristic(application, HEART_RATE_SERVICE_UUID, HRS_MC_UUID,
				GATT_CHR_PROP_NOTIFY) != BINC_OK) {
			fprintf( stderr, CRED"[%s] Error: Failed to add characteristic %s\n"CNRM, __func__, HRS_MC_UUID );
			return( -1 );
			// CCCD added automatically since specified NOTIFY
		}
		if( VERBOSE ) printf( PCOLOR"[%s]:\tAdded HRS Heart Rate Measurement Characteristic\n"CNRM, __func__ );

		if (binc_application_add_characteristic(application, HEART_RATE_SERVICE_UUID, HRS_BSLC_UUID,
				GATT_CHR_PROP_READ) != BINC_OK){
			fprintf( stderr, CRED"[%s] Error: Failed to add characteristic %s\n"CNRM, __func__, HRS_BSLC_UUID );
			return( -1 );
		}
		if( VERBOSE ) printf( PCOLOR"[%s]:\tAdded HRS Body Sensor Location\n"CNRM, __func__ );

		printf( PCOLOR"[%s]:\tAdded <%.8s> Heart Rate Service (HRS)\n"CNRM, __func__ , HEART_RATE_SERVICE_UUID );
	}

	return( 0 );
}



/***********************************************************************
 *
 * Free any allocations
*/
void gl_ble_hrs_free( struct hrs_struct *p_hrs ){
	// No pointers, nothing to do
	return;
}


/***********************************************************************
 *
 * Convert measurement characteristic data to gba
 *
*/
GByteArray *gl_ble_hrs_mc_struct_to_gba( struct hrs_mc_struct *p_mc )
{
	GByteArray* byteArray;
	uint8_t flags, tmp;

	flags = 0;
	byteArray = g_byte_array_new();

	if( p_mc->heart_rate_measurement > 255 ){
		flags = flags | HRS_MC_HRMV_16BITS;
		tmp = (uint8_t)((p_mc->heart_rate_measurement & 0xFF00) >> 8);
		g_byte_array_append( byteArray, &tmp, 1 );
	}
	tmp = (uint8_t)(p_mc->heart_rate_measurement & 0x00FF);
	g_byte_array_append( byteArray, &tmp, 1 );

	// Energy and R-R not implemented yet

	// Add flags at beginning
	g_byte_array_prepend( byteArray, &flags, 1 );

	//printf( "HRM characteristic: " );
	//for( int j = 0; j < byteArray->len; j++ ) printf( "0x%02x", *(byteArray->data+j) );
	//printf( " (length = %d)\n", byteArray->len );
	//fflush(stdout);

	return( byteArray );
}


/***********************************************************************
 *
 * Acquires latest data, either simulated or real, converts to gba, and
 * sends notify.
*/
gboolean gl_ble_hrs_mc_send_notify( gpointer gptr )
{
	GL_LServer *p_lserver = (GL_LServer *)gptr;

	#ifdef SIM_HR
		gl_ble_hrs_mc_simulate( &(p_lserver->hrs.mc) );
	#else
		// Get real data here
	#endif

	if( p_lserver->app != NULL ){ // Catch asynchronous SIGINT
		if( binc_application_char_is_notifying(p_lserver->app, HEART_RATE_SERVICE_UUID, HRS_MC_UUID) == TRUE ){
			binc_application_notify(p_lserver->app, HEART_RATE_SERVICE_UUID, HRS_MC_UUID,
					gl_ble_hrs_mc_struct_to_gba( &p_lserver->hrs.mc ) );
			if( VERBOSE ) printf( CLGRY"[%s]:\t'%s' [%s] is notifying HRS MC\n"CNRM, __func__,
					binc_adapter_get_name( p_lserver->adapter ), binc_adapter_get_address( p_lserver->adapter ));
		}
	}
	return( TRUE ); // Continue forever
}


/***********************************************************************
 *
 * Receives notify, parses gba into struct
*/
int gl_ble_hrs_mc_receive_notify( const GByteArray *byteArray,  struct hrs_mc_struct *p_mc )
{
	gl_ble_hrs_mc_gba_to_struct( byteArray, p_mc );

	return( 0 );
}


/***********************************************************************
 *
 * Parses gba into structure based on flags
*/
int gl_ble_hrs_mc_gba_to_struct( const GByteArray *byteArray, struct hrs_mc_struct *p_mc )
{
	Parser *parser;
	guint byte_offset;

	parser = parser_create(byteArray, LITTLE_ENDIAN);
	byte_offset = 0; // Flags
	parser_set_offset(parser, byte_offset);
	p_mc->flags = parser_get_uint8(parser);

	byte_offset =  byte_offset + 1;
	parser_set_offset(parser, byte_offset);
	p_mc->heart_rate_measurement = parser_get_uint8(parser);

	if (p_mc->flags & HRS_MC_HRMV_16BITS) {
		byte_offset =  byte_offset + 1;
		parser_set_offset(parser, byte_offset);
		p_mc->heart_rate_measurement = p_mc->heart_rate_measurement + (uint16_t)(parser_get_uint8(parser) << 8);
	}

	// Ignore contact, exp energy, and r-r for now

	parser_free(parser);
	return(0);
}


/***********************************************************************
 *
 * Simulate power (Watts), cadence (RPM), and speed (km/h) for CPS service
*/
gboolean gl_ble_hrs_mc_simulate( gpointer gptr )
{
	struct hrs_mc_struct *p_mc = (struct hrs_mc_struct *)gptr;

	p_mc->heart_rate_measurement = (uint8_t)(rand() % (SIM_HR_HIGH + 1 - SIM_HR_LOW) + SIM_HR_LOW);
	//printf( "Simulated BPM = %d\n", p_mc->heart_rate_measurement );
	return( TRUE ); // Continue forever
}


