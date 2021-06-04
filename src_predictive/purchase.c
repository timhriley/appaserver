/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/purchase.c				*/
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
#include "list.h"
#include "environ.h"
#include "boolean.h"
#include "folder.h"
#include "transaction.h"
#include "journal.h"
#include "entity.h"
#include "vendor_payment.h"
#include "inventory_purchase.h"
#include "specific_inventory_purchase.h"
#include "fixed_asset_purchase.h"
#include "supply_purchase.h"
#include "prepaid_asset_purchase.h"
#include "predictive.h"
#include "purchase.h"

PURCHASE *purchase_fetch(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '*' %s \"%s\" ''",
		 PURCHASE_TABLE,
		 /* -------------------------- */
		 /* Safely returns heap memory */
		 /* -------------------------- */
		 purchase_primary_where(
			full_name,
			street_address,
			purchase_date_time ) );

	return purchase_parse( pipe2string( system_string ) );
}

PURCHASE *purchase_new(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	PURCHASE *purchase;

	purchase = purchase_calloc();

	purchase->vendor_entity =
		entity_new(
			full_name,
			street_address ),

	purchase->purchase_date_time = purchase_date_time;

	return purchase;
}

PURCHASE *purchase_calloc( void )
{
	PURCHASE *purchase;

	if ( ! ( purchase = calloc( 1, sizeof( PURCHASE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return purchase;
}

PURCHASE *purchase_seek(
			LIST *purchase_list,
			char *purchase_date_time )
{
	PURCHASE *purchase;

	if ( !list_rewind( purchase_list ) ) return (PURCHASE *)0;

	do {
		purchase = list_get( purchase_list );

		if ( strcmp(	purchase->purchase_date_time,
				purchase_date_time ) == 0 )
		{
			return purchase;
		}

	} while( list_next( purchase_list ) );
	return (PURCHASE *)0;
}

FILE *purchase_update_open( void )
{
	char *key;
	char system_string[ 1024 ];

	key = "full_name,street_address,purchase_date_time";

	sprintf( system_string,
		 "update_statement.e table=%s key=%s carrot=y | sql",
		 PURCHASE_TABLE,
		 key );

	return popen( system_string, "w" );
}

void purchase_update(
			double fixed_asset_purchase_total,
			double invoice_amount,
			double vendor_payment_total,
			double amount_due,
			char *transaction_date_time,
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	FILE *update_pipe = purchase_update_open();

	fprintf(update_pipe,
		"%s^%s^%s^fixed_asset_purchase_total^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		purchase_date_time,
		fixed_asset_purchase_total );

	fprintf(update_pipe,
		"%s^%s^%s^invoice_amount^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		purchase_date_time,
		invoice_amount );

	fprintf(update_pipe,
		"%s^%s^%s^vendor_payment_total^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		purchase_date_time,
		vendor_payment_total );

	fprintf(update_pipe,
		"%s^%s^%s^amount_due^%.2lf\n",
		entity_escape_full_name( full_name ),
		street_address,
		purchase_date_time,
		amount_due );

	fprintf(update_pipe,
		"%s^%s^%s^transaction_date_time^%s\n",
		entity_escape_full_name( full_name ),
		street_address,
		purchase_date_time,
		transaction_date_time );

	pclose( update_pipe );
}

PURCHASE *purchase_parse( char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char purchase_date_time[ 128 ];
	char piece_buffer[ 128 ];
	PURCHASE *purchase;

	if ( !input ) return (PURCHASE *)0;

	/* See attribute_list purchase */
	/* --------------------------- */
	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( purchase_date_time, SQL_DELIMITER, input, 2 );

	purchase =
		purchase_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( purchase_date_time ) );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	purchase->sales_tax = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	purchase->freight_in = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	purchase->title_passage_rule_string = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	purchase->shipped_date = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	purchase->arrived_date_time = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 8 );
	purchase->fixed_asset_purchase_total = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 9 );
	purchase->inventory_purchase_total = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 10 );
	purchase->specific_inventory_purchase_total = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 11 );
	purchase->supply_purchase_total = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 12 );
	purchase->prepaid_asset_purchase_total = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 13 );
	purchase->purchase_invoice_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 14 );
	purchase->purchase_amount_due = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 15 );
	purchase->transaction_date_time = strdup( piece_buffer );

	purchase->fixed_asset_purchase_list =
		fixed_asset_purchase_list_fetch(
			purchase_primary_where(
				purchase->vendor_entity->full_name,
				purchase->vendor_entity->street_address,
				purchase->purchase_date_time ),
			0 /* not fetch_last_depreciation */,
			0 /* not fetch_last_recovery */ );

	purchase->vendor_payment_list =
		vendor_payment_list_fetch(
			purchase->vendor_entity->full_name,
			purchase->vendor_entity->street_address,
			purchase->purchase_date_time );

	return purchase;
}

/* Safely returns heap memory */
/* -------------------------- */
char *purchase_primary_where(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	char where[ 1024 ];

	sprintf( where,
		 "full_name = '%s' and		"
		 "street_address = '%s' and	"
		 "purchase_date_time = '%s'	",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( full_name ),
		 street_address,
		 purchase_date_time );

	return strdup( where );
}

double purchase_fetch_amount_due(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	char system_string[ 1024 ];
	char *results_string;

	sprintf( system_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 "amount_due",
		 PURCHASE_TABLE,
		 purchase_primary_where(
			full_name,
			street_address,
			purchase_date_time ) );

	results_string = pipe2string( system_string );

	if ( !results_string )
		return 0.0;
	else
		return atof( results_string );
}

char *purchase_transaction_refresh(
			double transaction_amount,
			LIST *journal_list,
			char *purchase_transaction_memo,
			char *full_name,
			char *street_address,
			char *transaction_date_time )
{
	return transaction_journal_refresh(
		full_name,
		street_address,
		transaction_date_time,
		transaction_amount,
		purchase_transaction_memo,
		0 /* check_number */,
		1 /* lock_transaction */,
		journal_list );
}

TRANSACTION *purchase_transaction(
			char *full_name,
			char *street_address,
			char *arrived_date_time,
			double invoice_amount,
			char *asset_account_name,
			char *account_payable )
{
	TRANSACTION *transaction;

	if ( !invoice_amount ) return (TRANSACTION *)0;

	transaction =
		transaction_new(
			full_name,
			street_address,
			arrived_date_time
				/* transaction_date_time */ );

	transaction->transaction_amount = invoice_amount;
	transaction->memo = PURCHASE_MEMO;

	transaction->journal_list =
		purchase_journal_list(
			invoice_amount,
			asset_account_name,
			account_payable );

	return transaction;
}

LIST *purchase_journal_list(
			double purchase_invoice_amount,
			char *purchase_asset_account_name,
			char *account_payable )
{
	JOURNAL *journal;
	LIST *journal_list = list_new();

	/* Set debit */
	/* --------- */
	journal =
		journal_new(
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			(char *)0 /* transaction_date_time */,
			purchase_asset_account_name );

	journal->debit_amount = purchase_invoice_amount;

	list_set( journal_list, journal );

	/* Set credit */
	/* ---------- */
	journal =
		journal_new(
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			(char *)0 /* transaction_date_time */,
			account_payable );

	journal->credit_amount = purchase_invoice_amount;

	list_set( journal_list, journal );

	return journal_list;
}

char *purchase_asset_account_name(
			LIST *fixed_asset_purchase_list )
{
	double highest_cost = 0.0;
	char *asset_account_name = {0};
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( !list_rewind( fixed_asset_purchase_list ) ) return (char *)0;

	do {
		fixed_asset_purchase =
			list_get( 
				fixed_asset_purchase_list );

		if ( fixed_asset_purchase->fixed_asset_cost > highest_cost )
		{
			asset_account_name =
				fixed_asset_purchase->
					fixed_asset->
					account_name;

			highest_cost = fixed_asset_purchase->fixed_asset_cost;
		}
	} while ( list_next( fixed_asset_purchase_list ) );

	return asset_account_name;
}

LIST *purchase_amount_due_purchase_list( void )
{
	if ( !folder_exists_folder( environment_application(), "purchase" ) )
		return (LIST *)0;

	return purchase_system_list(
		purchase_system_string(
			"ifnull( amount_due, 0.0 ) > 0.0",
			"purchase_date" /* order */ ) );
}

char *purchase_system_string(
			char *where,
			char *order )
{
	char system_string[ 1024 ];

	if ( !where || !*where ) where = "1 = 1";

	sprintf(system_string,
		"select.sh '*' %s \"%s\" \"%s\"",
		PURCHASE_TABLE,
		where,
		(order) ? order : "" );

	return strdup( system_string );
}

LIST *purchase_system_list( char *system_string )
{
	LIST *purchase_list;
	char input[ 1024 ];
	FILE *input_pipe;

	purchase_list = list_new();
	input_pipe = popen( system_string, "r" );

	while( string_input( input, input_pipe, 1024 ) )
	{

		list_set( purchase_list, purchase_parse( input ) );
	}
	pclose( input_pipe );
	return purchase_list;
}

PURCHASE *purchase_steady_state(
			double sales_tax,
			double freight_in,
			LIST *fixed_asset_purchase_list,
			LIST *inventory_purchase_list,
			LIST *specific_inventory_purchase_list,
			LIST *supply_purchase_list,
			LIST *prepaid_asset_purchase_list,
			LIST *vendor_payment_list,
			PURCHASE *purchase )
{
	purchase->fixed_asset_purchase_total =
		fixed_asset_purchase_total(
			fixed_asset_purchase_list );

	purchase->inventory_purchase_total =
		inventory_purchase_total(
			inventory_purchase_list );

	purchase->specific_inventory_purchase_total =
		specific_inventory_purchase_total(
			specific_inventory_purchase_list );

	purchase->supply_purchase_total =
		supply_purchase_total(
			supply_purchase_list );

	purchase->prepaid_asset_purchase_total =
		prepaid_asset_purchase_total(
			prepaid_asset_purchase_list );

	purchase->vendor_payment_total =
		vendor_payment_total(
			vendor_payment_list );

	purchase->purchase_invoice_amount =
		purchase_invoice_amount(
			purchase->fixed_asset_purchase_total,
			purchase->inventory_purchase_total,
			purchase->specific_inventory_purchase_total,
			purchase->supply_purchase_total,
			purchase->prepaid_asset_purchase_total,
			sales_tax,
			freight_in );

	return purchase;
}

double purchase_cost_basis(
			double purchase_cost,
			double sales_tax,
			double freight_in,
			double fixed_asset_purchase_total,
			double inventory_purchase_total,
			double specific_inventory_purchase_total,
			double supply_purchase_total,
			double prepaid_asset_purchase_total )
{
	double extra_total;
	double total;
	double percent_of_purchase;
	double extra_allocated;
	double cost_basis;

	extra_total = sales_tax + freight_in;

	total =
		purchase_total(
			fixed_asset_purchase_total,
			inventory_purchase_total,
			specific_inventory_purchase_total,
			supply_purchase_total,
			prepaid_asset_purchase_total );

	if ( !total ) return 0.0;

	percent_of_purchase = purchase_cost / total;
	extra_allocated = extra_total * percent_of_purchase;
	cost_basis = purchase_cost + extra_allocated;

	return cost_basis;
}

double purchase_total(
			double fixed_asset_purchase_total,
			double inventory_purchase_total,
			double specific_inventory_purchase_total,
			double supply_purchase_total,
			double prepaid_asset_purchase_total )
{
	double total =
		fixed_asset_purchase_total +
		inventory_purchase_total +
		specific_inventory_purchase_total +
		supply_purchase_total +
		prepaid_asset_purchase_total;

	return total;
}

double purchase_invoice_amount(
			double fixed_asset_purchase_total,
			double inventory_purchase_total,
			double specific_inventory_purchase_total,
			double supply_purchase_total,
			double prepaid_asset_purchase_total,
			double sales_tax,
			double freight_in )
{
	double total =
		purchase_total(
			fixed_asset_purchase_total,
			inventory_purchase_total,
			specific_inventory_purchase_total,
			supply_purchase_total,
			prepaid_asset_purchase_total );

	if ( !total ) return 0.0;

	return total + sales_tax + freight_in;
}

boolean purchase_is_participating( void )
{
	char system_string[ 1024 ];
	char where[ 512 ];

	sprintf(where,
		"folder = '%s'",
		PURCHASE_TABLE );

	sprintf(system_string,
		"select.sh \"%s\" role_folder \"%s\" ''",
		"count(1)",
		where );

	return atoi( string_pipe_fetch( system_string ) );
}

