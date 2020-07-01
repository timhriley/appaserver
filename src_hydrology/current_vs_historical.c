/* current_vs_historical.c */
/* ----------------------- */

#include <stdio.h>
#include <string.h>
#include "date.h"
#include "application_constants.h"
#include "current_vs_historical.h"

enum state current_vs_historical_get_state( char *state_string )
{
	if ( strcmp( state_string, "initial" ) == 0 )
		return initial;
	else
	if ( strcmp( state_string, "post_map" ) == 0 )
		return post_map;
	else
	if ( strcmp( state_string, "map" ) == 0 )
		return map;
	else
	if ( strcmp( state_string, "current" ) == 0 )
		return current;
	else
	if ( strcmp( state_string, "historical" ) == 0 )
		return historical;
	else
		return unknown;

} /* current_vs_historical_get_state() */

char *current_vs_historical_get_state_string(
				enum state state )
{
	if ( state == initial )
		return "initial";
	else
	if ( state == post_map )
		return "post_map";
	else
	if ( state == map )
		return "map";
	else
	if ( state == current )
		return "current";
	else
	if ( state == historical )
		return "historical";
	else
		return "unknown";

} /* current_vs_historical_get_state() */

void current_vs_historical_get_dates(
			char **por_historical_begin_date_string,
			char **por_historical_end_date_string,
			char **por_current_begin_date_string,
			char **por_current_end_date_string,
			char **current_begin_date_string,
			char **current_end_date_string,
			char *application_name )
{
	DATE *today;
	DATE *first_of_month;
	DATE *por_historical_begin_date;
	DATE *por_historical_end_date;
	DATE *por_current_begin_date;
	DATE *por_current_end_date;
	DATE *current_begin_date;
	int historical_range_years;
	APPLICATION_CONSTANTS *application_constants;

	application_constants = application_constants_new();
	application_constants->dictionary =
		application_constants_get_dictionary(
			application_name );

	if ( ! ( historical_range_years =
			atoi( application_constants_safe_fetch(
				application_constants->dictionary,
				"current_vs_historical_range_years" ) ) ) )
	{
		historical_range_years = HISTORICAL_RANGE_YEARS;
	}

	today = date_today_new( date_get_utc_offset() );

	first_of_month =
		date_forward_to_first_month(
			today,
			date_get_utc_offset() );

	por_historical_end_date = date_calloc();
	date_copy( por_historical_end_date, first_of_month );

	date_decrement_years(
		por_historical_end_date,
		1 );

	date_increment_months(
		por_historical_end_date,
		-1 );

	date_decrement_days(
		por_historical_end_date,
		1 );

	current_begin_date = date_today_new( date_get_utc_offset() );

	date_decrement_years(
		current_begin_date,
		1 );

	date_increment_days(
		current_begin_date,
		1.0,
		date_utc_offset() );

	por_current_begin_date = date_calloc();
	date_copy( por_current_begin_date, first_of_month );

	date_decrement_years(
		por_current_begin_date,
		1 );

	date_increment_months(
		por_current_begin_date,
		-1 );

	por_historical_begin_date = date_calloc();
	date_copy( por_historical_begin_date, first_of_month );

	date_decrement_years(
		por_historical_begin_date,
		historical_range_years + 1 );

	date_increment_months(
		por_historical_begin_date,
		-1 );

	por_current_end_date = date_calloc();
	date_copy( por_current_end_date, first_of_month );

	date_increment_months(
		por_current_end_date,
		-1 );

	date_decrement_days( por_current_end_date, 1 );

	*por_historical_begin_date_string =
		date_display_yyyy_mm_dd(
			por_historical_begin_date );

	*por_historical_end_date_string =
		date_display_yyyy_mm_dd(
			por_historical_end_date );

	*por_current_begin_date_string =
		date_display_yyyy_mm_dd(
			por_current_begin_date );

	*por_current_end_date_string =
		date_display_yyyy_mm_dd( por_current_end_date );

	*current_begin_date_string =
		date_display_yyyy_mm_dd(
			current_begin_date );

	*current_end_date_string = date_display_yyyy_mm_dd( today );

} /* current_vs_historical_get_dates() */

