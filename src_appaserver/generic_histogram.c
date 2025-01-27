/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/generic_histogram.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include "boolean.h"
#include "post_dictionary.h"
#include "appaserver_error.h"
#include "environ.h"
#include "document.h"
#include "process_generic_histogram.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *process_name;
	char *process_set_name;
	POST_DICTIONARY *post_dictionary;
	PROCESS_GENERIC_HISTOGRAM *process_generic_histogram;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf(stderr,
			"Usage: %s login_name process process_set dictionary\n",
			argv[ 0 ] );
		exit( 1 );
	}

	login_name = argv[ 1 ];
	process_name = argv[ 2 ];
	process_set_name = argv[ 3 ];

	post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_string_new(
			argv[ 4 ] );

	process_generic_histogram =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_histogram_new(
			application_name,
			login_name,
			process_name,
			process_set_name,
			post_dictionary->original_post_dictionary
				/* post_dictionary */ );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_generic_histogram->
			process_generic->
			title /* process_name */ );

	fflush( stdout );
	if ( system( process_generic_histogram->system_string ) ){}
	fflush( stdout );

	document_close();
	return 0;
}

