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
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time );

void tuition_payment_trigger_insert_update(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *student_full_name;
	char *street_address;
	char *course_name;
	char *season_name;
	int year;
	char *payor_full_name;
	char *payor_street_address;
	char *payment_date_time;
	char *state;

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
"Usage: %s student_full_name street_address course_name season_name year payor_full_name payor_street_address payment_date_time state\n",
			 argv[ 0 ] );
		fprintf(stderr,
			"state in {insert,update,predelete,delete,paypal}\n" );
		exit ( 1 );
	}

	student_full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	course_name = argv[ 3 ];
	season_name = argv[ 4 ];
	year = atoi( argv[ 5 ] );
	payor_full_name = argv[ 6 ];
	payor_street_address = argv[ 7 ];
	payment_date_time = argv[ 8 ];
	state = argv[ 9 ];

	if ( !year ) exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 )
	{
		tuition_payment_trigger_predelete(
			student_full_name,
			street_address,
			course_name,
			season_name,
			year,
			payor_full_name,
			payor_street_address,
			payment_date_time );
	}

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0 )
	{
		tuition_payment_trigger_insert_update(
			student_full_name,
			street_address,
			course_name,
			season_name,
			year,
			payor_full_name,
			payor_street_address,
			payment_date_time );

		enrollment_trigger(
			student_full_name,
			street_address,
			course_name,
			season_name,
			year );
	}

	if ( strcmp( state, "delete" ) == 0 )
	{
		enrollment_trigger(
			student_full_name,
			street_address,
			course_name,
			season_name,
			year );
	}


	return 0;
}

void tuition_payment_trigger_insert_update(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time )
{
	TUITION_PAYMENT *tuition_payment;
	int transaction_seconds_to_add = 0;

	if ( ! ( tuition_payment =
			tuition_payment_fetch(
				student_full_name,
				street_address,
				course_name,
				season_name,
				year,
				payor_full_name,
				payor_street_address,
				payment_date_time,
				1 /* fetch_enrollment */ ) ) )
	{
		return;
	}

	tuition_payment =
		tuition_payment_steady_state(
			tuition_payment,
			tuition_payment->payment_amount,
			tuition_payment->merchant_fees_expense );

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
				payment_date_time,
			tuition_payment->
				enrollment->
				offering->
				course->
				program->
				program_name,
			tuition_payment->payment_amount,
			tuition_payment->merchant_fees_expense,
			tuition_payment->
				tuition_payment_receivable_credit_amount,
			tuition_payment->tuition_payment_cash_debit_amount,
			entity_self_paypal_cash_account_name(),
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
		student_full_name,
		street_address,
		course_name,
		season_name,
		year,
		payor_full_name,
		payor_street_address,
		payment_date_time );
}

void tuition_payment_trigger_predelete(
			char *student_full_name,
			char *street_address,
			char *course_name,
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
				course_name,
				season_name,
				year,
				payor_full_name,
				payor_street_address,
				payment_date_time,
				0 /* not fetch_enrollment */ ) ) )
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
