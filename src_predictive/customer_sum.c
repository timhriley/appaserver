/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/customer_sum.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <unistd.h>
#include "String.h"
#include "appaserver_error.h"
#include "sql.h"
#include "entity.h"
#include "sale.h"
#include "inventory_sale.h"
#include "specific_inventory_sale.h"
#include "customer_sum.h"

CUSTOMER_SUM *customer_sum_fetch(
		char *customer_full_name,
		char *customer_contact_key,
		boolean contact_key_boolean )
{
	CUSTOMER_SUM *customer_sum;

	if ( !customer_full_name )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"customer_full_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	customer_sum = customer_sum_calloc();

	customer_sum->entity_primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_primary_where(
			ENTITY_FULL_NAME_COLUMN,
			ENTITY_CONTACT_KEY_COLUMN,
			CUSTOMER_TABLE_NAME,
			customer_full_name,
			customer_contact_key,
			contact_key_boolean );

	customer_sum->first_sale_date =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		customer_sum_first_sale_date(
			SALE_TABLE,
			customer_sum->entity_primary_where );

	customer_sum->sale_count =
		customer_sum_sale_count(
			SALE_TABLE,
			customer_sale->entity_primary_where );

	customer_sum->gross_revenue =
		customer_sum_gross_revenue(
			SALE_TABLE,
			customer_sum->entity_primary_where );

	customer_sum->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			CUSTOMER_TABLE /* folder_name */,
			(LIST *)0 /* role_attribute_exclude_name_list */,
			1 /* fetch_folder_attribute_list */,
			0 /* not fetch_attribute */,
			0 /* not cache_boolean */ );

	customer_sum->return_count_boolean =
		customer_sum_return_count_boolean(
			customer_sum->folder->folder_attribute_list );

	if ( customer_sum->return_count_boolean )
	{
		customer_sum->inventory_sale_return_boolean =
			inventory_sale_return_boolean(
				INVENTORY_SALE_RETURN_TABLE );

		customer_sum->specific_inventory_sale_return_boolean =
			specific_inventory_sale_return_boolean(
				SPECIFIC_INVENTORY_SALE_RETURN_TABLE );

		int customer_sum_return_count(
			INVENTORY_SALE_RETURN_TABLE,
			SPECIFIC_INVENTORY_SALE_RETURN_TABLE,
			entity_primary_where(),
			inventory_sale_return_boolean(),
			specific_inventory_sale_return_boolean() );

		double customer_sum_return_total(
			INVENTORY_SALE_TABLE,
			INVENTORY_SALE_RETURN_TABLE,
			SPECIFIC_INVENTORY_SALE_RETURN_TABLE,
			customer_full_name,
			customer_contact_key,
			entity_primary_where(),
			entity_contact_key_boolean,
			inventory_sale_return_boolean(),
			specific_inventory_sale_return_boolean() );

		double CUSTOMER_SUM_NET_REVENUE(
			double customer_sum_gross_revenue(),
			double customer_sum_return_total() );
	}

char *customer_sum_primary_data_string(
	SQL_DELIMITER,
	full_name,
	contact_key,
	entity_contact_key_boolean );

boolean customer_sum_receivable_expected_boolean(
	LIST *folder_fetch()->folder_attribute_list );

boolean customer_sum_payable_due_boolean(
	LIST *folder_fetch()->folder_attribute_list );

CUSTOMER_ACCRUAL *customer_accrual_fetch(
	customer_full_name,
	customer_contact_key,
	entity_contact_key_boolean,
	customer_sum_primary_data_string(),
	customer_sum_receivable_expected_boolean(),
	customer_sum_payable_due_boolean() );

LIST *customer_sum_update_string_list(
	customer_sum_primary_data_string(),
	customer_sum_first_sale_date(),
	customer_sum_sale_count(),
	customer_sum_gross_revenue(),
	customer_sum_return_count_boolean(),
	customer_sum_return_count(),
	customer_sum_return_total(),
	customer_sum_net_revenue(),
	customer_accrual_fetch()->update_string_list );

	return customer_sum;
}

CUSTOMER_SUM *customer_sum_calloc( void )
{
	CUSTOMER_SUM *customer_sum;

	if ( ! ( customer_sum = calloc( 1, sizeof ( CUSTOMER_SUM ) ) ) )
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

	return customer_sum;
}

char *customer_sum_first_sale_date(
		const char *sale_table,
		char *entity_primary_where )
{
	char *system_string;
	char *input;
	char first_sale_date[ 16 ];

	*first_sale_date = '\0';

	if ( !entity_primary_where )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"entity_primary_where is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			"min( sale_date_time )",
			sale_table,
			entity_primary_where );

	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	input = string_system_input( system_string );

	if ( input )
	{
		column( first_sale_date, input, 0 );
	}

	if ( *first_sale_date )
		return strdup( first_sale_date );
	else
		return NULL;
}

int customer_sum_sale_count(
		const char *sale_table,
		char *entity_primary_where )
{
	char *system_string;

	if ( !entity_primary_where )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"entity_primary_where is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			"count( 1 )",
			sale_table,
			entity_primary_where );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	string_atoi(
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_system_input( system_string ) );
}

double customer_sum_gross_revenue(
		const char *sale_table,
		char *entity_primary_where )
{
	char *system_string;

	if ( !entity_primary_where )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"entity_primary_where is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			"sum( gross_revenue )",
			sale_table,
			entity_primary_where );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	string_atof(
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_system_input( system_string ) );
}

int customer_sum_return_count(
		const char *inventory_sale_return_table,
		const char *specific_inventory_sale_return_table,
		char *entity_primary_where,
		boolean inventory_sale_return_boolean,
		boolean specific_inventory_sale_return_boolean )
{
	char *system_string;
	char *input;
	int return_count = 0;

	if ( !entity_primary_where )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"entity_primary_where is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( inventory_sale_return_boolean )
	{
		system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_system_string(
				"count( 1 )",
				inventory_sale_return_table,
				entity_primary_where );

		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		input = string_system_input( system_string );

		/* Safely returns */
		/* -------------- */
		return_count = string_atoi( input );
	}

	if ( specific_inventory_sale_return_boolean )
	{
		system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_system_string(
				"count( 1 )",
				specific_inventory_sale_return_table,
				entity_primary_where );

		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		input = string_system_input( system_string );

		/* Safely returns */
		/* -------------- */
		return_count += string_atoi( input );
	}

	return return_count;
}

double customer_sum_return_total(
		const char *inventory_sale_table,
		const char *inventory_sale_return_table,
		const char *specific_inventory_sale_return_table,
		char *entity_primary_where,
		boolean entity_contact_key_boolean,
		boolean inventory_sale_return_boolean,
		boolean specific_inventory_sale_return_boolean )
{
	char from[ 128 ];
	char *join;
	char where[ 512 ];
	char *system_string;
	char *input;
	double return_total = 0.0;

	if ( !entity_primary_where )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"entity_primary_where is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( inventory_sale_return_boolean )
	{
		snprintf(
			from,
			sizeof ( from ),
			"%s,%s",
			inventory_sale_table,
			inventory_sale_return_table );

		join =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			inventory_sale_join(
				inventory_sale_table,
				inventory_sale_return_table
					/* foreign_table */,
				ENTITY_FULL_NAME_COLUMN,
				ENTITY_CONTACT_KEY_COLUMN,
				SALE_DATE_TIME_COLUMN,
				entity_contact_key_boolean );

		snprintf(
			where[],
			sizeof ( where ),
			"%s and %s",
			entity_primary_where,
			join );

		system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_system_string(
				"sum( extended_price )",
				from,
				where );

		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		input = string_system_input( system_string );

		free( system_string );
		return_total = atof( input );
	}

	if ( specific_inventory_sale_return_boolean )
	{
		snprintf(
			from,
			sizeof ( from ),
			"%s,%s",
			specific_inventory_sale_table,
			specific_inventory_sale_return_table );

		join =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			inventory_sale_join(
				specific_inventory_sale_table,
				specific_inventory_sale_return_table
					/* foreign_table */,
				ENTITY_FULL_NAME_COLUMN,
				ENTITY_CONTACT_KEY_COLUMN,
				SALE_DATE_TIME_COLUMN,
				entity_contact_key_boolean );

		snprintf(
			where,
			sizeof ( where ),
			"%s and %s",
			entity_primary_where,
			join );

		system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_system_string(
				"sum( extended_price )",
				from,
				where );

		input = string_system_input( system_string );

		free( system_string );
		return_total = += atof( input );
	}

	return return_total;
}

LIST *customer_sum_update_string_list(
		char *customer_sum_primary_data_string,
		char *first_sale_date,
		int sale_count;
		double gross_revenue;
		boolean return_count_boolean;
		int return_count;
		double return_total;
		double net_revenue;
		LIST *customer_accrual_update_string_list )
{
	LIST *list = list_new();
	char *string;

	string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_text_string(
			SQL_DELIMITER,
			customer_sum_primary_data_string,
			"first_sale_date" /* column_name */,
			first_sale_date /* text */,
			1 /* set_boolean */ );

	list_set( list, string );

	string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_integer_string(
			sql_delimiter,
			customer_sum_primary_data_string,
			"sale_count" /* column_name */,
			sale_count /* integer */,
			1 /* set_boolean */ );

	list_set( list, string );

	string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_string(
			SQL_DELIMITER,
			customer_sum_primary_data_string,
			"gross_revenue" /* column_name */,
			gross_revenue /* money */,
			1 /* set_boolean */ );

	list_set( list, string );

	string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_integer_string(
			SQL_DELIMITER,
			customer_sum_primary_data_string,
			"return_count" /* column_name */,
			return_count /* integer */,
			return_count_boolean /* set_boolean */ );

	list_set( list, string );

	string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_string(
			SQL_DELIMITER,
			customer_sum_primary_data_string,
			"return_total" /* column_name */,
			return_total /* money */,
			return_count_boolean /* set_boolean */ );

	list_set( list, string );

	string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_string(
			SQL_DELIMITER,
			customer_sum_primary_data_string,
			"net_revenue" /* column_name */,
			net_revenue /* money */,
			return_count_boolean /* set_boolean */ );

	list_set( list, string );

	list_set_list( list, customer_accrual_update_string_list );

	return list;
}

char *customer_sum_primary_data_string(
		const char sql_delimiter,
		char *full_name,
		char *contact_key,
		boolean entity_contact_key_boolean )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	entity_primary_data_string(
		sql_delimiter,
		entity_contact_key_boolean,
		full_name,
		contact_key );
}

