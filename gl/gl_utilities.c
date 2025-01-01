//gl_utilities.c

#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

#include <glib.h>

#include "gl_colors.h"
#include "gl_utilities.h"


static int s_log_level;


int gl_log( int level, const char *format, ... )
{
	time_t now;
	struct tm *tm_info;
	char time_buff[20];
	va_list args;
	char logformat[1000];

	// Get current local time and format string
	time( &now );
	tm_info = localtime( &now );
	strftime( time_buff, 80, "%Y-%m-%d %H:%M:%S", tm_info );

	va_start( args, format );

	// Always print errors to stderr
	if( level == GL_LOG_ERROR ){
		snprintf( logformat, sizeof(logformat), "%s %sERROR %s%s", time_buff, CRED, format, CNRM );
		vfprintf( stderr, logformat, args );
	}
	else if( level <= gl_get_log_level() ){
		if( level == GL_LOG_ALERT ) snprintf( logformat, sizeof(logformat), "%s %sALERT %s%s", time_buff, CYEL, format, CNRM );
		else if( level == GL_LOG_INFO ) snprintf( logformat, sizeof(logformat), "%s %sINFO  %s%s", time_buff, CLGRY, format, CNRM );
		else if( level == GL_LOG_DEBUG ) snprintf( logformat, sizeof(logformat), "%s %sDEBUG %s%s", time_buff, CNRM, format, CNRM );
		vprintf( logformat, args );
	}

	va_end(args);

	return( 0 );
}


void gl_set_log_level( int level )
{
	s_log_level = level;
}


int gl_get_log_level( void )
{
	return( s_log_level );
}


char *gl_get_uuid_string( GPtrArray *adv_uuids )
{
	static char uuid_string[100] = "";
	char *cur = uuid_string;
	char *end = uuid_string + sizeof( uuid_string );

	if( adv_uuids != NULL ){
		for( guint i = 0; i < adv_uuids->len; i++ ){
			if( cur < end ){
				 cur += snprintf( cur, (size_t) (end-cur), "%.8s", (char *)g_ptr_array_index( adv_uuids, i ));
			}
			if( (cur < end) && (i < adv_uuids->len -1) ){
				cur += snprintf( cur, (size_t) (end-cur), ", " );
			}
		}
	}
	else{
		snprintf( uuid_string, sizeof(uuid_string), "(null)" );
	}

	//printf( "[%s] '%s'\n", __func__, uuid_string);

	return( uuid_string );
}

#if 0
void gl_gba_to_string( const GByteArray *byteArray, char *gba_string, size_t len )
{
	char *ptr = gba_string;

	for( guint j = 0; j < byteArray->len; j++ ) ptr += snprintf( ptr, (size_t) (ptr-gba_string), "%02x ", *(byteArray->data+j) );
	return;
}

#endif
