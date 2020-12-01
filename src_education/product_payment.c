/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/product_payment.c	*/
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
#include "product.h"
#include "transaction.h"
#include "journal.h"
#include "entity.h"
#include "account.h"
#include "deposit.h"
#include "program_payment_item_title.h"
#include "product_payment_fns.h"
#include "product_payment.h"

PRODUCT_PAYMENT *product_payment_calloc( void )
{
	PRODUCT_PAYMENT *product_payment;

	if ( ! ( product_payment = calloc( 1, sizeof( PRODUCT_PAYMENT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return product_payment;
}

PRODUCT_PAYMENT *product_payment_fetch(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_product,
			boolean fetch_deposit )
{
	PRODUCT_PAYMENT *product_payment;

	product_payment =
		product_payment_parse(
			pipe2string(
				product_payment_sys_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					product_payment_primary_where(
						product_name,
						payor_full_name,
						payor_street_address,
						season_name,
						year,
						deposit_date_time ) ) ),
			fetch_product,
			fetch_deposit );

	return product_payment;
}

LIST *product_payment_system_list(
			char *sys_string,
			boolean fetch_product,
			boolean fetch_deposit )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *product_payment_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			product_payment_list,
			product_payment_parse(
				input,
				fetch_product,
				fetch_deposit ) );
	}

	pclose( input_pipe );
	return product_payment_list;
}

char *product_payment_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 PRODUCT_PAYMENT_TABLE,
		 where );

	return strdup( sys_string );
}

void product_payment_list_insert( LIST *product_payment_list )
{
	PRODUCT_PAYMENT *product_payment;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];
	char *transaction_date_time;

	if ( !list_rewind( product_payment_list ) ) return;

	insert_pipe =
		product_payment_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		product_payment = list_get( product_payment_list );

		transaction_date_time =
			(product_payment->product_payment_transaction)
				? product_payment->
					product_payment_transaction->
					transaction_date_time
				: (char *)0;

		product_payment_insert_pipe(
			insert_pipe,
			product_payment->product->product_name,
			product_payment->deposit->payor_entity->full_name,
			product_payment->deposit->payor_entity->street_address,
			product_payment->deposit->semester->season_name,
			product_payment->deposit->semester->year,
			product_payment->deposit->deposit_date_time,
			product_payment->product_payment_amount,
			product_payment->fees_expense,
			product_payment->net_payment_amount,
			transaction_date_time );

	} while ( list_next( product_payment_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		char *title = "Insert Product Payment Errors";

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

FILE *product_payment_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement t=%s f=\"%s\" replace=%c delimiter='%c'|"
		"sql 2>&1						 |"
		"cat >%s 						  ",
		PRODUCT_PAYMENT_TABLE,
		PRODUCT_PAYMENT_INSERT_COLUMNS,
		'y',
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void product_payment_insert_pipe(
			FILE *insert_pipe,
			char *product_name,
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
		product_name_escape( product_name ),
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

PRODUCT_PAYMENT *product_payment_parse(
			char *input,
			boolean fetch_product,
			boolean fetch_deposit )
{
	char product_name[ 128 ];
	char payor_full_name[ 128 ];
	char payor_street_address[ 128 ];
	char season_name[ 128 ];
	char year[ 128 ];
	char deposit_date_time[ 128 ];
	char payment_amount[ 128 ];
	char fees_expense[ 128 ];
	char net_payment_amount[ 128 ];
	char transaction_date_time[ 128 ];
	PRODUCT_PAYMENT *product_payment;

	if ( !input || !*input ) return (PRODUCT_PAYMENT *)0;

	product_payment = product_payment_calloc();

	/* See: attribute_list product_payment */
	/* ----------------------------------- */
	piece( product_name, SQL_DELIMITER, input, 0 );
	product_payment->product = product_new( strdup( product_name ) );

	piece( payor_full_name, SQL_DELIMITER, input, 1 );
	piece( payor_street_address, SQL_DELIMITER, input, 2 );
	piece( season_name, SQL_DELIMITER, input, 3 );
	piece( year, SQL_DELIMITER, input, 4 );
	piece( deposit_date_time, SQL_DELIMITER, input, 5 );

	product_payment->deposit =
		deposit_new(
			strdup( payor_full_name ),
			strdup( payor_street_address ),
			strdup( season_name ),
			atoi( year ),
			strdup( deposit_date_time ) );

	piece( payment_amount, SQL_DELIMITER, input, 6 );
	product_payment->product_payment_amount =
		atof( payment_amount );

	piece( fees_expense, SQL_DELIMITER, input, 7 );
	product_payment->fees_expense = atof( fees_expense );

	piece( net_payment_amount, SQL_DELIMITER, input, 8 );
	product_payment->net_payment_amount = atof( net_payment_amount );

	piece( transaction_date_time, SQL_DELIMITER, input, 9 );
	product_payment->transaction_date_time =
		strdup( transaction_date_time );

	if ( fetch_product )
	{
		product_payment->product =
			product_fetch(
				product_payment->
					product->
					product_name );
	}

	if ( fetch_deposit )
	{
		product_payment->deposit =
		    deposit_fetch(
			 product_payment->deposit->payor_entity->full_name,
			 product_payment->deposit->payor_entity->street_address,
			 product_payment->deposit->semester->season_name,
			 product_payment->deposit->semester->year,
			 product_payment->deposit->deposit_date_time,
			 0 /* not fetch_tuition_payment_list */,
			 0 /* not fetch_program_payment_list */,
			 0 /* not fetch_product_payment_list */,
			 0 /* not fetch_tuition_refund_list */ );
	}

	return product_payment;
}

char *product_payment_primary_where(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time )
{
	char static where[ 1024 ];

	sprintf(where,
		"product_name = '%s' and		"
		"payor_full_name = '%s' and		"
		"payor_street_address = '%s' and	"
		"season_name = '%s' and			"
		"year = %d and				"
		"deposit_date_time = '%s'		",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 product_name_escape( product_name ),
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

TRANSACTION *product_payment_transaction(
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time,
			char *product_name,
			char *program_name,
			double payment_amount,
			double fees_expense,
			double net_payment_amount,
			char *entity_self_paypal_cash_account_name,
			char *account_fees_expense,
			char *product_revenue_account )
{
	TRANSACTION *transaction;
	JOURNAL *journal;

	if ( !product_revenue_account )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: empty product_revenue_account.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (TRANSACTION *)0;
	}

	if ( dollar_virtually_same( payment_amount, 0.0 ) )
		return (TRANSACTION *)0;

	transaction =
		transaction_full(
			payor_full_name,
			payor_street_address,
			deposit_date_time,
			payment_amount
				/* transaction_amount */,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			strdup( product_payment_memo( product_name ) ) );

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
				product_revenue_account ) ) );

	journal->credit_amount = payment_amount;

	return transaction;
}

FILE *product_payment_update_open( void )
{
	char sys_string[ 2048 ];

	sprintf( sys_string,
		 "update_statement table=%s key=%s carrot=y	|"
		 "tee_appaserver_error.sh 			|"
		 "sql						 ",
		 PRODUCT_PAYMENT_TABLE,
		 PRODUCT_PAYMENT_PRIMARY_KEY );

	return popen( sys_string, "w" );
}

void product_payment_update(
			char *transaction_date_time,
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time )
{
	FILE *update_pipe = product_payment_update_open();

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%d^%s^transaction_date_time^%s\n",
		 product_name,
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

char *product_payment_list_display( LIST *payment_list )
{
	char display[ 65536 ];
	char *ptr = display;
	PRODUCT_PAYMENT *payment;

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

		if ( payment && payment->product )
		{
			ptr += sprintf(	ptr,
					"%s",
					payment->product->product_name );
		}
		else
		{
			ptr += sprintf(	ptr,
					"%s",
					"Non existing product" );
		}

	} while ( list_next( payment_list ) );

	return strdup( display );
}

double product_payment_amount(
			double deposit_amount )
{
	return deposit_amount;
}

double product_payment_fees_expense(
			double deposit_fees_expense )
{
	return deposit_fees_expense;
}

double product_payment_net_payment_amount(
			double deposit_net_payment_amount )
{
	return deposit_net_payment_amount;
}

PRODUCT_PAYMENT *product_payment_steady_state(
			PRODUCT_PAYMENT *product_payment,
			double deposit_amount,
			double deposit_transaction_fee,
			double deposit_net_payment_amount )
{
	if ( !product_payment->product )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty product.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !product_payment->product->revenue_account
	||   !*product_payment->product->revenue_account )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty revenue_account.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PRODUCT_PAYMENT *)0;
	}

	product_payment->product_payment_amount =
		product_payment_amount(
			deposit_amount );

	product_payment->fees_expense =
		product_payment_fees_expense(
			deposit_transaction_fee );

	product_payment->net_payment_amount =
		product_payment_net_payment_amount(
			deposit_net_payment_amount );

	if ( !product_payment->transaction_date_time
	||   !*product_payment->transaction_date_time )
	{
		product_payment->transaction_date_time =
			product_payment->deposit->deposit_date_time;
	}

	if ( ( product_payment->product_payment_transaction =
	       product_payment_transaction(
			product_payment->deposit->payor_entity->full_name,
			product_payment->deposit->payor_entity->street_address,
			product_payment->transaction_date_time,
			product_payment->product->product_name,
			product_payment->product->program_name,
			product_payment->product_payment_amount,
			product_payment->fees_expense,
			product_payment->net_payment_amount,
			entity_self_paypal_cash_account_name(),
			account_fees_expense( (char *)0 ),
			product_payment->product->revenue_account ) ) )
	{
		product_payment->transaction_date_time =
			product_payment->product_payment_transaction->
				transaction_date_time;
	}
	else
	{
		product_payment->transaction_date_time = (char *)0;
	}

	return product_payment;
}

PRODUCT_PAYMENT *product_payment(
			char *item_title_P,
			int product_number,
			LIST *education_product_list,
			/* -------- */
			/* Set only */
			/* -------- */
			DEPOSIT *deposit )
{
	PRODUCT_PAYMENT *product_payment;
	PROGRAM_PAYMENT_ITEM_TITLE *payment_item_title;
	char *product_name;

	if ( ! ( payment_item_title =
			program_payment_item_title_new(
				item_title_P,
				product_number ) ) )
	{
		return (PRODUCT_PAYMENT *)0;
	}

	if ( ! ( product_name =
			product_payment_item_title_name(
				item_title_P,
				product_number,
				education_product_list ) ) )
	{
		return (PRODUCT_PAYMENT *)0;
	}

	/* New payment */
	/* ----------- */
	product_payment = product_payment_calloc();

	if ( ! ( product_payment->product =
			product_list_seek(
				education_product_list,
				product_name ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: product_list_seek(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			product_name );

		return (PRODUCT_PAYMENT *)0;
	}

	product_payment->program_payment_item_title = payment_item_title;
	product_payment->deposit = deposit;

	return product_payment;
}

LIST *product_payment_list(
			char *item_title_P,
			LIST *education_product_list,
			DEPOSIT *deposit )
{
	LIST *payment_list = list_new();
	PRODUCT_PAYMENT *payment;
	int product_number;

	for (	product_number = 1;
		( payment =
			product_payment(
				item_title_P,
				product_number,
				education_product_list,
				deposit ) );
		product_number++ )
	{
		list_set( payment_list, payment );
	}
	return payment_list;
}

void product_payment_trigger(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			char *state )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
"product_payment_trigger \"%s\" \"%s\" '%s' '%s' %d '%s' '%s'",
		product_name,
		payor_full_name,
		payor_street_address,
		season_name,
		year,
		deposit_date_time,
		state );

	if ( system( sys_string ) ){}
}

double product_payment_total( LIST *payment_list )
{
	PRODUCT_PAYMENT *payment;
	double total;

	if ( !list_rewind( payment_list ) ) return 0.0;

	total = 0.0;

	do {
		payment = list_get( payment_list );

		total += payment->product_payment_amount;

	} while ( list_next( payment_list ) );

	return total;
}

void product_payment_list_trigger(
			LIST *product_payment_list )
{
	PRODUCT_PAYMENT *product_payment;

	if ( !list_rewind( product_payment_list ) ) return;

	do {
		product_payment = list_get( product_payment_list );

		product_payment_trigger(
			product_payment->product->product_name,
			product_payment->
				deposit->
				payor_entity->
				full_name,
			product_payment->
				deposit->
				payor_entity->
				street_address,
			product_payment->deposit->semester->season_name,
			product_payment->deposit->semester->year,
			product_payment->
				deposit->
				deposit_date_time,
			"insert" /* state */ );

	} while ( list_next( product_payment_list ) );
}

LIST *product_payment_transaction_list(
			LIST *product_payment_list )
{
	PRODUCT_PAYMENT *product_payment;
	LIST *transaction_list;

	if ( !list_rewind( product_payment_list ) ) return (LIST *)0;

	transaction_list = list_new();

	do {
		product_payment =
			list_get(
				product_payment_list );

		if ( product_payment->product_payment_transaction )
		{
			list_set(
				transaction_list,
				product_payment->product_payment_transaction );
		}

	} while ( list_next( product_payment_list ) );

	return transaction_list;
}

LIST *product_payment_list_steady_state(
			LIST *deposit_product_payment_list,
			double deposit_amount,
			double transaction_fee,
			double net_payment_amount )
{
	PRODUCT_PAYMENT *product_payment;

	if ( !list_rewind( deposit_product_payment_list ) ) return (LIST *)0;

	do {
		product_payment = list_get( deposit_product_payment_list );

		product_payment =
			product_payment_steady_state(
				product_payment,
				deposit_amount,
				transaction_fee,
				net_payment_amount );

	} while( list_next( deposit_product_payment_list ) );

	return deposit_product_payment_list;
}

char *product_payment_memo( char *product_name )
{
	static char payment_memo[ 128 ];

	if ( product_name && *product_name )
	{
		sprintf(payment_memo,
			"%s/%s",
			PRODUCT_PAYMENT_MEMO,
			product_name );
	}
	else
	{
		sprintf(payment_memo,
			"%s Payment",
			PRODUCT_PAYMENT_MEMO );
	}
	return payment_memo;
}

void product_payment_list_payor_entity_insert(
			LIST *product_payment_list )
{
	PRODUCT_PAYMENT *product_payment;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( product_payment_list ) ) return;

	insert_pipe =
		entity_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		product_payment = list_get( product_payment_list );

		entity_insert_pipe(
			insert_pipe,
			product_payment->deposit->payor_entity->full_name,
			product_payment->deposit->payor_entity->street_address,
			product_payment->deposit->payor_entity->email_address );

	} while ( list_next( product_payment_list ) );

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

