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
			char *street_address,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time );

/* Returns list of one (TUITION_PAYMENT *) */
/* --------------------------------------- */
LIST *tuition_payment_trigger_insert_update(
			TUITION_PAYMENT *tuition_payment );

int main( int argc, char **argv )
{
	char *application_name;
	char *student_full_name;
	char *street_address;
	char *season_name;
	int year;
	char *payor_full_name;
	char *payor_street_address;
	char *payment_date_time;
	char *preupdate_transaction_date_time;
	char *state;
	TUITION_PAYMENT *tuition_payment;

	/* Exits if fails. */
	/* --------------- */
	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 10 )
	{
		fprintf(stderr,
"Usage: %s student_full_name street_address season_name year payor_full_name payor_street_address payment_date_time preupdate_transaction_date_time state\n",
			 argv[ 0 ] );
		fprintf(stderr,
			"state in {insert,update,predelete}\n" );
		exit ( 1 );
	}

	student_full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	season_name = argv[ 3 ];
	year = atoi( argv[ 4 ] );
	payor_full_name = argv[ 5 ];
	payor_street_address = argv[ 6 ];
	payment_date_time = argv[ 7 ];
	preupdate_transaction_date_time = argv[ 8 ];
	state = argv[ 9 ];

	if ( !year ) exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 )
	{
		tuition_payment_trigger_predelete(
			student_full_name,
			street_address,
			season_name,
			year,
			payor_full_name,
			payor_street_address,
			payment_date_time );
		exit( 0 );
	}

	if ( ! ( tuition_payment =
			tuition_payment_fetch(
				student_full_name,
				street_address,
				season_name,
				year,
				payor_full_name,
				payor_street_address,
				payment_date_time,
				1 /* fetch_registration */,
				1 /* fetch_enrollment_list */,
				1 /* fetch_offering */,
				1 /* fetch_course */,
				0 /* not fetch_program */,
				1 /* fetch_transaction */ ) ) )
	{
		exit( 0 );
	}


	if ( transaction_date_time_changed(
			preupdate_transaction_date_time )
	&&   tuition_payment->tuition_payment_transaction )
	{
		journal_account_name_list_propagate(
			transaction_date_time_earlier(
				tuition_payment->transaction_date_time,
				preupdate_transaction_date_time ),
			journal_list_account_name_list(
				tuition_payment->
					tuition_payment_transaction->
					journal_list ) );
	}

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0 )
	{
		LIST *tuition_payment_list;

		tuition_payment_list =
			tuition_payment_trigger_insert_update(
				tuition_payment );

		if ( list_length( tuition_payment_list ) )
		{
			registration_list_fetch_update(
				tuition_payment_registration_list(
					tuition_payment_list ),
				season_name,
				year );
		}
	}

	if ( strcmp( state, "delete" ) == 0 )
	{
		LIST *tuition_payment_list = list_new();
		TUITION_PAYMENT *tuition_payment;

		tuition_payment =
			tuition_payment_new(
				entity_new(
					student_full_name,
					street_address ),
				season_name,
				year,
				entity_new(
					payor_full_name,
					payor_street_address ),
				payment_date_time );

		list_set( tuition_payment_list, tuition_payment );

		registration_list_fetch_update(
			tuition_payment_registration_list(
				tuition_payment_list ),
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
	char *program_name = {0};

	/* If no enrollment */
	/* ---------------- */
	if ( !list_length( tuition_payment->
				registration->
				enrollment_list ) )
	{
		printf(
"<h3>Warning: No enrollments for this registration. Therefore, the tuition payment transaction has no program name. Best to delete this tuition payment and first insert the enrollment.</h3>\n" );
	}
	else
	{
		program_name =
			enrollment_list_program_name(
				tuition_payment->
					registration->
					enrollment_list );
	}

	tuition_payment =
		/* ----------------------- */
		/* Returns tuition_payment */
		/* ----------------------- */
		tuition_payment_steady_state(
			tuition_payment,
			tuition_payment->payment_amount,
			tuition_payment->merchant_fees_expense );

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
			tuition_payment->
				payor_entity->
				full_name,
			tuition_payment->
				payor_entity->
				street_address,
			tuition_payment->
				transaction_date_time,
			program_name,
			tuition_payment->payment_amount,
			tuition_payment->merchant_fees_expense,
			tuition_payment->
				tuition_payment_receivable_credit_amount,
			tuition_payment->net_payment_amount,
			account_cash( (char *)0 ),
			account_receivable( (char *)0 ),
			account_fees_expense( (char *)0 ) ) ) )
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

	tuition_payment_update(
		tuition_payment->net_payment_amount,
		tuition_payment->transaction_date_time,
		tuition_payment->registration->student_entity->full_name,
		tuition_payment->registration->student_entity->street_address,
		tuition_payment->offering->semester->season_name,
		tuition_payment->offering->semester->year,
		tuition_payment->payor_entity->full_name,
		tuition_payment->payor_entity->street_address,
		tuition_payment->payment_date_time );

	tuition_payment_list = list_new();
	list_set( tuition_payment_list, tuition_payment );

	return tuition_payment_list;
}

void tuition_payment_trigger_predelete(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time )
{
	TUITION_PAYMENT *tuition_payment;

	if ( ! ( tuition_payment =
			tuition_payment_fetch(
				student_full_name,
				street_address,
				season_name,
				year,
				payor_full_name,
				payor_street_address,
				payment_date_time,
				0 /* not fetch_registration */,
				0 /* fetch_enrollment_list */,
				0 /* fetch_offering */,
				0 /* fetch_course */,
				0 /* not fetch_program */,
				0 /* not fetch_transaction */ ) ) )
	{
		return;
	}

	if ( tuition_payment->transaction_date_time
	&&   *tuition_payment->transaction_date_time )
	{
		transaction_delete(
			tuition_payment->
				payor_entity->
				full_name,
			tuition_payment->
				payor_entity->
				street_address,
			tuition_payment->transaction_date_time );

		journal_account_name_list_propagate(
			tuition_payment->transaction_date_time,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			journal_delete(
				tuition_payment->
					payor_entity->
					full_name,
				tuition_payment->
					payor_entity->
					street_address,
				tuition_payment->transaction_date_time ) );
	}
}
