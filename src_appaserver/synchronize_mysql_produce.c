/* -----------------------------------------------------------		*/
/* $APPASERVER_HOME/src_appaserver/synchronize_mysql_produce.c		*/
/* -----------------------------------------------------------		*/
/* Input: mysql log files						*/
/* Output: sql statements to synchronize Appaserver			*/
/* -----------------------------------------------------------		*/
/* No warrancy and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "application.h"
#include "appaserver_error.h"
#include "environ.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"

#define CURRENT_LOGFILE_LOCAL_DIRECTORY	"/tmp"

boolean ftp_file(			char *source_file_directory,
					char *source_filename,
					char *destination_host,
					char *destination_username,
					char *destination_password,
					char *consume_directory );

boolean ftp_current_logfile_name(	char *current_logfile_name,
					char *logfile_name,
					char *destination_host,
					char *consume_directory,
					char *destination_username,
					char *destination_password );

LIST *get_logfile_list(			char *mysql_logfiles_directory,
					char *mysql_logfiles_prefix );

boolean flush_mysql_logfiles(		void );

boolean output_logfile_list(
					LIST *logfile_list,
					char *current_logfile_name,
					char *destination_host,
					char *consume_directory,
					char *destination_username,
					char *destination_password,
					char *mysql_logfiles_directory );

boolean remove_logfile(			char *mysql_logfiles_directory,
					char *logfile_name );

int main( int argc, char **argv )
{
	char *application_name;
	APPLICATION_CONSTANTS *application_constants;
	char *synchronize_destination_host;
	char *synchronize_consume_directory;
	char *synchronize_destination_username;
	char *synchronize_destination_password;
	char *synchronize_mysql_logfiles_directory;
	char *synchronize_mysql_logfiles_prefix;
	LIST *logfile_list;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s ignored\n", argv[ 0 ] );
		exit( 1 );
	}

	add_standard_unix_to_path();

	application_constants = application_constants_new();
	application_constants->dictionary =
		application_constants_get_dictionary(
			application_name );

	if ( ! ( synchronize_destination_host =
		application_constants_fetch(
			application_constants->dictionary,
			"synchronize_destination_host" ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s(): cannot get synchronize_destination_host from APPLICATION_CONSTANTS\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

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

	if ( ! ( synchronize_destination_username =
		application_constants_fetch(
			application_constants->dictionary,
			"synchronize_destination_username" ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s(): cannot get synchronize_destination_username from APPLICATION_CONSTANTS\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	if ( ! ( synchronize_destination_password =
		application_constants_fetch(
			application_constants->dictionary,
			"synchronize_destination_password" ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s(): cannot get synchronize_destination_password from APPLICATION_CONSTANTS\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	if ( ! ( synchronize_mysql_logfiles_directory =
		application_constants_fetch(
			application_constants->dictionary,
			"synchronize_mysql_logfiles_directory" ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s(): cannot get synchronize_mysql_logfiles_directory from APPLICATION_CONSTANTS\n",
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

	logfile_list =
		get_logfile_list(
			synchronize_mysql_logfiles_directory,
			synchronize_mysql_logfiles_prefix );

	if ( !list_length( logfile_list ) ) exit( 0 );

	if ( !flush_mysql_logfiles() )
	{
		fprintf( stderr,
		"ERROR in %s/%s(): cannot flush log files\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	if ( !output_logfile_list(	logfile_list,
					SYNCHRONIZE_CURRENT_LOGFILE_NAME,
					synchronize_destination_host,
					synchronize_consume_directory,
					synchronize_destination_username,
					synchronize_destination_password,
					synchronize_mysql_logfiles_directory ) )
	{
		fprintf( stderr, "ERROR occurred in %s\n", argv[ 0 ] );
		exit( 1 );
	}

	return 0;
} /* main() */

boolean flush_mysql_logfiles()
{
	char sys_string[ 1024 ];
	char *results;

	strcpy( sys_string, "mysqladmin -u flush -pflushpass flush-logs 2>&1" );
	if ( ( results = pipe2string( sys_string ) ) )
	{
		fprintf( stderr,
			 "%s:%s(): %s\n",
			 __FILE__,
			 __FUNCTION__,
			 results );
	}
	return !(boolean)results;
} /* flush_mysql_logfiles() */

LIST *get_logfile_list(	char *mysql_logfiles_directory,
			char *mysql_logfiles_prefix )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "cd %s && ls -1 %s.[0-9][0-9][0-9]* 2>&1",
		 mysql_logfiles_directory,
		 mysql_logfiles_prefix );

	return pipe2list( sys_string );
} /* get_logfile_list() */

boolean output_logfile_list(
			LIST *logfile_list,
			char *current_logfile_name,
			char *destination_host,
			char *consume_directory,
			char *destination_username,
			char *destination_password,
			char *mysql_logfiles_directory )
{
	char *logfile_name;

	if ( !list_rewind( logfile_list ) ) return 1;

	do {
		logfile_name = list_get_pointer( logfile_list );
		if ( !ftp_current_logfile_name(
					current_logfile_name,
					logfile_name,
					destination_host,
					consume_directory,
					destination_username,
					destination_password ) )
		{
			return 0;
		}

		if ( !ftp_file(	mysql_logfiles_directory,
				logfile_name,
				destination_host,
				destination_username,
				destination_password,
				consume_directory ) )
		{
			return 0;
		}

		if ( !remove_logfile( mysql_logfiles_directory,
				logfile_name ) )
		{
			return 0;
		}

	} while( list_next( logfile_list ) );

	if ( !ftp_current_logfile_name(
				SYNCHRONIZE_CURRENT_LOGFILE_NAME,
				"" /* logfile_name */,
				destination_host,
				consume_directory,
				destination_username,
				destination_password ) )
	{
		return 0;
	}

	return 1;
} /* output_logfile_list() */

boolean ftp_current_logfile_name(	char *current_logfile_name,
					char *logfile_name,
					char *destination_host,
					char *consume_directory,
					char *destination_username,
					char *destination_password )
{
	char source_filespecification[ 128 ];
	FILE *output_file;

	sprintf( source_filespecification,
		 "%s/%s",
		 CURRENT_LOGFILE_LOCAL_DIRECTORY,
		 current_logfile_name );

	if ( ! ( output_file = fopen( source_filespecification, "w" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for write\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 source_filespecification );
		return 0;
	}
	fprintf( output_file, "%s\n", logfile_name );
	fclose( output_file );
	return ftp_file(CURRENT_LOGFILE_LOCAL_DIRECTORY,
			current_logfile_name,
			destination_host,
			destination_username,
			destination_password,
			consume_directory );

} /* ftp_current_logfile_name() */

boolean ftp_file(	char *source_file_directory,
			char *source_filename,
			char *destination_host,
			char *destination_username,
			char *destination_password,
			char *consume_directory )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char buffer[ 1024 ];

	sprintf( sys_string,
		 "cd %s && ftp_put.sh %s %s %s %s %s 2>&1",
		 source_file_directory,
		 destination_host,
		 consume_directory,
		 source_filename,
		 destination_username,
		 destination_password );

	input_pipe = popen( sys_string, "r" );
	while( get_line( buffer, input_pipe ) )
	{
		if ( instr( "failure", buffer, 1 ) != -1
		||   instr( "Usage: ", buffer, 1 ) == 0
		||   instr( "incorrect", buffer, 1 ) != -1
		||   instr( "550 ", buffer, 1 ) == 0 )
		{
			fprintf(	stderr,
					"ERROR in %s/%s()/%d: %s\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					buffer );
			pclose( input_pipe );
			return 0;
		}
	}
	pclose( input_pipe );
	return 1;
} /* ftp_file() */

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
} /* remove_logfile() */

#ifdef NOT_DEFINED
boolean synchronize_output_logfile( char *logfile )
{
	char sys_string[ 1024 ];
	FILE *input_file;

	if ( ! ( input_file = fopen( logfile, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): cannot open for read (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 logfile );
		return 0;
	}
	fclose( input_file );

	sprintf(sys_string,
"cat %s									|"
"grep -v '^#'								|"
"grep -v '^use'								|"
"grep -v 'set next_session_number = next_session_number + 1'		|"
"grep -v 'set execution_count ='					|"
"grep -v 'password'							|"
"grep -v '^SET timestamp='						|"
"grep -v '^update appaserver_sessions'					|"
"grep -v '^insert into appaserver_sessions'				|"
"grep -v 'appaserver_sessions;$'					|"
"grep -v '^grant'							|"
"cat									 ",
		logfile );

	fflush( stdout );
	system( sys_string );
	fflush( stdout );
	return 1;
} /* synchronize_output_logfile() */
#endif

