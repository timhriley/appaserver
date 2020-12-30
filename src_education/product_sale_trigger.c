/* --------------------------------------------------------	*/
/* $APPASERVER_HOME/src_education/product_sale_trigger.c	*/
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
#include "product_sale.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void product_sale_trigger_predelete(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time );

void product_sale_trigger_insert_update(
			char *product_name,
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
		product_sale_trigger_predelete(
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
		product_sale_trigger_insert_update(
			product_name,
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
			"product_sale" /* state */ );
	}

	return 0;
}

void product_sale_trigger_insert_update(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time )
{
	PRODUCT_SALE *product_sale;
	int transaction_seconds_to_add = 0;

	if ( ! ( product_sale =
			product_sale_fetch(
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

	if ( ! ( product_sale =
			product_sale_steady_state(
				&transaction_seconds_to_add,
				product_sale,
				product_sale->
					paypal_deposit->
					deposit_amount,
				product_sale->
					paypal_deposit->
					transaction_fee ) ) )
	{
		return;
	}

	if ( product_sale->transaction_date_time
	&&  *product_sale->transaction_date_time )
	{
		TRANSACTION *t = product_sale->product_sale_transaction;

		product_sale->transaction_date_time =
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

	product_sale_update(
		product_sale->
			transaction_date_time,
		product_name,
		payor_full_name,
		payor_street_address,
		season_name,
		year,
		deposit_date_time );
}

void product_sale_trigger_predelete(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time )
{
	PRODUCT_SALE *product_sale;

	if ( ! ( product_sale =
			product_sale_fetch(
				product_name,
				payor_full_name,
				payor_street_address,
				season_name,
				year,
				deposit_date_time,
				0 /* not fetch_product */,
				0 /* not fetch_paypal */ ) ) )
	{
		return;
	}

	if ( product_sale->transaction_date_time
	&&   *product_sale->transaction_date_time )
	{
		transaction_delete(
			product_sale->
				paypal_deposit->
				payor_entity->
				full_name,
			product_sale->
				paypal_deposit->
				payor_entity->
				street_address,
			product_sale->transaction_date_time );

		journal_account_name_list_propagate(
			product_sale->transaction_date_time,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			journal_delete(
				product_sale->
					paypal_deposit->
					payor_entity->
					full_name,
				product_sale->
					paypal_deposit->
					payor_entity->
					street_address,
				product_sale->transaction_date_time ) );
	}
}
