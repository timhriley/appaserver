/* --------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/journal_ledger_trigger.c	*/
/* --------------------------------------------------------	*/
/* 								*/
/* Freely available software: see Appaserver.org		*/
/* --------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "account.h"
#include "transaction.h"
#include "journal.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void journal_ledger_trigger_insert(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *account );

void journal_ledger_trigger_update(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *account,
			char *preupdate_transaction_date_time,
			char *preupdate_account );

void journal_ledger_trigger_delete(
			char *full_name,
			char *street_address,
			char *transaction_date_time );

void journal_ledger_trigger_predelete(
			char *transaction_date_time,
			char *account );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *transaction_date_time;
	char *account;
	char *state;
	char *preupdate_transaction_date_time;
	char *preupdate_account;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s full_name street_address transaction_date_time account state preupdate_transaction_date_time preupdate_account\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	transaction_date_time = argv[ 3 ];
	account = argv[ 4 ];
	state = argv[ 5 ];
	preupdate_transaction_date_time = argv[ 6 ];
	preupdate_account = argv[ 7 ];

	if ( strcmp( transaction_date_time, "transaction_date_time" ) == 0 )
		exit( 0 );

	if ( strcmp( state, "insert" ) == 0 )
	{
		journal_ledger_trigger_insert(
			full_name,
			street_address,
			transaction_date_time,
			account );
	}
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		journal_ledger_trigger_update(
			full_name,
			street_address,
			transaction_date_time,
			account,
			preupdate_transaction_date_time,
			preupdate_account );
	}
	else
	if ( strcmp( state, "delete" ) == 0 )
	{
		journal_ledger_trigger_delete(
			full_name,
			street_address,
			transaction_date_time );
	}
	else
	if ( strcmp( state, "predelete" ) == 0 )
	{
		journal_ledger_trigger_predelete(
			transaction_date_time,
			account );
	}

	return 0;
}

void journal_ledger_trigger_update(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *account,
			char *preupdate_transaction_date_time,
			char *preupdate_account )
{
	LIST *account_name_list;

	if ( transaction_date_time_changed(
			preupdate_transaction_date_time ) )
	{
		transaction_date_time =
			transaction_date_time_earlier(
				transaction_date_time,
				preupdate_transaction_date_time );
	}

	if ( account_name_changed( preupdate_account ) )
	{
		/* Executes journal_list_set_balances() */
		/* ------------------------------------ */
		journal_propagate(
			transaction_date_time,
			preupdate_account /* account_name */ );

		journal_propagate(
			transaction_date_time,
			account /* account_name */ );
		return;
	}

	account_name_list =
		journal_account_name_list(
			full_name,
			street_address,
			transaction_date_time );

	if ( !list_rewind( account_name_list ) )
	{
		return;
	}

	do {
		/* Executes journal_list_set_balances() */
		/* ------------------------------------ */
		journal_propagate(
			transaction_date_time,
			list_get( account_name_list )
				 /* account_name */ );

	} while ( list_next( account_name_list ) );

	transaction_amount_fetch_update(
			full_name,
			street_address,
			transaction_date_time );
}

void journal_ledger_trigger_predelete(
			char *transaction_date_time,
			char *account )
{
	/* Executes journal_list_set_balances() */
	/* ------------------------------------ */
	journal_propagate(
		transaction_date_time,
		account /* account_name */ );
}

void journal_ledger_trigger_insert(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *account )
{
	/* Executes journal_list_set_balances() */
	/* ------------------------------------ */
	journal_propagate(
		transaction_date_time,
		account /* account_name */ );

	transaction_amount_fetch_update(
			full_name,
			street_address,
			transaction_date_time );
}

void journal_ledger_trigger_delete(
			char *full_name,
			char *street_address,
			char *transaction_date_time )
{
	transaction_amount_fetch_update(
			full_name,
			street_address,
			transaction_date_time );
}

