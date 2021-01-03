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
			char *payment_date_time,
			char *refund_date_time );

void tuition_refund_trigger_insert_update(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			char *refund_date_time );

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
	char *payment_date_time;
	char *refund_date_time;
	char *state;

	/* Exits if fails. */
	/* --------------- */
	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 11 )
	{
		fprintf(stderr,
"Usage: %s student_full_name street_address course_name season_name year payor_full_name payor_street_address payment_date_time refund_date_time state\n",
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
	payment_date_time = argv[ 8 ];
	refund_date_time = argv[ 9 ];
	state = argv[ 10 ];

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
			payment_date_time,
			refund_date_time );
	}

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0
	||   strcmp( state, "deposit" ) ==  0 )
	{

		tuition_refund_trigger_insert_update(
			student_full_name,
			street_address,
			course_name,
			season_name,
			year,
			payor_full_name,
			payor_street_address,
			payment_date_time,
			refund_date_time );

#ifdef NOT_DEFINED
		char sys_string[ 1024 ];
		/* ------------------------------------ */
		/* Even if called from deposit_trigger,	*/
		/* need to set tuition_refund_total.	*/
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
#endif
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
			char *payment_date_time,
			char *refund_date_time )
{
	TUITION_REFUND *tuition_refund;
	int transaction_seconds_to_add = 0;

	if ( ! ( tuition_refund =
			tuition_refund_fetch(
				student_full_name,
				street_address,
				course_name,
				season_name,
				year,
				payor_full_name,
				payor_street_address,
				payment_date_time,
				refund_date_time,
				1 /* fetch_payment */ ) ) )
	{
		return;
	}

	tuition_refund =
		tuition_refund_steady_state(
			tuition_refund,
			tuition_refund->refund_amount,
			tuition_refund->merchant_fees_expense );

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
				refund_date_time,
			tuition_refund->
				tuition_payment->
				enrollment->
				offering->
				course->
				course_name,
			tuition_refund->
				tuition_payment->
				enrollment->
				offering->
				course->
				program->
				program_name,
			tuition_refund->refund_amount,
			tuition_refund->merchant_fees_expense,
			tuition_refund->net_refund_amount,
			entity_self_paypal_cash_account_name(),
			account_fees_expense( (char *)0 ),
			tuition_refund->
				tuition_payment->
				enrollment->
				offering->
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
				t->program_name,
				t->transaction_amount,
				t->memo,
				0 /* check_number */,
				t->journal_list );
	}

	tuition_refund_update(
		tuition_refund->net_refund_amount,
		tuition_refund->transaction_date_time,
		student_full_name,
		street_address,
		course_name,
		season_name,
		year,
		payor_full_name,
		payor_street_address,
		payment_date_time,
		refund_date_time );
}

void tuition_refund_trigger_predelete(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			char *refund_date_time )
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
				payment_date_time,
				refund_date_time,
				0 /* not fetch_payment */ ) ) )
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
