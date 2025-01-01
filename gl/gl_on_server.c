/***********************************************************************
 *
 * gl_on_server.c
 *
*/

#include <stdio.h>

#include "adapter.h"
#include "device.h"
#include "application.h"

#include "gl_config.h"
#include "gl_utilities.h"
#include "gl_on_server.h"


/***********************************************************************
 *
 * Handle read requests from client. Assumes single GL_Server *g_p_lserver
 *
*/
const char *gl_on_server_charact_read_request( const Application *application, const char *client_address, const char *gl_service_uuid, const char *charact_uuid )
{
	//**********************************************************************
	if( g_str_equal(gl_service_uuid, DIS_SERVICE_UUID ) )
	{
		if( g_str_equal(charact_uuid, DIS_MODEL_UUID ) )
		{
			gl_log( GL_LOG_INFO, "[%s]: Client [%s] reading <%.8s>:<%.8s> Model '%s'\n",
					__func__, client_address, gl_service_uuid, charact_uuid, g_p_lserver->dis.model );
			binc_application_set_char_value(application, gl_service_uuid, charact_uuid,
				g_byte_array_append( g_byte_array_new(), (guint8 *)g_p_lserver->dis.model, (guint)strlen(g_p_lserver->dis.model) ) );
		}
		else if (g_str_equal(charact_uuid, DIS_SERIAL_UUID))
		{
			gl_log( GL_LOG_INFO, "[%s]: Client [%s] reading <%.8s>:<%.8s> Serial '%s'\n",
				__func__, client_address, gl_service_uuid, charact_uuid, g_p_lserver->dis.serial );
			binc_application_set_char_value( application, gl_service_uuid, charact_uuid,
				g_byte_array_append( g_byte_array_new(), (guint8 *)g_p_lserver->dis.serial, (guint)strlen(g_p_lserver->dis.serial) ) );
		}
		else if (g_str_equal(charact_uuid, DIS_HARDWARE_UUID))
		{
			gl_log( GL_LOG_INFO, "[%s]: Client [%s] reading <%.8s>:<%.8s> Hardware '%s'\n",
				__func__, client_address, gl_service_uuid, charact_uuid, g_p_lserver->dis.hardware );
			binc_application_set_char_value( application, gl_service_uuid, charact_uuid,
				g_byte_array_append( g_byte_array_new(), (guint8 *)g_p_lserver->dis.hardware, (guint)strlen(g_p_lserver->dis.hardware) ) );
		}
		else if (g_str_equal(charact_uuid, DIS_FIRMWARE_UUID))
		{
			gl_log( GL_LOG_INFO, "[%s]: Client [%s] reading <%.8s>:<%.8s> Firmware '%s'\n",
				__func__, client_address, gl_service_uuid, charact_uuid, g_p_lserver->dis.firmware );
			binc_application_set_char_value( application, gl_service_uuid, charact_uuid,
				g_byte_array_append( g_byte_array_new(), (guint8 *)g_p_lserver->dis.firmware, (guint)strlen(g_p_lserver->dis.firmware) ) );
		}
		else if (g_str_equal(charact_uuid, DIS_SOFTWARE_UUID))
		{
			gl_log( GL_LOG_INFO, "[%s]: Client [%s] reading <%.8s>:<%.8s> Software '%s'\n",
				__func__, client_address, gl_service_uuid, charact_uuid, g_p_lserver->dis.software );
			binc_application_set_char_value( application, gl_service_uuid, charact_uuid,
				g_byte_array_append( g_byte_array_new(), (guint8 *)g_p_lserver->dis.software, (guint)strlen(g_p_lserver->dis.software) ) );
		}
		else if (g_str_equal(charact_uuid, DIS_MANUFACTURER_UUID))
		{
			gl_log( GL_LOG_INFO, "[%s]: Client [%s] reading <%.8s>:<%.8s> Manufacturer '%s'\n",
				__func__, client_address, gl_service_uuid, charact_uuid, g_p_lserver->dis.manufacturer );
			binc_application_set_char_value( application, gl_service_uuid, charact_uuid,
				g_byte_array_append( g_byte_array_new(), (guint8 *)g_p_lserver->dis.manufacturer, (guint)strlen(g_p_lserver->dis.manufacturer) ) );
		}
		else if (g_str_equal(charact_uuid, DIS_SYSTEM_ID_UUID))
		{
			const char *server_mac = binc_adapter_get_address( g_p_lserver->adapter );
			uint8_t *ptr = (uint8_t *) &g_p_lserver->dis.system_id;
			*ptr++ = (uint8_t) strtoul( server_mac, NULL, 16);
			*ptr++ = (uint8_t) strtoul( server_mac+3, NULL, 16);
			*ptr++ = (uint8_t) strtoul( server_mac+6, NULL, 16);
			*ptr++ = (uint8_t) 0xFF;
			*ptr++ = (uint8_t) 0xEE;
			*ptr++ = (uint8_t) strtoul( server_mac+9, NULL, 16);
			*ptr++ = (uint8_t) strtoul( server_mac+12, NULL, 16);
			*ptr++ = (uint8_t) strtoul( server_mac+15, NULL, 16);

			gl_log( GL_LOG_ALERT, "[%s]: Client [%s] reading <%.8s>:<%.8s> System ID: <%016lx>\n", __func__,
					client_address, DIS_SYSTEM_ID_UUID, charact_uuid, BSWAP64(g_p_lserver->dis.system_id) );

			binc_application_set_char_value( application, gl_service_uuid, charact_uuid,
				g_byte_array_append( g_byte_array_new(), (guint8 *)(&g_p_lserver->dis.system_id), (guint)sizeof(g_p_lserver->dis.system_id) ) );
		}
		else{
			gl_log( GL_LOG_ERROR, "[%s] Error: Client [%s] read of <%.8s>:<%.8s> unsupported\n", __func__, client_address, gl_service_uuid, charact_uuid );
			return BLUEZ_ERROR_REJECTED;
		}
	}

	//**********************************************************************
	else if (g_str_equal(gl_service_uuid, BATTERY_SERVICE_UUID)){
		if( g_str_equal(charact_uuid, BS_BATTERY_LEVEL_UUID)){
			gl_log( GL_LOG_INFO, "[%s]: Client [%s] reading <%.8s>:<%.8s> Battery Level %d%%\n",
					__func__, client_address, gl_service_uuid, charact_uuid, g_p_lserver->bs.blc.percent );
			binc_application_set_char_value(application, gl_service_uuid, charact_uuid, \
					g_byte_array_append( g_byte_array_new(), (guint8 *)&g_p_lserver->bs.blc.percent, sizeof(g_p_lserver->bs.blc.percent) ) );
		}
		else{
			gl_log( GL_LOG_ERROR, "[%s] Error: Client [%s] read of <%.8s>:<%.8s> unsupported\n", __func__, client_address, gl_service_uuid, charact_uuid );
			return BLUEZ_ERROR_REJECTED;
		}
	}

	//**********************************************************************
	else if (g_str_equal(gl_service_uuid, HEART_RATE_SERVICE_UUID)){
		if( g_str_equal(charact_uuid, HRS_BSLC_UUID)){
			gl_log( GL_LOG_INFO, "[%s]: Client [%s] reading <%.8s>:<%.8s> HRS Body Sensor Location 0x%02x\n",
					__func__, client_address, gl_service_uuid, charact_uuid, g_p_lserver->hrs.bslc.location );
			binc_application_set_char_value(application, gl_service_uuid, charact_uuid,
				g_byte_array_append( g_byte_array_new(), (guint8 *) &g_p_lserver->hrs.bslc.location, sizeof(g_p_lserver->hrs.bslc.location) ) );
		}
		else{
			gl_log( GL_LOG_ERROR, "[%s] Error: Client [%s] read of <%.8s>:<%.8s> unsupported\n", __func__, client_address, gl_service_uuid, charact_uuid );
			return BLUEZ_ERROR_REJECTED;
		}
	}

	//**********************************************************************
	else if (g_str_equal(gl_service_uuid, CYCLING_POWER_SERVICE_UUID)){
		if( g_str_equal(charact_uuid, CPS_FC_UUID)){
			gl_log( GL_LOG_INFO, "[%s]: Client [%s] reading <%.8s>:<%.8s> CPS Feature 0x%08x>\n",
					__func__, client_address, gl_service_uuid, charact_uuid, g_p_lserver->cps.fc.flags );
			binc_application_set_char_value(application, gl_service_uuid, charact_uuid, \
				g_byte_array_append( g_byte_array_new(), (guint8 *) &g_p_lserver->cps.fc.flags, sizeof(g_p_lserver->cps.fc.flags) ) );
		}
		else if( g_str_equal(charact_uuid, CPS_SLC_UUID)){
			gl_log( GL_LOG_INFO, "[%s]: Client [%s] reading <%.8s>:<%.8s> CPS Sensor Location 0x%02x\n",
					__func__, client_address, gl_service_uuid, charact_uuid, g_p_lserver->cps.slc.flags );
			binc_application_set_char_value(application, gl_service_uuid, charact_uuid,
				g_byte_array_append( g_byte_array_new(), (guint8 *) &g_p_lserver->cps.slc.flags, sizeof(g_p_lserver->cps.slc.flags) ) );
		}
		else{
			gl_log( GL_LOG_ERROR, "[%s] Error: Client [%s] read of <%.8s>:<%.8s> unsupported\n", __func__, client_address, gl_service_uuid, charact_uuid );
			return BLUEZ_ERROR_REJECTED;
		}
	}

	//**********************************************************************
	else if( g_str_equal(gl_service_uuid, RUNNING_SPEED_AND_CADENCE_SERVICE_UUID) ){
		if( g_str_equal(charact_uuid, RSCS_FC_UUID)){
			gl_log( GL_LOG_INFO, "[%s]: Client [%s] reading <%.8s>:<%.8s> RSCS Feature 0x%04x\n",
					__func__, client_address, gl_service_uuid, charact_uuid, g_p_lserver->rscs.fc.flags );
			binc_application_set_char_value( application, gl_service_uuid, charact_uuid, \
					g_byte_array_append( g_byte_array_new(), (guint8 *)&g_p_lserver->rscs.fc.flags, sizeof(g_p_lserver->rscs.fc.flags) ) );
		}
		else if( g_str_equal( charact_uuid, RSCS_SLC_UUID )){
			gl_log( GL_LOG_INFO, "[%s]: Client [%s] reading <%.8s>:<%.8s> RSCS Sensor Location 0x%02x\n",
					__func__, client_address, gl_service_uuid, charact_uuid, g_p_lserver->rscs.slc.flags );
			binc_application_set_char_value( application, gl_service_uuid, charact_uuid, \
					g_byte_array_append( g_byte_array_new(), (guint8 *)&g_p_lserver->rscs.slc.flags, sizeof(g_p_lserver->rscs.slc.flags) ) );
		}
		else{
			gl_log( GL_LOG_ERROR, "[%s] Error: Client [%s] read of <%.8s>:<%.8s> unsupported\n", __func__, client_address, gl_service_uuid, charact_uuid );
			return BLUEZ_ERROR_REJECTED;
		}
	}

	//**********************************************************************
	else if( g_str_equal( gl_service_uuid, CYCLING_SPEED_AND_CADENCE_SERVICE_UUID ) ){
		if( g_str_equal( charact_uuid, CSCS_FC_UUID )){
			gl_log( GL_LOG_INFO, "[%s]: Client [%s] reading <%.8s>:<%.8s> CSCS Feature 0x%04x\n",
					__func__, client_address, gl_service_uuid, charact_uuid, g_p_lserver->cscs.fc.flags );
			binc_application_set_char_value( application, gl_service_uuid, charact_uuid, \
					g_byte_array_append( g_byte_array_new(), (guint8 *)&g_p_lserver->cscs.fc.flags, sizeof(g_p_lserver->cscs.fc.flags) ) );
		}
		else if( g_str_equal( charact_uuid, CSCS_SLC_UUID )){
			gl_log( GL_LOG_INFO, "[%s]: Client [%s] reading <%.8s>:<%.8s> CSCS Sensor Location 0x%02x>\n",
					__func__, client_address, gl_service_uuid, charact_uuid, g_p_lserver->cscs.slc.flags );
			binc_application_set_char_value( application, gl_service_uuid, charact_uuid, \
					g_byte_array_append( g_byte_array_new(), (guint8 *)&g_p_lserver->cscs.slc.flags, sizeof(g_p_lserver->cscs.slc.flags) ) );
		}
		else{
			gl_log( GL_LOG_ERROR, "[%s] Error: Client [%s] read of <%.8s>:<%.8s> unsupported\n", __func__, client_address, gl_service_uuid, charact_uuid );
			return BLUEZ_ERROR_REJECTED;
		}
	}

	//**********************************************************************
	else {
		gl_log( GL_LOG_ERROR, "[%s] Error: Client [%s] unknown service <%.8s>:<%.8s> unsupported\n", __func__, client_address, gl_service_uuid, charact_uuid );
		return BLUEZ_ERROR_REJECTED;
	}

	return( NULL ); // Read is accepted
}


/***********************************************************************
 *
 * Updated char. Know this means we sent a reply to a read, but don't know
 * if it also means we received a write???
*/
void gl_on_server_charact_updated(const Application *application, const char *gl_service_uuid, const char *charact_uuid, GByteArray *byteArray) {
	//if( VERBOSE ) printf( PCOLOR"[%s]: Updated <%.8s>:<%.8s>\n", __func__, gl_service_uuid, charact_uuid );
}


/***********************************************************************
 *
 * Received write to characteristic
*/
const char *gl_on_server_charact_write_request(const Application *application, const char *client_address, const char *gl_service_uuid, const char *charact_uuid, GByteArray *byteArray)
{
	gl_log( GL_LOG_INFO, "[%s]: [%s] wrote to <%.8s>:<%.8s>\n", __func__, client_address, gl_service_uuid, charact_uuid );

	// Client wrote new CPS_CPC data; process and send indicate
	if( g_str_equal( gl_service_uuid, CYCLING_POWER_SERVICE_UUID) && g_str_equal( charact_uuid, CPS_CPC_UUID )){
		//printf( CYEL"[%s]: Processing CPS Control Point command\n", __func__ );
		if( gl_ble_cps_cpc_process_indicate( byteArray, g_p_lserver ) )
		{
			gl_log( GL_LOG_ERROR, "[%s] Error: processing/indicating CPC Control Point\n", __func__ );
			return( NULL );
		}
	}

	// Add CPCs for other services and other writes here


	else{
		gl_log( GL_LOG_ERROR, "[%s] Error: Unsupported write from client [%s]\n", __func__, client_address );
		return( NULL );
	}

	return NULL;
}


/***********************************************************************
 *
 * Alert to descriptor write
*/const char *gl_on_server_desc_write_request(const Application *application, const char *client_address, const char *gl_service_uuid, const char *charact_uuid, const char *desc_uuid, const GByteArray *byteArray) {
	gl_log( GL_LOG_ALERT, "[%s]: wrote to descriptor <%.8s> : characteristic <%.8s> (unprocessed)\n", __func__, desc_uuid, charact_uuid);
	return( NULL );
}


/***********************************************************************
 *
 * Alert to descriptor read
*/const char *gl_on_server_desc_read_request(const Application *application, const char *client_address, const char *gl_service_uuid, const char *charact_uuid, const char *desc_uuid) {
	gl_log( GL_LOG_ALERT, "[%s]: read from descriptor <%.8s> : characteristic <%.8s> (unprocessed)\n", __func__, desc_uuid, charact_uuid);
	return( NULL );
}


/***********************************************************************
 *
 * Alert to notify start
*/
void gl_on_server_enable_notify(const Application *application, const char *gl_service_uuid, const char *charact_uuid)
{
	// KLUDGE for not detecting connection with kinomap--if get enable notify, set connected
	g_p_lserver->connection_state = BINC_CONNECTED;

	gl_log( GL_LOG_ALERT, "[%s]: App '%s' received <%.8s>:<%.8s> enable notify\n",
			__func__, binc_application_get_path(application), gl_service_uuid, charact_uuid );

}


/***********************************************************************
 *
 * Alert to notify stop
*/
void gl_on_server_disable_notify(const Application *application, const char *gl_service_uuid, const char *charact_uuid)
{
	gl_log( GL_LOG_ALERT, "[%s]: App '%s' received <%.8s>:<%.8s> disable notify\n",
			__func__, binc_application_get_path(application), gl_service_uuid, charact_uuid );
}

