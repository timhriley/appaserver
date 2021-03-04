/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/course_drop_trigger.c	*/
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
#include "registration.h"
#include "offering.h"
#include "account.h"
#include "transaction.h"
#include "journal.h"
#include "course.h"
#include "course_drop.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void course_drop_trigger_predelete(
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year );

/* Returns list of one (COURSE_DROP *) */
/* ---------------------------------- */
LIST *course_drop_trigger_insert_update(
			COURSE_DROP *course_drop );

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
		course_drop_trigger_predelete(
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
		LIST *course_drop_list;
		COURSE_DROP *course_drop;

		if ( ! ( course_drop =
				course_drop_fetch(
					student_full_name,
					student_street_address,
					course_name,
					season_name,
					year,
					1 /* fetch_enrollment */,
					1 /* fetch_offering */,
					1 /* fetch_course */,
					1 /* fetch_registration */ ) ) )
		{
			exit( 0 );
		}

		course_drop_list =
			/* ---------------------------------- */
			/* Returns list of one (COURSE_DROP *) */
			/* ---------------------------------- */
			course_drop_trigger_insert_update(
				course_drop );

		if ( list_length( course_drop_list ) )
		{
			registration_list_fetch_update(
			    enrollment_list_registration_list(
				course_drop_list_enrollment_list(
					course_drop_list ) ) );

			offering_list_fetch_update(
			   enrollment_list_offering_list(
				course_drop_list_enrollment_list(
					course_drop_list ) ) );
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

/* Returns list of one (COURSE_DROP *) */
/* ---------------------------------- */
LIST *course_drop_trigger_insert_update(
			COURSE_DROP *course_drop )
{
	LIST *course_drop_list;
	int transaction_seconds_to_add = 0;

	if ( !course_drop->enrollment
	||   !course_drop->enrollment->offering
	||   !course_drop->enrollment->offering->course )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: empty enrollment, offering or course.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	course_drop =
		course_drop_steady_state(
			course_drop,
			course_drop->course_drop_date_time,
			course_drop->payor_entity );

	if ( !course_drop->transaction_date_time
	||   !*course_drop->transaction_date_time )
	{
		course_drop->transaction_date_time =
			date_now19( date_utc_offset() );
	}

	if ( course_drop_set_transaction(
			&transaction_seconds_to_add,
			course_drop,
			course_drop->course_drop_date_time,
			course_drop->course_name,
			course_drop->enrollment->offering->course->program_name,
			course_drop->enrollment->offering->revenue_account,
			account_payable( (char *)0 ) ) )
	{
		TRANSACTION *t = course_drop->course_drop_transaction;

		course_drop->transaction_date_time =
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

	course_drop_update(
		course_drop->course_drop_date_time,
		course_drop->payor_entity->full_name,
		course_drop->payor_entity->street_address,
		course_drop->transaction_date_time,
		course_drop->student_entity->full_name,
		course_drop->student_entity->street_address,
		course_drop->course_name,
		course_drop->semester->season_name,
		course_drop->semester->year );

	course_drop_list = list_new();
	list_set( course_drop_list, course_drop );

	return course_drop_list;
}

void course_drop_trigger_predelete(
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year )
{
	COURSE_DROP *course_drop;

	if ( ! ( course_drop =
			course_drop_fetch(
				student_full_name,
				student_street_address,
				course_name,
				season_name,
				year,
				0 /* not fetch_enrollment */,
				0 /* not fetch_offering */,
				0 /* not fetch_course */,
				0 /* not fetch_registration */ ) ) )
	{
		return;
	}

	if ( course_drop->transaction_date_time
	&&   *course_drop->transaction_date_time )
	{
		transaction_delete(
			course_drop->student_entity->full_name,
			course_drop->student_entity->street_address,
			course_drop->transaction_date_time );

		journal_account_name_list_propagate(
			course_drop->transaction_date_time,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			journal_delete(
				course_drop->student_entity->full_name,
				course_drop->student_entity->street_address,
				course_drop->transaction_date_time ) );
	}
}
