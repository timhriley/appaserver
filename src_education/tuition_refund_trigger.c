/* --------------------------------------------------------	*/
/* $APPASERVER_HOME/src_education/tuition_refund_trigger.c	*/
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
#include "entity_self.h"
#include "account.h"
#include "tuition_refund.h"
#include "enrollment.h"
#include "registration.h"
#include "transaction.h"
#include "journal.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void tuition_refund_trigger_predelete(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time );

/* Returns list of one (TUITION_REFUND *) */
/* -------------------------------------- */
LIST *tuition_refund_trigger_insert_update(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time,
			char *preupdate_transaction_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *student_full_name;
	char *street_address;
	char *season_name;
	int year;
	char *payor_full_name;
	char *payor_street_address;
	char *refund_date_time;
	char *preupdate_transaction_date_time;
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
"Usage: %s student_full_name street_address season_name year payor_full_name payor_street_address refund_date_time preupdate_transaction_date_time state\n",
			 argv[ 0 ] );
		fprintf(stderr,
			"state in {insert,update,predelete,delete,deposit}\n" );
		exit ( 1 );
	}

	student_full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	season_name = argv[ 3 ];
	year = atoi( argv[ 4 ] );
	payor_full_name = argv[ 5 ];
	payor_street_address = argv[ 6 ];
	refund_date_time = argv[ 7 ];
	preupdate_transaction_date_time = argv[ 8 ];
	state = argv[ 9 ];

	if ( !year ) exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 )
	{
		tuition_refund_trigger_predelete(
			student_full_name,
			street_address,
			season_name,
			year,
			payor_full_name,
			payor_street_address,
			refund_date_time );
	}

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0 )
	{
		LIST *tuition_refund_list;

		tuition_refund_list =
			tuition_refund_trigger_insert_update(
				student_full_name,
				street_address,
				season_name,
				year,
				payor_full_name,
				payor_street_address,
				refund_date_time,
				preupdate_transaction_date_time );

		if ( list_length( tuition_refund_list ) )
		{
			registration_list_fetch_update(
				/* ------ */
				/* Caches */
				/* ------ */
				tuition_refund_registration_list(
					tuition_refund_list ),
				season_name,
				year );
		}
	}

	if ( strcmp( state, "delete" ) == 0 )
	{
		LIST *tuition_refund_list = list_new();
		TUITION_REFUND *tuition_refund;

		tuition_refund =
			tuition_refund_new(
			entity_new(
				student_full_name,
				street_address ),
			season_name,
			year,
			entity_new(
				payor_full_name,
				payor_street_address ),
			refund_date_time );

		list_set( tuition_refund_list, tuition_refund );

		registration_list_fetch_update(
			/* ------ */
			/* Caches */
			/* ------ */
			tuition_refund_registration_list(
				tuition_refund_list ),
			season_name,
			year );
	}

	return 0;
}

LIST *tuition_refund_trigger_insert_update(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time,
			char *preupdate_transaction_date_time )
{
	TUITION_REFUND *tuition_refund;
	LIST *tuition_refund_list;
	int transaction_seconds_to_add = 0;
	char *program_name = {0};
	char *revenue_account = {0};

	if ( ! ( tuition_refund =
			tuition_refund_fetch(
				student_full_name,
				street_address,
				season_name,
				year,
				payor_full_name,
				payor_street_address,
				refund_date_time,
				1 /* fetch_registration */,
				1 /* fetch_enrollment_list */,
				1 /* fetch_offering */,
				1 /* fetch_course */,
				0 /* fetch_program */ ) ) )
	{
		return (LIST *)0;
	}

	/* If no enrollment */
	/* ---------------- */
	if ( !list_length( tuition_refund->
				registration->
				enrollment_list ) )
	{
		printf(
"<h3>Warning: No enrollments for this registration. Therefore, no refund transaction was inserted. Best to delete this tuition refund and first insert the enrollment and tuition payment.</h3>\n" );

	}
	else
	{
		program_name =
			enrollment_list_program_name(
				tuition_refund->
					registration->
					enrollment_list );

		revenue_account =
			enrollment_list_revenue_account(
				tuition_refund->
					registration->
					enrollment_list );
	}

	tuition_refund =
		tuition_refund_steady_state(
			tuition_refund,
			tuition_refund->refund_amount,
			tuition_refund->merchant_fees_expense );

	if ( !tuition_refund->transaction_date_time )
	{
		tuition_refund->transaction_date_time =
			transaction_race_free(
				tuition_refund->
					refund_date_time );
	}

	if ( ( tuition_refund->tuition_refund_transaction =
		tuition_refund_transaction(
			&transaction_seconds_to_add,
			tuition_refund->
				payor_entity->
				full_name,
			tuition_refund->
				payor_entity->
				street_address,
			tuition_refund->
				transaction_date_time,
			program_name,
			tuition_refund->refund_amount,
			tuition_refund->merchant_fees_expense,
			tuition_refund->net_refund_amount,
			account_cash( (char *)0 ),
			account_fees_expense( (char *)0 ),
			revenue_account ) ) )
	{
		tuition_refund->transaction_date_time =
			tuition_refund->tuition_refund_transaction->
				transaction_date_time;
	}
	else
	{
		tuition_refund->transaction_date_time = (char *)0;
	}

	if ( tuition_refund->transaction_date_time
	&&   *tuition_refund->transaction_date_time )
	{
		TRANSACTION *t = tuition_refund->tuition_refund_transaction;

		tuition_refund->transaction_date_time =
			transaction_program_refresh(
				t->full_name,
				t->street_address,
				t->transaction_date_time,
				preupdate_transaction_date_time,
				t->program_name,
				t->transaction_amount,
				t->memo,
				0 /* check_number */,
				t->lock_transaction,
				t->journal_list );
	}

	tuition_refund_update(
		tuition_refund->refund_amount,
		tuition_refund->net_refund_amount,
		tuition_refund->transaction_date_time,
		student_full_name,
		street_address,
		season_name,
		year,
		payor_full_name,
		payor_street_address,
		refund_date_time );

	tuition_refund_list = list_new();
	list_set( tuition_refund_list, tuition_refund );
	return tuition_refund_list;
}

void tuition_refund_trigger_predelete(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time )
{
	TUITION_REFUND *tuition_refund;

	if ( ! ( tuition_refund =
			tuition_refund_fetch(
				student_full_name,
				street_address,
				season_name,
				year,
				payor_full_name,
				payor_street_address,
				refund_date_time,
				0 /* not fetch_registration */,
				0 /* not fetch_enrollment_list */,
				0 /* not fetch_offering */,
				0 /* not fetch_course */,
				0 /* not fetch_program */ ) ) )
	{
		return;
	}

	if ( tuition_refund->transaction_date_time
	&&   *tuition_refund->transaction_date_time )
	{
		transaction_delete(
			tuition_refund->
				payor_entity->full_name,
			tuition_refund->
				payor_entity->
				street_address,
			tuition_refund->transaction_date_time );

		journal_account_name_list_propagate(
			tuition_refund->transaction_date_time,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			journal_delete(
				tuition_refund->
					payor_entity->
					full_name,
				tuition_refund->
					payor_entity->
					street_address,
				tuition_refund->transaction_date_time ) );
	}
}

TUITION_REFUND *tuition_refund_new(
			ENTITY *student_entity,
			char *season_name,
			int year,
			ENTITY *payor_entity,
			char *refund_date_time )
{
	TUITION_REFUND *tuition_refund = tuition_refund_calloc();

	tuition_refund->registration =
		registration_new(
			student_entity,
			season_name,
			year );

	tuition_refund->payor_entity = payor_entity;
	tuition_refund->refund_date_time = refund_date_time;

	return tuition_refund;
}

