/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/rename_table.c	 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "shell_script.h"
#include "folder.h"
#include "role_folder.h"
#include "foreign_attribute.h"
#include "relation.h"
#include "folder_attribute.h"
#include "process_parameter.h"
#include "folder_operation.h"
#include "process_generic.h"
#include "create_table.h"
#include "rename_table.h"

RENAME_TABLE *rename_table_new(
		char *application_name,
		char *old_folder_name,
		char *new_folder_name,
		char *data_directory )
{
	RENAME_TABLE *rename_table;
	LIST *primary_key_list;

	if ( !application_name
	||   !old_folder_name
	||   !new_folder_name
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


	rename_table = rename_table_calloc();

	rename_table->old_folder_table_name =
		strdup(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			folder_table_name(
				application_name,
				old_folder_name ) );

	rename_table->new_folder_table_name =
		strdup(
			folder_table_name(
				application_name,
				new_folder_name ) );

	rename_table->execute_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		rename_table_execute_system_string(
			rename_table->old_folder_table_name,
			rename_table->new_folder_table_name );

	rename_table->drop_index_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		rename_table_drop_index_system_string(
			CREATE_TABLE_UNIQUE_SUFFIX,
			rename_table->old_folder_table_name,
			rename_table->new_folder_table_name );

	primary_key_list =
		folder_attribute_fetch_primary_key_list(
			new_folder_name );

	rename_table->create_table_primary_index_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		create_table_primary_index_system_string(
			CREATE_TABLE_UNIQUE_SUFFIX,
			primary_key_list,
			rename_table->new_folder_table_name );

	rename_table->update_statement_list =
		rename_table_update_statement_list(
			FOLDER_TABLE,
			ROLE_FOLDER_TABLE,
			RELATION_TABLE,
			FOLDER_ATTRIBUTE_TABLE,
			FOREIGN_ATTRIBUTE_TABLE,
			FOLDER_ROW_LEVEL_RESTRICTION_TABLE,
			PROCESS_PARAMETER_TABLE,
			PROCESS_SET_PARAMETER_TABLE,
			FOLDER_OPERATION_TABLE,
			PROCESS_GENERIC_TABLE,
			PROCESS_GENERIC_VALUE_TABLE,
			old_folder_name,
			new_folder_name );

	rename_table->shell_script =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		rename_table_shell_script(
			application_name,
			rename_table->execute_system_string,
			rename_table->drop_index_system_string,
			rename_table->create_table_primary_index_system_string,
			rename_table->update_statement_list );

	rename_table->process_filespecification =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		rename_table_process_filespecification(
			application_name,
			old_folder_name,
			data_directory );

	return rename_table;
}

RENAME_TABLE *rename_table_calloc( void )
{
	RENAME_TABLE *rename_table;

	if ( ! ( rename_table = calloc( 1, sizeof ( RENAME_TABLE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return rename_table;
}

char *rename_table_execute_system_string(
		char *old_folder_table_name,
		char *new_folder_table_name )
{
	static char system_string[ 256 ];

	if ( !old_folder_table_name
	||   !new_folder_table_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"echo \"alter table %s rename as %s\" | "
		"tee_appaserver.sh | "
		"sql.e 2>&1",
		old_folder_table_name,
		new_folder_table_name );

	return system_string;
}

char *rename_table_process_filespecification(
		char *application_name,
		char *old_folder_name,
		char *data_directory )
{
	if ( !application_name
	||   !old_folder_name
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

	return
	shell_script_filespecification(
		"rename_table" /* PROCESS_LABEL */,
		application_name,
		old_folder_name,
		data_directory );
}

char *rename_table_shell_script(
		char *application_name,
		char *execute_system_string,
		char *drop_index_system_string,
		char *create_table_primary_index_system_string,
		LIST *update_statement_list )
{
	char shell_script[ 65536 ];
	char *ptr = shell_script;
	char *update_statement;

	if ( !application_name
	||   !execute_system_string
	||   !drop_index_system_string
	||   !create_table_primary_index_system_string
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

	ptr += sprintf(
		ptr,
		"%s\n",
		drop_index_system_string );

	ptr += sprintf(
		ptr,
		"%s\n",
		create_table_primary_index_system_string );

	do {
		update_statement =
			list_get(
				update_statement_list );

	ptr +=
		sprintf(
			ptr,
			"%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			rename_table_system_string(
				update_statement ) );

	} while ( list_next( update_statement_list ) );

	ptr +=
		sprintf(
			ptr,
			"%s\n",
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			shell_script_close_string() );

	return strdup( shell_script );
}

LIST *rename_table_update_statement_list(
		const char *folder_table,
		const char *role_folder_table,
		const char *relation_table,
		const char *folder_attribute_table,
		const char *foreign_attribute_table,
		const char *folder_row_level_restriction_table,
		const char *process_parameter_table,
		const char *process_set_parameter_table,
		const char *folder_operation_table,
		const char *process_generic_table,
		const char *process_generic_value_table,
		char *old_folder_name,
		char *new_folder_name )
{
	LIST *statement_list;

	if ( !old_folder_name
	||   !new_folder_name )
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
		rename_table_update_statement(
			folder_table,
			old_folder_name,
			new_folder_name,
			FOLDER_PRIMARY_KEY ) );

	list_set(
		statement_list,
		rename_table_update_statement(
			role_folder_table,
			old_folder_name,
			new_folder_name,
			FOLDER_PRIMARY_KEY ) );

	list_set(
		statement_list,
		rename_table_update_statement(
			relation_table,
			old_folder_name,
			new_folder_name,
			FOLDER_PRIMARY_KEY ) );

	list_set(
		statement_list,
		rename_table_update_statement(
			relation_table,
			old_folder_name,
			new_folder_name,
			RELATION_FOREIGN_KEY ) );

	list_set(
		statement_list,
		rename_table_update_statement(
			folder_attribute_table,
			old_folder_name,
			new_folder_name,
			FOLDER_PRIMARY_KEY ) );

	list_set(
		statement_list,
		rename_table_update_statement(
			foreign_attribute_table,
			old_folder_name,
			new_folder_name,
			FOLDER_PRIMARY_KEY ) );

	list_set(
		statement_list,
		rename_table_update_statement(
			folder_row_level_restriction_table,
			old_folder_name,
			new_folder_name,
			FOLDER_PRIMARY_KEY ) );

	list_set(
		statement_list,
		rename_table_update_statement(
			process_parameter_table,
			old_folder_name,
			new_folder_name,
			FOLDER_PRIMARY_KEY ) );

	list_set(
		statement_list,
		rename_table_update_statement(
			process_set_parameter_table,
			old_folder_name,
			new_folder_name,
			FOLDER_PRIMARY_KEY ) );

	list_set(
		statement_list,
		rename_table_update_statement(
			folder_operation_table,
			old_folder_name,
			new_folder_name,
			FOLDER_PRIMARY_KEY ) );

	list_set(
		statement_list,
		rename_table_update_statement(
			process_generic_table,
			old_folder_name,
			new_folder_name,
			PROCESS_GENERIC_VALUE_FOLDER_PRIMARY_KEY ) );

	list_set(
		statement_list,
		rename_table_update_statement(
			process_generic_value_table,
			old_folder_name,
			new_folder_name,
			PROCESS_GENERIC_VALUE_FOLDER_PRIMARY_KEY ) );

	list_set(
		statement_list,
		rename_table_update_statement(
			process_generic_value_table,
			old_folder_name,
			new_folder_name,
			"datatype_folder" ) );

	list_set(
		statement_list,
		rename_table_update_statement(
			process_generic_value_table,
			old_folder_name,
			new_folder_name,
			"foreign_folder" ) );

	return statement_list;
}

char *rename_table_system_string( char *statement )
{
	static char system_string[ 512 ];

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

	sprintf(system_string,
		"echo \"%s\" | "
		"tee_appaserver.sh | "
		"sql.e 2>&1",
		statement );

	return system_string;
}

char *rename_table_update_statement(
		const char *table,
		char *old_folder_name,
		char *new_folder_name,
		const char *folder_attribute_name )
{
	char update_statement[ 256 ];

	if ( !old_folder_name
	||   !new_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(update_statement,
		"update %s set %s='%s' where %s='%s';",
		table,
		folder_attribute_name,
		new_folder_name,
		folder_attribute_name,
		old_folder_name );

	return strdup( update_statement );
}

char *rename_table_drop_index_system_string(
		const char *create_table_unique_suffix,
		char *old_folder_table_name,
		char *new_folder_table_name )
{
	static char system_string[ 256 ];

	if ( !old_folder_table_name
	||   !new_folder_table_name )
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

	sprintf(system_string,
		"echo \"alter table %s drop index %s;\" | "
		"tee_appaserver.sh | "
		"sql.e 2>&1",
		new_folder_table_name,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		create_table_unique_index_name(
			create_table_unique_suffix,
			old_folder_table_name ) );

	return system_string;
}

