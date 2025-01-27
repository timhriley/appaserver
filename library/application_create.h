/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application_create.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef APPLICATION_CREATE_H
#define APPLICATION_CREATE_H

#include "boolean.h"
#include "list.h"
#include "application_log.h"
#include "application_backup.h"
#include "application_data.h"
#include "application_upload.h"
#include "application_login.h"

#define APPLICATION_CREATE_DEFAULT_TITLE	"Appaserver Application"
#define APPLICATION_CREATE_TEMPLATE_NAME	"template"
#define APPLICATION_CREATE_EXECUTABLE		"create_empty_application"

typedef struct
{
	boolean application_name_invalid;
	char *database_system_string;
	APPLICATION_LOG *application_log;
	boolean application_exists_boolean;
	APPLICATION_BACKUP *application_backup;
	APPLICATION_DATA *application_data;
	APPLICATION_UPLOAD *application_upload;
	APPLICATION_LOGIN *application_login;
} APPLICATION_CREATE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPLICATION_CREATE *application_create_new(
		char *application_name,
		char *application_title,
		char *document_root,
		char *data_directory,
		char *upload_directory,
		char *log_directory,
		char *mount_point );

/* Process */
/* ------- */
APPLICATION_CREATE *application_create_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_create_database_system_string(
		char *application_name );

/* Usage */
/* ----- */
boolean application_create_system(
		boolean execute_boolean,
		char *system_string );

/* Driver */
/* ------ */
boolean application_create_execute(
		boolean execute_boolean,
		APPLICATION_CREATE *application_create );

#endif
