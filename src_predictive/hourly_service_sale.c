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
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *service_description )
{
	HOURLY_SERVICE_SALE *hourly_service_sale;

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

	hourly_service_sale = hourly_service_sale_calloc();

	hourly_service_sale->full_name = full_name;
	hourly_service_sale->street_address = street_address;
	hourly_service_sale->sale_date_time = sale_date_time;
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
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *input )
{
	HOURLY_SERVICE_SALE *hourly_service_sale;
	char service_name[ 128 ];
	char service_description[ 128 ];
	char piece_buffer[ 128 ];

	if ( !full_name
	||   !street_address
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
			full_name,
			street_address,
			sale_date_time,
			strdup( service_name ),
			strdup( service_description ) );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	if ( *piece_buffer )
		hourly_service_sale->estimated_hours =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	if ( *piece_buffer )
		hourly_service_sale->hourly_rate =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	if ( *piece_buffer )
		hourly_service_sale->estimated_revenue =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	if ( *piece_buffer )
		hourly_service_sale->discount_amount =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	if ( *piece_buffer )
		hourly_service_sale->work_hours =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	if ( *piece_buffer )
		hourly_service_sale->net_revenue =
			atof( piece_buffer );

	hourly_service_sale->hourly_service_sale_estimated_revenue =
		hourly_service_sale_estimated_revenue(
			hourly_service_sale->estimated_hours,
			hourly_service_sale->hourly_rate,
			hourly_service_sale->discount_amount );

	hourly_service_sale->hourly_service_work_list =
		hourly_service_work_list(
			HOURLY_SERVICE_WORK_SELECT,
			HOURLY_SERVICE_WORK_TABLE,
			full_name,
			street_address,
			sale_date_time,
			hourly_service_sale->service_name,
			hourly_service_sale->service_description );

	hourly_service_sale->hourly_service_work_hours =
		hourly_service_work_hours(
			hourly_service_sale->
				hourly_service_work_list );

	hourly_service_sale->hourly_service_sale_net_revenue =
		hourly_service_sale_net_revenue(
			hourly_service_sale->hourly_service_work_hours,
			hourly_service_sale->hourly_rate,
			hourly_service_sale->discount_amount );

	return hourly_service_sale;
}

char *hourly_service_sale_update_system_string(
		const char *hourly_service_sale_table )
{
	char system_string[ 1024 ];
	char *key;

	key =	"full_name,"
		"street_address,"
		"sale_date_time,"
		"service_name,"
		"service_description";

	snprintf(
		system_string,
		sizeof ( system_string ),
		"update_statement.e table=%s key=%s carrot=y | "
		"tee_appaserver.sh | "
		"sql.e",
		hourly_service_sale_table,
		key );

	return strdup( system_string );
}

void hourly_service_sale_update(
		const char *hourly_service_sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		double hourly_service_sale_estimated_revenue,
		double hourly_service_work_hours,
		double hourly_service_sale_net_revenue )
{
	char *system_string;
	FILE *pipe;

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

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		hourly_service_sale_update_system_string(
			hourly_service_sale_table );

	pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			system_string );

	free( system_string );

	fprintf(pipe,
	 	"%s^%s^%s^%s^%s^estimated_revenue^%.2lf\n",
		full_name,
		street_address,
		sale_date_time,
		service_name,
		service_description,
		hourly_service_sale_estimated_revenue );

	fprintf(pipe,
	 	"%s^%s^%s^%s^%s^work_hours^%.2lf\n",
		full_name,
		street_address,
		sale_date_time,
		service_name,
		service_description,
		hourly_service_work_hours );

	fprintf(pipe,
	 	"%s^%s^%s^%s^%s^net_revenue^%.2lf\n",
		full_name,
		street_address,
		sale_date_time,
		service_name,
		service_description,
		hourly_service_sale_net_revenue );

	pclose( pipe );
}

LIST *hourly_service_sale_list(
		const char *hourly_service_sale_select,
		const char *hourly_service_sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time )
{
	char *where;
	LIST *list = list_new();
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];
	HOURLY_SERVICE_SALE *hourly_service_sale;

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
			(char *)hourly_service_sale_select,
			(char *)hourly_service_sale_table,
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
		hourly_service_sale =
			hourly_service_sale_parse(
				full_name,
				street_address,
				sale_date_time,
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

double hourly_service_sale_net_revenue(
		double work_list_hours,
		double hourly_rate,
		double discount_amount )
{
	return
	( work_list_hours * hourly_rate ) - discount_amount;
}

char *hourly_service_sale_primary_where(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *service_description )
{
	static char where[ 512 ];
	char *primary_where;
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
		"%s and service_name = '%s' and service_description = '%s'",
		primary_where,
		(tmp1 =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			security_escape(
				service_name ) ),
		(tmp2 =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			security_escape(
				service_description ) ) );

	free( tmp1 );
	free( tmp2 );

	return where;
}

HOURLY_SERVICE_SALE *hourly_service_sale_fetch(
		const char *hourly_service_sale_select,
		const char *hourly_service_sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *service_description )
{
	char *where;
	char *system_string;
	char *fetch;

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
		hourly_service_sale_primary_where(
			full_name,
			street_address,
			sale_date_time,
			service_name,
			service_description );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)hourly_service_sale_select,
			(char *)hourly_service_sale_table,
			where );

	fetch =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_fetch(
			system_string );

	if ( !fetch ) return NULL;

	return
	hourly_service_sale_parse(
		full_name,
		street_address,
		sale_date_time,
		fetch );
}

double hourly_service_sale_estimated_revenue(
		double estimated_hours,
		double hourly_rate,
		double discount_amount )
{
	return
	( estimated_hours *
	  hourly_rate ) -
	discount_amount;
}
