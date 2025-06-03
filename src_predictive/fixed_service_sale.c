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
#include "security.h"
#include "sql.h"
#include "sale.h"
#include "fixed_service_work.h"
#include "fixed_service_sale.h"

FIXED_SERVICE_SALE *fixed_service_sale_new(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name )
{
	FIXED_SERVICE_SALE *fixed_service_sale;

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

	fixed_service_sale = fixed_service_sale_calloc();

	fixed_service_sale->full_name = full_name;
	fixed_service_sale->street_address = street_address;
	fixed_service_sale->sale_date_time = sale_date_time;
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
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *input )
{
	FIXED_SERVICE_SALE *fixed_service_sale;
	char service_name[ 128 ];
	char piece_buffer[ 128 ];

	if ( !full_name
	||   !street_address
	||   !sale_date_time
	||   !input
	||   !*input )
	{
		return NULL;
	}

	piece( service_name, SQL_DELIMITER, input, 0 );

	fixed_service_sale =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		fixed_service_sale_new(
			full_name,
			street_address,
			sale_date_time,
			strdup( service_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	if ( *piece_buffer )
		fixed_service_sale->fixed_price =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	if ( *piece_buffer )
		fixed_service_sale->estimated_hours =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	if ( *piece_buffer )
		fixed_service_sale->discount_amount =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	if ( *piece_buffer )
		fixed_service_sale->work_hours =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	if ( *piece_buffer )
		fixed_service_sale->net_revenue =
			atof( piece_buffer );

	fixed_service_sale->fixed_service_work_list =
		fixed_service_work_list(
			FIXED_SERVICE_WORK_SELECT,
			FIXED_SERVICE_WORK_TABLE,
			full_name,
			street_address,
			sale_date_time,
			fixed_service_sale->service_name );

	fixed_service_sale->fixed_service_work_hours =
		fixed_service_work_hours(
			fixed_service_sale->fixed_service_work_list );

	fixed_service_sale->fixed_service_sale_net_revenue =
		fixed_service_sale_net_revenue(
			fixed_service_sale->fixed_price,
			fixed_service_sale->discount_amount );

	return fixed_service_sale;
}

char *fixed_service_sale_update_system_string(
		const char *fixed_service_sale_table )
{
	char system_string[ 1024 ];
	char *key;

	key =	"full_name,"
		"street_address,"
		"sale_date_time,"
		"service_name";

	snprintf(
		system_string,
		sizeof ( system_string ),
		"update_statement.e table=%s key=%s carrot=y | "
		"tee_appaserver.sh | "
		"sql.e",
		fixed_service_sale_table,
		key );

	return strdup( system_string );
}

void fixed_service_sale_update(
		const char *fixed_service_sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		double fixed_service_work_hours,
		double fixed_service_sale_net_revenue )
{
	char *system_string;
	FILE *pipe;

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

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		fixed_service_sale_update_system_string(
			fixed_service_sale_table );

	pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			system_string );

	free( system_string );

	fprintf(pipe,
	 	"%s^%s^%s^%s^work_hours^%.2lf\n",
		full_name,
		street_address,
		sale_date_time,
		service_name,
		fixed_service_work_hours );

	fprintf(pipe,
	 	"%s^%s^%s^%s^net_revenue^%.2lf\n",
		full_name,
		street_address,
		sale_date_time,
		service_name,
		fixed_service_sale_net_revenue );

	pclose( pipe );
}

LIST *fixed_service_sale_list(
		const char *fixed_service_sale_select,
		const char *fixed_service_sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time )
{
	char *where;
	LIST *list = list_new();
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	FIXED_SERVICE_SALE *fixed_service_sale;

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
			(char *)fixed_service_sale_select,
			(char *)fixed_service_sale_table,
			where );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			system_string );

	free( system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		fixed_service_sale =
			fixed_service_sale_parse(
				full_name,
				street_address,
				sale_date_time,
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
				fixed_service_sale_net_revenue;

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

FIXED_SERVICE_SALE *fixed_service_sale_seek(
		LIST *fixed_service_sale_list,
		char *service_name )
{
	FIXED_SERVICE_SALE *fixed_service_sale;

	if ( !service_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"service_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( fixed_service_sale_list ) )
	do {
		fixed_service_sale =
			list_get(
				fixed_service_sale_list );

		if ( strcmp(
			service_name,
			fixed_service_sale->service_name ) == 0 )
		{
			return fixed_service_sale;
		}

	} while ( list_next( fixed_service_sale_list ) );

	return NULL;
}

FIXED_SERVICE_SALE *fixed_service_sale_fetch(
		const char *fixed_service_sale_select,
		const char *fixed_service_sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name )
{
	char *where;
	char *system_string;
	char *fetch;

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
		fixed_service_sale_primary_where(
			full_name,
			street_address,
			sale_date_time,
			service_name );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)fixed_service_sale_select,
			(char *)fixed_service_sale_table,
			where );

	fetch =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_fetch(
			system_string );

	if ( !fetch ) return NULL;

	return
	fixed_service_sale_parse(
		full_name,
		street_address,
		sale_date_time,
		fetch );
}

char *fixed_service_sale_primary_where(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name )
{
	static char where[ 512 ];
	char *primary_where;
	char *tmp;

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

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		sale_primary_where(
			full_name,
			street_address,
			sale_date_time );

	snprintf(
		where,
		sizeof ( where ),
		"%s and service_name = '%s'",
		primary_where,
		(tmp =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			security_escape(
				service_name ) ) );

	free( tmp );

	return where;
}

