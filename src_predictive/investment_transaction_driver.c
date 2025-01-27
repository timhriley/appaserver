/* -------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/investment_transaction_driver.c	*/
/* -------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "environ.h"
#include "appaserver_error.h"
#include "investment_transaction.h"

int main( int argc, char **argv )
{
	char *application_name;
	double investment_account_sum;
	boolean execute_boolean;
	INVESTMENT_TRANSACTION *investment_transaction;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s investment_account_sum execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	investment_account_sum = atof( argv[ 1 ] );
	execute_boolean = (*argv[ 2 ] == 'y');

	investment_transaction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		investment_transaction_new(
			investment_account_sum );

	if ( !investment_transaction->transaction )
	{
		printf(
			"%s\n",
			INVESTMENT_TRANSACTION_NO_CHANGE );
	
		exit( 0 );
	}

	if ( execute_boolean )
	{
		investment_transaction->
			transaction->
			transaction_date_time =
				/* -------------------------------------- */
				/* Returns inserted transaction_date_time */
				/* -------------------------------------- */
				transaction_insert(
					investment_transaction->
						transaction->
						full_name,
					investment_transaction->
						transaction->
						street_address,
					investment_transaction->
						transaction->
						transaction_date_time,
					investment_transaction->
						transaction->
						transaction_amount,
					0 /* check_number */,
					investment_transaction->
						transaction->
						memo,
					'n' /* lock_transaction_yn not there */,
					investment_transaction->
						transaction->
						journal_list,
					1 /* insert_journal_list_boolean */ );
	}

	transaction_html_display( investment_transaction->transaction );

	return 0;
}

