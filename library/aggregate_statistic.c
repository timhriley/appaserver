/* library/aggregate_statistic.c */
/* ----------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aggregate_statistic.h"

enum aggregate_statistic aggregate_statistic_get_aggregate_statistic(
				char *aggregate_statistic_string,
				enum aggregate_level aggregate_level )
{
	enum aggregate_statistic aggregate_statistic = aggregate_statistic_none;

	if ( aggregate_level == aggregate_level_none
	||   aggregate_level == real_time )
		aggregate_statistic = aggregate_statistic_none;
	else
	if ( !aggregate_statistic_string )
		aggregate_statistic = aggregate_statistic_none;
	else
	if ( strcmp( aggregate_statistic_string, "average" ) == 0 )
		aggregate_statistic = average;
	else
	if ( strcmp( aggregate_statistic_string, "sum" ) == 0 )
		aggregate_statistic = sum;
	else
	if ( strcmp( aggregate_statistic_string, "total" ) == 0 )
		aggregate_statistic = sum;
	else
	if ( strcmp( aggregate_statistic_string, "minimum" ) == 0 )
		aggregate_statistic = minimum;
	else
	if ( strcmp( aggregate_statistic_string, "median" ) == 0 )
		aggregate_statistic = median;
	else
	if ( strcmp( aggregate_statistic_string, "maximum" ) == 0 )
		aggregate_statistic = maximum;
	else
	if ( strcmp( aggregate_statistic_string, "range" ) == 0 )
		aggregate_statistic = range;
	else
	if ( strcmp( aggregate_statistic_string, "standard_deviation" ) == 0 )
		aggregate_statistic = standard_deviation;
	else
	if ( strcmp( aggregate_statistic_string, "non_zero_percent" ) == 0 )
		aggregate_statistic = non_zero_percent;
	else
		aggregate_statistic = aggregate_statistic_none;

	return aggregate_statistic;
}

char *aggregate_statistic_display(
			enum aggregate_statistic aggregate_statistic )
{
	return aggregate_statistic_string( aggregate_statistic );
}

char *aggregate_statistic_get_string(
			enum aggregate_statistic aggregate_statistic )
{
	return aggregate_statistic_string( aggregate_statistic );
}

char *aggregate_statistic_string(
			enum aggregate_statistic aggregate_statistic )
{
	if ( aggregate_statistic == average )
		return "average";
	else
	if ( aggregate_statistic == sum )
		return "sum";
	else
	if ( aggregate_statistic == minimum )
		return "minimum";
	else
	if ( aggregate_statistic == median )
		return "median";
	else
	if ( aggregate_statistic == maximum )
		return "maximum";
	else
	if ( aggregate_statistic == range )
		return "range";
	else
	if ( aggregate_statistic == standard_deviation )
		return "standard_deviation";
	else
	if ( aggregate_statistic == non_zero_percent )
		return "non_zero_percent";
	else
		return "value";
}

enum aggregate_statistic *aggregate_statistic_new_aggregate_statistic( void )
{
	enum aggregate_statistic *aggregate_statistic_pointer;

	aggregate_statistic_pointer =
		(enum aggregate_statistic *)
			calloc( 1, sizeof( enum aggregate_statistic ) );
	return aggregate_statistic_pointer;
}

