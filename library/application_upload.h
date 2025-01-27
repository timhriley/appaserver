/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application_upload.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef APPLICATION_UPLOAD_H
#define APPLICATION_UPLOAD_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include "boolean.h"
#include "list.h"

typedef struct
{
	char *directory;
	char *create_directory_system_string;
	char *delete_system_string;
} APPLICATION_UPLOAD;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPLICATION_UPLOAD *application_upload_new(
		char *application_name,
		char *upload_directory );

/* Process */
/* ------- */
APPLICATION_UPLOAD *application_upload_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *application_upload_directory(
		char *application_name,
		char *upload_directory );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_upload_create_directory_system_string(
		char *application_upload_directory );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_upload_delete_system_string(
		char *application_upload_directory );

#endif
