#ifndef GL_UTILITIES_H
#define GL_UTILITIES_H

#define GL_LOG_ERROR (0)
#define GL_LOG_ALERT (1)
#define GL_LOG_INFO  (2)
#define GL_LOG_DEBUG (3)

#define BSWAP64(x) \
    ((((x) & 0xff00000000000000ull) >> 56) | \
     (((x) & 0x00ff000000000000ull) >> 40) | \
     (((x) & 0x0000ff0000000000ull) >> 24) | \
     (((x) & 0x000000ff00000000ull) >> 8)  | \
     (((x) & 0x00000000ff000000ull) << 8)  | \
     (((x) & 0x0000000000ff0000ull) << 24) | \
     (((x) & 0x000000000000ff00ull) << 40) | \
     (((x) & 0x00000000000000ffull) << 56))


#include <glib.h>


int gl_log( int level, const char * format, ... );

void gl_set_log_level( int level );

int gl_get_log_level( void );

char *gl_get_uuid_string( GPtrArray *adv_uuids );

void gl_gba_to_string( const GByteArray *byteArray, char *gba_string, size_t len );

#endif
