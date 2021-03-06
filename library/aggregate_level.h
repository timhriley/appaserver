/* library/aggregate_level.h */
/* ------------------------- */

#ifndef AGGREGATE_LEVEL_H
#define AGGREGATE_LEVEL_H

enum aggregate_level {	aggregate_level_none,
			real_time,
			half_hour,
			hourly,
			daily,
			weekly,
			monthly,
			annually,
			delta };

enum aggregate_level aggregate_level_get_aggregate_level(
				char *aggregate_level_string );

char *aggregate_level_string(
				enum aggregate_level );

char *aggregate_level_get_string(
				enum aggregate_level );

char *aggregate_level_get_summation_heading(
				enum aggregate_level,
				char heading_delimiter );
#endif

