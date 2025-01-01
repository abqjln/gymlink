/***********************************************************************
 *
 * gl_ble_dis.c
 *
*/

#include "gl_config.h"
#include "gl_utilities.h"
#include "gl_ble_dis.h"


/*******************************************************************
 * init_dis
 * DIS (Device information service)
 * NOTE: To stop bluez from auto-registering DIS (0x180a) and PnP (0x2a50) (and colliding with this)
 * sudo nano /etc/bluetooth/main.conf
 * Under [General] DeviceID = false
 * Under [GATT] Cache=no (perhaps no necessary, but much nicer to start fresh when debugging)
 * sudo systemctl daemon-reload && sudo systemctl restart bluetooth && sudo systemctl status bluetooth
*/
int gl_ble_dis_init( Application *application, struct dis_struct *p_dis_s )
{

	if (binc_application_add_service(application, DIS_SERVICE_UUID) != BINC_OK) {
		gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add service %s", __func__, DIS_SERVICE_UUID );
		return( -1 );
	}else{
		gl_log( GL_LOG_INFO, "[%s]: <%.8s> Registering Device Information Service (DIS)\n", __func__, DIS_SERVICE_UUID );

		if (binc_application_add_characteristic(application, DIS_SERVICE_UUID, DIS_SYSTEM_ID_UUID, GATT_CHR_PROP_READ) != BINC_OK) {
			gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add characteristic %s", __func__, DIS_SYSTEM_ID_UUID );
			return( -1 );
		}else{
			gl_log( GL_LOG_INFO, "[%s]:\t<%.8s> Adding DIS System ID\n", __func__, DIS_SYSTEM_ID_UUID );
		}

		if (binc_application_add_characteristic(application, DIS_SERVICE_UUID, DIS_MODEL_UUID, GATT_CHR_PROP_READ) != BINC_OK) {
			gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add characteristic %s", __func__, DIS_MODEL_UUID );
			return( -1 );
		}else{
			gl_log( GL_LOG_INFO, "[%s]:\t<%.8s> Adding DIS Model\n", __func__, DIS_MODEL_UUID );
		}

		if (binc_application_add_characteristic(application, DIS_SERVICE_UUID, DIS_SERIAL_UUID, GATT_CHR_PROP_READ) != BINC_OK) {
			gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add characteristic %s", __func__, DIS_SERIAL_UUID );
			return( -1 );
		}else{
			gl_log( GL_LOG_INFO, "[%s]:\t<%.8s> Adding DIS Serial\n", __func__, DIS_SERIAL_UUID );
		}

		if (binc_application_add_characteristic(application, DIS_SERVICE_UUID, DIS_HARDWARE_UUID, GATT_CHR_PROP_READ) != BINC_OK) {
			gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add characteristic %s", __func__, DIS_HARDWARE_UUID );
			return( -1 );
		}else{
			gl_log( GL_LOG_INFO, "[%s]:\t<%.8s> Adding DIS Hardware\n", __func__, DIS_HARDWARE_UUID );
		}

		if (binc_application_add_characteristic(application, DIS_SERVICE_UUID, DIS_FIRMWARE_UUID, GATT_CHR_PROP_READ) != BINC_OK) {
			gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add characteristic %s", __func__, DIS_FIRMWARE_UUID );
			return( -1 );
		}else{
			gl_log( GL_LOG_INFO, "[%s]:\t<%.8s> Adding DIS Firmware\n", __func__, DIS_FIRMWARE_UUID );
		}

		if (binc_application_add_characteristic(application, DIS_SERVICE_UUID, DIS_SOFTWARE_UUID, GATT_CHR_PROP_READ) != BINC_OK) {
			gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add characteristic %s", __func__, DIS_SOFTWARE_UUID );
			return( -1 );
		}else{
			gl_log( GL_LOG_INFO, "[%s]:\t<%.8s> Adding DIS Software\n", __func__, DIS_SOFTWARE_UUID );
		}

		if (binc_application_add_characteristic(application, DIS_SERVICE_UUID, DIS_MANUFACTURER_UUID, GATT_CHR_PROP_READ) != BINC_OK){
			gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add characteristic %s", __func__, DIS_MANUFACTURER_UUID );
			return( -1 );
		}else{
			gl_log( GL_LOG_INFO, "[%s]:\t<%.8s> Adding DIS Manufacturer\n", __func__, DIS_MANUFACTURER_UUID );
		}

		if (binc_application_add_characteristic(application, DIS_SERVICE_UUID, DIS_SYSTEM_ID_UUID, GATT_CHR_PROP_READ) != BINC_OK){
			gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add characteristic %s", __func__, DIS_SYSTEM_ID_UUID );
			return( -1 );
		}else{
			gl_log( GL_LOG_INFO, "[%s]:\t<%.8s> Adding DIS System ID\n", __func__, DIS_SYSTEM_ID_UUID );
		}

		// PnP for DIS (180a) is NOT automatically added (tested using nRF scanner)
		// We don't use this and gives seg fault if not properly initialized
		//if (binc_application_add_characteristic(application, DIS_SERVICE_UUID, DIS_PnP_ID_UUID, GATT_CHR_PROP_READ) != BINC_OK){
		//gl_log( GL_LOG_ERROR, "[%s] Error: Failed to add characteristic %s", __func__, DIS_PnP_ID_UUID );
		//return( -1 );
		//}
	}

	return( 0 );
}


//**********************************************************************
void gl_ble_dis_inspect( struct dis_struct *p_dis_s )
{
	gl_log( GL_LOG_INFO, "SystemID OUI:		0x%8x\n", p_dis_s->system_id );
	gl_log( GL_LOG_INFO, "Model:			%s\n", p_dis_s->model );
	gl_log( GL_LOG_INFO, "Serial:			%s\n", p_dis_s->serial );
	gl_log( GL_LOG_INFO, "Hardware:		%s\n", p_dis_s->hardware);
	gl_log( GL_LOG_INFO, "Firmware		%s\n", p_dis_s->firmware );
	gl_log( GL_LOG_INFO, "Software		%s\n", p_dis_s->software );
	gl_log( GL_LOG_INFO, "Manufacturer		%s\n", p_dis_s->manufacturer );

	gl_log( GL_LOG_INFO,  "PnP VendorIDSource:	0x%0*x\n", (int)(2*sizeof(p_dis_s->pnp_id.vendor_id_source)), p_dis_s->pnp_id.vendor_id_source );
	gl_log( GL_LOG_INFO, "PnP VendorID:		0x%0*x\n", (int)(2*sizeof(p_dis_s->pnp_id.vendor_id)), p_dis_s->pnp_id.vendor_id );
	gl_log( GL_LOG_INFO, "PnP ProductID:		0x%0*x\n", (int)(2*sizeof(p_dis_s->pnp_id.product_id)), p_dis_s->pnp_id.product_id );
	gl_log( GL_LOG_INFO, "PnP ProductVersion:	0x%0*x\n", (int)(2*sizeof(p_dis_s->pnp_id.product_version)), p_dis_s->pnp_id.product_version );
}


/***********************************************************************
 *
 * Free any allocations
*/
void gl_ble_dis_free( struct dis_struct *p_dis ){

	if( p_dis->model != NULL ) g_free( p_dis->model );
	if( p_dis->serial != NULL ) g_free( p_dis->serial );
	if( p_dis->hardware != NULL ) g_free( p_dis->hardware );
	if( p_dis->firmware != NULL ) g_free( p_dis->firmware );
	if( p_dis->software != NULL ) g_free( p_dis->software );
	if( p_dis->manufacturer != NULL ) g_free( p_dis->manufacturer );
	return;
}

