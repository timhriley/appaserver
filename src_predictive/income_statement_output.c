/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/income_statement_output.c		*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "date.h"
#include "document.h"
#include "statement.h"
#include "income_statement.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *process_name;
	char *subclassification_option_string;
	char *output_medium_string;
	char *as_of_date_string;
	int prior_year_count;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	INCOME_STATEMENT *income_statement;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 9 )
	{
		fprintf(stderr,
"Usage: %s session login_name role process as_of_date prior_year_count subclassification_option output_medium\n",
			argv[ 0 ] );

		fprintf(stderr,
"Note: subclassification_option={display,omit,aggregate}\n" );

		fprintf(stderr,
"Note: output_medium={table,spreadsheet,PDF,stdout}\n" );

		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	process_name = argv[ 4 ];
	as_of_date_string = argv[ 5 ];
	prior_year_count = atoi( argv[ 6 ] );
	subclassification_option_string = argv[ 7 ];
	output_medium_string = argv[ 8 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	income_statement =
		income_statement_fetch(
			argv[ 0 ],
			application_name,
			session_key,
			login_name,
			role_name,
			process_name,
			appaserver_parameter_file->document_root,
			as_of_date_string,
			prior_year_count,
			subclassification_option_string,
			output_medium_string );

	if ( !income_statement )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf( "<h3>Nothing to display.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if (	income_statement->statement_output_medium !=
		statement_output_stdout )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf( "%s\n",
			income_statement->
				statement->
				statement_caption->
				frame_title );
	}

	if ( income_statement->income_statement_pdf )
	{
		statement_latex_output(
			income_statement->
				income_statement_pdf->
				income_statement_latex->
				latex,
			income_statement->
				income_statement_pdf->
				statement_link->
				ftp_output_filename,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			statement_pdf_prompt( process_name ),
			process_name,
			income_statement->
				statement->
				statement_caption->
				date_time_string );
	}
	else
	if ( income_statement->income_statement_html )
	{
		statement_html_output(
			income_statement->
				income_statement_html->
				html_table,
			(char *)0 /* secondary_title */ );
	}

	if (	income_statement->statement_output_medium !=
		statement_output_stdout )
	{
		document_close();
	}

	return 0;
}
