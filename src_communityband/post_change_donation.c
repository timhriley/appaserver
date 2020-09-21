/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_communityband/post_change_donation.c		*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "donation.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_donation_date(
				char *application_name,
				char *full_name,
				char *street_address,
				char *donation_date,
				char *preupdate_donation_date );

void post_change_donation_entity(
				char *application_name,
				char *full_name,
				char *street_address,
				char *donation_date,
				char *preupdate_full_name,
				char *preupdate_street_address );

void post_change_donation_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *donation_date,
				char *preupdate_full_name,
				char *preupdate_street_address,
				char *preupdate_donation_date );

void post_change_donation_delete(
				char *application_name,
				char *full_name,
				char *street_address,
				char *donation_date );

void post_change_donation_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *donation_date );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *donation_date;
	char *state;
	char *preupdate_full_name;
	char *preupdate_street_address;
	char *preupdate_donation_date;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 9 )
	{
		fprintf( stderr,
"Usage: %s ignored full_name street_address donation_date state preupdate_full_name preupdate_street_address preupdate_donation_date\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	donation_date = argv[ 4 ];
	state = argv[ 5 ];
	preupdate_full_name = argv[ 6 ];
	preupdate_street_address = argv[ 7 ];
	preupdate_donation_date = argv[ 8 ];

	/* ------------------------------------ */
	/* Execute on predelete because there's	*/
	/* DONATION.transaction_date_time.	*/
	/* ------------------------------------ */
	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_donation_insert(
				application_name,
				full_name,
				street_address,
				donation_date );
	}
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_donation_update(
				application_name,
				full_name,
				street_address,
				donation_date,
				preupdate_full_name,
				preupdate_street_address,
				preupdate_donation_date );
	}
	else
	if ( strcmp( state, "predelete" ) == 0 )
	{
		post_change_donation_delete(
				application_name,
				full_name,
				street_address,
				donation_date );
	}

	return 0;

} /* main() */

void post_change_donation_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *donation_date )
{
	DONATION *donation;

	if ( !( donation =
			donation_fetch(
				application_name,
				full_name,
				street_address,
				donation_date ) ) )
	{
		return;
	}

	if ( !donation->total_donation_amount ) return;

	donation->transaction_date_time =
		ledger_get_transaction_date_time(
			donation->donation_date );

	donation->transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			donation->transaction_date_time,
			DONATION_MEMO );
	
	ledger_transaction_insert(
		application_name,
		donation->transaction->full_name,
		donation->transaction->street_address,
		donation->transaction->transaction_date_time,
		donation->total_donation_amount /* transaction_amount */,
		donation->transaction->memo,
		donation->check_number,
		1 /* lock_transaction */ );

	donation_update(
			application_name,
			donation->full_name,
			donation->street_address,
			donation->donation_date,
			donation->total_donation_amount,
			donation->database_total_donation_amount,
			donation->transaction_date_time,
			donation->database_transaction_date_time );
}

void post_change_donation_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *donation_date )
{
	DONATION *donation;

	if ( !( donation =
			donation_fetch(
				application_name,
				full_name,
				street_address,
				donation_date ) ) )
	{
		return;
	}

	ledger_delete(	application_name,
			TRANSACTION_FOLDER_NAME,
			full_name,
			street_address,
			donation->transaction_date_time );

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			full_name,
			street_address,
			donation->transaction_date_time );

	donation_journal_ledger_refresh_and_propagate(
		application_name,
		full_name,
		street_address,
		donation->transaction_date_time,
		donation->donation_fund_list,
		1 /* propagate_only */ );

} /* post_change_donation_delete() */

void post_change_donation_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *donation_date,
				char *preupdate_full_name,
				char *preupdate_street_address,
				char *preupdate_donation_date )
{
	enum preupdate_change_state full_name_change_state;
	enum preupdate_change_state street_address_change_state;
	enum preupdate_change_state donation_date_change_state;

	full_name_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_full_name,
			full_name /* postupdate_data */,
			"preupdate_full_name" );

	street_address_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_street_address,
			street_address /* postupdate_data */,
			"preupdate_street_address" );

	donation_date_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_donation_date,
			donation_date /* postupdate_data */,
			"preupdate_donation_date" );

	if ( full_name_change_state == from_something_to_something_else
	||   street_address_change_state == from_something_to_something_else )
	{
		post_change_donation_entity(
			application_name,
			full_name,
			street_address,
			donation_date,
			preupdate_full_name,
			preupdate_street_address );
	}

	if ( donation_date_change_state == from_something_to_something_else )
	{
		post_change_donation_date(
			application_name,
			full_name,
			street_address,
			donation_date,
			preupdate_donation_date );
	}

#ifdef NOT_DEFINED
	DONATION *donation;
	char *propagate_transaction_date_time;
	donation =
		donation_fetch(
			application_name,
			full_name,
			street_address,
			donation_date );

	if ( !donation )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot find donation.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	if ( !donation->transaction )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty transaction for donation.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	donation->transaction_date_time =
		ledger_get_transaction_date_time(
			donation->donation_date );

	ledger_transaction_generic_update(
		application_name,
		full_name,
		street_address,
		donation->transaction->transaction_date_time,
		"transaction_date_time" /* attribute */,
		donation->transaction_date_time /* data */ );

	ledger_journal_generic_update(
		application_name,
		full_name,
		street_address,
		donation->transaction->transaction_date_time,
		"transaction_date_time" /* attribute */,
		donation->transaction_date_time /* data */ );

	donation_update(
			application_name,
			donation->full_name,
			donation->street_address,
			donation->donation_date,
			donation->total_donation_amount,
			donation->database_total_donation_amount,
			donation->transaction_date_time,
			donation->database_transaction_date_time );

	if ( strcmp(	preupdate_donation_date,
			donation_date ) < 0 )
	{
		propagate_transaction_date_time =
			donation->transaction->transaction_date_time;
	}
	else
	{
		propagate_transaction_date_time =
			donation->transaction_date_time;
	}

	donation_journal_ledger_refresh_and_propagate(
			application_name,
			donation->full_name,
			donation->street_address,
			propagate_transaction_date_time,
			donation->donation_fund_list,
			1 /* propagate_only */ );
#endif

} /* post_change_donation_update() */

void post_change_donation_entity(
			char *application_name,
			char *full_name,
			char *street_address,
			char *donation_date,
			char *preupdate_full_name,
			char *preupdate_street_address )
{
	DONATION *donation;

	/* ------------------------------------------------------------ */
	/* Expect donation_fetch() to not fully load the transaction	*/
	/* because the DONATION.entity changed to something other	*/
	/* than its corresponding TRANSACTION.entity.			*/
	/* ------------------------------------------------------------ */
	fprintf( stderr,
		 "Warning in %s/%s()/%d: ignore following warning.\n",
		 __FILE__,
		 __FUNCTION__,
		 __LINE__ );

	donation =
		donation_fetch(
			application_name,
			full_name,
			street_address,
			donation_date );

	if ( !donation )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot find donation.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	if ( !donation->transaction )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty transaction for donation.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	ledger_entity_update(
		application_name,
		full_name,
		street_address,
		donation->transaction_date_time,
		preupdate_full_name,
		preupdate_street_address );

} /* post_change_donation_entity() */

void post_change_donation_date(
			char *application_name,
			char *full_name,
			char *street_address,
			char *donation_date,
			char *preupdate_donation_date )
{
	DONATION *donation;
	char *propagate_transaction_date_time;

	donation =
		donation_fetch(
			application_name,
			full_name,
			street_address,
			donation_date );

	if ( !donation )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot find donation.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	if ( !donation->transaction )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty transaction for donation.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	donation->transaction_date_time =
		ledger_get_transaction_date_time(
			donation->donation_date );

	/* Update DONATION.transaction_date_time */
	/* ------------------------------------- */
	donation_update(
			application_name,
			donation->full_name,
			donation->street_address,
			donation->donation_date,
			donation->total_donation_amount,
			donation->database_total_donation_amount,
			donation->transaction_date_time,
			donation->database_transaction_date_time );

	/* ---------------------------------------- */
	/* Update LEDGER.transaction_date_time      */
	/* and JOURNAL_LEDGER.transaction_date_time */
	/* ---------------------------------------- */
	ledger_update_transaction_date_time(
		application_name,
		donation->transaction->full_name,
		donation->transaction->street_address,
		donation->transaction->transaction_date_time,
		donation->transaction_date_time
			/* new_transaction_date_time */ );

	if ( strcmp( donation_date, preupdate_donation_date ) < 0 )
	{
		propagate_transaction_date_time =
			donation->transaction_date_time;
	}
	else
	{
		propagate_transaction_date_time =
			donation->transaction->transaction_date_time;
	}

	ledger_propagate_journal_ledger_list(
		application_name,
		propagate_transaction_date_time,
		donation->transaction->journal_ledger_list );

} /* post_change_donation_date() */

