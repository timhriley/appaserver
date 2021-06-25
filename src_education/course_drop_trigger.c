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
void course_drop_trigger_insert(
			COURSE_DROP *course_drop );

void course_drop_trigger_update(
			COURSE_DROP *course_drop,
			char *preupdate_course_name );

void course_drop_trigger_predelete(
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
	char *preupdate_course_name;
	COURSE_DROP *course_drop = {0};

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
		course_drop =
			course_drop_fetch(
				student_full_name,
				student_street_address,
				course_name,
				season_name,
				year,
				1 /* fetch_enrollment */,
				1 /* fetch_offering */,
				1 /* fetch_course */,
				1 /* fetch_registration */ );
	}

	if ( strcmp( state, "predelete" ) == 0 )
	{
		course_drop_trigger_predelete( course_drop );
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		course_drop_trigger_insert( course_drop );
	}
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		course_drop_trigger_update(
			course_drop,
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

void course_drop_trigger_insert(
			COURSE_DROP *course_drop )
{
	int transaction_seconds_to_add = 0;

	if ( !course_drop
	||   !course_drop->enrollment
	||   !course_drop->enrollment->offering
	||   !course_drop->enrollment->offering->course )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: empty enrollment, offering or course.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return;
	}

	course_drop =
		/* ------------------------------------- */
		/* Sets course_drop_receivable_expecting */
		/* ------------------------------------- */
		course_drop_steady_state(
			course_drop,
			course_drop->course_drop_date_time,
			course_drop->payor_entity );

	if ( course_drop->refund_due
	&&   course_drop->payor_entity )
	{
		if ( !course_drop->transaction_date_time
		||   !*course_drop->transaction_date_time )
		{
			course_drop->transaction_date_time =
				transaction_race_free(
					course_drop->course_drop_date_time );
		}
	
		if ( ( course_drop->course_drop_transaction =
			course_drop_transaction(
				&transaction_seconds_to_add,
				course_drop->
					payor_entity->
					full_name,
				course_drop->
					payor_entity->
					street_address,
				course_drop->transaction_date_time,
				course_drop->
					enrollment->
					offering->
					course->
					program_name,
				course_drop->
					enrollment->
					offering->
					course_name,
				course_drop->
					enrollment->
					offering->
					course_price,
				course_drop->
					course_drop_receivable_expecting,
				account_receivable( (char *)0 ),
				account_payable( (char*)0 ),
				course_drop->
					enrollment->
					offering->
					revenue_account ) ) )
		{
			TRANSACTION *t = course_drop->course_drop_transaction;
	
			course_drop->transaction_date_time =
				course_drop->course_drop_transaction->
					transaction_date_time;
	
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
		else
		{
			course_drop->transaction_date_time = (char *)0;
		}
	}

	course_drop_update(
		course_drop->course_drop_date_time,
		course_drop->payor_entity,
		course_drop->transaction_date_time,
		course_drop->student_entity->full_name,
		course_drop->student_entity->street_address,
		course_drop->course_name,
		course_drop->semester->season_name,
		course_drop->semester->year );

	registration_fetch_update(
		course_drop->student_entity->full_name,
		course_drop->student_entity->street_address,
		course_drop->semester->season_name,
		course_drop->semester->year );

	offering_fetch_update(
		course_drop->enrollment->offering->course->course_name,
		course_drop->enrollment->semester->season_name,
		course_drop->enrollment->semester->year );
}

void course_drop_trigger_update(
			COURSE_DROP *course_drop,
			char *preupdate_course_name )
{
	int transaction_seconds_to_add = 0;

	if ( !course_drop
	||   !course_drop->enrollment
	||   !course_drop->enrollment->offering
	||   !course_drop->enrollment->offering->course )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: empty enrollment, offering or course.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return;
	}

	course_drop =
		/* ------------------------------------- */
		/* Sets course_drop_receivable_expecting */
		/* ------------------------------------- */
		course_drop_steady_state(
			course_drop,
			course_drop->course_drop_date_time,
			course_drop->payor_entity );

	if ( course_drop->refund_due
	&&   course_drop->payor_entity )
	{
		if ( !course_drop->transaction_date_time
		||   !*course_drop->transaction_date_time )
		{
			course_drop->transaction_date_time =
				transaction_race_free(
					course_drop->course_drop_date_time );
		}
	
		if ( ( course_drop->course_drop_transaction =
			course_drop_transaction(
				&transaction_seconds_to_add,
				course_drop->
					payor_entity->
					full_name,
				course_drop->
					payor_entity->
					street_address,
				course_drop->transaction_date_time,
				course_drop->
					enrollment->
					offering->
					course->
					program_name,
				course_drop->
					enrollment->
					offering->
					course_name,
				course_drop->
					enrollment->
					offering->
					course_price,
				course_drop->
					course_drop_receivable_expecting,
				account_receivable( (char*)0 ),
				account_payable( (char*)0 ),
				course_drop->
					enrollment->
					offering->
					revenue_account ) ) )
		{
			TRANSACTION *t = course_drop->course_drop_transaction;
	
			course_drop->transaction_date_time =
				course_drop->course_drop_transaction->
					transaction_date_time;
	
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
		else
		{
			course_drop->transaction_date_time = (char *)0;
		}
	}

	if ( !course_drop->refund_due
	&&   course_drop->payor_entity
	&&   course_drop->transaction_date_time
	&&   *course_drop->transaction_date_time )
	{
		transaction_delete(
			course_drop->payor_entity->full_name,
			course_drop->payor_entity->street_address,
			course_drop->transaction_date_time );

		journal_account_name_list_propagate(
			course_drop->transaction_date_time,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			journal_delete(
				course_drop->payor_entity->full_name,
				course_drop->payor_entity->street_address,
				course_drop->transaction_date_time ) );

		course_drop->transaction_date_time = (char *)0;
		course_drop->payor_entity = (ENTITY *)0;
	}

	course_drop_update(
		course_drop->course_drop_date_time,
		course_drop->payor_entity,
		course_drop->transaction_date_time,
		course_drop->student_entity->full_name,
		course_drop->student_entity->street_address,
		course_drop->course_name,
		course_drop->semester->season_name,
		course_drop->semester->year );

	registration_fetch_update(
		course_drop->student_entity->full_name,
		course_drop->student_entity->street_address,
		course_drop->semester->season_name,
		course_drop->semester->year );

	offering_fetch_update(
		course_drop->enrollment->offering->course->course_name,
		course_drop->enrollment->semester->season_name,
		course_drop->enrollment->semester->year );

	offering_fetch_update(
		preupdate_course_name,
		course_drop->enrollment->semester->season_name,
		course_drop->enrollment->semester->year );
}

void course_drop_trigger_predelete(
			COURSE_DROP *course_drop )
{
	if ( course_drop
	&&   course_drop->transaction_date_time
	&&   *course_drop->transaction_date_time
	&&   course_drop->payor_entity )
	{
		transaction_delete(
			course_drop->payor_entity->full_name,
			course_drop->payor_entity->street_address,
			course_drop->transaction_date_time );

		journal_account_name_list_propagate(
			course_drop->transaction_date_time,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			journal_delete(
				course_drop->payor_entity->full_name,
				course_drop->payor_entity->street_address,
				course_drop->transaction_date_time ) );
	}
}
