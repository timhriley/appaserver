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

void ticket_refund_trigger_insert_update(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address,
			char *sale_date_time,
			char *refund_date_time );

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
	char *state;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 9 )
	{
		fprintf(stderr,
"Usage: %s program_name event_date event_time payor_full_name payor_street_address sale_date_time refund_date_time state\n",
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
	state = argv[ 8 ];

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
	}

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0 )
	{
		ticket_refund_trigger_insert_update(
			program_name,
			event_date,
			event_time,
			payor_full_name,
			payor_street_address,
			sale_date_time,
			refund_date_time );
	}

	return 0;
}

void ticket_refund_trigger_insert_update(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address,
			char *sale_date_time,
			char *refund_date_time )
{
	TICKET_REFUND *ticket_refund;
	int transaction_seconds_to_add = 0;

	if ( ! ( ticket_refund =
			ticket_refund_fetch(
				program_name,
				event_date,
				event_time,
				payor_full_name,
				payor_street_address,
				sale_date_time,
				refund_date_time,
				1 /* fetch_event */ ) ) )
	{
		return;
	}

	if ( ! ( ticket_refund =
			ticket_refund_steady_state(
				ticket_refund,
				ticket_refund->
					refund_amount,
				ticket_refund->
					merchant_fees_expense ) ) )
	{
		return;
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
				ticket_sale->
				sale_date_time,
			ticket_refund->
				ticket_sale->
				event->
				program_name,
			ticket_refund->refund_amount,
			ticket_refund->merchant_fees_expense,
			ticket_refund->net_refund_amount,
			entity_self_paypal_cash_account_name(),
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

		t->program_name = program_name;

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
		program_name,
		event_date,
		event_time,
		payor_full_name,
		payor_street_address,
		sale_date_time,
		refund_date_time );
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
				0 /* not fetch_event */ ) ) )
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
