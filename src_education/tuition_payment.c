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
#include "tuition_payment_fns.h"
#include "registration.h"
#include "registration_fns.h"
#include "journal.h"
#include "education.h"
#include "offering.h"
#include "offering_fns.h"
#include "enrollment.h"
#include "enrollment_fns.h"
#include "semester.h"
#include "account.h"
#include "student.h"
#include "program.h"
#include "paypal_deposit.h"
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
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time )
{
	TUITION_PAYMENT *payment = tuition_payment_calloc();

	payment->enrollment =
		enrollment_new(
			student_full_name,
			street_address,
			course_name,
			season_name,
			year );

	payment->paypal_deposit =
		paypal_deposit_new(
			payor_full_name,
			payor_street_address,
			season_name,
			year,
			deposit_date_time );
	return payment;
}

TUITION_PAYMENT *tuition_payment_fetch(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			boolean fetch_enrollment,
			boolean fetch_paypal )
{
	TUITION_PAYMENT *payment;

	payment = tuition_payment_parse(
			pipe2string(
				tuition_payment_sys_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					tuition_payment_primary_where(
						student_full_name,
						street_address,
						course_name,
						season_name,
						year,
						payor_full_name,
						payor_street_address,
						deposit_date_time ) ) ),
			fetch_paypal,
			fetch_enrollment );

	return payment;
}

double tuition_payment_amount(
			double deposit_amount,
			double registration_invoice_amount_due,
			int paypal_registration_list_length )
{
	double payment_amount;

	payment_amount =
		deposit_amount /
		(double)paypal_registration_list_length;

	if ( payment_amount <= registration_invoice_amount_due )
		return payment_amount;
	else
		return registration_invoice_amount_due;
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
			double payment_amount,
			double fees_expense,
			double gain_donation,
			char *transaction_date_time,
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time )
{
	FILE *update_pipe = tuition_payment_update_open();

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%d^%s^%s^%s^payment_amount^%.2lf\n",
		 student_full_name,
		 street_address,
		 course_name,
		 season_name,
		 year,
		 payor_full_name,
		 payor_street_address,
		 deposit_date_time,
		 payment_amount );

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%d^%s^%s^%s^fees_expense^%.2lf\n",
		 student_full_name,
		 street_address,
		 course_name,
		 season_name,
		 year,
		 payor_full_name,
		 payor_street_address,
		 deposit_date_time,
		 fees_expense );

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%d^%s^%s^%s^gain_donation^%.2lf\n",
		 student_full_name,
		 street_address,
		 course_name,
		 season_name,
		 year,
		 payor_full_name,
		 payor_street_address,
		 deposit_date_time,
		 gain_donation );

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%d^%s^%s^%s^transaction_date_time^%s\n",
		 student_full_name,
		 street_address,
		 course_name,
		 season_name,
		 year,
		 payor_full_name,
		 payor_street_address,
		 deposit_date_time,
		 (transaction_date_time) ? transaction_date_time : "" );

	pclose( update_pipe );
}

LIST *tuition_payment_system_list(
			char *sys_string,
			boolean fetch_paypal,
			boolean fetch_enrollment )
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
				fetch_paypal,
				fetch_enrollment ) );
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

TUITION_PAYMENT *tuition_payment_parse(
			char *input,
			boolean fetch_paypal,
			boolean fetch_enrollment )
{
	TUITION_PAYMENT *payment;
	char student_full_name[ 128 ];
	char street_address[ 128 ];
	char course_name[ 128 ];
	char season_name[ 128 ];
	char year[ 128 ];
	char payor_full_name[ 128 ];
	char payor_street_address[ 128 ];
	char deposit_date_time[ 128 ];
	char payment_amount[ 128 ];
	char fees_expense[ 128 ];
	char gain_donation[ 128 ];
	char transaction_date_time[ 128 ];

	if ( !input || !*input ) return (TUITION_PAYMENT *)0;

	/* See: attribute_list tuition_payment */
	/* ----------------------------------- */
	piece( student_full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( course_name, SQL_DELIMITER, input, 2 );
	piece( season_name, SQL_DELIMITER, input, 3 );
	piece( year, SQL_DELIMITER, input, 4 );
	piece( payor_full_name, SQL_DELIMITER, input, 5 );
	piece( payor_street_address, SQL_DELIMITER, input, 6 );
	piece( deposit_date_time, SQL_DELIMITER, input, 7 );

	payment =
		tuition_payment_new(
			strdup( student_full_name ),
			strdup( street_address ),
			strdup( course_name ),
			strdup( season_name ),
			atoi( year ),
			strdup( payor_full_name ),
			strdup( payor_street_address ),
			strdup( deposit_date_time ) );

	piece( payment_amount, SQL_DELIMITER, input, 8 );
	payment->tuition_payment_amount = atof( payment_amount );

	piece( fees_expense, SQL_DELIMITER, input, 9 );
	payment->tuition_payment_fees_expense = atof( fees_expense );

	piece( gain_donation, SQL_DELIMITER, input, 10 );
	payment->tuition_payment_gain_donation = atof( gain_donation );

	piece( transaction_date_time, SQL_DELIMITER, input, 11 );
	payment->transaction_date_time = strdup( transaction_date_time );

	if ( fetch_paypal )
	{
		payment->paypal_deposit =
			paypal_deposit_fetch(
				payor_full_name,
				payor_street_address,
				season_name,
				atoi( year ),
				deposit_date_time,
				0 /* not fetch_tuition_payment_list */,
				0 /* not fetch_program_payment_list */,
				0 /* not fetch_product_payment_list */,
				0 /* not fetch_tuition_refund_list */ );
	}

	if ( fetch_enrollment )
	{
		payment->enrollment =
			enrollment_fetch(
				student_full_name,
				street_address,
				course_name,
				season_name,
				atoi( year ),
				0 /* not fetch_tuition_payment_list */,
				0 /* not fetch_tuition_refund_list */,
				1 /* fetch_offering */,
				1 /* fetch_registration */ );
	}
	return payment;
}

TRANSACTION *tuition_payment_transaction(
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			char *program_name,
			double payment_amount,
			double fees_expense,
			double gain_donation,
			double receivable_credit_amount,
			double cash_debit_amount,
			char *entity_self_paypal_cash_account_name,
			char *account_receivable,
			char *account_fees_expense,
			char *account_gain,
			int seconds_to_add )
{
	TRANSACTION *transaction;
	JOURNAL *journal;

	if ( dollar_virtually_same( payment_amount, 0.0 ) )
		return (TRANSACTION *)0;

	if ( !deposit_date_time )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty deposit_date_time\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	transaction =
		transaction_full(
			payor_full_name,
			payor_street_address,
			deposit_date_time
				/* transaction_date_time */,
			payment_amount
				/* transaction_amount */,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			strdup( tuition_payment_memo( program_name ) ),
			seconds_to_add );

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
				entity_self_paypal_cash_account_name ) ) );

	journal->debit_amount = cash_debit_amount;

	/* Debit fees_expense */
	/* ------------------ */
	list_set(
		transaction->journal_list,
		( journal =
			journal_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				account_fees_expense ) ) );

	journal->debit_amount = fees_expense;

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

	if ( gain_donation )
	{
		list_set(
			transaction->journal_list,
			( journal =
				journal_new(
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time,
					account_gain ) ) );

		journal->credit_amount = gain_donation;
	}
	return transaction;
}

double tuition_payment_total( LIST *payment_list )
{
	TUITION_PAYMENT *payment;
	double total;

	if ( !list_rewind( payment_list ) ) return 0.0;

	total = 0.0;

	do {
		payment = list_get( payment_list );

		total += payment->tuition_payment_amount;

	} while ( list_next( payment_list ) );

	return total;
}

double tuition_payment_gain_donation(
			double paypal_gain_donation,
			int paypal_registration_list_length )
{
	if ( !paypal_registration_list_length )
		return 0.0;
	else
		return	paypal_gain_donation /
			(double)paypal_registration_list_length;
}

double tuition_payment_fees_expense(
			double paypal_transaction_fee,
			int paypal_tuition_payment_list_length )
{
	if ( !paypal_tuition_payment_list_length ) return 0.0;

	return	paypal_transaction_fee /
		(double)paypal_tuition_payment_list_length;
}

TUITION_PAYMENT *tuition_payment_steady_state(
			int *transaction_seconds_to_add,
			TUITION_PAYMENT *tuition_payment,
			LIST *paypal_tuition_payment_list,
			LIST *paypal_registration_list,
			LIST *registration_enrollment_list,
			LIST *semester_offering_list,
			double deposit_amount,
			double paypal_transaction_fee )
{
	if ( !tuition_payment->enrollment->offering )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: missing offfering.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return tuition_payment;
	}

	tuition_payment->
		enrollment->
		registration->
		tuition_payment_total =
			registration_tuition_payment_total(
				paypal_registration_list );

	tuition_payment->
		enrollment->
		registration->
		tuition =
			registration_tuition(
				registration_enrollment_list,
				semester_offering_list );

	tuition_payment->
		enrollment->
		registration->
		invoice_amount_due =
			registration_invoice_amount_due(
				tuition_payment->
					enrollment->
					registration->
					tuition,
				tuition_payment->
					enrollment->
					registration->
					tuition_payment_total );

	tuition_payment->tuition_payment_amount =
		tuition_payment_amount(
			deposit_amount,
			tuition_payment->
				enrollment->
				registration->
				invoice_amount_due,
			list_length( paypal_registration_list ) );

	tuition_payment->tuition_payment_fees_expense =
		tuition_payment_fees_expense(
			paypal_transaction_fee,
			list_length( paypal_tuition_payment_list ) );

	tuition_payment->
		tuition_payment_total =
			tuition_payment_total(
				paypal_tuition_payment_list );

	tuition_payment->
		tuition_payment_gain_donation =
			tuition_payment_gain_donation(
				paypal_gain_donation(
					deposit_amount,
					paypal_registration_tuition(
						paypal_registration_list,
						semester_offering_list ) ),
				list_length( paypal_registration_list ) );

	tuition_payment->tuition_payment_cash_debit_amount =
		tuition_payment_cash_debit_amount(
			deposit_amount,
		 	tuition_payment->tuition_payment_fees_expense,
			list_length( paypal_registration_list ) );

	tuition_payment->tuition_payment_receivable_credit_amount =
		tuition_payment_receivable_credit_amount(
			tuition_payment->tuition_payment_amount );

	if ( !tuition_payment->transaction_date_time
	||  !*tuition_payment->transaction_date_time )
	{
		tuition_payment->transaction_date_time =
			tuition_payment->paypal_deposit->deposit_date_time;
	}

	if ( ( tuition_payment->tuition_payment_transaction =
		tuition_payment_transaction(
			tuition_payment->
				paypal_deposit->
				payor_entity->
				full_name,
			tuition_payment->
				paypal_deposit->
				payor_entity->
				street_address,
			tuition_payment->
				paypal_deposit->
				deposit_date_time,
			tuition_payment->
				enrollment->
				offering->
				course->
				program->
				program_name,
			tuition_payment->tuition_payment_amount,
			tuition_payment->tuition_payment_fees_expense,
			tuition_payment->tuition_payment_gain_donation,
			tuition_payment->
				tuition_payment_receivable_credit_amount,
			tuition_payment->tuition_payment_cash_debit_amount,
			entity_self_paypal_cash_account_name(),
			account_receivable( (char *)0 ),
			account_fees_expense( (char *)0 ),
			account_gain( (char *)0 ),
			*transaction_seconds_to_add ) ) )
	{
		tuition_payment->transaction_date_time =
			tuition_payment->tuition_payment_transaction->
				transaction_date_time;

		(*transaction_seconds_to_add)++;
	}
	else
	{
		tuition_payment->transaction_date_time = (char *)0;
	}

	return tuition_payment;
}

char *tuition_payment_primary_where(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time )
{
	char static where[ 1024 ];

	sprintf(where,
		"full_name = '%s' and			"
		"street_address = '%s' and		"
		"course_name = '%s' and			"
		"season_name = '%s' and			"
		"year = %d and				"
		"payor_full_name = '%s' and		"
		"payor_street_address = '%s' and	"
		"deposit_date_time = '%s'		",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 registration_escape_full_name( student_full_name ),
		 street_address,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 course_name_escape( course_name ),
		 season_name,
		 year,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( payor_full_name ),
		 payor_street_address,
		 deposit_date_time );

	return where;
}

TUITION_PAYMENT *tuition_payment_seek(
			LIST *paypal_tuition_payment_list,
			char *deposit_date_time )
{
	TUITION_PAYMENT *payment;

	if ( !list_rewind( paypal_tuition_payment_list ) )
		return (TUITION_PAYMENT *)0;

	do {
		payment = list_get( paypal_tuition_payment_list );

		if ( !payment->paypal_deposit )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty paypal_deposit.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( strcmp(	payment->paypal_deposit->deposit_date_time,
				deposit_date_time ) == 0 )
		{
			return payment;
		}
	} while ( list_next( paypal_tuition_payment_list ) );

	return (TUITION_PAYMENT *)0;
}

double tuition_payment_cash_debit_amount(
			double deposit_amount,
			double tuition_payment_fees_expense,
			int paypal_registration_list_length )
{
	return	( deposit_amount /
		  (double)paypal_registration_list_length ) -
		tuition_payment_fees_expense;
}

void tuition_payment_list_insert( LIST *payment_list )
{
	TUITION_PAYMENT *payment;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];
	char *transaction_date_time;

	if ( !list_rewind( payment_list ) ) return;

	insert_pipe =
		tuition_payment_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		payment = list_get( payment_list );

		transaction_date_time =
			(payment->tuition_payment_transaction)
				? payment->
					tuition_payment_transaction->
					transaction_date_time
				: (char *)0;

		tuition_payment_insert_pipe(
			insert_pipe,
			payment->enrollment->registration->student_full_name,
			payment->enrollment->registration->street_address,
			payment->enrollment->offering->course->course_name,
			payment->enrollment->offering->semester->season_name,
			payment->enrollment->offering->semester->year,
			payment->paypal_deposit->payor_entity->full_name,
			payment->paypal_deposit->payor_entity->street_address,
			payment->paypal_deposit->deposit_date_time,
			payment->tuition_payment_amount,
			payment->tuition_payment_fees_expense,
			payment->tuition_payment_gain_donation,
			transaction_date_time );

	} while ( list_next( payment_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf(sys_string,
			"cat %s						|"
			"queue_top_bottom_lines.e 300			|"
			"html_table.e 'Insert Payment Errors' '' '^'",
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
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			double payment_amount,
			double fees_expense,
			double gain_donation,
			char *transaction_date_time )
{
	fprintf(insert_pipe,
		"%s^%s^%s^%s^%d^%s^%s^%s^%.2lf^%.2lf^%.2lf^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		registration_escape_full_name( student_full_name ),
		street_address,
		course_name,
		season_name,
		year,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name( payor_full_name ),
		payor_street_address,
		deposit_date_time,
		payment_amount,
		fees_expense,
		gain_donation,
		(transaction_date_time)
			? transaction_date_time
			: "" );
}

void tuition_payment_list_enrollment_insert( LIST *payment_list )
{
	TUITION_PAYMENT *payment;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];
	char *transaction_date_time;

	if ( !list_rewind( payment_list ) ) return;

	insert_pipe =
		enrollment_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		payment = list_get( payment_list );

		transaction_date_time =
			(payment->enrollment->enrollment_transaction)
				? payment->
					enrollment->
					enrollment_transaction->
					transaction_date_time
				: (char *)0;

		enrollment_insert_pipe(
			insert_pipe,
			payment->enrollment->registration->student_full_name,
			payment->enrollment->registration->street_address,
			payment->enrollment->offering->course->course_name,
			payment->enrollment->offering->semester->season_name,
			payment->enrollment->offering->semester->year,
			transaction_date_time );

	} while ( list_next( payment_list ) );

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
			LIST *payment_list )
{
	TUITION_PAYMENT *payment;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( payment_list ) ) return;

	insert_pipe =
		registration_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		payment = list_get( payment_list );

		registration_insert_pipe(
			insert_pipe,
			payment->enrollment->registration->student_full_name,
			payment->enrollment->registration->street_address,
			payment->enrollment->registration->season_name,
			payment->enrollment->registration->year,
			payment->
				enrollment->
				registration->
				registration_date_time );

	} while ( list_next( payment_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		char *title = "Insert Registration Errors";

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
			LIST *payment_list )
{
	TUITION_PAYMENT *payment;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( payment_list ) ) return;

	insert_pipe =
		student_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		payment = list_get( payment_list );

		student_insert_pipe(
			insert_pipe,
			payment->enrollment->registration->student_full_name,
			payment->enrollment->registration->street_address );

	} while ( list_next( payment_list ) );

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
			payment->enrollment->registration->student_full_name,
			payment->enrollment->registration->street_address,
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
			payment->paypal_deposit->payor_entity->full_name,
			payment->paypal_deposit->payor_entity->street_address,
			payment->paypal_deposit->payor_entity->email_address );

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

char *tuition_payment_list_display( LIST *payment_list )
{
	char display[ 65536 ];
	char *ptr = display;
	TUITION_PAYMENT *payment;
	char *course_name;

	*ptr = '\0';

	if ( !list_rewind( payment_list ) )
	{
		return "";
	}

	ptr += sprintf( ptr, "Tuition payment: " );

	do {
		payment =
			list_get(
				payment_list );

		if ( !list_at_head( payment_list ) )
		{
			ptr += sprintf( ptr, ", " );
		}

		if ( payment->enrollment->offering )
		{
			course_name =
				payment->
					enrollment->
					offering->
					course->
					course_name;
		}
		else
		{
			course_name = "Unknown";
		}

		ptr += sprintf(	ptr,
				"%s will enroll in %s",
				entity_name_display(
					payment->
						enrollment->
						registration->
						student_full_name,
					payment->
						enrollment->
						registration->
						street_address ),
				course_name );

	} while ( list_next( payment_list ) );

	return strdup( display );
}

LIST *tuition_payment_registration_list(
			LIST *paypal_tuition_payment_list )
{
	TUITION_PAYMENT *payment;
	REGISTRATION *registration;
	LIST *registration_list;

	if ( !list_rewind( paypal_tuition_payment_list ) ) return (LIST *)0;

	registration_list = list_new();

	do {
		payment =
			list_get(
				paypal_tuition_payment_list );

		if ( !payment->enrollment )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty enrollment.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		if ( !payment->enrollment->registration )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: empty registration.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		registration = payment->enrollment->registration;

		list_set(
			registration_list,
			registration );

	} while ( list_next( paypal_tuition_payment_list ) );

	return registration_list;
}

LIST *tuition_payment_list_enrollment_list(
			LIST *tuition_payment_list )
{
	return tuition_payment_enrollment_list(
			tuition_payment_list );
}

LIST *tuition_payment_enrollment_list(
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

		if ( !tuition_payment->enrollment )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty enrollment.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		list_set(
			enrollment_list,
			tuition_payment->enrollment );

	} while ( list_next( tuition_payment_list ) );

	return enrollment_list;
}

void tuition_payment_list_trigger(
			LIST *paypal_tuition_payment_list )
{
	TUITION_PAYMENT *tuition_payment;

	if ( !list_rewind( paypal_tuition_payment_list ) ) return;

	do {
		tuition_payment =
			list_get(
				paypal_tuition_payment_list );

		tuition_payment_trigger(
			tuition_payment->
				enrollment->
				registration->
				student_full_name,
			tuition_payment->
				enrollment->
				registration->
				street_address,
			tuition_payment->
				enrollment->
				offering->
				course->
				course_name,
			tuition_payment->
				enrollment->
				offering->
				semester->
				season_name,
			tuition_payment->
				enrollment->
				offering->
				semester->
				year,
			tuition_payment->
				paypal_deposit->
				payor_entity->
				full_name,
			tuition_payment->
				paypal_deposit->
				payor_entity->
				street_address,
			tuition_payment->
				paypal_deposit->
				deposit_date_time,
			"insert" /* state */ );

	} while ( list_next( paypal_tuition_payment_list ) );
}

LIST *tuition_payment_list(
			char *season_name,
			int year,
			LIST *paypal_item_list,
			LIST *semester_offering_list,
			PAYPAL_DEPOSIT *paypal_deposit )
{
	LIST *payment_list;
	TUITION_PAYMENT *payment;
	OFFERING *offering;
	PAYPAL_ITEM *paypal_item;

	if ( !list_rewind( paypal_item_list ) ) return (LIST *)0;

	payment_list = list_new();

	do {

		paypal_item = list_get( paypal_item_list );

		if ( !paypal_item->benefit_entity ) continue;

		if ( ( offering = 
			offering_seek(
				paypal_item->item_data,
				semester_offering_list ) ) )
		{
			payment =
				tuition_payment(
					season_name,
					year,
					paypal_item->benefit_entity,
					paypal_item->item_value,
					paypal_item->item_fee,
					paypal_item->item_gain,
					offering,
					paypal_deposit );

			list_set( payment_list, payment );
		}
	} while ( list_next( paypal_item_list ) );

	return payment_list;
}

TUITION_PAYMENT *tuition_payment(
			char *season_name,
			int year,
			ENTITY *benefit_entity,
			double item_value,
			double item_fee,
			double item_gain,
			OFFERING *offering,
			PAYPAL_DEPOSIT *paypal_deposit )
{
	TUITION_PAYMENT *payment;

	/* New TUTION_PAYMENT */
	/* ------------------ */
	payment = tuition_payment_calloc();

	/* Build enrollment */
	/* ---------------- */
	payment->enrollment =
		enrollment_new(
			benefit_entity->
				full_name,
			benefit_entity->
				street_address,
			offering->course->course_name,
			season_name,
			year );

	payment->enrollment->offering = offering;

	/* Build registration */
	/* ------------------ */
	payment->enrollment->registration =
		registration_new(
			benefit_entity->
				full_name,
			benefit_entity->
				street_address,
			season_name,
			year );

	payment->enrollment->registration->registration_date_time =
		paypal_deposit->deposit_date_time;
	
	payment->
		enrollment->
		registration->
		enrollment_list =
			list_new();

	list_set(
		payment->
			enrollment->
			registration->
			enrollment_list,
		payment->enrollment );

	payment->tuition_payment_amount = item_value;

	payment->tuition_payment_fees_expense = item_fee;

	payment->tuition_payment_total = item_value;

	payment->tuition_payment_gain_donation = item_gain;

	payment->tuition_payment_receivable_credit_amount = item_value;

	payment->tuition_payment_cash_debit_amount =
		item_value - item_fee;

	/* Set paypal_deposit */
	/* ------------------ */
	payment->paypal_deposit = paypal_deposit;

	return payment;
}

double tuition_payment_receivable_credit_amount(
			double payment_amount )
{
	return payment_amount;
}

void tuition_payment_trigger(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			char *state )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
"tuition_payment_trigger \"%s\" '%s' \"%s\" '%s' %d \"%s\" '%s' '%s' '%s'",
		student_full_name,
		street_address,
		course_name,
		season_name,
		year,
		payor_full_name,
		payor_street_address,
		deposit_date_time,
		state );

	if ( system( sys_string ) ){}
}

void tuition_payment_list_enrollment_trigger(
			LIST *tuition_payment_list )
{
	TUITION_PAYMENT *payment;

	if ( !list_rewind( tuition_payment_list ) ) return;

	do {
		payment =
			list_get(
				tuition_payment_list );

		enrollment_trigger(
			payment->
				enrollment->
				registration->
				student_full_name,
			payment->
				enrollment->
				registration->
				street_address,
			payment->
				enrollment->
				offering->
				course->
				course_name,
			payment->
				enrollment->
				offering->
				semester->
				season_name,
			payment->
				enrollment->
				offering->
				semester->
				year );

	} while ( list_next( tuition_payment_list ) );
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

		if ( !tuition_payment->enrollment )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty enrollment.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		if ( tuition_payment->enrollment->enrollment_transaction )
		{
			list_set(
				transaction_list,
				tuition_payment->
					enrollment->
					enrollment_transaction );
		}

		if ( tuition_payment->tuition_payment_transaction )
		{
			list_set(
				transaction_list,
				tuition_payment->
					tuition_payment_transaction );
		}

	} while ( list_next( tuition_payment_list ) );

	return transaction_list;
}

boolean tuition_payment_structure(
			TUITION_PAYMENT *tuition_payment )
{
	if ( !tuition_payment )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty tuition_payment\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return 0;
	}

	if ( !tuition_payment->enrollment )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty enrollment\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return 0;
	}

	if ( !tuition_payment->enrollment->registration )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty registration\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return 0;
	}

	/* Offering can remain null */
	/* ------------------------ */
	return 1;

	if ( !tuition_payment->enrollment->offering )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty offering\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return 0;
	}

	if ( !tuition_payment->enrollment->offering->course )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty course\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return 0;
	}

	return 1;
}

LIST *tuition_payment_list_steady_state(
			int *transaction_seconds_to_add,
			LIST *paypal_tuition_payment_list,
			LIST *paypal_registration_list,
			LIST *semester_offering_list,
			double deposit_amount,
			double transaction_fee )
{
	TUITION_PAYMENT *tuition_payment;
	REGISTRATION *registration;
	OFFERING *offering;
	ENROLLMENT *enrollment;

	if ( !list_rewind( paypal_tuition_payment_list ) ) return (LIST *)0;

	do {
		tuition_payment = list_get( paypal_tuition_payment_list );

		if ( !tuition_payment_structure( tuition_payment ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: tuition_payment_structure() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		/* Separate out the components */
		/* --------------------------- */
		enrollment = tuition_payment->enrollment;
		registration = enrollment->registration;
		offering = enrollment->offering;

		if ( !offering ) continue;

		/* Execute OFFERING steady state */
		/* ----------------------------- */
		if ( ! ( offering =
				offering_steady_state(
					offering,
					offering->
						enrollment_list ) ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: offering_steady_state() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		/* Execute ENROLLMENT steady state */
		/* ------------------------------- */
		if ( ! ( enrollment =
				enrollment_steady_state(
					transaction_seconds_to_add,
					enrollment,
					deposit_amount ) ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: enrollment_steady_state() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		/* Execute REGISTRATION steady state */
		/* --------------------------------- */
		if ( ! ( registration =
				registration_steady_state(
					registration,
					registration->
					     enrollment_list,
					semester_offering_list ) ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: registration_steady_state() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		/* Execute TUITION_DEPOSIT steady state */
		/* ------------------------------------ */
		list_push( paypal_tuition_payment_list );

		tuition_payment =
			tuition_payment_steady_state(
				transaction_seconds_to_add,
				tuition_payment,
				paypal_tuition_payment_list,
				paypal_registration_list,
				registration->enrollment_list,
				semester_offering_list,
				deposit_amount,
				transaction_fee );

		list_pop( paypal_tuition_payment_list );

	} while ( list_next( paypal_tuition_payment_list ) );

	return paypal_tuition_payment_list;
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
	char *gain;

	if ( !list_rewind( tuition_payment_list ) ) return;

	cash_account_name = entity_self_paypal_cash_account_name();
	receivable = account_receivable( (char *)0 );
	fees_expense = account_fees_expense( (char *)0 );
	gain = account_gain( (char *)0 );

	do {
		tuition_payment = list_get( tuition_payment_list );

		tuition_payment_set_transaction(
			transaction_seconds_to_add,
			tuition_payment,
			cash_account_name,
			receivable,
			fees_expense,
			gain );

	} while ( list_next( tuition_payment_list ) );
}

void tuition_payment_set_transaction(
			int *transaction_seconds_to_add,
			TUITION_PAYMENT *tuition_payment,
			char *cash_account_name,
			char *account_receivable,
			char *account_fees_expense,
			char *account_gain )
{
	if ( ( tuition_payment->tuition_payment_transaction =
		tuition_payment_transaction(
			tuition_payment->
				paypal_deposit->
				payor_entity->
				full_name,
			tuition_payment->
				paypal_deposit->
				payor_entity->
				street_address,
			tuition_payment->
				paypal_deposit->
				deposit_date_time,
			tuition_payment->
				enrollment->
				offering->
				course->
				program->
				program_name,
			tuition_payment->tuition_payment_amount,
			tuition_payment->tuition_payment_fees_expense,
			tuition_payment->tuition_payment_gain_donation,
			tuition_payment->
				tuition_payment_receivable_credit_amount,
			tuition_payment->tuition_payment_cash_debit_amount,
			cash_account_name,
			account_receivable,
			account_fees_expense,
			account_gain,
			*transaction_seconds_to_add ) ) )
	{
		tuition_payment->transaction_date_time =
			tuition_payment->tuition_payment_transaction->
				transaction_date_time;

		(*transaction_seconds_to_add)++;
	}
	else
	{
		tuition_payment->transaction_date_time = (char *)0;
	}
}

