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
			char *payor_full_name,
			char *payor_street_address,
			char *sale_date_time );

/* Returns list of one */
/* ------------------- */
LIST *product_sale_trigger_insert_update(
			PRODUCT_SALE *product_sale );

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
"Usage: %s product_name payor_full_name payor_street_address sale_date_time state\n",
			 argv[ 0 ] );
		fprintf(stderr,
			"state in {insert,update,predelete,delete}\n" );
		exit ( 1 );
	}

	product_name = argv[ 1 ];
	payor_full_name = argv[ 2 ];
	payor_street_address = argv[ 3 ];
	sale_date_time = argv[ 4 ];
	state = argv[ 5 ];

	if ( strcmp( state, "predelete" ) == 0 )
	{
		product_sale_trigger_predelete(
			product_name,
			payor_full_name,
			payor_street_address,
			sale_date_time );
		exit( 0 );
	}

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0 )
	{
		PRODUCT_SALE *product_sale;
		LIST *product_sale_list;

		if ( ! ( product_sale =
				product_sale_fetch(
					product_name,
					payor_full_name,
					payor_street_address,
					sale_date_time,
					1 /* fetch_product */,
					0 /* not fetch_transaction */ ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: product_sale_fetch() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		product_sale_list =
			product_sale_trigger_insert_update(
				product_sale );

		product_list_fetch_update(
			product_sale_list_product_name_list(
				product_sale_list ) );
	}

	if ( strcmp( state, "delete" ) == 0 )
	{
		product_fetch_update( product_name );
	}
	return 0;
}

LIST *product_sale_trigger_insert_update(
			PRODUCT_SALE *product_sale )
{
	LIST *product_sale_list;
	int transaction_seconds_to_add = 0;

	if ( ! ( product_sale =
			product_sale_steady_state(
				product_sale,
				product_sale->quantity,
				product_sale->retail_price,
				product_sale->merchant_fees_expense ) ) )
	{
		return (LIST *)0;
	}

	if ( !product_sale->transaction_date_time
	||   !*product_sale->transaction_date_time )
	{
		product_sale->transaction_date_time =
			transaction_race_free(
				product_sale->
					sale_date_time );
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
				transaction_date_time,
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
			account_cash( (char *)0 ),
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
				t->lock_transaction,
				t->journal_list );
	}

	product_sale_update(
		product_sale->
			extended_price,
		product_sale->
			net_payment_amount,
		product_sale->
			transaction_date_time,
		product_sale->product_name,
		product_sale->payor_entity->full_name,
		product_sale->payor_entity->street_address,
		product_sale->sale_date_time );

	product_sale_list = list_new();
	list_set( product_sale_list, product_sale );

	return product_sale_list;
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
				payor_full_name,
				payor_street_address,
				sale_date_time,
				0 /* not fetch_product */,
				0 /* not fetch_transaction */ ) ) )
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
