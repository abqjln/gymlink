/***********************************************************************
 *
 * gl_ble_cps.c
 *
*/

#include <stdio.h>

#include "adapter.h"
#include "application.h"
#include "device.h"
#include "parser.h"
#include "logger.h"

#include "gl_colors.h"
#include "gl_utilities.h"
#include "gl_config.h"
#include "gl_structures.h"
#include "gl_client.h"
#include "gl_on_server.h"
#include "gl_ble_cps.h"

int gl_cps_mc_simulate = FALSE;


/***********************************************************************
 *
 * Initialize CPS (Cycling Power Service)
*/
int gl_ble_cps_init( Application *application, struct cps_struct *p_cps )
{
	if( binc_application_add_service(application, CYCLING_POWER_SERVICE_UUID) != BINC_OK ){
		gl_log( GL_LOG_ERROR, "[%s}: Failed to add service %s\n", __func__, CYCLING_POWER_SERVICE_UUID );
		return( -1 );
	}
	else{
		gl_log( GL_LOG_INFO, "[%s]: <%.8s> CPS Cycling Power Service\n", __func__, CYCLING_POWER_SERVICE_UUID );

		if( binc_application_add_characteristic(application, CYCLING_POWER_SERVICE_UUID, CPS_MC_UUID,
				GATT_CHR_PROP_NOTIFY) != BINC_OK ){
			gl_log( GL_LOG_ERROR, "[%s}: Failed to add <%.8s>:<%.8s>\n", __func__, CYCLING_POWER_SERVICE_UUID, CPS_MC_UUID );
			return( -1 );
			// CCCD added automatically since specified NOTIFY
		}
		else{
			gl_log( GL_LOG_INFO, "[%s]:\t<%.8s>:<%.8s> CPS Cycling Power Measurement\n", __func__, CYCLING_POWER_SERVICE_UUID, CPS_MC_UUID );
		}

		if( binc_application_add_characteristic(application, CYCLING_POWER_SERVICE_UUID, CPS_FC_UUID,
				GATT_CHR_PROP_READ) != BINC_OK ){
			gl_log( GL_LOG_ERROR, "[%s}: Failed to add <%.8s>:<%.8s>\n", __func__, CYCLING_POWER_SERVICE_UUID, CPS_FC_UUID );
			return( -1 );
		}
		else{
			// Do this in main cpf.flags = CPS_CPF_FLAGS; // Supported features
			gl_log( GL_LOG_INFO, "[%s]:\t<%.8s>:<%.8s> CPS Cycling Power Feature\n", __func__, CYCLING_POWER_SERVICE_UUID, CPS_FC_UUID );
		}

		if( binc_application_add_characteristic(application, CYCLING_POWER_SERVICE_UUID, CPS_SLC_UUID,
				GATT_CHR_PROP_READ) != BINC_OK) {
			gl_log( GL_LOG_ERROR, "[%s}: Failed to add  <%.8s>:<%.8s>", __func__, CYCLING_POWER_SERVICE_UUID, CPS_SLC_UUID );
			return( -1 );
		}
		else{
			gl_log( GL_LOG_INFO, "[%s]:\t<%.8s>:<%.8s> CPS Sensor Location\n", __func__, CYCLING_POWER_SERVICE_UUID, CPS_SLC_UUID );
		}

		if( binc_application_add_characteristic(application, CYCLING_POWER_SERVICE_UUID, CPS_CPC_UUID,
				GATT_CHR_PROP_WRITE | GATT_CHR_PROP_INDICATE) != BINC_OK) {
			gl_log( GL_LOG_ERROR, "[%s}: Failed to add  <%.8s>:<%.8s>", __func__, CYCLING_POWER_SERVICE_UUID, CPS_CPC_UUID );
			return( -1 );
		}
		else{
			gl_log( GL_LOG_INFO, "[%s]:\t<%.8s>:<%.8s> CPS Control Point\n", __func__, CYCLING_POWER_SERVICE_UUID, CPS_CPC_UUID );
		}

		if( binc_application_add_characteristic(application, CYCLING_POWER_SERVICE_UUID, CPS_CPV_UUID,
				GATT_CHR_PROP_NOTIFY) != BINC_OK) {
			gl_log( GL_LOG_ERROR, "[%s}: Failed to add  <%.8s>:<%.8s>", __func__, CYCLING_POWER_SERVICE_UUID, CPS_CPV_UUID );
			return( -1 );
		}
		else{
			gl_log( GL_LOG_INFO, "[%s]:\t<%.8s>:<%.8s> CPS Cycling Power Vector\n", __func__, CYCLING_POWER_SERVICE_UUID, CPS_CPV_UUID );
		}

		return( BINC_OK );
	}
}


/***********************************************************************
 *
 * Free any allocations
*/
void gl_ble_cps_free( struct cps_struct *p_cps ){
	// No pointers in structure, nothing to do
	return;
}


/***********************************************************************
 *
 * Convert Measurement Characteristic structure data to gba
*/
GByteArray* gl_ble_cps_mc_struct_to_gba( struct cps_mc_struct *p_mc )
{
	// Assume both flags and data are set in struct
	GByteArray* byteArray;
	byteArray = g_byte_array_new();
	g_byte_array_prepend( byteArray, (guint8 *)&(p_mc->flags), sizeof(p_mc->flags) );
	g_byte_array_append( byteArray, (guint8 *)&(p_mc->instantaneous_power), sizeof(p_mc->instantaneous_power) );

	if( CPS_FC_ACCUMULATED_TORQUE_SUPPORTED ){
		if( p_mc->flags & CPS_MC_ACCUMULATED_TORQUE_PRESENT ) g_byte_array_append( byteArray, (guint8 *)&(p_mc->accumulated_torque), sizeof(p_mc->accumulated_torque) );
	}

	if( CPS_FC_WHEEL_REVOLUTION_DATA_SUPPORTED ){
		if( p_mc->flags & CPS_MC_WHEEL_REVOLUTION_DATA_PRESENT ){
			g_byte_array_append( byteArray, (guint8 *)&(p_mc->cumulative_wheel_revs), sizeof(p_mc->cumulative_wheel_revs) );
			g_byte_array_append( byteArray, (guint8 *)&(p_mc->last_wheel_event_time), sizeof(p_mc->last_wheel_event_time) );
		}
	}

	if( CPS_FC_CRANK_REVOLUTION_DATA_SUPPORTED ){
		if( p_mc->flags & CPS_MC_CRANK_REVOLUTION_DATA_PRESENT ){
			g_byte_array_append( byteArray, (guint8 *)&(p_mc->cumulative_crank_revs), sizeof(p_mc->cumulative_crank_revs) );
			g_byte_array_append( byteArray, (guint8 *)&(p_mc->last_crank_event_time), sizeof(p_mc->last_crank_event_time) );
		}
	}

	if( CPS_FC_ACCUMULATED_ENERGY_SUPPORTED ){
	if( p_mc->flags & CPS_MC_ACCUMULATED_ENERGY_PRESENT ) g_byte_array_append( byteArray, (guint8 *)&(p_mc->accumulated_energy), sizeof(p_mc->accumulated_energy) );
}

//	Finish for completeness but likely won't use
// g_byte_array_append( byteArray, (guint8 *)&(cpm.ext_force_max), sizeof(cpm.ext_force_max) );
//	g_byte_array_append( byteArray, (guint8 *)&(cpm.ext_force_min), sizeof(cpm.ext_force_min) );
//	g_byte_array_append( byteArray, (guint8 *)&(cpm.ext_torque_max), sizeof(cpm.ext_torque_max) );
//	g_byte_array_append( byteArray, (guint8 *)&(cpm.ext_torque_min), sizeof(cpm.ext_torque_min) );
//	g_byte_array_append( byteArray, (guint8 *)&(cpm.ext_angles), sizeof(cpm.ext_angles) );
//	g_byte_array_append( byteArray, (guint8 *)&(cpm.top_dead_angle), sizeof(cpm.top_dead_angle) );
//	g_byte_array_append( byteArray, (guint8 *)&(cpm.bot_dead_angle), sizeof(cpm.bot_dead_angle) );

	#if 0
	printf( "CPS values: CumWheelRevs=%d, LastWheelTime=%d, CumCrankRevs=%d, LastCrankTime=%d\n",
			p_mc->cumulative_wheel_revs, p_mc->last_wheel_event_time, p_mc->cumulative_crank_revs, p_mc->last_crank_event_time );
	printf( "CPS_MC characteristic: " );
	for( guint j = 0; j < byteArray->len; j++ ) printf( "%02x ", *(byteArray->data+j) );
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
gboolean gl_ble_cps_mc_send_notify( gpointer gptr )
{
	GL_LServer *p_lserver = (GL_LServer *)gptr;

	// Get updated data (either real or simulated)
	gl_ble_cps_mc_update( &(p_lserver->cps.mc) );

	if( p_lserver->app != NULL ){ // Catch asynchronous SIGINT
		if( (binc_application_char_is_notifying(p_lserver->app, CYCLING_POWER_SERVICE_UUID, CPS_MC_UUID) == TRUE) &&
			(p_lserver->connection_state == BINC_CONNECTED) ){
			binc_application_notify(p_lserver->app, CYCLING_POWER_SERVICE_UUID, CPS_MC_UUID,
					gl_ble_cps_mc_struct_to_gba( &p_lserver->cps.mc ) );

		gl_log( GL_LOG_DEBUG, "[%s]: notify=%d, connected=%d: <%.8s> : (WheelRev=%6d, WheelTime=%6d, CrankRev=%6d, CrankTtime=%6d)\tWheelRevs/min=%4.1f, RPM=%4d, W=%4d %s\n",
				__func__,
				binc_application_char_is_notifying(p_lserver->app, CYCLING_POWER_SERVICE_UUID, CPS_MC_UUID),
				p_lserver->connection_state,
				CPS_MC_UUID,
				p_lserver->cps.mc.cumulative_wheel_revs,
				p_lserver->cps.mc.last_wheel_event_time,
				p_lserver->cps.mc.cumulative_crank_revs,
				p_lserver->cps.mc.last_crank_event_time,
				p_lserver->cps.mc.wheel_cadence,
				p_lserver->cps.mc.cadence,
				p_lserver->cps.mc.instantaneous_power,
				(gl_cps_mc_simulate ? "(simulated)" : "") );

		}
		else{
			//gl_log( GL_LOG_DEBUG, "[%s]: notify=%d, connected=%d\n",
			//		__func__,
			//		binc_application_char_is_notifying(p_lserver->app,
			//		CYCLING_POWER_SERVICE_UUID, CPS_MC_UUID), p_lserver->connection_state );
		}
	}

	return( TRUE ); // Continue forever
}


/***********************************************************************
 *
 * Receives notify, parses gba into struct, and computes cadence and wheel_cadence
*/
int gl_ble_cps_mc_receive_notify( const GByteArray *byteArray,  struct cps_mc_struct *p_mc )
{
	// Get new data
	gl_ble_cps_mc_gba_to_struct( byteArray, p_mc );

	// Compute wheel_cadence and cadence only if new timestamp (preserves last value otherwise)
	if( p_mc->last_wheel_event_time != p_mc->prev_wheel_event_time ){
		p_mc->wheel_cadence = (uint16_t) (((60.0 * 60.0 * (double) CPS_MC_WHEEL_EVENT_TIME_RESOLUTION) * p_mc->tire_circumference_km) *
				((double) p_mc->cumulative_wheel_revs - (double) p_mc->prev_cumulative_wheel_revs) /
				((double) p_mc->last_wheel_event_time - (double) p_mc->prev_wheel_event_time)) ;
		// Save previous values
		p_mc->prev_cumulative_wheel_revs = p_mc->cumulative_wheel_revs;
		p_mc->prev_wheel_event_time = p_mc->last_wheel_event_time;
	}

	if( p_mc->last_crank_event_time != p_mc->prev_crank_event_time ){
		p_mc->cadence = (uint16_t) ((60.0 * (double) CPS_MC_CRANK_EVENT_TIME_RESOLUTION) *
				((double) p_mc->cumulative_crank_revs - (double) p_mc->prev_cumulative_crank_revs) / \
				((double) p_mc->last_crank_event_time - (double) p_mc->prev_crank_event_time)) ;
		// Save previous values
		p_mc->prev_cumulative_crank_revs = p_mc->cumulative_crank_revs;
		p_mc->prev_crank_event_time = p_mc->last_crank_event_time;
	}

	//printf( CYEL"[%s]:\tcadence = %3.1f\n", __func__, p_mc->dcadence);
	return( 0 );
}


/***********************************************************************
 *
 * Parses gba into structure based on flags
*/
int gl_ble_cps_mc_gba_to_struct( const GByteArray *byteArray, struct cps_mc_struct *p_mc )
{
	Parser *parser;
	guint byte_offset;

	#if 0
	// LOG_DEBUG does this
	printf( CYEL"[%s]:\tReceived CPS_MC: ", __func__ );
	for( guint j = 0; j < byteArray->len; j++ ) printf( "%02x ", *(byteArray->data+j) );
	printf( " (length = %d)\n", byteArray->len );
	fflush(stdout);
	#endif

	parser = parser_create( byteArray, LITTLE_ENDIAN );
	byte_offset = 0;
	parser_set_offset(parser, byte_offset);

	p_mc->flags = parser_get_uint16(parser);
	byte_offset = byte_offset + 2; // Advance to next possible value
	parser_set_offset(parser, byte_offset);
	//printf( "flags 0x%x\n", p_mc->flags );

	p_mc->instantaneous_power = parser_get_sint16(parser);
	byte_offset =  byte_offset + 2;
	parser_set_offset(parser, byte_offset);
	//printf( "power %d\n", p_mc->instantaneous_power );

	if( p_mc->flags & CPS_MC_PEDAL_POWER_BAL_PRESENT ){
		p_mc->pedal_power_balance = parser_get_uint8(parser);
		byte_offset = byte_offset + 1;
		parser_set_offset(parser, byte_offset);
		//printf( "pedal\n" );
	}

	if( p_mc->flags & CPS_MC_ACCUMULATED_TORQUE_PRESENT ){
		p_mc->accumulated_torque = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		parser_set_offset(parser, byte_offset);
		//printf( "torque\n" );
	}

	if( p_mc->flags & CPS_MC_WHEEL_REVOLUTION_DATA_PRESENT ){
		p_mc->cumulative_wheel_revs = parser_get_uint32(parser);
		byte_offset = byte_offset + 4;
		parser_set_offset(parser, byte_offset);

		p_mc->last_wheel_event_time = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		parser_set_offset(parser, byte_offset);
		//printf( CYEL"[%s]:\twheelrevs %d, wheeltime %d\n", __func__, p_mc->cumulative_wheel_revs, p_mc->last_wheel_event_time);
	}

	if( p_mc->flags & CPS_MC_CRANK_REVOLUTION_DATA_PRESENT ){
		p_mc->cumulative_crank_revs = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		parser_set_offset(parser, byte_offset);

		p_mc->last_crank_event_time = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		parser_set_offset(parser, byte_offset);
		//printf( CYEL"[%s]:\tcrankrevs %d, cranktime %d\n", __func__, p_mc->cumulative_crank_revs, p_mc->last_crank_event_time);
	}

	if( p_mc->flags & CPS_MC_EXTREME_FORCE_MAGNITUDES_PRESENT ){
		p_mc->maximum_force_magnitude = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		parser_set_offset(parser, byte_offset);

		p_mc->minimum_force_magnitude = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		parser_set_offset(parser, byte_offset);
		//printf( "ext forces\n" );
	}

	if( p_mc->flags & CPS_MC_EXTREME_TORQUE_MAGNITUDES_PRESENT ){
		p_mc->maximum_torque_magnitude = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		parser_set_offset(parser, byte_offset);

		p_mc->minimum_torque_magnitude = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		parser_set_offset(parser, byte_offset);
		//printf( "torques extreme\n" );
	}

	if( p_mc->flags & CPS_MC_EXTREME_ANGLES_PRESENT ){
		p_mc->max_min_angle[0] = parser_get_uint8(parser);
		byte_offset = byte_offset + 1;
		parser_set_offset(parser, byte_offset);

		p_mc->max_min_angle[1] = parser_get_uint8(parser);
		byte_offset = byte_offset + 1;
		parser_set_offset(parser, byte_offset);

		p_mc->max_min_angle[2] = parser_get_uint8(parser);
		byte_offset = byte_offset + 1;
		parser_set_offset(parser, byte_offset);
		//printf( "ext angles\n" );
	}

	if( p_mc->flags & CPS_MC_TOP_DEAD_SPOT_ANGLE_PRESENT ){
		p_mc->top_dead_spot_angle = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		parser_set_offset(parser, byte_offset);
		//printf( "tds\n" );
	}

	if( p_mc->flags & CPS_MC_BOTTOM_DEAD_SPOT_ANGLE_PRESENT ){
		p_mc->bottom_dead_spot_angle = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		parser_set_offset(parser, byte_offset);
		//printf( "bds\n" );
	}

	if( p_mc->flags & CPS_MC_ACCUMULATED_ENERGY_PRESENT ){
		p_mc->accumulated_energy = parser_get_uint16(parser);
		byte_offset = byte_offset + 2;
		parser_set_offset(parser, byte_offset);
		//printf( "energy\n" );
	}

	parser_free(parser);

	return( 0 );
}







/***********************************************************************
 *
 * Parses opcode, performs action, and sends indicate
 *
*/
int gl_ble_cps_cpc_process_indicate( const GByteArray *opcodeArray, void *gptr )
{
	Parser *parser;
	uint16_t byte_offset;
	GByteArray *responseArray;

	GL_LServer *p_lserver = (GL_LServer *)gptr; // Need to make this GL_LServer * as parm

	parser = parser_create( opcodeArray, LITTLE_ENDIAN );
	byte_offset = 0;
	parser_set_offset(parser, byte_offset);
	p_lserver->cps.cpc.request_opcode = parser_get_uint8(parser);
	byte_offset =  byte_offset + 1; // Advance to request_opcode

	if( p_lserver->cps.cpc.request_opcode == CPS_CPC_OPCODE_SET_CUMULATIVE_VALUE )
	{
		parser_set_offset(parser, byte_offset);
		p_lserver->cps.cpc.cumulative_wheel_revs_value = parser_get_uint32(parser);
		gl_log( GL_LOG_INFO, "[%s]: Received request from [%s] to set cumulative wheel revs value to %d\n", __func__, binc_device_get_address( p_lserver->central_device ),
				p_lserver->cps.cpc.cumulative_wheel_revs_value );

		// Set response in this order
		responseArray = g_byte_array_new();
		p_lserver->cps.cpc.response_opcode = CPS_CPC_OPCODE_RESPONSE_CODE;
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.response_opcode), sizeof(p_lserver->cps.cpc.response_opcode) );
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.request_opcode), sizeof(p_lserver->cps.cpc.request_opcode) );
		p_lserver->cps.cpc.response_value = CPS_CPC_RESPONSE_SUCCESS;
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.response_value), sizeof(p_lserver->cps.cpc.response_value) );
		// NO RESPONSE PARAMETER
	}

	else if( p_lserver->cps.cpc.request_opcode == CPS_CPC_OPCODE_SET_CRANK_LENGTH )
	{
		parser_set_offset(parser, byte_offset);
		p_lserver->cps.cpc.crank_length = parser_get_uint16(parser); // Cache if needed
		gl_log( GL_LOG_INFO, "[%s]: Received request from [%s] to set crank length to %5.1f mm (%d)\n", __func__, binc_device_get_address( p_lserver->central_device ),
				(double) p_lserver->cps.cpc.crank_length/2.0, p_lserver->cps.cpc.crank_length );

		// Set response in this order
		responseArray = g_byte_array_new();
		p_lserver->cps.cpc.response_opcode = CPS_CPC_OPCODE_RESPONSE_CODE;
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.response_opcode), sizeof(p_lserver->cps.cpc.response_opcode) );
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.request_opcode), sizeof(p_lserver->cps.cpc.request_opcode) );
		p_lserver->cps.cpc.response_value = CPS_CPC_RESPONSE_SUCCESS;
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.response_value), sizeof(p_lserver->cps.cpc.response_value) );
		// NO RESPONSE PARAMETER
	}

	else if( p_lserver->cps.cpc.request_opcode == CPS_CPC_OPCODE_SET_CHAIN_LENGTH )
	{
		parser_set_offset(parser, byte_offset);
		p_lserver->cps.cpc.chain_length = parser_get_uint16(parser); // Cache if needed
		gl_log( GL_LOG_INFO, "[%s]: Received request from [%s] to set chain length to %d mm\n", __func__, binc_device_get_address( p_lserver->central_device ),
				p_lserver->cps.cpc.chain_length );

		// Set response in this order
		responseArray = g_byte_array_new();
		p_lserver->cps.cpc.response_opcode = CPS_CPC_OPCODE_RESPONSE_CODE;
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.response_opcode), sizeof(p_lserver->cps.cpc.response_opcode) );
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.request_opcode), sizeof(p_lserver->cps.cpc.request_opcode) );
		p_lserver->cps.cpc.response_value = CPS_CPC_RESPONSE_SUCCESS;
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.response_value), sizeof(p_lserver->cps.cpc.response_value) );
		// NO RESPONSE PARAMETER
	}

	else if( p_lserver->cps.cpc.request_opcode == CPS_CPC_OPCODE_SET_CHAIN_WEIGHT )
	{
		parser_set_offset(parser, byte_offset);
		p_lserver->cps.cpc.chain_weight = parser_get_uint16(parser); // Cache if needed
		gl_log( GL_LOG_INFO, "[%s]: Received request from [%s] to set chain weight to %d g\n", __func__, binc_device_get_address( p_lserver->central_device ),
				p_lserver->cps.cpc.chain_weight );

		// Set response in this order
		responseArray = g_byte_array_new();
		p_lserver->cps.cpc.response_opcode = CPS_CPC_OPCODE_RESPONSE_CODE;
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.response_opcode), sizeof(p_lserver->cps.cpc.response_opcode) );
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.request_opcode), sizeof(p_lserver->cps.cpc.request_opcode) );
		p_lserver->cps.cpc.response_value = CPS_CPC_RESPONSE_SUCCESS;
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.response_value), sizeof(p_lserver->cps.cpc.response_value) );
		// NO RESPONSE PARAMETER
	}

	else if( p_lserver->cps.cpc.request_opcode == CPS_CPC_OPCODE_SET_SPAN_LENGTH )
	{
		parser_set_offset(parser, byte_offset);
		p_lserver->cps.cpc.span_length = parser_get_uint16(parser); // Cache if needed as client won't necessarily set it every time
		gl_log( GL_LOG_INFO, "[%s]: Received request from [%s] to set span length to %d mm\n", __func__, binc_device_get_address( p_lserver->central_device ),
				p_lserver->cps.cpc.span_length );

		// Set response in this order
		responseArray = g_byte_array_new();
		p_lserver->cps.cpc.response_opcode = CPS_CPC_OPCODE_RESPONSE_CODE;
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.response_opcode), sizeof(p_lserver->cps.cpc.response_opcode) );
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.request_opcode), sizeof(p_lserver->cps.cpc.request_opcode) );
		p_lserver->cps.cpc.response_value = CPS_CPC_RESPONSE_SUCCESS;
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.response_value), sizeof(p_lserver->cps.cpc.response_value) );
		// NO RESPONSE PARAMETER
	}

	else if( p_lserver->cps.cpc.request_opcode == CPS_CPC_OPCODE_REQUEST_SAMPLING_RATE )
	{
		parser_set_offset(parser, byte_offset);
		p_lserver->cps.cpc.span_length = parser_get_uint16(parser); // Cache chain if needed as client won't necessarily set it every time
		p_lserver->cps.cpc.sampling_rate = (uint8_t) (1000.0 / (double) p_lserver->cps.mc.notify_ms);
		gl_log( GL_LOG_INFO, "[%s]: Received request from [%s] to get sampling rate %d Hz\n", __func__,
				binc_device_get_address( p_lserver->central_device ),
				p_lserver->cps.cpc.sampling_rate );

		// Set response in this order
		responseArray = g_byte_array_new();
		p_lserver->cps.cpc.response_opcode = CPS_CPC_OPCODE_RESPONSE_CODE;
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.response_opcode), sizeof(p_lserver->cps.cpc.response_opcode) );
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.request_opcode), sizeof(p_lserver->cps.cpc.request_opcode) );
		p_lserver->cps.cpc.response_value = CPS_CPC_RESPONSE_SUCCESS;
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.response_value), sizeof(p_lserver->cps.cpc.response_value) );
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.sampling_rate), sizeof(p_lserver->cps.cpc.sampling_rate) );
	}

	else if( p_lserver->cps.cpc.request_opcode == CPS_CPC_OPCODE_START_ENH_OFFSET_COMP )
	{
		parser_set_offset(parser, byte_offset);
		gl_log( GL_LOG_INFO, "[%s]: Received request from [%s] for enhanced offset compensation (not supported)\n", __func__,
				binc_device_get_address( p_lserver->central_device ) );

		// Set response in this order
		responseArray = g_byte_array_new();
		p_lserver->cps.cpc.response_opcode = CPS_CPC_OPCODE_RESPONSE_CODE;
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.response_opcode), sizeof(p_lserver->cps.cpc.response_opcode) );
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.request_opcode), sizeof(p_lserver->cps.cpc.request_opcode) );
		p_lserver->cps.cpc.response_value = CPS_CPC_RESPONSE_UNSUPPORTED;
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.response_value), sizeof(p_lserver->cps.cpc.response_value) );
	}

	else{
		gl_log( GL_LOG_ERROR, "[%s] Error: [%s] sent unsupported CPS control point op code %d\n", __func__, binc_device_get_address( p_lserver->central_device ),
				p_lserver->cps.cpc.request_opcode );

		// Set response in this order
		responseArray = g_byte_array_new();
		p_lserver->cps.cpc.response_opcode = CPS_CPC_OPCODE_RESPONSE_CODE;
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.response_opcode), sizeof(p_lserver->cps.cpc.response_opcode) );
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.request_opcode), sizeof(p_lserver->cps.cpc.request_opcode) );
		p_lserver->cps.cpc.response_value = CPS_CPC_RESPONSE_UNSUPPORTED;
		g_byte_array_append( responseArray, (guint8 *)&(p_lserver->cps.cpc.response_value), sizeof(p_lserver->cps.cpc.response_value) );
	}

	#if 1
	// binc utility.c has g_byte_array_to_hex use that??
	printf( "[%s]: Response: ", __func__ );
	for( guint j = 0; j < responseArray->len; j++ ) printf( "%02x ", *(responseArray->data+j) );
	printf( " (length = %d)\n", responseArray->len );
	fflush(stdout);
	#endif

	// Send the indicate
	if( p_lserver->app != NULL ){ // Catch asynchronous SIGINT
		if( binc_application_char_is_notifying( p_lserver->app, CYCLING_POWER_SERVICE_UUID, CPS_CPC_UUID) == TRUE ){
			//printf( CYEL"[%s]:\t'%s' [%s] is indicating CPS CPC\n", __func__,
			//		binc_adapter_get_name( p_lserver->adapter ), binc_adapter_get_address( p_lserver->adapter ) );
			binc_application_notify( p_lserver->app, CYCLING_POWER_SERVICE_UUID, CPS_CPC_UUID, responseArray );
		}
	}

	return( 0 );
}

#if 0
/***********************************************************************
 *
 * Converts structure data to gba for indication
*/
GByteArray* gl_ble_cps_cpc_struct_to_gba( struct cps_cpc_struct *p_cpc )
{

	THIS IS NOT CORRECT!!! This needs to be custom for each opcode


	GByteArray* byteArray;
	byteArray = g_byte_array_new();
	g_byte_array_append( byteArray, (guint8 *)&(p_cpc->opcode), sizeof(p_cpc->opcode) );
	g_byte_array_append( byteArray, (guint8 *)&(p_cpc->cumulative_value), sizeof(p_cpc->cumulative_value) );
	g_byte_array_append( byteArray, (guint8 *)&(p_cpc->sensor_location), sizeof(p_cpc->sensor_location) );
	g_byte_array_append( byteArray, (guint8 *)&(p_cpc->request_op_code), sizeof(p_cpc->request_op_code) );
	g_byte_array_append( byteArray, (guint8 *)&(p_cpc->response_value), sizeof(p_cpc->response_value) );
	g_byte_array_append( byteArray, (guint8 *)&(p_cpc->response_parameter), sizeof(p_cpc->response_parameter) );

	#if 1
	printf( CMAG"[%s]: CPS_CPC characteristic: ", __func__ );
	for( guint j = 0; j < byteArray->len; j++ ) printf( "%02x ", *(byteArray->data+j) );
	printf( " (length = %d)\n", byteArray->len );
	fflush(stdout);
	#endif

	return( byteArray );
}


/***********************************************************************
 *
 * Send indicate upon service change--not used in a timed loop
*/
int gl_ble_cps_cpc_send_indicate( void *gptr )
{
	GL_LServer *p_lserver = (GL_LServer *)gptr; // Need to make this GL_LServer * as parm

	if( p_lserver->app != NULL ){ // Catch asynchronous SIGINT
		if( binc_application_char_is_notifying( p_lserver->app, CYCLING_POWER_SERVICE_UUID, CPS_CPC_UUID) == TRUE ){
			binc_application_notify( p_lserver->app, CYCLING_POWER_SERVICE_UUID, CPS_CPC_UUID,

SENDING THE WRONG GBA IN RESPONSE here

					gl_ble_cps_cpc_struct_to_gba( &p_lserver->cps.cpc ) );
			gl_log( GL_LOG_INFO, "[%s]:\t'%s' [%s] is indicating CPS CPC\n", __func__,
					binc_adapter_get_name( p_lserver->adapter ), binc_adapter_get_address( p_lserver->adapter ) );
		}
	}
	return( 0 );
}


#endif





#define GAP_REPEAT (5)

/***********************************************************************
 *
 * Gets power (Watts), crank cadence (RPM), and wheel_cadence (wheel revs/min) data from M3i or simulation
 * and moves from lclient.m3i to p_mc
 *
 * */
int gl_ble_cps_mc_update( struct cps_mc_struct *p_mc )
{
	static uint16_t count=0;
	uint16_t dt_wheel, dt_crank;
	static int gap=0;

	if( count == 0 ){
		// Initialize charact structure
		p_mc->instantaneous_power = 0;
		p_mc->cumulative_wheel_revs = 0;
		p_mc->last_wheel_event_time = 0;
		p_mc->cumulative_crank_revs = 0;
		p_mc->last_crank_event_time = 0;
		p_mc->accumulated_energy = 0;
		p_mc->prev_cumulative_wheel_revs = 0;
		p_mc->prev_cumulative_crank_revs = 0;
		p_mc->cadence = 0;
		p_mc->wheel_cadence = 0;
	}

	// Initialize each time
	p_mc->flags = 0;

	// Get data
	if( gl_cps_mc_simulate == TRUE ){
		// Simulated data
		p_mc->instantaneous_power = (int16_t) (SIM_CPS_INST_POWER_LO + (count % (SIM_CPS_INST_POWER_HI - SIM_CPS_INST_POWER_LO)));
		p_mc->cadence = (uint16_t) (SIM_CPS_CADENCE_LO + (count % (SIM_CPS_CADENCE_HI - SIM_CPS_CADENCE_LO)));
		//p_mc->speed = (double) SIM_CPS_KPH_LO + (double) (count % (SIM_CPS_KPH_HI - SIM_CPS_KPH_LO));
		p_mc->wheel_cadence = (double) p_mc->cadence * p_mc->gear_ratio;
		count++;
	}
	else{
		if( g_p_lclient->m3i.updated ){
			// Use this data point
			g_p_lclient->m3i.updated = 0;
			gap = 0;
			p_mc->instantaneous_power = (int16_t) g_p_lclient->m3i.data.values.watts;
			//p_mc->speed = (uint16_t) (5 + (count % (10 - 5)));
			p_mc->wheel_cadence = g_p_lclient->m3i.data.values.cadence * p_mc->gear_ratio;
			p_mc->cadence = g_p_lclient->m3i.data.values.cadence;
			count++;
		}
		else if( gap < GAP_REPEAT ){
			// Fill short gap with last data
			gap++;
		}
		else{
			p_mc->instantaneous_power = 0;
			p_mc->wheel_cadence = 0;
			p_mc->cadence = 0;
		}
	}

	if( GYMLINK_CPS_FC_FLAGS & CPS_FC_WHEEL_REVOLUTION_DATA_SUPPORTED ){
		// Wheel--increment one revolution and adjust time
		// Does not appear on Garmin so can't confirm correct
		p_mc->flags = p_mc->flags | CPS_MC_WHEEL_REVOLUTION_DATA_PRESENT;
		p_mc->prev_cumulative_wheel_revs = p_mc->cumulative_wheel_revs;
		p_mc->cumulative_wheel_revs++;
		p_mc->prev_wheel_event_time = p_mc->last_wheel_event_time;
		//dt_wheel = (uint16_t) (((3600.0 * TIRE_650_CIRCUMFERENCE_KM) / (double) p_mc->speed) * (double) CPS_MC_WHEEL_EVENT_TIME_RESOLUTION);
		dt_wheel = (uint16_t) ((60.0 / p_mc->wheel_cadence) * (double) CPS_MC_WHEEL_EVENT_TIME_RESOLUTION);
		p_mc->last_wheel_event_time = p_mc->last_wheel_event_time + dt_wheel;
	}

	if( GYMLINK_CPS_FC_FLAGS & CPS_FC_CRANK_REVOLUTION_DATA_SUPPORTED ){
		// Cadence--increment one revolution and adjust time
		// Confirm on Garmin
		p_mc->flags = p_mc->flags | CPS_MC_CRANK_REVOLUTION_DATA_PRESENT;
		p_mc->prev_cumulative_crank_revs = p_mc->cumulative_crank_revs;
		p_mc->cumulative_crank_revs++;
		p_mc->prev_crank_event_time = p_mc->last_crank_event_time;
		dt_crank = (uint16_t) ((60.0 / (double) p_mc->cadence) * (double) CPS_MC_CRANK_EVENT_TIME_RESOLUTION);
		p_mc->last_crank_event_time = p_mc->last_crank_event_time + dt_crank;
	}
//#include <stdio.h>
//printf( "dtwheel = %d, dtcrank=%d\n", dt_wheel, dt_crank );

	return( 0 );
}
