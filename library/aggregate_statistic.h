/* library/aggregate_statistic.h */
/* ----------------------------- */

#ifndef AGGREGATE_STATISTIC_H
#define AGGREGATE_STATISTIC_H

#include "aggregate_level.h"

enum aggregate_statistic {	average,
				sum,
				minimum,
				median,
				maximum,
				range,
				standard_deviation,
				non_zero_percent,
				aggregate_statistic_none };

enum aggregate_statistic *aggregate_statistic_new_aggregate_statistic(
				void );

enum aggregate_statistic aggregate_statistic_get_aggregate_statistic(
				char *aggregate_statistic_string,
				enum aggregate_level );

char *aggregate_statistic_string(
			enum aggregate_statistic );

char *aggregate_statistic_get_string(
			 enum aggregate_statistic );

char *aggregate_statistic_display(
			enum aggregate_statistic aggregate_statistic );

#endif

