#ifndef GL_CENTRAL_H
#define GL_CENTRAL_H

#include <glib.h>

#include "adapter.h"


//**********************************************************************
int gl_central_set_filter_prefix( char *prefix );

char *gl_central_get_filter_prefix( void );

int gl_central_init_discovery( Adapter *adapter, GPtrArray *kfilt_adv_uuids, char *kfilt_pattern,
		AdapterDiscoveryResultCallback result_cb );

void gl_central_start_discovery_nodedup( Adapter *adapter );

#endif

