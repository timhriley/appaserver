/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/alter_datatype.c				*/
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
#include "relation.h"
#include "alter_datatype.h"

ALTER_DATATYPE *alter_datatype_new(
		char *application_name,
		char *folder_name,
		char *attribute_name,
		char *data_directory )
{
	ALTER_DATATYPE *alter_datatype;

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

	alter_datatype = alter_datatype_calloc();

	alter_datatype->folder_attribute =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_attribute_fetch(
			folder_name,
			attribute_name,
			1 /* fetch_attribute */ );

	alter_datatype->folder_table_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_table_name(
			application_name,
			folder_name );

	alter_datatype->attribute_database_datatype =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		attribute_database_datatype(
			alter_datatype->
				folder_attribute->
				attribute->
				datatype_name,
			alter_datatype->
				folder_attribute->
				attribute->
				width,
			alter_datatype->
				folder_attribute->
				attribute->
				float_decimal_places,
			alter_datatype->
				folder_attribute->
				primary_key_index,
			alter_datatype->
				folder_attribute->
				default_value );

	alter_datatype->execute_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		alter_datatype_execute_system_string(
			attribute_name,
			alter_datatype->folder_table_name,
			alter_datatype->attribute_database_datatype );

	alter_datatype->shell_script =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		alter_datatype_shell_script(
			application_name,
			alter_datatype->folder_attribute,
			alter_datatype->folder_table_name,
			alter_datatype->execute_system_string );

	alter_datatype->process_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		alter_datatype_process_filename(
			folder_name,
			attribute_name,
			data_directory );

	return alter_datatype;
}

ALTER_DATATYPE *alter_datatype_calloc( void )
{
	ALTER_DATATYPE *alter_datatype;

	if ( ! ( alter_datatype = calloc( 1, sizeof ( ALTER_DATATYPE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return alter_datatype;
}

char *alter_datatype_execute_system_string(
		char *attribute_name,
		char *folder_table_name,
		char *attribute_database_datatype )
{
	static char system_string[ 256 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"alter table %s modify %s %s;\" |"
		"tee_appaserver.sh |"
		"sql.e 2>&1",
		folder_table_name,
		attribute_name,
		attribute_database_datatype );

	return system_string;
}

char *alter_datatype_shell_script(
		char *application_name,
		FOLDER_ATTRIBUTE *folder_attribute,
		char *folder_table_name,
		char *alter_datatype_execute_system_string )
{
	char shell_script[ 65536 ];
	char *ptr = shell_script;
	char *statement;
	char *system_string;

	if ( !application_name
	||   !folder_attribute
	||   !folder_attribute->attribute
	||   !folder_table_name
	||   !alter_datatype_execute_system_string )
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
		alter_datatype_execute_system_string );

	statement =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		attribute_delete_statement(
			ATTRIBUTE_TABLE,
			folder_attribute->attribute_name );

	system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		alter_datatype_shell_script_system_string(
			statement );

	ptr += sprintf(
		ptr,
		"%s\n",
		system_string );

	statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		attribute_insert_statement(
			ATTRIBUTE_SELECT,
			ATTRIBUTE_TABLE,
			folder_attribute->attribute );

	system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		alter_datatype_shell_script_system_string(
			statement );

	free( statement );

	ptr += sprintf(
		ptr,
		"%s\n",
		system_string );

	ptr += sprintf(
		ptr,
		"%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		shell_script_close_string() );

	return strdup( shell_script );
}

char *alter_datatype_process_filename(
		char *folder_name,
		char *attribute_name,
		char *data_directory )
{
	static char process_filename[ 128 ];

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

	snprintf(
		process_filename,
		sizeof ( process_filename ),
		"%s/alter_datatype_%s_%s.sh",
		data_directory,
		folder_name,
		attribute_name );

	return process_filename;
}

char *alter_datatype_shell_script_system_string( char *statement )
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

