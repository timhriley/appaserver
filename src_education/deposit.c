/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/deposit.c		*/
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
#include "semester.h"
#include "registration.h"
#include "registration_fns.h"
#include "tuition_payment_fns.h"
#include "program_payment_fns.h"
#include "enrollment_fns.h"
#include "deposit.h"

LIST *deposit_fetch_program_payment_list(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_program,
			boolean fetch_deposit )
{
	return
		program_payment_system_list(
			program_payment_sys_string(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				deposit_primary_where(
					payor_full_name,
					payor_street_address,
					season_name,
					year,
					deposit_date_time ) ),
			fetch_program,
			fetch_deposit );
}

LIST *deposit_fetch_tuition_payment_list(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_deposit,
			boolean fetch_enrollment )
{
	return
		tuition_payment_system_list(
			tuition_payment_sys_string(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				deposit_primary_where(
					payor_full_name,
					payor_street_address,
					season_name,
					year,
					deposit_date_time ) ),
			fetch_deposit,
			fetch_enrollment );
}

double deposit_remaining(
			double deposit_amount,
			double deposit_payment_total )
{
	return deposit_amount - deposit_payment_total;
}

double deposit_net_revenue(
			double deposit_amount,
			double transaction_fee )
{
	return deposit_amount - transaction_fee;
}


char *deposit_primary_where(
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

DEPOSIT *deposit_calloc( void )
{
	DEPOSIT *deposit;

	if ( ! ( deposit = calloc( 1, sizeof( DEPOSIT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return deposit;
}

DEPOSIT *deposit_new(	char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time )
{
	DEPOSIT *deposit;

	if ( ! ( deposit = calloc( 1, sizeof( DEPOSIT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	deposit->payor_entity =
		entity_new(
			payor_full_name,
			payor_street_address );

	deposit->semester =
		semester_new(
			season_name,
			year );

	deposit->deposit_date_time = deposit_date_time;
	return deposit;
}

DEPOSIT *deposit_parse(	char *input,
			boolean fetch_tuition_payment_list,
			boolean fetch_program_payment_list )
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
	DEPOSIT *deposit;

	if ( !input || !*input ) return (DEPOSIT *)0;

	/* See: attribute_list deposit */
	/* --------------------------- */
	piece( payor_full_name, SQL_DELIMITER, input, 0 );
	piece( payor_street_address, SQL_DELIMITER, input, 1 );
	piece( season_name, SQL_DELIMITER, input, 2 );
	piece( year, SQL_DELIMITER, input, 3 );
	piece( deposit_date_time, SQL_DELIMITER, input, 4 );

	deposit =
		deposit_new(
			strdup( payor_full_name ),
			strdup( payor_street_address ),
			strdup( season_name ),
			atoi( year ),
			strdup( deposit_date_time ) );

	piece( deposit_amount, SQL_DELIMITER, input, 5 );
	deposit->deposit_amount = atof( deposit_amount );

	piece( transaction_fee, SQL_DELIMITER, input, 6 );
	deposit->transaction_fee = atof( transaction_fee );

	piece( net_revenue, SQL_DELIMITER, input, 7 );
	deposit->net_revenue = atof( net_revenue );

	piece( account_balance, SQL_DELIMITER, input, 8 );
	deposit->account_balance = atof( account_balance );

	piece( transaction_ID, SQL_DELIMITER, input, 9 );
	deposit->transaction_ID = strdup( transaction_ID );

	piece( invoice_number, SQL_DELIMITER, input, 10 );
	deposit->invoice_number = strdup( invoice_number );

	piece( from_email_address, SQL_DELIMITER, input, 11 );
	deposit->from_email_address = strdup( from_email_address );

	piece( tuition_payment_total, SQL_DELIMITER, input, 12 );
	deposit->deposit_tuition_payment_total = atof( tuition_payment_total );

	piece( program_payment_total, SQL_DELIMITER, input, 13 );
	deposit->deposit_program_payment_total = atof( program_payment_total );

	if ( fetch_tuition_payment_list )
	{
		deposit->deposit_tuition_payment_list =
			deposit_fetch_tuition_payment_list(
				payor_full_name,
				payor_street_address,
				season_name,
				deposit->semester->year,
				deposit_date_time,
				0 /* not fetch_deposit */,
				1 /* fetch_enrollment */ );
	}

	if ( fetch_program_payment_list )
	{
		deposit->deposit_program_payment_list =
			deposit_fetch_program_payment_list(
				payor_full_name,
				payor_street_address,
				season_name,
				deposit->semester->year,
				deposit_date_time,
				1 /* fetch_program */,
				1 /* fetch_deposit */ );
	}

	return deposit;
}

char *deposit_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"select.sh '*' %s \"%s\" select",
		DEPOSIT_TABLE,
		where );

	return strdup( sys_string );
}

LIST *deposit_system_list(
			char *sys_string,
			boolean fetch_tuition_payment_list,
			boolean fetch_program_payment_list )
{
	char input[ 1024 ];
	FILE *input_pipe = popen( sys_string, "r" );
	LIST *list = list_new();

	while( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			list,
			deposit_parse(
				input,
				fetch_tuition_payment_list,
				fetch_program_payment_list ) );
	}
	pclose( input_pipe );
	return list;
}

DEPOSIT *deposit_fetch(	char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_tuition_payment_list,
			boolean fetch_program_payment_list )
{
	DEPOSIT *deposit;

	deposit =
		deposit_parse(
			pipe2string(
				deposit_sys_string(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					deposit_primary_where(
						payor_full_name,
						payor_street_address,
						season_name,
						year,
						deposit_date_time ) ) ),
			fetch_tuition_payment_list,
			fetch_program_payment_list );
	return deposit;
}

double deposit_tuition_payment_total(
			LIST *deposit_tuition_payment_list )
{
	return tuition_payment_total( deposit_tuition_payment_list );
}

double deposit_program_payment_total(
			LIST *deposit_program_payment_list )
{
	return program_payment_total( deposit_program_payment_list );
}

DEPOSIT *deposit_steady_state(
			DEPOSIT *deposit,
			double deposit_amount,
			double transaction_fee,
			LIST *deposit_tuition_payment_list,
			LIST *deposit_program_payment_list )
{
	if ( !deposit->deposit_registration_list )
	{
		deposit->deposit_registration_list =
			deposit_registration_list(
				deposit_tuition_payment_list );
	}

	deposit->deposit_tuition_payment_total =
		deposit_tuition_payment_total(
			deposit_tuition_payment_list );

	deposit->deposit_program_payment_total =
		deposit_program_payment_total(
			deposit_program_payment_list );

	deposit->deposit_invoice_amount_due =
		deposit_invoice_amount_due(
			deposit->
				deposit_registration_list );

	deposit->deposit_net_revenue =
		deposit_net_revenue(
			deposit_amount,
			transaction_fee );

	return deposit;
}

FILE *deposit_update_open( void )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "update_statement table=%s key=%s carrot=y	|"
		 "tee_appaserver_error.sh			|"
		 "sql						 ",
		 DEPOSIT_TABLE,
		 DEPOSIT_PRIMARY_KEY );

	return popen( sys_string, "w" );
}

void deposit_update(
			double deposit_tuition_payment_total,
			double deposit_program_payment_total,
			double deposit_net_revenue,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time )
{
	FILE *update_pipe;

	update_pipe = deposit_update_open();

	fprintf( update_pipe,
		 "%s^%s^%s^%d^%s^tuition_payment_total^%.2lf\n",
		 payor_full_name,
		 payor_street_address,
		 season_name,
		 year,
		 deposit_date_time,
		 deposit_tuition_payment_total );

	fprintf( update_pipe,
		 "%s^%s^%s^%d^%s^program_payment_total^%.2lf\n",
		 payor_full_name,
		 payor_street_address,
		 season_name,
		 year,
		 deposit_date_time,
		 deposit_program_payment_total );

	fprintf( update_pipe,
		 "%s^%s^%s^%d^%s^net_revenue^%.2lf\n",
		 payor_full_name,
		 payor_street_address,
		 season_name,
		 year,
		 deposit_date_time,
		 deposit_net_revenue );

	pclose( update_pipe );
}

double deposit_invoice_amount_due(
			LIST *deposit_registration_list )
{
	REGISTRATION *registration;
	double invoice_amount_due;

	if ( !list_rewind( deposit_registration_list ) ) return 0.0;

	invoice_amount_due = 0.0;

	do {
		registration = list_get( deposit_registration_list );

		invoice_amount_due +=
			registration->
				registration_invoice_amount_due;

	} while ( list_next( deposit_registration_list ) );

	return invoice_amount_due;
}

double deposit_gain_donation(
			double deposit_amount,
			double deposit_payment_total )
{
	double d;
	double gain_donation;

	if ( ( d = deposit_remaining(
			deposit_amount,
			deposit_payment_total ) ) > 0 )
	{
		gain_donation = d;
	}
	else
	{
		gain_donation = 0.0;
	}

	return gain_donation;
}

FILE *deposit_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement t=%s f=\"%s\" replace=n delimiter='%c'|"
		"sql 2>&1						|"
		"cat >%s						 ",
		DEPOSIT_TABLE,
		DEPOSIT_INSERT_COLUMNS,
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void deposit_insert_pipe(
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

void deposit_list_insert( LIST *deposit_list )
{
	DEPOSIT *deposit;
	FILE *insert_pipe;
	char *error_filename;
	char sys_string[ 1024 ];

	if ( !list_rewind( deposit_list ) ) return;

	insert_pipe =
		deposit_insert_open(
			( error_filename =
				timlib_tmpfile() ) );

	do {
		deposit = list_get( deposit_list );

		deposit_insert_pipe(
			insert_pipe,
			deposit->payor_entity->full_name,
			deposit->payor_entity->street_address,
			deposit->semester->season_name,
			deposit->semester->year,
			deposit->deposit_date_time,
			deposit->deposit_amount,
			deposit->transaction_fee,
			deposit->net_revenue,
			deposit->account_balance,
			deposit->transaction_ID,
			deposit->invoice_number,
			deposit->from_email_address );

	} while ( list_next( deposit_list ) );

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

void deposit_list_payment_insert(
			LIST *deposit_list )
{
	DEPOSIT *deposit;

	if ( !list_rewind( deposit_list ) ) return;

	do {
		deposit = list_get( deposit_list );

		tuition_payment_list_insert(
			deposit->deposit_tuition_payment_list );

		program_payment_list_insert(
			deposit->deposit_program_payment_list );

	} while ( list_next( deposit_list ) );
}

void deposit_list_enrollment_insert(
			LIST *deposit_list )
{
	DEPOSIT *deposit;

	if ( !list_rewind( deposit_list ) ) return;

	do {
		deposit = list_get( deposit_list );

		tuition_payment_list_enrollment_insert(
			deposit->deposit_tuition_payment_list );

	} while ( list_next( deposit_list ) );
}

void deposit_list_registration_insert(
			LIST *deposit_list )
{
	DEPOSIT *deposit;

	if ( !list_rewind( deposit_list ) ) return;

	do {
		deposit = list_get( deposit_list );

		tuition_payment_list_registration_insert(
			deposit->deposit_tuition_payment_list );

	} while ( list_next( deposit_list ) );
}

void deposit_list_student_insert(
			LIST *deposit_list )
{
	DEPOSIT *deposit;

	if ( !list_rewind( deposit_list ) ) return;

	do {
		deposit = list_get( deposit_list );

		tuition_payment_list_student_insert(
			deposit->deposit_tuition_payment_list );

	} while ( list_next( deposit_list ) );
}

void deposit_list_student_entity_insert(
			LIST *deposit_list )
{
	DEPOSIT *deposit;

	if ( !list_rewind( deposit_list ) ) return;

	do {
		deposit = list_get( deposit_list );

		tuition_payment_list_student_entity_insert(
			deposit->deposit_tuition_payment_list );

	} while ( list_next( deposit_list ) );
}

void deposit_list_payor_entity_insert(
			LIST *deposit_list )
{
	DEPOSIT *deposit;

	if ( !list_rewind( deposit_list ) ) return;

	do {
		deposit = list_get( deposit_list );

		tuition_payment_list_payor_entity_insert(
			deposit->deposit_tuition_payment_list );

	} while ( list_next( deposit_list ) );
}

void deposit_list_program_payment_trigger(
			LIST *deposit_list )
{
	DEPOSIT *deposit;

	if ( !list_rewind( deposit_list ) ) return;

	do {
		deposit = list_get( deposit_list );

		if ( list_rewind( deposit->deposit_program_payment_list ) )
		{
			program_payment_list_trigger(
				deposit->deposit_program_payment_list );
		}
	} while ( list_next( deposit_list ) );
}

void deposit_list_tuition_payment_trigger(
			LIST *deposit_list )
{
	DEPOSIT *deposit;

	if ( !list_rewind( deposit_list ) ) return;

	do {
		deposit = list_get( deposit_list );

		if ( list_length( deposit->deposit_tuition_payment_list ) )
		{
			tuition_payment_list_trigger(
				deposit->deposit_tuition_payment_list );
		}
	} while ( list_next( deposit_list ) );
}

void deposit_list_enrollment_trigger(
			LIST *deposit_list )
{
	DEPOSIT *deposit;

	if ( !list_rewind( deposit_list ) ) return;

	do {
		deposit = list_get( deposit_list );

		if ( !list_rewind( deposit->deposit_tuition_payment_list ) )
		{
			continue;
		}

		tuition_payment_list_enrollment_trigger(
			deposit->deposit_tuition_payment_list );

	} while ( list_next( deposit_list ) );
}

LIST *deposit_course_name_list(
			LIST *deposit_list )
{
	DEPOSIT *deposit;
	LIST *course_name_list;

	if ( !list_rewind( deposit_list ) ) return (LIST *)0;

	course_name_list = list_new();

	do {
		deposit = list_get( deposit_list );

		list_append_list(
			course_name_list,
			enrollment_course_name_list(
				deposit_enrollment_list(
					deposit->
					     deposit_tuition_payment_list ) ) );

	} while ( list_next( deposit_list ) );

	return course_name_list;
}

LIST *deposit_program_payment_list(
			char *item_title_P,
			LIST *education_program_list,
			DEPOSIT *deposit )
{
	return program_payment_list(
			item_title_P,
			education_program_list,
			deposit );
}

LIST *deposit_tuition_payment_list(
			char *season_name,
			int year,
			char *item_title_P,
			DEPOSIT *deposit )
{
	return tuition_payment_list(
			season_name,
			year,
			item_title_P,
			deposit );
}

LIST *deposit_registration_list(
			LIST *deposit_tuition_payment_list )
{
	return tuition_payment_registration_list(
			deposit_tuition_payment_list );
}

LIST *deposit_enrollment_list(
			LIST *deposit_payment_list )
{
	return tuition_payment_enrollment_list(
			deposit_payment_list );
}

void deposit_trigger(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			char *state )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
	"deposit_trigger \"%s\" \"%s\" \"%s\" %d \"%s\" %s",
		entity_escape_full_name( payor_full_name ),
		payor_street_address,
		season_name,
		year,
		deposit_date_time,
		state );

	if ( system( sys_string ) ){}
}

LIST *deposit_transaction_list(
			LIST *deposit_list )
{
	DEPOSIT *deposit;
	LIST *transaction_list;

	if ( !list_rewind( deposit_list ) ) return (LIST *)0;

	transaction_list = list_new();

	do {
		deposit = list_get( deposit_list );

		list_append_list(
			transaction_list,
			tuition_payment_transaction_list(
				deposit->deposit_tuition_payment_list ) );

		list_append_list(
			transaction_list,
			program_payment_transaction_list(
				deposit->deposit_program_payment_list ) );

	} while ( list_next( deposit_list ) );

	return transaction_list;
}

LIST *deposit_list_steady_state(
			LIST *deposit_list )
{
	DEPOSIT *deposit;

	if ( !list_rewind( deposit_list ) ) return deposit_list;

	do {
		deposit = list_get( deposit_list );

		if ( !deposit->deposit_registration_list )
		{
			deposit->deposit_registration_list =
				deposit_registration_list(
					deposit->
						deposit_tuition_payment_list );
		}

		deposit->deposit_tuition_payment_list =
			tuition_payment_list_steady_state(
				deposit->deposit_tuition_payment_list,
				deposit->deposit_registration_list,
				deposit->deposit_amount,
				deposit->transaction_fee );

		deposit->deposit_program_payment_list =
			program_payment_list_steady_state(
				deposit->deposit_program_payment_list,
				deposit->deposit_amount,
				deposit->transaction_fee,
				deposit->net_revenue
					/* net_payment_amount */ );

		deposit =
			deposit_steady_state(
				deposit,
				deposit->deposit_amount,
				deposit->transaction_fee,
				deposit->deposit_tuition_payment_list,
				deposit->deposit_program_payment_list );

	} while ( list_next( deposit_list ) );

	return deposit_list;
}

LIST *deposit_not_exists_course_name_list(
			LIST *deposit_list )
{
	DEPOSIT *deposit;
	LIST *course_name_list = list_new();

	if ( !list_rewind( deposit_list ) ) return (LIST *)0;

	do {
		deposit = list_get( deposit_list );

		course_name_list =
			tuition_payment_not_exists_course_name_list(
				course_name_list,
				deposit->deposit_tuition_payment_list );

	} while ( list_next( deposit_list ) );

	return course_name_list;
}

