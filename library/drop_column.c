/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/drop_column.c				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "application.h"
#include "appaserver_parameter.h"
#include "folder_attribute.h"
#include "foreign_attribute.h"
#include "row_security_role_update.h"
#include "shell_script.h"
#include "drop_column.h"

DROP_COLUMN *drop_column_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *attribute_name )
{
	DROP_COLUMN *drop_column;

	if ( !argc
	||   !argv
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	drop_column = drop_column_calloc();

	drop_column->session_process =
		/* --------------------------------------------- */
		/* Sets appaserver environment and outputs argv. */
		/* Each parameter is security inspected.	 */
		/* Any error will exit( 1 ).			 */
		/* --------------------------------------------- */
		session_process_integrity_exit(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			DROP_COLUMN_EXECUTABLE
				/* process_or_set_name */ );

	drop_column->folder_table_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_table_name(
			application_name,
			folder_name );

	drop_column->execute_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		drop_column_execute_system_string(
			attribute_name,
			drop_column->folder_table_name );

	drop_column->delete_statement_list =
		drop_column_delete_statement_list(
			FOLDER_ATTRIBUTE_TABLE,
			FOREIGN_ATTRIBUTE_TABLE,
			ROW_SECURITY_ROLE_UPDATE_TABLE,
			folder_name,
			attribute_name );

	drop_column->shell_script =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		drop_column_shell_script(
			application_name,
			drop_column->execute_system_string,
			drop_column->delete_statement_list );

	drop_column->appaserver_parameter_data_directory =
		/* ------------------------------------------------ */
		/* Returns component of global_appaserver_parameter */
		/* ------------------------------------------------ */
		appaserver_parameter_data_directory();

	drop_column->process_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		drop_column_process_filename(
			folder_name,
			attribute_name,
			drop_column->appaserver_parameter_data_directory );

	return drop_column;
}

DROP_COLUMN *drop_column_calloc( void )
{
	DROP_COLUMN *drop_column;

	if ( ! ( drop_column = calloc( 1, sizeof ( DROP_COLUMN ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return drop_column;
}

char *drop_column_href_string(
		const char *drop_column_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *attribute_name )
{
	char href_string[ 1024 ];

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		href_string,
		sizeof ( href_string ),
		"%s/%s?%s+%s+%s+%s+%s+%s+%s+%s+%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_http_prompt(
			/* ----------------------------------------- */
			/* Returns component of APPASERVER_PARAMETER */
			/* ----------------------------------------- */
			appaserver_parameter_cgi_directory(),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
			application_ssl_support_boolean(
				application_name ) ),
		drop_column_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		drop_column_executable /* process_name */,
		folder_name,
		attribute_name,
		"y" /* content_type_yn */,
		"y" /* execute_yn */ );

	return strdup( href_string );
}

char *drop_column_execute_system_string(
		char *attribute_name,
		char *folder_table_name )
{
	static char system_string[ 512 ];

	if ( !attribute_name
	||   !folder_table_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"alter table %s drop %s;\"	|"
		"tee_appaserver.sh			|"
		"sql.e 2>&1 				 ",
		 folder_table_name,
		 attribute_name );

	return system_string;
}

char *drop_column_delete_statement_system_string( char *delete_statement )
{
	static char system_string[ 512 ];

	if ( !delete_statement )
	{
		char message[ 128 ];

		sprintf(message, "delete_statement is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		/* Stub */
		/* ---- */
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"%s\" | sql.e 2>&1",
		delete_statement );

	return system_string;
}

LIST *drop_column_delete_statement_list(
		const char *folder_attribute_table,
		const char *foreign_attribute_table,
		const char *row_security_role_update_table,
		char *folder_name,
		char *attribute_name )
{
	LIST *statement_list;

	if ( !folder_name
	||   !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	statement_list = list_new();

	list_set(
		statement_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		drop_column_delete_statement(
			folder_attribute_table,
			folder_name,
			attribute_name,
			"table_name" /* column_folder_name */,
			"column_name" /* column_attribute_name */ ) );

	list_set(
		statement_list,
		drop_column_delete_statement(
			foreign_attribute_table,
			folder_name,
			attribute_name,
			"table_name" /* column_folder_name */,
			"related_column" /* column_attribute_name */ ) );

	list_set(
		statement_list,
		drop_column_delete_statement(
			foreign_attribute_table,
			folder_name,
			attribute_name,
			"table_name" /* column_folder_name */,
			"foreign_column" /* column_attribute_name */ ) );

	list_set(
		statement_list,
		drop_column_delete_statement(
			foreign_attribute_table,
			folder_name,
			attribute_name,
			"related_table" /* column_folder_name */,
			"related_column" /* column_attribute_name */ ) );

	list_set(
		statement_list,
		drop_column_delete_statement(
			foreign_attribute_table,
			folder_name,
			attribute_name,
			"related_table" /* column_folder_name */,
			"foreign_column" /* column_attribute_name */ ) );

	list_set(
		statement_list,
		drop_column_delete_statement(
			row_security_role_update_table,
			folder_name,
			attribute_name,
			"table_name" /* column_folder_name */,
			"column_not_null" /* column_attribute_name */ ) );

	return statement_list;
}

char *drop_column_delete_statement(
		const char *table,
		char *folder_name,
		char *attribute_name,
		const char *column_folder_name,
		const char *column_attribute_name )
{
	char delete_statement[ 256 ];

	if ( !folder_name
	||   !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		/* Stub */
		/* ---- */
		exit( 1 );
	}

	snprintf(
		delete_statement,
		sizeof ( delete_statement ),
		"delete from %s where %s='%s' and %s='%s';",
		table,
		column_folder_name,
		folder_name,
		column_attribute_name,
		attribute_name );

	return strdup( delete_statement );
}

char *drop_column_shell_script(
		char *application_name,
		char *execute_system_string,
		LIST *delete_statement_list )
{
	char shell_script[ 4096 ];
	char *ptr = shell_script;

	if ( !application_name
	||   !execute_system_string
	||   !list_rewind( delete_statement_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		/* Stub */
		/* ---- */
		exit(1 );
	}

	ptr += sprintf(
		ptr,
		"%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		shell_script_open_string(
			application_name ) );

	ptr += sprintf(
		ptr,
		"%s\n",
		execute_system_string );

	do {
		ptr += sprintf(
			ptr,
			"%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			drop_column_delete_statement_system_string(
				list_get(
					delete_statement_list ) ) );

	} while ( list_next( delete_statement_list ) );


	ptr += sprintf(
		ptr,
		"%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		shell_script_close_string() );

	return strdup( shell_script );
}

char *drop_column_process_filename(
		char *folder_name,
		char *attribute_name,
		char *data_directory )
{
	static char process_filename[ 256 ];

	if ( !folder_name
	||   !attribute_name
	||   !data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(process_filename,
		"%s/drop_column_%s_%s.sh",
		data_directory,
		folder_name,
		attribute_name );

	return process_filename;
}

