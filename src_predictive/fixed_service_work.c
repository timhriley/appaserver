/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_service_work.c			*/
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
#include "fixed_service_sale.h"
#include "fixed_service_work.h"

LIST *fixed_service_work_list(
		const char *fixed_service_work_select,
		const char *fixed_service_work_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	LIST *list = list_new();
	char *where;
	char *system_string;
	FILE *pipe;
	char input[ 1024 ];
	FIXED_SERVICE_WORK *fixed_service_work;

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
			(char *)fixed_service_work_select,
			(char *)fixed_service_work_table,
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
		fixed_service_work =
			fixed_service_work_parse(
				fund_name,
				full_name,
				contact_key,
				sale_date_time,
				service_name,
				fund_boolean,
				contact_key_boolean,
				input );

		if ( !fixed_service_work )
		{
			char message[ 2048 ];

			snprintf(
				message,
				sizeof ( message ),
				"fixed_service_work_new(%s) returned empty.",
				input );

			pclose( pipe );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set( list, fixed_service_work );
	}

	pclose( pipe );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

FIXED_SERVICE_WORK *fixed_service_work_parse(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *input )
{
	char begin_work_date_time[ 128 ];
	char end_work_date_time[ 128 ];
	char activity[ 128 ];
	double work_hours;
	char buffer[ 1024 ];
	FIXED_SERVICE_WORK *fixed_service_work;

	if ( !full_name
	||   !sale_date_time
	||   !service_name
	||   !input
	||   !*input )
	{
		return NULL;
	}

	/* See FIXED_SERVICE_WORK_SELECT */
	/* ------------------------------ */
	piece( begin_work_date_time, SQL_DELIMITER, input, 0 );
	piece( end_work_date_time, SQL_DELIMITER, input, 1 );
	piece( activity, SQL_DELIMITER, input, 2 );
	piece( buffer, SQL_DELIMITER, input, 3 );

	work_hours = atof( buffer );

	fixed_service_work =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		fixed_service_work_new(
			strdup( begin_work_date_time ),
			strdup( end_work_date_time ),
			strdup( activity ),
			work_hours );

	fixed_service_work->sale_work_hours =
		sale_work_hours(
			fixed_service_work->begin_work_date_time,
			fixed_service_work->end_work_date_time );

	fixed_service_work->update_string_list =
		fixed_service_work_update_string_list(
			SQL_DELIMITER,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			service_name,
			fixed_service_work->begin_work_date_time,
			fund_boolean,
			contact_key_boolean,
			fixed_service_work->sale_work_hours );

	fixed_service_work->primary_key_list =
		fixed_service_work_primary_key_list(
			SALE_BEGIN_WORK_COLUMN,
			fund_boolean,
			contact_key_boolean );

	fixed_service_work->sale_update_system_string =
		/* -------------------- */
		/* Borrow sale_update() */
		/* Returns heap memory  */
		/* -------------------- */
		sale_update_system_string(
			FIXED_SERVICE_WORK_TABLE,
			fixed_service_work->primary_key_list );

	return fixed_service_work;
}

FIXED_SERVICE_WORK *fixed_service_work_new(
		char *begin_work_date_time,
		char *end_work_date_time,
		char *activity,
		double work_hours )
{
	FIXED_SERVICE_WORK *fixed_service_work;

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

	fixed_service_work = fixed_service_work_calloc();

	fixed_service_work->begin_work_date_time = begin_work_date_time;
	fixed_service_work->end_work_date_time = end_work_date_time;
	fixed_service_work->activity = activity;
	fixed_service_work->work_hours = work_hours;

	return fixed_service_work;
}

FIXED_SERVICE_WORK *fixed_service_work_calloc( void )
{
	FIXED_SERVICE_WORK *fixed_service_work;

	if ( ! ( fixed_service_work =
			calloc( 1,
				sizeof ( FIXED_SERVICE_WORK ) ) ) )
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

	return fixed_service_work;
}

double fixed_service_work_hours( LIST *fixed_service_work_list )
{
	FIXED_SERVICE_WORK *fixed_service_work;
	double hours = 0.0;

	if ( list_rewind( fixed_service_work_list ) )
	do {
		fixed_service_work =
			list_get( fixed_service_work_list );

		hours += fixed_service_work->work_hours;

	} while ( list_next( fixed_service_work_list ) );

	return hours;
}

FIXED_SERVICE_WORK *fixed_service_work_fetch(
		const char *fixed_service_work_select,
		const char *fixed_service_work_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
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
		fixed_service_work_primary_where(
			SALE_BEGIN_WORK_COLUMN,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			service_name,
			begin_work_date_time,
			fund_boolean,
			contact_key_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)fixed_service_work_select,
			(char *)fixed_service_work_table,
			where );

	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	input = string_system_input( system_string );

	if ( !input ) return NULL;

	return
	fixed_service_work_parse(
		fund_name,
		full_name,
		contact_key,
		sale_date_time,
		service_name,
		fund_boolean,
		contact_key_boolean,
		input );
}

char *fixed_service_work_primary_where(
		const char *sale_begin_work_column,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *begin_work_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	static char where[ 320 ];
	char *service_sale_primary_where;
	char *injection_escape;

	if ( !full_name
	||   !sale_date_time
	||   !service_name
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
		fixed_service_sale_primary_where(
			SALE_SERVICE_NAME_COLUMN,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			service_name,
			fund_boolean,
			contact_key_boolean );

	injection_escape =
		/* --------------------- */
		/* Returns heap memory */
		/* --------------------- */
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			begin_work_date_time /* datum */ );

	snprintf(
		where,
		sizeof ( where ),
		"%s and %s = '%s'",
		service_sale_primary_where,
		sale_begin_work_column,
		injection_escape );

	free( injection_escape );

	return where;
}

char *fixed_service_work_primary_data_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *begin_work_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *sale_primary_data_string;
	char *injection_escape;
	char work_primary_data_string[ 1024 ];

	if ( !full_name
	||   !sale_date_time
	||   !service_name
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
		fixed_service_sale_primary_data_string(
			sql_delimiter,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			service_name,
			fund_boolean,
			contact_key_boolean );

	injection_escape =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			begin_work_date_time /* datum */ );

	snprintf(
		work_primary_data_string,
		sizeof ( work_primary_data_string ),
		"%s%c%s",
		sale_primary_data_string,
		sql_delimiter,
		injection_escape );

	free( injection_escape );

	return strdup( work_primary_data_string );
}

LIST *fixed_service_work_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *begin_work_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean,
		double sale_work_hours )
{
	char *work_primary_data_string;
	char *update_string;
	LIST *list = list_new();

	if ( !full_name
	||   !sale_date_time
	||   !service_name
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
		fixed_service_work_primary_data_string(
			sql_delimiter,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			service_name,
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
			sale_work_hours /* money */,
			1 /* set_boolean */ );

	list_set( list, update_string );

	free( work_primary_data_string );

	return list;
}

void fixed_service_work_update(
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

LIST *fixed_service_work_primary_key_list(
		const char *sale_begin_work_column,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	LIST *list;

	list =
		fixed_service_sale_primary_key_list(
			SALE_SERVICE_NAME_COLUMN,
			fund_boolean,
			contact_key_boolean );

	list_set( list, (void *)sale_begin_work_column );

	return list;
}

void fixed_service_work_trigger(
		char *application_name,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *begin_work_date_time,
		char *state )
{
	FIXED_SERVICE_WORK *fixed_service_work;

	if ( !full_name
	||   !sale_date_time
	||   !service_name
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

		fixed_service_work =
			fixed_service_work_fetch(
				FIXED_SERVICE_WORK_SELECT,
				FIXED_SERVICE_WORK_TABLE,
				fund_name,
				full_name,
				contact_key,
				sale_date_time,
				service_name,
				begin_work_date_time,
				fund_boolean,
				contact_key_boolean );

		if ( !fixed_service_work )
		{
			char message[ 1024 ];

			snprintf(
				message,
				sizeof ( message ),
			"fixed_service_work_fetch(%s,%s,%s,%s) returned empty.",
				full_name,
				sale_date_time,
				service_name,
				begin_work_date_time );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}


		fixed_service_work_update(
			fixed_service_work->update_string_list,
			fixed_service_work->sale_update_system_string );
	}
	
	fixed_service_sale_trigger(
		application_name,
		fund_name,
		full_name,
		contact_key,
		sale_date_time,
		service_name,
		state );
}

