/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/program_donation.c	*/
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
#include "paypal_upload.h"
#include "program.h"
#include "transaction.h"
#include "journal.h"
#include "entity.h"
#include "account.h"
#include "education.h"
#include "paypal_deposit.h"
#include "paypal_item.h"
#include "program_donation.h"

PROGRAM_DONATION *program_donation_calloc( void )
{
	PROGRAM_DONATION *program_donation;

	if ( ! ( program_donation = calloc( 1, sizeof( PROGRAM_DONATION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return program_donation;
}

PROGRAM_DONATION *program_donation_fetch(
			char *program_name,
			char *payment_date_time,
			char *payor_full_name,
			char *payor_street_address,
			boolean fetch_program,
			boolean fetch_paypal )
{
	PROGRAM_DONATION *program_donation;

	program_donation =
		program_donation_parse(
			pipe2string(
				program_donation_sys_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					program_donation_primary_where(
						program_name,
						payment_date_time,
						payor_full_name,
						payor_street_address ) ) ),
			fetch_program,
			fetch_paypal );

	return program_donation;
}

LIST *program_donation_system_list(
			char *sys_string,
			boolean fetch_program,
			boolean fetch_deposit )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *program_donation_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			program_donation_list,
			program_donation_parse(
				input,
				fetch_program,
				fetch_deposit ) );
	}

	pclose( input_pipe );
	return program_donation_list;
}

char *program_donation_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 PROGRAM_DONATION_TABLE,
		 where );

	return strdup( sys_string );
}

void program_donation_list_insert(
			LIST *program_donation_list,
			char *paypal_date_time )
{
	PROGRAM_DONATION *program_donation;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( program_donation_list ) ) return;

	insert_pipe =
		program_donation_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		program_donation = list_get( program_donation_list );

		program_donation_insert_pipe(
			insert_pipe,
			program_donation->program->program_name,
			program_donation->payment_date_time,
			program_donation->
				payor_full_name,
			program_donation->
				payor_street_address,
			program_donation->donation_amount,
			program_donation->net_payment_amount,
			program_donation->transaction_date_time,
			program_donation->merchant_fees_expense,
			paypal_date_time );

	} while ( list_next( program_donation_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		char *title = "Insert Program Payment Errors";

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

FILE *program_donation_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement t=%s f=\"%s\" replace=%c delimiter='%c'|"
		"sql 2>&1						 |"
		"cat >%s 						  ",
		PROGRAM_DONATION_TABLE,
		PROGRAM_DONATION_INSERT_COLUMNS,
		'y',
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void program_donation_insert_pipe(
			FILE *insert_pipe,
			char *program_name,
			char *payment_date_time,
			char *payor_full_name,
			char *payor_street_address,
			double donation_amount,
			double net_payment_amount,
			char *transaction_date_time,
			double merchant_fees_expense,
			char *paypal_date_time )
{
	fprintf(insert_pipe,
		"%s^%s^%s^%s^%.2lf^%.2lf^%s^.%.2lf^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		program_name_escape( program_name ),
		payment_date_time,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name( payor_full_name ),
		payor_street_address,
		donation_amount,
		net_payment_amount,
		(transaction_date_time)
			? transaction_date_time
			: "",
		merchant_fees_expense,
		(paypal_date_time)
			? paypal_date_time
			: "" );
}

PROGRAM_DONATION *program_donation_parse(
			char *input,
			boolean fetch_program,
			boolean fetch_paypal )
{
	char program_name[ 128 ];
	char payment_date_time[ 128 ];
	char payor_full_name[ 128 ];
	char payor_street_address[ 128 ];
	char donation_amount[ 128 ];
	char net_payment_amount[ 128 ];
	char transaction_date_time[ 128 ];
	char merchant_fees_expense[ 128 ];
	char paypal_date_time[ 128 ];
	PROGRAM_DONATION *program_donation;

	if ( !input || !*input ) return (PROGRAM_DONATION *)0;

	program_donation = program_donation_calloc();

	/* See: attribute_list program_donation */
	/* ----------------------------------- */
	piece( program_name, SQL_DELIMITER, input, 0 );
	program_donation->program = program_new( strdup( program_name ) );

	piece( payment_date_time, SQL_DELIMITER, input, 1 );
	program_donation->payment_date_time = strdup( payment_date_time );

	piece( payor_full_name, SQL_DELIMITER, input, 2 );
	program_donation->payor_full_name = strdup( payor_full_name );

	piece( payor_street_address, SQL_DELIMITER, input, 3 );
	program_donation->payor_street_address = strdup( payor_street_address );

	piece( donation_amount, SQL_DELIMITER, input, 4 );
	program_donation->donation_amount = atof( donation_amount );

	piece( net_payment_amount, SQL_DELIMITER, input, 5 );
	program_donation->net_payment_amount = atof( net_payment_amount );

	piece( transaction_date_time, SQL_DELIMITER, input, 6 );
	program_donation->transaction_date_time =
		strdup( transaction_date_time );

	piece( merchant_fees_expense, SQL_DELIMITER, input, 7 );
	program_donation->merchant_fees_expense = atof( merchant_fees_expense );

	piece( paypal_date_time, SQL_DELIMITER, input, 8 );
	program_donation->paypal_date_time = strdup( paypal_date_time );

	if ( fetch_program )
	{
		program_donation->program =
			program_fetch(
				program_donation->program->program_name,
				1 /* fetch_alias_list */ );
	}

	if ( fetch_paypal )
	{
		program_donation->paypal_deposit =
		    paypal_deposit_fetch(
			 program_donation->
				payor_full_name,
			 program_donation->
				payor_street_address,
			 program_donation->
				paypal_date_time );
	}

	return program_donation;
}

char *program_donation_primary_where(
			char *program_name,
			char *payment_date_time,
			char *payor_full_name,
			char *payor_street_address )
{
	char static where[ 1024 ];

	sprintf(where,
		"program_name = '%s' and		"
		"payment_date_time = '%s' and		"
		"payor_full_name = '%s' and		"
		"payor_street_address = '%s'		",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 program_name_escape( program_name ),
		 payment_date_time,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( payor_full_name ),
		 payor_street_address );

	return where;
}

TRANSACTION *program_donation_transaction(
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			char *program_name,
			double donation_amount,
			double merchant_fees_expense,
			double net_payment_amount,
			char *entity_self_paypal_cash_account_name,
			char *account_fees_expense,
			char *program_revenue_account )
{
	TRANSACTION *transaction;
	JOURNAL *journal;

	if ( !program_revenue_account )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: empty program_revenue_account.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (TRANSACTION *)0;
	}

	if ( dollar_virtually_same( donation_amount, 0.0 ) )
		return (TRANSACTION *)0;

	transaction =
		transaction_full(
			payor_full_name,
			payor_street_address,
			payment_date_time
				/* transaction_date_time */,
			donation_amount
				/* transaction_amount */,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			strdup( program_donation_memo( program_name ) ),
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

	journal->debit_amount = merchant_fees_expense;

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

	journal->credit_amount = donation_amount;

	return transaction;
}

FILE *program_donation_update_open( void )
{
	char sys_string[ 2048 ];

	sprintf( sys_string,
		 "update_statement table=%s key=%s carrot=y	|"
		 "tee_appaserver_error.sh 			|"
		 "sql						 ",
		 PROGRAM_DONATION_TABLE,
		 PROGRAM_DONATION_PRIMARY_KEY );

	return popen( sys_string, "w" );
}

void program_donation_update(
			double net_payment_amount,
			char *transaction_date_time,
			char *program_name,
			char *payment_date_time,
			char *payor_full_name,
			char *payor_street_address )
{
	FILE *update_pipe = program_donation_update_open();

	fprintf( update_pipe,
		 "%s^%s^%s^%s^net_payment_amount^%.2lf\n",
		 program_name,
		 payment_date_time,
		 payor_full_name,
		 payor_street_address,
		 net_payment_amount );

	fprintf( update_pipe,
		 "%s^%s^%s^%s^transaction_date_time^%s\n",
		 program_name,
		 payment_date_time,
		 payor_full_name,
		 payor_street_address,
		 (transaction_date_time)
			? transaction_date_time
			: "" );

	pclose( update_pipe );
}

char *program_donation_list_display( LIST *payment_list )
{
	char display[ 65536 ];
	char *ptr = display;
	PROGRAM_DONATION *payment;

	*ptr = '\0';

	if ( !list_rewind( payment_list ) )
	{
		return "";
	}

	ptr += sprintf( ptr, "Program payment: " );

	do {
		payment =
			list_get(
				payment_list );

		if ( !list_at_head( payment_list ) )
		{
			ptr += sprintf( ptr, ", " );
		}

		if ( payment && payment->program )
		{
			ptr += sprintf(	ptr,
					"%s",
					payment->program->program_name );
		}
		else
		{
			ptr += sprintf(	ptr,
					"%s",
					"Non existing program" );
		}

	} while ( list_next( payment_list ) );

	return strdup( display );
}

PROGRAM_DONATION *program_donation_steady_state(
			PROGRAM_DONATION *program_donation,
			double donation_amount,
			double merchant_fees_expense )
{
	if ( !program_donation->program )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty program.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !program_donation->program->revenue_account
	||   !*program_donation->program->revenue_account )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty revenue_account.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PROGRAM_DONATION *)0;
	}

	program_donation->net_payment_amount =
		education_net_payment_amount(
			donation_amount,
			merchant_fees_expense );

	return program_donation;
}

PROGRAM_DONATION *program_donation_paypal(
			PROGRAM *program,
			double item_value,
			double item_fee,
			char *paypal_date_time )
{
	PROGRAM_DONATION *program_donation;

	program_donation = program_donation_calloc();

	program_donation->donation_amount = item_value;
	program_donation->merchant_fees_expense = item_fee;
	program_donation->paypal_date_time = paypal_date_time;

	program_donation->net_payment_amount =
		education_net_payment_amount(
			item_value,
			item_fee );

	program_donation->program = program;

	return program_donation;
}

LIST *program_donation_list_paypal(
			LIST *paypal_item_list,
			LIST *program_list,
			char *paypal_date_time )
{
	LIST *payment_list;
	PROGRAM_DONATION *payment;
	PAYPAL_ITEM *paypal_item;
	PROGRAM *program;

	if ( !list_rewind( paypal_item_list ) ) return (LIST *)0;

	payment_list = list_new();

	do {
		paypal_item = list_get( paypal_item_list );

		if ( paypal_item->benefit_entity ) continue;

		if ( ( program =
			program_seek(
				paypal_item->item_data,
				program_list ) ) )
		{
			payment =
				program_donation_paypal(
					program,
					paypal_item->item_value,
					paypal_item->item_fee,
					paypal_date_time );

			list_set( payment_list, payment );
		}
	} while ( list_next( paypal_item_list ) );

	return payment_list;
}

void program_donation_trigger(
			char *program_name,
			char *payment_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *state )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
"program_donation_trigger \"%s\" '%s' \"%s\" '%s' '%s'",
		program_name,
		payment_date_time,
		payor_full_name,
		payor_street_address,
		state );

	if ( system( sys_string ) ){}
}

double program_donation_total( LIST *payment_list )
{
	PROGRAM_DONATION *payment;
	double total;

	if ( !list_rewind( payment_list ) ) return 0.0;

	total = 0.0;

	do {
		payment = list_get( payment_list );

		total += payment->donation_amount;

	} while ( list_next( payment_list ) );

	return total;
}

void program_donation_list_trigger(
			LIST *program_donation_list )
{
	PROGRAM_DONATION *program_donation;

	if ( !list_rewind( program_donation_list ) ) return;

	do {
		program_donation = list_get( program_donation_list );

		program_donation_trigger(
			program_donation->program->program_name,
			program_donation->
				payment_date_time,
			program_donation->
				payor_full_name,
			program_donation->
				payor_street_address,
			"insert" /* state */ );

	} while ( list_next( program_donation_list ) );
}

LIST *program_donation_transaction_list(
			LIST *program_donation_list )
{
	PROGRAM_DONATION *program_donation;
	LIST *transaction_list;

	if ( !list_rewind( program_donation_list ) ) return (LIST *)0;

	transaction_list = list_new();

	do {
		program_donation =
			list_get(
				program_donation_list );

		if ( program_donation->program_donation_transaction )
		{
			list_set(
				transaction_list,
				program_donation->
					program_donation_transaction );
		}

	} while ( list_next( program_donation_list ) );

	return transaction_list;
}

LIST *program_donation_list_steady_state(
			LIST *program_donation_list )
{
	PROGRAM_DONATION *program_donation;

	if ( !list_rewind( program_donation_list ) ) return (LIST *)0;

	do {
		program_donation = list_get( program_donation_list );

		program_donation =
			program_donation_steady_state(
				program_donation,
				program_donation->donation_amount,
				program_donation->merchant_fees_expense );

	} while( list_next( program_donation_list ) );

	return program_donation_list;
}

char *program_donation_memo( char *program_name )
{
	static char payment_memo[ 128 ];

	if ( program_name && *program_name )
	{
		sprintf(payment_memo,
			"%s/%s",
			PROGRAM_DONATION_MEMO,
			program_name );
	}
	else
	{
		sprintf(payment_memo,
			"%s Payment",
			PROGRAM_DONATION_MEMO );
	}
	return payment_memo;
}

void program_donation_list_payor_entity_insert(
			LIST *program_donation_list )
{
	PROGRAM_DONATION *program_donation;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( program_donation_list ) ) return;

	insert_pipe =
		entity_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		program_donation = list_get( program_donation_list );

		entity_insert_pipe(
			insert_pipe,
			program_donation->
				paypal_deposit->
				payor_entity->
				full_name,
			program_donation->
				paypal_deposit->
				payor_entity->
				street_address,
			program_donation->
				paypal_deposit->
				payor_entity->
				email_address );

	} while ( list_next( program_donation_list ) );

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

void program_donation_list_set_transaction(
			int *seconds_to_add,
			LIST *program_donation_list )
{
	PROGRAM_DONATION *program_donation;
	char *cash_account_name;
	char *revenue_account;
	char *fees_expense;

	if ( !list_rewind( program_donation_list ) ) return;

	cash_account_name = entity_self_paypal_cash_account_name();
	fees_expense = account_fees_expense( (char *)0 );

	do {
		program_donation = list_get( program_donation_list );

		revenue_account =
			program_donation->
				program->
				revenue_account;

		program_donation_set_transaction(
			seconds_to_add,
			program_donation,
			cash_account_name,
			fees_expense,
			revenue_account );

	} while ( list_next( program_donation_list ) );
}

void program_donation_set_transaction(
			int *seconds_to_add,
			PROGRAM_DONATION *program_donation,
			char *cash_account_name,
			char *account_fees_expense,
			char *revenue_account )
{
	if ( ( program_donation->program_donation_transaction =
	       program_donation_transaction(
			seconds_to_add,
			program_donation->
				payor_full_name,
			program_donation->
				payor_street_address,
			program_donation->
				payment_date_time,
			program_donation->program->program_name,
			program_donation->donation_amount,
			program_donation->merchant_fees_expense,
			program_donation->net_payment_amount,
			cash_account_name,
			account_fees_expense,
			revenue_account ) ) )
	{
		program_donation->transaction_date_time =
			program_donation->program_donation_transaction->
				transaction_date_time;
	}
	else
	{
		program_donation->transaction_date_time = (char *)0;
	}
}
