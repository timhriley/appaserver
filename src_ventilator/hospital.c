/* --------------------------------------------- */
/* $APPASERVER_HOME/src_ventilator/hospital.c	 */
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

int hospital_ventilator_bed_occupied_percent(
				boolean *isnull,
				int ventilator_bed_capacity,
				LIST *current_bed_usage_list )
{
	CURRENT_BED_USAGE *current_bed_usage;
	double full_percent;

	*isnull = 0;

	if ( !ventilator_bed_capacity )
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
		( (double)current_bed_usage->ventilator_bed_occupied_count /
		  (double)ventilator_bed_capacity ) * 100.0;

	return (int)timlib_round_double( full_percent );

}  /* hospital_bed_occupied_percent() */

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
			hospital_current_ventilator_count_list(
				application_name,
				hospital->hospital_name,
				hospital->street_address );

		hospital->current_bed_capacity_list =
			hospital_current_bed_capacity_list(
				application_name,
				hospital->hospital_name,
				hospital->street_address );
	}

	pclose( input_pipe );

	return hospital;

} /* hospital_fetch() */

void hospital_update(
	char *application_name,
	char *hospital_name,
	char *street_address,

	boolean current_ventilator_ICU_bed_occupied_count_isnull,
	int current_ventilator_ICU_bed_occupied_count,

	boolean current_ventilator_count_isnull,
	int current_ventilator_count,

	boolean necessary_beds_without_ventilators_isnull,
	int necessary_beds_without_ventilators,

	boolean ventilator_bed_occupied_percent_isnull,
	int ventilator_bed_occupied_percent,

	boolean ICU_bed_occupied_percent_isnull,
	int ICU_bed_occupied_percent,

	boolean current_ventilator_bed_occupied_count_isnull,
	int current_ventilator_bed_occupied_count,

	boolean current_ICU_bed_occupied_count_isnull,
	int current_ICU_bed_occupied_count )
{
	char sys_string[ 1024 ];
	FILE *update_pipe;
	char output_buffer[ 128 ];

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 "hospital",
		 "hospital_name,street_address" );

	update_pipe = popen( sys_string, "w" );

	/* Current Ventilator ICU Bed Occupied Count */
	/* ----------------------------------------- */
	if ( current_ventilator_ICU_bed_occupied_count_isnull )
		*output_buffer = '\0';
	else
		sprintf(output_buffer,
			"%d",
			current_ventilator_ICU_bed_occupied_count );

	fprintf( update_pipe,
		 "%s^%s^current_ventilator_ICU_bed_occupied_count^%s\n",
		 hospital_name,
		 street_address,
		 output_buffer );

	/* Current Ventilator Count */
	/* ------------------------ */
	if ( current_ventilator_count_isnull )
		*output_buffer = '\0';
	else
		sprintf(output_buffer,
			"%d",
			current_ventilator_count );

	fprintf( update_pipe,
		 "%s^%s^current_ventilator_count^%s\n",
		 hospital_name,
		 street_address,
		 output_buffer );

	/* Necessary Beds Without Ventilators */
	/* ---------------------------------- */
	if ( necessary_beds_without_ventilators_isnull )
		*output_buffer = '\0';
	else
		sprintf(output_buffer,
			"%d",
			necessary_beds_without_ventilators );

	fprintf( update_pipe,
		 "%s^%s^necessary_beds_without_ventilators^%s\n",
		 hospital_name,
		 street_address,
		 output_buffer );

	/* Ventilator Bed Occupied Percent */
	/* ------------------------------- */
	if ( ventilator_bed_occupied_percent_isnull )
		*output_buffer = '\0';
	else
		sprintf(output_buffer,
			"%d",
			ventilator_bed_occupied_percent );

	fprintf( update_pipe,
		 "%s^%s^ventilator_bed_occupied_percent^%s\n",
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

	/* Current Ventilator Bed Occupied Count */
	/* ------------------------------------- */
	if ( current_ventilator_bed_occupied_count_isnull )
		*output_buffer = '\0';
	else
		sprintf(output_buffer,
			"%d",
			current_ventilator_bed_occupied_count );

	fprintf( update_pipe,
		 "%s^%s^current_ventilator_bed_occupied_count^%s\n",
		 hospital_name,
		 street_address,
		 output_buffer );

	/* Current ICU Bed Occupied Count */
	/* ------------------------------ */
	if ( current_ICU_bed_occupied_count_isnull )
		*output_buffer = '\0';
	else
		sprintf(output_buffer,
			"%d",
			current_ICU_bed_occupied_count );

	fprintf( update_pipe,
		 "%s^%s^current_ICU_bed_occupied_count^%s\n",
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
		 "			order=date_time_greenwich	",
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
			street_address ) );


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
	current_bed_usage->ventilator_bed_occupied_count = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 2 );
	current_bed_usage->ICU_bed_occupied_count = atoi( piece_buffer );

	return current_bed_usage;

} /* hospital_current_bed_usage_parse() */

char *hospital_current_ventilator_count_select( void )
{
	return
"date_time_greenwich,current_ventilator_count";
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
	CURRENT_VENTILATOR_COUNT *h;

	if ( ! ( h = (CURRENT_VENTILATOR_COUNT *)
			calloc( 1, sizeof( CURRENT_VENTILATOR_COUNT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return h;

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
	current_ventilator_count->current_ventilator_count =
		atoi( piece_buffer );

	return current_ventilator_count;

} /* hospital_current_ventilator_count_parse() */

int hospital_necessary_beds_without_ventilators(
				boolean *isnull,
				int ventilator_bed_capacity,
				int ICU_bed_capacity,
				LIST *current_ventilator_count_list )
{
	int total_beds;
	CURRENT_VENTILATOR_COUNT *current_ventilator_count;

	*isnull = 0;

	total_beds = ventilator_bed_capacity + ICU_bed_capacity;

	if ( !total_beds )
	{
		*isnull = 1;
		return 0;
	}

	if ( ! ( current_ventilator_count =
			list_get_last_pointer(
				current_ventilator_count_list ) ) )
	{
		*isnull = 1;
		return 0;
	}

	return total_beds - current_ventilator_count->current_ventilator_count;

} /* hospital_necessary_beds_without_ventilators() */

