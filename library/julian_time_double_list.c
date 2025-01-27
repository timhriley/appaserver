/* library/julian_time_double_list.c					*/
/* -------------------------------------------------------------------- */
/*									*/
/* -- Tim Riley								*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "julian.h"
#include "julian_time_double_list.h"

LIST *get_julian_time_double_list(	
				char *begin_date_yyyy_mm_dd,
				char *begin_hhmm,
				char *end_date_yyyy_mm_dd,
				char *end_hhmm,
				double time_segments_per_day )
{
	double day_increment;
	double begin_julian;
	double end_julian;
	LIST *return_list = list_new();

	if ( time_segments_per_day )
		day_increment = 1.0 / time_segments_per_day;
	else
		day_increment = 1.0;

	if ( !begin_hhmm ) begin_hhmm = "0000";

	begin_julian = 
		julian_yyyy_mm_dd_time_hhmm_to_julian(
			begin_date_yyyy_mm_dd,
			begin_hhmm );

	if ( !end_hhmm ) end_hhmm = "2359";

	end_julian = 
		julian_yyyy_mm_dd_time_hhmm_to_julian(
			end_date_yyyy_mm_dd,
			end_hhmm );

	while( begin_julian < end_julian )
	{
		list_append( return_list, &begin_julian, sizeof( double ) );
		begin_julian += day_increment;
	}
	return return_list;
} /* get_julian_time_double_list() */

