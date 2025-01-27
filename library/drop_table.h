/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/drop_table.h				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#ifndef DROP_TABLE_H
#define DROP_TABLE_H

#include "list.h"
#include "boolean.h"
#include "session.h"

#define DROP_TABLE_EXECUTABLE	"drop_table"

typedef struct
{
	SESSION_PROCESS *session_process;
	char *execute_system_string;
	char *shell_script;
	char *appaserver_parameter_data_directory;
	char *process_filename;
} DROP_TABLE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DROP_TABLE *drop_table_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *table_name );

/* Process */
/* ------- */
DROP_TABLE *drop_table_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *drop_table_execute_system_string(
		char *folder_table_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *drop_table_process_filename(
		char *table_name,
		char *data_directory );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *drop_table_href_string(
		const char *drop_table_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *table_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *drop_table_shell_script(
		char *application_name,
		char *drop_table_execute_system_string );

#endif
