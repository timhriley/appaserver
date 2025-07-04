/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/add_column.c	 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "folder_attribute.h"
#include "attribute.h"
#include "appaserver_error.h"
#include "shell_script.h"
#include "relation_mto1.h"
#include "add_column.h"

ADD_COLUMN *add_column_new(
		char *application_name,
		char *folder_name,
		char *attribute_name,
		char *data_directory )
{
	ADD_COLUMN *add_column;

	if ( !application_name
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

	add_column = add_column_calloc();

	add_column->folder_attribute =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_attribute_fetch(
			folder_name,
			attribute_name,
			1 /* fetch_attribute */ );

	add_column->folder_table_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_table_name(
			application_name,
			folder_name );

	add_column->attribute_database_datatype =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		attribute_database_datatype(
			add_column->folder_attribute->attribute->datatype_name,
			add_column->folder_attribute->attribute->width,
			add_column->
				folder_attribute->
				attribute->
				float_decimal_places,
			add_column->folder_attribute->primary_key_index,
			add_column->folder_attribute->default_value );

	add_column->execute_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		add_column_execute_system_string(
			attribute_name,
			add_column->folder_table_name,
			add_column->attribute_database_datatype );

	add_column->shell_script =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		add_column_shell_script(
			application_name,
			add_column->folder_attribute,
			add_column->folder_table_name,
			add_column->execute_system_string );

	add_column->process_filespecification =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		add_column_process_filespecification(
			application_name,
			folder_name,
			attribute_name,
			data_directory );

	return add_column;
}

ADD_COLUMN *add_column_calloc( void )
{
	ADD_COLUMN *add_column;

	if ( ! ( add_column = calloc( 1, sizeof ( ADD_COLUMN ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return add_column;
}

char *add_column_execute_system_string(
		char *attribute_name,
		char *folder_table_name,
		char *attribute_database_datatype )
{
	static char system_string[ 256 ];

	sprintf(system_string,
		"echo \"alter table %s add %s %s;\" | "
		"tee_appaserver.sh | "
		"sql.e 2>&1",
		folder_table_name,
		attribute_name,
		attribute_database_datatype );

	return system_string;
}

char *add_column_shell_script(
		char *application_name,
		FOLDER_ATTRIBUTE *folder_attribute,
		char *folder_table_name,
		char *add_column_execute_system_string )
{
	char shell_script[ 65536 ];
	char *ptr = shell_script;
	char *insert_statement;
	char *system_string;

	if ( !application_name
	||   !folder_attribute
	||   !folder_attribute->attribute
	||   !folder_table_name
	||   !add_column_execute_system_string )
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

	ptr += sprintf( ptr, "(\n" );

	ptr += sprintf(
		ptr,
		"%s\n",
		add_column_execute_system_string );

	insert_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		attribute_insert_statement(
			ATTRIBUTE_SELECT,
			ATTRIBUTE_TABLE,
			folder_attribute->attribute );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		add_column_shell_script_system_string(
			insert_statement );

	free( insert_statement );

	ptr += sprintf(
		ptr,
		"%s\n",
		system_string );

	free( system_string );

	insert_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		folder_attribute_insert_statement(
			FOLDER_ATTRIBUTE_SELECT,
			FOLDER_ATTRIBUTE_TABLE,
			folder_attribute );

	system_string =
		add_column_shell_script_system_string(
			insert_statement );

	free( insert_statement );

	ptr += sprintf(
		ptr,
		"%s\n",
		system_string );

	free( system_string );

	system_string =
		/* ----------------------------- */
		/* Returns static memory or null */
		/* ----------------------------- */
		add_column_relation_insert_system_string(
			RELATION_TABLE,
			RELATION_PRIMARY_KEY,
			folder_attribute->folder_name,
			folder_attribute->attribute_name );

	if ( system_string )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			system_string );
	}

	ptr += sprintf(
		ptr,
		") 2>&1 | grep -vi duplicate\n" );

	ptr += sprintf(
		ptr,
		"%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		shell_script_close_string() );

	return strdup( shell_script );
}

char *add_column_shell_script_system_string( char *insert_statement )
{
	char system_string[ 65536 ];

	if ( !insert_statement )
	{
		char message[ 128 ];

		sprintf(message, "insert_statement is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"echo \"%s\" | sql.e 2>&1",
		insert_statement );

	return strdup( system_string );
}

char *add_column_relation_insert_system_string(
		char *relation_table,
		char *relation_primary_key,
		char *folder_name,
		char *attribute_name )
{
	static char system_string[ 256 ];
	LIST *primary_key_list;
	LIST *mto1_list;
	RELATION_MTO1 *relation_mto1;

	primary_key_list =
		folder_attribute_fetch_primary_key_list(
			folder_name );

	if ( !list_length( primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message,
		"folder_attribute_fetch_primary_key_list(%s) returned empty.",
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	mto1_list =
		relation_mto1_list(
			folder_name /* many_folder_name */,
			primary_key_list );

	if ( !list_length( mto1_list ) ) return (char *)0;

	if ( ( relation_mto1 =
		relation_mto1_consumes(
			attribute_name /* many_attribute_name */,
			mto1_list /* relation_mto1_list */ ) ) )
	{
		sprintf(system_string,
			"echo \"%s\" | sql.e 2>&1",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			relation_insert_statement(
				relation_table,
				relation_primary_key,
				relation_mto1->many_folder_name,
				relation_mto1->one_folder_name,
				relation_mto1->
					relation->
					related_attribute_name ) );

		return system_string;
	}

	return (char *)0;
}

char *add_column_process_filespecification(
		char *application_name,
		char *folder_name,
		char *attribute_name,
		char *data_directory )
{
	static char process_filespecification[ 128 ];

	if ( !application_name
	||   !folder_name
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

	snprintf(
		process_filespecification,
		sizeof ( process_filespecification ),
		"%s/%s/add_column_%s_%s.sh",
		data_directory,
		application_name,
		folder_name,
		attribute_name );

	return process_filespecification;
}
