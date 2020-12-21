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
#include "list.h"
#include "entity.h"
#include "transaction.h"
#include "paypal_upload.h"
#include "paypal_item.h"
#include "semester.h"
#include "registration.h"
#include "registration_fns.h"
#include "tuition_payment.h"
#include "tuition_payment_fns.h"
#include "program_payment_fns.h"
#include "product_payment_fns.h"
#include "product_refund_fns.h"
#include "tuition_refund.h"
#include "tuition_refund_fns.h"
#include "enrollment_fns.h"
#include "offering_fns.h"
#include "paypal_deposit.h"

LIST *paypal_fetch_program_payment_list(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_program,
			boolean fetch_paypal )
{
	return
		program_payment_system_list(
			program_payment_sys_string(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				paypal_deposit_primary_where(
					payor_full_name,
					payor_street_address,
					season_name,
					year,
					deposit_date_time ) ),
			fetch_program,
			fetch_paypal );
}

LIST *paypal_fetch_tuition_payment_list(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_paypal,
			boolean fetch_enrollment )
{
	return
		tuition_payment_system_list(
			tuition_payment_sys_string(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				paypal_deposit_primary_where(
					payor_full_name,
					payor_street_address,
					season_name,
					year,
					deposit_date_time ) ),
			fetch_paypal,
			fetch_enrollment );
}

LIST *paypal_fetch_tuition_refund_list(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_paypal,
			boolean fetch_enrollment )
{
	return
		tuition_refund_system_list(
			tuition_refund_sys_string(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				paypal_deposit_primary_where(
					payor_full_name,
					payor_street_address,
					season_name,
					year,
					deposit_date_time ) ),
			fetch_paypal,
			fetch_enrollment );
}

double paypal_net_revenue(
			double paypal_amount,
			double transaction_fee )
{
	return paypal_amount - transaction_fee;
}


char *paypal_deposit_primary_where(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time )
{
	static char where[ 1024 ];

	sprintf( where,
		 "payor_full_name = '%s' and		"
		 "payor_street_address = '%s' and	"
		 "season_name = '%s' and		"
		 "year = %d and				"
		 "deposit_date_time = '%s'		",
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
			char *season_name,
			int year,
			char *deposit_date_time )
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

	paypal_deposit->semester =
		semester_new(
			season_name,
			year );

	paypal_deposit->deposit_date_time = deposit_date_time;
	return paypal_deposit;
}

PAYPAL_DEPOSIT *paypal_deposit_parse(
			char *input,
			boolean fetch_tuition_payment_list,
			boolean fetch_program_payment_list,
			boolean fetch_product_payment_list,
			boolean fetch_tuition_refund_list )
{
	char payor_full_name[ 128 ];
	char payor_street_address[ 128 ];
	char season_name[ 128 ];
	char year[ 128 ];
	char deposit_date_time[ 128 ];
	char deposit_amount[ 128 ];
	char transaction_fee[ 128 ];
	char net_revenue[ 128 ];
	char account_balance[ 128 ];
	char transaction_ID[ 128 ];
	char invoice_number[ 128 ];
	char from_email_address[ 128 ];
	char tuition_payment_total[ 128 ];
	char program_payment_total[ 128 ];
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !input || !*input ) return (PAYPAL_DEPOSIT *)0;

	/* See: attribute_list paypal_deposit */
	/* ---------------------------------- */
	piece( payor_full_name, SQL_DELIMITER, input, 0 );
	piece( payor_street_address, SQL_DELIMITER, input, 1 );
	piece( season_name, SQL_DELIMITER, input, 2 );
	piece( year, SQL_DELIMITER, input, 3 );
	piece( deposit_date_time, SQL_DELIMITER, input, 4 );

	paypal_deposit =
		paypal_deposit_new(
			strdup( payor_full_name ),
			strdup( payor_street_address ),
			strdup( season_name ),
			atoi( year ),
			strdup( deposit_date_time ) );

	piece( deposit_amount, SQL_DELIMITER, input, 5 );
	paypal_deposit->deposit_amount = atof( deposit_amount );

	piece( transaction_fee, SQL_DELIMITER, input, 6 );
	paypal_deposit->transaction_fee = atof( transaction_fee );

	piece( net_revenue, SQL_DELIMITER, input, 7 );
	paypal_deposit->net_revenue = atof( net_revenue );

	piece( account_balance, SQL_DELIMITER, input, 8 );
	paypal_deposit->account_balance = atof( account_balance );

	piece( transaction_ID, SQL_DELIMITER, input, 9 );
	paypal_deposit->transaction_ID = strdup( transaction_ID );

	piece( invoice_number, SQL_DELIMITER, input, 10 );
	paypal_deposit->invoice_number = strdup( invoice_number );

	piece( from_email_address, SQL_DELIMITER, input, 11 );
	paypal_deposit->from_email_address = strdup( from_email_address );

	piece( tuition_payment_total, SQL_DELIMITER, input, 12 );
	paypal_deposit->tuition_payment_total = atof( tuition_payment_total );

	piece( program_payment_total, SQL_DELIMITER, input, 13 );
	paypal_deposit->program_payment_total = atof( program_payment_total );

	if ( fetch_tuition_payment_list )
	{
		paypal_deposit->tuition_payment_list =
			paypal_fetch_tuition_payment_list(
				payor_full_name,
				payor_street_address,
				season_name,
				paypal_deposit->semester->year,
				deposit_date_time,
				0 /* not fetch_paypal */,
				1 /* fetch_enrollment */ );
	}

	if ( fetch_program_payment_list )
	{
		paypal_deposit->program_payment_list =
			paypal_fetch_program_payment_list(
				payor_full_name,
				payor_street_address,
				season_name,
				paypal_deposit->semester->year,
				deposit_date_time,
				1 /* fetch_program */,
				0 /* not fetch_paypal */ );
	}

	if ( fetch_product_payment_list )
	{
		paypal_deposit->product_payment_list =
			paypal_fetch_product_payment_list(
				payor_full_name,
				payor_street_address,
				season_name,
				paypal_deposit->semester->year,
				deposit_date_time,
				1 /* fetch_product */,
				0 /* not fetch_paypal */ );
	}

	if ( fetch_tuition_refund_list )
	{
		paypal_deposit->tuition_refund_list =
			paypal_fetch_tuition_refund_list(
				payor_full_name,
				payor_street_address,
				season_name,
				paypal_deposit->semester->year,
				deposit_date_time,
				0 /* not fetch_paypal */,
				1 /* fetch_enrollment */ );
	}

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
			char *sys_string,
			boolean fetch_tuition_payment_list,
			boolean fetch_program_payment_list,
			boolean fetch_product_payment_list,
			boolean fetch_tuition_refund_list )
{
	char input[ 1024 ];
	FILE *input_pipe = popen( sys_string, "r" );
	LIST *list = list_new();

	while( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			list,
			paypal_deposit_parse(
				input,
				fetch_tuition_payment_list,
				fetch_program_payment_list,
				fetch_product_payment_list,
				fetch_tuition_refund_list ) );
	}
	pclose( input_pipe );
	return list;
}

PAYPAL_DEPOSIT *paypal_deposit_fetch(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_tuition_payment_list,
			boolean fetch_program_payment_list,
			boolean fetch_product_payment_list,
			boolean fetch_tuition_refund_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	paypal_deposit =
		paypal_deposit_parse(
			pipe2string(
				paypal_deposit_sys_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					paypal_deposit_primary_where(
						payor_full_name,
						payor_street_address,
						season_name,
						year,
						deposit_date_time ) ) ),
			fetch_tuition_payment_list,
			fetch_program_payment_list,
			fetch_product_payment_list,
			fetch_tuition_refund_list );
	return paypal_deposit;
}

void paypal_deposit_set_transaction( PAYPAL_DEPOSIT *paypal_deposit )
{
	int transaction_seconds_to_add = 0;

	if ( list_length( paypal_deposit->tuition_payment_list ) )
	{
		enrollment_list_set_transaction(
			&transaction_seconds_to_add,
			tuition_payment_list_enrollment_list(
				paypal_deposit->tuition_payment_list ) );

		tuition_payment_list_set_transaction(
			&transaction_seconds_to_add,
			paypal_deposit->tuition_payment_list );
	}

	if ( list_length( paypal_deposit->program_payment_list ) )
	{
		program_payment_list_set_transaction(
			&transaction_seconds_to_add,
			paypal_deposit->program_payment_list );
	}

	if ( list_length( paypal_deposit->product_payment_list ) )
	{
		product_payment_list_set_transaction(
			&transaction_seconds_to_add,
			paypal_deposit->product_payment_list );
	}

	if ( list_length( paypal_deposit->tuition_refund_list ) )
	{
		tuition_refund_list_set_transaction(
			&transaction_seconds_to_add,
			paypal_deposit->tuition_refund_list );
	}

	if ( list_length( paypal_deposit->product_refund_list ) )
	{
		product_refund_list_set_transaction(
			&transaction_seconds_to_add,
			paypal_deposit->product_refund_list );
	}
}

PAYPAL_DEPOSIT *paypal_deposit_steady_state(
			PAYPAL_DEPOSIT *paypal_deposit,
			LIST *semester_offering_list )
{
	int transaction_seconds_to_add = 0;

	/* Build the registration list */
	/* --------------------------- */
	if ( list_length( paypal_deposit->tuition_payment_list )
	||   list_length( paypal_deposit->tuition_refund_list ) )
	{
		paypal_deposit->registration_list =
			paypal_deposit_registration_list(
				paypal_deposit->tuition_payment_list,
				paypal_deposit->tuition_refund_list );
	}

	/* Execute the four steady states */
	/* ------------------------------ */
	if ( list_length( paypal_deposit->tuition_payment_list ) )
	{
		paypal_deposit->tuition_payment_list =
			tuition_payment_list_steady_state(
				&transaction_seconds_to_add,
				paypal_deposit->tuition_payment_list,
				paypal_deposit->registration_list,
				semester_offering_list,
				paypal_deposit->deposit_amount,
				paypal_deposit->transaction_fee );
	}

	if ( list_length( paypal_deposit->program_payment_list ) )
	{
		paypal_deposit->program_payment_list =
			program_payment_list_steady_state(
				&transaction_seconds_to_add,
				paypal_deposit->program_payment_list,
				paypal_deposit->deposit_amount,
				paypal_deposit->transaction_fee );
	}

	if ( list_length( paypal_deposit->product_payment_list ) )
	{
		paypal_deposit->product_payment_list =
			product_payment_list_steady_state(
				&transaction_seconds_to_add,
				paypal_deposit->product_payment_list,
				paypal_deposit->deposit_amount,
				paypal_deposit->transaction_fee );
	}

	if ( list_length( paypal_deposit->tuition_refund_list ) )
	{
		paypal_deposit->tuition_refund_list =
			tuition_refund_list_steady_state(
				&transaction_seconds_to_add,
				paypal_deposit->tuition_refund_list,
				paypal_deposit->registration_list,
				semester_offering_list,
				paypal_deposit->deposit_amount,
				paypal_deposit->transaction_fee );
	}

	if ( list_length( paypal_deposit->product_refund_list ) )
	{
		paypal_deposit->product_refund_list =
			product_refund_list_steady_state(
				&transaction_seconds_to_add,
				paypal_deposit->product_refund_list,
				paypal_deposit->deposit_amount,
				paypal_deposit->transaction_fee );
	}

	/* Set the four totals */
	/* ------------------- */
	if ( list_length( paypal_deposit->tuition_payment_list ) )
	{
		paypal_deposit->tuition_payment_total =
			tuition_payment_total(
				paypal_deposit->tuition_payment_list );
	}

	if ( list_length( paypal_deposit->program_payment_list ) )
	{
		paypal_deposit->program_payment_total =
			program_payment_total(
				paypal_deposit->program_payment_list );
	}

	if ( list_length( paypal_deposit->tuition_refund_list ) )
	{
		paypal_deposit->tuition_refund_total =
			tuition_refund_total(
				paypal_deposit->tuition_refund_list );
	}

	/* Calculate paypal_registration_income */
	/* ------------------------------------ */
	paypal_deposit->registration_tuition =
		paypal_registration_tuition(
			paypal_deposit->
				registration_list,
			semester_offering_list );

	/* Calculate paypal_deposit_net_revenue */
	/* ------------------------------------ */
	paypal_deposit->net_revenue =
		paypal_net_revenue(
			paypal_deposit->deposit_amount,
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
			double paypal_deposit_program_payment_total,
			double paypal_deposit_net_revenue,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time )
{
	FILE *update_pipe;

	update_pipe = paypal_deposit_update_open();

	fprintf( update_pipe,
		 "%s^%s^%s^%d^%s^tuition_payment_total^%.2lf\n",
		 payor_full_name,
		 payor_street_address,
		 season_name,
		 year,
		 deposit_date_time,
		 paypal_deposit_tuition_payment_total );

	fprintf( update_pipe,
		 "%s^%s^%s^%d^%s^program_payment_total^%.2lf\n",
		 payor_full_name,
		 payor_street_address,
		 season_name,
		 year,
		 deposit_date_time,
		 paypal_deposit_program_payment_total );

	fprintf( update_pipe,
		 "%s^%s^%s^%d^%s^net_revenue^%.2lf\n",
		 payor_full_name,
		 payor_street_address,
		 season_name,
		 year,
		 deposit_date_time,
		 paypal_deposit_net_revenue );

	pclose( update_pipe );
}

double paypal_registration_tuition(
			LIST *paypal_registration_list,
			LIST *semester_offering_list )
{
	REGISTRATION *registration;
	double tuition;

	if ( !list_rewind( paypal_registration_list ) ) return 0.0;

	tuition = 0.0;

	do {
		registration = list_get( paypal_registration_list );

		tuition +=
			( registration->tuition =
				registration_tuition(
					registration->
					     enrollment_list,
					semester_offering_list ) );

	} while ( list_next( paypal_registration_list ) );

	return tuition;
}

double paypal_gain_donation(
			double deposit_amount,
			double registration_tuition )
{
	double gain_donation;

	gain_donation =
		deposit_amount -
		registration_tuition;

	if ( gain_donation <= 0.0 )
		gain_donation = 0.0;

	return gain_donation;
}

double paypal_overpayment_loss(
			double deposit_amount,
			double registration_tuition )
{
	double overpayment_loss;

	overpayment_loss =
		deposit_amount -
		registration_tuition;

	if ( overpayment_loss <= 0.0 )
		overpayment_loss = 0.0;

	return overpayment_loss;
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
			char *season_name,
			int year,
			char *deposit_date_time,
			double deposit_amount,
			double transaction_fee,
			double net_revenue,
			double account_balance,
			char *transaction_ID,
			char *invoice_number,
			char *from_email_address )
{
	fprintf(insert_pipe,
		"%s^%s^%s^%d^%s^%.2lf^%.2lf^%.2lf^%.2lf^%s^%s^%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_escape_full_name( payor_full_name ),
		payor_street_address,
		season_name,
		year,
		deposit_date_time,
		deposit_amount,
		transaction_fee,
		net_revenue,
		account_balance,
		transaction_ID,
		invoice_number,
		from_email_address );
}

void paypal_deposit_list_insert( LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( paypal_deposit_list ) ) return;

	insert_pipe =
		paypal_deposit_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		if ( !paypal_deposit->semester )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty semester.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		paypal_deposit_insert_pipe(
			insert_pipe,
			paypal_deposit->payor_entity->full_name,
			paypal_deposit->payor_entity->street_address,
			paypal_deposit->semester->season_name,
			paypal_deposit->semester->year,
			paypal_deposit->deposit_date_time,
			paypal_deposit->deposit_amount,
			paypal_deposit->transaction_fee,
			paypal_deposit->net_revenue,
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
			"html_table.e 'Insert Deposit Errors' '' '^'",
			 error_filename );

		if ( system( sys_string ) ){}
	}

	sprintf( sys_string, "rm %s", error_filename );

	if ( system( sys_string ) ){};
}

void paypal_list_tuition_payment_insert(
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

void paypal_list_program_payment_insert(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		program_payment_list_insert(
			paypal_deposit->program_payment_list );

	} while ( list_next( paypal_deposit_list ) );
}

void paypal_list_product_payment_insert(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		product_payment_list_insert(
			paypal_deposit->product_payment_list );

	} while ( list_next( paypal_deposit_list ) );
}

void paypal_list_tuition_refund_insert(
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

void paypal_list_enrollment_insert(
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

void paypal_list_registration_insert(
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

void paypal_list_student_insert(
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

void paypal_list_student_entity_insert(
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

void paypal_list_payor_entity_insert(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		tuition_payment_list_payor_entity_insert(
			paypal_deposit->tuition_payment_list );

		program_payment_list_payor_entity_insert(
			paypal_deposit->program_payment_list );

	} while ( list_next( paypal_deposit_list ) );
}

void paypal_list_program_payment_trigger(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		if ( list_rewind( paypal_deposit->program_payment_list ) )
		{
			program_payment_list_trigger(
				paypal_deposit->program_payment_list );
		}
	} while ( list_next( paypal_deposit_list ) );
}

void paypal_list_tuition_payment_trigger(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		if ( list_length( paypal_deposit->tuition_payment_list ) )
		{
			tuition_payment_list_trigger(
				paypal_deposit->tuition_payment_list );
		}
	} while ( list_next( paypal_deposit_list ) );
}

void paypal_list_enrollment_trigger(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		if ( !list_rewind( paypal_deposit->tuition_payment_list ) )
		{
			continue;
		}

		tuition_payment_list_enrollment_trigger(
			paypal_deposit->tuition_payment_list );

	} while ( list_next( paypal_deposit_list ) );
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

		list_append_list(
			course_name_list,
			enrollment_course_name_list(
				paypal_enrollment_list(
					paypal_deposit->
					     tuition_payment_list ) ) );

	} while ( list_next( paypal_deposit_list ) );

	return course_name_list;
}

LIST *paypal_deposit_registration_list(
			LIST *paypal_deposit_tuition_payment_list,
			LIST *paypal_deposit_tuition_refund_list )
{
	if ( list_length( paypal_deposit_tuition_payment_list ) )
	{
		return tuition_payment_registration_list(
			paypal_deposit_tuition_payment_list );
	}
	else
	{
		return tuition_refund_registration_list(
			paypal_deposit_tuition_refund_list );
	}
}

LIST *paypal_enrollment_list(
			LIST *paypal_deposit_tuition_payment_list )
{
	return tuition_payment_enrollment_list(
			paypal_deposit_tuition_payment_list );
}

void paypal_deposit_trigger(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			char *state )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
	"paypal_trigger \"%s\" \"%s\" \"%s\" %d \"%s\" %s",
		entity_escape_full_name( payor_full_name ),
		payor_street_address,
		season_name,
		year,
		deposit_date_time,
		state );

	if ( system( sys_string ) ){}
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

		list_append_list(
			transaction_list,
			paypal_deposit_transaction_list(
				paypal_deposit->tuition_payment_list,
				paypal_deposit->program_payment_list,
				paypal_deposit->product_payment_list,
				paypal_deposit->tuition_refund_list,
				paypal_deposit->product_refund_list ) );

	} while ( list_next( paypal_deposit_list ) );
	return transaction_list;
}

LIST *paypal_deposit_list_set_transaction(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return paypal_deposit_list;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		paypal_deposit_set_transaction( paypal_deposit );

	} while ( list_next( paypal_deposit_list ) );

	return paypal_deposit_list;
}

LIST *paypal_deposit_list_steady_state(
			LIST *paypal_deposit_list,
			LIST *semester_offering_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return paypal_deposit_list;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		paypal_deposit =
			paypal_deposit_steady_state(
				paypal_deposit,
				semester_offering_list );

	} while ( list_next( paypal_deposit_list ) );

	return paypal_deposit_list;
}

LIST *paypal_deposit_list_registration_fetch_update(
			LIST *paypal_deposit_list,
			char *season_name,
			int year )
{
	PAYPAL_DEPOSIT *paypal_deposit;

	if ( !list_rewind( paypal_deposit_list ) ) return (LIST *)0;

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		if ( list_length( paypal_deposit->registration_list ) )
		{
			registration_list_fetch_update(
				paypal_deposit->registration_list,
				season_name,
				year );
		}

	} while ( list_next( paypal_deposit_list ) );

	return paypal_deposit_list;
}

LIST *paypal_deposit_list_offering_fetch_update(
			LIST *paypal_deposit_list,
			char *season_name,
			int year )
{
	offering_list_fetch_update(
		paypal_deposit_course_name_list( paypal_deposit_list ),
		season_name,
		year );

	return paypal_deposit_list;
}

LIST *paypal_deposit_transaction_list(
			LIST *tuition_payment_list,
			LIST *program_payment_list,
			LIST *product_payment_list,
			LIST *tuition_refund_list,
			LIST *product_refund_list )
{
	LIST *transaction_list;

	transaction_list = list_new();

	list_append_list(
		transaction_list,
		tuition_payment_transaction_list(
			tuition_payment_list ) );

	list_append_list(
		transaction_list,
		program_payment_transaction_list(
			program_payment_list ) );

	list_append_list(
		transaction_list,
		product_payment_transaction_list(
			product_payment_list ) );

	list_append_list(
		transaction_list,
		tuition_refund_transaction_list(
			tuition_refund_list ) );

	list_append_list(
		transaction_list,
		product_refund_transaction_list(
			product_refund_list ) );

	return transaction_list;
}

LIST *paypal_deposit_list_enrollment_update(
			LIST *paypal_deposit_list,
			char *season_name,
			int year )
{
	enrollment_list_cancelled_update(
		paypal_list_refund_enrollment_list(
			paypal_deposit_list ),
		season_name,
		year );

	return paypal_deposit_list;
}

LIST *paypal_list_refund_enrollment_list(
			LIST *paypal_deposit_list )
{
	PAYPAL_DEPOSIT *paypal_deposit;
	LIST *enrollment_list;

	if ( !list_rewind( paypal_deposit_list ) ) return (LIST *)0;

	enrollment_list = list_new();

	do {
		paypal_deposit = list_get( paypal_deposit_list );

		if ( list_length( paypal_deposit->tuition_refund_list ) )
		{
			list_append_list(
				enrollment_list,
				tuition_refund_enrollment_list(
					paypal_deposit->tuition_refund_list ) );
		}

	} while ( list_next( paypal_deposit_list ) );

	return enrollment_list;
}

LIST *paypal_fetch_product_payment_list(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_product,
			boolean fetch_paypal )
{
	return
		product_payment_system_list(
			product_payment_sys_string(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				paypal_deposit_primary_where(
					payor_full_name,
					payor_street_address,
					season_name,
					year,
					deposit_date_time ) ),
			fetch_product,
			fetch_paypal );
}

void paypal_deposit_set_paypal_item_expected_revenue(
			LIST *paypal_item_list,
			LIST *semester_offering_list )
{
	PAYPAL_ITEM *paypal_item;
	OFFERING *offering;

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

	} while( list_next( paypal_item_list ) );
}

