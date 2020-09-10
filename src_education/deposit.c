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
#include "payment.h"
#include "payment_fns.h"
#include "deposit.h"

LIST *deposit_payment_list(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_deposit,
			boolean fetch_enrollment )
{
	LIST *payment_list;

	payment_list =
		payment_system_list(
			payment_sys_string(
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

	return payment_list;
}

LIST *deposit_registration_list(
			LIST *deposit_payment_list )
{
	PAYMENT *payment;
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
	char check_number[ 128 ];
	char transaction_ID[ 128 ];
	char invoice_number[ 128 ];
	char payment_total[ 128 ];
	char gain_donation[ 128 ];
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

	piece( check_number, SQL_DELIMITER, input, 9 );
	deposit->check_number = atoi( account_balance );

	piece( transaction_ID, SQL_DELIMITER, input, 10 );
	deposit->transaction_ID = strdup( transaction_ID );

	piece( invoice_number, SQL_DELIMITER, input, 11 );
	deposit->invoice_number = strdup( invoice_number );

	piece( payment_total, SQL_DELIMITER, input, 12 );
	deposit->deposit_payment_total = atof( payment_total );

	piece( gain_donation, SQL_DELIMITER, input, 13 );
	deposit->deposit_gain_donation = atof( gain_donation );

	if ( fetch_payment_list )
	{
		deposit->deposit_payment_list =
			deposit_payment_list(
				payor_full_name,
				payor_street_address,
				season_name,
				deposit->semester->year,
				deposit_date_time,
				0 /* not fetch_deposit */,
				0 /* not fetch_enrollment */ );
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

FILE *deposit_insert_open( void )
{
	char sys_string[ 1024 ];
	char *field;

	field=
		"payor_full_name,"
		"payor_street_address,"
		"season_name,"
		"year,"
		"deposit_date_time,"
		"deposit_amount,"
		"transaction_fee,"
		"net_revenue,"
		"account_balance,"
		"check_number,"
		"transaction_ID,"
		"invoice_number,"
		"payment_total,"
		"gain_donation";

	sprintf( sys_string,
		 "insert_statement table=%s field=%s delimiter='^'	|"
		 "sql							 ",
		 DEPOSIT_TABLE,
		 field );

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
			int check_number,
			char *transaction_ID,
			char *invoice_number,
			double payment_total,
			double gain_donation )
{
	char *format =
		"%s^%s^%s^%d^%s^%.2lf^%.2lf^%.2lf^%.2lf^%d^%s^%s^%.2lf^%.2lf\n";

	fprintf(	insert_pipe,
			format,
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
			check_number,
			transaction_ID,
			invoice_number,
			payment_total,
			gain_donation );
}

void deposit_insert(	char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			double deposit_amount,
			double transaction_fee,
			double net_revenue,
			double account_balance,
			int check_number,
			char *transaction_ID,
			char *invoice_number,
			double payment_total,
			double gain_donation )
{
	FILE *insert_pipe = deposit_insert_open();

	deposit_insert_pipe(
		insert_pipe,
		payor_full_name,
		payor_street_address,
		season_name,
		year,
		deposit_date_time,
		deposit_amount,
		transaction_fee,
		net_revenue,
		account_balance,
		check_number,
		transaction_ID,
		invoice_number,
		payment_total,
		gain_donation );

	pclose( insert_pipe );
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

fprintf(stderr,
	"%s/%s()/%d: deposit_payment_total = %.2lf\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
deposit->deposit_payment_total );

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
	PAYMENT *payment;
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

