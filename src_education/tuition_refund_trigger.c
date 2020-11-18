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
#include "tuition_refund.h"
#include "tuition_refund_fns.h"
#include "transaction.h"
#include "journal.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void tuition_refund_trigger_predelete(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time );

void tuition_refund_trigger_insert_update(
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
		tuition_refund_trigger_predelete(
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

		tuition_refund_trigger_insert_update(
			student_full_name,
			street_address,
			course_name,
			season_name,
			year,
			payor_full_name,
			payor_street_address,
			deposit_date_time );

		/* ------------------------------------ */
		/* Even if called from deposit_trigger,	*/
		/* need to set tuition_refund_total.		*/
		/* ------------------------------------ */
		sprintf(sys_string,
	 "deposit_trigger \"%s\" \"%s\" \"%s\" %d \"%s\" tuition_refund",
				entity_escape_full_name( payor_full_name ),
				payor_street_address,
				season_name,
				year,
				deposit_date_time );

		if ( system( sys_string ) ){}

		sprintf(sys_string,
		 "registration_trigger \"%s\" \"%s\" \"%s\" %d tuition_refund",
			entity_escape_full_name( student_full_name ),
			street_address,
			season_name,
			year );

		if ( system( sys_string ) ){}
	}

	return 0;
}

void tuition_refund_trigger_insert_update(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time )
{
	TUITION_REFUND *tuition_refund;

	if ( ! ( tuition_refund =
			tuition_refund_fetch(
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

	if ( !tuition_refund->deposit )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty deposit.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return;
	}

	tuition_refund =
		tuition_refund_steady_state(
			tuition_refund,
			tuition_refund->
				deposit->
				deposit_tuition_refund_list,
			tuition_refund->
				deposit->
				deposit_registration_list,
			tuition_refund->
				enrollment->
				registration->
				enrollment_list,
			semester_offering_list(
				season_name,
				year ),
			tuition_refund->
				deposit->
				deposit_amount,
			tuition_refund->
				deposit->
				transaction_fee,
			0 /* transaction_seconds_to_add */ );

	if ( tuition_refund->transaction_date_time
	&&  *tuition_refund->transaction_date_time
	&&   tuition_refund->tuition_refund_transaction->transaction_amount )
	{
		TRANSACTION *t = tuition_refund->tuition_refund_transaction;

		tuition_refund->transaction_date_time =
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

	tuition_refund_update(
		tuition_refund->tuition_refund_amount,
		tuition_refund->tuition_refund_fees_expense,
		tuition_refund->tuition_refund_overpayment_loss,
		tuition_refund->transaction_date_time,
		student_full_name,
		street_address,
		course_name,
		season_name,
		year,
		payor_full_name,
		payor_street_address,
		deposit_date_time );
}

void tuition_refund_trigger_predelete(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time )
{
	TUITION_REFUND *tuition_refund;

	if ( ! ( tuition_refund =
			tuition_refund_fetch(
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

	if ( tuition_refund->transaction_date_time
	&&   *tuition_refund->transaction_date_time )
	{
		transaction_delete(
			tuition_refund->deposit->payor_entity->full_name,
			tuition_refund->deposit->payor_entity->street_address,
			tuition_refund->transaction_date_time );

		journal_account_name_list_propagate(
			tuition_refund->transaction_date_time,
			/* ------------------------- */
			/* Returns account_name_list */
			/* ------------------------- */
			journal_delete(
				tuition_refund->
					deposit->
					payor_entity->
					full_name,
				tuition_refund->
					deposit->
					payor_entity->
					street_address,
				tuition_refund->transaction_date_time ) );
	}
}
