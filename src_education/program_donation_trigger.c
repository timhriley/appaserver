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

/* Returns list of one (PROGRAM_DONATION *) */
/* ---------------------------------------- */
LIST *program_donation_trigger_insert_update(
			PROGRAM_DONATION *program_donation );

int main( int argc, char **argv )
{
	char *application_name;
	char *program_name;
	char *payor_full_name;
	char *payor_street_address;
	char *payment_date_time;
	char *state;

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
			"state in {insert,update,predelete}\n" );
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
		exit( 0 );
	}

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0 )
	{
		PROGRAM_DONATION *program_donation;
		LIST *program_donation_list;

		if ( ! ( program_donation =
				program_donation_fetch(
					program_name,
					payor_full_name,
					payor_street_address,
					payment_date_time,
					1 /* fetch_program */,
					0 /* not fetch_transaction */ ) ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: program_donation_fetch() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		program_donation_list =
			program_donation_trigger_insert_update(
				program_donation );

		program_list_fetch_update(
			program_donation_program_name_list(
				program_donation_list ) );
	}

	if ( strcmp( state, "delete" ) ==  0 )
	{
		LIST *program_donation_list = list_new();

		list_set(
			program_donation_list,
			program_donation_new(
				program_name,
				payor_full_name,
				payor_street_address,
				payment_date_time ) );

		program_list_fetch_update(
			program_donation_program_name_list(
				program_donation_list ) );
	}

	return 0;
}

LIST *program_donation_trigger_insert_update(
			PROGRAM_DONATION *program_donation )
{
	LIST *program_donation_list;
	int transaction_seconds_to_add = 0;

	if ( ! ( program_donation =
			program_donation_steady_state(
				program_donation,
				program_donation->donation_amount,
				program_donation->merchant_fees_expense ) ) )
	{
		return (LIST *)0;
	}

	if ( !program_donation->transaction_date_time
	||   !*program_donation->transaction_date_time )
	{
		program_donation->transaction_date_time =
			transaction_race_free(
				program_donation->
					payment_date_time );
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
			account_cash( (char *)0 ),
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
				t->lock_transaction,
				t->journal_list );
	}

	program_donation_update(
		program_donation->net_payment_amount,
		program_donation->transaction_date_time,
		program_donation->program_name,
		program_donation->payor_entity->full_name,
		program_donation->payor_entity->street_address,
		program_donation->payment_date_time );

	program_donation_list = list_new();
	list_set( program_donation_list, program_donation );

	return program_donation_list;
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
				0 /* not fetch_program */,
				0 /* not fetch_transaction */ ) ) )
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
