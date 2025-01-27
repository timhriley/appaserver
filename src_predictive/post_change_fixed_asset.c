/* ---------------------------------------------------------------	*/
/* src_predictive/post_change_fixed_asset.c				*/
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
#include "column.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
/*
Don't want to change prior JOURNAL_LEDGER.account transactions.
void post_change_fixed_asset_account_update(
			char *application_name,
			char *asset_name,
			char *account_name,
			char *preupdate_account );

void post_change_fixed_asset_update(
			char *application_name,
			char *asset_name,
			char *account_name,
			char *preupdate_account );
*/

int main( int argc, char **argv )
{
	char *application_name;
	char *asset_name;
	char *account_name;
	char *state;
	char *preupdate_account;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s ignored asset_name account state preupdate_account\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	asset_name = argv[ 2 ];
	if ( ( account_name = argv[ 3 ] ) ) {};
	if ( ( state = argv[ 4 ] ) ) {};
	if ( ( preupdate_account = argv[ 5 ] ) ) {};

	/* If renamed ACCOUNT.account, then exit. */
	/* -------------------------------------- */
	if ( !*asset_name || strcmp( asset_name, "asset_name" ) == 0 )
		exit( 0 );

#ifdef NOT_DEFINED
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_fixed_asset_update(
			application_name,
			asset_name,
			account_name,
			preupdate_account );
	}
#endif

	return 0;

} /* main() */

#ifdef NOT_DEFINED
void post_change_fixed_asset_update(
			char *application_name,
			char *asset_name,
			char *account_name,
			char *preupdate_account )
{
	enum preupdate_change_state account_change_state;

	account_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_account,
			account_name /* postupdate_data */,
			"preupdate_account" );

	if ( account_change_state == from_something_to_something_else )
	{
		post_change_fixed_asset_account_update(
			application_name,
			asset_name,
			account_name,
			preupdate_account );
	}

} /* post_change_fixed_asset_update() */

void post_change_fixed_asset_account_update(
			char *application_name,
			char *asset_name,
			char *account_name,
			char *preupdate_account )
{
	FILE *input_pipe;
	FILE *output_pipe;
	char *select;
	char *from;
	char where[ 512 ];
	char buffer[ 512 ];
	char *key;
	char sys_string[ 1024 ];
	char transaction_date_time[ 32 ];
	char propagate_transaction_date_time[ 32 ];

	select =
"fixed_asset_purchase.full_name, fixed_asset_purchase.street_address,transaction_date_time";

	from = "fixed_asset_purchase, purchase_order";

	sprintf( where,
		 "transaction_date_time is not null and fixed_asset_purchase.full_name = purchase_order.full_name and fixed_asset_purchase.street_address = purchase_order.street_address and fixed_asset_purchase.purchase_date_time = purchase_order.purchase_date_time and fixed_asset_purchase.asset_name = '%s'",
		 timlib_escape_single_quotes(
			buffer,
			asset_name ) );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\"	|"
		 "sql.e					 ",
		 select,
		 from,
		 where );

	input_pipe = popen( sys_string, "r" );

	key = "full_name,street_address,transaction_date_time,account";

	sprintf( sys_string,
		 "sed \"s/$/^%s^account^%s/\"				  |"
		 "update_statement.e table=journal_ledger key=%s carrot=y |"
		 "sql.e							   ",
		 preupdate_account,
		 account_name,
		 key );

	output_pipe = popen( sys_string, "w" );

	*propagate_transaction_date_time = '\0';

	while( get_line( buffer, input_pipe ) )
	{
		piece(	transaction_date_time,
			FOLDER_DATA_DELIMITER,
			buffer,
			2 );

		if ( !*propagate_transaction_date_time )
		{
			strcpy( propagate_transaction_date_time,
				transaction_date_time );
		}
		else
		if ( strcmp(	transaction_date_time,
				propagate_transaction_date_time ) < 0 )
		{
			strcpy( propagate_transaction_date_time,
				transaction_date_time );
		}

		fprintf( output_pipe, "%s\n", buffer );
	}

	pclose( input_pipe );
	pclose( output_pipe );

	if ( *propagate_transaction_date_time )
	{
		sprintf( sys_string,
		 	"ledger_propagate %s \"%s\" '' \"%s\" \"%s\"",
		 	application_name,
		 	propagate_transaction_date_time,
			account_name,
			preupdate_account );

		system( sys_string );
	}

} /* post_change_fixed_asset_account_update() */
#endif
