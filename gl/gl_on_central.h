#ifndef GL_ON_CENTRAL_H
#define GL_ON_CENTRAL_H

#include <glib.h>

#include "adapter.h"
#include "device.h"

extern char *g_p_rserver_prefix;


//**********************************************************************
void gl_on_central_peripheral_connection_state_changed( Device *periph, ConnectionState state, const GError *error );

void gl_on_central_process_M3i_scan_result( Adapter *adapter, Device *device );

void gl_on_central_discovery_state_changed( Adapter *adapter, DiscoveryState state, const GError *error );

void gl_on_central_device_removal( Adapter *adapter, Device *periph );

void gl_on_central_state_changed( Device *periph, BondingState new_state, BondingState old_state, const GError *error );

gboolean gl_on_central_request_authorization( Device *periph );

uint32_t gl_on_central_request_passkey( Device *periph );

void gl_on_central_bonding_state_changed(Device *periph, BondingState new_state, BondingState old_state, const GError *error);

#endif
