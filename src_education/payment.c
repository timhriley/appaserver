/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/payment.c		*/
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
#include "payment.h"
#include "payment_fns.h"
#include "transaction.h"
#include "registration.h"
#include "registration_fns.h"
#include "journal.h"
#include "offering.h"
#include "offering_fns.h"
#include "enrollment.h"
#include "enrollment_fns.h"
#include "semester.h"
#include "account.h"
#include "student.h"
#include "program.h"
#include "deposit.h"

PAYMENT *payment_calloc( void )
{
	PAYMENT *payment;

	if ( ! ( payment = calloc( 1, sizeof( PAYMENT ) ) ) )
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

PAYMENT *payment_new(	char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time )
{
	PAYMENT *payment = payment_calloc();

	payment->enrollment =
		enrollment_new(
			student_full_name,
			street_address,
			course_name,
			season_name,
			year );

	payment->deposit =
		deposit_new(
			payor_full_name,
			payor_street_address,
			season_name,
			year,
			deposit_date_time );
	return payment;
}

PAYMENT *payment_fetch(	char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			boolean fetch_deposit,
			boolean fetch_enrollment )
{
	PAYMENT *payment;

	payment = payment_parse(
			pipe2string(
				payment_sys_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					payment_primary_where(
						student_full_name,
						street_address,
						course_name,
						season_name,
						year,
						payor_full_name,
						payor_street_address,
						deposit_date_time ) ) ),
			fetch_deposit,
			fetch_enrollment );

	return payment;
}

double payment_amount(	double deposit_amount,
			double registration_invoice_amount_due )
{
	if ( deposit_amount <= registration_invoice_amount_due )
		return deposit_amount;
	else
		return registration_invoice_amount_due;
}

FILE *payment_update_open( void )
{
	char sys_string[ 2048 ];

	sprintf( sys_string,
		 "update_statement table=%s key=%s carrot=y	|"
		 "tee_appaserver_error.sh 			|"
		 "sql						 ",
		 "payment",
		 PAYMENT_PRIMARY_KEY );

	return popen( sys_string, "w" );
}

void payment_update(	double payment_amount,
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
	FILE *update_pipe = payment_update_open();

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

LIST *payment_system_list(
			char *sys_string,
			boolean fetch_deposit,
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
			payment_parse(
				input,
				fetch_deposit,
				fetch_enrollment ) );
	}

	pclose( input_pipe );
	return payment_list;
}

char *payment_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"select.sh '*' %s \"%s\" select",
		"payment",
		where );

	return strdup( sys_string );
}

PAYMENT *payment_parse(
			char *input,
			boolean fetch_deposit,
			boolean fetch_enrollment )
{
	PAYMENT *payment;
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

	if ( !input || !*input ) return (PAYMENT *)0;

	/* See: attribute_list payment */
	/* ---------------------------- */
	piece( student_full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( course_name, SQL_DELIMITER, input, 2 );
	piece( season_name, SQL_DELIMITER, input, 3 );
	piece( year, SQL_DELIMITER, input, 4 );
	piece( payor_full_name, SQL_DELIMITER, input, 5 );
	piece( payor_street_address, SQL_DELIMITER, input, 6 );
	piece( deposit_date_time, SQL_DELIMITER, input, 7 );

	payment =
		payment_new(
			strdup( student_full_name ),
			strdup( street_address ),
			strdup( course_name ),
			strdup( season_name ),
			atoi( year ),
			strdup( payor_full_name ),
			strdup( payor_street_address ),
			strdup( deposit_date_time ) );

	piece( payment_amount, SQL_DELIMITER, input, 8 );
	payment->payment_amount = atof( payment_amount );

	piece( fees_expense, SQL_DELIMITER, input, 9 );
	payment->payment_fees_expense = atof( fees_expense );

	piece( gain_donation, SQL_DELIMITER, input, 10 );
	payment->payment_gain_donation = atof( gain_donation );

	piece( transaction_date_time, SQL_DELIMITER, input, 11 );

	if ( *transaction_date_time )
	{
		if ( ! ( payment->payment_transaction =
				transaction_fetch(
					payment->
						enrollment->
						registration->
						student_full_name,
					payment->
						enrollment->
						registration->
						street_address,
					transaction_date_time ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: transaction_fetch() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}
	}

	if ( fetch_deposit )
	{
		payment->deposit =
			deposit_fetch(
				payor_full_name,
				payor_street_address,
				season_name,
				atoi( year ),
				deposit_date_time,
				1 /* fetch_payment_list */ );
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
				0 /* not fetch_payment_list */,
				1 /* not fetch_offering */,
				1 /* fetch_registration */ );

	}

	return payment;
}

TRANSACTION *payment_transaction(
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			char *program_name,
			double payment_amount,
			double fees_expense,
			double gain_donation,
			double receivable_credit_amount,
			double cash_debit_amount,
			char *account_cash,
			char *account_receivable,
			char *account_fees_expense,
			char *account_gain )
{
	TRANSACTION *transaction;
	JOURNAL *journal;

	if ( dollar_virtually_same( payment_amount, 0.0 ) )
		return (TRANSACTION *)0;

	transaction =
		transaction_full(
			payor_full_name,
			payor_street_address,
			deposit_date_time,
			payment_amount
				/* transaction_amount */,
			PAYMENT_MEMO );

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
				account_cash ) ) );

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
		/* Credit account_receivable */
		/* ------------------------- */
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

double payment_total( LIST *payment_list )
{
	PAYMENT *payment;
	double total;

	if ( !list_rewind( payment_list ) ) return 0.0;

	total = 0.0;

	do {
		payment = list_get( payment_list );

		total += payment->payment_amount;

	} while ( list_next( payment_list ) );

	return total;
}

double payment_gain_donation(
			double deposit_amount,
			LIST *deposit_registration_list )
{
	double remaining;
	double gain_donation;

	if ( ( remaining =
			deposit_remaining(
				deposit_amount,
				registration_tuition(
					deposit_registration_list ) ) ) > 0.0 )
	{
		gain_donation = remaining;
	}
	else
	{
		gain_donation = 0.0;
	}
	return gain_donation;
}

double payment_fees_expense(
			double deposit_transaction_fee,
			LIST *deposit_payment_list )
{
	int length = list_length( deposit_payment_list );

	if ( !length ) return 0.0;

	return	deposit_transaction_fee /
		(double)length;
}

PAYMENT *payment_steady_state(
			DEPOSIT *deposit,
			double deposit_amount,
			double deposit_transaction_fee,
			char *program_name,
			PAYMENT *payment )
{
	if ( !payment->deposit )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty deposit.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !payment->enrollment )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty enrollment.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !payment->enrollment->registration )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty registration.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	/* Set the input parameters */
	/* ------------------------ */
	payment->deposit_amount = deposit_amount;
	payment->deposit_transaction_fee = deposit_transaction_fee;

	/* Do the work */
	/* ----------- */
	payment->
		deposit->
		deposit_payment_list =
			deposit_payment_list(
				payment->
					deposit->
					payor_entity->
					full_name,
				payment->
					deposit->
					payor_entity->
					street_address,
				payment->
					enrollment->
					offering->
					season_name,
				payment->
					enrollment->
					offering->
					year,
				payment->
					deposit->
					deposit_date_time,
				0 /* not fetch_deposit */,
				0 /* not fetch_enrollment */ );

	payment->
		enrollment->
		registration->
		registration_invoice_amount_due =
			registration_invoice_amount_due(
				payment->
					enrollment->
					registration->
					registration_tuition,
				payment->
					enrollment->
					registration->
					registration_payment_total );

	payment->payment_amount =
		payment_amount(
			payment->
				deposit_amount,
			payment->
				enrollment->
				registration->
					registration_invoice_amount_due );

	payment->payment_fees_expense =
		payment_fees_expense(
			payment->deposit_transaction_fee,
			payment->deposit->deposit_payment_list );

	/* Set the payment amount in the list, so the total will add up. */
	/* ------------------------------------------------------------- */
	{
		PAYMENT *p;

		if ( ! ( p = payment_seek(
				payment->
					deposit->
					deposit_payment_list,
				payment->
					deposit->
					deposit_date_time ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: payment_seek() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		p->payment_amount = payment->payment_amount;
	}

	payment->
		deposit->
		deposit_payment_total =
			deposit_payment_total(
				payment->
					deposit->
					deposit_payment_list );

	payment->
		deposit->
		deposit_gain_donation =
			deposit_gain_donation(
				payment->
					deposit_amount,
				payment->
					deposit->
					deposit_payment_total );

	payment->payment_gain_donation =
		payment_gain_donation(
			payment->
				deposit->
				deposit_gain_donation,
			payment->
				deposit->
				deposit_payment_list );

	payment->receivable_credit_amount = payment->payment_amount;

	payment->payment_cash_debit_amount =
		payment_cash_debit_amount(
			payment->payment_amount,
			payment->payment_gain_donation,
			payment->payment_fees_expense );

	payment->payment_transaction =
		payment_transaction(
			deposit->payor_entity->full_name,
			deposit->payor_entity->street_address,
			deposit->deposit_date_time,
			program_name,
			payment->payment_amount,
			payment->payment_fees_expense,
			payment->payment_gain_donation,
			payment->receivable_credit_amount,
			payment->payment_cash_debit_amount,
			account_cash( (char *)0 ),
			account_receivable( (char *)0 ),
			account_fees_expense( (char *)0 ),
			account_gain( (char *)0 ) );

	return payment;
}

char *payment_primary_where(
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

PAYMENT *payment_seek(
			LIST *deposit_payment_list,
			char *deposit_date_time )
{
	PAYMENT *payment;

	if ( !list_rewind( deposit_payment_list ) )
		return (PAYMENT *)0;

	do {
		payment = list_get( deposit_payment_list );

		if ( !payment->deposit )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty deposit.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( strcmp(	payment->deposit->deposit_date_time,
				deposit_date_time ) == 0 )
		{
			return payment;
		}
	} while ( list_next( deposit_payment_list ) );

	return (PAYMENT *)0;
}

double payment_cash_debit_amount(
			double payment_amount,
			double payment_gain_donation,
			double payment_fees_expense )
{
	return	payment_amount +
		payment_gain_donation -
		payment_fees_expense;
}

void payment_list_insert( LIST *payment_list )
{
	PAYMENT *payment;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( payment_list ) ) return;

	insert_pipe =
		payment_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		payment = list_get( payment_list );

		payment_insert_pipe(
			insert_pipe,
			payment->enrollment->registration->student_full_name,
			payment->enrollment->registration->street_address,
			payment->enrollment->offering->course->course_name,
			payment->enrollment->offering->season_name,
			payment->enrollment->offering->year,
			payment->deposit->payor_entity->full_name,
			payment->deposit->payor_entity->street_address,
			payment->deposit->deposit_date_time );

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

FILE *payment_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement table=%s field=\"%s\" delimiter='%c'	|"
		"sql >%s 2>&1						 ",
		PAYMENT_TABLE,
		PAYMENT_INSERT_COLUMNS,
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void payment_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time )
{
	fprintf(insert_pipe,
		"%s^%s^%s^%s^%d^%s^%s^%s\n",
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
		deposit_date_time );
}

void payment_list_enrollment_insert( LIST *payment_list )
{
	PAYMENT *payment;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( payment_list ) ) return;

	insert_pipe =
		enrollment_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		payment = list_get( payment_list );

		enrollment_insert_pipe(
			insert_pipe,
			payment->enrollment->registration->student_full_name,
			payment->enrollment->registration->street_address,
			payment->enrollment->offering->course->course_name,
			payment->enrollment->offering->season_name,
			payment->enrollment->offering->year );

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

void payment_list_registration_insert(
			LIST *payment_list )
{
	PAYMENT *payment;
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
			payment->enrollment->offering->season_name,
			payment->enrollment->offering->year );

	} while ( list_next( payment_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf(sys_string,
			"cat %s						|"
			"queue_top_bottom_lines.e 300			|"
			"html_table.e 'Insert Registration Errors' '' '^'",
			 error_filename );

		if ( system( sys_string ) ){}
	}

	sprintf( sys_string, "rm %s", error_filename );

	if ( system( sys_string ) ){};
}

void payment_list_offering_insert(
			LIST *payment_list )
{
	PAYMENT *payment;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( payment_list ) ) return;

	insert_pipe =
		offering_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		payment = list_get( payment_list );

		offering_insert_pipe(
			insert_pipe,
			payment->enrollment->offering->course->course_name,
			payment->enrollment->offering->season_name,
			payment->enrollment->offering->year );

	} while ( list_next( payment_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf(sys_string,
			"cat %s						|"
			"queue_top_bottom_lines.e 300			|"
			"html_table.e 'Insert Offering Errors' '' '^'	 ",
			 error_filename );

		if ( system( sys_string ) ){}
	}

	sprintf( sys_string, "rm %s", error_filename );

	if ( system( sys_string ) ){};
}

void payment_list_course_insert(
			LIST *payment_list )
{
	PAYMENT *payment;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( payment_list ) ) return;

	insert_pipe =
		course_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		payment = list_get( payment_list );

		course_insert_pipe(
			insert_pipe,
			payment->enrollment->offering->course->course_name,
			payment->enrollment->offering->course->course_price );

	} while ( list_next( payment_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf(sys_string,
			"cat %s						|"
			"queue_top_bottom_lines.e 300			|"
			"html_table.e 'Insert Course Errors' '' '^'	 ",
			 error_filename );

		if ( system( sys_string ) ){}
	}

	sprintf( sys_string, "rm %s", error_filename );

	if ( system( sys_string ) ){};
}

void payment_list_student_insert(
			LIST *payment_list )
{
	PAYMENT *payment;
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

void payment_list_student_entity_insert(
			LIST *payment_list )
{
	PAYMENT *payment;
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
			payment->enrollment->registration->street_address );

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

void payment_list_payor_entity_insert(
			LIST *payment_list )
{
	PAYMENT *payment;
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
			payment->deposit->payor_entity->full_name,
			payment->deposit->payor_entity->street_address );

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

void payment_list_program_insert(
			LIST *payment_list )
{
	PAYMENT *payment;
	COURSE *course;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( payment_list ) ) return;

	insert_pipe =
		program_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		payment = list_get( payment_list );

		course = payment->enrollment->offering->course;

		if ( course->course_price )
		{
			program_insert_pipe(
				insert_pipe,
				course->program->program_name );
		}

	} while ( list_next( payment_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf(sys_string,
			"cat %s						|"
			"queue_top_bottom_lines.e 300			|"
			"html_table.e 'Insert Program Errors' '' '^'	 ",
			 error_filename );

		if ( system( sys_string ) ){}
	}

	sprintf( sys_string, "rm %s", error_filename );

	if ( system( sys_string ) ){};
}

