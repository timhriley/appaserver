/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/execute_select.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "sql.h"
#include "token.h"
#include "timlib.h"
#include "html.h"
#include "role.h"
#include "role_folder.h"
#include "appaserver.h"
#include "application.h"
#include "folder_attribute.h"
#include "application_clone.h"
#include "security.h"
#include "appaserver_error.h"
#include "execute_select.h"

EXECUTE_SELECT *execute_select_new(
		char *application_name,
		char *session_key,
		char *role_name,
		char *select_statement_title,
		char *statement,
		char *data_directory )
{
	EXECUTE_SELECT *execute_select;

	if ( !application_name
	||   !session_key
	||   !role_name
	||   !select_statement_title
	||   !statement
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

	execute_select = execute_select_calloc();

	execute_select->statement = statement;

	if ( *select_statement_title
	&&   strcmp(	select_statement_title,
			"select_statement_title" ) != 0 )
	{
		if ( ! ( execute_select->select_statement =
				select_statement_fetch(
					select_statement_title ) ) )
		{
			char message[ 128 ];

			sprintf(message,
			"select_statement_fetch( %s ) returned empty.",
				select_statement_title );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		execute_select->statement =
			execute_select->
				select_statement->
				statement;
	}
	else
	{
		execute_select->statement =
			/* ---------------------------- */
			/* Returns heap memory or datum */
			/* ---------------------------- */
			security_sql_injection_unescape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				execute_select->statement );

		execute_select->statement =
			/* ---------------------------- */
			/* Returns heap memory or datum */
			/* ---------------------------- */
			security_remove_semicolon(
				execute_select->statement );
	}

	if ( !*execute_select->statement
	||   strcmp( execute_select->statement, "statement" ) == 0 )
	{
		execute_select->error_message =
			"Please choose a select statement";

		return execute_select;
	}

	execute_select->role =
		role_fetch(
			role_name,
			1 /* fetch_attribute_exclude_list */ );

	if ( !execute_select->role )
	{
		char message[ 128 ];

		sprintf(message,
			"role_fetch(%s) returned empty.",
			role_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	execute_select->column_name_list =
		execute_select_column_name_list(
			&execute_select->error_message,
			role_name,
			execute_select->role->role_attribute_exclude_list,
			execute_select->statement );

	if ( execute_select->error_message
	||  !list_length( execute_select->column_name_list ) )
	{
		return execute_select;
	}

	execute_select->folder_name_list =
		execute_select_folder_name_list(
			&execute_select->error_message,
			role_name,
			execute_select->statement );

	if ( execute_select->error_message
	||  !list_length( execute_select->folder_name_list ) )
	{
		return execute_select;
	}

	execute_select->first_column_asterisk_boolean =
		execute_select_first_column_asterisk_boolean(
			list_first( execute_select->column_name_list )
				/* first_column_name */ );

	if ( execute_select->first_column_asterisk_boolean )
	{
		execute_select->column_name_list =
			execute_select_asterisk_name_list(
				list_first(
					execute_select->folder_name_list
						/* folder_name */ ) );

		if ( !list_length( execute_select->column_name_list ) )
		{
			char message[ 128 ];

			sprintf(message,
			"execute_select_asterisk_name_list(%s) returned empty.",
				(char *)list_first(
					execute_select->
						folder_name_list ) );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}
	}

	execute_select->html_heading_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		execute_select_html_heading_string(
			execute_select->column_name_list );

	execute_select->html_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		execute_select_html_system_string(
			SQL_DELIMITER,
			HTML_TABLE_QUEUE_TOP_BOTTOM,
			execute_select->html_heading_string );

	execute_select->appaserver_link =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_link_new(
			application_http_prefix(
				application_ssl_support_boolean(
					application_name ) ),
			application_server_address(),
			data_directory,
			"execute_select" /* filename_stem */,
			application_name,
			0 /* process_id */,
			session_key,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"csv" /* extension */ );

	execute_select->prompt_filename =
		execute_select->
			appaserver_link->
			appaserver_link_prompt->
			filename;

	execute_select->output_filename =
		execute_select->
			appaserver_link->
			appaserver_link_output->
			filename;

	execute_select->spreadsheet_heading_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		execute_select_spreadsheet_heading_string(
			SQL_DELIMITER,
			execute_select->column_name_list );

	execute_select->spreadsheet_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		execute_select_spreadsheet_system_string(
			SQL_DELIMITER,
			execute_select->output_filename );

	execute_select->error_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		execute_select_error_filename(
			session_key );

	execute_select->input_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		execute_select_input_system_string(
			SQL_DELIMITER,
			execute_select->statement,
			execute_select->error_filename );

	execute_select->appaserver_link_anchor_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_anchor_html(
			execute_select->prompt_filename,
			"_new" /* target_window */,
			"Link to spreadsheet" /* prompt_message */ );

	return execute_select;
}

EXECUTE_SELECT *execute_select_calloc( void )
{
	EXECUTE_SELECT *execute_select;

	if ( ! ( execute_select =
			calloc( 1,
				sizeof ( EXECUTE_SELECT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return execute_select;
}

boolean execute_select_first_column_asterisk_boolean(
		char *first_column_name )
{
	if ( !first_column_name )
	{
		char message[ 128 ];

		sprintf(message, "first_column_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return ( strcmp( first_column_name, "*" ) == 0 );
}

int execute_select_generate(
		char *html_system_string,
		char *output_filename,
		char *spreadsheet_heading_string,
		char *spreadsheet_system_string,
		char *error_filename,
		char *input_system_string )
{
	FILE *html_output_pipe;
	FILE *spreadsheet_output_pipe;
	FILE *input_pipe;
	char input[ 65536 ];
	char *file_fetch;
	int row_count = 0;

	if ( !html_system_string
	||   !output_filename
	||   !spreadsheet_heading_string
	||   !spreadsheet_system_string
	||   !error_filename
	||   !input_system_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	html_output_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		execute_select_html_output_pipe(
			html_system_string );

	timlib_zap_file( output_filename );

	spreadsheet_output_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		execute_select_spreadsheet_output_pipe(
			spreadsheet_system_string );

	fprintf(spreadsheet_output_pipe,
		"%s\n",
		spreadsheet_heading_string );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		execute_select_input_pipe(
			input_system_string );

	while ( string_input( input, input_pipe, 65536 ) )
	{
		fprintf(html_output_pipe,
			"%s\n",
			input );

		fprintf(spreadsheet_output_pipe,
			"%s\n",
			input );

		row_count++;
	}

	pclose( input_pipe );
	pclose( spreadsheet_output_pipe );
	pclose( html_output_pipe );

	file_fetch =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_file_fetch(
			error_filename,
			(char *)0 /* delimiter */ );

	if ( file_fetch )
	{
		printf(	"<h3>%s</h3>\n",
			file_fetch );
	}

	timlib_remove_file( error_filename );

	return row_count;
}

FILE *execute_select_html_output_pipe( char *system_string )
{
	if ( !system_string )
	{
		char message[ 128 ];

		sprintf(message, "system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* Safely returns */
	/* -------------- */
	return popen( system_string, "w" );
}

FILE *execute_select_spreadsheet_output_pipe( char *system_string )
{
	if ( !system_string )
	{
		char message[ 128 ];

		sprintf(message, "system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* Safely returns */
	/* -------------- */
	return popen( system_string, "w" );
}

FILE *execute_select_input_pipe( char *system_string )
{
	if ( !system_string )
	{
		char message[ 128 ];

		sprintf(message, "system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* Safely returns */
	/* -------------- */
	return popen( system_string, "r" );
}

char *execute_select_html_system_string(
		char sql_delimiter,
		int html_table_queue_top_bottom,
		char *html_heading_string )
{
	char system_string[ 1024 ];

	if ( !sql_delimiter
	||   !html_table_queue_top_bottom
	||   !html_heading_string )
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
		"queue_top_bottom_lines.e %d |"
		"html_table.e '' \"%s\" '%c'  ",
		html_table_queue_top_bottom,
		html_heading_string,
		sql_delimiter );

	return strdup( system_string );
}

char *execute_select_spreadsheet_system_string(
		char sql_delimiter,
		char *output_filename )
{
	char system_string[ 1024 ];

	if ( !sql_delimiter
	||   !output_filename )
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
		"double_quote_comma_delimited.e '%c' >> %s",
		sql_delimiter,
		output_filename );

	return strdup( system_string );
}

char *execute_select_input_system_string(
		const char sql_delimiter,
		char *statement,
		char *error_filename )
{
	char system_string[ 65536 ];

	if ( !statement
	||   !error_filename )
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
		"echo \"%s\"   |"
		"grep -v '^#'  |"
		"sql.e '%c' 2>%s",
		statement,
		sql_delimiter,
		error_filename );

	return strdup( system_string );
}

char *execute_select_html_heading_string( LIST *column_name_list )
{
	char heading_string[ 512 ];

	if ( !list_length( column_name_list ) ) return (char *)0;

	strcpy(
		heading_string,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_display_delimited(
			column_name_list, '%' ) );

	string_search_replace(
		heading_string /* source_destination */,
		"," /* search_character */,
		"\\," /* replace_character */ );

	string_search_replace_character(
		heading_string /* source_destination */,
		'%' /* search_character */,
		',' /* replace_character */ );

	return strdup( heading_string );
}

LIST *execute_select_column_name_list(
			char **error_message,
			char *role_name,
			LIST *role_attribute_exclude_list,
			char *statement )
{
	int state = EXECUTE_BEGIN_STATE;
	char column_name[ 256 ];
	int parenthesis_nest = 0;
	char *token_ptr;
	LIST *column_name_list;

	if ( !error_message
	||   !role_name
	||   !statement )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	column_name_list = list_new();

	*column_name = '\0';

	/* Returns static memory or null */
	/* ----------------------------- */
	while ( ( token_ptr = token_get( &statement ) ) )
	{
		if ( *token_ptr == ';' )
		{
			break;
		}
		else
		if ( state == EXECUTE_BEGIN_STATE
		&&   strcasecmp( token_ptr, "select" ) != 0 )
		{
			*error_message = "Not a select statement";
			return (LIST *)0;
		}
		else
		if ( state == EXECUTE_BEGIN_STATE )
		{
			state = EXECUTE_SELECT_STATE;
		}
		else
		if (	strcasecmp( token_ptr, "from" ) == 0
		||	strcasecmp( token_ptr, "where" ) == 0
		||	strcasecmp( token_ptr, "order" ) == 0
		||	strcasecmp( token_ptr, "group" ) == 0 )
		{
			list_set(
				column_name_list,
				strdup( column_name ) );

			break;
		}
		else
		if ( *token_ptr == '(' )
		{
			strcat( column_name, token_ptr );
			parenthesis_nest++;
		}
		else
		if ( *token_ptr == ')' )
		{
			if ( !parenthesis_nest )
			{
				*error_message = "Mismatched ')'";
				return (LIST *)0;
			}

			strcat( column_name, token_ptr );
			parenthesis_nest--;
		}
		else
		if ( *token_ptr == ',' && !parenthesis_nest )
		{
			list_set(
				column_name_list,
				strdup( column_name ) );

			*column_name = '\0';
		}
		else
		{
			boolean exists_attribute_exclude =
				execute_select_exists_attribute_exclude(
					role_attribute_exclude_list,
					token_ptr /* attribute_name */ );

			if ( exists_attribute_exclude )
			{
				char *violation =
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					execute_select_exclude_violation(
					    role_name,
					    token_ptr /* attribute_name */ );

				*error_message = violation;

				return (LIST *)0;
			}

			strcat( column_name, token_ptr );
		}
	}

	if ( state == EXECUTE_BEGIN_STATE )
		return (LIST *)0;
	else
		return column_name_list;
}

LIST *execute_select_asterisk_name_list( char *folder_name )
{
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

	return
	folder_attribute_name_list(
		(char *)0 /* folder_name */,
		folder_attribute_list(
			folder_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			0 /* not fetch_attribute */,
			0 /* not cache_boolean */ ) );
}

char *execute_select_spreadsheet_heading_string(
		const char sql_delimiter,
		LIST *column_name_list )
{
	return
	/* ------------------------- */
	/* Returns heap memory or "" */
	/* ------------------------- */
	list_display_delimited(
		column_name_list,
		(char )sql_delimiter );
}

LIST *execute_select_folder_name_list(
		char **error_message,
		char *role_name,
		char *statement )
{
	int state = EXECUTE_BEGIN_STATE;
	char *token_ptr;
	LIST *folder_name_list;

	if ( !error_message
	||   !role_name
	||   !statement )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	folder_name_list = list_new();

	/* Returns static memory or null */
	/* ----------------------------- */
	while ( ( token_ptr = token_get( &statement ) ) )
	{
		if ( *token_ptr == ';' )
		{
			break;
		}
		else
		if ( strcasecmp( token_ptr, "from" ) == 0 )
		{
			state = EXECUTE_FROM_STATE;
		}
		else
		if ( state == EXECUTE_FROM_STATE )
		{
			boolean exists_role_folder;

			if (	strcasecmp( token_ptr, "where" ) == 0
			||	strcasecmp( token_ptr, "order" ) == 0
			||	strcasecmp( token_ptr, "group" ) == 0 )
			{
				break;
			}

			if ( *token_ptr == ',' ) continue;

			exists_role_folder =
				execute_select_exists_role_folder(
					ROLE_SYSTEM,
					role_name,
					token_ptr /* folder_name */ );

			if ( !exists_role_folder )
			{
				char *violation;

				violation =
				    /* --------------------- */
				    /* Returns static memory */
				    /* --------------------- */
				    execute_select_not_exists_role_violation(
					role_name,
					token_ptr /* folder_name */ );

				*error_message = violation;

				return (LIST *)0;
			}

			list_set(
				folder_name_list,
				strdup( token_ptr )
					/* folder_name */ );
		}
	}

	if ( state == EXECUTE_BEGIN_STATE )
		return (LIST *)0;
	else
		return folder_name_list;
}

boolean execute_select_exists_attribute_exclude(
		LIST *role_attribute_exclude_list,
		char *attribute_name )
{
	boolean exclude_exists_permission;

	if ( !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( role_attribute_exclude_list ) )
		return 0;

	exclude_exists_permission =
		role_attribute_exclude_exists_permission(
			APPASERVER_LOOKUP_STATE,
			attribute_name,
			role_attribute_exclude_list );

	if ( exclude_exists_permission ) return 1;

	exclude_exists_permission =
		role_attribute_exclude_exists_permission(
			APPASERVER_UPDATE_STATE,
			attribute_name,
			role_attribute_exclude_list );

	if ( exclude_exists_permission ) return 1;

	return 0;
}

char *execute_select_exclude_violation(
		char *role_name,
		char *attribute_name )
{
	static char violation[ 256 ];

	if ( !role_name
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

	sprintf(violation,
		"%s: %s, logged in as role: %s",
		EXECUTE_SELECT_ATTRIBUTE_VIOLATION,
		attribute_name,
		role_name );


	return violation;
}

boolean execute_select_exists_role_folder(
		const char *role_system,
		char *role_name,
		char *folder_name )
{
	static LIST *lookup_name_list = {0};

	if ( !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strcmp(
		role_name,
		role_system ) == 0 )
	{
		return 1;
	}

	if ( !lookup_name_list )
	{
		lookup_name_list =
			role_folder_name_list(
				role_folder_lookup_list(
					role_name ) );
	}

	return
	list_exists_string(
		folder_name,
		lookup_name_list );
}

char *execute_select_not_exists_role_violation(
		char *role_name,
		char *folder_name )
{
	static char violation[ 256 ];

	if ( !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(violation,
		"%s: %s, logged in as role: %s",
		EXECUTE_SELECT_FOLDER_VIOLATION,
		folder_name,
		role_name );

	return violation;
}

char *execute_select_error_filename( char *session_key )
{
	static char error_filename[ 128 ];

	if ( !session_key )
	{
		char message[ 128 ];

		sprintf(message, "session_key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		error_filename,
		sizeof ( error_filename ),
		"/tmp/error_%s",
		session_key );

	return error_filename;
}

