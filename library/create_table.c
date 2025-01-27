/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/create_table.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "application.h"
#include "folder.h"
#include "folder_attribute.h"
#include "create_table.h"

CREATE_TABLE *create_table_new(
		char *application_name,
		char *folder_name,
		boolean execute_boolean,
		char *data_directory )
{
	CREATE_TABLE *create_table;

	if ( !application_name
	||   !folder_name
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

	create_table = create_table_calloc();

	create_table->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			(char *)0 /* role_name */,
			(LIST *)0 /* exclude_attribute_name_list */,
			/* -------------------------------------- */
			/* Sets folder_attribute_primary_list	  */
			/* Sets folder_attribute_primary_key_list */
			/* Sets folder_attribute_name_list	  */
			/* -------------------------------------- */
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */ );

	create_table->folder_table_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_table_name(
			application_name,
			folder_name );

	create_table->drop_statement =
		create_table_drop_statement(
			create_table->folder->create_view_statement,
			create_table->folder_table_name );

	create_table->statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		create_table_statement(
			create_table->folder->create_view_statement,
			create_table->folder->folder_attribute_list,
			create_table->folder->data_directory,
			create_table->folder->index_directory,
			create_table->folder->storage_engine,
			create_table->folder_table_name );

	create_table->unique_index_statement =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		create_table_unique_index_statement(
			create_table->
				folder->
				folder_attribute_primary_key_list,
			create_table->folder_table_name );

	create_table->additional_unique_index_list =
		create_table_additional_unique_index_list(
			create_table->folder->folder_attribute_list,
			create_table->folder_table_name );

	create_table->additional_index_list =
		create_table_additional_index_list(
			create_table->folder->folder_attribute_list,
			create_table->folder_table_name );

	create_table->shell_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		create_table_shell_filename(
			folder_name,
			data_directory );

	create_table->sql_statement_list =
		create_table_sql_statement_list(
			create_table->drop_statement,
			create_table->statement,
			create_table->unique_index_statement,
			create_table->additional_unique_index_list,
			create_table->additional_index_list );

	create_table->shell_script =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		create_table_shell_script(
			application_name,
			execute_boolean,
			create_table->shell_filename,
			create_table->sql_statement_list );

	return create_table;
}

CREATE_TABLE *create_table_calloc( void )
{
	CREATE_TABLE *create_table;

	if ( ! ( create_table = calloc( 1, sizeof ( CREATE_TABLE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return create_table;
}

char *create_table_statement(
		char *create_view_statement,
		LIST *folder_attribute_list,
		char *data_directory,
		char *index_directory,
		char *storage_engine,
		char *folder_table_name )
{
	char statement[ 4096 ];
	char *ptr = statement;
	boolean first_time = 1;
	FOLDER_ATTRIBUTE *folder_attribute;
	char *database_datatype;

	if ( !list_rewind( folder_attribute_list )
	||   !folder_table_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( create_view_statement )
	{
		return create_view_statement;
	}

	ptr += sprintf(
		ptr,
		"create table %s (",
		folder_table_name );

	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( !folder_attribute->attribute
		||   !folder_attribute->attribute->datatype_name
		||   !folder_attribute->attribute->width )
		{
			char message[ 128 ];

			sprintf(message,
	"for attribute=(%s), folder_attribute->attribute is incomplete.",
				folder_attribute->attribute_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( first_time )
			first_time = 0;
		else
			ptr += sprintf( ptr, "," );

		database_datatype =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			attribute_database_datatype(
				folder_attribute->attribute->datatype_name,
				folder_attribute->attribute->width,
				folder_attribute->
					attribute->
					float_decimal_places,
				folder_attribute->primary_key_index,
				folder_attribute->default_value );

		ptr += sprintf(
			ptr,
			"%s %s",
			folder_attribute->attribute_name,
			database_datatype );

		free( database_datatype );

	} while ( list_next( folder_attribute_list ) );

	ptr += sprintf( ptr, ")" );

	if ( data_directory )
	{
		ptr += sprintf(
			ptr,
			" data directory = '%s',",
			data_directory );
	}

	if ( index_directory )
	{
		ptr += sprintf(
			ptr,
			" index directory = '%s',",
			index_directory );
	}

	ptr += sprintf(
		ptr,
		" engine %s;",
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		create_table_storage_engine(
			CREATE_TABLE_MYISAM_ENGINE,
			storage_engine ) );

	return strdup( statement );
}

char *create_table_shell_filename(
		char *folder_name,
		char *data_directory )
{
	static char shell_filename[ 128 ];

	if ( !folder_name
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
		shell_filename,
		sizeof ( shell_filename ),
		"%s/create_table_%s.sh",
		data_directory,
		folder_name );

	return shell_filename;
}

LIST *create_table_additional_unique_index_list(
		LIST *folder_attribute_list,
		char *folder_table_name )
{
	LIST *additional_unique_index_name_list;
	char *additional_unique_index_name;
	LIST *list = list_new();
	char statement[ 256 ];

	if ( !list_length( folder_attribute_list )
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

	additional_unique_index_name_list =
		create_table_additional_unique_index_name_list(
			folder_attribute_list );

	if ( list_rewind( additional_unique_index_name_list ) )
	do {
		additional_unique_index_name =
			list_get(
				additional_unique_index_name_list );

		snprintf(
			statement,
			sizeof ( statement ),
			"create unique index %s "
			"on %s (%s);",
			additional_unique_index_name,
			folder_table_name,
			additional_unique_index_name );

		list_set( list, strdup( statement ) );

	} while ( list_next( additional_unique_index_name_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

LIST *create_table_additional_index_list(
		LIST *folder_attribute_list,
		char *folder_table_name )
{
	LIST *additional_index_name_list;
	LIST *list = list_new();
	char *additional_index_name;
	char statement[ 256 ];

	if ( !list_length( folder_attribute_list )
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

	additional_index_name_list =
		create_table_additional_index_name_list(
			folder_attribute_list );

	if ( list_rewind( additional_index_name_list ) )
	do {
		additional_index_name =
			list_get(
				additional_index_name_list );

		snprintf(
			statement,
			sizeof ( statement ),
			"create index %s on %s (%s);",
			additional_index_name,
			folder_table_name,
			additional_index_name );

		list_set( list, strdup( statement ) );

	} while ( list_next( additional_index_name_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

char *create_table_unique_index_statement(
		LIST *folder_attribute_primary_key_list,
		char *folder_table_name )
{
	static char statement[ 256 ];

	if ( !list_length( folder_attribute_primary_key_list )
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
		statement,
		sizeof ( statement ),
		"create unique index %s on %s (%s);",
		folder_table_name,
		folder_table_name,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_display_delimited(
			folder_attribute_primary_key_list,
			',' ) );

	return statement;
}

LIST *create_table_sql_statement_list(
		char *drop_statement,
		char *statement,
		char *unique_index_statement,
		LIST *additional_unique_index_list,
		LIST *additional_index_list )
{
	LIST *list = list_new();

	list_set( list, drop_statement );
	list_set( list, statement );
	list_set( list, unique_index_statement );

	list_set_list(
		list,
		additional_unique_index_list );

	list_set_list(
		list,
		additional_index_list );

	return list;
}

SHELL_SCRIPT *create_table_shell_script(
		char *application_name,
		boolean execute_boolean,
		char *create_table_shell_filename,
		LIST *create_table_sql_statement_list )
{
	if ( !application_name
	||   !create_table_shell_filename )
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
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	shell_script_new(
		application_name,
		create_table_shell_filename,
		create_table_sql_statement_list,
		execute_boolean /* tee_appaserver_boolean */,
		execute_boolean /* html_paragraph_wrapper_boolean */ );
}

LIST *create_table_additional_index_name_list(
		LIST *folder_attribute_list )
{
	LIST *list = list_new();
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( list_rewind( folder_attribute_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( folder_attribute->additional_index )
		{
			list_set(
				list,
				folder_attribute->attribute_name );
		}

	} while ( list_next( folder_attribute_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

LIST *create_table_additional_unique_index_name_list(
		LIST *folder_attribute_list )
{
	LIST *list = list_new();
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( list_rewind( folder_attribute_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( folder_attribute->additional_unique_index )
		{
			list_set(
				list,
				folder_attribute->attribute_name );
		}

	} while ( list_next( folder_attribute_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

char *create_table_drop_statement(
		char *create_view_statement,
		char *folder_table_name )
{
	static char statement[ 128 ];

	if ( !folder_table_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_table_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( create_view_statement )
	{
		snprintf(
			statement,
			sizeof ( statement ),
			"drop view if exists %s;",
			folder_table_name );
	}
	else
	{
		snprintf(
			statement,
			sizeof ( statement ),
			"drop table if exists %s;",
			folder_table_name );
	}

	return statement;
}

char *create_table_primary_index_system_string(
		LIST *primary_key_list,
		char *folder_table_name )
{
	static char system_string[ 512 ];

	if ( !list_length( primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "primary_key_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !folder_table_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_table_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"create unique index %s on %s (%s);\" |"
		"sql.e 2>&1",
		folder_table_name,
		folder_table_name,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_display_delimited(
			primary_key_list,
			',' ) );

	return system_string;
}

char *create_table_storage_engine(
		const char *create_table_myisam_engine,
		char *storage_engine )
{
	if ( storage_engine )
		return storage_engine;
	else
		return (char *)create_table_myisam_engine;
}
