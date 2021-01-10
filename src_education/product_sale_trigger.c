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
#include "entity_self.h"
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
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address );

void product_sale_trigger_insert_update(
			char *product_name,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address );

int main( int argc, char **argv )
{
	char *application_name;
	char *product_name;
	char *payor_full_name;
	char *payor_street_address;
	char *sale_date_time;
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
"Usage: %s product_name sale_date_time payor_full_name payor_street_address state\n",
			 argv[ 0 ] );
		fprintf(stderr,
			"state in {insert,update,predelete,delete}\n" );
		exit ( 1 );
	}

	product_name = argv[ 1 ];
	sale_date_time = argv[ 2 ];
	payor_full_name = argv[ 3 ];
	payor_street_address = argv[ 4 ];
	state = argv[ 5 ];

	if ( strcmp( state, "predelete" ) == 0 )
	{
		product_sale_trigger_predelete(
			product_name,
			sale_date_time,
			payor_full_name,
			payor_street_address );
	}

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0 )
	{
		product_sale_trigger_insert_update(
			product_name,
			sale_date_time,
			payor_full_name,
			payor_street_address );

		product_trigger( product_name );
	}

	if ( strcmp( state, "delete" ) == 0 )
	{
		product_trigger( product_name );
	}

	return 0;
}

void product_sale_trigger_insert_update(
			char *product_name,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address )
{
	PRODUCT_SALE *product_sale;
	int transaction_seconds_to_add = 0;

	if ( ! ( product_sale =
			product_sale_fetch(
				product_name,
				sale_date_time,
				payor_full_name,
				payor_street_address,
				1 /* fetch_product */ ) ) )
	{
		return;
	}

	if ( ! ( product_sale =
			product_sale_steady_state(
				product_sale,
				product_sale->quantity,
				product_sale->retail_price,
				product_sale->merchant_fees_expense ) ) )
	{
		return;
	}

	if ( ( product_sale->product_sale_transaction =
		product_sale_transaction(
			&transaction_seconds_to_add,
			product_sale->
				payor_entity->
				full_name,
			product_sale->
				payor_entity->
				street_address,
			product_sale->
				sale_date_time,
			product_sale->
				product->
				product_name,
			product_sale->
				product->
				program->
				program_name,
			product_sale->extended_price,
			product_sale->merchant_fees_expense,
			product_sale->net_payment_amount,
			entity_self_paypal_cash_account_name(),
			account_fees_expense( (char *)0 ),
			product_sale->
				product->
				revenue_account ) ) )
	{
		product_sale->transaction_date_time =
			product_sale->product_sale_transaction->
				transaction_date_time;
	}
	else
	{
		product_sale->transaction_date_time = (char *)0;
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
			extended_price,
		product_sale->
			net_payment_amount,
		product_sale->
			transaction_date_time,
		product_name,
		sale_date_time,
		payor_full_name,
		payor_street_address );
}

void product_sale_trigger_predelete(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *sale_date_time )
{
	PRODUCT_SALE *product_sale;

	if ( ! ( product_sale =
			product_sale_fetch(
				product_name,
				sale_date_time,
				payor_full_name,
				payor_street_address,
				0 /* not fetch_product */ ) ) )
	{
		return;
	}

	if ( product_sale->transaction_date_time
	&&   *product_sale->transaction_date_time )
	{
		transaction_delete(
			product_sale->
				payor_entity->
				full_name,
			product_sale->
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
					payor_entity->
					full_name,
				product_sale->
					payor_entity->
					street_address,
				product_sale->transaction_date_time ) );
	}
}
