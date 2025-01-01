#ifndef GL_ON_PERIPHERAL_H
#define GL_ON_PERIPHERAL_H

#include <glib.h>

#include "adapter.h"
#include "device.h"


//**********************************************************************
void gl_on_peripheral_central_connection_state_changed( Adapter *peripheral, Device *central );

#endif

