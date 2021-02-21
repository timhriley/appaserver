/* --------------------------------------------------------	*/
/* $APPASERVER_HOME/src_education/paypal_sweep_trigger.c	*/
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
#include "entity_self.h"
#include "transaction.h"
#include "account.h"
#include "product.h"
#include "journal.h"
#include "paypal_sweep.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void paypal_sweep_trigger_predelete(
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time );

void paypal_sweep_trigger_insert_update(
			PAYPAL_SWEEP *paypal_sweep );

int main( int argc, char **argv )
{
	char *application_name;
	char *payor_full_name;
	char *payor_street_address;
	char *paypal_date_time;
	char *preupdate_transaction_date_time;
	char *state;
	PAYPAL_SWEEP *paypal_sweep;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf(stderr,
"Usage: %s payor_full_name payor_street_address paypal_date_time preupdate_transaction_date_time state\n",
			 argv[ 0 ] );
		fprintf(stderr,
			"state in {insert,update,predelete,delete}\n" );
		exit ( 1 );
	}

	payor_full_name = argv[ 1 ];
	payor_street_address = argv[ 2 ];
	paypal_date_time = argv[ 3 ];
	preupdate_transaction_date_time = argv[ 4 ];
	state = argv[ 5 ];

	if ( strcmp( state, "predelete" ) == 0 )
	{
		paypal_sweep_trigger_predelete(
			payor_full_name,
			payor_street_address,
			paypal_date_time );
		exit( 0 );
	}

	if ( ! ( paypal_sweep =
			paypal_sweep_fetch(
				payor_full_name,
				payor_street_address,
				paypal_date_time,
				1 /* fetch_transaction */ ) ) )
	{
		exit( 0 );
	}

	if ( transaction_date_time_changed(
			preupdate_transaction_date_time )
	&&   paypal_sweep->paypal_sweep_transaction )
	{
		journal_account_name_list_propagate(
			transaction_date_time_earlier(
				paypal_sweep->transaction_date_time,
				preupdate_transaction_date_time ),
			journal_list_account_name_list(
				paypal_sweep->
					paypal_sweep_transaction->
					journal_list ) );
	}

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0 )
	{
		paypal_sweep_trigger_insert_update(
			paypal_sweep );
	}

	return 0;
}

void paypal_sweep_trigger_insert_update(
			PAYPAL_SWEEP *paypal_sweep )
{
	int transaction_seconds_to_add = 0;

	if ( !paypal_sweep->transaction_date_time
	||   !*paypal_sweep->transaction_date_time )
	{
		paypal_sweep->transaction_date_time =
			transaction_race_free(
				paypal_sweep->
					paypal_date_time );
	}

	if ( ( paypal_sweep->paypal_sweep_transaction =
		paypal_sweep_transaction(
			&transaction_seconds_to_add,
			paypal_sweep->payor_full_name,
			paypal_sweep->payor_street_address,
			paypal_sweep->paypal_date_time,
			paypal_sweep->sweep_amount,
			entity_self_paypal_cash_account_name(),
			account_cash( (char *)0 ) ) ) )
	{
		paypal_sweep->transaction_date_time =
			paypal_sweep->paypal_sweep_transaction->
				transaction_date_time;
	}
	else
	{
		paypal_sweep->transaction_date_time = (char *)0;
	}

	if ( paypal_sweep->transaction_date_time
	&&  *paypal_sweep->transaction_date_time )
	{
		TRANSACTION *t = paypal_sweep->paypal_sweep_transaction;

		paypal_sweep->transaction_date_time =
			transaction_refresh(
				t->full_name,
				t->street_address,
				t->transaction_date_time,
				t->transaction_amount,
				t->memo,
				0 /* check_number */,
				t->lock_transaction,
				t->journal_list );
	}

	paypal_sweep_update(
		paypal_sweep->transaction_date_time,
		paypal_sweep->payor_full_name,
		paypal_sweep->payor_street_address,
		paypal_sweep->paypal_date_time );
}

void paypal_sweep_trigger_predelete(
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time )
{
	PAYPAL_SWEEP *paypal_sweep;

	if ( ! ( paypal_sweep =
			paypal_sweep_fetch(
				payor_full_name,
				payor_street_address,
				paypal_date_time,
				0 /* not fetch_transaction */ ) ) )
	{
		return;
	}

	if ( paypal_sweep->transaction_date_time
	&&   *paypal_sweep->transaction_date_time )
	{
		transaction_delete(
			paypal_sweep->payor_full_name,
			paypal_sweep->payor_street_address,
			paypal_sweep->transaction_date_time );

		journal_account_name_list_propagate(
			paypal_sweep->transaction_date_time,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			journal_delete(
				paypal_sweep->payor_full_name,
				paypal_sweep->payor_street_address,
				paypal_sweep->transaction_date_time ) );
	}
}
