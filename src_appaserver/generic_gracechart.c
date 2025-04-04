/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/generic_gracechart.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and Freely available software: see Appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_error.h"
#include "post_dictionary.h"
#include "document.h"
#include "appaserver_parameter.h"
#include "application.h"
#include "environ.h"
#include "process_generic_grace.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_set_name;
	char *process_name;
	char *login_name;
	POST_DICTIONARY *post_dictionary;
	APPASERVER_PARAMETER *appaserver_parameter;
	PROCESS_GENERIC_GRACE *process_generic_grace;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf(stderr,
		"Usage: %s login_name process_set process dictionary\n",
			argv[ 0 ] );
		exit( 1 );
	}

	login_name = argv[ 1 ];
	process_set_name = argv[ 2 ];
	process_name = argv[ 3 ];

	post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_string_new(
			argv[ 4 ] /* dictionary_string */ );

	/* -------------- */
	/* Safely returns */
	/* -------------- */
	appaserver_parameter = appaserver_parameter_new();

	process_generic_grace =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_grace_new(
			application_name,
			login_name,
			process_name,
			process_set_name,
			appaserver_parameter->document_root,
			post_dictionary->original_post_dictionary
				/* post_dictionary */ );

	grace_single_graph_pdf_create(
		process_generic_grace->
			grace_generic->
			grace_setup->
			grace_ps_system_string,
		process_generic_grace->
			grace_generic->
			grace_setup->
			grace_page,
		process_generic_grace->
			grace_generic->
			grace_setup->
			grace_title,
		process_generic_grace->grace_generic->grace_graph,
		process_generic_grace->
			grace_generic->
			grace_setup->
			grace_pdf_system_string );

	printf ("%s\n", process_generic_grace->grace_window->html );

	document_close();
	return 0;
}

