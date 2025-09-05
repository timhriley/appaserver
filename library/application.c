/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "piece.h"
#include "String.h"
#include "sql.h"
#include "environ.h"
#include "security.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "appaserver_user.h"
#include "application.h"

static APPLICATION *global_application = {0};

APPLICATION *application_calloc( void )
{
	APPLICATION *application;

	if ( ! ( application = calloc( 1, sizeof( APPLICATION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return application;
}

APPLICATION *application_new( char *application_name )
{
	APPLICATION *application = application_calloc();

	application->application_name = application_name;

	return application;
}

char *application_title( char *application_name )
{
	return application_title_string( application_name );
}

char *application_title_string( char *application_name )
{
	if ( !global_application )
	{
		global_application =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			application_fetch(
				application_name );
	}

	return global_application->application_title;
}

char *application_first_relative_source_directory(
			char *application_name )
{
	static char first_relative_source_directory[ 128 ];
	char *relative_source_directory;
	char delimiter;

	relative_source_directory =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		application_relative_source_directory(
			application_name );

	if ( ! ( delimiter =
			string_delimiter(
				relative_source_directory ) ) )
	{
		/* No delimiter, so set it to anything. */
		/* ------------------------------------ */
		delimiter = ':';
	}

	piece(	first_relative_source_directory,
		delimiter,
		relative_source_directory,
		0 );

	return first_relative_source_directory;
}

char *application_relative_source_directory( char *application_name )
{
	if ( !global_application )
	{
		global_application =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			application_fetch(
				application_name );
	}

	if ( !global_application->relative_source_directory )
		return "";
	else
		return global_application->relative_source_directory;
}

char *application_background_color(char *application_name )
{
	if ( !global_application )
	{
		global_application =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			application_fetch(
				application_name );
	}

	return global_application->background_color;
}

char *application_grace_home_directory( char *application_name )
{
	if ( !global_application )
	{
		global_application =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			application_fetch(
				application_name );
	}

	return global_application->grace_home_directory;
}

char *application_grace_execution_directory( char *application_name )
{
	if ( !global_application )
	{
		global_application =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			application_fetch(
				application_name );
	}

	return global_application->grace_execution_directory;
}

boolean application_menu_horizontal_boolean( char *application_name )
{
	if ( !global_application )
	{
		global_application =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			application_fetch(
				application_name );
	}

	return global_application->menu_horizontal_boolean;
}

boolean application_ssl_support_boolean( char *application_name )
{
	if ( !global_application )
	{
		global_application =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			application_fetch(
				application_name );
	}

	return global_application->ssl_support_boolean;
}

int application_max_drop_down_size( char *application_name )
{
	if ( !global_application )
	{
		global_application =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			application_fetch(
				application_name );
	}

	return global_application->max_drop_down_size;
}

int application_max_query_rows_for_drop_downs( char *application_name )
{
	if ( !global_application )
	{
		global_application =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			application_fetch(
				application_name );
	}

	return global_application->max_query_rows_for_drop_downs;
}

char *application_http_prefix( boolean application_ssl_support_boolean )
{
	if ( application_ssl_support_boolean )
		return "https";
	else
		return "http";
}

char *application_primary_where( char *application_name )
{
	static char where[ 64 ];

	if ( !application_name )
	{
		char message[ 128 ];

		sprintf(message, "application_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(where,
		"application = '%s'",
		application_name );

	return where;
}

char *application_system_string(
		char *application_select,
		char *application_table_name,
		char *where )
{
	char system_string[ 65536 ];

	if ( !application_select
	||   !application_table_name
	||   !where )
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
		"select.sh \"%s\" %s \"%s\"",
		application_select,
		application_table_name,
		where );

	return strdup( system_string );
}

APPLICATION *application_fetch( char *application_name )
{
	char *primary_where;
	char *system_string;
	char *pipe_fetch;
	APPLICATION *application;

	if ( !application_name )
	{
		char message[ 128 ];

		sprintf(message, "application_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_primary_where(
			application_name );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_system_string(
			APPLICATION_SELECT,
			APPLICATION_TABLE,
			primary_where );

	pipe_fetch =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe_fetch(
			system_string );

	if ( !pipe_fetch )
	{
		char message[ 1024 ];

		sprintf(message,
			"string_pipe_fetch(%s) returned empty.",
			system_string );

		/* -------------------------------------------------- */
		/* Note: don't execute appaserver_error_stderr_exit() */
		/* -------------------------------------------------- */
		fprintf(stderr,
			"ERROR in %s/%s()/%d: %s\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		exit( 1 );
	}

	application =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		application_parse(
			pipe_fetch /* input */ );

	free( pipe_fetch );
	free( system_string );

	return application;
}

APPLICATION *application_parse( char *input )
{
	char application_name[ 128 ];
	char piece_buffer[ 128 ];
	APPLICATION *application;

	if ( !input || !*input )
	{
		char message[ 128 ];

		sprintf(message, "input is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* See APPLICATION_SELECT */
	/* ---------------------- */
	piece( application_name, SQL_DELIMITER, input, 0 );

	application =
		application_new(
			strdup( application_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	if ( *piece_buffer )
		application->application_title = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	if ( *piece_buffer )
		application->user_date_format = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	if ( *piece_buffer )
		application->relative_source_directory = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	if ( *piece_buffer )
		application->next_session_number = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	if ( *piece_buffer )
		application->next_reference_number = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	if ( *piece_buffer )
		application->background_color = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	if ( *piece_buffer )
		application->grace_home_directory = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 8 );
	if ( *piece_buffer )
		application->grace_execution_directory = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 9 );
	if ( *piece_buffer )
		application->menu_horizontal_boolean = (*piece_buffer == 'y');

	piece( piece_buffer, SQL_DELIMITER, input, 10 );
	if ( *piece_buffer )
		application->ssl_support_boolean = (*piece_buffer == 'y');

	piece( piece_buffer, SQL_DELIMITER, input, 11 );
	if ( *piece_buffer )
		application->max_query_rows_for_drop_downs =
			atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 12 );
	if ( *piece_buffer )
		application->max_drop_down_size = atoi( piece_buffer );

	return application;
}

char *application_table_name(
		char *folder_name )
{
	static char table_name[ 128 ];

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

	if ( strcmp( folder_name, "application" ) == 0 )
	{
		strcpy( table_name, "appaserver_application" );
	}
	else
	if ( strcmp( folder_name, "table" ) == 0 )
	{
		strcpy( table_name, "appaserver_table" );
	}
	else
	if ( strcmp( folder_name, "column" ) == 0 )
	{
		strcpy( table_name, "appaserver_column" );
	}
	else
	{
		string_strcpy( table_name, folder_name, 128 );
	}

	return table_name;
}

boolean application_name_invalid(
		char *security_escape_character_string,
		char *application_name )
{
	char ch;

	if (	!application_name
	||	!*application_name
	||	string_character_exists( application_name, ' ' )
	||	isdigit( *application_name ) )
	{
		return 1;
	}

	while ( ( ch = *security_escape_character_string ) )
	{
		if ( string_character_exists( 
			application_name,
			ch ) )
		{
			return 1;
		}

		security_escape_character_string++;
	}

	return 0;
}

boolean application_exists_boolean( char *application_log_filename )
{
	boolean file_exists_boolean;

	if ( !application_log_filename )
	{
		char message[ 128 ];

		sprintf(message, "application_log_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	file_exists_boolean =
		application_file_exists_boolean(
			application_log_filename );

	return file_exists_boolean;
}

char *application_server_address( void )
{
	static char *server_address = {0};

	if ( !server_address )
	{
		server_address =
			string_pipe_fetch(
				"ip_address.sh" );
	}

	return server_address;
}

char *application_http_prompt(
			char *cgi_directory,
		 	char *server_address,
			boolean ssl_support_boolean )
{
	static char http_prompt[ 512 ];

	if ( !*http_prompt )
	{
/*		strcpy( http_prompt, cgi_directory ); */

		if ( ssl_support_boolean )
		{
			sprintf(http_prompt,
		 		"https://%s%s",
		 		server_address,
		 		cgi_directory );
		}
		else
		{
			sprintf(http_prompt,
		 		"http://%s%s",
		 		server_address,
		 		cgi_directory );
		}
	}

	return http_prompt;
}

void application_output_ftp_prompt(
			char *prompt_filename,
			char *prompt,
			char *target,
			char *mime_type )
{
	char mime_type_prompt[ 128 ];

	if ( mime_type && *mime_type )
	{
		sprintf( mime_type_prompt,
			 "type=\"%s\"",
			 mime_type );
	}
	else
	{
		*mime_type_prompt = '\0';
	}

	if ( target && *target )
	{
		printf(
		"<br><a %s href=\"%s\" target=\"%s\">%s</a>\n",
			mime_type_prompt,
			prompt_filename,
			target,
			prompt );
	}
	else
	{
		printf(
		"<br><a %s href=\"%s\">%s</a>\n",
			mime_type_prompt,
			prompt_filename,
			prompt );
	}
}

LIST *application_table_name_list( void )
{
	char *system_string;
	system_string = "echo \"show tables;\" | sql.e";
	return list_pipe_fetch( system_string );
}

LIST *application_non_system_folder_name_list( void )
{
	char *system_string;
	system_string = "application_folder_list.sh n";
	return list_pipe_fetch( system_string );
}

LIST *application_name_list(
		char *application_log_extension,
		char *log_directory )
{
	char regular_expression[ 128 ];
	char system_string[ 256 ];

	sprintf(regular_expression,
		"appaserver_*\\.%s",
		application_log_extension );

	sprintf(system_string,
		"cd %s && "
		"ls -1 %s |"
		"sed 's/^appaserver_//' |"
		"piece.e '.' 0",
		log_directory,
		regular_expression );

	return list_pipe_fetch( system_string );
}

LIST *application_relative_source_directory_list(
		char *application_name )
{
	LIST *list;
	char *relative_source_directory;
	int p;
	char source_directory[ 128 ];

	if ( !application_name )
	{
		char message[ 128 ];

		sprintf(message, "application_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	relative_source_directory =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		application_relative_source_directory(
			application_name );

	if ( !*relative_source_directory ) return NULL;

	list = list_new();

	for(	p = 0;
		piece(
			source_directory,
			':',
			relative_source_directory,
			p );
		p++ )
	{
		list_set(
			list,
			strdup( source_directory ) );
	}

	return list;
}

boolean application_file_exists_boolean( char *filename )
{
	struct stat s;

	if ( !filename || !*filename ) return 0;

	if ( stat( filename, &s ) == -1 )
		return 0;
	else
		return 1;
}

LIST *application_not_null_data_list(
		char *table_name,
		char *column_name )
{
	if ( !table_name
	||   !column_name )
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
	list_pipe_fetch(
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_not_null_system(
			table_name,
			column_name ) );
}

char *application_not_null_system(
		char *table_name,
		char *column_name )
{
	static char system_string[ 256 ];

	if ( !table_name
	||   !column_name )
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
		"select.sh %s %s \"%s is not null\"",
		column_name,
		table_name,
		column_name );

	return system_string;
}

