/* library/date_aggregate.h     */
/* ---------------------------- */
/* Copyright 2001-02 Tim Riley	*/
/* ---------------------------- */

#include <stdio.h>
#include "date_aggregate.h"
#include "aggregate_level.h"

DATE_AGGREGATE *date_aggregate_new( char *aggregate_level_string )
{
	DATE_AGGREGATE *date_aggregate;
	date_aggregate = calloc( 1, sizeof( DATE_AGGREGATE ) );
	date_aggregate->first_time = 1;

	date_aggregate->aggregate_level =
		aggregate_level_get( aggregate_level_string );

	return date_aggregate;
}

void date_aggregate_set_yyyy_mm_dd( 	DATE_AGGREGATE *date_aggregate,
					char *date_yyyy_mm_dd )
{
	date_aggregate->new_date = 
		date_new_yyyy_mm_dd(
			date_yyyy_mm_dd );

	if ( date_aggregate->first_time )
	{
		date_aggregate->first_time = 0;
		date_aggregate->old_date = date_aggregate->new_date;
		return;
	}

	if ( date_aggregate->aggregate_level == daily )
	{
/*
fprintf( stderr, "for date = (%s), old = %s, new = %s\n",
date_yyyy_mm_dd,
date_display( date_aggregate->old_date ),
date_display( date_aggregate->new_date ) );
fprintf( stderr, "for date = (%s), day number for old = %d, new = %d\n",
date_yyyy_mm_dd,
date_get_day_number( date_aggregate->old_date ),
date_get_day_number( date_aggregate->new_date ) );
*/

		if ( ( date_day_number( date_aggregate->old_date ) !=
		       date_day_number( date_aggregate->new_date ) )
		||   ( date_year_number( date_aggregate->old_date ) !=
	     	       date_year_number( date_aggregate->new_date ) ) )
		{
			date_aggregate->old_date = 
				date_aggregate->new_date;
		}
	}
	else
	if ( date_aggregate->aggregate_level == weekly )
	{
		if ( ( date_week_number( date_aggregate->old_date ) !=
		       date_week_number( date_aggregate->new_date ) )
		||   ( date_year_number( date_aggregate->old_date ) !=
	     	       date_year_number( date_aggregate->new_date ) ) )
		{
			date_aggregate->old_date = 
				date_aggregate->new_date;
		}
	}
	else
	if ( date_aggregate->aggregate_level == monthly )
	{
		if ( ( date_month_number( date_aggregate->old_date ) !=
		       date_month_number( date_aggregate->new_date ) )
		||   ( date_year_number( date_aggregate->old_date ) !=
	     	       date_year_number( date_aggregate->new_date ) ) )
		{
			date_aggregate->old_date = 
				date_aggregate->new_date;
		}
	}
	else
	if ( date_aggregate->aggregate_level == annually )
	{
		if ( ( date_year_number( date_aggregate->old_date ) !=
		       date_year_number( date_aggregate->new_date ) ) )
		{
			date_aggregate->old_date = 
				date_aggregate->new_date;
		}
	}
}

char *date_aggregate_yyyy_mm_dd( DATE_AGGREGATE *date_aggregate )
{
	return date_yyyy_mm_dd_string( date_aggregate->old_date );
}

