/* ------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/budget_report.c		*/
/* ------------------------------------------------------------	*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------	*/

#include <stdio.h>
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
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
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	BUDGET *budget;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
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

	appaserver_parameter_file = appaserver_parameter_file_new();

	budget =
		budget_fetch(
			application_name,
			process_name,
			appaserver_parameter_file->document_root,
			as_of_date_string,
			output_medium_string );

	if ( !budget )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf( "<h3>Nothing to display.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if (	budget->statement_output_medium !=
		statement_output_stdout )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf( "%s\n",
			budget->
				statement->
				statement_caption->
				frame_title );
	}

	if ( budget->budget_pdf )
	{
		statement_latex_output(
			budget->
				budget_pdf->
				budget_latex->
				latex,
			budget->
				budget_pdf->
				statement_link->
				ftp_output_filename,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			statement_pdf_prompt( process_name ),
			process_name,
			budget->
				statement->
				statement_caption->
				date_time_string );
	}
	else
	if ( budget->budget_html )
	{
		statement_html_output(
			budget->budget_html->table,
			(char *)0 /* secondary_title */ );
	}

	if (	budget->statement_output_medium !=
		statement_output_stdout )
	{
		document_close();
	}

	return 0;
}
