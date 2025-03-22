/* $APPASERVER_HOME/src_predictive/generate_invoice.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/
/* Note: links to generate_workorder					*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "environ.h"
#include "document.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "invoice_latex.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *customer_full_name;
	char *customer_street_address;
	char *sale_date_time;
	boolean workorder_boolean;
	LIST *line_item_list;
	INVOICE *invoice;
	INVOICE_LATEX *invoice_latex;

	application_name =
		environment_exit_application_name(
			argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf( stderr,
		"Usage: %s process full_name street_address sale_date_time\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	workorder_boolean = ( strcmp( argv[ 0 ], "generate_workorder" ) == 0 );

	process_name = argv[ 1 ];
	customer_full_name = argv[ 2 ];
	customer_street_address = argv[ 3 ];
	sale_date_time = argv[ 4 ];

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name /* title */ );

	line_item_list =
		invoice_line_item_list(
			customer_full_name,
			customer_street_address,
			sale_date_time );

	invoice =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		invoice_new(
			(char *)0 /* invoice_key */,
			(char *)0 /* transaction_date_time_string */,
			sale_date_time
				/* invoice_date_time_string */,
			customer_full_name,
			customer_street_address,
			(workorder_boolean)
				? invoice_workorder
				: invoice_due,
			line_item_list );

	invoice_latex =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		invoice_latex_new(
			application_name,
			process_name,
			appaserver_parameter_data_directory(),
			invoice );

	invoice_latex_output( invoice_latex );

	document_close();

	return 0;
}
