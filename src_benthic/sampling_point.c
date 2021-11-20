/* ---------------------------------------------------	*/
/* src_benthic/sampling_point.c				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "folder.h"
#include "sampling_point.h"
#include "piece.h"

SAMPLING_POINT *sampling_point_new(
			char *anchor_date,
			char *anchor_time,
			char *location,
			int site_number )
{
	SAMPLING_POINT *sampling_point;

	if ( ! ( sampling_point = calloc( 1, sizeof( SAMPLING_POINT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sampling_point->anchor_date = anchor_date;
	sampling_point->anchor_time = anchor_time;
	sampling_point->location = location;
	sampling_point->site_number = site_number;
	return sampling_point;
}

boolean sampling_point_load(
			char **collection_name,
			char **actual_latitude,
			char **actual_longitude,
			char **boat_lead_researcher,
			char **second_boat_researcher,
			char **collection_diver1,
			char **collection_diver2,
			char **begin_collection_dive_time,
			char **end_collection_dive_time,
			char **collection_air_pressure_in_PSI,
			char **collection_air_pressure_out_PSI,
			char **begin_habitat_dive_time,
			char **end_habitat_dive_time,
			char **habitat_diver,
			char **habitat_air_pressure_in_PSI,
			char **habitat_air_pressure_out_PSI,
			char **notes,
			char **bucket_number,
			char **tide,
			char **project,
			SAMPLING_POINT *sampling_point,
			char *application_name )
{
	char sys_string[ 1024 ];
	char where_clause[ 1024 ];
	char *select;
	char *results;
	char piece_buffer[ 128 ];

	select =
"collection_name,actual_latitude,actual_longitude,boat_lead_researcher,second_boat_researcher,collection_diver1,collection_diver2,begin_collection_dive_time,end_collection_dive_time,collection_air_pressure_in_PSI,collection_air_pressure_out_PSI,begin_habitat_dive_time,end_habitat_dive_time,habitat_diver,habitat_air_pressure_in_PSI,habitat_air_pressure_out_PSI,notes,bucket_number,tide,project";

	sprintf( where_clause,
		 "anchor_date = '%s' and			"
		 "anchor_time = '%s' and			"
		 "location = '%s' and				"
		 "site_number = %d				",
		 sampling_point->anchor_date,
		 sampling_point->anchor_time,
		 sampling_point->location,
		 sampling_point->site_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=sampling_point	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where_clause );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0;

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 0 );
	*collection_name = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 1 );
	*actual_latitude = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 2 );
	*actual_longitude = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 3 );
	*boat_lead_researcher = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 4 );
	*second_boat_researcher = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 5 );
	*collection_diver1 = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 6 );
	*collection_diver2 = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 7 );
	*begin_collection_dive_time = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 8 );
	*end_collection_dive_time = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 9 );
	*collection_air_pressure_in_PSI = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 10 );
	*collection_air_pressure_out_PSI = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 11 );
	*begin_habitat_dive_time = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 12 );
	*end_habitat_dive_time = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 13 );
	*habitat_diver = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 14 );
	*habitat_air_pressure_in_PSI = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 15 );
	*habitat_air_pressure_out_PSI = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 16 );
	*notes = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 17 );
	*bucket_number = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 18 );
	*tide = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 19 );
	*project = strdup( piece_buffer );

	return 1;
}

char *sampling_point_get_primary_key_where_clause(
			char *anchor_date,
			char *anchor_time,
			char *location,
			int site_number )
{
	static char where_clause[ 1024 ] = {0};

	if ( *where_clause ) return where_clause;

	sprintf( where_clause,
		 "anchor_date = '%s' and		"
		 "anchor_time = '%s' and		"
		 "location = '%s' and			"
		 "site_number = %d 			",
		 anchor_date,
		 anchor_time,
		 location,
		 site_number );

	return where_clause;

}

