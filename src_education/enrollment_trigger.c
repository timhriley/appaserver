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
#include "enrollment_fns.h"
#include "registration.h"
#include "registration_fns.h"
#include "offering.h"
#include "transaction.h"
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
	char *transaction_date_time = {0};

	if ( ! ( enrollment =
			enrollment_fetch(
				student_full_name,
				street_address,
				course_name,
				season_name,
				year,
				1 /* fetch_payment_list */ ) ) )
	{
		return;
	}

	enrollment =
		enrollment_steady_state(
			enrollment->registration,
			enrollment->offering,
			enrollment->enrollment_payment_list,
			/* ----------------------------- */
			/* Don't take anything from here */
			/* ----------------------------- */
			enrollment );

	if ( enrollment->enrollment_transaction )
	{
		TRANSACTION *t = enrollment->enrollment_transaction;

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

	enrollment_update(
		transaction_date_time,
		enrollment->
			registration->
			student_full_name,
		enrollment->
			registration->
			street_address,
		enrollment->
			offering->
			course->
			course_name,
		enrollment->
			offering->
			season_name,
		enrollment->
			offering->
			year );
}

void enrollment_trigger_predelete(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year )
{
if ( student_full_name ){}
if ( street_address ){}
if ( course_name ){}
if ( season_name ){}
if ( year ){}
}
