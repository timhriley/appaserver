/* ---------------------------------------------------	*/
/* src_benthic/sweep.c					*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sweep.h"
#include "sampling_point.h"
#include "timlib.h"
#include "appaserver_library.h"
#include "piece.h"

SWEEP *sweep_new( int sweep_number )
{
	SWEEP *sweep;

	if ( ! ( sweep = calloc( 1, sizeof( SWEEP ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sweep->sweep_number = sweep_number;
	return sweep;
}

SWEEP_LIST *sweep_list_new(	char *application_name,
				char *anchor_date,
				char *anchor_time,
				char *location,
				int site_number,
				int replicate_number )
{
	SWEEP_LIST *sweep_list;

	if ( ! ( sweep_list = calloc( 1, sizeof( SWEEP_LIST ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sweep_list->non_primary_key_list =
		sweep_non_primary_key_list(
			1 /* with_total_minutes */ );

	sweep_list->sweep_list =
		sweep_sampling_point_sweep_list(
				application_name,
				anchor_date,
				anchor_time,
				location,
				site_number,
				replicate_number,
				sweep_list->non_primary_key_list );

	return sweep_list;
}

LIST *sweep_non_primary_key_list( boolean with_total_minutes )
{
	LIST *non_primary_key_list = list_new();

	list_append_pointer(	non_primary_key_list,
				"sweep_number" );
	list_append_pointer(	non_primary_key_list,
				"pick_date" );
	list_append_pointer(	non_primary_key_list,
				"quality_control_date" );
	list_append_pointer(	non_primary_key_list,
				"wash_time_minutes" );
	list_append_pointer(	non_primary_key_list,
				"pick_time_minutes" );
	list_append_pointer(	non_primary_key_list,
				"quality_control_time_minutes" );
	list_append_pointer(	non_primary_key_list,
				"sort_minutes" );
	list_append_pointer(	non_primary_key_list,
				"identify_penaeid_minutes" );
	list_append_pointer(	non_primary_key_list,
				"identify_fish_minutes" );
	list_append_pointer(	non_primary_key_list,
				"identify_caridean_minutes" );
	list_append_pointer(	non_primary_key_list,
				"identify_crabs_minutes" );

	if ( with_total_minutes )
	{
		list_append_pointer(	non_primary_key_list,
					SWEEP_TOTAL_MINUTES_KEY );
	}

	list_append_pointer(	non_primary_key_list,
				"pick_researcher" );
	list_append_pointer(	non_primary_key_list,
				"quality_control_researcher" );
	list_append_pointer(	non_primary_key_list,
				"laboratory_researcher" );
	list_append_pointer(	non_primary_key_list,
				"no_animals_found_yn" );
	list_append_pointer(	non_primary_key_list,
				"tag_missing_yn" );
	list_append_pointer(	non_primary_key_list,
				"notes" );

	return non_primary_key_list;

}

LIST *sweep_sampling_point_sweep_list(
				char *application_name,
				char *anchor_date,
				char *anchor_time,
				char *location,
				int site_number,
				int replicate_number,
				LIST *non_primary_key_list )
{
	char sys_string[ 2048 ];
	char select_string[ 1024 ];
	LIST *sweep_list = list_new();
	FILE *input_pipe;
	SWEEP *sweep;
	char input_buffer[ 2048 ];
	char piece_buffer[ 1024 ];
	char *primary_key_where_clause;
	char where_clause[ 1024 ];

	primary_key_where_clause =
		sampling_point_primary_key_where_clause(
			anchor_date,
			anchor_time,
			location,
			site_number );

	sprintf( where_clause,
		 "%s and replicate_number = %d",
		 primary_key_where_clause,
		 replicate_number );

	timlib_strcpy(	select_string,
		 	list_display_delimited(
				non_primary_key_list, ',' ),
			1024 );

	search_replace_string(
		select_string,
		SWEEP_TOTAL_MINUTES_KEY,
		sweep_minutes_sum_expression() );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=sweep		"
		 "			where=\"%s\"		",
		 application_name,
		 select_string,
		 where_clause );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			sweep_non_primary_key_list_piece_offset(
				non_primary_key_list,
				"sweep_number" ) );

		sweep = sweep_new( atoi( piece_buffer ) );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			sweep_non_primary_key_list_piece_offset(
				non_primary_key_list,
				"pick_date" ) );
		sweep->pick_date = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			sweep_non_primary_key_list_piece_offset(
				non_primary_key_list,
				"quality_control_date" ) );
		sweep->quality_control_date = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			sweep_non_primary_key_list_piece_offset(
				non_primary_key_list,
				"wash_time_minutes" ) );
		sweep->wash_time_minutes = atoi( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			sweep_non_primary_key_list_piece_offset(
				non_primary_key_list,
				"pick_time_minutes" ) );
		sweep->pick_time_minutes = atoi( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			sweep_non_primary_key_list_piece_offset(
				non_primary_key_list,
				"quality_control_time_minutes" ) );
		sweep->quality_control_time_minutes = atoi( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			sweep_non_primary_key_list_piece_offset(
				non_primary_key_list,
				"sort_minutes" ) );
		sweep->sort_minutes = atoi( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			sweep_non_primary_key_list_piece_offset(
				non_primary_key_list,
				"identify_penaeid_minutes" ) );
		sweep->identify_penaeid_minutes = atoi( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			sweep_non_primary_key_list_piece_offset(
				non_primary_key_list,
				"identify_fish_minutes" ) );
		sweep->identify_fish_minutes = atoi( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			sweep_non_primary_key_list_piece_offset(
				non_primary_key_list,
				"identify_caridean_minutes" ) );
		sweep->identify_caridean_minutes = atoi( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			sweep_non_primary_key_list_piece_offset(
				non_primary_key_list,
				"identify_crabs_minutes" ) );
		sweep->identify_crabs_minutes = atoi( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			sweep_non_primary_key_list_piece_offset(
				non_primary_key_list,
				SWEEP_TOTAL_MINUTES_KEY ) );
		sweep->total_minutes = atoi( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			sweep_non_primary_key_list_piece_offset(
				non_primary_key_list,
				"pick_researcher" ) );
		sweep->pick_researcher = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			sweep_non_primary_key_list_piece_offset(
				non_primary_key_list,
				"quality_control_researcher" ) );
		sweep->quality_control_researcher = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			sweep_non_primary_key_list_piece_offset(
				non_primary_key_list,
				"laboratory_researcher" ) );
		sweep->laboratory_researcher = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			sweep_non_primary_key_list_piece_offset(
				non_primary_key_list,
				"no_animals_found_yn" ) );
		sweep->no_animals_found_yn = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			sweep_non_primary_key_list_piece_offset(
				non_primary_key_list,
				"tag_missing_yn" ) );
		sweep->tag_missing_yn = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			sweep_non_primary_key_list_piece_offset(
				non_primary_key_list,
				"notes" ) );
		sweep->notes = strdup( piece_buffer );

		list_append_pointer( sweep_list, sweep );
	}

	pclose( input_pipe );
	return sweep_list;

}

int sweep_non_primary_key_list_piece_offset(
			LIST *non_primary_key_list,
			char *attribute_name )
{
	char *search_attribute_name;
	int piece_offset = 0;

	if ( !list_rewind( non_primary_key_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty non_primary_key_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	do {
		search_attribute_name =
			list_pointer( 
				non_primary_key_list );

		if ( timlib_strcmp(	search_attribute_name,
					attribute_name ) == 0 )
		{
			return piece_offset;
		}

		piece_offset++;
	} while( list_next( non_primary_key_list ) );

	fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot find (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
		attribute_name );
	exit( 0 );
}

char *sweep_minutes_sum_expression( void )
{
	return	"ifnull(wash_time_minutes,0) + 			"
		"ifnull(pick_time_minutes,0) +			"
		"ifnull(quality_control_time_minutes,0) +	"
		"ifnull(sort_minutes,0) +			"
		"ifnull(identify_penaeid_minutes,0) +		"
		"ifnull(identify_fish_minutes,0) +		"
		"ifnull(identify_caridean_minutes,0) +		"
		"ifnull(identify_crabs_minutes,0)		";

}

