/* --------------------------------------------------------	*/
/* $APPASERVER_HOME/src_education/ticket_sale_trigger.c		*/
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
#include "ticket_sale.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void ticket_sale_trigger_predelete(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address,
			char *sale_date_time );

/* Returns list of one (TICKET_SALE *) */
/* ----------------------------------- */
LIST *ticket_sale_trigger_insert_update(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address,
			char *sale_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *program_name;
	char *event_date;
	char *event_time;
	char *payor_full_name;
	char *payor_street_address;
	char *sale_date_time;
	char *state;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf(stderr,
"Usage: %s program_name evet_date event_time payor_full_name payor_street_address sale_date_time state\n",
			 argv[ 0 ] );
		fprintf(stderr,
			"state in {insert,update,predelete,delete}\n" );
		exit ( 1 );
	}

	program_name = argv[ 1 ];
	event_date = argv[ 2 ];
	event_time = argv[ 3 ];
	payor_full_name = argv[ 4 ];
	payor_street_address = argv[ 5 ];
	sale_date_time = argv[ 6 ];
	state = argv[ 7 ];

	if ( strcmp( state, "predelete" ) == 0 )
	{
		ticket_sale_trigger_predelete(
			program_name,
			event_date,
			event_time,
			payor_full_name,
			payor_street_address,
			sale_date_time );
	}

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0 )
	{
		LIST *ticket_sale_list =
			ticket_sale_trigger_insert_update(
				program_name,
				event_date,
				event_time,
				payor_full_name,
				payor_street_address,
				sale_date_time );

		event_list_fetch_update(
			ticket_sale_event_list(
				ticket_sale_list ) );
	}

	if ( strcmp( state, "delete" ) == 0 )
	{
		LIST *ticket_sale_list = list_new();

		list_set(
			ticket_sale_list,
			ticket_sale_new(
				program_name,
				event_date,
				event_time,
				entity_new(
					payor_full_name,
					payor_street_address ),
				sale_date_time ) );

		event_list_fetch_update(
			ticket_sale_event_list(
				ticket_sale_list ) );
	}
	return 0;
}

LIST *ticket_sale_trigger_insert_update(
			char *program_name,
			char *event_date,
			char *event_time,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address )
{
	TICKET_SALE *ticket_sale;
	LIST *ticket_sale_list;
	int transaction_seconds_to_add = 0;

	if ( ! ( ticket_sale =
			ticket_sale_fetch(
				program_name,
				event_date,
				event_time,
				sale_date_time,
				payor_full_name,
				payor_street_address,
				1 /* fetch_event */ ) ) )
	{
		return (LIST *)0;
	}

	if ( ! ( ticket_sale =
			ticket_sale_steady_state(
				ticket_sale,
				ticket_sale->quantity,
				ticket_sale->ticket_price,
				ticket_sale->merchant_fees_expense ) ) )
	{
		return (LIST *)0;
	}

	if ( !ticket_sale->transaction_date_time )
	{
		ticket_sale->transaction_date_time =
			transaction_race_free(
				ticket_sale->
					sale_date_time );
	}

	if ( ( ticket_sale->ticket_sale_transaction =
		ticket_sale_transaction(
			&transaction_seconds_to_add,
			ticket_sale->
				payor_entity->
				full_name,
			ticket_sale->
				payor_entity->
				street_address,
			ticket_sale->
				transaction_date_time,
			ticket_sale->
				event->
				program_name,
			ticket_sale->extended_price,
			ticket_sale->merchant_fees_expense,
			ticket_sale->net_payment_amount,
			account_cash( (char *)0 ),
			account_fees_expense( (char *)0 ),
			ticket_sale->
				event->
				revenue_account ) ) )
	{
		ticket_sale->transaction_date_time =
			ticket_sale->ticket_sale_transaction->
				transaction_date_time;
	}
	else
	{
		ticket_sale->transaction_date_time = (char *)0;
	}

	if ( ticket_sale->transaction_date_time
	&&  *ticket_sale->transaction_date_time )
	{
		TRANSACTION *t = ticket_sale->ticket_sale_transaction;

		ticket_sale->transaction_date_time =
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

	ticket_sale_update(
		ticket_sale->
			extended_price,
		ticket_sale->
			net_payment_amount,
		ticket_sale->
			transaction_date_time,
		program_name,
		event_date,
		event_time,
		sale_date_time,
		payor_full_name,
		payor_street_address );

	ticket_sale_list = list_new();
	list_set( ticket_sale_list, ticket_sale );

	return ticket_sale_list;
}

void ticket_sale_trigger_predelete(
			char *program_name,
			char *event_date,
			char *event_time,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address )
{
	TICKET_SALE *ticket_sale;

	if ( ! ( ticket_sale =
			ticket_sale_fetch(
				program_name,
				event_date,
				event_time,
				sale_date_time,
				payor_full_name,
				payor_street_address,
				0 /* not fetch_event */ ) ) )
	{
		return;
	}

	if ( ticket_sale->transaction_date_time
	&&   *ticket_sale->transaction_date_time )
	{
		transaction_delete(
			ticket_sale->
				payor_entity->
				full_name,
			ticket_sale->
				payor_entity->
				street_address,
			ticket_sale->transaction_date_time );

		journal_account_name_list_propagate(
			ticket_sale->transaction_date_time,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			journal_delete(
				ticket_sale->
					payor_entity->
					full_name,
				ticket_sale->
					payor_entity->
					street_address,
				ticket_sale->transaction_date_time ) );
	}
}
