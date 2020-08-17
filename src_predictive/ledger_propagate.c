/* ---------------------------------------------------------------	*/
/* src_predictive/ledger_propagate.c					*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "bank_upload.h"
#include "list.h"
#include "environ.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void ledger_propagate_each_account(	char *application_name,
					char *propagate_transaction_date_time );

/* Global variables */
/* ---------------- */
enum bank_upload_exception bank_upload_exception = {0};

int main( int argc, char **argv )
{
	char *application_name;
	char *transaction_date_time;
	char *preupdate_transaction_date_time;
	char *propagate_transaction_date_time;
	char propagate_transaction_date[ 16 ];
	char *account_name = {0};

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc < 4 )
	{
		fprintf( stderr, 
"Usage: %s ignored transaction_date_time preupdate_transaction_date_time [account ...]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	transaction_date_time = argv[ 2 ];
	preupdate_transaction_date_time = argv[ 3 ];

	if ( strcmp( transaction_date_time, "transaction_date_time" ) == 0 )
		transaction_date_time = "";

	if ( strcmp(	preupdate_transaction_date_time,
			"preupdate_transaction_date_time" ) == 0 )
	{
		preupdate_transaction_date_time = "";
	}

	/* Take the earlier of the change in transaction_date_time */
	/* ------------------------------------------------------- */
	if ( *preupdate_transaction_date_time
	&&   strcmp(	preupdate_transaction_date_time,
				transaction_date_time ) < 0 )
	{
		propagate_transaction_date_time =
			preupdate_transaction_date_time;
	}
	else
	{
		propagate_transaction_date_time =
			transaction_date_time;
	}

	column(	propagate_transaction_date,
		0,
		propagate_transaction_date_time );

	bank_upload_transaction_balance_propagate(
		propagate_transaction_date
			/* bank_date */ );

	/* ---------------------------------------------------- */
	/* If TRANSACTION.transaction_date_time was changed.	*/
	/* ---------------------------------------------------- */
	if ( argc == 6
	&&   *transaction_date_time
	&&   strcmp( argv[ 4 ], "account" ) == 0
	&&   strcmp( argv[ 5 ], "preupdate_account" ) == 0 )
	{
		LIST *account_name_list;

		account_name_list =
			ledger_transaction_date_time_account_name_list(
				application_name,
				transaction_date_time );

		if ( list_rewind( account_name_list ) )
		{
			char *account_name;
			do {
				account_name =
					list_get_pointer(
						account_name_list );

				ledger_propagate(
					application_name,
					propagate_transaction_date_time,
					account_name );

			} while( list_next( account_name_list ) );
		}
	}
	else
	if ( argc == 6
	&&   strcmp( argv[ 4 ], "account" ) == 0
	&&   strcmp( argv[ 5 ], "preupdate_account" ) == 0 )
	{
		ledger_propagate_each_account(
			application_name,
			propagate_transaction_date_time );
	}
	else
	if ( argc < 5 )

	{
		ledger_propagate_each_account(
			application_name,
			propagate_transaction_date_time );
	}
	else
	{
		int i = 4;

		while( i < argc )
		{
			account_name = argv[ i ];

			if ( strcmp(	account_name,
					"preupdate_account" ) != 0
			&&   strcmp(	account_name,
					"account" ) != 0 )
			{
				ledger_propagate(
					application_name,
					propagate_transaction_date_time,
					account_name );
			}
			i++;
		}
	}

	return 0;

} /* main() */

void ledger_propagate_each_account(	char *application_name,
					char *propagate_transaction_date_time )
{
	char sys_string[ 1024 ];
	char account[ 256 ];
	FILE *input_pipe;

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=account		"
		 "			folder=account		",
		 application_name );

	input_pipe = popen( sys_string, "r" );

	while( get_line( account, input_pipe ) )
	{
		ledger_propagate(
			application_name,
			propagate_transaction_date_time,
			account );
	}

	pclose( input_pipe );

} /* ledger_propagate_each_account() */

