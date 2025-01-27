/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_load.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "appaserver_error.h"
#include "document.h"
#include "list.h"
#include "environ.h"
#include "process.h"
#include "application.h"
#include "feeder.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *login_name;
	char *feeder_account_name;
	char *feeder_load_filename;
	int date_column;
	int description_column;
	int debit_column;
	int credit_column;
	int balance_column;
	int reference_column;
	boolean reverse_order_boolean;
	double parameter_end_balance = 0.0;
	boolean parameter_end_balance_not_null_boolean = 0;
	boolean execute_boolean;
	FEEDER *feeder;
	FEEDER_AUDIT *feeder_audit;

	application_name =
		environment_exit_application_name(
			argv[ 0 ] );

	if ( argc != 14 )
	{
		fprintf( stderr,
"Usage: %s process_name login_name feeder_account filename date_column description_column debit_column credit_column balance_column reference_column reverse_order_yn account_end_balance execute_yn\n",
			 argv[ 0 ] );

		fprintf( stderr,
"\nNotes: Column numbers are one based. Delimiters are either comma only, or quote-comma.\n" );

		exit ( 1 );
	}

	process_name = argv[ 1 ];
	login_name = argv[ 2 ];
	feeder_account_name = argv[ 3 ];
	feeder_load_filename = argv[ 4 ];
	date_column = atoi( argv[ 5 ] );
	description_column = atoi( argv[ 6 ] );
	debit_column = atoi( argv[ 7 ] );
	credit_column = atoi( argv[ 8 ] );
	balance_column = atoi( argv[ 9 ] );
	reference_column = atoi( argv[ 10 ] );
	reverse_order_boolean = (*argv[ 11 ] == 'y');

	if ( isdigit( *argv[ 12 ] ) || ( *argv[ 12 ] == '-' ) )
	{
		parameter_end_balance = atof( argv[ 12 ] );
		parameter_end_balance_not_null_boolean = 1;
	}

	execute_boolean = (*argv[ 13 ] == 'y');

	appaserver_error_argv_file(
		argc,
		argv,
		application_name,
		login_name );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name /* title */ );

	if ( !*feeder_account_name
	||   strcmp( feeder_account_name, "feeder_account" ) == 0 )
	{
		printf( "<h3>Please choose a feeder account.</h3>\n" );
		document_close();
		exit( 0 );
	}

	feeder =
		feeder_fetch(
			application_name,
			login_name,
			feeder_account_name,
			feeder_load_filename,
			date_column,
			description_column,
			debit_column,
			credit_column,
			balance_column,
			reference_column,
			reverse_order_boolean,
			parameter_end_balance,
			parameter_end_balance_not_null_boolean );

	if ( feeder )
	{
		if ( !feeder->feeder_row_count )
		{
			printf( "<h3>No new feeder rows to process.</h3>\n" );
		}
		else
		if ( execute_boolean )
		{
			feeder_execute( process_name, feeder );
		}
		else
		{
			feeder_display( feeder );
		}
	}

	if ( !feeder || execute_boolean )
	{
		feeder_audit =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			feeder_audit_fetch(
				application_name,
				login_name,
				feeder_account_name,
				reverse_order_boolean );

		if ( !feeder_audit->feeder_load_event )
		{
			printf(
			"<h3>Warning: no feeder load events.</h3>\n" );
		}
		else
		if ( !feeder_audit->feeder_row_final_number )
		{
			printf(
			"<h3>ERROR: feeder_row_final_number is empty.</h3>\n" );
		}
		else
		if ( !feeder_audit->journal_latest )
		{
			printf(
			"<h3>Warning: "
			"no journal entries exist for account=%s"
			"</h3>\n",
			feeder_account_name );
		}
		else
		if ( !feeder_audit->html_table )
		{
			printf(
			"<h3>ERROR: html_table is empty.</h3>\n" );
		}
		else
		{
			html_table_output(
				feeder_audit->html_table,
				HTML_TABLE_ROWS_BETWEEN_HEADING );
		}
	}

	document_close();

	return 0;
}

