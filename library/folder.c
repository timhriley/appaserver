/* $APPASERVER_HOME/libary/folder.c					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "sql.h"
#include "piece.h"
#include "environ.h"
#include "process.h"
#include "folder_attribute.h"
#include "role_folder.h"
#include "folder.h"

FOLDER *folder_new( char *sql_injection_escape_folder_name )
{
	FOLDER *folder;

	if ( ! ( folder = calloc( 1, sizeof( FOLDER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	folder->sql_injection_escape_folder_name =
		sql_injection_escape_folder_name;

	return folder;
}

char *folder_primary_where(
			char *sql_injection_escape_folder_name )
{
	static char where[ 256 ];

	sprintf(where,
		"folder = '%s'",
		sql_injection_escape_folder_name );

	return where;
}

char *folder_system_string( char *where )
{
	char system_string[ 1024 ];

	if ( !where ) where = "1 = 1";

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		FOLDER_SELECT,
		FOLDER_TABLE,
		where );

	return strdup( system_string );
}

LIST *folder_system_list(
			char *system_string,
			char *sql_injection_escape_role_name,
			LIST *role_exclude_attribute_name_lst,
			boolean fetch_folder_attribute_list,
			boolean fetch_process,
			boolean fetch_role_folder_list,
			boolean fetch_row_level_restriction )
{
	char input[ 2048 ];
	FILE *pipe;
	LIST *list = {0};
	FOLDER *folder;

	pipe = popen( system_string, "r"  );

	while ( string_input( input, pipe, 2048 ) )
	{
		if ( ( folder =
			folder_parse(
				input,
				sql_injection_escape_role_name,
				role_exclude_attribute_name_lst,
				fetch_folder_attribute_list,
				fetch_process,
				fetch_role_folder_list,
				fetch_row_level_restriction ) ) )
		{
			if ( !list ) list = list_new();

			list_set( list, folder_parse );
		}
	}

	pclose( pipe );
	return list;
}

FOLDER *folder_parse(	char *input,
			char *sql_injection_escape_role_name,
			LIST *role_exclude_attribute_name_list,
			boolean fetch_folder_attribute_list,
			boolean fetch_process,
			boolean fetch_role_folder_list,
			boolean fetch_row_level_restriction )
{
	char folder_name[ 128 ];
	char form[ 128 ];
	char insert_rows_number[ 128 ];
	char lookup_email_output_yn[ 128 ];
	char notepad[ 65536 ];
	char populate_drop_down_process[ 128 ];
	char post_change_process[ 128 ];
	char html_help_file_anchor[ 128 ];
	char post_change_javascript[ 2048 ];
	char subschema[ 128 ];
	char exclude_application_export_yn[ 128 ];
	char lookup_before_drop_down_yn[ 128 ];
	char no_initial_capital_yn[ 128 ];
	char index_directory[ 128 ];
	char data_directory[ 128 ];
	char create_view_statement[ 65536 ];
	char appaserver_yn[ 128 ];
	FOLDER *folder;

	if ( !input || !*input ) return (FOLDER *)0;

	/* See: FOLDER_SELECT_COLUMNS */
	/* -------------------------- */
	piece( folder_name, SQL_DELIMITER, input, 0 );
	folder = folder_new( strdup( folder_name ) );

	piece( form, SQL_DELIMITER, input, 1 );
	folder->folder_form = strdup( form );

	piece( insert_rows_number, SQL_DELIMITER, input, 2 );
	folder->insert_rows_number = atoi( insert_rows_number );

	piece( lookup_email_output_yn, SQL_DELIMITER, input, 3 );
	folder->lookup_email_output = ( *lookup_email_output_yn == 'y' );

	piece( notepad, SQL_DELIMITER, input, 4 );
	folder->notepad = strdup( notepad );

	if ( fetch_process )
	{
		piece( populate_drop_down_process, SQL_DELIMITER, input, 5 );

		folder->populate_drop_down_process =
			/* -------------------- */
			/* Does a PROCESS fetch */
			/* -------------------- */
			process_new(
				environment_application(),
				strdup( populate_drop_down_process ),
				0 /* not check_executable_inside_filesystem */);

		piece( post_change_process, SQL_DELIMITER, input, 6 );

		folder->post_change_process =
			process_new(
				environment_application(),
				strdup( post_change_process ),
				1 /* check_executable_inside_filesystem */ );
	}

	piece( html_help_file_anchor, SQL_DELIMITER, input, 7 );
	folder->html_help_file_anchor = strdup( html_help_file_anchor );

	piece( post_change_javascript, SQL_DELIMITER, input, 8 );
	folder->post_change_javascript = strdup( post_change_javascript );

	piece( subschema, SQL_DELIMITER, input, 9 );
	folder->subschema = strdup( subschema );

	piece( exclude_application_export_yn, SQL_DELIMITER, input, 10 );
	folder->exclude_application_export =
		( *exclude_application_export_yn == 'y' );

	piece( lookup_before_drop_down_yn, SQL_DELIMITER, input, 11 );
	folder->lookup_before_drop_down =
		( *lookup_before_drop_down_yn == 'y' );

	piece( no_initial_capital_yn, SQL_DELIMITER, input, 12 );
	folder->no_initial_capital =
		( *no_initial_capital_yn == 'y' );

	piece( index_directory, SQL_DELIMITER, input, 13 );
	folder->index_directory = strdup( index_directory );

	piece( data_directory, SQL_DELIMITER, input, 14 );
	folder->data_directory = strdup( data_directory );

	piece( create_view_statement, SQL_DELIMITER, input, 15 );
	folder->create_view_statement = strdup( create_view_statement );

	piece( appaserver_yn, SQL_DELIMITER, input, 16 );
	folder->appaserver = ( *appaserver_yn == 'y' );

	if ( fetch_folder_attribute_list )
	{
		folder->role_exclude_attribute_name_list =
				role_exclude_attribute_name_list;

		folder->folder_attribute_list =
			folder_attribute_list(
				folder->sql_injection_escape_folder_name,
				role_exclude_attribute_name_list );
	}

	if ( fetch_role_folder_list )
	{
		folder->sql_injection_escape_role_name =
			sql_injection_escape_role_name;

		folder->role_folder_list =
			role_folder_list(
				folder->sql_injection_escape_role_name,
				folder->sql_injection_escape_folder_name );
	}

	if ( fetch_row_level_restriction )
	{
		folder->folder_row_level_restriction_string =
			folder_row_level_restriction_string(
				folder->sql_injection_escape_folder_name );
	}

	return folder;
}

long int folder_row_count(
			char *folder_table_name )
{
	char system_string[ 1024 ];
	char *results;

	sprintf(system_string,
		"echo \"select count(1) from %s;\" | sql_timeout.sh 1",
		folder_table_name );

	if ( ( results = string_pipe_fetch( system_string ) ) )
		return atol( results );
	else
		return 0;
}

char *folder_table_name(
			char *application_name,
			char *folder_name )
{
	char table_name[ 128 ];

	if ( !application_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: application_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( strcmp( folder_name, "application" ) == 0 )
	{
		sprintf(table_name,
			"%s_application",
			application_name );
	}
	else
	{
		strcpy( table_name, folder_name );
	}

	return strdup( table_name );
}

FOLDER *folder_fetch(	char *sql_injection_escape_folder_name,
			char *sql_injection_escape_role_name,
			LIST *role_exclude_attribute_name_list,
			boolean fetch_folder_attribute_list,
			boolean fetch_process,
			boolean fetch_role_folder_list,
			boolean fetch_row_level_restriction )
{
	return
	folder_parse(
		string_pipe_fetch(
			folder_system_string(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				folder_primary_where(
					sql_injection_escape_folder_name ) ) ),
		sql_injection_escape_role_name,
		role_exclude_attribute_name_list,
		fetch_folder_attribute_list,
		fetch_process,
		fetch_role_folder_list,
		fetch_row_level_restriction );
}

char *folder_row_level_restriction_string( char *folder_name )
{
	char system_string[ 1024 ];
	char *select = "row_level_restriction";

	sprintf(system_string,
		"select.sh %s %s \"%s\"",
		select,
		FOLDER_ROW_LEVEL_RESTRICTION_TABLE,
		folder_primary_where( folder_name ) );

	return string_fetch_pipe( system_string );
}

