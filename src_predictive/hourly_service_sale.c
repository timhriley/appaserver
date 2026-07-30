/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/hourly_service_sale.c		*/
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
#include "sql.h"
#include "security.h"
#include "sale.h"
#include "hourly_service_work.h"
#include "hourly_service_sale.h"

HOURLY_SERVICE_SALE *hourly_service_sale_new(
		char *service_name,
		char *service_description )
{
	HOURLY_SERVICE_SALE *hourly_service_sale;

	if ( !service_name
	||   !service_description )
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

	hourly_service_sale = hourly_service_sale_calloc();

	hourly_service_sale->service_name = service_name;
	hourly_service_sale->service_description = service_description;

	return hourly_service_sale;
}

HOURLY_SERVICE_SALE *hourly_service_sale_calloc( void )
{
	HOURLY_SERVICE_SALE *hourly_service_sale;

	if ( ! ( hourly_service_sale =
			calloc( 1,
				sizeof ( HOURLY_SERVICE_SALE ) ) ) )
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

	return hourly_service_sale;
}

HOURLY_SERVICE_SALE *hourly_service_sale_parse(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean,
		boolean hourly_service_work_boolean,
		char *input )
{
	HOURLY_SERVICE_SALE *hourly_service_sale;
	char service_name[ 128 ];
	char service_description[ 128 ];
	char buffer[ 128 ];

	if ( !full_name
	||   !sale_date_time
	||   !input
	||   !*input )
	{
		return NULL;
	}

	/* See HOURLY_SERVICE_SALE_SELECT */
	/* ------------------------------ */
	piece( service_name, SQL_DELIMITER, input, 0 );
	piece( service_description, SQL_DELIMITER, input, 1 );

	hourly_service_sale =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		hourly_service_sale_new(
			strdup( service_name ),
			strdup( service_description ) );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) hourly_service_sale->estimated_hours = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) hourly_service_sale->hourly_rate = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) hourly_service_sale->estimated_revenue = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer ) hourly_service_sale->discount_amount = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer ) hourly_service_sale->work_hours = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	if ( *buffer ) hourly_service_sale->net_revenue = atof( buffer );

	if ( hourly_service_work_boolean )
	{
		hourly_service_sale->hourly_service_sale_estimated_revenue =
			HOURLY_SERVICE_SALE_ESTIMATED_REVENUE(
				hourly_service_sale->estimated_hours,
				hourly_service_sale->hourly_rate,
				hourly_service_sale->discount_amount );

		hourly_service_sale->hourly_service_work_list =
			hourly_service_work_list(
				HOURLY_SERVICE_WORK_SELECT,
				HOURLY_SERVICE_WORK_TABLE,
				fund_name,
				full_name,
				contact_key,
				sale_date_time,
				hourly_service_sale->service_name,
				hourly_service_sale->service_description,
				fund_boolean,
				contact_key_boolean );

		hourly_service_sale->hourly_service_work_hours =
			hourly_service_work_hours(
				hourly_service_sale->
					hourly_service_work_list );

		hourly_service_sale->hourly_service_sale_net_revenue =
			HOURLY_SERVICE_SALE_NET_REVENUE(
				hourly_service_sale->hourly_service_work_hours,
				hourly_service_sale->hourly_rate,
				hourly_service_sale->discount_amount );

		hourly_service_sale->update_string_list =
			hourly_service_sale_update_string_list(
				SQL_DELIMITER,
				fund_name,
				full_name,
				contact_key,
				sale_date_time,
				service_name,
				service_description,
				fund_boolean,
				contact_key_boolean,
				hourly_service_sale->
					hourly_service_sale_estimated_revenue,
				hourly_service_sale->
					hourly_service_work_hours,
				hourly_service_sale->
					hourly_service_sale_net_revenue );

		hourly_service_sale->primary_key_list =
			hourly_service_sale_primary_key_list(
					SALE_SERVICE_NAME_COLUMN,
					SALE_SERVICE_DESCRIPTION_COLUMN,
					fund_boolean,
					contact_key_boolean );

		hourly_service_sale->sale_update_system_string =
			/* -------------------- */
			/* Borrow sale_update() */
			/* Returns heap memory  */
			/* -------------------- */
			sale_update_system_string(
				HOURLY_SERVICE_SALE_TABLE,
				hourly_service_sale->primary_key_list );
	}

	return hourly_service_sale;
}

void hourly_service_sale_update(
		LIST *update_string_list,
		char *sale_update_system_string )
{
	/* Borrow sale_update() */
	/* -------------------- */
	(void)sale_update(
		(char *)0 /* application_name for transaction_update */,
		update_string_list,
		sale_update_system_string,
		(SALE_TRANSACTION *)0,
		(SALE_LOSS_TRANSACTION*)0 );
}

HOURLY_SERVICE_SALE *hourly_service_sale_fetch(
		const char *hourly_service_sale_select,
		const char *hourly_service_sale_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		boolean fund_boolean,
		boolean contact_key_boolean,
		boolean hourly_service_work_boolean )
{
	char *where;
	char *system_string;
	char *input;

	if ( !full_name
	||   !sale_date_time
	||   !service_name
	||   !service_description )
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
		hourly_service_sale_primary_where(
			SALE_SERVICE_NAME_COLUMN,
			SALE_SERVICE_DESCRIPTION_COLUMN,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			service_name,
			service_description,
			fund_boolean,
			contact_key_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)hourly_service_sale_select,
			(char *)hourly_service_sale_table,
			where );

	input =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_system_input(
			system_string );

	if ( !input ) return NULL;

	return
	hourly_service_sale_parse(
		fund_name,
		full_name,
		contact_key,
		sale_date_time,
		fund_boolean,
		contact_key_boolean,
		hourly_service_work_boolean,
		input );
}

HOURLY_SERVICE_SALE *hourly_service_sale_trigger(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		char *state )
{
	HOURLY_SERVICE_SALE *hourly_service_sale = {0};

	if ( !full_name
	||   !sale_date_time
	||   !service_name
	||   !service_description
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

	if ( strcmp( state, APPASERVER_PREDELETE_STATE ) == 0 ) return NULL;

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

		hourly_service_sale =
			hourly_service_sale_fetch(
				HOURLY_SERVICE_SALE_SELECT,
				HOURLY_SERVICE_SALE_TABLE,
				fund_name,
				full_name,
				contact_key,
				sale_date_time,
				service_name,
				service_description,
				fund_boolean,
				contact_key_boolean,
				1 /* hourly_service_work_boolean */ );
	}

	return hourly_service_sale;
}

char *hourly_service_sale_primary_data_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *primary_data_string;
	char *escape_name;
	char *escape_description;
	char service_sale_primary_data_string[ 1024 ];

	if ( !full_name
	||   !sale_date_time
	||   !service_name
	||   !service_description )
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

	escape_name =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		security_escape(
			service_name /* datum */ );

	escape_description =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		security_escape(
			service_description /* datum */ );

	snprintf(
		service_sale_primary_data_string,
		sizeof ( service_sale_primary_data_string ),
		"%s%c%s%c%s",
		primary_data_string,
		sql_delimiter,
		escape_name,
		sql_delimiter,
		escape_description );

	free( primary_data_string );
	free( escape_name );
	free( escape_description );

	return strdup( service_sale_primary_data_string );
}

LIST *hourly_service_sale_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		boolean fund_boolean,
		boolean contact_key_boolean,
		double estimated_revenue,
		double work_hours,
		double net_revenue )
{
	char *primary_data_string;
	char *update_string;
	LIST *list = list_new();

	if ( !full_name
	||   !sale_date_time
	||   !service_name
	||   !service_description )
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
		hourly_service_sale_primary_data_string(
			sql_delimiter,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			service_name,
			service_description,
			fund_boolean,
			contact_key_boolean );

	update_string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_string(
			sql_delimiter,
			primary_data_string,
			"estimated_revenue" /* column_name */,
			estimated_revenue /* money */,
			1 /* set_boolean */ );

	list_set( list, update_string );

	update_string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_string(
			sql_delimiter,
			primary_data_string,
			"work_hours" /* column_name */,
			work_hours /* money */,
			1 /* set_boolean */ );

	list_set( list, update_string );

	update_string =
		sale_update_string(
			sql_delimiter,
			primary_data_string,
			"net_revenue" /* column_name */,
			net_revenue /* money */,
			1 /* set_boolean */ );

	list_set( list, update_string );

	free( primary_data_string );

	return list;
}

double hourly_service_sale_total( LIST *hourly_service_sale_list )
{
	HOURLY_SERVICE_SALE *hourly_service_sale;
	double total = 0.0;

	if ( list_rewind( hourly_service_sale_list ) )
	do {
		hourly_service_sale = list_get( hourly_service_sale_list );

		total +=
			hourly_service_sale->
				net_revenue;

	} while( list_next( hourly_service_sale_list ) );

	return total;
}

char *hourly_service_sale_primary_where(
		const char *sale_service_name_column,
		const char *sale_service_description_column,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	static char where[ 320 ];
	char *primary_where;
	char *escape_name;
	char *escape_description;

	if ( !full_name
	||   !sale_date_time
	||   !service_name
	||   !service_description )
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
		"%s and %s = '%s' and %s = '%s'",
		primary_where,
		sale_service_name_column,
		( escape_name =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			security_escape(
				service_name ) ),
		sale_service_description_column,
		( escape_description =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			security_escape(
				service_description ) ) );

	free( escape_name );
	free( escape_description );

	return where;
}

LIST *hourly_service_sale_primary_key_list(
		const char *sale_service_name_column,
		const char *sale_service_description_column,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	LIST *primary_key_list;

	primary_key_list =
		sale_fetch_primary_key_list(
			PREDICTIVE_FUND_COLUMN,
			ENTITY_FULL_NAME_COLUMN,
			ENTITY_CONTACT_KEY_COLUMN,
			SALE_DATE_TIME_COLUMN,
			fund_boolean,
			contact_key_boolean );

	list_set( primary_key_list, (void *)sale_service_name_column );
	list_set( primary_key_list, (void *)sale_service_description_column );

	return primary_key_list;
}

LIST *hourly_service_sale_list(
		const char *hourly_service_sale_select,
		const char *hourly_service_sale_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean,
		boolean hourly_service_work_boolean )
{
	char *where;
	LIST *list = list_new();
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	HOURLY_SERVICE_SALE *hourly_service_sale;

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
			(char *)hourly_service_sale_select,
			(char *)hourly_service_sale_table,
			where );

	/* -------------- */
	/* Safely returns */
	/* -------------- */
	input_pipe = appaserver_input_pipe( system_string );

	free( system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		hourly_service_sale =
			hourly_service_sale_parse(
				fund_name,
				full_name,
				contact_key,
				sale_date_time,
				fund_boolean,
				contact_key_boolean,
				hourly_service_work_boolean,
				input );

		if ( !hourly_service_sale )
		{
			char message[ 2048 ];

			snprintf(
				message,
				sizeof ( message ),
			"hourly_service_sale_parse(%s) returned empty.",
				input );

			pclose( input_pipe );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set( list, hourly_service_sale );
	}

	pclose( input_pipe );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

