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
			LIST *folder_attribute_name_list,
			DICTIONARY *row_zero_dictionary,
			DICTIONARY *dictionary_separate_multi_row,
			LIST *ignore_name_list,
			char *process_name,
			char *post_change_command_line )
{
	INSERT *insert = insert_calloc();

	insert->insert_row_list = list_new();

	insert->folder_table_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_table_name(
			application_name,
			folder_name );

	if ( !dictionary_separate_multi_row )
	{
		insert->insert_row =
			insert_row_new(
				insert->folder_table_name,
				application_name,
				session_key,
				login_name,
				role_name,
				primary_key_list,
				folder_attribute_name_list,
				row_zero_dictionary,
				(DICTIONARY *)0 /* dictionary_separate_row */,
				ignore_name_list,
				process_name,
				post_change_command_line );

		if ( insert->insert_row )
		{
			list_set(
				insert->insert_row_list,
				insert->insert_row );
		}
	}
	else
	{
		insert->dictionary_highest_row =
			dictionary_highest_row(
				dictionary_separate_multi_row );

		if ( insert->dictionary_highest_row < 1 )
		{
			list_free( insert->insert_row_list );
			free( insert );
			return (INSERT *)0;
		}

		for (	insert->row_number = 1;
			insert->row_number <= insert->dictionary_highest_row;
			insert->row_number++ )
		{
			insert->dictionary_separate_row =
				dictionary_separate_row(
					folder_attribute_name_list,
					dictionary_separate_multi_row,
					insert->row_number );

			if ( !dictionary_length(
				insert->
					dictionary_separate_row ) )
			{
				continue;
			}

			insert->insert_row =
				insert_row_new(
					insert->folder_table_name,
					application_name,
					session_key,
					login_name,
					role_name,
					primary_key_list,
					folder_attribute_name_list,
					row_zero_dictionary,
					insert->dictionary_separate_row,
					ignore_name_list,
					process_name,
					post_change_command_line );

			if ( insert->insert_row )
			{
				list_set(
					insert->insert_row_list,
					insert->insert_row );
			}
		}
	}

	if ( !list_length( insert->insert_row_list ) )
	{
		list_free( insert->insert_row_list );
		free( insert );
		insert = (INSERT *)0;
	}

	return insert;
}

LIST *insert_row_sql_statement_list(
			LIST *insert_row_list )
{
	INSERT_ROW *insert_row;
	LIST *sql_statement_list;

	if ( !list_rewind( insert_row_list ) ) return (LIST *)0;

	sql_statement_list = list_new();

	do {
		insert_row = list_get( insert_row_list );

		if ( !insert_row->sql_statement )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: sql_statement is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			sql_statement_list,
			insert_row->sql_statement );

	} while ( list_next( insert_row_list ) );

	return sql_statement_list;
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
			char *folder_table_name,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			LIST *primary_key_list,
			LIST *folder_attribute_name_list,
			DICTIONARY *row_zero_dictionary,
			DICTIONARY *dictionary_separate_row,
			LIST *ignore_name_list,
			char *process_name,
			char *post_change_command_line )
{
	INSERT_ROW *insert_row;
	INSERT_DATA *insert_data;
	char *attribute_name;

	if ( !folder_table_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !list_length( primary_key_list )
	||   !list_rewind( folder_attribute_name_list ) )
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
	insert_row->insert_data_list = list_new();

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

	if ( !insert_row_primary_key_okay(
		insert_data_name_list(
			insert_row->insert_data_list ),
		primary_key_list ) )
	{
		list_free( insert_row->insert_data_list );
		free( insert_row );
		return (INSERT_ROW *)0;
	}

	insert_row->sql_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		insert_row_sql_statement(
			folder_table_name,
			insert_data_name_list(
				insert_row->insert_data_list ),
			insert_data_extract_list(
				insert_row->insert_data_list ) );


	if ( process_name && *process_name )
	{
		insert_row->command_line =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			insert_row_command_line(
				post_change_command_line,
				application_name,
				session_key,
				login_name,
				role_name,
				APPASERVER_INSERT_STATE,
				process_name,
				insert_data_key_data_list(
					primary_key_list,
					insert_row->insert_data_list )
						/* primary_data_list */,
				insert_row->insert_data_list );
	}

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

boolean insert_row_primary_key_okay(
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

INSERT_DATA *insert_data_seek(
			char *attribute_name,
			LIST *insert_data_list )
{
	INSERT_DATA *insert_data;

	if ( !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( insert_data_list ) ) return (INSERT_DATA *)0;

	do {
		insert_data = list_get( insert_data_list );

		if ( strcmp(
			insert_data->attribute_name,
			attribute_name ) == 0 )
		{
			return insert_data;
		}
	} while ( list_next( insert_data_list ) );

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

char *insert_sql_execute( LIST *insert_row_list )
{
	INSERT_ROW *insert_row;
	char system_string[ 128 ];
	FILE *output_pipe;
	char *temp_filename = timlib_temp_filename( "insert" /* key */ );

	if ( !list_rewind( insert_row_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: insert_row_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"sql 2>%s",
		temp_filename );

	output_pipe = popen( system_string, "w" );

	do {
		insert_row = list_get( insert_row_list );

		if ( !insert_row->sql_statement )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: sql_statement is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );

			pclose( output_pipe );
			exit( 1 );
		}

		fprintf(output_pipe,
			"%s\n",
			insert_row->sql_statement );

	} while ( list_next( insert_row_list ) );

	pclose( output_pipe );

	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	string_file_fetch(
		temp_filename,
		"\n<br>" /* delimiter */ );
}

void insert_command_line_execute( LIST *insert_row_list )
{
	INSERT_ROW *insert_row;

	if ( !list_rewind( insert_row_list ) ) return;

	do {
		insert_row =
			list_get(
				insert_row_list );

		if ( insert_row->command_line )
		{
			if ( system( insert_row->command_line ) ){}
		}

	} while ( list_next( insert_row_list ) );
}

