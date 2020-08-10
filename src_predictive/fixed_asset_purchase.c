/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_asset_purchase.c		*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "list.h"
#include "sql.h"
#include "piece.h"
#include "folder.h"
#include "environ.h"
#include "boolean.h"
#include "transaction.h"
#include "account.h"
#include "depreciation.h"
#include "tax_recovery.h"
#include "purchase.h"

FIXED_ASSET_PURCHASE *fixed_asset_purchase_new(
			char *asset_name,
			char *serial_number )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( ! ( fixed_asset_purchase =
			calloc( 1, sizeof( FIXED_ASSET_PURCHASE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	fixed_asset_purchase->asset_name = asset_name;
	fixed_asset_purchase->serial_number = serial_number;
	return fixed_asset_purchase;
}

FIXED_ASSET_PURCHASE *fixed_asset_purchase_parse(
			char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_date_time[ 128 ];
	char account_name[ 128 ];
	char piece_buffer[ 1024 ];
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( !input ) return (FIXED_ASSET_PURCHASE *)0;

	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( transaction_date_time, SQL_DELIMITER, input, 2 );
	piece( account_name, SQL_DELIMITER, input, 3 );

	fixed_asset_purchase = fixed_asset_purchase_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( transaction_date_time ),
			strdup( account_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	fixed_asset_purchase->previous_balance =
	fixed_asset_purchase->previous_balance_database = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	fixed_asset_purchase->debit_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	fixed_asset_purchase->credit_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	fixed_asset_purchase->balance =
	fixed_asset_purchase->balance_database = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	fixed_asset_purchase->transaction_count =
	fixed_asset_purchase->transaction_count_database = atoi( piece_buffer );

	return fixed_asset_purchase;
}

LIST *fixed_asset_purchase_list_fetch(
			char *where_clause )
{
}

FIXED_ASSET_PURCHASE *fixed_asset_purchase_fetch(
			char *asset_name,
			char *serial_number )
{
}

FIXED_ASSET_PURCHASE *fixed_asset_purchase_list_seek(
			LIST *fixed_asset_purchase_list,
			char *asset_name,
			char *serial_number );

char *fixed_asset_purchase_select(
			void );

void fixed_asset_purchase_update_stream(
			FILE *update_pipe,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			double finance_accumulated_depreciation );

FILE *fixed_asset_purchase_get_update_pipe(
				char *application_name );

void fixed_asset_purchase_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			double finance_accumulated_depreciation,
			double database_finance_accumulated_depreciation );

void fixed_asset_purchase_depreciation_update_and_transaction_propagate(
				FIXED_ASSET_PURCHASE *fixed_asset_purchase,
				char *service_placement_date,
				char *application_name,
				char *fund_name );

void fixed_asset_purchase_depreciation_delete(
				FIXED_ASSET_PURCHASE *fixed_asset_purchase,
				char *application_name,
				char *fund_name );

void fixed_asset_purchase_list_depreciation_method_update(
				LIST *fixed_asset_purchase_purchase_list,
				char *service_placement_date,
				char *application_name,
				char *fund_name );

void fixed_asset_purchase_list_depreciation_delete(
				LIST *fixed_asset_purchase_list,
				char *application_name,
				char *fund_name );

LIST *fixed_asset_purchase_get_account_name_list(
				char *application_name );

LIST *fixed_asset_purchase_account_journal_ledger_refresh(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				LIST *purchase_asset_account_list );

void fixed_asset_purchase_depreciation_update_and_transaction_propagate(
				FIXED_ASSET_PURCHASE *fixed_asset_purchase,
				char *service_placement_date,
				char *application_name,
				char *fund_name );

char *fixed_asset_purchase_get_account_name(
				char *application_name,
				char *asset_name );

LIST *fixed_asset_purchase_purchase_list(
				char *full_name,
				char *street_address,
				char *purchase_date_time );

LIST *fixed_property_depreciation_prior_fetch_list(
				char *application_name,
				char *fund_name );

LIST *fixed_property_depreciation_purchase_fetch_list(
				char *application_name,
				char *fund_name );

LIST *fixed_asset_purchase_depreciation_prior_fetch_list(
				char *application_name,
				char *fund_name );

LIST *fixed_asset_purchase_depreciation_purchase_fetch_list(
				char *application_name,
				char *fund_name );

char *fixed_asset_purchase_purchase_get_fund_where(
				char **folder_from,
				char *folder_name,
				char *fund_name );

char *fixed_asset_purchase_depreciation_get_where(
				char *fund_where,
				char *cost_attribute_name );

/*
void fixed_asset_purchase_depreciation_fund_list_update(
				LIST *depreciation_fund_list );

void fixed_asset_purchase_accumulated_depreciation_update(
				FILE *output_pipe,
				LIST *fixed_asset_purchase_list );

char *fixed_asset_purchase_accumulated_depreciation_get_update_sys_string(
				char *folder_name );
*/

LIST *fixed_asset_purchase_fetch_tax_list(
				char *application_name,
				int tax_year,
				char *folder_name );

char *fixed_asset_purchase_tax_get_where(
				int tax_year );

#endif

