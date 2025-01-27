/* histogram_dvr.c */
/* --------------- */

#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "histogram.h"

void populate_number_double_list( LIST *number_double_list );

int main()
{
	HISTOGRAM *histogram;

	histogram = histogram_new_histogram();

	populate_number_double_list( histogram->number_double_list );

	histogram->segment_list =
		histogram_get_segment_list(
					&histogram->number_of_segments,
					histogram->number_double_list );

	histogram_count(	histogram->segment_list,
				histogram->number_double_list );

	histogram_segment_list_display(
		histogram->segment_list );

	return 0;
} /* main() */

void populate_number_double_list( LIST *number_double_list )
{
	histogram_set_number_double( number_double_list, 3.50 );
	histogram_set_number_double( number_double_list, 3.40 );
	histogram_set_number_double( number_double_list, 3.20 );
	histogram_set_number_double( number_double_list, 3.30 );
	histogram_set_number_double( number_double_list, 3.10 );
	histogram_set_number_double( number_double_list, 3.50 );
	histogram_set_number_double( number_double_list, 3.40 );
	histogram_set_number_double( number_double_list, 3.20 );
	histogram_set_number_double( number_double_list, 3.50 );
	histogram_set_number_double( number_double_list, 3.50 );
	histogram_set_number_double( number_double_list, 3.60 );
	histogram_set_number_double( number_double_list, 3.60 );
	histogram_set_number_double( number_double_list, 3.60 );
	histogram_set_number_double( number_double_list, 3.70 );
	histogram_set_number_double( number_double_list, 3.90 );
	histogram_set_number_double( number_double_list, 3.80 );
	histogram_set_number_double( number_double_list, 3.60 );
	histogram_set_number_double( number_double_list, 3.50 );
	histogram_set_number_double( number_double_list, 3.30 );
	histogram_set_number_double( number_double_list, 3.30 );
	histogram_set_number_double( number_double_list, 3.30 );
	histogram_set_number_double( number_double_list, 3.20 );
	histogram_set_number_double( number_double_list, 3.40 );
	histogram_set_number_double( number_double_list, 3.60 );
	histogram_set_number_double( number_double_list, 3.60 );
	histogram_set_number_double( number_double_list, 3.47 );
	histogram_set_number_double( number_double_list, 3.40 );
	histogram_set_number_double( number_double_list, 3.20 );
} /* populate_number_double_list() */

#ifdef NOT_DEFINED
void populate_number_double_list( LIST *number_double_list )
{
	histogram_set_number_double( number_double_list, 3.50 );
	histogram_set_number_double( number_double_list, 4.40 );
	histogram_set_number_double( number_double_list, 4.20 );
	histogram_set_number_double( number_double_list, 5.30 );
	histogram_set_number_double( number_double_list, 5.10 );
	histogram_set_number_double( number_double_list, 4.50 );
	histogram_set_number_double( number_double_list, 3.40 );
	histogram_set_number_double( number_double_list, 3.20 );
	histogram_set_number_double( number_double_list, 3.50 );
	histogram_set_number_double( number_double_list, 2.50 );
	histogram_set_number_double( number_double_list, 3.60 );
	histogram_set_number_double( number_double_list, 6.60 );
	histogram_set_number_double( number_double_list, 5.60 );
	histogram_set_number_double( number_double_list, 5.70 );
	histogram_set_number_double( number_double_list, 4.90 );
	histogram_set_number_double( number_double_list, 4.80 );
	histogram_set_number_double( number_double_list, 2.60 );
	histogram_set_number_double( number_double_list, 3.50 );
	histogram_set_number_double( number_double_list, 3.30 );
	histogram_set_number_double( number_double_list, 3.30 );
	histogram_set_number_double( number_double_list, 2.30 );
	histogram_set_number_double( number_double_list, 2.20 );
	histogram_set_number_double( number_double_list, 3.40 );
	histogram_set_number_double( number_double_list, 3.60 );
	histogram_set_number_double( number_double_list, 5.60 );
	histogram_set_number_double( number_double_list, 5.47 );
	histogram_set_number_double( number_double_list, 3.40 );
	histogram_set_number_double( number_double_list, 3.20 );
} /* populate_number_double_list() */
#endif

