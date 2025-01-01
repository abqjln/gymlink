/***********************************************************************
 *
 * gl_ble_cscs.c
 *
*/

#define TIRE_650_CIRCUMFERENCE_KM (1.920/1000.0)


#include <stdio.h>

#include "adapter.h"
#include "application.h"
#include "parser.h"

#include "gl_utilities.h"
#include "gl_structures.h"
#include "gl_ble_cscs.h"

int gl_cscs_mc_source = -1;  // 0=simulated data, 1=live data


/***********************************************************************
 *
 * Cycling Speed and Cadence Service (CSCS)
*/
int gl_ble_cscs_init( Application *application, struct cscs_struct *p_cscs )
{
	if( binc_application_add_service(application, CYCLING_SPEED_AND_CADENCE_SERVICE_UUID) != BINC_OK ){
		gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add service %s\n", __func__, CYCLING_SPEED_AND_CADENCE_SERVICE_UUID );
		return( -1 );
	}
	else{
		if( binc_application_add_characteristic(application, CYCLING_SPEED_AND_CADENCE_SERVICE_UUID, CSCS_MC_UUID,
				GATT_CHR_PROP_NOTIFY) != BINC_OK ){
			gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add service %s\n", __func__, CSCS_MC_UUID );
			return( -1 );
			// CCCD added automatically since specified NOTIFY
		}
		else{
			gl_log( GL_LOG_INFO, "[%s]:\t\tAdded CSCS Measurement Characteristic\n", __func__ );
		}

		if( binc_application_add_characteristic(application, CYCLING_SPEED_AND_CADENCE_SERVICE_UUID, CSCS_FC_UUID,
				GATT_CHR_PROP_READ) != BINC_OK ){
			gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add characteristic %s\n", __func__, CSCS_FC_UUID );
			return( -1 );
		}
		else{
			gl_log( GL_LOG_INFO, "[%s]:\t\tAdded CSCS Feature Characteristic\n", __func__ );
		}

		if( binc_application_add_characteristic(application, CYCLING_SPEED_AND_CADENCE_SERVICE_UUID, CSCS_SLC_UUID,
				GATT_CHR_PROP_READ) != BINC_OK ){
			gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add characteristic %s\n", __func__, CSCS_SLC_UUID );
			return( -1 );
		}
		else{
			gl_log( GL_LOG_INFO, "[%s]:\t\tAdded CSCS Sensor Location Characteristic\n", __func__ );
		}

		if( binc_application_add_characteristic(application, CYCLING_SPEED_AND_CADENCE_SERVICE_UUID, CSCS_CPC_UUID,
				GATT_CHR_PROP_WRITE | GATT_CHR_PROP_INDICATE) != BINC_OK ){
			gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add characteristic %s\n", __func__, CSCS_CPC_UUID );
			return( -1 );
		}
		else{
			gl_log( GL_LOG_INFO, "[%s]:\t\tAdded CSCS Control Point Characteristic\n", __func__ );
		}
	}

	gl_log( GL_LOG_INFO, "[%s]:\tAdded Cycling Speed and Cadence Service (CSCS)\n", __func__ );
	return( BINC_OK );
}


/***********************************************************************
 *
 * Free any allocations
*/
void gl_ble_cscs_free( struct cscs_struct *p_cscs ){
	// No pointers, nothing to do
	return;
}


/***********************************************************************
*
* Convert structure data to gba
*/
GByteArray* gl_ble_cscs_mc_struct_to_gba( struct cscs_mc_struct *p_mc )
{
	// Data generator set flags and data in struct
	GByteArray* byteArray;
	byteArray = g_byte_array_new();
	g_byte_array_prepend( byteArray, (guint8 *)&(p_mc->flags), sizeof(p_mc->flags) );

	if( p_mc->flags & CSCS_MC_WHEEL_REVS_PRESENT ){
		g_byte_array_append( byteArray, (guint8 *)&(p_mc->cumulative_wheel_revs), sizeof(p_mc->cumulative_wheel_revs) );
		g_byte_array_append( byteArray, (guint8 *)&(p_mc->last_wheel_event_time), sizeof(p_mc->last_wheel_event_time) );
	}

	if( p_mc->flags & CSCS_MC_CRANK_REVS_PRESENT ){
		g_byte_array_append( byteArray, (guint8 *)&(p_mc->cumulative_crank_revs), sizeof(p_mc->cumulative_crank_revs) );
		g_byte_array_append( byteArray, (guint8 *)&(p_mc->last_crank_event_time), sizeof(p_mc->last_crank_event_time) );
	}

	// printf( "CSCS MC characteristic: " );
	//for( int j = 0; j < byteArray->len; j++ ) printf( "%02x", *(byteArray->data+j) );
	//printf( " (length = %d)\n", byteArray->len );
	//fflush(stdout);

	return( byteArray );
}


/***********************************************************************
 *
 * Timed loop issues notify if enabled
*/
gboolean gl_ble_cscs_mc_send_notify( gpointer gptr )
{
	GL_LServer *p_lserver = (GL_LServer *)gptr;

	// Get updated data (either real or simulated)
	gl_ble_cscs_mc_update( &(p_lserver->cscs.mc) );


	if( p_lserver->app != NULL ){ // Catch asynchronous SIGINT
		if( binc_application_char_is_notifying( p_lserver->app, CYCLING_SPEED_AND_CADENCE_SERVICE_UUID, CSCS_MC_UUID) == TRUE ){
			binc_application_notify(p_lserver->app, CYCLING_SPEED_AND_CADENCE_SERVICE_UUID, CSCS_MC_UUID,
					gl_ble_cscs_mc_struct_to_gba( &p_lserver->cscs.mc ) );
			gl_log( GL_LOG_INFO, "[%s]:\t'%s' [%s] is notifying CSCS MC\n", __func__,
					binc_adapter_get_name( p_lserver->adapter ), binc_adapter_get_address( p_lserver->adapter ) );
		}
	}
	return( TRUE ); // Continue forever
}


/***********************************************************************
 *
 * Receives and parses CSCS MC, then computes cadence and speed
*/
int gl_ble_cscs_mc_receive_notify( const GByteArray *byteArray,  struct cscs_mc_struct *p_mc )
{
		// Get new data
		gl_ble_cscs_mc_gba_to_struct( byteArray, p_mc );

		// Compute speed and cadence only if new timestamp (preserves last value otherwise)
		if( p_mc->last_wheel_event_time != p_mc->prev_wheel_event_time ){
			p_mc->dspeed = ((60.0 * 60.0 * (double) CSCS_MC_WHEEL_EVENT_TIME_RESOLUTION) * TIRE_650_CIRCUMFERENCE_KM) *
				((double) p_mc->cumulative_wheel_revs - (double) p_mc->prev_cumulative_wheel_revs) /
					((double) p_mc->last_wheel_event_time - (double) p_mc->prev_wheel_event_time) ;
			// Save previous values
			p_mc->prev_cumulative_wheel_revs = p_mc->cumulative_wheel_revs;
			p_mc->prev_wheel_event_time = p_mc->last_wheel_event_time;
		}

		if( p_mc->last_crank_event_time != p_mc->prev_crank_event_time ){
			p_mc->dcadence = (uint16_t) (60.0 * (double) CSCS_MC_CRANK_EVENT_TIME_RESOLUTION) *
					((double) p_mc->cumulative_crank_revs - (double) p_mc->prev_cumulative_crank_revs) / \
					((double) p_mc->last_crank_event_time - (double) p_mc->prev_crank_event_time) ;
			// Save previous values
			p_mc->prev_cumulative_crank_revs = p_mc->cumulative_crank_revs;
			p_mc->prev_crank_event_time = p_mc->last_crank_event_time;
		}

		//printf( CYEL"[%s]:\tcadence = %3.1f\n", __func__, p_mc->dcadence);
	return( 0 );
}


/***********************************************************************
 *
 * Parses CSCS MC notify gba into structure
*/
int gl_ble_cscs_mc_gba_to_struct( const GByteArray *byteArray, struct cscs_mc_struct *p_mc )
{
	Parser *parser;
	guint byte_offset;

	parser = parser_create( byteArray, LITTLE_ENDIAN );
	byte_offset = 0; // Flags
	parser_set_offset(parser, byte_offset);
	p_mc->flags = parser_get_uint8(parser);
	byte_offset =  byte_offset + 1; // Advance to next possible value

	#if 1
	// LOG_DEBUG does this
	printf( "[%s]: Received CSCS MC: ", __func__ );
	for( guint j = 0; j < byteArray->len; j++ ) printf( "%02x ", *(byteArray->data+j) );
	printf( " (length = %d)\n", byteArray->len );
	fflush(stdout);
	#endif

	if( p_mc->flags & CSCS_MC_WHEEL_REVS_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_mc->cumulative_wheel_revs = parser_get_uint32(parser);
		byte_offset = byte_offset + 4;
		parser_set_offset(parser, byte_offset);
		p_mc->last_wheel_event_time = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
	}

	if( p_mc->flags & CSCS_MC_CRANK_REVS_PRESENT ){
		parser_set_offset(parser, byte_offset);
		p_mc->cumulative_crank_revs = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		parser_set_offset(parser, byte_offset);
		p_mc->last_crank_event_time = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
	}

	parser_free(parser);

	return( 0 );
}


/***********************************************************************
 *
 * Receives and parses CSCS CPC, then acts on opcode
 *
 * Passes cscs_struct since need to access mc from this cpc
*/
int gl_ble_cscs_cpc_gba_process( const GByteArray *byteArray, struct cscs_struct *p_cscs )
{
	Parser *parser;
	guint byte_offset;

	parser = parser_create( byteArray, LITTLE_ENDIAN );
	byte_offset = 0;
	parser_set_offset(parser, byte_offset);
	p_cscs->cpc.opcode = parser_get_uint8(parser);
	byte_offset =  byte_offset + 1; // Advance to next value

	if( p_cscs->cpc.opcode == CSCS_CPC_OPCODE_SET_CUMULATIVE_VALUE )
	{
		gl_log( GL_LOG_INFO, "[%s]: Processing cscs opcode\n", __func__ );
		parser_set_offset(parser, byte_offset);
		p_cscs->cpc.cumulative_value = parser_get_uint32(parser); // Get value
		p_cscs->mc.cumulative_wheel_revs = p_cscs->cpc.cumulative_value; // Set value
		byte_offset =  byte_offset + 4;

		// Set response
		p_cscs->cpc.opcode = CSCS_CPC_OPCODE_RESPONSE_CODE;
		// cumulative value set above
		// sensor location already set
		p_cscs->cpc.request_op_code = p_cscs->cpc.opcode;
		p_cscs->cpc.response_value = CSCS_CPC_RESPONSE_SUCCESS;
		p_cscs->cpc.response_parameter = 0;

		// Calling function will send indicate
	}
	else{
		gl_log( GL_LOG_ERROR, "[%s] Error: Unsupported CSCS control point op code %d\n", __func__, p_cscs->cpc.opcode );
		return( -1 );
	}

	return( 0 );
}

/***********************************************************************
 *
 * Converts structure data to gba for indication
*/
GByteArray* gl_ble_cscs_cpc_struct_to_gba( struct cscs_cpc_struct *p_cpc )
{
	GByteArray* byteArray;
	byteArray = g_byte_array_new();
	g_byte_array_append( byteArray, (guint8 *)&(p_cpc->opcode), sizeof(p_cpc->opcode) );
	g_byte_array_append( byteArray, (guint8 *)&(p_cpc->cumulative_value), sizeof(p_cpc->cumulative_value) );
	g_byte_array_append( byteArray, (guint8 *)&(p_cpc->sensor_location), sizeof(p_cpc->sensor_location) );
	g_byte_array_append( byteArray, (guint8 *)&(p_cpc->request_op_code), sizeof(p_cpc->request_op_code) );
	g_byte_array_append( byteArray, (guint8 *)&(p_cpc->response_value), sizeof(p_cpc->response_value) );
	g_byte_array_append( byteArray, (guint8 *)&(p_cpc->response_parameter), sizeof(p_cpc->response_parameter) );

	#if 1
	printf( "CSCS_CPC characteristic: " );
	for( guint j = 0; j < byteArray->len; j++ ) printf( "%02x", *(byteArray->data+j) );
	printf( " (length = %d)\n", byteArray->len );
	fflush(stdout);
	#endif

	return( byteArray );
}


/***********************************************************************
 *
 * Send indicate upon service change--not used in a timed loop
*/
int gl_ble_cscs_cpc_send_indicate( void *gptr )
{
	GL_LServer *p_lserver = (GL_LServer *)gptr; // Need to make this GL_LServer * as parm

	if( p_lserver->app != NULL ){ // Catch asynchronous SIGINT
		if( binc_application_char_is_notifying( p_lserver->app, CYCLING_SPEED_AND_CADENCE_SERVICE_UUID, CSCS_CPC_UUID) == TRUE ){
			binc_application_notify( p_lserver->app, CYCLING_SPEED_AND_CADENCE_SERVICE_UUID, CSCS_CPC_UUID,
					gl_ble_cscs_cpc_struct_to_gba( &p_lserver->cscs.cpc ) );
			gl_log( GL_LOG_INFO, "[%s]:\t'%s' [%s] is indicating CSCS CPC\n", __func__,
					binc_adapter_get_name( p_lserver->adapter ), binc_adapter_get_address( p_lserver->adapter ) );
		}
	}
	return( 0 );
}


/***********************************************************************
 *
 * Simulate cadence and speed using CSCS service
 *
 * Fenix8
*/
void gl_ble_cscs_mc_update( struct cscs_mc_struct *p_mc )
{
	static uint16_t count=0;
	//uint16_t dt;

	if( count == 0 ){
		// Initialize charact structure
		p_mc->cumulative_wheel_revs = 0;
		p_mc->last_wheel_event_time = 0;
		p_mc->cumulative_crank_revs = 0;
		p_mc->last_crank_event_time = 0;
		p_mc->prev_cumulative_wheel_revs = 0;
		p_mc->prev_cumulative_crank_revs = 0;
	}

#if 0 // Copy from CPS when it works
	// Wheel--increment one revolution and adjust time
	// Does not appear on Garmin so can't confirm correct
	p_mc->flags = p_mc->flags | CSCS_MC_WHEEL_REVS_PRESENT;
	p_mc->prev_cumulative_wheel_revs = p_mc->cumulative_wheel_revs;
	p_mc->cumulative_wheel_revs++;
	dt = (uint16_t) ((3600.0 * TIRE_650_CIRCUMFERENCE_KM) / (SIM_KPH_LO + (count % (SIM_KPH_HI - SIM_KPH_LO))) * CSCS_MC_WHEEL_EVENT_TIME_RESOLUTION);
	p_mc->prev_wheel_event_time = p_mc->last_wheel_event_time;
	p_mc->last_wheel_event_time = p_mc->last_wheel_event_time + dt;

	// Cadence--increment one revolution and adjust time
	// Confirm on Garmin
	p_mc->flags = p_mc->flags | CSCS_MC_CRANK_REVS_PRESENT;
	p_mc->prev_cumulative_crank_revs = p_mc->cumulative_crank_revs;
	p_mc->cumulative_crank_revs++;
	dt = (uint16_t) (60.0 / (SIM_CADENCE_LO + (count % (SIM_CADENCE_HI - SIM_CADENCE_LO))) * CSCS_MC_CRANK_EVENT_TIME_RESOLUTION);
	p_mc->prev_crank_event_time = p_mc->last_crank_event_time;
	p_mc->last_crank_event_time = p_mc->last_crank_event_time + dt;

#endif

	count++; // Use this for updating data

	return;
}

