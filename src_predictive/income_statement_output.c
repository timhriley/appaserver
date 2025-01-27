/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/income_statement_output.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software: Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
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
	APPASERVER_PARAMETER *appaserver_parameter;
	INCOME_STATEMENT *income_statement;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
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

	appaserver_parameter = appaserver_parameter_new();

	income_statement =
		income_statement_fetch(
			argv[ 0 ],
			application_name,
			session_key,
			login_name,
			role_name,
			process_name,
			appaserver_parameter->data_directory,
			as_of_date_string,
			prior_year_count,
			subclassification_option_string,
			output_medium_string );

	if ( !income_statement )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			process_name /* title */ );

		printf( "<h3>Nothing to display.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if (	income_statement->statement_output_medium !=
		statement_output_stdout )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			(char *)0 /* title */ );

		printf( "%s\n",
			income_statement->
				statement->
				statement_caption->
				frame_title );
	}

	if ( income_statement->income_statement_latex )
	{
		latex_table_output(
			income_statement->
				income_statement_latex->
				statement_link->
				tex_filename,
			income_statement->
				income_statement_latex->
				statement_link->
				appaserver_link_working_directory,
			income_statement->
				income_statement_latex->
				statement_link->
				pdf_anchor_html,
			income_statement->
				income_statement_latex->
				latex,
			income_statement->
				income_statement_latex->
				latex_table );
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
