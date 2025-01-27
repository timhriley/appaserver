/* -----------------------------------------------------------	*/
/* synchronize_mysql_consume.c					*/
/* -----------------------------------------------------------	*/
/* Input: sql statements from synchronize_mysql_produce		*/
/* Process: sql statements sent to sql.e			*/
/* -----------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "application_constants.h"

boolean remove_logfile(		char *mysql_logfiles_directory,
				char *logfile_name );

boolean output_logfile_list(	LIST *logfile_list,
				char *consume_directory,
				char *destination_dbms );

LIST *get_logfile_list(		char *synchronize_current_logfile_name,
				char *synchronize_consume_directory,
				char *mysql_logfiles_prefix );

int main( int argc, char **argv )
{
	char *application_name;
	APPLICATION_CONSTANTS *application_constants;
	char *synchronize_consume_directory;
	char *synchronize_mysql_logfiles_prefix;
	char *destination_dbms;
	LIST *logfile_list;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 3 )
	{
		fprintf(stderr,
"Usage: %s ignored destination_dbms\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	destination_dbms = argv[ 2 ];

	add_standard_unix_to_path();

	application_constants = application_constants_new();
	application_constants->dictionary =
		application_constants_get_dictionary(
			application_name );

	if ( ! ( synchronize_consume_directory =
		application_constants_fetch(
			application_constants->dictionary,
			"synchronize_consume_directory" ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s(): cannot get synchronize_consume_directory from APPLICATION_CONSTANTS\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	if ( ! ( synchronize_mysql_logfiles_prefix =
		application_constants_fetch(
			application_constants->dictionary,
			"synchronize_mysql_logfiles_prefix" ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s(): cannot get synchronize_mysql_logfiles_prefix from APPLICATION_CONSTANTS\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	if ( ! ( logfile_list = get_logfile_list(
			SYNCHRONIZE_CURRENT_LOGFILE_NAME,
			synchronize_consume_directory,
			synchronize_mysql_logfiles_prefix ) ) )
	{
		fprintf( stderr,
			 "ERROR: %s/%s()/%d exiting early\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_length( logfile_list ) ) exit( 0 );

	if ( !output_logfile_list(	logfile_list,
					synchronize_consume_directory,
					destination_dbms ) )
	{
		fprintf( stderr, "ERROR occurred in %s\n", argv[ 0 ] );
		exit( 1 );
	}

	return 0;
}

boolean output_logfile_list(	LIST *logfile_list,
				char *consume_directory,
				char *destination_dbms )
{
	char sys_string[ 2048 ];
	char date_convert_process[ 128 ];
	char *logfile_name;

	strcpy( date_convert_process, "cat" );

	if ( !list_rewind( logfile_list ) ) return 0;

	do {
		logfile_name = list_get_pointer( logfile_list );

		sprintf( sys_string,
"cat %s									|"
"%s									|"
"grep -v '^#'								|"
"grep -v '^use'								|"
"grep -v 'set next_session_number = next_session_number + 1'		|"
"grep -v 'set execution_count ='					|"
"grep -v '^SET timestamp='						|"
"grep -v '^update appaserver_sessions'					|"
"grep -v '^insert into appaserver_sessions'				|"
"grep -v 'appaserver_sessions;$'					|"
"grep -v '^grant'							|"
"commit.e 500								|"
"sql.e '^' %s 2>&1							|"
"grep -v duplicate							 ",
			 logfile_name,
			 date_convert_process,
			 destination_dbms );

		if ( system( sys_string ) ){};

		if ( !remove_logfile( consume_directory,
				logfile_name ) )
		{
			return 0;
		}

	} while( list_next( logfile_list ) );

	return 1;
}

LIST *get_logfile_list(	char *synchronize_current_logfile_name,
			char *synchronize_consume_directory,
			char *mysql_logfiles_prefix )
{
	char sys_string[ 1024 ];
	LIST *logfile_list;
	FILE *input_file;
	char input_buffer[ 128 ];

	add_pwd_to_path();
	
	if ( chdir( synchronize_consume_directory ) != 0 )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot chdir(%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 synchronize_consume_directory );
		exit( 1 );
	}

	sprintf( sys_string,
		 "ls -1 %s.[0-9][0-9][0-9]* 2>&1 | grep -v 'No such'",
		 mysql_logfiles_prefix );

	logfile_list = pipe2list( sys_string );

	if ( list_length( logfile_list ) )
	{
		input_file = fopen( synchronize_current_logfile_name, "r" );
		if ( !input_file ) return logfile_list;
		get_line( input_buffer, input_file );
		fclose( input_file );
		list_subtract_string( logfile_list, input_buffer );
	}

	return logfile_list;
}

boolean remove_logfile( char *mysql_logfiles_directory,
			char *logfile_name )
{
	char sys_string[ 1024 ];
	char *results;

	sprintf( sys_string,
		 "rm -f %s/%s 2>&1",
		 mysql_logfiles_directory,
		 logfile_name );
	if ( ( results = pipe2string( sys_string ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 results );
		return 0;
	}
	return 1;
}

