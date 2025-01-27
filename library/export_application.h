/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/export_application.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef EXPORT_APPLICATION_H
#define EXPORT_APPLICATION_H

#include "list.h"
#include "boolean.h"

typedef struct
{
	char *where;
	LIST *folder_name_list;
	LIST *export_table_list;
} EXPORT_APPLICATION;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
EXPORT_APPLICATION *export_application_new(
		char *application_name,
		boolean system_folders_boolean,
		char *data_directory );

/* Process */
/* ------- */
EXPORT_APPLICATION *export_application_calloc(
		void );

/* Usage */
/* ----- */
LIST *export_application_folder_name_list(
		const char *folder_primary_key,
		const char *folder_table,
		char *export_application_where );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *export_application_where(
		const char *role_folder_table,
		const char *role_system,
		const char *role_supervisor,
		const char *folder_primary_key,
		boolean system_folders_boolean );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *export_application_role_folder_subquery(
		const char *role_folder_table,
		const char *role_system,
		const char *role_supervisor,
		const char *folder_primary_key,
		boolean system_folders_boolean );

/* Process */
/* ------- */

/* Returns either parameter */
/* ------------------------ */
const char *export_application_role(
		const char *role_system,
		const char *role_supervisor,
		boolean system_folders_boolean );

/* Driver */
/* ------ */
void export_application_spreadsheet_output(
		char *application_name,
		LIST *export_table_list,
		char *data_directory );

/* Driver */
/* ------ */
void export_application_shell_output(
		char *application_name,
		LIST *export_table_list,
		char *data_directory );

/* Driver */
/* ------ */
void export_application_html_output(
		LIST *export_table_list );

#endif
