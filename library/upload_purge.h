/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/upload_purge.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef UPLOAD_PURGE_H
#define UPLOAD_PURGE_H

#include "boolean.h"
#include "list.h"

#define UPLOAD_PURGE_OLDER_MINUTES	60

typedef struct
{
	char *directory;
	LIST *application_not_null_data_list;
	char *filename_system_string;
	LIST *filename_list;
	LIST *system_list;
} UPLOAD_PURGE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
UPLOAD_PURGE *upload_purge_new(
		char *application_name,
		char *key,
		char *table_name,
		char *column_name,
		char *appaserver_parameter_upload_directory );

/* Process */
/* ------- */
UPLOAD_PURGE *upload_purge_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *upload_purge_directory(
		char *application_name,
		char *key,
		char *appaserver_parameter_upload_directory );

LIST *upload_purge_filename_list(
		char *upload_purge_filename_system_string );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *upload_purge_filename_system_string(
		const int upload_purge_older_minutes,
		char *upload_purge_directory );

/* Usage */
/* ----- */
LIST *upload_purge_system_list(
		LIST *application_not_null_data_list,
		LIST *upload_purge_filename_list );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *upload_purge_system_string(
		char *filename );

/* Driver */
/* ------ */
void upload_purge_execute(
		char *upload_purge_directory,
		LIST *upload_purge_system_list,
		boolean display_only_boolean );

#endif
