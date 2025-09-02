/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/libary/folder.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "sql.h"
#include "piece.h"
#include "appaserver.h"
#include "folder_attribute.h"
#include "appaserver_error.h"
#include "role.h"
#include "role_folder.h"
#include "widget.h"
#include "application.h"
#include "folder.h"

FOLDER *folder_new( char *folder_name )
{
	FOLDER *folder;

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

	folder = folder_calloc();
	folder->folder_name = folder_name;

	return folder;
}

FOLDER *folder_calloc( void )
{
	FOLDER *folder;

	if ( ! ( folder = calloc( 1, sizeof ( FOLDER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return folder;
}

char *folder_primary_where_string(
		const char *folder_primary_key,
		char *folder_name )
{
	static char where_string[ 128 ];

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


	sprintf(where_string,
		"%s = '%s'",
		folder_primary_key,
		folder_name );

	return where_string;
}

FOLDER *folder_parse(
		LIST *role_attribute_exclude_lookup_name_list,
		boolean fetch_folder_attribute_list,
		boolean fetch_attribute,
		boolean cache_boolean,
		char *input )
{
	char field[ 128 ];
	char notepad[ 65536 ];
	FOLDER *folder;

	if ( !input || !*input ) return NULL;

	/* See: FOLDER_SELECT */
	/* ------------------ */
	piece( field, SQL_DELIMITER, input, 0 );

	if ( strcmp( field, "null" ) == 0 ) return NULL;

	folder = folder_new( strdup( field ) );

	piece( field, SQL_DELIMITER, input, 1 );
	if ( *field ) folder->appaserver_form = strdup( field );

	piece( field, SQL_DELIMITER, input, 2 );
	if ( *field ) folder->insert_rows_number = atoi( field );

	piece( notepad, SQL_DELIMITER, input, 3 );
	if ( *notepad ) folder->notepad = strdup( notepad );

	piece( field, SQL_DELIMITER, input, 4 );
	if ( *field ) folder->populate_drop_down_process_name = strdup( field );

	piece( field, SQL_DELIMITER, input, 5 );
	if ( *field ) folder->post_change_process_name = strdup( field );

	piece( field, SQL_DELIMITER, input, 6 );
	if ( *field ) folder->html_help_file_anchor = strdup( field );

	piece( field, SQL_DELIMITER, input, 7 );
	if ( *field ) folder->post_change_javascript = strdup( field );

	piece( field, SQL_DELIMITER, input, 8 );
	if ( *field ) folder->subschema = strdup( field );

	piece( field, SQL_DELIMITER, input, 9 );
	if ( *field ) folder->storage_engine = strdup( field );

	piece( field, SQL_DELIMITER, input, 10 );
	folder->exclude_application_export = ( *field == 'y' );

	piece( field, SQL_DELIMITER, input, 11 );
	folder->drillthru = ( *field == 'y' );

	piece( field, SQL_DELIMITER, input, 12 );
	folder->no_initial_capital = ( *field == 'y' );

	piece( field, SQL_DELIMITER, input, 13 );
	if ( *field ) folder->index_directory = strdup( field );

	piece( field, SQL_DELIMITER, input, 14 );
	if ( *field ) folder->data_directory = strdup( field );

	piece( notepad, SQL_DELIMITER, input, 15 );
	if ( *notepad ) folder->create_view_statement = strdup( notepad );

	piece( field, SQL_DELIMITER, input, 16 );
	if ( *field ) folder->javascript_filename = strdup( field );

	if ( fetch_folder_attribute_list )
	{
		/* Will be empty if inserting new rows. */
		/* ------------------------------------ */
		folder->folder_attribute_list =
			folder_attribute_list(
				folder->folder_name,
				role_attribute_exclude_lookup_name_list,
				fetch_attribute,
				cache_boolean );

		folder->folder_attribute_primary_list =
			folder_attribute_primary_list(
				folder->folder_name,
				folder->folder_attribute_list );

		folder->folder_attribute_primary_key_list =
			folder_attribute_primary_key_list(
				folder->folder_name,
				folder->folder_attribute_primary_list );

		folder->folder_attribute_name_list =
			folder_attribute_name_list(
				folder->folder_name,
				folder->folder_attribute_list );
	}

	return folder;
}

unsigned long folder_row_count(
		char *folder_table_name )
{
	char system_string[ 1024 ];
	char *pipe_fetch;

	sprintf(system_string,
		"echo \"select count(1) from %s;\" | sql_timeout.sh 1",
		folder_table_name );

	if ( ( pipe_fetch = string_pipe_fetch( system_string ) ) )
		return strtoul( pipe_fetch, NULL, 10 );
	else
		return 0;
}

char *folder_table_name(
		char *application_name,
		char *folder_name )
{
	if ( application_name ){}

	return
	/* --------------------- */
	/* Returns static memory */
	/* --------------------- */
	application_table_name(
		folder_name );
}

FOLDER *folder_fetch(
		char *folder_name,
		LIST *role_attribute_exclude_name_list,
		boolean fetch_folder_attribute_list,
		boolean fetch_attribute,
		boolean cache_boolean )
{
	FOLDER *folder = {0};

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

	if ( cache_boolean )
	{
		folder =
			folder_cache_fetch(
				folder_name,
				role_attribute_exclude_name_list,
				fetch_folder_attribute_list,
				fetch_attribute );
	}

	if ( !folder )
	{
		folder =
			folder_where_fetch(
				folder_name,
				role_attribute_exclude_name_list,
				fetch_folder_attribute_list,
				fetch_attribute );
	}

	if ( !folder )
	{
		char message[ 128 ];

		sprintf(message,
			"Non existant folder_name=[%s]",
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return folder;
}

LIST *folder_list(
		LIST *role_attribute_exclude_lookup_name_list,
		boolean fetch_folder_attribute_list,
		boolean fetch_attribute )
{
	FILE *input_pipe;
	LIST *list;
	char input[ STRING_64K ];

	input_pipe =
		appaserver_input_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_system_string(
				FOLDER_SELECT,
				FOLDER_TABLE,
				(char *)0 /* where_string */ ) );

	list = list_new();

	while ( string_input( input, input_pipe, STRING_64K ) )
	{
		list_set(
			list,
			folder_parse(
				role_attribute_exclude_lookup_name_list,
				fetch_folder_attribute_list,
				fetch_attribute,
				1 /* cache_boolean */,
				input ) );
	}

	pclose( input_pipe );
	return list;
}

LIST *folder_name_list( LIST *folder_list )
{
	LIST *name_list = list_new();
	FOLDER *folder;

	if ( list_rewind( folder_list ) )
	do {
		folder = list_get( folder_list );

		list_set(
			name_list,
			folder->folder_name );

	} while ( list_next( folder_list ) );

	if ( !list_length( name_list ) )
	{
		list_free( name_list );
		name_list = NULL;
	}

	return name_list;
}

LIST *folder_table_name_list( char *role_name )
{
	char *where_string;
	char *system_string;
	LIST *name_list = list_new();
	FILE *input_pipe;
	char folder_name[ 128 ];

	where_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_role_where_string(
			ROLE_FOLDER_TABLE,
			FOLDER_PRIMARY_KEY,
			role_name /* role_name_list_string */ );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			FOLDER_PRIMARY_KEY /* select */,
			FOLDER_TABLE,
			where_string );

	/* Safely returns */
	/* -------------- */
	input_pipe = appaserver_input_pipe( system_string );

	while ( string_input( folder_name, input_pipe, 128 ) )
	{
		list_set(
			name_list,
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				application_table_name(
					folder_name ) ) );
	}

	pclose( input_pipe );
	free( system_string );

	return name_list;
}

char *folder_role_where_string(
		const char *role_folder_table,
		const char *folder_primary_key,
		char *role_name_list_string )
{
	static char where_string[ 256 ];
	LIST *list;
	char *in_clause;

	if ( !role_name_list_string )
	{
		char message[ 128 ];

		sprintf(message, "role_name_list_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list = string_list( role_name_list_string );
	in_clause = string_in_clause( list );

	sprintf(where_string,
		"%s <> 'null' and %s in (select %s from %s where role in (%s))",
		folder_primary_key,
		folder_primary_key,
		folder_primary_key,
		role_folder_table,
		in_clause );

	return where_string;
}

FOLDER *folder_seek(
		char *folder_name,
		LIST *folder_list )
{
	FOLDER *folder;

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

	if ( list_rewind( folder_list ) )
	do {
		folder = list_get( folder_list );

		if ( strcmp(
			folder_name,
			folder->folder_name ) == 0 )
		{
			return folder;
		}

	} while ( list_next( folder_list ) );

	return NULL;
}

FOLDER *folder_cache_fetch(
		char *folder_name,
		LIST *role_attribute_exclude_lookup_name_list,
		boolean fetch_folder_attribute_list,
		boolean fetch_attribute )
{
	static LIST *list = {0};

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

	if ( !list )
	{
		list =
			folder_list(
				role_attribute_exclude_lookup_name_list,
				fetch_folder_attribute_list,
				fetch_attribute );
	}

	return
	folder_seek(
		folder_name,
		list /* folder_list */ );
}

FOLDER *folder_where_fetch(
		char *folder_name,
		LIST *role_attribute_exclude_lookup_name_list,
		boolean fetch_folder_attribute_list,
		boolean fetch_attribute )
{
	char *input;

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


	input =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe_input(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_system_string(
				FOLDER_SELECT,
				FOLDER_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				folder_primary_where_string(
					FOLDER_PRIMARY_KEY,
					folder_name ) ) );

	if ( !input ) return NULL;

	return
	folder_parse(
		role_attribute_exclude_lookup_name_list,
		fetch_folder_attribute_list,
		fetch_attribute,
		0 /* not cache_boolean */,
		input );
}

char *folder_appaserver_form( char *folder_name )
{
	FOLDER *folder;

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

	folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			0 /* not fetch_folder_attribute_list */,
			0 /* not fetch_attribute */,
			0 /* not cache_boolean */ );

	return folder->appaserver_form;
}

char *folder_notepad( char *folder_name )
{
	FOLDER *folder;

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

	folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			0 /* not fetch_folder_attribute_list */,
			0 /* not fetch_attribute */,
			0 /* not cache_boolean */ );

	return folder->notepad;
}

boolean folder_no_initial_capital( char *folder_name )
{
	FOLDER *folder;

	if ( !folder_name
	||   strcmp( folder_name, "null" ) == 0 )
	{
		return 0;
	}

	folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			0 /* not fetch_folder_attribute_list */,
			0 /* not fetch_attribute */,
			0 /* not cache_boolean */ );

	return folder->no_initial_capital;
}

LIST *folder_system_name_list( const char *role_system )
{
	char *where_string;
	char *system_string;

	where_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_role_where_string(
			ROLE_FOLDER_TABLE,
			FOLDER_PRIMARY_KEY,
			(char *)role_system );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			FOLDER_PRIMARY_KEY /* select */,
			FOLDER_TABLE,
			where_string );

	return
	list_pipe_fetch( system_string );
}

LIST *folder_fetch_name_list(
		const char *folder_primary_key,
		const char *folder_table )
{
	char *system_string;
	char *where_string;

	where_string = "table_name <> 'null'";

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)folder_primary_key /* select */,
			(char *)folder_table,
			where_string );

	return
	list_pipe_fetch( system_string );
}

char *folder_column_fetch(
		const char *column_name,
		char *folder_name )
{
	char *where_string;
	char *system_string;
	char *pipe_input;

	if ( !folder_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	where_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_primary_where_string(
			FOLDER_PRIMARY_KEY,
			folder_name );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)column_name /* select */,
			FOLDER_TABLE,
			where_string );

	pipe_input =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe_input(
			system_string );

	free( system_string );

	if ( pipe_input && *pipe_input )
		return pipe_input;
	else
		return NULL;
}

boolean folder_column_boolean(
		char *table_name,
		char *column_name )
{
	char system_string[ 1024 ];
	int result;

	if ( !table_name
	||   !column_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"table_column_exists.sh %s %s",
		table_name,
		column_name );

	result = system( system_string );

	/* Shell returns 0 if true */
	/* ----------------------- */
	return (boolean)!result;
}

boolean folder_form_prompt_boolean(
		const char *form_appaserver_prompt,
		char *appaserver_form )
{
	if ( !appaserver_form )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"appaserver_form is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strcmp( appaserver_form, form_appaserver_prompt ) == 0 )
		return 1;
	else
		return 0;
}
