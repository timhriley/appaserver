/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/bank_upload_transaction_insert.c	*/
/* ----------------------------------------------------------------	*/
/* Input: BANK_UPLOAD and JOURNAL_LEDGER where account is cash.		*/
/* Output: Insert into BANK_UPLOAD_TRANSACTION statements.		*/
/* ----------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "date.h"
#include "environ.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "bank_upload.h"

/* Constants */
/* --------- */

/* Global variables */
/* ---------------- */
enum bank_upload_exception bank_upload_exception = {0};

/* Prototypes */
/* ---------- */
void seek_withdrawal(		char *application_name,
				char *fund_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *fund_name = {0};
	char *operation;
	char bank_date[ 128 ];
	char bank_description_embedded[ 1024 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_date_time[ 64 ];
	int delimiter_count;

	if ( argc < 2 )
	{
		fprintf( stderr,
"Usage: %s bank_date^bank_description_embedded^full_name^street_address^transaction_date_time [fund]\n",
			 argv[ 0 ] );

		exit ( 1 );
	}

	operation = argv[ 1 ];

	if ( argc == 3 ) fund_name = argv[ 2 ];

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	delimiter_count =
		timlib_count_delimiters(
			'^', operation );

	/* ---------------------------- */
	/* This is called from		*/
	/* direct_transaction_assign.sh	*/
	/* ---------------------------- */
	if ( delimiter_count == 4 )
	{
		piece( bank_date, '^', operation, 0 );

		piece( bank_description_embedded, '^', operation, 1 );

		piece( full_name, '^', operation, 2 );
		piece( street_address, '^', operation, 3 );
		piece( transaction_date_time, '^', operation, 4 );

		bank_upload_transaction_direct_insert(
			bank_date,
			bank_description_embedded,
			full_name,
			street_address,
			transaction_date_time );
	}

	return 0;

} /* main() */

