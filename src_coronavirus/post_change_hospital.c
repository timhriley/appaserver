/* -------------------------------------------------------	*/
/* $APPASERVER_HOME/src_coronavirus/post_change_hospital.c   	*/
/* -------------------------------------------------------	*/
/* 								*/
/* Freely available software: see Appaserver.org		*/
/* -------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "document.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "hospital.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_hospital_update(
				char *application_name,
				char *hospital_name,
				char *street_address );

int main( int argc, char **argv )
{
	char *application_name;
	char *hospital_name;
	char *street_address;
	char *state;

	/* Exits if fails */
	/* -------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf( stderr,
			 "Usage: %s hospital_name street_address state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	hospital_name = argv[ 1 ];
	street_address = argv[ 2 ];
	state = argv[ 3 ];

	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_hospital_update(
			application_name,
			hospital_name,
			street_address );
	}

	return 0;

} /* main() */

void post_change_hospital_update(
				char *application_name,
				char *hospital_name,
				char *street_address )
{
	HOSPITAL *hospital;

	if ( ! ( hospital =
			hospital_fetch(
					application_name,
					hospital_name,
					street_address ) ) )
	{
		fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot hospital_fetch()\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Sets:
		---------------------------------------
		last->coronavirus_admitted_daily_change
		last->coronavirus_released_daily_change
		last->coronavirus_mortality_daily_change
		last->coronavirus_current_patient_count
		---------------------------------------
	*/
	if  ( !hospital_current_patient_count_set_last(
			hospital->current_patient_count_list ) )
	{
		hospital->coronavirus_current_patient_count_isnull = 1;
		hospital->coronavirus_admitted_todate = 1;
		hospital->coronavirus_released_todate = 1;
		hospital->coronavirus_mortality_todate = 1;
		hospital->coronavirus_admitted_daily_change = 1;
		hospital->coronavirus_released_daily_change = 1;
		hospital->coronavirus_mortality_daily_change = 1;
	}

	hospital->regular_bed_occupied_percent =
		hospital_regular_bed_occupied_percent(
			&hospital->regular_bed_occupied_percent_isnull,
			hospital->regular_bed_capacity,
			hospital->current_bed_usage_list );

	hospital->ICU_bed_occupied_percent =
		hospital_ICU_bed_occupied_percent(
			&hospital->ICU_bed_occupied_percent_isnull,
			hospital->ICU_bed_capacity,
			hospital->current_bed_usage_list );

	if ( list_length( hospital->current_patient_count )
	&&   list_length( hospital->current_ventilator_count ) )
	{
		hospital->coronavirus_patients_without_ventilators =
			hospital_coronavirus_patients_without_ventilators(
				list_get_last_pointer(
					hospital->current_patient_count ),
				list_get_last_pointer(
					hospital->current_ventilator_count ) );
	}
	else
	{
		hospital->coronavirus_patients_without_ventilators_isnull = 1;
	}

	hospital_update(
		application_name,
		hospital_name,
		street_address,

		hospital->current_ventilator_ICU_bed_occupied_count_isnull,
		hospital->current_ventilator_ICU_bed_occupied_count,

		hospital->current_ventilator_count_isnull,
		hospital->current_ventilator_count,

		hospital->necessary_beds_without_ventilators_isnull,
		hospital->necessary_beds_without_ventilators,

		hospital->ventilator_bed_occupied_percent_isnull,
		hospital->ventilator_bed_occupied_percent,

		hospital->ICU_bed_occupied_percent_isnull,
		hospital->ICU_bed_occupied_percent,

		hospital->current_ventilator_bed_occupied_count_isnull,
		hospital->current_ventilator_bed_occupied_count,

		hospital->current_ICU_bed_occupied_count_isnull,
		hospital->current_ICU_bed_occupied_count );

} /* post_change_hospital_update() */

