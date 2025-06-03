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
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name )
{
	LIST *list = list_new();
	char *where;
	char *system_string;
	FILE *pipe;
	char input[ 1024 ];
	FIXED_SERVICE_WORK *fixed_service_work;

	if ( !full_name
	||   !street_address
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
		fixed_service_work_where(
			full_name,
			street_address,
			sale_date_time,
			service_name );

	system_string =
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
				full_name,
				street_address,
				sale_date_time,
				service_name,
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

char *fixed_service_work_where(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name )
{
	static char where[ 256 ];
	char *tmp1;

	if ( !full_name
	||   !street_address
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

	snprintf(
		where,
		sizeof ( where ),
		"%s and "
		"service_name = '%s'",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		sale_primary_where(
			full_name,
			street_address,
			sale_date_time ),
		/* --------------------- */
		/* Returns heap memory */
		/* --------------------- */
		( tmp1 = security_escape( service_name ) ) );

	free( tmp1 );

	return where;
}

FIXED_SERVICE_WORK *fixed_service_work_parse(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *input )
{
	char begin_work_date_time[ 128 ];
	char piece_buffer[ 1024 ];
	FIXED_SERVICE_WORK *fixed_service_work;

	if ( !full_name
	||   !street_address
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

	fixed_service_work =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		fixed_service_work_new(
			full_name,
			street_address,
			sale_date_time,
			service_name,
			strdup( begin_work_date_time ) );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	if ( *piece_buffer )
		fixed_service_work->end_work_date_time =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	if ( *piece_buffer )
		fixed_service_work->work_description =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	if ( *piece_buffer )
		fixed_service_work->activity =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	if ( *piece_buffer )
		fixed_service_work->appaserver_full_name =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	if ( *piece_buffer )
		fixed_service_work->appaserver_street_address =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	if ( *piece_buffer )
		fixed_service_work->work_hours =
			atof( piece_buffer );

	fixed_service_work->sale_work_hours =
		sale_work_hours(
			fixed_service_work->begin_work_date_time,
			fixed_service_work->end_work_date_time );

	return fixed_service_work;
}

FIXED_SERVICE_WORK *fixed_service_work_new(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *begin_work_date_time )
{
	FIXED_SERVICE_WORK *fixed_service_work;

	if ( !full_name
	||   !street_address
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

	fixed_service_work = fixed_service_work_calloc();

	fixed_service_work->full_name = full_name;
	fixed_service_work->street_address = street_address;
	fixed_service_work->sale_date_time = sale_date_time;
	fixed_service_work->service_name = service_name;
	fixed_service_work->begin_work_date_time = begin_work_date_time;

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

		hours +=
			fixed_service_work->
				sale_work_hours;

	} while ( list_next( fixed_service_work_list ) );

	return hours;
}

char *fixed_service_work_update_system_string(
		const char *fixed_service_work_table )
{
	char system_string[ 1024 ];
	char *key;

	key =	"full_name,"
		"street_address,"
		"sale_date_time,"
		"service_name,"
		"begin_work_date_time";

	snprintf(
		system_string,
		sizeof ( system_string ),
		"update_statement.e table=%s key=%s carrot=y | "
		"tee_appaserver.sh | "
		"sql.e",
		fixed_service_work_table,
		key );

	return strdup( system_string );
}

void fixed_service_work_update(
		const char *fixed_service_work_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *begin_work_date_time,
		double sale_work_hours )
{
	char *system_string;
	FILE *pipe;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		fixed_service_work_update_system_string(
			fixed_service_work_table );

	pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			system_string );

	fprintf(pipe,
	 	"%s^%s^%s^%s^%s^work_hours^%.2lf\n",
		full_name,
		street_address,
		sale_date_time,
		service_name,
		begin_work_date_time,
		sale_work_hours );

	pclose( pipe );
}

FIXED_SERVICE_WORK *fixed_service_work_fetch(
		const char *fixed_service_work_select,
		const char *fixed_service_work_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *begin_work_date_time )
{
	char *where;
	char *system_string;
	char *fetch;

	if ( !full_name
	||   !street_address
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
			full_name,
			street_address,
			sale_date_time,
			service_name,
			begin_work_date_time );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)fixed_service_work_select,
			(char *)fixed_service_work_table,
			where );

	fetch =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_fetch(
			system_string );

	if ( !fetch ) return NULL;

	return
	fixed_service_work_parse(
		full_name,
		street_address,
		sale_date_time,
		service_name,
		fetch );
}

char *fixed_service_work_primary_where(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *begin_work_date_time )
{
	static char where[ 256 ];
	char *tmp;

	if ( !full_name
	||   !street_address
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

	snprintf(
		where,
		sizeof ( where ),
		"%s and begin_work_date_time = '%s'",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		fixed_service_sale_primary_where(
			full_name,
			street_address,
			sale_date_time,
			service_name ),
		/* --------------------- */
		/* Returns heap memory */
		/* --------------------- */
		( tmp = security_escape( begin_work_date_time ) ) );

	free( tmp );

	return where;
}

