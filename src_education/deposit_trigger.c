/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/deposit_trigger.c	*/
/* ---------------------------------------------------- */
/* 							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "semester.h"
#include "deposit.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void deposit_trigger(
				char *payor_full_name,
				char *payor_street_address,
				char *season_name,
				int year,
				char *deposit_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *payor_full_name;
	char *payor_street_address;
	char *season_name;
	int year;
	char *deposit_date_time;
	char *state;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s payor_full_name payor_street_address season_name year deposit_date_time state\n",
			 argv[ 0 ] );
		fprintf( stderr,
			 "state in {insert,update,delete,payment}\n" );
		exit ( 1 );
	}

	payor_full_name = argv[ 1 ];
	payor_street_address = argv[ 2 ];
	season_name = argv[ 3 ];
	year = atoi( argv[ 4 ] );
	deposit_date_time = argv[ 5 ];
	state = argv[ 6 ];

	if ( !*season_name
	||   strcmp( season_name, "season_name" ) == 0 )
	{
		exit( 0 );
	}

	if ( !year ) exit( 0 );

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) ==  0
	||   strcmp( state, "payment" ) ==  0 )
	{
		deposit_trigger(
			payor_full_name,
			payor_street_address,
			season_name,
			year,
			deposit_date_time );
	}
	return 0;
}

void deposit_trigger(
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time )
{
	DEPOSIT *deposit;

	if ( ! ( deposit =
			deposit_fetch(
				payor_full_name,
				payor_street_address,
				season_name,
				year,
				deposit_date_time,
				1 /* fetch_payment_list */ ) ) )
	{
		return;
	}

	if ( !deposit->payor_entity )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: deposit_fetch() returned empty payor_entity.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !deposit->semester )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: deposit_fetch() returned empty semester.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( deposit =
			deposit_steady_state(
				deposit->deposit_amount,
				deposit->transaction_fee,
				deposit->deposit_payment_list,
				/* ----------------------------- */
				/* Don't take anything from here */
				/* ----------------------------- */
				deposit ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: deposit_steady_state() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !deposit->payor_entity )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: deposit_steady_state() returned empty payor_entity.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !deposit->semester )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: deposit_steady_state() returned empty semester.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	deposit_update(
			deposit->deposit_payment_total,
			deposit->deposit_net_revenue,
			deposit->payor_entity->full_name,
			deposit->payor_entity->street_address,
			deposit->semester->season_name,
			deposit->semester->year,
			deposit->deposit_date_time );
}

