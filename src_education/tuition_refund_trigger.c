/* --------------------------------------------------------	*/
/* $APPASERVER_HOME/src_education/tuition_refund_trigger.c	*/
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
#include "account.h"
#include "transaction.h"
#include "journal.h"
#include "tuition_refund.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void tuition_refund_trigger_predelete(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *refund_date_time );

/* Returns list of one (TUITION_REFUND *) */
/* -------------------------------------- */
LIST *tuition_refund_trigger_insert_update(
			TUITION_REFUND *tuition_refund );

int main( int argc, char **argv )
{
	char *application_name;
	char *student_full_name;
	char *street_address;
	char *season_name;
	int year;
	char *refund_date_time;
	char *state;

	/* Exits if fails. */
	/* --------------- */
	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf(stderr,
"Usage: %s student_full_name street_address season_name year refund_date_time state\n",
			 argv[ 0 ] );
		fprintf(stderr,
			"state in {insert,update,predelete}\n" );
		exit ( 1 );
	}

	student_full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	season_name = argv[ 3 ];
	year = atoi( argv[ 4 ] );
	refund_date_time = argv[ 5 ];
	state = argv[ 6 ];

	if ( !year ) exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 )
	{
		tuition_refund_trigger_predelete(
			student_full_name,
			street_address,
			season_name,
			year,
			refund_date_time );
		exit( 0 );
	}

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0 )
	{
		TUITION_REFUND *tuition_refund;
		LIST *tuition_refund_list;

		if ( ! ( tuition_refund =
				tuition_refund_fetch(
					student_full_name,
					street_address,
					season_name,
					year,
					refund_date_time,
					1 /* fetch_registration */ ) ) )
		{
			exit( 0 );
		}

		tuition_refund_list =
			tuition_refund_trigger_insert_update(
				tuition_refund );

		registration_list_fetch_update(
			tuition_refund_list_registration_list(
				tuition_refund_list ) );
	}

	return 0;
}

LIST *tuition_refund_trigger_insert_update(
			TUITION_REFUND *tuition_refund )
{
	LIST *tuition_refund_list;
	int transaction_seconds_to_add = 0;

	tuition_refund =
		tuition_refund_steady_state(
			tuition_refund,
			tuition_refund->refund_amount,
			tuition_refund->merchant_fees_expense,
			tuition_refund->payor_entity );

	if ( !tuition_refund->transaction_date_time
	||   !*tuition_refund->transaction_date_time )
	{
		tuition_refund->transaction_date_time =
			transaction_race_free(
				tuition_refund->
					refund_date_time );
	}

	if ( ( tuition_refund->tuition_refund_transaction =
		tuition_refund_transaction(
			&transaction_seconds_to_add,
			tuition_refund->
				payor_entity->
				full_name,
			tuition_refund->
				payor_entity->
				street_address,
			tuition_refund->
				transaction_date_time,
			tuition_refund->refund_amount,
			tuition_refund->merchant_fees_expense,
			tuition_refund->net_refund_amount,
			account_payable( (char *)0 ),
			account_cash( (char *)0 ),
			account_fees_expense( (char *)0 ) ) ) )
	{
		tuition_refund->transaction_date_time =
			tuition_refund->tuition_refund_transaction->
				transaction_date_time;
	}
	else
	{
		tuition_refund->transaction_date_time = (char *)0;
	}

	if ( tuition_refund->transaction_date_time
	&&   *tuition_refund->transaction_date_time )
	{
		TRANSACTION *t = tuition_refund->tuition_refund_transaction;

		tuition_refund->transaction_date_time =
			transaction_refresh(
				t->full_name,
				t->street_address,
				t->transaction_date_time,
				t->transaction_amount,
				t->memo,
				0 /* check_number */,
				t->lock_transaction,
				t->journal_list );
	}

	tuition_refund_update(
		tuition_refund->refund_amount,
		tuition_refund->net_refund_amount,
		tuition_refund->payor_entity->full_name,
		tuition_refund->payor_entity->street_address,
		tuition_refund->transaction_date_time,
		tuition_refund->student_entity->full_name,
		tuition_refund->student_entity->street_address,
		tuition_refund->semester->season_name,
		tuition_refund->semester->year,
		tuition_refund->refund_date_time );

	tuition_refund_list = list_new();
	list_set( tuition_refund_list, tuition_refund );
	return tuition_refund_list;
}

void tuition_refund_trigger_predelete(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *refund_date_time )
{
	TUITION_REFUND *tuition_refund;

	if ( ! ( tuition_refund =
			tuition_refund_fetch(
				student_full_name,
				street_address,
				season_name,
				year,
				refund_date_time,
				0 /* not fetch_registration */ ) ) )
	{
		return;
	}

	if ( tuition_refund->transaction_date_time
	&&   *tuition_refund->transaction_date_time )
	{
		transaction_delete(
			tuition_refund->
				payor_entity->full_name,
			tuition_refund->payor_entity->street_address,
			tuition_refund->transaction_date_time );

		journal_account_name_list_propagate(
			tuition_refund->transaction_date_time,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			journal_delete(
				tuition_refund->
					payor_entity->
					full_name,
				tuition_refund->
					payor_entity->
					street_address,
				tuition_refund->transaction_date_time ) );
	}
}

