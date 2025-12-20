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
	char *session_key;
	char *process_name;
	char *output_medium_string;
	APPASERVER_PARAMETER *appaserver_parameter;
	BUDGET *budget;
	BUDGET_ANNUALIZED *budget_annualized;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf(stderr,
			"Usage: %s session process output_medium\n",
			argv[ 0 ] );

		fprintf(stderr,
"Note: output_medium={table,spreadsheet,PDF,stdout}\n" );

		exit ( 1 );
	}

	session_key = argv[ 1 ];
	process_name = argv[ 2 ];
	output_medium_string = argv[ 3 ];

	appaserver_parameter = appaserver_parameter_new();

	budget =
		budget_fetch(
			application_name,
			session_key,
			process_name,
			(char *)0 /* fund_name */,
			output_medium_string,
			appaserver_parameter->data_directory );

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
			(char *)0 /* title */ );

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
				tex_anchor_html,
			budget->
				budget_latex->
				statement_link->
				pdf_anchor_html,
			budget->
				budget_latex->
				statement_link->
				appaserver_link_working_directory,
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
	else
	{
		printf(
		"<h3>Sorry, but this output option isn't written yet.</h3>\n" );
	}

	if ( list_rewind( budget->budget_annualized_list ) )
	do {
		budget_annualized =
			list_get(
				budget->
					budget_annualized_list );

		if ( !budget_annualized->
			budget_regression
		||   !budget_annualized->
			budget_regression->
			budget_link
		||   !budget_annualized->
			budget_regression->
			budget_link->
			pdf_anchor_html )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"budget_annualizedl is incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( budget_annualized->budget_regression->confidence_integer )
		{
			printf( "%s\n",
				budget_annualized->
					budget_regression->
					budget_link->
					pdf_anchor_html );
		}

	} while ( list_next( budget->budget_annualized_list ) );

	if (	budget->statement_output_medium !=
		statement_output_stdout )
	{
		document_close();
	}

	return 0;
}
