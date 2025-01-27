/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/generic_high_average_low.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "application.h"
#include "appaserver_error.h"
#include "environ.h"
#include "document.h"
#include "appaserver_parameter.h"
#include "post_dictionary.h"
#include "process_generic_whisker.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *process_set_name;
	char *process_name;
	APPASERVER_PARAMETER *appaserver_parameter;
	POST_DICTIONARY *post_dictionary;
	PROCESS_GENERIC_WHISKER *process_generic_whisker;

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
		post_dictionary_string_new(
			argv[ 4 ] );

	/* Safely returns */
	/* -------------- */
	appaserver_parameter = appaserver_parameter_new();

	process_generic_whisker =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_whisker_new(
			application_name,
			login_name,
			process_name,
			process_set_name,
			appaserver_parameter->document_root,
			post_dictionary->original_post_dictionary
				/* post_dictionary */ );

	grace_single_graph_pdf_create(
		process_generic_whisker->
			grace_whisker->
			grace_setup->
			grace_ps_system_string,
		process_generic_whisker->
			grace_whisker->
			grace_setup->
			grace_page,
		process_generic_whisker->
			grace_whisker->
			grace_setup->
			grace_title,
		process_generic_whisker->grace_whisker->grace_graph,
		process_generic_whisker->
			grace_whisker->
			grace_setup->
			grace_pdf_system_string );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_generic_whisker->
			process_generic->
			title /* process_name */ );

	printf( "%s\n",
		process_generic_whisker->
		grace_window->
		html );

	document_close();

	return 0;
}
