/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/generic_statistics_report.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "boolean.h"
#include "document.h"
#include "environ.h"
#include "application.h"
#include "appaserver_error.h"
#include "post_dictionary.h"
#include "process_generic_statistic.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *process_set_name;
	char *process_name;
	POST_DICTIONARY *post_dictionary;
	PROCESS_GENERIC_STATISTIC *process_generic_statistic;

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
		exit ( 1 );
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

	process_generic_statistic =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_statistic_new(
			application_name,
			login_name,
			process_name,
			process_set_name,
			post_dictionary->original_post_dictionary
				/* post_dictionary */ );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_generic_statistic->process_generic->title );

	html_table_output(
		process_generic_statistic->html_table,
		0 /* rows_between_heading */ );

	document_close();

	process_execution_count_increment( process_name );

	return 0;
}

