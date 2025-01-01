/***********************************************************************
 *
 * gl_ble_rscs.c
 *
*/

#include <stdio.h>
#include <stdint.h>

#include "adapter.h"
#include "application.h"
#include "characteristic.h"
#include "parser.h"

#include "gl_utilities.h"
#include "gl_structures.h"
#include "gl_ble_rscs.h"


//******************************************************************
// Running Speed and Cadence Service (RSCS)
int gl_ble_rscs_init( Application *application, struct rscs_struct *p_rscs )
{
	if (binc_application_add_service(application, RUNNING_SPEED_AND_CADENCE_SERVICE_UUID) != BINC_OK) {
		 gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add service <%.8s>\n", __func__, RUNNING_SPEED_AND_CADENCE_SERVICE_UUID );
		return( -1 );
	}
	else{
		if (binc_application_add_characteristic(application, RUNNING_SPEED_AND_CADENCE_SERVICE_UUID, RSCS_MC_UUID,
				GATT_CHR_PROP_NOTIFY) != BINC_OK){
			 gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add service <%.8s>\n", __func__, RSCS_MC_UUID );
			return( -1 );
			// CCCD added automatically since specified NOTIFY
		}
		else gl_log( GL_LOG_INFO, "[%s]:\tAdded RSCS Measurement Characteristic <%.8s>\n", __func__, RSCS_MC_UUID);

		if (binc_application_add_characteristic(application, RUNNING_SPEED_AND_CADENCE_SERVICE_UUID, RSCS_FC_UUID,
				GATT_CHR_PROP_READ) != BINC_OK){
			 gl_log( GL_LOG_ERROR, "[%s] Error:\tFailed to add characteristic <%.8s>\n", __func__, RSCS_FC_UUID );
			return( -1 );
		}
		else gl_log( GL_LOG_INFO, "[%s]:\tAdded RSCS Feature Characteristic <%.8s>\n", __func__, RSCS_FC_UUID );

		if (binc_application_add_characteristic(application, RUNNING_SPEED_AND_CADENCE_SERVICE_UUID, RSCS_SLC_UUID,
				GATT_CHR_PROP_READ) != BINC_OK){
			 gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add characteristic <%.8s>", __func__, RSCS_SLC_UUID );
			return( -1 );
		}
		else gl_log( GL_LOG_INFO, "[%s]:\tAdded RSCS Sensor Location Characteristic <%.8s>\n", __func__ ,RSCS_SLC_UUID );

		if (binc_application_add_characteristic(application, RUNNING_SPEED_AND_CADENCE_SERVICE_UUID, RSCS_CPC_UUID,
				GATT_CHR_PROP_WRITE | GATT_CHR_PROP_INDICATE) != BINC_OK){
			 gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add characteristic <%.8s>\n", __func__, RSCS_CPC_UUID );
			return( -1 );
		}
		else gl_log( GL_LOG_INFO, "[%s]:\tAdded RSCS Control Point Characteristic <%.8s>\n", __func__, RSCS_CPC_UUID );

		gl_log( GL_LOG_INFO, "[%s]:\tAdded <%.8s> Running Speed and Cadence Service (RSCS)\n", __func__, RUNNING_SPEED_AND_CADENCE_SERVICE_UUID );
	}

	return( 0 );
}


//**********************************************************************
GByteArray* gl_ble_rscs_mc_struct_to_gba( struct rscs_mc_struct *p_mc )
{
	// Data generator set flags and data in struct
	GByteArray* byteArray;
	byteArray = g_byte_array_new();
	g_byte_array_prepend( byteArray, (guint8 *)&(p_mc->flags), sizeof(p_mc->flags) );

	g_byte_array_append( byteArray, (guint8 *)&(p_mc->instantaneous_speed), sizeof(p_mc->instantaneous_speed) );
	g_byte_array_append( byteArray, (guint8 *)&(p_mc->instantaneous_cadence), sizeof(p_mc->instantaneous_cadence) );

	if( p_mc->flags & RSCS_MC_INSTANTANEOUS_STRIDE_LENGTH_PRESENT ){
		g_byte_array_append( byteArray, (guint8 *)&(p_mc->instantaneous_stride_length), sizeof(p_mc->instantaneous_stride_length) );
	}

	if( p_mc->flags & RSCS_MC_TOTAL_DISTANCE_PRESENT ){
		g_byte_array_append( byteArray, (guint8 *)&(p_mc->total_distance), sizeof(p_mc->total_distance) );
	}

	#if 1
	printf( "RSCS characteristic: " );
	for( guint j = 0; j < byteArray->len; j++ ) printf( "%02x", *(byteArray->data+j) );
	printf( " (length = %d)\n", byteArray->len );
	fflush(stdout);
	#endif

	return( byteArray );
}


/***********************************************************************
 *
 * Parse GByteArray and store
*/
int gl_ble_rscs_mc_gba_to_struct( const GByteArray *byteArray, struct rscs_mc_struct *p_mc )
{
	Parser *parser;
	guint byte_offset;

	parser = parser_create( byteArray, LITTLE_ENDIAN );
	byte_offset = 0; // Flags
	parser_set_offset(parser, byte_offset);
	p_mc->flags = parser_get_uint8(parser);
	byte_offset =  byte_offset + 1; // Advance to next possible value

	// Always present
	parser_set_offset(parser, byte_offset);
	p_mc->instantaneous_speed = parser_get_uint16(parser);
	byte_offset =  byte_offset + 2;

	// Always present
	parser_set_offset(parser, byte_offset);
	p_mc->instantaneous_cadence = parser_get_uint8(parser);
	byte_offset =  byte_offset + 1;

	if( p_mc->flags & RSCS_MC_INSTANTANEOUS_STRIDE_LENGTH_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_mc->instantaneous_stride_length = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
	}

	if( p_mc->flags & RSCS_MC_TOTAL_DISTANCE_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_mc->total_distance = parser_get_uint32(parser);
		byte_offset = byte_offset + 4;
	}

	parser_free( parser );
	return( 0 );
}


/***********************************************************************
 *
 * Receives and parses data from notify characteristic, and computes cadence and speed
*/
int gl_ble_rscs_mc_receive_notify( const GByteArray *byteArray,  struct rscs_mc_struct *p_mc )
{
	// Put data into structure
	gl_ble_rscs_mc_gba_to_struct( byteArray, p_mc );

//	gl_log( GL_LOG_ALERT, "[%s]: Raw speed=%d, cadence=%d, stride=%d distance=%d\n", __func__,
//			p_mc->instantaneous_speed,
//			p_mc->instantaneous_cadence,
//			p_mc->instantaneous_stride_length,
//			p_mc->total_distance );
#if 0
	gl_log( GL_LOG_INFO, "[%s]: InstSpeed = %5.1f m/s, InstCadence = %5.1f steps/min, InstStride = %4.3f meters, Distance = %5.1f meters\n", __func__,
			(double) p_mc->instantaneous_speed / (double) RSCS_MC_SPEED_SCALE,
			(double) p_mc->instantaneous_cadence / (double) RSCS_MC_CADENCE_SCALE,
			(double) p_mc->instantaneous_stride_length / (double) RSCS_MC_STRIDE_LENGTH_SCALE,
			(double) p_mc->total_distance / (double) RSCS_MC_DISTANCE_SCALE );
#endif

	return( 0 );
}




/***********************************************************************
 *
 * Free any allocations
*/
void gl_ble_rscs_free( struct rscs_struct *p_rscs ){
	// No pointers, nothing to do
	return;
}


//**********************************************************************
#define CADENCE_HIGH (65) // 65 strides or 130 steps/min
#define CADENCE_LOW  (50) // 100 steps/min
#define SPEED_HIGH (15.0) // 4mph to m/s
#define SPEED_LOW  (1.0) // 3mph scaled
gboolean gl_ble_rscs_mc_simulate( gpointer gptr )
{
	static double speed = SPEED_LOW;
	static int cadence = CADENCE_LOW;

	struct rscs_mc_struct *p_mc = (struct rscs_mc_struct *)gptr;

	p_mc->flags = 0; // Stride and distance not present

	// Convert from mph to 1/256 m/s
//	p_mc->instantaneous_speed = (uint16_t) (speed * (1609.34/3600.0) * RSCS_MC_SPEED_SCALE);
	p_mc->instantaneous_speed = (uint16_t) (speed / RSCS_MC_SPEED_SCALE_MPH);
	speed = speed + 1.0;
	if( speed > SPEED_HIGH) speed = SPEED_LOW;

	p_mc->instantaneous_cadence = (uint8_t) (cadence / RSCS_MC_CADENCE_SCALE);
	cadence = cadence + 1;
	if( cadence > CADENCE_HIGH) cadence = CADENCE_LOW;

	gl_log( GL_LOG_INFO, "[%s]:\tNew data: speed %f, speed %d, cadence %4d\n", __func__,
			speed, p_mc->instantaneous_speed, p_mc->instantaneous_cadence );

	return( TRUE ); // Continue forever
}



/***********************************************************************
 *
 * Timed loop issues notify if enabled
*/
gboolean gl_ble_rscs_mc_send_notify( gpointer gptr )
{
	GL_LServer *p_lserver = (GL_LServer *)gptr; // Not the global, but a specific server

	#ifdef SIMDATA
		gl_ble_rscs_mc_simulate( &(p_lserver->rscs.mc) );
	#else
		// Get real data here
	#endif

	if( p_lserver->app != NULL ){ // Catch asynchronous SIGINT
		if( binc_application_char_is_notifying( p_lserver->app, RUNNING_SPEED_AND_CADENCE_SERVICE_UUID, RSCS_MC_UUID) == TRUE ){
			binc_application_notify( p_lserver->app, RUNNING_SPEED_AND_CADENCE_SERVICE_UUID, RSCS_MC_UUID,
					gl_ble_rscs_mc_struct_to_gba( &p_lserver->rscs.mc ) );
			gl_log( GL_LOG_INFO, "[%s]:\t'%s' [%s] is notifying RSCS MC\n", __func__,
					binc_adapter_get_name( p_lserver->adapter ), binc_adapter_get_address( p_lserver->adapter) );
			}
	}
	return( TRUE ); // Continue forever
}


