/* --------------------------------------------------------	*/
/* $APPASERVER_HOME/src_education/product_payment_trigger.c	*/
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
#include "product.h"
#include "journal.h"
#include "product_payment_fns.h"
#include "product_payment.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void product_payment_trigger_predelete(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

void product_payment_trigger_insert_update(
			char *product_name,
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *product_name;
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
"Usage: %s product_name payor_full_name payor_street_address season_name year deposit_date_time state\n",
			 argv[ 0 ] );
		fprintf(stderr,
			"state in {insert,update,predelete,delete,deposit}\n" );
		exit ( 1 );
	}

	product_name = argv[ 1 ];
	payor_full_name = argv[ 2 ];
	payor_street_address = argv[ 3 ];
	season_name = argv[ 4 ];
	year = atoi( argv[ 5 ] );
	deposit_date_time = argv[ 8 ];
	state = argv[ 9 ];

	if ( !year ) exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 )
	{
		product_payment_trigger_predelete(
			product_name,
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
		product_payment_trigger_insert_update(
			product_name,
			product_fetch_program_name( product_name ),
			payor_full_name,
			payor_street_address,
			season_name,
			year,
			deposit_date_time );

		/* ------------------------------------ */
		/* Even if called from deposit_trigger,	*/
		/* need to set payment_total.		*/
		/* ------------------------------------ */
		deposit_trigger(
			payor_full_name,
			payor_street_address,
			season_name,
			year,
			deposit_date_time,
			"product_payment" /* state */ );
	}

	return 0;
}

void product_payment_trigger_insert_update(
			char *product_name,
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time )
{
	PRODUCT_PAYMENT *product_payment;
	int transaction_seconds_to_add = 0;

	if ( ! ( product_payment =
			product_payment_fetch(
				product_name,
				payor_full_name,
				payor_street_address,
				season_name,
				year,
				deposit_date_time,
				1 /* fetch_product */,
				1 /* fetch_deposit */ ) ) )
	{
		return;
	}

	if ( ! ( product_payment =
			product_payment_steady_state(
				&transaction_seconds_to_add,
				product_payment,
				product_payment->deposit->deposit_amount,
				product_payment->deposit->transaction_fee ) ) )
	{
		return;
	}

	if ( product_payment->transaction_date_time
	&&  *product_payment->transaction_date_time )
	{
		TRANSACTION *t = product_payment->product_payment_transaction;

		product_payment->transaction_date_time =
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

	product_payment_update(
		product_payment->
			transaction_date_time,
		product_name,
		payor_full_name,
		payor_street_address,
		season_name,
		year,
		deposit_date_time );
}

void product_payment_trigger_predelete(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time )
{
	PRODUCT_PAYMENT *product_payment;

	if ( ! ( product_payment =
			product_payment_fetch(
				product_name,
				payor_full_name,
				payor_street_address,
				season_name,
				year,
				deposit_date_time,
				0 /* not fetch_product */,
				0 /* not fetch_deposit */ ) ) )
	{
		return;
	}

	if ( product_payment->transaction_date_time
	&&   *product_payment->transaction_date_time )
	{
		transaction_delete(
			product_payment->
				deposit->
				payor_entity->
				full_name,
			product_payment->
				deposit->
				payor_entity->
				street_address,
			product_payment->transaction_date_time );

		journal_account_name_list_propagate(
			product_payment->transaction_date_time,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			journal_delete(
				product_payment->
					deposit->
					payor_entity->
					full_name,
				product_payment->
					deposit->
					payor_entity->
					street_address,
				product_payment->transaction_date_time ) );
	}
}
