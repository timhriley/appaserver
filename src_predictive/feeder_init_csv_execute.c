/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/feeder_init_execute.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "document.h"
#include "sql.h"
#include "security.h"
#include "exchange_csv.h"
#include "import_predict.h"
#include "feeder_init.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *process_name;
	char *full_name;
	char *street_address;
	char *account_type;
	char *csv_format_filename;
	boolean execute_boolean;
	boolean checking_boolean;
	boolean okay_continue = 1;
	EXCHANGE_CSV *exchange_csv = {0};
	FEEDER_INIT *feeder_init = {0};
	char *error_string;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 10 )
	{
		fprintf(stderr,
"Usage: %s session login_name role process full_name street_address account_type csv_format_filename execute_yn\n",
			argv[ 0 ] );

		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	process_name = argv[ 4 ];
	full_name = argv[ 5 ];
	street_address = argv[ 6 ];
	account_type = argv[ 7 ];
	csv_format_filename = argv[ 8 ];
	execute_boolean = (*argv[ 9 ] == 'y');

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name /* title */ );

	if ( !*full_name
	||   strcmp(
		full_name,
		"full_name" ) == 0 )
	{
		printf(	"%s\n",
			FEEDER_INIT_INSTITUTION_MISSING_MESSAGE );

		okay_continue = 0;
	}

	if ( !*account_type
	||   strcmp(
		account_type,
		"account_type" ) == 0 )
	{
		printf(
		"<h3>Please select an account type.</h3>\n" );

		okay_continue = 0;
	}

	if ( !*csv_format_filename
	||   strcmp(
		csv_format_filename,
		"filename" ) == 0 )
	{
		printf(
		"<h3>Please transmit a CSV formatted file.</h3>\n" );

		okay_continue = 0;
	}

	if ( okay_continue )
	{
		exchange_csv =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			exchange_csv_fetch(
				application_name,
				csv_format_filename,
				date_column /* one based */,
				description_column /* one based */,
				debit_column /* one based */,
				credit_column /* one based */,
				balance_column /* one based */,
				reference_column /* one based */,
				reverse_order_boolean,
				balance_amount /* optional */,
				appaserver_parameter_upload_directory() );
	}

	if ( exchange_csv
	&&   !list_length( exchange_csv->exchange_journal_list ) )
	{
		printf(
"<h3>Sorry, but this exchange formatted file doesn't have any transactions.</h3>\n" );
		okay_continue = 0;
	}

	checking_boolean = ( strcmp( account_type, "checking" ) == 0 );

	if ( okay_continue )
	{
		feeder_init =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			feeder_init_new(
				application_name,
				session_key,
				login_name,
				role_name,
				full_name
				/* financial_institution_full_name */,
				street_address
				/* financial_institution_street_address */,
				checking_boolean,
				exchange->exchange_journal_begin_amount,
				exchange->minimum_date_string );
	}

	feeder_init_process(
		application_name,
		login_name,
		execute_boolean,
		checking_boolean,
		(exchange)
			? exchange->exchange_journal_begin_amount
			: 0.0,
		(exchange)
			? exchange->minimum_date_string
			: NULL,
		feeder_init );

	document_close();

	return 0;
}

