/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/output_histogram.c			*/
/* --------------------------------------------------------------------	*/
/* This process is triggered if you select the histogram radio		*/
/* button in the lookup forms that prompt folders with numbers.		*/
/*									*/
/* No warranty and freely available software: see Appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "environ.h"
#include "document.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "lookup_histogram.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	char *dictionary_string;
	LOOKUP_HISTOGRAM *lookup_histogram;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf(stderr, 
			"Usage: %s session login_name role folder dictionary\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	folder_name = argv[ 4 ];
	dictionary_string = argv[ 5 ];

	lookup_histogram =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		lookup_histogram_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			dictionary_string,
			/* ------------------------------------------------ */
			/* Returns component of global_appaserver_parameter */
			/* ------------------------------------------------ */
			appaserver_parameter_data_directory() );

	if ( lookup_histogram->chart_input->row_list_length
	&&   list_rewind( lookup_histogram->lookup_histogram_graph_list ) )
	{
		LOOKUP_HISTOGRAM_GRAPH *lookup_histogram_graph;

		document_head_output_html_stream( stdout );

		printf(	"%s\n",
			lookup_histogram->lookup_histogram_window->html );

		do {
			lookup_histogram_graph =
				list_get(
					lookup_histogram->
						lookup_histogram_graph_list );

			histogram_pdf_create(
				lookup_histogram_graph->
					histogram->
					number_list,
				lookup_histogram_graph->
					histogram->
					system_string );

			printf( "%s\n",
				lookup_histogram_graph->
					lookup_histogram_prompt->
					html );
		} while ( list_next(
				lookup_histogram->
					lookup_histogram_graph_list ) );
	}
	else
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			"output_histogram" /* title */ );

		printf( "<h2>No data selected.</h2>\n" );
	}

	document_close();

	return 0;
}

