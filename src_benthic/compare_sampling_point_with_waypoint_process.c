/* --------------------------------------------------------------*/
/* src_benthic/compare_sampling_point_with_waypoint_process.c	 */
/* --------------------------------------------------------------*/
/* Freely available software: see Appaserver.org	 	 */
/* --------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"
#include "date_convert.h"
#include "piece.h"
#include "query.h"
#include "list.h"
#include "dictionary.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "hash_table.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "benthic_library.h"
#include "waypoint.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
LIST *get_utm_distance_list(	char *application_name,
				LIST *collection_list,
				LIST *project_list,
				HASH_TABLE *candidate_hash_table );

void get_both_waypoint_lists(	LIST **waypoint1_list,
				LIST **waypoint2_list,
				char *application_name,
				LIST *collection_list,
				LIST *project_list,
				HASH_TABLE *candidate_hash_table );

void parse_input_buffer(	char *collection_name,
				char *location,
				char *site_number,
				char *anchor_date,
				char *anchor_time,
				char *actual_latitude,
				char *actual_longitude,
				char *sample_number,
				char *input_buffer );

char *get_input_sys_string(
				char *application_name,
				LIST *collection_list,
				LIST *project_list );

char *get_query_or_sequence_where_clause(
				char *application_name,
				LIST *collection_list,
				LIST *project_list );

void output_compare_waypoint_process(
				char *application_name,
				LIST *collection_list,
				LIST *project_list,
				char delimiter,
				HASH_TABLE *candidate_hash_table,
				LIST *utm_distance_list );

int main( int argc, char **argv )
{
	char *application_name;
	char *database_string = {0};
	char *collection_list_string;
	char *project_list_string;
	LIST *collection_list;
	LIST *project_list;
	char delimiter;
	HASH_TABLE *candidate_hash_table;
	LIST *utm_distance_list;

	if ( argc != 5 )
	{
		fprintf( stderr, 
"Usage: %s application collection_list project_list delimiter\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	collection_list_string = argv[ 2 ];
	project_list_string = argv[ 3 ];
	delimiter = *argv[ 4 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

/*
	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );
*/

	collection_list = list_string2list( collection_list_string, ',' );
	project_list = list_string2list( project_list_string, ',' );

	if ( !*collection_list_string
	||   strcmp( collection_list_string, "collection_name" ) == 0 )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: no collection_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	candidate_hash_table =
		benthic_get_sampling_point_candidate_hash_table(
				application_name,
				collection_list );

	if ( ! ( utm_distance_list =
		get_utm_distance_list(	application_name,
					collection_list,
					project_list,
					candidate_hash_table ) ) )
	{
		printf( "Error: no candidates for this collection.\n" );
		exit( 0 );
	}

	output_compare_waypoint_process(
				application_name,
				collection_list,
				project_list,
				delimiter,
				candidate_hash_table,
				utm_distance_list );

	return 0;
}

void output_compare_waypoint_process(
				char *application_name,
				LIST *collection_list,
				LIST *project_list,
				char delimiter,
				HASH_TABLE *candidate_hash_table,
				LIST *utm_distance_list )
{
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char *sys_string;
	char collection_name[ 128 ];
	char location[ 128 ];
	char site_number[ 128 ];
	char anchor_date[ 128 ];
	char anchor_time[ 128 ];
	char actual_latitude[ 128 ];
	char actual_longitude[ 128 ];
	char sample_number[ 128 ];
	char *sampling_point_candidate_key;
	SAMPLING_POINT_CANDIDATE *sampling_point_candidate;
	double latitude;
	double longitude;
	int *distance;

	sys_string =
		get_input_sys_string(
			application_name,
			collection_list,
			project_list );

	input_pipe = popen( sys_string, "r" );
	list_rewind( utm_distance_list );

	while( get_line( input_buffer, input_pipe ) )
	{
		distance = list_get_pointer( utm_distance_list );

		parse_input_buffer(
				collection_name,
				location,
				site_number,
				anchor_date,
				anchor_time,
				actual_latitude,
				actual_longitude,
				sample_number,
				input_buffer );

		printf( "%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s",
			collection_name,
			delimiter,
			location,
			delimiter,
			site_number,
			delimiter,
			anchor_date,
			delimiter,
			anchor_time,
			delimiter,
			actual_latitude,
			delimiter,
			actual_longitude,
			delimiter,
			sample_number );

		sampling_point_candidate_key =
			benthic_get_sampling_point_candidate_key(
				location,
				collection_name,
				atoi( site_number ),
				atoi( sample_number ) );

		if ( ! (sampling_point_candidate =
			hash_table_fetch(
				candidate_hash_table,
				sampling_point_candidate_key ) ) )
		{
			printf( "\n" );
		}
		else
		{
			if ( !waypoint_convert_degrees_float_latitude_longitude(
				&latitude,
				&longitude,
				sampling_point_candidate->latitude,
				sampling_point_candidate->longitude ) )
			{
				latitude = 0.0;
				longitude = 0.0;
			}

			printf( "%c%.5lf%c%.5lf%c%d\n",
				delimiter,
				latitude,
				delimiter,
				longitude,
				delimiter,
				*distance );
		}

		list_next( utm_distance_list );
	}

	pclose( input_pipe );

}

char *get_query_or_sequence_where_clause(
				char *application_name,
				LIST *collection_list,
				LIST *project_list )
{
	LIST *attribute_name_list;
	char *sampling_point_table_name;
	char attribute_name[ 128 ];
	QUERY_OR_SEQUENCE *query_or_sequence;

	attribute_name_list = list_new();

	sampling_point_table_name =
		get_table_name(
			application_name, "sampling_point" );

	sprintf(	attribute_name,
			"%s.collection_name",
			sampling_point_table_name );

	list_append_pointer( attribute_name_list, strdup( attribute_name ) );

	sprintf(	attribute_name,
			"%s.project",
			sampling_point_table_name );

	list_append_pointer( attribute_name_list, strdup( attribute_name ) );

	query_or_sequence = query_or_sequence_new( attribute_name_list );

	query_or_sequence_set_data_list(
			query_or_sequence->data_list_list,
			collection_list );

	query_or_sequence_set_data_list(
			query_or_sequence->data_list_list,
			project_list );

	return query_or_sequence_where_clause(
				query_or_sequence->attribute_name_list,
				query_or_sequence->data_list_list,
				0 /* not with_and_prefix */ );
}


char *get_input_sys_string(
			char *application_name,
			LIST *collection_list,
			LIST *project_list )
{
	char sys_string[ 4096 ];
	char *where_or_clause;
	char where[ 4096 ];
	char where_sampling_site_attempt_success_join[ 1024 ];
	char select[ 1024 ];
	char *sampling_point_table_name;
	char *sampling_site_attempt_success_table_name;
	char *folder_clause;

	sampling_point_table_name =
		get_table_name( application_name,
				"sampling_point" );

	sampling_site_attempt_success_table_name =
		get_table_name( application_name,
				"sampling_site_attempt_success" );

	sprintf( select,
"collection_name,%s.location,lpad(%s.site_number,2,'0'),date_format(%s.anchor_date,'%cc/%ce/%cY'),%s.anchor_time,%s.actual_latitude,%s.actual_longitude,sample_number",
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 '%', '%', '%',
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name );

	folder_clause = "sampling_site_attempt_success,sampling_point";

	where_or_clause =
		get_query_or_sequence_where_clause(
			application_name,
			collection_list,
			project_list );

	sprintf( where_sampling_site_attempt_success_join,
		 "%s.anchor_date = %s.anchor_date and		"
		 "%s.anchor_time = %s.anchor_time and		"
		 "%s.location = %s.location and			"
		 "%s.site_number = %s.site_number		",
		 sampling_point_table_name,
		 sampling_site_attempt_success_table_name,
		 sampling_point_table_name,
		 sampling_site_attempt_success_table_name,
		 sampling_point_table_name,
		 sampling_site_attempt_success_table_name,
		 sampling_point_table_name,
		 sampling_site_attempt_success_table_name );

	sprintf( where,
		 "%s and %s and attempt_success = 'sampled'",
		 where_or_clause,
		 where_sampling_site_attempt_success_join );

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=\"%s\"		 "
		 "			folder=%s		 "
		 "			where=\"%s\"		 "
		 "			order=select		|"
		 "cat						 ",
		 application_name,
		 select,
		 folder_clause,
		 where );

	return strdup( sys_string );

}

void parse_input_buffer(	char *collection_name,
				char *location,
				char *site_number,
				char *anchor_date,
				char *anchor_time,
				char *actual_latitude,
				char *actual_longitude,
				char *sample_number,
				char *input_buffer )
{
	piece(	collection_name,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		0 );

	piece(	location,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		1 );

	piece(	site_number,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		2 );

	piece(	anchor_date,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		3 );

	piece(	anchor_time,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		4 );

	piece(	actual_latitude,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		5 );

	piece(	actual_longitude,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		6 );

	piece(	sample_number,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		7 );

}

void get_both_waypoint_lists(	LIST **waypoint1_list,
				LIST **waypoint2_list,
				char *application_name,
				LIST *collection_list,
				LIST *project_list,
				HASH_TABLE *candidate_hash_table )
{
	WAYPOINT *waypoint;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char *sys_string;
	char collection_name[ 128 ];
	char location[ 128 ];
	char site_number[ 128 ];
	char anchor_date[ 128 ];
	char anchor_time[ 128 ];
	char actual_latitude[ 128 ];
	char actual_longitude[ 128 ];
	char sample_number[ 128 ];
	char *sampling_point_candidate_key;
	SAMPLING_POINT_CANDIDATE *sampling_point_candidate;

	sys_string =
		get_input_sys_string(
			application_name,
			collection_list,
			project_list );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		parse_input_buffer(
				collection_name,
				location,
				site_number,
				anchor_date,
				anchor_time,
				actual_latitude,
				actual_longitude,
				sample_number,
				input_buffer );

		waypoint = waypoint_new();
		list_append_pointer( *waypoint1_list, waypoint );

		waypoint->latitude = atof( actual_latitude );
		waypoint->longitude = atof( actual_longitude );

		waypoint = waypoint_new();
		list_append_pointer( *waypoint2_list, waypoint );

		sampling_point_candidate_key =
			benthic_get_sampling_point_candidate_key(
				location,
				collection_name,
				atoi( site_number ),
				atoi( sample_number ) );

		if ( ( sampling_point_candidate =
			hash_table_fetch(
				candidate_hash_table,
				sampling_point_candidate_key ) ) )
		{
			waypoint->latitude_degrees_float =
				sampling_point_candidate->latitude,
			waypoint->longitude_degrees_float =
				sampling_point_candidate->longitude;

			if ( !waypoint_convert_degrees_float_latitude_longitude(
				&waypoint->latitude,
				&waypoint->longitude,
				waypoint->latitude_degrees_float,
				waypoint->longitude_degrees_float ) )
			{
				fprintf( stderr,
	"Warning in %s/%s()/%d: cannot convert degrees float: (%s/%s).\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__,
					 waypoint->latitude_degrees_float,
					 waypoint->longitude_degrees_float );
			}
		}
	}

	pclose( input_pipe );

}

LIST *get_utm_distance_list(	char *application_name,
				LIST *collection_list,
				LIST *project_list,
					HASH_TABLE *candidate_hash_table )
{
	LIST *waypoint1_list = list_new();
	LIST *waypoint2_list = list_new();

	get_both_waypoint_lists(&waypoint1_list,
				&waypoint2_list,
				application_name,
				collection_list,
				project_list,
				candidate_hash_table );

	if ( !list_length( waypoint1_list )
	||   list_length( waypoint2_list ) != list_length( waypoint2_list ) )
	{
		return (LIST *)0;
	}

	waypoint_list_convert_to_utm( waypoint1_list );
	waypoint_list_convert_to_utm( waypoint2_list );

	return waypoint_list_get_utm_distance_list(
		waypoint1_list,
		waypoint2_list );

}

