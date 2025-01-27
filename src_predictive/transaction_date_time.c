/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/transaction_date_time.c		*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "environ.h"
#include "date.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "transaction_balance.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

/* Global variables */
/* ---------------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *transaction_date_time;
	boolean prior = 0;
	TRANSACTION_BALANCE_ROW *transaction_balance_row;

	/* Exits if fails */
	/* -------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	if ( argc < 2 )
	{
		fprintf( stderr,
			 "Usage: %s transaction_date_time [prior]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	transaction_date_time = argv[ 1 ];

	if ( argc == 3
	&&   strcmp( argv[ 2 ], "prior" ) == 0 )
	{
		prior = 1;
	}

	if ( prior )
	{
		transaction_balance_row =
			transaction_balance_prior_fetch(
				application_name,
				transaction_date_time );
	}
	else
	{
		transaction_balance_row =
			transaction_balance_transaction_date_time_fetch(
				application_name,
				transaction_date_time );
	}

	if ( transaction_balance_row )
	{
		transaction_balance_row_stdout( transaction_balance_row );
	}
	else
	{
		printf( "No transaction.\n" );
	}

	return 0;

} /* main() */

