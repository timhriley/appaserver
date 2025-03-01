/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/shell_script.c				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "appaserver.h"
#include "shell_script.h"

char *shell_script_open_string( char *application_name )
{
	static char open_string[ 1024 ];
	char *ptr = open_string;

	ptr += sprintf( ptr,
"#!/bin/bash\n" );

	ptr += sprintf( ptr,
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
"fi\n\n" );

	if ( application_name )
	{
		ptr += sprintf(
			ptr,
"if [ \"$application\" != \"%s\" ]\n"
"then\n"
"	exit 0\n"
"fi\n",
			application_name );
	}

	return open_string;
}

char *shell_script_close_string( void )
{
	return "exit 0";
}

char *shell_script_here_open_string( const char *shell_script_here_label )
{
	static char here_open_string[ 128 ];

	sprintf(here_open_string,
		"(\ncat << %s",
		shell_script_here_label );

	return here_open_string;
}

char *shell_script_here_close_string(
		const char *shell_script_here_label,
		boolean tee_appaserver_boolean,
		boolean html_paragraph_wrapper_boolean )
{
	static char here_close_string[ 128 ];
	char *ptr = here_close_string;

	ptr += sprintf(
		ptr,
		"%s\n) | "
		"sql.e 2>&1 | "
		"grep -iv duplicate",
		shell_script_here_label );

	if ( tee_appaserver_boolean )
	{
		ptr += sprintf(
			ptr,
			" | tee_appaserver.sh" );
	}

	if ( html_paragraph_wrapper_boolean )
	{
		ptr += sprintf(
			ptr,
			" | html_paragraph_wrapper.e" );
	}

	return here_close_string;
}

SHELL_SCRIPT *shell_script_calloc( void )
{
	SHELL_SCRIPT *shell_script;

	if ( ! ( shell_script = calloc( 1, sizeof( SHELL_SCRIPT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return shell_script;
}

SHELL_SCRIPT *shell_script_new(
		char *application_name,
		char *shell_script_filespecification,
		LIST *sql_statement_list,
		boolean tee_appaserver_boolean,
		boolean html_paragraph_wrapper_boolean )
{
	SHELL_SCRIPT *shell_script;
	FILE *output_file;

	if ( !application_name
	||   !shell_script_filespecification )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	shell_script = shell_script_calloc();

	output_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_file(
			shell_script_filespecification );

	fprintf(output_file,
		"%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		shell_script_open_string(
			application_name ) );

	fprintf(output_file,
		"%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		shell_script_here_open_string(
			SHELL_SCRIPT_HERE_LABEL ) );

	if ( list_rewind( sql_statement_list ) )
	do {
		fprintf(output_file,
			"%s\n",
			(char *)list_get( sql_statement_list ) );

	} while ( list_next( sql_statement_list ) );

	fprintf(output_file,
		"%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		shell_script_here_close_string(
			SHELL_SCRIPT_HERE_LABEL,
			tee_appaserver_boolean,
			html_paragraph_wrapper_boolean ) );

	fprintf(output_file,
		"%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		shell_script_close_string() );

	fclose( output_file );

	if ( system(
		shell_script_execute_bit_system_string(
			shell_script_filespecification ) ) ){}

	return shell_script;
}

void shell_script_spool_string_list(
		char *application_name,
		char *shell_filename,
		LIST *sql_statement_list )
{
	FILE *output_file;

	if ( !application_name
	||   !shell_filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* Safely returns */
	/* -------------- */
	output_file = appaserver_output_file( shell_filename );

	fprintf(output_file,
		"%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		shell_script_open_string(
			application_name ) );

	fprintf(output_file,
		"%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		shell_script_here_open_string(
			SHELL_SCRIPT_HERE_LABEL ) );

	if ( list_rewind( sql_statement_list ) )
	do {
		fprintf(output_file,
			"%s\n",
			(char *)list_get( sql_statement_list ) );

	} while ( list_next( sql_statement_list ) );

	fprintf(output_file,
		"%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		shell_script_here_close_string(
			SHELL_SCRIPT_HERE_LABEL,
			0 /* not tee_appaserver_boolean */,
			0 /* not html_paragraph_wrapper_boolean */ ) );

	fprintf(output_file,
		"%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		shell_script_close_string() );

	fclose( output_file );
}

char *shell_script_filespecification(
		const char *process_label,
		char *application_name,
		char *folder_name,
		char *data_directory )
{
	static char specification[ 128 ];

	if ( !application_name
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

	if ( !folder_name )
	{
		snprintf(
			specification,
			sizeof ( specification ),
			"%s/%s/%s.sh",
			data_directory,
			application_name,
			process_label );
	}
	else
	{
		snprintf(
			specification,
			sizeof ( specification ),
			"%s/%s/%s_%s.sh",
			data_directory,
			application_name,
			process_label,
			folder_name );
	}

	return specification;
}

char *shell_script_execute_bit_system_string( char *specification )
{
	char system_string[ 256 ];

	if ( !specification )
	{
		char message[ 128 ];

		sprintf(message, "specification is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"chmod +x,g+w %s",
		specification );

	return strdup( system_string );
}

