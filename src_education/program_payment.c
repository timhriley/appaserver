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
#include "transaction.h"
#include "journal.h"
#include "entity.h"
#include "account.h"
#include "deposit.h"
#include "program_payment_item_title.h"
#include "program_payment_fns.h"
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
			boolean fetch_program,
			boolean fetch_deposit )
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
			fetch_program,
			fetch_deposit );

	return program_payment;
}

LIST *program_payment_system_list(
			char *sys_string,
			boolean fetch_program,
			boolean fetch_deposit )
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
				fetch_program,
				fetch_deposit ) );
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
			program_payment->program->program_name,
			program_payment->deposit->payor_entity->full_name,
			program_payment->deposit->payor_entity->street_address,
			program_payment->deposit->semester->season_name,
			program_payment->deposit->semester->year,
			program_payment->deposit->deposit_date_time,
			program_payment->program_payment_amount,
			program_payment->program_payment_fees_expense,
			program_payment->program_payment_net_payment_amount,
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
			boolean fetch_program,
			boolean fetch_deposit )
{
	char program_name[ 128 ];
	char payor_full_name[ 128 ];
	char payor_street_address[ 128 ];
	char season_name[ 128 ];
	char year[ 128 ];
	char deposit_date_time[ 128 ];
	char payment_amount[ 128 ];
	char fees_expense[ 128 ];
	char net_payment_amount[ 128 ];
	char transaction_date_time[ 128 ];
	PROGRAM_PAYMENT *program_payment;

	if ( !input || !*input ) return (PROGRAM_PAYMENT *)0;

	program_payment = program_payment_calloc();

	/* See: attribute_list program_payment */
	/* ----------------------------------- */
	piece( program_name, SQL_DELIMITER, input, 0 );
	program_payment->program = program_new( strdup( program_name ) );

	piece( payor_full_name, SQL_DELIMITER, input, 1 );
	piece( payor_street_address, SQL_DELIMITER, input, 2 );
	piece( season_name, SQL_DELIMITER, input, 3 );
	piece( year, SQL_DELIMITER, input, 4 );
	piece( deposit_date_time, SQL_DELIMITER, input, 5 );

	program_payment->deposit =
		deposit_new(
			strdup( payor_full_name ),
			strdup( payor_street_address ),
			strdup( season_name ),
			atoi( year ),
			strdup( deposit_date_time ) );

	piece( payment_amount, SQL_DELIMITER, input, 6 );
	program_payment->program_payment_amount =
		atof( payment_amount );

	piece( fees_expense, SQL_DELIMITER, input, 7 );
	program_payment->program_payment_fees_expense = atof( fees_expense );

	piece( net_payment_amount, SQL_DELIMITER, input, 8 );
	program_payment->program_payment_net_payment_amount =
		atof( net_payment_amount );

	piece( transaction_date_time, SQL_DELIMITER, input, 9 );
	program_payment->transaction_date_time =
		strdup( transaction_date_time );

	if ( fetch_program )
	{
		program_payment->program =
			program_fetch(
				program_payment->program->program_name,
				1 /* fetch_alias_list */ );
	}

	if ( fetch_deposit )
	{
		program_payment->deposit =
		    deposit_fetch(
			 program_payment->deposit->payor_entity->full_name,
			 program_payment->deposit->payor_entity->street_address,
			 program_payment->deposit->semester->season_name,
			 program_payment->deposit->semester->year,
			 program_payment->deposit->deposit_date_time,
			 0 /* not fetch_tuition_payment_list */,
			 0 /* not fetch_program_payment_list */ );
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
			PROGRAM_PAYMENT_MEMO );

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
		 PROGRAM_PAYMENT_PRIMARY_KEY );

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

char *program_payment_list_display( LIST *payment_list )
{
	char display[ 65536 ];
	char *ptr = display;
	PROGRAM_PAYMENT *payment;

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
				"%s\n",
				payment->program->program_name );

	} while ( list_next( payment_list ) );

	return strdup( display );
}

double program_payment_amount(
			double deposit_amount )
{
	return deposit_amount;
}

double program_payment_fees_expense(
			double deposit_fees_expense )
{
	return deposit_fees_expense;
}

double program_payment_net_payment_amount(
			double deposit_net_payment_amount )
{
	return deposit_net_payment_amount;
}

PROGRAM_PAYMENT *program_payment_steady_state(
			PROGRAM_PAYMENT *program_payment,
			double deposit_amount,
			double deposit_transaction_fee,
			double deposit_net_payment_amount )
{
	program_payment->program_payment_amount =
		program_payment_amount(
			deposit_amount );

	program_payment->program_payment_fees_expense =
		program_payment_fees_expense(
			deposit_transaction_fee );

	program_payment->program_payment_net_payment_amount =
		program_payment_net_payment_amount(
			deposit_net_payment_amount );

	if ( !program_payment->transaction_date_time
	||   !*program_payment->transaction_date_time )
	{
		program_payment->transaction_date_time =
			program_payment->deposit->deposit_date_time;
	}

	if ( ( program_payment->program_payment_transaction =
		program_payment_transaction(
			program_payment->deposit->payor_entity->full_name,
			program_payment->deposit->payor_entity->street_address,
			program_payment->transaction_date_time,
			program_payment->program->program_name,
			program_payment->program_payment_amount,
			program_payment->program_payment_fees_expense,
			program_payment->program_payment_net_payment_amount,
			account_cash( (char *)0 ),
			account_fees_expense( (char *)0 ),
			program_payment->program->revenue_account ) ) )
	{
		program_payment->transaction_date_time =
			program_payment->program_payment_transaction->
				transaction_date_time;
	}
	else
	{
		program_payment->transaction_date_time = (char *)0;
	}

	return program_payment;
}

PROGRAM_PAYMENT *program_payment(
			LIST *not_exists_program_name_list,
			char *item_title_P,
			int program_number,
			LIST *education_program_list,
			/* -------- */
			/* Set only */
			/* -------- */
			DEPOSIT *deposit )
{
	PROGRAM_PAYMENT *program_payment;
	PROGRAM_PAYMENT_ITEM_TITLE *payment_item_title;
	char *program_name;

	if ( ! ( payment_item_title =
			program_payment_item_title_new(
				item_title_P,
				program_number ) ) )
	{
		return (PROGRAM_PAYMENT *)0;
	}

	if ( ! ( program_name =
			program_payment_item_title_name(
				not_exists_program_name_list,
				item_title_P,
				program_number,
				education_program_list ) ) )
	{
		return (PROGRAM_PAYMENT *)0;
	}

	/* New payment */
	/* ----------- */
	program_payment = program_payment_calloc();

	program_payment->program = program_new( strdup( program_name ) );
	program_payment->program_payment_item_title = payment_item_title;
	program_payment->deposit = deposit;

	return program_payment;
}

LIST *program_payment_list(
			LIST *not_exists_program_payment_list,
			char *item_title_P,
			LIST *education_program_list,
			DEPOSIT *deposit )
{
	LIST *payment_list = list_new();
	PROGRAM_PAYMENT *payment;
	int program_number;

	for (	program_number = 1;
		( payment =
			program_payment(
				not_exists_program_payment_list,
				item_title_P,
				program_number,
				education_program_list,
				deposit ) );
		program_number++ )
	{
		list_set( payment_list, payment );
	}
	return payment_list;
}

void program_payment_trigger(
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			char *state )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
"program_payment_trigger \"%s\" \"%s\" '%s' '%s' %d '%s' '%s'",
		program_name,
		payor_full_name,
		payor_street_address,
		season_name,
		year,
		deposit_date_time,
		state );

	if ( system( sys_string ) ){}
}

double program_payment_total( LIST *payment_list )
{
	PROGRAM_PAYMENT *payment;
	double total;

	if ( !list_rewind( payment_list ) ) return 0.0;

	total = 0.0;

	do {
		payment = list_get( payment_list );

		total += payment->program_payment_amount;

	} while ( list_next( payment_list ) );

	return total;
}

void program_payment_list_trigger(
			LIST *program_payment_list )
{
	PROGRAM_PAYMENT *program_payment;

	if ( !list_rewind( program_payment_list ) ) return;

	do {
		program_payment = list_get( program_payment_list );

		program_payment_trigger(
			program_payment->program->program_name,
			program_payment->
				deposit->
				payor_entity->
				full_name,
			program_payment->
				deposit->
				payor_entity->
				street_address,
			program_payment->deposit->semester->season_name,
			program_payment->deposit->semester->year,
			program_payment->
				deposit->
				deposit_date_time,
			"insert" /* state */ );

	} while ( list_next( program_payment_list ) );
}

LIST *program_payment_transaction_list(
			LIST *program_payment_list )
{
	PROGRAM_PAYMENT *program_payment;
	LIST *transaction_list;

	if ( !list_rewind( program_payment_list ) ) return (LIST *)0;

	transaction_list = list_new();

	do {
		program_payment =
			list_get(
				program_payment_list );

		if ( program_payment->program_payment_transaction )
		{
			list_set(
				transaction_list,
				program_payment->program_payment_transaction );
		}

	} while ( list_next( program_payment_list ) );

	return transaction_list;
}

LIST *program_payment_list_steady_state(
			LIST *deposit_program_payment_list,
			double deposit_amount,
			double transaction_fee,
			double net_payment_amount )
{
	PROGRAM_PAYMENT *program_payment;

	if ( !list_rewind( deposit_program_payment_list ) ) return (LIST *)0;

	do {
		program_payment = list_get( deposit_program_payment_list );

		program_payment =
			program_payment_steady_state(
				program_payment,
				deposit_amount,
				transaction_fee,
				net_payment_amount );

		list_pop( deposit_program_payment_list );

	} while( list_next( deposit_program_payment_list ) );

	return deposit_program_payment_list;
}

