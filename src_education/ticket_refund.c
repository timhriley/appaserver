/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/ticket_refund.c	*/
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
#include "ticket_sale.h"
#include "education.h"
#include "ticket_refund.h"

TICKET_REFUND *ticket_refund_calloc( void )
{
	TICKET_REFUND *ticket_refund;

	if ( ! ( ticket_refund = calloc( 1, sizeof( TICKET_REFUND ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return ticket_refund;
}

TICKET_REFUND *ticket_refund_fetch(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address,
			boolean fetch_sale,
			boolean fetch_event,
			boolean fetch_transaction )
{
	return
		ticket_refund_parse(
			string_pipe_fetch(
				ticket_refund_system_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					ticket_refund_primary_where(
						program_name,
						event_date,
						event_time,
						payor_full_name,
						payor_street_address ) ) ),
			fetch_sale,
			fetch_event,
			fetch_transaction );
}

LIST *ticket_refund_system_list(
			char *system_string,
			boolean fetch_sale,
			boolean fetch_event,
			boolean fetch_transaction )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *ticket_refund_list = list_new();

	input_pipe = popen( system_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			ticket_refund_list,
			ticket_refund_parse(
				input,
				fetch_sale,
				fetch_event,
				fetch_transaction ) );
	}

	pclose( input_pipe );
	return ticket_refund_list;
}

char *ticket_refund_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf( system_string,
		 "select.sh '*' %s \"%s\" select",
		 TICKET_REFUND_TABLE,
		 where );

	return strdup( system_string );
}

void ticket_refund_list_insert( LIST *ticket_refund_list )
{
	TICKET_REFUND *ticket_refund;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( ticket_refund_list ) ) return;

	insert_pipe =
		ticket_refund_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		ticket_refund = list_get( ticket_refund_list );

		ticket_refund_insert_pipe(
			insert_pipe,
			ticket_refund->program_name,
			ticket_refund->event_date,
			ticket_refund->event_time,
			ticket_refund->payor_entity->full_name,
			ticket_refund->payor_entity->street_address,
			ticket_refund->refund_date_time,
			ticket_refund->extended_price,
			ticket_refund->refund_amount,
			ticket_refund->merchant_fees_expense,
			ticket_refund->net_refund_amount,
			ticket_refund->transaction_date_time,
			ticket_refund->paypal_date_time );

	} while ( list_next( ticket_refund_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		char *title = "Insert Ticket Refund Errors";

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

FILE *ticket_refund_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement t=%s f=\"%s\" replace=%c delimiter='%c'|"
		"sql 2>&1						 |"
		"cat >%s 						  ",
		TICKET_REFUND_TABLE,
		TICKET_REFUND_INSERT_COLUMNS,
		'n',
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void ticket_refund_insert_pipe(
			FILE *insert_pipe,
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time,
			double extended_price,
			double refund_amount,
			double merchant_fees_expense,
			double net_refund_amount,
			char *transaction_date_time,
			char *paypal_date_time )
{
	fprintf(insert_pipe,
		"%s^%s^%s^%s^%s^%s^%.2lf^%.2lf^%.2lf^%.2lf^%s^%s\n",
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
		refund_date_time,
		extended_price,
		refund_amount,
		merchant_fees_expense,
		net_refund_amount,
		(transaction_date_time)
			? transaction_date_time
			: "",
		(paypal_date_time)
			? paypal_date_time
			: "" );
}

TICKET_REFUND *ticket_refund_parse(
			char *input,
			boolean fetch_sale,
			boolean fetch_event,
			boolean fetch_transaction )
{
	char program_name[ 128 ];
	char event_date[ 128 ];
	char event_time[ 128 ];
	char payor_full_name[ 128 ];
	char payor_street_address[ 128 ];
	char refund_date_time[ 128 ];
	char extended_price[ 128 ];
	char refund_amount[ 128 ];
	char merchant_fees_expense[ 128 ];
	char net_refund_amount[ 128 ];
	char transaction_date_time[ 128 ];
	char paypal_date_time[ 128 ];
	TICKET_REFUND *ticket_refund;

	if ( !input || !*input ) return (TICKET_REFUND *)0;

	/* See: attribute_list ticket_refund */
	/* ----------------------------------- */
	piece( program_name, SQL_DELIMITER, input, 0 );
	piece( event_date, SQL_DELIMITER, input, 1 );
	piece( event_time, SQL_DELIMITER, input, 2 );
	piece( payor_full_name, SQL_DELIMITER, input, 3 );
	piece( payor_street_address, SQL_DELIMITER, input, 4 );

	ticket_refund =
		ticket_refund_new(
			strdup( program_name ),
			strdup( event_date ),
			strdup( event_time ),
			entity_new(
				strdup( payor_full_name ),
				strdup( payor_street_address ) ) );

	piece( refund_date_time, SQL_DELIMITER, input, 5 );
	ticket_refund->refund_date_time = strdup( refund_date_time );

	piece( extended_price, SQL_DELIMITER, input, 6 );
	ticket_refund->extended_price = atof( extended_price );

	piece( refund_amount, SQL_DELIMITER, input, 7 );
	ticket_refund->refund_amount = atof( refund_amount );

	piece( merchant_fees_expense, SQL_DELIMITER, input, 8 );
	ticket_refund->merchant_fees_expense = atof( merchant_fees_expense );

	piece( net_refund_amount, SQL_DELIMITER, input, 9 );
	ticket_refund->net_refund_amount = atof( net_refund_amount );

	piece( transaction_date_time, SQL_DELIMITER, input, 10 );
	ticket_refund->transaction_date_time = strdup( transaction_date_time );

	piece( paypal_date_time, SQL_DELIMITER, input, 11 );
	ticket_refund->paypal_date_time = strdup( paypal_date_time );

	if ( fetch_sale )
	{
		ticket_refund->ticket_sale =
			ticket_sale_fetch(
				ticket_refund->program_name,
				ticket_refund->event_date,
				ticket_refund->event_time,
				ticket_refund->payor_entity->full_name,
				ticket_refund->payor_entity->street_address,
				fetch_event,
				0 /* not fetch_transaction */ );
	}

	if ( fetch_transaction && *ticket_refund->transaction_date_time )
	{
		ticket_refund->ticket_refund_transaction =
			transaction_fetch(
				ticket_refund->payor_entity->full_name,
				ticket_refund->payor_entity->street_address,
				ticket_refund->transaction_date_time );
	}

	return ticket_refund;
}

char *ticket_refund_primary_where(
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
		"payor_street_address = '%s' 		",
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

TRANSACTION *ticket_refund_transaction(
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *transaction_date_time,
			char *program_name,
			double refund_amount,
			double merchant_fees_expense,
			double net_refund_amount,
			char *entity_self_paypal_cash_account_name,
			char *account_fees_expense,
			char *event_revenue_account )
{
	TRANSACTION *transaction;
	JOURNAL *journal;

	if ( !event_revenue_account )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: empty event_revenue_account.\n",
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
			transaction_date_time,
			refund_amount
				/* transaction_amount */,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			strdup( ticket_refund_memo( program_name ) ),
			1 /* lock_transaction */,
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
				event_revenue_account ) ) );

	journal->debit_amount = 0.0 - refund_amount;

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

FILE *ticket_refund_update_open( void )
{
	char sys_string[ 2048 ];

	sprintf( sys_string,
		 "update_statement table=%s key=%s carrot=y	|"
		 "tee_appaserver_error.sh 			|"
		 "sql						 ",
		 TICKET_REFUND_TABLE,
		 TICKET_REFUND_PRIMARY_KEY );

	return popen( sys_string, "w" );
}

void ticket_refund_update(
			char *refund_date_time,
			double net_refund_amount,
			char *transaction_date_time,
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address )
{
	FILE *update_pipe = ticket_refund_update_open();

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%s^refund_date_time^%s\n",
		 program_name,
		 event_date,
		 event_time,
		 payor_full_name,
		 payor_street_address,
		 refund_date_time );

	fprintf( update_pipe,
		 "%s^%s^%s^%s^%s^net_refund_amount^%.2lf\n",
		 program_name,
		 event_date,
		 event_time,
		 payor_full_name,
		 payor_street_address,
		 net_refund_amount );

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

char *ticket_refund_list_display( LIST *refund_list )
{
	char display[ 65536 ];
	char *ptr = display;
	TICKET_REFUND *refund;

	*ptr = '\0';

	if ( !list_rewind( refund_list ) )
	{
		return "";
	}

	ptr += sprintf( ptr, "Ticket refund: " );

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
				ticket_sale
		&&	refund->
				ticket_sale->
				event )
		{
			ptr += sprintf(	ptr,
					"%s",
					refund->
						ticket_sale->
						event->
						program_name );
		}
		else
		{
			ptr += sprintf(	ptr,
					"%s",
					"Non existing event" );
		}

	} while ( list_next( refund_list ) );

	ptr += sprintf( ptr, "; " );

	return strdup( display );
}

TICKET_REFUND *ticket_refund_steady_state(
			TICKET_REFUND *ticket_refund,
			double refund_amount,
			double merchant_fees_expense,
			char *refund_date_time )
{
	if ( !ticket_refund->ticket_sale
	||   !ticket_refund->ticket_sale->event )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty ticket_sale or event.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( refund_amount < 0.0 )
	{
		ticket_refund->refund_amount = 0.0 - refund_amount;
	}

	ticket_refund->net_refund_amount =
		education_net_refund_amount(
			ticket_refund->refund_amount,
			merchant_fees_expense );

	if ( !refund_date_time || !*refund_date_time )
	{
		ticket_refund->refund_date_time =
			date_now19( date_utc_offset() );
	}

	return ticket_refund;
}

double ticket_refund_total( LIST *refund_list )
{
	TICKET_REFUND *refund;
	double total;

	if ( !list_rewind( refund_list ) ) return 0.0;

	total = 0.0;

	do {
		refund = list_get( refund_list );

		total += refund->refund_amount;

	} while ( list_next( refund_list ) );

	return total;
}

LIST *ticket_refund_transaction_list(
			LIST *ticket_refund_list )
{
	TICKET_REFUND *ticket_refund;
	LIST *transaction_list;

	if ( !list_rewind( ticket_refund_list ) ) return (LIST *)0;

	transaction_list = list_new();

	do {
		ticket_refund =
			list_get(
				ticket_refund_list );

		if ( ticket_refund->ticket_refund_transaction )
		{
			list_set(
				transaction_list,
				ticket_refund->ticket_refund_transaction );
		}

	} while ( list_next( ticket_refund_list ) );

	return transaction_list;
}

LIST *ticket_refund_list_steady_state(
			LIST *ticket_refund_list,
			double refund_amount,
			double merchant_fees_expense )
{
	TICKET_REFUND *ticket_refund;

	if ( !list_rewind( ticket_refund_list ) ) return (LIST *)0;

	do {
		ticket_refund = list_get( ticket_refund_list );

		ticket_refund =
			ticket_refund_steady_state(
				ticket_refund,
				refund_amount,
				merchant_fees_expense,
				ticket_refund->refund_date_time );

	} while( list_next( ticket_refund_list ) );

	return ticket_refund_list;
}

char *ticket_refund_memo( char *program_name )
{
	static char memo[ 128 ];

	sprintf(memo,
		"%s/%s",
		TICKET_REFUND_MEMO,
		program_name );

	return memo;
}

void ticket_refund_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *ticket_refund_list )
{
	TICKET_REFUND *ticket_refund;
	char *cash_account_name;
	char *revenue_account;
	char *fees_expense;

	if ( !list_rewind( ticket_refund_list ) ) return;

	cash_account_name = entity_self_paypal_cash_account_name();
	fees_expense = account_fees_expense( (char *)0 );

	do {
		ticket_refund = list_get( ticket_refund_list );

		if ( !ticket_refund->ticket_sale
		||   !ticket_refund->ticket_sale->event )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: empty ticket_sale or event.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );

		}

		revenue_account =
			ticket_refund->
				ticket_sale->
				event->
				revenue_account;

		ticket_refund_set_transaction(
			transaction_seconds_to_add,
			ticket_refund,
			cash_account_name,
			fees_expense,
			revenue_account );

	} while ( list_next( ticket_refund_list ) );
}

void ticket_refund_set_transaction(
			int *transaction_seconds_to_add,
			TICKET_REFUND *ticket_refund,
			char *cash_account_name,
			char *account_fees_expense,
			char *revenue_account )
{
	if ( ( ticket_refund->ticket_refund_transaction =
		ticket_refund_transaction(
			transaction_seconds_to_add,
			ticket_refund->payor_entity->full_name,
			ticket_refund->payor_entity->street_address,
			ticket_refund->refund_date_time,
			ticket_refund->ticket_sale->event->program_name,
			ticket_refund->refund_amount,
			ticket_refund->merchant_fees_expense,
			ticket_refund->net_refund_amount,
			cash_account_name,
			account_fees_expense,
			revenue_account ) ) )
	{
		ticket_refund->transaction_date_time =
			ticket_refund->ticket_refund_transaction->
				transaction_date_time;
	}
	else
	{
		ticket_refund->transaction_date_time = (char *)0;
	}
}

LIST *ticket_refund_list_paypal(
			ENTITY *payor_entity,
			char *paypal_date_time,
			LIST *paypal_item_list,
			LIST *education_event_list )
{
	LIST *ticket_refund_list = {0};
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
			if ( !ticket_refund_list )
				ticket_refund_list =
					list_new();

			list_set(
				ticket_refund_list,
				ticket_refund_paypal(
					payor_entity,
					paypal_date_time,
					paypal_item->item_value,
					paypal_item->item_fee,
					event ) );

			paypal_item->taken = 1;
		}

	} while ( list_next( paypal_item_list ) );

	return ticket_refund_list;
}

TICKET_REFUND *ticket_refund_paypal(
			ENTITY *payor_entity,
			char *paypal_date_time,
			double item_value,
			double item_fee,
			EVENT *event )
{
	TICKET_REFUND *ticket_refund;

	ticket_refund = ticket_refund_calloc();

	if ( ! ( ticket_refund->ticket_sale =
			ticket_sale_integrity_fetch(
				event->program_name,
				event->event_date,
				event->event_time,
				payor_entity->full_name,
				payor_entity->street_address ) ) )
	{
		free( ticket_refund );
		return (TICKET_REFUND *)0;
	}

	ticket_refund->program_name = event->program_name;
	ticket_refund->event_date = event->event_date;
	ticket_refund->event_time = event->event_time;
	ticket_refund->payor_entity = payor_entity;

	ticket_refund->refund_date_time =
	ticket_refund->paypal_date_time = paypal_date_time;

	ticket_refund->refund_amount = item_value;
	ticket_refund->merchant_fees_expense = item_fee;

	ticket_refund->net_refund_amount =
		education_net_refund_amount(
			ticket_refund->refund_amount,
			ticket_refund->merchant_fees_expense );

	ticket_refund->ticket_sale->event = event;

	return ticket_refund;
}

TICKET_REFUND *ticket_refund_new(
			char *program_name,
			char *event_date,
			char *event_time,
			ENTITY *payor_entity )
{
	TICKET_REFUND *ticket_refund = ticket_refund_calloc();

	ticket_refund->program_name = program_name;
	ticket_refund->event_date = event_date;
	ticket_refund->event_time = event_time;
	ticket_refund->payor_entity = payor_entity;

	return ticket_refund;
}

TICKET_REFUND *ticket_refund_list_seek(
			char *program_name,
			char *event_date,
			char *event_time,
			char *payor_full_name,
			char *payor_street_address,
			LIST *ticket_refund_list )
{
	TICKET_REFUND *ticket_refund;

	if ( !list_rewind( ticket_refund_list ) )
		return (TICKET_REFUND *)0;

	do {
		ticket_refund = list_get( ticket_refund_list );

		if ( strcmp(
			ticket_refund->program_name,
			program_name ) == 0
		&&   strcmp(
			ticket_refund->event_date,
			event_date ) == 0
		&&   strcmp(
			ticket_refund->event_time,
			event_time ) == 0
		&&   strcmp(
			ticket_refund->payor_entity->full_name,
			payor_full_name ) == 0
		&&   strcmp(
			ticket_refund->payor_entity->street_address,
			payor_street_address ) == 0 )
		{
			return ticket_refund;
		}
	} while ( list_next( ticket_refund_list ) );

	return (TICKET_REFUND *)0;
}

boolean ticket_refund_list_any_exists(
			LIST *ticket_refund_list,
			LIST *existing_ticket_refund_list )
{
	TICKET_REFUND *ticket_refund;

	if ( !list_rewind( ticket_refund_list ) ) return 0;

	do {
		ticket_refund = list_get( ticket_refund_list );

		if ( ticket_refund_list_seek(
			ticket_refund->program_name,
			ticket_refund->event_date,
			ticket_refund->event_time,
			ticket_refund->payor_entity->full_name,
			ticket_refund->payor_entity->street_address,
			existing_ticket_refund_list ) )
		{
			return 1;
		}
	} while ( list_next( ticket_refund_list ) );

	return 0;
}

