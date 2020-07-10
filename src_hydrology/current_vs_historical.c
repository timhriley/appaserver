/* -----------------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/current_vs_historical.c	*/
/* -----------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------------	*/

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

void current_vs_historical_dates(
			char **por_historical_begin_date_string,
			char **por_historical_end_date_string,
			char **current_begin_date_string,
			char **current_end_date_string,
			int *historical_range_years,
			char *application_name,
			int current_year )
{
	DATE *today;
	DATE *por_historical_begin_date;
	DATE *por_historical_end_date;
	DATE *current_begin_date;
	APPLICATION_CONSTANTS *application_constants;

	application_constants = application_constants_new();
	application_constants->dictionary =
		application_constants_get_dictionary(
			application_name );

	if ( !*historical_range_years )
	{
		if ( ! ( *historical_range_years =
			   atoi( application_constants_safe_fetch(
				application_constants->dictionary,
				"current_vs_historical_range_years" ) ) ) )
		{
			*historical_range_years = HISTORICAL_RANGE_YEARS;
		}
	}

	today = date_today_new( date_utc_offset() );

	if ( current_year )
	{
		date_set_year( today, current_year, date_utc_offset() );
	}

	/* Set POR Historical Begin Date */
	/* ----------------------------- */
	por_historical_begin_date = date_calloc();
	date_copy( por_historical_begin_date, today );

	date_decrement_years(
		por_historical_begin_date,
		*historical_range_years + 1,
		0 /* utc_offset */ );

	date_increment_days(
		por_historical_begin_date,
		1.0,
		0 /* utc_offset */ );

	/* Set POR Historical End Date */
	/* --------------------------- */
	por_historical_end_date = date_calloc();
	date_copy( por_historical_end_date, today );

	date_decrement_years(
		por_historical_end_date,
		1,
		0 /* utc_offset */ );

	/* Set Current Begin Date */
	/* ---------------------- */
	current_begin_date = date_calloc(); 
	date_copy( current_begin_date, today );

	date_decrement_years(
		current_begin_date,
		1,
		0 /* utc_offset */ );

	date_increment_days(
		current_begin_date,
		1.0,
		0 /* utc_offset */ );

	*por_historical_begin_date_string =
		date_display_yyyy_mm_dd(
			por_historical_begin_date );

	*por_historical_end_date_string =
		date_display_yyyy_mm_dd(
			por_historical_end_date );

	*current_begin_date_string =
		date_display_yyyy_mm_dd(
			current_begin_date );

	*current_end_date_string = date_display_yyyy_mm_dd( today );
}

void current_vs_historical_output_body(	FILE *output_file,
					char *additional_javascript )
{
	fprintf(output_file, "<body" );

	if ( additional_javascript && additional_javascript )
	{
		fprintf( output_file, " onload=\"%s\"", additional_javascript );
	}

	fprintf(output_file,
		" leftmargin=0 topmargin=0 marginwidth=0 marginheight=0>\n" );
}

