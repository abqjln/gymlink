/***********************************************************************
 *
 * gl_on_client.c
 *
 * callbacks for gl_client use
*/

//#define PCOLOR CCYN
//#define VERBOSE (1)

#include <stdio.h>
#include <signal.h>
#include <stdint.h>

#include "adapter.h"
#include "device.h"
#include "agent.h"
#include "parser.h"

#include "gl_utilities.h"
#include "gl_config.h"
#include "gl_ble_gap_gatt.h"
#include "gl_ble_dis.h"
#include "gl_ble_bs.h"
#include "gl_ble_hrs.h"
#include "gl_ble_cps.h"
#include "gl_ble_cscs.h"
#include "gl_ble_rscs.h"
#include "gl_ble_ftms.h"
#include "gl_on_central.h"
#include "gl_client.h"
#include "gl_on_client.h"


/***********************************************************************
 *
 * After services resolved, perform initial reads and/or start notifies
 * Does all services resolved, not only those advertised
*/
void gl_on_client_services_resolved( Device *server )
{
	const char* server_name = binc_device_get_name( server );
	const char* server_address = binc_device_get_address( server );
	GList *service_uuids, *iter;
	char *notify_uuid;

	service_uuids = binc_device_get_uuids( server );
	for( iter = service_uuids; iter != NULL; iter = iter->next ){

		// Device Information Service (DIS)
		if( g_str_equal( iter->data, DIS_SERVICE_UUID )){
			gl_log( GL_LOG_INFO, "[%s]: <%.8s> DIS resolved\n", __func__, iter->data );
			if( binc_device_read_char( server, DIS_SERVICE_UUID, DIS_MODEL_UUID) == FALSE )
				gl_log( GL_LOG_ERROR, "[%s] Error: '%s' [%s] read failure <%.8s>:<%.8s>\n", __func__,
						server_name, server_address, DIS_SERVICE_UUID, DIS_MODEL_UUID );
			else
				gl_log( GL_LOG_INFO, "[%s]: '%s' [%s] read <%.8s>:<%.8s>\n", __func__,
						server_name, server_address, DIS_SERVICE_UUID, DIS_MODEL_UUID );

			if( binc_device_read_char( server, DIS_SERVICE_UUID, DIS_SERIAL_UUID) == FALSE )
				gl_log( GL_LOG_ERROR, "[%s] Error: '%s' [%s] read failure <%.8s>:<%.8s>\n", __func__,
						server_name, server_address, DIS_SERVICE_UUID, DIS_SERIAL_UUID );
			else
				gl_log( GL_LOG_INFO, "[%s]: '%s' [%s] read <%.8s>:<%.8s>\n", __func__,
						server_name, server_address, DIS_SERVICE_UUID, DIS_SERIAL_UUID );

			if( binc_device_read_char( server, DIS_SERVICE_UUID, DIS_FIRMWARE_UUID) == FALSE )
				gl_log( GL_LOG_ERROR, "[%s] Error: '%s' [%s] read failure <%.8s>:<%.8s>\n", __func__,
						server_name, server_address, DIS_SERVICE_UUID, DIS_FIRMWARE_UUID );
			else
				gl_log( GL_LOG_INFO, "[%s]: '%s' [%s] read <%.8s>:<%.8s>\n", __func__,
						server_name, server_address, DIS_SERVICE_UUID, DIS_FIRMWARE_UUID );

			if( binc_device_read_char( server, DIS_SERVICE_UUID, DIS_HARDWARE_UUID) == FALSE )
				gl_log( GL_LOG_ERROR, "[%s] Error: '%s' [%s] read failure <%.8s>:<%.8s>\n", __func__,
						server_name, server_address, DIS_SERVICE_UUID, DIS_HARDWARE_UUID );
			else
				gl_log( GL_LOG_INFO, "[%s]: '%s' [%s] read <%.8s>:<%.8s>\n", __func__,
						server_name, server_address, DIS_SERVICE_UUID, DIS_HARDWARE_UUID );

			if( binc_device_read_char( server, DIS_SERVICE_UUID, DIS_SOFTWARE_UUID) == FALSE )
				gl_log( GL_LOG_ERROR, "[%s] Error: '%s' [%s] read failure <%.8s>:<%.8s>\n", __func__,
						server_name, server_address, DIS_SERVICE_UUID, DIS_SOFTWARE_UUID );
			else
				gl_log( GL_LOG_INFO, "[%s]: '%s' [%s] read <%.8s>:<%.8s>\n", __func__,
						server_name, server_address, DIS_SERVICE_UUID, DIS_SOFTWARE_UUID );

			if( binc_device_read_char( server, DIS_SERVICE_UUID, DIS_MANUFACTURER_UUID) == FALSE )
				gl_log( GL_LOG_ERROR, "[%s] Error: '%s' [%s] read failure <%.8s>:<%.8s>\n", __func__,
						server_name, server_address, DIS_SERVICE_UUID, DIS_MANUFACTURER_UUID );
			else
				gl_log( GL_LOG_INFO, "[%s]: '%s' [%s] read <%.8s>:<%.8s>\n", __func__,
						server_name, server_address, DIS_SERVICE_UUID, DIS_MANUFACTURER_UUID );
		}

		// Battery Service (BS)
		else if( g_str_equal( iter->data, BATTERY_SERVICE_UUID )){
			gl_log( GL_LOG_INFO, "[%s]: <%.8s> BS resolved\n", __func__, iter->data );
			if( binc_device_read_char( server, BATTERY_SERVICE_UUID, BS_BATTERY_LEVEL_UUID) == FALSE )
				gl_log( GL_LOG_ERROR, "[%s] Error: '%s' [%s] read failure <%.8s>:<%.8s>\n", __func__,
						server_name, server_address, BATTERY_SERVICE_UUID, BS_BATTERY_LEVEL_UUID );
			else
				gl_log( GL_LOG_INFO, "[%s]: '%s' [%s] read <%.8s>:<%.8s>\n", __func__,
						server_name, server_address, BATTERY_SERVICE_UUID, BS_BATTERY_LEVEL_UUID );
			binc_device_start_notify( server, BATTERY_SERVICE_UUID, BS_BATTERY_LEVEL_UUID );
		}

		// Heart Rate Service (HRS)
		else if( g_str_equal( iter->data, HEART_RATE_SERVICE_UUID )){
			gl_log( GL_LOG_INFO, "[%s]: <%.8s> HRS resolved\n", __func__, iter->data );
			gl_log( GL_LOG_INFO, "[%s]: Sending start notify command to server %s\n", __func__, binc_device_get_name( server ) );
			binc_device_start_notify( server, HEART_RATE_SERVICE_UUID, HRS_MC_UUID );
		}

		// Cycling Power Service (CPS)
		else if( g_str_equal( iter->data, CYCLING_POWER_SERVICE_UUID )){
			gl_log( GL_LOG_INFO, "[%s]: <%.8s> CPS resolved\n", __func__ );
			if( binc_device_read_char( server, CYCLING_POWER_SERVICE_UUID, CPS_SLC_UUID) == FALSE )
				gl_log( GL_LOG_ERROR, "[%s] Error: '%s' [%s] read failure <%.8s>:<%.8s>\n", __func__,
						server_name, server_address, CYCLING_POWER_SERVICE_UUID, CPS_SLC_UUID );
			else
				gl_log( GL_LOG_INFO, "[%s]: '%s' [%s] read <%.8s>:<%.8s>\n", __func__,
						server_name, server_address, CYCLING_POWER_SERVICE_UUID, CPS_SLC_UUID );

			gl_log( GL_LOG_INFO, "[%s]: Sending start notify command to server %s\n", __func__, binc_device_get_name( server ) );
			binc_device_start_notify( server, CYCLING_POWER_SERVICE_UUID, CPS_MC_UUID );
		}

		// Cycling Speed and Cadence Service (CSCS)
		else if( g_str_equal( iter->data, CYCLING_SPEED_AND_CADENCE_SERVICE_UUID )){
			gl_log( GL_LOG_INFO, "[%s]: <%.8s> CSCS resolved\n", __func__, iter->data );

		// Should we read FC here?
			gl_log( GL_LOG_INFO, "[%s]: Sending start notify & indicate commands to server %s\n", __func__, binc_device_get_name( server ) );
			binc_device_start_notify( server, CYCLING_SPEED_AND_CADENCE_SERVICE_UUID, CSCS_MC_UUID );
			binc_device_start_notify( server, CYCLING_SPEED_AND_CADENCE_SERVICE_UUID, CSCS_CPC_UUID ); // Indicate
		}

		// Running Speed and Cadence Service (RSCS)
		else if( g_str_equal( iter->data, RUNNING_SPEED_AND_CADENCE_SERVICE_UUID )){
			gl_log( GL_LOG_INFO, "[%s]: <%.8s> RSCS resolved\n", __func__, iter->data );

			notify_uuid = RSCS_MC_UUID;
			gl_log( GL_LOG_INFO, "[%s]: Sending start notify <%.8s><%.8s> to server %s\n", __func__,
					iter->data, notify_uuid, binc_device_get_name( server ) );
			binc_device_start_notify( server, iter->data, notify_uuid );
		}

		// Fitness Machine (FTMS)
		else if( g_str_equal( iter->data, FITNESS_MACHINE_SERVICE_UUID )){
			gl_log( GL_LOG_INFO, "[%s]: <%.8s> FTMS resolved\n", __func__, iter->data );


// HERE NEED TO KNOW WHICH SERVICE IS BEING ADVERTISED--or can we try to read to figure out???

			notify_uuid = FTMS_TDC_UUID;


			gl_log( GL_LOG_INFO, "[%s]: Sending start notify <%.8s><%.8s> to server %s\n", __func__,
					iter->data, notify_uuid, binc_device_get_name( server ) );
			binc_device_start_notify( server, iter->data, notify_uuid );
		}

		else if( g_str_equal( iter->data, GATT_SERVICE_UUID )){
			// Handled by bluez
		}

		else if( g_str_equal( iter->data, GAP_SERVICE_UUID )){
			// Handled by bluez
		}

		else gl_log( GL_LOG_ALERT, "[%s]: <%.8s> unhandled service\n", __func__, iter->data );
	}

	// After resolving services, restart discovery to continue scanning
	binc_adapter_start_discovery( binc_device_get_adapter( server ) );
}


/***********************************************************************
 *
 * On notification, finds the index of the structure for this server
 * and stores the data
*/
void gl_on_client_received_notification( Device *server, Characteristic *characteristic, const GByteArray *byteArray )
{
    const char *uuid;
//	int sindex;

    uuid = binc_characteristic_get_uuid( characteristic );

	// Find server index
//	if( (sindex = gl_client_conn_list_index( server )) < 0 ){
//		gl_log( GL_LOG_ERROR, "[%s] Error: finding connection index\n", __func__ );
//		return;
//	}

//	gl_log( GL_LOG_INFO, "[%s]: Received notification on <%.8s> with device connection index is %3h\n", __func__, uuid, sindex );

	//***************
	if( g_str_equal(uuid, HRS_MC_UUID )){
		gl_ble_hrs_mc_receive_notify( byteArray, &g_p_lclient->hrs.mc );
		gl_log( GL_LOG_DEBUG, "[%s]: '%s' received notify <%.8s> from '%s':\t\t\t\t\t\t\t\t %3hu BPM\n", __func__,
				binc_adapter_get_name( binc_device_get_adapter( server ) ), uuid, binc_device_get_name( server ),
				g_p_lclient->hrs.mc.heart_rate_measurement );
	}

	//***************
	else if( g_str_equal( uuid, CPS_MC_UUID )){
		// Receive, parse, and compute values
		gl_ble_cps_mc_receive_notify( byteArray, &g_p_lclient->cps.mc );
		gl_log( GL_LOG_DEBUG, "[%s]: '%s' received notify <%.8s> from '%s':\tCumWheelRevs=%10d, LastWheelTime=%6d, CumCrankRevs=%6d, LastCrankTime=%6d, RPM=%3d, KPH=%3d, W=%4d\n", __func__,
				binc_adapter_get_name( binc_device_get_adapter( server ) ), uuid, binc_device_get_name( server ),
				g_p_lclient->cps.mc.cumulative_wheel_revs,
				g_p_lclient->cps.mc.last_wheel_event_time,
				g_p_lclient->cps.mc.cumulative_crank_revs,
				g_p_lclient->cps.mc.last_crank_event_time,
				g_p_lclient->cps.mc.cadence,
				g_p_lclient->cps.mc.wheel_cadence,
				g_p_lclient->cps.mc.instantaneous_power );
	}

	//***************
	else if( g_str_equal( uuid, RSCS_MC_UUID )){
		gl_ble_rscs_mc_receive_notify( byteArray, &g_p_lclient->rscs.mc );
		gl_log( GL_LOG_DEBUG, "[%s]: '%s' received notify <%.8s> from '%s': speed=%f mph, distance=%f miles, incline=%f %%, egain=%f ft, etime=%f, cadence=%d\r", __func__,
				binc_adapter_get_name( binc_device_get_adapter( server ) ), uuid, binc_device_get_name( server ),
				(double) (g_p_lclient->rscs.mc.instantaneous_speed) * RSCS_MC_SPEED_SCALE_MPH,
				(double) (g_p_lclient->rscs.mc.total_distance) * RSCS_MC_DISTANCE_SCALE_MILES,
				0,
				0,
				0,
				g_p_lclient->rscs.mc.instantaneous_cadence);
	}

	//***************
	else if( g_str_equal( uuid, CSCS_MC_UUID )){
		gl_ble_cscs_mc_receive_notify( byteArray, &g_p_lclient->cscs.mc );
		gl_log( GL_LOG_DEBUG, "[%s]: '%s' received notify <%.8s> from '%s':\t%10d, %6d, %6d, %6d, %4.1f RPM, %4.1f KPH, %4d W\n", __func__,
				binc_adapter_get_name( binc_device_get_adapter( server ) ), uuid, binc_device_get_name( server ),
				g_p_lclient->cscs.mc.cumulative_wheel_revs,
				g_p_lclient->cscs.mc.last_wheel_event_time,
				g_p_lclient->cscs.mc.cumulative_crank_revs,
				g_p_lclient->cscs.mc.last_crank_event_time,
				g_p_lclient->cscs.mc.dcadence,
				g_p_lclient->cscs.mc.dspeed,
				0 );
	}

	//***************
	else if( g_str_equal( uuid, BS_BATTERY_LEVEL_UUID )){
		gl_ble_bs_blc_receive_notify( byteArray, &g_p_lclient->bs.blc );
		gl_log( GL_LOG_DEBUG, "[%s]: '%s' received notify <%.8s> from '%s': %3d%%\n", __func__,
				binc_adapter_get_name( binc_device_get_adapter( server ) ), uuid, binc_device_get_name( server ),
				g_p_lclient->bs.blc.percent );
	}

	//***************
	else if( g_str_equal( uuid, FTMS_TDC_UUID )){
		gl_ble_ftms_tdc_receive_notify( byteArray, &g_p_lclient->ftms.tdc );
		gl_log( GL_LOG_DEBUG, "[%s]: '%s' received notify <%.8s> from '%s': speed=%f mph, distance=%f miles, incline=%f %%, egain=%f ft, etime=%f, cadence=%d\r", __func__,
				binc_adapter_get_name( binc_device_get_adapter( server ) ), uuid, binc_device_get_name( server ),
				(double) (g_p_lclient->ftms.tdc.instantaneous_speed) * FTMS_TDC_SPEED_SCALE_MPH,
				(double) (g_p_lclient->ftms.tdc.total_distance[0] + \
						(g_p_lclient->ftms.tdc.total_distance[1]<<8) + \
						(g_p_lclient->ftms.tdc.total_distance[2]<<8)) * FTMS_TDC_DISTANCE_SCALE_MILES,
				(double) (g_p_lclient->ftms.tdc.ramp_inclination) * FTMS_TDC_RAMP_INCLINATION_PERCENT,
				(double) (g_p_lclient->ftms.tdc.pos_elevation_gain) * FTMS_TDC_POS_ELEVATION_GAIN_FT,
				(double) (g_p_lclient->ftms.tdc.elapsed_time) * FTMS_TDC_ELAPSED_TIME_S,
				0 );
	}

//match speed for rscs-mc and ftms-tdc


	//***************
	else{
		gl_log( GL_LOG_ERROR, "[%s] Error: Unhandled notify from '%s' [%s] (<%.8s>)\n", __func__,
				uuid, binc_device_get_name( server ), binc_device_get_address( server ) );
	}
}


/***********************************************************************
 *
 * On receipt of read value, finds index of server and updates data
*/
void gl_on_client_received_char_read( Device *server, Characteristic *characteristic, const GByteArray *byteArray, const GError *error ){
//	const char* server_name = binc_device_get_name( server );
//	const char *server_address = binc_device_get_address( server );
	Parser *parser;
	GString *parsed_string;
	const char *uuid;

	uuid = binc_characteristic_get_uuid(characteristic);
	const char *server_name = binc_device_get_name( server );

	if( error != NULL ){
		gl_log( GL_LOG_ERROR, "[%s] Error: Read failure: <%.8s>' (error %d: %s)\n", __func__, uuid, error->code, error->message );
		return;
	}
	else if (byteArray == NULL){
		gl_log( GL_LOG_ERROR, "[%s] Error: NULL byteArray\n", __func__ );
		 return;
	}

	// Find server index
//	if( (sindex = gl_client_conn_list_index( server )) < 0 ){
//		gl_log( GL_LOG_ERROR, "[%s] Error: finding connection index\n", __func__ );
//		return;
//	}

	//gl_log( GL_LOG_INFO, "[%s]: Parsing <%.8s> with device connection index is %d\n", __func__, uuid, 999 );

	parser = parser_create( byteArray, LITTLE_ENDIAN );

	if( g_str_equal( uuid, BS_BATTERY_LEVEL_UUID )){
//		((g_p_lclient->server)+sindex)->bs.blc.percent = parser_get_uint8( parser );
		gl_log( GL_LOG_INFO, "[%s]: Received from '%s': <%.8s>, battery level	= %d\n", __func__,
				server_name, uuid, g_p_lclient->bs.blc.percent);
	}
	else if( g_str_equal(uuid, DIS_MODEL_UUID )){
		parsed_string = parser_get_string( parser );
		g_p_lclient->dis.model = g_strdup( parsed_string->str );
		gl_log( GL_LOG_INFO, "[%s]: Received from '%s': <%.8s> model		= %s\n", __func__,
				server_name, uuid, g_p_lclient->dis.model );
	}
	else if (g_str_equal(uuid, DIS_SERIAL_UUID)) {
		parsed_string = parser_get_string( parser );
		g_p_lclient->dis.serial = g_strdup( parsed_string->str );
		gl_log( GL_LOG_INFO, "[%s]: Received from '%s': <%.8s> serial		= %s\n", __func__,
				server_name, uuid, g_p_lclient->dis.serial );
	}
	else if( g_str_equal(uuid, DIS_FIRMWARE_UUID )){
		parsed_string = parser_get_string( parser );
		g_p_lclient->dis.firmware = g_strdup( parsed_string->str );
		gl_log( GL_LOG_INFO, "[%s]: Received from '%s': <%.8s> firmware		= %s\n", __func__,
				server_name, uuid, g_p_lclient->dis.firmware );
	}
	else if( g_str_equal(uuid, DIS_HARDWARE_UUID )){
		parsed_string = parser_get_string( parser );
		g_p_lclient->dis.hardware = g_strdup( parsed_string->str );
		gl_log( GL_LOG_INFO, "[%s]: Received from '%s': <%.8s> hardware		= %s\n", __func__,
				server_name, uuid, g_p_lclient->dis.hardware );
	}
	else if( g_str_equal(uuid, DIS_SOFTWARE_UUID )) {
		parsed_string = parser_get_string( parser );
		g_p_lclient->dis.software = g_strdup( parsed_string->str );
		gl_log( GL_LOG_INFO, "[%s]: Received from '%s': <%.8s> software		= %s\n", __func__,
				server_name, uuid, g_p_lclient->dis.software );
	}
	else if( g_str_equal(uuid, DIS_MANUFACTURER_UUID )) {
		parsed_string = parser_get_string( parser );
		g_p_lclient->dis.manufacturer = g_strdup( parsed_string->str );
		gl_log( GL_LOG_INFO, "[%s]: Received from '%s': <%.8s> manufacturer	= %s\n", __func__,
				server_name, uuid, g_p_lclient->dis.manufacturer );
	}
	else if( g_str_equal(uuid, CPS_SLC_UUID )) {
		g_p_lclient->cps.slc.flags = parser_get_uint8( parser );
		gl_log( GL_LOG_INFO, "[%s]: Received from '%s': <%.8s> CPS Sensor Location= %u\n", __func__,
				server_name, uuid, g_p_lclient->cps.slc.flags );
	}

//	else if (g_str_equal(uuid, HRS_BSLC_UUID)) {
//		((g_p_lclient->server)+sindex)->hrs_s.bslc.location = parser_get_uint8( parser );
//		gl_log( GL_LOG_INFO, "[%s]: Received <%.8s>, HR body sensor location		= %d\n", __func__,
//				uuid, ((g_p_lclient->server)+sindex)->hrs_s.bslc.location );
//	}
	else {
		// Catch all
		parsed_string = parser_get_string(parser);
		if( parsed_string != NULL ){
				gl_log( GL_LOG_ERROR, "[%s] Error: Unrecognized from %s: UUID <%.8s>, String = %s\n", __func__,
						server_name, uuid, parsed_string->str);
		}
	}

	parser_free(parser);
}


/***********************************************************************
 *
*/void gl_on_client_char_write(Device *server, Characteristic *characteristic, const GByteArray *byteArray, const GError *error) {
	const char *server_name = binc_device_get_name( server );
	const char *server_address = binc_device_get_address( server );
    gl_log( GL_LOG_ALERT, "[%s]: Writing?? to '%s' [%s]",
			__func__, server_name, server_address );
}


/***********************************************************************
 *
*/void gl_on_client_desc_read(Device *server, Descriptor *descriptor, const GByteArray *byteArray, const GError *error) {
	const char *server_name = binc_device_get_name( server );
	const char *server_address = binc_device_get_address( server );

	gl_log( GL_LOG_INFO, "[%s]: Reading descriptor from '%s' [%s]",
			__func__, server_name, server_address );

	Parser *parser = parser_create(byteArray, LITTLE_ENDIAN);
	GString *parsed_string = parser_get_string(parser);
	gl_log( GL_LOG_ALERT, "[%s]: CUD %s", __func__, parsed_string->str);
	parser_free(parser);
}


/***********************************************************************
 *
*/
void gl_on_client_server_notification_state_changed( Device *server, Characteristic *characteristic, const GError *error )
{
	const char *server_name = binc_device_get_name(server);
	const char *server_address = binc_device_get_address( server );
	const char *uuid = binc_characteristic_get_uuid(characteristic);

	if (error != NULL) {
		gl_log( GL_LOG_ERROR, "[%s] Error: Device '%s' [%s] notifying <%.8s> failed (error %d: %s)\n",
				__func__, server_name, server_address, uuid, error->code, error->message);
		return;
	}

	gl_log( GL_LOG_ALERT, "[%s]: Server '%s' [%s] uuid <%.8s> notification state '%s'\n",
			__func__, server_name, server_address, uuid, binc_characteristic_is_notifying(characteristic) ? "true" : "false");
}


/***********************************************************************
 *
*/
uint32_t gl_on_central_request_passkey( Device *periph )
{
	guint32 pass = 000000;
	gl_log( GL_LOG_ALERT, "[%s]: Peripheral '%s' [%s] requesting passkey\n",
			__func__, binc_device_get_name( periph ), binc_device_get_address( periph ) );
	gl_log( GL_LOG_ALERT, "\t\tEnter 6 digit pin code: ");
	int result = fscanf(stdin, "%d", &pass);
	if (result != 1) {
		gl_log( GL_LOG_ERROR, "[%s] Error: didn't read a pin code\n", __func__ );
	}
	return( pass );
}

