/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/insert.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "timlib.h"
#include "folder.h"
#include "relation_mto1.h"
#include "appaserver.h"
#include "application.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "process.h"
#include "widget.h"
#include "sql.h"
#include "role.h"
#include "piece.h"
#include "insert.h"

INSERT *insert_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *folder_attribute_primary_key_list,
		LIST *folder_attribute_append_isa_list,
		LIST *relation_mto1_isa_list,
		DICTIONARY *prompt_dictionary,
		DICTIONARY *multi_row_dictionary,
		LIST *ignore_name_list,
		PROCESS *post_change_process )
{
	INSERT *insert;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !list_length( folder_attribute_primary_key_list )
	||   !list_length( folder_attribute_append_isa_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !dictionary_length( prompt_dictionary )
	&&   !dictionary_length( multi_row_dictionary ) )
	{
		return NULL;
	}

	insert = insert_calloc();

	insert->appaserver_error_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_error_filename(
			application_name );

	if ( !dictionary_length( multi_row_dictionary ) )
	{
		insert->insert_zero =
			insert_zero_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				folder_attribute_primary_key_list,
				folder_attribute_append_isa_list,
				relation_mto1_isa_list,
				prompt_dictionary,
				ignore_name_list,
				post_change_process,
				insert->appaserver_error_filename );
	}
	else
	{
		insert->insert_multi =
			insert_multi_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				folder_attribute_primary_key_list,
				folder_attribute_append_isa_list,
				relation_mto1_isa_list,
				prompt_dictionary,
				multi_row_dictionary /* in/out */,
				ignore_name_list,
				post_change_process,
				insert->appaserver_error_filename );
	}

	if ( !insert->insert_zero && !insert->insert_multi )
	{
		free( insert );
		return NULL;
	}

	insert->insert_statement =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		insert_statement_new(
			insert->insert_zero,
			insert->insert_multi );

	insert->insert_folder_statement_sql_list_length =
		insert_folder_statement_sql_list_length(
			insert->
				insert_statement->
				insert_folder_statement_list );

	insert->results_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		insert_results_string(
			folder_name,
			relation_mto1_isa_list,
			insert->insert_folder_statement_sql_list_length );

	return insert;
}

LIST *insert_statement_extract_sql_list(
		LIST *insert_folder_statement_list )
{
	INSERT_FOLDER_STATEMENT *insert_folder_statement;
	LIST *sql_list;

	if ( !list_rewind( insert_folder_statement_list ) )
	{
		char message[ 128 ];

		sprintf(message, "insert_folder_statement_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sql_list = list_new();

	do {
		insert_folder_statement =
			list_get(
				insert_folder_statement_list );

		if ( !list_length(
			insert_folder_statement->
				sql_statement_list ) )
		{
			char message[ 128 ];

			sprintf(message,
		"insert_folder_statement->sql_statement_list is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set_list(
			sql_list,
			insert_folder_statement->sql_statement_list );

	} while ( list_next( insert_folder_statement_list ) );

	return sql_list;
}

LIST *insert_statement_extract_command_list(
		LIST *insert_folder_statement_list )
{
	INSERT_FOLDER_STATEMENT *insert_folder_statement;
	LIST *command_list;

	if ( !list_rewind( insert_folder_statement_list ) ) return NULL;

	command_list = list_new();

	do {
		insert_folder_statement =
			list_get(
				insert_folder_statement_list );

		list_set_list(
			command_list,
			insert_folder_statement->command_line_list );

	} while ( list_next( insert_folder_statement_list ) );

	if ( !list_length( command_list ) )
	{
		list_free( command_list );
		command_list = NULL;
	}

	return command_list;
}

INSERT_ROW *insert_row_calloc( void )
{
	INSERT_ROW *insert_row;

	if ( ! ( insert_row = calloc( 1, sizeof ( INSERT_ROW ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return insert_row;
}

INSERT_ROW *insert_row_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *folder_attribute_primary_key_list,
		LIST *folder_attribute_append_isa_list,
		LIST *relation_mto1_isa_list,
		DICTIONARY *prompt_dictionary,
		DICTIONARY *multi_row_dictionary,
		LIST *ignore_name_list,
		PROCESS *post_change_process,
		char *appaserver_error_filename,
		int row_number )
{
	INSERT_ROW *insert_row;
	RELATION_MTO1 *relation_mto1;
	LIST *name_list;
	char *mount_point;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !list_length( folder_attribute_primary_key_list )
	||   !list_length( folder_attribute_append_isa_list )
	||   !appaserver_error_filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !dictionary_length( prompt_dictionary )
	&&   !dictionary_length( multi_row_dictionary ) )
	{
		return NULL;
	}

	insert_row = insert_row_calloc();

	name_list =
		folder_attribute_name_list(
			folder_name,
			folder_attribute_append_isa_list );

	if ( ! ( insert_row->insert_folder =
			insert_folder_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				folder_attribute_primary_key_list
				   /* root_folder_attribute_primary_key_list */,
				(LIST *)0
				   /* folder_attribute_primary_key_list */,
				folder_attribute_append_isa_list,
				prompt_dictionary,
				multi_row_dictionary,
				ignore_name_list,
				post_change_process,
				appaserver_error_filename,
				row_number,
				name_list ) ) )
	{
		free( insert_row );
		return NULL;
	}

	if ( !list_rewind( relation_mto1_isa_list ) ) return insert_row;

	insert_row->insert_folder_isa_list = list_new();

	mount_point = appaserver_parameter_mount_point();

	do {
		relation_mto1 = list_get( relation_mto1_isa_list );

		if ( !relation_mto1->one_folder_name
		||   !relation_mto1->one_folder
		||   !list_length(
			relation_mto1->
				one_folder->
				folder_attribute_primary_key_list ) )
		{
			char message[ 128 ];

			sprintf(message, "relation_mto1 is incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		name_list =
			folder_attribute_name_list(
				relation_mto1->one_folder_name,
				folder_attribute_append_isa_list );

		if ( relation_mto1->
			one_folder->
			post_change_process_name )
		{
			post_change_process =
				process_fetch(
					relation_mto1->
						one_folder->
						post_change_process_name,
					(char *)0 /* document_root */,
					(char *)0 /* relative_source */,
					1 /* check_executable */,
					mount_point );
		}
		else
		{
			post_change_process = NULL;
		}

		list_set(
			insert_row->insert_folder_isa_list,
			insert_folder_new(
				application_name,
				session_key,
				login_name,
				role_name,
				relation_mto1->one_folder_name,
				folder_attribute_primary_key_list
				   /* root_folder_attribute_primary_key_list */,
				relation_mto1->
					one_folder->
					folder_attribute_primary_key_list,
				folder_attribute_append_isa_list,
				prompt_dictionary,
				multi_row_dictionary,
				ignore_name_list,
				post_change_process,
				appaserver_error_filename,
				row_number,
				name_list ) );

	} while ( list_next( relation_mto1_isa_list ) );

	return insert_row;
}

INSERT *insert_calloc( void )
{
	INSERT *insert;

	if ( ! ( insert = calloc( 1, sizeof ( INSERT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return insert;
}

INSERT_DATUM *insert_datum_calloc( void )
{
	INSERT_DATUM *insert_datum;

	if ( ! ( insert_datum = calloc( 1, sizeof ( INSERT_DATUM ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return insert_datum;
}

INSERT_DATUM *insert_datum_new(
		char *attribute_name,
		char *datum,
		int primary_key_index,
		boolean attribute_is_number )
{
	INSERT_DATUM *insert_datum = insert_datum_calloc();

	insert_datum->attribute_name = attribute_name;
	insert_datum->datum = datum;
	insert_datum->primary_key_index = primary_key_index;
	insert_datum->attribute_is_number = attribute_is_number;

	return insert_datum;
}

INSERT_DATUM *insert_datum_extract(
		DICTIONARY *prompt_dictionary,
		DICTIONARY *multi_row_dictionary,
		int row_number,
		char *attribute_name,
		int primary_key_index,
		boolean attribute_is_number )
{
	char *datum;

	if ( ( datum =
		dictionary_get(
			attribute_name,
			prompt_dictionary ) ) )
	{
		return
		insert_datum_new(
			attribute_name,
			datum,
			primary_key_index,
			attribute_is_number );
	}

	if ( multi_row_dictionary
	&& ( datum = dictionary_get(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_key(
				attribute_name /* widget_name */,
				row_number ),
			multi_row_dictionary ) ) )
	{
		return
		insert_datum_new(
			attribute_name,
			datum,
			primary_key_index,
			attribute_is_number );
	}

	return NULL;
}

LIST *insert_datum_key_datum_list(
		LIST *insert_datum_list )
{
	INSERT_DATUM *insert_datum;
	LIST *key_datum_list;

	if ( !list_rewind( insert_datum_list ) )
	{
		char message[ 128 ];

		sprintf(message, "insert_datum_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	key_datum_list = list_new();

	do {
		insert_datum = list_get( insert_datum_list );

		if ( !insert_datum->primary_key_index )
		{
			return key_datum_list;
		}

		list_set( key_datum_list, insert_datum->datum );

	} while ( list_next( insert_datum_list ) );

	return key_datum_list;
}

char *insert_statement_sql_execute(
		char *appaserver_error_filename,
		LIST *insert_statement_extract_sql_list )
{
	char system_string[ 128 ];
	FILE *output_pipe;
	char *temp_filename;
	char *error_string;

	if ( !appaserver_error_filename )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_error_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( insert_statement_extract_sql_list ) )
	{
		return (char *)0;
	}

	sprintf(system_string,
		"tee -a %s | sql.e 2>%s",
		appaserver_error_filename,
		( temp_filename =
			timlib_temp_filename( "insert" /* key */ ) ) );

	output_pipe = appaserver_output_pipe( system_string );

	do {
		fprintf(output_pipe,
			"%s\n",
			(char *)list_get(
				insert_statement_extract_sql_list ) );

	} while ( list_next( insert_statement_extract_sql_list ) );

	pclose( output_pipe );

	error_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_file_fetch(
			temp_filename,
			"\n<br>" /* delimiter */ );

	timlib_remove_file( temp_filename );

	return
	string_search_replace(
		error_string,
		"ERROR 1062 (23000) ",
		"" );
}

void insert_statement_command_execute(
		LIST *insert_statement_extract_command_list )
{
	LIST *list = insert_statement_extract_command_list;

	if ( !list_rewind( list ) ) return;

	do {
		if ( system( (char *)list_get( list ) ) ){}

	} while ( list_next( list ) );
}

INSERT_FOLDER_STATEMENT *insert_folder_statement_new(
		INSERT_FOLDER *insert_folder,
		LIST *insert_folder_isa_list )
{
	INSERT_FOLDER_STATEMENT *insert_folder_statement;

	if ( !insert_folder
	||   !insert_folder->insert_datum_sql_statement )
	{
		char message[ 128 ];

		sprintf(message, "insert_folder is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	insert_folder_statement = insert_folder_statement_calloc();

	insert_folder_statement->sql_statement_list = list_new();

	list_set(
		insert_folder_statement->sql_statement_list,
		insert_folder->insert_datum_sql_statement );

	if ( insert_folder->command_line )
	{
		insert_folder_statement->command_line_list = list_new();

		list_set(
			insert_folder_statement->command_line_list,
			insert_folder->command_line );
	}

	if ( list_rewind( insert_folder_isa_list ) )
	{
		do {
			insert_folder =
				list_get(
					insert_folder_isa_list );
	
			if ( !insert_folder->insert_datum_sql_statement )
			{
				char message[ 128 ];
	
				sprintf(message,
			"insert_folder->insert_datum_sql_statement is empty." );
	
				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}
	
			list_set(
				insert_folder_statement->sql_statement_list,
				insert_folder->insert_datum_sql_statement );
	
			if ( insert_folder->command_line )
			{
				if ( !insert_folder_statement->
					command_line_list )
				{
					insert_folder_statement->
						command_line_list =
							list_new();
				}
	
				list_set(
					insert_folder_statement->
						command_line_list,
					insert_folder->command_line );
			}
	
		} while ( list_next( insert_folder_isa_list ) );
	}

	return insert_folder_statement;
}

INSERT_FOLDER_STATEMENT *insert_folder_statement_calloc( void )
{
	INSERT_FOLDER_STATEMENT *insert_folder_statement;

	if ( ! ( insert_folder_statement =
			calloc(	1,
				sizeof ( INSERT_FOLDER_STATEMENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return insert_folder_statement;
}

INSERT_STATEMENT *insert_statement_calloc( void )
{
	INSERT_STATEMENT *insert_statement;

	if ( ! ( insert_statement =
			calloc(	1,
				sizeof ( INSERT_STATEMENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return insert_statement;
}

char *insert_statement_execute(
		char *application_name,
		LIST *insert_folder_statement_list,
		char *appaserver_error_filename )
{
	char *error_string;
	LIST *extract_sql_list;
	LIST *extract_command_list;

	if ( !appaserver_error_filename )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_error_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( insert_folder_statement_list ) ) return NULL;

	extract_sql_list =
		insert_statement_extract_sql_list(
			insert_folder_statement_list );

	if ( !list_length( extract_sql_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"insert_statement_extract_sql_list() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	error_string =
		/* --------------------------------------------- */
		/* Returns insert_statement_error_string or null */
		/* --------------------------------------------- */
		insert_statement_sql_execute(
			appaserver_error_filename,
			extract_sql_list );

	if ( error_string )
	{
		appaserver_error_message_file(
			application_name,
			(char *)0 /* login_name */,
			error_string /* message */ );

		return error_string;
	}

	extract_command_list =
		insert_statement_extract_command_list(
			insert_folder_statement_list );

	if ( list_length( extract_command_list ) )
	{
		insert_statement_command_execute(
			extract_command_list );
	}

	return (char *)0;
}

INSERT_FOLDER *insert_folder_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *root_folder_attribute_primary_key_list,
		LIST *folder_attribute_primary_key_list,
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *prompt_dictionary,
		DICTIONARY *multi_row_dictionary,
		LIST *ignore_name_list,
		PROCESS *post_change_process,
		char *appaserver_error_filename,
		int row_number,
		LIST *folder_attribute_name_list )
{
	INSERT_FOLDER *insert_folder;
	char *attribute_name;
	FOLDER_ATTRIBUTE *folder_attribute;
	boolean is_number;
	INSERT_DATUM *datum_extract;
	char *table_name;
	char *primary_data_list_string;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !list_rewind( root_folder_attribute_primary_key_list )
	||   !appaserver_error_filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( folder_attribute_name_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"folder_name=%s has empty folder_attribute_name_list.",
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !dictionary_length( prompt_dictionary )
	&&   !dictionary_length( multi_row_dictionary ) )
	{
		return NULL;
	}

	insert_folder = insert_folder_calloc();

	insert_folder->application_table_name =
		strdup(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			application_table_name(
				folder_name ) );

	insert_folder->insert_datum_list = list_new();

	if ( folder_attribute_primary_key_list )
	{
		list_rewind( folder_attribute_primary_key_list );
	}

	do {
		attribute_name =
			list_get(
				root_folder_attribute_primary_key_list );

		if ( ! ( folder_attribute =
				folder_attribute_seek(
					attribute_name,
					folder_attribute_append_isa_list ) ) )
		{
			char message[ 128 ];

			sprintf(message,
				"folder_attribute_seek(%s) returned empty.",
				attribute_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !folder_attribute->attribute )
		{
			char message[ 128 ];

			sprintf(message,
				"folder_attribute->attribute is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		is_number =
			attribute_is_number(
				folder_attribute->
					attribute->
					datatype_name );

		if ( ! ( datum_extract =
				insert_datum_extract(
					prompt_dictionary,
					multi_row_dictionary,
					row_number,
					attribute_name,
					folder_attribute->primary_key_index,
					is_number ) ) )
		{
			list_free( insert_folder->insert_datum_list );
			free( insert_folder );
			return NULL;
		}

		if ( folder_attribute_primary_key_list )
		{
			datum_extract->attribute_name =
				list_get( folder_attribute_primary_key_list );

			list_next( folder_attribute_primary_key_list );
		}

		list_set(
			insert_folder->insert_datum_list,
			datum_extract );

	} while ( list_next( root_folder_attribute_primary_key_list ) );

	list_rewind( folder_attribute_name_list );

	do {
		attribute_name =
			list_get(
				folder_attribute_name_list );

		if ( list_string_exists(
			attribute_name,
			ignore_name_list ) )
		{
			continue;
		}

		if ( ! ( folder_attribute =
				folder_attribute_seek(
					attribute_name,
					folder_attribute_append_isa_list ) ) )
		{
			char message[ 128 ];

			sprintf(message,
				"folder_attribute_seek(%s) returned empty.",
				attribute_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !folder_attribute->attribute )
		{
			char message[ 128 ];

			sprintf(message,
				"folder_attribute->attribute is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( folder_attribute->primary_key_index ) continue;

		is_number =
			attribute_is_number(
				folder_attribute->
					attribute->
					datatype_name );

		if ( ( datum_extract =
				insert_datum_extract(
					prompt_dictionary,
					multi_row_dictionary,
					row_number,
					attribute_name,
					0 /* primary_key_index */,
					is_number ) ) )
		{
			list_set(
				insert_folder->insert_datum_list,
				datum_extract );
		}

	} while ( list_next( folder_attribute_name_list ) );

	table_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_table_name(
			application_name,
			folder_name );

	insert_folder->insert_datum_sql_statement =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		insert_datum_sql_statement(
			table_name,
			insert_folder->insert_datum_list );

	if ( post_change_process )
	{
		if ( !post_change_process->command_line )
		{
			char message[ 128 ];

			sprintf(message,
				"post_change_process->command_line is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		insert_folder->insert_datum_key_datum_list =
			insert_datum_key_datum_list(
				insert_folder->insert_datum_list );

		if ( !list_length(
			insert_folder->
				insert_datum_key_datum_list ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: insert_datum_key_datum_list() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		primary_data_list_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			insert_folder_primary_data_list_string(
				insert_folder->insert_datum_key_datum_list,
				SQL_DELIMITER );

		insert_folder->command_line =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			insert_folder_command_line(
				post_change_process->command_line,
				session_key,
				login_name,
				role_name,
				folder_name,
				post_change_process->process_name,
				appaserver_error_filename,
				APPASERVER_INSERT_STATE,
				insert_folder->insert_datum_list,
				primary_data_list_string );

		free( primary_data_list_string );
	}

	return insert_folder;
}

INSERT_FOLDER *insert_folder_calloc( void )
{
	INSERT_FOLDER *insert_folder;

	if ( ! ( insert_folder = calloc( 1, sizeof ( INSERT_FOLDER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return insert_folder;
}

char *insert_folder_sql_statement_string(
		char *folder_table_name,
		char *attribute_name_list_string,
		char *value_list_string )
{
	char sql_statement_string[ STRING_64K ];

	if ( !folder_table_name
	||   !attribute_name_list_string
	||   !value_list_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if (	strlen( folder_table_name ) +
		strlen( attribute_name_list_string ) +
		strlen( value_list_string ) + 26 >= STRING_64K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_64K );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		sql_statement_string,
		sizeof ( sql_statement_string ),
		"insert into %s (%s) values (%s);",
		folder_table_name,
		attribute_name_list_string,
		value_list_string );

	return strdup( sql_statement_string );
}

char *insert_folder_primary_data_list_string(
		LIST *insert_datum_key_datum_list,
		char sql_delimiter )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	list_display_delimited(
		insert_datum_key_datum_list,
		sql_delimiter );
}

char *insert_folder_command_line(
		char *post_change_process_command_line,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *post_change_process_name,
		char *appaserver_error_filename,
		char *appaserver_insert_state,
		LIST *insert_datum_list,
		char *insert_folder_primary_data_list_string )
{
	char command_line[ STRING_8K ];
	char destination[ STRING_8K ];
	INSERT_DATUM *insert_datum;

	if ( !post_change_process_command_line
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !post_change_process_name
	||   !appaserver_error_filename
	||   !appaserver_insert_state
	||   !list_length( insert_datum_list )
	||   !insert_folder_primary_data_list_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	string_strcpy(
		command_line,
		post_change_process_command_line,
		STRING_8K );

	string_replace_command_line(
		command_line,
		session_key,
		PROCESS_SESSION_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		login_name,
		PROCESS_LOGIN_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		role_name,
		PROCESS_ROLE_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		folder_name,
		PROCESS_FOLDER_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		folder_name,
		PROCESS_TABLE_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		post_change_process_name,
		PROCESS_NAME_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		appaserver_insert_state,
		PROCESS_STATE_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		insert_folder_primary_data_list_string,
		PROCESS_PRIMARY_PLACEHOLDER );

	list_rewind( insert_datum_list );

	do {
		insert_datum = list_get( insert_datum_list );

		string_replace_command_line(
			command_line,
			insert_datum->datum,
			insert_datum->attribute_name );

	} while ( list_next( insert_datum_list ) );

	sprintf(command_line + strlen( command_line ),
		" 2>>%s",
		appaserver_error_filename );

	return
	strdup(
		string_escape_dollar(
			destination,
			command_line ) );
}

char *insert_datum_value_string(
		char *datum,
		boolean attribute_is_number )
{
	char value_string[ STRING_64K ];
	char destination[ STRING_64K ];

	if ( string_strlen( datum ) + 2 >= STRING_64K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_64K );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !datum || !*datum )
		strcpy( value_string, "null" );
	else
	if ( attribute_is_number )
		sprintf( value_string, "%s", datum );
	else
	{
		snprintf(
			value_string,
			sizeof ( value_string ),
			"'%s'",
			string_escape_quote(
				destination,
				datum ) );
	}

	return strdup( value_string );
}

char *insert_folder_string_sql_statement(
		const char sql_delimiter,
		char *table_name,
		char *attribute_name_list_string,
		char *delimited_string )
{
	char *value_list_string;
	char *sql_statement_string;

	if ( !table_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"table_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !attribute_name_list_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"attribute_name_list_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !delimited_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"For %s^%s: delimited_string is empty.",
			table_name,
			attribute_name_list_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	value_list_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		insert_folder_value_list_string(
			sql_delimiter,
			delimited_string );

	sql_statement_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		insert_folder_sql_statement_string(
			table_name,
			attribute_name_list_string,
			value_list_string );

	free( value_list_string );

	return sql_statement_string;
}

char *insert_folder_value_list_string(
		const char sql_delimiter,
		char *delimited_string )
{
	char value_list_string[ STRING_128K ];
	char *ptr = value_list_string;
	char datum[ STRING_64K ];
	char *value_string;
	int p;

	if ( !delimited_string )
	{
		char message[ 128 ];

		sprintf(message, "delimited_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	for (	p = 0;
		piece(	datum,
			sql_delimiter,
			delimited_string,
			p );
		p++ )
	{
		value_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			insert_datum_value_string(
				datum,
				0 /* not attribute_is_number */ );

		if (	strlen( value_list_string ) +
			strlen( value_string ) + 1 >= STRING_128K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_128K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( ptr != value_list_string ) ptr += sprintf( ptr, "," );

		ptr += sprintf(
			ptr,
			"%s",
			value_string );

		free( value_string );
	}

	return strdup( value_list_string );
}

char *insert_results_string(
		char *folder_name,
		LIST *relation_mto1_isa_list,
		int insert_folder_statement_sql_list_length )
{
	static char results_string[ 256 ];
	char buffer[ 256 ];
	char *label;
	LIST *mto1_folder_name_list = {0};
	char *display;

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( insert_folder_statement_sql_list_length == 1 )
		label = "Row";
	else
		label = "Rows";

	if ( list_length( relation_mto1_isa_list ) )
	{
		mto1_folder_name_list =
			relation_mto1_folder_name_list(
				relation_mto1_isa_list );
	}

	display =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		insert_results_folder_display(
			folder_name,
			mto1_folder_name_list );

	sprintf(results_string,
		"Inserted %d %s into %s",
		insert_folder_statement_sql_list_length,
		label,
		string_initial_capital(
			buffer,
			display ) );

	return results_string;
}

char *insert_login_name(
		char *login_name,
		LIST *role_attribute_exclude_name_list,
		LIST *folder_attribute_append_isa_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( list_string_exists(
		ROLE_LOGIN_NAME_COLUMN,
		role_attribute_exclude_name_list ) )
	{
		return login_name;
	}

	folder_attribute =
		folder_attribute_seek(
			ROLE_LOGIN_NAME_COLUMN,
			folder_attribute_append_isa_list );

	if (	folder_attribute
	&& (	folder_attribute->omit_insert
	||	folder_attribute->omit_insert_prompt ) )
	{
		return login_name;
	}

	return (char *)0;
}

char *insert_results_folder_display(
		char *folder_name,
		LIST *relation_mto1_folder_name_list )
{
	static char display[ 256 ];
	char *ptr = display;

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf( ptr, "%s", folder_name );

	if ( list_rewind( relation_mto1_folder_name_list ) )
	do {
		folder_name =
			list_get(
				relation_mto1_folder_name_list );

		ptr += sprintf(
			ptr,
			", %s",
			folder_name );

	} while ( list_next( relation_mto1_folder_name_list ) );

	return display;
}

LIST *insert_statement_multi_list( LIST *insert_row_list )
{
	LIST *insert_folder_statement_list;
	INSERT_ROW *insert_row;

	insert_folder_statement_list = list_new();

	if ( list_rewind( insert_row_list ) )
	do {
		insert_row = list_get( insert_row_list );

		list_set(
			insert_folder_statement_list,
			insert_folder_statement_new(
				insert_row->insert_folder,
				insert_row->insert_folder_isa_list ) );

	} while ( list_next( insert_row_list ) );

	if ( !list_length( insert_folder_statement_list ) )
	{
		free( insert_folder_statement_list );
		insert_folder_statement_list = NULL;
	}

	return insert_folder_statement_list;
}

int insert_folder_statement_sql_list_length(
		LIST *insert_folder_statement_list )
{
	int length = 0;
	INSERT_FOLDER_STATEMENT *insert_folder_statement;

	if ( list_rewind( insert_folder_statement_list ) )
	do {
		insert_folder_statement =
			list_get(
				insert_folder_statement_list );

		length +=
			list_length(
				insert_folder_statement->
					sql_statement_list );

	} while ( list_next( insert_folder_statement_list ) );

	return length;
}

INSERT_ZERO *insert_zero_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *folder_attribute_primary_key_list,
		LIST *folder_attribute_append_isa_list,
		LIST *relation_mto1_isa_list,
		DICTIONARY *prompt_dictionary,
		LIST *ignore_name_list,
		PROCESS *post_change_process,
		char *appaserver_error_filename )
{
	INSERT_ZERO *insert_zero;
	LIST *name_list;
	RELATION_MTO1 *relation_mto1;
	char *mount_point;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !list_length( folder_attribute_primary_key_list )
	||   !list_length( folder_attribute_append_isa_list )
	||   !dictionary_length( prompt_dictionary )
	||   !appaserver_error_filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	insert_zero = insert_zero_calloc();

	insert_zero_attribute_default_set(
		folder_attribute_append_isa_list,
		prompt_dictionary /* in/out */,
		ignore_name_list );

	name_list =
		folder_attribute_name_list(
			folder_name,
			folder_attribute_append_isa_list );

	if ( ! ( insert_zero->insert_folder =
			insert_folder_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				folder_attribute_primary_key_list
				   /* root_folder_attribute_primary_key_list */,
				(LIST *)0
				   /* folder_attribute_primary_key_list */,
				folder_attribute_append_isa_list,
				prompt_dictionary,
				(DICTIONARY *)0 /* multi_row_dictionary */,
				ignore_name_list,
				post_change_process,
				appaserver_error_filename,
				0 /* row_number */,
				name_list ) ) )
	{
		free( insert_zero );
		return NULL;
	}

	if ( !list_rewind( relation_mto1_isa_list ) ) return insert_zero;

	mount_point = appaserver_parameter_mount_point();

	insert_zero->insert_folder_isa_list = list_new();

	do {
		relation_mto1 = list_get( relation_mto1_isa_list );

		if ( !relation_mto1->one_folder_name
		||   !relation_mto1->one_folder
		||   !list_length(
			relation_mto1->
				one_folder->
				folder_attribute_primary_key_list ) )
		{
			char message[ 128 ];

			sprintf(message, "relation_mto1 is incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		name_list =
			folder_attribute_name_list(
				relation_mto1->one_folder_name,
				folder_attribute_append_isa_list );

		if ( relation_mto1->
			one_folder->
			post_change_process_name )
		{
			post_change_process =
				process_fetch(
					relation_mto1->
						one_folder->
						post_change_process_name,
					(char *)0 /* document_root */,
					(char *)0 /* relative_source */,
					1 /* check_executable */,
					mount_point );
		}
		else
		{
			post_change_process = NULL;
		}

		list_set(
			insert_zero->insert_folder_isa_list,
			insert_folder_new(
				application_name,
				session_key,
				login_name,
				role_name,
				relation_mto1->one_folder_name,
				folder_attribute_primary_key_list
				   /* root_folder_attribute_primary_key_list */,
				relation_mto1->
					one_folder->
					folder_attribute_primary_key_list,
				folder_attribute_append_isa_list,
				prompt_dictionary,
				(DICTIONARY *)0 /* multi_row_dictionary */,
				ignore_name_list,
				post_change_process,
				appaserver_error_filename,
				0 /* row_number */,
				name_list ) );

	} while ( list_next( relation_mto1_isa_list ) );

	return insert_zero;
}

INSERT_ZERO *insert_zero_calloc( void )
{
	INSERT_ZERO *insert_zero;

	if ( ! ( insert_zero = calloc( 1, sizeof ( INSERT_ZERO ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return insert_zero;
}

INSERT_MULTI *insert_multi_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *folder_attribute_primary_key_list,
		LIST *folder_attribute_append_isa_list,
		LIST *relation_mto1_isa_list,
		DICTIONARY *prompt_dictionary,
		DICTIONARY *multi_row_dictionary,
		LIST *ignore_name_list,
		PROCESS *post_change_process,
		char *appaserver_error_filename )
{
	INSERT_MULTI *insert_multi;
	int row_number;
	INSERT_ROW *insert_row;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !list_length( folder_attribute_primary_key_list )
	||   !list_length( folder_attribute_append_isa_list )
	||   !dictionary_length( multi_row_dictionary )
	||   !appaserver_error_filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	insert_multi = insert_multi_calloc();

	insert_multi->dictionary_highest_index =
		dictionary_highest_index(
			multi_row_dictionary );

	if ( insert_multi->dictionary_highest_index < 1 )
	{
		free( insert_multi );
		return NULL;
	}

	for (	row_number = 1;
		row_number <= insert_multi->dictionary_highest_index;
		row_number++ )
	{
		insert_multi_attribute_default_set(
			folder_attribute_append_isa_list,
			multi_row_dictionary /* in/out */,
			row_number );

		if ( insert_multi_all_primary_null_boolean(
			APPASERVER_NULL_STRING,
			folder_attribute_primary_key_list,
			multi_row_dictionary,
			row_number ) )
		{
			continue;
		}

		insert_row =
			insert_row_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				folder_attribute_primary_key_list,
				folder_attribute_append_isa_list,
				relation_mto1_isa_list,
				prompt_dictionary,
				multi_row_dictionary,
				ignore_name_list,
				post_change_process,
				appaserver_error_filename,
				row_number );

		if ( insert_row )
		{
			if ( !insert_multi->insert_row_list )
			{
				insert_multi->insert_row_list = list_new();
			}
	
			list_set( insert_multi->insert_row_list, insert_row );
		}
	}

	if ( !list_length( insert_multi->insert_row_list ) )
	{
		free( insert_multi );
		insert_multi = NULL;
	}

	return insert_multi;
}

INSERT_MULTI *insert_multi_calloc( void )
{
	INSERT_MULTI *insert_multi;

	if ( ! ( insert_multi = calloc( 1, sizeof ( INSERT_MULTI ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return insert_multi;
}

char *insert_datum_attribute_name_list_string( LIST *insert_datum_list )
{
	char string[ 1024 ];
	char *ptr = string;
	INSERT_DATUM *insert_datum;

	if ( !list_rewind( insert_datum_list ) ) return (char *)0;

	do {
		insert_datum = list_get( insert_datum_list );

		if ( ptr != string ) ptr += sprintf( ptr, "," );

		ptr += sprintf(
			ptr,
			"%s",
			insert_datum->attribute_name );

	} while ( list_next( insert_datum_list ) );

	return strdup( string );
}

char *insert_datum_value_list_string( LIST *insert_datum_list )
{
	char string[ STRING_128K ];
	char *ptr = string;
	INSERT_DATUM *insert_datum;
	char *datum_value_string;

	if ( !list_rewind( insert_datum_list ) ) return (char *)0;

	do {
		insert_datum = list_get( insert_datum_list );

		datum_value_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			insert_datum_value_string(
				insert_datum->datum,
				insert_datum->attribute_is_number );

		if (	strlen( string ) +
			strlen( datum_value_string ) + 1 >= STRING_128K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_128K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( ptr != string ) ptr += sprintf( ptr, "," );

		ptr += sprintf(
			ptr,
			"%s",
			datum_value_string );

		free( datum_value_string );

	} while ( list_next( insert_datum_list ) );

	return strdup( string );
}

char *insert_datum_sql_statement(
		char *folder_table_name,
		LIST *insert_datum_list )
{
	char *attribute_name_list_string;
	char *value_list_string;
	char *sql_statement_string;

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

	attribute_name_list_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		insert_datum_attribute_name_list_string(
			insert_datum_list );

	if ( !attribute_name_list_string ) return (char *)0;

	value_list_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		insert_datum_value_list_string(
			insert_datum_list );

	sql_statement_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		insert_folder_sql_statement_string(
			folder_table_name,
			attribute_name_list_string,
			value_list_string );

	free( attribute_name_list_string );
	free( value_list_string );

	return sql_statement_string;
}

INSERT_STATEMENT *insert_statement_new(
		INSERT_ZERO *insert_zero,
		INSERT_MULTI *insert_multi )
{
	INSERT_STATEMENT *insert_statement;

	if ( !insert_zero && !insert_multi )
	{
		char message[ 128 ];

		sprintf(message,
			"both insert_zero and insert_multi are empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	insert_statement = insert_statement_calloc();

	if ( insert_zero )
	{
		insert_statement->insert_folder_statement_list = list_new();
	
		list_set(
			insert_statement->insert_folder_statement_list,
			insert_folder_statement_new(
				insert_zero->insert_folder,
				insert_zero->insert_folder_isa_list ) );
	}
	else
	{
		insert_statement->insert_folder_statement_list =
			insert_statement_multi_list(
				insert_multi->insert_row_list );
	}

	if ( !list_length( insert_statement->insert_folder_statement_list ) )
	{
		char message[ 128 ];

		sprintf(message,
		"insert_statement->insert_folder_statement_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return insert_statement;
}

DICTIONARY *insert_automatic_multi_row_dictionary(
			LIST *folder_attribute_non_primary_key_list,
			DICTIONARY *multi_row_dictionary )
{
	DICTIONARY *dictionary;
	LIST *key_list;
	int highest_index;
	int index;
	LIST *name_list;

	if ( !list_length( folder_attribute_non_primary_key_list )
	||   !dictionary_length( multi_row_dictionary ) )
	{
		return NULL;
	}

	dictionary = dictionary_medium();

	key_list =
		dictionary_key_list(
			multi_row_dictionary );

	highest_index =
		dictionary_highest_index(
			multi_row_dictionary );

	for(	index = 1;
		index <= highest_index;
		index++ )
	{
		name_list =
			dictionary_index_populated_name_list(
				folder_attribute_non_primary_key_list,
				multi_row_dictionary,
				index );

		if ( !list_length( name_list ) ) continue;

		dictionary_index_set(
			dictionary /* out */,
			multi_row_dictionary,
			key_list,
			index );
	}

	return dictionary;
}

INSERT *insert_automatic_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *folder_attribute_primary_key_list,
		LIST *folder_attribute_append_isa_list,
		LIST *relation_mto1_isa_list,
		DICTIONARY *prompt_dictionary,
		DICTIONARY *multi_row_dictionary,
		PROCESS *post_change_process )
{
	LIST *non_primary_key_list;
	DICTIONARY *dictionary;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !list_length( folder_attribute_primary_key_list )
	||   !list_length( folder_attribute_append_isa_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !dictionary_length( prompt_dictionary )
	&&   !dictionary_length( multi_row_dictionary ) )
	{
		return NULL;
	}

	non_primary_key_list =
		folder_attribute_non_primary_key_list(
			folder_attribute_append_isa_list );

	if ( !list_length( non_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message,
		"folder_attribute_non_primary_key_list() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	dictionary =
		insert_automatic_multi_row_dictionary(
			non_primary_key_list,
			multi_row_dictionary );

	if ( dictionary_length( dictionary ) )
	{
		return
		insert_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			folder_attribute_primary_key_list,
			folder_attribute_append_isa_list,
			relation_mto1_isa_list,
			prompt_dictionary,
			dictionary,
			(LIST *)0 /* ignore_name_list */,
			post_change_process );
	}

	return NULL;
}

void insert_multi_attribute_default_set(
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *multi_row_dictionary /* in/out */,
		int row_number )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	char *key;
	char *get;

	if ( list_rewind( folder_attribute_append_isa_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		if ( folder_attribute->default_value )
		{
			key =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				insert_multi_key(
					folder_attribute->attribute_name,
					row_number );

			get =
				dictionary_get(
					key,
					multi_row_dictionary );

			if ( !get )
			{
				dictionary_set(
					multi_row_dictionary,
					strdup( key ),
					folder_attribute->default_value );
			}
		}

	} while ( list_next( folder_attribute_append_isa_list ) );
}

char *insert_multi_key(
		char *attribute_name,
		int row_number )
{
	static char key[ 128 ];

	if ( !attribute_name
	||   !row_number )
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
		key,
		sizeof ( key ),
		"%s_%d",
		attribute_name,
		row_number );

	return key;
}

boolean insert_multi_all_primary_null_boolean(
		const char *appaserver_null_string,
		LIST *primary_key_list,
		DICTIONARY *multi_row_dictionary,
		int row_number )
{
	char *primary_key;
	char *key;
	char *get;

	if ( !list_rewind( primary_key_list )
	||   !dictionary_length( multi_row_dictionary )
	||   !row_number )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	do {
		primary_key =
			list_get(
				primary_key_list );


		key =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			insert_multi_key(
				primary_key /* attribute_name */,
				row_number );

		get =
			dictionary_get(
				key,
				multi_row_dictionary );

		if ( string_strcmp(
			get,
			(char *)appaserver_null_string ) != 0 )
		{
			return 0;
		}

	} while ( list_next( primary_key_list ) );

	return 1;
}

void insert_zero_attribute_default_set(
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *prompt_dictionary /* in/out */,
		LIST *ignore_name_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	char *ignore_name;
	char *get;

	if ( list_rewind( ignore_name_list ) )
	do {
		ignore_name = list_get( ignore_name_list );

		if ( ! ( folder_attribute =
				folder_attribute_seek(
					ignore_name,
					folder_attribute_append_isa_list ) ) )
		{
			char message[ 128 ];

			sprintf(message,
				"folder_attribute_seek(%s) returned empty.",
				ignore_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( folder_attribute->default_value )
		{
			get =
				dictionary_get(
					folder_attribute->attribute_name,
					prompt_dictionary );

			if ( !get )
			{
				dictionary_set(
					prompt_dictionary,
					folder_attribute->attribute_name,
					folder_attribute->default_value );
			}
		}

	} while ( list_next( ignore_name_list ) );
}

char *insert_folder_statement_system_string(
		const char sql_delimiter,
		char *application_table_name,
		char *field_list_string )
{
	char system_string[ 1024 ];

	if ( !application_table_name
	||   !field_list_string )
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
		"insert_statement.e "
		"table_name=%s "
		"field_list=%s "
		"delimiter='%c'",
		application_table_name,
		field_list_string,
		sql_delimiter );

	return strdup( system_string );
}

