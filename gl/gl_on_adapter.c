/***********************************************************************
 *
 * gl_on_adapter.c
 *
*/


#include "adapter.h"

#include "gl_utilities.h"
#include "gl_on_adapter.h"


/***********************************************************************
 *
*/
void gl_on_adapter_powered_state_changed( Adapter *adapter, gboolean state )
{
	gl_log( GL_LOG_INFO, "[%s]: Adapter '%s' powered (%s)\n", __func__, binc_adapter_get_path(adapter), state ? "on" : "off");

}

