/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/grace_histogram.c				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "String.h"
#include "piece.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "grace.h"
#include "histogram.h"
#include "list.h"
#include "environ.h"

LIST *grace_histogram_number_double_list(
			char delimiter,
			int value_piece );

int main( int argc, char **argv )
{
	char *application_name;
	char *title;
	char *sub_title;
	char *attribute_name;
	char delimiter;
	int value_piece;
	APPASERVER_PARAMETER *appaserver_parameter;
	LIST *number_double_list;
	LIST *histogram_list;
	GRACE_HISTOGRAM *grace_histogram;

	if ( argc != 7 )
	{
		fprintf(stderr,
"Usage: %s application title sub_title attribute_name delimiter value_piece\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	title = argv[ 2 ];
	sub_title = argv[ 3 ];
	attribute_name = argv[ 4 ];
	delimiter = *argv[ 5 ];
	value_piece = atoi( argv[ 6 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	number_double_list =
		grace_histogram_number_double_list(
			delimiter,
			value_piece );

	if ( !list_length( number_double_list ) ) exit( 0 );

	histogram_list = list_new();

	list_set(
		histogram_list,
		histogram_new(
			attribute_name,
			number_double_list ) );

	grace_histogram =
		grace_histogram_new(
			application_name,
			(char *)0 /* session_key */,
			appaserver_parameter->data_directory,
			title,
			sub_title,
			histogram_list );

	if ( grace_histogram )
	{
		GRACE_WINDOW *grace_window =
			grace_window_new(
				grace_histogram->
					grace_setup->
					grace_filename,
				(char *)0 /* sub_title */,
				getpid() /* process_id */ );

		grace_pdf_create(
			grace_histogram->
				grace_setup->
				grace_ps_system_string,
			grace_histogram->
				grace_setup->
				grace_page,
			grace_histogram->
				grace_setup->
				grace_title,
			grace_histogram->grace_graph_list,
			grace_histogram->
				grace_setup->
				grace_pdf_system_string );

		printf ( "%s\n", grace_window->html );
	}

	return 0;
}

LIST *grace_histogram_number_double_list(
			char delimiter,
			int value_piece )
{
	char input[ 1024 ];
	char value_string[ 128 ];
	LIST *number_double_list = list_new();

	while( string_input( input, stdin, 128 ) )
	{
		if ( !*input ) continue;

		piece( value_string, delimiter, input, value_piece );

		histogram_set_number_double(
			number_double_list,
			string_atof( value_string ) );
	}

	return number_double_list;
}

