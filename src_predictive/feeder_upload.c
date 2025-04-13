/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_upload.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "document.h"
#include "list.h"
#include "environ.h"
#include "process.h"
#include "application.h"
#include "exchange.h"
#include "feeder.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *login_name;
	char *feeder_account_name;
	char *exchange_format_filename;
	boolean execute_boolean;
	EXCHANGE *exchange = {0};
	FEEDER *feeder = {0};
	FEEDER_AUDIT *feeder_audit;

	application_name =
		environment_exit_application_name(
			argv[ 0 ] );

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s process_name login_name feeder_account excxhange_format_filename execute_yn\n",
			 argv[ 0 ] );

		exit ( 1 );
	}

	process_name = argv[ 1 ];
	login_name = argv[ 2 ];
	feeder_account_name = argv[ 3 ];
	exchange_format_filename = argv[ 4 ];
	execute_boolean = (*argv[ 5 ] == 'y');

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

	if ( *exchange_format_filename
	&&   strcmp(
		exchange_format_filename,
		"exchange_format_filename" ) != 0 )
	{
		exchange =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			exchange_parse(
				application_name,
				exchange_format_filename,
				appaserver_parameter_upload_directory() );
	}

	if ( exchange )
	{
		if ( !exchange->open_tag_boolean )
		{
			printf(
		"<h3>Sorry, but this file is not in exchange format.</h3>\n" );
			document_close();
			exit( 0 );
		}

		if ( !list_length( exchange->exchange_journal_list ) )
		{
			printf(
"<h3>Sorry, but this exchange formatted file doesn't have any transactions.</h3>\n" );
			document_close();
			exit( 0 );
		}

		feeder =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			feeder_fetch(
				application_name,
				login_name,
				feeder_account_name,
				exchange_format_filename,
				exchange->exchange_journal_list,
				exchange->balance_amount,
				exchange->minimum_date_string );
	}

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
				feeder_account_name );

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

