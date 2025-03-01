/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/rename_column.c	 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "shell_script.h"
#include "attribute.h"
#include "folder_attribute.h"
#include "relation.h"
#include "role.h"
#include "foreign_attribute.h"
#include "process_parameter.h"
#include "process_generic.h"
#include "rename_column.h"

RENAME_COLUMN *rename_column_new(
		char *application_name,
		char *folder_name,
		char *old_attribute_name,
		char *new_attribute_name,
		char *data_directory )
{
	RENAME_COLUMN *rename_column;

	if ( !application_name
	||   !folder_name
	||   !old_attribute_name
	||   !new_attribute_name
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

	rename_column = rename_column_calloc();

	rename_column->folder_attribute =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_attribute_fetch(
			folder_name,
			new_attribute_name,
			1 /* fetch_attribute */ );

	rename_column->folder_table_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_table_name(
			application_name,
			folder_name );

	rename_column->attribute_database_datatype =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		attribute_database_datatype(
			rename_column->
				folder_attribute->
				attribute->
				datatype_name,
			rename_column->
				folder_attribute->
				attribute->
				width,
			rename_column->
				folder_attribute->
				attribute->
				float_decimal_places,
			rename_column->
				folder_attribute->
				primary_key_index,
			rename_column->
				folder_attribute->
				default_value );

	rename_column->execute_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		rename_column_execute_system_string(
			old_attribute_name,
			new_attribute_name,
			rename_column->folder_table_name,
			rename_column->attribute_database_datatype );

	rename_column->update_statement_list =
		rename_column_update_statement_list(
			ATTRIBUTE_TABLE,
			FOLDER_ATTRIBUTE_TABLE,
			RELATION_TABLE,
			FOREIGN_ATTRIBUTE_TABLE,
			ROW_SECURITY_ROLE_UPDATE_TABLE,
			ROLE_ATTRIBUTE_EXCLUDE_TABLE,
			PROCESS_PARAMETER_TABLE,
			PROCESS_SET_PARAMETER_TABLE,
			PROCESS_GENERIC_VALUE_TABLE,
			old_attribute_name,
			new_attribute_name );

	rename_column->shell_script =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		rename_column_shell_script(
			application_name,
			rename_column->execute_system_string,
			rename_column->update_statement_list );

	rename_column->process_filespecification =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		rename_column_process_filespecification(
			application_name,
			folder_name,
			new_attribute_name,
			data_directory );

	return rename_column;
}

RENAME_COLUMN *rename_column_calloc( void )
{
	RENAME_COLUMN *rename_column;

	if ( ! ( rename_column = calloc( 1, sizeof ( RENAME_COLUMN ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return rename_column;
}

char *rename_column_execute_system_string(
		char *old_attribute_name,
		char *new_attribute_name,
		char *folder_table_name,
		char *attribute_database_datatype )
{
	static char system_string[ 256 ];

	if ( !old_attribute_name
	||   !new_attribute_name
	||   !folder_table_name
	||   !attribute_database_datatype )
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
		"echo \"alter table %s change %s %s %s;\" | "
		"tee_appaserver.sh | "
		"sql.e 2>&1",
		folder_table_name,
		old_attribute_name,
		new_attribute_name,
		attribute_database_datatype );

	return system_string;
}

char *rename_column_process_filespecification(
		char *application_name,
		char *folder_name,
		char *new_attribute_name,
		char *data_directory )
{
	static char process_filespecification[ 256 ];

	if ( !application_name
	||   !folder_name
	||   !new_attribute_name
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

	snprintf(
		process_filespecification,
		sizeof ( process_filespecification ),
		"%s/%s/rename_column_%s_%s.sh",
		data_directory,
		application_name,
		folder_name,
		new_attribute_name );

	return process_filespecification;
}

char *rename_column_shell_script(
		char *application_name,
		char *execute_system_string,
		LIST *update_statement_list )
{
	char shell_script[ 65536 ];
	char *ptr = shell_script;
	char *update_statement;

	if ( !application_name
	||   !execute_system_string
	||   !list_rewind( update_statement_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
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
		update_statement =
			list_get(
				update_statement_list );

		ptr += sprintf(
			ptr,
			"%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			rename_column_system_string(
				update_statement ) );

	} while ( list_next( update_statement_list ) );

	ptr += sprintf(
		ptr,
		"%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		shell_script_close_string() );

	return strdup( shell_script );
}

LIST *rename_column_update_statement_list(
		const char *attribute_table,
		const char *folder_attribute_table,
		const char *relation_table,
		const char *foreign_attribute_table,
		const char *row_security_role_update_table,
		const char *role_attribute_exclude_table,
		const char *process_parameter_table,
		const char *process_set_parameter_table,
		const char *process_generic_value_table,
		char *old_attribute_name,
		char *new_attribute_name )
{
	LIST *statement_list;

	if ( !old_attribute_name
	||   !new_attribute_name )
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

	statement_list = list_new();

	list_set(
		statement_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		rename_column_update_statement(
			attribute_table,
			old_attribute_name,
			(new_attribute_name) ? new_attribute_name : "",
			"attribute" ) );

	list_set(
		statement_list,
		rename_column_update_statement(
			folder_attribute_table,
			old_attribute_name,
			(new_attribute_name) ? new_attribute_name : "",
			"attribute" ) );

	list_set(
		statement_list,
		rename_column_update_statement(
			relation_table,
			old_attribute_name,
			(new_attribute_name) ? new_attribute_name : "",
			"related_attribute" ) );

	list_set(
		statement_list,
		rename_column_update_statement(
			foreign_attribute_table,
			old_attribute_name,
			(new_attribute_name) ? new_attribute_name : "",
			"related_attribute" ) );

	list_set(
		statement_list,
		rename_column_update_statement(
			foreign_attribute_table,
			old_attribute_name,
			(new_attribute_name) ? new_attribute_name : "",
			"foreign_attribute" ) );

	list_set(
		statement_list,
		rename_column_update_statement(
			row_security_role_update_table,
			old_attribute_name,
			new_attribute_name,
			"attribute_not_null" ) );

	list_set(
		statement_list,
		rename_column_update_statement(
			role_attribute_exclude_table,
			old_attribute_name,
			new_attribute_name,
			"attribute" ) );

	list_set(
		statement_list,
		rename_column_update_statement(
			process_parameter_table,
			old_attribute_name,
			new_attribute_name,
			"attribute" ) );

	list_set(
		statement_list,
		rename_column_update_statement(
			process_set_parameter_table,
			old_attribute_name,
			new_attribute_name,
			"attribute" ) );

	list_set(
		statement_list,
		rename_column_update_statement(
			process_generic_value_table,
			old_attribute_name,
			(new_attribute_name) ? new_attribute_name : "",
			"datatype_attribute" ) );

	list_set(
		statement_list,
		rename_column_update_statement(
			process_generic_value_table,
			old_attribute_name,
			(new_attribute_name) ? new_attribute_name : "",
			"date_attribute" ) );

	list_set(
		statement_list,
		rename_column_update_statement(
			process_generic_value_table,
			old_attribute_name,
			(new_attribute_name) ? new_attribute_name : "",
			"time_attribute" ) );

	list_set(
		statement_list,
		rename_column_update_statement(
			process_generic_value_table,
			old_attribute_name,
			new_attribute_name,
			"value_attribute" ) );

	return statement_list;
}

char *rename_column_update_statement(
		const char *table,
		char *old_attribute_name,
		char *new_attribute_name,
		const char *folder_attribute_name )
{
	char update_statement[ 1024 ];

	if ( !old_attribute_name
	||   !new_attribute_name )
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
		update_statement,
		sizeof ( update_statement ),
		"update %s set %s='%s' where %s='%s';",
		table,
		folder_attribute_name,
		new_attribute_name,
		folder_attribute_name,
		old_attribute_name );

	return strdup( update_statement );
}

char *rename_column_system_string( char *statement )
{
	static char system_string[ 256 ];

	if ( !statement )
	{
		char message[ 128 ];

		sprintf(message, "statement is empty." );

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
		"echo \"%s\" | "
		"tee_appaserver.sh | "
		"sql.e 2>&1",
		statement );

	return system_string;
}

