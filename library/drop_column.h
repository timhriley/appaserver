/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/drop_column.h				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#ifndef DROP_COLUMN_H
#define DROP_COLUMN_H

#include "list.h"
#include "boolean.h"
#include "session.h"

#define DROP_COLUMN_EXECUTABLE	"drop_column"

typedef struct
{
	SESSION_PROCESS *session_process;
	char *folder_table_name;
	char *execute_system_string;
	LIST *delete_statement_list;
	char *shell_script;
	char *appaserver_parameter_data_directory;
	char *process_filespecification;
} DROP_COLUMN;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DROP_COLUMN *drop_column_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *attribute_name );

/* Process */
/* ------- */
DROP_COLUMN *drop_column_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *drop_column_execute_system_string(
		char *attribute_name,
		char *folder_table_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *drop_column_process_filespecification(
		char *application_name,
		char *folder_name,
		char *attribute_name,
		char *data_directory );

/* Usage */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *drop_column_href_string(
		const char *drop_column_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *attribute_name );

/* Usage */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *drop_column_shell_script(
		char *application_name,
		char *drop_column_execute_system_string,
		LIST *drop_column_delete_statement_list );

/* Usage */
/* ----- */
LIST *drop_column_delete_statement_list(
		const char *folder_attribute_table,
		const char *foreign_attribute_table,
		const char *row_security_role_update_table,
		char *folder_name,
		char *attribute_name );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *drop_column_delete_statement(
		const char *table,
		char *folder_name,
		char *attribute_name,
		const char *column_folder_name,
		const char *column_attribute_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *drop_column_delete_statement_system_string(
		char *delete_statement );

#endif
