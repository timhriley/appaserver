/* $APPASERVER_HOME/src_appaserver/output_google_chart.c		*/
/* --------------------------------------------------------------------	*/
/* This process is triggered if you select the google chart radio	*/
/* button in the lookup forms that prompt folders with numbers.		*/
/*									*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "environ.h"
#include "document.h"
#include "appaserver.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "lookup_google.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	char *dictionary_string;
	LOOKUP_GOOGLE *lookup_google;

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

	lookup_google =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		lookup_google_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			dictionary_string,
			appaserver_parameter_data_directory() );

	if ( lookup_google->chart_input->row_list_length
	&&   list_rewind( lookup_google->lookup_google_graph_list ) )
	{
		LOOKUP_GOOGLE_GRAPH *lookup_google_graph;
		FILE *output_file;

		document_head_output_html_stream( stdout );

		printf( "%s\n",
			lookup_google->
				google_window->
				html );

		printf( "<table border=1>\n" );
		printf( "<tr><th>Chart\n" );

		do {
			lookup_google_graph =
				list_get(
					lookup_google->
						lookup_google_graph_list );

			output_file =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				appaserver_output_file(
					lookup_google_graph->
						google_graph->
						google_filename->
						http_output_filename );

			google_graph_output(
				lookup_google_graph->
					google_graph,
				output_file );

			fclose( output_file );

			printf( "%s\n",
				lookup_google_graph->
					google_prompt->
					html );

		} while ( list_next(
				lookup_google->
					lookup_google_graph_list ) );

		printf( "</table>\n" );
	}
	else
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			"Google Chart" /* title_string */ );

		printf( "<h2>No data for selection.</h2>\n" );
	}

	document_close();

	return 0;
}
