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
#include "account.h"
#include "transaction.h"
#include "journal.h"
#include "course.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void enrollment_trigger_insert(
			ENROLLMENT *enrollment );

void enrollment_trigger_update(
			ENROLLMENT *enrollment,
			char *preupdate_course_name );

void enrollment_trigger_predelete(
			ENROLLMENT *enrollment );

int main( int argc, char **argv )
{
	char *application_name;
	char *student_full_name;
	char *student_street_address;
	char *course_name;
	char *season_name;
	int year;
	char *state;
	char *preupdate_course_name;
	ENROLLMENT *enrollment = {0};

	/* Exits if fails. */
	/* --------------- */
	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s student_full_name student_street_address course_name season_name year state preupdate_course_name\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	student_full_name = argv[ 1 ];
	student_street_address = argv[ 2 ];
	course_name = argv[ 3 ];
	season_name = argv[ 4 ];
	year = atoi( argv[ 5 ] );
	state = argv[ 6 ];
	preupdate_course_name = argv[ 7 ];

	if ( !year ) exit( 0 );

	if ( strcmp( state, "delete" ) != 0 )
	{
		enrollment =
			enrollment_fetch(
				student_full_name,
				student_street_address,
				course_name,
				season_name,
				year,
				1 /* fetch_offering */,
				1 /* fetch_course */,
				0 /* not fetch_program */,
				1 /* fetch_registration */ );
	}

	if ( strcmp( state, "predelete" ) == 0 )
	{
		enrollment_trigger_predelete( enrollment );
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		enrollment_trigger_insert( enrollment );
	}
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		enrollment_trigger_update(
			enrollment,
			preupdate_course_name );
	}
	else
	if ( strcmp( state, "delete" ) ==  0 )
	{
		registration_fetch_update(
			student_full_name,
			student_street_address,
			season_name,
			year );

		offering_fetch_update(
			course_name,
			season_name,
			year );
	}
	return 0;
}

void enrollment_trigger_insert(
			ENROLLMENT *enrollment )
{
	int transaction_seconds_to_add = 0;

	if ( !enrollment ) return;

	enrollment =
		enrollment_steady_state(
			enrollment,
			enrollment->enrollment_date_time,
			enrollment->payor_entity );

	if ( !enrollment->payor_entity )
	{
		return;
	}

	if ( !enrollment->enrollment_date_time
	||   !*enrollment->enrollment_date_time )
	{
		return;
	}

	if ( !enrollment->offering )
	{
		return;
	}

	if ( !enrollment->transaction_date_time
	||   !*enrollment->transaction_date_time )
	{
		enrollment->transaction_date_time =
			transaction_race_free(
				enrollment->enrollment_date_time );
	}

	enrollment->enrollment_transaction =
		enrollment_transaction(
			&transaction_seconds_to_add,
			enrollment->payor_entity->full_name,
			enrollment->payor_entity->street_address,
			enrollment->transaction_date_time,
			enrollment->offering->course->program_name,
			enrollment->offering->course_name,
			enrollment->offering->course_price,
			enrollment->liability_entity_prepaid,
			account_receivable( (char *)0 ),
			account_payable( (char *)0 ),
			enrollment->offering->revenue_account );

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
				t->lock_transaction,
				t->journal_list );
	}
	else
	{
		enrollment->transaction_date_time = (char *)0;
	}

	enrollment_update(
		enrollment->enrollment_date_time,
		enrollment->payor_entity->full_name,
		enrollment->payor_entity->street_address,
		enrollment->transaction_date_time,
		enrollment->student_entity->full_name,
		enrollment->student_entity->street_address,
		enrollment->offering->course->course_name,
		enrollment->semester->season_name,
		enrollment->semester->year );

	registration_fetch_update(
		enrollment->student_entity->full_name,
		enrollment->student_entity->street_address,
		enrollment->semester->season_name,
		enrollment->semester->year );

	offering_fetch_update(
		enrollment->offering->course->course_name,
		enrollment->semester->season_name,
		enrollment->semester->year );
}

void enrollment_trigger_update(
			ENROLLMENT *enrollment,
			char *preupdate_course_name )
{
	int transaction_seconds_to_add = 0;

	if ( !enrollment ) return;

	enrollment =
		enrollment_steady_state(
			enrollment,
			enrollment->enrollment_date_time,
			enrollment->payor_entity );

	if ( !enrollment->payor_entity )
	{
		return;
	}

	if ( !enrollment->enrollment_date_time
	||   !*enrollment->enrollment_date_time )
	{
		return;
	}

	if ( !enrollment->offering )
	{
		return;
	}

	if ( !enrollment->transaction_date_time
	||   !*enrollment->transaction_date_time )
	{
		enrollment->transaction_date_time =
			transaction_race_free(
				enrollment->enrollment_date_time );
	}

	enrollment->enrollment_transaction =
		enrollment_transaction(
			&transaction_seconds_to_add,
			enrollment->payor_entity->full_name,
			enrollment->payor_entity->street_address,
			enrollment->transaction_date_time,
			enrollment->offering->course->program_name,
			enrollment->offering->course_name,
			enrollment->offering->course_price,
			enrollment->liability_entity_prepaid,
			account_receivable( (char *)0 ),
			account_payable( (char *)0 ),
			enrollment->offering->revenue_account );

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
				t->lock_transaction,
				t->journal_list );
	}
	else
	{
		enrollment->transaction_date_time = (char *)0;
	}

	enrollment_update(
		enrollment->enrollment_date_time,
		enrollment->payor_entity->full_name,
		enrollment->payor_entity->street_address,
		enrollment->transaction_date_time,
		enrollment->student_entity->full_name,
		enrollment->student_entity->street_address,
		enrollment->offering->course->course_name,
		enrollment->semester->season_name,
		enrollment->semester->year );

	registration_fetch_update(
		enrollment->student_entity->full_name,
		enrollment->student_entity->street_address,
		enrollment->semester->season_name,
		enrollment->semester->year );

	offering_fetch_update(
		enrollment->offering->course->course_name,
		enrollment->semester->season_name,
		enrollment->semester->year );

	offering_fetch_update(
		preupdate_course_name,
		enrollment->semester->season_name,
		enrollment->semester->year );
}

void enrollment_trigger_predelete(
			ENROLLMENT *enrollment )
{
	if ( enrollment->transaction_date_time
	&&   *enrollment->transaction_date_time )
	{
		transaction_delete(
			enrollment->student_entity->full_name,
			enrollment->student_entity->street_address,
			enrollment->transaction_date_time );

		journal_account_name_list_propagate(
			enrollment->transaction_date_time,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			journal_delete(
				enrollment->student_entity->full_name,
				enrollment->student_entity->street_address,
				enrollment->transaction_date_time ) );
	}
}
