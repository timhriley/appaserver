/* --------------------------------------------- */
/* $APPASERVER_HOME/src_coronavirus/hospital.c	 */
/* --------------------------------------------- */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"
#include "hospital.h"

HOSPITAL *hospital_new(	void )
{
	HOSPITAL *h;

	if ( ! ( h = (HOSPITAL *)calloc( 1, sizeof( HOSPITAL ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return h;

} /* hospital_new() */

int hospital_regular_bed_occupied_percent(
				boolean *isnull,
				int regular_bed_capacity,
				LIST *current_bed_usage_list )
{
	CURRENT_BED_USAGE *current_bed_usage;
	double full_percent;

	*isnull = 0;

	if ( !regular_bed_capacity )
	{
		*isnull = 1;
		return 0;
	}

	if ( ! ( current_bed_usage =
			list_get_last_pointer(
				current_bed_usage_list ) ) )
	{
		*isnull = 1;
		return 0;
	}

	full_percent =
		( (double)current_bed_usage->regular_bed_occupied_count /
		  (double)regular_bed_capacity ) * 100.0;

	return (int)timlib_round_double( full_percent );

}  /* hospital_regular_bed_occupied_percent() */

int hospital_ICU_bed_occupied_percent(
				boolean *isnull,
				int ICU_bed_capacity,
				LIST *current_bed_usage_list )
{
	CURRENT_BED_USAGE *current_bed_usage;
	double full_percent;

	*isnull = 0;

	if ( !ICU_bed_capacity )
	{
		*isnull = 1;
		return 0;
	}

	if ( ! ( current_bed_usage =
			list_get_last_pointer(
				current_bed_usage_list ) ) )
	{
		*isnull = 1;
		return 0;
	}

	full_percent =
		( (double)current_bed_usage->ICU_bed_occupied_count /
		  (double)ICU_bed_capacity ) * 100.0;

	return (int)timlib_round_double( full_percent );

} /* hospital_ICU_bed_occupied_percent() */

char *hospital_select( void )
{
return
"hospital_name,street_address";
}

HOSPITAL *hospital_parse( char *input_line )
{
	HOSPITAL *hospital;
	char piece_buffer[ 512 ];

	hospital = hospital_new();

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 0 );
	hospital->hospital_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 1 );
	hospital->street_address = strdup( piece_buffer );

	return hospital;

} /* hospital_parse() */

/* Returns static memory. */
/* ---------------------- */
char *hospital_escape_name( char *hospital_name )
{
	static char escaped_name[ 256 ];

	escape_character(
		escaped_name,
		hospital_name,
		'\'' );

	return escaped_name;

} /* hospital_escape_name() */

/* Returns heap memory. */
/* -------------------- */
char *hospital_where(		char *hospital_name,
				char *street_address )
{
	char where[ 1024 ];
	char buffer[ 512 ];

	sprintf( where,
		 "hospital_name = '%s' and	"
		 "street_address = '%s'		",
		 /* ---------------------- */
		 /* Returns static memory. */
		 /* ---------------------- */
		 hospital_escape_name( hospital_name ),
		 street_address );

	return strdup( where );

} /* hospital_where() */

HOSPITAL *hospital_fetch(	char *application_name,
				char *hospital_name,
				char *street_address )
{
	char sys_string[ 2048 ];
	char input_line[ 1024 ];
	FILE *input_pipe;
	HOSPITAL *hospital = {0};

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=hospital		"
		 "			where=\"%s\"		",
		 application_name,
		 /* ----------------------- */
		 /* Returns program memory. */
		 /* ----------------------- */
		 hospital_select(),
		 /* -------------------- */
		 /* Returns heap memory. */
		 /* -------------------- */
		 hospital_where(
			hospital_name,
			street_address ) );

#ifdef NOT_DEFINE
	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\"	|"
		 "sql.e %s				 ",
		 /* ----------------------- */
		 /* Returns program memory. */
		 /* ----------------------- */
		 hospital_select(),
		 "hospital",
		 /* -------------------- */
		 /* Returns heap memory. */
		 /* -------------------- */
		 hospital_where(
			hospital_name,
			street_address ),
		 application_name );
#endif

	input_pipe = popen( sys_string, "r" );

	if ( get_line( input_line, input_pipe ) )
	{
		hospital = hospital_parse( input_line );

		hospital->current_bed_usage_list =
			hospital_current_bed_usage_list(
				application_name,
				hospital->hospital_name,
				hospital->street_address );

		hospital->current_ventilator_count_list =
			hospital_current_ventilator_count_list(
				application_name,
				hospital->hospital_name,
				hospital->street_address );

		hospital->current_patient_count_list =
			hospital_current_patient_count_list(
				application_name,
				hospital->hospital_name,
				hospital->street_address );

		if  ( !hospital_current_patient_count_set_last(
				hospital->current_patient_count_list ) )
		{
			hospital->coronavirus_current_patient_count_isnull = 1;
			hospital->coronavirus_patients_without_ventilators = 1;
			hospital->coronavirus_admitted_todate = 1;
			hospital->coronavirus_released_todate = 1;
			hospital->coronavirus_mortality_todate = 1;
			hospital->coronavirus_admitted_daily_change = 1;
			hospital->coronavirus_released_daily_change = 1;
			hospital->coronavirus_mortality_daily_change = 1;
		}

		hospital->current_bed_capacity_list =
			hospital_current_bed_capacity_list(
				application_name,
				hospital->hospital_name,
				hospital->street_address );
	}

	pclose( input_pipe );

	return hospital;

} /* hospital_fetch() */

boolean hospital_current_patient_count_set_last(
				LIST *current_patient_count_list )
{
	CURRENT_PATIENT_COUNT *c;

	if ( !list_length( current_patient_count_list ) ) return 0;

	c = list_get_last_pointer( current_patient_count_list );

	c->coronavirus_current_patient_count =
		hospital_coronavirus_current_patient_count(
			current_patient_count_list );

	c->coronavirus_admitted_daily_change =
		hospital_coronavirus_admitted_daily_change(
			current_patient_count_list );

	c->coronavirus_released_daily_change =
		hospital_coronavirus_released_daily_change(
			current_patient_count_list );

	c->coronavirus_mortality_daily_change =
		hospital_coronavirus_mortality_daily_change(
			current_patient_count_list );

	return 1;

} /* hospital_current_patient_count_set_last() */

void hospital_update(
	char *application_name,
	char *hospital_name,
	char *street_address,

	boolean non_coronavirus_current_patient_count_isnull,
	int non_coronavirus_current_patient_count,

	boolean coronavirus_current_patient_count_isnull,
	int coronavirus_current_patient_count,

	boolean ventilator_count_isnull,
	int ventilator_count,

	boolean coronavirus_patients_without_ventilators_isnull,
	int coronavirus_patients_without_ventilators,

	boolean coronavirus_released_todate_isnull,
	int coronavirus_released_todate,

	boolean ICU_bed_occupied_percent_isnull,
	int ICU_bed_occupied_percent,

	boolean regular_bed_occupied_count_isnull,
	int regular_bed_occupied_count,

	boolean ICU_bed_occupied_count_isnull,
	int ICU_bed_occupied_count )
{
	char sys_string[ 1024 ];
	FILE *update_pipe;
	char output_buffer[ 128 ];

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 "hospital",
		 "hospital_name,street_address" );

	update_pipe = popen( sys_string, "w" );

	/* Ventilator Count */
	/* ---------------- */
	if ( ventilator_count_isnull )
		*output_buffer = '\0';
	else
		sprintf(output_buffer,
			"%d",
			ventilator_count );

	fprintf( update_pipe,
		 "%s^%s^ventilator_count^%s\n",
		 hospital_name,
		 street_address,
		 output_buffer );

	/* Coronavirus Patients Without Ventilators */
	/* ---------------------------------------- */
	if ( coronavirus_patients_without_ventilators_isnull )
		*output_buffer = '\0';
	else
		sprintf(output_buffer,
			"%d",
			coronavirus_patients_without_ventilators );

	fprintf( update_pipe,
		 "%s^%s^coronavirus_patients_without_ventilators^%s\n",
		 hospital_name,
		 street_address,
		 output_buffer );

	/* Regular Bed Occupied Percent */
	/* ---------------------------- */
	if ( regular_bed_occupied_percent_isnull )
		*output_buffer = '\0';
	else
		sprintf(output_buffer,
			"%d",
			regular_bed_occupied_percent );

	fprintf( update_pipe,
		 "%s^%s^regular_bed_occupied_percent^%s\n",
		 hospital_name,
		 street_address,
		 output_buffer );

	/* ICU Bed Occupied Percent */
	/* ------------------------ */
	if ( ICU_bed_occupied_percent_isnull )
		*output_buffer = '\0';
	else
		sprintf(output_buffer,
			"%d",
			ICU_bed_occupied_percent );

	fprintf( update_pipe,
		 "%s^%s^ICU_bed_occupied_percent^%s\n",
		 hospital_name,
		 street_address,
		 output_buffer );

	/* Regular Bed Occupied Count */
	/* -------------------------- */
	if ( regular_bed_occupied_count_isnull )
		*output_buffer = '\0';
	else
		sprintf(output_buffer,
			"%d",
			regular_bed_occupied_count );

	fprintf( update_pipe,
		 "%s^%s^regular_bed_occupied_count^%s\n",
		 hospital_name,
		 street_address,
		 output_buffer );

	/* ICU Bed Occupied Count */
	/* ---------------------- */
	if ( ICU_bed_occupied_count_isnull )
		*output_buffer = '\0';
	else
		sprintf(output_buffer,
			"%d",
			ICU_bed_occupied_count );

	fprintf( update_pipe,
		 "%s^%s^ICU_bed_occupied_count^%s\n",
		 hospital_name,
		 street_address,
		 output_buffer );

	pclose( update_pipe );

} /* hospital_update() */

char *hospital_current_bed_usage_select( void )
{
	return
"date_time_greenwich,regular_bed_occupied_count,ICU_bed_occupied_count";
}

LIST *hospital_current_bed_usage_list(
				char *application_name,
				char *hospital_name,
				char *street_address )
{
	char sys_string[ 1024 ];
	char input_line[ 1024 ];
	FILE *input_pipe;
	LIST *current_bed_usage_list = list_new();
	CURRENT_BED_USAGE *current_bed_usage;

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		"
		 "			order=%s		",
		 application_name,
		 /* ----------------------- */
		 /* Returns program memory. */
		 /* ----------------------- */
		 hospital_current_bed_usage_select(),
		 "current_bed_usage",
		 /* -------------------- */
		 /* Returns heap memory. */
		 /* -------------------- */
		 hospital_where(
			hospital_name,
			street_address ),
		 "greenwich_mean" );


	input_pipe = popen( sys_string, "r" );

	while ( get_line( input_line, input_pipe ) )
	{
		current_bed_usage =
			hospital_current_bed_usage_parse(
				input_line );

		list_append_pointer(
			current_bed_usage_list,
			current_bed_usage );
	}

	pclose( input_pipe );
	return current_bed_usage_list;

} /* hospital_current_bed_usage_list() */

CURRENT_BED_USAGE *hospital_current_bed_usage_new( void )
{
	CURRENT_BED_USAGE *h;

	if ( ! ( h = (CURRENT_BED_USAGE *)
			calloc( 1, sizeof( CURRENT_BED_USAGE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return h;

} /* hospital_current_bed_usage_new() */

CURRENT_BED_USAGE *hospital_current_bed_usage_parse(
				char *input_line )
{
	CURRENT_BED_USAGE *current_bed_usage;
	char piece_buffer[ 512 ];

	current_bed_usage = hospital_current_bed_usage_new();

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 0 );
	current_bed_usage->date_time_greenwich = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 1 );
	current_bed_usage->regular_bed_occupied_count = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 2 );
	current_bed_usage->ICU_bed_occupied_count = atoi( piece_buffer );

	return current_bed_usage;

} /* hospital_current_bed_usage_parse() */

char *hospital_current_ventilator_count_select( void )
{
	return
"date_time_greenwich,ventilator_count";
}

LIST *hospital_current_ventilator_count_list(
				char *application_name,
				char *hospital_name,
				char *street_address )
{
	char sys_string[ 1024 ];
	char input_line[ 1024 ];
	FILE *input_pipe;
	LIST *current_ventilator_count_list = list_new();
	CURRENT_VENTILATOR_COUNT *current_ventilator_count;

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		"
		 "			order=date_time_greenwich	",
		 application_name,
		 /* ----------------------- */
		 /* Returns program memory. */
		 /* ----------------------- */
		 hospital_current_ventilator_count_select(),
		 "current_ventilator_count",
		 /* -------------------- */
		 /* Returns heap memory. */
		 /* -------------------- */
		 hospital_where(
			hospital_name,
			street_address ) );

	input_pipe = popen( sys_string, "r" );

	while ( get_line( input_line, input_pipe ) )
	{
		current_ventilator_count =
			hospital_current_ventilator_count_parse(
				input_line );

		list_append_pointer(
			current_ventilator_count_list,
			current_ventilator_count );
	}

	pclose( input_pipe );
	return current_ventilator_count_list;

} /* hospital_current_ventilator_count_list() */

CURRENT_VENTILATOR_COUNT *hospital_current_ventilator_count_new( void )
{
	CURRENT_VENTILATOR_COUNT *c;

	if ( ! ( c = (CURRENT_VENTILATOR_COUNT *)
			calloc( 1, sizeof( CURRENT_VENTILATOR_COUNT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return c;

} /* hospital_current_ventilator_count_new() */

CURRENT_VENTILATOR_COUNT *hospital_current_ventilator_count_parse(
				char *input_line )
{
	CURRENT_VENTILATOR_COUNT *current_ventilator_count;
	char piece_buffer[ 512 ];

	current_ventilator_count =
		hospital_current_ventilator_count_new();

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 0 );
	current_ventilator_count->date_time_greenwich = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 1 );
	current_ventilator_count->ventilator_count =
		atoi( piece_buffer );

	return current_ventilator_count;

} /* hospital_current_ventilator_count_parse() */

int hospital_coronavirus_patients_without_ventilators(
				int coronavirus_current_patient_count,
				int ventilator_count )
{
	return 	coronavirus_current_patient_count -
		ventilator_count;
}

CURRENT_PATIENT_COUNT *hospital_current_patient_count_new( void )
{
	CURRENT_PATIENT_COUNT *c;

	if ( ! ( c = (CURRENT_PATIENT_COUNT *)
			calloc( 1, sizeof( CURRENT_PATIENT_COUNT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return c;
}

char *hospital_current_patient_count_select( void )
{
	return
"date_time_greenwich,non_coronavirus_current_patient_count,coronavirus_admitted_todate,coronavirus_released_todate,coronavirus_mortality_todate";
}

CURRENT_PATIENT_COUNT *hospital_current_patient_count_parse(
				char *input_line )
{
	CURRENT_PATIENT_COUNT *c;
	char piece_buffer[ 512 ];

	c = hospital_current_patient_count_new();

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 0 );
	c->date_time_greenwich = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 1 );
	c->non_coronavirus_current_patient_count = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 2 );
	c->coronavirus_admitted_todate = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 3 );
	c->coronavirus_released_todate = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 4 );
	c->coronavirus_mortality_todate = atoi( piece_buffer );

	return c;

} /* hospital_current_patient_count_parse() */

LIST *hospital_current_patient_count_list(
				char *application_name,
				char *hospital_name,
				char *street_address )
{
	char sys_string[ 1024 ];
	char input_line[ 1024 ];
	FILE *input_pipe;
	LIST *current_patient_count_list = list_new();
	CURRENT_PATIENT_COUNT *c;

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		"
		 "			order=%s		",
		 application_name,
		 /* ----------------------- */
		 /* Returns program memory. */
		 /* ----------------------- */
		 hospital_current_patient_count_select(),
		 "current_patient_count",
		 /* -------------------- */
		 /* Returns heap memory. */
		 /* -------------------- */
		 hospital_where(
			hospital_name,
			street_address ),
		 "greenwich_mean" );

	input_pipe = popen( sys_string, "r" );

	while ( get_line( input_line, input_pipe ) )
	{
		c = hospital_current_patient_count_parse( input_line );

		list_append_pointer( current_patient_count_list, c );
	}

	pclose( input_pipe );

	return current_patient_count_list;

} /* hospital_current_patient_count_list() */

int hospital_admitted_delta(	LIST *current_patient_count_list )
{
	CURRENT_PATIENT_COUNT *current_patient_count;
	CURRENT_PATIENT_COUNT *prior_patient_count;
	int length;
	int return_value;
	LIST *l;

	l = current_patient_count_list;
	length = list_length( l );

	if ( !length ) return 0;

	if ( length == 1 )
	{
		current_patient_count = list_get_first_pointer( l );

		return_value =
			current_patient_count->
				coronavirus_admitted_todate;
	}
	else
	/* ---------- */
	/* length > 1 */
	/* ---------- */
	{
		list_go_last( l );
		current_patient_count = list_get_pointer( l );

		list_previous( l );
		prior_patient_count = list_get_pointer( l );

		return_value =
			current_patient_count->
				coronavirus_admitted_todate -
			prior_patient_count->
				coronavirus_admitted_todate;
	}

	return return_value;

} hospital_admitted_delta() */

int hospital_released_delta(	LIST *current_patient_count_list )
{
	CURRENT_PATIENT_COUNT *current_patient_count;
	CURRENT_PATIENT_COUNT *prior_patient_count;
	int length;
	int return_value;
	LIST *l;

	l = current_patient_count_list;
	length = list_length( l );

	if ( !length ) return 0;

	if ( length == 1 )
	{
		current_patient_count = list_get_first_pointer( l );

		return_value =
			current_patient_count->
				coronavirus_released_todate;
	}
	else
	/* ---------- */
	/* length > 1 */
	/* ---------- */
	{
		list_go_last( l );
		current_patient_count = list_get_pointer( l );

		list_previous( l );
		prior_patient_count = list_get_pointer( l );

		return_value =
			current_patient_count->
				coronavirus_released_todate -
			prior_patient_count->
				coronavirus_released_todate;
	}

	return return_value;

} hospital_released_delta() */

int hospital_mortality_delta(	LIST *current_patient_count_list )
{
	CURRENT_PATIENT_COUNT *current_patient_count;
	CURRENT_PATIENT_COUNT *prior_patient_count;
	int length;
	int return_value;
	LIST *l;

	l = current_patient_count_list;
	length = list_length( l );

	if ( !length ) return 0;

	if ( length == 1 )
	{
		current_patient_count = list_get_first_pointer( l );

		return_value =
			current_patient_count->
				coronavirus_mortality_todate;
	}
	else
	/* ---------- */
	/* length > 1 */
	/* ---------- */
	{
		list_go_last( l );
		current_patient_count = list_get_pointer( l );

		list_previous( l );
		prior_patient_count = list_get_pointer( l );

		return_value =
			current_patient_count->
				coronavirus_mortality_todate -
			prior_patient_count->
				coronavirus_mortality_todate;
	}

	return return_value;

} hospital_mortality_delta() */

int hospital_coronavirus_current_patient_count(
			LIST *current_patient_count_list )
{
	int admitted_delta;
	int released_delta;
	int mortality_delta;
	int current_patient_count;

	if ( !list_length( current_patient_count_list ) ) return 0;

	admitted_delta =
		hospital_admitted_delta(
			current_patient_count_list );

	released_delta =
		hospital_released_delta(
			current_patient_count_list );

	mortality_delta =
		hospital_mortality_delta(
			current_patient_count_list );

	current_patient_count =
		admitted_delta -
		( released_delta + mortality_delta );

	return current_patient_count;

} /* hospital_coronavirus_current_patient_count() */

int hospital_coronavirus_patient_daily_change(
			boolean *coronavirus_patient_daily_change_isnull,
			LIST *current_patient_count_list )
{
} /* hospital_coronavirus_patient_daily_change() */

int hospital_coronavirus_released_daily_change(
			boolean *coronavirus_released_daily_change_isnull,
			LIST *current_patient_count_list )
{
} /* hospital_coronavirus_released_daily_change() */

int hospital_coronavirus_mortality_daily_change(
			boolean *coronavirus_mortality_daily_change_isnull,
			LIST *current_patient_count_list )
{
} /* hospital_coronavirus_mortality_daily_change() */

