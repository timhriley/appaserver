/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/environ.c			   		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "String.h"
#include "piece.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "application_constant.h"
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
	string_strcpy( looking_string, s, sizeof ( looking_string ) );

	while( 1 )
	{
		strcpy( looking_string_old, looking_string );

		if ( environ_variable_exists( variable_name, looking_string ) )
		{
			environ_name_to_value( variable_value, variable_name );

			string_search_replace(
				looking_string /* source_destination */,
				variable_name /* search_string */, 
				variable_value /* replace_string */ ); 
		}

		/* Exit if no changes */
		/* ------------------ */
		if ( strcmp( looking_string, looking_string_old ) == 0 )
			break;
	}

	strcpy( return_string, looking_string );

	return return_string;
}

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

}

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
}

char *environment_application_name( void )
{
	char *result;

	/* Returns heap memory or null */
	/* --------------------------- */
	if ( ( result = environment_get( "APPASERVER_DATABASE" ) ) )
	{
		return result;
	}
	else
	if ( ( result = environment_get( "DATABASE" ) ) )
	{
		return result;
	}
	else
	{
		fprintf(stderr,
"Warning in %s/%s()/%d: both APPASERVER_DATABASE and DATABASE are empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (char *)0;
	}
}

char *environment_application( void )
{
	return environment_application_name();
}

char *environment_get( char *variable_name )
{
	char datum[ 1024 ];

	if ( environ_name_to_value( datum, variable_name ) )
		return strdup( datum );
	else
		return (char *)0;
}

boolean environ_name_to_value( char *variable_value, char *variable_name )
{
	char *value;

	/* Skip the '$' */
	/* ------------ */
	if ( *variable_name == '$' ) variable_name++;

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
}

void environ_set_environment(
		char *environment,
		char *datum )
{
	environment_set( environment, datum );
}

void set_environment(	char *environment,
			char *datum )
{
	environment_set( environment, datum );
}

void environment_set(	char *environment,
			char *datum )
{
	char statement[ 4096 ];

	sprintf( statement, "%s=%s", environment, datum );
	putenv( strdup( statement ) );
}

void environment_database_set( char *database )
{
	environment_set( "DATABASE", database );
	environment_set( "APPASERVER_DATABASE", database );
}

void environment_append_path( char *directory )
{
	char *path_environment;
	char new_path[ 4096 ];

	path_environment = getenv( "PATH" );

	if ( !path_environment )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: getenv(PATH) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( new_path, "PATH=%s:%s", path_environment, directory );

	putenv( strdup( new_path ) );
}

void environment_prepend_path( char *directory )
{
	char *path_environment;
	char new_path[ 4096 ];

	path_environment = getenv( "PATH" );

	if ( !path_environment )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: getenv(PATH) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( new_path, "PATH=%s:%s", directory, path_environment );

	putenv( strdup( new_path ) );
}

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

}

boolean environ_browser_internet_explorer( void )
{
	char http_user_agent[ 512 ];

	if ( !environ_name_to_value( http_user_agent, "HTTP_USER_AGENT" ) )
		return 0;

	return ( string_instr( "MSIE", http_user_agent, 1 ) != -1 );

}

char *environ_http_referer( void )
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
}

void environ_set_utc_offset( void )
{
	APPLICATION_CONSTANT *application_constant;
	char *utc_offset;

	/* Safely returns */
	/* -------------- */
	application_constant = application_constant_new();

	if ( ( utc_offset =
		application_constant_get(
				"utc_offset",
				application_constant->dictionary ) ) )
	{
		environ_set_environment(
			"UTC_OFFSET",
			utc_offset );
	}
	else
	{
		fprintf( stderr,
	"Warning in %s/%s/%d: APPLICATION_CONSTANT=utc_offset not found.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
	}

	application_constant_free( application_constant );
}

char *environ_http_referer_filename( void )
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

		ch_count =
			string_character_count(
				'/',
				http_referer_with_pathname );

		piece(	http_referer_filename,
			'/',
			http_referer_with_pathname,
			ch_count );

		return http_referer_filename;
	}
}

void environ_standard_unix_to_path( void )
{
	environ_local_bin_to_path();
	set_path( "/bin:/usr/bin:/etc" );
}

void environ_append_dot_to_path( void )
{
	environment_append_path( "." );
}

void add_dot_to_path( void )
{
	set_path( "." );
}

void add_pwd_to_path( void )
{
	set_path( string_pipe_fetch( "pwd" ) );
}

void add_etc_to_path( void )
{
	set_path( "/etc" );
}

void add_local_bin_to_path( void )
{
	environ_local_bin_to_path();
}

void environ_local_bin_to_path( void )
{
	set_path( "/usr/local/bin" );
}

void environ_usr2_bin_to_path( void )
{
	set_path( "/usr2/bin" );
}

void add_utility_to_path( void )
{
	environ_utility_to_path();
}

void environment_utility_path( void )
{
	environ_utility_to_path();
}

void environ_utility_to_path( void )
{
	char *appaserver_mount_point;
	char utility_path[ 128 ];

	appaserver_mount_point =
		appaserver_parameter_mount_point();

	sprintf( utility_path, "%s/utility", appaserver_mount_point );
	set_path( utility_path );
}

void add_appaserver_home_to_environment( void )
{
	environ_appaserver_home();
}

void environ_appaserver_home( void )
{
	environ_set_environment(
		"APPASERVER_HOME",
		appaserver_parameter_mount_point() );
}

void environ_umask( mode_t application_umask )
{
	umask( application_umask );
}

void add_src_appaserver_to_path( void )
{
	environ_src_appaserver_to_path();
}

void environ_src_appaserver_to_path( void )
{
	char bin_path[ 256 ];

	sprintf(bin_path,
		"%s/%s",
		appaserver_parameter_mount_point(),
		"src_appaserver" );

	set_path( bin_path );
}

void environ_src_system_to_path( void )
{
	char bin_path[ 256 ];

	sprintf(bin_path,
		"%s/%s",
		appaserver_parameter_mount_point(),
		"src_system" );

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
		appaserver_parameter_mount_point();
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
}

void add_appaserver_home_to_python_path( void )
{
	char *appaserver_mount_point;

	appaserver_mount_point = 
		appaserver_parameter_mount_point();

	set_environment( "PYTHONPATH", appaserver_mount_point );
}

void add_path( char *path_to_add )
{
	set_path( path_to_add );
}

void add_relative_source_directory_to_path( char *application_name )
{
	environ_relative_source_directory_to_path( application_name );
}

void environ_relative_source_directory_to_path( char *application_name )
{
	char *appaserver_mount_point;
	char *relative_source_directory;
	char piece_buffer[ 128 ];
	char bin_path[ 256 ];
	int index;
	char delimiter;

	appaserver_mount_point =
		appaserver_parameter_mount_point();

	relative_source_directory =
		application_relative_source_directory(
			application_name );

	if ( ! ( delimiter =
			string_delimiter(
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
}

char *environ_exit_application_name( char *argv_0 )
{
	return environment_exit_application_name( argv_0 );
}

char *environ_application_name( char *argv_0 )
{
	return environ_exit_application_name( argv_0 );
}

char *environment_exit_application_name( char *argv_0 )
{
	char *application_name;

	if ( ! ( application_name = environment_application() ) )
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
		 environ_shell_snippet() );

}

char *environ_shell_snippet( void )
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
}

char *environment_database( void )
{
	return environment_application_name();
}

char *environment_http_user_agent(
		const char *environment_user_agent_key )
{
	char *result;

	if ( ( result = environment_get( (char *)environment_user_agent_key ) ) )
	{
		return result;
	}
	else
	{
		return (char *)0;
	}
}

char *environment_remote_ip_address(
		const char *environment_remote_key )
{
	char *result;

	if ( ( result = environment_get( (char *)environment_remote_key ) ) )
	{
		return result;
	}
	else
	{
		return (char *)0;
	}
}

void environment_session_set( char *application_name )
{
	environment_database_set( application_name );
	environ_src_appaserver_to_path();
	environ_src_system_to_path();
	environ_utility_to_path();
	environ_relative_source_directory_to_path( application_name );
	environ_append_dot_to_path();
	environ_appaserver_home();
	environ_set_utc_offset();
	environ_umask( APPLICATION_UMASK );
}

