/* ------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_coronavirus/post_change_patient_count.c	*/
/* ------------------------------------------------------------	*/
/* 								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------	*/

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
void post_change_patient_count_update(
				char *application_name,
				char *hospital_name,
				char *street_address,
				char *date_current );

int main( int argc, char **argv )
{
	char *application_name;
	char *hospital_name;
	char *street_address;
	char *date_current;
	char *state;

	/* Exits if fails */
	/* -------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf( stderr,
		 "Usage: %s hospital_name street_address date_current state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	hospital_name = argv[ 1 ];
	street_address = argv[ 2 ];
	date_current = argv[ 3 ];
	state = argv[ 4 ];

	if ( strcmp( state, "update" ) == 0
	||   strcmp( state, "insert" ) == 0
	||   strcmp( state, "delete" ) == 0 )
	{
		char sys_string[ 1024 ];

		post_change_patient_count_update(
			application_name,
			hospital_name,
			street_address,
			date_current );

		sprintf( sys_string,
			 "post_change_hospital \"%s\" \"%s\" %s",
			 hospital_name,
			 street_address,
			 state );

		if ( system( sys_string ) ){};
	}

	return 0;

} /* main() */

void post_change_patient_count_update(
				char *application_name,
				char *hospital_name,
				char *street_address,
				char *date_current )
{
	HOSPITAL *hospital;
	CURRENT_PATIENT_COUNT *current_patient_count;

	if ( ! ( hospital =
			hospital_fetch(
					application_name,
					hospital_name,
					street_address ) ) )
	{
		return;
	}

	/* CURRENT_PATIENT_COUNT */
	/* ===================== */
	hospital_current_patient_count_set_last(
		&hospital->coronavirus_admitted_daily_change_isnull,
		&hospital->coronavirus_released_daily_change_isnull,
		&hospital->coronavirus_mortality_daily_change_isnull,
		&hospital->coronavirus_current_patient_count_isnull,
		hospital->current_patient_count_list );

	if ( ! ( current_patient_count =
			hospital_current_patient_count_seek(
				hospital->current_patient_count_list,
				date_current ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot seek date_current = [%s]\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 date_current );
		exit( 1 );
	}

	hospital_current_patient_count_update(
		application_name,
		hospital->hospital_name,
		hospital->street_address,
		current_patient_count->date_current,

		current_patient_count->coronavirus_current_patient_count,
		hospital->coronavirus_current_patient_count_isnull,

		current_patient_count->coronavirus_admitted_daily_change,
		hospital->coronavirus_admitted_daily_change_isnull,

		current_patient_count->coronavirus_released_daily_change,
		hospital->coronavirus_released_daily_change_isnull,

		current_patient_count->coronavirus_mortality_daily_change,
		hospital->coronavirus_mortality_daily_change_isnull );

} /* post_change_patient_count_update() */

