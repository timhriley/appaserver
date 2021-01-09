/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/ticket_sale.c		*/
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
#include "ticket_sale.h"

TICKET_SALE *ticket_sale_calloc( void )
{
	TICKET_SALE *ticket_sale;

	if ( ! ( ticket_sale = calloc( 1, sizeof( TICKET_SALE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return ticket_sale;
}

TICKET_SALE *ticket_sale_new(
			char *event_name,
			char *event_date,
			char *event_time,
			char *sale_date_time,
			ENTITY *payor_entity )
{
	TICKET_SALE *ticket_sale = ticket_sale_calloc();

	ticket_sale->event =
		event_new(
			event_name,
			event_date,
			event_time );

	ticket_sale->sale_date_time = sale_date_time;
	ticket_sale->payor_entity = payor_entity;

	return ticket_sale;
}

TICKET_SALE *ticket_sale_fetch(
			char *event_name,
			char *event_date,
			char *event_time,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address,
			boolean fetch_event )
{
	TICKET_SALE *ticket_sale;

	ticket_sale =
		ticket_sale_parse(
			pipe2string(
				ticket_sale_sys_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					ticket_sale_primary_where(
						event_name,
						event_date,
						event_time,
						sale_date_time,
						payor_full_name,
						payor_street_address ) ) ),
			fetch_event );

	return ticket_sale;
}

LIST *ticket_sale_system_list(
			char *sys_string,
			boolean fetch_event )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *ticket_sale_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			ticket_sale_list,
			ticket_sale_parse(
				input,
				fetch_event ) );
	}

	pclose( input_pipe );
	return ticket_sale_list;
}

LIST *ticket_sale_list_fetch(
			char *where,
			boolean fetch_event )
{
	return ticket_sale_list( where, fetch_event );
}

LIST *ticket_sale_list(
			char *where,
			boolean fetch_event )
{
	return ticket_sale_system_list(
			ticket_sale_sys_string( where ),
			fetch_event );
}

char *ticket_sale_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 TICKET_SALE_TABLE,
		 where );

	return strdup( sys_string );
}

void ticket_sale_list_insert( LIST *ticket_sale_list )
{
	TICKET_SALE *ticket_sale;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( ticket_sale_list ) ) return;

	insert_pipe =
		ticket_sale_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		ticket_sale = list_get( ticket_sale_list );

		ticket_sale_insert_pipe(
			insert_pipe,
			ticket_sale->event->event_name,
			ticket_sale->event->event_date,
			ticket_sale->event->event_time,
			ticket_sale->sale_date_time,
			ticket_sale->payor_entity->full_name,
			ticket_sale->payor_entity->street_address,
			ticket_sale->quantity,
			ticket_sale->ticket_price,
			ticket_sale->extended_price,
			ticket_sale->net_payment_amount,
			ticket_sale->transaction_date_time,
			ticket_sale->merchant_fees_expense,
			ticket_sale->paypal_date_time );

	} while ( list_next( ticket_sale_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		char *title = "Insert Ticket Sale Errors";

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

FILE *ticket_sale_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement t=%s f=\"%s\" replace=%c delimiter='%c'|"
		"sql 2>&1						 |"
		"cat >%s 						  ",
		TICKET_SALE_TABLE,
		TICKET_SALE_INSERT_COLUMNS,
		'y',
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void ticket_sale_insert_pipe(
			FILE *insert_pipe,
			char *event_name,
			char *event_date,
			char *event_time,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address,
			int quantity,
			double ticket_price,
			double extended_price,
			double net_payment_amount,
			char *transaction_date_time,
			double merchant_fees_expense,
			char *paypal_date_time )
{
	fprintf(insert_pipe,
		"%s^%s^%s^%s^%s^%s^%d^%.2lf^%.2lf^%.2lf^%s^%.2lf^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		event_name_escape( event_name ),
		event_date,
		event_time,
		sale_date_time,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name( payor_full_name ),
		payor_street_address,
		quantity,
		ticket_price,
		extended_price,
		net_payment_amount,
		(transaction_date_time)
			? transaction_date_time
			: "",
		merchant_fees_expense,
		(paypal_date_time)
			? paypal_date_time
			: "" );
}

TICKET_SALE *ticket_sale_parse(
			char *input,
			boolean fetch_event )
{
	char event_name[ 128 ];
	char event_date[ 128 ];
	char event_time[ 128 ];
	char sale_date_time[ 128 ];
	char payor_full_name[ 128 ];
	char payor_street_address[ 128 ];
	char quantity[ 128 ];
	char ticket_price[ 128 ];
	char extended_price[ 128 ];
	char net_payment_amount[ 128 ];
	char transaction_date_time[ 128 ];
	char merchant_fees_expense[ 128 ];
	char paypal_date_time[ 128 ];
	TICKET_SALE *ticket_sale;

	if ( !input || !*input ) return (TICKET_SALE *)0;

	ticket_sale = ticket_sale_calloc();

	/* See: attribute_list ticket_sale */
	/* ----------------------------------- */
	piece( event_name, SQL_DELIMITER, input, 0 );
	piece( event_date, SQL_DELIMITER, input, 1 );
	piece( event_time, SQL_DELIMITER, input, 2 );

	ticket_sale->event =
		event_new(	strdup( event_name ),
				strdup( event_date ),
				strdup( event_time ) );

	piece( sale_date_time, SQL_DELIMITER, input, 3 );
	ticket_sale->sale_date_time = strdup( sale_date_time );

	piece( payor_full_name, SQL_DELIMITER, input, 4 );
	piece( payor_street_address, SQL_DELIMITER, input, 5 );

	ticket_sale->payor_entity =
		entity_new(
			strdup( payor_full_name ),
			strdup( payor_street_address ) );

	piece( quantity, SQL_DELIMITER, input, 6 );
	ticket_sale->quantity = atoi( quantity );

	piece( ticket_price, SQL_DELIMITER, input, 7 );
	ticket_sale->ticket_price = atof( ticket_price );

	piece( extended_price, SQL_DELIMITER, input, 8 );
	ticket_sale->extended_price = atof( extended_price );

	piece( net_payment_amount, SQL_DELIMITER, input, 9 );
	ticket_sale->net_payment_amount = atof( net_payment_amount );

	piece( transaction_date_time, SQL_DELIMITER, input, 10 );
	ticket_sale->transaction_date_time = strdup( transaction_date_time );

	piece( merchant_fees_expense, SQL_DELIMITER, input, 11 );
	ticket_sale->merchant_fees_expense = atof( merchant_fees_expense );

	piece( paypal_date_time, SQL_DELIMITER, input, 12 );
	ticket_sale->paypal_date_time = strdup( paypal_date_time );

	if ( fetch_event )
	{
		ticket_sale->event =
			event_fetch(
				ticket_sale->
					event->
					event_name,
				ticket_sale->
					event->
					event_date,
				ticket_sale->
					event->
					event_time,
				0 /* not fetch_program */,
				0 /* not fetch_venue */,
				0 /* not fetch_sale_list */,
				0 /* not fetch_refund_list */ );
	}

	return ticket_sale;
}

char *ticket_sale_primary_where(
			char *event_name,
			char *event_date,
			char *event_time,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address )
{
	char static where[ 1024 ];

	sprintf(where,
		"event_name = '%s' and			"
		"event_date = '%s' and			"
		"event_time = '%s' and			"
		"sale_date_time = '%s' and		"
		"payor_full_name = '%s' and		"
		"payor_street_address = '%s'		",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 event_name_escape( event_name ),
		 event_date,
		 event_time,
		 sale_date_time,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( payor_full_name ),
		 payor_street_address );

	return where;
}

TRANSACTION *ticket_sale_transaction(
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *sale_date_time,
			char *event_name,
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

	if ( !sale_date_time || !*sale_date_time )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: empty sale_date_time.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (TRANSACTION *)0;
	}

	if ( !revenue_account )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: empty product_revenue_account.\n",
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
			sale_date_time
				/* transaction_date_time */,
			extended_price
				/* transaction_amount */,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			strdup( ticket_sale_memo( event_name ) ),
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

FILE *ticket_sale_update_open( void )
{
	char sys_string[ 2048 ];

	sprintf( sys_string,
		 "update_statement table=%s key=%s carrot=y	|"
		 "tee_appaserver_error.sh 			|"
		 "sql						 ",
		 TICKET_SALE_TABLE,
		 TICKET_SALE_PRIMARY_KEY );

	return popen( sys_string, "w" );
}

void ticket_sale_update(
			double extended_price,
			double net_payment_amount,
			char *transaction_date_time,
			char *event_name,
			char *event_date,
			char *event_time,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address )
{
	FILE *update_pipe = ticket_sale_update_open();

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%s^%s^extended_price^%.2lf\n",
		 event_name,
		 event_date,
		 event_time,
		 sale_date_time,
		 payor_full_name,
		 payor_street_address,
		 extended_price );

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%s^%s^net_payment_amount^%.2lf\n",
		 event_name,
		 event_date,
		 event_time,
		 sale_date_time,
		 payor_full_name,
		 payor_street_address,
		 net_payment_amount );

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%s^%s^transaction_date_time^%s\n",
		 event_name,
		 event_date,
		 event_time,
		 sale_date_time,
		 payor_full_name,
		 payor_street_address,
		 (transaction_date_time)
			? transaction_date_time
			: "" );

	pclose( update_pipe );
}

char *ticket_sale_list_display( LIST *sale_list )
{
	char display[ 65536 ];
	char *ptr = display;
	TICKET_SALE *sale;

	*ptr = '\0';

	if ( !list_rewind( sale_list ) )
	{
		return "";
	}

	ptr += sprintf( ptr, "Ticket sale: " );

	do {
		sale =
			list_get(
				sale_list );

		if ( !list_at_head( sale_list ) )
		{
			ptr += sprintf( ptr, ", " );
		}

		if ( sale && sale->event )
		{
			ptr += sprintf(	ptr,
					"%s",
					sale->event->event_name );
		}
		else
		{
			ptr += sprintf(	ptr,
					"%s",
					"Non existing event" );
		}

	} while ( list_next( sale_list ) );

	ptr += sprintf( ptr, "; " );

	return strdup( display );
}

TICKET_SALE *ticket_sale_steady_state(
			TICKET_SALE *ticket_sale,
			int quantity,
			double ticket_price,
			double merchant_fees_expense )
{
	if ( !ticket_sale->event )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty event.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !ticket_sale->event->revenue_account
	||   !*ticket_sale->event->revenue_account )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty revenue_account.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (TICKET_SALE *)0;
	}

	ticket_sale->extended_price =
		(double)quantity * ticket_price;

	ticket_sale->net_payment_amount =
		education_net_payment_amount(
			ticket_sale->extended_price,
			merchant_fees_expense );

	return ticket_sale;
}

void ticket_sale_trigger(
			char *event_name,
			char *event_date,
			char *event_time,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *state )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
"ticket_sale_trigger \"%s\" '%s' '%s' '%s' \"%s\" '%s' '%s'",
		event_name,
		event_date,
		event_time,
		sale_date_time,
		payor_full_name,
		payor_street_address,
		state );

	if ( system( sys_string ) ){}
}

double ticket_sale_total( LIST *sale_list )
{
	TICKET_SALE *sale;
	double total;

	if ( !list_rewind( sale_list ) ) return 0.0;

	total = 0.0;

	do {
		sale = list_get( sale_list );

		total += sale->extended_price;

	} while ( list_next( sale_list ) );

	return total;
}

double ticket_sale_fee_total( LIST *ticket_sale_list )
{
	TICKET_SALE *ticket_sale;
	double fee_total;

	if ( !list_rewind( ticket_sale_list ) ) return 0.0;

	fee_total = 0.0;

	do {
		ticket_sale = list_get( ticket_sale_list );

		fee_total += ticket_sale->merchant_fees_expense;

	} while ( list_next( ticket_sale_list ) );

	return fee_total;
}

void ticket_sale_list_trigger(
			LIST *ticket_sale_list )
{
	TICKET_SALE *ticket_sale;

	if ( !list_rewind( ticket_sale_list ) ) return;

	do {
		ticket_sale = list_get( ticket_sale_list );

		ticket_sale_trigger(
			ticket_sale->event->event_name,
			ticket_sale->event->event_date,
			ticket_sale->event->event_time,
			ticket_sale->sale_date_time,
			ticket_sale->payor_entity->full_name,
			ticket_sale->payor_entity->street_address,
			"insert" /* state */ );

	} while ( list_next( ticket_sale_list ) );
}

LIST *ticket_sale_transaction_list(
			LIST *ticket_sale_list )
{
	TICKET_SALE *ticket_sale;
	LIST *transaction_list;

	if ( !list_rewind( ticket_sale_list ) ) return (LIST *)0;

	transaction_list = list_new();

	do {
		ticket_sale =
			list_get(
				ticket_sale_list );

		if ( ticket_sale->ticket_sale_transaction )
		{
			list_set(
				transaction_list,
				ticket_sale->ticket_sale_transaction );
		}

	} while ( list_next( ticket_sale_list ) );

	return transaction_list;
}

LIST *ticket_sale_list_steady_state(
			LIST *ticket_sale_list )
{
	TICKET_SALE *ticket_sale;

	if ( !list_rewind( ticket_sale_list ) ) return (LIST *)0;

	do {
		ticket_sale = list_get( ticket_sale_list );

		ticket_sale =
			ticket_sale_steady_state(
				ticket_sale,
				ticket_sale->quantity,
				ticket_sale->ticket_price,
				ticket_sale->merchant_fees_expense );

	} while( list_next( ticket_sale_list ) );

	return ticket_sale_list;
}

char *ticket_sale_memo( char *event_name )
{
	static char payment_memo[ 128 ];

	if ( event_name && *event_name )
	{
		sprintf(payment_memo,
			"%s/%s",
			TICKET_SALE_MEMO,
			event_name );
	}
	else
	{
		sprintf(payment_memo,
			"%s Payment",
			TICKET_SALE_MEMO );
	}
	return payment_memo;
}

void ticket_sale_list_payor_entity_insert(
			LIST *ticket_sale_list )
{
	TICKET_SALE *ticket_sale;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( ticket_sale_list ) ) return;

	insert_pipe =
		entity_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		ticket_sale = list_get( ticket_sale_list );

		entity_insert_pipe(
			insert_pipe,
			ticket_sale->payor_entity->full_name,
			ticket_sale->payor_entity->street_address,
			ticket_sale->payor_entity->email_address );

	} while ( list_next( ticket_sale_list ) );

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

void ticket_sale_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *ticket_sale_list )
{
	TICKET_SALE *ticket_sale;
	char *cash_account_name;
	char *revenue_account;
	char *fees_expense;

	if ( !list_rewind( ticket_sale_list ) ) return;

	cash_account_name = entity_self_paypal_cash_account_name();
	fees_expense = account_fees_expense( (char *)0 );

	do {
		ticket_sale = list_get( ticket_sale_list );

		revenue_account =
			ticket_sale->
				event->
				revenue_account;

		/* ------------------------------------------------------- */
		/* Sets ticket_sale->ticket_sale_transaction and
			ticket_sale->transaction_date_time		   */
		/* ------------------------------------------------------- */
		ticket_sale_set_transaction(
			transaction_seconds_to_add,
			ticket_sale,
			cash_account_name,
			fees_expense,
			revenue_account );

	} while ( list_next( ticket_sale_list ) );
}

void ticket_sale_set_transaction(
			int *seconds_to_add,
			TICKET_SALE *ticket_sale,
			char *cash_account_name,
			char *account_fees_expense,
			char *revenue_account )
{
	if ( ( ticket_sale->ticket_sale_transaction =
	       ticket_sale_transaction(
			seconds_to_add,
			ticket_sale->payor_entity->full_name,
			ticket_sale->payor_entity->street_address,
			ticket_sale->sale_date_time,
			ticket_sale->event->event_name,
			ticket_sale->event->program->program_name,
			ticket_sale->extended_price,
			ticket_sale->merchant_fees_expense,
			ticket_sale->net_payment_amount,
			cash_account_name,
			account_fees_expense,
			revenue_account ) ) )
	{
		ticket_sale->transaction_date_time =
			ticket_sale->ticket_sale_transaction->
				transaction_date_time;
	}
	else
	{
		ticket_sale->transaction_date_time = (char *)0;
	}
}

LIST *ticket_sale_list_paypal(
			ENTITY *payor_entity,
			char *paypal_date_time,
			LIST *paypal_item_list,
			LIST *semester_event_list )
{
	LIST *ticket_sale_list = {0};
	PAYPAL_ITEM *paypal_item;
	EVENT *event;

	if ( !list_rewind( paypal_item_list ) ) return (LIST *)0;

	do {
		paypal_item = list_get( paypal_item_list );

		if ( paypal_item->taken ) continue;

		if ( paypal_item->benefit_entity ) continue;

		if ( ( event =
			event_list_seek(
				paypal_item->item_data,
				semester_event_list ) ) )
		{
			if ( !ticket_sale_list )
				ticket_sale_list =
					list_new();

			list_set(
				ticket_sale_list,
				ticket_sale_paypal(
					payor_entity,
					paypal_date_time,
					paypal_item->item_value,
					paypal_item->item_fee,
					event,
					list_length( paypal_item_list ) ) );

			paypal_item->taken = 1;
		}
		else
		if ( ( event =
			event_paypal_label_seek(
				paypal_item->item_data,
				semester_event_list ) ) )
		{
			if ( !ticket_sale_list )
				ticket_sale_list =
					list_new();

			list_set(
				ticket_sale_list,
				ticket_sale_paypal(
					payor_entity,
					paypal_date_time,
					paypal_item->item_value,
					paypal_item->item_fee,
					event,
					list_length( paypal_item_list ) ) );

			paypal_item->taken = 1;
		}

	} while ( list_next( paypal_item_list ) );

	return ticket_sale_list;
}

TICKET_SALE *ticket_sale_paypal(
			ENTITY *payor_entity,
			char *paypal_date_time,
			double item_value,
			double item_fee,
			EVENT *event,
			int paypal_item_list_length )
{
	TICKET_SALE *ticket_sale;

	ticket_sale = ticket_sale_calloc();

	ticket_sale->payor_entity = payor_entity;

	ticket_sale->sale_date_time =
	ticket_sale->paypal_date_time = paypal_date_time;

	if ( paypal_item_list_length == 1 )
	{
		ticket_sale->quantity =
			(int)( item_value / event->ticket_price );
	}
	else
	{
		ticket_sale->quantity = 1;

	}

	ticket_sale->ticket_price = event->ticket_price;

	ticket_sale->extended_price = item_value;
	ticket_sale->merchant_fees_expense = item_fee;

	ticket_sale->net_payment_amount =
		education_net_payment_amount(
			ticket_sale->extended_price,
			ticket_sale->merchant_fees_expense );

	ticket_sale->event = event;
	return ticket_sale;
}

void ticket_sale_list_event_insert(
			LIST *ticket_sale_list )
{
	TICKET_SALE *ticket_sale;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( ticket_sale_list ) ) return;

	insert_pipe =
		event_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		ticket_sale = list_get( ticket_sale_list );

		event_insert_pipe(
			insert_pipe,
			ticket_sale->
				event->
				event_name,
			ticket_sale->
				event->
				event_date,
			ticket_sale->
				event->
				event_time );

	} while ( list_next( ticket_sale_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf(sys_string,
			"cat %s						|"
			"queue_top_bottom_lines.e 300			|"
			"html_table.e 'Insert Event Errors' '' '^'	 ",
			 error_filename );

		if ( system( sys_string ) ){}
	}

	sprintf( sys_string, "rm %s", error_filename );

	if ( system( sys_string ) ){};
}

