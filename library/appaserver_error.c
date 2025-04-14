/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/appaserver_error.c				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_parameter.h"
#include "date.h"
#include "environ.h"
#include "document.h"
#include "application.h"
#include "application_log.h"
#include "appaserver_error.h"

void appaserver_error_message_file(
		char *application_name,
		char *login_name,
		char *message )
{
	FILE *file;

	/* Safely returns */
	/* -------------- */
	file = appaserver_error_open_file( application_name );

	fprintf(file,
		"%s %s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_error_prompt(
			(char *)0 /* argv_0 */,
			login_name ),
		message );

	fclose( file );
}

char *appaserver_error_filename( char *application_name )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	appaserver_error_filespecification(
		application_name );
}

char *appaserver_error_filespecification( char *application_name )
{
	APPASERVER_PARAMETER *appaserver_parameter;

	if ( !application_name )
	{
		application_name = environment_application_name();

		if ( !application_name )
		{
			char message[ 128 ];

			sprintf(message,
			"environment_application_name() returned empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}
	}

	appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_application(
			application_name );

	if ( !appaserver_parameter->log_directory )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: log_directory is null for application_name=%s.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			application_name );
		exit( 1 );
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	application_log_filename(
		APPLICATION_LOG_EXTENSION,
		application_name,
		appaserver_parameter->log_directory );
}

void appaserver_error_argv_file(
		int argc,
		char **argv,
		char *application_name,
		char *login_name )
{
	FILE *file;

	/* Safely returns */
	/* -------------- */
	file = appaserver_error_open_file( application_name );

	fprintf(file,
		"%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_error_prompt(
			*argv,
			login_name ) );

	while( --argc ) fprintf( file, " %s", *++argv );
	fprintf( file, "\n" );
	fclose( file );
}

FILE *appaserver_error_open_file( char *application_name )
{
	FILE *file;
	char *filename;

	filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_error_filename(
			application_name );

	if ( ! ( file = fopen( filename, "a" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s()/%s/%d: cannot open %s for append.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 filename );

		exit( 1 );
	}

	return file;
}

void msg( char *application_name, char *message )
{
	appaserver_error_message_file(
		application_name,
		(char *)0 /* login_name */,
		message );
}

void appaserver_error_stderr(
		int argc,
		char **argv )
{
	fprintf(stderr,
		"%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_error_prompt(
			*argv,
			(char *)0 /* login_name */ ) );

	while ( --argc ) fprintf( stderr, " %s", *++argv );
	fprintf( stderr, "\n" );
	fflush( stderr );
}

void appaserver_error_stderr_exit(
		const char *file,
		const char *function,
		const int line,
		char *message )
{
	char string[ 65536 ];
	char *application_name;

	snprintf(
		string,
		sizeof ( string ),
		"ERROR in %s/%s()/%d: %s\n",
		file,
		function,
		line,
		message );

	fprintf( stderr, "%s\n", string );

	/* Returns heap memory or null */
	/* --------------------------- */
	application_name = environment_application_name();

	if ( application_name )
	{
		printf( "</table></form></body></html>\n" );

		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			"Error Occurred" /* title_string */ );

		printf( "<h3>ERROR in %s/%s()/%d: %s</h3>\n",
			file,
			function,
			line,
			message );

		document_close();
	}

	exit( 1 );
}

void appaserver_error_warning(
		const char *file,
		const char *function,
		const int line,
		char *message )
{
	fprintf(stderr,
		"\nWarning in %s/%s()/%d: %s\n\n",
		file,
		function,
		line,
		message );
}

APPASERVER_ERROR *appaserver_error_new( char *application_name )
{
	APPASERVER_ERROR *appaserver_error;

	appaserver_error = appaserver_error_calloc();

	appaserver_error->filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_error_filename( application_name );

	return appaserver_error;
}

APPASERVER_ERROR *appaserver_error_calloc( void )
{
	APPASERVER_ERROR *appaserver_error;

	if ( ! ( appaserver_error = calloc( 1, sizeof ( APPASERVER_ERROR ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return appaserver_error;
}

char *appaserver_error_prompt(
		char *argv_0,
		char *login_name )
{
	static char prompt[ 256 ];
	char *ptr = prompt;
	int utc_offset;
	char *remote_ip_address;

	utc_offset = date_utc_offset();

	ptr += sprintf(
		ptr,
		"%s %s",
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		date_now_yyyy_mm_dd( utc_offset ),
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		date_now_hhmm( utc_offset ) );

	remote_ip_address = environment_get( "REMOTE_ADDR" );

	if ( remote_ip_address )
	{
		ptr += sprintf(
			ptr,
			" %s",
			remote_ip_address );
	}

	if ( login_name )
	{
		ptr += sprintf(
			ptr,
			" %s",
			login_name );
	}

	ptr += sprintf( ptr, ":" );

	if ( argv_0 )
	{
		ptr += sprintf(
			ptr,
			" %s",
			argv_0 );
	}

	return prompt;
}

void appaserver_error_argv_append_file(
		int argc,
		char **argv,
		char *application_name )
{
	appaserver_error_argv_file(
		argc,
		argv,
		application_name,
		(char *)0 /* login_name */ );
}

void appaserver_error_login_name_append_file(
		int argc,
		char **argv,
		char *application_name,
		char *login_name )
{
	appaserver_error_argv_file(
		argc,
		argv,
		application_name,
		login_name );
}

