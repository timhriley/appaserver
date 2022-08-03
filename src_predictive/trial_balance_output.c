/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/trial_balance_output.c		*/
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
#include "trial_balance.h"

#define TRIAL_BALANCE_TITLE		"Trial Balance"
#define TRIAL_BALANCE_PRECLOSE_TITLE	"Trial Balance (preclose)"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *process_name;
	char *subclassification_option_string;
	char *output_medium_string;
	char *as_of_date;
	int prior_year_count;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	TRIAL_BALANCE *trial_balance;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
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
	as_of_date = argv[ 5 ];
	prior_year_count = atoi( argv[ 6 ] );
	subclassification_option_string = argv[ 7 ];
	output_medium_string = argv[ 8 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	trial_balance =
		trial_balance_fetch(
			application_name,
			session_key,
			login_name,
			role_name,
			process_name,
			appaserver_parameter_file->document_root,
			as_of_date,
			prior_year_count,
			subclassification_option_string,
			output_medium_string );

	if ( !trial_balance )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: trial_balance_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( trial_balance->statement_output_medium != output_stdout )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );
	}

	if ( trial_balance->statement_subclassification_option ==
			subclassification_display
	&& trial_balance->trial_balance_pdf )
	{
		char *date_time_string =
			date_get_now_yyyy_mm_dd_hh_mm(
				date_utc_offset() );

		printf( "%s\n",
			trial_balance_pdf_title(
				process_name,
				date_time_string ) );

		if ( trial_balance->
			trial_balance_pdf->
			preclose_trial_balance_latex )
		{
			statement_latex_output(
				trial_balance->
					trial_balance_pdf->
					preclose_trial_balance_latex->
					trial_balance_subclassification_latex->
					latex,
				trial_balance->
					trial_balance_pdf->
					preclose_statement_link->
					ftp_output_filename,
				TRIAL_BALANCE_PRECLOSE_TITLE /* prompt */,
				process_name,
				date_time_string );
		}

		statement_latex_output(
			trial_balance->
				trial_balance_pdf->
				trial_balance_latex->
				trial_balance_subclassification_latex->
				latex,
			trial_balance->
				trial_balance_pdf->
				statement_link->
				ftp_output_filename,
			TRIAL_BALANCE_TITLE /* prompt */,
			process_name,
			date_time_string );
	}
	else
	if ( trial_balance->statement_subclassification_option ==
			subclassification_display
	&&   trial_balance->trial_balance_table )
	{
		if ( trial_balance->
			trial_balance_table->
			preclose_trial_balance_html )
		{
			statement_html_output(
				trial_balance->
					trial_balance_table->
					preclose_trial_balance_html->
					trial_balance_subclassification_html->
					html_table,
				TRIAL_BALANCE_PRECLOSE_TITLE /* title */ );
		}

		statement_html_output(
			trial_balance->
				trial_balance_table->
				trial_balance_html->
				trial_balance_subclassification_html->
				html_table,
			TRIAL_BALANCE_TITLE /* title */ );
	}
	else
	if ( trial_balance->statement_subclassification_option ==
			subclassification_omit
	&& trial_balance->trial_balance_pdf )
	{
		char *date_time_string =
			date_get_now_yyyy_mm_dd_hh_mm(
				date_utc_offset() );

		printf( "%s\n",
			trial_balance_pdf_title(
				process_name,
				date_time_string ) );

		if ( trial_balance->
			trial_balance_pdf->
			preclose_trial_balance_latex )
		{
			statement_latex_output(
				trial_balance->
					trial_balance_pdf->
					preclose_trial_balance_latex->
					trial_balance_account_latex->
					latex,
				trial_balance->
					trial_balance_pdf->
					preclose_statement_link->
					ftp_output_filename,
				TRIAL_BALANCE_PRECLOSE_TITLE
					/* prompt */,
				process_name,
				date_time_string );
		}

		statement_latex_output(
			trial_balance->
				trial_balance_pdf->
				trial_balance_latex->
				trial_balance_account_latex->
				latex,
			trial_balance->
				trial_balance_pdf->
				statement_link->
				ftp_output_filename,
			TRIAL_BALANCE_TITLE /* prompt */,
			process_name,
			date_time_string );
	}
	else
	if ( trial_balance->statement_subclassification_option ==
			subclassification_omit
	&& trial_balance->trial_balance_table )
	{
		if ( trial_balance->
			trial_balance_table->
			preclose_trial_balance_html )
		{
			statement_html_output(
				trial_balance->
					trial_balance_table->
					preclose_trial_balance_html->
					trial_balance_account_html->
					html_table,
				TRIAL_BALANCE_PRECLOSE_TITLE /* title */ );
		}

		statement_html_output(
			trial_balance->
				trial_balance_table->
				trial_balance_html->
				trial_balance_account_html->
				html_table,
			TRIAL_BALANCE_TITLE /* title */ );
	}

	if ( trial_balance->statement_output_medium != output_stdout )
	{
		document_close();
	}

	return 0;
}
