/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/generic_daily_moving_average.c	*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "boolean.h"
#include "appaserver_error.h"
#include "environ.h"
#include "appaserver.h"
#include "document.h"
#include "appaserver_parameter.h"
#include "post_dictionary.h"
#include "process_generic_moving.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *process_name;
	char *process_set_name;
	int moving_statistic_days;
	char *output_medium_string;
	POST_DICTIONARY *post_dictionary;
	APPASERVER_PARAMETER *appaserver_parameter;
	PROCESS_GENERIC_MOVING *process_generic_moving;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s login process process_set moving_statistic_days output_medium dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	process_name = argv[ 2 ];
	process_set_name = argv[ 3 ];
	moving_statistic_days = atoi( argv[ 4 ] );
	output_medium_string = argv[ 5 ];

	post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_string_new(
			argv[ 6 ] );

	appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	process_generic_moving =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_moving_new(
			post_dictionary->
				original_post_dictionary
					/* post_dictionary in/out */,
			application_name,
			login_name,
			process_name,
			process_set_name,
			moving_statistic_days,
			output_medium_string,
			appaserver_parameter->document_root );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_generic_moving->
			process_generic->
			title /* process_name */ );
	
	if ( process_generic_moving->
		process_generic->
		process_generic_input->
		output_medium == table )
	{
		html_table_output(
			process_generic_moving->
				process_generic_moving_table->
				html_table,
				HTML_TABLE_ROWS_BETWEEN_HEADING );
	}
	else
	if ( process_generic_moving->
		process_generic->
		process_generic_input->
		output_medium == gracechart )
	{
		grace_single_graph_pdf_create(
			process_generic_moving->
				process_generic_moving_grace->
				grace_generic->
				grace_setup->
				grace_ps_system_string,
			process_generic_moving->
				process_generic_moving_grace->
				grace_generic->
				grace_setup->
				grace_page,
			process_generic_moving->
				process_generic_moving_grace->
				grace_generic->
				grace_setup->
				grace_title,
			process_generic_moving->
				process_generic_moving_grace->
				grace_generic->
				grace_graph,
			process_generic_moving->
				process_generic_moving_grace->
				grace_generic->
				grace_setup->
				grace_pdf_system_string );

		printf(	"%s\n",
			process_generic_moving->
				process_generic_moving_grace->
				grace_window->
				html );
	}
	else
	if ( process_generic_moving->
		process_generic->
		process_generic_input->
		output_medium == spreadsheet )
	{
		FILE *output_file;

		output_file =
			/* Safely returns */
			/* -------------- */
			appaserver_output_file(
				process_generic_moving->
					process_generic_moving_spreadsheet->
					spreadsheet->
					output_filename );

		spreadsheet_output(
			process_generic_moving->
				process_generic_moving_spreadsheet->
				spreadsheet,
				output_file );

		fclose( output_file );

		printf(	"%s\n",
			process_generic_moving->
				process_generic_moving_spreadsheet->
				spreadsheet->
				appaserver_link_anchor_html );
	}
	else
	if ( process_generic_moving->
		process_generic->
		process_generic_input->
		output_medium == googlechart )
	{
		google_generic_create(
			process_generic_moving->
				process_generic_moving_google->
				google_generic->
				google_filename->
				http_output_filename,
			process_generic_moving->
				process_generic_moving_google->
				google_generic->
				google_single_chart_list_html );
	
		printf(	"%s\n",
			process_generic_moving->
				process_generic_moving_google->
				google_window->
				html );
	}

	document_close();

	return 0;
}
