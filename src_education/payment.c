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
#include "transaction.h"
#include "journal.h"
#include "enrollment.h"
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

double payment_amount(
			double deposit_remaining,
			double registration_invoice_amount_due )
{
	if ( deposit_remaining < registration_invoice_amount_due )
		return registration_invoice_amount_due;
	else
		return deposit_remaining;
}

/* Returns true transaction_date_time */
/* ---------------------------------- */
char *payment_transaction_refresh(
			char *student_full_name,
			char *student_street_address,
			char *transaction_date_time,
			char *program_name,
			double payment_amount,
			LIST *journal_list )
{
	return transaction_journal_refresh(
		student_full_name,
		student_street_address,
		transaction_date_time,
		payment_amount,
		program_name /* memo */,
		0 /* check_number */,
		journal_list );
}

char *payment_update(	double payment_amount,
			double fees_expense,
			double gain_donation,
			char *transaction_date_time,
			char *student_full_name,
			char *student_street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time )
{
if ( payment_amount ){}
if ( fees_expense ){}
if ( gain_donation ){}
if ( transaction_date_time ){}
if ( student_full_name ){}
if ( student_street_address ){}
if ( course_name ){}
if ( season_name ){}
if ( year ){}
if ( payor_full_name ){}
if ( payor_street_address ){}
if ( deposit_date_time ){}

	return transaction_date_time;
}

LIST *payment_system_list( char *sys_string )
{
	char input[ 1024 ];
	FILE *input_pipe = popen( sys_string, "r" );
	LIST *payment_list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			payment_list,
			payment_parse(
				input ) );
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

PAYMENT *payment_parse( char *input )
{
	PAYMENT *payment;
	char student_full_name[ 128 ];
	char student_street_address[ 128 ];
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
	piece( student_street_address, SQL_DELIMITER, input, 1 );
	piece( course_name, SQL_DELIMITER, input, 2 );
	piece( season_name, SQL_DELIMITER, input, 3 );
	piece( year, SQL_DELIMITER, input, 4 );
	piece( payor_full_name, SQL_DELIMITER, input, 5 );
	piece( payor_street_address, SQL_DELIMITER, input, 6 );
	piece( deposit_date_time, SQL_DELIMITER, input, 7 );

	payment = payment_calloc();

	payment->enrollment =
		enrollment_new(
			student_full_name,
			student_street_address,
			course_name,
			season_name,
			atoi( year ) );

	payment->deposit =
		deposit_new(
			payor_full_name,
			payor_street_address,
			season_name,
			atoi( year ),
			deposit_date_time );

	return payment;
}

TRANSACTION *payment_transaction(
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			char *program_name,
			double payment_amount,
			double fees_expense,
			double donation_gain,
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
		payment_amount - fees_expense + donation_gain;

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

	if ( donation_gain )
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

		journal->credit_amount = donation_gain;
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

