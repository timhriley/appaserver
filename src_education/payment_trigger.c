/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/payment_trigger.c	*/
/* ---------------------------------------------------- */
/* 							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "payment.h"
#include "payment_fns.h"
#include "transaction.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void payment_trigger_predelete(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time );

void payment_trigger_insert_update(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time );

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
	char *deposit_date_time;
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
"Usage: %s student_full_name street_address course_name season_name year payor_full_name payor_street_address deposit_date_time state\n",
			 argv[ 0 ] );
		fprintf(stderr,
			"state in {insert,update,predelete,delete,deposit}\n" );
		exit ( 1 );
	}

	student_full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	course_name = argv[ 3 ];
	season_name = argv[ 4 ];
	year = atoi( argv[ 5 ] );
	payor_full_name = argv[ 6 ];
	payor_street_address = argv[ 7 ];
	deposit_date_time = argv[ 8 ];
	state = argv[ 9 ];

	if ( !year ) exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 )
	{
		payment_trigger_predelete(
			student_full_name,
			street_address,
			course_name,
			season_name,
			year,
			payor_full_name,
			payor_street_address,
			deposit_date_time );
	}

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0
	||   strcmp( state, "deposit" ) ==  0 )
	{
		char sys_string[ 1024 ];

		payment_trigger_insert_update(
			student_full_name,
			street_address,
			course_name,
			season_name,
			year,
			payor_full_name,
			payor_street_address,
			deposit_date_time );

		if ( strcmp( state, "deposit" ) != 0 )
		{
			sprintf(sys_string,
		 "deposit_trigger \"%s\" \"%s\" \"%s\" %d \"%s\" payment",
				entity_escape_full_name( payor_full_name ),
				payor_street_address,
				season_name,
				year,
				deposit_date_time );

			if ( system( sys_string ) ){}
		}

		sprintf(sys_string,
		 "registration_trigger \"%s\" \"%s\" \"%s\" %d payment",
			entity_escape_full_name( student_full_name ),
			street_address,
			season_name,
			year );

		if ( system( sys_string ) ){}
	}

	return 0;
}

void payment_trigger_insert_update(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time )
{
	PAYMENT *payment;
	char *transaction_date_time = {0};

	if ( ! ( payment =
			payment_fetch(
				student_full_name,
				street_address,
				course_name,
				season_name,
				year,
				payor_full_name,
				payor_street_address,
				deposit_date_time,
				1 /* fetch_deposit */,
				1 /* fetch_enrollment */ ) ) )
	{
		return;
	}

	if ( !payment->deposit )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty deposit.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return;
	}

	if ( !payment->enrollment->offering->course->program_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: program_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	payment =
		payment_steady_state(
			payment->deposit /* in/out */,
			payment->deposit->deposit_amount,
			payment->deposit->transaction_fee,
			payment->enrollment->offering->course->program_name,
			/* ----------------------------- */
			/* Don't take anything from here */
			/* ----------------------------- */
			payment /* in only */ );

	if ( payment->payment_transaction )
	{
		TRANSACTION *t = payment->payment_transaction;

		transaction_date_time =
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

	payment_update(
		payment->payment_amount,
		payment->payment_fees_expense,
		payment->payment_gain_donation,
		transaction_date_time,
		student_full_name,
		street_address,
		course_name,
		season_name,
		year,
		payor_full_name,
		payor_street_address,
		deposit_date_time );
}

void payment_trigger_predelete(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time )
{
	PAYMENT *payment;

	if ( ! ( payment =
			payment_fetch(
				student_full_name,
				street_address,
				course_name,
				season_name,
				year,
				payor_full_name,
				payor_street_address,
				deposit_date_time,
				0 /* not fetch_deposit */,
				0 /* not fetch_enrollment */ ) ) )
	{
		return;
	}

	if ( payment->payment_transaction )
	{
		transaction_delete(
			payment->
				payment_transaction->
				full_name,
			payment->
				payment_transaction->
				street_address,
			payment->
				payment_transaction->
				transaction_date_time );

		journal_delete(
			payment->
				payment_transaction->
				full_name,
			payment->
				payment_transaction->
				street_address,
			payment->
				payment_transaction->
				transaction_date_time );

	}
}
