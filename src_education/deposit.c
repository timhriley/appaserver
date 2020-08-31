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
#include "payment.h"
#include "deposit.h"

LIST *deposit_payment_list(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time )
{
	return	payment_system_list(
			payment_sys_string(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				deposit_primary_where(
					payor_full_name,
					payor_street_address,
					season_name,
					year,
					deposit_date_time ) ) );
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
			LIST *deposit_registration_list )
{
	return 0.0;
/*
	return		deposit_amount -
			sum( B->registration->registration_tuition() );
*/
}

double deposit_total( LIST *deposit_payment_list )
{
	/* return sum( payment_list->payment->payment_amount ); */
	return 0.0;
}

double deposit_net_revenue(
			double deposit_amount,
			double transaction_fee )
{
	return deposit_amount - transaction_fee;
}

void deposit_insert(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			double deposit_total )
{
}


double deposit_gain_donation_amount(
			double deposit_amount,
			LIST *deposit_registration_list )
{
	double remaining;
	double donation_amount;

	if ( ( remaining =
			deposit_remaining(
				deposit_amount,
				deposit_registration_list ) ) > 0 )
	{
		donation_amount = remaining;
	}
	else
	{
		donation_amount = 0.0;
	}
	return donation_amount;
}

char *deposit_primary_where(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			char *year,
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

	deposit->payor_full_name = payor_full_name;
	deposit->payor_street_address = payor_street_address;
	deposit->season_name = season_name;
	deposit->year = year;
	deposit->deposit_date_time = deposit_date_time;

	return deposit;
}

DEPOSIT *deposit_parse( char *input )
{
	char payor_full_name[ 128 ];
	char payor_street_address[ 128 ];
	char season_name[ 128 ];
	char year[ 128 ];
	char deposit_date_time[ 128 ];
	DEPOSIT *deposit;

	if ( !input || !*input ) return (DEPOSIT *)0;

	piece( payor_full_name, SQL_DELIMITER, input, 0 );
	piece( payor_street_address, SQL_DELIMITER, input, 0 );
	piece( season_name, SQL_DELIMITER, input, 0 );
	piece( year, SQL_DELIMITER, input, 0 );
	piece( deposit_date_time, SQL_DELIMITER, input, 0 );

	deposit =
		deposit_new(
			strdup( payor_full_name ),
			strdup( payor_street_address ),
			strdup( season_name ),
			atoi( year ),
			strdup( deposit_date_time ) );

	return deposit;
}

char *deposit_sys_string(
			char *where )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"select.sh '*' %s \"%s\" select",
		DEPOSIT_TABLE,
		where );

	return strdup( sys_string );
}

LIST *deposit_system_list( char *sys_string )
{
	char input[ 1024 ];
	FILE *input_pipe = popen( sys_string, "r" );
	LIST *list = list_new();

	while( string_input( input, input_pipe, 1024 ) )
	{
		list_set( list, deposit_parse( input ) );
	}
	return list;
}

DEPOSIT *deposit_fetch(	char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time )
{
	return	deposit_parse(
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
						deposit_date_time ) ) ) );
}

