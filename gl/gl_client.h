#ifndef GL_CLIENT_H
#define GL_CLIENT_H

#include <glib.h>

#include "adapter.h"
#include "device.h"

#include "gl_server.h"


//******************************************************************
extern GL_LClient *g_p_lclient; //Declare global pointer used in libs


//******************************************************************
int gl_client_close( gpointer gptr );

#endif
