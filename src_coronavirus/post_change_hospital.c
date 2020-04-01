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

	if ( strcmp( state, "update" ) == 0
	||   strcmp( state, "insert" ) == 0
	||   strcmp( state, "delete" ) == 0 )
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

	/* CURRENT_PATIENT_COUNT */
	/* ===================== */
	hospital_current_patient_count_set_last(
		&hospital->coronavirus_admitted_daily_change_isnull,
		&hospital->coronavirus_released_daily_change_isnull,
		&hospital->coronavirus_mortality_daily_change_isnull,
		&hospital->coronavirus_current_patient_count_isnull,
		hospital->current_patient_count_list );

	if ( list_length( hospital->current_patient_count_list ) )
	{
		hospital->coronavirus_admitted_todate =
			hospital_coronavirus_admitted_todate(
				list_get_last_pointer(
					hospital->
					current_patient_count_list ) );

		hospital->coronavirus_released_todate =
			hospital_coronavirus_released_todate(
				list_get_last_pointer(
					hospital->
					current_patient_count_list ) );

		hospital->coronavirus_mortality_todate =
			hospital_coronavirus_mortality_todate(
				list_get_last_pointer(
					hospital->
					current_patient_count_list ) );

		hospital->non_coronavirus_current_patient_count =
			hospital_non_coronavirus_current_patient_count(
				list_get_last_pointer(
					hospital->
					current_patient_count_list ) );

		hospital->coronavirus_current_patient_count =
			hospital_coronavirus_current_patient_count(
				list_get_last_pointer(
					hospital->
					current_patient_count_list ) );

		if ( list_length( hospital->current_patient_count_list ) > 1 )
		{
			hospital->coronavirus_admitted_daily_change =
				hospital_coronavirus_admitted_daily_change(
					list_get_last_pointer(
						hospital->
						current_patient_count_list ) );

			hospital->coronavirus_released_daily_change =
				hospital_coronavirus_released_daily_change(
					list_get_last_pointer(
						hospital->
						current_patient_count_list ) );

			hospital->coronavirus_mortality_daily_change =
				hospital_coronavirus_mortality_daily_change(
					list_get_last_pointer(
						hospital->
						current_patient_count_list ) );
		}
	}
	else
	{
		hospital->non_coronavirus_current_patient_count_isnull = 1;
		hospital->coronavirus_current_patient_count_isnull = 1;
	}

	/* CURRENT_PATIENT_COUNT and CURRENT_VENTILATOR_COUNT */
	/* -------------------------------------------------- */
	if ( list_length( hospital->current_patient_count_list )
	&&   list_length( hospital->current_ventilator_count_list ) )
	{
		hospital->coronavirus_patients_without_ventilators =
			hospital_coronavirus_patients_without_ventilators(
				list_get_last_pointer(
					hospital->
					current_patient_count_list ),
				list_get_last_pointer(
					hospital->
						current_ventilator_count_list
							) );
	}
	else
	{
		hospital->coronavirus_patients_without_ventilators_isnull = 1;
	}

	/* CURRENT_BED_USAGE */
	/* ----------------- */
	if ( list_length( hospital->current_bed_usage_list ) )
	{
		hospital->regular_bed_occupied_count =
			hospital_regular_bed_occupied_count(
				list_get_last_pointer(
					hospital->current_bed_usage_list ) );

		hospital->ICU_bed_occupied_count =
			hospital_ICU_bed_occupied_count(
				list_get_last_pointer(
					hospital->current_bed_usage_list ) );
	}
	else
	{
		hospital->regular_bed_occupied_count_isnull = 1;
		hospital->ICU_bed_occupied_count_isnull = 1;
	}

	/* CURRENT_BED_USAGE and CURRENT_BED_CAPACITY */
	/* ------------------------------------------ */
	if ( list_length( hospital->current_bed_usage_list )
	&&   list_length( hospital->current_bed_capacity_list ) )
	{
		hospital->regular_bed_occupied_percent =
			hospital_regular_bed_occupied_percent(
				list_get_last_pointer(
					hospital->current_bed_usage_list ),
				list_get_last_pointer(
					hospital->current_bed_capacity_list ) );

		hospital->ICU_bed_occupied_percent =
			hospital_ICU_bed_occupied_percent(
				list_get_last_pointer(
					hospital->current_bed_usage_list ),
				list_get_last_pointer(
					hospital->current_bed_capacity_list ) );
	}
	else
	{
		hospital->regular_bed_occupied_percent_isnull = 1;
		hospital->ICU_bed_occupied_percent_isnull = 1;
	}

	/* CURRENT_VENTILATOR_COUNT */
	/* ======================== */
	if ( list_length( hospital->current_ventilator_count_list ) )
	{
		hospital->ventilator_count =
			hospital_current_ventilator_count(
				list_get_last_pointer(
					hospital->
					current_ventilator_count_list ) );
	}
	else
	{
		hospital->ventilator_count_isnull = 1;
	}

	/* CURRENT_BED_CAPACITY */
	/* ==================== */
	if ( list_length( hospital->current_bed_capacity_list ) )
	{
		hospital->regular_bed_capacity =
			hospital_regular_bed_capacity(
				list_get_last_pointer(
					hospital->
					current_bed_capacity_list ) );

		hospital->ICU_bed_capacity =
			hospital_ICU_bed_capacity(
				list_get_last_pointer(
					hospital->
					current_bed_capacity_list ) );

		/* That column may be blank */
		/*  ----------------------- */
		if ( !hospital->ICU_bed_capacity )
			hospital->ICU_bed_capacity_isnull = 1;
	}
	else
	{
		hospital->regular_bed_capacity_isnull = 1;
		hospital->ICU_bed_capacity_isnull = 1;
	}

	/* UPDATE */
	/* ====== */
	hospital_update(
		application_name,
		hospital->hospital_name,
		hospital->street_address,

		hospital->non_coronavirus_current_patient_count,
		hospital->non_coronavirus_current_patient_count_isnull,

		hospital->coronavirus_current_patient_count,
		hospital->coronavirus_current_patient_count_isnull,

		hospital->ventilator_count,
		hospital->ventilator_count_isnull,

		hospital->coronavirus_patients_without_ventilators,
		hospital->coronavirus_patients_without_ventilators_isnull,

		hospital->coronavirus_admitted_todate,
		hospital->coronavirus_admitted_todate_isnull,

		hospital->coronavirus_released_todate,
		hospital->coronavirus_released_todate_isnull,

		hospital->coronavirus_mortality_todate,
		hospital->coronavirus_mortality_todate_isnull,

		hospital->regular_bed_capacity,
		hospital->regular_bed_capacity_isnull,

		hospital->regular_bed_occupied_count,
		hospital->regular_bed_occupied_count_isnull,

		hospital->regular_bed_occupied_percent,
		hospital->regular_bed_occupied_percent_isnull,

		hospital->ICU_bed_capacity,
		hospital->ICU_bed_capacity_isnull,

		hospital->ICU_bed_occupied_count,
		hospital->ICU_bed_occupied_count_isnull,

		hospital->ICU_bed_occupied_percent,
		hospital->ICU_bed_occupied_percent_isnull,

		hospital->coronavirus_admitted_daily_change,
		hospital->coronavirus_admitted_daily_change_isnull,

		hospital->coronavirus_released_daily_change,
		hospital->coronavirus_released_daily_change_isnull,

		hospital->coronavirus_mortality_daily_change,
		hospital->coronavirus_mortality_daily_change_isnull );

} /* post_change_hospital_update() */

