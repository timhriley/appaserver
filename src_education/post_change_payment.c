/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/post_change_payment.c	*/
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
#include "registration.h"
#include "registration_fns.h"
#include "offering.h"
#include "transaction.h"
#include "course.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_payment_predelete(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time );

void post_change_payment_insert_update(
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
		fprintf( stderr,
"Usage: %s student_full_name street_address course_name season_name year payor_full_name payor_street_address deposit_date_time state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	student_full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	payor_full_name = argv[ 3 ];
	payor_street_address = argv[ 4 ];
	course_name = argv[ 5 ];
	season_name = argv[ 6 ];
	year = atoi( argv[ 7 ] );
	deposit_date_time = argv[ 8 ];
	state = argv[ 9 ];

	if ( !year ) exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 )
	{
		post_change_payment_predelete(
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
	||   strcmp( state, "update" ) ==  0 )
	{
		post_change_payment_insert_update(
			student_full_name,
			street_address,
			course_name,
			season_name,
			year,
			payor_full_name,
			payor_street_address,
			deposit_date_time );
	}

	return 0;
}

void post_change_payment_insert_update(
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
				1 /* fetch_enrollment */,
				1 /* fetch_deposit */ ) ) )
	{
		return;
	}

	payment =
		payment_steady_state(
			payment->enrollment,
			payment->deposit,
			payment->deposit_amount,
			payment->deposit_transaction_fee,
			payment->enrollment->offering->course->program_name );

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
		payment->
			enrollment->
			registration->
			student_full_name,
		payment->
			enrollment->
			registration->
			street_address,
		payment->
			enrollment->
			offering->
			course->
			course_name,
		payment->
			enrollment->
			offering->
			season_name,
		payment->
			enrollment->
			offering->
			year,
		payment->
			deposit->
			payor_entity->
			full_name,
		payment->
			deposit->
			payor_entity->
			street_address,
		payment->
			deposit->
			deposit_date_time );
}

void post_change_payment_predelete(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time )
{
if ( student_full_name ){}
if ( street_address ){}
if ( course_name ){}
if ( season_name ){}
if ( year ){}
if ( payor_full_name ){}
if ( payor_street_address ){}
if ( deposit_date_time ){}
}
