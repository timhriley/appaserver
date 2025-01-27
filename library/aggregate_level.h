/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/aggregate_level.h	 			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef AGGREGATE_LEVEL_H
#define AGGREGATE_LEVEL_H

#include "dictionary.h"

#define AGGREGATE_LEVEL_LABEL	"aggregate_level"

enum aggregate_level {	aggregate_level_none,
			real_time,
			moving,
			half_hour,
			hourly,
			delta,
			daily,
			weekly,
			monthly,
			annually };

/* Usage */
/* ----- */
enum aggregate_level aggregate_level_get(
		char *aggregate_level_string );

/* Usage */
/* ----- */
enum aggregate_level aggregate_level_dictionary_extract(
		DICTIONARY *dictionary );

/* Public */
/* ------ */

/* Returns program memory */
/* ---------------------- */
char *aggregate_level_string(
		enum aggregate_level );

char *aggregate_level_summation_heading(
		enum aggregate_level,
		char heading_delimiter );

#endif

