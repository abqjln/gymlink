/***********************************************************************
 *
 * gl_server.c
 *
*/

#include <glib.h>

#include "adapter.h"
#include "advertisement.h"
#include "application.h"
#include "device.h"
#include "logger.h"

#include "gl_colors.h"
#include "gl_config.h"
#include "gl_utilities.h"
#include "gl_structures.h"
#include "gl_on_peripheral.h"
#include "gl_server.h"

// Define global pointer needed in cbs
GL_LServer *g_p_lserver;


/***********************************************************************
 *
 * Starts application and sets callbacks
*/
int gl_server_start_application( GL_LServer *p_lserver,
		onLocalCharacteristicRead on_char_read,
		onLocalCharacteristicWrite on_char_write,
		onLocalCharacteristicUpdated on_char_updated,
		onLocalDescriptorRead on_desc_read,
		onLocalDescriptorWrite on_desc_write,
		onLocalCharacteristicStartNotify on_char_start_notify,
		onLocalCharacteristicStopNotify on_char_stop_notify )
{
	if( (p_lserver->app = binc_create_application( p_lserver->adapter )) == NULL ){
		gl_log( GL_LOG_ERROR, "Failed to create application\n" );
		return( -1 );
	}

	// Set callbacks
	binc_application_set_char_read_cb( p_lserver->app, on_char_read );
	binc_application_set_char_updated_cb( p_lserver->app, on_char_updated) ;
	binc_application_set_char_write_cb( p_lserver->app, on_char_write );
	binc_application_set_desc_read_cb( p_lserver->app, on_desc_read );
	binc_application_set_desc_write_cb( p_lserver->app, on_desc_write);
	binc_application_set_char_start_notify_cb( p_lserver->app, on_char_start_notify );
	binc_application_set_char_stop_notify_cb( p_lserver->app, on_char_stop_notify );

	binc_adapter_set_remote_central_cb( p_lserver->adapter, gl_on_peripheral_central_connection_state_changed );
	binc_adapter_register_application( p_lserver->adapter, p_lserver->app );

	gl_log( GL_LOG_INFO, "{%s]: Published application '%s' using dbus %p on %s:[%s] '%s'\n",
			__func__, binc_application_get_path( p_lserver->app ), p_lserver->dbusConnection, \
			binc_adapter_get_name( p_lserver->adapter ), binc_adapter_get_address( p_lserver->adapter ), p_lserver->adv_name );
	return( 0 );
}


/***********************************************************************
 *
 * Adds DIS and BS service, then spins up services based on uuid list
 *
*/
int gl_server_add_services( GL_LServer *p_lserver )
{
	guint j;

	//******************************************************************
	// GATT 1801 is added by bluez (bluetoothctl show)
	//gl_service_gs_init( p_lserver->app ); // This GATT Server is not implemented correctly, use bluez's

	// so on connect, immediately send indicate to force service resolution??

	// If no services, its a beacon so just return
	if( p_lserver->adv_service_uuids == NULL ) return( 1 );

	// Always add DIS whether advertised or not
	if( gl_ble_dis_init( p_lserver->app, &p_lserver->dis ) == BINC_OK ){
		//gl_ble_dis_inspect( &p_lserver->dis );
		// No notify
	}
	else{
		gl_log( GL_LOG_ERROR, "Failed to initialize DIS\n" );
		return( -1 );
	}

	// Always add Battery service (not advertised, discovered)
	if( gl_ble_bs_init( p_lserver->app, &p_lserver->bs ) == BINC_OK ){
		// Notify loop
		g_timeout_add( p_lserver->bs.blc.notify_ms, gl_ble_bs_blc_send_notify, p_lserver );
	}
	else{
		gl_log( GL_LOG_ERROR, "Failed to initialize BS\n" );
		return( -1 );
	}

	// Add additional GATT services based on adv_service_uuids
	for( j = 0; j < p_lserver->adv_service_uuids->len; j++ )
	{
		if( g_str_equal( g_ptr_array_index( p_lserver->adv_service_uuids, j ), DIS_SERVICE_UUID ) ){
			// Already initialized
		}

		//**********
		else if( g_str_equal( g_ptr_array_index( p_lserver->adv_service_uuids, j ), HEART_RATE_SERVICE_UUID ) ){
			if( gl_ble_hrs_init( p_lserver->app, &p_lserver->hrs ) == BINC_OK ){
				// Notify loop
				g_timeout_add( p_lserver->hrs.mc.notify_ms, gl_ble_hrs_mc_send_notify, p_lserver );
			}
			else{
				gl_log( GL_LOG_ERROR, "Failed to initialize HRS\n" );
				return( -1 );
			}
		}

		//**********
		else if( g_str_equal( g_ptr_array_index( p_lserver->adv_service_uuids, j ), CYCLING_POWER_SERVICE_UUID ) ){
			if( gl_ble_cps_init( p_lserver->app, &p_lserver->cps ) == BINC_OK ){
				// Notify loop
				g_timeout_add( p_lserver->cps.mc.notify_ms, gl_ble_cps_mc_send_notify, p_lserver );
				// Indicate cps_cpc only on a change or in response to write, not timed loop
			}
			else{
				gl_log( GL_LOG_ERROR, "Failed to initialize CPS\n" );
				return( -1 );
			}
		}

		//**********
		else if( g_str_equal( g_ptr_array_index( p_lserver->adv_service_uuids, j ), CYCLING_SPEED_AND_CADENCE_SERVICE_UUID ) ){
			if( gl_ble_cscs_init( p_lserver->app, &p_lserver->cscs ) == BINC_OK ){
				// Notify loop
				g_timeout_add( p_lserver->cscs.mc.notify_ms, gl_ble_cscs_mc_send_notify, p_lserver );
				// Indicate cscs_cpc only on a change or in response to write, not timed loop
			}
			else{
				gl_log( GL_LOG_ERROR, "Failed to initialize CSCS\n" );
				return( -1 );
			}
		}

		//**********
		else if( g_str_equal( g_ptr_array_index( p_lserver->adv_service_uuids, j ), RUNNING_SPEED_AND_CADENCE_SERVICE_UUID ) ){
			if( gl_ble_rscs_init( p_lserver->app, &p_lserver->rscs ) == BINC_OK ){
				// Notify loop
				g_timeout_add( p_lserver->rscs.mc.notify_ms, gl_ble_rscs_mc_send_notify, p_lserver );
			}
			else{
				gl_log( GL_LOG_ERROR, "Failed to initialize RSCS\n" );
				return( -1 );
			}
		}
	}
	return( 0 );
}


/***********************************************************************
 *
 * Advertising
 * Local name to be advertised along with uuids
 * Garmin does not detect name, only address, but nRF Connect does
*/
int gl_server_start_advertising( GL_LServer (*p_lserver) )
{
	// Only support exactly one advertised service (too messy for me to do more)
	if( (p_lserver->adv_service_uuids == NULL) | (p_lserver->adv_service_uuids->len != 1)){
		gl_log( GL_LOG_ERROR, "[%s] Error: Exactly one advertised service supported\n", __func__ );
		return( -1 );
	}

	// Create advertisement structure and populate
	p_lserver->advertisement = binc_advertisement_create();
	binc_advertisement_set_local_name( p_lserver->advertisement, p_lserver->adv_name );
	binc_advertisement_set_interval( p_lserver->advertisement, p_lserver->adv_min_interval, p_lserver->adv_max_interval );
	//binc_advertisement_set_tx_power( p_lserver->advertisement, power ); // Added to binc 12/14/2024, untested
	binc_advertisement_set_services( p_lserver->advertisement, p_lserver->adv_service_uuids );


//	guint mfglen, svclen, totlen = 12;
//	mfglen = 1; // Min len is 1; did pull request to change to zero (aka just mfr_id
//	svclen = totlen - mfglen;

// No service data can do 16 mfglen (add two for mfg_id to get 18 total??)
// No mfg data can do 16 svclen
// If have both, max total length is 12

	guint mfglen = 1;
	if( p_lserver->adv_manufacturer_id != 0 ){
		for( uint8_t i=0; i < mfglen; i++ ){
			p_lserver->adv_manufacturer_data[i] = i; // Dummy manufacturer_data

		}

		GByteArray* byteArray;
		byteArray = g_byte_array_new();
		g_byte_array_append( byteArray, (guint8 *)(p_lserver->adv_manufacturer_data), mfglen );
		binc_advertisement_set_manufacturer_data( p_lserver->advertisement, p_lserver->adv_manufacturer_id, byteArray );
		g_byte_array_free( byteArray, TRUE );
	}

//https://terminology.hl7.org/5.0.0/NamingSystem-bluetooth-address-identifier.html#:~:text=The%20Bluetooth%20Device%20Address%20(sometimes,33%3AFF%3AEE).
	// For now, the only service that requires service data ad is FTMS
	if( !strcmp( (char *)g_ptr_array_index( p_lserver->adv_service_uuids, 0 ), FITNESS_MACHINE_SERVICE_UUID  )){
		GByteArray *byteArray2 = g_byte_array_new();
		g_byte_array_append( byteArray2, (guint8 *)(&(p_lserver->ftms.adv.flags)), 1 );
		g_byte_array_append( byteArray2, (guint8 *) &(p_lserver->ftms.adv.ftms_type)+0, 1 );
		g_byte_array_append( byteArray2, (guint8 *) &(p_lserver->ftms.adv.ftms_type)+1, 1 );
		binc_advertisement_set_service_data( p_lserver->advertisement, (char *)g_ptr_array_index( p_lserver->adv_service_uuids, 0 ), byteArray2 );
	}

	binc_adapter_start_advertising( p_lserver->adapter, p_lserver->advertisement );

	gl_log( GL_LOG_ALERT, "[%s]; Advertising %s : <%s> using '%s' on %s:[%s] '%s'\n", __func__,
			binc_advertisement_get_local_name( p_lserver->advertisement ),
			gl_get_uuid_string( p_lserver->adv_service_uuids ), binc_advertisement_get_path( p_lserver->advertisement ),
			binc_adapter_get_name(p_lserver->adapter),
			binc_adapter_get_address(p_lserver->adapter), p_lserver->adv_name );

	return( 0 );
}


/***********************************************************************
 *
 * Closes and frees server info
*/
int gl_server_close( GL_LServer *p_lserver )
{
	gl_log( GL_LOG_INFO, "[%s]: Closing server %s\n", __func__, p_lserver->adv_name );

	g_ptr_array_unref( p_lserver->adv_service_uuids ); // Since we created

	// Disconnect
	if( p_lserver->central_device != NULL ){
		if( binc_device_get_connection_state( p_lserver->central_device ) != BINC_DISCONNECTED ){
			gl_log( GL_LOG_INFO, "\tdisconnect from client '%s'...\n", binc_device_get_name( p_lserver->central_device ) );
			binc_device_disconnect( p_lserver->central_device );
		}
		p_lserver->central_device = NULL;
	}
	sleep( 1 ); // Pause for disconnect messages

	// Free app
	if( p_lserver->app != NULL ){
		gl_log( GL_LOG_INFO, "\tfreeing app %s...\n", binc_application_get_path( p_lserver->app ) );
		binc_adapter_unregister_application( p_lserver->adapter, p_lserver->app );
		binc_application_free( p_lserver->app );
		p_lserver->app = NULL;
	}

	// Free server adapter
	if( p_lserver->adapter != NULL ){
		gl_log( GL_LOG_INFO, "\tfreeing adapter %s...\n", binc_adapter_get_path( p_lserver->adapter ) );
		binc_adapter_free( p_lserver->adapter );
		p_lserver->adapter = NULL;
	}

	// Disconnect from DBus
	if( p_lserver->dbusConnection != NULL ){
		gl_log( GL_LOG_INFO, "\tdisconnecting from dbus 0x%p...\n", p_lserver->dbusConnection );
		g_dbus_connection_close_sync( p_lserver->dbusConnection, NULL, NULL );
		g_object_unref( p_lserver->dbusConnection );
		p_lserver->dbusConnection = NULL;
	}

	return( 0 );
}
