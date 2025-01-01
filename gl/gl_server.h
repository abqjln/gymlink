#ifndef GL_SERVER_H
#define GL_SERVER_H

#include <glib.h>

#include "application.h"

#include "gl_structures.h"

// Declare global pointer needed in cbs
extern GL_LServer *g_p_lserver;


//**********************************************************************
int gl_server_start_application( GL_LServer *p_lserver,
		onLocalCharacteristicRead on_char_read,
		onLocalCharacteristicWrite on_char_write,
		onLocalCharacteristicUpdated on_char_updated,
		onLocalDescriptorRead on_desc_read,
		onLocalDescriptorWrite on_desc_write,
		onLocalCharacteristicStartNotify on_char_start_notify,
		onLocalCharacteristicStopNotify on_char_stop_notify );

int gl_server_add_services( GL_LServer (*p_lserver) );

int gl_server_start_advertising( GL_LServer (*p_lserver) );

int gl_server_close( GL_LServer *p_lserver );

#endif
