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
			char *program_name,
			char *event_date,
			char *event_time,
			ENTITY *payor_entity )
{
	TICKET_SALE *ticket_sale = ticket_sale_calloc();

	ticket_sale->program_name = program_name;
	ticket_sale->event_date = event_date;
	ticket_sale->event_time = event_time;
	ticket_sale->payor_entity = payor_entity;

	return ticket_sale;
}

TICKET_SALE *ticket_sale_fetch(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address,
			boolean fetch_event )
{
	return
		ticket_sale_parse(
			string_pipe_fetch(
				ticket_sale_system_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					ticket_sale_primary_where(
						program_name,
						event_date,
						event_time,
						payor_full_name,
						payor_street_address ) ) ),
		fetch_event );
}

LIST *ticket_sale_system_list(
			char *system_string,
			boolean fetch_event )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *ticket_sale_list = list_new();

	if ( !system_string ) return (LIST *)0;

	input_pipe = popen( system_string, "r" );

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

char *ticket_sale_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf( system_string,
		 "select.sh '*' %s \"%s\" select",
		 TICKET_SALE_TABLE,
		 where );

	return strdup( system_string );
}

void ticket_sale_list_insert( LIST *ticket_sale_list )
{
	TICKET_SALE *ticket_sale;
	FILE *insert_pipe;
	char *error_filename;
	char system_string[ 1024 ];

	if ( !list_rewind( ticket_sale_list ) ) return;

	insert_pipe =
		ticket_sale_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		ticket_sale = list_get( ticket_sale_list );

		ticket_sale_insert_pipe(
			insert_pipe,
			ticket_sale->program_name,
			ticket_sale->event_date,
			ticket_sale->event_time,
			ticket_sale->payor_entity->full_name,
			ticket_sale->payor_entity->street_address,
			ticket_sale->sale_date_time,
			ticket_sale->quantity,
			ticket_sale->ticket_price,
			ticket_sale->extended_price,
			ticket_sale->merchant_fees_expense,
			ticket_sale->net_payment_amount,
			ticket_sale->transaction_date_time,
			ticket_sale->paypal_date_time );

	} while ( list_next( ticket_sale_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		char *title = "Insert Ticket Sale Errors";

		sprintf(system_string,
			"cat %s						|"
			"queue_top_bottom_lines.e 300			|"
			"html_table.e '%s' '' '^'			 ",
			error_filename,
			title );

		if ( system( system_string ) ){}
	}

	sprintf( system_string, "rm %s", error_filename );

	if ( system( system_string ) ){};
}

FILE *ticket_sale_insert_open( char *error_filename )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"insert_statement t=%s f=\"%s\" replace=%c delimiter='%c'|"
		"sql 2>&1						 |"
		"cat >%s 						  ",
		TICKET_SALE_TABLE,
		TICKET_SALE_INSERT_COLUMNS,
		'n',
		SQL_DELIMITER,
		error_filename );

	return popen( system_string, "w" );
}

void ticket_sale_insert_pipe(
			FILE *insert_pipe,
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address,
			char *sale_date_time,
			int quantity,
			double ticket_price,
			double extended_price,
			double merchant_fees_expense,
			double net_payment_amount,
			char *transaction_date_time,
			char *paypal_date_time )
{
	fprintf(insert_pipe,
		"%s^%s^%s^%s^%s^%s^%d^%.2lf^%.2lf^%.2lf^%.2lf^%s^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		program_name_escape( program_name ),
		event_date,
		event_time,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name( payor_full_name ),
		payor_street_address,
		sale_date_time,
		quantity,
		ticket_price,
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

TICKET_SALE *ticket_sale_parse(
			char *input,
			boolean fetch_event )
{
	char program_name[ 128 ];
	char event_date[ 128 ];
	char event_time[ 128 ];
	char payor_full_name[ 128 ];
	char payor_street_address[ 128 ];
	char sale_date_time[ 128 ];
	char quantity[ 128 ];
	char ticket_price[ 128 ];
	char extended_price[ 128 ];
	char merchant_fees_expense[ 128 ];
	char net_payment_amount[ 128 ];
	char transaction_date_time[ 128 ];
	char paypal_date_time[ 128 ];
	TICKET_SALE *ticket_sale;

	if ( !input || !*input ) return (TICKET_SALE *)0;

	ticket_sale = ticket_sale_calloc();

	/* See: attribute_list ticket_sale */
	/* ----------------------------------- */
	piece( program_name, SQL_DELIMITER, input, 0 );
	ticket_sale->program_name = strdup( program_name );

	piece( event_date, SQL_DELIMITER, input, 1 );
	ticket_sale->event_date = strdup( event_date );

	piece( event_time, SQL_DELIMITER, input, 2 );
	ticket_sale->event_time = strdup( event_time );

	piece( payor_full_name, SQL_DELIMITER, input, 3 );
	piece( payor_street_address, SQL_DELIMITER, input, 4 );

	ticket_sale->payor_entity =
		entity_new(
			strdup( payor_full_name ),
			strdup( payor_street_address ) );

	piece( sale_date_time, SQL_DELIMITER, input, 5 );
	ticket_sale->sale_date_time = strdup( sale_date_time );

	piece( quantity, SQL_DELIMITER, input, 6 );
	ticket_sale->quantity = atoi( quantity );

	piece( ticket_price, SQL_DELIMITER, input, 7 );
	ticket_sale->ticket_price = atof( ticket_price );

	piece( extended_price, SQL_DELIMITER, input, 8 );
	ticket_sale->extended_price = atof( extended_price );

	piece( merchant_fees_expense, SQL_DELIMITER, input, 9 );
	ticket_sale->merchant_fees_expense = atof( merchant_fees_expense );

	piece( net_payment_amount, SQL_DELIMITER, input, 10 );
	ticket_sale->net_payment_amount = atof( net_payment_amount );

	piece( transaction_date_time, SQL_DELIMITER, input, 11 );
	ticket_sale->transaction_date_time = strdup( transaction_date_time );

	piece( paypal_date_time, SQL_DELIMITER, input, 12 );
	ticket_sale->paypal_date_time = strdup( paypal_date_time );

	if ( fetch_event )
	{
		ticket_sale->event =
			event_fetch(
				ticket_sale->program_name,
				ticket_sale->event_date,
				ticket_sale->event_time,
				0 /* not fetch_program */,
				0 /* not fetch_venue */ );
	}

	return ticket_sale;
}

char *ticket_sale_primary_where(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address )
{
	char static where[ 1024 ];

	sprintf(where,
		"program_name = '%s' and		"
		"event_date = '%s' and			"
		"event_time = '%s' and			"
		"payor_full_name = '%s' and		"
		"payor_street_address = '%s'		",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 program_name_escape( program_name ),
		 event_date,
		 event_time,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( payor_full_name ),
		 payor_street_address );

	return where;
}

char *ticket_sale_integrity_where(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address )
{
	char static where[ 1024 ];

	sprintf(where,
		"program_name = '%s' and		"
		"event_date = '%s' and			"
		"event_time = '%s' and			"
		"payor_full_name = '%s' and		"
		"payor_street_address = '%s'		",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 program_name_escape( program_name ),
		 event_date,
		 event_time,
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
			char *transaction_date_time,
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

	if ( !transaction_date_time || !*transaction_date_time )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: empty transaction_date_time.\n",
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
			transaction_date_time,
			extended_price
				/* transaction_amount */,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			strdup( ticket_sale_memo( program_name ) ),
			1 /* lock_transaction */,
			(*seconds_to_add)++ );

	transaction->program_name = program_name;
	transaction->journal_list = list_new();

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

FILE *ticket_sale_update_open( void )
{
	char system_string[ 2048 ];

	sprintf( system_string,
		 "update_statement table=%s key=%s carrot=y	|"
		 "tee_appaserver_error.sh 			|"
		 "sql						 ",
		 TICKET_SALE_TABLE,
		 TICKET_SALE_PRIMARY_KEY );

	return popen( system_string, "w" );
}

void ticket_sale_update(
			char *sale_date_time,
			double extended_price,
			double net_payment_amount,
			char *transaction_date_time,
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address )
{
	FILE *update_pipe = ticket_sale_update_open();

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%s^sale_date_time^%s\n",
		 program_name,
		 event_date,
		 event_time,
		 payor_full_name,
		 payor_street_address,
		 sale_date_time );

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%s^extended_price^%.2lf\n",
		 program_name,
		 event_date,
		 event_time,
		 payor_full_name,
		 payor_street_address,
		 extended_price );

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%s^net_payment_amount^%.2lf\n",
		 program_name,
		 event_date,
		 event_time,
		 payor_full_name,
		 payor_street_address,
		 net_payment_amount );

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%s^transaction_date_time^%s\n",
		 program_name,
		 event_date,
		 event_time,
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

		ptr += sprintf(	ptr,
				"%s",
				sale->program_name );

	} while ( list_next( sale_list ) );

	ptr += sprintf( ptr, "; " );

	return strdup( display );
}

TICKET_SALE *ticket_sale_steady_state(
			TICKET_SALE *ticket_sale,
			int quantity,
			double ticket_price,
			double merchant_fees_expense,
			char *sale_date_time )
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

	ticket_sale->extended_price =
		(double)quantity * ticket_price;

	ticket_sale->net_payment_amount =
		education_net_payment_amount(
			ticket_sale->extended_price,
			merchant_fees_expense );

	if ( !sale_date_time || !*sale_date_time )
	{
		ticket_sale->sale_date_time =
			date_now19( date_utc_offset() );
	}

	return ticket_sale;
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

char *ticket_sale_memo( char *program_name )
{
	static char payment_memo[ 128 ];

	sprintf(payment_memo,
		"%s/%s",
		TICKET_SALE_MEMO,
		program_name );

	return payment_memo;
}

void ticket_sale_list_payor_entity_insert(
			LIST *ticket_sale_list )
{
	TICKET_SALE *ticket_sale;
	FILE *insert_pipe;
	char *error_filename;
	char system_string[ 1024 ];

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
		sprintf(system_string,
			"cat %s						|"
			"queue_top_bottom_lines.e 300			|"
			"html_table.e 'Insert Entity Errors' '' '^'	 ",
			 error_filename );

		if ( system( system_string ) ){}
	}

	sprintf( system_string, "rm %s", error_filename );

	if ( system( system_string ) ){};
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
			ticket_sale->event->program_name,
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
			LIST *education_event_list,
			double paypal_amount )
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
				education_event_list ) ) )
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
					paypal_amount,
					list_length( paypal_item_list ) ) );

			paypal_item->taken = 1;
		}
		else
		if ( ( event =
			event_long_label_seek(
				paypal_item->item_data,
				education_event_list ) ) 
		||   ( event =
			event_short_label_seek(
				paypal_item->item_data,
				education_event_list ) ) )
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
					paypal_amount,
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
			double paypal_amount,
			int paypal_item_list_length )
{
	TICKET_SALE *ticket_sale;

	ticket_sale = ticket_sale_calloc();

	ticket_sale->program_name = event->program_name;
	ticket_sale->event_date = event->event_date;
	ticket_sale->event_time = event->event_time;
	ticket_sale->payor_entity = payor_entity;

	ticket_sale->sale_date_time =
	ticket_sale->paypal_date_time = paypal_date_time;

	ticket_sale->ticket_price = event->ticket_price;
	ticket_sale->merchant_fees_expense = item_fee;
	ticket_sale->event = event;

	if ( paypal_item_list_length == 1 )
	{
		ticket_sale->quantity =
			(int)( paypal_amount / event->ticket_price );

		ticket_sale->extended_price =
			ticket_sale->ticket_price *
			(double)ticket_sale->quantity;
	}
	else
	{
		ticket_sale->quantity = 1;
		ticket_sale->extended_price = item_value;
	}

	ticket_sale->net_payment_amount =
		education_net_payment_amount(
			ticket_sale->extended_price,
			ticket_sale->merchant_fees_expense );

	return ticket_sale;
}

TICKET_SALE *ticket_sale_integrity_fetch(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address )
{
	TICKET_SALE *ticket_sale;

	ticket_sale =
		ticket_sale_parse(
			pipe2string(
				ticket_sale_system_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					ticket_sale_integrity_where(
						program_name,
						event_date,
						event_time,
						payor_full_name,
						payor_street_address ) ) ),
		0 /* not fetch_event */ );

	return ticket_sale;
}

TICKET_SALE *ticket_sale_list_seek(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address,
			LIST *ticket_sale_list )
{
	TICKET_SALE *ticket_sale;

	if ( !list_rewind( ticket_sale_list ) )
		return (TICKET_SALE *)0;

	do {
		ticket_sale = list_get( ticket_sale_list );

		if ( strcmp(
			ticket_sale->program_name,
			program_name ) == 0
		&&   strcmp(
			ticket_sale->event_date,
			event_date ) == 0
		&&   strcmp(
			ticket_sale->event_time,
			event_time ) == 0
		&&   strcmp(
			ticket_sale->payor_entity->full_name,
			payor_full_name ) == 0
		&&   strcmp(
			ticket_sale->payor_entity->street_address,
			payor_street_address ) == 0 )
		{
			return ticket_sale;
		}
	} while ( list_next( ticket_sale_list ) );

	return (TICKET_SALE *)0;
}

boolean ticket_sale_list_any_exists(
			LIST *ticket_sale_list,
			LIST *existing_ticket_sale_list )
{
	TICKET_SALE *ticket_sale;

	if ( !list_rewind( ticket_sale_list ) ) return 0;

	do {
		ticket_sale = list_get( ticket_sale_list );

		if ( ticket_sale_list_seek(
			ticket_sale->program_name,
			ticket_sale->event_date,
			ticket_sale->event_time,
			ticket_sale->payor_entity->full_name,
			ticket_sale->payor_entity->street_address,
			existing_ticket_sale_list ) )
		{
			return 1;
		}
	} while ( list_next( ticket_sale_list ) );

	return 0;
}

double ticket_sale_total(
			LIST *ticket_sale_list )
{
	TICKET_SALE *ticket_sale;
	double total;

	if ( !list_rewind( ticket_sale_list ) ) return 0.0;

	total = 0.0;

	do {
		ticket_sale = list_get( ticket_sale_list );
		total += ticket_sale->extended_price;

	} while ( list_next( ticket_sale_list ) );

	return total;
}

double ticket_sale_fee_total(
			LIST *ticket_sale_list )
{
	TICKET_SALE *ticket_sale;
	double total;

	if ( !list_rewind( ticket_sale_list ) ) return 0.0;

	total = 0.0;

	do {
		ticket_sale = list_get( ticket_sale_list );
		total += ticket_sale->merchant_fees_expense;

	} while ( list_next( ticket_sale_list ) );

	return total;
}

