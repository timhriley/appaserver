/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/depreciation.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "piece.h"
#include "date.h"
#include "sql.h"
#include "transaction.h"
#include "equipment_purchase.h"
#include "depreciation.h"

double depreciation_amount(
			char *depreciation_method,
			double equipment_cost,
			int estimated_residual_value,
			int estimated_useful_life_years,
			int estimated_useful_life_units,
			int declining_balance_n,
			char *prior_depreciation_date,
			char *depreciation_date,
			double finance_accumulated_depreciation,
			char *service_placement_date,
			int units_produced )
{
	if ( !depreciation_method || !*depreciation_method )
	{
		/* Land is not depreciated. */
		/* ------------------------ */
		return 0.0;
	}
	else
	if ( strcmp( depreciation_method, "straight_line" ) == 0 )
	{
		return depreciation_straight_line(
			equipment_cost,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_depreciation_date,
			depreciation_date,
			finance_accumulated_depreciation );
	}
	else
	if ( strcmp( depreciation_method, "units_of_production" ) == 0 )
	{
		return depreciation_units_of_production(
			equipment_cost,
			estimated_residual_value,
			estimated_useful_life_units,
			units_produced,
			finance_accumulated_depreciation );
	}
	else
	if ( strcmp( depreciation_method, "double_declining_balance" ) == 0 )
	{
		return depreciation_double_declining_balance(
			equipment_cost,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_depreciation_date,
			depreciation_date,
			finance_accumulated_depreciation );
	}
	else
	if ( strcmp( depreciation_method, "n_declining_balance" ) == 0 )
	{
		return depreciation_n_declining_balance(
			equipment_cost,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_depreciation_date,
			depreciation_date,
			finance_accumulated_depreciation,
			declining_balance_n );
	}
	else
	if ( strcmp( depreciation_method, "sum_of_years_digits" ) == 0 )
	{
		return depreciation_sum_of_years_digits(
			equipment_cost,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_depreciation_date,
			depreciation_date,
			finance_accumulated_depreciation,
			service_placement_date );
	}
	else
	{
		return 0.0;
	}
}

double depreciation_units_of_production(
			double equipment_cost,
			int estimated_residual_value,
			int estimated_useful_life_units,
			int units_produced,
			double finance_accumulated_depreciation )
{
	double depreciation_rate_per_unit = 0.0;
	double depreciation_base;
	double depreciation_amount;

	depreciation_base = equipment_cost - (double)estimated_residual_value;

	if ( estimated_useful_life_units )
	{
		depreciation_rate_per_unit =
			depreciation_base /
			(double)estimated_useful_life_units;
	}

	depreciation_amount = 
		depreciation_rate_per_unit *
		(double)units_produced;


	if (	finance_accumulated_depreciation + depreciation_amount >
		depreciation_base )
	{
		depreciation_amount =
			depreciation_base -
			finance_accumulated_depreciation;
	}

	return depreciation_amount;
}

double depreciation_straight_line(
			double equipment_cost,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date,
			char *depreciation_date,
			double finance_accumulated_depreciation )
{
	double depreciation_base;
	double annual_depreciation_amount;
	double fraction_of_year = 0.0;
	double depreciation_amount;

	if ( prior_depreciation_date
	&&   *prior_depreciation_date )
	{
		fraction_of_year =
			depreciation_fraction_of_year(
				prior_depreciation_date,
				depreciation_date );
	}

	depreciation_base = equipment_cost - (double)estimated_residual_value;

	if ( estimated_useful_life_years )
	{
		annual_depreciation_amount =
			depreciation_base /
			(double) estimated_useful_life_years;
	}
	else
	{
		annual_depreciation_amount = 0.0;
	}

	depreciation_amount = annual_depreciation_amount * fraction_of_year;

	/* If depreciation_date is past the useful life. */
	/* --------------------------------------------- */
	if (	finance_accumulated_depreciation + depreciation_amount >
		depreciation_base )
	{
		depreciation_amount =
			depreciation_base -
			finance_accumulated_depreciation;
	}

	return depreciation_amount;
}

double depreciation_sum_of_years_digits(
			double equipment_cost,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date,
			char *depreciation_date,
			double finance_accumulated_depreciation,
			char *service_placement_date )
{
	double denominator;
	double depreciation_base;
	double annual_depreciation_amount;
	double fraction_of_year;
	double depreciation_fraction;
	double depreciation_amount;
	int current_age_years;
	int remaining_life_years;

	if ( estimated_useful_life_years <= 0 ) return 0.0;

	depreciation_base = equipment_cost - (double)estimated_residual_value;

	denominator =
		( (double)( estimated_useful_life_years *
			  ( estimated_useful_life_years + 1 ) ) )
			  / 2.0;

	current_age_years =
		date_years_between(
			depreciation_date,
			service_placement_date );

	remaining_life_years = estimated_useful_life_years - current_age_years;

	if ( remaining_life_years < 0 ) remaining_life_years = 0;

	depreciation_fraction =
		(double)remaining_life_years /
		(double)denominator;

	annual_depreciation_amount =
		depreciation_base *
		depreciation_fraction;

	fraction_of_year =
		depreciation_fraction_of_year(
			prior_depreciation_date,
			depreciation_date );

	depreciation_amount = annual_depreciation_amount * fraction_of_year;

	/* If depreciation_date is past the useful life. */
	/* --------------------------------------------- */
	if (	finance_accumulated_depreciation + depreciation_amount >
		depreciation_base )
	{
		depreciation_amount =
			depreciation_base -
			finance_accumulated_depreciation;
	}

	return depreciation_amount;
}

double depreciation_n_declining_balance(
			double equipment_cost,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date,
			char *depreciation_date,
			double finance_accumulated_depreciation,
			int n )
{
	double annual_depreciation_amount;
	double fraction_of_year;
	double depreciation_amount;
	double book_value;
	double maximum_depreciation;

	book_value = equipment_cost - finance_accumulated_depreciation;

	fraction_of_year =
		depreciation_fraction_of_year(
			prior_depreciation_date,
			depreciation_date );

	annual_depreciation_amount =
		( book_value * (double)n ) /
			       (double)estimated_useful_life_years;

	depreciation_amount = annual_depreciation_amount * fraction_of_year;

	maximum_depreciation = book_value - (double)estimated_residual_value;

	if ( depreciation_amount > maximum_depreciation )
		depreciation_amount = maximum_depreciation;

	return depreciation_amount;
}

double depreciation_fraction_of_year(
			char *prior_depreciation_date,
			char *date_string )
{
	int days_between;
	DATE *prior_date;
	DATE *date;

	if ( !prior_depreciation_date
	||   !*prior_depreciation_date
	||   !date_string
	||   !*date_string )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: empty date = [prior=%s or date=%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 prior_depreciation_date,
			 date_string );
		return 0.0;
	}

	prior_date =
		date_yyyy_mm_dd_new(
			prior_depreciation_date );

	date = date_yyyy_mm_dd_new( date_string );

	days_between =
		date_subtract_days(
			date /* later_date */,
			prior_date /* earlier_date */ );

	return 	(double)days_between /
		(double)date_get_days_in_year(
				date_get_year(
					prior_date ) );
}

double depreciation_double_declining_balance(
			double equipment_cost,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date,
			char *depreciation_date,
			double finance_accumulated_depreciation )
{
	return 
		depreciation_n_declining_balance(
			equipment_cost,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_depreciation_date,
			depreciation_date,
			finance_accumulated_depreciation,
			2 /* n */ );
}

DEPRECIATION *depreciation_new(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *depreciation_date )
{
	DEPRECIATION *depreciation;

	if ( ! ( depreciation = calloc( 1, sizeof( DEPRECIATION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	depreciation->asset_name = asset_name;
	depreciation->serial_number = serial_number;

	depreciation->vendor_entity =
		entity_new(
			full_name,
			street_address );

	depreciation->purchase_date_time = purchase_date_time;
	depreciation->depreciation_date = depreciation_date;

	return depreciation;
}

char *depreciation_select( void )
{
	return	"asset_name,"
		"serial_number,"
		"full_name,"
		"street_address,"
		"purchase_date_time,"
		"depreciation_date,"
		"depreciation_amount,"
		"accumulated_depreciation,"
		"transaction_date_time";
}

DEPRECIATION *depreciation_parse( char *input )
{
	char asset_name[ 128 ];
	char serial_number[ 128 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	char purchase_date_time[ 128 ];
	char depreciation_date_time[ 128 ];
	char piece_buffer[ 1024 ];
	DEPRECIATION *depreciation;

	if ( !input ) return (DEPRECIATION *)0;

	piece( asset_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );
	piece( full_name, SQL_DELIMITER, input, 2 );
	piece( street_address, SQL_DELIMITER, input, 3 );
	piece( purchase_date_time, SQL_DELIMITER, input, 4 );
	piece( depreciation_date_time, SQL_DELIMITER, input, 5 );

	depreciation =
		depreciation_new(
			strdup( asset_name ),
			strdup( serial_number ),
			strdup( full_name ),
			strdup( street_address ),
			strdup( purchase_date_time ),
			strdup( depreciation_date_time ) );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	depreciation->depreciation_amount = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	depreciation->depreciation_accumulated_depreciation =
		atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 8 );
	if ( *piece_buffer )
	{
		depreciation->depreciation_transaction =
			transaction_fetch(
				depreciation->
					vendor_entity->
					full_name,
				depreciation->
					vendor_entity->
					street_address,
				piece_buffer /* transaction_date_time */ );
	}

	return depreciation;
}

char *depreciation_primary_where(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *depreciation_date_string )
{
	char where[ 1024 ];

	sprintf( where,
		 "asset_name = '%s' and		"
		 "serial_number = '%s' and	"
		 "full_name = '%s' and		"
		 "street_address = '%s' and	"
		 "purchase_date_time = '%s'	"
		 "depreciation_date = '%s'	",
		 asset_name,
		 serial_number,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( full_name ),
		 street_address,
		 purchase_date_time,
		 depreciation_date_string );

	return strdup( where );
}

DEPRECIATION *depreciation_fetch(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *depreciation_date_string )
{
	char sys_string[ 1024 ];

	if ( !asset_name
	||   !serial_number
	||   !full_name
	||   !street_address )
	{
		return (DEPRECIATION *)0;
	}

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 depreciation_select(),
		 DEPRECIATION_TABLE_NAME,
		 /* -------------------------- */
		 /* Safely returns heap memory */
		 /* -------------------------- */
		 depreciation_primary_where(
			asset_name,
			serial_number,
			full_name,
			street_address,
			purchase_date_time,
			depreciation_date_string ) );

	return depreciation_parse( pipe2string( sys_string ) );
}

LIST *depreciation_system_list( char *sys_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *depreciation_list;

	depreciation_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(	depreciation_list, 
				depreciation_parse( input ) );
	}

	pclose( input_pipe );
	return depreciation_list;
}

char *depreciation_sys_string( char *where )
{
	char sys_string[ 1024 ];

	if ( !where ) return (char *)0;

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 depreciation_select(),
		 DEPRECIATION_TABLE_NAME,
		 where,
		 "depreciation_date" );

	return strdup( sys_string );
}

LIST *depreciation_list_fetch( char *where )
{
	if ( !where ) return (LIST *)0;

	return depreciation_system_list(
			depreciation_sys_string(
				where ) );
}

LIST *depreciation_list(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	if ( !asset_name
	||   !serial_number
	||   !full_name
	||   !street_address )
	{
		return (LIST *)0;
	}

	return depreciation_fetch_list(
		 /* -------------------------- */
		 /* Safely returns heap memory */
		 /* -------------------------- */
		 equipment_purchase_primary_where(
			asset_name,
			serial_number,
			full_name,
			street_address,
			purchase_date_time ) );
}

FILE *depreciation_update_open( void )
{
	return (FILE *)0;
}

void depreciation_update(
			double depreciation_amount,
			char *transaction_date_time,
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *depreciation_date_time )
{
}

double depreciation_amount_total(
			LIST *depreciation_list )
{
	DEPRECIATION *depreciation;
	double total;

	if ( !list_rewind( depreciation_list ) ) return 0.0;

	total = 0.0;

	do {
		depreciation = list_get( depreciation_list );
		total += depreciation->depreciation_amount;

	} while ( list_next( depreciation_list ) );

	return total;
}

LIST *depreciation_fetch_list( char *where )
{
	if ( !where ) return (LIST *)0;

	return depreciation_system_list(
			depreciation_sys_string(
				where ) );
}

