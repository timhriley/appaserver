/* $APPASERVER_HOME/library/application.c				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver APPLICATION ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "application.h"
#include "piece.h"
#include "timlib.h"
#include "environ.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"

static APPLICATION *global_application = {0};
static boolean is_primary_application = -1;

APPLICATION *application_new_application( char *application_string )
{
	char sys_string[ 1024 ];
	char piece_buffer[ 1024 ];
	char *input_string;
	APPLICATION *application;

	sprintf(	sys_string, 
			"%s/src_appaserver/application_record.sh %s '%c'",
			appaserver_parameter_file_get_appaserver_mount_point(),
			application_string,
			APPLICATION_RECORD_DELIMITER );

	input_string = pipe2string( sys_string );

	if ( !input_string )
	{
		char msg[ 1024 ];

		sprintf( msg,
"Error in %s/%s()/%d: cannot get application record from application_record.sh for application = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 application_string );

		appaserver_output_error_message(
			application_string,
			msg,
			(char *)0 /* login_name */ );

		exit( 1 );
		/* return (APPLICATION *)0; */
	}

	if ( character_count(	APPLICATION_RECORD_DELIMITER,
				input_string ) != 16 )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: not 16 delimiter=[%c] from application_record.sh: [%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 APPLICATION_RECORD_DELIMITER,
			input_string );
		exit( 1 );
	}

	application = (APPLICATION *)calloc( 1, sizeof( APPLICATION ) );

	application->application_name = application_string;

	piece( piece_buffer, APPLICATION_RECORD_DELIMITER, input_string, 0 );
	application->application_title = strdup( piece_buffer );

/* Retired.
	piece( piece_buffer, APPLICATION_RECORD_DELIMITER, input_string, 1 );
	application->only_one_primary_yn = *piece_buffer;
*/

	piece( piece_buffer, APPLICATION_RECORD_DELIMITER, input_string, 2 );
	application->relative_source_directory = strdup( piece_buffer );

	piece( piece_buffer, APPLICATION_RECORD_DELIMITER, input_string, 3 );
	application->background_color = strdup( piece_buffer );

	piece( piece_buffer, APPLICATION_RECORD_DELIMITER, input_string, 4 );
	application->distill_directory = strdup( piece_buffer );

	piece( piece_buffer, APPLICATION_RECORD_DELIMITER, input_string, 5 );
	application->ghost_script_directory = strdup( piece_buffer );

	piece( piece_buffer, APPLICATION_RECORD_DELIMITER, input_string, 6 );
	application->grace_home_directory = strdup( piece_buffer );

	piece( piece_buffer, APPLICATION_RECORD_DELIMITER, input_string, 7 );
	application->grace_execution_directory = strdup( piece_buffer );

	piece( piece_buffer, APPLICATION_RECORD_DELIMITER, input_string, 8 );
	application->grace_free_option_yn = *piece_buffer;

	piece( piece_buffer, APPLICATION_RECORD_DELIMITER, input_string, 9 );
	application->grace_output = strdup( piece_buffer );

	piece( piece_buffer, APPLICATION_RECORD_DELIMITER, input_string, 10 );
	application->frameset_menu_horizontal_yn = *piece_buffer;

	piece( piece_buffer, APPLICATION_RECORD_DELIMITER, input_string, 11 );
	application->ssl_support_yn = *piece_buffer;

	piece( piece_buffer, APPLICATION_RECORD_DELIMITER, input_string, 12 );
	application->prepend_http_protocol_yn = *piece_buffer;

	piece( piece_buffer, APPLICATION_RECORD_DELIMITER, input_string, 13 );
	application->max_query_rows_for_drop_downs = atoi( piece_buffer );

	piece( piece_buffer, APPLICATION_RECORD_DELIMITER, input_string, 14 );
	application->max_drop_down_size = atoi( piece_buffer );

	piece( piece_buffer, APPLICATION_RECORD_DELIMITER, input_string, 15 );
	application->chart_email_command_line = strdup( piece_buffer );

	piece( piece_buffer, APPLICATION_RECORD_DELIMITER, input_string, 16 );
	application->appaserver_version = strdup( piece_buffer );

	return application;

}

char *application_title( char *application_string )
{
	return application_title_string( application_string );
}

char *application_title_string( char *application_string )
{
	if ( !global_application )
	{
		global_application =
			application_new_application( application_string );
	}

	if ( !global_application )
		return "";
	else
		return global_application->application_title;

}

char *application_first_relative_source_directory(
			char *application_string )
{
	static char first_relative_source_directory[ 128 ];
	char *relative_source_directory;
	char delimiter;

	relative_source_directory =
		application_relative_source_directory(
			application_string );

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

char *application_relative_source_directory( char *application_string )
{
	if ( !global_application )
	{
		global_application =
			application_new_application( application_string );
	}

	if ( !global_application )
		return "";
	else
		return global_application->relative_source_directory;

}

char *application_background_color(char *application_string )
{
	if ( !global_application )
	{
		global_application =
			application_new_application( application_string );
	}

	if ( !global_application )
		return "";
	else
		return global_application->background_color;

}

char *application_distill_directory(char *application_string )
{
	if ( !global_application )
	{
		global_application =
			application_new_application( application_string );
	}

	if ( !global_application )
		return "";
	else
		return global_application->distill_directory;

}

char *application_ghost_script_directory(char *application_string )
{
	if ( !global_application )
	{
		global_application =
			application_new_application( application_string );
	}

	if ( !global_application )
		return "";
	else
		return global_application->ghost_script_directory;

}

boolean application_is_primary_application( char *application_name )
{
/* Stub */
/* ---- */
application_name = "";
	return 1;
}

char *application_grace_home_directory( char *application_string )
{
	if ( !global_application )
	{
		global_application =
			application_new_application( application_string );
	}

	if ( !global_application )
		return "";
	else
		return global_application->grace_home_directory;

}

char *application_grace_execution_directory( char *application_string )
{
	if ( !global_application )
	{
		global_application =
			application_new_application( application_string );
	}

	if ( !global_application )
		return "";
	else
		return global_application->grace_execution_directory;

}

char *application_chart_email_command_line( char *application_string )
{
	if ( !global_application )
	{
		global_application =
			application_new_application( application_string );
	}

	if ( !global_application )
		return "";
	else
		return global_application->chart_email_command_line;
}

char application_grace_free_option_yn( char *application_string )
{
	if ( !global_application )
	{
		global_application =
			application_new_application( application_string );
	}

	if ( !global_application )
		return 'n';
	else
		return global_application->grace_free_option_yn;

}

char *application_grace_output( char *application_string )
{
	if ( !global_application )
	{
		global_application =
			application_new_application( application_string );
	}

	if ( !global_application )
		return "";
	else
		return global_application->grace_output;
}

char application_frameset_menu_horizontal_yn( char *application_string )
{
	if ( !global_application )
	{
		global_application =
			application_new_application( application_string );
	}

	if ( !global_application )
		return 'n';
	else
		return global_application->frameset_menu_horizontal_yn;

}

char application_ssl_support_yn( char *application_string )
{
	if ( !global_application )
	{
		global_application =
			application_new_application( application_string );
	}

	if ( !global_application )
		return 'n';
	else
	{
		return ( (global_application->ssl_support_yn) 		?
				global_application->ssl_support_yn	:
				'n' );
	}
}

char application_prepend_http_protocol_yn( char *application_string )
{
	if ( !global_application )
	{
		global_application =
			application_new_application( application_string );
	}

	if ( !global_application )
		return 'n';
	else
	{
		return (
			(global_application->prepend_http_protocol_yn)	?
			global_application->prepend_http_protocol_yn	:
			'n' );
	}
}

int application_max_drop_down_size( char *application_string )
{
	if ( !global_application )
	{
		global_application =
			application_new_application( application_string );
	}

	if ( !global_application )
		return 0;
	else
	{
		return global_application->max_drop_down_size;
	}
}

int application_max_query_rows_for_drop_downs( char *application_string )
{
	if ( !global_application )
	{
		global_application =
			application_new_application( application_string );
	}

	if ( !global_application )
		return 0;
	else
	{
		return global_application->max_query_rows_for_drop_downs;
	}
}

char *application_version( char *application_string )
{
	if ( !global_application )
	{
		global_application =
			application_new_application( application_string );
	}

	if ( !global_application )
		return "0.0";
	else
	{
		return global_application->appaserver_version;
	}
}

void application_reset( void )
{
	global_application = 0;
	is_primary_application = -1;
}

char *application_http_prefix( char *application_string )
{
	char ssl_support_yn;

	if ( !application_string )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty application_string.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	ssl_support_yn = application_ssl_support_yn( application_string );

	if ( ssl_support_yn == 'y' )
		return "https";
	else
		return "http";
}

boolean application_get_is_primary_application( char *stub )
{
	if ( stub ){};
	return 1;
}
