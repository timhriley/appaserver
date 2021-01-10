/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/paypal_sweep.c	*/
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
#include "entity_self.h"
#include "account.h"
#include "transaction.h"
#include "journal.h"
#include "paypal_sweep.h"

PAYPAL_SWEEP *paypal_sweep_new(
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time )
{
	PAYPAL_SWEEP *paypal_sweep = paypal_sweep_calloc();

	paypal_sweep->payor_full_name = payor_full_name;
	paypal_sweep->payor_street_address = payor_street_address;
	paypal_sweep->paypal_date_time = paypal_date_time;

	return paypal_sweep;
}

PAYPAL_SWEEP *paypal_sweep_fetch(
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time )
{
	char sys_string[ 1024 ];

	if ( !payor_full_name || !*payor_full_name
	||   !payor_street_address || !*payor_street_address
	||   !paypal_date_time || !*paypal_date_time )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty primary information.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 PAYPAL_SWEEP_TABLE,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 paypal_sweep_primary_where(
			payor_full_name,
			payor_street_address,
			paypal_date_time ) );

	return
		paypal_sweep_parse(
			pipe2string( sys_string ) );
}


LIST *paypal_sweep_list( char *where )
{
	return paypal_sweep_system_list(
			paypal_sweep_sys_string(
				where ) );
}

PAYPAL_SWEEP *paypal_sweep_calloc( void )
{
	PAYPAL_SWEEP *paypal_sweep;

	if ( ! ( paypal_sweep = calloc( 1, sizeof( PAYPAL_SWEEP ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return paypal_sweep;
}

LIST *paypal_sweep_system_list( char *sys_string )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *sweep_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			sweep_list,
			paypal_sweep_parse(
				input ) );
	}

	pclose( input_pipe );
	return sweep_list;
}

PAYPAL_SWEEP *paypal_sweep_parse( char *input )
{
	char payor_full_name[ 128 ];
	char payor_street_address[ 128 ];
	char paypal_date_time[ 128 ];
	char sweep_amount[ 128 ];
	char transaction_date_time[ 128 ];
	PAYPAL_SWEEP *paypal_sweep;

	if ( !input || !*input ) return (PAYPAL_SWEEP *)0;

	/* See: attribute_list */
	/* ------------------- */
	piece( payor_full_name, SQL_DELIMITER, input, 0 );
	piece( payor_street_address, SQL_DELIMITER, input, 1 );
	piece( paypal_date_time, SQL_DELIMITER, input, 2 );

	paypal_sweep =
		paypal_sweep_new(
			strdup( payor_full_name ),
			strdup( payor_street_address ),
			strdup( paypal_date_time ) );

	piece( sweep_amount, SQL_DELIMITER, input, 3 );
	paypal_sweep->sweep_amount = atof( sweep_amount );

	piece( transaction_date_time, SQL_DELIMITER, input, 4 );
	paypal_sweep->transaction_date_time = strdup( transaction_date_time );

	return paypal_sweep;
}

char *paypal_sweep_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 PAYPAL_SWEEP_TABLE,
		 where );

	return strdup( sys_string );
}

char *paypal_sweep_primary_where(
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time  )
{
	char static where[ 128 ];

	sprintf(where,
		"payor_full_name = '%s' and		"
		"payor_street_address = '%s' and	"
		"paypal_date_time = '%s'		",
		entity_name_escape( payor_full_name ),
		payor_street_address,
		paypal_date_time );

	return where;
}

void paypal_sweep_insert( PAYPAL_SWEEP *paypal_sweep )
{
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !paypal_sweep ) return;

	insert_pipe =
		paypal_sweep_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	paypal_sweep_insert_pipe(
		insert_pipe,
			paypal_sweep->payor_full_name,
			paypal_sweep->payor_street_address,
			paypal_sweep->paypal_date_time,
			paypal_sweep->sweep_amount,
			paypal_sweep->transaction_date_time );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf(sys_string,
			"cat %s						|"
			"queue_top_bottom_lines.e 300			|"
			"html_table.e 'Insert Sweep Errors' '' '^'",
			 error_filename );

		if ( system( sys_string ) ){}
	}

	sprintf( sys_string, "rm %s", error_filename );

	if ( system( sys_string ) ){};
}

FILE *paypal_sweep_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement t=%s f=\"%s\" replace=%c delimiter='%c'|"
		"sql 2>&1						 |"
		"cat >%s 						  ",
		PAYPAL_SWEEP_TABLE,
		PAYPAL_SWEEP_INSERT_COLUMNS,
		'y',
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void paypal_sweep_insert_pipe(
			FILE *insert_pipe,
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time,
			double sweep_amount,
			char *transaction_date_time )
{
	fprintf(insert_pipe,
		"%s^%s^%s^%.2lf^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name( payor_full_name ),
		payor_street_address,
		paypal_date_time,
		sweep_amount,
		(transaction_date_time)
			? transaction_date_time
			: "" );
}

void paypal_sweep_set_transaction(
			int *seconds_to_add,
			PAYPAL_SWEEP *paypal_sweep )
{
	if ( ( paypal_sweep->paypal_sweep_transaction =
	          paypal_sweep_transaction(
			seconds_to_add,
			paypal_sweep->payor_full_name,
			paypal_sweep->payor_street_address,
			paypal_sweep->paypal_date_time,
			paypal_sweep->sweep_amount,
			entity_self_paypal_cash_account_name(),
			account_cash( (char *)0 ) ) ) )
	{
		paypal_sweep->transaction_date_time =
			paypal_sweep->paypal_sweep_transaction->
				transaction_date_time;
	}
	else
	{
		paypal_sweep->transaction_date_time = (char *)0;
	}
}

TRANSACTION *paypal_sweep_transaction(
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time,
			double sweep_amount,
			char *paypal_cash_account_name,
			char *account_cash )
{
	TRANSACTION *transaction;
	JOURNAL *journal;

	if ( dollar_virtually_same( sweep_amount, 0.0 ) )
		return (TRANSACTION *)0;

	if ( !paypal_date_time )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty paypal_date_time\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	transaction =
		transaction_full(
			payor_full_name,
			payor_street_address,
			paypal_date_time
				/* transaction_date_time */,
			sweep_amount
				/* transaction_amount */,
			"Sweep" /* memo */,
			1 /* lock_transaction */,
			(*seconds_to_add)++ );

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

	journal->debit_amount = sweep_amount;

	/* Debit paypal cash account */
	/* ------------------------- */
	list_set(
		transaction->journal_list,
		( journal =
			journal_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				paypal_cash_account_name ) ) );

	journal->credit_amount = sweep_amount;

	return transaction;
}

FILE *paypal_sweep_update_open( void )
{
	char sys_string[ 2048 ];

	sprintf( sys_string,
		 "update_statement table=%s key=%s carrot=y	|"
		 "tee_appaserver_error.sh 			|"
		 "sql						 ",
		 PAYPAL_SWEEP_TABLE,
		 PAYPAL_SWEEP_PRIMARY_KEY );

	return popen( sys_string, "w" );
}

void paypal_sweep_update(
			char *transaction_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time )
{
	FILE *update_pipe = paypal_sweep_update_open();

	fprintf(update_pipe,
		"%s^%s^%s^transaction_date_time^%s\n",
		payor_full_name,
		payor_street_address,
		paypal_date_time,
		(transaction_date_time)
			? transaction_date_time
			: "" );

	pclose( update_pipe );
}

PAYPAL_SWEEP *paypal_sweep_paypal(
			ENTITY *payor_entity,
			char *paypal_date_time,
			double paypal_amount )
{
	PAYPAL_SWEEP *paypal_sweep;

	if ( !paypal_amount ) return (PAYPAL_SWEEP *)0;

	paypal_sweep =
		paypal_sweep_new(
			payor_entity->full_name,
			payor_entity->street_address,
			paypal_date_time );

	paypal_sweep->sweep_amount = paypal_amount;
	return paypal_sweep;
}

/* Safely returns heap memory */
/* -------------------------- */
char *paypal_sweep_display(
			PAYPAL_SWEEP *paypal_sweep )
{
	char display[ 128 ];

	*display = '\0';

	if ( paypal_sweep )
	{
		sprintf(	display,
				"Sweep: %.2lf; ",
				paypal_sweep->sweep_amount );
	}
	return strdup( display );
}

