/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/output_rchart.c			*/
/* --------------------------------------------------------------------	*/
/* This process is triggered if you select the R chart radio		*/
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
#include "role.h"
#include "folder.h"
#include "security.h"
#include "post_dictionary.h"
#include "dictionary_separate.h"
#include "query.h"
#include "document.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "lookup_rchart.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	char *dictionary_string;
	LOOKUP_RCHART *lookup_rchart;
	LOOKUP_RCHART_CHART *lookup_rchart_chart;

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

	lookup_rchart =
		lookup_rchart_new(
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

	if ( !lookup_rchart )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			"R Chart" /* title_string */ );
		printf( "<h3>Invalid input to generate a chart.</h3>\n" );
		document_close();
		exit( 0 );
	}

	document_head_output_html_stream( stdout );

	printf( "%s\n",
		lookup_rchart->rchart_window->html );

	printf( "<table border=1>\n" );

	printf( "<tr><th>PDF<th>GUI\n" );

	if ( list_rewind( lookup_rchart->lookup_rchart_chart_list ) )
	do {
		lookup_rchart_chart =
			list_get(
				lookup_rchart->lookup_rchart_chart_list );

		if ( system(
			lookup_rchart_chart->
				rchart->
				script_system_string ) ){}

		rchart_free( lookup_rchart_chart->rchart );

		printf( "%s\n",
			lookup_rchart_chart->
				rchart_prompt->
				html );

	} while ( list_next(
			lookup_rchart->
				lookup_rchart_chart_list ) );
	else
	{
		printf( "<h2>No data selected.</h2>\n" );
	}

	printf( "</table>\n" );

	document_close();

	return 0;
}

