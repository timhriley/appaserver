/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/product_refund.c	*/
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
#include "paypal_deposit.h"
#include "paypal_item.h"
#include "product_sale.h"
#include "education.h"
#include "product_refund.h"

PRODUCT_REFUND *product_refund_calloc( void )
{
	PRODUCT_REFUND *product_refund;

	if ( ! ( product_refund = calloc( 1, sizeof( PRODUCT_REFUND ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return product_refund;
}

PRODUCT_REFUND *product_refund_fetch(
			char *product_name,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time,
			boolean fetch_sale,
			boolean fetch_paypal )
{
	PRODUCT_REFUND *product_refund;

	product_refund =
		product_refund_parse(
			pipe2string(
				product_refund_sys_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					product_refund_primary_where(
						product_name,
						sale_date_time,
						payor_full_name,
						payor_street_address,
						refund_date_time ) ) ),
			fetch_sale,
			fetch_paypal );

	return product_refund;
}

LIST *product_refund_system_list(
			char *sys_string,
			boolean fetch_sale,
			boolean fetch_paypal )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *product_refund_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			product_refund_list,
			product_refund_parse(
				input,
				fetch_sale,
				fetch_paypal ) );
	}

	pclose( input_pipe );
	return product_refund_list;
}

char *product_refund_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 PRODUCT_REFUND_TABLE,
		 where );

	return strdup( sys_string );
}

void product_refund_list_insert( LIST *product_refund_list )
{
	PRODUCT_REFUND *product_refund;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( product_refund_list ) ) return;

	insert_pipe =
		product_refund_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		product_refund = list_get( product_refund_list );

		product_refund_insert_pipe(
			insert_pipe,
			product_refund->product_sale->product->product_name,
			product_refund->sale_date_time,
			product_refund->payor_full_name,
			product_refund->payor_street_address,
			product_refund->refund_date_time,
			product_refund->refund_amount,
			product_refund->net_refund_amount,
			product_refund->transaction_date_time,
			product_refund->merchant_fees_expense,
			product_refund->paypal_date_time );

	} while ( list_next( product_refund_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		char *title = "Insert Product Refund Errors";

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

FILE *product_refund_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement t=%s f=\"%s\" replace=%c delimiter='%c'|"
		"sql 2>&1						 |"
		"cat >%s 						  ",
		PRODUCT_REFUND_TABLE,
		PRODUCT_REFUND_INSERT_COLUMNS,
		'y',
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void product_refund_insert_pipe(
			FILE *insert_pipe,
			char *product_name,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time,
			double refund_amount,
			double net_refund_amount,
			char *transaction_date_time,
			double merchant_fees_expense,
			char *paypal_date_time )
{
	fprintf(insert_pipe,
		"%s^%s^%s^%s^%s^%.2lf^%.2lf^%s^%.2lf^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		product_name_escape( product_name ),
		sale_date_time,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name( payor_full_name ),
		payor_street_address,
		refund_date_time,
		refund_amount,
		net_refund_amount,
		(transaction_date_time)
			? transaction_date_time
			: "",
		merchant_fees_expense,
		(paypal_date_time)
			? paypal_date_time
			: "" );
}

PRODUCT_REFUND *product_refund_parse(
			char *input,
			boolean fetch_sale,
			boolean fetch_paypal )
{
	char product_name[ 128 ];
	char sale_date_time[ 128 ];
	char payor_full_name[ 128 ];
	char payor_street_address[ 128 ];
	char refund_date_time[ 128 ];
	char refund_amount[ 128 ];
	char net_refund_amount[ 128 ];
	char transaction_date_time[ 128 ];
	char merchant_fees_expense[ 128 ];
	char paypal_date_time[ 128 ];
	PRODUCT_REFUND *product_refund;

	if ( !input || !*input ) return (PRODUCT_REFUND *)0;

	product_refund = product_refund_calloc();

	/* See: attribute_list product_refund */
	/* ----------------------------------- */
	piece( product_name, SQL_DELIMITER, input, 0 );
	piece( sale_date_time, SQL_DELIMITER, input, 1 );
	piece( payor_full_name, SQL_DELIMITER, input, 2 );
	piece( payor_street_address, SQL_DELIMITER, input, 3 );

	product_refund->product_sale =
		product_sale_new(
			strdup( product_name ),
			strdup( sale_date_time ),
			strdup( payor_full_name ),
			strdup( payor_street_address ) );

	piece( refund_date_time, SQL_DELIMITER, input, 4 );
	product_refund->refund_date_time = strdup( refund_date_time );

	piece( refund_amount, SQL_DELIMITER, input, 5 );
	product_refund->refund_amount = atof( refund_amount );

	piece( net_refund_amount, SQL_DELIMITER, input, 6 );
	product_refund->net_refund_amount = atof( net_refund_amount );

	piece( transaction_date_time, SQL_DELIMITER, input, 7 );
	product_refund->transaction_date_time = strdup( transaction_date_time );

	piece( merchant_fees_expense, SQL_DELIMITER, input, 8 );
	product_refund->merchant_fees_expense = atof( merchant_fees_expense );

	piece( paypal_date_time, SQL_DELIMITER, input, 9 );
	product_refund->paypal_date_time = strdup( paypal_date_time );


	if ( fetch_sale )
	{
		product_refund->product_sale =
			product_sale_fetch(
				product_refund->
					product_sale->
					product->
					product_name,
				product_refund->sale_date_time,
				product_refund->payor_full_name,
				product_refund->payor_street_address,
				1 /* fetch_product */,
				0 /* not fetch_paypal */ );
	}

	if ( fetch_paypal )
	{
		product_refund->paypal_deposit =
		    paypal_deposit_fetch(
			 product_refund->payor_full_name,
			 product_refund->payor_street_address,
			 product_refund->paypal_date_time );
	}

	return product_refund;
}

char *product_refund_primary_where(
			char *product_name,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time )
{
	char static where[ 1024 ];

	sprintf(where,
		"product_name = '%s' and		"
		"sale_date_time = '%s' and		"
		"payor_full_name = '%s' and		"
		"payor_street_address = '%s' and	"
		"refund_date_time = '%s'		",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 product_name_escape( product_name ),
		 /* --------------------- */
		 sale_date_time,
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( payor_full_name ),
		 payor_street_address,
		 refund_date_time );

	return where;
}

TRANSACTION *product_refund_transaction(
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time,
			char *product_name,
			char *program_name,
			double refund_amount,
			double merchant_fees_expense,
			double net_refund_amount,
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

	if ( dollar_virtually_same( refund_amount, 0.0 ) )
		return (TRANSACTION *)0;

	transaction =
		transaction_full(
			payor_full_name,
			payor_street_address,
			refund_date_time
				/* transaction_date_time */,
			refund_amount
				/* transaction_amount */,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			strdup( product_refund_memo( product_name ) ),
			(*seconds_to_add)++ );

	transaction->program_name = program_name;

	if ( !transaction->journal_list )
	{
		transaction->journal_list = list_new();
	}

	/* Debit revenue */
	/* ------------- */
	list_set(
		transaction->journal_list,
		( journal =
			journal_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				product_revenue_account ) ) );

	journal->debit_amount = refund_amount;

	/* Credit account_cash */
	/* ------------------- */
	list_set(
		transaction->journal_list,
		( journal =
			journal_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				entity_self_paypal_cash_account_name ) ) );

	journal->credit_amount = 0.0 - net_refund_amount;

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

	journal->credit_amount = merchant_fees_expense;

	return transaction;
}

FILE *product_refund_update_open( void )
{
	char sys_string[ 2048 ];

	sprintf( sys_string,
		 "update_statement table=%s key=%s carrot=y	|"
		 "tee_appaserver_error.sh 			|"
		 "sql						 ",
		 PRODUCT_REFUND_TABLE,
		 PRODUCT_REFUND_PRIMARY_KEY );

	return popen( sys_string, "w" );
}

void product_refund_update(
			double net_refund_amount,
			char *transaction_date_time,
			char *product_name,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time )
{
	FILE *update_pipe = product_refund_update_open();

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%s^net_refund_amount^%.2lf\n",
		 product_name,
		 sale_date_time,
		 payor_full_name,
		 payor_street_address,
		 refund_date_time,
		 net_refund_amount );

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%s^transaction_date_time^%s\n",
		 product_name,
		 sale_date_time,
		 payor_full_name,
		 payor_street_address,
		 refund_date_time,
		 (transaction_date_time)
			? transaction_date_time
			: "" );

	pclose( update_pipe );
}

char *product_refund_list_display( LIST *refund_list )
{
	char display[ 65536 ];
	char *ptr = display;
	PRODUCT_REFUND *refund;

	*ptr = '\0';

	if ( !list_rewind( refund_list ) )
	{
		return "";
	}

	ptr += sprintf( ptr, "Product refund: " );

	do {
		refund =
			list_get(
				refund_list );

		if ( !list_at_head( refund_list ) )
		{
			ptr += sprintf( ptr, ", " );
		}

		if (	refund
		&&	refund->
				product_sale
		&&	refund->
				product_sale->
				product )
		{
			ptr += sprintf(	ptr,
					"%s",
					refund->
						product_sale->
						product->
						product_name );
		}
		else
		{
			ptr += sprintf(	ptr,
					"%s",
					"Non existing product" );
		}

	} while ( list_next( refund_list ) );

	return strdup( display );
}

double product_refund_amount(
			double deposit_amount )
{
	return 0.0 - deposit_amount;
}

PRODUCT_REFUND *product_refund_steady_state(
			PRODUCT_REFUND *product_refund,
			double refund_amount,
			double merchant_fees_expense )
{
	if ( !product_refund->product_sale
	||   !product_refund->product_sale->product )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty product_sale or product.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !product_refund->product_sale->product->revenue_account
	||   !*product_refund->product_sale->product->revenue_account )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty revenue_account.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PRODUCT_REFUND *)0;
	}

	product_refund->net_refund_amount =
		education_net_refund_amount(
			refund_amount,
			merchant_fees_expense );

	return product_refund;
}

PRODUCT_REFUND *product_refund_paypal(
			LIST *product_list,
			char *item_data,
			double item_value,
			double item_fee,
			char *paypal_date_time )
{
	PRODUCT_REFUND *product_refund;
	PRODUCT *product;

	if ( ! ( product =
			product_seek(
				item_data,
				product_list ) ) )
	{
		return (PRODUCT_REFUND *)0;
	}

	/* New refund */
	/* ---------- */
	product_refund = product_refund_calloc();

	product_refund->paypal_date_time = paypal_date_time;
	product_refund->refund_amount = item_value;
	product_refund->merchant_fees_expense = item_fee;

	product_refund->net_refund_amount =
		education_net_refund_amount(
			item_value,
			item_fee );

	product_refund->product_sale =
		product_sale_new(
			product->product_name,
			(char *)0 /* sale_date_time */,
			(char *)0 /* payor_full_name */,
			(char *)0 /* payor_street_address */ );

	product_refund->product_sale->product = product;

	return product_refund;
}

LIST *product_refund_list_paypal(
			LIST *paypal_item_list,
			LIST *product_list,
			char *paypal_date_time )
{
	LIST *refund_list = list_new();
	PAYPAL_ITEM *paypal_item;

	if ( !list_rewind( paypal_item_list ) ) return (LIST *)0;

	do {
		paypal_item = list_get( paypal_item_list );

		if ( paypal_item->benefit_entity ) continue;

		list_set(
			refund_list,
			product_refund_paypal(
				product_list,
				paypal_item->item_data,
				paypal_item->item_value,
				paypal_item->item_fee,
				paypal_date_time ) );

	} while ( list_next( paypal_item_list ) );

	return refund_list;
}

void product_refund_trigger(
			char *product_name,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time,
			char *state )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
"product_refund_trigger \"%s\" '%s' \"%s\" '%s' '%s' '%s'",
		product_name,
		sale_date_time,
		payor_full_name,
		payor_street_address,
		refund_date_time,
		state );

	if ( system( sys_string ) ){}
}

double product_refund_total( LIST *refund_list )
{
	PRODUCT_REFUND *refund;
	double total;

	if ( !list_rewind( refund_list ) ) return 0.0;

	total = 0.0;

	do {
		refund = list_get( refund_list );

		total += refund->refund_amount;

	} while ( list_next( refund_list ) );

	return total;
}

void product_refund_list_trigger(
			LIST *product_refund_list )
{
	PRODUCT_REFUND *product_refund;

	if ( !list_rewind( product_refund_list ) ) return;

	do {
		product_refund = list_get( product_refund_list );

		if ( !product_refund->product_sale
		||   !product_refund->product_sale->product )
		{
			fprintf(stderr,
		"Warning in %s/%s()/%d: empty product_sale or product.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			continue;
		}

		product_refund_trigger(
			product_refund->product_sale->product->product_name,
			product_refund->sale_date_time,
			product_refund->payor_full_name,
			product_refund->payor_street_address,
			product_refund->refund_date_time,
			"insert" /* state */ );

	} while ( list_next( product_refund_list ) );
}

LIST *product_refund_transaction_list(
			LIST *product_refund_list )
{
	PRODUCT_REFUND *product_refund;
	LIST *transaction_list;

	if ( !list_rewind( product_refund_list ) ) return (LIST *)0;

	transaction_list = list_new();

	do {
		product_refund =
			list_get(
				product_refund_list );

		if ( product_refund->product_refund_transaction )
		{
			list_set(
				transaction_list,
				product_refund->product_refund_transaction );
		}

	} while ( list_next( product_refund_list ) );

	return transaction_list;
}

LIST *product_refund_list_steady_state(
			LIST *product_refund_list,
			double refund_amount,
			double merchant_fees_expense )
{
	PRODUCT_REFUND *product_refund;

	if ( !list_rewind( product_refund_list ) ) return (LIST *)0;

	do {
		product_refund = list_get( product_refund_list );

		product_refund =
			product_refund_steady_state(
				product_refund,
				refund_amount,
				merchant_fees_expense );

	} while( list_next( product_refund_list ) );

	return product_refund_list;
}

char *product_refund_memo( char *product_name )
{
	static char payment_memo[ 128 ];

	if ( product_name && *product_name )
	{
		sprintf(payment_memo,
			"%s/%s",
			PRODUCT_REFUND_MEMO,
			product_name );
	}
	else
	{
		sprintf(payment_memo,
			"%s",
			PRODUCT_REFUND_MEMO );
	}
	return payment_memo;
}

void product_refund_list_payor_entity_insert(
			LIST *product_refund_list )
{
	PRODUCT_REFUND *product_refund;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( product_refund_list ) ) return;

	insert_pipe =
		entity_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		product_refund = list_get( product_refund_list );

		if ( !product_refund->paypal_deposit
		||   !product_refund->paypal_deposit->payor_entity )
		{
			fprintf(stderr,
	"Warning in %s/%s()/%d: empty paypal_deposit or empty payor_entity.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			return;
		}

		entity_insert_pipe(
			insert_pipe,
			product_refund->payor_full_name,
			product_refund->payor_street_address,
			product_refund->
				paypal_deposit->
				payor_entity->
				email_address );

	} while ( list_next( product_refund_list ) );

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

void product_refund_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *product_refund_list )
{
	PRODUCT_REFUND *product_refund;
	char *cash_account_name;
	char *revenue_account;
	char *fees_expense;

	if ( !list_rewind( product_refund_list ) ) return;

	cash_account_name = entity_self_paypal_cash_account_name();
	fees_expense = account_fees_expense( (char *)0 );

	do {
		product_refund = list_get( product_refund_list );

		revenue_account =
			product_refund->
				product_sale->
				product->
				revenue_account;

		product_refund_set_transaction(
			transaction_seconds_to_add,
			product_refund,
			cash_account_name,
			fees_expense,
			revenue_account );

	} while ( list_next( product_refund_list ) );
}

void product_refund_set_transaction(
			int *transaction_seconds_to_add,
			PRODUCT_REFUND *product_refund,
			char *cash_account_name,
			char *account_fees_expense,
			char *revenue_account )
{
	if ( ( product_refund->product_refund_transaction =
		product_refund_transaction(
			transaction_seconds_to_add,
			product_refund->payor_full_name,
			product_refund->payor_street_address,
			product_refund->refund_date_time,
			product_refund->product_sale->product->product_name,
			product_refund->product_sale->product->program_name,
			product_refund->refund_amount,
			product_refund->merchant_fees_expense,
			product_refund->net_refund_amount,
			cash_account_name,
			account_fees_expense,
			revenue_account ) ) )
	{
		product_refund->transaction_date_time =
			product_refund->product_refund_transaction->
				transaction_date_time;
	}
	else
	{
		product_refund->transaction_date_time = (char *)0;
	}
}
