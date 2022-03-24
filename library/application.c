/* $APPASERVER_HOME/library/application.c				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver APPLICATION ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"
#include "String.h"
#include "timlib.h"
#include "sql.h"
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
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
			application_fetch(
				application_name );
	}

	if ( !global_application )
		return "";
	else
		return global_application->application_title;
}

char *application_first_relative_source_directory(
			char *application_name )
{
	static char first_relative_source_directory[ 128 ];
	char *relative_source_directory;
	char delimiter;

	relative_source_directory =
		application_relative_source_directory(
			application_name );

	if ( ! ( delimiter =
			timlib_get_delimiter(
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
			application_fetch(
				application_name );
	}

	if ( !global_application )
		return "";
	else
		return global_application->relative_source_directory;

}

char *application_background_color(char *application_name )
{
	if ( !global_application )
	{
		global_application =
			application_fetch(
				application_name );
	}

	if ( !global_application )
		return "";
	else
		return global_application->background_color;

}

char *application_distill_directory(char *application_name )
{
	if ( !global_application )
	{
		global_application =
			application_fetch(
				application_name );
	}

	if ( !global_application )
		return "";
	else
		return global_application->distill_directory;

}

char *application_ghost_script_directory(char *application_name )
{
	if ( !global_application )
	{
		global_application =
			application_fetch(
				application_name );
	}

	if ( !global_application )
		return "";
	else
		return global_application->ghost_script_directory;

}

char *application_grace_home_directory( char *application_name )
{
	if ( !global_application )
	{
		global_application =
			application_fetch(
				application_name );
	}

	if ( !global_application )
		return "";
	else
		return global_application->grace_home_directory;

}

char *application_grace_execution_directory( char *application_name )
{
	if ( !global_application )
	{
		global_application =
			application_fetch(
				application_name );
	}

	if ( !global_application )
		return "";
	else
		return global_application->grace_execution_directory;

}

char *application_chart_email_command_line( char *application_name )
{
	if ( !global_application )
	{
		global_application =
			application_fetch(
				application_name );
	}

	if ( !global_application )
		return "";
	else
		return global_application->chart_email_command_line;
}

boolean application_grace_free_option( char *application_name )
{
	if ( !global_application )
	{
		global_application =
			application_fetch(
				application_name );
	}

	if ( !global_application )
		return 0;
	else
		return global_application->grace_free_option;

}

char *application_grace_output( char *application_name )
{
	if ( !global_application )
	{
		global_application =
			application_fetch(
				application_name );
	}

	if ( !global_application )
		return "";
	else
		return global_application->grace_output;
}

boolean application_frameset_menu_horizontal_yn( char *application_name )
{
	if ( !global_application )
	{
		global_application =
			application_fetch(
				application_name );
	}

	if ( !global_application )
		return 0;
	else
		return global_application->frameset_menu_horizontal;
}

char application_ssl_support_yn( char *application_name )
{
	if ( application_ssl_support( application_name ) )
		return 'y';
	else
		return 'n';
}

boolean application_ssl_support( char *application_name )
{
	if ( !global_application )
	{
		global_application =
			application_fetch(
				application_name );
	}

	if ( !global_application )
		return 0;
	else
	{
		return global_application->ssl_support;
	}
}

char application_prepend_http_protocol_yn(
			char *application_name )
{
	if ( application_prepend_http_protocol( application_name ) )
		return 'y';
	else
		return 'n';
}

boolean application_prepend_http_protocol( char *application_name )
{
	if ( !global_application )
	{
		global_application =
			application_fetch(
				application_name );
	}

	if ( !global_application )
		return 0;
	else
	{
		return global_application->prepend_http_protocol;
	}
}

int application_max_drop_down_size( char *application_name )
{
	if ( !global_application )
	{
		global_application =
			application_fetch(
				application_name );
	}

	if ( !global_application )
		return 0;
	else
	{
		return global_application->max_drop_down_size;
	}
}

int application_max_query_rows_for_drop_downs( char *application_name )
{
	if ( !global_application )
	{
		global_application =
			application_fetch(
				application_name );
	}

	if ( !global_application )
		return 0;
	else
	{
		return global_application->max_query_rows_for_drop_downs;
	}
}

void application_reset( void )
{
	global_application = (APPLICATION *)0;
}

char *application_http_prefix( char *application_name )
{
	if ( !application_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty application_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( application_ssl_support( application_name ) )
		return "https";
	else
		return "http";
}

char *application_primary_where(
			char *application_name )
{
	static char where[ 64 ];

	sprintf( where,
		 "application = '%s'",
		 application_name );

	return where;
}

char *application_system_string(
			char *application_select,
			char *where )
{
	char system_string[ 1024 ];

	if ( !where ) return (char *)0;

	sprintf( system_string,
		 "select.sh '%s' %s \"%s\"",
		 application_select,
		 application_table_name( "application" ),
		 where );

	return strdup( system_string );
}

APPLICATION *application_fetch( char *application_name )
{
	return application_parse(
		string_pipe_fetch(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_system_string(
				APPLICATION_SELECT,
		 		/* --------------------- */
		 		/* Returns static memory */
		 		/* --------------------- */
		 		application_primary_where(
					application_name ) ) ) );
}

APPLICATION *application_parse( char *input )
{
	char application_name[ 128 ];
	char piece_buffer[ 128 ];
	APPLICATION *application;

	if ( !input || !*input ) return (APPLICATION *)0;

	/* See APPLICATION_SELECT */
	/* ---------------------- */
	piece( application_name, SQL_DELIMITER, input, 0 );

	application =
		application_new(
			strdup( application_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	application->application_title = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	application->user_date_format = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	application->relative_source_directory = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	application->next_session_number = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	application->next_reference_number = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	application->background_color = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	application->distill_directory = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 8 );
	application->ghost_script_directory = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 9 );
	application->grace_home_directory = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 10 );
	application->grace_execution_directory = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 11 );
	application->grace_free_option = (*piece_buffer == 'y');

	piece( piece_buffer, SQL_DELIMITER, input, 12 );
	application->grace_output = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 13 );
	application->frameset_menu_horizontal = (*piece_buffer == 'y');

	piece( piece_buffer, SQL_DELIMITER, input, 14 );
	application->ssl_support = (*piece_buffer == 'y');

	piece( piece_buffer, SQL_DELIMITER, input, 15 );
	application->prepend_http_protocol = (*piece_buffer == 'y');

	piece( piece_buffer, SQL_DELIMITER, input, 16 );
	application->chart_email_command_line = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 17 );
	application->max_query_rows_for_drop_downs = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 18 );
	application->max_drop_down_size = atoi( piece_buffer );

	return application;
}

char *application_table_name(
			char *folder_name )
{
	char table_name[ 512 ];
	char *application_name = environment_application_name();

	if ( strcmp( folder_name, "application" ) == 0 )
	{
		sprintf( table_name, "%s_%s", application_name, folder_name );
	}
	else
	{
		strcpy( table_name, folder_name );
	}

	return strdup( table_name );
}

