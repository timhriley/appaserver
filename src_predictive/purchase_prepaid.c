/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/purchase.c				*/
/* -------------------------------------------------------------------- */
/* This is the PredictiveBooks purchase ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "environ.h"
#include "date.h"
#include "folder.h"
#include "list.h"
#include "purchase.h"
#include "depreciation.h"
#include "accrual.h"
#include "inventory.h"
#include "fixed_asset.h"
#include "entity.h"
#include "journal.h"
#include "transaction.h"

PURCHASE_ORDER *purchase_order_calloc( void )
{
	PURCHASE_ORDER *p =
		(PURCHASE_ORDER *)
			calloc( 1, sizeof( PURCHASE_ORDER ) );

	if ( !p )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	return p;

} /* purchase_order_calloc() */

PURCHASE_ORDER *purchase_order_new(	char *full_name,
					char *street_address,
					char *purchase_date_time )
{
	PURCHASE_ORDER *p;
	double sum_inventory_extension;

	p = purchase_order_calloc();
	p->full_name = full_name;
	p->street_address = street_address;
	p->purchase_date_time = purchase_date_time;

	if ( !purchase_order_load(
				&p->sum_extension,
				&p->database_sum_extension,
				&p->sales_tax,
				&p->freight_in,
				&p->purchase_amount,
				&p->database_purchase_amount,
				&p->amount_due,
				&p->database_amount_due,
				&p->title_passage_rule,
				&p->shipped_date,
				&p->database_shipped_date,
				&p->arrived_date_time,
				&p->database_arrived_date_time,
				&p->transaction_date_time,
				&p->database_transaction_date_time,
				&p->fund_name,
				&p->property_street_address,
				environment_application_name(),
				p->full_name,
				p->street_address,
				p->purchase_date_time ) )
	{
		return (PURCHASE_ORDER *)0;
	}

	p->liability_payment_amount = p->purchase_amount - p->amount_due;

	p->inventory_purchase_list =
		inventory_inventory_purchase_list(
				environment_application_name(),
				p->full_name,
				p->street_address,
				p->purchase_date_time );

	if ( ledger_folder_exists(
		environment_application_name(),
		"specific_inventory_purchase" ) )
	{
		p->specific_inventory_purchase_list =
			purchase_specific_inventory_list(
				environment_application_name(),
				p->full_name,
				p->street_address,
				p->purchase_date_time );
	}

	p->supply_purchase_list =
		purchase_supply_list(
				environment_application_name(),
				p->full_name,
				p->street_address,
				p->purchase_date_time );

	p->service_purchase_list =
		purchase_service_list(
				environment_application_name(),
				p->full_name,
				p->street_address,
				p->purchase_date_time );

	p->fixed_asset_purchase_list =
		fixed_asset_purchase_list(
				environment_application_name(),
				p->full_name,
				p->street_address,
				p->purchase_date_time );

	if ( ledger_folder_exists(
		environment_application_name(),
		"prepaid_asset_purchase" ) )
	{
		p->prepaid_asset_purchase_list =
			purchase_prepaid_asset_get_list(
				environment_application_name(),
				p->full_name,
				p->street_address,
				p->purchase_date_time );
	}

	if ( p->transaction_date_time )
	{
		p->transaction =
			transaction_fetch(
				p->full_name,
				p->street_address,
				p->transaction_date_time );
	}

	p->purchase_amount =
		purchase_order_get_purchase_amount(
			&p->sum_inventory_extension,
			&p->sum_specific_inventory_extension,
			&p->sum_supply_extension,
			&p->sum_service_extension,
			&p->sum_fixed_asset_extension,
			&p->sum_prepaid_asset_extension,
			&p->sum_extension,
			p->inventory_purchase_list,
			p->specific_inventory_purchase_list,
			p->supply_purchase_list,
			p->service_purchase_list,
			p->fixed_asset_purchase_list,
			p->prepaid_asset_purchase_list,
			p->sales_tax,
			p->freight_in );

	if ( ( p->vendor_payment_list =
		purchase_get_vendor_payment_list(
			environment_application_name(),
			p->full_name,
			p->street_address,
			p->purchase_date_time ) ) )
	{
		p->sum_payment_amount =
			purchase_get_sum_payment_amount(
				p->vendor_payment_list );
	}

	p->amount_due =
		PURCHASE_GET_AMOUNT_DUE(
			p->purchase_amount,
			p->sum_payment_amount );

	sum_inventory_extension =
		p->sum_inventory_extension +
		p->sum_specific_inventory_extension;

	inventory_purchase_list_set_capitalized_unit_cost(
		p->inventory_purchase_list,
		sum_inventory_extension,
		p->sales_tax,
		p->freight_in );

	purchase_specific_inventory_set_capitalized_extension(
		p->specific_inventory_purchase_list,
		sum_inventory_extension,
		p->sales_tax,
		p->freight_in );

	return p;

} /* purchase_order_new() */

double purchase_order_get_purchase_amount(
				double *sum_inventory_extension,
				double *sum_specific_inventory_extension,
				double *sum_supply_extension,
				double *sum_service_extension,
				double *sum_fixed_asset_extension,
				double *sum_prepaid_asset_extension,
				double *sum_extension,
				LIST *inventory_purchase_list,
				LIST *specific_inventory_purchase_list,
				LIST *supply_purchase_list,
				LIST *service_purchase_list,
				LIST *fixed_asset_purchase_list,
				LIST *prepaid_asset_purchase_list,
				double sales_tax,
				double freight_in )
{
	*sum_inventory_extension = 0.0;
	*sum_inventory_extension =
		inventory_purchase_list_set_extension(
			inventory_purchase_list );

	*sum_specific_inventory_extension = 0.0;
	*sum_specific_inventory_extension =
		purchase_get_sum_specific_inventory_extension(
			specific_inventory_purchase_list );

	*sum_supply_extension = 0.0;
	*sum_supply_extension =
		purchase_get_sum_supply_extension(
			supply_purchase_list );

	*sum_service_extension = 0.0;
	*sum_service_extension =
		purchase_get_sum_service_extension(
			service_purchase_list );

	*sum_fixed_asset_extension = 0.0;
	*sum_fixed_asset_extension =
		purchase_get_sum_fixed_asset_extension(
			fixed_asset_purchase_list );

	*sum_prepaid_asset_extension = 0.0;
	*sum_prepaid_asset_extension =
		purchase_get_sum_prepaid_asset_extension(
			prepaid_asset_purchase_list );

	*sum_extension =	*sum_inventory_extension +
				*sum_specific_inventory_extension +
				*sum_supply_extension +
				*sum_service_extension +
				*sum_fixed_asset_extension +
				*sum_prepaid_asset_extension;

	return *sum_extension + sales_tax + freight_in;

} /* purchase_order_get_purchase_amount() */

char *purchase_order_select( void )
{
	char select[ 1024 ];
	char *fund_select;
	char *property_select;
	char *title_passage_rule_select;

	if ( ledger_fund_attribute_exists(
			environment_application_name() ) )
	{
		fund_select = "fund";
	}
	else
	{
		fund_select = "''";
	}

	if ( ledger_title_passage_rule_attribute_exists(
			environment_application_name(),
			"purchase_order" /* folder_name */ ) )
	{
		title_passage_rule_select = "title_passage_rule";
	}
	else
	{
		title_passage_rule_select = "''";
	}

	if ( ledger_property_street_address_attribute_exists(
			environment_application_name() ) )
	{
		property_select = "property_street_address";
	}
	else
	{
		property_select = "''";
	}

	sprintf( select,
"full_name,street_address,purchase_date_time,sum_extension,sales_tax,freight_in,purchase_amount,amount_due,%s,shipped_date,arrived_date_time,transaction_date_time,%s,%s",
		 title_passage_rule_select,
		 fund_select,
		 property_select );

	return strdup( select );

} /* purchase_order_get_select() */

boolean purchase_order_load(	double *sum_extension,
				double *database_sum_extension,
				double *sales_tax,
				double *freight_in,
				double *purchase_amount,
				double *database_purchase_amount,
				double *amount_due,
				double *database_amount_due,
				enum title_passage_rule *title_passage_rule,
				char **shipped_date,
				char **database_shipped_date,
				char **arrived_date_time,
				char **database_arrived_date_time,
				char **transaction_date_time,
				char **database_transaction_date_time,
				char **fund_name,
				char **property_street_address,
				char *full_name,
				char *street_address,
				char *purchase_date_time )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;
	char *results;

	select = purchase_order_get_select();

	where = ledger_get_transaction_where(
			full_name,
			street_address,
			purchase_date_time,
			(char *)0 /* folder_name */,
			"purchase_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=purchase_order		"
		 "			where=\"%s\"			",
		 environment_application_name(),
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0;

	purchase_order_parse(
			(char **)0 /* full_name */,
			(char **)0 /* street_address */,
			&purchase_date_time
				/* Won't change it, but needed */
				/* to set arrived_date_time    */,
			sum_extension,
			database_sum_extension,
			sales_tax,
			freight_in,
			purchase_amount,
			database_purchase_amount,
			amount_due,
			database_amount_due,
			title_passage_rule,
			shipped_date,
			database_shipped_date,
			arrived_date_time,
			database_arrived_date_time,
			transaction_date_time,
			database_transaction_date_time,
			fund_name,
			property_street_address,
			results );

	free( results );

	return 1;

} /* purchase_order_load() */

void purchase_order_parse(	char **full_name,
				char **street_address,
				char **purchase_date_time,
				double *sum_extension,
				double *database_sum_extension,
				double *sales_tax,
				double *freight_in,
				double *purchase_amount,
				double *database_purchase_amount,
				double *amount_due,
				double *database_amount_due,
				enum title_passage_rule *title_passage_rule,
				char **shipped_date,
				char **database_shipped_date,
				char **arrived_date_time,
				char **database_arrived_date_time,
				char **transaction_date_time,
				char **database_transaction_date_time,
				char **fund_name,
				char **property_street_address,
				char *results )
{
	char piece_buffer[ 1024 ];

	if ( full_name )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 0 );
		if ( *piece_buffer )
			*full_name = strdup( piece_buffer );
	}

	if ( street_address )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 1 );
		if ( *piece_buffer )
			*street_address = strdup( piece_buffer );
	}

	if ( purchase_date_time )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 2 );
		if ( *piece_buffer )
			*purchase_date_time = strdup( piece_buffer );
	}

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 3 );
	if ( *piece_buffer )
		*sum_extension =
		*database_sum_extension = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 4 );
	if ( *piece_buffer )
		*sales_tax = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 5 );
	if ( *piece_buffer )
		*freight_in = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 6 );
	if ( *piece_buffer )
		*purchase_amount =
		*database_purchase_amount = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 7 );
	if ( *piece_buffer )
		*amount_due =
		*database_amount_due = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 8 );
	if ( *piece_buffer )
		*title_passage_rule =
			entity_get_title_passage_rule(
				piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 9 );
	if ( *piece_buffer )
		*shipped_date =
		*database_shipped_date = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 10 );
	if ( *piece_buffer )
		*arrived_date_time =
		*database_arrived_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 11 );
	if ( *piece_buffer )
		*transaction_date_time =
		*database_transaction_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 12 );
	if ( *piece_buffer )
		*fund_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 13 );
	if ( *piece_buffer )
		*property_street_address = strdup( piece_buffer );

} /* purchase_order_parse() */

PURCHASE_SPECIFIC_INVENTORY *purchase_specific_inventory_new(
				char *inventory_name )
{
	PURCHASE_SPECIFIC_INVENTORY *p =
		(PURCHASE_SPECIFIC_INVENTORY *)
			calloc( 1, sizeof( PURCHASE_SPECIFIC_INVENTORY ) );

	p->inventory_name = inventory_name;
	return p;

} /* purchase_specific_inventory_new() */

PURCHASE_SUPPLY *purchase_supply_new( char *supply_name )
{
	PURCHASE_SUPPLY *p =
		(PURCHASE_SUPPLY *)
			calloc( 1, sizeof( PURCHASE_SUPPLY ) );

	p->supply_name = supply_name;
	return p;

} /* purchase_supply_new() */

PURCHASE_SERVICE *purchase_service_new( char *account_name )
{
	PURCHASE_SERVICE *p =
		(PURCHASE_SERVICE *)
			calloc( 1, sizeof( PURCHASE_SERVICE ) );

	p->account_name = account_name;
	return p;

} /* purchase_service_new() */

PURCHASE_PREPAID_ASSET *purchase_prepaid_asset_new( void )
{
	PURCHASE_PREPAID_ASSET *p =
		(PURCHASE_PREPAID_ASSET *)
			calloc( 1, sizeof( PURCHASE_PREPAID_ASSET ) );

	if ( !p )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return p;

} /* purchase_prepaid_asset_new() */

LIST *purchase_supply_list(	char *full_name,
				char *street_address,
				char *purchase_date_time )
{
	char sys_string[ 1024 ];
	char *supply_purchase_where;
	char *join_where;
	char where[ 512 ];
	char *select;
	char input_buffer[ 1024 ];
	char piece_buffer[ 256 ];
	FILE *input_pipe;
	PURCHASE_SUPPLY *purchase_supply;
	LIST *supply_purchase_list;
	char *folder;

	select =
	"supply_purchase.supply_name,quantity,unit_cost,extension,account";

	supply_purchase_where =
		ledger_get_transaction_where(
				full_name,
				street_address,
				purchase_date_time,
				(char *)0 /* folder_name */,
				"purchase_date_time" );

	join_where = "supply_purchase.supply_name = supply.supply_name";

	sprintf( where, "%s and %s", supply_purchase_where, join_where );

	folder = "supply_purchase,supply";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 environment_application_name(),
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );
	supply_purchase_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		purchase_supply =
			purchase_supply_new(
				strdup( piece_buffer ) );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		purchase_supply->quantity = atoi( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		purchase_supply->unit_cost = atof( piece_buffer );

		purchase_supply->extension =
			purchase_supply_get_extension(
				purchase_supply->unit_cost,
				purchase_supply->quantity );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
		purchase_supply->database_extension = atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
		purchase_supply->account_name = strdup( piece_buffer );

		list_append_pointer( supply_purchase_list, purchase_supply );
	}

	pclose( input_pipe );
	return supply_purchase_list;
}

LIST *purchase_specific_inventory_get_list(
					char *full_name,
					char *street_address,
					char *purchase_date_time )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char *where;
	char input_buffer[ 1024 ];
	char piece_buffer[ 256 ];
	FILE *input_pipe;
	PURCHASE_SPECIFIC_INVENTORY *purchase_specific_inventory;
	LIST *specific_inventory_purchase_list;

	select = "inventory_name,serial_number,extension,capitalized_extension";

	folder = "specific_inventory_purchase";

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 environment_application_name(),
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );
	specific_inventory_purchase_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		purchase_specific_inventory =
			purchase_specific_inventory_new(
				strdup( piece_buffer ) );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		purchase_specific_inventory->serial_number =
			strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		purchase_specific_inventory->extension = atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		purchase_specific_inventory->capitalized_extension =
		purchase_specific_inventory->database_capitalized_extension =
			atof( piece_buffer );

		list_append_pointer(
			specific_inventory_purchase_list,
			purchase_specific_inventory );
	}

	pclose( input_pipe );
	return specific_inventory_purchase_list;

} /* purchase_specific_inventory_get_list() */

double purchase_supply_get_extension(
				double unit_cost,
				int quantity )
{
	return unit_cost * (double)quantity;

}

LIST *purchase_service_list(
				char *full_name,
				char *street_address,
				char *purchase_date_time )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;
	char input_buffer[ 1024 ];
	char piece_buffer[ 256 ];
	FILE *input_pipe;
	PURCHASE_SERVICE *purchase_service;
	LIST *service_purchase_list;

	select = "account,extension";

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=service_purchase		"
		 "			where=\"%s\"			",
		 environment_application_name(),
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );
	service_purchase_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		purchase_service =
			purchase_service_new(
				strdup( piece_buffer ) );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		purchase_service->extension = atof( piece_buffer );

		list_append_pointer( service_purchase_list, purchase_service );
	}

	pclose( input_pipe );
	return service_purchase_list;
}

double purchase_get_sum_specific_inventory_extension(
			LIST *specific_inventory_purchase_list )
{
	PURCHASE_SPECIFIC_INVENTORY *purchase_specific_inventory;
	double sum_extension;

	if ( !list_rewind( specific_inventory_purchase_list ) ) return 0.0;

	sum_extension = 0.0;

	do {
		purchase_specific_inventory =
			list_get_pointer(
				specific_inventory_purchase_list );

		sum_extension += purchase_specific_inventory->extension;

	} while( list_next( specific_inventory_purchase_list ) );

	return sum_extension;

} /* purchase_get_sum_specific_inventory_extension() */

double purchase_get_sum_supply_extension(
			LIST *supply_purchase_list )
{
	PURCHASE_SUPPLY *purchase_supply;
	double sum_extension;

	if ( !list_rewind( supply_purchase_list ) ) return 0.0;

	sum_extension = 0.0;

	do {
		purchase_supply = list_get( supply_purchase_list );
		sum_extension += purchase_supply->extension;

	} while( list_next( supply_purchase_list ) );

	return sum_extension;
} /* purchase_get_sum_supply_extension() */

double purchase_get_sum_service_extension(
			LIST *service_purchase_list )
{
	PURCHASE_SERVICE *purchase_service;
	double sum_extension;

	if ( !list_rewind( service_purchase_list ) ) return 0.0;

	sum_extension = 0.0;

	do {
		purchase_service = list_get( service_purchase_list );
		sum_extension += purchase_service->extension;

	} while( list_next( service_purchase_list ) );

	return sum_extension;
} /* purchase_get_sum_service_extension() */

double purchase_get_sum_fixed_asset_extension(
			LIST *fixed_asset_purchase_list )
{
	FIXED_ASSET *purchase_fixed_asset;
	double sum_extension;

	if ( !list_rewind( fixed_asset_purchase_list ) ) return 0.0;

	sum_extension = 0.0;

	do {
		purchase_fixed_asset = list_get( fixed_asset_purchase_list );
		sum_extension += purchase_fixed_asset->extension;

	} while( list_next( fixed_asset_purchase_list ) );

	return sum_extension;
} /* purchase_get_sum_fixed_asset_extension() */

double purchase_get_sum_prepaid_asset_extension(
			LIST *prepaid_asset_purchase_list )
{
	PURCHASE_PREPAID_ASSET *purchase_prepaid_asset;
	double sum_extension;

	if ( !list_rewind( prepaid_asset_purchase_list ) ) return 0.0;

	sum_extension = 0.0;

	do {
		purchase_prepaid_asset =
			list_get( prepaid_asset_purchase_list );
		sum_extension += purchase_prepaid_asset->extension;

	} while( list_next( prepaid_asset_purchase_list ) );

	return sum_extension;
}

char *purchase_prepaid_asset_update_sys_string( void )
{
	static char sys_string[ 256 ];
	char *key;

	key =
"full_name,street_address,purchase_date_time,asset_name";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 "prepaid_asset_purchase",
		 key );

	return sys_string;
}

char *purchase_order_update_sys_string( void )
{
	static char sys_string[ 256 ];
	char *key;

	key = "full_name,street_address,purchase_date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 "purchase_order",
		 key );

	return sys_string;
}

char *purchase_vendor_payment_update_sys_string( void )
{
	static char sys_string[ 256 ];
	char *key;

	key = "full_name,street_address,purchase_date_time,payment_date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 "vendor_payment",
		 key );

	return sys_string;

} /* purchase_vendor_payment_get_update_sys_string() */

void purchase_order_update(	char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				double sum_extension,
				double database_sum_extension,
				double purchase_amount,
				double database_purchase_amount,
				double amount_due,
				double database_amount_due,
				char *transaction_date_time,
				char *database_transaction_date_time,
				char *arrived_date_time,
				char *database_arrived_date_time,
				char *shipped_date,
				char *database_shipped_date )
{
	char *sys_string;
	FILE *output_pipe;

	sys_string = purchase_order_get_update_sys_string();
	output_pipe = popen( sys_string, "w" );

	if ( !double_virtually_same(
			sum_extension,
			database_sum_extension ) )
	{
		fprintf(output_pipe,
		 	"%s^%s^%s^sum_extension^%.2lf\n",
			full_name,
			street_address,
			purchase_date_time,
			sum_extension );
	}

	if ( !dollar_virtually_same(
			purchase_amount,
			database_purchase_amount ) )
	{
		fprintf(output_pipe,
	 		"%s^%s^%s^purchase_amount^%.2lf\n",
			full_name,
			street_address,
			purchase_date_time,
			purchase_amount );
	}

	if ( !dollar_virtually_same(
			amount_due,
			database_amount_due ) )
	{
		fprintf(output_pipe,
	 		"%s^%s^%s^amount_due^%.2lf\n",
			full_name,
			street_address,
			purchase_date_time,
			amount_due );
	}

	if ( timlib_strcmp(	transaction_date_time,
				database_transaction_date_time ) != 0 )
	{
		fprintf(output_pipe,
		 	"%s^%s^%s^transaction_date_time^%s\n",
			full_name,
			street_address,
			purchase_date_time,
			(transaction_date_time)
				? transaction_date_time
				: "" );
	}

	if ( timlib_strcmp(	arrived_date_time,
				database_arrived_date_time ) != 0 )
	{
		fprintf(output_pipe,
	 		"%s^%s^%s^arrived_date_time^%s\n",
	 		full_name,
	 		street_address,
	 		purchase_date_time,
	 		(arrived_date_time)
	 			? arrived_date_time
				: "" );
	}

	if ( timlib_strcmp(	shipped_date,
				database_shipped_date ) != 0 )
	{
		fprintf(output_pipe,
	 		"%s^%s^%s^shipped_date^%s\n",
	 		full_name,
	 		street_address,
	 		purchase_date_time,
	 		(shipped_date)
				? shipped_date
				: "" );
	}

	pclose( output_pipe );

} /* purchase_order_update() */

void purchase_order_arrived_date_time_update(
				PURCHASE_ORDER *purchase_order )
{
	char *sys_string;
	FILE *output_pipe;
	char buffer[ 128 ];

	if ( !purchase_order ) return;

	sys_string = purchase_order_update_sys_string();
	output_pipe = popen( sys_string, "w" );

	fprintf(output_pipe,
	 	"%s^%s^%s^arrived_date_time^%s\n",
		escape_character(	buffer,
					purchase_order->full_name,
					'\'' ),
		purchase_order->street_address,
		purchase_order->purchase_date_time,
		(purchase_order->arrived_date_time)
			? purchase_order->arrived_date_time
			: "" );

	pclose( output_pipe );

} /* purchase_order_arrived_date_time_update() */

FILE *purchase_fixed_asset_get_update_pipe( void )
{
	char sys_string[ 256 ];
	char *key;

	key =
"full_name,street_address,purchase_date_time,asset_name,serial_number";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 "fixed_asset_purchase",
		 key );

	return popen( sys_string, "w" );
}

void purchase_fixed_asset_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			double finance_accumulated_depreciation,
			double database_finance_accumulated_depreciation )
{
	FILE *update_pipe;

	if ( double_virtually_same(
			finance_accumulated_depreciation,
			database_finance_accumulated_depreciation ) )
	{
		return;
	}

	update_pipe = purchase_fixed_asset_get_update_pipe();

	purchase_fixed_asset_update_stream(
			update_pipe,
			full_name,
			street_address,
			purchase_date_time,
			asset_name,
			serial_number,
			finance_accumulated_depreciation );

	pclose( update_pipe );

} /* purchase_fixed_asset_update() */

void purchase_fixed_asset_update_stream(
			FILE *update_pipe,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *serial_number,
			double finance_accumulated_depreciation )
{
	fprintf(update_pipe,
		"%s^%s^%s^%s^%s^finance_accumulated_depreciation^%.2lf\n",
		full_name,
		street_address,
		purchase_date_time,
		asset_name,
		serial_number,
		finance_accumulated_depreciation );

} /* purchase_fixed_asset_update_stream() */

void purchase_vendor_payment_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *payment_date_time,
			char *transaction_date_time,
			char *database_transaction_date_time )
{
	char *sys_string;
	FILE *output_pipe;

	if ( timlib_strcmp(	transaction_date_time,
				database_transaction_date_time ) == 0 )
	{
		return;
	}

	sys_string =
		purchase_vendor_payment_update_sys_string();

	output_pipe = popen( sys_string, "w" );

	fprintf(output_pipe,
	 	"%s^%s^%s^%s^transaction_date_time^%s\n",
		full_name,
		street_address,
		purchase_date_time,
		payment_date_time,
		(transaction_date_time)
			? transaction_date_time
			: "" );

	pclose( output_pipe );

} /* purchase_vendor_payment_update() */

void purchase_order_shipped_date_update(
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *shipped_date,
				char *application_name )
{
	char *sys_string;
	FILE *output_pipe;

	sys_string = purchase_order_update_sys_string();
	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
	 	"%s^%s^%s^shipped_date^%s\n",
		full_name,
		street_address,
		purchase_date_time,
		(shipped_date)
			? shipped_date
			: "" );

	pclose( output_pipe );

} /* purchase_order_shipped_date_update() */

void purchase_order_transaction_date_time_update(
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *transaction_date_time,
				char *application_name )
{
	char *sys_string;
	FILE *output_pipe;
	char buffer[ 128 ];

	sys_string = purchase_order_update_sys_string();
	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
	 	"%s^%s^%s^transaction_date_time^%s\n",
		escape_character(	buffer,
					full_name,
					'\'' ),
		street_address,
		purchase_date_time,
		(transaction_date_time)
			? transaction_date_time
			: "" );

	pclose( output_pipe );

} /* purchase_order_transaction_date_time_update() */

/* Returns propagate_account_list */
/* ------------------------------ */
LIST *purchase_vendor_payment_journal_ledger_refresh(
					char *application_name,
					char *fund_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					double payment_amount,
					int check_number )
{
	char *checking_account = {0};
	char *uncleared_checks_account = {0};
	char *account_payable_account = {0};
	LIST *propagate_account_list = {0};
	char *credit_account_name;

	ledger_delete(			application_name,
					LEDGER_FOLDER_NAME,
					full_name,
					street_address,
					transaction_date_time );

	ledger_get_vendor_payment_account_names(
		&checking_account,
		&uncleared_checks_account,
		&account_payable_account,
		application_name,
		fund_name );

	if (	!account_payable_account
	||	!checking_account
	||	!account_payable_account )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot get vendor payment account names.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	ledger_journal_ledger_insert(
		application_name,
		full_name,
		street_address,
		transaction_date_time,
		account_payable_account,
		payment_amount,
		1 /* is_debit */ );

	if ( check_number && uncleared_checks_account )
		credit_account_name = uncleared_checks_account;
	else
		credit_account_name = checking_account;

	ledger_journal_ledger_insert(
		application_name,
		full_name,
		street_address,
		transaction_date_time,
		credit_account_name,
		payment_amount,
		0 /* not is_debit */ );

	propagate_account_list = list_new();

	ledger_append_propagate_account_list(
		propagate_account_list,
		transaction_date_time,
		checking_account,
		application_name );

	ledger_append_propagate_account_list(
		propagate_account_list,
		transaction_date_time,
		uncleared_checks_account,
		application_name );

	ledger_append_propagate_account_list(
		propagate_account_list,
		transaction_date_time,
		account_payable_account,
		application_name );

	return propagate_account_list;

} /* purchase_vendor_payment_journal_ledger_refresh() */

char *purchase_get_arrived_purchase_date_time(
				char *application_name,
				char *arrived_date_time )
{
	char sys_string[ 1024 ];
	char where[ 128 ];

	if ( !arrived_date_time || !*arrived_date_time ) return (char *)0;

	sprintf( where,
		 "arrived_date_time = '%s'",
		 arrived_date_time );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=purchase_date_time	"
		 "			folder=purchase_order		"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return pipe2string( sys_string );

} /* purchase_get_arrived_purchase_date_time() */
 
char *purchase_order_fetch_arrived_date_time(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time )
{

	char sys_string[ 1024 ];
	char *where;

	where = ledger_get_transaction_where(
			full_name,
			street_address,
			purchase_date_time,
			"purchase_order",
			"purchase_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=arrived_date_time	"
		 "			folder=purchase_order		"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return pipe2string( sys_string );

} /* purchase_order_fetch_arrived_date_time() */

char *purchase_order_fetch_transaction_date_time(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time )
{

	char sys_string[ 1024 ];
	char *where;

	where = ledger_get_transaction_where(
			full_name,
			street_address,
			purchase_date_time,
			"purchase_order",
			"purchase_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=transaction_date_time	"
		 "			folder=purchase_order		"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return pipe2string( sys_string );

} /* purchase_order_fetch_transaction_date_time() */

PURCHASE_ORDER *purchase_order_seek(
				LIST *purchase_order_list,
				char *purchase_date_time )
{
	PURCHASE_ORDER *purchase_order;

	if ( !list_rewind( purchase_order_list ) )
		return (PURCHASE_ORDER *)0;

	do {
		purchase_order =
			list_get( 
				purchase_order_list );

		if ( strcmp(	purchase_order->purchase_date_time,
				purchase_date_time ) == 0 )
		{
			return purchase_order;
		}

	} while( list_next( purchase_order_list ) );

	return (PURCHASE_ORDER *)0;

} /* purchase_order_seek() */

char *purchase_order_get_inventory_purchase_join_where( void )
{
	char *join_where;

	join_where =	"purchase_order.full_name =			"
			"	inventory_purchase.full_name and	"
			"purchase_order.street_address =		"
			"	inventory_purchase.street_address and	"
			"purchase_order.purchase_date_time =		"
			"	inventory_purchase.purchase_date_time	";

	return join_where;

} /* purchase_order_get_inventory_purchase_join_where() */

LIST *purchase_get_inventory_purchase_list(
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				HASH_TABLE *inventory_purchase_hash_table,
				LIST *inventory_purchase_name_list )
{
	char *inventory_name;
	INVENTORY_PURCHASE *inventory_purchase;
	LIST *inventory_purchase_list = {0};
	char *key;

	if ( !list_rewind( inventory_purchase_name_list ) )
		return (LIST *)0;

	do {
		inventory_name = list_get( inventory_purchase_name_list );

		key = inventory_purchase_get_hash_table_key(
				full_name,
				street_address,
				purchase_date_time,
				inventory_name );

		if ( ( inventory_purchase =
			hash_table_fetch(
				inventory_purchase_hash_table,
				key ) ) )
		{
			if ( !inventory_purchase_list )
				inventory_purchase_list = list_new();

			list_append_pointer(
				inventory_purchase_list,
				inventory_purchase );
		}

	} while( list_next( inventory_purchase_name_list ) );

	return inventory_purchase_list;

} /* purchase_get_inventory_purchase_list() */

LIST *purchase_get_inventory_purchase_order_list(
			char *application_name,
			char *inventory_name,
			HASH_TABLE *inventory_purchase_hash_table,
			LIST *inventory_purchase_name_list,
			HASH_TABLE *transaction_hash_table,
			HASH_TABLE *journal_ledger_hash_table )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char *inventory_subquery;
	char *select;
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	PURCHASE_ORDER *purchase_order;
	LIST *purchase_order_list = {0};

	select = purchase_order_get_select( application_name );

	inventory_subquery =
		inventory_get_subquery(
			inventory_name,
			"purchase_order"     /* one2m_folder_name */,
			"inventory_purchase" /* mto1_folder_name */,
			"purchase_date_time" /* foreign_attribute_name */ );

	sprintf(	where,
			"%s and arrived_date_time is not null",
			inventory_subquery );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=purchase_order		"
		 "			where=\"%s\"			"
		 "			order=arrived_date_time		",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		purchase_order = purchase_order_calloc();

		purchase_order_parse(
			&purchase_order->full_name,
			&purchase_order->street_address,
			&purchase_order->purchase_date_time,
			&purchase_order->sum_extension,
			&purchase_order->database_sum_extension,
			&purchase_order->sales_tax,
			&purchase_order->freight_in,
			&purchase_order->purchase_amount,
			&purchase_order->database_purchase_amount,
			&purchase_order->amount_due,
			&purchase_order->database_amount_due,
			&purchase_order->title_passage_rule,
			&purchase_order->shipped_date,
			&purchase_order->database_shipped_date,
			&purchase_order->arrived_date_time,
			&purchase_order->database_arrived_date_time,
			&purchase_order->transaction_date_time,
			&purchase_order->database_transaction_date_time,
			&purchase_order->fund_name,
			&purchase_order->property_street_address,
			input_buffer );

		purchase_order->inventory_purchase_list =
			purchase_get_inventory_purchase_list(
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->purchase_date_time,
				inventory_purchase_hash_table,
				inventory_purchase_name_list );

		purchase_order->sum_inventory_extension =
			inventory_purchase_list_set_extension(
				purchase_order->inventory_purchase_list );

		inventory_purchase_list_set_capitalized_unit_cost(
			purchase_order->inventory_purchase_list,
			purchase_order->sum_inventory_extension,
			purchase_order->sales_tax,
			purchase_order->freight_in );

		purchase_order->purchase_amount =
			purchase_order_get_purchase_amount(
				&purchase_order->sum_inventory_extension,
				&purchase_order->
					sum_specific_inventory_extension,
				&purchase_order->sum_supply_extension,
				&purchase_order->sum_service_extension,
				&purchase_order->sum_fixed_asset_extension,
				&purchase_order->sum_prepaid_asset_extension,
				&purchase_order->sum_extension,
				purchase_order->inventory_purchase_list,
				purchase_order->
					specific_inventory_purchase_list,
				purchase_order->supply_purchase_list,
				purchase_order->service_purchase_list,
				purchase_order->fixed_asset_purchase_list,
				purchase_order->prepaid_asset_purchase_list,
				purchase_order->sales_tax,
				purchase_order->freight_in );

		if ( purchase_order->transaction_date_time )
		{
			/* -------------------------------------------- */
			/* This excludes sales_tax and freight_in	*/
			/* which are capitalized into inventory.	*/
			/* -------------------------------------------- */
			purchase_order->transaction =
			 inventory_purchase_hash_table_build_transaction(
					application_name,
					purchase_order->fund_name,
					purchase_order->full_name,
					purchase_order->street_address,
					purchase_order->transaction_date_time,
					transaction_hash_table,
					journal_ledger_hash_table );
		}

		if ( !purchase_order_list )
			purchase_order_list = list_new();

		list_append_pointer( purchase_order_list, purchase_order );
	}

	pclose( input_pipe );
	return purchase_order_list;

} /* purchase_get_inventory_purchase_order_list() */

boolean purchase_inventory_is_latest(
				char *application_name,
				char *inventory_name,
				char *purchase_date_time )
{
	INVENTORY *inventory;

	inventory = inventory_load_new( application_name, inventory_name );

	inventory->
		last_inventory_balance->
		inventory_purchase =
			inventory_get_last_inventory_purchase(
				application_name,
				inventory_name );

	/* If deleted the last inventory purchase. */
	/* --------------------------------------- */
	if ( !inventory->last_inventory_balance->inventory_purchase )
		return 0;

	inventory->
		last_inventory_balance->
		inventory_sale =
			inventory_get_last_inventory_sale(
				application_name,
				inventory_name );

	return inventory_is_latest_purchase(
			inventory->
				last_inventory_balance->
				inventory_purchase->
				purchase_date_time,
			inventory->
				last_inventory_balance->
				inventory_sale,
			inventory->
				last_inventory_balance->
				inventory_purchase->
				arrived_date_time,
			purchase_date_time );

} /* purchase_inventory_is_latest() */

void purchase_order_transaction_delete_with_propagate(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time )
{
	ledger_delete(	application_name,
			TRANSACTION_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

	entity_propagate_purchase_order_ledger_accounts(
		application_name,
		fund_name,
		transaction_date_time );

} /* purchase_order_transaction_delete_with_propagate() */

double purchase_get_sum_payment_amount(
				LIST *vendor_payment_list )
{
	VENDOR_PAYMENT *vendor_payment;
	double sum_payment_amount;

	if ( !list_rewind( vendor_payment_list ) ) return 0.0;

	sum_payment_amount = 0.0;

	do {
		vendor_payment = list_get( vendor_payment_list );

		sum_payment_amount += vendor_payment->payment_amount;

	} while( list_next( vendor_payment_list ) );

	return sum_payment_amount;

} /* purchase_get_sum_payment_amount() */

VENDOR_PAYMENT *purchase_vendor_payment_new(
					char *payment_date_time )
{
	VENDOR_PAYMENT *p =
		(VENDOR_PAYMENT *)
			calloc( 1, sizeof( VENDOR_PAYMENT ) );

	if ( !p )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	p->payment_date_time = payment_date_time;
	return p;

} /* purchase_vendor_payment_new() */


VENDOR_PAYMENT *purchase_vendor_payment_seek(
				LIST *vendor_payment_list,
				char *payment_date_time )
{
	VENDOR_PAYMENT *vendor_payment;

	if ( !list_rewind( vendor_payment_list ) ) return (VENDOR_PAYMENT *)0;

	do {
		vendor_payment = list_get( vendor_payment_list );

		if ( timlib_strcmp(	vendor_payment->payment_date_time,
					payment_date_time ) == 0 )
		{
			return vendor_payment;
		}

	} while( list_next( vendor_payment_list ) );

	return (VENDOR_PAYMENT *)0;

} /* purchase_vendor_payment_seek() */

LIST *purchase_get_vendor_payment_list(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;
	char input_buffer[ 1024 ];
	char piece_buffer[ 256 ];
	FILE *input_pipe;
	VENDOR_PAYMENT *vendor_payment;
	LIST *payment_list;

	select =
	"payment_date_time,payment_amount,check_number,transaction_date_time";

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=vendor_payment		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );
	payment_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		vendor_payment =
			purchase_vendor_payment_new(
				strdup( piece_buffer ) );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		if ( *piece_buffer )
			vendor_payment->payment_amount = atof( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		if ( *piece_buffer )
			vendor_payment->check_number = atoi( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
		if ( *piece_buffer )
		{
			vendor_payment->transaction_date_time =
			vendor_payment->database_transaction_date_time =
				strdup( piece_buffer );

			vendor_payment->transaction =
				transaction_fetch(
					full_name,
					street_address,
					vendor_payment->transaction_date_time );
		}

		list_append_pointer( payment_list, vendor_payment );
	}

	pclose( input_pipe );
	return payment_list;

} /* purchase_get_vendor_payment_list() */

PURCHASE_SERVICE *purchase_service_list_seek(
			LIST *service_purchase_list,
			char *account )
{
	PURCHASE_SERVICE *purchase_service;

	if ( !list_rewind( service_purchase_list ) )
		return (PURCHASE_SERVICE *)0;

	do {
		purchase_service = list_get( service_purchase_list );

		if ( strcmp(	purchase_service->account_name,
				account ) == 0 )
		{
			return purchase_service;
		}

	} while( list_next( service_purchase_list ) );

	return (PURCHASE_SERVICE *)0;

} /* purchase_service_list_seek() */

PURCHASE_PREPAID_ASSET *purchase_prepaid_asset_list_seek(
				LIST *prepaid_asset_purchase_list,
				char *asset_name )
{
	PURCHASE_PREPAID_ASSET *purchase_prepaid_asset;

	if ( !list_rewind( prepaid_asset_purchase_list ) )
		return (PURCHASE_PREPAID_ASSET *)0;

	do {
		purchase_prepaid_asset =
			list_get( prepaid_asset_purchase_list );

		if ( strcmp(	purchase_prepaid_asset->asset_name,
				asset_name ) == 0 )
		{
			return purchase_prepaid_asset;
		}

	} while( list_next( prepaid_asset_purchase_list ) );

	return (PURCHASE_PREPAID_ASSET *)0;

} /* purchase_prepaid_asset_list_seek() */

PURCHASE_SUPPLY *purchase_supply_list_seek(
			LIST *supply_purchase_list,
			char *supply_name )
{
	PURCHASE_SUPPLY *purchase_supply;

	if ( !list_rewind( supply_purchase_list ) )
		return (PURCHASE_SUPPLY *)0;

	do {
		purchase_supply = list_get( supply_purchase_list );

		if ( strcmp(	purchase_supply->supply_name,
				supply_name ) == 0 )
		{
			return purchase_supply;
		}

	} while( list_next( supply_purchase_list ) );

	return (PURCHASE_SUPPLY *)0;

} /* purchase_supply_list_seek() */

void purchase_supply_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *supply_name,
				double extension,
				double database_extension )
{
	FILE *update_pipe;
	char *table_name;
	char *key_column_list_string;
	char sys_string[ 1024 ];

	if ( dollar_virtually_same( extension, database_extension ) )
	{
		return;
	}

	table_name = get_table_name( application_name, "supply_purchase" );

	key_column_list_string =
		"full_name,street_address,purchase_date_time,supply_name";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 table_name,
		 key_column_list_string );

	update_pipe = popen( sys_string, "w" );

	fprintf(update_pipe,
	 	"%s^%s^%s^%s^extension^%.2lf\n",
	 	full_name,
	 	street_address,
	 	purchase_date_time,
	 	supply_name,
	 	extension );

	pclose( update_pipe );

} /* purchase_supply_update() */

double purchase_order_get_amount_due(	char *application_name,
					char *full_name,
					char *street_address,
					char *purchase_date_time )
{
	char *table_name;
	char sys_string[ 1024 ];
	char *select;
	char *where;
	char *results_string;

	select = "amount_due";

	table_name = get_table_name( application_name, "purchase_order" );

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( sys_string,
		 "echo \"	select %s			 "
		 "		from %s				 "
		 "		where %s;\"			|"
		 "sql.e						 ",
		 select,
		 table_name,
		 where );

	results_string = pipe2string( sys_string );

	if ( !results_string )
		return 0.0;
	else
		return atof( results_string );

} /* purchase_order_get_amount_due() */

double purchase_order_get_total_payment(	char *application_name,
						char *full_name,
						char *street_address,
						char *purchase_date_time )
{
	char *table_name;
	char sys_string[ 1024 ];
	char *where;
	char *results_string;

	table_name = get_table_name( application_name, "vendor_payment" );

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( sys_string,
		 "echo \"	select sum( payment_amount )	 "
		 "		from %s				 "
		 "		where %s;\"			|"
		 "sql.e						 ",
		 table_name,
		 where );

	results_string = pipe2string( sys_string );

	if ( !results_string )
		return 0.0;
	else
		return atof( results_string );

} /* purchase_order_get_total_payment() */

double purchase_get_asset_account_debit_amount(
				LIST *fixed_asset_purchase_list,
				LIST *prepaid_asset_purchase_list,
				char *asset_account_name )
{
	double debit_amount;
	FIXED_ASSET *purchase_fixed_asset;
	PURCHASE_PREPAID_ASSET *purchase_prepaid_asset;

	if ( !list_length( fixed_asset_purchase_list )
	&&   !list_length( prepaid_asset_purchase_list ) )
		return 0.0;

	debit_amount = 0.0;

	if ( list_rewind( fixed_asset_purchase_list ) )
	{
		do {
			purchase_fixed_asset =
				list_get( fixed_asset_purchase_list );
	
			if ( !purchase_fixed_asset->debit_account_name )
			{
				fprintf( stderr,
		"ERROR in %s/%s()/%d: empty account for fixed_asset = %s.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__,
					purchase_fixed_asset->asset_name );
				exit( 1 );
			}
	
			if ( timlib_strcmp(	purchase_fixed_asset->
							debit_account_name,
						asset_account_name ) == 0 )
			{
				debit_amount +=
					purchase_fixed_asset->extension;
			}
	
		} while( list_next( fixed_asset_purchase_list ) );
	}

	if ( list_rewind( prepaid_asset_purchase_list ) )
	{
		do {
			purchase_prepaid_asset =
				list_get( prepaid_asset_purchase_list );
	
			if ( !purchase_prepaid_asset->asset_account_name )
			{
				fprintf( stderr,
		"ERROR in %s/%s()/%d: empty account for prepaid_asset = %s.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__,
					purchase_prepaid_asset->asset_name );
				exit( 1 );
			}
	
			if ( timlib_strcmp(	purchase_prepaid_asset->
							asset_account_name,
						asset_account_name ) == 0 )
			{
				debit_amount +=
					purchase_prepaid_asset->extension;
			}
	
		} while( list_next( prepaid_asset_purchase_list ) );
	}

	return debit_amount;

} /* purchase_get_asset_account_debit_amount() */

char *purchase_prepaid_asset_get_select( void )
{
	char *select =
"full_name,street_address,purchase_date_time,prepaid_asset_purchase.asset_name,prepaid_asset.asset_account,prepaid_asset.expense_account,extension,accrual_period_years,accumulated_accrual";
	return select;
}

PURCHASE_PREPAID_ASSET *purchase_prepaid_asset_parse( char *input_buffer )
{
	PURCHASE_PREPAID_ASSET *purchase_prepaid_asset;
	char piece_buffer[ 256 ];

	purchase_prepaid_asset = purchase_prepaid_asset_new();

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	purchase_prepaid_asset->full_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	purchase_prepaid_asset->street_address = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	purchase_prepaid_asset->purchase_date_time = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	purchase_prepaid_asset->asset_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	if ( *piece_buffer )
		purchase_prepaid_asset->asset_account_name =
			strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	if ( *piece_buffer )
		purchase_prepaid_asset->expense_account_name =
			strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
	if ( *piece_buffer )
		purchase_prepaid_asset->extension = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );
	if ( *piece_buffer )
		purchase_prepaid_asset->accrual_period_years =
			atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 8 );
	if ( *piece_buffer )
		purchase_prepaid_asset->accumulated_accrual =
		purchase_prepaid_asset->database_accumulated_accrual =
			atof( piece_buffer );

	return purchase_prepaid_asset;

} /* purchase_prepaid_asset_parse() */

PURCHASE_ORDER *purchase_order_transaction_date_time_fetch(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time )
{
	char sys_string[ 2048 ];
	char *where;
	char *select;
	char *results;
	PURCHASE_ORDER *purchase_order;

	select = purchase_order_get_select( application_name );

	where = ledger_get_transaction_where(
			full_name,
			street_address,
			transaction_date_time,
			(char *)0 /* folder_name */,
			"transaction_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=purchase_order		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
		return (PURCHASE_ORDER *)0;

	purchase_order = purchase_order_calloc();

	purchase_order_parse(
			&purchase_order->full_name,
			&purchase_order->street_address,
			&purchase_order->purchase_date_time,
			&purchase_order->sum_extension,
			&purchase_order->database_sum_extension,
			&purchase_order->sales_tax,
			&purchase_order->freight_in,
			&purchase_order->purchase_amount,
			&purchase_order->database_purchase_amount,
			&purchase_order->amount_due,
			&purchase_order->database_amount_due,
			&purchase_order->title_passage_rule,
			&purchase_order->shipped_date,
			&purchase_order->database_shipped_date,
			&purchase_order->arrived_date_time,
			&purchase_order->database_arrived_date_time,
			&purchase_order->transaction_date_time,
			&purchase_order->database_transaction_date_time,
			&purchase_order->fund_name,
			&purchase_order->property_street_address,
			results );

	free( results );

	return purchase_order;

} /* purchase_order_transaction_date_time_fetch() */

PURCHASE_PREPAID_ASSET *purchase_prepaid_asset_fetch(
					char *application_name,
					char *full_name,
					char *street_address,
					char *purchase_date_time,
					char *asset_name )
{
	char sys_string[ 2048 ];
	char *ledger_where;
	char buffer[ 256 ];
	char where[ 1024 ];
	char *join_where;
	char *select;
	char *folder;
	char *results;
	PURCHASE_PREPAID_ASSET *purchase_prepaid_asset;

	select = purchase_prepaid_asset_get_select();

	folder = "prepaid_asset_purchase,prepaid_asset";

	ledger_where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	join_where =
	"prepaid_asset_purchase.asset_name = prepaid_asset.asset_name";

	sprintf( where,
		 "%s and prepaid_asset_purchase.asset_name = '%s' and %s",
		 ledger_where,
		 escape_character(	buffer,
					asset_name,
					'\'' ),
		 join_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		return (PURCHASE_PREPAID_ASSET *)0;
	}

	purchase_prepaid_asset = purchase_prepaid_asset_parse( results );

	purchase_prepaid_asset->accrual_list =
		accrual_fetch_list(
			application_name,
			purchase_prepaid_asset->full_name,
			purchase_prepaid_asset->street_address,
			purchase_prepaid_asset->purchase_date_time,
			purchase_prepaid_asset->asset_name );

	return purchase_prepaid_asset;

} /* purchase_prepaid_asset_fetch() */

LIST *purchase_prepaid_asset_get_list(	char *application_name,
					char *full_name,
					char *street_address,
					char *purchase_date_time )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char *ledger_where;
	char *join_where;
	char *select;
	char *folder;
	char input_buffer[ 2048 ];
	FILE *input_pipe;
	PURCHASE_PREPAID_ASSET *purchase_prepaid_asset;
	LIST *prepaid_asset_purchase_list;

	select = purchase_prepaid_asset_get_select();

	folder = "prepaid_asset_purchase,prepaid_asset";

	join_where =
	"prepaid_asset_purchase.asset_name = prepaid_asset.asset_name";

	ledger_where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( where, "%s and %s", ledger_where, join_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );
	prepaid_asset_purchase_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		purchase_prepaid_asset =
			purchase_prepaid_asset_parse(
				input_buffer );

		purchase_prepaid_asset->accrual_list =
			accrual_fetch_list(
				application_name,
				purchase_prepaid_asset->full_name,
				purchase_prepaid_asset->street_address,
				purchase_prepaid_asset->purchase_date_time,
				purchase_prepaid_asset->asset_name );

		list_append_pointer(	prepaid_asset_purchase_list,
					purchase_prepaid_asset );
	}

	pclose( input_pipe );

	return prepaid_asset_purchase_list;

} /* purchase_prepaid_asset_get_list() */

void purchase_prepaid_asset_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			double accumulated_accrual,
			double database_accumulated_accrual )
{
	char *sys_string;
	FILE *output_pipe;

	if ( double_virtually_same(
			accumulated_accrual,
			database_accumulated_accrual ) )
	{
		return;
	}

	sys_string =
		purchase_prepaid_asset_get_update_sys_string(
			application_name );

	output_pipe = popen( sys_string, "w" );

	fprintf(output_pipe,
		"%s^%s^%s^%s^accumulated_accrual^%.2lf\n",
		full_name,
		street_address,
		purchase_date_time,
		asset_name,
		accumulated_accrual );

	pclose( output_pipe );

} /* purchase_prepaid_asset_update() */

void purchase_accrual_update_and_transaction_propagate(
			PURCHASE_PREPAID_ASSET *purchase_prepaid_asset,
			char *arrived_date_string,
			char *application_name )
{
	if ( !list_length( purchase_prepaid_asset->accrual_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty accrual_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	purchase_prepaid_asset->accumulated_accrual =
		accrual_list_set(
			purchase_prepaid_asset->accrual_list,
			/* ----------------------------------- */
			/* Arrived date is the effective date. */
			/* ----------------------------------- */
			arrived_date_string,
			purchase_prepaid_asset->extension,
			purchase_prepaid_asset->accrual_period_years );

	accrual_list_update_and_transaction_propagate(
		purchase_prepaid_asset->accrual_list,
		application_name,
		purchase_prepaid_asset->asset_account_name,
		purchase_prepaid_asset->expense_account_name );

	purchase_prepaid_asset_update(
			application_name,
			purchase_prepaid_asset->full_name,
			purchase_prepaid_asset->street_address,
			purchase_prepaid_asset->purchase_date_time,
			purchase_prepaid_asset->asset_name,
			purchase_prepaid_asset->accumulated_accrual,
			purchase_prepaid_asset->
				database_accumulated_accrual );

} /* purchase_accrual_update_and_transaction_propagate() */

char *purchase_fixed_asset_get_account_name(
				char *application_name,
				char *asset_name )
{
	char sys_string[ 1024 ];
	char where[ 256 ];
	char buffer[ 128 ];

	sprintf( where,
		 "asset_name = '%s'",
		 escape_character(	buffer,
					asset_name,
					'\'' ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=account		"
		 "			folder=fixed_asset	"
		 "			where=\"%s\"		",
		 application_name,
		 where );

	return pipe2string( sys_string );

} /* purchase_fixed_asset_get_account_name() */

boolean purchase_prepaid_asset_get_account_names(
				char **asset_account_name,
				char **expense_account_name,
				char *application_name,
				char *asset_name )
{
	char sys_string[ 1024 ];
	char where[ 256 ];
	char buffer[ 128 ];
	char *select;
	char *results;
	char piece_buffer[ 128 ];

	select = "asset_account,expense_account";

	sprintf( where,
		 "asset_name = '%s'",
		 escape_character(	buffer,
					asset_name,
					'\'' ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=fixed_asset	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );


	if ( ! ( results = pipe2string( sys_string ) ) ) return 0;

	if ( !*results ) return 0;

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 0 );

	*asset_account_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 1 );

	if ( !*piece_buffer ) return 0;

	*expense_account_name = strdup( piece_buffer );

	return 1;

} /* purchase_fixed_asset_get_account_name() */

void purchase_vendor_payment_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *payment_date_time,
				double payment_amount,
				int check_number,
				char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char *field_list_string;
	FILE *output_pipe;
	char *table_name;
	char buffer[ 128 ];

	field_list_string =
"full_name,street_address,purchase_date_time,payment_date_time,payment_amount,check_number,transaction_date_time";

	table_name = get_table_name( application_name, "vendor_payment" );

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s | sql.e",
		 table_name,
		 field_list_string );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
		 "%s|%s|%s|%s|%.2lf",
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 street_address,
		 purchase_date_time,
		 payment_date_time,
		 payment_amount );

	if ( check_number )
	{
		fprintf( output_pipe,
			 "|%d",
			 check_number );
	}
	else
	{
		fprintf( output_pipe, "|" );
	}

	if ( transaction_date_time )
	{
		fprintf( output_pipe,
			 "|%s\n",
			 transaction_date_time );
	}
	else
	{
		fprintf( output_pipe, "|\n" );
	}

	pclose( output_pipe );

} /* purchase_vendor_payment_insert() */

char *purchase_order_display( PURCHASE_ORDER *purchase_order )
{
	char buffer[ 65536 ];

	sprintf(	buffer,
			"\n"
			"full_name = %s;"
			"street_address = %s;"
			"purchase_date_time = %s;"
			"transaction_date_time = %s;"
			"sum_extension = %.2lf;"
			"sales_tax = %.2lf;"
			"freight_in = %.2lf;"
			"purchase_amount = %.2lf;"
			"sum_payment_amount = %.2lf;"
			"amount_due = %.2lf\n",
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->purchase_date_time,
			purchase_order->transaction_date_time,
			purchase_order->sum_extension,
			purchase_order->sales_tax,
			purchase_order->freight_in,
			purchase_order->purchase_amount,
			purchase_order->sum_payment_amount,
			purchase_order->amount_due );

	return strdup( buffer );

} /* purchase_order_display() */

char *purchase_order_list_display( LIST *purchase_order_list )
{
	char buffer[ 65536 ];
	char *ptr = buffer;
	PURCHASE_ORDER *purchase_order;

	*ptr = '\0';

	if ( list_rewind( purchase_order_list ) )
	{
		do {
			purchase_order = list_get( purchase_order_list );

			ptr += sprintf(
				ptr,
				"\n"
				"purchase_order: %s\n",
				purchase_order_display( purchase_order ) );

		} while( list_next( purchase_order_list ) );

	}

	return strdup( buffer );

} /* purchase_order_list_display() */

void purchase_specific_inventory_set_capitalized_extension(
				LIST *specific_inventory_purchase_list,
				double sum_specific_inventory_extension,
				double sales_tax,
				double freight_in )
{
	PURCHASE_SPECIFIC_INVENTORY *purchase_specific_inventory;
	double extra_cost;
	double percent_of_total;
	double capitalized_extra;

	extra_cost = sales_tax + freight_in;

	if ( !list_rewind( specific_inventory_purchase_list ) ) return;

	do {
		purchase_specific_inventory =
			list_get_pointer(
				specific_inventory_purchase_list );

		if ( timlib_dollar_virtually_same(
			purchase_specific_inventory->extension,
			0.0 ) )
		{
			continue;
		}

		if ( timlib_dollar_virtually_same( extra_cost, 0.0 ) )
		{
			purchase_specific_inventory->capitalized_extension =
				purchase_specific_inventory->extension;
		}
		else
		{
			percent_of_total =
				purchase_specific_inventory->extension /
				sum_specific_inventory_extension;

			capitalized_extra =
				extra_cost * percent_of_total;

			purchase_specific_inventory->capitalized_extension =
				purchase_specific_inventory->extension +
				capitalized_extra;
		}

	} while ( list_next( specific_inventory_purchase_list ) );

} /* purchase_specific_inventory_set_capitalized_extension() */

/* Returns journal_list */
/* --------------------------- */
LIST *purchase_inventory_distinct_account_extract(
					double *sum_debit_amount,
					LIST *inventory_purchase_list )
{
	INVENTORY_PURCHASE *inventory_purchase;
	double extension_capitalized_addition;
	LIST *journal_list;
	JOURNAL *journal;
	double extension_plus_capitalized_addition;

	if ( !list_rewind( inventory_purchase_list ) ) return (LIST *)0;

	journal_list = list_new();

	do {
		inventory_purchase =
			list_get(
				inventory_purchase_list );

		if ( !inventory_purchase->inventory_account_name )
		{
			fprintf( stderr,
	"ERROR in %s/%s()/%d: empty inventory_account_name for (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 inventory_purchase->inventory_name );
			exit( 1 );
		}

		journal =
			journal_getset(
				journal_list,
				inventory_purchase->inventory_account_name );

		extension_capitalized_addition =
			( inventory_purchase->capitalized_unit_cost -
			  inventory_purchase->unit_cost ) *
			  (double)inventory_purchase->ordered_quantity;

		extension_plus_capitalized_addition =
			inventory_purchase->extension +
			extension_capitalized_addition;

		journal->debit_amount +=
			extension_plus_capitalized_addition;

		if ( sum_debit_amount )
		{
			*sum_debit_amount += 
				extension_plus_capitalized_addition;
		}

	} while( list_next( inventory_purchase_list ) );

	return journal_list;
}

/* Returns journal_ledger_list */
/* --------------------------- */
LIST *purchase_supply_distinct_account_extract(
					double *sum_debit_amount,
					LIST *supply_purchase_list )
{
	PURCHASE_SUPPLY *purchase_supply;
	LIST *journal_ledger_list;
	JOURNAL_LEDGER *journal_ledger;

	if ( !list_rewind( supply_purchase_list ) ) return (LIST *)0;

	journal_ledger_list = list_new();

	do {
		purchase_supply =
			list_get_pointer(
				supply_purchase_list );

		if ( !purchase_supply->account_name )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: empty account_name for (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 purchase_supply->supply_name );
			exit( 1 );
		}

		journal_ledger =
			ledger_get_or_set_journal_ledger(
				journal_ledger_list,
				purchase_supply->account_name );

		journal_ledger->debit_amount +=
			purchase_supply->extension;

		if ( sum_debit_amount )
		{
			*sum_debit_amount += 
				purchase_supply->extension;
		}

	} while( list_next( supply_purchase_list ) );

	return journal_ledger_list;

} /* purchase_supply_distinct_account_extract() */

/* Returns journal_ledger_list */
/* --------------------------- */
LIST *purchase_fixed_asset_distinct_account_extract(
					double *sum_debit_amount,
					LIST *fixed_asset_purchase_list )
{
	FIXED_ASSET *purchase_fixed_asset;
	LIST *journal_ledger_list;
	JOURNAL_LEDGER *journal_ledger;

	if ( !list_rewind( fixed_asset_purchase_list ) ) return (LIST *)0;

	journal_ledger_list = list_new();

	do {
		purchase_fixed_asset =
			list_get_pointer(
				fixed_asset_purchase_list );

		if ( !purchase_fixed_asset->debit_account_name )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: empty account_name for (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 purchase_fixed_asset->asset_name );
			exit( 1 );
		}

		journal_ledger =
			ledger_get_or_set_journal_ledger(
				journal_ledger_list,
				purchase_fixed_asset->debit_account_name );

		journal_ledger->debit_amount +=
			purchase_fixed_asset->extension;

		if ( sum_debit_amount )
		{
			*sum_debit_amount += 
				purchase_fixed_asset->extension;
		}

	} while( list_next( fixed_asset_purchase_list ) );

	return journal_ledger_list;

} /* purchase_fixed_asset_distinct_account_extract() */

/* Returns journal_ledger_list */
/* --------------------------- */
LIST *purchase_prepaid_asset_distinct_account_extract(
					double *sum_debit_amount,
					LIST *prepaid_asset_purchase_list )
{
	PURCHASE_PREPAID_ASSET *purchase_prepaid_asset;
	LIST *journal_ledger_list;
	JOURNAL_LEDGER *journal_ledger;

	if ( !list_rewind( prepaid_asset_purchase_list ) ) return (LIST *)0;

	journal_ledger_list = list_new();

	do {
		purchase_prepaid_asset =
			list_get_pointer(
				prepaid_asset_purchase_list );

		if ( !purchase_prepaid_asset->asset_account_name )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: empty asset_account_name for (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 purchase_prepaid_asset->asset_name );
			exit( 1 );
		}

		journal_ledger =
			ledger_get_or_set_journal_ledger(
				journal_ledger_list,
				purchase_prepaid_asset->asset_account_name );

		journal_ledger->debit_amount +=
			purchase_prepaid_asset->extension;

		if ( sum_debit_amount )
		{
			*sum_debit_amount += 
				purchase_prepaid_asset->extension;
		}

	} while( list_next( prepaid_asset_purchase_list ) );

	return journal_ledger_list;

} /* purchase_prepaid_asset_distinct_account_extract() */

LIST *purchase_get_amount_due_purchase_order_list(
			char *application_name )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	PURCHASE_ORDER *purchase_order;
	LIST *purchase_order_list = {0};

	select = purchase_order_get_select( application_name );

	where = "ifnull( amount_due, 0.0 ) > 0.0";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=purchase_order		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		purchase_order = purchase_order_calloc();

		purchase_order_parse(
			&purchase_order->full_name,
			&purchase_order->street_address,
			&purchase_order->purchase_date_time,
			&purchase_order->sum_extension,
			&purchase_order->database_sum_extension,
			&purchase_order->sales_tax,
			&purchase_order->freight_in,
			&purchase_order->purchase_amount,
			&purchase_order->database_purchase_amount,
			&purchase_order->amount_due,
			&purchase_order->database_amount_due,
			&purchase_order->title_passage_rule,
			&purchase_order->shipped_date,
			&purchase_order->database_shipped_date,
			&purchase_order->arrived_date_time,
			&purchase_order->database_arrived_date_time,
			&purchase_order->transaction_date_time,
			&purchase_order->database_transaction_date_time,
			&purchase_order->fund_name,
			&purchase_order->property_street_address,
			input_buffer );

		if ( !purchase_order_list )
			purchase_order_list = list_new();

		list_append_pointer( purchase_order_list, purchase_order );
	}

	pclose( input_pipe );
	return purchase_order_list;

} /* purchase_get_amount_due_purchase_order_list() */

double purchase_get_sum_vendor_payment_amount(
				LIST *vendor_payment_list )
{
	return purchase_get_sum_payment_amount( vendor_payment_list );

}

TRANSACTION *inventory_purchase_hash_table_build_transaction(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				HASH_TABLE *transaction_hash_table,
				HASH_TABLE *journal_ledger_hash_table )
{
	char *inventory_account_name;
	static LIST *inventory_account_name_list = {0};
	static char *account_payable_account = {0};
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	char *key;

	key = ledger_get_transaction_hash_table_key(
			full_name,
			street_address,
			transaction_date_time );

	if ( ! ( transaction =
			hash_table_fetch( 
				transaction_hash_table,
				key ) ) )
	{
		return (TRANSACTION *)0;
	}

	if ( !inventory_account_name_list )
	{
		inventory_account_name_list =
			ledger_get_inventory_account_name_list(
				application_name );

		ledger_get_account_payable_account_name(
			&account_payable_account,
			application_name,
			fund_name );
	}

	/* ================== */
	/* Build journal_list */
	/* ================== */
	transaction->journal_list = list_new();

	/* Account payable */
	/* --------------- */
	key = ledger_get_journal_ledger_hash_table_key(
			full_name,
			street_address,
			transaction_date_time,
			account_payable_account );


	if ( key && ( journal_ledger =
			hash_table_fetch( 
				journal_ledger_hash_table,
				key ) ) )
	{
		list_append_pointer(
			transaction->journal_list,
			journal_ledger );
	}

	/* Inventory and cost of goods sold */
	/* -------------------------------- */
	if ( list_rewind( inventory_account_name_list ) )
	{
		do {
			inventory_account_name =
				list_get_pointer(
					inventory_account_name_list );

			key = ledger_get_journal_ledger_hash_table_key(
					full_name,
					street_address,
					transaction_date_time,
					inventory_account_name );


			if ( key && ( journal_ledger =
					hash_table_fetch( 
						journal_ledger_hash_table,
						key ) ) )
			{
				list_append_pointer(
					transaction->journal_list,
					journal_ledger );
			}

		} while( list_next( inventory_account_name_list ) );
	}

	return transaction;
}

TRANSACTION *purchase_inventory_build_transaction(
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *memo,
				LIST *inventory_purchase_list,
				char *fund_name )
{
	TRANSACTION *transaction;
	JOURNAL *journal;
	char *account_payable_account = {0};
	double sum_debit_amount = 0.0;

	if ( !full_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty full_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Exits if account_payable_account is not found. */
	/* ---------------------------------------------- */
	ledger_get_account_payable_account_name(
				&account_payable_account,
				application_name,
				fund_name );

	transaction =
		transaction_new(
			strdup( full_name ),
			strdup( street_address ),
			strdup( transaction_date_time ),
			strdup( memo ) );

	transaction->journal_list =
		purchase_inventory_distinct_account_extract(
			&sum_debit_amount,
			inventory_purchase_list );

	/* Empty if deleted */
	/* ---------------- */
	if ( !list_rewind( transaction->journal_list ) )
		return (TRANSACTION *)0;

	/* account_payable */
	/* --------------- */
	journal =
		journal_new(
			transaction->full_name,
			transaction->street_address,
			transaction_date_time,
			account_payable_account );

	journal_ledger->credit_amount = sum_debit_amount;

	list_et(
		transaction->journal_list,
		journal );

	transaction->transaction_amount = sum_debit_amount;

	return transaction;
}

TRANSACTION *purchase_specific_inventory_build_transaction(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *memo,
				LIST *specific_inventory_purchase_list,
				char *fund_name )
{
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	PURCHASE_SPECIFIC_INVENTORY *purchase_specific_inventory;
	char *sales_tax_expense_account = {0};
	char *freight_in_expense_account = {0};
	char *account_payable_account = {0};
	double sum_debit_amount = 0.0;

	if ( !full_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty full_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Exits if account_payable_account is not found. */
	/* ---------------------------------------------- */
	ledger_get_purchase_order_account_names(
				&sales_tax_expense_account,
				&freight_in_expense_account,
				&account_payable_account,
				application_name,
				fund_name );

	transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			transaction_date_time,
			memo );

	transaction->journal_ledger_list = list_new();

	/* SPECIFIC_INVENTORY_PURCHASE */
	/* --------------------------- */
	if ( list_rewind( specific_inventory_purchase_list ) )
	{
		do {
			purchase_specific_inventory =
				list_get_pointer(
					specific_inventory_purchase_list );

			journal_ledger =
				journal_ledger_new(
					transaction->full_name,
					transaction->street_address,
					transaction_date_time,
					purchase_specific_inventory->
						inventory_account_name );

			journal_ledger->debit_amount =
				purchase_specific_inventory->
					capitalized_extension;

			list_append_pointer(
				transaction->journal_ledger_list,
				journal_ledger );

			sum_debit_amount +=
				purchase_specific_inventory->
					capitalized_extension;

		} while( list_next( specific_inventory_purchase_list ) );
	}

	/* account_payable */
	/* --------------- */
	journal_ledger =
		journal_ledger_new(
			transaction->full_name,
			transaction->street_address,
			transaction_date_time,
			account_payable_account );

	journal_ledger->credit_amount = sum_debit_amount;

	list_append_pointer(
		transaction->journal_ledger_list,
		journal_ledger );

	transaction->transaction_amount = sum_debit_amount;

	return transaction;

}

TRANSACTION *purchase_purchase_order_build_transaction(
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *memo,
				double sales_tax,
				double freight_in,
				LIST *supply_purchase_list,
				LIST *service_purchase_list,
				LIST *fixed_asset_purchase_list,
				LIST *prepaid_asset_purchase_list,
				char *fund_name )
{
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	PURCHASE_SERVICE *purchase_service;
	char *sales_tax_expense_account = {0};
	char *freight_in_expense_account = {0};
	char *account_payable_account = {0};
	double sum_debit_amount = 0.0;

	if ( !full_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty full_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Exits if account_payable_account is not found. */
	/* ---------------------------------------------- */
	ledger_get_purchase_order_account_names(
				&sales_tax_expense_account,
				&freight_in_expense_account,
				&account_payable_account,
				application_name,
				fund_name );

	if ( sales_tax && !sales_tax_expense_account )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: sales tax exists without sales tax expense account.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( freight_in && !freight_in_expense_account )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: freight in exists without freight in expense account.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			transaction_date_time,
			memo );

	transaction->journal_ledger_list = list_new();

	/* SUPPLY_PURCHASE */
	/* --------------- */
	if ( list_length( supply_purchase_list ) )
	{
		list_append_list(
			transaction->journal_ledger_list,
			purchase_supply_distinct_account_extract(
				&sum_debit_amount,
				supply_purchase_list ) );
	}

	/* SERVICE_PURCHASE */
	/* ---------------- */
	if ( list_rewind( service_purchase_list ) )
	{
		do {
			purchase_service =
				list_get_pointer(
					service_purchase_list );

			journal_ledger =
				journal_ledger_new(
					transaction->full_name,
					transaction->street_address,
					transaction_date_time,
					purchase_service->account_name );

			journal_ledger->debit_amount =
				purchase_service->extension;

			list_append_pointer(
				transaction->journal_ledger_list,
				journal_ledger );

			sum_debit_amount += purchase_service->extension;

		} while( list_next( service_purchase_list ) );
	}

	/* FIXED_ASSET_PURCHASE */
	/* -------------------- */
	if ( list_length( fixed_asset_purchase_list ) )
	{
		list_append_list(
			transaction->journal_ledger_list,
			purchase_fixed_asset_distinct_account_extract(
				&sum_debit_amount,
				fixed_asset_purchase_list ) );
	}

	/* PREPAID_ASSET_PURCHASE */
	/* ---------------------- */
	if ( list_length( prepaid_asset_purchase_list ) )
	{
		list_append_list(
			transaction->journal_ledger_list,
			purchase_prepaid_asset_distinct_account_extract(
				&sum_debit_amount,
				prepaid_asset_purchase_list ) );
	}

	/* sales_tax */
	/* --------- */
	if ( sales_tax )
	{
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction_date_time,
				sales_tax_expense_account );

		journal_ledger->debit_amount = sales_tax;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );

		sum_debit_amount += sales_tax;
	}


	/* freight_in */
	/* ---------- */
	if ( freight_in )
	{
		journal_ledger =
			journal_ledger_new(
				transaction->full_name,
				transaction->street_address,
				transaction_date_time,
				freight_in_expense_account );

		journal_ledger->debit_amount = freight_in;

		list_append_pointer(
			transaction->journal_ledger_list,
			journal_ledger );

		sum_debit_amount += freight_in;
	}

	/* account_payable */
	/* --------------- */
	journal_ledger =
		journal_ledger_new(
			transaction->full_name,
			transaction->street_address,
			transaction_date_time,
			account_payable_account );

	journal_ledger->credit_amount = sum_debit_amount;

	list_append_pointer(
		transaction->journal_ledger_list,
		journal_ledger );

	transaction->transaction_amount = sum_debit_amount;

	return transaction;

}

