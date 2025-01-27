/* library/julian_time_double_list.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* -- Tim Riley								*/
/* -------------------------------------------------------------------- */

#ifndef JULIAN_TIME_DOUBLE_LIST_H
#define JULIAN_TIME_DOUBLE_LIST_H

#include "list.h"

LIST *get_julian_time_double_list(	
				char *begin_date_yyyy_mm_dd,
				char *begin_hhmm,
				char *end_date_yyyy_mm_dd,
				char *end_hhmm,
				double time_segments_per_day );
#endif
