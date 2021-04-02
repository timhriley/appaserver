/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_alligator/alligator.c		*/
/* ---------------------------------------------------	*/
/* This is the library for the alligator project.	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "hash_table.h"
#include "timlib.h"
#include "appaserver_library.h"
#include "date_convert.h"
#include "alligator.h"

/* Global variables */
/* ---------------- */
static LIST *transect_list = {0};

char *alligator_get_materials(
				char *application_name,
				char *materials_code )
{
	static LIST *materials_record_list = {0};
	static char materials[ 128 ];
	char local_materials_code[ 16 ];
	char *record;

	if ( !*materials_code ) return "";

	if ( !materials_record_list )
	{
		char sys_string[ 1024 ];
		char *select = "materials_code,materials";

		sprintf(
		sys_string,
		"get_folder_data	application=%s			 "
		"			select=%s			 "
		"			folder=materials",
		application_name,
		select );

		materials_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( materials_record_list ) )
	{
		fprintf( stderr,
		"Warning in %s/%s()/%d: empty materials_record_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (char *)0;
	}

	do {
		record = list_get_pointer( materials_record_list );

		piece(	local_materials_code,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		if ( strcasecmp(local_materials_code,
				materials_code ) == 0 )
		{
			piece(	materials,
				FOLDER_DATA_DELIMITER,
				record,
				1 );

			return materials;
		}

	} while( list_next( materials_record_list ) );

	return (char *)0;

} /* alligator_get_materials() */

char *alligator_get_basin_code(	char *application_name,
				char *basin_name )
{
	static LIST *basin_record_list = {0};
	static char basin_code[ 128 ];
	char local_basin_name[ 128 ];
	char *record;

	if ( !*basin_name ) return "";

	if ( !basin_record_list )
	{
		char sys_string[ 1024 ];
		char *select = "basin,basin_code";

		sprintf(
		sys_string,
		"get_folder_data	application=%s			 "
		"			select=%s			 "
		"			folder=basin",
		application_name,
		select );

		basin_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( basin_record_list ) )
	{
		fprintf( stderr,
		"Warning in %s/%s()/%d: empty basin_record_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return "";
	}

	do {
		record = list_get_pointer( basin_record_list );

		piece(	local_basin_name,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		if ( strcasecmp(local_basin_name,
				basin_name ) == 0 )
		{
			piece(	basin_code,
				FOLDER_DATA_DELIMITER,
				record,
				1 );

			return basin_code;
		}

	} while( list_next( basin_record_list ) );

	return "";

} /* alligator_get_basin_code() */

char *alligator_get_materials_code(	char *application_name,
					char *materials_name )
{
	static LIST *materials_record_list = {0};
	static char materials_code[ 128 ];
	char local_materials_name[ 128 ];
	char *record;

	if ( !*materials_name ) return "";

	if ( !materials_record_list )
	{
		char sys_string[ 1024 ];
		char *select = "materials,materials_code";

		sprintf(
		sys_string,
		"get_folder_data	application=%s			 "
		"			select=%s			 "
		"			folder=materials",
		application_name,
		select );

		materials_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( materials_record_list ) )
	{
		fprintf( stderr,
		"Warning in %s/%s()/%d: empty materials_record_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return "";
	}

	do {
		record = list_get_pointer( materials_record_list );

		piece(	local_materials_name,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		if ( strcasecmp(local_materials_name,
				materials_name ) == 0 )
		{
			piece(	materials_code,
				FOLDER_DATA_DELIMITER,
				record,
				1 );

			return materials_code;
		}

	} while( list_next( materials_record_list ) );

	return "";

} /* alligator_get_materials_code() */

char *alligator_get_basin(
				char *application_name,
				char *basin_code )
{
	static LIST *basin_record_list = {0};
	static char basin[ 128 ];
	char local_basin_code[ 16 ];
	char *record;

	if ( !*basin_code ) return "";

	if ( !basin_record_list )
	{
		char sys_string[ 1024 ];
		char *select = "basin_code,basin";

		sprintf(
		sys_string,
		"get_folder_data	application=%s			 "
		"			select=%s			 "
		"			folder=basin",
		application_name,
		select );

		basin_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( basin_record_list ) )
	{
		fprintf( stderr,
		"Warning in %s/%s()/%d: empty basin_record_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (char *)0;
	}

	do {
		record = list_get_pointer( basin_record_list );

		piece(	local_basin_code,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		if ( strcasecmp(local_basin_code,
				basin_code ) == 0 )
		{
			piece(	basin,
				FOLDER_DATA_DELIMITER,
				record,
				1 );

			return basin;
		}

	} while( list_next( basin_record_list ) );

	return (char *)0;

} /* alligator_get_basin() */

char *alligator_get_nest_status(
				char *application_name,
				char *nest_status_code )
{
	static LIST *nest_status_record_list = {0};
	static char nest_status[ 128 ];
	char local_nest_status_code[ 16 ];
	char *record;

	if ( !*nest_status_code ) return "";

	if ( !nest_status_record_list )
	{
		char sys_string[ 1024 ];
		char *select = "nest_status_code,nest_status";

		sprintf(
		sys_string,
		"get_folder_data	application=%s			 "
		"			select=%s			 "
		"			folder=nest_status",
		application_name,
		select );

		nest_status_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( nest_status_record_list ) )
	{
		fprintf( stderr,
		"Warning in %s/%s()/%d: empty nest_status_record_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (char *)0;
	}

	do {
		record = list_get_pointer( nest_status_record_list );

		piece(	local_nest_status_code,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		if ( strcasecmp(local_nest_status_code,
				nest_status_code ) == 0 )
		{
			piece(	nest_status,
				FOLDER_DATA_DELIMITER,
				record,
				1 );

			return nest_status;
		}

	} while( list_next( nest_status_record_list ) );

	return "";

} /* alligator_get_nest_status() */

char *alligator_get_female_behavior(
				char *application_name,
				char *female_behavior_code )
{
	static LIST *female_behavior_record_list = {0};
	static char female_behavior[ 128 ];
	char local_female_behavior_code[ 16 ];
	char *record;

	if ( !*female_behavior_code ) return "";

	if ( !female_behavior_record_list )
	{
		char sys_string[ 1024 ];
		char *select = "female_behavior_code,female_behavior";

		sprintf(
		sys_string,
		"get_folder_data	application=%s			 "
		"			select=%s			 "
		"			folder=female_behavior",
		application_name,
		select );

		female_behavior_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( female_behavior_record_list ) )
	{
		fprintf( stderr,
		"Warning in %s/%s()/%d: empty female_behavior_record_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (char *)0;
	}

	do {
		record = list_get_pointer( female_behavior_record_list );

		piece(	local_female_behavior_code,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		if ( strcasecmp(local_female_behavior_code,
				female_behavior_code ) == 0 )
		{
			piece(	female_behavior,
				FOLDER_DATA_DELIMITER,
				record,
				1 );

			return female_behavior;
		}

	} while( list_next( female_behavior_record_list ) );

	/* return (char *)0; */
	return "";

} /* alligator_get_female_behavior() */

char *alligator_get_habitat_code(
				char *application_name,
				char *habitat_name )
{
	static LIST *habitat_record_list = {0};
	static char habitat_code[ 128 ];
	char local_habitat_name[ 128 ];
	char *record;

	if ( !*habitat_name ) return "";

	if ( !habitat_record_list )
	{
		char sys_string[ 1024 ];
		char *select = "habitat,habitat_code";

		sprintf(
		sys_string,
		"get_folder_data	application=%s			 "
		"			select=%s			 "
		"			folder=habitat",
		application_name,
		select );

		habitat_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( habitat_record_list ) )
	{
		fprintf( stderr,
		"Warning in %s/%s()/%d: empty habitat_record_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (char *)0;
	}

	do {
		record = list_get_pointer( habitat_record_list );

		piece(	local_habitat_name,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		if ( strcasecmp(local_habitat_name,
				habitat_name ) == 0 )
		{
			piece(	habitat_code,
				FOLDER_DATA_DELIMITER,
				record,
				1 );

			return habitat_code;
		}

	} while( list_next( habitat_record_list ) );

	return (char *)0;

} /* alligator_get_habitat_code() */

char *alligator_get_habitat(
				char *application_name,
				char *habitat_code )
{
	static LIST *habitat_record_list = {0};
	static char habitat[ 128 ];
	char local_habitat_code[ 16 ];
	char *record;

	if ( !*habitat_code ) return "";

	if ( !habitat_record_list )
	{
		char sys_string[ 1024 ];
		char *select = "habitat_code,habitat";

		sprintf(
		sys_string,
		"get_folder_data	application=%s			 "
		"			select=%s			 "
		"			folder=habitat",
		application_name,
		select );

		habitat_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( habitat_record_list ) )
	{
		fprintf( stderr,
		"Warning in %s/%s()/%d: empty habitat_record_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (char *)0;
	}

	do {
		record = list_get_pointer( habitat_record_list );

		piece(	local_habitat_code,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		if ( strcasecmp(local_habitat_code,
				habitat_code ) == 0 )
		{
			piece(	habitat,
				FOLDER_DATA_DELIMITER,
				record,
				1 );

			return habitat;
		}

	} while( list_next( habitat_record_list ) );

	return (char *)0;

} /* alligator_get_habitat() */

ALLIGATOR *alligator_new(	char *application_name,
				char *discovery_date_list_string,
				char *primary_researcher_list_string,
				char *begin_discovery_date,
				char *end_discovery_date,
				boolean with_secondary_researchers,
				boolean with_nest_list,
				boolean with_observation_list )
{
	ALLIGATOR *alligator;
	boolean sufficient_input = 0;

	if ( begin_discovery_date
	&&   *begin_discovery_date
	&&   strcmp( begin_discovery_date, "begin_discovery_date" ) != 0 )
	{
		sufficient_input = 1;
	}

	if ( !sufficient_input )
	{
		if ( discovery_date_list_string
		&&   *discovery_date_list_string
		&&   strcmp(	discovery_date_list_string,
				"discovery_date" ) != 0 )
		{
			sufficient_input = 1;
		}
	}

	if ( !sufficient_input ) return (ALLIGATOR *)0;

	if ( ! ( alligator = (ALLIGATOR *)
				calloc( 1, sizeof( ALLIGATOR ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( alligator->alligator_census_list =
		alligator_get_census_list(
			application_name,
			discovery_date_list_string,
			primary_researcher_list_string,
			begin_discovery_date,
			end_discovery_date,
			with_secondary_researchers ) ) )
	{
		return (ALLIGATOR *)0;
	}

	if ( with_nest_list )
	{
		alligator->nest_list =
			alligator_get_nest_list(
				application_name,
				discovery_date_list_string,
				primary_researcher_list_string,
				begin_discovery_date,
				end_discovery_date,
				with_observation_list,
				alligator->alligator_census_list );
	}

	return alligator;

} /* alligator_new() */

LIST *alligator_get_census_list(char *application_name,
				char *discovery_date_list_string,
				char *primary_researcher_list_string,
				char *begin_discovery_date,
				char *end_discovery_date,
				boolean with_secondary_researchers )
{
	ALLIGATOR_CENSUS *alligator_census;
	char *where_clause;
	char *select;
	LIST *record_list;
	char *record;
	char discovery_date[ 16 ];
	char primary_researcher[ 128 ];
	char pilot[ 128 ];
	char sys_string[ 65536 ];
	LIST *census_list;

	where_clause =
		alligator_get_discovery_primary_where_clause(
				application_name,
				"alligator_census",
				discovery_date_list_string,
				primary_researcher_list_string,
				begin_discovery_date,
				end_discovery_date );

	select = "discovery_date,primary_researcher,pilot";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=alligator_census	"
		 "			where=\"1 = 1 %s\"	",
		 application_name,
		 select,
		 where_clause );

	record_list = pipe2list( sys_string );

	if ( !list_rewind( record_list ) )
	{
		return (LIST *)0;
	}

	census_list = list_new();

	do {
		record = list_get_pointer( record_list );

		piece(	discovery_date,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		piece(	primary_researcher,
			FOLDER_DATA_DELIMITER,
			record,
			1 );

		piece(	pilot,
			FOLDER_DATA_DELIMITER,
			record,
			2 );

		alligator_census =
			alligator_census_new(
				application_name,
				strdup( discovery_date ),
				strdup( primary_researcher ),
				strdup( pilot ),
				with_secondary_researchers );

		alligator_census->discovery_year =
			strdup( alligator_get_discovery_year(
				alligator_census->discovery_date ) );

		alligator_census->discovery_date_american =
			strdup( alligator_get_discovery_date_american(
				alligator_census->
					discovery_date ) );

		list_append_pointer(
				census_list,
				alligator_census );

	} while( list_next( record_list ) );

	return census_list;
} /* alligator_get_census_list() */

char *alligator_get_discovery_date_american(
				char *discovery_date_international )
{
	DATE_CONVERT *date_convert;
	static char discovery_date_american[ 128 ];

	date_convert = date_convert_new_date_convert(
				american,
				discovery_date_international );
	strcpy( discovery_date_american, date_convert->return_date );
	date_convert_free( date_convert );
	return discovery_date_american;
}

char *alligator_get_discovery_year(
			char *discovery_date_international )
{
	static char discovery_year[ 16 ];

	piece( discovery_year, '-', discovery_date_international, 0 );
	return discovery_year;
}

ALLIGATOR_CENSUS *alligator_census_new(
				char *application_name,
				char *discovery_date,
				char *primary_researcher,
				char *pilot,
				boolean with_secondary_researchers )
{
	ALLIGATOR_CENSUS *alligator_census;

	if ( ! ( alligator_census = (ALLIGATOR_CENSUS *)
				calloc( 1, sizeof( ALLIGATOR_CENSUS ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	alligator_census->discovery_date = discovery_date;
	alligator_census->primary_researcher = primary_researcher;
	alligator_census->pilot = pilot;

	if ( ! ( alligator_census->pilot =
		alligator_get_pilot(
			application_name,
			discovery_date,
			primary_researcher ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot get pilot for (%s/%s)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			discovery_date,
			primary_researcher );
		exit( 1 );
	}

	if ( with_secondary_researchers )
	{
		alligator_census->secondary_researcher_list =
			alligator_get_secondary_researcher_list(
				application_name,
				discovery_date,
				primary_researcher );
	}

	return alligator_census;

} /* alligator_census_new() */

LIST *alligator_get_secondary_researcher_list(
				char *application_name,
				char *discovery_date,
				char *primary_researcher )
{
	FILE *input_pipe;
	char sys_string[ 1024 ];
	char secondary_researcher[ 128 ];
	LIST *secondary_researcher_list;
	char where_clause[ 256 ];
	char *select = "researcher";

	sprintf(where_clause,
		"discovery_date = '%s' and			"
		"primary_researcher = '%s'			",
		discovery_date,
		primary_researcher );

	sprintf(sys_string,
		"get_folder_data	application=%s		    "
		"			select=%s		    "
		"			folder=secondary_researcher "
		"			where=\"%s\"		    ",
		application_name,
		select,
		where_clause );

	input_pipe = popen( sys_string, "r" );
	secondary_researcher_list = list_new();

	while( get_line( secondary_researcher, input_pipe ) )
	{
		list_append_pointer(
			secondary_researcher_list,
			strdup( secondary_researcher ) );
	}

	return secondary_researcher_list;

} /* alligator_get_secondary_researcher_list() */

OBSERVATION *alligator_observation_new(
				char *observation_date )
{
	OBSERVATION *observation;

	if ( ! ( observation = (OBSERVATION *)
				calloc( 1, sizeof( OBSERVATION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	observation->observation_date = observation_date;
	return observation;

} /* alligator_observation_new() */

NEST *alligator_nest_new( char *nest_number_key )
{
	NEST *nest;

	if ( ! ( nest = (NEST *)calloc( 1, sizeof( NEST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	nest->nest_number_key = nest_number_key;
	return nest;

} /* alligator_nest_new() */

char *alligator_get_pilot(
			char *application_name,
			char *discovery_date,
			char *primary_researcher )
{
	static LIST *alligator_census_record_list = {0};
	char pilot[ 128 ];
	char *alligator_census_record;
	char local_discovery_date[ 16 ];
	char local_primary_researcher[ 128 ];

	if ( !alligator_census_record_list )
	{
		alligator_census_record_list =
			alligator_get_alligator_census_record_list(
				application_name );
	}

	if ( !list_rewind( alligator_census_record_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: empty alligator_census_record_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		alligator_census_record =
			list_get_pointer(
				alligator_census_record_list );

		piece(	local_discovery_date,
			FOLDER_DATA_DELIMITER,
			alligator_census_record,
			0 );

		piece(	local_primary_researcher,
			FOLDER_DATA_DELIMITER,
			alligator_census_record,
			1 );

		if ( strcmp( local_discovery_date, discovery_date ) == 0
		&&   strcmp(	local_primary_researcher,
				primary_researcher ) == 0 )
		{
			piece(	pilot,
				FOLDER_DATA_DELIMITER,
				alligator_census_record,
				2 );

			return strdup( pilot );
		}
	} while( list_next( alligator_census_record_list ) );

	return (char *)0;

} /* alligator_get_pilot() */

LIST *alligator_get_nest_list(	char *application_name,
				char *discovery_date_list_string,
				char *primary_researcher_list_string,
				char *begin_discovery_date,
				char *end_discovery_date,
				boolean with_observation_list,
				LIST *alligator_census_list )
{
	LIST *nest_list;
	char sys_string[ 65536 ];
	char *nest_table_name;
	char input_buffer[ 1024 ];
	char piece_buffer[ 1024 ];
	char discovery_date[ 128 ];
	char primary_researcher[ 128 ];
	char nest_number_key[ 16 ];
	FILE *input_pipe;
	NEST *nest;
	ALLIGATOR_CENSUS *alligator_census;
	char *where_clause;
	char *sort_command;
	char *select =
"nest_number,discovery_date,primary_researcher,utm_easting,utm_northing,transect_number,monitor_nest_yn,female_size_cm,materials,basin,habitat,nest_height_cm,in_transect_boundary_yn,near_pond_yn,total_eggs,banded_eggs,not_banded_eggs,nest_notepad";

	where_clause =
		alligator_get_discovery_primary_where_clause(
				application_name,
				"nest",
				discovery_date_list_string,
				primary_researcher_list_string,
				begin_discovery_date,
				end_discovery_date );

	nest_table_name =
		get_table_name( application_name, "nest" );

	sort_command = "sort -n -k2 -t'-'";

	sprintf(sys_string,
		"echo \"select %s					 "
		"	from %s						 "
		"	where 1 = 1 %s;\"				|"
		"sql.e '%c'						|"
		"%s							 ",
		select,
		nest_table_name,
		where_clause,
		FOLDER_DATA_DELIMITER,
		sort_command );

	nest_list = list_new();
	input_pipe = popen( sys_string, "r" );

/*
"nest_number,discovery_date,primary_researcher,utm_easting,utm_northing,transect_number,monitor_nest_yn,female_size_cm,materials,basin,habitat,nest_height_cm,in_transect_boundary_yn,near_pond_yn,total_eggs,banded_eggs,not_banded_eggs,nest_notepad";
*/

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	discovery_date,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	primary_researcher,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		if ( ! ( alligator_census =
			alligator_census_fetch(
				discovery_date,
				primary_researcher,
				alligator_census_list ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot fetch census for (%s)/(%s)\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				discovery_date,
				primary_researcher );
			exit( 1 );
		}

		piece(	nest_number_key,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		nest = alligator_nest_new( strdup( nest_number_key ) );

		if ( piece( piece_buffer, '-', nest_number_key, 1 ) )
			nest->nest_number = atoi( piece_buffer );

		nest->alligator_census = alligator_census;

		piece(	nest->utm_easting,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		piece(	nest->utm_northing,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			4 );

		piece(	nest->transect_number,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			5 );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			6 );
		nest->monitor_nest_yn =
			(*piece_buffer) ? *piece_buffer : 'n';

		piece(	nest->female_size_cm_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			7 );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			8 );
		nest->materials = strdup( piece_buffer );

		nest->materials_code =
			strdup( alligator_get_materials_code(
				application_name,
				nest->materials ) );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			9 );
		nest->basin = strdup( piece_buffer );

		nest->basin_code =
			strdup( alligator_get_basin_code(
				application_name,
				nest->basin ) );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			10 );
		nest->habitat = strdup( piece_buffer );

		nest->habitat_code =
			strdup( alligator_get_habitat_code(
				application_name,
				nest->habitat ) );

		piece(	nest->nest_height_cm_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			11 );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			12 );
		nest->in_transect_boundary_yn =
			(*piece_buffer) ? *piece_buffer : 'n';

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			13 );
		nest->near_pond_yn =
			(*piece_buffer) ? *piece_buffer : 'n';

		piece(	nest->total_eggs_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			14 );

		piece(	nest->banded_eggs_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			15 );

		piece(	nest->not_banded_eggs_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			16 );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			17 );
		nest->nest_notepad = strdup( piece_buffer );

		if ( with_observation_list )
		{
			nest->observation_list =
				alligator_get_observation_list(
					application_name,
					nest->nest_number_key,
					discovery_date_list_string,
					primary_researcher_list_string,
					begin_discovery_date,
					end_discovery_date );
		}

		list_append_pointer( nest_list, nest );
	}
	pclose( input_pipe );
	return nest_list;

} /* alligator_get_nest_list() */

ALLIGATOR_CENSUS *alligator_census_fetch(
				char *discovery_date,
				char *primary_researcher,
				LIST *alligator_census_list )
{
	ALLIGATOR_CENSUS *alligator_census;

	if ( !list_rewind( alligator_census_list ) )
		return (ALLIGATOR_CENSUS *)0;

	do {
		alligator_census = list_get_pointer( alligator_census_list );

		if ( strcmp(	alligator_census->discovery_date,
				discovery_date ) == 0
		&&   strcmp(	alligator_census->primary_researcher,
				primary_researcher ) == 0 )
		{
			return alligator_census;
		}
	} while( list_next( alligator_census_list ) );

	return (ALLIGATOR_CENSUS *)0;
} /* alligator_census_fetch() */

LIST *alligator_get_observation_list(
				char *application_name,
				char *nest_number,
				char *discovery_date_list_string,
				char *primary_researcher_list_string,
				char *begin_discovery_date,
				char *end_discovery_date )
{
	LIST *observation_list = {0};
	static LIST *observation_record_list = {0};
	char *observation_record;
	char local_nest_number[ 16 ];
	int first_time = 1;

	if ( !observation_record_list )
	{
		observation_record_list =
			alligator_get_observation_record_list(
				application_name,
				discovery_date_list_string,
				primary_researcher_list_string,
				begin_discovery_date,
				end_discovery_date );
	}

	if ( !list_rewind( observation_record_list ) )
		return (LIST *)0;

	do {
		observation_record =
			list_get_pointer( observation_record_list );

		piece(	local_nest_number,
			FOLDER_DATA_DELIMITER,
			observation_record,
			0 );

		if ( strcmp( nest_number, local_nest_number ) == 0 )
		{
			char observation_date[ 16 ];
			char piece_buffer[ 256 ];
			OBSERVATION *observation;

			piece(	observation_date,
				FOLDER_DATA_DELIMITER,
				observation_record,
				1 );

			observation =
				alligator_observation_new(
					strdup( observation_date ) );

			piece(	observation->nest_visit_number,
				FOLDER_DATA_DELIMITER,
				observation_record,
				2 );

			piece(	piece_buffer,
				FOLDER_DATA_DELIMITER,
				observation_record,
				3 );
			observation->observation_view = strdup( piece_buffer );
			format_initial_capital(
					observation->observation_view,
					observation->observation_view );

			piece(	piece_buffer,
				FOLDER_DATA_DELIMITER,
				observation_record,
				4 );
			observation->nest_status = strdup( piece_buffer );
			format_initial_capital(
					observation->nest_status,
					observation->nest_status );

			piece(	piece_buffer,
				FOLDER_DATA_DELIMITER,
				observation_record,
				5 );
			observation->female_behavior = strdup( piece_buffer );
			format_initial_capital(
					observation->female_behavior,
					observation->female_behavior );

			piece(	observation->hatchlings_count_string,
				FOLDER_DATA_DELIMITER,
				observation_record,
				6 );

			piece(	observation->hatched_eggs_string,
				FOLDER_DATA_DELIMITER,
				observation_record,
				7 );

			piece(	observation->flooded_eggs_string,
				FOLDER_DATA_DELIMITER,
				observation_record,
				8 );

			piece(	observation->predated_eggs_string,
				FOLDER_DATA_DELIMITER,
				observation_record,
				9 );

			piece(	observation->crushed_eggs_string,
				FOLDER_DATA_DELIMITER,
				observation_record,
				10 );

			piece(	observation->other_egg_mortality_string,
				FOLDER_DATA_DELIMITER,
				observation_record,
				11 );

			piece(	observation->removed_viable_eggs_string,
				FOLDER_DATA_DELIMITER,
				observation_record,
				12 );

			piece(	observation->removed_non_viable_eggs_string,
				FOLDER_DATA_DELIMITER,
				observation_record,
				13 );

			piece(	observation->water_depth_average_cm_string,
				FOLDER_DATA_DELIMITER,
				observation_record,
				14 );

			piece(	observation->nest_height_above_water_cm_string,
				FOLDER_DATA_DELIMITER,
				observation_record,
				15 );

			piece(	observation->observation_notepad,
				FOLDER_DATA_DELIMITER,
				observation_record,
				16 );

			observation->water_depth_list =
				alligator_get_water_depth_list(
					application_name,
					nest_number,
					observation->observation_date,
					discovery_date_list_string,
					primary_researcher_list_string,
					begin_discovery_date,
					end_discovery_date );

			if ( first_time )
			{
				observation_list = list_new();
				first_time = 0;
			}

			list_append_pointer(
				observation_list,
				observation );
		}
	} while( list_next( observation_record_list ) );
	return observation_list;

} /* alligator_get_observation_list() */

LIST *alligator_get_observation_record_list(
				char *application_name,
				char *discovery_date_list_string,
				char *primary_researcher_list_string,
				char *begin_discovery_date,
				char *end_discovery_date )
{
	char sys_string[ 65536 ];
	char *nest_observation_table_name;
	char *nest_table_name;
	char *where_clause;
	char join_where_clause[ 1024 ];
	char select[ 1024 ];

	nest_observation_table_name =
		get_table_name( application_name, "nest_visit" );

	nest_table_name =
		get_table_name( application_name, "nest" );

	sprintf( select,
"%s.nest_number,observation_date,nest_visit_number,observation_view,nest_status,female_behavior,hatchlings_count,hatched_eggs,flooded_eggs,predated_eggs,crushed_eggs,other_egg_mortality,removed_viable_eggs,removed_non_viable_eggs,water_depth_average_cm,nest_height_above_water_cm,observation_notepad",
		 nest_observation_table_name );

	sprintf( join_where_clause,
		 "%s.nest_number = %s.nest_number			",
		 nest_table_name,
		 nest_observation_table_name );

	where_clause =
		alligator_get_discovery_primary_where_clause(
				application_name,
				"nest",
				discovery_date_list_string,
				primary_researcher_list_string,
				begin_discovery_date,
				end_discovery_date );

	sprintf(sys_string,
	"echo \"select %s					 "
	"	from %s,%s					 "
	"	where %s %s					 "
	"	order by nest_number,observation_date;\"	|"
	"sql.e '%c'						 ",
		select,
		nest_observation_table_name,
		nest_table_name,
		join_where_clause,
		where_clause,
		FOLDER_DATA_DELIMITER );

	return pipe2list( sys_string );

} /* alligator_get_observation_record_list() */

LIST *alligator_get_alligator_census_record_list(
				char *application_name )
{
	char sys_string[ 1024 ];
	char *alligator_census_table_name;
	char *select = "discovery_date,primary_researcher,pilot";

	alligator_census_table_name =
		get_table_name(	application_name,
				"alligator_census" );

	sprintf(sys_string,
		"echo \"select %s				 "
		"	from %s;\"				|"
		"sql.e '%c'					 ",
		select,
		alligator_census_table_name,
		FOLDER_DATA_DELIMITER );

	return pipe2list( sys_string );
} /* alligator_get_alligator_census_record_list() */

void alligator_get_secondary_researchers(
				char **secondary_researcher_1,
				char **secondary_researcher_2,
				LIST *secondary_researcher_list )
{
	*secondary_researcher_1 = "";
	*secondary_researcher_2 = "";

return;

	if ( list_rewind( secondary_researcher_list ) )
	{
		*secondary_researcher_1 =
			(char *)list_get_pointer( secondary_researcher_list );
		if ( list_next( secondary_researcher_list ) )
		{
			*secondary_researcher_2 =
				(char *)list_get_pointer(
						secondary_researcher_list );
		}
	}

} /* alligator_get_secondary_researchers() */

char *alligator_get_nest_key(	char *discovery_date_international,
				char *discovery_year,
				char *nest_number )
{
	static char nest_key[ 128 ];
	char local_discovery_year[ 16 ];

	if ( discovery_date_international )
	{
		if ( character_count( '-', discovery_date_international ) != 2 )
		{
			return ( char *)0;
		}
		piece(	local_discovery_year,
			'-',
			discovery_date_international,
			0 );
	}
	else
	if ( discovery_year )
	{
		strcpy( local_discovery_year, discovery_year );
	}
	else
	{
		return ( char *)0;
	}

	if ( strlen( local_discovery_year ) == 2 )
	{
		char tmp[ 3 ];

		strcpy( tmp, local_discovery_year );
		if ( atoi( local_discovery_year ) <= 8 )
			sprintf( local_discovery_year, "20%s", tmp );
		else
			sprintf( local_discovery_year, "19%s", tmp );

	}

	sprintf(nest_key,
		"%s-%s",
		local_discovery_year,
		nest_number );
	return nest_key;
} /* alligator_get_nest_key() */

char *alligator_get_discovery_primary_where_clause(
				char *application_name,
				char *folder_name,
				char *discovery_date_list_string,
				char *primary_researcher_list_string,
				char *begin_discovery_date,
				char *end_discovery_date )
{
	char where_clause[ 131072 ];
	char *where_clause_pointer = where_clause;
	char *table_name;

	table_name = get_table_name( application_name, folder_name );

	if ( begin_discovery_date
	&&   *begin_discovery_date
	&&   strcmp( begin_discovery_date, "begin_discovery_date" ) != 0
	&&   end_discovery_date
	&&   *end_discovery_date
	&&   strcmp( end_discovery_date, "end_discovery_date" ) != 0 )
	{
		sprintf( where_clause,
			 "%s.discovery_date between '%s' and '%s'",
			 table_name,
			 begin_discovery_date,
			 end_discovery_date );
	}
	else
	{
		where_clause_pointer +=
			sprintf( where_clause_pointer,
			 	" and %s.discovery_date in (%s)",
			 	table_name,
			 	timlib_get_in_clause(
					discovery_date_list_string ) );

		where_clause_pointer +=
			sprintf( where_clause_pointer,
			 	" and %s.primary_researcher in (%s)",
			 	table_name,
			 	timlib_get_in_clause(
				 	primary_researcher_list_string ) );
	}

	return strdup( where_clause );

} /* alligator_get_discovery_primary_where_clause() */

char *alligator_get_transect_number_string(
				char *application_name,
				int utm_northing )
{
	static char transect_number_string[ 16 ];
	int distance;
	int transect_number;

	if ( ! ( transect_number = alligator_get_transect_number_should_be(
				&distance,
				application_name,
				utm_northing ) ) )
	{
		return (char *)0;
	}

	sprintf( transect_number_string, "%d", transect_number );
	return transect_number_string;

/*
	TRANSECT *transect;
	double utm_northing_double;

	if ( !transect_list )
	{
		transect_list = alligator_get_transect_list( application_name );
	}

	utm_northing_double = (double)utm_northing;

	if ( !list_rewind( transect_list ) ) return (char *)0;

	do {
		transect = list_get_pointer( transect_list );

		if ( utm_northing_double <= transect->utm_northing + 1000
		&&   utm_northing_double >= transect->utm_northing - 1000 )
		{
			sprintf(	transect_number_string,
					"%.1lf",
					transect->transect_number );
			return transect_number_string;
		}
	} while( list_next( transect_list ) );

	return (char *)0;
*/
} /* alligator_get_transect_number_string() */

int alligator_get_transect_number_should_be(
				int *parameter_distance,
				char *application_name,
				int nest_utm_northing )
{
	TRANSECT *transect;
	int transect_number_should_be = 0;
	int shortest_distance = INT_MAX;
	int distance;
	int mid_point;

	if ( !transect_list )
	{
		transect_list = alligator_get_transect_list( application_name );
	}

	*parameter_distance = 0;
	if ( !list_rewind( transect_list ) ) return 0;

	do {
		transect = list_get_pointer( transect_list );

		mid_point =
		( transect->west_northing + transect->east_northing ) / 2;

		distance = abs( nest_utm_northing - mid_point );
		if ( distance < shortest_distance )
		{
			transect_number_should_be = transect->transect_number;
			shortest_distance = distance;
		}
	} while( list_next( transect_list ) );

	*parameter_distance = shortest_distance;
	return transect_number_should_be;
} /* alligator_get_transect_should_be() */

char alligator_get_in_transect_boundary_should_be_yn(
				double *distance,
				char *application_name,
				int transect_number,
				int nest_utm_northing,
				int nest_utm_easting )
{
	TRANSECT *transect;
	double transect_northing_at_nest_easting;

	if ( !transect_number )
	{
		*distance = 0.0;
		return 'n';
	}

	if ( !transect_list )
	{
		transect_list = alligator_get_transect_list( application_name );
	}

	if ( ! ( transect = alligator_seek_transect(
					transect_list,
					transect_number ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot seek transect = %d.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 transect_number );
		exit( 1 );
	}

	if ( nest_utm_easting > transect->east_easting
	||   nest_utm_easting < transect->west_easting )
	{
		*distance = 0.0;
		return 'n';
	}

	transect_northing_at_nest_easting =
		alligator_get_transect_northing_at_nest_easting(
			transect,
			nest_utm_easting );

	*distance =	transect_northing_at_nest_easting - 
			(double)nest_utm_northing;

	if ( *distance > 250.0 || *distance < -250.0 )
		return 'n';
	else
		return 'y';

} /* alligator_get_in_transect_boundary_should_be_yn() */

TRANSECT *alligator_transect_new( double transect_number )
{
	TRANSECT *transect;

	if ( ! ( transect = calloc( 1, sizeof( TRANSECT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	transect->transect_number = transect_number;

	return transect;
} /* alligator_transect_new() */

LIST *alligator_get_transect_list( char *application_name )
{
	LIST *transect_list;
	TRANSECT *transect;
	FILE *input_pipe;
	char sys_string[ 1024 ];
	char input_string[ 256 ];
	char transect_number[ 16 ];
	char piece_buffer[ 16 ];
	char *select;

	select =
"transect_number,utm_northing,east_easting,east_northing,west_easting,west_northing";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=transect		",
		 application_name,
		 select );

	input_pipe = popen( sys_string, "r" );

	transect_list = list_new();

	while( get_line( input_string, input_pipe ) )
	{
		piece(	transect_number,
			FOLDER_DATA_DELIMITER,
			input_string,
			0 );

		transect = alligator_transect_new(
				atof( transect_number ) );
		list_append_pointer( transect_list, transect );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_string,
			1 );
		transect->utm_northing = atof( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_string,
			2 );
		transect->east_easting = atoi( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_string,
			3 );
		transect->east_northing = atoi( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_string,
			4 );
		transect->west_easting = atoi( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_string,
			5 );
		transect->west_northing = atoi( piece_buffer );
	}

	pclose( input_pipe );
	return transect_list;
} /* alligator_get_transect_list() */

TRANSECT *alligator_seek_transect(	LIST *transect_list,
					int transect_number )
{
	TRANSECT *transect;

	if ( !list_rewind( transect_list ) ) return (TRANSECT *)0;

	do {
		transect = list_get_pointer( transect_list );
		if ( (int)transect->transect_number == transect_number )
			return transect;
	} while( list_next( transect_list ) );
	return (TRANSECT *)0;
} /* alligator_seek_transect() */

char *alligator_get_basin_should_be(	int *cell_number,
					char *application_name,
					double utm_northing,
					double utm_easting )
{
	static LIST *cell_list = {0};
	CELL *cell;

	if ( !cell_list )
	{
		cell_list = alligator_get_cell_list( application_name );
	}

	*cell_number = 0;

	if ( !list_rewind( cell_list ) ) return (char *)0;

	do {
		cell = list_get_pointer( cell_list );

		if ( utm_northing <= cell->utm_northing + 1000
		&&   utm_northing >= cell->utm_northing - 1000
		&&   utm_easting <= cell->utm_easting + 1000
		&&   utm_easting >= cell->utm_easting - 1000 )
		{
			*cell_number = cell->cell_number;
			return cell->basin;
		}
	} while( list_next( cell_list ) );
	return (char *)0;
} /* alligator_get_basin_should_be() */

CELL *alligator_cell_new(		int cell_number,
					double utm_northing,
					double utm_easting,
				char *basin )
{
	CELL *cell;

	if ( ! ( cell = calloc( 1, sizeof( CELL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	cell->cell_number = cell_number;
	cell->utm_northing = utm_northing;
	cell->utm_easting = utm_easting;
	cell->basin = basin;
	return cell;
} /* alligator_cell_new() */

LIST *alligator_get_cell_list(	char *application_name )
{
	LIST *cell_list;
	CELL *cell;
	FILE *input_pipe;
	char sys_string[ 1024 ];
	char input_string[ 256 ];
	char cell_number[ 16 ];
	char utm_northing[ 16 ];
	char utm_easting[ 16 ];
	char basin[ 128 ];
	char *cell_table_name;
	char *transect_table_name;
	char where[ 128 ];
	char *select =
		 "cell_number,utm_northing,utm_easting,basin";

	cell_table_name = get_table_name( application_name, "cell" );
	transect_table_name = get_table_name( application_name, "transect" );

	sprintf( where,
		 "%s.transect_number = %s.transect_number",
		 cell_table_name,
		 transect_table_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=cell,transect		"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	cell_list = list_new();

	while( get_line( input_string, input_pipe ) )
	{
		piece(	cell_number,
			FOLDER_DATA_DELIMITER,
			input_string,
			0 );

		piece(	utm_northing,
			FOLDER_DATA_DELIMITER,
			input_string,
			1 );

		piece(	utm_easting,
			FOLDER_DATA_DELIMITER,
			input_string,
			2 );

		piece(	basin,
			FOLDER_DATA_DELIMITER,
			input_string,
			3 );

		cell = alligator_cell_new(
				atoi( cell_number ),
				atof( utm_northing ),
				atof( utm_easting ),
				strdup( basin ) );

		list_append_pointer( cell_list, cell );
	}

	pclose( input_pipe );
	return cell_list;
} /* alligator_get_cell_list() */

double alligator_get_transect_northing_at_nest_easting(
			TRANSECT *transect,
			int nest_utm_easting )
{
	int utm_easting_range;
	int utm_northing_range;
	int nest_distance_west_of_east_transect_boundary;
	double transect_distance_north_of_west_transect_boundary;
	double utm_northings_per_utm_easting;
	double transect_northing_at_nest_easting;

	utm_easting_range = transect->east_easting - transect->west_easting;
	utm_northing_range = transect->east_northing - transect->west_northing;

	if ( utm_easting_range )
	{
		utm_northings_per_utm_easting =
			(double)utm_northing_range / (double)utm_easting_range;
	}
	else
	{
		utm_northings_per_utm_easting = 0.0;
	}

	nest_distance_west_of_east_transect_boundary =
		transect->east_easting - nest_utm_easting;

	transect_distance_north_of_west_transect_boundary =
		(double)nest_distance_west_of_east_transect_boundary *
		utm_northings_per_utm_easting;

	transect_northing_at_nest_easting =
		(double)transect->east_northing -
		transect_distance_north_of_west_transect_boundary;
		
	return transect_northing_at_nest_easting;
} /* alligator_get_transect_northing_at_nest_easting() */

WATER_DEPTH *alligator_water_depth_new(
				int water_depth_cm )
{
	WATER_DEPTH *water_depth;

	if ( ! ( water_depth = (WATER_DEPTH *)
				calloc( 1, sizeof( WATER_DEPTH ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	water_depth->water_depth_cm = water_depth_cm;
	return water_depth;

} /* water_depth_water_depth_new() */

LIST *alligator_get_water_depth_record_list(
				char *application_name,
				char *discovery_date_list_string,
				char *primary_researcher_list_string,
				char *begin_discovery_date,
				char *end_discovery_date )
{
	char sys_string[ 65536 ];
	char *water_depth_table_name;
	char *nest_table_name;
	char *where_clause;
	char join_where_clause[ 1024 ];
	char select[ 1024 ];

	water_depth_table_name =
		get_table_name( application_name, "water_depth" );

	nest_table_name =
		get_table_name( application_name, "nest" );

	sprintf( select,
"%s.nest_number,observation_date,water_depth_cm",
		 water_depth_table_name );

	sprintf( join_where_clause,
		 "%s.nest_number = %s.nest_number			",
		 nest_table_name,
		 water_depth_table_name );

	where_clause =
		alligator_get_discovery_primary_where_clause(
				application_name,
				"nest",
				discovery_date_list_string,
				primary_researcher_list_string,
				begin_discovery_date,
				end_discovery_date );

	sprintf(sys_string,
	"echo \"select %s					 "
	"	from %s,%s					 "
	"	where %s %s					 "
	"	order by measurement_number;\"			|"
	"sql.e '%c'						 ",
		select,
		water_depth_table_name,
		nest_table_name,
		join_where_clause,
		where_clause,
		FOLDER_DATA_DELIMITER );

	return pipe2list( sys_string );

} /* alligator_get_water_depth_record_list() */

LIST *alligator_get_water_depth_list(
				char *application_name,
				char *nest_number,
				char *observation_date,
				char *discovery_date_list_string,
				char *primary_researcher_list_string,
				char *begin_discovery_date,
				char *end_discovery_date )
{
	LIST *water_depth_list = {0};
	static LIST *water_depth_record_list = {0};
	char *water_depth_record;
	char local_nest_number[ 16 ];
	char local_observation_date[ 16 ];
	int first_time = 1;

	if ( !water_depth_record_list )
	{
		water_depth_record_list =
			alligator_get_water_depth_record_list(
				application_name,
				discovery_date_list_string,
				primary_researcher_list_string,
				begin_discovery_date,
				end_discovery_date );
	}

	if ( !list_rewind( water_depth_record_list ) )
		return (LIST *)0;

	do {
		water_depth_record =
			list_get_pointer( water_depth_record_list );

		piece(	local_nest_number,
			FOLDER_DATA_DELIMITER,
			water_depth_record,
			0 );

		piece(	local_observation_date,
			FOLDER_DATA_DELIMITER,
			water_depth_record,
			1 );

		if ( strcmp( nest_number, local_nest_number ) == 0
		&&   strcmp( observation_date, local_observation_date ) == 0 )
		{
			char water_depth_cm[ 16 ];
			WATER_DEPTH *water_depth;

			piece(	water_depth_cm,
				FOLDER_DATA_DELIMITER,
				water_depth_record,
				2 );

			water_depth =
				alligator_water_depth_new(
					atoi( water_depth_cm ) );

			if ( first_time )
			{
				water_depth_list = list_new();
				first_time = 0;
			}

			list_append_pointer(
				water_depth_list,
				water_depth );
		}
	} while( list_next( water_depth_record_list ) );
	return water_depth_list;

} /* alligator_get_water_depth_list() */

char *alligator_get_female_behavior_code(
				char *application_name,
				char *female_behavior )
{
	static LIST *female_behavior_record_list = {0};
	static char female_behavior_code[ 128 ];
	char local_female_behavior[ 128 ];
	char *record;

	if ( !*female_behavior ) return "";

	if ( !female_behavior_record_list )
	{
		char sys_string[ 1024 ];
		char *select = "female_behavior,female_behavior_code";

		sprintf(
		sys_string,
		"get_folder_data	application=%s			 "
		"			select=%s			 "
		"			folder=female_behavior",
		application_name,
		select );

		female_behavior_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( female_behavior_record_list ) )
	{
		fprintf( stderr,
		"Warning in %s/%s()/%d: empty female_behavior_record_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (char *)0;
	}

	do {
		record = list_get_pointer( female_behavior_record_list );

		piece(	local_female_behavior,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		if ( strcasecmp(local_female_behavior,
				female_behavior ) == 0 )
		{
			piece(	female_behavior_code,
				FOLDER_DATA_DELIMITER,
				record,
				1 );

			return female_behavior_code;
		}

	} while( list_next( female_behavior_record_list ) );

	return "";

} /* alligator_get_female_behavior_code() */

char *alligator_get_nest_status_code(
				char *application_name,
				char *nest_status )
{
	static LIST *nest_status_record_list = {0};
	static char nest_status_code[ 128 ];
	char local_nest_status[ 128 ];
	char *record;

	if ( !*nest_status ) return "";

	if ( !nest_status_record_list )
	{
		char sys_string[ 1024 ];
		char *select = "nest_status,nest_status_code";

		sprintf(
		sys_string,
		"get_folder_data	application=%s			 "
		"			select=%s			 "
		"			folder=nest_status",
		application_name,
		select );

		nest_status_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( nest_status_record_list ) )
	{
		fprintf( stderr,
		"Warning in %s/%s()/%d: empty nest_status_record_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (char *)0;
	}

	do {
		record = list_get_pointer( nest_status_record_list );

		piece(	local_nest_status,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		if ( strcasecmp(local_nest_status,
				nest_status ) == 0 )
		{
			piece(	nest_status_code,
				FOLDER_DATA_DELIMITER,
				record,
				1 );

			return nest_status_code;
		}

	} while( list_next( nest_status_record_list ) );

	return "";

} /* alligator_get_nest_status_code() */

