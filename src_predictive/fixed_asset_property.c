/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_asset.c			*/
/* -------------------------------------------------------------------- */
/* This is the PredictiveBooks fixed_asset ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"
#include "fixed_asset.h"

FIXED_ASSET *fixed_asset_new( void )
{
	FIXED_ASSET *p =
		(FIXED_ASSET *)
			calloc( 1, sizeof( FIXED_ASSET ) );

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

} /* fixed_asset_new() */

char *fixed_asset_purchase_get_fund_where(
					char **folder_from,
					char *folder_name,
					char *fund_name )
{
	char fund_where[ 1024 ];

	if ( fund_name && *fund_name )
	{
		char buffer[ 128 ];

		sprintf( buffer,
			 "%s,purchase_order",
			 folder_name );

		*folder_from = strdup( buffer );

		sprintf(fund_where,
			"%s.full_name =		"
			"purchase_order.full_name and			"
			"%s.street_address =		"
			"purchase_order.street_address and		"
			"%s.purchase_date_time =	"
			"purchase_order.purchase_date_time and		"
			"fund = '%s'					",
			folder_name,
			folder_name,
			folder_name,
			fund_name );
	}
	else
	{
		*folder_from = folder_name;
		strcpy( fund_where, "1 = 1" );
	}

	return strdup( fund_where );

} /* fixed_asset_purchase_get_fund_where() */

char *fixed_property_select(
				char *folder_name,
				boolean with_account )
{
	char select[ 1024 ];
	char *account_select;
	char *tax_select;

	if ( with_account )
		account_select = "account";
	else
		account_select = "''";

	if ( ledger_tax_recovery_period_attribute_exists(
				environment_application_name() ) )
	{
		tax_select =
	"tax_cost_basis,tax_recovery_period,tax_accumulated_depreciation";
	}
	else
	{
		tax_select = "'','',''";
	}

	sprintf( select,
"%s.property_street_address,'',%s,service_placement_date,structure_cost,estimated_useful_life_years,'',estimated_residual_value,declining_balance_n,depreciation_method,%s,disposal_date,finance_accumulated_depreciation",
		 folder_name,
		 account_select,
		 tax_select );

	return strdup( select) ;
}

char *fixed_asset_select(	char *folder_name,
				boolean with_account )
{
	char select[ 1024 ];
	char *account_select;
	char *tax_select;

	if ( with_account )
		account_select = "account";
	else
		account_select = "''";

	if ( ledger_tax_recovery_period_attribute_exists(
				environment_application_name() ) )
	{
		tax_select =
	"tax_cost_basis,tax_recovery_period,tax_accumulated_depreciation";
	}
	else
	{
		tax_select = "'','',''";
	}

	sprintf( select,
"%s.asset_name,serial_number,%s,service_placement_date,extension,estimated_useful_life_years,estimated_useful_life_units,estimated_residual_value,declining_balance_n,depreciation_method,%s,disposal_date,finance_accumulated_depreciation",
		 folder_name,
		 account_select,
		 tax_select );

	return strdup( select) ;
}

FIXED_ASSET *fixed_asset_parse( char *input_buffer )
{
	FIXED_ASSET *fixed_asset;
	char piece_buffer[ 256 ];

	fixed_asset = fixed_asset_new();

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	fixed_asset->asset_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	fixed_asset->serial_number = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	if ( *piece_buffer )
		fixed_asset->debit_account_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	if ( *piece_buffer )
		fixed_asset->service_placement_date =
			strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	if ( *piece_buffer )
		fixed_asset->extension = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	if ( *piece_buffer )
		fixed_asset->estimated_useful_life_years =
			atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
	if ( *piece_buffer )
		fixed_asset->estimated_useful_life_units =
			atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );
	if ( *piece_buffer )
		fixed_asset->estimated_residual_value =
			atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 8 );
	if ( *piece_buffer )
		fixed_asset->declining_balance_n =
			atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 9 );
	if ( *piece_buffer )
		fixed_asset->depreciation_method =
			strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 10 );
	if ( *piece_buffer )
		fixed_asset->tax_cost_basis =
			atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 11 );
	if ( *piece_buffer )
		fixed_asset->tax_recovery_period =
			strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 12 );
	if ( *piece_buffer )
		fixed_asset->tax_accumulated_depreciation =
		fixed_asset->database_tax_accumulated_depreciation =
			atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 13 );
	if ( *piece_buffer )
		fixed_asset->disposal_date =
			strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 14 );
	if ( *piece_buffer )
		fixed_asset->finance_accumulated_depreciation =
		fixed_asset->
			database_finance_accumulated_depreciation =
				atof( piece_buffer );

	return fixed_asset;

} /* fixed_asset_parse() */

FIXED_ASSET *fixed_asset_purchase_fetch(
					char *asset_name,
					char *serial_number )
{
	char sys_string[ 2048 ];
	char buffer[ 256 ];
	char where[ 1024 ];
	char *join_where;
	char *select;
	char *folder;
	char *results;
	FIXED_ASSET *fixed_asset;

	select =
		fixed_asset_select(
			"fixed_asset_purchase",
			1 /* with_account */ );

	folder = "fixed_asset_purchase,fixed_asset";

	join_where = "fixed_asset_purchase.asset_name = fixed_asset.asset_name";

	sprintf( where,
"fixed_asset_purchase.asset_name = '%s' and serial_number = '%s' and %s",
		 escape_character(	buffer,
					asset_name,
					'\'' ),
		 serial_number,
		 join_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 environment_application_name(),
		 select,
		 folder,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		return (FIXED_ASSET *)0;
	}

	fixed_asset = fixed_asset_parse( results );

	return fixed_asset;
}

char *fixed_asset_tax_where( int tax_year )
{
	char where[ 1024 ];
	char minimum_disposal_date[ 16 ];

	sprintf( minimum_disposal_date,
		 "%d-01-01",
		 tax_year );

	sprintf( where,
		 "(disposal_date is null or disposal_date >= %s) and	"
		 "ifnull(tax_cost_basis,0.0) <> 0.0) and		"
		 "tax_recovery_period is not null and			"
		 "tax_accumulated_depreciation < tax_cost_basis		",
		 minimum_disposal_date );

	return strdup( where );
}

char *fixed_asset_depreciation_where(
				char *fund_where,
				char *cost_attribute_name )
{
	char where[ 1024 ];

	sprintf( where,
		 "depreciation_method is not null and			"
		 "service_placement_date is not null and		"
		 "ifnull(%s,0) <> 0 and					"
		 "ifnull(finance_accumulated_depreciation,0) < %s and	"
		 "%s							",
		 cost_attribute_name,
		 cost_attribute_name,
		 fund_where );

	return strdup( where );
}

LIST *fixed_asset_purchase_list(
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
	FIXED_ASSET *fixed_asset;
	LIST *fixed_asset_purchase_list;

	select =
		fixed_asset_get_select(
			environment_application_name(),
			"fixed_asset_purchase",
			1 /* with_account */ );

	folder = "fixed_asset_purchase,fixed_asset";

	join_where = "fixed_asset_purchase.asset_name = fixed_asset.asset_name";

	ledger_where = ledger_transaction_where(
				full_name,
				street_address,
				purchase_date_time,
				(char *)0 /* folder_name */,
				"purchase_date_time" );

	sprintf( where, "%s and %s", ledger_where, join_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 environment_application_name(),
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );
	fixed_asset_purchase_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		fixed_asset = fixed_asset_parse( input_buffer );

		list_set(	fixed_asset_purchase_list,
					fixed_asset );
	}

	pclose( input_pipe );
	return fixed_asset_purchase_list;
}

LIST *fixed_asset_depreciation_prior_list(
					char *fund_name )
{
	char sys_string[ 1024 ];
	char *where;
	char fund_where[ 128 ];
	char *select;
	char *folder_from = {0};
	char input_buffer[ 2048 ];
	FILE *input_pipe;
	FIXED_ASSET *fixed_asset;
	LIST *fixed_asset_list;

	select =
		fixed_asset_select(
			"prior_fixed_asset",
			0 /* not with_account */ );

	folder_from = "prior_fixed_asset";

	if ( fund_name && *fund_name )
	{
		sprintf( fund_where,
			 "fund = '%s'",
			 fund_name );
	}
	else
	{
		strcpy( fund_where, "1 = 1" );
	}

	where =
		fixed_asset_depreciation_get_where(
			fund_where,
			"extension"
				/* cost_attribute_name */ );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=\"%s\"			"
		 "			where=\"%s\"			"
		 "			order=select			",
		 application_name,
		 select,
		 folder_from,
		 where );

	input_pipe = popen( sys_string, "r" );

	fixed_asset_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		fixed_asset = fixed_asset_parse( input_buffer );

		list_append_pointer(
			fixed_asset_list,
			fixed_asset );
	}

	pclose( input_pipe );

	return fixed_asset_list;
}

LIST *fixed_asset_depreciation_purchase_list(
					char *fund_name )
{
	char sys_string[ 1024 ];
	char *where;
	char *fund_where;
	char *select;
	char *folder_from = {0};
	char input_buffer[ 2048 ];
	FILE *input_pipe;
	FIXED_ASSET *fixed_asset;
	LIST *fixed_asset_list;

	select =
		fixed_asset_select(
			"fixed_asset_purchase",
			0 /* not with_account */ );

	fund_where =
		fixed_asset_purchase_fund_where(
			&folder_from,
			"fixed_asset_purchase"
				/* folder_name */,
			fund_name );

	where =
		fixed_asset_depreciation_where(
			fund_where,
			"extension"
				/* cost_attribute_name */ );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=\"%s\"			"
		 "			where=\"%s\"			"
		 "			order=select			",
		 environment_application_name(),
		 select,
		 folder_from,
		 where );

	input_pipe = popen( sys_string, "r" );

	fixed_asset_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		fixed_asset = fixed_asset_parse( input_buffer );

		list_set( fixed_asset_list, fixed_asset );
	}

	pclose( input_pipe );

	return fixed_asset_list;
}

LIST *fixed_property_depreciation_prior_list(
					char *fund_name )
{
	char sys_string[ 1024 ];
	char *where;
	char fund_where[ 128 ];
	char *select;
	char *folder_from = {0};
	char input_buffer[ 2048 ];
	FILE *input_pipe;
	FIXED_ASSET *fixed_asset;
	LIST *fixed_asset_list;

	select =
		fixed_property_select(
			"prior_property",
			0 /* not with_account */ );

	folder_from = "prior_property";

	if ( fund_name && *fund_name )
	{
		sprintf( fund_where,
			 "fund = '%s'",
			 fund_name );
	}
	else
	{
		strcpy( fund_where, "1 = 1" );
	}

	where =
		fixed_asset_depreciation_get_where(
			fund_where,
			"structure_cost"
				/* cost_attribute_name */ );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=\"%s\"			"
		 "			where=\"%s\"			"
		 "			order=select			",
		 environment_application_name(),
		 select,
		 folder_from,
		 where );

	input_pipe = popen( sys_string, "r" );

	fixed_asset_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		fixed_asset = fixed_asset_parse( input_buffer );

		list_set(
			fixed_asset_list,
			fixed_asset );
	}

	pclose( input_pipe );

	return fixed_asset_list;
}

LIST *fixed_property_depreciation_purchase_list(
					char *fund_name )
{
	char sys_string[ 1024 ];
	char *where;
	char *fund_where;
	char *select;
	char *folder_from = {0};
	char input_buffer[ 2048 ];
	FILE *input_pipe;
	FIXED_ASSET *fixed_asset;
	LIST *fixed_asset_list;

	select =
		fixed_property_select(
			"property_purchase",
			0 /* not with_account */ );

	fund_where =
		fixed_asset_purchase_get_fund_where(
			&folder_from,
			"property_purchase"
				/* folder_name */,
			fund_name );

	where =
		fixed_asset_depreciation_where(
			fund_where,
			"structure_cost"
				/* cost_attribute_name */ );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=\"%s\"			"
		 "			where=\"%s\"			"
		 "			order=select			",
		 environment_application_name(),
		 select,
		 folder_from,
		 where );

	input_pipe = popen( sys_string, "r" );

	fixed_asset_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		fixed_asset = fixed_asset_parse( input_buffer );

		list_set(
			fixed_asset_list,
			fixed_asset );
	}

	pclose( input_pipe );

	return fixed_asset_list;
}

FIXED_ASSET *fixed_asset_seek(
			LIST *fixed_asset_list,
			char *asset_name,
			char *serial_number )
{
	FIXED_ASSET *fixed_asset;

	if ( !list_rewind( fixed_asset_list ) )
		return (FIXED_ASSET *)0;

	do {
		fixed_asset = list_get( fixed_asset_list );

		if ( strcmp(	fixed_asset->asset_name,
				asset_name ) == 0
		&&   strcmp(	fixed_asset->serial_number,
				serial_number ) == 0 )
		{
			return fixed_asset;
		}

	} while( list_next( fixed_asset_list ) );

	return (FIXED_ASSET *)0;

}

LIST *fixed_asset_tax_list(
			int tax_year,
			char *folder_name )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;
	char input_buffer[ 2048 ];
	FILE *input_pipe;
	FIXED_ASSET *fixed_asset;
	LIST *fixed_asset_list;

	select =
		fixed_asset_select(
			folder_name,
			0 /* not with_account */ );

	where = fixed_asset_tax_get_where( tax_year );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=\"%s\"			"
		 "			where=\"%s\"			"
		 "			order=select			",
		 environment_application_name(),
		 select,
		 folder_name,
		 where );

	input_pipe = popen( sys_string, "r" );

	fixed_asset_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		fixed_asset = fixed_asset_parse( input_buffer );

		list_set( fixed_asset_list, fixed_asset );
	}

	pclose( input_pipe );

	return fixed_asset_list;
}

