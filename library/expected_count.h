/* library/expected_count.h				   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef EXPECTED_COUNT_H
#define EXPECTED_COUNT_H

#include <time.h>
#include "list.h"
#include "aggregate_level.h"

#define DEFAULT_EXPECTED_COUNT_PER_DAY			24
#define EXPECTED_COUNT_DEFAULT_MINIMUM_BEGIN_DATE	"0000-00-00"

typedef struct
{
	char *begin_measurement_date;
	char *begin_measurement_time;
	int expected_count;
	double expected_count_julian;
	time_t expected_count_change_time_t;
} EXPECTED_COUNT;

EXPECTED_COUNT *expected_count_new_expected_count(
				char *begin_measurement_date,
				char *begin_measurement_time,
				int expected_count_int );
LIST *expected_count_with_string_get_expected_count_list(
				char *expected_count_list_string,
				enum aggregate_level );

LIST *expected_count_get_expected_count_list(
				char *application_name,
				char *station,
				char *datatype );
char *expected_count_list_display(
				LIST *expected_count_list );
char *expected_count_display(	EXPECTED_COUNT *expected_count );
boolean expected_count_invalid_collection_frequency_date(
				LIST *expected_count_list,
				char *begin_date );
boolean expected_count_synchronized(
				char *application_name,
				LIST *station_name_list,
				LIST *datatype_name_list,
				char *begin_date_string,
				char *end_date_string );
int expected_count_get_date_range_count(
				LIST *expected_count_list,
				char *begin_date_string,
				char *end_date_string );

EXPECTED_COUNT *expected_count_fetch(
				LIST *expected_count_list,
				double current );

int expected_count_get_int(	char *expected_count_string );

#endif
