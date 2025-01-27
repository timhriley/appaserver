/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/hourly_service_sale.c		*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "piece.h"
#include "date.h"
#include "sql.h"
#include "list.h"
#include "boolean.h"
#include "sale.h"
#include "entity.h"
#include "work.h"
#include "hourly_service_sale.h"

HOURLY_SERVICE_SALE *hourly_service_sale_new(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			char *service_description )
{
	HOURLY_SERVICE_SALE *hourly_service_sale;

	if ( ! ( hourly_service_sale =
			calloc( 1, sizeof( HOURLY_SERVICE_SALE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	hourly_service_sale->customer_entity =
		entity_new(
			full_name,
			street_address );

	hourly_service_sale->sale_date_time = sale_date_time;
	hourly_service_sale->service_name = service_name;
	hourly_service_sale->service_description = service_description;

	return hourly_service_sale;
}

char *hourly_service_sale_description_escape(
			char *service_description )
{
	static char escape_description[ 512 ];

	return string_escape_quote(
			escape_description,
			service_description );
}

char *hourly_service_sale_primary_where(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			char *service_description )
{
	char where[ 1024 ];

	sprintf( where,
		 "full_name = '%s' and		"
		 "street_address = '%s' and	"
		 "sale_date_time = '%s' and	"
		 "service_name = '%s' and	"
		 "service_description = '%s'	",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( full_name ),
		 street_address,
		 sale_date_time,
		 service_name,
		 hourly_service_sale_description_escape(
			service_description ) );

	return strdup( where );
}

/* Returns program memory */
/* ---------------------- */
char *hourly_service_sale_select( void )
{
	return
	"full_name,"
	"street_address,"
	"sale_date_time,"
	"service_name,"
	"service_description,"
	"hourly_rate,"
	"discount_amount,"
	"work_hours,"
	"estimated_hours,"
	"net_revenue";
}

HOURLY_SERVICE_SALE *hourly_service_sale_parse( char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char sale_date_time[ 128 ];
	char service_name[ 128 ];
	char service_description[ 128 ];
	char hourly_rate[ 128 ];
	char discount_amount[ 128 ];
	char work_hours[ 128 ];
	char estimated_hours[ 128 ];
	char net_revenue[ 128 ];

	if ( !input || !*input ) return (HOURLY_SERVICE_SALE *)0;

	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( sale_date_time, SQL_DELIMITER, input, 2 );
	piece( service_name, SQL_DELIMITER, input, 3 );
	piece( service_description, SQL_DELIMITER, input, 4 );
	piece( hourly_rate, SQL_DELIMITER, input, 5 );
	piece( discount_amount, SQL_DELIMITER, input, 6 );
	piece( work_hours, SQL_DELIMITER, input, 7 );
	piece( estimated_hours, SQL_DELIMITER, input, 8 );
	piece( net_revenue, SQL_DELIMITER, input, 9 );

	return hourly_service_sale_steady_state(
			strdup( full_name ),
			strdup( street_address ),
			strdup( sale_date_time ),
			strdup( service_name ),
			strdup( service_description ),
			atof( hourly_rate ),
			atof( discount_amount ),
			atof( work_hours )
				/* work_hours_database */,
			atoi( estimated_hours ),
			atof( net_revenue )
				/* net_revenue_database */,
			work_hourly_list(
				full_name,
				street_address,
				sale_date_time,
				service_name,
				service_description ) );
}

FILE *hourly_service_sale_update_open( void )
{
	char sys_string[ 1024 ];
	char *key;

	key =	"full_name,"
		"street_address,"
		"sale_date_time,"
		"service_name,"
		"service_description";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql",
		 HOURLY_SERVICE_SALE_TABLE,
		 key );

	return fopen( sys_string, "w" );
}

void hourly_service_sale_update(
			double hourly_service_sale_work_hours,
			double hourly_service_sale_net_revenue,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			char *service_description )
{
	FILE *update_pipe;

	update_pipe = hourly_service_sale_update_open();

	fprintf(update_pipe,
	 	"%s^%s^%s^%s^%s^work_hours^%.2lf\n",
		full_name,
		street_address,
		sale_date_time,
		service_name,
		service_description,
		hourly_service_sale_work_hours );

	fprintf(update_pipe,
	 	"%s^%s^%s^%s^%s^net_revenue^%.2lf\n",
		full_name,
		street_address,
		sale_date_time,
		service_name,
		service_description,
		hourly_service_sale_net_revenue );

	pclose( update_pipe );
}

HOURLY_SERVICE_SALE *hourly_service_sale_steady_state(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			char *service_description,
			double hourly_rate,
			double discount_amount,
			double work_hours_database,
			int estimated_hours,
			double net_revenue_database,
			LIST *hourly_service_work_list )
{
	HOURLY_SERVICE_SALE *hourly_service_sale;

	hourly_service_sale =
		hourly_service_sale_new(
			full_name,
			street_address,
			sale_date_time,
			service_name,
			service_description );

	hourly_service_sale->hourly_rate = hourly_rate;
	hourly_service_sale->discount_amount = discount_amount;
	hourly_service_sale->work_hours_database = work_hours_database;
	hourly_service_sale->estimated_hours = estimated_hours;
	hourly_service_sale->net_revenue_database = net_revenue_database;

	hourly_service_sale->hourly_service_work_list =
		hourly_service_work_list;

	hourly_service_sale->hourly_service_sale_work_hours =
		work_list_hours(
			hourly_service_sale->
				hourly_service_work_list );

	hourly_service_sale->hourly_service_sale_net_revenue =
		hourly_service_sale_net_revenue(
			hourly_service_sale->hourly_rate,
			hourly_service_sale->hourly_service_sale_work_hours,
			hourly_service_sale->discount_amount );

	return hourly_service_sale;
}

char *hourly_service_sale_sys_string( char *where )
{
	char sys_string[ 1024 ];

	if ( !where ) return (char *)0;

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 hourly_service_sale_select(),
		 HOURLY_SERVICE_SALE_TABLE,
		 where,
		 "service_name,service_description" );

	return strdup( sys_string );
}

LIST *hourly_service_sale_system_list( char *sys_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *hourly_service_sale_list;

	if ( !sys_string ) return (LIST *)0;

	hourly_service_sale_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			hourly_service_sale_list, 
			hourly_service_sale_parse( input ) );
	}

	pclose( input_pipe );
	return hourly_service_sale_list;
}

LIST *hourly_service_sale_list(
			char *full_name,
			char *street_address,
			char *sale_date_time )
{
	if ( !full_name
	||   !street_address
	||   !sale_date_time )
	{
		return (LIST *)0;
	}

	return hourly_service_sale_system_list(
			hourly_service_sale_sys_string(
		 	/* -------------------------- */
		 	/* Safely returns heap memory */
		 	/* -------------------------- */
		 	sale_primary_where(
				full_name,
				street_address,
				sale_date_time ) ) );
}

double hourly_service_sale_total(
			LIST *hourly_service_sale_list )
{
	HOURLY_SERVICE_SALE *hourly_service_sale;
	double total;

	if ( !list_rewind( hourly_service_sale_list ) ) return 0.0;

	total = 0.0;

	do {
		hourly_service_sale = list_get( hourly_service_sale_list );

		total += hourly_service_sale->hourly_service_sale_net_revenue;

	} while( list_next( hourly_service_sale_list ) );

	return total;
}

double hourly_service_sale_net_revenue(
			double hourly_rate,
			double work_hours,
			double discount_amount )
{
	return ( hourly_rate * work_hours ) - discount_amount;
}

HOURLY_SERVICE_SALE *hourly_service_sale_seek(
			LIST *hourly_service_sale_list,
			char *service_name,
			char *service_description )
{
	HOURLY_SERVICE_SALE *hourly_service_sale;

	if ( !list_rewind( hourly_service_sale_list ) )
		return (HOURLY_SERVICE_SALE *)0;

	do {
		hourly_service_sale = list_get( hourly_service_sale_list );

		if ( timlib_strcmp(
			hourly_service_sale->service_name,
			service_name ) == 0
		&&   timlib_strcmp(
			hourly_service_sale->service_description,
			service_description ) == 0 )
		{
			return hourly_service_sale;
		}

	} while( list_next( hourly_service_sale_list ) );

	return (HOURLY_SERVICE_SALE *)0;
}

