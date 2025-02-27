/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application_delete.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef APPLICATION_DELETE_H
#define APPLICATION_DELETE_H

#include "boolean.h"
#include "list.h"
#include "application_log.h"
#include "application_backup.h"
#include "application_data.h"
#include "application_upload.h"
#include "application_login.h"

typedef struct
{
	boolean application_name_invalid;
	APPLICATION_LOG *application_log;
	boolean application_log_exists_boolean;
	APPLICATION_BACKUP *application_backup;
	APPLICATION_DATA *application_data;
	APPLICATION_UPLOAD *application_upload;
	APPLICATION_LOGIN *application_login;
	char *drop_system_string;
} APPLICATION_DELETE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPLICATION_DELETE *application_delete_new(
		char *application_name,
		char *document_root,
		char *data_directory,
		char *upload_directory,
		char *log_directory,
		char *mount_point );

/* Process */
/* ------- */
APPLICATION_DELETE *application_delete_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *application_delete_drop_system_string(
		char *application_name );

/* Usage */
/* ----- */
void application_delete_system(
		boolean execute_boolean,
		char *system_string );

#endif
