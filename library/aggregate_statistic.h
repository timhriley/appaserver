/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/aggregate_statistic.h			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef AGGREGATE_STATISTIC_H
#define AGGREGATE_STATISTIC_H

#include "boolean.h"
#include "dictionary.h"
#include "aggregate_level.h"

#define AGGREGATE_STATISTIC_LABEL	"aggregate_statistic"

enum aggregate_statistic {	aggregate_statistic_none,
				average,
				sum,
				minimum,
				median,
				maximum,
				range,
				standard_deviation,
				non_zero_percent };

/* Usage */
/* ----- */
enum aggregate_statistic aggregate_statistic_get(
		char *aggregate_statistic_string,
		enum aggregate_level aggregate_level );

/* Usage */
/* ----- */
enum aggregate_statistic aggregate_statistic_dictionary_extract(
		DICTIONARY *dictionary,
		boolean aggregation_sum_boolean,
		enum aggregate_level aggregate_level );

/* Usage */
/* ----- */
enum aggregate_statistic aggregate_statistic_infer(
		boolean aggregation_sum_boolean );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *aggregate_statistic_string(
		enum aggregate_statistic );

#endif

