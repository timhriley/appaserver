/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/paypal_deposit.c	*/
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
#include "environ.h"
#include "date_convert.h"
#include "list.h"
#include "entity_self.h"
#include "transaction.h"
#include "paypal_upload.h"
#include "paypal_item.h"
#include "semester.h"
#include "education.h"
#include "registration.h"
#include "tuition_payment.h"
#include "ticket_sale.h"
#include "program_donation.h"
#include "product_sale.h"
#include "product_refund.h"
#include "tuition_refund.h"
#include "ticket_refund.h"
#include "paypal_sweep.h"
#include "enrollment.h"
#include "offering.h"
#include "course_drop.h"
#include "paypal_deposit.h"

double paypal_net_revenue(
			double paypal_amount,
			double transaction_fee )
{
	return paypal_amount + transaction_fee;
}


char *paypal_deposit_primary_where(
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time )
{
	static char where[ 512 ];

	sprintf( where,
		 "payor_full_name = '%s' and		"
		 "payor_street_address = '%s' and	"
		 "paypal_date_time = '%s'		",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( payor_full_name ),
		 payor_street_address,
		 paypal_date_time );

	return where;
}

PAYPAL_DEPOSIT *paypal_deposit_calloc( void )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( ! ( paypal_deposit = calloc( 1, sizeof( PAYPAL_DEPOSIT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return paypal_deposit;
}

PAYPAL_DEPOSIT *paypal_deposit_new(
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( ! ( paypal_deposit = calloc( 1, sizeof( PAYPAL_DEPOSIT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	paypal_deposit->payor_entity =
		entity_new(
			payor_full_name,
			payor_street_address );

	paypal_deposit->paypal_date_time = paypal_date_time;
	return paypal_deposit;
}

PAYPAL_DEPOSIT *paypal_deposit_parse(
			char *input )
{
	char payor_full_name[ 128 ];
	char payor_street_address[ 128 ];
	char paypal_date_time[ 128 ];
	char row_number[ 128 ];
	char season_name[ 128 ];
	char year[ 128 ];
	char paypal_amount[ 128 ];
	char transaction_fee[ 128 ];
	char net_revenue[ 128 ];
	char tuition_payment_total[ 128 ];
	char program_donation_total[ 128 ];
	char ticket_sale_total[ 128 ];
	char product_sale_total[ 128 ];
	char sweep_amount[ 128 ];
	char tuition_refund_total[ 128 ];
	char ticket_refund_total[ 128 ];
	char product_refund_total[ 128 ];
	char from_email_address[ 128 ];
	char account_balance[ 128 ];
	char transaction_ID[ 128 ];
	char invoice_number[ 128 ];
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !input || !*input ) return (PAYPAL_DEPOSIT *)0;

	/* See: attribute_list paypal_deposit */
	/* ---------------------------------- */
	piece( payor_full_name, SQL_DELIMITER, input, 0 );
	piece( payor_street_address, SQL_DELIMITER, input, 1 );
	piece( paypal_date_time, SQL_DELIMITER, input, 2 );

	paypal_deposit =
		paypal_deposit_new(
			strdup( payor_full_name ),
			strdup( payor_street_address ),
			strdup( paypal_date_time ) );

	piece( row_number, SQL_DELIMITER, input, 3 );
	paypal_deposit->row_number = atoi( row_number );

	piece( season_name, SQL_DELIMITER, input, 4 );
	piece( year, SQL_DELIMITER, input, 5 );

	paypal_deposit->semester =
		semester_new(
			strdup( season_name ),
			atoi( year ) );
	
	piece( paypal_amount, SQL_DELIMITER, input, 6 );
	paypal_deposit->paypal_amount = atof( paypal_amount );

	piece( transaction_fee, SQL_DELIMITER, input, 7 );
	paypal_deposit->transaction_fee = atof( transaction_fee );

	piece( net_revenue, SQL_DELIMITER, input, 8 );
	paypal_deposit->net_revenue = atof( net_revenue );

	piece( tuition_payment_total, SQL_DELIMITER, input, 9 );
	paypal_deposit->tuition_payment_total = atof( tuition_payment_total );

	piece( program_donation_total, SQL_DELIMITER, input, 8 );
	paypal_deposit->program_donation_total = atof( program_donation_total );

	piece( ticket_sale_total, SQL_DELIMITER, input, 8 );
	paypal_deposit->ticket_sale_total = atof( ticket_sale_total );

	piece( product_sale_total, SQL_DELIMITER, input, 8 );
	paypal_deposit->product_sale_total = atof( product_sale_total );

	piece( sweep_amount, SQL_DELIMITER, input, 8 );
	paypal_deposit->sweep_amount = atof( sweep_amount );

	piece( tuition_refund_total, SQL_DELIMITER, input, 8 );
	paypal_deposit->tuition_refund_total = atof( tuition_refund_total );

	piece( ticket_refund_total, SQL_DELIMITER, input, 8 );
	paypal_deposit->ticket_refund_total = atof( ticket_refund_total );

	piece( product_refund_total, SQL_DELIMITER, input, 8 );
	paypal_deposit->product_refund_total = atof( product_refund_total );

	piece( from_email_address, SQL_DELIMITER, input, 11 );
	paypal_deposit->from_email_address = strdup( from_email_address );

	piece( account_balance, SQL_DELIMITER, input, 8 );
	paypal_deposit->account_balance = atof( account_balance );

	piece( transaction_ID, SQL_DELIMITER, input, 9 );
	paypal_deposit->transaction_ID = strdup( transaction_ID );

	piece( invoice_number, SQL_DELIMITER, input, 10 );
	paypal_deposit->invoice_number = strdup( invoice_number );

	return paypal_deposit;
}

char *paypal_deposit_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"select.sh '*' %s \"%s\" select",
		PAYPAL_DEPOSIT_TABLE,
		where );

	return strdup( sys_string );
}

LIST *paypal_deposit_system_list(
			char *sys_string )
{
	char input[ 1024 ];
	FILE *input_pipe = popen( sys_string, "r" );
	LIST *list = list_new();

	while( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			list,
			paypal_deposit_parse(
				input ) );
	}
	pclose( input_pipe );
	return list;
}

PAYPAL_DEPOSIT *paypal_deposit_fetch(
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	paypal_deposit =
		paypal_deposit_parse(
			pipe2string(
				/* -------------------------- */
				/* Safely returns heap memory */
				/* -------------------------- */
				paypal_deposit_sys_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					paypal_deposit_primary_where(
						payor_full_name,
						payor_street_address,
						paypal_date_time ) ) ) );
	return paypal_deposit;
}

void paypal_deposit_set_transaction(
			PAYPAL_DEPOSIT *paypal_deposit )
{
	int transaction_seconds_to_add = 0;

	if ( list_length( paypal_deposit->registration_list ) )
	{
		enrollment_list_set_transaction(
			&transaction_seconds_to_add,
			registration_list_enrollment_list(
				paypal_deposit->registration_list ) );

		course_drop_list_set_transaction(
			&transaction_seconds_to_add,
			enrollment_list_course_drop_list(
				registration_list_enrollment_list(
					paypal_deposit->registration_list ) ) );

		tuition_payment_list_set_transaction(
			&transaction_seconds_to_add,
			registration_list_tuition_payment_list(
				paypal_deposit->registration_list ) );

		tuition_refund_list_set_transaction(
			&transaction_seconds_to_add,
			registration_list_tuition_refund_list(
				paypal_deposit->registration_list ) );
	}

	if ( list_length( paypal_deposit->ticket_sale_list ) )
	{
		ticket_sale_list_set_transaction(
			&transaction_seconds_to_add,
			paypal_deposit->ticket_sale_list );
	}

	if ( list_length( paypal_deposit->product_sale_list ) )
	{
		product_sale_list_set_transaction(
			&transaction_seconds_to_add,
			paypal_deposit->product_sale_list );
	}

	if ( list_length( paypal_deposit->ticket_refund_list ) )
	{
		ticket_refund_list_set_transaction(
			&transaction_seconds_to_add,
			paypal_deposit->ticket_refund_list );
	}

	if ( list_length( paypal_deposit->product_refund_list ) )
	{
		product_refund_list_set_transaction(
			&transaction_seconds_to_add,
			paypal_deposit->product_refund_list );
	}

	if ( list_length( paypal_deposit->program_donation_list ) )
	{
		program_donation_list_set_transaction(
			&transaction_seconds_to_add,
			paypal_deposit->program_donation_list );
	}

	if ( paypal_deposit->paypal_sweep )
	{
		paypal_sweep_set_transaction(
			&transaction_seconds_to_add,
			paypal_deposit->paypal_sweep );
	}
}

PAYPAL_DEPOSIT *paypal_deposit_steady_state(
			PAYPAL_DEPOSIT *paypal_deposit )
{
	if ( list_length( paypal_deposit->tuition_payment_list ) )
	{
		paypal_deposit->tuition_payment_total =
			tuition_payment_total(
				paypal_deposit->tuition_payment_list );
	}

	if ( list_length( paypal_deposit->program_donation_list ) )
	{
		paypal_deposit->program_donation_total =
			program_donation_total(
				paypal_deposit->program_donation_list );
	}

	if ( list_length( paypal_deposit->product_sale_list ) )
	{
		paypal_deposit->product_sale_total =
			product_sale_total(
				paypal_deposit->product_sale_list );
	}

	if ( list_length( paypal_deposit->ticket_sale_list ) )
	{
		paypal_deposit->ticket_sale_total =
			ticket_sale_total(
				paypal_deposit->ticket_sale_list );
	}

	if ( list_length( paypal_deposit->tuition_refund_list ) )
	{
		paypal_deposit->tuition_refund_total =
			tuition_refund_total(
				paypal_deposit->tuition_refund_list );
	}

	if ( list_length( paypal_deposit->product_refund_list ) )
	{
		paypal_deposit->product_refund_total =
			product_refund_total(
				paypal_deposit->product_refund_list );
	}

	if ( list_length( paypal_deposit->ticket_refund_list ) )
	{
		paypal_deposit->ticket_refund_total =
			product_refund_total(
				paypal_deposit->ticket_refund_list );
	}

	paypal_deposit->net_revenue =
		paypal_net_revenue(
			paypal_deposit->paypal_amount,
			paypal_deposit->transaction_fee );

	return paypal_deposit;
}

FILE *paypal_deposit_update_open( void )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "update_statement table=%s key=%s carrot=y	|"
		 "tee_appaserver_error.sh			|"
		 "sql						 ",
		 PAYPAL_DEPOSIT_TABLE,
		 PAYPAL_PRIMARY_KEY );

	return popen( sys_string, "w" );
}

void paypal_deposit_update(
			double paypal_deposit_tuition_payment_total,
			double paypal_deposit_program_donation_total,
			double paypal_deposit_net_revenue,
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time )
{
	FILE *update_pipe;

	update_pipe = paypal_deposit_update_open();

	fprintf( update_pipe,
		 "%s^%s^%s^tuition_payment_total^%.2lf\n",
		 payor_full_name,
		 payor_street_address,
		 paypal_date_time,
		 paypal_deposit_tuition_payment_total );

	fprintf( update_pipe,
		 "%s^%s^%s^program_donation_total^%.2lf\n",
		 payor_full_name,
		 payor_street_address,
		 paypal_date_time,
		 paypal_deposit_program_donation_total );

	fprintf( update_pipe,
		 "%s^%s^%s^net_revenue^%.2lf\n",
		 payor_full_name,
		 payor_street_address,
		 paypal_date_time,
		 paypal_deposit_net_revenue );

	pclose( update_pipe );
}

LIST *paypal_deposit_course_name_list(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;
	LIST *course_name_list;

	if ( !list_rewind( paypal_deposit_list ) ) return (LIST *)0;

	course_name_list = list_new();

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		list_unique_list(
			course_name_list,
			registration_course_name_list(
				tuition_payment_registration_list(
					paypal_deposit->
					     tuition_payment_list ) ) );

	} while ( list_next( paypal_deposit_list ) );

	return course_name_list;
}

LIST *paypal_deposit_list_transaction_list(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;
	LIST *transaction_list;

	if ( !list_rewind( paypal_deposit_list ) ) return (LIST *)0;

	transaction_list = list_new();

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		if ( !paypal_deposit->exclude_existing_transaction )
		{
			list_append_list(
				transaction_list,
				paypal_deposit_transaction_list(
					paypal_deposit->tuition_payment_list,
					paypal_deposit->product_sale_list,
					paypal_deposit->ticket_sale_list,
					paypal_deposit->tuition_refund_list,
					paypal_deposit->product_refund_list,
					paypal_deposit->ticket_refund_list,
					paypal_deposit->program_donation_list,
					paypal_deposit->paypal_sweep ) );
		}
	} while ( list_next( paypal_deposit_list ) );
	return transaction_list;
}

LIST *paypal_deposit_list_set_transaction(
			LIST *paypal_deposit_list,
			/* ------------------------------------ */
			/* To set program_name for 		*/
			/* tuition payment and tuition refund	*/
			/* ------------------------------------ */
			LIST *semester_offering_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) )
		return paypal_deposit_list;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		if ( !paypal_deposit->exclude_existing_transaction )
		{
			paypal_deposit_set_transaction(
				paypal_deposit );
		}

	} while ( list_next( paypal_deposit_list ) );

	return paypal_deposit_list;
}

LIST *paypal_deposit_list_steady_state(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return paypal_deposit_list;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		paypal_deposit =
			paypal_deposit_steady_state(
				paypal_deposit );

	} while ( list_next( paypal_deposit_list ) );

	return paypal_deposit_list;
}

LIST *paypal_deposit_transaction_list(
			LIST *tuition_payment_list,
			LIST *product_sale_list,
			LIST *ticket_sale_list,
			LIST *tuition_refund_list,
			LIST *product_refund_list,
			LIST *ticket_refund_list,
			LIST *program_donation_list,
			PAYPAL_SWEEP *paypal_sweep )
{
	LIST *transaction_list;

	transaction_list = list_new();

	list_append_list(
		transaction_list,
		tuition_payment_transaction_list(
			tuition_payment_list ) );

	list_append_list(
		transaction_list,
		product_sale_transaction_list(
			product_sale_list ) );

	list_append_list(
		transaction_list,
		ticket_sale_transaction_list(
			ticket_sale_list ) );

	list_append_list(
		transaction_list,
		tuition_refund_transaction_list(
			tuition_refund_list ) );

	list_append_list(
		transaction_list,
		product_refund_transaction_list(
			product_refund_list ) );

	list_append_list(
		transaction_list,
		ticket_refund_transaction_list(
			ticket_refund_list ) );

	list_append_list(
		transaction_list,
		program_donation_transaction_list(
			program_donation_list ) );

	list_append_list(
		transaction_list,
		paypal_sweep_transaction_list(
			paypal_sweep ) );

	return transaction_list;
}

void paypal_deposit_set_expected_revenue(
			LIST *paypal_item_list,
			LIST *semester_offering_list,
			LIST *product_list,
			LIST *event_list )
{
	PAYPAL_ITEM *paypal_item;
	OFFERING *offering;
	PRODUCT *product;
	EVENT *event;

	if ( !list_rewind( paypal_item_list ) ) return;

	do {
		paypal_item = list_get( paypal_item_list );

		if ( ( offering =
			offering_course_name_seek(
				paypal_item->item_data,
				semester_offering_list ) ) )
		{
			paypal_item->expected_revenue = offering->course_price;
		}
		else
		if ( ( product =
			product_name_seek(
				paypal_item->item_data,
				product_list ) ) )
		{
			paypal_item->expected_revenue = product->retail_price;
		}
		else
		if ( ( event =
			event_program_name_seek(
				paypal_item->item_data,
				event_list ) ) )
		{
			paypal_item->expected_revenue = event->ticket_price;
		}
		else
		if ( ( event =
			event_paypal_long_label_seek(
				paypal_item->item_data,
				event_list ) ) )
		{
			paypal_item->expected_revenue = event->ticket_price;
		}
		else
		if ( ( event =
			event_paypal_short_label_seek(
				paypal_item->item_data,
				event_list ) ) )
		{
			paypal_item->expected_revenue = event->ticket_price;
		}

	} while( list_next( paypal_item_list ) );
}

PAYPAL_DEPOSIT *paypal_deposit_sweep(
			PAYPAL_DATASET *paypal_dataset,
			int row_number )
{
	PAYPAL_DEPOSIT *paypal_deposit;
	ENTITY_SELF *entity_self;

	if ( !paypal_dataset )
	{
		fprintf(stderr,
			"%s/%s()/%d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	paypal_deposit = paypal_deposit_calloc();

	paypal_deposit->row_number = row_number;

	/* Columns A and B */
	/* --------------- */
	paypal_deposit->paypal_date_time =
		paypal_deposit_date_time(
			paypal_dataset->date_A,
			paypal_dataset->time_B );

	entity_self = entity_self_fetch();
	paypal_deposit->payor_entity = entity_self->entity;

	/* Column H */
	/* -------- */
	if ( ! ( paypal_deposit->paypal_amount =
			atof( paypal_dataset->gross_revenue_H ) ) )
	{
		return (PAYPAL_DEPOSIT *)0;
	}

	/* Column K */
	/* -------- */
	paypal_deposit->from_email_address =
	paypal_deposit->payor_entity->email_address =
		strdup( paypal_dataset->from_email_address_K );

	/* Column M */
	/* -------- */
	paypal_deposit->transaction_ID = paypal_dataset->transaction_ID_M;

	/* Column Z */
	/* -------- */
	paypal_deposit->invoice_number = paypal_dataset->invoice_number_Z;

	/* Column AD */
	/* --------- */
	paypal_deposit->account_balance =
		atof( paypal_dataset->account_balance_AD );

	paypal_deposit->paypal_sweep =
		paypal_sweep_paypal(
				paypal_deposit->payor_entity,
				paypal_deposit->paypal_date_time,
				paypal_deposit->paypal_amount );

	return paypal_deposit;
}

FILE *paypal_deposit_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement t=%s f=\"%s\" replace=%c delimiter='%c'|"
		"sql 2>&1						 |"
		"cat >%s						  ",
		PAYPAL_DEPOSIT_TABLE,
		PAYPAL_INSERT_COLUMNS,
		'y',
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void paypal_deposit_insert_pipe(
			FILE *insert_pipe,
			char *payor_full_name,
			char *payor_street_address,
			char *paypal_date_time,
			int row_number,
			char *season_name,
			int year,
			double paypal_amount,
			double transaction_fee,
			double net_revenue,
			double tuition_payment_total,
			double program_donation_total,
			double ticket_sale_total,
			double product_sale_total,
			double sweep_amount,
			double tuition_refund_total,
			double ticket_refund_total,
			double product_refund_total,
			double account_balance,
			char *transaction_ID,
			char *invoice_number,
			char *from_email_address )
{
	fprintf(insert_pipe,
"%s^%s^%s^%d^%s^%d^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%.2lf^%s^%.2lf^%s^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name( payor_full_name ),
		payor_street_address,
		paypal_date_time,
		row_number,
		season_name,
		year,
		paypal_amount,
		transaction_fee,
		net_revenue,
		tuition_payment_total,
		program_donation_total,
		ticket_sale_total,
		product_sale_total,
		sweep_amount,
		tuition_refund_total,
		ticket_refund_total,
		product_refund_total,
		from_email_address,
		account_balance,
		transaction_ID,
		invoice_number );
}

void paypal_deposit_paypal_insert( LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];
	char *season_name;
	int year;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	insert_pipe =
		paypal_deposit_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		if ( !paypal_deposit->semester )
		{
			season_name = "";
			year = 0;
		}
		else
		{
			season_name = paypal_deposit->semester->season_name;
			year = paypal_deposit->semester->year;
		}

		if ( !paypal_deposit->payor_entity )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty payor_entity.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !paypal_deposit->paypal_date_time )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: empty paypal_date_time.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		paypal_deposit_insert_pipe(
			insert_pipe,
			paypal_deposit->payor_entity->full_name,
			paypal_deposit->payor_entity->street_address,
			paypal_deposit->paypal_date_time,
			paypal_deposit->row_number,
			season_name,
			year,
			paypal_deposit->paypal_amount,
			paypal_deposit->transaction_fee,
			paypal_deposit->net_revenue,
			paypal_deposit->tuition_payment_total,
			paypal_deposit->program_donation_total,
			paypal_deposit->ticket_sale_total,
			paypal_deposit->product_sale_total,
			paypal_deposit->sweep_amount,
			paypal_deposit->tuition_refund_total,
			paypal_deposit->ticket_refund_total,
			paypal_deposit->product_refund_total,
			paypal_deposit->account_balance,
			paypal_deposit->transaction_ID,
			paypal_deposit->invoice_number,
			paypal_deposit->from_email_address );

	} while ( list_next( paypal_deposit_list ) );

	pclose( insert_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf(sys_string,
			"cat %s						|"
			"queue_top_bottom_lines.e 300			|"
			"html_table.e 'Insert Paypal Errors' '' '^'",
			 error_filename );

		if ( system( sys_string ) ){}
	}

	sprintf( sys_string, "rm %s", error_filename );

	if ( system( sys_string ) ){};
}

void paypal_deposit_paypal_sweep_insert(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		if ( paypal_deposit->paypal_sweep )
		{
			paypal_sweep_insert(
				paypal_deposit->paypal_sweep );
		}

	} while ( list_next( paypal_deposit_list ) );
}

void paypal_deposit_tuition_payment_insert(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		tuition_payment_list_insert(
			paypal_deposit->tuition_payment_list );

	} while ( list_next( paypal_deposit_list ) );
}

void paypal_deposit_program_donation_insert(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		program_donation_list_insert(
			paypal_deposit->program_donation_list );

	} while ( list_next( paypal_deposit_list ) );
}

void paypal_deposit_ticket_sale_insert(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		ticket_sale_list_insert(
			paypal_deposit->ticket_sale_list );

	} while ( list_next( paypal_deposit_list ) );
}

void paypal_deposit_ticket_sale_event_insert(
			LIST *paypal_deposit_list,
			char *season_name,
			int year )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		ticket_sale_list_event_insert(
			paypal_deposit->ticket_sale_list,
			season_name,
			year );

	} while ( list_next( paypal_deposit_list ) );
}

void paypal_deposit_product_sale_insert(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		product_sale_list_insert(
			paypal_deposit->product_sale_list );

	} while ( list_next( paypal_deposit_list ) );
}

void paypal_deposit_ticket_refund_insert(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		ticket_refund_list_insert(
			paypal_deposit->ticket_refund_list );

	} while ( list_next( paypal_deposit_list ) );
}

void paypal_deposit_product_refund_insert(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		product_refund_list_insert(
			paypal_deposit->product_refund_list );

	} while ( list_next( paypal_deposit_list ) );
}

void paypal_deposit_tuition_refund_insert(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		tuition_refund_list_insert(
			paypal_deposit->tuition_refund_list );

	} while ( list_next( paypal_deposit_list ) );
}

void paypal_deposit_enrollment_insert(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		tuition_payment_list_enrollment_insert(
			paypal_deposit->tuition_payment_list );

	} while ( list_next( paypal_deposit_list ) );
}

void paypal_deposit_registration_insert(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		tuition_payment_list_registration_insert(
			paypal_deposit->tuition_payment_list );

	} while ( list_next( paypal_deposit_list ) );
}

void paypal_deposit_student_insert(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		tuition_payment_list_student_insert(
			paypal_deposit->tuition_payment_list );

	} while ( list_next( paypal_deposit_list ) );
}

void paypal_deposit_student_entity_insert(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		tuition_payment_list_student_entity_insert(
			paypal_deposit->tuition_payment_list );

	} while ( list_next( paypal_deposit_list ) );
}

void paypal_deposit_payor_entity_insert(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		tuition_payment_list_payor_entity_insert(
			paypal_deposit->tuition_payment_list );

		program_donation_list_payor_entity_insert(
			paypal_deposit->program_donation_list );

		product_sale_list_payor_entity_insert(
			paypal_deposit->product_sale_list );

		ticket_sale_list_payor_entity_insert(
			paypal_deposit->ticket_sale_list );

	} while ( list_next( paypal_deposit_list ) );
}

char *paypal_deposit_date_time(
			char *date_A,
			char *time_B )
{
	char paypal_date_time[ 128 ];

	sprintf(paypal_date_time,
		"%s %s",
		/* --------------------------------- */
		/* Looks like: 1/2/2020 and 12:28:39 */
		/* --------------------------------- */
		date_convert_international_string(
			date_A ),
		time_B );

	return strdup( paypal_date_time );
}

LIST *paypal_deposit_sale_product_name_list(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;
	LIST *product_name_list;

	if ( !list_rewind( paypal_deposit_list ) ) return (LIST *)0;

	product_name_list = list_new();

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		list_unique_list(
			product_name_list,
			product_sale_product_name_list(
				paypal_deposit->product_sale_list ) );

	} while ( list_next( paypal_deposit_list ) );

	return product_name_list;
}

LIST *paypal_deposit_refund_product_name_list(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;
	LIST *product_name_list;

	if ( !list_rewind( paypal_deposit_list ) ) return (LIST *)0;

	product_name_list = list_new();

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		list_unique_list(
			product_name_list,
			product_refund_product_name_list(
				paypal_deposit->product_refund_list ) );

	} while ( list_next( paypal_deposit_list ) );

	return product_name_list;
}

PAYPAL_DEPOSIT *paypal_deposit_education(
			char *season_name,
			int year,
			LIST *semester_offering_list,
			LIST *program_list,
			LIST *product_list,
			LIST *semester_event_list,
			PAYPAL_DATASET *paypal_dataset,
			int row_number,
			/* ------------------------------------------- */
			/* Seek out existing enrollments in this file. */
			/* ------------------------------------------- */
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !paypal_dataset )
	{
		fprintf(stderr,
			"%s/%s()/%d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	paypal_deposit = paypal_deposit_calloc();

	paypal_deposit->row_number = row_number;

	paypal_deposit->semester =
		semester_new(
			season_name,
			year );

	/* Columns A and B */
	/* --------------- */
	paypal_deposit->paypal_date_time =
		paypal_deposit_date_time(
			paypal_dataset->date_A,
			paypal_dataset->time_B );

	/* Column D */
	/* -------- */
	if ( !paypal_dataset->full_name_D
	||  !*paypal_dataset->full_name_D )
	{
		paypal_dataset->full_name_D = PAYPAL_UPLOAD_DEFAULT_FULL_NAME;
	}

	paypal_deposit->payor_entity =
		entity_full_name_entity(
			paypal_dataset->full_name_D );

	/* Column H */
	/* -------- */
	paypal_deposit->paypal_amount =
		atof( paypal_dataset->gross_revenue_H );

	/* Column I */
	/* -------- */
	paypal_deposit->transaction_fee = 
		atof( paypal_dataset->transaction_fee_I );

	/* Column J				   */
	/* Note: this is going to be recalculated. */
	/* --------------------------------------- */
	paypal_deposit->net_revenue =
		atof( paypal_dataset->net_revenue_J );

	/* Column K */
	/* -------- */
	paypal_deposit->from_email_address =
	paypal_deposit->payor_entity->email_address =
		strdup( paypal_dataset->from_email_address_K );

	/* Column M */
	/* -------- */
	paypal_deposit->transaction_ID = paypal_dataset->transaction_ID_M;

	/* Column Z */
	/* -------- */
	paypal_deposit->invoice_number = paypal_dataset->invoice_number_Z;

	/* Column AB */
	/* --------- */
	paypal_deposit->quantity =
		atoi( paypal_dataset->quantity_AB );

	/* Column AD */
	/* --------- */
	paypal_deposit->account_balance =
		atof( paypal_dataset->account_balance_AD );

	/* Steady state */
	/* ------------ */
	paypal_deposit->paypal_item_list =
		paypal_item_list(
			paypal_item_entity_delimit(
				paypal_dataset->item_title_P ),
			paypal_dataset->transaction_type_E,
			education_paypal_allowed_list(
				offering_name_list(
					semester_offering_list ),
				program_name_list(
					program_list ),
				program_alias_name_list(
					program_list ),
				product_name_list(
					product_list ) ),
			event_label_list(
				semester_event_list ) );

	if ( !list_length( paypal_deposit->paypal_item_list ) )
	{
		return (PAYPAL_DEPOSIT *)0;
	}

	paypal_deposit_set_expected_revenue(
		paypal_deposit->paypal_item_list,
		semester_offering_list,
		product_list,
		semester_event_list );

	paypal_deposit->paypal_item_expected_revenue_total =
		paypal_item_expected_revenue_total(
			paypal_deposit->paypal_item_list );

	paypal_deposit->paypal_item_nonexpected_revenue_length =
		paypal_item_nonexpected_revenue_length(
			paypal_deposit->paypal_item_list );

	paypal_deposit->paypal_item_expected_revenue_length =
		paypal_item_expected_revenue_length(
			paypal_deposit->paypal_item_list );

	paypal_deposit->paypal_item_list_steady_state =
		paypal_item_list_steady_state(
			paypal_deposit->paypal_item_list,
			paypal_deposit->paypal_amount,
			paypal_deposit->transaction_fee,
			paypal_deposit->paypal_item_expected_revenue_total,
			paypal_deposit->paypal_item_nonexpected_revenue_length,
			paypal_deposit->paypal_item_expected_revenue_length );

	/* Columns E and P */
	/* --------------- */
	paypal_deposit->registration_list =
		registration_list_list_paypal(
			season_name,
			year,
			paypal_deposit->payor_entity,
			paypal_deposit->paypal_date_time,
			paypal_deposit->paypal_item_list_steady_state,
			semester_offering_list,
			/* ------------------------------------ */
			/* Seek out registrations in this file. */
			/* ------------------------------------ */
			paypal_deposit_list );


	if ( !paypal_deposit->registration_list
	&&   paypal_deposit->paypal_amount > 0.0 )
	{
		double consumed_item_value;
		double consumed_item_fee;

		paypal_deposit->product_sale_list =
			product_sale_list_paypal(
				paypal_deposit->payor_entity,
				paypal_deposit->paypal_date_time,
				paypal_deposit->paypal_item_list_steady_state,
				product_list );

		paypal_deposit->ticket_sale_list =
			ticket_sale_list_paypal(
				paypal_deposit->payor_entity,
				paypal_deposit->paypal_date_time,
				paypal_deposit->paypal_item_list_steady_state,
				semester_event_list,
				paypal_deposit->paypal_amount );

		paypal_deposit->program_donation_list =
			program_donation_list_paypal(
				paypal_deposit->payor_entity,
				paypal_deposit->paypal_date_time,
				paypal_deposit->paypal_item_list_steady_state,
				program_list );

		/* --------------------------------- */
		/* Overpayment is a program donation */
		/* --------------------------------- */
		if ( !list_length( paypal_deposit->program_donation_list ) )
		{
			consumed_item_value =
				tuition_payment_total(
					paypal_deposit->
						tuition_payment_list ) +
				product_sale_total(
					paypal_deposit->
						product_sale_list ) +
				ticket_sale_total(
					paypal_deposit->
						ticket_sale_list );

			consumed_item_fee =
				tuition_payment_fee_total(
					paypal_deposit->
						tuition_payment_list ) +
				product_sale_fee_total(
					paypal_deposit->
						product_sale_list ) +
				ticket_sale_fee_total(
					paypal_deposit->
						ticket_sale_list );

			if (	consumed_item_value <
				paypal_deposit->paypal_amount )
			{
				paypal_deposit->program_donation_list =
					program_donation_list_overpayment(
						paypal_deposit->paypal_amount -
						consumed_item_value
							/* item_value */,
						paypal_deposit->
							transaction_fee -
						consumed_item_fee
							/* item_fee */,
						paypal_deposit->
							payor_entity,
						paypal_deposit->
							paypal_date_time,
						list_first(
							paypal_deposit->
							 tuition_payment_list ),
						list_first(
							paypal_deposit->
							 ticket_sale_list ),
						list_first(
							paypal_deposit->
							 product_sale_list ) );
			}
		}
	}
	else
	if ( !paypal_deposit->registration_list
	&&   paypal_deposit->paypal_amount < 0.0 )
	{
		paypal_deposit->product_refund_list =
			product_refund_list_paypal(
				paypal_deposit->payor_entity,
				paypal_deposit->paypal_date_time,
				paypal_deposit->paypal_item_list_steady_state,
				product_list );

		paypal_deposit->ticket_refund_list =
			ticket_refund_list_paypal(
				paypal_deposit->payor_entity,
				paypal_deposit->paypal_date_time,
				paypal_deposit->paypal_item_list_steady_state,
				semester_event_list );
	}

	if ( list_length( paypal_deposit->tuition_payment_list )
	||   list_length( paypal_deposit->program_donation_list )
	||   list_length( paypal_deposit->product_sale_list )
	||   list_length( paypal_deposit->ticket_sale_list )
	||   list_length( paypal_deposit->tuition_refund_list )
	||   list_length( paypal_deposit->product_refund_list )
	||   list_length( paypal_deposit->ticket_refund_list ) )
	{
		return paypal_deposit;
	}
	else
	{
		return (PAYPAL_DEPOSIT *)0;
	}
}

void paypal_deposit_list_insert(
			LIST *paypal_deposit_list,
			char *season_name,
			int year )
{
	/* ------ */
	/* Paypal */
	/* ------ */
	paypal_deposit_paypal_insert( paypal_deposit_list );

	/* ---------- */
	/* Enrollment */
	/* ---------- */
	paypal_deposit_registration_insert( paypal_deposit_list );
	paypal_deposit_enrollment_insert( paypal_deposit_list );
	paypal_deposit_tuition_payment_insert( paypal_deposit_list );

	/* -------------- */
	/* Other deposits */
	/* -------------- */
	paypal_deposit_program_donation_insert( paypal_deposit_list );
	paypal_deposit_product_sale_insert( paypal_deposit_list );
	paypal_deposit_ticket_sale_insert( paypal_deposit_list );

	paypal_deposit_ticket_sale_event_insert(
			paypal_deposit_list,
			season_name,
			year );

	/* ----- */
	/* Sweep */
	/* ----- */
	paypal_deposit_paypal_sweep_insert( paypal_deposit_list );

	/* ------- */
	/* Refunds */
	/* ------- */
	paypal_deposit_tuition_refund_insert( paypal_deposit_list );
	paypal_deposit_product_refund_insert( paypal_deposit_list );

	/* ---------- */
	/* Dependents */
	/* ---------- */
	paypal_deposit_student_insert( paypal_deposit_list );
	paypal_deposit_student_entity_insert( paypal_deposit_list );

	/* -------------------- */
	/* Payor entity for:	*/
	/* tuition payment,	*/
	/* ticket sale,		*/
	/* product sale,	*/
	/* program donation.	*/
	/* -------------------- */
	paypal_deposit_payor_entity_insert( paypal_deposit_list );
}

LIST *paypal_deposit_list_registration_list(
			LIST *paypal_deposit_list )
{
	LIST *registration_list;
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return (LIST *)0;

	registration_list = list_new();

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		if ( list_length( paypal_deposit->registration_list ) )
		{
			list_set_list(
				registration_list,
				paypal_deposit->registration_list );
		}

	} while ( list_next( paypal_deposit_list ) );

	return registration_list;
}

LIST *paypal_deposit_tuition_refund_list(
			LIST *paypal_deposit_list )
{
	LIST *tuition_refund_list;
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return (LIST *)0;

	tuition_refund_list = list_new();

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		if ( list_length( paypal_deposit->tuition_refund_list ) )
		{
			list_set_list(
				tuition_refund_list,
				paypal_deposit->tuition_refund_list );
		}

	} while ( list_next( paypal_deposit_list ) );

	return tuition_refund_list;
}

LIST *paypal_deposit_ticket_sale_list(
			LIST *paypal_deposit_list )
{
	LIST *ticket_sale_list;
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return (LIST *)0;

	ticket_sale_list = list_new();

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		if ( list_length( paypal_deposit->ticket_sale_list ) )
		{
			list_set_list(
				ticket_sale_list,
				paypal_deposit->ticket_sale_list );
		}

	} while ( list_next( paypal_deposit_list ) );

	return ticket_sale_list;
}

LIST *paypal_deposit_ticket_refund_list(
			LIST *paypal_deposit_list )
{
	LIST *ticket_refund_list;
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return (LIST *)0;

	ticket_refund_list = list_new();

	do {
		paypal_deposit = list_get( paypal_deposit_list ); 
		if ( list_length( paypal_deposit->ticket_refund_list ) )
		{
			list_set_list(
				ticket_refund_list,
				paypal_deposit->ticket_refund_list );
		}

	} while ( list_next( paypal_deposit_list ) );

	return ticket_refund_list;
}

LIST *paypal_deposit_list_enrollment_list(
			LIST *paypal_deposit_list )
{
	LIST *enrollment_list;
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return (LIST *)0;

	enrollment_list = list_new();

	do {
		paypal_deposit = list_get( paypal_deposit_list ); 

		if ( list_length( paypal_deposit->registration_list ) )
		{
			list_set_list(
				enrollment_list,
				registration_list_enrollment_list(
					paypal_deposit->
						registration_list ) );
		}

	} while ( list_next( paypal_deposit_list ) );

	return enrollment_list;
}

boolean paypal_deposit_exclude_existing_transaction(
			PAYPAL_DEPOSIT *paypal_deposit,
			LIST *existing_program_donation_list,
			LIST *existing_tuition_payment_list,
			LIST *existing_tuition_refund_list,
			LIST *existing_product_sale_list,
			LIST *existing_product_refund_list,
			LIST *existing_ticket_sale_list,
			LIST *existing_ticket_refund_list,
			LIST *existing_paypal_sweep_list )
{
	if ( program_donation_list_exists(
		paypal_deposit->program_donation_list,
		existing_program_donation_list ) )
	{
		return 1;
	}

	if ( tuition_payment_list_exists(
		paypal_deposit->tuition_payment_list,
		existing_tuition_payment_list ) )
	{
		return 1;
	}

	if ( tuition_refund_list_exists(
		paypal_deposit->tuition_refund_list,
		existing_tuition_refund_list ) )
	{
		return 1;
	}

	if ( product_sale_list_exists(
		paypal_deposit->product_sale_list,
		existing_product_sale_list ) )
	{
		return 1;
	}

	if ( product_refund_list_exists(
		paypal_deposit->product_refund_list,
		existing_product_refund_list ) )
	{
		return 1;
	}

	if ( ticket_sale_list_exists(
		paypal_deposit->ticket_sale_list,
		existing_ticket_sale_list ) )
	{
		return 1;
	}

	if ( ticket_refund_list_exists(
		paypal_deposit->ticket_refund_list,
		existing_ticket_refund_list ) )
	{
		return 1;
	}

	if ( paypal_sweep_exists(
		paypal_deposit->paypal_sweep,
		existing_paypal_sweep_list ) )
	{
		return 1;
	}

	return 0;
}

