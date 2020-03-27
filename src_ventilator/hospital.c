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

	if ( !list_get_last_pointer( current_bed_usage_list ) )
	{
		*isnull = 1;
		return 0;
	}

	full_percent =
		( (double)current_bed_usage->regular_bed_occupied_count /
		  (double)regular_bed_capacity ) * 100.0;

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

	if ( !list_get_last_pointer( current_bed_usage_list ) )
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
"hospital_name,street_address,city,state_code,zip_code,zip4,telephone,regular_bed_capacity,ICU_bed_capacity,active_yn,regular_bed_occupied_percent,ICU_bed_occupied_percent,beds_without_ventilators,hospital_type,owner_type,helipad_yn,latitude,longitude,population_thousands,hospital_website";
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

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 2 );
	hospital->city = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 3 );
	hospital->state_code = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 4 );
	hospital->zip_code = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 5 );
	hospital->zip4 = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 6 );
	hospital->telephone = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 7 );
	hospital->regular_bed_capacity = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 8 );
	hospital->ICU_bed_capacity = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 9 );
	hospital->active = ( *piece_buffer == 'y' );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 10 );
	hospital->regular_bed_occupied_percent = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 11 );
	hospital->ICU_bed_occupied_percent = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 12 );
	hospital->beds_without_ventilators = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 13 );
	hospital->hospital_type = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 14 );
	hospital->owner_type = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 15 );
	hospital->helipad = ( *piece_buffer == 'y' );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 16 );
	hospital->latitude = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 17 );
	hospital->longitude = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 18 );
	hospital->population_thousands = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 19 );
	hospital->hospital_website = strdup( piece_buffer );

	return hospital;

} /* hospital_parse() */

/* Returns heap memory. */
/* -------------------- */
char *hospital_where(		char *hospital_name,
				char *street_address )
{
	char where[ 1024 ];
	char buffer[ 512 ];

	sprintf( where,
		 "hospital_name = '%s' and	"
		 "street_address = '%s		",
		 escape_character(	buffer,
					hospital_name,
					'\'' ),
		 street_address );

	return strdup( where );

} /* hospital_where() */

HOSPITAL *hospital_fetch(	char *application_name,
				char *hospital_name,
				char *street_address )
{
	char sys_string[ 1024 ];
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

	input_pipe = popen( sys_string, "r" );

	if ( get_line( input_line, input_pipe ) )
	{
		hospital = hospital_parse( input_line );

		hospital->current_bed_usage_list =
			hospital_current_bed_usage_list(
				application_name,
				hospital->hospital_name,
				hospital->street_address );
	}

	pclose( input_pipe );
	return hospital;

} /* hospital_fetch() */

void hospital_update(		char *application_name,
				char *hospital_name,
				char *street_address,
				boolean regular_bed_occupied_percent_isnull,
				int regular_bed_occupied_percent,
				boolean ICU_bed_occupied_percent_isnull,
				int ICU_bed_occupied_percent,
				boolean beds_without_ventilators_isnull,
				int beds_without_ventilators )
{
	char sys_string[ 1024 ];
	FILE *update_pipe;
	char output_buffer[ 128 ];

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
		 "hospital",
		 "hospital_name,street_address" );

	update_pipe = popen( sys_string, "w" );

	/* Regular Bed Occupied Percent */
	/* ---------------------------- */
	if ( regular_bed_occupied_percent_isnull )
		strcpy( output_buffer, "null" );
	else
		sprintf( output_buffer, "%d", regular_bed_occupied_percent );

	fprintf( update_pipe,
		 "%s^%s^regular_bed_occupied_percent^%s\n",
		 hospital_name,
		 street_address,
		 output_buffer );

	/* ICU Bed Occupied Percent */
	/* ------------------------ */
	if ( ICU_bed_occupied_percent_isnull )
		strcpy( output_buffer, "null" );
	else
		sprintf( output_buffer, "%d", ICU_bed_occupied_percent );

	fprintf( update_pipe,
		 "%s^%s^ICU_bed_occupied_percent^%s\n",
		 hospital_name,
		 street_address,
		 output_buffer );

	/* Beds Without Ventilators */
	/* ------------------------ */
	if ( beds_without_ventilators_isnull )
		strcpy( output_buffer, "null" );
	else
		sprintf( output_buffer, "%d", beds_without_ventilators );

	fprintf( update_pipe,
		 "%s^%s^beds_without_ventilators^%s\n",
		 hospital_name,
		 street_address,
		 output_buffer );

	pclose( update_pipe );

} /* hospital_update() */

char *hospital_current_bed_usage_select( void )
{
	return
"date_time_zulu,regular_bed_occupied_count,ICU_bed_occupied_count";
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
		 "			order=date_time_zulu	",
		 application_name,
		 /* ----------------------- */
		 /* Returns program memory. */
		 /* ----------------------- */
		 hospital_current_bed_usage_select(),
		 /* -------------------- */
		 /* Returns heap memory. */
		 /* -------------------- */
		 hospital_where(
			hospital_name,
			street_address ),
		 "current_bed_usage" );

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
	current_bed_usage->date_time_zulu = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 1 );
	current_bed_usage->regular_bed_occupied_count = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 2 );
	current_bed_usage->ICU_bed_occupied_count = atoi( piece_buffer );

	return current_bed_usage;

} /* hospital_current_bed_usage_parse() */

char *hospital_current_ventilator_count_select( void )
{
	return
"date_time_zulu,current_ventilator_count";
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
		 "			order=date_time_zulu	",
		 application_name,
		 /* ----------------------- */
		 /* Returns program memory. */
		 /* ----------------------- */
		 hospital_current_ventilator_count_select(),
		 /* -------------------- */
		 /* Returns heap memory. */
		 /* -------------------- */
		 hospital_where(
			hospital_name,
			street_address ),
		 "current_ventilator_count" );

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
	current_ventilator_count->date_time_zulu = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_line, 1 );
	current_ventilator_count->current_ventilator_count =
		atoi( piece_buffer );

	return current_ventilator_count;

} /* hospital_current_ventilator_count_parse() */

