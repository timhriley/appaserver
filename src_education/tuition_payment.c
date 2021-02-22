/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/tuition_payment.c	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "timlib.h"
#include "sql.h"
#include "boolean.h"
#include "float.h"
#include "list.h"
#include "entity_self.h"
#include "paypal_upload.h"
#include "paypal_item.h"
#include "journal.h"
#include "student.h"
#include "education.h"
#include "semester.h"
#include "account.h"
#include "offering.h"
#include "registration.h"
#include "enrollment.h"
#include "tuition_payment.h"

TUITION_PAYMENT *tuition_payment_calloc( void )
{
	TUITION_PAYMENT *payment;

	if ( ! ( payment = calloc( 1, sizeof( TUITION_PAYMENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return payment;
}

TUITION_PAYMENT *tuition_payment_new(
			ENTITY *student_entity,
			char *season_name,
			int year,
			ENTITY *payor_entity,
			char *payment_date_time )
{
	TUITION_PAYMENT *tuition_payment = tuition_payment_calloc();

	tuition_payment->registration =
		registration_new(
			student_entity,
			season_name,
			year );

	tuition_payment->payor_entity = payor_entity;
	tuition_payment->payment_date_time = payment_date_time;

	return tuition_payment;
}

TUITION_PAYMENT *tuition_payment_fetch(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			boolean fetch_registration,
			boolean fetch_enrollment_list,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_transaction )
{
	return
		tuition_payment_parse(
			pipe2string(
				tuition_payment_sys_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					tuition_payment_primary_where(
						student_full_name,
						street_address,
						season_name,
						year,
						payor_full_name,
						payor_street_address,
						payment_date_time ) ) ),
			fetch_registration,
			fetch_enrollment_list,
			fetch_offering,
			fetch_course,
			fetch_program,
			fetch_transaction );
}

FILE *tuition_payment_update_open( void )
{
	char sys_string[ 2048 ];

	sprintf( sys_string,
		 "update_statement table=%s key=%s carrot=y	|"
		 "tee_appaserver_error.sh 			|"
		 "sql						 ",
		 TUITION_PAYMENT_TABLE,
		 TUITION_PAYMENT_PRIMARY_KEY );

	return popen( sys_string, "w" );
}

void tuition_payment_update(
			double net_payment_amount,
			char *transaction_date_time,
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time )
{
	FILE *update_pipe = tuition_payment_update_open();

	fprintf( update_pipe,
		 "%s^%s^%s^%d^%s^%s^%s^net_payment_amount^%.2lf\n",
		 student_full_name,
		 street_address,
		 season_name,
		 year,
		 payor_full_name,
		 payor_street_address,
		 payment_date_time,
		 net_payment_amount );

	fprintf( update_pipe,
		 "%s^%s^%s^%d^%s^%s^%s^transaction_date_time^%s\n",
		 student_full_name,
		 street_address,
		 season_name,
		 year,
		 payor_full_name,
		 payor_street_address,
		 payment_date_time,
		 (transaction_date_time) ? transaction_date_time : "" );

	pclose( update_pipe );
}

LIST *tuition_payment_system_list(
			char *sys_string,
			boolean fetch_registration,
			boolean fetch_enrollment_list,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_transaction )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *payment_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			payment_list,
			tuition_payment_parse(
				input,
				fetch_registration,
				fetch_enrollment_list,
				fetch_offering,
				fetch_course,
				fetch_program,
				fetch_transaction ) );
	}

	pclose( input_pipe );
	return payment_list;
}

char *tuition_payment_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"select.sh '*' %s \"%s\" select",
		TUITION_PAYMENT_TABLE,
		where );

	return strdup( sys_string );
}

TRANSACTION *tuition_payment_transaction(
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time,
			char *program_name,
			double payment_amount,
			double merchant_fees_expense,
			double receivable_credit_amount,
			double net_payment_amount,
			char *cash_account_name,
			char *account_receivable,
			char *account_fees_expense )
{
	TRANSACTION *transaction;
	JOURNAL *journal;

	if ( dollar_virtually_same( payment_amount, 0.0 ) )
		return (TRANSACTION *)0;

	if ( !transaction_date_time )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty transaction_date_time\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	transaction =
		transaction_full(
			payor_full_name,
			payor_street_address,
			transaction_date_time,
			payment_amount
				/* transaction_amount */,
			tuition_payment_memo( program_name ),
			1 /* lock_transaction */,
			(*seconds_to_add)++ );

	transaction->program_name = program_name;

	if ( !transaction->journal_list )
	{
		transaction->journal_list = list_new();
	}

	/* Debit account_cash */
	/* ------------------- */
	list_set(
		transaction->journal_list,
		( journal =
			journal_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				cash_account_name ) ) );

	journal->debit_amount = net_payment_amount;

	/* Debit fees_expense */
	/* ------------------ */
	list_set(
		transaction->journal_list,
		journal_merchant_fees_expense(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			merchant_fees_expense,
			account_fees_expense ) );

	/* Credit account_receivable */
	/* ------------------------- */
	list_set(
		transaction->journal_list,
		( journal =
			journal_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				account_receivable ) ) );

	journal->credit_amount = receivable_credit_amount;

	return transaction;
}

double tuition_payment_total( LIST *tuition_payment_list )
{
	TUITION_PAYMENT *tuition_payment;
	double total;

	if ( !list_rewind( tuition_payment_list ) ) return 0.0;

	total = 0.0;

	do {
		tuition_payment = list_get( tuition_payment_list );

		total += tuition_payment->payment_amount;

	} while ( list_next( tuition_payment_list ) );

	return total;
}

double tuition_payment_fee_total( LIST *tuition_payment_list )
{
	TUITION_PAYMENT *tuition_payment;
	double fee_total;

	if ( !list_rewind( tuition_payment_list ) ) return 0.0;

	fee_total = 0.0;

	do {
		tuition_payment = list_get( tuition_payment_list );

		fee_total += tuition_payment->merchant_fees_expense;

	} while ( list_next( tuition_payment_list ) );

	return fee_total;
}

TUITION_PAYMENT *tuition_payment_steady_state(
			TUITION_PAYMENT *tuition_payment,
			double payment_amount,
			double merchant_fees_expense )
{
	if ( !tuition_payment->registration )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: registration is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	tuition_payment->net_payment_amount =
		education_net_payment_amount(
			payment_amount,
			merchant_fees_expense );

	tuition_payment->tuition_payment_receivable_credit_amount =
		tuition_payment_receivable_credit_amount(
			tuition_payment->payment_amount );

	return tuition_payment;
}

char *tuition_payment_primary_where(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time )
{
	char static where[ 1024 ];

	sprintf(where,
		"full_name = '%s' and			"
		"street_address = '%s' and		"
		"season_name = '%s' and			"
		"year = %d and				"
		"payor_full_name = '%s' and		"
		"payor_street_address = '%s' and	"
		"payment_date_time = '%s'		",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 registration_escape_full_name( student_full_name ),
		 street_address,
		 season_name,
		 year,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( payor_full_name ),
		 payor_street_address,
		 payment_date_time );

	return where;
}

void tuition_payment_list_insert(
			LIST *tuition_payment_list )
{
	TUITION_PAYMENT *tuition_payment;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( tuition_payment_list ) ) return;

	insert_pipe =
		tuition_payment_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		tuition_payment = list_get( tuition_payment_list );

		tuition_payment_insert_pipe(
			insert_pipe,
			tuition_payment->
				registration->
				student_entity->
				full_name,
			tuition_payment->
				registration->
				student_entity->
				street_address,
			tuition_payment->
				registration->
				season_name,
			tuition_payment->
				registration->
				year,
			tuition_payment->payor_entity->full_name,
			tuition_payment->payor_entity->street_address,
			tuition_payment->payment_date_time,
			tuition_payment->payment_amount,
			tuition_payment->merchant_fees_expense,
			tuition_payment->net_payment_amount,
			tuition_payment->transaction_date_time,
			tuition_payment->paypal_date_time );

	} while ( list_next( tuition_payment_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf(sys_string,
			"cat %s						   |"
			"queue_top_bottom_lines.e 300			   |"
			"html_table.e 'Insert Tuition Payment Errors' '' '^'",
			 error_filename );

		if ( system( sys_string ) ){}
	}

	sprintf( sys_string, "rm %s", error_filename );

	if ( system( sys_string ) ){};
}

FILE *tuition_payment_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement t=%s f=\"%s\" replace=%c delimiter='%c'|"
		"sql 2>&1						 |"
		"cat >%s 						  ",
		TUITION_PAYMENT_TABLE,
		TUITION_PAYMENT_INSERT_COLUMNS,
		'y',
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void tuition_payment_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			double payment_amount,
			double merchant_fees_expense,
			double net_payment_amount,
			char *transaction_date_time,
			char *paypal_date_time )
{
	fprintf(insert_pipe,
		"%s^%s^%s^%d^%s^%s^%s^%.2lf^%.2lf^%.2lf^%s^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		registration_escape_full_name( student_full_name ),
		street_address,
		season_name,
		year,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name( payor_full_name ),
		payor_street_address,
		payment_date_time,
		payment_amount,
		merchant_fees_expense,
		net_payment_amount,
		(transaction_date_time)
			? transaction_date_time
			: "",
		(paypal_date_time)
			? paypal_date_time
			: "" );
}

void tuition_payment_list_enrollment_insert(
		LIST *tuition_payment_list )
{
	TUITION_PAYMENT *tuition_payment;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( tuition_payment_list ) ) return;

	insert_pipe =
		registration_enrollment_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		tuition_payment = list_get( tuition_payment_list );

		if ( !tuition_payment->registration )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty registration.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			pclose( insert_pipe );
			exit( 1 );
		}

		registration_enrollment_list_insert(
			insert_pipe,
			tuition_payment->
				registration->
				enrollment_list );

	} while ( list_next( tuition_payment_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf(sys_string,
			"cat %s						|"
			"queue_top_bottom_lines.e 300			|"
			"html_table.e 'Insert Enrollment Errors' '' '^'	 ",
			 error_filename );

		if ( system( sys_string ) ){}
	}

	sprintf( sys_string, "rm %s", error_filename );

	if ( system( sys_string ) ){};
}

void tuition_payment_list_registration_insert(
			LIST *tuition_payment_list )
{
	TUITION_PAYMENT *tuition_payment;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( tuition_payment_list ) ) return;

	insert_pipe =
		registration_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		tuition_payment = list_get( tuition_payment_list );

		registration_insert_pipe(
			insert_pipe,
			tuition_payment->
				registration->
				student_entity->
				full_name,
			tuition_payment->
				registration->
				student_entity->
				street_address,
			tuition_payment->
				registration->
				season_name,
			tuition_payment->
				registration->
				year,
			tuition_payment->
				registration->
				registration_date_time );

	} while ( list_next( tuition_payment_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		char *title = "Insert Tuition Registration Errors";

		sprintf(sys_string,
			"cat %s						|"
			"queue_top_bottom_lines.e 300			|"
			"html_table.e '%s' '' '^'			 ",
			 error_filename,
			 title );

		if ( system( sys_string ) ){}
	}

	sprintf( sys_string, "rm %s", error_filename );

	if ( system( sys_string ) ){};
}

void tuition_payment_list_student_insert(
			LIST *tuition_payment_list )
{
	TUITION_PAYMENT *tuition_payment;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( tuition_payment_list ) ) return;

	insert_pipe =
		student_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		tuition_payment = list_get( tuition_payment_list );

		student_insert_pipe(
			insert_pipe,
			tuition_payment->
				registration->
				student_entity->
				full_name,
			tuition_payment->
				registration->
				student_entity->
				street_address );

	} while ( list_next( tuition_payment_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf(sys_string,
			"cat %s						|"
			"queue_top_bottom_lines.e 300			|"
			"html_table.e 'Insert Student Errors' '' '^'	 ",
			 error_filename );

		if ( system( sys_string ) ){}
	}

	sprintf( sys_string, "rm %s", error_filename );

	if ( system( sys_string ) ){};
}

void tuition_payment_list_student_entity_insert(
			LIST *payment_list )
{
	TUITION_PAYMENT *payment;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( payment_list ) ) return;

	insert_pipe =
		entity_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		payment = list_get( payment_list );

		entity_insert_pipe(
			insert_pipe,
			payment->
				registration->
				student_entity->
				full_name,
			payment->
				registration->
				student_entity->
				street_address,
			(char *)0 /* email_address */ );

	} while ( list_next( payment_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf(sys_string,
			"cat %s						|"
			"queue_top_bottom_lines.e 300			|"
			"html_table.e 'Insert Entity Errors' '' '^'	 ",
			 error_filename );

		if ( system( sys_string ) ){}
	}

	sprintf( sys_string, "rm %s", error_filename );

	if ( system( sys_string ) ){};
}

void tuition_payment_list_payor_entity_insert(
			LIST *payment_list )
{
	TUITION_PAYMENT *payment;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( payment_list ) ) return;

	insert_pipe =
		entity_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		payment = list_get( payment_list );

		entity_insert_pipe(
			insert_pipe,
			payment->payor_entity->full_name,
			payment->payor_entity->street_address,
			payment->payor_entity->email_address );

	} while ( list_next( payment_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf(sys_string,
			"cat %s						|"
			"queue_top_bottom_lines.e 300			|"
			"html_table.e 'Insert Entity Errors' '' '^'	 ",
			 error_filename );

		if ( system( sys_string ) ){}
	}

	sprintf( sys_string, "rm %s", error_filename );

	if ( system( sys_string ) ){};
}

LIST *tuition_payment_list_enrollment_list(
			LIST *tuition_payment_list )
{
	TUITION_PAYMENT *tuition_payment;
	LIST *enrollment_list;

	if ( !list_rewind( tuition_payment_list ) ) return (LIST *)0;

	enrollment_list = list_new();

	do {
		tuition_payment =
			list_get(
				tuition_payment_list );

		if ( !tuition_payment->registration )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: empty registration.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		if ( !list_length( 
			tuition_payment->
			     registration->
			     enrollment_list ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: empty enrollment_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );

		}

		list_set_list(
			enrollment_list,
			tuition_payment->
			       registration->
			       enrollment_list );

	} while ( list_next( tuition_payment_list ) );

	return enrollment_list;
}

LIST *tuition_payment_enrollment_list(
			TUITION_PAYMENT *tuition_payment )
{
	if ( !tuition_payment->registration )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty registration.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_length( tuition_payment->registration->enrollment_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty enrollment_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return tuition_payment->registration->enrollment_list;
}

double tuition_payment_receivable_credit_amount(
			double payment_amount )
{
	return payment_amount;
}

LIST *tuition_payment_transaction_list(
			LIST *tuition_payment_list )
{
	TUITION_PAYMENT *tuition_payment;
	LIST *transaction_list;

	if ( !list_rewind( tuition_payment_list ) ) return (LIST *)0;

	transaction_list = list_new();

	do {
		tuition_payment =
			list_get(
				tuition_payment_list );

		if ( !tuition_payment->registration )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty registration.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		list_set(
			transaction_list,
			registration_enrollment_seek_transaction(
				tuition_payment->
					registration->
					enrollment_list ) );

		list_set(
			transaction_list,
			tuition_payment->
				tuition_payment_transaction );

	} while ( list_next( tuition_payment_list ) );

	return transaction_list;
}

LIST *tuition_payment_list_steady_state(
			LIST *tuition_payment_list )
{

	TUITION_PAYMENT *tuition_payment;

	if ( !list_rewind( tuition_payment_list ) ) return (LIST *)0;

	do {
		tuition_payment = list_get( tuition_payment_list );

		tuition_payment =
			tuition_payment_steady_state(
				tuition_payment,
				tuition_payment->payment_amount,
				tuition_payment->merchant_fees_expense );

	} while ( list_next( tuition_payment_list ) );

	return tuition_payment_list;
}

char *tuition_payment_memo( char *program_name )
{
	static char payment_memo[ 128 ];

	if ( program_name && *program_name )
	{
		sprintf(payment_memo,
			"%s/%s",
			TUITION_PAYMENT_MEMO,
			program_name );
	}
	else
	{
		sprintf(payment_memo,
			"%s Payment",
			TUITION_PAYMENT_MEMO );
	}
	return payment_memo;
}

boolean tuition_payment_is_tuition(
			char *item_title_block )
{
	if ( instr(	"Child: " /* substr */,
			item_title_block /* string */,
			1 /* occurrence */ ) >= 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void tuition_payment_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *tuition_payment_list )
{
	TUITION_PAYMENT *tuition_payment;
	char *cash_account_name;
	char *receivable;
	char *fees_expense;

	if ( !list_rewind( tuition_payment_list ) ) return;

	cash_account_name = entity_self_paypal_cash_account_name();
	receivable = account_receivable( (char *)0 );
	fees_expense = account_fees_expense( (char *)0 );

	do {
		tuition_payment = list_get( tuition_payment_list );

		/* ----------------------------------------------------- */
		/* Sets tuition_payment->tuition_payment_transaction and
			tuition_payment->transaction_date_time		 */
		/* ----------------------------------------------------- */
		tuition_payment_set_transaction(
			transaction_seconds_to_add,
			tuition_payment,
			enrollment_list_first_program_name(
				tuition_payment->enrollment_list ) ),
			cash_account_name,
			receivable,
			fees_expense );

	} while ( list_next( tuition_payment_list ) );
}

void tuition_payment_set_transaction(
			int *transaction_seconds_to_add,
			TUITION_PAYMENT *tuition_payment,
			char *program_name,
			char *cash_account_name,
			char *account_receivable,
			char *account_fees_expense )
{
	OFFERING *offering;

	if ( ! ( offering =
			offering_seek(
				tuition_payment->course_name,
				semester_offering_list ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: offering_seek(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			tuition_payment->course_name );
		exit( 1 );
	}

	if ( ( tuition_payment->tuition_payment_transaction =
		tuition_payment_transaction(
			transaction_seconds_to_add,
			tuition_payment->payor_entity->full_name,
			tuition_payment->payor_entity->street_address,
			tuition_payment->payment_date_time,
			offering->
				course->
				program->
				program_name,
			tuition_payment->payment_amount,
			tuition_payment->merchant_fees_expense,
			tuition_payment->
				tuition_payment_receivable_credit_amount,
			tuition_payment->net_payment_amount,
			cash_account_name,
			account_receivable,
			account_fees_expense ) ) )
	{
		tuition_payment->transaction_date_time =
			tuition_payment->tuition_payment_transaction->
				transaction_date_time;
	}
	else
	{
		tuition_payment->transaction_date_time = (char *)0;
	}
}

LIST *tuition_payment_list_paypal(
			char *season_name,
			int year,
			ENTITY *payor_entity,
			char *paypal_date_time,
			LIST *paypal_item_list,
			LIST *semester_offering_list )
{
	LIST *tuition_payment_list = {0};
	PAYPAL_ITEM *paypal_item;
	OFFERING *offering;

	if ( !list_rewind( paypal_item_list ) ) return (LIST *)0;

	do {
		paypal_item = list_get( paypal_item_list );

		if ( paypal_item->taken ) continue;

		if ( !paypal_item->benefit_entity ) continue;

		if ( ( offering =
			offering_seek( 
				paypal_item->item_data,
				semester_offering_list ) ) )
		{
			if ( !tuition_payment_list )
				tuition_payment_list =
					list_new();

			list_set(
				tuition_payment_list,
				tuition_payment_paypal(
					season_name,
					year,
					paypal_item->benefit_entity,
					payor_entity,
					paypal_date_time,
					paypal_item->item_value,
					paypal_item->item_fee,
					offering ) );

			paypal_item->taken = 1;
		}

	} while ( list_next( paypal_item_list ) );

	return tuition_payment_list;
}

TUITION_PAYMENT *tuition_payment_paypal(
			char *season_name,
			int year,
			ENTITY *student_entity,
			ENTITY *payor_entity,
			char *paypal_date_time,
			double item_value,
			double item_fee,
			OFFERING *offering )
{
	TUITION_PAYMENT *tuition_payment;
	ENROLLMENT *enrollment;

	if ( !student_entity || !payor_entity )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: missing student_entity or payor_entity.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	tuition_payment = tuition_payment_calloc();

	tuition_payment->payor_entity = payor_entity;

	tuition_payment->payment_date_time =
	tuition_payment->paypal_date_time = paypal_date_time;

	tuition_payment->registration =
		registration_new(
			student_entity,
			season_name,
			year );

	tuition_payment->registration->registration_date_time =
		tuition_payment->payment_date_time;

	enrollment =
		enrollment_new(
			student_entity,
			offering->course->course_name,
			season_name,
			year,
			tuition_payment->registration,
			offering );

	tuition_payment->registration->enrollment_list =
		list_new();

	list_set( 
		tuition_payment->registration->enrollment_list,
		enrollment );

	tuition_payment->registration->registration_date_time =
		paypal_date_time;

	tuition_payment->course_name = offering->course->course_name;

	tuition_payment->payment_amount =
	tuition_payment->tuition_payment_receivable_credit_amount = item_value;

	tuition_payment->merchant_fees_expense = item_fee;

	tuition_payment->net_payment_amount =
		education_net_payment_amount(
			tuition_payment->payment_amount,
			tuition_payment->merchant_fees_expense );

	return tuition_payment;
}

TUITION_PAYMENT *tuition_payment_integrity_fetch(
			char *student_full_name,
			char *student_street_address,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address )
{
	return
		tuition_payment_parse(
			pipe2string(
				tuition_payment_sys_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					tuition_payment_integrity_where(
						student_full_name,
						student_street_address,
						season_name,
						year,
						payor_full_name,
						payor_street_address ) ) ),
			0 /* not fetch_registration */,
			0 /* not fetch_enrollment_list */,
			0 /* not fetch_offering */,
			0 /* not fetch_course */,
			0 /* not fetch_program */,
			0 /* not fetch_transaction */ );
}

char *tuition_payment_integrity_where(
			char *student_full_name,
			char *street_address,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address )
{
	char static where[ 1024 ];

	sprintf(where,
		"full_name = '%s' and			"
		"street_address = '%s' and		"
		"season_name = '%s' and			"
		"year = %d and				"
		"payor_full_name = '%s' and		"
		"payor_street_address = '%s'		",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 registration_escape_full_name( student_full_name ),
		 street_address,
		 season_name,
		 year,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( payor_full_name ),
		 payor_street_address );

	return where;
}

TUITION_PAYMENT *tuition_payment_parse(
			char *input,
			boolean fetch_registration,
			boolean fetch_enrollment_list,
			boolean fetch_offering,
			boolean fetch_course,
			boolean fetch_program,
			boolean fetch_transaction )
{
	char student_full_name[ 128 ];
	char student_street_address[ 128 ];
	char season_name[ 128 ];
	char year[ 128 ];
	char payor_full_name[ 128 ];
	char payor_street_address[ 128 ];
	char payment_date_time[ 128 ];
	char payment_amount[ 128 ];
	char merchant_fees_expense[ 128 ];
	char net_payment_amount[ 128 ];
	char transaction_date_time[ 128 ];
	char paypal_date_time[ 128 ];
	TUITION_PAYMENT *tuition_payment;

	if ( !input || !*input ) return (TUITION_PAYMENT *)0;

	tuition_payment = tuition_payment_calloc();

	/* See: attribute_list tuition_payment */
	/* ----------------------------------- */
	piece( student_full_name, SQL_DELIMITER, input, 0 );
	piece( student_street_address, SQL_DELIMITER, input, 1 );
	piece( season_name, SQL_DELIMITER, input, 2 );
	piece( year, SQL_DELIMITER, input, 3 );

	tuition_payment->registration =
		registration_new(
			entity_new(
				strdup( student_full_name ),
				strdup( student_street_address ) ),
			strdup( season_name ),
			atoi( year ) );

	piece( payor_full_name, SQL_DELIMITER, input, 4 );
	piece( payor_street_address, SQL_DELIMITER, input, 5 );

	tuition_payment->payor_entity =
		entity_new(
			strdup( payor_full_name ),
			strdup( payor_street_address ) ),

	piece( payment_date_time, SQL_DELIMITER, input, 6 );
	tuition_payment->payment_date_time = strdup( payment_date_time );

	piece( payment_amount, SQL_DELIMITER, input, 7 );
	tuition_payment->payment_amount = atof( payment_amount );

	piece( merchant_fees_expense, SQL_DELIMITER, input, 8 );
	tuition_payment->merchant_fees_expense = atof( merchant_fees_expense );

	piece( net_payment_amount, SQL_DELIMITER, input, 9 );
	tuition_payment->net_payment_amount = atof( net_payment_amount );

	piece( transaction_date_time, SQL_DELIMITER, input, 10 );
	tuition_payment->transaction_date_time =
		strdup( transaction_date_time );

	piece( paypal_date_time, SQL_DELIMITER, input, 11 );
	tuition_payment->paypal_date_time = strdup( paypal_date_time );

	if ( fetch_registration )
	{
		tuition_payment->registration =
			registration_fetch(
				student_full_name,
				student_street_address,
				tuition_payment->
					registration->
					season_name,
				tuition_payment->
					registration->
					year,
				fetch_enrollment_list,
				fetch_offering,
				fetch_course,
				fetch_program,
				0 /* not fetch_tuition_payment_list */,
				0 /* not fetch_tuition_refund_list */ );
	}

	if ( fetch_transaction && *tuition_payment->transaction_date_time )
	{
		tuition_payment->tuition_payment_transaction =
			transaction_fetch(
				tuition_payment->payor_entity->full_name,
				tuition_payment->payor_entity->street_address,
				tuition_payment->transaction_date_time );
	}

	return tuition_payment;
}

LIST *tuition_payment_registration_list(
			LIST *tuition_payment_list )
{
	TUITION_PAYMENT *tuition_payment;
	LIST *registration_list;

	if ( !list_rewind( tuition_payment_list ) ) return (LIST *)0;

	registration_list = list_new();

	do {
		tuition_payment =
			list_get(
				tuition_payment_list );

		if ( !tuition_payment->registration )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: empty registration.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		list_set(
			registration_list,
			tuition_payment->registration );

	} while ( list_next( tuition_payment_list ) );

	return registration_list;
}

LIST *tuition_payment_list( char *where )
{
	return tuition_payment_system_list(
		tuition_payment_sys_string( where ),
		1 /* fetch_registration */,
		0 /* not fetch_enrollment_list */,
		0 /* not fetch_offering */,
		0 /* not fetch_course */,
		0 /* not fetch_program */,
		0 /* not fetch_transaction */ );
}

TUITION_PAYMENT *tuition_payment_seek(
			char *student_full_name,
			char *student_street_address,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *payment_date_time,
			LIST *tuition_payment_list )
{
	TUITION_PAYMENT *tuition_payment;

	if ( !list_rewind( tuition_payment_list ) )
		return (TUITION_PAYMENT *)0;

	do {
		tuition_payment = list_get( tuition_payment_list );

		if ( !tuition_payment->registration )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty registration.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !tuition_payment->payor_entity )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty payor_entity.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( string_strcmp(
			tuition_payment->
				registration->
				student_entity->
				full_name,
			student_full_name ) == 0
		&&   string_strcmp(
			tuition_payment->
				registration->
				student_entity->
				street_address,
			student_street_address ) == 0
		&&   string_strcmp(
			tuition_payment->payor_entity->full_name,
			payor_full_name ) == 0
		&&   string_strcmp(
			tuition_payment->payor_entity->street_address,
			payor_street_address ) == 0
		&&   string_strcmp(
			tuition_payment->registration->season_name,
			season_name ) == 0
		&&   tuition_payment->registration->year == year
		&&   string_strcmp(
			tuition_payment->payment_date_time,
			payment_date_time ) == 0 )
		{
			return tuition_payment;
		}
	} while ( list_next( tuition_payment_list ) );

	return (TUITION_PAYMENT *)0;
}

boolean tuition_payment_list_exists(
			LIST *tuition_payment_list,
			LIST *existing_tuition_payment_list )
{
	TUITION_PAYMENT *tuition_payment;

	if ( !list_rewind( tuition_payment_list ) ) return 0;

	do {
		tuition_payment = list_get( tuition_payment_list );

		if ( !tuition_payment->registration )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty registration.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !tuition_payment->payor_entity )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty payor_entity.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( tuition_payment_seek(
			tuition_payment->
				registration->
				student_entity->
				full_name,
			tuition_payment->
				registration->
				student_entity->
				street_address,
			tuition_payment->payor_entity->full_name,
			tuition_payment->payor_entity->street_address,
			tuition_payment->registration->season_name,
			tuition_payment->registration->year,
			tuition_payment->payment_date_time,
			existing_tuition_payment_list ) )
		{
			return 1;
		}
	} while ( list_next( tuition_payment_list ) );
	return 0;
}

