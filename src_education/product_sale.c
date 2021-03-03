/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/product_sale.c	*/
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
#include "education.h"
#include "paypal_upload.h"
#include "product.h"
#include "transaction.h"
#include "journal.h"
#include "entity.h"
#include "account.h"
#include "paypal_item.h"
#include "product_sale.h"

PRODUCT_SALE *product_sale_calloc( void )
{
	PRODUCT_SALE *product_sale;

	if ( ! ( product_sale = calloc( 1, sizeof( PRODUCT_SALE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return product_sale;
}

PRODUCT_SALE *product_sale_new(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *sale_date_time )
{
	PRODUCT_SALE *product_sale = product_sale_calloc();

	product_sale->product = product_new( product_name );

	product_sale->payor_entity =
		entity_new(
			payor_full_name,
			payor_street_address );

	product_sale->sale_date_time = sale_date_time;
	return product_sale;
}

PRODUCT_SALE *product_sale_fetch(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *sale_date_time,
			boolean fetch_product,
			boolean fetch_program )
{
	return
		product_sale_parse(
			pipe2string(
				product_sale_system_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					product_sale_primary_where(
						product_name,
						payor_full_name,
						payor_street_address,
						sale_date_time ) ) ),
		fetch_product,
		fetch_program );
}

LIST *product_sale_system_list(
			char *system_string,
			boolean fetch_product,
			boolean fetch_program )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *product_sale_list = list_new();

	input_pipe = popen( system_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			product_sale_list,
			product_sale_parse(
				input,
				fetch_product,
				fetch_program ) );
	}

	pclose( input_pipe );
	return product_sale_list;
}

char *product_sale_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf( system_string,
		 "select.sh '*' %s \"%s\" select",
		 PRODUCT_SALE_TABLE,
		 where );

	return strdup( system_string );
}

void product_sale_list_insert(
			LIST *product_sale_list )
{
	PRODUCT_SALE *product_sale;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( product_sale_list ) ) return;

	insert_pipe =
		product_sale_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		product_sale = list_get( product_sale_list );

		if ( !product_sale->product
		||   !product_sale->payor_entity
		||   !product_sale->sale_date_time )
		{
			fprintf(stderr,
"Warning in %s/%s()/%d: empty product, payor_entity, or sale_date_time.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			continue;
		}

		product_sale_insert_pipe(
			insert_pipe,
			product_sale->product->product_name,
			product_sale->payor_entity->full_name,
			product_sale->payor_entity->street_address,
			product_sale->sale_date_time,
			product_sale->quantity,
			product_sale->retail_price,
			product_sale->extended_price,
			product_sale->merchant_fees_expense,
			product_sale->net_payment_amount,
			product_sale->transaction_date_time,
			product_sale->paypal_date_time );

	} while ( list_next( product_sale_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		char *title = "Insert Product Sale Errors";

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

FILE *product_sale_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement t=%s f=\"%s\" replace=%c delimiter='%c'|"
		"sql 2>&1						 |"
		"cat >%s 						  ",
		PRODUCT_SALE_TABLE,
		PRODUCT_SALE_INSERT_COLUMNS,
		'y',
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void product_sale_insert_pipe(
			FILE *insert_pipe,
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *sale_date_time,
			int quantity,
			double retail_price,
			double extended_price,
			double merchant_fees_expense,
			double net_payment_amount,
			char *transaction_date_time,
			char *paypal_date_time )
{
	fprintf(insert_pipe,
		"%s^%s^%s^%s^%d^%.2lf^%.2lf^%.2lf^%.2lf^%s^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		product_name_escape( product_name ),
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name( payor_full_name ),
		payor_street_address,
		sale_date_time,
		quantity,
		retail_price,
		extended_price,
		merchant_fees_expense,
		net_payment_amount,
		(transaction_date_time)
			? transaction_date_time
			: "",
		(paypal_date_time)
			? paypal_date_time
			: "" );
}

PRODUCT_SALE *product_sale_parse(
			char *input,
			boolean fetch_product,
			boolean fetch_program )
{
	char product_name[ 128 ];
	char payor_full_name[ 128 ];
	char payor_street_address[ 128 ];
	char sale_date_time[ 128 ];
	char quantity[ 128 ];
	char retail_price[ 128 ];
	char extended_price[ 128 ];
	char merchant_fees_expense[ 128 ];
	char net_payment_amount[ 128 ];
	char transaction_date_time[ 128 ];
	char paypal_date_time[ 128 ];
	PRODUCT_SALE *product_sale;

	if ( !input || !*input ) return (PRODUCT_SALE *)0;

	product_sale = product_sale_calloc();

	/* See: attribute_list product_sale */
	/* ----------------------------------- */
	piece( product_name, SQL_DELIMITER, input, 0 );
	product_sale->product_name = strdup( product_name );

	piece( payor_full_name, SQL_DELIMITER, input, 1 );
	piece( payor_street_address, SQL_DELIMITER, input, 2 );

	product_sale->payor_entity =
		entity_new(
			strdup( payor_full_name ),
			strdup( payor_street_address ) );

	piece( sale_date_time, SQL_DELIMITER, input, 3 );
	product_sale->sale_date_time = strdup( sale_date_time );

	piece( quantity, SQL_DELIMITER, input, 4 );
	product_sale->quantity = atoi( quantity );

	piece( retail_price, SQL_DELIMITER, input, 5 );
	product_sale->retail_price = atof( retail_price );

	piece( extended_price, SQL_DELIMITER, input, 6 );
	product_sale->extended_price = atof( extended_price );

	piece( merchant_fees_expense, SQL_DELIMITER, input, 7 );
	product_sale->merchant_fees_expense = atof( merchant_fees_expense );

	piece( net_payment_amount, SQL_DELIMITER, input, 8 );
	product_sale->net_payment_amount = atof( net_payment_amount );

	piece( transaction_date_time, SQL_DELIMITER, input, 9 );
	product_sale->transaction_date_time = strdup( transaction_date_time );

	piece( paypal_date_time, SQL_DELIMITER, input, 10 );
	product_sale->paypal_date_time = strdup( paypal_date_time );

	if ( fetch_product )
	{
		product_sale->product =
			product_fetch(
				product_sale->product_name,
				fetch_program );
	}

	return product_sale;
}

char *product_sale_primary_where(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *sale_date_time )
{
	char static where[ 1024 ];

	sprintf(where,
		"product_name = '%s' and		"
		"payor_full_name = '%s' and		"
		"payor_street_address = '%s' and	"
		"sale_date_time = '%s' 			",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 product_name_escape( product_name ),
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( payor_full_name ),
		 payor_street_address,
		 sale_date_time );

	return where;
}

TRANSACTION *product_sale_transaction(
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time,
			char *product_name,
			char *program_name,
			double extended_price,
			double merchant_fees_expense,
			double net_payment_amount,
			char *entity_self_paypal_cash_account_name,
			char *account_fees_expense,
			char *revenue_account )
{
	TRANSACTION *transaction;
	JOURNAL *journal;

	if ( !revenue_account )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: empty revenue_account.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (TRANSACTION *)0;
	}

	if ( dollar_virtually_same( extended_price, 0.0 ) )
		return (TRANSACTION *)0;

	transaction =
		transaction_full(
			payor_full_name,
			payor_street_address,
			transaction_date_time,
			extended_price
				/* transaction_amount */,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			strdup( product_sale_memo( product_name ) ),
			1 /* lock_transaction */,
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

	/* Debit fees_expense (maybe) */
	/* -------------------------- */
	list_set(
		transaction->journal_list,
		journal_merchant_fees_expense(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			merchant_fees_expense,
			account_fees_expense ) );

	/* Credit revenue */
	/* -------------- */
	list_set(
		transaction->journal_list,
		( journal =
			journal_new(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				revenue_account ) ) );

	journal->credit_amount = extended_price;

	return transaction;
}

FILE *product_sale_update_open( void )
{
	char sys_string[ 2048 ];

	sprintf( sys_string,
		 "update_statement table=%s key=%s carrot=y	|"
		 "tee_appaserver_error.sh 			|"
		 "sql						 ",
		 PRODUCT_SALE_TABLE,
		 PRODUCT_SALE_PRIMARY_KEY );

	return popen( sys_string, "w" );
}

void product_sale_update(
			double extended_price,
			double net_payment_amount,
			char *transaction_date_time,
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *sale_date_time )
{
	FILE *update_pipe = product_sale_update_open();

	fprintf( update_pipe,
		 "%s^%s^%s^%s^extended_price^%.2lf\n",
		 product_name,
		 payor_full_name,
		 payor_street_address,
		 sale_date_time,
		 extended_price );

	fprintf( update_pipe,
		 "%s^%s^%s^%s^net_payment_amount^%.2lf\n",
		 product_name,
		 payor_full_name,
		 payor_street_address,
		 sale_date_time,
		 net_payment_amount );

	fprintf( update_pipe,
		 "%s^%s^%s^%s^transaction_date_time^%s\n",
		 product_name,
		 payor_full_name,
		 payor_street_address,
		 sale_date_time,
		 (transaction_date_time)
			? transaction_date_time
			: "" );

	pclose( update_pipe );
}

char *product_sale_list_display( LIST *payment_list )
{
	char display[ 65536 ];
	char *ptr = display;
	PRODUCT_SALE *payment;

	*ptr = '\0';

	if ( !list_rewind( payment_list ) )
	{
		return "";
	}

	ptr += sprintf( ptr, "Product sale: " );

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

	ptr += sprintf( ptr, "; " );

	return strdup( display );
}

PRODUCT_SALE *product_sale_steady_state(
			PRODUCT_SALE *product_sale,
			int quantity,
			double retail_price,
			double merchant_fees_expense )
{
	if ( !product_sale->product )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty product.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !product_sale->product->revenue_account
	||   !*product_sale->product->revenue_account )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty revenue_account.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (PRODUCT_SALE *)0;
	}

	product_sale->quantity = quantity;
	product_sale->retail_price = retail_price;
	product_sale->merchant_fees_expense = merchant_fees_expense;

	if ( !product_sale->quantity )
		product_sale->quantity = 1;

	product_sale->extended_price =
		(double)product_sale->quantity *
		product_sale->retail_price;

	product_sale->net_payment_amount =
		education_net_payment_amount(
			product_sale->extended_price,
			product_sale->merchant_fees_expense );

	return product_sale;
}

void product_sale_trigger(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *sale_date_time,
			char *state )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
"product_sale_trigger \"%s\" \"%s\" '%s' '%s' '%s'",
		product_name,
		payor_full_name,
		payor_street_address,
		sale_date_time,
		state );

	if ( system( sys_string ) ){}
}

double product_sale_total( LIST *product_sale_list )
{
	PRODUCT_SALE *product_sale;
	double total;

	if ( !list_rewind( product_sale_list ) ) return 0.0;

	total = 0.0;

	do {
		product_sale = list_get( product_sale_list );

		total += product_sale->extended_price;

	} while ( list_next( product_sale_list ) );

	return total;
}

double product_sale_fee_total( LIST *product_sale_list )
{
	PRODUCT_SALE *product_sale;
	double fee_total;

	if ( !list_rewind( product_sale_list ) ) return 0.0;

	fee_total = 0.0;

	do {
		product_sale = list_get( product_sale_list );

		fee_total += product_sale->merchant_fees_expense;

	} while ( list_next( product_sale_list ) );

	return fee_total;
}

void product_sale_list_trigger(
			LIST *product_sale_list )
{
	PRODUCT_SALE *product_sale;

	if ( !list_rewind( product_sale_list ) ) return;

	do {
		product_sale = list_get( product_sale_list );

		product_sale_trigger(
			product_sale->product->product_name,
			product_sale->payor_entity->full_name,
			product_sale->payor_entity->street_address,
			product_sale->sale_date_time,
			"insert" /* state */ );

	} while ( list_next( product_sale_list ) );
}

LIST *product_sale_transaction_list(
			LIST *product_sale_list )
{
	PRODUCT_SALE *product_sale;
	LIST *transaction_list;

	if ( !list_rewind( product_sale_list ) ) return (LIST *)0;

	transaction_list = list_new();

	do {
		product_sale =
			list_get(
				product_sale_list );

		if ( product_sale->product_sale_transaction )
		{
			list_set(
				transaction_list,
				product_sale->product_sale_transaction );
		}

	} while ( list_next( product_sale_list ) );

	return transaction_list;
}

LIST *product_sale_list_steady_state(
			LIST *product_sale_list )
{
	PRODUCT_SALE *product_sale;

	if ( !list_rewind( product_sale_list ) ) return (LIST *)0;

	do {
		product_sale = list_get( product_sale_list );

		product_sale =
			product_sale_steady_state(
				product_sale,
				product_sale->quantity,
				product_sale->retail_price,
				product_sale->merchant_fees_expense );

	} while( list_next( product_sale_list ) );

	return product_sale_list;
}

char *product_sale_memo( char *product_name )
{
	static char payment_memo[ 128 ];

	if ( product_name && *product_name )
	{
		sprintf(payment_memo,
			"%s/%s",
			PRODUCT_SALE_MEMO,
			product_name );
	}
	else
	{
		sprintf(payment_memo,
			"%s Payment",
			PRODUCT_SALE_MEMO );
	}
	return payment_memo;
}

void product_sale_list_payor_entity_insert(
			LIST *product_sale_list )
{
	PRODUCT_SALE *product_sale;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( product_sale_list ) ) return;

	insert_pipe =
		entity_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		product_sale = list_get( product_sale_list );

		entity_insert_pipe(
			insert_pipe,
			product_sale->payor_entity->full_name,
			product_sale->payor_entity->street_address,
			product_sale->payor_entity->email_address );

	} while ( list_next( product_sale_list ) );

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

void product_sale_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *product_sale_list )
{
	PRODUCT_SALE *product_sale;
	char *cash_account_name;
	char *revenue_account;
	char *fees_expense;

	if ( !list_rewind( product_sale_list ) ) return;

	cash_account_name = account_cash( (char *)0 );
	fees_expense = account_fees_expense( (char *)0 );

	do {
		product_sale = list_get( product_sale_list );

		revenue_account =
			product_sale->
				product->
				revenue_account;

		/* ------------------------------------------------------- */
		/* Sets product_sale->product_sale_transaction and
			product_sale->transaction_date_time		   */
		/* ------------------------------------------------------- */
		product_sale_set_transaction(
			transaction_seconds_to_add,
			product_sale,
			cash_account_name,
			fees_expense,
			revenue_account );

	} while ( list_next( product_sale_list ) );
}

void product_sale_set_transaction(
			int *transaction_seconds_to_add,
			PRODUCT_SALE *product_sale,
			char *cash_account_name,
			char *account_fees_expense,
			char *revenue_account )
{
	if ( ( product_sale->product_sale_transaction =
	       product_sale_transaction(
			transaction_seconds_to_add,
			product_sale->payor_entity->full_name,
			product_sale->payor_entity->street_address,
			product_sale->sale_date_time,
			product_sale->product->product_name,
			product_sale->product->program->program_name,
			product_sale->extended_price,
			product_sale->merchant_fees_expense,
			product_sale->net_payment_amount,
			cash_account_name,
			account_fees_expense,
			revenue_account ) ) )
	{
		product_sale->transaction_date_time =
			product_sale->product_sale_transaction->
				transaction_date_time;
	}
	else
	{
		product_sale->transaction_date_time = (char *)0;
	}
}

LIST *product_sale_list_paypal(
			ENTITY *payor_entity,
			char *paypal_date_time,
			LIST *paypal_item_list,
			LIST *product_list )
{
	LIST *product_sale_list = {0};
	PAYPAL_ITEM *paypal_item;
	PRODUCT *product;

	if ( !list_rewind( paypal_item_list ) ) return (LIST *)0;

	do {
		paypal_item = list_get( paypal_item_list );

		if ( paypal_item->taken ) continue;

		if ( paypal_item->benefit_entity ) continue;

		if ( ( product =
			product_list_seek(
				paypal_item->item_data,
				product_list ) ) )
		{
			if ( !product_sale_list )
				product_sale_list =
					list_new();

			list_set(
				product_sale_list,
				product_sale_paypal(
					payor_entity,
					paypal_date_time,
					paypal_item->item_value,
					paypal_item->item_fee,
					product ) );

			paypal_item->taken = 1;
		}
	} while ( list_next( paypal_item_list ) );

	return product_sale_list;
}

PRODUCT_SALE *product_sale_paypal(
			ENTITY *payor_entity,
			char *paypal_date_time,
			double item_value,
			double item_fee,
			PRODUCT *product )
{
	PRODUCT_SALE *product_sale;

	product_sale = product_sale_calloc();

	product_sale->product_name = product->product_name;
	product_sale->payor_entity = payor_entity;

	product_sale->sale_date_time =
	product_sale->paypal_date_time = paypal_date_time;

	product_sale->quantity = 1;

	product_sale->retail_price =
	product_sale->extended_price = item_value;

	product_sale->merchant_fees_expense = item_fee;

	product_sale->net_payment_amount =
		education_net_payment_amount(
			product_sale->extended_price,
			product_sale->merchant_fees_expense );

	product_sale->product = product;

	return product_sale;
}

LIST *product_sale_product_name_list( LIST *sale_list )
{
	PRODUCT_SALE *product_sale;
	LIST *product_name_list;

	if ( !list_rewind( sale_list ) ) return (LIST *)0;

	product_name_list = list_new();

	do {

		product_sale = list_get( sale_list );

		list_set(
			product_name_list,
			product_sale->product->product_name );

	} while ( list_next( sale_list ) );

	return product_name_list;
}

PRODUCT_SALE *product_sale_integrity_fetch(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address )
{
	return
		product_sale_parse(
			string_pipe_fetch(
				product_sale_system_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					product_sale_integrity_where(
						product_name,
						payor_full_name,
						payor_street_address ) ) ),
		0 /* not fetch_product */,
		0 /* not fetch_program */ );
}

char *product_sale_integrity_where(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address )
{
	char static where[ 1024 ];

	sprintf(where,
		"product_name = '%s' and		"
		"payor_full_name = '%s' and		"
		"payor_street_address = '%s' 		",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 product_name_escape( product_name ),
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( payor_full_name ),
		 payor_street_address );

	return where;
}

PRODUCT_SALE *product_sale_list_seek(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *sale_date_time,
			LIST *product_sale_list )
{
	PRODUCT_SALE *product_sale;

	if ( !list_rewind( product_sale_list ) )
		return (PRODUCT_SALE *)0;

	do {
		product_sale = list_get( product_sale_list );

		if ( !product_sale->payor_entity )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty payor_entity.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( strcmp(
			product_sale->product_name,
			product_name ) == 0
		&&   strcmp(
			product_sale->payor_entity->full_name,
			payor_full_name ) == 0
		&&   strcmp(
			product_sale->payor_entity->street_address,
			payor_street_address ) == 0
		&&   strcmp(
			product_sale->sale_date_time,
			sale_date_time ) == 0 )
		{
			return product_sale;
		}
	} while ( list_next( product_sale_list ) );

	return (PRODUCT_SALE *)0;
}

boolean product_sale_list_exists(
			LIST *product_sale_list,
			LIST *existing_product_sale_list )
{
	PRODUCT_SALE *product_sale;

	if ( !list_rewind( product_sale_list ) ) return 0;

	do {
		product_sale = list_get( product_sale_list );

		if ( !product_sale->payor_entity )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty payor_entity.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( product_sale_list_seek(
			product_sale->product_name,
			product_sale->payor_entity->full_name,
			product_sale->payor_entity->street_address,
			product_sale->sale_date_time,
			existing_product_sale_list ) )
		{
			return 1;
		}
	} while ( list_next( product_sale_list ) );

	return 0;
}

