/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_service_sale.c			*/
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
#include "fixed_service_sale.h"

FIXED_SERVICE_SALE *fixed_service_sale_new(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name )
{
	FIXED_SERVICE_SALE *fixed_service_sale;

	if ( ! ( fixed_service_sale =
			calloc( 1, sizeof( FIXED_SERVICE_SALE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	fixed_service_sale->customer_entity =
		entity_new(
			full_name,
			street_address );

	fixed_service_sale->sale_date_time = sale_date_time;
	fixed_service_sale->service_name = service_name;

	return fixed_service_sale;
}

/* Returns program memory */
/* ---------------------- */
char *fixed_service_sale_select( void )
{
	return
	"full_name,"
	"street_address,"
	"sale_date_time,"
	"service_name,"
	"fixed_price,"
	"estimated_hours,"
	"work_hours";
}

char *fixed_service_sale_primary_where(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name )
{
	char where[ 1024 ];

	sprintf( where,
		 "full_name = '%s' and		"
		 "street_address = '%s' and	"
		 "sale_date_time = '%s' and	"
		 "service_name = '%s'		",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( full_name ),
		 street_address,
		 sale_date_time,
		 service_name );

	return strdup( where );
}

FIXED_SERVICE_SALE *fixed_service_sale_parse( char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char sale_date_time[ 128 ];
	char service_name[ 128 ];
	char fixed_price[ 128 ];
	char estimated_hours[ 128 ];
	char work_hours[ 128 ];

	if ( !input || !*input ) return (FIXED_SERVICE_SALE *)0;

	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( sale_date_time, SQL_DELIMITER, input, 2 );
	piece( service_name, SQL_DELIMITER, input, 3 );
	piece( fixed_price, SQL_DELIMITER, input, 4 );
	piece( estimated_hours, SQL_DELIMITER, input, 5 );
	piece( work_hours, SQL_DELIMITER, input, 6 );

	return fixed_service_sale_steady_state(
			strdup( full_name ),
			strdup( street_address ),
			strdup( sale_date_time ),
			strdup( service_name ),
			atof( fixed_price ),
			atoi( estimated_hours ),
			atoi( work_hours ) /* database */,
			work_fixed_list(
				full_name,
				street_address,
				sale_date_time,
				service_name ) );
}

FILE *fixed_service_sale_update_open( void )
{
	char sys_string[ 1024 ];
	char *key;

	key =	"full_name,"
		"street_address,"
		"sale_date_time,"
		"service_name";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql",
		 FIXED_SERVICE_SALE_TABLE,
		 key );

	return fopen( sys_string, "w" );
}

void fixed_service_sale_update(
			double fixed_service_sale_work_hours,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name )
{
	FILE *update_pipe;

	update_pipe = fixed_service_sale_update_open();

	fprintf(update_pipe,
	 	"%s^%s^%s^%s^work_hours^%.2lf\n",
		full_name,
		street_address,
		sale_date_time,
		service_name,
		fixed_service_sale_work_hours );

	pclose( update_pipe );
}

FIXED_SERVICE_SALE *fixed_service_sale_steady_state(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			double fixed_price,
			int estimated_hours,
			double work_hours_database,
			LIST *fixed_service_work_list )
{
	FIXED_SERVICE_SALE *fixed_service_sale;

	fixed_service_sale =
		fixed_service_sale_new(
			full_name,
			street_address,
			sale_date_time,
			service_name );

	fixed_service_sale->fixed_price = fixed_price;
	fixed_service_sale->estimated_hours = estimated_hours;
	fixed_service_sale->work_hours_database = work_hours_database;

	fixed_service_sale->fixed_service_work_hours =
		work_list_hours(
			fixed_service_work_list );

	return fixed_service_sale;
}

char *fixed_service_sale_sys_string( char *where )
{
	char sys_string[ 1024 ];

	if ( !where ) return (char *)0;

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 fixed_service_sale_select(),
		 FIXED_SERVICE_SALE_TABLE,
		 where,
		 "service_name" );

	return strdup( sys_string );
}

LIST *fixed_service_sale_system_list( char *sys_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *fixed_service_sale_list;

	if ( !sys_string ) return (LIST *)0;

	fixed_service_sale_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			fixed_service_sale_list, 
			fixed_service_sale_parse( input ) );
	}

	pclose( input_pipe );
	return fixed_service_sale_list;
}

LIST *fixed_service_sale_list(
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

	return fixed_service_sale_system_list(
			fixed_service_sale_sys_string(
		 	/* -------------------------- */
		 	/* Safely returns heap memory */
		 	/* -------------------------- */
		 	sale_primary_where(
				full_name,
				street_address,
				sale_date_time ) ) );
}

double fixed_service_sale_total(
			LIST *fixed_service_sale_list )
{
	FIXED_SERVICE_SALE *fixed_service_sale;
	double total;

	if ( !list_rewind( fixed_service_sale_list ) ) return 0.0;

	total = 0.0;

	do {
		fixed_service_sale = list_get( fixed_service_sale_list );

		total += fixed_service_sale->fixed_service_sale_net_revenue;

	} while( list_next( fixed_service_sale_list ) );

	return total;
}

double fixed_service_sale_net_revenue(
			double fixed_price,
			double discount_amount )
{
	return fixed_price - discount_amount;
}

FIXED_SERVICE_SALE *fixed_service_sale_seek(
			LIST *fixed_service_sale_list,
			char *service_name )
{
	FIXED_SERVICE_SALE *fixed_service_sale;

	if ( !list_rewind( fixed_service_sale_list ) )
		return (FIXED_SERVICE_SALE *)0;

	do {
		fixed_service_sale = list_get( fixed_service_sale_list );

		if ( timlib_strcmp(
			fixed_service_sale->service_name,
			service_name ) == 0 )
		{
			return fixed_service_sale;
		}

	} while( list_next( fixed_service_sale_list ) );

	return (FIXED_SERVICE_SALE *)0;
}

