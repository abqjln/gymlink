/***********************************************************************
 *
 * gl_ble_ftms.c
 *
*/

#include <stdio.h>
#include <stdint.h>

#include "adapter.h"
#include "application.h"
#include "characteristic.h"
#include "parser.h"
#include "utility.h"

#include "gl_utilities.h"
#include "gl_structures.h"
#include "gl_ble_ftms.h"

#if 0
//******************************************************************
// Fitness Machine Service (FTMS)
int gl_ble_ftms_init( Application *application, struct rscs_struct *p_rscs )
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
GByteArray* gl_ble_ftms_tdc_struct_to_gba( struct rscs_mc_struct *p_tdc )
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
#endif


/***********************************************************************
 *
 * Parse GByteArray and store
*/
int gl_ble_ftms_tdc_gba_to_struct( const GByteArray *byteArray, struct ftms_tdc_struct *p_tdc )
{
	Parser *parser;
	guint byte_offset;

	parser = parser_create( byteArray, LITTLE_ENDIAN );
	byte_offset = 0; // Flags
	parser_set_offset(parser, byte_offset);
	p_tdc->flags = parser_get_uint16(parser);
	byte_offset =  byte_offset + 2; // Advance to next possible value

	// Always present
	parser_set_offset(parser, byte_offset);
	p_tdc->instantaneous_speed = parser_get_uint16(parser);
	byte_offset =  byte_offset + 2;
	//gl_log( GL_LOG_DEBUG, "[%s: raw instantaneous_speed=%d\n", __func__, p_tdc->instantaneous_speed );

	if( p_tdc->flags & FTMS_TDC_AVERAGE_SPEED_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_tdc->average_speed = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		//gl_log( GL_LOG_DEBUG, "[%s]: raw average_speed=%d\n", __func__, p_tdc->average_speed );
	}

	if( p_tdc->flags & FTMS_TDC_TOTAL_DISTANCE_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_tdc->total_distance[0] = parser_get_uint8(parser);
		byte_offset = byte_offset + 1;
		p_tdc->total_distance[1] = parser_get_uint8(parser);
		byte_offset = byte_offset + 1;
		p_tdc->total_distance[2] = parser_get_uint8(parser);
		byte_offset = byte_offset + 1;
		//gl_log( GL_LOG_DEBUG, "[%s]: raw total_distance=%d\n", __func__, (p_tdc->total_distance[0] + (p_tdc->total_distance[1]<<8) + (p_tdc->total_distance[2]<<8)) );
	}

	if( p_tdc->flags & FTMS_TDC_RAMP_INCLINATION_ANGLE_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_tdc->ramp_inclination = parser_get_sint16(parser);
		byte_offset = byte_offset + 2;
		p_tdc->ramp_angle = parser_get_sint16(parser);
		byte_offset = byte_offset + 2;
		//gl_log( GL_LOG_DEBUG, "[%s]: raw ramp_inclination=%d, ramp_angle=%d\n", __func__, p_tdc->ramp_inclination, p_tdc->ramp_angle );
	}

	if( p_tdc->flags & FTMS_TDC_ELEVATION_GAIN_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_tdc->pos_elevation_gain = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		p_tdc->neg_elevation_gain = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		//gl_log( GL_LOG_DEBUG, "[%s]: raw +gain=%d, -gain=%d\n", __func__, p_tdc->pos_elevation_gain, p_tdc->neg_elevation_gain );
	}

	if( p_tdc->flags & FTMS_TDC_INSTANTANEOUS_PACE_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_tdc->instantaneous_pace = parser_get_uint8(parser);
		byte_offset = byte_offset + 1;
		//gl_log( GL_LOG_DEBUG, "[%s]: raw instantaneous_pace=%d\n", __func__, p_tdc->instantaneous_pace );
	}

	if( p_tdc->flags & FTMS_TDC_AVERAGE_PACE_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_tdc->average_pace = parser_get_uint8(parser);
		byte_offset = byte_offset + 1;
		//gl_log( GL_LOG_DEBUG, "[%s]: raw avgerage_pace=%d\n", __func__, p_tdc->average_pace );
	}

	if( p_tdc->flags & FTMS_TDC_ENERGY_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_tdc->energy_total = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		p_tdc->energy_per_hour = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		p_tdc->energy_per_min = parser_get_uint8(parser);
		byte_offset = byte_offset + 1;
		//gl_log( GL_LOG_DEBUG, "[%s]: raw energy=%d, e/hr=%d, e/min=%d\n", __func__, p_tdc->energy_total, p_tdc->energy_per_hour, p_tdc->energy_per_min );
	}

	if( p_tdc->flags & FTMS_TDC_HEART_RATE_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_tdc->heart_rate = parser_get_uint8(parser);
		byte_offset = byte_offset + 1;
		//gl_log( GL_LOG_DEBUG, "[%s]: raw bpm=%d\n", __func__, p_tdc->heart_rate );
	}

	if( p_tdc->flags & FTMS_TDC_MET_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_tdc->met = parser_get_uint8(parser);
		byte_offset = byte_offset + 1;
		//gl_log( GL_LOG_DEBUG, "[%s]:raw  met=%d\n", __func__, p_tdc->met );
	}

	if( p_tdc->flags & FTMS_TDC_ELAPSED_TIME_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_tdc->elapsed_time = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		//gl_log( GL_LOG_DEBUG, "[%s]: raw elapsed_time=%d\n", __func__, p_tdc->elapsed_time );
	}

	if( p_tdc->flags & FTMS_TDC_REMAINING_TIME_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_tdc->remaining_time = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		//gl_log( GL_LOG_DEBUG, "[%s]: raw remaining_time=%d\n", __func__, p_tdc->remaining_time );
	}

	if( p_tdc->flags & FTMS_TDC_BELT_FORCE_POWER_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_tdc->belt_force = parser_get_sint16(parser);
		byte_offset = byte_offset + 2;
		p_tdc->belt_power = parser_get_sint16(parser);
		byte_offset = byte_offset + 2;
		//gl_log( GL_LOG_DEBUG, "[%s]: raw belt_force=%d, belt_power=%d\n", __func__, p_tdc->belt_force, p_tdc->belt_power );
	}

	parser_free( parser );
	return( 0 );
}


/***********************************************************************
 *
 * Receives and parses data from notify characteristic, and computes cadence and speed
*/
int gl_ble_ftms_tdc_receive_notify( const GByteArray *byteArray,  struct ftms_tdc_struct *p_tdc )
{

//	printf( "TDC characteristic: <" );
//	for( guint j = 0; j < byteArray->len; j++ ) printf( "%02x ", *(byteArray->data+j) );
//	printf( "> (length = %d)\n", byteArray->len );

	// Put data into structure
	gl_ble_ftms_tdc_gba_to_struct( byteArray, p_tdc );

	return( 0 );
}


/***********************************************************************
 *
 * Parse GByteArray and store
*/
int gl_ble_ftms_ibdc_gba_to_struct( const GByteArray *byteArray, struct ftms_ibdc_struct *p_ibdc )
{
	Parser *parser;
	guint byte_offset;

	parser = parser_create( byteArray, LITTLE_ENDIAN );
	byte_offset = 0; // Flags
	parser_set_offset(parser, byte_offset);
	p_ibdc->flags = parser_get_uint16(parser);
	byte_offset =  byte_offset + 2; // Advance to next possible value

	// Always present
	parser_set_offset(parser, byte_offset);
	p_ibdc->instantaneous_speed = parser_get_uint16(parser);
	byte_offset =  byte_offset + 2;
	gl_log( GL_LOG_DEBUG, "[%s: raw instantaneous_speed=%d\n", __func__, p_ibdc->instantaneous_speed );

	if( p_ibdc->flags & FTMS_IBDC_AVERAGE_SPEED_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_ibdc->average_speed = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		gl_log( GL_LOG_DEBUG, "[%s]: raw average_speed=%d\n", __func__, p_ibdc->average_speed );
	}

	if( p_ibdc->flags & FTMS_IBDC_INSTANTANEOUS_CADENCE_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_ibdc->instantaneous_cadence = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		gl_log( GL_LOG_DEBUG, "[%s: raw instantaneous cadence=%d\n", __func__, p_ibdc->instantaneous_cadence );
	}

	if( p_ibdc->flags & FTMS_IBDC_AVERAGE_CADENCE_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_ibdc->average_cadence = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		gl_log( GL_LOG_DEBUG, "[%s: raw average cadence=%d\n", __func__, p_ibdc->average_cadence );
	}

	if( p_ibdc->flags & FTMS_IBDC_TOTAL_DISTANCE_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_ibdc->total_distance[0] = parser_get_uint8(parser);
		byte_offset = byte_offset + 1;
		p_ibdc->total_distance[1] = parser_get_uint8(parser);
		byte_offset = byte_offset + 1;
		p_ibdc->total_distance[2] = parser_get_uint8(parser);
		byte_offset = byte_offset + 1;
		gl_log( GL_LOG_DEBUG, "[%s]: raw total_distance=%d\n", __func__, (p_ibdc->total_distance[0] + (p_ibdc->total_distance[1]<<8) + (p_ibdc->total_distance[2]<<8)) );
	}

	if( p_ibdc->flags & FTMS_IBDC_RESISTANCE_LEVEL_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_ibdc->resistance_level = parser_get_sint16(parser);
		byte_offset = byte_offset + 2;
		gl_log( GL_LOG_DEBUG, "[%s]: raw resistance=%d\n", __func__, p_ibdc->resistance_level );
	}

	if( p_ibdc->flags & FTMS_IBDC_INSTANTANEOUS_POWER_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_ibdc->instantaneous_power = parser_get_sint16(parser);
		byte_offset = byte_offset + 2;
		gl_log( GL_LOG_DEBUG, "[%s]: raw instantaneous_power=%d\n", __func__, p_ibdc->instantaneous_power );
	}

	if( p_ibdc->flags & FTMS_IBDC_AVERAGE_POWER_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_ibdc->average_power = parser_get_sint16(parser);
		byte_offset = byte_offset + 2;
		gl_log( GL_LOG_DEBUG, "[%s]: raw average_power=%d\n", __func__, p_ibdc->average_power );
	}

	if( p_ibdc->flags & FTMS_IBDC_EXP_ENERGY_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_ibdc->energy_total = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		p_ibdc->energy_per_hour = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		p_ibdc->energy_per_min = parser_get_uint8(parser);
		byte_offset = byte_offset + 1;
		gl_log( GL_LOG_DEBUG, "[%s]: raw energy=%d, e/hr=%d, e/min=%d\n", __func__, p_ibdc->energy_total, p_ibdc->energy_per_hour, p_ibdc->energy_per_min );
	}

	if( p_ibdc->flags & FTMS_IBDC_HEART_RATE_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_ibdc->heart_rate = parser_get_uint8(parser);
		byte_offset = byte_offset + 1;
		gl_log( GL_LOG_DEBUG, "[%s]: raw bpm=%d\n", __func__, p_ibdc->heart_rate );
	}

	if( p_ibdc->flags & FTMS_IBDC_MET_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_ibdc->met = parser_get_uint8(parser);
		byte_offset = byte_offset + 1;
		gl_log( GL_LOG_DEBUG, "[%s]:raw  met=%d\n", __func__, p_ibdc->met );
	}

	if( p_ibdc->flags & FTMS_IBDC_ELAPSED_TIME_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_ibdc->elapsed_time = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		gl_log( GL_LOG_DEBUG, "[%s]: raw elapsed_time=%d\n", __func__, p_ibdc->elapsed_time );
	}

	if( p_ibdc->flags & FTMS_IBDC_REMAINING_TIME_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_ibdc->remaining_time = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		gl_log( GL_LOG_DEBUG, "[%s]: raw remaining_time=%d\n", __func__, p_ibdc->remaining_time );
	}

	parser_free( parser );
	return( 0 );
}


/***********************************************************************
 *
 * Receives and parses data from notify characteristic, and computes cadence and speed
*/
int gl_ble_ftms_ibdc_receive_notify( const GByteArray *byteArray,  struct ftms_ibdc_struct *p_ibdc )
{

//	printf( "TDC characteristic: <" );
//	for( guint j = 0; j < byteArray->len; j++ ) printf( "%02x ", *(byteArray->data+j) );
//	printf( "> (length = %d)\n", byteArray->len );

	// Put data into structure
	gl_ble_ftms_ibdc_gba_to_struct( byteArray, p_ibdc );

	return( 0 );
}



































































#if 0
/***********************************************************************
 *
 * Free any allocations
*/
void gl_ble_ftms_free( struct rscs_struct *p_ftms ){
	// No pointers, nothing to do
	return;
}


//**********************************************************************
#define CADENCE_HIGH (65) // 65 strides or 130 steps/min
#define CADENCE_LOW  (50) // 100 steps/min
#define SPEED_HIGH (15.0) // 4mph to m/s
#define SPEED_LOW  (1.0) // 3mph scaled
gboolean gl_ble_ftms_tdc_simulate( gpointer gptr )
{
	static double speed = SPEED_LOW;
	static int cadence = CADENCE_LOW;

	struct rscs_mc_struct *p_mc = (struct rscs_mc_struct *)gptr;

	p_mc->flags = 0; // Stride and distance not present

	// Convert from mph to 1/256 m/s
	p_mc->instantaneous_speed = (uint16_t) (speed * (1609.34/3600.0) * RSCS_MC_SPEED_SCALE);
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
gboolean gl_ble ftms_tdc_send_notify( gpointer gptr )
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


#endif
