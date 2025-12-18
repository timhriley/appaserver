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
	char *fund_name;
	char *feeder_account_name;
	char *exchange_format_filename;
	boolean execute_boolean;
	boolean okay_continue = 1;
	EXCHANGE *exchange = {0};
	FEEDER *feeder = {0};

	application_name =
		environment_exit_application_name(
			argv[ 0 ] );

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s process_name login_name fund feeder_account exchange_format_filename execute_yn\n",
			 argv[ 0 ] );

		exit ( 1 );
	}

	process_name = argv[ 1 ];
	login_name = argv[ 2 ];
	fund_name = argv[ 3 ];
	feeder_account_name = argv[ 4 ];
	exchange_format_filename = argv[ 5 ];
	execute_boolean = (*argv[ 6 ] == 'y');

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

		okay_continue = 0;
	}

	if ( okay_continue
	&&   *exchange_format_filename
	&&   strcmp(
		exchange_format_filename,
		"exchange_format_filename" ) != 0 )
	{
		exchange =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			exchange_fetch(
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

			okay_continue = 0;
		}

		if ( okay_continue
		&&   !list_length( exchange->exchange_journal_list ) )
		{
			printf(
"<h3>Sorry, but this exchange formatted file doesn't have any transactions.</h3>\n" );

			okay_continue = 0;
		}

		if ( okay_continue )
		{
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
					exchange->exchange_journal_begin_amount,
					exchange->balance_amount,
					exchange->minimum_date_string );
		}
	}

	feeder_process(
		application_name,
		process_name,
		login_name,
		fund_name,
		feeder_account_name,
		execute_boolean,
		(exchange)
			? exchange->exchange_journal_begin_amount
			: 0.0,
		feeder );

	document_close();

	return 0;
}

