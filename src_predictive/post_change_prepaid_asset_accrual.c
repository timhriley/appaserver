/* ---------------------------------------------------------------	*/
/* src_predictive/post_change_prepaid_asset_accrual.c			*/
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
#include "purchase.h"
#include "column.h"
#include "accrual.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_accrual_stdin(
			char *application_name );

void post_change_accrual_entity(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *preupdate_full_name,
			char *preupdate_street_address,
			char *application_name );

void post_change_accrual_date_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *accrual_date );

void post_change_accrual_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *accrual_date );

void post_change_accrual_insert(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *accrual_date );

void post_change_accrual_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *accrual_date,
			char *preupdate_full_name,
			char *preupdate_street_address,
			char *preupdate_purchase_date_time,
			char *preupdate_asset_name,
			char *preupdate_accrual_date );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *asset_name;
	char *accrual_date;
	char *state;
	char *preupdate_full_name;
	char *preupdate_street_address;
	char *preupdate_purchase_date_time;
	char *preupdate_asset_name;
	char *preupdate_accrual_date;

	appaserver_error_output_starting_argv_stderr( argc, argv );

	if ( argc == 3 && strcmp( argv[ 2 ], "stdin" ) == 0 )
	{
		post_change_accrual_stdin( argv[ 1 ] );

		exit( 0 );
	}

	application_name = environ_get_application_name( argv[ 0 ] );

	if ( argc != 13 )
	{
		fprintf( stderr,
"Usage: %s ignored full_name|stdin street_address purchase_date_time asset_name accrual_date state preupdate_full_name preupdate_street_address preupdate_purchase_date_time preupdate_asset_name preupdate_accrual_date\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	purchase_date_time = argv[ 4 ];
	asset_name = argv[ 5 ];
	accrual_date = argv[ 6 ];
	state = argv[ 7 ];
	preupdate_full_name = argv[ 8 ];
	preupdate_street_address = argv[ 9 ];
	preupdate_purchase_date_time = argv[ 10 ];
	preupdate_asset_name = argv[ 11 ];
	preupdate_accrual_date = argv[ 12 ];

	if ( strcmp( purchase_date_time, "purchase_date_time" ) == 0 )
		exit( 0 );

	/* -------------------------------------------- */
	/* Execute on preupdate because we have		*/
	/* PREPAID_ASSET_ACCRUAL.transaction_date_time. */
	/* -------------------------------------------- */

	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 )
	{
		post_change_accrual_delete(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			accrual_date );
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_accrual_insert(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			accrual_date );
	}
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_accrual_update(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			accrual_date,
			preupdate_full_name,
			preupdate_street_address,
			preupdate_purchase_date_time,
			preupdate_asset_name,
			preupdate_accrual_date );
	}

	return 0;

} /* main() */

void post_change_accrual_stdin( char *application_name )
{
	char input_buffer[ 1024 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	char purchase_date_time[ 128 ];
	char asset_name[ 128 ];
	char accrual_date[ 128 ];

	while( get_line( input_buffer, stdin ) )
	{
		if ( timlib_count_delimiters(
			FOLDER_DATA_DELIMITER,
			input_buffer ) != 4 )
		{
			fprintf( stderr,
"Warning in %s/%s()/%d: not 4 delimiters in (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );
			continue;
		}

		piece(	full_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	street_address,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	purchase_date_time,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	asset_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		piece(	accrual_date,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			4 );

		post_change_accrual_insert(
			application_name,
			strdup( full_name ),
			strdup( street_address ),
			strdup( purchase_date_time ),
			strdup( asset_name ),
			strdup( accrual_date ) );
	}

} /* post_change_accrual_stdin() */

void post_change_accrual_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *accrual_date,
			char *preupdate_full_name,
			char *preupdate_street_address,
			char *preupdate_purchase_date_time,
			char *preupdate_asset_name,
			char *preupdate_accrual_date )
{
	enum preupdate_change_state full_name_change_state;
	enum preupdate_change_state street_address_change_state;
	enum preupdate_change_state purchase_date_time_change_state;
	enum preupdate_change_state asset_name_change_state;
	enum preupdate_change_state accrual_date_change_state;

	full_name_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_full_name,
			full_name /* postupdate_data */,
			"preupdate_full_name" );

	street_address_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_street_address,
			street_address /* postupdate_data */,
			"preupdate_street_address" );

	purchase_date_time_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_purchase_date_time,
			purchase_date_time /* postupdate_data */,
			"preupdate_purchase_date_time" );

	asset_name_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_asset_name,
			asset_name /* postupdate_data */,
			"preupdate_asset_name" );

	accrual_date_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_accrual_date,
			accrual_date /* postupdate_data */,
			"preupdate_accrual_date" );

	if ( full_name_change_state == from_something_to_something_else
	||   street_address_change_state == from_something_to_something_else )
	{
		post_change_accrual_entity(
			full_name,
			street_address,
			purchase_date_time,
			preupdate_full_name,
			preupdate_street_address,
			application_name );
	}

	if ( asset_name_change_state == from_something_to_something_else
	||   purchase_date_time_change_state ==
		from_something_to_something_else )
	{
		/* Do nothing. */
	}

	if (	accrual_date_change_state ==
		from_something_to_something_else )
	{
		post_change_accrual_date_update(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			accrual_date );
	}

} /* post_change_accrual_update() */

void post_change_accrual_date_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *accrual_date )
{
	PURCHASE_PREPAID_ASSET *purchase_prepaid_asset;
	ACCRUAL *accrual;
	char *arrived_date_time;
	char arrived_date_string[ 16 ];

	if ( ! ( arrived_date_time =
			purchase_order_fetch_arrived_date_time(
				application_name,
				full_name,
				street_address,
				purchase_date_time ) ) )
	{
		/* Shouldn't have inserted the row. */
		/* -------------------------------- */
		accrual_delete(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			accrual_date );
		return;
	}

	column( arrived_date_string, 0, arrived_date_time );

	if ( ! ( purchase_prepaid_asset =
			purchase_prepaid_asset_fetch(
				application_name,
				full_name,
				street_address,
				purchase_date_time,
				asset_name ) ) )
	{
		fprintf( stderr,
		"ERROR In %s/%s()/%d: cannot fetch purchase_prepaid_asset.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !purchase_prepaid_asset->accrual_list )
	{
		purchase_prepaid_asset->accrual_list =
			accrual_fetch_list(
				application_name,
				purchase_prepaid_asset->full_name,
				purchase_prepaid_asset->street_address,
				purchase_prepaid_asset->purchase_date_time,
				purchase_prepaid_asset->asset_name );
	}

	if ( ! ( accrual =
			accrual_list_seek(
				purchase_prepaid_asset->accrual_list,
				accrual_date ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek accrual = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 accrual_date );
		exit( 1 );
	}

	accrual->transaction_date_time =
		predictive_transaction_date_time(
			accrual_date );

	accrual_update(
		application_name,
		accrual->full_name,
		accrual->street_address,
		accrual->purchase_date_time,
		accrual->asset_name,
		accrual->accrual_date,
		accrual->accrual_amount,
		accrual->database_accrual_amount,
		accrual->transaction_date_time,
		accrual->database_transaction_date_time );

	ledger_transaction_generic_update(
		application_name,
		accrual->full_name,
		accrual->street_address,
		accrual->transaction->transaction_date_time,
		"transaction_date_time" /* attribute_name */,
		accrual->transaction_date_time /* data */ );

	ledger_journal_generic_update(
		application_name,
		accrual->full_name,
		accrual->street_address,
		accrual->transaction->transaction_date_time,
		"transaction_date_time" /* attribute_name */,
		accrual->transaction_date_time /* data */ );

	accrual->transaction->transaction_date_time =
		accrual->transaction_date_time;

	purchase_accrual_update_and_transaction_propagate(
		purchase_prepaid_asset,
		arrived_date_string,
		application_name );

} /* post_change_accrual_date_update() */

void post_change_accrual_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *accrual_date )
{
	PURCHASE_PREPAID_ASSET *purchase_prepaid_asset;
	ACCRUAL *accrual;

	if ( ! ( purchase_prepaid_asset =
			purchase_prepaid_asset_fetch(
				application_name,
				full_name,
				street_address,
				purchase_date_time,
				asset_name ) ) )
	{
		fprintf( stderr,
		"ERROR In %s/%s()/%d: cannot fetch purchase_prepaid_asset.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ! ( accrual =
			accrual_list_seek( 
				purchase_prepaid_asset->
					accrual_list,
				accrual_date ) ) )
	{
		fprintf( stderr,
		"ERROR In %s/%s()/%d: cannot seek accrual = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 accrual_date );
		exit( 1 );
	}

	if ( !accrual->transaction )
	{
		fprintf( stderr,
			 "ERROR In %s/%s()/%d: empty transaction.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Delete the TRANSACTION */
	/* ---------------------- */
	ledger_delete(	application_name,
			TRANSACTION_FOLDER_NAME,
			accrual->transaction->full_name,
			accrual->transaction->street_address,
			accrual->
				transaction->
				transaction_date_time );

	/* Delete the JOURNAL_LEDGER */
	/* ------------------------- */
	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			accrual->transaction->full_name,
			accrual->transaction->street_address,
			accrual->
				transaction->
				transaction_date_time );

	/* Propagate the debit and credit accounts */
	/* --------------------------------------- */
	ledger_propagate(
		application_name,
		accrual->transaction_date_time,
		purchase_prepaid_asset->asset_account_name );

	ledger_propagate(
		application_name,
		accrual->transaction_date_time,
		purchase_prepaid_asset->expense_account_name );

	/* Subtract the accumulated_accrual */
	/* -------------------------------- */
	purchase_prepaid_asset->accumulated_accrual -= accrual->accrual_amount;

	purchase_prepaid_asset_update(
		application_name,
		purchase_prepaid_asset->full_name,
		purchase_prepaid_asset->street_address,
		purchase_prepaid_asset->purchase_date_time,
		purchase_prepaid_asset->asset_name,
		purchase_prepaid_asset->accumulated_accrual,
		purchase_prepaid_asset->database_accumulated_accrual );
}

void post_change_accrual_insert(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *accrual_date )
{
	PURCHASE_PREPAID_ASSET *purchase_prepaid_asset;
	ACCRUAL *accrual;
	char *prior_accrual_date;
	char *arrived_date_time;
	char arrived_date_string[ 16 ];

	if ( ! ( arrived_date_time =
			purchase_order_fetch_arrived_date_time(
				application_name,
				full_name,
				street_address,
				purchase_date_time ) ) )
	{
		/* Shouldn't have inserted the row. */
		/* -------------------------------- */
		accrual_delete(
			application_name,
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			accrual_date );
		return;
	}

	column( arrived_date_string, 0, arrived_date_time );

	if ( ! ( purchase_prepaid_asset =
			purchase_prepaid_asset_fetch(
				application_name,
				full_name,
				street_address,
				purchase_date_time,
				asset_name ) ) )
	{
		fprintf( stderr,
		"ERROR In %s/%s()/%d: cannot fetch purchase_prepaid_asset.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_length( purchase_prepaid_asset->accrual_list ) )
	{
		purchase_prepaid_asset->accrual_list =
			accrual_fetch_list(
				application_name,
				purchase_prepaid_asset->full_name,
				purchase_prepaid_asset->street_address,
				purchase_prepaid_asset->
					purchase_date_time,
				purchase_prepaid_asset->asset_name );
	}

	if ( list_length( purchase_prepaid_asset->accrual_list ) == 1 )
	{
		prior_accrual_date = arrived_date_string;
	}
	else
	{
		if ( ! ( accrual =
				accrual_list_seek( 
					purchase_prepaid_asset->
						accrual_list,
					accrual_date ) ) )
		{
			fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek accrual = (%s).\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
			 	accrual_date );
			exit( 1 );
		}

		if ( !list_at_tail( 
				purchase_prepaid_asset->
					accrual_list ) )
		{
			/* If inserting a middle accrual. */
			/* ------------------------------ */
			prior_accrual_date =
				accrual_get_prior_accrual_date(
					purchase_prepaid_asset->accrual_list );

			accrual =
				list_get(
					purchase_prepaid_asset->
						accrual_list );

			accrual->accrual_amount =
				accrual_get_amount(
					purchase_prepaid_asset->extension,
					purchase_prepaid_asset->
						accrual_period_years,
					prior_accrual_date,
					accrual->accrual_date,
					purchase_prepaid_asset->
						accumulated_accrual );

			purchase_accrual_update_and_transaction_propagate(
				purchase_prepaid_asset,
				arrived_date_string,
				application_name );

			return;
		}

		prior_accrual_date =
			accrual_get_prior_accrual_date(
				purchase_prepaid_asset->accrual_list );
	}

	accrual =
		list_get_last_pointer( 
			purchase_prepaid_asset->
				accrual_list );

	accrual->accrual_amount =
		accrual_get_amount(
			purchase_prepaid_asset->extension,
			purchase_prepaid_asset->accrual_period_years,
			prior_accrual_date,
			accrual->accrual_date,
			purchase_prepaid_asset->accumulated_accrual );

	if ( !accrual->accrual_amount )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty accrual_amount.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return;
	}

	accrual->transaction_date_time =
		predictive_transaction_date_time(
			accrual->accrual_date );
	
	accrual->transaction =
		ledger_transaction_new(
			accrual->full_name,
			accrual->street_address,
			accrual->transaction_date_time,
			ACCRUAL_MEMO );
	
	accrual->transaction_date_time =
	accrual->transaction->transaction_date_time =
	ledger_transaction_insert(
		application_name,
		accrual->transaction->full_name,
		accrual->transaction->street_address,
		accrual->transaction->transaction_date_time,
		accrual->accrual_amount /* transaction_amount */,
		accrual->transaction->memo,
		0 /* check_number */,
		1 /* lock_transaction */ );

	accrual_journal_ledger_refresh(
		application_name,
		accrual->transaction->full_name,
		accrual->transaction->street_address,
		accrual->
			transaction->
			transaction_date_time,
		accrual->accrual_amount,
		purchase_prepaid_asset->asset_account_name,
		purchase_prepaid_asset->expense_account_name );

	ledger_propagate(
		application_name,
		accrual->transaction_date_time,
		purchase_prepaid_asset->asset_account_name );

	ledger_propagate(
		application_name,
		accrual->transaction_date_time,
		purchase_prepaid_asset->expense_account_name );

	accrual_update(
		application_name,
		accrual->full_name,
		accrual->street_address,
		accrual->purchase_date_time,
		accrual->asset_name,
		accrual->accrual_date,
		accrual->accrual_amount,
		accrual->database_accrual_amount,
		accrual->transaction_date_time,
		accrual->database_transaction_date_time );

	/* Add the accumulated_accrual */
	/* --------------------------- */
	purchase_prepaid_asset->accumulated_accrual += accrual->accrual_amount;

	purchase_prepaid_asset_update(
		application_name,
		purchase_prepaid_asset->full_name,
		purchase_prepaid_asset->street_address,
		purchase_prepaid_asset->purchase_date_time,
		purchase_prepaid_asset->asset_name,
		purchase_prepaid_asset->accumulated_accrual,
		purchase_prepaid_asset->database_accumulated_accrual );
}

void post_change_accrual_entity(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *preupdate_full_name,
			char *preupdate_street_address,
			char *application_name )
{
	PURCHASE_ORDER *purchase_order;
	PURCHASE_PREPAID_ASSET *purchase_prepaid_asset;
	ACCRUAL *accrual;

	if ( ! ( purchase_order =
			purchase_order_new(
				application_name,
				full_name,
				street_address,
				purchase_date_time ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load purchase_order.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( purchase_order->prepaid_asset_purchase_list ) )
		return;

	do {
		purchase_prepaid_asset =
			list_get_pointer(
				purchase_order->
					prepaid_asset_purchase_list );

		if ( !purchase_prepaid_asset->accrual_list )
		{
			purchase_prepaid_asset->accrual_list =
				accrual_fetch_list(
					application_name,
					purchase_prepaid_asset->full_name,
					purchase_prepaid_asset->street_address,
					purchase_prepaid_asset->
						purchase_date_time,
					purchase_prepaid_asset->asset_name );
		}

		if ( !list_rewind( purchase_prepaid_asset->accrual_list ) )
		{
			continue;
		}

		do {
			accrual =
				list_get_pointer(
					purchase_prepaid_asset->
						accrual_list );

			if ( !accrual->transaction )
			{
				fprintf( stderr,
"ERROR in %s/%s()/%d: empty transaction for accrual_date = (%s).\n",
			 		__FILE__,
			 		__FUNCTION__,
			 		__LINE__,
			 		accrual->accrual_date );
				exit( 1 );
			}


			ledger_entity_update(
				application_name,
				full_name,
				street_address,
				accrual->transaction_date_time,
				preupdate_full_name,
				preupdate_street_address );

		} while( list_next( purchase_prepaid_asset->accrual_list ) );

	} while( list_next( purchase_order->prepaid_asset_purchase_list ) );
}

