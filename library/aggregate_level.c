/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/aggregate_level.c				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aggregate_level.h"

enum aggregate_level aggregate_level_get( char *aggregate_level_string )
{
	enum aggregate_level aggregate_level;

	if ( ( !aggregate_level_string || !*aggregate_level_string ) )
		aggregate_level = real_time;
	else
	if ( strcmp( aggregate_level_string, "real_time" ) == 0 )
		aggregate_level = real_time;
	else
	if ( strcmp( aggregate_level_string, "moving" ) == 0 )
		aggregate_level = moving;
	else
	if ( strcmp( aggregate_level_string, "half_hour" ) == 0 )
		aggregate_level = half_hour;
	else
	if ( strcmp( aggregate_level_string, "gordon" ) == 0 )
		aggregate_level = half_hour;
	else
	if ( strcmp( aggregate_level_string, "hourly" ) == 0
	||   strcmp( aggregate_level_string, "hour" ) == 0 )
		aggregate_level = hourly;
	else
	if ( strcmp( aggregate_level_string, "daily" ) == 0
	||   strcmp( aggregate_level_string, "day" ) == 0 )
		aggregate_level = daily;
	else
	if ( strcmp( aggregate_level_string, "weekly" ) == 0
	||   strcmp( aggregate_level_string, "week" ) == 0 )
		aggregate_level = weekly;
	else
	if ( strcmp( aggregate_level_string, "monthly" ) == 0
	||   strcmp( aggregate_level_string, "month" ) == 0 )
		aggregate_level = monthly;
	else
	if ( strcmp( aggregate_level_string, "annually" ) == 0
	||   strcmp( aggregate_level_string, "annual" ) == 0 )
		aggregate_level = annually;
	else
		aggregate_level = real_time;

	return aggregate_level;
}

char *aggregate_level_string( enum aggregate_level aggregate_level )
{
	if ( aggregate_level == real_time )
		return "real_time";
	else
	if ( aggregate_level == half_hour )
		return "half_hour";
	else
	if ( aggregate_level == moving )
		return "moving";
	else
	if ( aggregate_level == hourly )
		return "hourly";
	else
	if ( aggregate_level == daily )
		return "daily";
	else
	if ( aggregate_level == weekly )
		return "weekly";
	else
	if ( aggregate_level == monthly )
		return "monthly";
	else
	if ( aggregate_level == annually )
		return "annually";
	else
	if ( aggregate_level == delta )
		return "delta";
	else
		return "";
}

char *aggregate_level_summation_heading(
		enum aggregate_level aggregate_level,
		char heading_delimiter )
{
	static char summation_heading[ 128 ];

	if ( aggregate_level == real_time )
		strcpy( summation_heading, "" );
	else
	if ( aggregate_level == half_hour )
	{
		sprintf(summation_heading,
			"%cValues/HalfHour",
			heading_delimiter );
	}
	else
	if ( aggregate_level == hourly )
		sprintf(summation_heading,
			"%cValues/Hour",
			heading_delimiter );
	else
	if ( aggregate_level == daily )
		sprintf(summation_heading,
			"%cValues/Day",
			heading_delimiter );
	else
	if ( aggregate_level == weekly )
		sprintf(summation_heading,
			"%cValues/Week",
			heading_delimiter );
	else
	if ( aggregate_level == monthly )
		sprintf(summation_heading,
			"%cValues/Month",
			heading_delimiter );
	else
	if ( aggregate_level == annually )
		sprintf(summation_heading,
			"%cValues/Year",
			heading_delimiter );
	else
		*summation_heading = '\0';

	return summation_heading;
}

enum aggregate_level aggregate_level_dictionary_extract(
		DICTIONARY *dictionary )
{
	return
	aggregate_level_get(
		dictionary_get(
			AGGREGATE_LEVEL_LABEL,
			dictionary ) );
}

