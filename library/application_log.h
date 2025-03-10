/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application_log.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef APPLICATION_LOG_H
#define APPLICATION_LOG_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include "boolean.h"
#include "list.h"

#define APPLICATION_LOG_EXTENSION	"err"

typedef struct
{
	char *filename;
	char *create_system_string;
	char *delete_system_string;
} APPLICATION_LOG;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPLICATION_LOG *application_log_new(
		const char *application_log_extension,
		char *application_name,
		char *log_directory );

/* Process */
/* ------- */
APPLICATION_LOG *application_log_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_log_filename(
		const char *application_log_extension,
		char *application_name,
		char *log_directory );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_log_create_system_string(
		char *application_log_filename );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_log_delete_system_string(
		char *application_log_filename );

/* Usage */
/* ----- */
boolean application_log_exists_boolean(
		char *application_name,
		char *application_log_filename );

#endif
