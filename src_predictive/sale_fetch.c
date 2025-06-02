/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/sale_fetch.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "piece.h"
#include "date.h"
#include "sql.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "folder_attribute.h"
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
		boolean inventory_sale_boolean,
		boolean specific_inventory_sale_boolean,
		boolean title_passage_rule_boolean,
		boolean shipping_charge_boolean,
		boolean instructions_boolean,
		boolean hourly_service_sale_boolean,
		boolean fixed_service_sale_boolean )
{
	char select[ 1024 ];
	char *ptr = select;

	ptr += sprintf(
		ptr,
		"%s",
		sale_select );

	if ( inventory_sale_boolean )
	{
		ptr += sprintf(
			ptr,
			",inventory_sale_total" );
	}

	if ( specific_inventory_sale_boolean )
	{
		ptr += sprintf(
			ptr,
			",specific_inventory_sale_total" );
	}

	if ( inventory_sale_boolean
	||   specific_inventory_sale_boolean )
	{
		ptr += sprintf(
			ptr,
			",sales_tax" );
	}

	if ( title_passage_rule_boolean )
	{
		ptr += sprintf(
			ptr,
			",title_passage_rule"
			",shipped_date_time"
			",arived_date" );
	}

	if ( shipping_charge_boolean )
	{
		ptr += sprintf(
			ptr,
			",shipping_charge" );
	}

	if ( instructions_boolean )
	{
		ptr += sprintf(
			ptr,
			",instructions" );
	}

	if ( hourly_service_sale_boolean )
	{
		ptr += sprintf(
			ptr,
			",hourly_service_sale_total" );
	}

	if ( fixed_service_sale_boolean )
	{
		ptr += sprintf(
			ptr,
			",fixed_service_sale_total" );
	}

	return strdup( select );
}

SALE_FETCH *sale_fetch_parse(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		SALE_FETCH *sale_fetch /* in/out */,
		boolean inventory_sale_boolean,
		boolean specific_inventory_sale_boolean,
		boolean title_passage_rule_boolean,
		boolean shipping_charge_boolean,
		boolean instructions_boolean,
		boolean hourly_service_sale_boolean,
		boolean fixed_service_sale_boolean,
		char *fetch )
{
	char piece_buffer[ STRING_64K ];
	int optional_piece_offset;

	if ( !full_name
	||   !street_address
	||   !sale_date_time
	||   !sale_fetch
	||   !fetch )
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

	/* See sale_fetch_select() */
	/* ----------------------- */
	piece( piece_buffer, SQL_DELIMITER, fetch, 0 );
	if ( *piece_buffer )
		sale_fetch->
			gross_revenue =
				atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, fetch, 1 );
	if ( *piece_buffer )
		sale_fetch->
			invoice_amount =
				atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, fetch, 2 );
	if ( *piece_buffer )
		sale_fetch->
			payment_total =
				atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, fetch, 3 );
	if ( *piece_buffer )
		sale_fetch->
			amount_due =
				atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, fetch, 4 );
	if ( *piece_buffer )
		sale_fetch->
			completed_date_time =
				strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, fetch, 5 );
	if ( *piece_buffer )
		sale_fetch->
			uncollectible_date_time =
				strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, fetch, 6 );
	if ( *piece_buffer )
		sale_fetch->
			transaction_date_time =
				strdup( piece_buffer );

	optional_piece_offset = 7;

	if ( inventory_sale_boolean )
	{
		piece(	piece_buffer,
			SQL_DELIMITER,
			fetch,
			optional_piece_offset++ );

		if ( *piece_buffer )
			sale_fetch->
				inventory_sale_total =
					atof( piece_buffer );
	}

	if ( specific_inventory_sale_boolean )
	{
		piece(	piece_buffer,
			SQL_DELIMITER,
			fetch,
			optional_piece_offset++ );

		if ( *piece_buffer )
			sale_fetch->
				specific_inventory_sale_total =
					atof( piece_buffer );
	}

	if ( inventory_sale_boolean
	||   specific_inventory_sale_boolean )
	{
		piece(	piece_buffer,
			SQL_DELIMITER,
			fetch,
			optional_piece_offset++ );

		if ( *piece_buffer )
			sale_fetch->
				sales_tax =
					atof( piece_buffer );
	}

	if ( title_passage_rule_boolean )
	{
		piece(	piece_buffer,
			SQL_DELIMITER,
			fetch,
			optional_piece_offset++ );

		if ( *piece_buffer )
			sale_fetch->
				title_passage_rule_string =
					strdup( piece_buffer );

		piece(	piece_buffer,
			SQL_DELIMITER,
			fetch,
			optional_piece_offset++ );

		if ( *piece_buffer )
			sale_fetch->
				shipped_date_time =
					strdup( piece_buffer );

		piece(	piece_buffer,
			SQL_DELIMITER,
			fetch,
			optional_piece_offset++ );

		if ( *piece_buffer )
			sale_fetch->
				arrived_date =
					strdup( piece_buffer );

	}

	if ( shipping_charge_boolean )
	{
		piece(	piece_buffer,
			SQL_DELIMITER,
			fetch,
			optional_piece_offset++ );

		if ( *piece_buffer )
			sale_fetch->
				shipping_charge =
					atof( piece_buffer );
	}

	if ( instructions_boolean )
	{
		piece(	piece_buffer,
			SQL_DELIMITER,
			fetch,
			optional_piece_offset++ );

		if ( *piece_buffer )
			sale_fetch->
				instructions =
					strdup( piece_buffer );
	}

	if ( fixed_service_sale_boolean )
	{
		piece(	piece_buffer,
			SQL_DELIMITER,
			fetch,
			optional_piece_offset++ );

		if ( *piece_buffer )
			sale_fetch->
				fixed_service_sale_total =
					atof( piece_buffer );
	}

	if ( hourly_service_sale_boolean )
	{
		piece(	piece_buffer,
			SQL_DELIMITER,
			fetch,
			optional_piece_offset++ );

		if ( *piece_buffer )
			sale_fetch->
				hourly_service_sale_total =
					atof( piece_buffer );
	}

	if ( title_passage_rule_boolean )
	{
		sale_fetch->predictive_title_passage_rule =
			predictive_resolve_title_passage_rule(
				sale_fetch->
					title_passage_rule_string );
	}

	return sale_fetch;
}

SALE_FETCH *sale_fetch_new(
		const char *sale_select,
		const char *sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time )
{
	char *select;
	char *where;
	char *system_string;
	char *fetch;
	SALE_FETCH *sale_fetch;

	if ( !full_name
	||   !street_address
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

	sale_fetch->folder_fetch =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			SALE_TABLE /* folder_name */,
			(char *)0 /* role_name */,
			(LIST *)0 /* role_attribute_exclude_name_list */,
			1 /* fetch_folder_attribute_list */,
			0 /* not fetch_attribute */ );

	sale_fetch->inventory_sale_boolean =
		sale_fetch_inventory_sale_boolean(
			sale_fetch->folder_fetch->folder_attribute_list );

	sale_fetch->specific_inventory_sale_boolean =
		sale_fetch_specific_inventory_sale_boolean(
			sale_fetch->folder_fetch->folder_attribute_list );

	sale_fetch->title_passage_rule_boolean =
		sale_fetch_title_passage_rule_boolean(
			sale_fetch->folder_fetch->folder_attribute_list );

	sale_fetch->shipping_charge_boolean =
		sale_fetch_shipping_charge_boolean(
			sale_fetch->folder_fetch->folder_attribute_list );

	sale_fetch->instructions_boolean =
		sale_fetch_instructions_boolean(
			sale_fetch->folder_fetch->folder_attribute_list );

	sale_fetch->hourly_service_sale_boolean =
		sale_fetch_hourly_service_sale_boolean(
			sale_fetch->folder_fetch->folder_attribute_list );

	sale_fetch->fixed_service_sale_boolean =
		sale_fetch_fixed_service_sale_boolean(
			sale_fetch->folder_fetch->folder_attribute_list );

	select =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		sale_fetch_select(
			sale_select,
			sale_fetch->inventory_sale_boolean,
			sale_fetch->specific_inventory_sale_boolean,
			sale_fetch->title_passage_rule_boolean,
			sale_fetch->shipping_charge_boolean,
			sale_fetch->instructions_boolean,
			sale_fetch->hourly_service_sale_boolean,
			sale_fetch->fixed_service_sale_boolean );

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		sale_primary_where(
			full_name,
			street_address,
			sale_date_time );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select,
			(char *)sale_table,
			where );

	free( select );

	if ( ! ( fetch =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			string_pipe_fetch(
				system_string ) ) )
	{
		return NULL;
	}

	free( system_string );

	sale_fetch =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		sale_fetch_parse(
			full_name,
			street_address,
			sale_date_time,
			sale_fetch,
			sale_fetch->inventory_sale_boolean,
			sale_fetch->specific_inventory_sale_boolean,
			sale_fetch->title_passage_rule_boolean,
			sale_fetch->shipping_charge_boolean,
			sale_fetch->instructions_boolean,
			sale_fetch->hourly_service_sale_boolean,
			sale_fetch->fixed_service_sale_boolean,
			fetch );

	if ( sale_fetch->inventory_sale_boolean )
	{
		sale_fetch->inventory_sale_list =
			inventory_sale_list(
				INVENTORY_SALE_TABLE,
				full_name,
				street_address,
				sale_date_time );
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

	if ( sale_fetch->fixed_service_sale_boolean )
	{
		sale_fetch->fixed_service_sale_list =
			fixed_service_sale_list(
				FIXED_SERVICE_SALE_TABLE,
				full_name,
				street_address,
				sale_date_time );
	}

	if ( sale_fetch->hourly_service_sale_boolean )
	{
		sale_fetch->hourly_service_sale_list =
			hourly_service_sale_list(
				HOURLY_SERVICE_SALE_TABLE,
				full_name,
				street_address,
				sale_date_time );
	}

	return sale_fetch;
}

boolean sale_fetch_inventory_sale_boolean(
		LIST *folder_attribute_list )
{
	if ( folder_attribute_seek(
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

