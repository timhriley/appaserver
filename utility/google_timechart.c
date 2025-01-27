/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/google_timechart.c	       			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "list.h"
#include "boolean.h"
#include "document.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "environ.h"
#include "google_chart.h"

typedef struct
{
	GOOGLE_STDIN *google_stdin;
	GOOGLE_WINDOW *google_window;
} OUTPUT_GOOGLE_STDIN;

/* Usage */
/* ----- */
OUTPUT_GOOGLE_STDIN *output_google_stdin_new(
		char *application_name,
		char *session_key,
		char *document_root_directory,
		LIST *number_attribute_name_list,
		char delimiter );

/* Process */
/* ------- */
OUTPUT_GOOGLE_STDIN *output_google_stdin_calloc(
		void );

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	OUTPUT_GOOGLE_STDIN *output_google_stdin;
	APPASERVER_PARAMETER *appaserver_parameter;
	LIST *number_attribute_name_list;
	char delimiter;

	if ( argc != 5 )
	{
		fprintf(stderr,
"Usage: %s application session attribute_name[^attribute_name...] delimiter\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	number_attribute_name_list = list_string_list( argv[ 3 ], '^' );
	delimiter = *argv[ 4 ];

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	appaserver_parameter = appaserver_parameter_new();

	output_google_stdin =
		output_google_stdin_new(
			application_name,
			session_key,
			appaserver_parameter->document_root,
			number_attribute_name_list,
			delimiter );

	if ( !output_google_stdin
	||   !output_google_stdin->google_stdin
	||   !output_google_stdin->google_window
	||   !output_google_stdin->google_stdin->google_filename )
	{
		exit( 1 );
	}

	google_stdin_create(
		output_google_stdin->
			google_stdin->
			google_filename->
			http_output_filename,
		output_google_stdin->
			google_stdin->
			html );

	printf(	"%s\n",
		output_google_stdin->
			google_window->
			html );

	return 0;
}

OUTPUT_GOOGLE_STDIN *output_google_stdin_new(
		char *application_name,
		char *session_key,
		char *document_root_directory,
		LIST *number_attribute_name_list,
		char delimiter )
{
	OUTPUT_GOOGLE_STDIN *output_google_stdin;

	if ( !application_name
	||   !session_key
	||   !document_root_directory
	||   !list_length( number_attribute_name_list ) )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	output_google_stdin = output_google_stdin_calloc();

	if ( ! ( output_google_stdin->google_stdin =
			google_stdin_new(
				application_name,
				session_key,
				document_root_directory,
				number_attribute_name_list,
				delimiter ) ) )
	{
		free( output_google_stdin );
		return (OUTPUT_GOOGLE_STDIN *)0;
	}

	output_google_stdin->google_window =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		google_window_new(
			application_name,
			output_google_stdin->
				google_stdin->
				google_filename->
				http_prompt_filename,
			(char *)0 /* query_table_edit_where_string */,
			getpid() /* process_id */ );

	return output_google_stdin;
}

OUTPUT_GOOGLE_STDIN *output_google_stdin_calloc( void )
{
	OUTPUT_GOOGLE_STDIN *output_google_stdin;

	if ( ! ( output_google_stdin =
			calloc( 1,
				sizeof( OUTPUT_GOOGLE_STDIN ) ) ) )
	{
		char message[ 256 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return output_google_stdin;
}

