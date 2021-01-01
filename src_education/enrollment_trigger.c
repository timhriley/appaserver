/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/enrollment_trigger.c	*/
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
#include "enrollment.h"
#include "registration.h"
#include "offering.h"
#include "transaction.h"
#include "journal.h"
#include "course.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void enrollment_trigger_predelete(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

void enrollment_trigger_insert_update(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year );

int main( int argc, char **argv )
{
	char *application_name;
	char *student_full_name;
	char *street_address;
	char *course_name;
	char *season_name;
	int year;
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
		fprintf( stderr,
"Usage: %s full_name street_address course_name season_name year state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	student_full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	course_name = argv[ 3 ];
	season_name = argv[ 4 ];
	year = atoi( argv[ 5 ] );
	state = argv[ 6 ];

	if ( !year ) exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 )
	{
		enrollment_trigger_predelete(
			student_full_name,
			street_address,
			course_name,
			season_name,
			year );
	}
	else
	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0 )
	{
		char sys_string[ 1024 ];

		enrollment_trigger_insert_update(
			student_full_name,
			street_address,
			course_name,
			season_name,
			year );

		sprintf(sys_string,
		"registration_trigger \"%s\" \"%s\" \"%s\" %d enrollment",
			student_full_name,
			street_address,
			season_name,
			year );

		if ( system( sys_string ) ){}

		sprintf(sys_string,
			"offering_trigger \"%s\" \"%s\" %d enrollment",
			course_name,
			season_name,
			year );

		if ( system( sys_string ) ){}
	}
	else
	if ( strcmp( state, "delete" ) ==  0 )
	{
		char sys_string[ 1024 ];

		sprintf(sys_string,
		"registration_trigger \"%s\" \"%s\" \"%s\" %d enrollment",
			student_full_name,
			street_address,
			season_name,
			year );

		if ( system( sys_string ) ){}

		sprintf(sys_string,
			"offering_trigger \"%s\" \"%s\" %d enrollment",
			course_name,
			season_name,
			year );

		if ( system( sys_string ) ){}
	}

	return 0;
}

void enrollment_trigger_insert_update(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year )
{
	ENROLLMENT *enrollment;
	int transaction_seconds_to_add = 0;

	if ( ! ( enrollment =
			enrollment_fetch(
				student_full_name,
				street_address,
				course_name,
				season_name,
				year,
				1 /* fetch_tuition_payment_list */,
				1 /* fetch_tuition_refund_list */,
				1 /* fetch_offering */,
				1 /* fetch_registration */ ) ) )
	{
		return;
	}

	if ( !enrollment->registration )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: registration is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !enrollment->offering )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: offering is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !enrollment->offering->course )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: course is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	enrollment =
		enrollment_steady_state(
			&transaction_seconds_to_add,
			enrollment,
			0.0 /* zero deposit_amount shows not a refund */ );

	if ( enrollment->enrollment_transaction )
	{
		TRANSACTION *t = enrollment->enrollment_transaction;

		enrollment->transaction_date_time =
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
}

void enrollment_trigger_predelete(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year )
{
	ENROLLMENT *enrollment;

	if ( ! ( enrollment =
			enrollment_fetch(
				student_full_name,
				street_address,
				course_name,
				season_name,
				year,
				0 /* not fetch_tution_payment_list */,
				0 /* not fetch_tution_refund_list */,
				0 /* not fetch_offering */,
				0 /* not fetch_registration */ ) ) )
	{
		return;
	}

	if ( enrollment->transaction_date_time
	&&   *enrollment->transaction_date_time )
	{
		transaction_delete(
			enrollment->registration->student_full_name,
			enrollment->registration->street_address,
			enrollment->transaction_date_time );

		journal_account_name_list_propagate(
			enrollment->transaction_date_time,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			journal_delete(
				enrollment->registration->student_full_name,
				enrollment->registration->street_address,
				enrollment->transaction_date_time ) );
	}
}
