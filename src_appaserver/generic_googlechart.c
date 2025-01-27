/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/generic_googlechart.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_error.h"
#include "environ.h"
#include "document.h"
#include "process.h"
#include "appaserver_parameter.h"
#include "post_dictionary.h"
#include "process_generic_google.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *process_name;
	char *process_set_name;
	POST_DICTIONARY *post_dictionary;
	APPASERVER_PARAMETER *appaserver_parameter;
	PROCESS_GENERIC_GOOGLE *process_generic_google;

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
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	process_name = argv[ 2 ];
	process_set_name = argv[ 3 ];

	post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_string_new(
			argv[ 4 ] /* dictionary_string */ );

	/* Safely returns */
	/* -------------- */
	appaserver_parameter = appaserver_parameter_new();

	process_generic_google =
		process_generic_google_new(
			application_name,
			login_name,
			process_name,
			process_set_name,
			appaserver_parameter->data_directory,
			post_dictionary->
				original_post_dictionary
					/* post_dictionary */ );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_generic_google->
			process_generic->
			title /* process_name */ );

	google_generic_create(
		process_generic_google->
			google_generic->
			google_filename->
			http_output_filename,
		process_generic_google->
			google_generic->
			google_single_chart_list_html );

	printf(	"%s\n",
		process_generic_google->
		google_window->
		html );

	document_close();

	return 0;
}

