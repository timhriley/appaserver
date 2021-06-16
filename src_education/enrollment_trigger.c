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
void enrollment_trigger_predelete(
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year );

/* Returns list of one (ENROLLMENT *) */
/* ---------------------------------- */
LIST *enrollment_trigger_insert_update(
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
"Usage: %s student_full_name student_street_address course_name season_name year state\n",
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

	if ( strcmp( state, "predelete" ) == 0 )
	{
		enrollment_trigger_predelete(
			student_full_name,
			student_street_address,
			course_name,
			season_name,
			year );
		exit( 0 );
	}

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0 )
	{
		LIST *enrollment_list;
		ENROLLMENT *enrollment;

		if ( ! ( enrollment =
				enrollment_fetch(
					student_full_name,
					student_street_address,
					course_name,
					season_name,
					year,
					1 /* fetch_offering */,
					1 /* fetch_course */,
					0 /* not fetch_program */,
					1 /* fetch_registration */ ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: enrollment_fetch() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		enrollment_list =
			/* ---------------------------------- */
			/* Returns list of one (ENROLLMENT *) */
			/* ---------------------------------- */
			enrollment_trigger_insert_update(
				enrollment );

		if ( list_length( enrollment_list ) )
		{
			registration_list_fetch_update(
				enrollment_list_registration_list(
					enrollment_list ) );

			offering_list_fetch_update(
				registration_list_offering_list(
					enrollment_list_registration_list(
						enrollment_list ) ) );
		}
	}

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

/* Returns list of one (ENROLLMENT *) */
/* ---------------------------------- */
LIST *enrollment_trigger_insert_update(
			ENROLLMENT *enrollment )
{
	LIST *enrollment_list;
	int transaction_seconds_to_add = 0;

	enrollment =
		enrollment_steady_state(
			enrollment,
			enrollment->enrollment_date_time,
			enrollment->payor_entity );

	if ( !enrollment->offering )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty offering.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !enrollment->transaction_date_time
	||   !*enrollment->transaction_date_time )
	{
		enrollment->transaction_date_time =
			enrollment->enrollment_date_time;
	}

	if ( enrollment_set_transaction(
			&transaction_seconds_to_add,
			enrollment,
			account_receivable( (char *)0 ),
			enrollment->offering->revenue_account,
			(LIST *)0 /* liability_entity_list */ ) )
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

	enrollment_list = list_new();
	list_set( enrollment_list, enrollment );

	return enrollment_list;
}

void enrollment_trigger_predelete(
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year )
{
	ENROLLMENT *enrollment;

	if ( ! ( enrollment =
			enrollment_fetch(
				student_full_name,
				student_street_address,
				course_name,
				season_name,
				year,
				0 /* not fetch_offering */,
				0 /* not fetch_course */,
				0 /* not fetch_program */,
				0 /* not fetch_registration */ ) ) )
	{
		return;
	}

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
