/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/ledger_propagate.c			*/
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
#include "list.h"
#include "environ.h"
#include "appaserver_parameter_file.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "transaction.h"
#include "journal.h"
#include "account.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *transaction_date_time;
	char *preupdate_transaction_date_time = "";
	char *propagate_transaction_date_time = {0};

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 2 )
	{
		fprintf( stderr, 
"Usage: %s transaction_date_time [preupdate_transaction_date_time] [account ...]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	transaction_date_time = argv[ 1 ];

/*
	if ( !*transaction_date_time
	||   strcmp( transaction_date_time, "transaction_date_time" ) == 0 )
	{
		transaction_date_time =
			pipe2string(
				"transaction_date_time_minimum.sh" );
	}
*/

	if ( argc >= 3 )
	{
		preupdate_transaction_date_time = argv[ 2 ];

		if ( strcmp( 
			preupdate_transaction_date_time,
			"preupdate_transaction_date_time" ) == 0 )
		{
			preupdate_transaction_date_time = "";
		}
	}

	/* Take the earlier of the two */
	/* --------------------------- */
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

	/* If every account */
	/* ---------------- */
	if (	argc < 4
	|| 	( argc >= 4 && strcmp( argv[ 3 ], "account" ) == 0 ) )
	{
		LIST *account_name_list;
		char *account_name;

		account_name_list =
			journal_date_time_account_name_list(
				JOURNAL_TABLE,
				propagate_transaction_date_time );

		if ( list_rewind( account_name_list ) )
		{
			do {
				account_name =
					list_get(
						account_name_list );

				journal_propagate(
					propagate_transaction_date_time,
					account_name );

			} while( list_next( account_name_list ) );
		}
	}
	else
	/* ---------------------- */
	/* If listed the accounts */
	/* ---------------------- */
	if ( argc >= 4 )
	{
		int i;
		char *account_name;

		for(	i = 3;
			i < argc;
			i++ )
		{
			account_name = argv[ i ];

			journal_propagate(
				propagate_transaction_date_time,
				account_name );
			i++;
		}
	}
	return 0;
}

