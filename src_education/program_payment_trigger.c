/* --------------------------------------------------------	*/
/* $APPASERVER_HOME/src_education/program_payment_trigger.c	*/
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
#include "program.h"
#include "journal.h"
#include "program_payment.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void program_payment_trigger_predelete(
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

void program_payment_trigger_insert_update(
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *program_name;
	char *payor_full_name;
	char *payor_street_address;
	char *season_name;
	int year;
	char *deposit_date_time;
	char *state;

	/* Exits if fails. */
	/* --------------- */
	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf(stderr,
"Usage: %s program_name payor_full_name payor_street_address season_name year deposit_date_time state\n",
			 argv[ 0 ] );
		fprintf(stderr,
			"state in {insert,update,predelete,delete,deposit}\n" );
		exit ( 1 );
	}

	program_name = argv[ 1 ];
	payor_full_name = argv[ 2 ];
	payor_street_address = argv[ 3 ];
	season_name = argv[ 4 ];
	year = atoi( argv[ 5 ] );
	deposit_date_time = argv[ 8 ];
	state = argv[ 9 ];

	if ( !year ) exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 )
	{
		program_payment_trigger_predelete(
			program_name,
			payor_full_name,
			payor_street_address,
			season_name,
			year,
			deposit_date_time );
	}

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0
	||   strcmp( state, "deposit" ) ==  0 )
	{
		program_payment_trigger_insert_update(
			program_name,
			payor_full_name,
			payor_street_address,
			season_name,
			year,
			deposit_date_time );

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
			"program_payment" /* state */ );
	}

	return 0;
}

void program_payment_trigger_insert_update(
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time )
{
	PROGRAM_PAYMENT *program_payment;
	int transaction_seconds_to_add = 0;

	if ( ! ( program_payment =
			program_payment_fetch(
				program_name,
				payor_full_name,
				payor_street_address,
				season_name,
				year,
				deposit_date_time,
				1 /* fetch_program */,
				1 /* fetch_deposit */ ) ) )
	{
		return;
	}

	if ( ! ( program_payment =
			program_payment_steady_state(
				&transaction_seconds_to_add,
				program_payment,
				program_payment->
					paypal_deposit->
					deposit_amount,
				program_payment->
					paypal_deposit->
					transaction_fee ) ) )
	{
		return;
	}

	if ( program_payment->transaction_date_time
	&&  *program_payment->transaction_date_time )
	{
		TRANSACTION *t = program_payment->program_payment_transaction;

		program_payment->transaction_date_time =
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

	program_payment_update(
		program_payment->
			transaction_date_time,
		program_name,
		payor_full_name,
		payor_street_address,
		season_name,
		year,
		deposit_date_time );
}

void program_payment_trigger_predelete(
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time )
{
	PROGRAM_PAYMENT *program_payment;

	if ( ! ( program_payment =
			program_payment_fetch(
				program_name,
				payor_full_name,
				payor_street_address,
				season_name,
				year,
				deposit_date_time,
				0 /* not fetch_program */,
				1 /* fetch_paypal */ ) ) )
	{
		return;
	}

	if ( program_payment->transaction_date_time
	&&   *program_payment->transaction_date_time )
	{
		transaction_delete(
			program_payment->
				paypal_deposit->
				payor_entity->
				full_name,
			program_payment->
				paypal_deposit->
				payor_entity->
				street_address,
			program_payment->transaction_date_time );

		journal_account_name_list_propagate(
			program_payment->transaction_date_time,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			journal_delete(
				program_payment->
					paypal_deposit->
					payor_entity->
					full_name,
				program_payment->
					paypal_deposit->
					payor_entity->
					street_address,
				program_payment->transaction_date_time ) );
	}
}
