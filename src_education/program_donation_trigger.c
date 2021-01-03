/* --------------------------------------------------------	*/
/* $APPASERVER_HOME/src_education/program_donation_trigger.c	*/
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
#include "entity.h"
#include "transaction.h"
#include "account.h"
#include "entity_self.h"
#include "program.h"
#include "journal.h"
#include "program_donation.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void program_donation_trigger_predelete(
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time );

void program_donation_trigger_insert_update(
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *program_name;
	char *payor_full_name;
	char *payor_street_address;
	char *payment_date_time;
	char *state;

	/* Exits if fails. */
	/* --------------- */
	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf(stderr,
"Usage: %s program_name payor_full_name payor_street_address payment_date_time state\n",
			 argv[ 0 ] );
		fprintf(stderr,
			"state in {insert,update,predelete,delete}\n" );
		exit ( 1 );
	}

	program_name = argv[ 1 ];
	payor_full_name = argv[ 2 ];
	payor_street_address = argv[ 3 ];
	payment_date_time = argv[ 4 ];
	state = argv[ 5 ];

	if ( strcmp( state, "predelete" ) == 0 )
	{
		program_donation_trigger_predelete(
			program_name,
			payor_full_name,
			payor_street_address,
			payment_date_time );
	}

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0 )
	{
		program_donation_trigger_insert_update(
			program_name,
			payor_full_name,
			payor_street_address,
			payment_date_time );

#ifdef NOT_DEFINED
		/* ------------------------------------ */
		/* Even if called from deposit_trigger,	*/
		/* need to set payment_total.		*/
		/* ------------------------------------ */
		paypal_deposit_trigger(
			payor_full_name,
			payor_street_address,
			season_name,
			year,
			deposit_date_time,
			"program_donation" /* state */ );
#endif
	}

	return 0;
}

void program_donation_trigger_insert_update(
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time )
{
	PROGRAM_DONATION *program_donation;
	int transaction_seconds_to_add = 0;

	if ( ! ( program_donation =
			program_donation_fetch(
				program_name,
				payor_full_name,
				payor_street_address,
				payment_date_time,
				1 /* fetch_program */ ) ) )
	{
		return;
	}

	if ( ! ( program_donation =
			program_donation_steady_state(
				program_donation,
				program_donation->donation_amount,
				program_donation->merchant_fees_expense ) ) )
	{
		return;
	}

	if ( ( program_donation->program_donation_transaction =
		program_donation_transaction(
			&transaction_seconds_to_add,
			program_donation->
				payor_entity->
				full_name,
			program_donation->
				payor_entity->
				street_address,
			program_donation->
				payment_date_time,
			program_donation->
				program->
				program_name,
			program_donation->donation_amount,
			program_donation->merchant_fees_expense,
			program_donation->net_payment_amount,
			entity_self_paypal_cash_account_name(),
			account_fees_expense( (char *)0 ),
			program_donation->
				program->
				revenue_account ) ) )
	{
		program_donation->transaction_date_time =
			program_donation->program_donation_transaction->
				transaction_date_time;
	}
	else
	{
		program_donation->transaction_date_time = (char *)0;
	}

	if ( program_donation->transaction_date_time
	&&  *program_donation->transaction_date_time )
	{
		TRANSACTION *t = program_donation->program_donation_transaction;

		program_donation->transaction_date_time =
			transaction_program_refresh(
				t->full_name,
				t->street_address,
				t->transaction_date_time,
				t->program_name,
				t->transaction_amount,
				t->memo,
				0 /* check_number */,
				t->journal_list );
	}

	program_donation_update(
		program_donation->net_payment_amount,
		program_donation->transaction_date_time,
		program_name,
		payor_full_name,
		payor_street_address,
		payment_date_time );
}

void program_donation_trigger_predelete(
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time )
{
	PROGRAM_DONATION *program_donation;

	if ( ! ( program_donation =
			program_donation_fetch(
				program_name,
				payor_full_name,
				payor_street_address,
				payment_date_time,
				0 /* not fetch_program */ ) ) )
	{
		return;
	}

	if ( program_donation->transaction_date_time
	&&   *program_donation->transaction_date_time )
	{
		transaction_delete(
			program_donation->
				payor_entity->
				full_name,
			program_donation->
				payor_entity->
				street_address,
			program_donation->transaction_date_time );

		journal_account_name_list_propagate(
			program_donation->transaction_date_time,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			journal_delete(
				program_donation->
					payor_entity->
					full_name,
				program_donation->
					payor_entity->
					street_address,
				program_donation->transaction_date_time ) );
	}
}
