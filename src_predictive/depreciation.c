/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/depreciation.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "piece.h"
#include "date.h"
#include "sql.h"
#include "appaserver_error.h"
#include "transaction.h"
#include "journal.h"
#include "predictive.h"
#include "entity.h"
#include "fixed_asset_purchase.h"
#include "purchase.h"
#include "depreciation.h"

double depreciation_amount(
		enum depreciation_method depreciation_method,
		char *service_placement_date,
		char *prior_depreciation_date,
		char *depreciation_date,
		double cost_basis,
		int units_produced,
		int estimated_residual_value,
		int estimated_useful_life_years,
		int estimated_useful_life_units,
		int declining_balance_n,
		double prior_accumulated_depreciation )
{
	double amount = {0};

	if ( depreciation_method == not_depreciated )
	{
		/* amount = 0.0; */
	}
	else
	if ( depreciation_method == straight_line )
	{
		amount =
			depreciation_straight_line(
				service_placement_date,
				prior_depreciation_date,
				depreciation_date,
				cost_basis,
				estimated_residual_value,
				estimated_useful_life_years,
				prior_accumulated_depreciation );
	}
	else
	if ( depreciation_method == sum_of_years_digits )
	{
		amount =
			depreciation_sum_of_years_digits(
				service_placement_date,
				prior_depreciation_date,
				depreciation_date,
				cost_basis,
				estimated_residual_value,
				estimated_useful_life_years,
				prior_accumulated_depreciation );
	}
	else
	if ( depreciation_method == double_declining_balance )
	{
		amount =
			depreciation_double_declining_balance(
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
		amount =
			depreciation_n_declining_balance(
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
	if ( depreciation_method == units_of_production )
	{
		amount =
			depreciation_units_of_production(
				cost_basis,
				estimated_residual_value,
				estimated_useful_life_units,
				units_produced,
				prior_accumulated_depreciation );
	}

	if ( amount < 0.0 ) amount = 0.0;

	return amount;
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

DEPRECIATION *depreciation_calloc( void )
{
	DEPRECIATION *depreciation;

	if ( ! ( depreciation = calloc( 1, sizeof ( DEPRECIATION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return depreciation;
}

DEPRECIATION *depreciation_new(
		char *asset_name,
		char *serial_label,
		char *depreciation_date )
{
	DEPRECIATION *depreciation;

	if ( !asset_name
	||   !serial_label
	||   !depreciation_date )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	depreciation = depreciation_calloc();

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

	/* See attribute_list depreciation */
	/* ------------------------------- */
	piece( asset_name, SQL_DELIMITER, input, 0 );
	piece( serial_label, SQL_DELIMITER, input, 1 );
	piece( depreciation_date, SQL_DELIMITER, input, 2 );

	depreciation =
		depreciation_new(
			strdup( asset_name ),
			strdup( serial_label ),
			strdup( depreciation_date ) );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	depreciation->units_produced = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	depreciation->amount = atof( piece_buffer );

	*full_name = '\0';

	piece( full_name, SQL_DELIMITER, input, 5 );
	piece( street_address, SQL_DELIMITER, input, 6 );

	if ( *full_name )
	{
		depreciation->entity_self =
			entity_self_new(
				strdup( full_name ),
				strdup( street_address ) );
	}

	if ( piece( piece_buffer, SQL_DELIMITER, input, 7 )
	&&   *piece_buffer )
	{
		depreciation->transaction_date_time = strdup( piece_buffer );
	}

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

	return where;
}

DEPRECIATION *depreciation_fetch(
		char *asset_name,
		char *serial_label,
		char *depreciation_date )
{
	if ( !asset_name
	||   !serial_label
	||   !depreciation_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	depreciation_parse(
		string_pipe_fetch(
			depreciation_system_string(
				DEPRECIATION_TABLE,
		 		/* --------------------- */
		 		/* Returns static memory */
		 		/* --------------------- */
		 		depreciation_primary_where(
					asset_name,
					serial_label,
					depreciation_date ) ) ) );
}

FILE *depreciation_input_pipe_open( char *system_string )
{
	if ( !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return popen( system_string, "r" );
}

LIST *depreciation_system_list( char *system_string )
{
	FILE *input_pipe_open;
	char input[ 1024 ];
	LIST *list;

	if ( !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list = list_new();

	input_pipe_open =
		depreciation_input_pipe_open(
			system_string );

	while ( string_input( input, input_pipe_open, 1024 ) )
	{
		list_set(
			list, 
			depreciation_parse(
				input ) );
	}

	pclose( input_pipe_open );
	return list;
}

char *depreciation_system_string(
		char *depreciation_table,
		char *where )
{
	char system_string[ 1024 ];

	if ( !where ) where = "1 = 1";

	sprintf(system_string,
		"select.sh '*' %s \"%s\" depreciation_date",
		depreciation_table,
		where );

	return strdup( system_string );
}

LIST *depreciation_list_fetch(
		char *depreciation_table,
		char *asset_name,
		char *serial_label )
{
	return
	depreciation_system_list(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		depreciation_system_string(
			depreciation_table,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			fixed_asset_purchase_primary_where(
				asset_name,
				serial_label ) ) );
}

FILE *depreciation_delete_open(
		char *depreciation_primary_key,
		char *depreciation_table )
{
	char system_string[ 1024 ];

	sprintf( system_string,
		 "delete_statement table=%s field=%s delimiter='%c'	|"
		 "tee_appaserver.sh					|"
		 "sql							 ",
		 depreciation_table,
		 depreciation_primary_key,
		 '^' );

	return popen( system_string, "w" );
}

double depreciation_accumulated(
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

	if ( !depreciation_amount
	||   depreciation_amount < 0 )
	{
		return (TRANSACTION *)0;
	}

	transaction_date_time =
		/* ------------------------------------ */
		/* Returns heap memory.			*/
		/* Increments second each invocation.   */
		/* ------------------------------------ */
		transaction_increment_date_time(
			depreciation_date );

	if ( ! ( transaction =
			transaction_new(
				full_name,
				street_address,
				transaction_date_time ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: transaction_new(%s,%s,%s) returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 full_name,
			 street_address,
			 transaction_date_time );
		exit( 1 );
	}

	transaction->transaction_amount = depreciation_amount;
	transaction->memo = DEPRECIATION_MEMO;

	transaction->journal_list =
		journal_binary_list(
			full_name,
			street_address,
			transaction_date_time,
			transaction->transaction_amount,
			account_fetch(
				account_depreciation_expense,
				1 /* fetch_subclassification */,
				1 /* fetch_element */ )
					/* debit_account */,
			account_fetch(
				account_accumulated_depreciation,
				1 /* fetch_subclassification */,
				1 /* fetch_element */ )
					/* credit_account */ );

	return transaction;
}

FILE *depreciation_insert_pipe_open(
		char *depreciation_table,
		char *depreciation_insert_columns )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"insert_statement table=%s field='%s' delimiter='^'	|"
		"tee_appaserver.sh					|"
		"sql 2>&1						 ",
		depreciation_table,
		depreciation_insert_columns );

	return popen( system_string, "w" );
}

void depreciation_insert_pipe(
		char *asset_name,
		char *serial_label,
		char *depreciation_date,
		int units_produced,
		double amount,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		FILE *insert_pipe_open )
{
	if ( !asset_name
	||   !serial_label
	||   !depreciation_date
	||   !amount
	||   !full_name
	||   !street_address
	||   !transaction_date_time
	||   !insert_pipe_open )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		if ( insert_pipe_open ) pclose( insert_pipe_open );
		exit( 1 );
	}

	fprintf(insert_pipe_open,
		"%s^%s^%s^%d^%.2lf^%s^%s^%s\n",
		asset_name,
		serial_label,
		depreciation_date,
		units_produced,
		amount,
		full_name,
		street_address,
		transaction_date_time );
}

void depreciation_list_insert( LIST *depreciation_list )
{
	DEPRECIATION *depreciation;
	FILE *insert_pipe_open;

	if ( !list_rewind( depreciation_list ) ) return;

	insert_pipe_open =
		depreciation_insert_pipe_open(
			DEPRECIATION_TABLE,
			DEPRECIATION_INSERT_COLUMNS );

	do {
		depreciation =
			list_get(
				depreciation_list );

		if ( !depreciation->transaction_date_time )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: depreciation->transaction_date_time is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			pclose( insert_pipe_open );
			exit( 1 );
		}

		if ( !depreciation->entity_self )
		{
			fprintf(stderr,
				"ERROR  in %s/%s()/%d: empty entity_self.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			pclose( insert_pipe_open );
			exit( 1 );
		}

		if ( depreciation->amount > 0.0 )
		{
			depreciation_insert_pipe(
				depreciation->asset_name,
				depreciation->serial_label,
				depreciation->depreciation_date,
				depreciation->units_produced,
				depreciation->amount,
				depreciation->
					entity_self->
					entity->
					full_name,
				depreciation->
					entity_self->
					entity->
					street_address,
				depreciation->transaction_date_time,
				insert_pipe_open );
		}

	} while ( list_next( depreciation_list ) );

	pclose( insert_pipe_open );
}

char *depreciation_prior_depreciation_date(
		char *depreciation_table,
		char *asset_name,
		char *serial_label,
		char *depreciation_date )
{
	char system_string[ 1024 ];
	char where[ 512 ];
	char asset_where[ 256 ];
	char depreciation_date_where[ 128 ];

	if ( asset_name && *asset_name )
	{
		sprintf(asset_where,
			"asset_name = '%s' and serial_label = '%s'",
			fixed_asset_name_escape( asset_name ),
			serial_label );
	}
	else
	{
		strcpy( asset_where, "1 = 1" );
	}

	if ( depreciation_date && *depreciation_date )
	{
		sprintf(depreciation_date_where,
			"depreciation_date < '%s'",
			depreciation_date );
	}
	else
	{
		strcpy( depreciation_date_where, "1 = 1" );
	}

	sprintf(where,
		"%s and %s",
		asset_where,
		depreciation_date_where );

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" ''",
		"max( depreciation_date )",
		depreciation_table,
		where );

	return string_pipe_fetch( system_string );
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

enum depreciation_method depreciation_method_evaluate(
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
	int units_produced;
	double amount;
	double accumulated_depreciation;
	static ENTITY_SELF *entity_self = {0};
	DEPRECIATION *depreciation;

	units_produced =
		depreciation_units_produced(
			asset_name,
			serial_label,
			depreciation_method,
			units_produced_so_far );

	amount =
		depreciation_amount(
			depreciation_method,
			service_placement_date,
			prior_depreciation_date,
			depreciation_date,
			cost_basis,
			units_produced,
			estimated_residual_value,
			estimated_useful_life_years,
			estimated_useful_life_units,
			declining_balance_n,
			prior_accumulated_depreciation );

	if ( amount <= 0.0 ) return NULL;

	accumulated_depreciation =
		depreciation_accumulated(
			prior_accumulated_depreciation,
			amount );

	if ( !entity_self )
	{
		entity_self =
			entity_self_fetch(
				0 /* not fetch_entity_boolean */);

		if ( !entity_self )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: entity_self_fetch() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}
	}

	depreciation =
		depreciation_new(
			asset_name,
			serial_label,
			depreciation_date );

	depreciation->depreciation_method = depreciation_method;
	depreciation->units_produced = units_produced;
	depreciation->amount = amount;
	depreciation->accumulated_depreciation = accumulated_depreciation;
	depreciation->entity_self = entity_self;

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

	if ( !service_placement_date || !*service_placement_date )
	{
		earlier_date_string = prior_depreciation_date;
	}
	else
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
		char *depreciation_table,
		char *asset_name,
		char *serial_label )
{
	char system_string[ 1024 ];

	if ( !depreciation_table
	||   !asset_name
	||   !serial_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" ''",
		"sum( units_produced )",
		depreciation_table,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		fixed_asset_purchase_primary_where(
			asset_name,
			serial_label ) );

	return
	string_atoi( string_pipe_fetch( system_string ) );
}

int depreciation_units_produced_current(
		int units_produced_so_far,
		int units_produced_total )
{
	return units_produced_so_far - units_produced_total;
}

char *depreciation_subquery_exists_where(
		char *depreciation_table,
		char *fixed_asset_purchase_table,
		char *depreciation_date )
{
	char where[ 1024 ];

	sprintf(where,
		"exists ( select 1				"
		"	  from %s				"
		"	  where %s.asset_name =	"
		"		%s.asset_name			"
		"	    and %s.serial_label =		"
		"		%s.serial_label			"
		"	    and depreciation_date = '%s' )	",
		depreciation_table,
		fixed_asset_purchase_table,
		depreciation_table,
		fixed_asset_purchase_table,
		depreciation_table,
		depreciation_date );

	return strdup( where );
}

FILE *depreciation_update_pipe_open(
		char *table,
		char *primary_key )
{
	char system_string[ 1024 ];

	sprintf( system_string,
		 "update_statement.e table=%s key=%s carrot=y	|"
		 "tee_appaserver.sh				|"
		 "sql						 ",
		 table,
		 primary_key );

	return popen( system_string, "w" );
}

void depreciation_update(
		int units_produced,
		double depreciation_amount,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *asset_name,
		char *serial_label,
		char *depreciation_date )
{
	FILE *update_pipe_open;

	if ( !full_name
	||   !street_address
	||   !transaction_date_time
	||   !asset_name
	||   !serial_label
	||   !depreciation_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	update_pipe_open =
		depreciation_update_pipe_open(
			DEPRECIATION_TABLE,
			DEPRECIATION_PRIMARY_KEY );

	depreciation_update_pipe(
		units_produced,
		depreciation_amount,
		full_name,
		street_address,
		transaction_date_time,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		fixed_asset_name_escape( asset_name ),
		serial_label,
		depreciation_date,
		update_pipe_open );

	pclose( update_pipe_open );
}

void depreciation_update_pipe(
		int units_produced,
		double depreciation_amount,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *fixed_asset_name_escape,
		char *serial_label,
		char *depreciation_date,
		FILE *update_pipe_open )
{
	fprintf(update_pipe_open,
		"%s^%s^%s^units_produced^%d\n",
		fixed_asset_name_escape,
		serial_label,
		depreciation_date,
		units_produced );

	fprintf(update_pipe_open,
		"%s^%s^%s^depreciation_amount^%.2lf\n",
		fixed_asset_name_escape,
		serial_label,
		depreciation_date,
		depreciation_amount );

	fprintf(update_pipe_open,
		"%s^%s^%s^full_name^%s\n",
		fixed_asset_name_escape,
		serial_label,
		depreciation_date,
		full_name );

	fprintf(update_pipe_open,
		"%s^%s^%s^street_address^%s\n",
		fixed_asset_name_escape,
		serial_label,
		depreciation_date,
		street_address );

	fprintf(update_pipe_open,
		"%s^%s^%s^transaction_date_time^%s\n",
		fixed_asset_name_escape,
		serial_label,
		depreciation_date,
		transaction_date_time );
}

int depreciation_units_produced(
		char *asset_name,
		char *serial_label,
		enum depreciation_method depreciation_method,
		int units_produced_so_far )
{
	if ( !asset_name
	||   !serial_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( depreciation_method != units_of_production )
	{
		return 0;
	}

	return
	depreciation_units_produced_current(
		units_produced_so_far,
		depreciation_units_produced_total(
			DEPRECIATION_TABLE,
			asset_name,
			serial_label ) );
}

