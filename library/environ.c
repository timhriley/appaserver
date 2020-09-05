/* $APPASERVER_HOME/library/environ.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "timlib.h"
#include "piece.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "appaserver.h"
#include "application.h"
#include "environ.h"

char *resolve_environment_variables( char *return_string, char *s )
{
	char variable_name[ 128 ];
	char variable_value[ 128 ];
	char looking_string[ 1024 ];
	char looking_string_old[ 1024 ];

	*return_string = '\0';

	/* Work only with local memory */
	/* --------------------------- */
	strcpy( looking_string, s );

	while( 1 )
	{
		strcpy( looking_string_old, looking_string );

		if ( environ_variable_exists( variable_name, looking_string ) )
		{
			environ_name_to_value( variable_value, variable_name );
			search_replace(	variable_name, 
					variable_value, 
					looking_string );
		}

		/* Exit if no changes */
		/* ------------------ */
		if ( strcmp( looking_string, looking_string_old ) == 0 )
			break;
	}

	strcpy( return_string, looking_string );
	return return_string;

} /* resolve_environment_variables() */

boolean environ_variable_exists( char *variable_name, char *looking_string )
{
	int next_delimiter;

	/* Look for the '$' */
	/* ---------------- */
	while( *looking_string )
	{
		if ( *looking_string == '$' )
		{
			next_delimiter = 
				environ_next_delimiter( looking_string );

			strncpy(	variable_name, 
					looking_string, 
					next_delimiter );

			/* TRAP: strncpy does not null terminate! */
			/* -------------------------------------- */
			variable_name[ next_delimiter ] = '\0';

			return 1;
		}

		looking_string++;
	}

	*variable_name = '\0';
	return 0;

} /* environ_variable_exists() */

int environ_next_delimiter( char *looking_string )
{
	int i = 0;

	/* Skip the '$' */
	/* ------------ */
	if ( *looking_string == '$' )
		looking_string++;

	while( *looking_string )
	{
		if ( *looking_string != '_' && !isalnum( *looking_string ) )
		{

			return i + 1;
		}
		else
		{
			looking_string++;
			i++;
		}
	}

	return i + 1;

} /* environ_next_delimiter() */

char *environment_application_name( void )
{
	return environment_application();
}

char *environment_application( void )
{
	char *results;

	if ( ( results = environment_get( "DATABASE" ) ) )
	{
		return results;
	}
	else
	if ( ( results = environment_get( "APPASERVER_DATABASE" ) ) )
	{
		return results;
	}

	fprintf(stderr,
		"ERROR in %s/%s()/%d: can't get appaserver database.\n",
		__FILE__,
		__FUNCTION__,
		__LINE__ );
	exit( 1 );
}

char *environment_get( char *variable_name )
{
	char datum[ 1024 ];

	if ( environ_name_to_value( datum, variable_name ) )
		return strdup( datum );
	else
		return (char *)0;
}

char *environ_get_environment( char *variable_name )
{
	return environment_get( variable_name );
}

boolean environ_name_to_value( char *variable_value, char *variable_name )
{
	char *value;

	/* Skip the '$' */
	/* ------------ */
	if ( *variable_name == '$' )
		variable_name++;

 	value = getenv( variable_name );

	if ( value )
	{
		strcpy( variable_value, value );
		return 1;
	}
	else
	{
		*variable_value = '\0';
		return 0;
	}

} /* environ_name_to_value() */

void environ_set_environment( char *environment, char *datum )
{
	set_environment( environment, datum );
}

void set_environment( char *environment, char *datum )
{
	char statement[ 4096 ];

	sprintf( statement, "%s=%s", environment, datum );
	putenv( strdup( statement ) );

} /* set_environment() */

void environ_prepend_path( char *path_to_add )
{
	char *path_environment;
	char new_path[ 4096 ];

	path_environment = getenv( "PATH" );
	if ( !path_environment )
	{
		fprintf( stderr, 
			 "%s/%s(%s) cannot get PATH\n", 
			 __FILE__,
			 __FUNCTION__,
			 path_to_add );
		exit( 1 );
	}
	sprintf( new_path, "PATH=%s:%s", path_to_add, path_environment );

	putenv( strdup( new_path ) );

} /* environ_prepend_path() */

void set_path( char *path_to_add )
{
	char *path_environment;
	char new_path[ 4096 ];

	path_environment = getenv( "PATH" );
	if ( !path_environment )
	{
		fprintf( stderr, 
			 "%s/%s(%s) cannot get PATH\n", 
			 __FILE__,
			 __FUNCTION__,
			 path_to_add );
		exit( 1 );
	}
	sprintf( new_path, "PATH=%s:%s", path_environment, path_to_add );

	putenv( strdup( new_path ) );

} /* set_path() */

boolean environ_get_browser_internet_explorer( void )
{
	char http_user_agent[ 512 ];

	if ( !environ_name_to_value( http_user_agent, "HTTP_USER_AGENT" ) )
		return 0;

	return ( instr( "MSIE", http_user_agent, 1 ) != -1 );

} /* environ_get_browser_internet_explorer() */

char *environ_get_http_referer(	void )
{
	char http_referer_with_parameter[ 512 ];
	static char http_referer[ 512 ];

	if ( !environ_name_to_value(
				http_referer_with_parameter,
				"HTTP_REFERER" ) )
	{
		return (char *)0;
	}
	else
	{
		piece( http_referer, '?', http_referer_with_parameter, 0 );
		return http_referer;
	}

} /* environ_get_http_referer() */

void environ_set_utc_offset( char *application_name )
{
	APPLICATION_CONSTANTS *application_constants;
	char *utc_offset;

	application_constants = application_constants_new();

	application_constants->dictionary =
		application_constants_get_dictionary(
			application_name );

	if ( ( utc_offset =
		application_constants_fetch(
				application_constants->dictionary,
				"utc_offset" ) ) )
	{
		environ_set_environment(
			"UTC_OFFSET",
			utc_offset );
	}
	else
	{
		fprintf( stderr,
	"Warning in %s/%s/%d: APPLICATION_CONSTANTS=utc_offset not found.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
	}

} /* environ_set_utc_offset() */

char *environ_get_http_referer_filename( void )
{
	char http_referer_with_parameter[ 512 ];
	char http_referer_with_pathname[ 512 ];
	static char http_referer_filename[ 512 ];
	int ch_count;

	if ( !environ_name_to_value(
				http_referer_with_parameter,
				"HTTP_REFERER" ) )
	{
		return (char *)0;
	}
	else
	{
		piece(	http_referer_with_pathname,
			'?',
			http_referer_with_parameter,
			0 );

		ch_count = character_count( '/', http_referer_with_pathname );

		piece(	http_referer_filename,
			'/',
			http_referer_with_pathname,
			ch_count );

		return http_referer_filename;
	}

} /* environ_get_http_referer_filename() */

void add_standard_unix_to_path( void )
{
	add_local_bin_to_path();
	set_path( "/bin:/usr/bin:/etc" );
}

void environ_prepend_dot_to_path( void )
{
	environ_prepend_path( "." );
}

void add_dot_to_path( void )
{
	set_path( "." );
}

void add_pwd_to_path( void )
{
	set_path( pipe2string( "pwd" ) );
}

void add_etc_to_path( void )
{
	set_path( "/etc" );
}

void add_local_bin_to_path( void )
{
	set_path( "/usr/local/bin" );
}

void add_utility_to_path( void )
{
	char *appaserver_mount_point;
	char utility_path[ 128 ];

	appaserver_mount_point =
		appaserver_parameter_file_get_appaserver_mount_point();

	sprintf( utility_path, "%s/utility", appaserver_mount_point );
	set_path( utility_path );
}

void add_appaserver_home_to_environment( void )
{
	environ_set_environment(
		"APPASERVER_HOME",
		appaserver_parameter_file_get_appaserver_mount_point() );

} /* add_appaserver_home_to_environment() */

void add_src_appaserver_to_path( void )
{
	char *appaserver_mount_point;
	char bin_path[ 256 ];

	appaserver_mount_point =
		appaserver_parameter_file_get_appaserver_mount_point();

	sprintf(	bin_path,
			"%s/%s",
			appaserver_mount_point,
			APPASERVER_RELATIVE_SOURCE_DIRECTORY );

	set_path( bin_path );
}

void add_python_library_path( void )
{
	add_library_to_python_path();
}

void add_library_to_python_path( void )
{
	char *appaserver_mount_point;
	char python_library_path[ 128 ];

	appaserver_mount_point = 
		appaserver_parameter_file_get_appaserver_mount_point();
	sprintf( python_library_path,
		 "%s/library", 
		 appaserver_mount_point );
	set_environment( "PYTHONPATH", python_library_path );
}

void environ_display( FILE *output_pipe )
{
	extern char **environ;
	char **ptr;

	ptr = environ;

	while( *ptr )
	{
		fprintf( output_pipe, "%s\n", *ptr );
		ptr++;
	}

} /* environ_display() */

void add_appaserver_home_to_python_path( void )
{
	char *appaserver_mount_point;

	appaserver_mount_point = 
		appaserver_parameter_file_get_appaserver_mount_point();

	set_environment( "PYTHONPATH", appaserver_mount_point );
}

void add_path( char *path_to_add )
{
	set_path( path_to_add );
}

void add_relative_source_directory_to_path( char *application_name )
{
	char *appaserver_mount_point;
	char *relative_source_directory;
	char piece_buffer[ 128 ];
	char bin_path[ 128 ];
	int index;
	char delimiter;

	appaserver_mount_point =
		appaserver_parameter_file_get_appaserver_mount_point();

	relative_source_directory =
		application_get_relative_source_directory(
			application_name );

	if ( ! ( delimiter =
			timlib_get_delimiter(
				relative_source_directory ) ) )
	{
		/* No delimiter, so set it to anything. */
		/* ------------------------------------ */
		delimiter = ';';
	}

	for(	index = 0;
		piece(	piece_buffer,
			delimiter,
			relative_source_directory,
			index );
		index++ )
	{
		sprintf(bin_path,
			"%s/%s",
			appaserver_mount_point,
			piece_buffer );
		set_path( bin_path );
	}
} /* add_relative_source_directory_to_path() */

char *environ_exit_application_name( char *argv_0 )
{
	return environ_get_application_name( argv_0 );
}

char *environ_get_application_name( char *argv_0 )
{
	char *application_name;

	application_name =
		environ_get_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE );

	if ( !application_name )
	{
		application_name =
			environ_get_environment(
				"DATABASE" );
	}

	if ( !application_name )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get application from (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 argv_0 );

		fprintf( stderr, "Try executing . set_database\n" );

		exit( 1 );
	}

	return application_name;
}

void environ_output_application_shell( FILE *output_file )
{
	fprintf( output_file,
		 "%s",
		 environ_get_shell_snippet() );

} /* environ_output_application_shell() */

char *environ_get_shell_snippet( void )
{
	char buffer[ 1024 ];
	char *buffer_ptr = buffer;


	buffer_ptr += sprintf( buffer_ptr,
	"#!/bin/bash\n" );

	buffer_ptr += sprintf( buffer_ptr,
"if [ \"$APPASERVER_DATABASE\" != \"\" ]\n"
"then\n"
"	application=$APPASERVER_DATABASE\n"
"elif [ \"$DATABASE\" != \"\" ]\n"
"then\n"
"	application=$DATABASE\n"
"fi\n"
"\n"
"if [ \"$application\" = \"\" ]\n"
"then\n"
"	echo \"Error in `basename.e $0 n`: you must first:\" 1>&2\n"
"	echo \"$ . set_database\" 1>&2\n"
"	exit 1\n"
"fi\n" );

	return strdup( buffer );

} /* environ_get_shell_snippet() */

