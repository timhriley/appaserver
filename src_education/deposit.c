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
#include "tuition_payment.h"
#include "program_payment.h"
#include "tuition_payment_fns.h"
#include "enrollment_fns.h"
#include "deposit.h"

LIST *deposit_fetch_program_payment_list(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_program )
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
			fetch_program );
}

LIST *deposit_fetch_tuition_payment_list(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_deposit,
			boolean fetch_enrollment,
			boolean fetch_transaction )
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
			fetch_enrollment,
			fetch_transaction );
}

LIST *deposit_registration_list(
			LIST *deposit_payment_list )
{
	TUITION_PAYMENT *payment;
	LIST *registration_list;

	if ( !list_rewind( deposit_payment_list ) ) return (LIST *)0;

	registration_list = list_new();

	do {
		payment =
			list_get(
				deposit_payment_list );

		if ( !payment->enrollment )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty enrollment.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		if ( !payment->enrollment->registration )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: empty registration.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		list_set(
			registration_list,
			payment->enrollment->registration );

	} while ( list_next( deposit_payment_list ) );

	return registration_list;
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
			boolean fetch_payment_list )
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
	char payment_total[ 128 ];
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

	piece( payment_total, SQL_DELIMITER, input, 12 );
	deposit->deposit_payment_total = atof( payment_total );

	if ( fetch_payment_list )
	{
		deposit->deposit_tuition_payment_list =
			deposit_fetch_tuition_payment_list(
				payor_full_name,
				payor_street_address,
				season_name,
				deposit->semester->year,
				deposit_date_time,
				0 /* not fetch_deposit */,
				0 /* not fetch_enrollment */ );

		deposit->deposit_program_payment_list =
			deposit_fetch_program_payment_list(
				payor_full_name,
				payor_street_address,
				season_name,
				deposit->semester->year,
				deposit_date_time,
				1 /* fetch_program */ );
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
			boolean fetch_payment_list )
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
				fetch_payment_list ) );
	}
	pclose( input_pipe );
	return list;
}

DEPOSIT *deposit_fetch(	char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_payment_list )
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
			fetch_payment_list );
	return deposit;
}

double deposit_payment_total(
			LIST *deposit_payment_list )
{
	return payment_total( deposit_payment_list );
}

DEPOSIT *deposit_steady_state(
			double deposit_amount,
			double transaction_fee,
			LIST *deposit_payment_list,
			DEPOSIT *deposit )
{
	deposit->deposit_registration_list =
		deposit_registration_list(
			deposit_payment_list );

	deposit->deposit_payment_total =
		deposit_payment_total(
			deposit_payment_list );

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
			double deposit_payment_total,
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
		 "%s^%s^%s^%d^%s^payment_total^%.2lf\n",
		 payor_full_name,
		 payor_street_address,
		 season_name,
		 year,
		 deposit_date_time,
		 deposit_payment_total );

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

LIST *deposit_enrollment_list(
			LIST *deposit_payment_list )
{
	TUITION_PAYMENT *payment;
	LIST *enrollment_list;

	if ( !list_rewind( deposit_payment_list ) ) return (LIST *)0;

	enrollment_list = list_new();

	do {
		payment =
			list_get(
				deposit_payment_list );

		if ( !payment->enrollment )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: empty enrollment.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		list_set(
			enrollment_list,
			payment->enrollment );

	} while ( list_next( deposit_payment_list ) );

	return enrollment_list;
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

		payment_list_insert( deposit->deposit_payment_list );

	} while ( list_next( deposit_list ) );
}

void deposit_list_enrollment_insert(
			LIST *deposit_list )
{
	DEPOSIT *deposit;

	if ( !list_rewind( deposit_list ) ) return;

	do {
		deposit = list_get( deposit_list );

		payment_list_enrollment_insert(
			deposit->deposit_payment_list );

	} while ( list_next( deposit_list ) );
}

void deposit_list_registration_insert(
			LIST *deposit_list )
{
	DEPOSIT *deposit;

	if ( !list_rewind( deposit_list ) ) return;

	do {
		deposit = list_get( deposit_list );

		payment_list_registration_insert(
			deposit->deposit_payment_list );

	} while ( list_next( deposit_list ) );
}

void deposit_list_offering_insert(
			LIST *deposit_list )
{
	DEPOSIT *deposit;

	if ( !list_rewind( deposit_list ) ) return;

	do {
		deposit = list_get( deposit_list );

		payment_list_offering_insert(
			deposit->deposit_payment_list );

	} while ( list_next( deposit_list ) );
}

void deposit_list_course_insert(
			LIST *deposit_list )
{
	DEPOSIT *deposit;

	if ( !list_rewind( deposit_list ) ) return;

	do {
		deposit = list_get( deposit_list );

		payment_list_course_insert(
			deposit->deposit_payment_list );

	} while ( list_next( deposit_list ) );
}

void deposit_list_student_insert(
			LIST *deposit_list )
{
	DEPOSIT *deposit;

	if ( !list_rewind( deposit_list ) ) return;

	do {
		deposit = list_get( deposit_list );

		payment_list_student_insert(
			deposit->deposit_payment_list );

	} while ( list_next( deposit_list ) );
}

void deposit_list_student_entity_insert(
			LIST *deposit_list )
{
	DEPOSIT *deposit;

	if ( !list_rewind( deposit_list ) ) return;

	do {
		deposit = list_get( deposit_list );

		payment_list_student_entity_insert(
			deposit->deposit_payment_list );

	} while ( list_next( deposit_list ) );
}

void deposit_list_payor_entity_insert(
			LIST *deposit_list )
{
	DEPOSIT *deposit;

	if ( !list_rewind( deposit_list ) ) return;

	do {
		deposit = list_get( deposit_list );

		payment_list_payor_entity_insert(
			deposit->deposit_payment_list );

	} while ( list_next( deposit_list ) );
}

void deposit_list_program_payment_trigger(
			char *season_name,
			int year,
			LIST *deposit_list )
{
	DEPOSIT *deposit;
	PROGRAM_PAYMENT *program_payment;

	if ( !list_rewind( deposit_list ) ) return;

	do {
		deposit = list_get( deposit_list );

		if ( !list_rewind( deposit->deposit_program_payment_list ) )
		{
			continue;
		}

		do {
			program_payment =
				list_get(
					deposit->
						deposit_program_payment_list );

			deposit_program_payment_trigger(
				program_payment->
					program_name,
				program_payment->
					payor_full_name,
				program_payment->
					payor_street_address,
				season_name,
				year,
				program_payment->
					deposit_date_time );

		} while ( list_next( deposit->deposit_program_payment_list ) );

	} while ( list_next( deposit_list ) );
}

void deposit_list_tuition_payment_trigger(
			char *season_name,
			int year,
			LIST *deposit_list )
{
	DEPOSIT *deposit;
	TUITION_PAYMENT *payment;

	if ( !list_rewind( deposit_list ) ) return;

	do {
		deposit = list_get( deposit_list );

		if ( !list_rewind( deposit->deposit_payment_list ) )
		{
			continue;
		}

		do {
			payment =
				list_get(
					deposit->deposit_payment_list );

			deposit_tuition_payment_trigger(
				payment->
					enrollment->
					registration->
					student_full_name,
				payment->
					enrollment->
					registration->
					street_address,
				payment->
					enrollment->
					offering->
					course->
					course_name,
				season_name,
				year,
				payment->
					deposit->
					payor_entity->
					full_name,
				payment->
					deposit->
					payor_entity->
					street_address,
				payment->
					deposit->
					deposit_date_time );

		} while ( list_next( deposit->deposit_payment_list ) );

	} while ( list_next( deposit_list ) );
}

void deposit_list_enrollment_trigger(
			char *season_name,
			int year,
			LIST *deposit_list )
{
	DEPOSIT *deposit;
	TUITION_PAYMENT *payment;

	if ( !list_rewind( deposit_list ) ) return;

	do {
		deposit = list_get( deposit_list );

		if ( !list_rewind( deposit->deposit_payment_list ) )
		{
			continue;
		}

		do {
			payment =
				list_get(
					deposit->deposit_payment_list );

			deposit_enrollment_trigger(
				payment->
					enrollment->
					registration->
					student_full_name,
				payment->
					enrollment->
					registration->
					street_address,
				payment->
					enrollment->
					offering->
					course->
					course_name,
				season_name,
				year );

		} while ( list_next( deposit->deposit_payment_list ) );

	} while ( list_next( deposit_list ) );
}

void deposit_program_payment_trigger(
			char *program_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
"program_payment_trigger \"%s\" \"%s\" '%s' '%s' %d '%s' deposit",
		program_name,
		payor_full_name,
		payor_street_address,
		season_name,
		year,
		deposit_date_time );

	if ( system( sys_string ) ){}
}

void deposit_tuition_payment_trigger(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year,
			char *payor_full_name,
			char *payor_street_address,
			char *deposit_date_time )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
"tuition_payment_trigger \"%s\" '%s' \"%s\" '%s' %d \"%s\" '%s' '%s' deposit",
		student_full_name,
		street_address,
		course_name,
		season_name,
		year,
		payor_full_name,
		payor_street_address,
		deposit_date_time );

	if ( system( sys_string ) ){}
}

void deposit_enrollment_trigger(
			char *student_full_name,
			char *street_address,
			char *course_name,
			char *season_name,
			int year )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
	"enrollment_trigger \"%s\" '%s' \"%s\" '%s' %d insert",
		student_full_name,
		street_address,
		course_name,
		season_name,
		year );

	if ( system( sys_string ) ){}
}

LIST *deposit_course_name_list(
			LIST *deposit_list )
{
	DEPOSIT *deposit;
	LIST *course_name_list;

	if ( !list_rewind( deposit_list ) ) return (LIST *)0;

	course_name_list = list_new();

	do {
		deposit =
			list_get(
				deposit_list );

		list_append_list(
			course_name_list,
			enrollment_course_name_list(
				deposit_enrollment_list(
					deposit->deposit_payment_list ) ) );

	} while ( list_next( deposit_list ) );

	return course_name_list;
}

LIST *deposit_program_payment_list(
			char *season_name,
			int year,
			char *item_title_P,
			double gross_revenue_H,
			DEPOSIT *deposit )
{
	LIST *payment_list = list_new();
	PROGRAM_PAYMENT *payment;
	int program_number;

	for (	program_number = 1;
		( payment =
			deposit_program_payment(
				season_name,
				year,
				item_title_P,
				gross_revenue_H,
				program_number,
				deposit ) );
		program_number++ )
	{
		list_set( payment_list, payment );
	}

	return payment_list;
}

LIST *deposit_tuition_payment_list(
			char *season_name,
			int year,
			char *item_title_P,
			double gross_revenue_H,
			DEPOSIT *deposit )
{
	LIST *payment_list = list_new();
	TUITION_PAYMENT *payment;
	int student_number;

	for (	student_number = 1;
		( payment =
			deposit_tuition_payment(
				season_name,
				year,
				item_title_P,
				gross_revenue_H,
				student_number,
				deposit ) );
		student_number++ )
	{
		list_set( payment_list, payment );
	}

	return payment_list;
}

TUITION_PAYMENT *deposit_tuition_payment(
			char *season_name,
			int year,
			char *item_title_P,
			double gross_revenue_H,
			int student_number,
			/* -------- */
			/* Set only */
			/* -------- */
			DEPOSIT *deposit )
{
	TUITION_PAYMENT *payment;
	TUITION_PAYMENT_ITEM_TITLE *payment_item_title;

	if ( ! ( payment_item_title =
			tuition_payment_item_title_new(
				item_title_P,
				student_number ) ) )
	{
		return (TUITION_PAYMENT *)0;
	}

	if ( ! ( payment_item_title->
			payment_item_title_entity =
				payment_item_title_entity(
				item_title_P,
				student_number ) ) )
	{
		return (TUITION_PAYMENT *)0;
	}

	if ( ! ( payment_item_title->
			payment_item_title_course_name =
				payment_item_title_course_name(
					item_title_P,
					student_number ) ) )
	{
		return (TUITION_PAYMENT *)0;
	}

	/* New payment */
	/* ----------- */
	payment = payment_calloc();

	/* Build enrollment */
	/* ---------------- */
	payment->enrollment =
		enrollment_new(
			payment_item_title->
				payment_item_title_entity->
				full_name,
			payment_item_title->
				payment_item_title_entity->
				street_address,
			payment_item_title->
				payment_item_title_course_name,
			season_name,
			year );

	/* Build offering */
	/* -------------- */
	payment->enrollment->offering =
		offering_new(
			payment_item_title->
				payment_item_title_course_name,
			season_name,
			year );

	/* Build course */
	/* ------------ */
	payment->enrollment->offering->course =
		course_new(
			payment_item_title->
				payment_item_title_course_name );

	/* Not sure of the best way to ensure accurate course_price */
	/* -------------------------------------------------------- */
	if ( student_number == 1 )
	{
		payment->enrollment->offering->course->course_price =
			gross_revenue_H;
	}

	/* Build registration */
	/* ------------------ */
	payment->enrollment->registration =
		registration_new(
			payment_item_title->
				payment_item_title_entity->
				full_name,
			payment_item_title->
				payment_item_title_entity->
				street_address,
			season_name,
			year );

	/* Set deposit */
	/* ----------- */
	payment->deposit = deposit;

	return payment;
}

PROGRAM_PAYMENT *deposit_program_payment(
			char *season_name,
			int year,
			char *item_title_P,
			double gross_revenue_H,
			int program_number,
			/* -------- */
			/* Set only */
			/* -------- */
			DEPOSIT *deposit )
{
	PROGRAM_PAYMENT *payment;
	PROGRAM_PAYMENT_ITEM_TITLE *payment_item_title;

	if ( ! ( payment_item_title =
			program_payment_item_title_new(
				item_title_P,
				program_number ) ) )
	{
		return (PROGRAM_PAYMENT *)0;
	}

	/* New payment */
	/* ----------- */
	payment = program_payment_calloc();

	/* Build enrollment */
	/* ---------------- */
	payment->enrollment =
		enrollment_new(
			payment_item_title->
				payment_item_title_entity->
				full_name,
			payment_item_title->
				payment_item_title_entity->
				street_address,
			payment_item_title->
				payment_item_title_course_name,
			season_name,
			year );

	/* Build offering */
	/* -------------- */
	payment->enrollment->offering =
		offering_new(
			payment_item_title->
				payment_item_title_course_name,
			season_name,
			year );

	/* Build course */
	/* ------------ */
	payment->enrollment->offering->course =
		course_new(
			payment_item_title->
				payment_item_title_course_name );

	/* Not sure of the best way to ensure accurate course_price */
	/* -------------------------------------------------------- */
	if ( student_number == 1 )
	{
		payment->enrollment->offering->course->course_price =
			gross_revenue_H;
	}

	/* Build registration */
	/* ------------------ */
	payment->enrollment->registration =
		registration_new(
			payment_item_title->
				payment_item_title_entity->
				full_name,
			payment_item_title->
				payment_item_title_entity->
				street_address,
			season_name,
			year );

	/* Set deposit */
	/* ----------- */
	payment->deposit = deposit;

	return payment;
}

