/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/purchase_fetch.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "entity.h"
#include "predictive.h"
#include "sale_fetch.h"
#include "fixed_asset_purchase.h"
#include "specific_inventory_purchase.h"
#include "supply_purchase.h"
#include "purchase.h"
#include "purchase_fetch.h"

PURCHASE_FETCH *purchase_fetch_new(
		const char *purchase_select,
		const char *purchase_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time )
{
	char *select;
	char *system_string;
	char *input;
	char *primary_where;
	PURCHASE_FETCH *purchase_fetch;

	if ( !full_name
	||   !purchase_date_time )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	purchase_fetch = purchase_fetch_calloc();

	purchase_fetch->folder_fetch =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			purchase_table /* folder_name */,
			(LIST *)0 /* role_attribute_exclude_name_list */,
			1 /* fetch_folder_attribute_list */,
			0 /* not fetch_attribute */,
			0 /* not cache_boolean */ );

	purchase_fetch->title_passage_rule_boolean =
		purchase_fetch_title_passage_rule_boolean(
			purchase_folder->folder->folder_attribute_list );

	purchase_fetch->shipped_date_boolean =
		purchase_fetch_shipped_date_boolean(
			purchase_folder->folder->folder_attribute_list );

	purchase_fetch->arrived_date_time_boolean =
		purchase_fetch_arrived_date_time_boolean(
			purchase_folder->folder->folder_attribute_list );

	purchase_fetch->inventory_total_boolean =
		purchase_fetch_inventory_total_boolean(
			purchase_folder->folder->folder_attribute_list );

	purchase_fetch->specific_inventory_total_boolean =
		purchase_fetch_specific_inventory_total_boolean(
			purchase_folder->folder->folder_attribute_list );

	purchase_fetch->prepaid_asset_total_boolean =
		purchase_fetch_prepaid_asset_total_boolean(
			purchase_folder->folder->folder_attribute_list );

	purchase_fetch->return_total_boolean =
		purchase_fetch_return_total_boolean(
			purchase_folder->folder->folder_attribute_list );

	purchase_fetch->program_name_boolean =
		purchase_fetch_program_name_boolean(
			purchase_folder->folder->folder_attribute_list );

	purchase_fetch->property_address_boolean =
		purchase_fetch_property_address_boolean(
			purchase_folder->folder->folder_attribute_list );

	select =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		purchase_fetch_select(
			PURCHASE_SELECT,
			purchase_fetch->title_passage_rule_boolean,
			purchase_fetch->shipped_date_boolean,
			purchase_fetch->arrived_date_time_boolean,
			purchase_fetch->inventory_total_boolean,
			purchase_fetch->specific_inventory_total_boolean,
			purchase_fetch->prepaid_asset_total_boolean,
			purchase_fetch->return_total_boolean,
			purchase_fetch->program_name_boolean,
			purchase_fetch->property_address_boolean );

	purchase_fetch->predictive_fund_boolean =
		predictive_fund_boolean(
			PREDICTIVE_FUND_TABLE,
			PREDICTIVE_FUND_COLUMN );

	purchase_fetch->entity_contact_key_boolean =
		entity_contact_key_boolean(
			ENTITY_TABLE,
			ENTITY_CONTACT_KEY_COLUMN );

	purchase_fetch->purchase_primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		purchase_primary_where(
			PURCHASE_DATE_TIME_COLUMN,
			fund_name,
			full_name,
			contact_key,
			purchase_date_time,
			purchase_fetch->predictive_fund_boolean,
			purchase_fetch->entity_contact_key_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			purchase_table,
			select,
			purchase_fetch->purchase_primary_where );

	free( select );

	input = string_system_input( system_string );

	/* May be deleted */
	/* -------------- */
	if ( !input ) return NULL;

	free( system_string );

	purchase_fetch_parse(
		purchase_fetch /* in/out */,
		purchase_fetch->title_passage_rule_boolean,
		purchase_fetch->shipped_date_boolean,
		purchase_fetch->arrived_date_time_boolean(),
		purchase_fetch->inventory_total_boolean,
		purchase_fetch->specific_inventory_total_boolean,
		purchase_fetch->prepaid_asset_total_boolean,
		purchase_fetch->return_total_boolean,
		purchase_fetch->program_boolean,
		purchase_fetch->property_address_boolean,
		input );

	purchase_fetch->fixed_asset_purchase_list =
		fixed_asset_purchase_list(
			FIXED_ASSET_PURCHASE_SELECT,
			FIXED_ASSET_PURCHASE_TABLE,
			purchase_fetch->purchase_primary_where );

/*
	if ( purchase_fetch->inventory_total_boolean )
	{
		purchase_fetch->inventory_purchase_list(
			inventory_purchase_list(
				INVENTORY_PURCHASE_SELECT,
				INVENTORY_PURCHASE_TABLE,
				purchase_fetch->purchase_primary_where );
	}
*/

	if ( purchase_fetch->specific_inventory_total_boolean )
	{
		purchase_fetch->specific_inventory_purchase_list =
			specific_inventory_purchase_list(
				SPECIFIC_INVENTORY_PURCHASE_SELECT,
				SPECIFIC_INVENTORY_PURCHASE_TABLE,
				purchase_fetch->purchase_primary_where );
	}

	purchase_fetch->supply_purchase_list =
		supply_purchase_list(
			SUPPLY_PURCHASE_SELECT,
			SUPPLY_PURCHASE_TABLE,
			purchase_primary_where() );

/*
	if ( purchase_fetch->prepaid_asset_total_boolean )
	{
		purchase_fetch->prepaid_asset_purchase_list =
			prepaid_asset_purchase_list(
				PREPAID_ASSET_PURCHASE_SELECT,
				PREPAID_ASSET_PURCHASE_TABLE,
				purchase_fetch->purchase_primary_where );
	}
*/

/*
	if ( purchase_fetch->return_total_boolean )
	{
		purchase_fetch->return_list(
			purchase_return_list(
				INVENTORY_PURCHASE_RETURN_TABLE,
				SPECIFIC_INVENTORY_PURCHASE_RETURN_TABLE,
				COMPONENT_INVENTORY_PURCHASE_RETURN_TABLE,
				SUPPLY_PURCHASE_RETURN_TABLE,
				purchase_fetch->purchase_primary_where );
	}
*/

	purchase_fetch->primary_key_list =
		purchase_fetch_primary_key_list(
			PREDICTIVE_FUND_COLUMN,
			ENTITY_FULL_NAME_COLUMN,
			ENTITY_CONTACT_KEY_COLUMN,
			PURCHASE_DATE_TIME_COLUMN,
			purchase_fetch->predictive_fund_boolean,
			purchase_fetch->entity_contact_key_boolean );

	return purchase_fetch;
}

PURCHASE_FETCH *purchase_fetch_calloc( void )
{
	PURCHASE_FETCH *purchase_fetch;

	if ( ! ( purchase_fetch = calloc( 1, sizeof ( PURCHASE_FETCH ) ) ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return purchase_fetch;
}

LIST *purchase_fetch_primary_key_list(
		const char *predictive_fund_column,
		const char *entity_full_name_column,
		const char *entity_contact_key_column,
		const char *purchase_date_time_column,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	return
	sale_fetch_primary_key_list(
		predictive_fund_column,
		entity_full_name_column,
		entity_contact_key_column,
		purchase_date_time_column
			/* SALE_DATE_TIME_COLUMN */,
		fund_boolean,
		contact_key_boolean );
}

void purchase_fetch_parse(
		PURCHASE_FETCH *purchase_fetch /* in/out */,
		boolean title_passage_rule_boolean,
		boolean shipped_date_boolean,
		boolean arrived_date_time_boolean,
		boolean inventory_total_boolean,
		boolean specific_inventory_total_boolean,
		boolean prepaid_asset_total_boolean,
		boolean purchase_return_boolean,
		boolean program_boolean,
		boolean property_address_boolean,
		char *input )
{
	char buffer[ 1024 ];
	int optional_piece_offset;

	if ( !purchase_fetch )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"purchase_fetch() is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !input || !*input ) return;

	/* See purchase_fetch_select() */
	/* --------------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	if ( *buffer ) purchase_fetch->full_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) purchase_fetch->purchase_date_time = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) purchase_fetch->sales_tax = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) purchase_fetch->freight_in = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) purchase_fetch->fixed_asset_total = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer ) purchase_fetch->supply_total = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer ) purchase_fetch->invoice_amount = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	if ( *buffer ) purchase_fetch->transaction_date_time = strdup( buffer );

	optional_piece_offset = 8;

	if ( title_passage_rule_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer )
		{
			purchase_fetch->
				title_passage_rule_string =
					strdup( buffer );

			purchase_fetch->predictive_title_passage_rule =
				predictive_resolve_title_passage_rule(
					purchase_fetch->
						title_passage_rule_string );
		}

	}

	if ( shipped_date_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer )
			purchase_fetch->
				shipped_date =
					strdup( buffer );

	}

	if ( arrived_date_time_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer )
			purchase_fetch->
				arrived_date_time =
					strdup( buffer );

	}

	if ( inventory_total_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer )
			purchase_fetch->
				inventory_total =
					atof( buffer );
	}

	if ( specific_inventory_total_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer )
			purchase_fetch->
				specific_inventory_total =
					atof( buffer );
	}

	if ( prepaid_asset_total_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer )
			purchase_fetch->
				prepaid_asset_total =
					atof( buffer );
	}

	if ( purchase_return_total_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer )
			purchase_fetch->
				purchase_return_total =
					atof( buffer );
	}

	if ( program_name_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer )
			purchase_fetch->
				program_name =
					strdup( buffer );
	}

	if ( property_address_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer )
			purchase_fetch->
				property_street_address =
					strdup( buffer );
	}
}

boolean purchase_fetch_title_passage_rule_boolean( LIST *folder_attribute_list )
{
	return
	(boolean)(unsigned int)(long)folder_attribute_seek(
		(char *)0 /* folder_name */,
		"title_passage_rule",
		folder_attribute_list );
}

boolean purchase_fetch_shipped_date_boolean( LIST *folder_attribute_list )
{
	return
	(boolean)(unsigned int)(long)folder_attribute_seek(
		(char *)0 /* folder_name */,
		"shipped_date",
		folder_attribute_list );
}

boolean purchase_fetch_arrived_date_time_boolean( LIST *folder_attribute_list )
{
	return
	(boolean)(unsigned int)(long)folder_attribute_seek(
		(char *)0 /* folder_name */,
		"arrived_date_time",
		folder_attribute_list );
}

boolean purchase_fetch_inventory_total_boolean( LIST *folder_attribute_list )
{
	return
	(boolean)(unsigned int)(long)folder_attribute_seek(
		(char *)0 /* folder_name */,
		"inventory_total",
		folder_attribute_list );
}

boolean purchase_fetch_specific_inventory_total_boolean(
		LIST *folder_attribute_list )
{
	return
	(boolean)(unsigned int)(long)folder_attribute_seek(
		(char *)0 /* folder_name */,
		"specific_inventory_total",
		folder_attribute_list );
}

boolean purchase_fetch_prepaid_asset_total_boolean(
		LIST *folder_attribute_list )
{
	return
	(boolean)(unsigned int)(long)folder_attribute_seek(
		(char *)0 /* folder_name */,
		"prepaid_asset_total",
		folder_attribute_list );
}

boolean purchase_fetch_return_total_boolean( LIST *folder_attribute_list )
{
	return
	(boolean)(unsigned int)(long)folder_attribute_seek(
		(char *)0 /* folder_name */,
		"purchase_return_total",
		folder_attribute_list );
}

boolean purchase_fetch_program_name_boolean( LIST *folder_attribute_list )
{
	return
	(boolean)(unsigned int)(long)folder_attribute_seek(
		(char *)0 /* folder_name */,
		"program_name",
		folder_attribute_list );
}

boolean purchase_fetch_property_address_boolean( LIST *folder_attribute_list )
{
	return
	(boolean)(unsigned int)(long)folder_attribute_seek(
		(char *)0 /* folder_name */,
		"property_street_address",
		folder_attribute_list );
}

char *purchase_fetch_select(
		const char *purchase_select,
		boolean title_passage_rule_boolean,
		boolean shipped_date_boolean,
		boolean arrived_date_time_boolean,
		boolean inventory_total_boolean,
		boolean specific_inventory_total_boolean,
		boolean prepaid_asset_total_boolean,
		boolean return_total_boolean,
		boolean program_boolean,
		boolean property_address_boolean )
{
	OPTIONAL_COLUMN *optional_column;

	optional_column =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		optional_column_new(
			',' /* delimiter */,
			(char *)purchase_select /* base_string */,
			"title_passage_rule" /* component */,
			0 /* not escape_boolean */,
			title_passage_rule_boolean /* set_boolean */ );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			"shipped_date" /* component */,
			0 /* not escape_boolean */,
			shipped_date_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			"arrived_date_time" /* component */,
			0 /* not escape_boolean */,
			arrived_date_time_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			"inventory_total" /* component */,
			0 /* not escape_boolean */,
			inventory_total_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			"specific_inventory_total" /* component */,
			0 /* not escape_boolean */,
			specific_inventory_total_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			"prepaid_asset_total" /* component */,
			0 /* not escape_boolean */,
			prepaid_asset_total_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			"purchase_return_total" /* component */,
			0 /* not escape_boolean */,
			purchase_return_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			"program_name" /* component */,
			0 /* not escape_boolean */,
			program_name_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			"property_street_address" /* component */,
			0 /* not escape_boolean */,
			property_address_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	return optional_column->return_string;
}

