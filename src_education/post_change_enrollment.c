/* ------------------------------------------------------- */
/* $APPASERVER_HOME/src_education/post_change_enrollment.c */
/* ------------------------------------------------------- */
/* 							   */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "enrollment.h"
#include "registration.h"
#include "offering.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_enrollment(
				char *student_full_name,
				char *student_street_address,
				char *course_name,
				char *season_name,
				int year );

int main( int argc, char **argv )
{
	char *application_name;
	char *student_full_name;
	char *student_street_address;
	char *course_name;
	char *season_name;
	int year;
	char *state;

	/* Exits if fails. */
	/* --------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s full_name street_address course_name season_name year state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	student_full_name = argv[ 1 ];
	student_street_address = argv[ 2 ];
	course_name = argv[ 3 ];
	season_name = argv[ 4 ];
	year = atoi( argv[ 5 ] );
	state = argv[ 6 ];

	if ( !year ) exit( 0 );

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0
	||   strcmp( state, "delete" ) ==  0 )
	{
		post_change_enrollment(
			student_full_name,
			student_street_address,
			course_name,
			season_name,
			year );
	}

	return 0;

} /* main() */

void post_change_enrollment(
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year )
{
	OFFERING *offering;
	REGISTRATION *registration;

	if ( ( offering =
			offering_fetch(
				course_name,
				season_name,
				year ) ) )
	{
		offering->offering_enrollment_count =
			offering_enrollment_count(
				offering->enrollment_list );

		offering->offering_capacity_available =
			offering_capacity_available(
				offering->class_capacity,
				offering->offering_enrollment_count );

		offering_refresh(
			offering->offering_enrollment_count,
			offering->offering_capacity_available,
			offering->enrollment_list,
			offering->course_name,
			offering->season_name,
			offering->year );
	}

	if ( ( registration =
			registration_fetch(
				student_full_name,
				student_street_address,
				season_name,
				year ) ) )
	{
		registration->registration_tuition =
			registration_tuition(
				registration->
					registration_enrollment_list );

		registration->registration_payment_total =
			registration_payment_total(
				registration->
					registration_payment_list );

		registration->invoice_amount_due =
			registration_invoice_amount_due(
				registration->registration_tuition,
				registration->registration_payment_total );

		registration->registration_revenue_transaction =
			registration_revenue_transaction(
				student_full_name,
				street_address,
				registration_date_time,
				registration->registration_tuition,
				ledger_receivable_account(),
				offering_revenue_account() );

		registration->
			registration_revenue_transaction->
			transaction_date_time =
				registration_refresh(
					registration->
					     registration_tuition,
					registration->
					     registration_payment_total,
					registration->
					     registration_invoice_amount_due,
					registration->
					     registration_revenue_transaction->
						transaction_date_time,
					registration->student_full_name,
					     registration->
						student_street_address,
					registration->
					     season_name,
					registration->
					     year );
	}
}

