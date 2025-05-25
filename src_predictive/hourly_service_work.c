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
#include "hourly_service_work.h"

LIST *hourly_service_work_list(
		const char *hourly_service_work_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *service_description )
{
	LIST *list = list_new();
	char *where;
	char *system_string;
	FILE *pipe;
	char input[ 1024 ];
	HOURLY_SERVICE_WORK *hourly_service_work;

	if ( !full_name
	||   !street_address
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
		hourly_service_work_where(
			full_name,
			street_address,
			sale_date_time,
			service_name,
			service_description );

	system_string =
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			HOURLY_SERVICE_WORK_SELECT,
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
				full_name,
				street_address,
				sale_date_time,
				service_name,
				service_description,
				input );

		if ( !hourly_service_work )
		{
			char message[ 2048 ];

			snprintf(
				message,
				sizeof ( message ),
				"hourly_service_work_new(%s) returned empty.",
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

char *hourly_service_work_where(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *service_description )
{
	static char where[ 256 ];
	char *tmp1;
	char *tmp2;

	if ( !full_name
	||   !street_address
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

	snprintf(
		where,
		sizeof ( where ),
		"%s and "
		"service_name = '%s' and "
		"service_description = '%s'",
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
		( tmp1 = security_escape( service_name ) ),
		( tmp2 = security_escape( service_description ) ) );

	free( tmp1 );
	free( tmp2 );

	return where;
}

HOURLY_SERVICE_WORK *hourly_service_work_parse(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		char *input )
{
	char begin_work_date_time[ 128 ];
	char piece_buffer[ 1024 ];
	HOURLY_SERVICE_WORK *hourly_service_work;

	if ( !full_name
	||   !street_address
	||   !sale_date_time
	||   !service_name
	||   !service_description
	||   !input
	||   !*input )
	{
		return NULL;
	}

	/* See HOURLY_SERVICE_WORK_SELECT */
	/* ------------------------------ */
	piece( begin_work_date_time, SQL_DELIMITER, input, 0 );

	hourly_service_work =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		hourly_service_work_new(
			full_name,
			street_address,
			sale_date_time,
			service_name,
			service_description,
			strdup( begin_work_date_time ) );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	if ( *piece_buffer )
		hourly_service_work->end_work_date_time =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	if ( *piece_buffer )
		hourly_service_work->work_description =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	if ( *piece_buffer )
		hourly_service_work->activity =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	if ( *piece_buffer )
		hourly_service_work->appaserver_full_name =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	if ( *piece_buffer )
		hourly_service_work->appaserver_street_address =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	if ( *piece_buffer )
		hourly_service_work->work_hours =
			atof( piece_buffer );

	hourly_service_work->sale_work_hours =
		sale_work_hours(
			hourly_service_work->begin_work_date_time,
			hourly_service_work->end_work_date_time );

	return hourly_service_work;
}

HOURLY_SERVICE_WORK *hourly_service_work_new(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		char *begin_work_date_time )
{
	HOURLY_SERVICE_WORK *hourly_service_work;

	if ( !full_name
	||   !street_address
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

	hourly_service_work = hourly_service_work_calloc();

	hourly_service_work->full_name = full_name;
	hourly_service_work->street_address = street_address;
	hourly_service_work->sale_date_time = sale_date_time;
	hourly_service_work->service_name = service_name;
	hourly_service_work->service_description = service_description;
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

double hourly_service_work_hours( LIST *hourly_service_work_list )
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

char *hourly_service_work_update_system_string(
		const char *hourly_service_work_table )
{
	char system_string[ 1024 ];
	char *key;

	key =	"full_name,"
		"street_address,"
		"sale_date_time,"
		"service_name,"
		"service_description,"
		"begin_work_date_time";

	snprintf(
		system_string,
		sizeof ( system_string ),
		"update_statement.e table=%s key=%s carrot=y | "
		"tee_appaserver.sh | "
		"sql.e",
		hourly_service_work_table,
		key );

	return strdup( system_string );
}

void hourly_service_work_update(
		const char *hourly_service_work_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		char *begin_work_date_time,
		double sale_work_hours )
{
	char *system_string;
	FILE *pipe;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		hourly_service_work_update_system_string(
			hourly_service_work_table );

	pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			system_string );

	fprintf(pipe,
	 	"%s^%s^%s^%s^%s^%s^work_hours^%.2lf\n",
		full_name,
		street_address,
		sale_date_time,
		service_name,
		service_description,
		begin_work_date_time,
		sale_work_hours );

	pclose( pipe );
}

HOURLY_SERVICE_WORK *hourly_service_work_seek(
		LIST *hourly_service_work_list,
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

	if ( list_rewind( hourly_service_work_list ) )
	do {
		hourly_service_work =
			list_get(
				hourly_service_work_list );

		if ( strcmp(
			begin_work_date_time,
			hourly_service_work->begin_work_date_time ) == 0 )
		{
			return hourly_service_work;
		}

	} while ( list_next( hourly_service_work_list ) );

	return NULL;
}
