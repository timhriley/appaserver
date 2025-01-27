/* library/date_aggregate.h */
/* ------------------------ */

#ifndef DATE_AGGREGATE_H
#define DATE_AGGREGATE_H

#include <stdio.h>
#include "date.h"
#include "aggregate_level.h"

typedef struct
{
	enum aggregate_level aggregate_level;
	DATE *new_date;
	DATE *old_date;
	int first_time;
} DATE_AGGREGATE;

DATE_AGGREGATE *date_aggregate_new(	char *aggregate_level_string );

void date_aggregate_set_yyyy_mm_dd( 	DATE_AGGREGATE *date_aggregate,
					char *date_yyyy_mm_dd );

char *date_aggregate_yyyy_mm_dd(	DATE_AGGREGATE *date_aggregate );

#endif

