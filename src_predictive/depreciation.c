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
#include "piece.h"
#include "date.h"
#include "depreciation.h"

double depreciation_equipment(
			char *depreciation_method,
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			int estimated_useful_life_units,
			int declining_balance_n,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
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
			extension,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_depreciation_date_string,
			depreciation_date_string,
			finance_accumulated_depreciation );
	}
	else
	if ( strcmp( depreciation_method, "units_of_production" ) == 0 )
	{
		return depreciation_units_of_production(
			extension,
			estimated_residual_value,
			estimated_useful_life_units,
			units_produced,
			finance_accumulated_depreciation );
	}
	else
	if ( strcmp( depreciation_method, "double_declining_balance" ) == 0 )
	{
		return depreciation_n_declining_balance(
			extension,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_depreciation_date_string,
			depreciation_date_string,
			finance_accumulated_depreciation,
			2 /* n */ );
	}
	else
	if ( strcmp( depreciation_method, "n_declining_balance" ) == 0 )
	{
		return depreciation_n_declining_balance(
			extension,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_depreciation_date_string,
			depreciation_date_string,
			finance_accumulated_depreciation,
			declining_balance_n );
	}
	else
	if ( strcmp( depreciation_method, "sum_of_years_digits" ) == 0 )
	{
		return depreciation_sum_of_years_digits(
			extension,
			estimated_residual_value,
			estimated_useful_life_years,
			prior_depreciation_date_string,
			depreciation_date_string,
			finance_accumulated_depreciation,
			service_placement_date );
	}
	else
	{
		return 0.0;
	}
}

double depreciation_units_of_production(
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_units,
			int units_produced,
			double finance_accumulated_depreciation )
{
	double depreciation_rate_per_unit = 0.0;
	double depreciation_base;
	double depreciation_amount;

	depreciation_base = extension - (double)estimated_residual_value;

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
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double finance_accumulated_depreciation )
{
	double depreciation_base;
	double annual_depreciation_amount;
	double fraction_of_year = 0.0;
	double depreciation_amount;

	if ( prior_depreciation_date_string
	&&   *prior_depreciation_date_string )
	{
		fraction_of_year =
			depreciation_fraction_of_year(
				prior_depreciation_date_string,
				depreciation_date_string );
	}

	depreciation_base = extension - (double)estimated_residual_value;

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
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
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

	depreciation_base = extension - (double)estimated_residual_value;

	denominator =
		( (double)( estimated_useful_life_years *
			  ( estimated_useful_life_years + 1 ) ) )
			  / 2.0;

	current_age_years =
		date_years_between(
			depreciation_date_string,
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
			prior_depreciation_date_string,
			depreciation_date_string );

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
			double extension,
			int estimated_residual_value,
			int estimated_useful_life_years,
			char *prior_depreciation_date_string,
			char *depreciation_date_string,
			double finance_accumulated_depreciation,
			int n )
{
	double annual_depreciation_amount;
	double fraction_of_year;
	double depreciation_amount;
	double book_value;
	double maximum_depreciation;

	book_value = extension - finance_accumulated_depreciation;

	fraction_of_year =
		depreciation_fraction_of_year(
			prior_depreciation_date_string,
			depreciation_date_string );

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
			char *prior_date_string,
			char *date_string )
{
	int days_between;
	DATE *prior_date;
	DATE *date;

	if ( !prior_date_string
	||   !*prior_date_string
	||   !date_string
	||   !*date_string )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: empty date = [prior=%s or date=%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 prior_date_string,
			 date_string );
		return 0.0;
	}

	prior_date =
		date_yyyy_mm_dd_new(
			prior_date_string );

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

