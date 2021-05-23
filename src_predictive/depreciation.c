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
#include "journal.h"
#include "predictive.h"
#include "transaction.h"
#include "entity.h"
#include "fixed_asset.h"
#include "purchase.h"
#include "depreciation.h"

double depreciation_amount(
			enum depreciation_method depreciation_method,
			char *service_placement_date,
			char *prior_depreciation_date,
			char *depreciation_date,
			double cost_basis,
			int units_produced_current,
			int estimated_residual_value,
			int estimated_useful_life_years,
			int estimated_useful_life_units,
			int declining_balance_n,
			double prior_accumulated_depreciation )
{
	if ( depreciation_method == not_depreciated )
	{
		return 0.0;
	}
	else
	if ( depreciation_method == straight_line )
	{
		return depreciation_straight_line(
			service_placement_date,
			prior_depreciation_date,
			depreciation_date,
			cost_basis,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_accumulated_depreciation );
	}
	else
	if ( depreciation_method == units_of_production )
	{
		return depreciation_units_of_production(
			cost_basis,
			estimated_residual_value,
			estimated_useful_life_units,
			units_produced_current,
			prior_accumulated_depreciation );
	}
	else
	if ( depreciation_method == double_declining_balance )
	{
		return depreciation_double_declining_balance(
			service_placement_date,
			prior_depreciation_date,
			depreciation_date,
			cost_basis,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_accumulated_depreciation );
	}
	else
	if ( depreciation_method == n_declining_balance )
	{
		return depreciation_n_declining_balance(
			service_placement_date,
			prior_depreciation_date,
			depreciation_date,
			cost_basis,
			estimated_residual_value,
			estimated_useful_life_years,
			declining_balance_n,
			prior_accumulated_depreciation );
	}
	else
	if ( depreciation_method == sum_of_years_digits)
	{
		return depreciation_sum_of_years_digits(
			service_placement_date,
			prior_depreciation_date,
			depreciation_date,
			cost_basis,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_accumulated_depreciation );
	}
	else
	{
		return 0.0;
	}
}

double depreciation_units_of_production(
			double cost_basis,
			int estimated_residual_value,
			int estimated_useful_life_units,
			int units_produced_current,
			double prior_accumulated_depreciation )
{
	double depreciation_rate_per_unit = 0.0;
	double depreciation_base;
	double depreciation_amount;

	depreciation_base = cost_basis - (double)estimated_residual_value;

	if ( estimated_useful_life_units )
	{
		depreciation_rate_per_unit =
			depreciation_base /
			(double)estimated_useful_life_units;
	}

	depreciation_amount = 
		depreciation_rate_per_unit *
		(double)units_produced_current;


	if (	prior_accumulated_depreciation + depreciation_amount >
		depreciation_base )
	{
		depreciation_amount =
			depreciation_base -
			prior_accumulated_depreciation;
	}

	return depreciation_amount;
}

double depreciation_straight_line(
			char *service_placement_date,
			char *prior_depreciation_date,
			char *depreciation_date,
			double cost_basis,
			int estimated_residual_value,
			int estimated_useful_life_years,
			double prior_accumulated_depreciation )
{
	double depreciation_base;
	double annual_depreciation_amount;
	double fraction_of_year;
	double depreciation_amount;

	if ( !service_placement_date || !*service_placement_date ) return 0.0;

	fraction_of_year =
		depreciation_fraction_of_year(
			service_placement_date,
			prior_depreciation_date,
			depreciation_date );

	depreciation_base = cost_basis - (double)estimated_residual_value;

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
	if (	prior_accumulated_depreciation + depreciation_amount >
		depreciation_base )
	{
		depreciation_amount =
			depreciation_base -
			prior_accumulated_depreciation;
	}

	return depreciation_amount;
}

double depreciation_sum_of_years_digits(
			char *service_placement_date,
			char *prior_depreciation_date,
			char *depreciation_date,
			double cost_basis,
			int estimated_residual_value,
			int estimated_useful_life_years,
			double prior_accumulated_depreciation )
{
	double denominator;
	double depreciation_base;
	double annual_depreciation_amount;
	double fraction_of_year;
	double depreciation_fraction;
	double depreciation_amount;
	int current_age_years;
	int remaining_life_years;

	if ( !service_placement_date || !*service_placement_date ) return 0.0;
	if ( estimated_useful_life_years <= 0 ) return 0.0;

	depreciation_base = cost_basis - (double)estimated_residual_value;

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
			service_placement_date,
			prior_depreciation_date,
			depreciation_date );

	depreciation_amount = annual_depreciation_amount * fraction_of_year;

	/* If depreciation_date is past the useful life. */
	/* --------------------------------------------- */
	if (	prior_accumulated_depreciation + depreciation_amount >
		depreciation_base )
	{
		depreciation_amount =
			depreciation_base -
			prior_accumulated_depreciation;
	}

	return depreciation_amount;
}

double depreciation_n_declining_balance(
			char *service_placement_date,
			char *prior_depreciation_date,
			char *depreciation_date,
			double cost_basis,
			int estimated_residual_value,
			int estimated_useful_life_years,
			int declining_balance_n,
			double prior_accumulated_depreciation )
{
	double annual_depreciation_amount;
	double fraction_of_year;
	double depreciation_amount;
	double book_value;
	double maximum_depreciation;

	if ( !service_placement_date || !*service_placement_date ) return 0.0;

	book_value = cost_basis - prior_accumulated_depreciation;

	fraction_of_year =
		depreciation_fraction_of_year(
			service_placement_date,
			prior_depreciation_date,
			depreciation_date );

	annual_depreciation_amount =
		( book_value * (double)declining_balance_n ) /
			       (double)estimated_useful_life_years;

	depreciation_amount = annual_depreciation_amount * fraction_of_year;

	maximum_depreciation = book_value - (double)estimated_residual_value;

	if ( depreciation_amount > maximum_depreciation )
		depreciation_amount = maximum_depreciation;

	return depreciation_amount;
}

double depreciation_double_declining_balance(
			char *service_placement_date,
			char *prior_depreciation_date,
			char *depreciation_date,
			double cost_basis,
			int estimated_residual_value,
			int estimated_useful_life_years,
			double prior_accumulated_depreciation )
{
	if ( !service_placement_date || !*service_placement_date ) return 0.0;

	return 
		depreciation_n_declining_balance(
			service_placement_date,
			prior_depreciation_date,
			depreciation_date,
			cost_basis,
			estimated_residual_value,
			estimated_useful_life_years,
			2 /* declining_balance_n */,
			prior_accumulated_depreciation );
}

DEPRECIATION *depreciation_new(
			char *asset_name,
			char *serial_label,
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
	depreciation->serial_label = serial_label;
	depreciation->depreciation_date = depreciation_date;

	return depreciation;
}

DEPRECIATION *depreciation_parse( char *input )
{
	char asset_name[ 128 ];
	char serial_label[ 128 ];
	char depreciation_date[ 128 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	char piece_buffer[ 1024 ];
	DEPRECIATION *depreciation;

	if ( !input || !*input ) return (DEPRECIATION *)0;

	piece( asset_name, SQL_DELIMITER, input, 0 );
	piece( serial_label, SQL_DELIMITER, input, 1 );
	piece( depreciation_date, SQL_DELIMITER, input, 2 );

	depreciation =
		depreciation_new(
			strdup( asset_name ),
			strdup( serial_label ),
			strdup( depreciation_date ) );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	depreciation->units_produced_current = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	depreciation->depreciation_amount = atof( piece_buffer );

	piece( full_name, SQL_DELIMITER, input, 5 );
	piece( street_address, SQL_DELIMITER, input, 6 );

	depreciation->self_entity =
		entity_new(
			strdup( full_name ),
			strdup( street_address ) );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	depreciation->transaction_date_time = strdup( piece_buffer );

	return depreciation;
}

char *depreciation_primary_where(
			char *asset_name,
			char *serial_label,
			char *depreciation_date )
{
	static char where[ 256 ];

	sprintf(where,
		"asset_name = '%s' and		"
		"serial_label = '%s' and	"
		"depreciation_date = '%s'	",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		fixed_asset_name_escape( asset_name ),
		serial_label,
		depreciation_date );

	return strdup( where );
}

DEPRECIATION *depreciation_fetch(
			char *asset_name,
			char *serial_label,
			char *depreciation_date )
{
	char sys_string[ 1024 ];

	if ( !asset_name
	||   !serial_label )
	{
		return (DEPRECIATION *)0;
	}

	sprintf(sys_string,
		"select.sh '*' %s \"%s\" ''",
		 DEPRECIATION_TABLE,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 depreciation_primary_where(
			asset_name,
			serial_label,
			depreciation_date ) );

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

char *depreciation_system_string(
			char *where )
{
	char system_string[ 1024 ];

	if ( !where ) return (char *)0;

	sprintf(system_string,
		"select.sh '*' %s \"%s\" depreciation_date",
		DEPRECIATION_TABLE,
		where );

	return strdup( system_string );
}

LIST *depreciation_list_fetch(
			char *asset_name,
			char *serial_label )
{
	return depreciation_system_list(
			depreciation_system_string(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				fixed_asset_primary_where(
					asset_name,
					serial_label ) ) );
}

FILE *depreciation_delete_open( void )
{
	char system_string[ 1024 ];
	char *key;

	key =	"asset_name,"
		"serial_label,"
		"depreciation_date";

	sprintf( system_string,
		 "delete_statement table=%s field=%s delimiter='%c'	|"
		 "tee_appaserver_error.sh				|"
		 "sql							 ",
		 DEPRECIATION_TABLE,
		 key,
		 '^' );

	return popen( system_string, "w" );
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

double depreciation_accumulated_depreciation(
			double prior_accumulated_depreciation,
			double depreciation_amount )
{
	return prior_accumulated_depreciation + depreciation_amount;
}

TRANSACTION *depreciation_transaction(
			char *full_name,
			char *street_address,
			char *depreciation_date,
			double depreciation_amount,
			char *account_depreciation_expense,
			char *account_accumulated_depreciation )
{
	TRANSACTION *transaction;
	char *transaction_date_time;

	if ( !depreciation_amount ) return (TRANSACTION *)0;

	if ( ! ( transaction =
			transaction_new(
				full_name,
				street_address,
				( transaction_date_time =
					predictive_transaction_date_time(
						depreciation_date ) ) ) ) );
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: transaction_new(%s,%s,%s) returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 full_name,
			 street_address,
			 depreciation_date );
		exit( 1 );
	}

	transaction->transaction_amount = depreciation_amount;

	transaction->journal_list =
		journal_binary_list(
			full_name,
			street_address,
			transaction_date_time,
			transaction->transaction_amount,
			account_depreciation_expense
				/* debit_account */,
			account_accumulated_depreciation
				/* credit_account */ );

	return transaction;
}

LIST *depreciation_transaction_list(
			LIST *depreciation_list )
{
	DEPRECIATION *depreciation;
	LIST *transaction_list;

	if ( !list_rewind( depreciation_list ) ) return (LIST *)0;

	transaction_list = list_new();

	do {
		depreciation = list_get( depreciation_list );

		if ( depreciation->depreciation_transaction )
		{
			list_set(
				transaction_list,
				depreciation->depreciation_transaction );
		}

	} while ( list_next( depreciation_list ) );

	return transaction_list;
}

FILE *depreciation_insert_open( void )
{
	char sys_string[ 1024 ];
	char *field;

	field =	"asset_name,"
		"serial_label,"
		"depreciation_date,"
		"depreciation_amount,"
		"transaction_date_time";

	sprintf( sys_string,
		 "insert_statement table=%s field=%s delimiter='^'	|"
		 "sql 2>&1						 ",
		 "depreciation",
		 field );

	return popen( sys_string, "w" );
}

void depreciation_insert(
			FILE *insert_pipe,
			char *asset_name,
			char *serial_label,
			char *depreciation_date,
			int units_produced_current,
			double depreciation_amount,
			char *transaction_date_time )
{
	if ( !asset_name
	||   !serial_label
	||   !depreciation_date
	||   !depreciation_amount
	||   !transaction_date_time )
	{
		return;
	}

	fprintf(insert_pipe,
		"%s^%s^%s^%d^%.2lf^%s\n",
		asset_name,
		serial_label,
		depreciation_date,
		units_produced_current,
		depreciation_amount,
		transaction_date_time );
}

void depreciation_list_insert(
			LIST *depreciation_list )
{
	DEPRECIATION *depreciation;
	FILE *insert_pipe;

	if ( !list_rewind( depreciation_list ) ) return;

	insert_pipe = depreciation_insert_open();

	do {
		depreciation =
			list_get(
				depreciation_list );

		if ( !depreciation->depreciation_transaction )
		{
			continue;
		}

		depreciation_insert(
			insert_pipe,
			depreciation->asset_name,
			depreciation->serial_label,
			depreciation->depreciation_date,
			depreciation->units_produced_current,
			depreciation->depreciation_amount,
			depreciation->
				depreciation_transaction->
				transaction_date_time );

	} while ( list_next( depreciation_list ) );

	pclose( insert_pipe );
}

char *depreciation_prior_depreciation_date(
			char *asset_name,
			char *serial_label )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"select.sh \"%s\" %s \"%s\" ''",
		"max( depreciation_date )",
		DEPRECIATION_TABLE,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		fixed_asset_primary_where(
			asset_name,
			serial_label ) );

	return pipe2string( sys_string );
}

DEPRECIATION *depreciation_seek(
			LIST *depreciation_list,
			char *depreciation_date )
{
	DEPRECIATION *depreciation;

	if ( !list_rewind( depreciation_list ) ) return (DEPRECIATION *)0;

	do {
		depreciation =
			list_get(
				depreciation_list );

		if ( timlib_strcmp(
			depreciation->depreciation_date,
			depreciation_date ) == 0 )
		{
			return depreciation;
		}
	} while ( list_next( depreciation_list ) );

	return (DEPRECIATION *)0;
}

char *depreciation_method_string(
	 		enum depreciation_method depreciation_method )
{
	if ( depreciation_method == straight_line )
		return STRAIGHT_LINE;
	else
	if ( depreciation_method == double_declining_balance )
		return DOUBLE_DECLINING_BALANCE;
	else
	if ( depreciation_method == n_declining_balance )
		return N_DECLINING_BALANCE;
	else
	if ( depreciation_method == sum_of_years_digits )
		return SUM_OF_YEARS_DIGITS;
	else
	if ( depreciation_method == units_of_production )
		return UNITS_OF_PRODUCTION;
	else
		return NOT_DEPRECIATED;
}

enum depreciation_method depreciation_method_resolve(
			char *depreciation_method_string )
{
	if ( !depreciation_method_string )
	{
		return not_depreciated;
	}
	else
	if ( timlib_strcmp(
			depreciation_method_string,
			DOUBLE_DECLINING_BALANCE ) == 0 )
	{
		return double_declining_balance;
	}
	else
	if ( timlib_strcmp(
			depreciation_method_string,
			N_DECLINING_BALANCE ) == 0 )
	{
		return n_declining_balance;
	}
	else
	if ( timlib_strcmp(
			depreciation_method_string,
			STRAIGHT_LINE ) == 0 )
	{
		return straight_line;
	}
	else
	if ( timlib_strcmp(
			depreciation_method_string,
			SUM_OF_YEARS_DIGITS ) == 0 )
	{
		return sum_of_years_digits;
	}
	else
	if ( timlib_strcmp(
			depreciation_method_string,
			UNITS_OF_PRODUCTION ) == 0 )
	{
		return units_of_production;
	}
	else
	{
		return not_depreciated;
	}
}

DEPRECIATION *depreciation_evaluate(
			char *asset_name,
			char *serial_label,
			enum depreciation_method depreciation_method,
			char *service_placement_date,
			char *prior_depreciation_date,
			char *depreciation_date,
			double cost_basis,
			int units_produced_so_far,
			int estimated_residual_value,
			int estimated_useful_life_years,
			int estimated_useful_life_units,
			int declining_balance_n,
			double prior_accumulated_depreciation )
{
	DEPRECIATION *depreciation;
	double amount;
	int units_produced_current = 0;

	amount =
		depreciation_amount(
			depreciation_method,
			service_placement_date,
			prior_depreciation_date,
			depreciation_date,
			cost_basis,
			(depreciation_method == units_of_production)
			 ? ( units_produced_current =
				depreciation_units_produced_current(
					units_produced_so_far,
					depreciation_units_produced_total(
						asset_name,
						serial_label ) ) )
			 : 0,
			estimated_residual_value,
			estimated_useful_life_years,
			estimated_useful_life_units,
			declining_balance_n,
			prior_accumulated_depreciation );

	if ( timlib_dollar_virtually_same(
		amount,
		0.0 ) )
	{
		return (DEPRECIATION *)0;
	}

	depreciation =
		depreciation_new(
			asset_name,
			serial_label,
			depreciation_date );

	depreciation->depreciation_amount = amount;
	depreciation->units_produced_current = units_produced_current;

	depreciation->depreciation_accumulated_depreciation =
		depreciation_accumulated_depreciation(
			prior_accumulated_depreciation,
			depreciation->depreciation_amount );

	return depreciation;
}

double depreciation_fraction_of_year(
			char *service_placement_date,
			char *prior_depreciation_date,
			char *depreciation_date_string )
{
	char *earlier_date_string;
	int days_between;
	DATE *earlier_date;
	DATE *depreciation_date;

	if ( !service_placement_date
	||   !*service_placement_date
	||   !depreciation_date_string
	||   !*depreciation_date_string )
	{
		return 0.0;
	}

	if ( !prior_depreciation_date || !*prior_depreciation_date )
	{
		earlier_date_string = service_placement_date;
	}
	else
	{
		earlier_date_string = prior_depreciation_date;
	}

	earlier_date =
		date_yyyy_mm_dd_new(
			earlier_date_string );

	depreciation_date = date_yyyy_mm_dd_new( depreciation_date_string );

	days_between =
		date_subtract_days(
			depreciation_date /* later_date */,
			earlier_date  );

	return 	(double)days_between /
		(double)date_days_in_year(
				date_year(
					earlier_date ) );
}

int depreciation_units_produced_total(
			char *asset_name,
			char *serial_label )
{
	char system_string[ 1024 ];
	char *results;

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" ''",
		"sum( units_produced )",
		DEPRECIATION_TABLE,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		fixed_asset_primary_where(
			asset_name,
			serial_label ) );

	if ( ( results = string_pipe_fetch( system_string ) ) )
		return atoi( results );
	else
		return 0;
}

int depreciation_units_produced_current(
			int units_produced_so_far,
			int units_produced_total )
{
	return units_produced_so_far - units_produced_total;
}

