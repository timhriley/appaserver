/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_service_sale.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "date.h"
#include "entity.h"
#include "security.h"
#include "sql.h"
#include "sale.h"
#include "predictive.h"
#include "fixed_service_work.h"
#include "fixed_service_sale.h"

FIXED_SERVICE_SALE *fixed_service_sale_new(
		char *service_name )
{
	FIXED_SERVICE_SALE *fixed_service_sale;

	if ( !service_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"service_naem is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	fixed_service_sale = fixed_service_sale_calloc();
	fixed_service_sale->service_name = service_name;

	return fixed_service_sale;
}

FIXED_SERVICE_SALE *fixed_service_sale_calloc( void )
{
	FIXED_SERVICE_SALE *fixed_service_sale;

	if ( ! ( fixed_service_sale =
			calloc( 1,
				sizeof ( FIXED_SERVICE_SALE ) ) ) )
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

	return fixed_service_sale;
}

FIXED_SERVICE_SALE *fixed_service_sale_parse(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean,
		boolean fixed_service_work_boolean,
		char *input )
{
	FIXED_SERVICE_SALE *fixed_service_sale;
	char service_name[ 128 ];
	char buffer[ 128 ];

	if ( !input || !*input ) return NULL;

	/* See FIXED_SERVICE_SALE_SELECT */
	/* ----------------------------- */
	piece( service_name, SQL_DELIMITER, input, 0 );

	/* -------------- */
	/* Safely returns */
	/* -------------- */
	fixed_service_sale = fixed_service_sale_new( strdup( service_name ) );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) fixed_service_sale->fixed_price = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) fixed_service_sale->estimated_hours = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) fixed_service_sale->discount_amount = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) fixed_service_sale->work_hours = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer ) fixed_service_sale->net_revenue = atof( buffer );

	if ( fixed_service_work_boolean )
	{
		fixed_service_sale->fixed_service_work_list =
			fixed_service_work_list(
				FIXED_SERVICE_WORK_SELECT,
				FIXED_SERVICE_WORK_TABLE,
				fund_name,
				full_name,
				contact_key,
				sale_date_time,
				fixed_service_sale->service_name,
				fund_boolean,
				contact_key_boolean );

		fixed_service_sale->fixed_service_work_hours =
			fixed_service_work_hours(
				fixed_service_sale->fixed_service_work_list );

		fixed_service_sale->fixed_service_sale_net_revenue =
			fixed_service_sale_net_revenue(
				fixed_service_sale->fixed_price,
				fixed_service_sale->discount_amount );

		fixed_service_sale->update_string_list =
			fixed_service_sale_update_string_list(
				SQL_DELIMITER,
				fund_name,
				full_name,
				contact_key,
				sale_date_time,
				service_name,
				fund_boolean,
				contact_key_boolean,
				fixed_service_sale->
					fixed_service_work_hours,
				fixed_service_sale->
					fixed_service_sale_net_revenue );

		fixed_service_sale->primary_key_list =
			fixed_service_sale_primary_key_list(
					SALE_SERVICE_NAME_COLUMN,
					fund_boolean,
					contact_key_boolean );
	}

	return fixed_service_sale;
}

LIST *fixed_service_sale_list(
		const char *fixed_service_sale_select,
		const char *fixed_service_sale_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean,
		boolean fixed_service_work_boolean )
{
	char *where;
	LIST *list = list_new();
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	FIXED_SERVICE_SALE *fixed_service_sale;

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

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		sale_primary_where(
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			fund_boolean,
			contact_key_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)fixed_service_sale_select,
			(char *)fixed_service_sale_table,
			where );

	/* -------------- */
	/* Safely returns */
	/* -------------- */
	input_pipe = appaserver_input_pipe( system_string );

	free( system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		fixed_service_sale =
			fixed_service_sale_parse(
				fund_name,
				full_name,
				contact_key,
				sale_date_time,
				fund_boolean,
				contact_key_boolean,
				fixed_service_work_boolean,
				input );

		if ( !fixed_service_sale )
		{
			char message[ 2048 ];

			snprintf(
				message,
				sizeof ( message ),
			"fixed_service_sale_parse(%s) returned empty.",
				input );

			pclose( input_pipe );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set( list, fixed_service_sale );
	}

	pclose( input_pipe );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

double fixed_service_sale_total( LIST *fixed_service_sale_list )
{
	FIXED_SERVICE_SALE *fixed_service_sale;
	double total = 0.0;

	if ( list_rewind( fixed_service_sale_list ) )
	do {
		fixed_service_sale = list_get( fixed_service_sale_list );

		total +=
			fixed_service_sale->
				net_revenue;

	} while( list_next( fixed_service_sale_list ) );

	return total;
}

double fixed_service_sale_net_revenue(
		double fixed_rate,
		double discount_amount )
{
	return
	fixed_rate - discount_amount;
}

FIXED_SERVICE_SALE *fixed_service_sale_fetch(
		const char *fixed_service_sale_select,
		const char *fixed_service_sale_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		boolean fund_boolean,
		boolean contact_key_boolean,
		boolean fixed_service_work_boolean )
{
	char *where;
	char *system_string;
	char *input;

	if ( !full_name
	||   !sale_date_time
	||   !service_name )
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

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		fixed_service_sale_primary_where(
			SALE_SERVICE_NAME_COLUMN,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			service_name,
			fund_boolean,
			contact_key_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)fixed_service_sale_select,
			(char *)fixed_service_sale_table,
			where );

	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	input = string_system_input( system_string );

	if ( !input ) return NULL;

	return
	fixed_service_sale_parse(
		fund_name,
		full_name,
		contact_key,
		sale_date_time,
		fund_boolean,
		contact_key_boolean,
		fixed_service_work_boolean,
		input );
}

char *fixed_service_sale_primary_where(
		const char *sale_service_name_column,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	static char where[ 288 ];
	char *primary_where;
	char *injection_escape;

	if ( !full_name
	||   !sale_date_time
	||   !service_name )
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

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		sale_primary_where(
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			fund_boolean,
			contact_key_boolean );

	snprintf(
		where,
		sizeof ( where ),
		"%s and %s = '%s'",
		primary_where,
		sale_service_name_column,
		( injection_escape =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				service_name ) ) );

	free( injection_escape );

	return where;
}

void fixed_service_sale_trigger(
		char *application_name,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *state )
{
	FIXED_SERVICE_SALE *fixed_service_sale;
	SALE *sale;

	if ( !full_name
	||   !sale_date_time
	||   !service_name
	||   !state )
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

	if ( strcmp( state, APPASERVER_PREDELETE_STATE ) == 0 ) return;

	if ( strcmp(
		state,
		APPASERVER_INSERT_STATE ) == 0
	||   strcmp(
		state,
		APPASERVER_UPDATE_STATE ) == 0 )
	{
		boolean fund_boolean;
		boolean contact_key_boolean;

		fund_boolean =
			predictive_fund_boolean(
				PREDICTIVE_FUND_TABLE,
				PREDICTIVE_FUND_COLUMN );

		contact_key_boolean =
			entity_contact_key_boolean(
				ENTITY_TABLE,
				ENTITY_CONTACT_KEY_COLUMN );

		fixed_service_sale =
			fixed_service_sale_fetch(
				FIXED_SERVICE_SALE_SELECT,
				FIXED_SERVICE_SALE_TABLE,
				fund_name,
				full_name,
				contact_key,
				sale_date_time,
				service_name,
				fund_boolean,
				contact_key_boolean,
				1 /* fixed_service_work_boolean */ );
	
		if ( !fixed_service_sale )
		{
			char message[ 1024 ];

			snprintf(
				message,
				sizeof ( message ),
			"fixed_service_sale_fetch(%s,%s) returned empty.",
				full_name,
				sale_date_time );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		fixed_service_sale_update(
			FIXED_SERVICE_SALE_TABLE,
			fixed_service_sale->update_string_list,
			fixed_service_sale->primary_key_list );
	}
	
	sale =
		sale_trigger_new(
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			state,
			(char *)0 /* preupdate_fund_name */,
			(char *)0 /* preupdate_full_name */,
			(char *)0 /* preupdate_contact_key */,
			(char *)0 /* preupdate_uncollectible_date_time */ );

	if ( !sale ) return;

	(void)sale_update(
		SALE_TABLE,
		application_name /* for transaction_update */,
		sale->update_string_list,
		sale->sale_fetch->sale_primary_key_list,
		sale->sale_transaction,
		(SALE_LOSS_TRANSACTION *)0 );
}

char *fixed_service_sale_primary_data_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *primary_data_string;
	char *injection_escape;
	char service_sale_primary_data_string[ 1024 ];

	if ( !full_name
	||   !sale_date_time
	||   !service_name )
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

	primary_data_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		sale_primary_data_string(
			sql_delimiter,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			fund_boolean,
			contact_key_boolean );

	injection_escape =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			service_name /* datum */ );

	snprintf(
		service_sale_primary_data_string,
		sizeof ( service_sale_primary_data_string ),
		"%s%c%s",
		primary_data_string,
		sql_delimiter,
		injection_escape );

	free( primary_data_string );
	free( injection_escape );

	return strdup( service_sale_primary_data_string );
}

LIST *fixed_service_sale_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		boolean fund_boolean,
		boolean contact_key_boolean,
		double fixed_service_work_hours,
		double fixed_service_sale_net_revenue )
{
	char *primary_data_string;
	char *update_string;
	LIST *list = list_new();

	if ( !full_name
	||   !sale_date_time
	||   !service_name )
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

	primary_data_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		fixed_service_sale_primary_data_string(
			sql_delimiter,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			service_name,
			fund_boolean,
			contact_key_boolean );

	update_string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_string(
			sql_delimiter,
			primary_data_string,
			"work_hours" /* column_name */,
			fixed_service_work_hours /* money */,
			1 /* set_boolean */ );

	list_set( list, update_string );

	update_string =
		sale_update_string(
			sql_delimiter,
			primary_data_string,
			"net_revenue" /* column_name */,
			fixed_service_sale_net_revenue /* money */,
			1 /* set_boolean */ );

	list_set( list, update_string );
	free( primary_data_string );

	return list;
}

void fixed_service_sale_update(
		const char *fixed_service_sale_table,
		LIST *update_string_list,
		LIST *primary_key_list )
{
	/* Borrow sale_update() */
	/* -------------------- */
	(void)sale_update(
		fixed_service_sale_table,
		(char *)0 /* application_name for transaction_update */,
		update_string_list,
		primary_key_list,
		(SALE_TRANSACTION *)0,
		(SALE_LOSS_TRANSACTION*)0 );
}

LIST *fixed_service_sale_primary_key_list(
		const char *sale_service_name_column,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	LIST *primary_key_list;

	primary_key_list =
		sale_primary_key_list(
			PREDICTIVE_FUND_COLUMN,
			ENTITY_FULL_NAME_COLUMN,
			ENTITY_CONTACT_KEY_COLUMN,
			SALE_DATE_TIME_COLUMN,
			fund_boolean,
			contact_key_boolean );

	list_set( primary_key_list, (void *)sale_service_name_column );

	return primary_key_list;
}
