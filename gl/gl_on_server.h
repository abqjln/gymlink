#ifndef GL_ON_SERVER_H
#define GL_ON_SERVER_H

#include "application.h"

#include "gl_server.h"


//**********************************************************************
const char *gl_on_server_charact_read_request( const Application *application,
		const char *address, const char *gl_service_uuid, const char *charact_uuid );

const char *gl_on_server_charact_write_request( const Application *application,
		const char *address, const char *gl_service_uuid, const char *charact_uuid, GByteArray *byteArray );

void gl_on_server_charact_updated( const Application *application,
		const char *gl_service_uuid, const char *charact_uuid, GByteArray *byteArray );

const char *gl_on_server_desc_read_request(const Application *application,
		const char *address, const char *gl_service_uuid, const char *charact_uuid, const char *desc_uuid);

const char *gl_on_server_desc_write_request( const Application *application,
		const char *address, const char *gl_service_uuid, const char *charact_uuid,
		const char *desc_uuid, const GByteArray *byteArray );

void gl_on_server_enable_notify( const Application *application,
		const char *gl_service_uuid, const char *charact_uuid );

void gl_on_server_disable_notify(const Application *application,
		const char *gl_service_uuid, const char *charact_uuid );

#endif
