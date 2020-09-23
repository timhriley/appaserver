/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_program/program_payment.c	*/
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
#include "program.h"
#include "payment.h"
#include "transaction.h"
#include "journal.h"
#include "entity.h"
#include "program_payment.h"

PROGRAM_PAYMENT *program_payment_calloc( void )
{
	PROGRAM_PAYMENT *program_payment;

	if ( ! ( program_payment = calloc( 1, sizeof( PROGRAM_PAYMENT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return program_payment;
}

PROGRAM_PAYMENT *program_payment_fetch(
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_program )
{
	PROGRAM_PAYMENT *program_payment;

	program_payment =
		program_payment_parse(
			pipe2string(
				program_payment_sys_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					program_payment_primary_where(
						program_name,
						payor_full_name,
						payor_street_address,
						season_name,
						year,
						deposit_date_time ) ) ),
			fetch_program );

	return program_payment;
}

LIST *program_payment_system_list(
			char *sys_string,
			boolean fetch_program )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *program_payment_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			program_payment_list,
			program_payment_parse(
				input,
				fetch_program ) );
	}

	pclose( input_pipe );
	return program_payment_list;
}

char *program_payment_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 PROGRAM_PAYMENT_TABLE,
		 where );

	return strdup( sys_string );
}

void program_payment_list_insert( LIST *program_payment_list )
{
	PROGRAM_PAYMENT *program_payment;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( program_payment_list ) ) return;

	insert_pipe =
		program_payment_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		program_payment = list_get( program_payment_list );

		program_payment_insert_pipe(
			insert_pipe,
			program_payment->program_name,
			program_payment->payor_full_name,
			program_payment->payor_street_address,
			program_payment->season_name,
			program_payment->year,
			program_payment->deposit_date_time,
			program_payment->payment_amount,
			program_payment->fees_expense,
			program_payment->net_payment_amount,
			program_payment->transaction_date_time );

	} while ( list_next( program_payment_list ) );

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

FILE *program_payment_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement t=%s f=\"%s\" replace=n delimiter='%c'|"
		"sql 2>&1						|"
		"cat >%s 						 ",
		PROGRAM_PAYMENT_TABLE,
		PROGRAM_PAYMENT_INSERT_COLUMNS,
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void program_payment_insert_pipe(
			FILE *insert_pipe,
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			double payment_amount,
			double fees_expense,
			double net_payment_amount,
			char *transaction_date_time )
{
	fprintf(insert_pipe,
		"%s^%s^%s^%s^%d^%s^%.2lf^%.2lf^%.2lf^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		program_name_escape( program_name ),
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name( payor_full_name ),
		payor_street_address,
		season_name,
		year,
		deposit_date_time,
		payment_amount,
		fees_expense,
		net_payment_amount,
		(transaction_date_time)
			? transaction_date_time
			: "" );
}

PROGRAM_PAYMENT *program_payment_parse(
			char *input,
			boolean fetch_program )
{
	char buffer[ 128 ];
	PROGRAM_PAYMENT *program_payment;

	if ( !input || !*input ) return (PROGRAM_PAYMENT *)0;

	program_payment = program_payment_calloc();

	/* See: attribute_list */
	/* ------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	program_payment->program_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	program_payment->payor_full_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	program_payment->payor_street_address = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	program_payment->season_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	program_payment->year = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	program_payment->deposit_date_time = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	program_payment->payment_amount = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	program_payment->fees_expense = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 8 );
	program_payment->net_payment_amount = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 9 );
	program_payment->transaction_date_time = strdup( buffer );

	if ( fetch_program )
	{
		program_payment->program =
			program_fetch(
				program_payment->program_name,
				0 /* not fetch_alias_list */ );
	}

	return program_payment;
}

char *program_payment_primary_where(
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time )
{
	char static where[ 1024 ];

	sprintf(where,
		"program_name = '%s' and		"
		"payor_full_name = '%s' and		"
		"payor_street_address = '%s' and	"
		"season_name = '%s' and			"
		"year = %d and				"
		"deposit_date_time = '%s'		",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 program_name_escape( program_name ),
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( payor_full_name ),
		 payor_street_address,
		 season_name,
		 year,
		 deposit_date_time );

	return where;
}

TRANSACTION *program_payment_transaction(
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			char *program_name,
			double payment_amount,
			double fees_expense,
			double net_payment_amount,
			char *account_cash,
			char *account_fees_expense,
			char *program_revenue_account )
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

	journal->debit_amount = net_payment_amount;

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

	/* Credit revenue */
	/* -------------- */
	list_set(
		transaction->journal_list,
		( journal =
			journal_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				program_revenue_account ) ) );

	journal->credit_amount = payment_amount;

	return transaction;
}

FILE *program_payment_update_open( void )
{
	char sys_string[ 2048 ];

	sprintf( sys_string,
		 "update_statement table=%s key=%s carrot=y	|"
		 "tee_appaserver_error.sh 			|"
		 "sql						 ",
		 PROGRAM_PAYMENT_TABLE,
		 PAYMENT_PRIMARY_KEY );

	return popen( sys_string, "w" );
}

void program_payment_update(
			char *transaction_date_time,
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time )
{
	FILE *update_pipe = program_payment_update_open();

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%d^%s^transaction_date_time^%s\n",
		 program_name,
		 payor_full_name,
		 payor_street_address,
		 season_name,
		 year,
		 deposit_date_time,
		 (transaction_date_time)
			? transaction_date_time
			: "" );

	pclose( update_pipe );
}

