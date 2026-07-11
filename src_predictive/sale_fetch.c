/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/sale_fetch.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "date.h"
#include "sql.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "folder_attribute.h"
#include "optional_column.h"
#include "specific_inventory_sale.h"
#include "inventory_sale.h"
#include "fixed_service_sale.h"
#include "hourly_service_sale.h"
#include "customer_payment.h"
#include "self_tax.h"
#include "sale.h"
#include "sale_fetch.h"

char *sale_fetch_select(
		const char *sale_select,
		boolean cash_account_boolean,
		boolean shipping_charge_boolean,
		boolean instructions_boolean,
		boolean inventory_sale_boolean,
		boolean specific_inventory_sale_boolean,
		boolean fixed_service_sale_boolean,
		boolean hourly_service_sale_boolean,
		boolean sales_tax_boolean,
		boolean title_passage_rule_boolean,
		boolean shipped_date_time_boolean,
		boolean arrived_date_boolean,
		boolean uncollectible_date_time_boolean )
{
	OPTIONAL_COLUMN *optional_column;

	optional_column =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		optional_column_new(
			',' /* delimiter */,
			(char *)sale_select /* base_string */,
			"cash_account" /* component */,
			0 /* not escape_boolean */,
			cash_account_boolean /* set_boolean */ );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			"shipping_charge" /* component */,
			0 /* not escape_boolean */,
			shipping_charge_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			"instructions" /* component */,
			0 /* not escape_boolean */,
			instructions_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			"inventory_sale_total" /* component */,
			0 /* not escape_boolean */,
			inventory_sale_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			"specific_inventory_sale_total" /* component */,
			0 /* not escape_boolean */,
			specific_inventory_sale_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			"fixed_service_sale_total" /* component */,
			0 /* not escape_boolean */,
			fixed_service_sale_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			"hourly_service_sale_total" /* component */,
			0 /* not escape_boolean */,
			hourly_service_sale_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			"sales_tax" /* component */,
			0 /* not escape_boolean */,
			sales_tax_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			"title_passage_rule" /* component */,
			0 /* not escape_boolean */,
			title_passage_rule_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			"shipped_date_time" /* component */,
			0 /* not escape_boolean */,
			shipped_date_time_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			"arrived_date" /* component */,
			0 /* not escape_boolean */,
			arrived_date_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			"uncollectible_date_time" /* component */,
			0 /* not escape_boolean */,
			uncollectible_date_time_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	return optional_column->return_string;
}

void sale_fetch_parse(
		SALE_FETCH *sale_fetch /* in/out */,
		boolean cash_account_boolean,
		boolean shipping_charge_boolean,
		boolean instructions_boolean,
		boolean inventory_sale_boolean,
		boolean specific_inventory_sale_boolean,
		boolean fixed_service_sale_boolean,
		boolean hourly_service_sale_boolean,
		boolean sales_tax_boolean,
		boolean title_passage_rule_boolean,
		boolean shipped_date_time_boolean,
		boolean arrived_date_boolean,
		boolean uncollectible_date_time_boolean,
		char *input )
{
	char buffer[ 1024 ];
	int optional_piece_offset;

	if ( !sale_fetch )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"sale_fetch() is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !input || !*input )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"input is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	/* See sale_fetch_select() */
	/* ----------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	if ( *buffer ) sale_fetch->full_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) sale_fetch->sale_date_time = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) sale_fetch->gross_revenue = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) sale_fetch->invoice_amount = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) sale_fetch->payment_total = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer ) sale_fetch->amount_due = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer ) sale_fetch->completed_date_time = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	if ( *buffer ) sale_fetch->transaction_date_time = strdup( buffer );

	optional_piece_offset = 8;

	if ( cash_account_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer ) sale_fetch->cash_account = strdup( buffer );
	}

	if ( shipping_charge_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer ) sale_fetch->shipping_charge = atof( buffer );
	}

	if ( instructions_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer ) sale_fetch->instructions = strdup( buffer );
	}

	if ( inventory_sale_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer )
			sale_fetch->
				inventory_sale_total =
					atof( buffer );
	}

	if ( specific_inventory_sale_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer )
			sale_fetch->
				specific_inventory_sale_total =
					atof( buffer );
	}

	if ( fixed_service_sale_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer )
			sale_fetch->
				fixed_service_sale_total =
					atof( buffer );
	}

	if ( hourly_service_sale_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer )
			sale_fetch->
				hourly_service_sale_total =
					atof( buffer );
	}

	if ( sales_tax_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer )
			sale_fetch->
				sales_tax =
					atof( buffer );
	}

	if ( title_passage_rule_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer )
		{
			sale_fetch->
				title_passage_rule_string =
					strdup( buffer );

			sale_fetch->predictive_title_passage_rule =
				predictive_resolve_title_passage_rule(
					sale_fetch->
						title_passage_rule_string );
		}

	}

	if ( shipped_date_time_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer )
			sale_fetch->
				shipped_date_time =
					strdup( buffer );
	}

	if ( arrived_date_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer )
			sale_fetch->
				arrived_date =
					strdup( buffer );

	}

	if ( uncollectible_date_time_boolean )
	{
		piece(	buffer,
			SQL_DELIMITER,
			input,
			optional_piece_offset++ );

		if ( *buffer )
			sale_fetch->
				uncollectible_date_time =
					strdup( buffer );
	}

	sale_fetch->primary_key_list =
		sale_fetch_primary_key_list(
			PREDICTIVE_FUND_COLUMN,
			ENTITY_FULL_NAME_COLUMN,
			ENTITY_CONTACT_KEY_COLUMN,
			SALE_DATE_TIME_COLUMN,
			sale_fetch->predictive_fund_boolean,
			sale_fetch->entity_contact_key_boolean );
}

SALE_FETCH *sale_fetch_new(
		const char *sale_select,
		const char *sale_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time )
{
	char *select;
	char *where;
	char *system_string;
	char *input;
	SALE_FETCH *sale_fetch;

	if ( !full_name
	||   !sale_date_time )
	{
		char message[ 128 ];

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

	sale_fetch = sale_fetch_calloc();

	sale_fetch->fund_name = fund_name;
	sale_fetch->full_name = full_name;
	sale_fetch->contact_key = contact_key;
	sale_fetch->sale_date_time = sale_date_time;

	sale_fetch->folder_fetch =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			(char *)sale_table /* folder_name */,
			(LIST *)0 /* role_attribute_exclude_name_list */,
			1 /* fetch_folder_attribute_list */,
			0 /* not fetch_attribute */,
			0 /* not cache_boolean */ );

	sale_fetch->cash_account_boolean =
		sale_fetch_cash_account_boolean(
			sale_fetch->folder_fetch->folder_attribute_list );

	sale_fetch->shipping_charge_boolean =
		sale_fetch_shipping_charge_boolean(
			sale_fetch->folder_fetch->folder_attribute_list );

	sale_fetch->instructions_boolean =
		sale_fetch_instructions_boolean(
			sale_fetch->folder_fetch->folder_attribute_list );

	sale_fetch->inventory_sale_boolean =
		sale_fetch_inventory_sale_boolean(
			sale_fetch->folder_fetch->folder_attribute_list );

	sale_fetch->specific_inventory_sale_boolean =
		sale_fetch_specific_inventory_sale_boolean(
			sale_fetch->folder_fetch->folder_attribute_list );

	sale_fetch->fixed_service_sale_boolean =
		sale_fetch_fixed_service_sale_boolean(
			sale_fetch->folder_fetch->folder_attribute_list );

	sale_fetch->hourly_service_sale_boolean =
		sale_fetch_hourly_service_sale_boolean(
			sale_fetch->folder_fetch->folder_attribute_list );

	sale_fetch->sales_tax_boolean =
		sale_fetch_sales_tax_boolean(
			sale_fetch->folder_fetch->folder_attribute_list );

	sale_fetch->payment_list_boolean =
		sale_fetch_payment_list_boolean(
			CUSTOMER_PAYMENT_TABLE,
			CUSTOMER_PAYMENT_DATE_COLUMN,
			sale_fetch->folder_fetch->folder_attribute_list );

	sale_fetch->title_passage_rule_boolean =
		sale_fetch_title_passage_rule_boolean(
			sale_fetch->folder_fetch->folder_attribute_list );

	sale_fetch->shipped_date_time_boolean =
		sale_fetch_shipped_date_time_boolean(
			sale_fetch->folder_fetch->folder_attribute_list );

	sale_fetch->arrived_date_boolean =
		sale_fetch_arrived_date_boolean(
			sale_fetch->folder_fetch->folder_attribute_list );

	sale_fetch->uncollectible_date_time_boolean =
		sale_fetch_uncollectible_date_time_boolean(
			sale_fetch->folder_fetch->folder_attribute_list );

	select =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		sale_fetch_select(
			sale_select,
			sale_fetch->cash_account_boolean,
			sale_fetch->shipping_charge_boolean,
			sale_fetch->instructions_boolean,
			sale_fetch->inventory_sale_boolean,
			sale_fetch->specific_inventory_sale_boolean,
			sale_fetch->fixed_service_sale_boolean,
			sale_fetch->hourly_service_sale_boolean,
			sale_fetch->sales_tax_boolean,
			sale_fetch->title_passage_rule_boolean,
			sale_fetch->shipped_date_time_boolean,
			sale_fetch->arrived_date_boolean,
			sale_fetch->uncollectible_date_time_boolean );

	sale_fetch->predictive_fund_boolean =
		predictive_fund_boolean(
			PREDICTIVE_FUND_TABLE,
			PREDICTIVE_FUND_COLUMN );

	sale_fetch->entity_contact_key_boolean =
		entity_contact_key_boolean(
			ENTITY_TABLE,
			ENTITY_CONTACT_KEY_COLUMN );

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		sale_primary_where(
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			sale_fetch->predictive_fund_boolean,
			sale_fetch->entity_contact_key_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select,
			(char *)sale_table,
			where );

	free( select );

	if ( ! ( input =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			string_system_input(
				system_string ) ) )
	{
		return NULL;
	}

	free( system_string );

	sale_fetch_parse(
		sale_fetch /* in/out */,
		sale_fetch->cash_account_boolean,
		sale_fetch->shipping_charge_boolean,
		sale_fetch->instructions_boolean,
		sale_fetch->inventory_sale_boolean,
		sale_fetch->specific_inventory_sale_boolean,
		sale_fetch->fixed_service_sale_boolean,
		sale_fetch->hourly_service_sale_boolean,
		sale_fetch->sales_tax_boolean,
		sale_fetch->title_passage_rule_boolean,
		sale_fetch->shipped_date_time_boolean,
		sale_fetch->arrived_date_boolean,
		sale_fetch->uncollectible_date_time_boolean,
		input );

#ifdef NOT_DEFINED

	if ( sale_fetch->inventory_sale_boolean )
	{
		sale_fetch->inventory_sale_list =
			inventory_sale_list(
				INVENTORY_SALE_TABLE,
				INVENTORY_SALE_SELECT,
				full_name,
				contact_key,
				sale_date_time,
				contact_key_boolean );
	}

	if ( sale_fetch->specific_inventory_sale_boolean )
	{
		sale_fetch->specific_inventory_sale_list =
			specific_inventory_sale_list(
				SPECIFIC_INVENTORY_SALE_TABLE,
				full_name,
				street_address,
				sale_date_time );
	}

	if (	sale_fetch->inventory_sale_boolean
	||	sale_fetch->specific_inventory_sale_boolean )
	{
		if ( ! ( sale_fetch->customer =
			   customer_fetch(
				full_name,
				street_address,
				0 /* not fetch_entity_boolean */,
				0 /* not fetch_payable_balance_boolean */ ) ) )
		{
			char message[ 256 ];

			snprintf(
				message,
				sizeof ( message ),
				"customer_fetch(%s/%s) returned empty.",
				full_name,
				street_address );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !sale_fetch->customer->sales_tax_exempt_boolean )
		{
			if ( ! ( sale_fetch->entity_self =
				     entity_self_fetch(
					0 /* not fetch_entity_boolean */ ) ) )
			{
				char message[ 128 ];

				snprintf(
					message,
					sizeof ( message ),
					"entity_self_fetch() returned empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			sale_fetch->self_tax_state_sales_tax_rate =
				self_tax_state_sales_tax_rate(
					sale_fetch->
						entity_self->
						full_name,
					sale_fetch->
						entity_self->
							street_address );

			if ( !sale_fetch->self_tax_state_sales_tax_rate )
			{
				char message[ 128 ];

				snprintf(
					message,
					sizeof ( message ),
			"self_tax_state_sales_tax_rate() returned empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}
		}
	}
#endif

	if ( sale_fetch->fixed_service_sale_boolean )
	{
		sale_fetch->fixed_service_sale_list =
			fixed_service_sale_list(
				FIXED_SERVICE_SALE_SELECT,
				FIXED_SERVICE_SALE_TABLE,
				fund_name,
				full_name,
				contact_key,
				sale_date_time,
				sale_fetch->predictive_fund_boolean,
				sale_fetch->entity_contact_key_boolean,
				1 /* fixed_service_work_boolean */ );
	}

/*
	if ( sale_fetch->hourly_service_sale_boolean )
	{
		sale_fetch->hourly_service_sale_list =
			hourly_service_sale_list(
				HOURLY_SERVICE_SALE_SELECT,
				HOURLY_SERVICE_SALE_TABLE,
				full_name,
				street_address,
				sale_date_time );
	}
*/

	sale_fetch->primary_key_list =
		sale_fetch_primary_key_list(
			PREDICTIVE_FUND_COLUMN,
			ENTITY_FULL_NAME_COLUMN,
			ENTITY_CONTACT_KEY_COLUMN,
			SALE_DATE_TIME_COLUMN,
			sale_fetch->predictive_fund_boolean,
			sale_fetch->entity_contact_key_boolean );

	return sale_fetch;
}

boolean sale_fetch_payment_list_boolean(
		const char *customer_payment_table,
		const char *customer_payment_date_column,
		LIST *folder_attribute_list )
{
	if ( folder_attribute_seek(
		(char *)customer_payment_table,
		(char *)customer_payment_date_column,
		folder_attribute_list ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean sale_fetch_uncollectible_date_time_boolean(
		LIST *folder_attribute_list )
{
	if ( folder_attribute_seek(
		(char *)0 /* folder_name */,
		"uncollectible_date_time",
		folder_attribute_list ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean sale_fetch_shipped_date_time_boolean( LIST *folder_attribute_list )
{
	if ( folder_attribute_seek(
		(char *)0 /* folder_name */,
		"shipped_date_time",
		folder_attribute_list ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean sale_fetch_arrived_date_boolean( LIST *folder_attribute_list )
{
	if ( folder_attribute_seek(
		(char *)0 /* folder_name */,
		"arrived_date",
		folder_attribute_list ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean sale_fetch_sales_tax_boolean( LIST *folder_attribute_list )
{
	if ( folder_attribute_seek(
		(char *)0 /* folder_name */,
		"sales_tax",
		folder_attribute_list ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean sale_fetch_inventory_sale_boolean(
		LIST *folder_attribute_list )
{
	if ( folder_attribute_seek(
		(char *)0 /* folder_name */,
		"inventory_sale_total",
		folder_attribute_list ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean sale_fetch_specific_inventory_sale_boolean(
		LIST *folder_attribute_list )
{
	if ( folder_attribute_seek(
		(char *)0 /* folder_name */,
		"specific_inventory_sale_total",
		folder_attribute_list ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean sale_fetch_title_passage_rule_boolean(
		LIST *folder_attribute_list )
{
	if ( folder_attribute_seek(
		(char *)0 /* folder_name */,
		"title_passage_rule",
		folder_attribute_list ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean sale_fetch_shipping_charge_boolean(
		LIST *folder_attribute_list )
{
	if ( folder_attribute_seek(
		(char *)0 /* folder_name */,
		"shipping_charge",
		folder_attribute_list ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean sale_fetch_instructions_boolean(
		LIST *folder_attribute_list )
{
	if ( folder_attribute_seek(
		(char *)0 /* folder_name */,
		"instructions",
		folder_attribute_list ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean sale_fetch_hourly_service_sale_boolean(
		LIST *folder_attribute_list )
{
	if ( folder_attribute_seek(
		(char *)0 /* folder_name */,
		"hourly_service_sale_total",
		folder_attribute_list ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean sale_fetch_fixed_service_sale_boolean(
		LIST *folder_attribute_list )
{
	if ( folder_attribute_seek(
		(char *)0 /* folder_name */,
		"fixed_service_sale_total",
		folder_attribute_list ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean sale_fetch_cash_account_boolean(
		LIST *folder_attribute_list )
{
	if ( folder_attribute_seek(
		(char *)0 /* folder_name */,
		"cash_account",
		folder_attribute_list ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

SALE_FETCH *sale_fetch_calloc( void )
{
	SALE_FETCH *sale_fetch;

	if ( ! ( sale_fetch = calloc( 1, sizeof ( SALE_FETCH ) ) ) )
	{
		char message[ 128 ];

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

	return sale_fetch;
}

LIST *sale_fetch_primary_key_list(
		const char *predictive_fund_column,
		const char *entity_full_name_column,
		const char *entity_contact_key_column,
		const char *sale_date_time_column,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *fund_string;
	LIST *list = list_new();

	fund_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		predictive_fund_string(
			0 /* delimiter */,
			(char *)predictive_fund_column,
			fund_boolean );

	if ( *fund_string )
	{
		list_set( list, fund_string );
	}

	list_set_list(
		list,
		entity_primary_key_list(
			entity_full_name_column,
			entity_contact_key_column,
			contact_key_boolean ) );

	list_set( list, (void *)sale_date_time_column );

	return list;
}

