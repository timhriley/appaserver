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
#include "tuition_payment_fns.h"
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
#include "tuition_payment_item_title.h"
#include "tuition_payment.h"

TUITION_PAYMENT *payment_calloc( void )
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
	TUITION_PAYMENT *payment = payment_calloc();

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
			boolean fetch_deposit )
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
			fetch_deposit,
			fetch_enrollment );

	return payment;
}

double tuition_payment_amount(
			double deposit_amount,
			double registration_invoice_amount_due )
{
	if ( deposit_amount <= registration_invoice_amount_due )
		return deposit_amount;
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
			boolean fetch_enrollment,
			boolean fetch_deposit )
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
				fetch_enrollment,
				fetch_deposit ) );
	}

	pclose( input_pipe );
	return payment_list;
}

char *tuition_payment_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"select.sh '*' %s \"%s\" select",
		"payment",
		where );

	return strdup( sys_string );
}

TUITION_PAYMENT *tuition_payment_parse(
			char *input,
			boolean fetch_deposit,
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

	if ( fetch_deposit )
	{
		payment->deposit =
			deposit_fetch(
				payor_full_name,
				payor_street_address,
				season_name,
				atoi( year ),
				deposit_date_time,
				1 /* fetch_tuition_payment_list */,
				0 /* not fetch_program_payment_list */ );
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
			TUITION_PAYMENT_MEMO );

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
			double deposit_gain_donation,
			LIST *deposit_registration_list )
{
	int length = list_length( deposit_registration_list );

	if ( !length )
		return 0.0;
	else
		return deposit_gain_donation / (double)length;
}

double tuition_payment_fees_expense(
			double deposit_transaction_fee,
			LIST *deposit_tuition_payment_list )
{
	int length = list_length( deposit_tuition_payment_list );

	if ( !length ) return 0.0;

	return	deposit_transaction_fee /
		(double)length;
}

TUITION_PAYMENT *tuition_payment_steady_state(
			TUITION_PAYMENT *tuition_payment,
			LIST *deposit_tuition_payment_list,
			LIST *deposit_registration_list,
			LIST *registration_enrollment_list,
			double deposit_amount,
			double deposit_transaction_fee )
{
	tuition_payment->
		enrollment->
		registration->
		registration_tuition_payment_total =
			registration_tuition_payment_total(
				deposit_registration_list );

	tuition_payment->
		enrollment->
		registration->
		registration_tuition =
			registration_tuition(
				registration_enrollment_list );

	tuition_payment->
		enrollment->
		registration->
		registration_invoice_amount_due =
			registration_invoice_amount_due(
				tuition_payment->
					enrollment->
					registration->
					registration_tuition,
				tuition_payment->
					enrollment->
					registration->
					registration_tuition_payment_total );

	tuition_payment->tuition_payment_amount =
		tuition_payment_amount(
			deposit_amount,
			tuition_payment->
				enrollment->
				registration->
				registration_invoice_amount_due );

	/* Set the payment_amount in the list, so the total will add up. */
	/* ------------------------------------------------------------- */
	{
		TUITION_PAYMENT *p;

		if ( ! ( p = tuition_payment_seek(
				deposit_tuition_payment_list,
				tuition_payment->
					deposit->
					deposit_date_time ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: tuition_payment_seek() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		p->tuition_payment_amount =
			tuition_payment->
				tuition_payment_amount;
	}

	tuition_payment->tuition_payment_fees_expense =
		tuition_payment_fees_expense(
			deposit_transaction_fee,
			deposit_tuition_payment_list );

	tuition_payment->
		tuition_payment_total =
			tuition_payment_total(
				deposit_tuition_payment_list );

	tuition_payment->
		tuition_payment_gain_donation =
			tuition_payment_gain_donation(
				deposit_gain_donation(
					deposit_amount,
					deposit_tuition_payment_total(
					     deposit_tuition_payment_list ) ),
				deposit_registration_list );

	tuition_payment->tuition_payment_cash_debit_amount =
		tuition_payment_cash_debit_amount(
			tuition_payment->tuition_payment_amount,
			tuition_payment->tuition_payment_gain_donation,
			tuition_payment->tuition_payment_fees_expense );

	tuition_payment->tuition_payment_receivable_credit_amount =
		tuition_payment_receivable_credit_amount(
			tuition_payment->tuition_payment_amount );

	if ( !tuition_payment->transaction_date_time
	||   !*tuition_payment->transaction_date_time )
	{
		tuition_payment->transaction_date_time =
			tuition_payment->deposit->deposit_date_time;
	}

	tuition_payment->tuition_payment_transaction =
		tuition_payment_transaction(
			tuition_payment->deposit->payor_entity->full_name,
			tuition_payment->deposit->payor_entity->street_address,
			tuition_payment->transaction_date_time,
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
			account_cash( (char *)0 ),
			account_receivable( (char *)0 ),
			account_fees_expense( (char *)0 ),
			account_gain( (char *)0 ) );

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

TUITION_PAYMENT *payment_seek(
			LIST *deposit_tuition_payment_list,
			char *deposit_date_time )
{
	TUITION_PAYMENT *payment;

	if ( !list_rewind( deposit_tuition_payment_list ) )
		return (TUITION_PAYMENT *)0;

	do {
		payment = list_get( deposit_tuition_payment_list );

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
	} while ( list_next( deposit_tuition_payment_list ) );

	return (TUITION_PAYMENT *)0;
}

double tuition_payment_cash_debit_amount(
			double tuition_payment_amount,
			double tuition_payment_gain_donation,
			double tuition_payment_fees_expense )
{
	return	tuition_payment_amount +
		tuition_payment_gain_donation -
		tuition_payment_fees_expense;
}

void tuition_payment_list_insert( LIST *payment_list )
{
	TUITION_PAYMENT *payment;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( payment_list ) ) return;

	insert_pipe =
		tuition_payment_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		payment = list_get( payment_list );

		tuition_payment_insert_pipe(
			insert_pipe,
			payment->enrollment->registration->student_full_name,
			payment->enrollment->registration->street_address,
			payment->enrollment->offering->course->course_name,
			payment->enrollment->offering->semester->season_name,
			payment->enrollment->offering->semester->year,
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

FILE *tuition_payment_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement t=%s f=\"%s\" replace=n delimiter='%c'|"
		"sql 2>&1						|"
		"cat >%s 						 ",
		TUITION_PAYMENT_TABLE,
		TUITION_PAYMENT_INSERT_COLUMNS,
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

void tuition_payment_list_enrollment_insert( LIST *payment_list )
{
	TUITION_PAYMENT *payment;
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
			payment->enrollment->offering->semester->season_name,
			payment->enrollment->offering->semester->year );

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
			payment->enrollment->offering->semester->season_name,
			payment->enrollment->offering->semester->year,
			payment->deposit->deposit_date_time
				/* registration_date_time */ );

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

char *tuition_payment_list_display( LIST *payment_list )
{
	char display[ 65536 ];
	char *ptr = display;
	TUITION_PAYMENT *payment;

	*ptr = '\0';

	if ( !list_rewind( payment_list ) )
	{
		return "";
	}

	do {
		payment =
			list_get(
				payment_list );

		if ( !list_at_head( payment_list ) )
		{
			ptr += sprintf( ptr, ", " );
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
				payment->
					enrollment->
					offering->
					course->
					course_name );

	} while ( list_next( payment_list ) );

	ptr += sprintf( ptr, "\n" );

	return strdup( display );
}

LIST *tuition_payment_registration_list(
			LIST *deposit_tuition_payment_list )
{
	TUITION_PAYMENT *payment;
	LIST *registration_list;

	if ( !list_rewind( deposit_tuition_payment_list ) ) return (LIST *)0;

	registration_list = list_new();

	do {
		payment =
			list_get(
				deposit_tuition_payment_list );

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

		list_set(
			registration_list,
			payment->enrollment->registration );

	} while ( list_next( deposit_tuition_payment_list ) );

	return registration_list;
}

LIST *tuition_payment_enrollment_list(
			LIST *deposit_tuition_payment_list )
{
	TUITION_PAYMENT *payment;
	LIST *enrollment_list;

	if ( !list_rewind( deposit_tuition_payment_list ) ) return (LIST *)0;

	enrollment_list = list_new();

	do {
		payment =
			list_get(
				deposit_tuition_payment_list );

		if ( !payment->enrollment )
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
			payment->enrollment );

	} while ( list_next( deposit_tuition_payment_list ) );

	return enrollment_list;
}

void tuition_payment_list_trigger(
			LIST *deposit_tuition_payment_list )
{
	TUITION_PAYMENT *tuition_payment;

	if ( !list_rewind( deposit_tuition_payment_list ) ) return;

	do {
		tuition_payment =
			list_get(
				deposit_tuition_payment_list );

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
				deposit->
				payor_entity->
				full_name,
			tuition_payment->
				deposit->
				payor_entity->
				street_address,
			tuition_payment->
				deposit->
				deposit_date_time,
			"insert" /* state */ );

	} while ( list_next( deposit_tuition_payment_list ) );
}

LIST *tuition_payment_list(
			char *season_name,
			int year,
			char *item_title_P,
			DEPOSIT *deposit )
{
	LIST *payment_list = list_new();
	TUITION_PAYMENT *payment;
	int student_number;

	for (	student_number = 1;
		( payment =
			tuition_payment(
				season_name,
				year,
				item_title_P,
				student_number,
				deposit ) );
		student_number++ )
	{
		list_set( payment_list, payment );
	}

	return payment_list;
}

TUITION_PAYMENT *tuition_payment(
			char *season_name,
			int year,
			char *item_title_P,
			int student_number,
			/* -------- */
			/* Set only */
			/* -------- */
			DEPOSIT *deposit )
{
	TUITION_PAYMENT *payment;
	TUITION_PAYMENT_ITEM_TITLE *tuition_payment_item_title;

	if ( ! ( tuition_payment_item_title =
			tuition_payment_item_title_new(
				item_title_P,
				student_number ) ) )
	{
		return (TUITION_PAYMENT *)0;
	}

	if ( ! ( tuition_payment_item_title->
			tuition_payment_item_title_entity =
				tuition_payment_item_title_entity(
				item_title_P,
				student_number ) ) )
	{
		return (TUITION_PAYMENT *)0;
	}

	if ( ! ( tuition_payment_item_title->
			tuition_payment_item_title_course_name =
				tuition_payment_item_title_course_name(
					item_title_P,
					student_number ) ) )
	{
		return (TUITION_PAYMENT *)0;
	}

	/* New payment */
	/* ----------- */
	payment = tuition_payment_calloc();

	/* Build enrollment */
	/* ---------------- */
	payment->enrollment =
		enrollment_new(
			tuition_payment_item_title->
				tuition_payment_item_title_entity->
				full_name,
			tuition_payment_item_title->
				tuition_payment_item_title_entity->
				street_address,
			tuition_payment_item_title->
				tuition_payment_item_title_course_name,
			season_name,
			year );

	/* Build registration */
	/* ------------------ */
	payment->enrollment->registration =
		registration_new(
			tuition_payment_item_title->
				tuition_payment_item_title_entity->
				full_name,
			tuition_payment_item_title->
				tuition_payment_item_title_entity->
				street_address,
			season_name,
			year );

	/* Set deposit */
	/* ----------- */
	payment->deposit = deposit;

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