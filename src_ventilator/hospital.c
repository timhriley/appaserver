/* --------------------------------------------- */
/* $APPASERVER_HOME/src_ventilator/hospital.c	 */
/* --------------------------------------------- */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "hospital.h"

HOSPITAL *hospital_new(		char *hospital_name,
				char *street_address )
{
} /* hospital_new() */

int hospital_bed_capacity_percent(
				boolean *isnull,
				int hospital_bed_capacity,
				LIST *current_bed_usage_list )
{
}  /* hospital_bed_capacity_percent() */

int hospital_ICU_bed_capacity_percent(
				boolean *isnull,
				int hospital_ICU_bed_capacity,
				LIST *current_bed_usage_list )
{
} /* hospital_ICU_bed_capacity_percent() */

HOSPITAL *hospital_fetch(	char *application_name,
				char *hospital_name,
				char *street_address )
{
	char sys_string[ 1024 ];
	char input_line[ 1024 ];
	FILE *input_pipe;
	HOSPITAL *hospital;

	pclose( input_pipe );
	return hospital;

} /* hospital_fetch() */

HOSPITAL *hospital_parse( char *input_line )
{
	HOSPITAL *hospital;

	return hospital;

} /* hospital_parse() */

void hospital_update(		char *application_name,
				char *hospital_name,
				char *street_address,
				boolean hospital_bed_capacity_percent_isnull,
				int hospital_bed_capacity_percent,
				boolean ICU_bed_capacity_percent_isnull,
				int ICU_bed_capacity_percent,
				boolean beds_without_ventilators_isnull,
				int beds_without_ventilators )
{
} /* hospital_update() */

