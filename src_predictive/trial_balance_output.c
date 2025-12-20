/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/trial_balance_output.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "date.h"
#include "document.h"
#include "statement.h"
#include "trial_balance.h"

#define TRIAL_BALANCE_PROMPT		"Trial Balance"
#define TRIAL_BALANCE_PRECLOSE_PROMPT	"Trial Balance (preclose)"
#define TRIAL_BALANCE_PRECLOSE_TITLE	"(Preclose)"
#define TRIAL_BALANCE_CLOSE_TITLE	"(Postclose)"

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
	TRIAL_BALANCE *trial_balance;

	application_name = environment_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 9 )
	{
		fprintf( stderr,
"Usage: %s session login_name role process as_of_date prior_year_count subclassification_option output_medium\n",
			 argv[ 0 ] );

		fprintf( stderr,
"Note: subclassification_option={display,omit}\n" );

		fprintf( stderr,
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

	trial_balance =
		trial_balance_fetch(
			application_name,
			session_key,
			login_name,
			role_name,
			process_name,
			(char *)0 /* fund_name */,
			appaserver_parameter->data_directory,
			as_of_date_string,
			prior_year_count,
			subclassification_option_string,
			output_medium_string );

	if ( !trial_balance )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			process_name /* title_string */ );

		printf( "<h3>Nothing to display.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( trial_balance->statement_output_medium != statement_output_stdout )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			(char *)0 /* title_string */ );

		printf( "%s\n",
			trial_balance->
				statement->
				statement_caption->
				frame_title );

		if ( trial_balance->statement->greater_year_message )
		{
			printf( "%s\n",
				trial_balance->
					statement->
					greater_year_message );
		}
	}

	if ( trial_balance->trial_balance_pdf )
	{
		if ( trial_balance->
			trial_balance_pdf->
			preclose_trial_balance_latex )
		{
			latex_table_output(
				trial_balance->
					trial_balance_pdf->
					preclose_statement_link->
					tex_filename,
				trial_balance->
					trial_balance_pdf->
					preclose_statement_link->
					tex_anchor_html,
				trial_balance->
					trial_balance_pdf->
					preclose_statement_link->
					pdf_anchor_html,
				trial_balance->
					trial_balance_pdf->
					preclose_statement_link->
					appaserver_link_working_directory,
				trial_balance->
					trial_balance_pdf->
					preclose_trial_balance_latex->
					latex,
				trial_balance->
					trial_balance_pdf->
					preclose_trial_balance_latex->
					latex_table );
		}

		latex_table_output(
			trial_balance->
				trial_balance_pdf->
				statement_link->
				tex_filename,
			trial_balance->
				trial_balance_pdf->
				statement_link->
				tex_anchor_html,
			trial_balance->
				trial_balance_pdf->
				statement_link->
				pdf_anchor_html,
			trial_balance->
				trial_balance_pdf->
				statement_link->
				appaserver_link_working_directory,
			trial_balance->
				trial_balance_pdf->
				trial_balance_latex->
				latex,
			trial_balance->
				trial_balance_pdf->
				trial_balance_latex->
				latex_table );
	}
	else
	if ( trial_balance->trial_balance_table )
	{
		char *postclose_secondary_title = {0};

		if ( trial_balance->
			trial_balance_table->
			preclose_trial_balance_html )
		{
			statement_html_output(
				trial_balance->
					trial_balance_table->
					preclose_trial_balance_html->
					html_table,
				TRIAL_BALANCE_PRECLOSE_TITLE
					/* secondary_title */ );

			postclose_secondary_title =
				TRIAL_BALANCE_CLOSE_TITLE;
		}

		statement_html_output(
			trial_balance->
				trial_balance_table->
				trial_balance_html->
				html_table,
			postclose_secondary_title );
	}

	if ( trial_balance->statement_output_medium != statement_output_stdout )
	{
		document_close();
	}

	return 0;
}
