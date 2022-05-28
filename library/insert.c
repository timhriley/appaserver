/* $APPASERVER_HOME/library/insert.c					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "timlib.h"
#include "folder.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "dictionary_separate.h"
#include "insert.h"

INSERT *insert_new(	char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			LIST *primary_key_list,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_isa_list,
			DICTIONARY *row_zero_dictionary,
			DICTIONARY *multi_row_dictionary,
			LIST *ignore_name_list,
			char *process_name,
			char *post_change_command_line )
{
	INSERT *insert = insert_calloc();

	insert->insert_row_list =
		insert_row_list_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			primary_key_list,
			folder_attribute_append_isa_list,
			relation_mto1_isa_list,
			row_zero_dictionary,
			multi_row_dictionary,
			ignore_name_list,
			process_name,
			post_change_command_line,
			appaserver_error_filename( application_name ) );

	if ( !insert->insert_row_list )
	{
		free( insert );
		return (INSERT *)0;
	}

	insert->insert_sql_statement_list =
		insert_sql_statement_list_new(
			insert->insert_row_list );

	return insert;
}

LIST *insert_sql_statement_list_extract_list(
			INSERT_SQL_STATEMENT_LIST *insert_sql_statement_list )
{
	INSERT_FOLDER_SQL_STATEMENT *insert_folder_sql_statement;
	LIST *sql_statement_list;

	if ( !insert_sql_statement_list
	||   !list_rewind( insert_sql_statement_list->list ) )
	{
		return (LIST *)0;
	}

	sql_statement_list = list_new();

	do {
		insert_folder_sql_statement =
			list_get(
				insert_sql_statement_list->
					list );

		if ( !list_rewind(
			insert_folder_sql_statement->
				sql_statement_list ) )
		{
			continue;
		}

		do {
			list_set(
				sql_statement_list,
				list_get(
					insert_folder_sql_statement->
						sql_statement_list ) );

		} while ( list_next(
				insert_folder_sql_statement->
					sql_statement_list ) );

	} while ( list_next( insert_sql_statement_list->list ) );

	return sql_statement_list;
}

LIST *insert_sql_statement_list_extract_command_line_list(
			INSERT_SQL_STATEMENT_LIST *insert_sql_statement_list )
{
	INSERT_FOLDER_SQL_STATEMENT *insert_folder_sql_statement;
	LIST *command_line_list;

	if ( !insert_sql_statement_list
	||   !list_rewind( insert_sql_statement_list->list ) )
	{
		return (LIST *)0;
	}

	command_line_list = list_new();

	do {
		insert_folder_sql_statement =
			list_get(
				insert_sql_statement_list->
					list );

		if ( !list_rewind(
			insert_folder_sql_statement->
				command_line_list ) )
		{
			continue;
		}

		do {
			list_set(
				command_line_list,
				list_get(
					insert_folder_sql_statement->
						command_line_list ) );

		} while ( list_next(
				insert_folder_sql_statement->
					command_line_list ) );

	} while ( list_next( insert_sql_statement_list->list ) );

	return command_line_list;
}

LIST *insert_row_post_change_command_line_list(
			LIST *insert_row_list )
{
	INSERT_ROW *insert_row;
	LIST *command_line_list;

	if ( !list_rewind( insert_row_list ) ) return (LIST *)0;

	command_line_list = list_new();

	do {
		insert_row = list_get( insert_row_list );

		if ( !insert_row->command_line )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: command_line is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			command_line_list,
			insert_row->command_line );

	} while ( list_next( insert_row_list ) );

	return command_line_list;
}

INSERT_ROW *insert_row_calloc( void )
{
	INSERT_ROW *insert_row;

	if ( ! ( insert_row = calloc( 1, sizeof( INSERT_ROW ) ) ) )
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
			LIST *primary_key_list,
			LIST *folder_attribute_append_isa_list,
			DICTIONARY *row_zero_dictionary,
			DICTIONARY *dictionary_separate_row,
			LIST *ignore_name_list,
			char *process_name,
			char *post_change_command_line,
			char *appaserver_error_filename )
{
	INSERT_ROW *insert_row;
	INSERT_FOLDER *insert_folder;
	RELATION *relation;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !list_length( primary_key_list )
	||   !list_length( folder_attribute_append_isa_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !dictionary_length( row_zero_dictionary )
	&&   !dictionary_length( dictionary_separate_row ) )
	{
		return (INSERT_ROW *)0;
	}

	insert_row = insert_row_calloc();
	insert_row->insert_folder_list = list_new();

	insert_row->folder_attribute_name_list =
		folder_attribute_name_list(
			folder_name,
			folder_attribute_append_isa_list );

	insert_folder =
		insert_folder_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			primary_key_list,
			insert_row->folder_attribute_name_list,
			row_zero_dictionary,
			dictionary_separate_row,
			ignore_name_list,
			process_name,
			post_change_command_line,
			appaserver_error_filename );

	if ( !insert_folder )
	{
		list_free( insert_row->insert_folder_list );
		free( insert_row );
		return (INSERT_ROW *)0;
	}

	list_set( insert_row->insert_folder_list, insert_folder );

	if ( !list_rewind( relation_mto1_isa_list ) )
	{
		return insert_row;
	}

	do {
		relation = list_get( relation_mto1_isa_list );

		if ( !relation->one_folder )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: one_folder is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !list_length( relation->one_folder->primary_key_list ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: one_folder->primary_key_list is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		insert_row->folder_attribute_name_list =
			folder_attribute_name_list(
				relation->one_folder->folder_name,
				folder_attribute_append_isa_list );

		insert_folder =
			insert_folder_new(
				application_name,
				session_key,
				login_name,
				role_name,
				relation->one_folder->folder_name,
				relation->one_folder->primary_key_list,
				insert_row->folder_attribute_name_list,
				row_zero_dictionary,
				dictionary_separate_row,
				ignore_name_list,
				relation->one_folder->process_name,
				(relation->
					one_folder->
					process)
						? relation->
							one_folder->
							process->
							command_line
						: (char *)0,
				appaserver_error_filename );

		if ( insert_folder )
		{
			list_set( insert_folder_list, insert_folder );
		}

	} while ( list_next( relation_mto1_isa_list ) );

	return insert_row;
}

char *insert_row_command_line(
			char *post_change_command_line,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *appaserver_insert_state,
			char *process_name,
			LIST *primary_data_list,
			LIST *insert_data_list )
{
	char command_line[ 1024 ];
	char buffer[ 512 ];
	INSERT_DATA *insert_data;

	if ( !post_change_command_line
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !appaserver_insert_state
	||   !process_name
	||   !list_length( primary_data_list )
	||   !list_rewind( insert_data_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	string_strcpy(
		command_line,
		post_change_command_line,
		1024 );

	search_replace_word(
		command_line,
		"$session",
		double_quotes_around(
			buffer,
			session_key ) );

	search_replace_word(
		command_line,
		"$login",
		double_quotes_around(
			buffer,
			login_name ) );

	search_replace_word(
		command_line,
		"$role",
		double_quotes_around(
			buffer,
			role_name ) );

	search_replace_word(
		command_line,
		"$state",
		double_quotes_around(
			buffer,
			appaserver_insert_state ) );

	search_replace_word(
		command_line,
		"$process",
		double_quotes_around(
			buffer,
			process_name ) );

	search_replace_word(
		command_line,
		"$primary_data_list",
		double_quotes_around(
			buffer,
			list_display_delimited(
				primary_data_list,
				',' ) ) );

	do {
		insert_data = list_get( insert_data_list );

		search_replace_word(
			command_line,
			insert_data->attribute_name,
			double_quotes_around(
				buffer,
				insert_data->data ) );

	} while ( list_next( insert_data_list ) );

	sprintf(command_line + strlen( command_line ),
		" 2>>%s",
		appaserver_error_filename(
			application_name ) );

	return strdup( command_line );
}

boolean insert_folder_primary_key_okay(
			LIST *insert_data_name_list,
			LIST *primary_key_list )
{
	LIST *results_list;

	results_list =
		list_subtract(
			primary_key_list,
			insert_data_name_list );

	if ( list_length( results_list ) )
		return 0;
	else
		return 1;
}

char *insert_row_sql_statement(
			char *folder_table_name,
			LIST *insert_data_name_list,
			LIST *insert_data_extract_list )
{
	char sql_statement[ 1024 ];
	char buffer[ 1024 ];

	if ( !folder_table_name
	||   !list_length( insert_data_name_list )
	||   !list_length( insert_data_extract_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if (	list_length( insert_data_name_list ) !=
		list_length( insert_data_extract_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: length=%d != length=%d.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_length( insert_data_name_list ),
			list_length( insert_data_extract_list ) );
		exit( 1 );
	}

	sprintf(sql_statement,
		"insert into %s (%s) values (%s)",
		folder_table_name,
		list_display_delimited(
			insert_data_name_list,
			',' ),
		list_display_quoted_delimited(
			buffer,
			insert_data_extract_list,
			',' ) );

	return strdup( sql_statement );
}

LIST *insert_data_name_list( LIST *insert_data_list )
{
	INSERT_DATA *insert_data;
	LIST *name_list;

	if ( !list_rewind( insert_data_list ) ) return (LIST *)0;

	name_list = list_new();

	do {
		insert_data =
			list_get(
				insert_data_list );

		list_set(
			name_list,
			insert_data->attribute_name );

	} while ( list_next( insert_data_list ) );

	return name_list;
}

LIST *insert_data_extract_list( LIST *insert_data_list )
{
	INSERT_DATA *insert_data;
	LIST *data_list;

	if ( !list_rewind( insert_data_list ) ) return (LIST *)0;

	data_list = list_new();

	do {
		insert_data =
			list_get(
				insert_data_list );

		list_set(
			data_list,
			insert_data->data );

	} while ( list_next( insert_data_list ) );

	return data_list;
}

INSERT *insert_calloc( void )
{
	INSERT *insert;

	if ( ! ( insert = calloc( 1, sizeof( INSERT ) ) ) )
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

INSERT_DATA *insert_data_calloc( void )
{
	INSERT_DATA *insert_data;

	if ( ! ( insert_data = calloc( 1, sizeof( INSERT_DATA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return insert_data;
}

INSERT_DATA *insert_data_new(
			char *attribute_name,
			char *data )
{
	INSERT_DATA *insert_data = insert_data_calloc();

	insert_data->attribute_name = attribute_name;
	insert_data->data = data;

	return insert_data;
}

INSERT_DATA *insert_data_extract(
			DICTIONARY *row_zero_dictionary,
			DICTIONARY *dictionary_separate_row,
			char *attribute_name )
{
	char *data;

	if ( ( data =
		dictionary_get(
			attribute_name,
			row_zero_dictionary ) ) )
	{
		return
		insert_data_new(
			attribute_name,
			data );
	}

	if ( ( data =
		dictionary_get(
			attribute_name,
			dictionary_separate_row ) ) )
	{
		return
		insert_data_new(
			attribute_name,
			data );
	}

	return (INSERT_DATA *)0;
}

LIST *insert_data_key_data_list(
			LIST *primary_key_list,
			LIST *insert_data_list )
{
	char *primary_key;
	INSERT_DATA *insert_data;
	LIST *data_list = {0};

	if ( !list_rewind( primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		primary_key = list_get( primary_key_list );

		if ( ( insert_data =
				insert_data_seek(
					primary_key,
					insert_data_list ) ) )
		{
			if ( !data_list ) data_list = list_new();

			list_set( data_list, insert_data->data );
		}
	} while ( list_next( primary_key_list ) );

	return data_list;
}

char *insert_sql_statement_list_extract_list_execute(
			LIST *insert_sql_statement_list_extract_list )
{
	char system_string[ 128 ];
	FILE *output_pipe;
	char *temp_filename;
	char *return_string;

	if ( !list_rewind( insert_sql_statement_list_extract_list ) )
	{
		return (char *)0;
	}

	sprintf(system_string,
		"sql 2>%s",
		( temp_filename =
			timlib_temp_filename( "insert" /* key */ ) ) );

	output_pipe = popen( system_string, "w" );

	do {
		fprintf(output_pipe,
			"%s\n",
			(char *)list_get(
				insert_sql_statement_list_extract_list ) );

	} while ( list_next(
			insert_sql_statement_list_extract_list ) );

	pclose( output_pipe );

	return_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_file_fetch(
			temp_filename,
			"\n<br>" /* delimiter */ );

	timlib_remove_file( temp_filename );

	return return_string;
}

void insert_sql_statement_list_extract_command_line_list_execute(
		LIST *insert_sql_statement_list_extract_command_line_list )
{
	LIST *list = insert_sql_statement_list_extract_command_line_list;

	if ( !list_rewind( list ) ) return;

	do {
		if ( system( (char *)list_get( list ) ) ){}

	} while ( list_next( list ) );
}

INSERT_FOLDER_SQL_STATEMENT *insert_folder_sql_statement_new(
			LIST *insert_folder_list )
{
	INSERT_FOLDER_SQL_STATEMENT *insert_folder_sql_statement;

	if ( !list_rewind( insert_folder_list ) )
	{
		return (INSERT_FOLDER_SQL_STATEMENT *)0;
	}

	insert_folder_sql_statement = insert_folder_sql_statement_calloc();

	insert_folder_sql_statement->sql_statement_list = list_new();

	do {
		insert_folder =
			list_get(
				insert_folder_list );

		if ( !insert_folder->sql_statement )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: sql_statement is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			insert_folder_sql_statement->sql_statement_list,
			insert_folder->sql_statement );

		if ( insert_folder->command_line )
		{
			if ( !insert_folder_sql_statement->command_line_list )
			{
				insert_folder_sql_statement->
					command_line_list =
						list_new();
			}

			list_set(
				insert_folder_sql_statement->command_line_list,
				insert_folder->command_line );
		}

	} while ( list_next( insert_folder_list ) );

	return insert_folder_sql_statement;
}

INSERT_FOLDER_SQL_STATEMENT *insert_folder_sql_statement_calloc( void )
{
	INSERT_FOLDER_SQL_STATEMENT *insert_folder_sql_statement;

	if ( ! ( insert_folder_sql_statement =
			calloc(	1,
				sizeof( INSERT_FOLDER_SQL_STATEMENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return insert_folder_sql_statement;
}

INSERT_SQL_STATEMENT_LIST *insert_sql_statement_list_new(
			INSERT_ROW_LIST *insert_row_list )
{
	INSERT_SQL_STATEMENT_LIST *insert_sql_statement_list;

	if ( !insert_row_list
	||   !list_rewind( insert_row_list->list ) )
	{
		return (INSERT_SQL_STATEMENT_LIST *)0;
	}

	insert_sql_statement_list = insert_sql_statement_list_calloc();

	insert_sql_statement_list->list = list_new();

	do {
		insert_sql_statement_list->insert_row =
			list_get(
				insert_row_list->list );

		if ( !list_length( insert_row->insert_folder_list ) )
			continue;

		list_set(
			insert_sql_statement_list->list,
			insert_folder_sql_statement_new(
				insert_row->insert_folder_list ) );

	} while ( list_next( insert_row_list->list ) );

	return insert_sql_statement_list;
}

INSERT_SQL_STATEMENT_LIST *insert_sql_statement_list_calloc( void )
{
	INSERT_SQL_STATEMENT_LIST *insert_sql_statement_list;

	if ( ! ( insert_sql_statement_list =
			calloc(	1,
				sizeof( INSERT_SQL_STATEMENT_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return insert_sql_statement_list;
}

char *insert_sql_statement_list_execute(
			INSERT_SQL_STATEMENT_LIST *insert_sql_statement_list )
{
	char *error_message_list_string;
	LIST *sql_statement_list;
	LIST *command_line_list;

	sql_statement_list =
		insert_sql_statement_list_extract_list(
			insert_sql_statement_list );

	if ( !list_length( sql_statement_list ) ) return (char *)0;

	error_message_list_string =
		insert_sql_statement_list_extract_list_execute(
			sql_statement_list );

	command_line_list =
		insert_sql_statement_list_extract_command_line_list(
			insert_sql_statement_list );

	if ( list_length( command_line_list ) )
	{
		insert_sql_statement_list_extract_command_line_list_execute(
			command_line_list );
	}

	return error_message_list_string;
}

INSERT_FOLDER *insert_folder_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			LIST *primary_key_list,
			LIST *folder_attribute_name_list,
			DICTIONARY *row_zero_dictionary,
			DICTIONARY *dictionary_separate_row,
			LIST *ignore_name_list,
			char *process_name,
			char *post_change_command_line,
			char *appaserver_error_filename )
{
	INSERT_FOLDER *insert_folder;
	INSERT_DATA *insert_data;
	char *attribute_name;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !list_length( primary_key_list )
	||   !list_rewind( folder_attribute_name_list )
	||   !appaserver_error_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !dictionary_length( row_zero_dictionary )
	&&   !dictionary_length( dictionary_separate_row ) )
	{
		return (INSERT_FOLDER *)0;
	}

	insert_folder = insert_folder_calloc();
	insert_folder->insert_data_list = list_new();

	do {
		attribute_name =
			list_get(
				folder_attribute_name_list );

		if ( !list_string_exists(
			attribute_name,
			ignore_name_list ) )
		{
			if ( ( insert_data =
				insert_data_extract(
					row_zero_dictionary,
					dictionary_separate_row,
					attribute_name ) ) )
			{
				list_set(
					insert_row->insert_data_list,
					insert_data );
			}
		}

	} while ( list_next( folder_attribute_name_list ) );

	if ( !list_length( insert_folder->insert_data_list ) )
	{
		list_free( insert_folder->insert_data_list );
		free( insert_folder );
		return (INSERT_FOLDER *)0;
	}

	insert_folder->insert_data_name_list =
		insert_data_name_list(
			insert_folder->insert_data_list );

	if ( !insert_folder_primary_key_okay(
		insert_folder->insert_data_name_list,
		primary_key_list ) )
	{
		list_free( insert_folder->insert_data_list );
		free( insert_folder );
		return (INSERT_FOLDER *)0;
	}

	insert_folder->insert_data_extract_list =
		insert_data_extract_list(
			insert_folder->insert_data_list );

	insert_folder->folder_table_name =
		folder_table_name(
			application_name,
			folder_name );

	insert_folder->sql_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		insert_folder_sql_statement(
			folder_table_name,
			insert_folder->insert_data_name_list,
			insert_folder->insert_data_extract_list );


	if ( process_name )
	{
		if ( !post_change_command_line )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: post_change_command_line is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		insert_folder->insert_data_key_data_list =
			insert_data_key_data_list(
				primary_key_list,
				insert_folder->insert_data_list );

		if ( !list_length( insert_folder->insert_data_key_data_list ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: insert_data_key_data_list() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		insert_folder->primary_data_list_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			insert_folder_primary_data_list_string(
				insert_folder->insert_data_key_data_list,
				SQL_DELIMITER );

		insert_folder->command_line =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			insert_folder_command_line(
				post_change_command_line,
				session_key,
				login_name,
				role_name,
				folder_name,
				process_name,
				APPASERVER_INSERT_STATE,
				insert_folder->insert_data_list,
				insert_folder->primary_data_list_string );
	}

	return insert_folder;
}

INSERT_FOLDER *insert_folder_calloc( void )
{
	INSERT_FOLDER *insert_folder;

	if ( ! ( insert_folder = calloc( 1, sizeof( INSERT_FOLDER ) ) ) )
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

char *insert_folder_sql_statement(
			char *folder_table_name,
			LIST *insert_data_name_list,
			LIST *insert_data_extract_list )
{
	char sql_statement[ STRING_64K ];
	char buffer[ 1024 ];

	if ( !folder_table_name
	||   !list_length( insert_data_name_list )
	||   !list_length( insert_data_extract_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if (	list_length( insert_data_name_list ) !=
		list_length( insert_data_extract_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: length=%d != length=%d.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_length( insert_data_name_list ),
			list_length( insert_data_extract_list ) );
		exit( 1 );
	}

	sprintf(sql_statement,
		"insert into %s (%s) values (%s)",
		folder_table_name,
		list_display_delimited(
			insert_data_name_list,
			',' ),
		list_display_quoted_delimited(
			buffer,
			insert_data_extract_list,
			',' ) );

	return strdup( sql_statement );
}

char *insert_folder_primary_data_list_string(
			LIST *insert_data_key_data_list,
			char sql_delimiter )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	list_display_delimited(
		insert_data_key_data_list,
		sql_delimiter );
}

char *insert_folder_command_line(
			char *post_change_command_line,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *process_name,
			char *appaserver_error_filename,
			char *appaserver_insert_state,
			LIST *insert_data_list,
			char *insert_folder_primary_data_list_string )
{
	char command_line[ STRING_8K ];
	char buffer[ 512 ];
	INSERT_DATA *insert_data;

	if ( !post_change_command_line
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !process_name
	||   !appaserver_insert_state
	||   !list_length( insert_data_list )
	||   !insert_folder_primary_data_list_string )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	string_strcpy(
		command_line,
		post_change_command_line,
		1024 );

	search_replace_word(
		command_line,
		"$session",
		double_quotes_around(
			buffer,
			session_key ) );

	search_replace_word(
		command_line,
		"$login",
		double_quotes_around(
			buffer,
			login_name ) );

	search_replace_word(
		command_line,
		"$role",
		double_quotes_around(
			buffer,
			role_name ) );

	search_replace_word(
		command_line,
		"$folder",
		double_quotes_around(
			buffer,
			folder_name ) );

	search_replace_word(
		command_line,
		"$process",
		double_quotes_around(
			buffer,
			process_name ) );

	search_replace_word(
		command_line,
		"$state",
		double_quotes_around(
			buffer,
			appaserver_insert_state ) );

	search_replace_word(
		command_line,
		"$primary_data_list",
		double_quotes_around(
			buffer,
			insert_folder_primary_data_list_string ) );

	list_rewind( insert_data_list );

	do {
		insert_data = list_get( insert_data_list );

		search_replace_word(
			command_line,
			insert_data->attribute_name,
			double_quotes_around(
				buffer,
				insert_data->data ) );

	} while ( list_next( insert_data_list ) );

	sprintf(command_line + strlen( command_line ),
		" 2>>%s",
		appaserver_error_filename );

	return strdup( command_line );
}

INSERT_ROW_LIST *insert_row_list_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			LIST *primary_key_list,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_isa_list,
			DICTIONARY *row_zero_dictionary,
			DICTIONARY *multi_row_dictionary,
			LIST *ignore_name_list,
			char *process_name,
			char *post_change_command_line,
			char *appaserver_error_filename )
{
}

INSERT_ROW_LIST *insert_row_list_calloc( void )
{
}

