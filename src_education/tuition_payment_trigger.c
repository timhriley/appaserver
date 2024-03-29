/* --------------------------------------------------------	*/
/* $APPASERVER_HOME/src_education/tuition_payment_trigger.c	*/
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
#include "semester.h"
#include "entity_self.h"
#include "enrollment.h"
#include "account.h"
#include "tuition_payment.h"
#include "transaction.h"
#include "journal.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void tuition_payment_trigger_predelete(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			char *payment_date_time );

/* Returns list of one (TUITION_PAYMENT *) */
/* --------------------------------------- */
LIST *tuition_payment_trigger_insert_update(
			TUITION_PAYMENT *tuition_payment );

int main( int argc, char **argv )
{
	char *application_name;
	char *student_full_name;
	char *student_street_address;
	char *season_name;
	int year;
	char *payment_date_time;
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
"Usage: %s student_full_name student_street_address season_name year payment_date_time state\n",
			 argv[ 0 ] );
		fprintf(stderr,
			"state in {insert,update,predelete}\n" );
		exit ( 1 );
	}

	student_full_name = argv[ 1 ];
	student_street_address = argv[ 2 ];
	season_name = argv[ 3 ];
	year = atoi( argv[ 4 ] );
	payment_date_time = argv[ 5 ];
	state = argv[ 6 ];

	if ( !year ) exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 )
	{
		tuition_payment_trigger_predelete(
			student_full_name,
			student_street_address,
			season_name,
			year,
			payment_date_time );
		exit( 0 );
	}

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0 )
	{
		TUITION_PAYMENT *tuition_payment;
		LIST *tuition_payment_list;

		if ( ! ( tuition_payment =
				tuition_payment_fetch(
					student_full_name,
					student_street_address,
					season_name,
					year,
					payment_date_time,
					1 /* fetch_registration */ ) ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: tuition_payment_fetch() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		tuition_payment_list =
			tuition_payment_trigger_insert_update(
				tuition_payment );

		if ( list_length( tuition_payment_list ) )
		{
			registration_list_fetch_update(
				tuition_payment_list_registration_list(
					tuition_payment_list ) );
		}
	}

	if ( strcmp( state, "delete" ) == 0 )
	{
		registration_fetch_update(
			student_full_name,
			student_street_address,
			season_name,
			year );
	}
	return 0;
}

LIST *tuition_payment_trigger_insert_update(
			TUITION_PAYMENT *tuition_payment )
{
	LIST *tuition_payment_list;
	int transaction_seconds_to_add = 0;

	tuition_payment =
		/* ----------------------- */
		/* Returns tuition_payment */
		/* ----------------------- */
		tuition_payment_steady_state(
			tuition_payment,
			tuition_payment->payment_amount,
			tuition_payment->merchant_fees_expense,
			tuition_payment->payor_entity );

	if ( !tuition_payment->transaction_date_time
	||   !*tuition_payment->transaction_date_time )
	{
		tuition_payment->transaction_date_time =
			transaction_race_free(
				tuition_payment->
					payment_date_time );
	}

	if ( ( tuition_payment->tuition_payment_transaction =
		tuition_payment_transaction(
			&transaction_seconds_to_add,
			tuition_payment->payor_entity->full_name,
			tuition_payment->payor_entity->street_address,
			tuition_payment->transaction_date_time,
			tuition_payment->payment_amount,
			tuition_payment->merchant_fees_expense,
			tuition_payment->net_payment_amount,
			account_cash( (char *)0 ),
			account_fees_expense( (char *)0 ),
			account_receivable( (char *)0 ) ) ) )
	{
		tuition_payment->transaction_date_time =
			tuition_payment->tuition_payment_transaction->
				transaction_date_time;
	}
	else
	{
		tuition_payment->transaction_date_time = (char *)0;
	}

	if ( tuition_payment->transaction_date_time )
	{
		TRANSACTION *t = tuition_payment->tuition_payment_transaction;

		tuition_payment->transaction_date_time =
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

	tuition_payment_update(
		tuition_payment->net_payment_amount,
		tuition_payment->payor_entity->full_name,
		tuition_payment->payor_entity->street_address,
		tuition_payment->transaction_date_time,
		tuition_payment->student_entity->full_name,
		tuition_payment->student_entity->street_address,
		tuition_payment->semester->season_name,
		tuition_payment->semester->year,
		tuition_payment->payment_date_time );

	tuition_payment_list = list_new();
	list_set( tuition_payment_list, tuition_payment );

	return tuition_payment_list;
}

void tuition_payment_trigger_predelete(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			char *payment_date_time )
{
	TUITION_PAYMENT *tuition_payment;

	if ( ! ( tuition_payment =
			tuition_payment_fetch(
				student_full_name,
				student_street_address,
				season_name,
				year,
				payment_date_time,
				0 /* not fetch_registration */ ) ) )
	{
		return;
	}

	if ( tuition_payment->transaction_date_time
	&&   *tuition_payment->transaction_date_time )
	{
		transaction_delete(
			tuition_payment->payor_entity->full_name,
			tuition_payment->payor_entity->street_address,
			tuition_payment->transaction_date_time );

		journal_account_name_list_propagate(
			tuition_payment->transaction_date_time,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			journal_delete(
				tuition_payment->payor_entity->full_name,
				tuition_payment->payor_entity->street_address,
				tuition_payment->transaction_date_time ) );
	}
}
