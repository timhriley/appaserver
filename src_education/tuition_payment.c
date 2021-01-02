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
#include "registration.h"
#include "journal.h"
#include "education.h"
#include "offering.h"
#include "enrollment.h"
#include "semester.h"
#include "account.h"
#include "student.h"
#include "program.h"
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
			char *course_name,
			char *season_name,
			int year,
			ENTITY *payor_entity,
			char *payment_date_time )
{
	TUITION_PAYMENT *payment = tuition_payment_calloc();

	payment->enrollment =
		/* ---------------------------------- */
		/* Builds enrollment and registration */
		/* ---------------------------------- */
		enrollment_new(
			student_entity,
			course_name,
			season_name,
			year );

	payment->payor_entity = payor_entity;
	payment->payment_date_time = payment_date_time;

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
			char *payment_date_time,
			boolean fetch_enrollment )
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
						payment_date_time ) ) ),
			fetch_enrollment );

	return payment;
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
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time )
{
	FILE *update_pipe = tuition_payment_update_open();

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%d^%s^%s^%s^net_payment_amount^%.2lf\n",
		 student_full_name,
		 street_address,
		 course_name,
		 season_name,
		 year,
		 payor_full_name,
		 payor_street_address,
		 payment_date_time,
		 net_payment_amount );

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%d^%s^%s^%s^transaction_date_time^%s\n",
		 student_full_name,
		 street_address,
		 course_name,
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
	char payment_date_time[ 128 ];
	char payment_amount[ 128 ];
	char net_payment_amount[ 128 ];
	char transaction_date_time[ 128 ];
	char merchant_fees_expense[ 128 ];
	char paypal_date_time[ 128 ];

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
	piece( payment_date_time, SQL_DELIMITER, input, 7 );

	payment =
		tuition_payment_new(
			entity_new(
				strdup( student_full_name ),
				strdup( street_address ) ),
			strdup( course_name ),
			strdup( season_name ),
			atoi( year ),
			entity_new(
				strdup( payor_full_name ),
				strdup( payor_street_address ) ),
			strdup( payment_date_time ) );

	piece( payment_amount, SQL_DELIMITER, input, 8 );
	payment->payment_amount = atof( payment_amount );

	piece( merchant_fees_expense, SQL_DELIMITER, input, 9 );
	payment->merchant_fees_expense = atof( merchant_fees_expense );

	piece( net_payment_amount, SQL_DELIMITER, input, 10 );
	payment->net_payment_amount = atof( net_payment_amount );

	piece( transaction_date_time, SQL_DELIMITER, input, 11 );
	payment->transaction_date_time = strdup( transaction_date_time );

	piece( merchant_fees_expense, SQL_DELIMITER, input, 12 );
	payment->merchant_fees_expense = atof( merchant_fees_expense );

	piece( paypal_date_time, SQL_DELIMITER, input, 13 );
	payment->paypal_date_time = strdup( paypal_date_time );

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
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			char *program_name,
			double payment_amount,
			double merchant_fees_expense,
			double receivable_credit_amount,
			double cash_debit_amount,
			char *entity_self_paypal_cash_account_name,
			char *account_receivable,
			char *account_fees_expense )
{
	TRANSACTION *transaction;
	JOURNAL *journal;

	if ( dollar_virtually_same( payment_amount, 0.0 ) )
		return (TRANSACTION *)0;

	if ( !payment_date_time )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty payment_date_time\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	transaction =
		transaction_full(
			payor_full_name,
			payor_street_address,
			payment_date_time
				/* transaction_date_time */,
			payment_amount
				/* transaction_amount */,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			strdup( tuition_payment_memo( program_name ) ),
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

	journal->debit_amount = merchant_fees_expense;

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

double tuition_payment_total( LIST *payment_list )
{
	TUITION_PAYMENT *payment;
	double total;

	if ( !list_rewind( payment_list ) ) return 0.0;

	total = 0.0;

	do {
		payment = list_get( payment_list );

		total += payment->payment_amount;

	} while ( list_next( payment_list ) );

	return total;
}

TUITION_PAYMENT *tuition_payment_steady_state(
			TUITION_PAYMENT *tuition_payment,
			double payment_amount,
			double merchant_fees_expense )
{
	tuition_payment->net_payment_amount =
		education_net_payment_amount(
			payment_amount,
			merchant_fees_expense );

	tuition_payment->tuition_payment_receivable_credit_amount =
		tuition_payment_receivable_credit_amount(
			tuition_payment->payment_amount );

	tuition_payment->tuition_payment_cash_debit_amount =
		tuition_payment_cash_debit_amount(
			tuition_payment->payment_amount,
			tuition_payment->merchant_fees_expense );

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

double tuition_payment_cash_debit_amount(
			double payment_amount,
			double merchant_fees_expense )
{
	return	payment_amount -
		merchant_fees_expense;
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
				enrollment->
				registration->
				student_entity->
				full_name,
			tuition_payment->
				enrollment->
				registration->
				student_entity->
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
			tuition_payment->payor_entity->full_name,
			tuition_payment->payor_entity->street_address,
			tuition_payment->payment_date_time,
			tuition_payment->payment_amount,
			tuition_payment->net_payment_amount,
			tuition_payment->transaction_date_time,
			tuition_payment->merchant_fees_expense,
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
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			double payment_amount,
			double net_payment_amount,
			char *transaction_date_time,
			double merchant_fees_expense,
			char *paypal_date_time )
{
	fprintf(insert_pipe,
		"%s^%s^%s^%s^%d^%s^%s^%s^%.2lf^%.2lf^%s^%.2lf^%s\n",
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
		payment_date_time,
		payment_amount,
		net_payment_amount,
		(transaction_date_time)
			? transaction_date_time
			: "",
		merchant_fees_expense,
		(paypal_date_time)
			? paypal_date_time
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
			payment->
				enrollment->
				registration->
				student_entity->
				full_name,
			payment->
				enrollment->
				registration->
				student_entity->
				street_address,
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
			payment->
				enrollment->
				registration->
				student_entity->
				full_name,
			payment->
				enrollment->
				registration->
				student_entity->
				street_address,
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
			payment->
				enrollment->
				registration->
				student_entity->
				full_name,
			payment->
				enrollment->
				registration->
				student_entity->
				street_address );

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
			payment->
				enrollment->
				registration->
				student_entity->
				full_name,
			payment->
				enrollment->
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
						student_entity->
						full_name,
					payment->
						enrollment->
						registration->
						student_entity->
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
				student_entity->
				full_name,
			tuition_payment->
				enrollment->
				registration->
				student_entity->
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
				payor_entity->
				full_name,
			tuition_payment->
				payor_entity->
				street_address,
			tuition_payment->
				payment_date_time,
			"insert" /* state */ );

	} while ( list_next( paypal_tuition_payment_list ) );
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
			char *payment_date_time,
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
		payment_date_time,
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
				student_entity->
				full_name,
			payment->
				enrollment->
				registration->
				student_entity->
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
			cash_account_name,
			receivable,
			fees_expense );

	} while ( list_next( tuition_payment_list ) );
}

void tuition_payment_set_transaction(
			int *transaction_seconds_to_add,
			TUITION_PAYMENT *tuition_payment,
			char *cash_account_name,
			char *account_receivable,
			char *account_fees_expense )
{
	if ( ( tuition_payment->tuition_payment_transaction =
		tuition_payment_transaction(
			transaction_seconds_to_add,
			tuition_payment->payor_entity->full_name,
			tuition_payment->payor_entity->street_address,
			tuition_payment->payment_date_time,
			tuition_payment->
				enrollment->
				offering->
				course->
				program->
				program_name,
			tuition_payment->payment_amount,
			tuition_payment->merchant_fees_expense,
			tuition_payment->
				tuition_payment_receivable_credit_amount,
			tuition_payment->tuition_payment_cash_debit_amount,
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

	/* New TUTION_PAYMENT */
	/* ------------------ */
	tuition_payment = tuition_payment_calloc();

	tuition_payment->payor_entity = payor_entity;

	tuition_payment->enrollment =
		/* ---------------------------------- */
		/* Builds enrollment and registration */
		/* ---------------------------------- */
		enrollment_new(
			student_entity,
			offering->course->course_name,
			season_name,
			year );

	tuition_payment->enrollment->offering = offering;

	tuition_payment->enrollment->registration->registration_date_time =
		paypal_date_time;
	
	tuition_payment->
		enrollment->
		registration->
		enrollment_list =
			list_new();

	list_set(
		tuition_payment->
			enrollment->
			registration->
			enrollment_list,
		tuition_payment->enrollment );

	tuition_payment->payment_amount =
	tuition_payment->tuition_payment_receivable_credit_amount = item_value;

	tuition_payment->merchant_fees_expense = item_fee;

	tuition_payment->net_payment_amount =
	tuition_payment->tuition_payment_cash_debit_amount =
		education_net_payment_amount(
			tuition_payment->payment_amount,
			tuition_payment->merchant_fees_expense );

	return tuition_payment;
}

