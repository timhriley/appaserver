/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/hourly_service_work.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "sql.h"
#include "date.h"
#include "entity.h"
#include "security.h"
#include "sale.h"
#include "hourly_service_sale.h"
#include "hourly_service_work.h"

LIST *hourly_service_work_list(
		const char *hourly_service_work_select,
		const char *hourly_service_work_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	LIST *list = list_new();
	char *where;
	char *system_string;
	FILE *pipe;
	char input[ 1024 ];
	HOURLY_SERVICE_WORK *hourly_service_work;

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
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)hourly_service_work_select,
			(char *)hourly_service_work_table,
			where );

	pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			system_string );

	free( system_string );

	while ( string_input( input, pipe, sizeof ( input ) ) )
	{
		hourly_service_work =
			hourly_service_work_parse(
				fund_name,
				full_name,
				contact_key,
				sale_date_time,
				service_name,
				service_description,
				fund_boolean,
				contact_key_boolean,
				input );

		if ( !hourly_service_work )
		{
			char message[ 2048 ];

			snprintf(
				message,
				sizeof ( message ),
				"hourly_service_work_parse(%s) returned empty.",
				input );

			pclose( pipe );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set( list, hourly_service_work );
	}

	pclose( pipe );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

HOURLY_SERVICE_WORK *hourly_service_work_parse(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *input )
{
	char begin_work_date_time[ 128 ];
	char buffer[ 1024 ];
	HOURLY_SERVICE_WORK *hourly_service_work;

	if ( !input || !*input ) return NULL;

	/* See HOURLY_SERVICE_WORK_SELECT */
	/* ------------------------------ */
	piece( begin_work_date_time, SQL_DELIMITER, input, 0 );

	hourly_service_work =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		hourly_service_work_new(
			strdup( begin_work_date_time ) );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer )
		hourly_service_work->end_work_date_time =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) hourly_service_work->work_description = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) hourly_service_work->activity = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) hourly_service_work->discount_hours = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer ) hourly_service_work->work_hours = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer )
		hourly_service_work->
			appaserver_full_name =
				strdup( buffer );

	hourly_service_work->hourly_service_work_hours =
		hourly_service_work_hours(
			hourly_service_work->begin_work_date_time,
			hourly_service_work->end_work_date_time,
			hourly_service_work->discount_hours );

	hourly_service_work->primary_key_list =
		hourly_service_work_primary_key_list(
			SALE_BEGIN_WORK_COLUMN,
			fund_boolean,
			contact_key_boolean );

	hourly_service_work->update_string_list =
		hourly_service_work_update_string_list(
			SQL_DELIMITER,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			service_name,
			service_description,
			hourly_service_work->begin_work_date_time,
			fund_boolean,
			contact_key_boolean,
			hourly_service_work->hourly_service_work_hours );

	hourly_service_work->sale_update_system_string =
		/* -------------------- */
		/* Borrow sale_update() */
		/* Returns heap memory  */
		/* -------------------- */
		sale_update_system_string(
			HOURLY_SERVICE_WORK_TABLE,
			hourly_service_work->primary_key_list );

	return hourly_service_work;
}

HOURLY_SERVICE_WORK *hourly_service_work_new(
		char *begin_work_date_time )
{
	HOURLY_SERVICE_WORK *hourly_service_work;

	if ( !begin_work_date_time )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"begin_work_date_time is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	hourly_service_work = hourly_service_work_calloc();
	hourly_service_work->begin_work_date_time = begin_work_date_time;

	return hourly_service_work;
}

HOURLY_SERVICE_WORK *hourly_service_work_calloc( void )
{
	HOURLY_SERVICE_WORK *hourly_service_work;

	if ( ! ( hourly_service_work =
			calloc( 1,
				sizeof ( HOURLY_SERVICE_WORK ) ) ) )
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

	return hourly_service_work;
}

double hourly_service_work_list_hours( LIST *hourly_service_work_list )
{
	HOURLY_SERVICE_WORK *hourly_service_work;
	double hours = 0.0;

	if ( list_rewind( hourly_service_work_list ) )
	do {
		hourly_service_work =
			list_get( hourly_service_work_list );

		hours += hourly_service_work->work_hours;

	} while ( list_next( hourly_service_work_list ) );

	return hours;
}

HOURLY_SERVICE_WORK *hourly_service_work_fetch(
		const char *hourly_service_work_select,
		const char *hourly_service_work_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		char *begin_work_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *where;
	char *system_string;
	char *input;

	if ( !full_name
	||   !sale_date_time
	||   !service_name
	||   !service_description
	||   !begin_work_date_time )
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
		hourly_service_work_primary_where(
			SALE_BEGIN_WORK_COLUMN,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			service_name,
			service_description,
			begin_work_date_time,
			fund_boolean,
			contact_key_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)hourly_service_work_select,
			(char *)hourly_service_work_table,
			where );

	input =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_system_input(
			system_string );

	if ( !input ) return NULL;

	return
	hourly_service_work_parse(
		fund_name,
		full_name,
		contact_key,
		sale_date_time,
		service_name,
		service_description,
		fund_boolean,
		contact_key_boolean,
		input );
}

void hourly_service_work_update(
		LIST *update_string_list,
		char *system_string )
{
	/* Borrow sale_update() */
	/* -------------------- */
	(void)sale_update(
		(char *)0 /* application_name for update_statement_execute */,
		update_string_list,
		system_string,
		(SALE_TRANSACTION *)0,
		(SALE_LOSS_TRANSACTION*)0 );
}

HOURLY_SERVICE_WORK *hourly_service_work_trigger(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		char *begin_work_date_time,
		char *state )
{
	HOURLY_SERVICE_WORK *hourly_service_work = {0};

	if ( !full_name
	||   !sale_date_time
	||   !service_name
	||   !service_description
	||   !begin_work_date_time
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

		hourly_service_work =
			hourly_service_work_fetch(
				HOURLY_SERVICE_WORK_SELECT,
				HOURLY_SERVICE_WORK_TABLE,
				fund_name,
				full_name,
				contact_key,
				sale_date_time,
				service_name,
				service_description,
				begin_work_date_time,
				fund_boolean,
				contact_key_boolean );
	}
	
	return hourly_service_work;
}

char *hourly_service_work_primary_data_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		char *begin_work_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *sale_primary_data_string;
	char *escape;
	char work_primary_data_string[ 1024 ];

	if ( !full_name
	||   !sale_date_time
	||   !service_name
	||   !service_description
	||   !begin_work_date_time )
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

	sale_primary_data_string =
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

	escape =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		security_escape(
			begin_work_date_time /* datum */ );

	snprintf(
		work_primary_data_string,
		sizeof ( work_primary_data_string ),
		"%s%c%s",
		sale_primary_data_string,
		sql_delimiter,
		escape );

	free( escape );

	return strdup( work_primary_data_string );
}

LIST *hourly_service_work_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		char *begin_work_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean,
		double work_hours )
{
	char *work_primary_data_string;
	char *update_string;
	LIST *list = list_new();

	if ( !full_name
	||   !sale_date_time
	||   !service_name
	||   !service_description
	||   !begin_work_date_time )
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

	work_primary_data_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		hourly_service_work_primary_data_string(
			sql_delimiter,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			service_name,
			service_description,
			begin_work_date_time,
			fund_boolean,
			contact_key_boolean );

	update_string =
		/* ------------------------------------------------ */
		/* Returns heap memory or null (if not set_boolean) */
		/* ------------------------------------------------ */
		sale_update_string(
			sql_delimiter,
			work_primary_data_string,
			"work_hours" /* column_name */,
			work_hours /* money */,
			1 /* set_boolean */ );

	list_set( list, update_string );

	free( work_primary_data_string );

	return list;
}

char *hourly_service_work_primary_where(
		const char *sale_begin_work_column,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		char *begin_work_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	static char where[ 320 ];
	char *service_sale_primary_where;
	char *escape;

	if ( !full_name
	||   !sale_date_time
	||   !service_name
	||   !service_description
	||   !begin_work_date_time )
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

	service_sale_primary_where =
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

	escape =
		/* --------------------- */
		/* Returns heap memory */
		/* --------------------- */
		security_escape(
			begin_work_date_time /* datum */ );

	snprintf(
		where,
		sizeof ( where ),
		"%s and %s = '%s'",
		service_sale_primary_where,
		sale_begin_work_column,
		escape );

	free( escape );

	return where;
}

LIST *hourly_service_work_primary_key_list(
		const char *sale_begin_work_column,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	LIST *list;

	list =
		hourly_service_sale_primary_key_list(
			SALE_SERVICE_NAME_COLUMN,
			SALE_SERVICE_DESCRIPTION_COLUMN,
			fund_boolean,
			contact_key_boolean );

	list_set( list, (void *)sale_begin_work_column );

	return list;
}

double hourly_service_work_hours(
		char *begin_work_date_time,
		char *end_work_date_time,
		double discount_hours )
{
	return
	sale_work_hours(
		begin_work_date_time,
		end_work_date_time ) -
	discount_hours;
}
