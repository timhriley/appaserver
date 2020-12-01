/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/tuition_refund.c	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "column.h"
#include "timlib.h"
#include "sql.h"
#include "boolean.h"
#include "float.h"
#include "list.h"
#include "entity_self.h"
#include "paypal_upload.h"
#include "tuition_refund_fns.h"
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
#include "deposit.h"
#include "tuition_payment_item_title.h"
#include "tuition_refund.h"

TUITION_REFUND *tuition_refund_calloc( void )
{
	TUITION_REFUND *refund;

	if ( ! ( refund = calloc( 1, sizeof( TUITION_REFUND ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return refund;
}

TUITION_REFUND *tuition_refund_new(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time )
{
	TUITION_REFUND *refund = tuition_refund_calloc();

	refund->enrollment =
		enrollment_new(
			student_full_name,
			street_address,
			course_name,
			season_name,
			year );

	refund->deposit =
		deposit_new(
			payor_full_name,
			payor_street_address,
			season_name,
			year,
			deposit_date_time );
	return refund;
}

TUITION_REFUND *tuition_refund_fetch(
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
	TUITION_REFUND *refund;

	refund = tuition_refund_parse(
			pipe2string(
				tuition_refund_sys_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					tuition_refund_primary_where(
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

	return refund;
}

double tuition_refund_amount(
			double deposit_amount,
			double registration_tuition,
			int deposit_registration_list_length )
{
	double refund_amount;

	refund_amount =
		deposit_amount /
		(double)deposit_registration_list_length;

	if ( refund_amount <= registration_tuition )
		return refund_amount;
	else
		return registration_tuition;
}

FILE *tuition_refund_update_open( void )
{
	char sys_string[ 2048 ];

	sprintf( sys_string,
		 "update_statement table=%s key=%s carrot=y	|"
		 "tee_appaserver_error.sh 			|"
		 "sql						 ",
		 TUITION_REFUND_TABLE,
		 TUITION_REFUND_PRIMARY_KEY );

	return popen( sys_string, "w" );
}

void tuition_refund_update(
			double refund_amount,
			double fees_expense,
			double overpayment_loss,
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
	FILE *update_pipe = tuition_refund_update_open();

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%d^%s^%s^%s^refund_amount^%.2lf\n",
		 student_full_name,
		 street_address,
		 course_name,
		 season_name,
		 year,
		 payor_full_name,
		 payor_street_address,
		 deposit_date_time,
		 refund_amount );

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
		 "%s^%s^%s^%s^%d^%s^%s^%s^overpayment_loss^%.2lf\n",
		 student_full_name,
		 street_address,
		 course_name,
		 season_name,
		 year,
		 payor_full_name,
		 payor_street_address,
		 deposit_date_time,
		 overpayment_loss );

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

LIST *tuition_refund_system_list(
			char *sys_string,
			boolean fetch_deposit,
			boolean fetch_enrollment )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *refund_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			refund_list,
			tuition_refund_parse(
				input,
				fetch_deposit,
				fetch_enrollment ) );
	}

	pclose( input_pipe );
	return refund_list;
}

char *tuition_refund_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"select.sh '*' %s \"%s\" select",
		TUITION_REFUND_TABLE,
		where );

	return strdup( sys_string );
}

TUITION_REFUND *tuition_refund_parse(
			char *input,
			boolean fetch_deposit,
			boolean fetch_enrollment )
{
	TUITION_REFUND *refund;
	char student_full_name[ 128 ];
	char street_address[ 128 ];
	char course_name[ 128 ];
	char season_name[ 128 ];
	char year[ 128 ];
	char payor_full_name[ 128 ];
	char payor_street_address[ 128 ];
	char deposit_date_time[ 128 ];
	char refund_amount[ 128 ];
	char fees_expense[ 128 ];
	char overpayment_loss[ 128 ];
	char transaction_date_time[ 128 ];

	if ( !input || !*input ) return (TUITION_REFUND *)0;

	/* See: attribute_list tuition_refund */
	/* ----------------------------------- */
	piece( student_full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( course_name, SQL_DELIMITER, input, 2 );
	piece( season_name, SQL_DELIMITER, input, 3 );
	piece( year, SQL_DELIMITER, input, 4 );
	piece( payor_full_name, SQL_DELIMITER, input, 5 );
	piece( payor_street_address, SQL_DELIMITER, input, 6 );
	piece( deposit_date_time, SQL_DELIMITER, input, 7 );

	refund =
		tuition_refund_new(
			strdup( student_full_name ),
			strdup( street_address ),
			strdup( course_name ),
			strdup( season_name ),
			atoi( year ),
			strdup( payor_full_name ),
			strdup( payor_street_address ),
			strdup( deposit_date_time ) );

	piece( refund_amount, SQL_DELIMITER, input, 8 );
	refund->tuition_refund_amount = atof( refund_amount );

	piece( fees_expense, SQL_DELIMITER, input, 9 );
	refund->tuition_refund_fees_expense = atof( fees_expense );

	piece( overpayment_loss, SQL_DELIMITER, input, 10 );
	refund->tuition_refund_overpayment_loss = atof( overpayment_loss );

	piece( transaction_date_time, SQL_DELIMITER, input, 11 );
	refund->transaction_date_time = strdup( transaction_date_time );

	if ( fetch_deposit )
	{
		refund->deposit =
			deposit_fetch(
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
		refund->enrollment =
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
	return refund;
}

TRANSACTION *tuition_refund_transaction(
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			char *program_name,
			double refund_amount,
			double fees_expense,
			double overpayment_loss,
			double revenue_debit_amount,
			double cash_credit_amount,
			char *entity_self_paypal_cash_account_name,
			char *account_revenue,
			char *account_fees_expense,
			char *account_loss,
			int seconds_to_add )
{
	TRANSACTION *transaction;
	JOURNAL *journal;
	DATE *transaction_date;

	if ( dollar_virtually_same( refund_amount, 0.0 ) )
		return (TRANSACTION *)0;

	transaction_date =
		date_yyyy_mm_dd_hms_new(
			deposit_date_time );

	date_add_seconds( transaction_date, seconds_to_add );

	transaction =
		transaction_full(
			payor_full_name,
			payor_street_address,
			date_display_19( transaction_date ),
			refund_amount
				/* transaction_amount */,
			tuition_refund_memo( program_name ) );

	transaction->program_name = program_name;

	if ( !transaction->journal_list )
	{
		transaction->journal_list = list_new();
	}

	/* Debit account_revenue */
	/* --------------------- */
	list_set(
		transaction->journal_list,
		( journal =
			journal_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				account_revenue ) ) );

	journal->debit_amount = revenue_debit_amount;

	/* Credit account_cash */
	/* -------------------- */
	list_set(
		transaction->journal_list,
		( journal =
			journal_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				entity_self_paypal_cash_account_name ) ) );

	journal->credit_amount = cash_credit_amount;

	/* Credit fees_expense */
	/* ------------------- */
	list_set(
		transaction->journal_list,
		( journal =
			journal_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				account_fees_expense ) ) );

	journal->credit_amount = fees_expense;

	if ( overpayment_loss )
	{
		list_set(
			transaction->journal_list,
			( journal =
				journal_new(
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time,
					account_loss ) ) );

		journal->debit_amount = overpayment_loss;
	}
	return transaction;
}

double tuition_refund_total( LIST *refund_list )
{
	TUITION_REFUND *refund;
	double total;

	if ( !list_rewind( refund_list ) ) return 0.0;

	total = 0.0;

	do {
		refund = list_get( refund_list );

		total += refund->tuition_refund_amount;

	} while ( list_next( refund_list ) );

	return total;
}

double tuition_refund_overpayment_loss(
			double deposit_overpayment_loss,
			LIST *deposit_registration_list )
{
	int length = list_length( deposit_registration_list );

	if ( !length )
		return 0.0;
	else
		return deposit_overpayment_loss / (double)length;
}

double tuition_refund_fees_expense(
			double deposit_transaction_fee,
			int deposit_tuition_refund_list_length )
{
	if ( !deposit_tuition_refund_list_length ) return 0.0;

	return	deposit_transaction_fee /
		(double)deposit_tuition_refund_list_length;
}

TUITION_REFUND *tuition_refund_steady_state(
			TUITION_REFUND *tuition_refund,
			LIST *deposit_tuition_refund_list,
			LIST *deposit_registration_list,
			LIST *registration_enrollment_list,
			LIST *semester_offering_list,
			double deposit_amount,
			double deposit_transaction_fee,
			int transaction_seconds_to_add )
{
	if ( !tuition_refund->enrollment->offering ) return tuition_refund;

	tuition_refund->
		enrollment->
		registration->
		tuition_refund_total =
			registration_tuition_refund_total(
				deposit_registration_list );

	tuition_refund->
		enrollment->
		registration->
		tuition =
			registration_tuition(
				registration_enrollment_list,
				semester_offering_list );

	tuition_refund->
		enrollment->
		registration->
		invoice_amount_due =
			registration_invoice_amount_due(
				tuition_refund->
					enrollment->
					registration->
					tuition,
				tuition_refund->
					enrollment->
					registration->
					tuition_refund_total );

	tuition_refund->tuition_refund_amount =
		tuition_refund_amount(
			float_abs( deposit_amount ),
			tuition_refund->
				enrollment->
				registration->
				tuition,
			list_length( deposit_registration_list ) );

	tuition_refund->tuition_refund_fees_expense =
		tuition_refund_fees_expense(
			deposit_transaction_fee,
			list_length( deposit_tuition_refund_list ) );

	tuition_refund->
		tuition_refund_total =
			tuition_refund_total(
				deposit_tuition_refund_list );

	tuition_refund->
		tuition_refund_overpayment_loss =
			tuition_refund_overpayment_loss(
				deposit_overpayment_loss(
					deposit_amount,
					deposit_registration_tuition(
						deposit_registration_list,
						semester_offering_list ) ),
				deposit_registration_list );

	tuition_refund->tuition_refund_cash_credit_amount =
		tuition_refund_cash_credit_amount(
			deposit_amount,
		 	tuition_refund->tuition_refund_fees_expense,
			list_length( deposit_registration_list ) );

	tuition_refund->tuition_refund_revenue_debit_amount =
		tuition_refund_revenue_debit_amount(
			tuition_refund->tuition_refund_amount );

	if ( !tuition_refund->transaction_date_time
	||  !*tuition_refund->transaction_date_time )
	{
		tuition_refund->transaction_date_time =
			tuition_refund->deposit->deposit_date_time;
	}

	if ( ( tuition_refund->tuition_refund_transaction =
		tuition_refund_transaction(
			tuition_refund->deposit->payor_entity->full_name,
			tuition_refund->deposit->payor_entity->street_address,
			tuition_refund->transaction_date_time,
			tuition_refund->
				enrollment->
				offering->
				course->
				program->
				program_name,
			tuition_refund->tuition_refund_amount,
			tuition_refund->tuition_refund_fees_expense,
			tuition_refund->tuition_refund_overpayment_loss,
			tuition_refund->
				tuition_refund_revenue_debit_amount,
			tuition_refund->tuition_refund_cash_credit_amount,
			entity_self_paypal_cash_account_name(),
			tuition_refund->
				enrollment->
				offering->
				revenue_account,
			account_fees_expense( (char *)0 ),
			account_loss( (char *)0 ),
			transaction_seconds_to_add ) ) )
	{
		tuition_refund->transaction_date_time =
			tuition_refund->tuition_refund_transaction->
				transaction_date_time;
	}
	else
	{
		tuition_refund->transaction_date_time = (char *)0;
	}

	return tuition_refund;
}

char *tuition_refund_primary_where(
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

TUITION_REFUND *tuition_refund_seek(
			LIST *deposit_tuition_refund_list,
			char *deposit_date_time )
{
	TUITION_REFUND *refund;

	if ( !list_rewind( deposit_tuition_refund_list ) )
		return (TUITION_REFUND *)0;

	do {
		refund = list_get( deposit_tuition_refund_list );

		if ( !refund->deposit )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty deposit.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( strcmp(	refund->deposit->deposit_date_time,
				deposit_date_time ) == 0 )
		{
			return refund;
		}
	} while ( list_next( deposit_tuition_refund_list ) );

	return (TUITION_REFUND *)0;
}

double tuition_refund_cash_credit_amount(
			double deposit_amount,
			double tuition_refund_fees_expense,
			int deposit_registration_list_length )
{
	if ( !deposit_registration_list_length ) return 0.0;

	return	0.0 -
		( deposit_amount /
		  (double)deposit_registration_list_length ) -
		tuition_refund_fees_expense;
}

void tuition_refund_list_insert( LIST *refund_list )
{
	TUITION_REFUND *refund;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];
	char *transaction_date_time;

	if ( !list_rewind( refund_list ) ) return;

	insert_pipe =
		tuition_refund_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		refund = list_get( refund_list );

		transaction_date_time =
			(refund->tuition_refund_transaction)
				? refund->
					tuition_refund_transaction->
					transaction_date_time
				: (char *)0;

		tuition_refund_insert_pipe(
			insert_pipe,
			refund->enrollment->registration->student_full_name,
			refund->enrollment->registration->street_address,
			refund->enrollment->offering->course->course_name,
			refund->enrollment->offering->semester->season_name,
			refund->enrollment->offering->semester->year,
			refund->deposit->payor_entity->full_name,
			refund->deposit->payor_entity->street_address,
			refund->deposit->deposit_date_time,
			refund->tuition_refund_amount,
			refund->tuition_refund_fees_expense,
			refund->tuition_refund_overpayment_loss,
			transaction_date_time );

	} while ( list_next( refund_list ) );

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

FILE *tuition_refund_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement t=%s f=\"%s\" replace=%c delimiter='%c'|"
		"sql 2>&1						 |"
		"cat >%s 						  ",
		TUITION_REFUND_TABLE,
		TUITION_REFUND_INSERT_COLUMNS,
		'y',
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void tuition_refund_insert_pipe(
			FILE *insert_pipe,
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			double refund_amount,
			double fees_expense,
			double overpayment_loss,
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
		refund_amount,
		fees_expense,
		overpayment_loss,
		(transaction_date_time)
			? transaction_date_time
			: "" );
}

void tuition_refund_list_enrollment_insert( LIST *refund_list )
{
	TUITION_REFUND *refund;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];
	char *transaction_date_time;

	if ( !list_rewind( refund_list ) ) return;

	insert_pipe =
		enrollment_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		refund = list_get( refund_list );

		transaction_date_time =
			(refund->enrollment->enrollment_transaction)
				? refund->
					enrollment->
					enrollment_transaction->
					transaction_date_time
				: (char *)0;

		enrollment_insert_pipe(
			insert_pipe,
			refund->enrollment->registration->student_full_name,
			refund->enrollment->registration->street_address,
			refund->enrollment->offering->course->course_name,
			refund->enrollment->offering->semester->season_name,
			refund->enrollment->offering->semester->year,
			transaction_date_time );

	} while ( list_next( refund_list ) );

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

void tuition_refund_list_registration_insert(
			LIST *refund_list )
{
	TUITION_REFUND *refund;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( refund_list ) ) return;

	insert_pipe =
		registration_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		refund = list_get( refund_list );

		registration_insert_pipe(
			insert_pipe,
			refund->enrollment->registration->student_full_name,
			refund->enrollment->registration->street_address,
			refund->enrollment->registration->season_name,
			refund->enrollment->registration->year,
			refund->
				enrollment->
				registration->
				registration_date_time );

	} while ( list_next( refund_list ) );

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

void tuition_refund_list_student_insert(
			LIST *refund_list )
{
	TUITION_REFUND *refund;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( refund_list ) ) return;

	insert_pipe =
		student_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		refund = list_get( refund_list );

		student_insert_pipe(
			insert_pipe,
			refund->enrollment->registration->student_full_name,
			refund->enrollment->registration->street_address );

	} while ( list_next( refund_list ) );

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

void tuition_refund_list_student_entity_insert(
			LIST *refund_list )
{
	TUITION_REFUND *refund;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( refund_list ) ) return;

	insert_pipe =
		entity_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		refund = list_get( refund_list );

		entity_insert_pipe(
			insert_pipe,
			refund->enrollment->registration->student_full_name,
			refund->enrollment->registration->street_address,
			(char *)0 /* email_address */ );

	} while ( list_next( refund_list ) );

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

void tuition_refund_list_payor_entity_insert(
			LIST *refund_list )
{
	TUITION_REFUND *refund;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( refund_list ) ) return;

	insert_pipe =
		entity_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		refund = list_get( refund_list );

		entity_insert_pipe(
			insert_pipe,
			refund->deposit->payor_entity->full_name,
			refund->deposit->payor_entity->street_address,
			refund->deposit->payor_entity->email_address );

	} while ( list_next( refund_list ) );

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

LIST *tuition_refund_registration_list(
			LIST *deposit_tuition_refund_list )
{
	TUITION_REFUND *refund;
	REGISTRATION *registration;
	LIST *registration_list;

	if ( !list_rewind( deposit_tuition_refund_list ) ) return (LIST *)0;

	registration_list = list_new();

	do {
		refund =
			list_get(
				deposit_tuition_refund_list );

		if ( !refund->enrollment )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty enrollment.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		if ( !refund->enrollment->registration )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: empty registration.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		registration = refund->enrollment->registration;

		list_set(
			registration_list,
			registration );

	} while ( list_next( deposit_tuition_refund_list ) );

	return registration_list;
}

LIST *tuition_refund_enrollment_list(
			LIST *deposit_tuition_refund_list )
{
	TUITION_REFUND *refund;
	char enrollment_cancelled_date[ 128 ];
	LIST *enrollment_list;

	if ( !list_rewind( deposit_tuition_refund_list ) ) return (LIST *)0;

	enrollment_list = list_new();

	do {
		refund =
			list_get(
				deposit_tuition_refund_list );

		if ( !refund->enrollment )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty enrollment.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		refund->enrollment->enrollment_cancelled_date =
			strdup(
				column(
					enrollment_cancelled_date,
					0,
					refund->
						enrollment->
						registration->
						registration_date_time ) );

		list_set(
			enrollment_list,
			refund->enrollment );

	} while ( list_next( deposit_tuition_refund_list ) );

	return enrollment_list;
}

void tuition_refund_list_trigger(
			LIST *deposit_tuition_refund_list )
{
	TUITION_REFUND *tuition_refund;

	if ( !list_rewind( deposit_tuition_refund_list ) ) return;

	do {
		tuition_refund =
			list_get(
				deposit_tuition_refund_list );

		tuition_refund_trigger(
			tuition_refund->
				enrollment->
				registration->
				student_full_name,
			tuition_refund->
				enrollment->
				registration->
				street_address,
			tuition_refund->
				enrollment->
				offering->
				course->
				course_name,
			tuition_refund->
				enrollment->
				offering->
				semester->
				season_name,
			tuition_refund->
				enrollment->
				offering->
				semester->
				year,
			tuition_refund->
				deposit->
				payor_entity->
				full_name,
			tuition_refund->
				deposit->
				payor_entity->
				street_address,
			tuition_refund->
				deposit->
				deposit_date_time,
			"insert" /* state */ );

	} while ( list_next( deposit_tuition_refund_list ) );
}

LIST *tuition_refund_list(
			char *season_name,
			int year,
			char *item_title_P,
			DEPOSIT *deposit )
{
	LIST *refund_list = list_new();
	TUITION_REFUND *refund;
	int student_number;

	for (	student_number = 1;
		( refund =
			tuition_refund(
				season_name,
				year,
				item_title_P,
				student_number,
				deposit ) );
		student_number++ )
	{
		list_set( refund_list, refund );
	}

	return refund_list;
}

TUITION_REFUND *tuition_refund(
			char *season_name,
			int year,
			char *item_title_P,
			int student_number,
			DEPOSIT *deposit )
{
	TUITION_REFUND *refund;
	TUITION_PAYMENT_ITEM_TITLE *tuition_payment_item_title;

	if ( ! ( tuition_payment_item_title =
			tuition_payment_item_title_new(
				item_title_P,
				student_number ) ) )
	{
		return (TUITION_REFUND *)0;
	}

	tuition_payment_item_title->
		tuition_payment_item_title_entity =
			tuition_payment_item_title_entity(
				tuition_payment_item_title->
					item_title_enrollment );

	tuition_payment_item_title->
		tuition_payment_item_title_course_name =
			tuition_payment_item_title_course_name(
				tuition_payment_item_title->
					item_title_enrollment );

	/* New refund */
	/* ---------- */
	refund = tuition_refund_calloc();

	/* Fetch enrollment */
	/* ---------------- */
	refund->enrollment =
		enrollment_fetch(
			tuition_payment_item_title->
				tuition_payment_item_title_entity->
				full_name,
			tuition_payment_item_title->
				tuition_payment_item_title_entity->
				street_address,
			tuition_payment_item_title->
				tuition_payment_item_title_course_name,
			season_name,
			year,
			0 /* not fetch_tuition_payment_list */,
			0 /* not fetch_tuition_refund_list */,
			1 /* fetch_offering */,
			1 /* fetch_registration */ );

	if ( !refund->enrollment )
	{
		/* Enrolled and refunded in the same spreadsheet. */
		/* ---------------------------------------------- */
		refund->enrollment =
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
	}

	/* Fetch the offering, course, and program */
	/* --------------------------------------- */
	refund->enrollment->offering =
			offering_fetch(
				tuition_payment_item_title->
					tuition_payment_item_title_course_name,
				season_name,
				year,
				1 /* fetch_course */,
				0 /* not fetch_enrollment_list */ );

	if ( !refund->enrollment->offering )
	{
		printf(
		"<h3>Warning: refund without an enrollment for %s/%s</h3>\n",
				tuition_payment_item_title->
					tuition_payment_item_title_entity->
					full_name,
				tuition_payment_item_title->
					tuition_payment_item_title_entity->
					street_address );
		return (TUITION_REFUND *)0;
	}

	/* Build registration */
	/* ------------------ */
	refund->enrollment->registration =
		registration_new(
			tuition_payment_item_title->
				tuition_payment_item_title_entity->
				full_name,
			tuition_payment_item_title->
				tuition_payment_item_title_entity->
				street_address,
			season_name,
			year );

	refund->enrollment->registration->registration_date_time =
		deposit->deposit_date_time;
	
	refund->
		enrollment->
		registration->
		enrollment_list =
			list_new();

	list_set(
		refund->
			enrollment->
			registration->
			enrollment_list,
		refund->enrollment );

	/* Set deposit */
	/* ----------- */
	refund->deposit = deposit;

	return refund;
}

double tuition_refund_revenue_debit_amount(
			double refund_amount )
{
	return refund_amount;
}

void tuition_refund_trigger(
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
"tuition_refund_trigger \"%s\" '%s' \"%s\" '%s' %d \"%s\" '%s' '%s' '%s'",
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

void tuition_refund_list_enrollment_trigger(
			LIST *tuition_refund_list )
{
	TUITION_REFUND *refund;

	if ( !list_rewind( tuition_refund_list ) ) return;

	do {
		refund =
			list_get(
				tuition_refund_list );

		enrollment_trigger(
			refund->
				enrollment->
				registration->
				student_full_name,
			refund->
				enrollment->
				registration->
				street_address,
			refund->
				enrollment->
				offering->
				course->
				course_name,
			refund->
				enrollment->
				offering->
				semester->
				season_name,
			refund->
				enrollment->
				offering->
				semester->
				year );

	} while ( list_next( tuition_refund_list ) );
}

LIST *tuition_refund_transaction_list(
			LIST *tuition_refund_list )
{
	TUITION_REFUND *tuition_refund;
	LIST *transaction_list;

	if ( !list_rewind( tuition_refund_list ) ) return (LIST *)0;

	transaction_list = list_new();

	do {
		tuition_refund =
			list_get(
				tuition_refund_list );

		if ( !tuition_refund->enrollment )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty enrollment.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		if ( tuition_refund->enrollment->enrollment_transaction )
		{
			list_set(
				transaction_list,
				tuition_refund->
					enrollment->
					enrollment_transaction );
		}

		if ( tuition_refund->tuition_refund_transaction )
		{
			list_set(
				transaction_list,
				tuition_refund->
					tuition_refund_transaction );
		}

	} while ( list_next( tuition_refund_list ) );

	return transaction_list;
}

boolean tuition_refund_structure(
			TUITION_REFUND *tuition_refund )
{
	if ( !tuition_refund )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty tuition_refund\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return 0;
	}

	if ( !tuition_refund->enrollment )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty enrollment\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return 0;
	}

	if ( !tuition_refund->enrollment->registration )
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

	if ( !tuition_refund->enrollment->offering )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty offering\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return 0;
	}

	if ( !tuition_refund->enrollment->offering->course )
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

LIST *tuition_refund_list_steady_state(
			LIST *deposit_tuition_refund_list,
			LIST *deposit_registration_list,
			LIST *semester_offering_list,
			double deposit_amount,
			double transaction_fee )
{
	TUITION_REFUND *tuition_refund;
	REGISTRATION *registration;
	OFFERING *offering;
	ENROLLMENT *enrollment;

	/* -------------------------------------------- */
	/* Note: ENROLLMENT.transaction_date_time gets 	*/
	/* REGISTRATION.registration_date_time.		*/
	/* So, start with 1.				*/
	/* Note: DEPOSIT.deposit_date becomes		*/
	/* REGISTRATION.registration_date_time.		*/
	/* -------------------------------------------- */
	int transaction_seconds_to_add = 1;

	if ( !list_rewind( deposit_tuition_refund_list ) ) return (LIST *)0;

	do {
		tuition_refund = list_get( deposit_tuition_refund_list );

		if ( !tuition_refund_structure( tuition_refund ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: tuition_refund_structure() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		/* Separate out the components */
		/* --------------------------- */
		enrollment = tuition_refund->enrollment;
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
					enrollment,
					transaction_seconds_to_add - 1,
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
		list_push( deposit_tuition_refund_list );

		tuition_refund =
			tuition_refund_steady_state(
				tuition_refund,
				deposit_tuition_refund_list,
				deposit_registration_list,
				registration->enrollment_list,
				semester_offering_list,
				deposit_amount,
				transaction_fee,
				transaction_seconds_to_add );

		list_pop( deposit_tuition_refund_list );

		transaction_seconds_to_add += 2;

	} while ( list_next( deposit_tuition_refund_list ) );

	return deposit_tuition_refund_list;
}

char *tuition_refund_memo( char *program_name )
{
	static char refund_memo[ 128 ];

	if ( program_name && *program_name )
	{
		sprintf(refund_memo,
			"%s/%s",
			TUITION_REFUND_MEMO,
			program_name );
	}
	else
	{
		sprintf(refund_memo,
			"%s Payment",
			TUITION_REFUND_MEMO );
	}
	return refund_memo;
}

char *tuition_refund_list_display( LIST *refund_list )
{
	char display[ 65536 ];
	char *ptr = display;
	TUITION_REFUND *refund;
	char *course_name;

	*ptr = '\0';

	if ( !list_rewind( refund_list ) )
	{
		return "";
	}

	do {
		refund =
			list_get(
				refund_list );

		if ( !list_at_head( refund_list ) )
		{
			ptr += sprintf( ptr, ", " );
		}

		if ( refund->enrollment->offering )
		{
			course_name =
				refund->
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
				"%s will be refunded in %s",
				entity_name_display(
					refund->
						enrollment->
						registration->
						student_full_name,
					refund->
						enrollment->
						registration->
						street_address ),
				course_name );

	} while ( list_next( refund_list ) );

	ptr += sprintf( ptr, "\n" );

	return strdup( display );
}

