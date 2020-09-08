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
#include "list.h"
#include "payment.h"
#include "payment_fns.h"
#include "transaction.h"
#include "deposit.h"
#include "registration.h"
#include "registration_fns.h"
#include "journal.h"
#include "offering.h"
#include "offering_fns.h"
#include "enrollment.h"
#include "semester.h"
#include "account.h"
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

double payment_amount(	double deposit_remaining,
			double registration_invoice_amount_due )
{
	if ( deposit_remaining < registration_invoice_amount_due )
		return registration_invoice_amount_due;
	else
		return deposit_remaining;
}

FILE *payment_update_open( void )
{
	char sys_string[ 2048 ];

	sprintf( sys_string,
		 "update_statement table=%s key=%s carrot=y	|"
"tee -a /var/log/appaserver/appaserver_tnt.err |"
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
		 transaction_date_time );

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

	if ( fetch_enrollment )
	{
fprintf(stderr,
	"%s/%s()/%d: fetching enrollment\n",
	__FILE__,
	__FUNCTION__,
	__LINE__ );

		payment->enrollment =
			enrollment_fetch(
				student_full_name,
				street_address,
				course_name,
				season_name,
				atoi( year ),
				0 /* not fetch_payment_list */ );
fprintf(stderr,
	"%s/%s()/%d: got course = [%x]\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
payment->enrollment->offering->course );

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
				0 /* not fetch_payment_list */ );
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
			char *account_cash,
			char *account_receivable,
			char *account_fees_expense,
			char *account_gain )
{
	TRANSACTION *transaction;
	JOURNAL *journal;

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

	journal->debit_amount =
		payment_amount - fees_expense + gain_donation;

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

	journal->credit_amount = payment_amount;

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
			ENROLLMENT *enrollment,
			DEPOSIT *deposit,
			double deposit_amount,
			double deposit_transaction_fee,
			char *program_name,
			PAYMENT *payment )
{
	if ( !enrollment )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty enrollment.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

fprintf(stderr,
	"%s/%s()/%d: BUT NOW? got course = [%x]\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
enrollment->offering->course );

	/* Set the input parameters */
	/* ------------------------ */
	payment->deposit_amount = deposit_amount;
	payment->deposit_transaction_fee = deposit_transaction_fee;

	/* Do the work */
	/* ----------- */
	payment->registration_tuition_total =
		registration_tuition_total(
			deposit->
				deposit_registration_list );

	payment->deposit_remaining =
		deposit_remaining(
			deposit_amount,
			registration->
				registration_invoice_amount_due );

	payment->payment_amount =
		payment_amount(
			payment->
				deposit_remaining,
			registration->
				registration_invoice_amount_due );

	payment->payment_fees_expense =
		payment_fees_expense(
			deposit_transaction_fee,
			payment->deposit_payment_list );

	payment->payment_gain_donation =
		payment_gain_donation(
			deposit_amount,
			payment->deposit_registration_list );

	payment->payment_transaction =
		payment_transaction(
			payment->deposit->payor_entity->full_name,
			payment->deposit->payor_entity->street_address,
			payment->deposit->deposit_date_time,
			program_name,
			payment->payment_amount,
			payment->payment_fees_expense,
			payment->payment_gain_donation,
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

