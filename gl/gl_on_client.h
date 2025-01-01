#ifndef GL_ON_CLIENT_H
#define GL_ON_CLIENT_H

#include <glib.h>

#include "adapter.h"
#include "device.h"


//**********************************************************************
void gl_on_adapter_scan_result( Adapter *adapter, Device *device );

void gl_on_client_char_write( Device *server, Characteristic *characteristic, const GByteArray *byteArray, const GError *error );

void gl_on_client_desc_read( Device *server, Descriptor *descriptor, const GByteArray *byteArray, const GError *error );

void gl_on_client_server_notification_state_changed(Device *server, Characteristic *characteristic, const GError *error);

void gl_on_client_received_notification( Device *server, Characteristic *characteristic, const GByteArray *byteArray );

void gl_on_client_received_char_read( Device *server, Characteristic *characteristic, const GByteArray *byteArray, const GError *error );

void gl_on_client_services_resolved( Device *server );


#endif
