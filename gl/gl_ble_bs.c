/***********************************************************************
 *
 * gl_ble_bs.c
 *
*/

#include "application.h"
#include "characteristic.h"
#include "parser.h"

#include "gl_utilities.h"
#include "gl_structures.h"
#include "gl_ble_bs.h"


/***********************************************************************
 *
 * Initializes Battery Service (BS)
*/
int gl_ble_bs_init( Application *application, struct bs_struct *p_bs  )
{
	if (binc_application_add_service(application, BATTERY_SERVICE_UUID) != BINC_OK) {
		gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add service %s\n", __func__, BATTERY_SERVICE_UUID );
		return( -1 );
	}
	else{
		gl_log( GL_LOG_INFO, "[%s]: <%.8s> Battery Service (BS)\n", __func__, BATTERY_SERVICE_UUID );
		if (binc_application_add_characteristic(application, BATTERY_SERVICE_UUID, BS_BATTERY_LEVEL_UUID,
			GATT_CHR_PROP_READ | GATT_CHR_PROP_NOTIFY) != BINC_OK){
			gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add service %s", __func__, BS_BATTERY_LEVEL_UUID );
			return( -1 );
		}
	}

	gl_log( GL_LOG_INFO, "[%s]:\t<%.8s>:<%.8s> BS Charge Level\n", __func__, BATTERY_SERVICE_UUID, BS_BATTERY_LEVEL_UUID);

	return( 0 );
}


/***********************************************************************
 *
 * Free any allocations
*/
void gl_ble_bs_free( struct bs_struct *p_bs ){
	// No pointers, nothing to do
	return;
}


/***********************************************************************
 *
 * Convert Battery Characteristic structure data to gba
*/
GByteArray* gl_ble_bs_blc_struct_to_gba( struct bs_blc_struct *p_bs_blc )
{
	GByteArray* byteArray;
	byteArray = g_byte_array_new();
	g_byte_array_append( byteArray, (guint8 *)&(p_bs_blc->percent), sizeof(p_bs_blc->percent) );

	#if 0
	printf( "BS BLC characteristic: " );
	for( guint j = 0; j < byteArray->len; j++ ) printf( "%02x", *(byteArray->data+j) );
	printf( " (length = %d)\n", byteArray->len );
	fflush(stdout);
	#endif

	return( byteArray );
}


/***********************************************************************
 *
 * Acquires latest data, either simulated or real, converts to gba, and
 * sends notify.
*/
gboolean gl_ble_bs_blc_send_notify( gpointer gptr )
{
	GL_LServer *p_lserver = (GL_LServer *)gptr;

	#ifdef SIM_BATTERY_LEVEL
		gl_ble_bs_blc_simulate( &(p_lserver->bs.blc) );
	#else
		// Acquire real data here
	#endif

	if( p_lserver->app != NULL ){ // Catch asynchronous SIGINT
		if( binc_application_char_is_notifying( p_lserver->app, BATTERY_SERVICE_UUID, BS_BATTERY_LEVEL_UUID ) == TRUE ){
			binc_application_notify( p_lserver->app, BATTERY_SERVICE_UUID, BS_BATTERY_LEVEL_UUID,
					gl_ble_bs_blc_struct_to_gba( &p_lserver->bs.blc ) );
			gl_log( GL_LOG_INFO, "[%s]: '%s' [%s] is notifying BS BLC\n", __func__,
					binc_adapter_get_name( p_lserver->adapter ), binc_adapter_get_address( p_lserver->adapter ));
		}
	}
	return( TRUE ); // Continue forever
}


/***********************************************************************
 *
 * Receives notify and puts in structure
*/
int gl_ble_bs_blc_receive_notify( const GByteArray *byteArray,  struct bs_blc_struct *p_blc )
{
	// Receive new data
	gl_ble_bs_blc_gba_to_struct( byteArray, p_blc );

	return( 0 );
}


/***********************************************************************
 *
 * Convert received gba to structure
*/
int gl_ble_bs_blc_gba_to_struct( const GByteArray *byteArray, struct bs_blc_struct *p_blc )
{
	Parser *parser;
	guint byte_offset;

	parser = parser_create(byteArray, LITTLE_ENDIAN);
	byte_offset = 0; // No flags
	parser_set_offset(parser, byte_offset);

	p_blc->percent = parser_get_uint8(parser);

	parser_free(parser);
	return(0);
}


/***********************************************************************
 *
 * Simulates draining battery
*/
void gl_ble_bs_blc_simulate( struct bs_blc_struct *p_blc )
{
	static uint8_t full = SIM_BATTERY_HI;
	if( full < SIM_BATTERY_LO ) full = SIM_BATTERY_HI;
	p_blc->percent = (uint8_t)full;
	full = full - (uint8_t) (SIM_BATTERY_HI * 0.1); // 10% drain
	return;
}

