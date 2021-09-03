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
#include "role_operation.h"
#include "process.h"
#include "query.h"
#include "security.h"
#include "folder.h"

FOLDER *folder_new( char *folder_name )
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

	folder->folder_name = folder_name;

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
			LIST *exclude_attribute_name_lst,
			boolean fetch_folder_attribute_list,
			boolean fetch_relation_mto1_non_isa_list,
			boolean fetch_relation_mto1_isa_list,
			boolean fetch_relation_one2m_list,
			boolean fetch_relation_one2m_recursive_list,
			boolean fetch_process,
			boolean fetch_role_folder_list,
			boolean fetch_row_level_restriction,
			boolean fetch_role_operation_list )
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
				exclude_attribute_name_lst,
				fetch_folder_attribute_list,
				fetch_relation_mto1_non_isa_list,
				fetch_relation_mto1_isa_list,
				fetch_relation_one2m_list,
				fetch_relation_one2m_recursive_list,
				fetch_process,
				fetch_role_folder_list,
				fetch_row_level_restriction,
				fetch_role_operation_list ) ) )
		{
			if ( !list ) list = list_new();

			list_set( list, folder_parse );
		}
	}

	pclose( pipe );
	return list;
}

FOLDER *folder_parse(	char *input,
			char *role_name,
			LIST *exclude_attribute_name_list,
			boolean fetch_folder_attribute_list,
			boolean fetch_relation_mto1_non_isa_list,
			boolean fetch_relation_mto1_isa_list,
			boolean fetch_relation_one2m_list,
			boolean fetch_relation_one2m_recursive_list,
			boolean fetch_process,
			boolean fetch_role_folder_list,
			boolean fetch_row_level_restriction,
			boolean fetch_role_operation_list )
{
	char folder_name[ 128 ];
	char form[ 128 ];
	char insert_rows_number[ 128 ];
	char lookup_email_output_yn[ 128 ];
	char notepad[ 65536 ];
	char populate_drop_down_process_name[ 128 ];
	char post_change_process_name[ 128 ];
	char html_help_file_anchor[ 128 ];
	char post_change_javascript[ 2048 ];
	char subschema[ 128 ];
	char exclude_application_export_yn[ 128 ];
	char drilldown_yn[ 128 ];
	char no_initial_capital_yn[ 128 ];
	char index_directory[ 128 ];
	char data_directory[ 128 ];
	char create_view_statement[ 65536 ];
	char javascript_filename[ 128 ];
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

	piece( populate_drop_down_process_name, SQL_DELIMITER, input, 5 );
	folder->populate_drop_down_process_name =
		strdup( populate_drop_down_process_name );

	piece( post_change_process_name, SQL_DELIMITER, input, 6 );
	folder->post_change_process_name =
		strdup( post_change_process_name );

	piece( html_help_file_anchor, SQL_DELIMITER, input, 7 );
	folder->html_help_file_anchor = strdup( html_help_file_anchor );

	piece( post_change_javascript, SQL_DELIMITER, input, 8 );
	folder->post_change_javascript = strdup( post_change_javascript );

	piece( subschema, SQL_DELIMITER, input, 9 );
	folder->subschema = strdup( subschema );

	piece( exclude_application_export_yn, SQL_DELIMITER, input, 10 );
	folder->exclude_application_export =
		( *exclude_application_export_yn == 'y' );

	piece( drilldown_yn, SQL_DELIMITER, input, 11 );
	folder->drilldown = ( *drilldown_yn == 'y' );

	piece( no_initial_capital_yn, SQL_DELIMITER, input, 12 );
	folder->no_initial_capital =
		( *no_initial_capital_yn == 'y' );

	piece( index_directory, SQL_DELIMITER, input, 13 );
	folder->index_directory = strdup( index_directory );

	piece( data_directory, SQL_DELIMITER, input, 14 );
	folder->data_directory = strdup( data_directory );

	piece( create_view_statement, SQL_DELIMITER, input, 15 );
	folder->create_view_statement = strdup( create_view_statement );

	piece( javascript_filename, SQL_DELIMITER, input, 16 );
	folder->javascript_filename = strdup( javascript_filename );

	if ( fetch_folder_attribute_list )
	{
		folder->folder_attribute_list =
			folder_attribute_list(
				folder->folder_name,
				exclude_attribute_name_list );

		folder->primary_key_list =
			folder_attribute_primary_key_list(
				folder->folder_attribute_list );
	}

	if ( fetch_relation_mto1_non_isa_list )
	{
		folder->relation_mto1_non_isa_list =
			relation_mto1_non_isa_list(
				folder->folder_name );
	}

	if ( fetch_relation_mto1_isa_list )
	{
		folder->relation_mto1_isa_list =
			relation_mto1_isa_list(
				(LIST *)0 /* mto1_isa_list */,
				folder->folder_name );

		if ( list_length( folder->folder_attribute_list ) )
		{
			folder->folder_attribute_append_isa_list =
				folder_attribute_append_isa_list(
					list_copy(
						folder->
							folder_attribute_list ),
					folder->relation_mto1_isa_list );
		}
	}

	if ( fetch_relation_one2m_list )
	{
		folder->relation_one2m_list =
			relation_one2m_list(
				folder->folder_name );

	}

	if ( fetch_relation_one2m_recursive_list )
	{
		folder->relation_one2m_recursive_list =
			relation_one2m_recursive_list(
				(LIST *)0 /* one2m_recursive_list */,
				folder->folder_name );
	}

	if ( fetch_process )
	{
		folder->populate_drop_down_process =
			process_fetch(
				folder->populate_drop_down_process_name,
				(char *)0 /* document_root_directory */,
				(char *)0 /* relative_source_directory */,
				0 /* not check_executable_inside_filesystem */);

		folder->post_change_process =
			process_fetch(
				folder->post_change_process_name,
				(char *)0 /* document_root_directory */,
				(char *)0 /* relative_source_directory */,
				0 /* not check_executable_inside_filesystem */);
	}

	if ( fetch_role_folder_list && role_name )
	{
		folder->role_name = role_name;

		folder->role_folder_list =
			role_folder_fetch_list(
				folder->role_name,
				folder->folder_name );
	}

	if ( fetch_row_level_restriction )
	{
		folder->row_level_restriction_string =
			folder_row_level_restriction_string(
				folder->folder_name );

		folder->non_owner_view_only =
			folder_non_owner_view_only(
				folder->row_level_restriction_string );

		folder->non_owner_forbid =
			folder_non_owner_forbid(
				folder->row_level_restriction_string );
	}

	if ( fetch_role_operation_list )
	{
		folder->role_operation_list =
			role_operation_fetch_list(
				folder_name,
				role_name );
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

FOLDER *folder_quick_fetch( char *folder_name )
{
	return
	folder_fetch(
		folder_name,
		(char *)0 /* role_name */,
		(LIST *)0 /* exclude_attribute_name_list */,
		0 /* not fetch_folder_attribute_list */,
		0 /* not fetch_relation_mto1_non_isa_list */,
		0 /* not fetch_relation_mto1_isa_list */,
		0 /* not fetch_relation_one2m_list */,
		0 /* not fetch_relation_one2m_recursive_list */,
		0 /* not fetch_process */,
		0 /* not fetch_role_folder_list */,
		0 /* not fetch_row_level_restriction */,
		0 /* not fetch_role_operation_list */ );
}

FOLDER *folder_fetch(	char *sql_injection_escape_folder_name,
			char *sql_injection_escape_role_name,
			LIST *exclude_attribute_name_list,
			boolean fetch_folder_attribute_list,
			boolean fetch_relation_mto1_non_isa_list,
			boolean fetch_relation_mto1_isa_list,
			boolean fetch_relation_one2m_list,
			boolean fetch_relation_one2m_recursive_list,
			boolean fetch_process,
			boolean fetch_role_folder_list,
			boolean fetch_row_level_restriction,
			boolean fetch_role_operation_list )
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
		exclude_attribute_name_list,
		fetch_folder_attribute_list,
		fetch_relation_mto1_non_isa_list,
		fetch_relation_mto1_isa_list,
		fetch_relation_one2m_list,
		fetch_relation_one2m_recursive_list,
		fetch_process,
		fetch_role_folder_list,
		fetch_row_level_restriction,
		fetch_role_operation_list );
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

LIST *folder_delimited_fetch(
			char *table_name,
			LIST *attribute_name_list,
			char *where )
{
	char system_string[ STRING_SYSTEM_BUFFER ];

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		list_display_delimited(
			attribute_name_list,
			',' ),
		table_name,
		where );

	return list_pipe_fetch( system_string );
}

LIST *folder_delimited_list(
			char *table_name,
			LIST *attribute_name_list,
			char *where_clause )
{
	char system_string[ STRING_SYSTEM_BUFFER ];

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		list_display_delimited(
			attribute_name_list,
			',' ),
		table_name,
		where_clause );

	return list_pipe_fetch( system_string );
}

char *folder_delimited(
			char *table_name,
			LIST *primary_key_list,
			char *primary_where_clause )
{
	char system_string[ STRING_SYSTEM_BUFFER ];

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		list_display_delimited(
			primary_key_list,
			',' ),
		table_name,
		primary_where_clause );

	return string_pipe_fetch( system_string );
}

#ifdef NOT_DEFINED
FOLDER *folder_drop_down_delimited_fetch(
			char *folder_name,
			char *role_name,
			char *login_name,
			char *one2m_folder_name,
			char *state,
			char *populate_drop_down_process_name,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *working_post_dictionary )
{
	FOLDER *folder;
	ROLE *role;
	SECURITY_ENTITY *security_entity;

	if ( ! (folder =
		   folder_parse(
			string_pipe_fetch(
				folder_system_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					folder_primary_where(
						folder_name ) ) ),
			/* ----------------------------- */
			/* Not fetching role_folder_list */
			/* ----------------------------- */
			(char *)0 /* role_name */,
			(LIST *)0 /*exclude_attribute_name_list */,
			/* -------------------------- */
			/* Also sets primary_key_list */
			/* -------------------------- */
			1 /* fetch_folder_attribute_list */,
			0 /* not fetch_relation_mto1_non_isa_list */,
			0 /* not fetch_relation_mto1_isa_list */,
			0 /* not fetch_relation_one2m_list */,
			0 /* not fetch_relation_one2m_recursive_list */,
			1 /* fetch_process */,
			0 /* not fetch_role_folder_list */,
			1 /* fetch_row_level_restriction */ ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: folder_process_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			folder_name );
		exit( 1 );
	}

	role =
		role_fetch(
			role_name,
			0 /* not fetch_role_attribute_exclude_list */ );

	if ( !role )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: role_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			role_name );
		exit( 1 );
	}

	security_entity =
		/* Always returns */
		/* -------------- */
		security_entity_new(
			login_name,
			folder->non_owner_forbid,
			role->override_row_restrictions );

	if ( populate_drop_down_process_name
	&&   *populate_drop_down_process_name )
	{
		folder->populate_drop_down_process =
			process_fetch(
				populate_drop_down_process_name,
				/* ------------------------- */
				/* Don't check role security */
				/* ------------------------- */
				(char *)0 /* role_name */,
				1 /* check_executable_inside_filesystem */ );
	}

	if ( folder->populate_drop_down_process )
	{
		return folder_drop_down_delimited_list(
			folder->populate_drop_down_process,
			security_entity,
			one2m_folder_name,
			state,
			preprompt_dictionary,
			working_post_dictionary );
	}
	else
	{
		return folder_drop_down_delimited_list(
			folder->primary_key_list,
			security_entity,
			preprompt_dictionary );
	}
}
#endif

boolean folder_non_owner_view_only(
			char *row_level_restriction_string )
{
	return (string_strcmp(
			row_level_restriction_string,
			"row_level_non_owner_view_only" ) == 0 );
}

boolean folder_non_owner_forbid(
			char *row_level_restriction_string )
{
	return (string_strcmp(
			row_level_restriction_string,
			"row_level_non_owner_forbid" ) == 0 );
}

