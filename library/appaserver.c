/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/appaserver.c				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "String.h"
#include "security.h"
#include "appaserver_error.h"
#include "application.h"
#include "appaserver.h"

char *appaserver_system_string(
		char *select,
		char *table,
		char *where )
{
	char system_string[ 65536 ];

	if ( !select
	||   !table )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !where ) where = "";

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh \"%s\" %s \"%s\" select",
		select,
		table,
		where );

	return strdup( system_string );
}

FILE *appaserver_input_pipe( char *system_string )
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

	if ( !security_system_string_valid_boolean(
		SECURITY_FORK_CHARACTER,
		SECURITY_FORK_STRING,
		system_string ) )
	{
		char message[ STRING_65K ];

		snprintf(
			message,
			sizeof ( message ),
			"Cannot execute=[%s].",
			system_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return popen( system_string, "r" );
}

FILE *appaserver_append_file( char *filespecification )
{
	FILE *file;

	if ( !filespecification )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: filespecification is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( file = fopen( filespecification, "a" ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: fopen(%s) for append returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			filespecification );
		exit( 1 );
	}

	return file;
}

FILE *appaserver_output_file( char *filespecification )
{
	FILE *file;

	if ( !filespecification )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: filespecification is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( file = fopen( filespecification, "w" ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: fopen(%s) for write returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			filespecification );
		exit( 1 );
	}

	return file;
}

FILE *appaserver_input_file( char *filespecification )
{
	FILE *file;

	if ( !filespecification )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: filespecification is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( file = fopen( filespecification, "r" ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: fopen(%s) for read returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			filespecification );
		exit( 1 );
	}

	return file;
}

FILE *appaserver_output_pipe( char *system_string )
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

	if ( !security_system_string_valid_boolean(
		SECURITY_FORK_CHARACTER,
		SECURITY_FORK_STRING,
		system_string ) )
	{
		char message[ STRING_65K ];

		snprintf(
			message,
			sizeof ( message ),
			"Cannot execute=[%s].",
			system_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return popen( system_string, "w" );
}

char *appaserver_select( LIST *folder_attribute_name_list )
{
	return
	/* ------------------------- */
	/* Returns heap memory or "" */
	/* ------------------------- */
	list_delimited( folder_attribute_name_list, ',' );
}

char *appaserver_spreadsheet_heading_string( LIST *folder_attribute_name_list )
{
	char destination[ 1024 ];

	if ( !list_length( folder_attribute_name_list ) )
		return (char *)0;

	list_double_quote_comma_display(
		destination,
		folder_attribute_name_list );

	return strdup( destination );
}

char *appaserver_spreadsheet_output_system_string(
		char delimiter,
		char *spreadsheet_filespecification )
{
	static char system_string[ 256 ];

	if ( !delimiter
	||   !spreadsheet_filespecification )
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
		"double_quote_comma_delimited.e '%c' >> %s",
		delimiter,
		spreadsheet_filespecification );

	return system_string;
}

char *appaserver_spool_filespecification(
		char *application_name,
		char *folder_name,
		char *session_key,
		pid_t process_id,
		char *data_directory )
{
	char filespecification[ 128 ];

	if ( !application_name
	||   !folder_name
	||   !session_key
	||   !process_id
	||   !data_directory )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		filespecification,
		sizeof ( filespecification ),
		"%s/%s/%s_%s_%d.dat",
		data_directory,
		application_name,
		folder_name,
		session_key,
		process_id );

	return strdup( filespecification );
}

char *appaserver_mnemonic( LIST *key_list )
{
	if ( !list_length( key_list ) ) return (char *)0;

	return
	/* ------------------------- */
	/* Returns heap memory or "" */
	/* ------------------------- */
	list_delimited( key_list, '_' );
}

char *appaserver_table_name( char *folder_name )
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
	/* --------------------- */
	/* Returns static memory */
	/* --------------------- */
	application_table_name(
		folder_name );
}

char *appaserver_mailname( const char *mailname_filespecification )
{
	FILE *input_file;
	static char input[ 32 ];

	input_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_file(
			(char *)mailname_filespecification );

	*input = '\0';

	string_input( input, input_file, sizeof ( input ) );
	fclose( input_file );

	return input;
}

boolean appaserver_executable_exists_boolean( char *executable )
{
	char system_string[ 128 ];
	char *fetch;

	if ( !executable )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: executable is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof( system_string ),
		"which %s",
		executable );

	/* Returns heap memory or null */
	/* --------------------------- */
	if ( ! ( fetch = string_pipe_fetch( system_string ) ) )
	{
		return 0;
	}

	free( fetch );

	return 1;
}

