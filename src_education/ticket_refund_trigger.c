/* --------------------------------------------------------	*/
/* $APPASERVER_HOME/src_education/ticket_refund_trigger.c	*/
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
#include "event.h"
#include "journal.h"
#include "program.h"
#include "ticket_refund.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void ticket_refund_trigger_predelete(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address,
			char *sale_date_time,
			char *refund_date_time );

/* Returns list of one (TICKET_REFUND *) */
/* ------------------------------------- */
LIST *ticket_refund_trigger_insert_update(
			TICKET_REFUND *ticket_refund );

int main( int argc, char **argv )
{
	char *application_name;
	char *program_name;
	char *event_date;
	char *event_time;
	char *payor_full_name;
	char *payor_street_address;
	char *sale_date_time;
	char *refund_date_time;
	char *preupdate_transaction_date_time;
	char *state;
	TICKET_REFUND *ticket_refund;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 10 )
	{
		fprintf(stderr,
"Usage: %s program_name event_date event_time payor_full_name payor_street_address sale_date_time refund_date_time preupdate_transaction_date_time state\n",
			 argv[ 0 ] );
		fprintf(stderr,
			"state in {insert,update,predelete}\n" );
		exit ( 1 );
	}

	program_name = argv[ 1 ];
	event_date = argv[ 2 ];
	event_time = argv[ 3 ];
	payor_full_name = argv[ 4 ];
	payor_street_address = argv[ 5 ];
	sale_date_time = argv[ 6 ];
	refund_date_time = argv[ 7 ];
	preupdate_transaction_date_time = argv[ 8 ];
	state = argv[ 9 ];

	if ( strcmp( state, "predelete" ) == 0 )
	{
		ticket_refund_trigger_predelete(
			program_name,
			event_date,
			event_time,
			payor_full_name,
			payor_street_address,
			sale_date_time,
			refund_date_time );
		exit( 0 );
	}

	if ( ! ( ticket_refund =
			ticket_refund_fetch(
				program_name,
				event_date,
				event_time,
				payor_full_name,
				payor_street_address,
				sale_date_time,
				refund_date_time,
				1 /* fetch_event */,
				1 /* fetch_transaction */ ) ) )
	{
		exit( 0 );
	}

	if ( transaction_date_time_changed(
			preupdate_transaction_date_time )
	&&   ticket_refund->ticket_refund_transaction )
	{
		journal_account_name_list_propagate(
			transaction_date_time_earlier(
				ticket_refund->transaction_date_time,
				preupdate_transaction_date_time ),
			journal_list_account_name_list(
				ticket_refund->
					ticket_refund_transaction->
					journal_list ) );
	}

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0 )
	{
		LIST *ticket_refund_list;

		ticket_refund_list =
			ticket_refund_trigger_insert_update(
				ticket_refund );

		event_list_fetch_update(
			ticket_refund_event_list(
				ticket_refund_list ) );
	}

	if ( strcmp( state, "delete" ) == 0 )
	{
		LIST *ticket_refund_list = list_new();

		list_set(
			ticket_refund_list,
			ticket_refund_new(
				program_name,
				event_date,
				event_time,
				entity_new(
					payor_full_name,
					payor_street_address ),
				sale_date_time,
				refund_date_time ) );

		event_list_fetch_update(
			ticket_refund_event_list(
				ticket_refund_list ) );
	}

	return 0;
}

LIST *ticket_refund_trigger_insert_update(
			TICKET_REFUND *ticket_refund )
{
	LIST *ticket_refund_list;
	int transaction_seconds_to_add = 0;

	if ( ! ( ticket_refund =
			ticket_refund_steady_state(
				ticket_refund,
				ticket_refund->
					refund_amount,
				ticket_refund->
					merchant_fees_expense ) ) )
	{
		return (LIST *)0;
	}

	if ( !ticket_refund->transaction_date_time
	||   !*ticket_refund->transaction_date_time )
	{
		ticket_refund->transaction_date_time =
			transaction_race_free(
				ticket_refund->
					refund_date_time );
	}

	if ( ( ticket_refund->ticket_refund_transaction =
		ticket_refund_transaction(
			&transaction_seconds_to_add,
			ticket_refund->
				payor_entity->
				full_name,
			ticket_refund->
				payor_entity->
				street_address,
			ticket_refund->
				transaction_date_time,
			ticket_refund->
				ticket_sale->
				event->
				program_name,
			ticket_refund->refund_amount,
			ticket_refund->merchant_fees_expense,
			ticket_refund->net_refund_amount,
			account_cash( (char *)0 ),
			account_fees_expense( (char *)0 ),
			ticket_refund->
				ticket_sale->
				event->
				revenue_account ) ) )
	{
		ticket_refund->transaction_date_time =
			ticket_refund->ticket_refund_transaction->
				transaction_date_time;
	}
	else
	{
		ticket_refund->transaction_date_time = (char *)0;
	}

	if ( ticket_refund->transaction_date_time
	&&  *ticket_refund->transaction_date_time )
	{
		TRANSACTION *t = ticket_refund->ticket_refund_transaction;

		ticket_refund->transaction_date_time =
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

	ticket_refund_update(
		ticket_refund->
			net_refund_amount,
		ticket_refund->
			transaction_date_time,
		ticket_refund->program_name,
		ticket_refund->event_date,
		ticket_refund->event_time,
		ticket_refund->payor_entity->full_name,
		ticket_refund->payor_entity->street_address,
		ticket_refund->sale_date_time,
		ticket_refund->refund_date_time );

	ticket_refund_list = list_new();
	list_set( ticket_refund_list, ticket_refund );

	return ticket_refund_list;
}

void ticket_refund_trigger_predelete(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address,
			char *sale_date_time,
			char *refund_date_time )
{
	TICKET_REFUND *ticket_refund;

	if ( ! ( ticket_refund =
			ticket_refund_fetch(
				program_name,
				event_date,
				event_time,
				payor_full_name,
				payor_street_address,
				sale_date_time,
				refund_date_time,
				0 /* not fetch_sale */,
				0 /* not fetch_transaction */ ) ) )
	{
		return;
	}

	if ( ticket_refund->transaction_date_time
	&&   *ticket_refund->transaction_date_time )
	{
		transaction_delete(
			ticket_refund->
				payor_entity->
				full_name,
			ticket_refund->
				payor_entity->
				street_address,
			ticket_refund->transaction_date_time );

		journal_account_name_list_propagate(
			ticket_refund->transaction_date_time,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			journal_delete(
				ticket_refund->
					payor_entity->
					full_name,
				ticket_refund->
					payor_entity->
					street_address,
				ticket_refund->transaction_date_time ) );
	}
}
