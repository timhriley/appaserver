/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/budget_report.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "date.h"
#include "document.h"
#include "statement.h"
#include "budget.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *output_medium_string;
	char *as_of_date_string;
	APPASERVER_PARAMETER *appaserver_parameter;
	BUDGET *budget;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf(stderr,
			"Usage: %s process as_of_date output_medium\n",
			argv[ 0 ] );

		fprintf(stderr,
"Note: output_medium={table,spreadsheet,PDF,stdout}\n" );

		exit ( 1 );
	}

	process_name = argv[ 1 ];
	as_of_date_string = argv[ 2 ];
	output_medium_string = argv[ 3 ];

	appaserver_parameter = appaserver_parameter_new();

	budget =
		budget_fetch(
			application_name,
			process_name,
			appaserver_parameter->data_directory,
			as_of_date_string,
			output_medium_string );

	if ( !budget )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			process_name /* title */ );

		printf( "<h3>Nothing to display.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if (	budget->statement_output_medium !=
		statement_output_stdout )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			process_name /* title */ );

		printf( "%s\n",
			budget->
				statement->
				statement_caption->
				frame_title );
	}

	if ( budget->budget_latex )
	{
		latex_table_output(
			budget->
				budget_latex->
				statement_link->
				tex_filename,
			budget->
				budget_latex->
				statement_link->
				appaserver_link_working_directory,
			budget->
				budget_latex->
				statement_link->
				pdf_anchor_html,
			budget->
				budget_latex->
				latex,
			budget->
				budget_latex->
				latex_table );
	}
	else
	if ( budget->budget_html )
	{
		statement_html_output(
			budget->budget_html->html_table,
			(char *)0 /* secondary_title */ );
	}

	if (	budget->statement_output_medium !=
		statement_output_stdout )
	{
		document_close();
	}

	return 0;
}
