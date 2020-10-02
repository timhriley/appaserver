/* ---------------------------------------------------	*/
/* src_creel/creel_library.c				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "query.h"
#include "dictionary.h"
#include "hashtbl.h"
#include "date.h"
#include "appaserver_library.h"
#include "statistics_weighted.h"
#include "document.h"
#include "creel_library.h"

/* appaserver_link_file */

/* Global variables */
/* ---------------- */
static LIST *measurements_record_list = {0};

boolean creel_library_catch_exists_measurements(
				char *application_name,
				char *fishing_purpose,
				char *census_date,
				char *interview_location,
				char *interview_number,
				char *family,
				char *genus,
				char *species,
				char *begin_date_string,
				char *end_date_string )
{
	char *measurements_record;
	char *search_string;
	int str_len;

	if ( !measurements_record_list )
	{
		measurements_record_list =
			creel_library_get_measurements_record_list(
				application_name,
				fishing_purpose,
				begin_date_string,
				end_date_string );
	}

	search_string =
		creel_library_get_measurements_record_search_string(
				fishing_purpose,
				census_date,
				interview_location,
				interview_number,
				family,
				genus,
				species);

	str_len = strlen( search_string );

	if ( !list_rewind( measurements_record_list ) ) return 0;

	do {
		measurements_record =
			list_get_pointer( measurements_record_list );

		if ( strncmp(	measurements_record,
				search_string,
				str_len ) == 0 )
		{
			return 1;
		}

	} while( list_next( measurements_record_list ) );

	return 0;
}

boolean creel_library_species_get_measurements(
				CATCH_MEASUREMENTS *catch_measurements,
				char *application_name,
				char *fishing_purpose,
				char *census_date,
				char *interview_location,
				char *interview_number,
				char *family,
				char *genus,
				char *species,
				char *begin_date_string,
				char *end_date_string )
{
	static LIST *measurements_record_list = {0};
	char *measurements_record;
	char *search_string;
	static boolean new_beginning = 0;
	static boolean found_one = 0;
	static boolean last_one = 0;

	if ( last_one )
	{
		last_one = 0;
		return 0;
	}

	if ( !measurements_record_list )
	{
		measurements_record_list =
			creel_library_get_measurements_record_list(
				application_name,
				fishing_purpose,
				begin_date_string,
				end_date_string );

		new_beginning = 1;
	}

	if ( new_beginning )
	{
		new_beginning = 0;
		found_one = 0;

		if ( !list_rewind( measurements_record_list ) )
		{
			return 0;
		}
	}

	search_string =
		creel_library_get_measurements_record_search_string(
				fishing_purpose,
				census_date,
				interview_location,
				interview_number,
				family,
				genus,
				species);

	do {
		measurements_record =
			list_get_pointer( measurements_record_list );

		if ( timlib_strncmp(
				measurements_record,
				search_string ) == 0 )
		{
/*
"fishing_purpose,census_date,interview_location,interview_number,%s.family,%s.genus,%s.species,%s.common_name,%s.florida_state_code,catch_measurement_number,length_millimeters",
*/
			piece(	catch_measurements->family,
				'^',
				measurements_record,
				4 );
		
			piece(	catch_measurements->genus,
				'^',
				measurements_record,
				5 );
		
			piece(	catch_measurements->species,
				'^',
				measurements_record,
				6 );
		
			piece(	catch_measurements->common_name,
				'^',
				measurements_record,
				7 );
		
			piece(	catch_measurements->florida_state_code,
				'^',
				measurements_record,
				8 );
		
			piece(	catch_measurements->catch_measurement_number,
				'^',
				measurements_record,
				9 );

			piece(	catch_measurements->length_millimeters,
				'^',
				measurements_record,
				10 );
			catch_measurements->length_millimeters_integer =
				atoi( catch_measurements->length_millimeters );

			found_one = 1;

			if ( !list_next( measurements_record_list ) )
				last_one = 1;

			return 1;
		}

		/* No need to go to the end of the list */
		/* ------------------------------------ */
		if ( found_one ) break;

	} while( list_next( measurements_record_list ) );

	new_beginning = 1;
	return 0;
}

boolean creel_library_census_date_get_catches(
				CATCHES *catches,
				char *application_name,
				char *fishing_purpose,
				char *census_date,
				char *interview_location,
				char *interview_number,
				char *begin_date_string,
				char *end_date_string )
{
	static LIST *catches_record_list = {0};
	char *catches_record;
	char search_string[ 512 ];
	int str_len;
	static boolean new_beginning = 1;
	static boolean found_one = 0;
	static boolean last_one = 0;

	if ( last_one )
	{
		last_one = 0;
		new_beginning = 1;
		found_one = 0;

		return 0;
	}

	if ( !catches_record_list )
	{
		char sys_string[ 2048 ];
		char catches_where_clause[ 1024 ];
		char common_name_join_where_clause[ 1024 ];
		char select[ 1024 ];
		char *catches_table_name;
		char *species_table_name;

		catches_table_name =
			get_table_name(	application_name,
					"catches" );

		species_table_name =
			get_table_name(	application_name,
					"species" );

		sprintf( select,
"census_date,interview_location,interview_number,%s.family,%s.genus,%s.species,%s.common_name,%s.florida_state_code,kept_count,released_count",
			 species_table_name,
			 species_table_name,
			 species_table_name,
			 species_table_name,
			 species_table_name );

		if ( !end_date_string
		||   !*end_date_string
		||   strcmp( end_date_string, "end_date" ) == 0 )
		{	
			end_date_string = begin_date_string;
		}

		if (	fishing_purpose
		&& 	*fishing_purpose
		&&	strcmp( fishing_purpose, "fishing_purpose" ) != 0 )
		{
			sprintf(catches_where_clause,
			 	"and fishing_purpose = '%s'		"
			 	"and census_date between '%s' and '%s'	",
			 	fishing_purpose,
			 	begin_date_string,
			 	end_date_string );
		}
		else
		{
			sprintf(catches_where_clause,
			 	"and census_date between '%s' and '%s'	",
			 	begin_date_string,
			 	end_date_string );
		}

		sprintf( common_name_join_where_clause,
			 "and %s.family = %s.family		"
			 "and %s.genus = %s.genus		"
			 "and %s.species = %s.species		",
			 catches_table_name,
			 species_table_name,
			 catches_table_name,
			 species_table_name,
			 catches_table_name,
			 species_table_name );

		sprintf( sys_string,
			 "echo	\"select %s				 "
			 "	  from %s,%s				 "
			 "	  where 1 = 1 %s %s;\"			|"
			 "sql.e '^'					|"
			 "sort						 ",
			 select,
			 catches_table_name,
			 species_table_name,
			 catches_where_clause,
			 common_name_join_where_clause );

		catches_record_list = pipe2list( sys_string );
		new_beginning = 1;
	}

	if ( new_beginning )
	{
		new_beginning = 0;
		found_one = 0;
		if ( !list_rewind( catches_record_list ) ) return 0;
	}

	sprintf(search_string,
		"%s^%s^%s^",
		census_date,
		interview_location,
		interview_number );

	str_len = strlen( search_string );

	do {
		catches_record = list_get_pointer( catches_record_list );
		if ( strncmp( catches_record, search_string, str_len ) == 0 )
		{
			piece(	catches->family,
				'^',
				catches_record,
				3 );
		
			piece(	catches->genus,
				'^',
				catches_record,
				4 );
		
			piece(	catches->species,
				'^',
				catches_record,
				5 );
		
			piece(	catches->common_name,
				'^',
				catches_record,
				6 );
		
			piece(	catches->florida_state_code,
				'^',
				catches_record,
				7 );
		
			piece(	catches->kept_count,
				'^',
				catches_record,
				8 );
			catches->kept_integer =
				atoi( catches->kept_count );
		
			piece(	catches->released_count,
				'^',
				catches_record,
				9 );
			catches->released_integer =
				atoi( catches->released_count );

			found_one = 1;

			if ( !list_next( catches_record_list ) )
				last_one = 1;

			return 1;
		}

		/* No need to go to the end of the list */
		/* ------------------------------------ */
		if ( found_one ) break;

	} while( list_next( catches_record_list ) );

	new_beginning = 1;

	return 0;
}

boolean creel_library_census_date_get_catches_fresh_database_fetch(
				CATCHES *catches,
				char *application_name,
				char *fishing_purpose,
				char *census_date,
				char *interview_location,
				char *interview_number )
{
	char buffer[ 1024 ];
	static FILE *input_pipe = {0};

	if ( !input_pipe )
	{
		char sys_string[ 2048 ];
		char catches_where_clause[ 1024 ];
		char common_name_join_where_clause[ 1024 ];
		char select[ 1024 ];
		char *catches_table_name;
		char *species_table_name;

		catches_table_name =
			get_table_name(	application_name,
					"catches" );

		species_table_name =
			get_table_name(	application_name,
					"species" );

		sprintf( select,
"%s.family,%s.genus,%s.species,%s.common_name,%s.florida_state_code,kept_count,released_count",
			 species_table_name,
			 species_table_name,
			 species_table_name,
			 species_table_name,
			 species_table_name );

		sprintf( catches_where_clause,
			 "and fishing_purpose = '%s'		"
			 "and census_date = '%s'		"
			 "and interview_location = '%s'		"
			 "and interview_number = %s		",
			 fishing_purpose,
			 census_date,
			 interview_location,
			 interview_number );

		sprintf( common_name_join_where_clause,
			 "and %s.family = %s.family		"
			 "and %s.genus = %s.genus		"
			 "and %s.species = %s.species		",
			 catches_table_name,
			 species_table_name,
			 catches_table_name,
			 species_table_name,
			 catches_table_name,
			 species_table_name );

		sprintf( sys_string,
			 "echo	\"select %s				 "
			 "	  from %s,%s				 "
			 "	  where 1 = 1 %s %s;\"			|"
			 "sql.e						|"
			 "sort						 ",
			 select,
			 catches_table_name,
			 species_table_name,
			 catches_where_clause,
			 common_name_join_where_clause );

		input_pipe = popen( sys_string, "r" );
	}

	if ( !get_line( buffer, input_pipe ) )
	{
		pclose( input_pipe );
		input_pipe = (FILE *)0;
		return 0;
	}

	piece(	catches->family,
		FOLDER_DATA_DELIMITER,
		buffer,
		0 );

	piece(	catches->genus,
		FOLDER_DATA_DELIMITER,
		buffer,
		1 );

	piece(	catches->species,
		FOLDER_DATA_DELIMITER,
		buffer,
		2 );

	piece(	catches->common_name,
		FOLDER_DATA_DELIMITER,
		buffer,
		3 );

	piece(	catches->florida_state_code,
		FOLDER_DATA_DELIMITER,
		buffer,
		4 );

	piece(	catches->kept_count,
		FOLDER_DATA_DELIMITER,
		buffer,
		5 );
	catches->kept_integer =
		atoi( catches->kept_count );

	piece(	catches->released_count,
		FOLDER_DATA_DELIMITER,
		buffer,
		6 );
	catches->released_integer =
		atoi( catches->released_count );

	return 1;
} /* creel_library_census_date_get_catches_fresh_database_fetch() */

boolean creel_library_get_fishing_trips_between_dates_preferred_caught(
				FISHING_TRIPS *fishing_trips,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *family,
				char *genus,
				char *species_preferred,
				char *fishing_purpose )
{
	char buffer[ 1024 ];
	static FILE *input_pipe = {0};

	if ( !input_pipe )
	{
		input_pipe = creel_library_get_fishing_trips_input_pipe(
				application_name,
				fishing_purpose,
				begin_date_string,
				end_date_string,
				family,
				genus,
				species_preferred );
	}

	if ( !get_line( buffer, input_pipe ) )
	{
		pclose( input_pipe );
		input_pipe = (FILE *)0;
		return 0;
	}

	creel_library_parse_fishing_trips(
			fishing_trips,
			buffer );

	return 1;
} /* creel_library_get_fishing_trips_between_dates_preferred_caught() */

boolean creel_library_get_fishing_trips_between_dates(
				FISHING_TRIPS *fishing_trips,
				char *application_name,
				char *fishing_purpose,
				char *begin_date_string,
				char *end_date_string )
{
	char buffer[ 1024 ];
	static FILE *input_pipe = {0};

	if ( !input_pipe )
	{
		input_pipe = creel_library_get_fishing_trips_input_pipe(
				application_name,
				fishing_purpose,
				begin_date_string,
				end_date_string,
				(char *)0 /* family */,
				(char *)0 /* genus */,
				(char *)0 /* species_preferred */ );
	}

	if ( !get_line( buffer, input_pipe ) )
	{
		pclose( input_pipe );
		input_pipe = (FILE *)0;
		return 0;
	}

	creel_library_parse_fishing_trips(
			fishing_trips,
			buffer );

	return 1;
} /* creel_library_get_fishing_trips_between_dates() */

void creel_library_parse_fishing_trips(
			FISHING_TRIPS *fishing_trips,
			char *input_buffer )
{
	piece(	fishing_trips->fishing_purpose,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		0 );

	piece(	fishing_trips->census_date,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		1 );

	piece(	fishing_trips->interview_location,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		2 );

	piece(	fishing_trips->interview_number,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		3 );

	piece(	fishing_trips->interview_time,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		4 );

	piece(	fishing_trips->trip_origin_location,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		5 );

	piece(	fishing_trips->fishing_purpose,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		6 );

	piece(	fishing_trips->family,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		7 );

	piece(	fishing_trips->genus,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		8 );

	piece(	fishing_trips->species_preferred,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		9 );

	piece(	fishing_trips->fishing_area,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		10 );
	up_string( fishing_trips->fishing_area );

	piece(	fishing_trips->fishing_party_composition,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		11 );

	piece(	fishing_trips->fishing_trip_duration_hours,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		12 );

	piece(	fishing_trips->fishing_duration_hours,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		13 );

	piece(	fishing_trips->number_of_people_fishing,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		14 );

	piece(	fishing_trips->permit_code,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		15 );

	piece(	fishing_trips->recreational_angler_reside,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		16 );

} /* creel_library_parse_fishing_trips() */

boolean creel_library_get_fishing_trip(
				FISHING_TRIPS *fishing_trips,
				char *application_name,
				char *fishing_purpose,
				char *census_date,
				char *interview_location )
{
	char buffer[ 1024 ];
	static FILE *input_pipe = {0};

	if ( !input_pipe )
	{
		char sys_string[ 2048 ];
		char select[ 1024 ];
		char where_clause[ 1024 ];
		char *order;
		char *fishing_trips_table;

		fishing_trips_table =
			get_table_name( application_name,
					"fishing_trips" );

		sprintf( select,
			 CREEL_LIBRARY_FISHING_TRIPS_SELECT,
			 fishing_trips_table );

		if ( interview_location && *interview_location )
		{
			sprintf( where_clause,
			 	"fishing_purpose = '%s' and		"
			 	"census_date = '%s' and			"
			 	"interview_location = '%s'		",
			 	fishing_purpose,
			 	census_date,
			 	interview_location );
		}
		else
		{
			sprintf( where_clause,
			 	"fishing_purpose = '%s' and		"
			 	"census_date = '%s' 			",
			 	fishing_purpose,
			 	census_date );
		}

		order = "census_date,interview_number";

		sprintf( sys_string,
			 "get_folder_data	application=%s		"
			 "			folder=fishing_trips	"
			 "			select=%s		"
			 "			where=\"%s\"		"
			 "			order=%s		",
			 application_name,
			 select,
			 where_clause,
			 order );

		input_pipe = popen( sys_string, "r" );
	}

	if ( !get_line( buffer, input_pipe ) )
	{
		pclose( input_pipe );
		input_pipe = (FILE *)0;
		return 0;
	}

	creel_library_parse_fishing_trips(
			fishing_trips,
			buffer );

	return 1;
}


boolean creel_library_get_catch_measurements(
				CATCH_MEASUREMENTS
					*catch_measurements,
				char *application_name,
				char *fishing_purpose,
				char *census_date,
				char *interview_location,
				char *interview_number )
{
	char buffer[ 1024 ];
	static FILE *input_pipe = {0};

	if ( !input_pipe )
	{
		char sys_string[ 2048 ];
		char select[ 1024 ];
		char measurements_where_clause[ 1024 ];
		char common_name_join_where_clause[ 1024 ];
		char *catch_measurements_table_name;
		char *species_table_name;

		catch_measurements_table_name =
			get_table_name(	application_name,
					"catch_measurements" );

		species_table_name =
			get_table_name(	application_name,
					"species" );

		sprintf( select,
"%s.family,%s.genus,%s.species,%s.common_name,%s.florida_state_code,catch_measurement_number,length_millimeters",
			 catch_measurements_table_name,
			 catch_measurements_table_name,
			 catch_measurements_table_name,
			 species_table_name,
			 species_table_name );

		sprintf( measurements_where_clause,
			 "and fishing_purpose = '%s'		"
			 "and census_date = '%s'		"
			 "and interview_location = '%s'		"
			 "and interview_number = %s		",
			 fishing_purpose,
			 census_date,
			 interview_location,
			 interview_number );

		sprintf( common_name_join_where_clause,
			 "and %s.family = %s.family		"
			 "and %s.genus = %s.genus		"
			 "and %s.species = %s.species		",
			 catch_measurements_table_name,
			 species_table_name,
			 catch_measurements_table_name,
			 species_table_name,
			 catch_measurements_table_name,
			 species_table_name );

		sprintf( sys_string,
			 "echo	\"select %s				 "
			 "	  from %s,%s				 "
			 "	  where 1 = 1 %s %s;\"			|"
			 "sql.e						|"
			 "sort						 ",
			 select,
			 catch_measurements_table_name,
			 species_table_name,
			 measurements_where_clause,
			 common_name_join_where_clause );


		input_pipe = popen( sys_string, "r" );
	}

	if ( !get_line( buffer, input_pipe ) )
	{
		pclose( input_pipe );
		input_pipe = (FILE *)0;
		return 0;
	}

	piece(	catch_measurements->family,
		FOLDER_DATA_DELIMITER,
		buffer,
		0 );

	piece(	catch_measurements->genus,
		FOLDER_DATA_DELIMITER,
		buffer,
		1 );

	piece(	catch_measurements->species,
		FOLDER_DATA_DELIMITER,
		buffer,
		2 );

	piece(	catch_measurements->common_name,
		FOLDER_DATA_DELIMITER,
		buffer,
		3 );

	piece(	catch_measurements->florida_state_code,
		FOLDER_DATA_DELIMITER,
		buffer,
		4 );

	piece(	catch_measurements->catch_measurement_number,
		FOLDER_DATA_DELIMITER,
		buffer,
		5 );

	piece(	catch_measurements->length_millimeters,
		FOLDER_DATA_DELIMITER,
		buffer,
		6 );
	catch_measurements->length_millimeters_integer =
		atoi( catch_measurements->length_millimeters );

	return 1;
}

int creel_library_get_trailer_count(
				char *application_name,
				char *census_date,
				char *parking_lot )
{
	char sys_string[ 1024 ];
	char where[ 1024 ];
	char *return_string;

	sprintf( where,
		 "date = '%s' and		"
		 "parking_lot = '%s'		",
		 census_date,
		 parking_lot );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=trailer_count		"
		 "			folder=trailer_count		"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return_string = pipe2string( sys_string );
	if ( return_string && *return_string )
		return atoi( return_string );
	else
		return 0;

}

CATCHES *creel_library_get_fishing_area_summary_catches(
				HASH_TABLE *catches_hash_table,
				char *family,
				char *genus,
				char *species,
				char *fishing_area,
				char *common_name,
				char *florida_state_code )
{
	CATCHES *catches;
	char *hash_table_key;

	hash_table_key =
		creel_library_get_species_fishing_area_hash_table_key(
			family,
			genus,
			species,
			fishing_area );

	if ( !( catches = hash_table_get_pointer(
					catches_hash_table,
					hash_table_key ) ) )
	{
		catches = creel_library_catches_new();

		strcpy( catches->family, family );
		strcpy( catches->genus, genus );
		strcpy( catches->species, species );
		strcpy( catches->fishing_area, fishing_area );
		strcpy( catches->common_name, common_name );
		strcpy(	catches->florida_state_code,
			florida_state_code );

		hash_table_add_pointer(
			catches_hash_table,
			strdup( hash_table_key ),
			catches );
	}
	return catches;
}

CATCHES *creel_library_get_summary_catches(
				HASH_TABLE *catches_hash_table,
				char *family,
				char *genus,
				char *species,
				char *common_name,
				char *florida_state_code )
{
	CATCHES *catches;
	char *hash_table_key;

	hash_table_key =
		creel_library_get_species_hash_table_key(
			family,
			genus,
			species );

	if ( !( catches = hash_table_get_pointer(
					catches_hash_table,
					hash_table_key ) ) )
	{
		catches = creel_library_catches_new();

		strcpy( catches->family, family );
		strcpy( catches->genus, genus );
		strcpy( catches->species, species );
		strcpy( catches->common_name, common_name );
		strcpy(	catches->florida_state_code,
			florida_state_code );

		hash_table_add_pointer(
			catches_hash_table,
			strdup( hash_table_key ),
			catches );
	}
	return catches;
}

CATCHES *creel_library_catches_new( void )
{
	CATCHES *catches;

	catches = (CATCHES *) calloc( 1, sizeof( CATCHES ) );

	if ( !catches )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return catches;
}

CATCH_MEASUREMENTS *creel_library_catch_measurements_new( void )
{
	CATCH_MEASUREMENTS *catch_measurements;

	catch_measurements =
		(CATCH_MEASUREMENTS *)
			calloc( 1, sizeof( CATCH_MEASUREMENTS ) );

	if ( !catch_measurements )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return catch_measurements;
}

CATCH_MEASUREMENTS
	*creel_library_get_summary_catch_measurements(
				HASH_TABLE *catch_measurements_hash_table,
				char *family,
				char *genus,
				char *species,
				char *common_name,
				char *florida_state_code )
{
	CATCH_MEASUREMENTS *catch_measurements;
	char *hash_table_key;

	hash_table_key =
		creel_library_get_species_hash_table_key(
			family,
			genus,
			species );

	if ( !( catch_measurements = hash_table_get_pointer(
					catch_measurements_hash_table,
					hash_table_key ) ) )
	{
		catch_measurements =
			creel_library_catch_measurements_new();

		strcpy( catch_measurements->family, family );
		strcpy( catch_measurements->genus, genus );
		strcpy( catch_measurements->species, species );
		strcpy(	catch_measurements->common_name,
			common_name );
		strcpy(	catch_measurements->florida_state_code,
			florida_state_code );

		hash_table_add_pointer(
			catch_measurements_hash_table,
			strdup( hash_table_key ),
			catch_measurements );
	}
	return catch_measurements;
}

boolean creel_library_date_range_get_catches(
				CATCHES *catches,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *fishing_area_list_string,
				char *family_list_string,
				char *genus_list_string,
				char *species_list_string,
				char *fishing_purpose )
{
	char buffer[ 1024 ];
	char piece_buffer[ 128 ];
	static FILE *input_pipe = {0};

	if ( !input_pipe )
	{
		char sys_string[ 65536 ];
		char where_clause[ 65536 ];
		char join_fishing_trips_where_clause[ 512 ];
		char join_species_where_clause[ 512 ];
		char family_list_where_clause[ 65536 ];
		char *fishing_trips_table_name;
		char *catches_table_name;
		char *species_table_name;
		char catches_select[ 1024 ];
		char *order_clause;

		fishing_trips_table_name =
			get_table_name(	application_name,
					"fishing_trips" );

		catches_table_name =
			get_table_name(	application_name,
					"catches" );

		species_table_name =
			get_table_name(	application_name,
					"species" );

		sprintf( catches_select,
"%s.census_date,%s.family,%s.genus,%s.species,%s.common_name,%s.florida_state_code,kept_count,released_count,fishing_area,hours_fishing,number_of_people_fishing,%s.fishing_purpose,%s.interview_location,%s.interview_number",
			 catches_table_name,
			 species_table_name,
			 species_table_name,
			 species_table_name,
			 species_table_name,
			 species_table_name,
			 catches_table_name,
			 catches_table_name,
			 catches_table_name);

		creel_library_get_fishing_trips_census_date_where_clause(
				where_clause,
				application_name,
				fishing_area_list_string,
				begin_date_string,
				end_date_string,
				fishing_purpose );

		sprintf( join_fishing_trips_where_clause,
			 " and %s.fishing_purpose = %s.fishing_purpose	     "
			 " and %s.census_date = %s.census_date		     "
			 " and %s.interview_location = %s.interview_location "
			 " and %s.interview_number = %s.interview_number     ",
			 catches_table_name,
			 fishing_trips_table_name,
			 catches_table_name,
			 fishing_trips_table_name,
			 catches_table_name,
			 fishing_trips_table_name,
			 catches_table_name,
			 fishing_trips_table_name );

		sprintf( join_species_where_clause,
			 " and %s.family = %s.family		     "
			 " and %s.genus = %s.genus		     "
			 " and %s.species = %s.species		     ",
			 catches_table_name,
			 species_table_name,
			 catches_table_name,
			 species_table_name,
			 catches_table_name,
			 species_table_name );

		if ( !*family_list_string
		||   strcmp( family_list_string, "family" ) == 0 )
		{
			strcpy( family_list_where_clause,
				" and 1 = 1" );
		}
		else
		{
			creel_library_build_family_list_where_clause(
				family_list_where_clause,
				application_name,
				"species",
				family_list_string,
				genus_list_string,
				species_list_string );
		}

		order_clause = "fishing_purpose,census_date,interview_number";

		sprintf(sys_string,
			"echo \"select %s				     "
			"	 from %s,%s,%s				     "
			"	 where 1 = 1 %s %s %s %s		     "
			"	 order by %s;\"				     |"
			"sql.e						     ",
			catches_select,
			catches_table_name,
			species_table_name,
			fishing_trips_table_name,
			where_clause,
			join_species_where_clause,
			join_fishing_trips_where_clause,
		       	family_list_where_clause,
			order_clause );

		input_pipe = popen( sys_string, "r" );
	}

	if ( !get_line( buffer, input_pipe ) )
	{
		pclose( input_pipe );
		input_pipe = (FILE *)0;
		return 0;
	}

	piece(	catches->census_date,
		FOLDER_DATA_DELIMITER,
		buffer,
		0 );

	piece(	catches->family,
		FOLDER_DATA_DELIMITER,
		buffer,
		1 );

	piece(	catches->genus,
		FOLDER_DATA_DELIMITER,
		buffer,
		2 );

	piece(	catches->species,
		FOLDER_DATA_DELIMITER,
		buffer,
		3 );

	piece(	catches->common_name,
		FOLDER_DATA_DELIMITER,
		buffer,
		4 );

	piece(	catches->florida_state_code,
		FOLDER_DATA_DELIMITER,
		buffer,
		5 );

	piece(	catches->kept_count,
		FOLDER_DATA_DELIMITER,
		buffer,
		6 );
	catches->kept_integer = atoi( catches->kept_count );

	piece(	catches->released_count,
		FOLDER_DATA_DELIMITER,
		buffer,
		7 );
	catches->released_integer = atoi( catches->released_count );

	catches->caught_integer =
			catches->kept_integer +
			catches->released_integer;

	piece(	catches->fishing_area,
		FOLDER_DATA_DELIMITER,
		buffer,
		8 );
	up_string( catches->fishing_area );

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		buffer,
		9 );
	catches->hours_fishing = atoi( piece_buffer );

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		buffer,
		10 );
	catches->number_of_people_fishing = atoi( piece_buffer );

	piece(	catches->fishing_purpose,
		FOLDER_DATA_DELIMITER,
		buffer,
		11 );

	piece(	catches->interview_location,
		FOLDER_DATA_DELIMITER,
		buffer,
		12 );

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		buffer,
		13 );
	catches->interview_number = atoi( piece_buffer );

	return 1;
}

FILE *creel_library_date_range_get_fishing_trip_input_pipe(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *fishing_area_list_string,
				char *fishing_purpose,
				char *preferred_family_string,
				char *preferred_genus_string,
				char *preferred_species_string,
				boolean exclude_zero_catches )
{
	char sys_string[ 4096 ];
	char sub_query_where[ 1024 ];
	char where_clause[ 4096 ];
	char *fishing_trips_table_name;
	char *catches_table_name;
	char *fishing_trips_select;

	fishing_trips_table_name =
		get_table_name(	application_name,
				"fishing_trips" );

	catches_table_name =
		get_table_name(	application_name,
				"catches" );

	fishing_trips_select =
"fishing_purpose,census_date,interview_location,interview_number,family,genus,species_preferred,fishing_area,hours_fishing,number_of_people_fishing";

	creel_library_get_fishing_trips_census_date_where_clause(
			where_clause,
			application_name,
			fishing_area_list_string,
			begin_date_string,
			end_date_string,
			fishing_purpose );

	if ( !exclude_zero_catches )
	{
		strcpy( sub_query_where, "1 = 1" );
	}
	else
	{
		sprintf( sub_query_where,
			 "exists (					      "
			 "select * from %s				      "
			 "where %s.fishing_purpose = %s.fishing_purpose	      "
			 "  and %s.census_date = %s.census_date		      "
			 "  and %s.interview_location = %s.interview_location "
			 "  and %s.interview_number = %s.interview_number     "
			 "  and %s.family = '%s'			      "
			 "  and %s.genus = '%s'				      "
			 "  and %s.species = '%s' )			      ",
			 catches_table_name,
			 catches_table_name,
			 fishing_trips_table_name,
			 catches_table_name,
			 fishing_trips_table_name,
			 catches_table_name,
			 fishing_trips_table_name,
			 catches_table_name,
			 fishing_trips_table_name,
			 catches_table_name,
			 preferred_family_string,
			 catches_table_name,
			 preferred_genus_string,
			 catches_table_name,
			 preferred_species_string );
	}

	sprintf(sys_string,
		"echo \"select %s				     "
		"	 from %s				     "
		"	 where 1 = 1 %s and %s;\"		    |"
		"sql.e | sort					     ",
		fishing_trips_select,
		fishing_trips_table_name,
		where_clause,
		sub_query_where );

	return popen( sys_string, "r" );
}


boolean creel_library_date_range_get_fishing_trip(
				FISHING_TRIPS *fishing_trip,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *fishing_area_list_string,
				char *fishing_purpose,
				char *preferred_family_string,
				char *preferred_genus_string,
				char *preferred_species_string,
				boolean exclude_zero_catches )
{
	char buffer[ 1024 ];
	static FILE *input_pipe = {0};

	if ( !input_pipe )
	{
		input_pipe =
			creel_library_date_range_get_fishing_trip_input_pipe(
				application_name,
				begin_date_string,
				end_date_string,
				fishing_area_list_string,
				fishing_purpose,
				preferred_family_string,
				preferred_genus_string,
				preferred_species_string,
				exclude_zero_catches );
	}

	if ( !get_line( buffer, input_pipe ) )
	{
		pclose( input_pipe );
		input_pipe = (FILE *)0;
		return 0;
	}

/*
"fishing_purpose,census_date,interview_location,interview_number,family,genus,species_preferred,fishing_area,hours_fishing,number_of_people_fishing";
*/
	piece(	fishing_trip->fishing_purpose,
		FOLDER_DATA_DELIMITER,
		buffer,
		0 );

	piece(	fishing_trip->census_date,
		FOLDER_DATA_DELIMITER,
		buffer,
		1 );

	piece(	fishing_trip->interview_location,
		FOLDER_DATA_DELIMITER,
		buffer,
		2 );

	piece(	fishing_trip->interview_number,
		FOLDER_DATA_DELIMITER,
		buffer,
		3 );

	piece(	fishing_trip->family,
		FOLDER_DATA_DELIMITER,
		buffer,
		4 );

	piece(	fishing_trip->genus,
		FOLDER_DATA_DELIMITER,
		buffer,
		5 );

	piece(	fishing_trip->species_preferred,
		FOLDER_DATA_DELIMITER,
		buffer,
		6 );

	piece(	fishing_trip->fishing_area,
		FOLDER_DATA_DELIMITER,
		buffer,
		7 );
	up_string( fishing_trip->fishing_area );

	piece(	fishing_trip->fishing_duration_hours,
		FOLDER_DATA_DELIMITER,
		buffer,
		8 );

	piece(	fishing_trip->number_of_people_fishing,
		FOLDER_DATA_DELIMITER,
		buffer,
		9 );

	return 1;
}

void creel_library_build_family_list_where_clause(
				char *family_list_where_clause,
				char *application_name,
				char *folder_name,
				char *family_list_string,
				char *genus_list_string,
				char *species_list_string )
{
	char *table_name;

	table_name = get_table_name( application_name, folder_name );

	if ( !genus_list_string && !species_list_string )
	{
		sprintf(family_list_where_clause,
			" and %s.family in (%s)",
			table_name,
			timlib_get_in_clause( family_list_string ) );
	}
	else
	{
		sprintf(family_list_where_clause,
			" and %s.family in (%s)"
			" and %s.genus in (%s)"
			" and %s.species in (%s)",
			table_name,
			timlib_get_in_clause( family_list_string ),
			table_name,
			timlib_get_in_clause( genus_list_string ),
			table_name,
			timlib_get_in_clause( species_list_string ) );
	}

} /* creel_library_build_family_list_where_clause() */

void creel_library_get_fishing_trips_census_date_where_clause(
			char *where_clause,
			char *application_name,
			char *fishing_area_list_string,
			char *begin_date_string,
			char *end_date_string,
			char *fishing_purpose )
{
	char *fishing_trips_table_name;
	char *fishing_area_where;
	char fishing_purpose_where[ 128 ];
	char *hours_fishing_where;

	if (	fishing_purpose
	&& 	*fishing_purpose
	&&	strcmp( fishing_purpose, "fishing_purpose" ) != 0 )
	{
		char *fishing_trips_table_name;

		fishing_trips_table_name =
			get_table_name(
				application_name, "fishing_trips" );

		sprintf( fishing_purpose_where,
			 "%s.fishing_purpose = '%s'",
			 fishing_trips_table_name,
			 fishing_purpose );
	}
	else
	{
		strcpy( fishing_purpose_where, "1 = 1" );
	}

	/* Places preceeding "and" */
	/* ----------------------- */
	fishing_area_where =
		creel_library_get_fishing_area_where(
			fishing_area_list_string );

	fishing_trips_table_name =
		get_table_name(	application_name,
				"fishing_trips" );

	hours_fishing_where =
		"hours_fishing is not null and hours_fishing > 0";

	sprintf(where_clause,
" and %s.census_date between '%s' and '%s' %s and %s and %s",
			fishing_trips_table_name,
	 	begin_date_string,
	 	end_date_string,
		fishing_area_where,
		fishing_purpose_where,
		hours_fishing_where );

} /* creel_library_get_fishing_trips_census_date_where_clause() */

char *creel_library_get_recreational_angler_reside(
				char *application_name,
				char *recreational_angler_reside,
				char *recreational_angler_reside_code )
{
	static LIST *recreational_angler_reside_list = {0};
	static char local_recreational_angler_reside[ 128 ];
	static char local_recreational_angler_reside_code[ 128 ];
	char *record;
	char *select = "recreational_angler_reside,code";

	if ( !recreational_angler_reside_list )
	{
		char sys_string[ 1024 ];

		sprintf(
		sys_string,
		"get_folder_data	application=%s			 "
		"			select=%s			 "
		"			folder=recreational_angler_reside",
		application_name,
		select );

		recreational_angler_reside_list = pipe2list( sys_string );
	}

	if ( !list_rewind( recreational_angler_reside_list ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: empty recreational_angler_reside_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		record = list_get_pointer( recreational_angler_reside_list );

		piece(	local_recreational_angler_reside,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		piece(	local_recreational_angler_reside_code,
			FOLDER_DATA_DELIMITER,
			record,
			1 );

		if ( recreational_angler_reside )
		{
			if ( strcmp(	local_recreational_angler_reside,
					recreational_angler_reside ) == 0 )
			{
				return local_recreational_angler_reside_code;
			}
		}
		else
		{
			if ( atoi( local_recreational_angler_reside_code ) ==
			     atoi( recreational_angler_reside_code ) )
			{
				return local_recreational_angler_reside;
			}
		}

	} while( list_next( recreational_angler_reside_list ) );

	if ( recreational_angler_reside )
		return recreational_angler_reside;
	else
		return recreational_angler_reside_code;
} /* creel_library_get_recreational_angler_reside() */

char *creel_library_get_fishing_composition(
				char *application_name,
				char *fishing_party_composition,
				char *fishing_party_composition_code )
{
	static LIST *fishing_party_composition_list = {0};
	static char local_fishing_party_composition[ 128 ];
	static char local_fishing_party_composition_code[ 128 ];
	char *record;
	char *select = "fishing_party_composition,code";

	if ( !fishing_party_composition_list )
	{
		char sys_string[ 1024 ];

		sprintf(
		sys_string,
		"get_folder_data	application=%s			 "
		"			select=%s			 "
		"			folder=fishing_party_compositions",
		application_name,
		select );

		fishing_party_composition_list = pipe2list( sys_string );
	}

	if ( !list_rewind( fishing_party_composition_list ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: empty fishing_party_composition_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		record = list_get_pointer( fishing_party_composition_list );

		piece(	local_fishing_party_composition,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		piece(	local_fishing_party_composition_code,
			FOLDER_DATA_DELIMITER,
			record,
			1 );

		if ( fishing_party_composition )
		{
			if ( strcmp(	local_fishing_party_composition,
					fishing_party_composition ) == 0 )
			{
				return local_fishing_party_composition_code;
			}
		}
		else
		{
			if ( atoi( local_fishing_party_composition_code ) ==
			     atoi( fishing_party_composition_code ) )
			{
				return local_fishing_party_composition;
			}
		}

	} while( list_next( fishing_party_composition_list ) );

	if ( fishing_party_composition )
		return fishing_party_composition;
	else
		return fishing_party_composition_code;

} /* creel_library_get_fishing_composition() */

char *creel_library_get_trip_origin(	char *application_name,
					char *trip_origin_location,
					char *trip_origin_location_code )
{
	static LIST *trip_origin_location_list = {0};
	static char local_trip_origin_location[ 128 ];
	static char local_trip_origin_location_code[ 128 ];
	char *record;
	char *select = "trip_origin_location,code";

	if ( !trip_origin_location_list )
	{
		char sys_string[ 1024 ];

		sprintf(
		sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=trip_origin_locations	",
		application_name,
		select );

		trip_origin_location_list = pipe2list( sys_string );
	}

	if ( !list_rewind( trip_origin_location_list ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: empty trip_origin_location_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		record = list_get_pointer( trip_origin_location_list );

		piece(	local_trip_origin_location,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		piece(	local_trip_origin_location_code,
			FOLDER_DATA_DELIMITER,
			record,
			1 );

		if (trip_origin_location )
		{
			if ( strcmp(	local_trip_origin_location,
					trip_origin_location ) == 0 )
			{
				return local_trip_origin_location_code;
			}
		}
		else
		{
			if ( atoi( local_trip_origin_location_code ) ==
			     atoi( trip_origin_location_code ) )
			{
				return local_trip_origin_location;
			}
		}

	} while( list_next( trip_origin_location_list ) );

	if ( trip_origin_location )
		return trip_origin_location;
	else
		return trip_origin_location_code;

} /* creel_library_get_trip_origin() */

char *creel_library_get_interview_location(
				char *application_name,
				char *interview_location,
				char *interview_location_code )
{
	static LIST *interview_location_list = {0};
	static char local_interview_location[ 128 ];
	static char local_interview_location_code[ 128 ];
	char *record;
	char *select = "interview_location,code";

	if ( !interview_location_list )
	{
		char sys_string[ 1024 ];

		sprintf(
		sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=interview_locations	",
		application_name,
		select );

		interview_location_list = pipe2list( sys_string );
	}

	if ( !list_rewind( interview_location_list ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: empty interview_location_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		record = list_get_pointer( interview_location_list );

		piece(	local_interview_location,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		piece(	local_interview_location_code,
			FOLDER_DATA_DELIMITER,
			record,
			1 );

		if ( interview_location )
		{
			if ( strcmp(	local_interview_location,
					interview_location ) == 0 )
			{
				return local_interview_location_code;
			}
		}
		else
		{
			if ( atoi( local_interview_location_code ) == 
			     atoi( interview_location_code ) )
			{
				return local_interview_location;
			}
		}

	} while( list_next( interview_location_list ) );

	if ( interview_location )
		return interview_location;
	else
		return interview_location_code;

} /* creel_library_get_interview_location() */

char *creel_library_get_researcher(
				char *application_name,
				char *researcher,
				char *researcher_code )
{
	static LIST *researcher_list = {0};
	static char local_researcher[ 128 ];
	static char local_researcher_code[ 128 ];
	char *record;
	char *select = "researcher,code";

	if ( !researcher_list )
	{
		char sys_string[ 1024 ];

		sprintf(
		sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=researchers		",
		application_name,
		select );

		researcher_list = pipe2list( sys_string );
	}

	if ( !list_rewind( researcher_list ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: empty researcher_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		record = list_get_pointer( researcher_list );

		piece(	local_researcher,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		piece(	local_researcher_code,
			FOLDER_DATA_DELIMITER,
			record,
			1 );

		if ( researcher )
		{
			if ( strcmp(	local_researcher,
					researcher ) == 0 )
			{
				return local_researcher_code;
			}
		}
		else
		{
			if ( !atoi( researcher_code ) )
				return CREEL_LIBRARY_UNKNOWN_RESEARCHER;

			if ( atoi( local_researcher_code ) == 
			     atoi( researcher_code ) )
			{
				return local_researcher;
			}
		}

	} while( list_next( researcher_list ) );

	return CREEL_LIBRARY_UNKNOWN_RESEARCHER;

} /* creel_library_get_researcher() */

char *creel_library_get_species(
				char *application_name,
				char *family_name,
				char *genus_name,
				char *species_name,
				char *species_code )
{
	static LIST *species_list = {0};
	char local_family_name[ 128 ];
	char local_genus_name[ 128 ];
	char local_species_name[ 128 ];
	char local_family_genus_species[ 128 ];
	char local_code[ 128 ];
	char *record;
	char *select = "family,genus,species,florida_state_code";

	if ( !species_list )
	{
		char sys_string[ 1024 ];

		sprintf(
		sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=species			",
		application_name,
		select );

		species_list = pipe2list( sys_string );
	}

	if ( !list_rewind( species_list ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: empty species_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		record = list_get_pointer( species_list );

		piece(	local_family_name,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		piece(	local_genus_name,
			FOLDER_DATA_DELIMITER,
			record,
			1 );

		piece(	local_species_name,
			FOLDER_DATA_DELIMITER,
			record,
			2 );

		piece(	local_code,
			FOLDER_DATA_DELIMITER,
			record,
			3 );

		if ( family_name && genus_name && species_name )
		{
			if ( strcmp(	family_name,
					local_family_name ) == 0
			&&   strcmp(	genus_name,
					local_genus_name ) == 0
			&&   strcmp(	species_name,
					local_species_name ) == 0 )
			{
				return strdup( local_code);
			}
		}
		else
		{
			if ( atoi( local_code ) == 
			     atoi( species_code ) )
			{
				sprintf( local_family_genus_species,
					 "%s %s %s",
					 local_family_name,
					 local_genus_name,
					 local_species_name );
				return strdup( local_family_genus_species );
			}
		}

	} while( list_next( species_list ) );

	return (char *)0;

} /* creel_library_get_species() */

#ifdef NOT_DEFINED
char *creel_library_get_family(
				char *application_name,
				char *family_name,
				char *family_code )
{
	static LIST *family_list = {0};
	static char local_family_name[ 128 ];
	static char local_code[ 128 ];
	char *record;
	char *select = "family,florida_state_code";

	if ( !family_list )
	{
		char sys_string[ 1024 ];

		sprintf(
		sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=fish_families		",
		application_name,
		select );

		family_list = pipe2list( sys_string );
	}

	if ( !list_rewind( family_list ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: empty family_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		record = list_get_pointer( family_list );

		piece(	local_family_name,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		piece(	local_code,
			FOLDER_DATA_DELIMITER,
			record,
			1 );

		if ( family_name )
		{
			if ( strcmp(	family_name,
					local_family_name ) == 0 )
			{
				return local_code;
			}
		}
		else
		{
			if ( atoi( local_code ) == 
			     atoi( family_code ) )
			{
				return local_family_name;
			}
		}

	} while( list_next( family_list ) );

	return (char *)0;

} /* creel_library_get_family() */
#endif

char *creel_library_get_species_hash_table_key(
			char *family,
			char *genus,
			char *species )
{
	static char hash_table_key[ 128 ];
	sprintf( hash_table_key, "%s^%s^%s", family, genus, species );
	return hash_table_key;
}

char *creel_library_get_species_fishing_area_hash_table_key(
			char *family,
			char *genus,
			char *species,
			char *fishing_area )
{
	static char hash_table_key[ 256 ];
	sprintf( hash_table_key,
		 "%s^%s^%s^%s",
		 family, genus, species, fishing_area );
	return hash_table_key;
}

char *creel_library_get_catches_hash_table_key(
			char *fishing_purpose,
			char *census_date,
			char *interview_location,
			char *interview_number )
{
	static char hash_table_key[ 128 ];
	sprintf(	hash_table_key,
			"%s^%s^%s^%s",
			fishing_purpose,
			census_date,
			interview_location,
			interview_number );
	return hash_table_key;
} /* creel_library_get_catches_hash_table_key() */

char *creel_library_get_census_researcher(
				char *application_name,
				char *fishing_purpose,
				char *census_date,
				char *interview_location )
{
	static HASH_TABLE *census_hash_table = {0};
	char *key;
	char *researcher;

	if ( !census_hash_table )
	{
		char *select;
		char where[ 128 ];
		char sys_string[ 1024 ];
		char input_string[ 128 ];
		char local_fishing_purpose[ 16 ];
		char local_census_date[ 16 ];
		char local_interview_location[ 32 ];
		char local_researcher[ 32 ];
		FILE *input_pipe;

		census_hash_table =
			hash_table_new( hash_table_medium );

		select =
		"fishing_purpose,census_date,interview_location,researcher";

		sprintf( where, "fishing_purpose = '%s'", fishing_purpose );

		sprintf( sys_string,
			 "get_folder_data	application=%s		"
			 "			select=%s		"
			 "			folder=creel_census	"
			 "			where=\"%s\"		",
			 application_name,
			 select,
			 where );

		input_pipe = popen( sys_string, "r" );
		while( get_line( input_string, input_pipe ) )
		{
			piece(	local_fishing_purpose,
				FOLDER_DATA_DELIMITER,
				input_string,
				0 );

			piece(	local_census_date,
				FOLDER_DATA_DELIMITER,
				input_string,
				1 );

			piece(	local_interview_location,
				FOLDER_DATA_DELIMITER,
				input_string,
				2 );

			piece(	local_researcher,
				FOLDER_DATA_DELIMITER,
				input_string,
				3 );

			key = creel_library_get_census_hash_table_key(
					local_fishing_purpose,
					local_census_date,
					local_interview_location );

			hash_table_set_pointer(
				census_hash_table,
				strdup( key ),
				strdup( local_researcher ) );
		}
		pclose( input_pipe );
	}

	key = creel_library_get_census_hash_table_key(
			fishing_purpose,
			census_date,
			interview_location );

	if ( !( researcher = hash_table_get_pointer(
					census_hash_table,
					key ) ) )
	{
		return "";
	}

	return researcher;
} /* creel_library_get_census_researcher() */

char *creel_library_get_census_researcher_code(
				char *application_name,
				char *fishing_purpose,
				char *census_date,
				char *interview_location )
{
	static HASH_TABLE *researcher_hash_table = {0};
	char *researcher_code;
	char *researcher;

	if ( !( researcher = creel_library_get_census_researcher(
				application_name,
				fishing_purpose,
				census_date,
				interview_location ) ) )
	{
/*
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot find researcher for %s/%s/%s\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			fishing_purpose,
			census_date,
			interview_location );
		exit( 1 );
*/
		return "";
	}

	if ( researcher && !*researcher ) return "";

	if ( !researcher_hash_table )
	{
		char *select;
		char sys_string[ 1024 ];
		char input_string[ 128 ];
		char local_researcher_code[ 32 ];
		char local_researcher[ 32 ];
		FILE *input_pipe;

		researcher_hash_table =
			hash_table_new( hash_table_small );

		select = "researcher,code";

		sprintf( sys_string,
			 "get_folder_data	application=%s		"
			 "			select=%s		"
			 "			folder=researchers	",
			 application_name,
			 select );

		input_pipe = popen( sys_string, "r" );
		while( get_line( input_string, input_pipe ) )
		{
			piece(	local_researcher,
				FOLDER_DATA_DELIMITER,
				input_string,
				0 );

			piece(	local_researcher_code,
				FOLDER_DATA_DELIMITER,
				input_string,
				1 );

			hash_table_set_pointer(
				researcher_hash_table,
				strdup( local_researcher ),
				strdup( local_researcher_code ) );
		}
		pclose( input_pipe );
	}

	if ( !( researcher_code = hash_table_get_pointer(
					researcher_hash_table,
					researcher ) ) )
	{
		return "";
	}

	return researcher_code;
} /* creel_library_get_census_researcher_code() */

char *creel_library_get_recreational_angler_reside_code(
				char *application_name,
				char *recreational_angler_reside )
{
	static LIST *recreational_angler_record_list = {0};
	char search_string[ 128 ];
	char *record_pointer;
	static char reside_code[ 16 ];

	if ( !recreational_angler_record_list )
	{
		char *select;
		char *folder_name;
		char sys_string[ 1024 ];

		select = "recreational_angler_reside,code";
		folder_name = "recreational_angler_reside";

		sprintf( sys_string,
			 "get_folder_data	application=%s		"
			 "			select=%s		"
			 "			folder=%s		",
			 application_name,
			 select,
			 folder_name );

		recreational_angler_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( recreational_angler_record_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: empty recreational_angler_record_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( search_string,
		 "%s%c",
		 recreational_angler_reside,
		 FOLDER_DATA_DELIMITER );

	do {
		record_pointer =
			list_get_pointer( 	
				recreational_angler_record_list );
		if ( timlib_strncmp( record_pointer, search_string ) == 0 )
		{
			piece(	reside_code,
				FOLDER_DATA_DELIMITER,
				record_pointer,
				1 );
			return reside_code;
		}
	} while( list_next( recreational_angler_record_list ) );
	return "";
} /* creel_library_get_recreational_angler_reside_code() */

char *creel_library_get_census_hash_table_key(
			char *fishing_purpose,
			char *census_date,
			char *interview_location )
{
	static char hash_table_key[ 128 ];
	sprintf(	hash_table_key,
			"%s^%s^%s",
			fishing_purpose,
			census_date,
			interview_location );
	return hash_table_key;
} /* creel_library_get_census_hash_table_key() */

boolean creel_library_date_range_get_catch_measurements(
				CATCH_MEASUREMENTS *catch_measurements,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *fishing_area_list_string,
				char *family_list_string,
				char *genus_list_string,
				char *species_list_string )
{
	char buffer[ 1024 ];
	static FILE *input_pipe = {0};

	if ( !input_pipe )
	{
		char sys_string[ 65536 ];
		char where_clause[ 65536 ];
		char family_list_where_clause[ 65536 ];
		char join_fishing_trips_where_clause[ 512 ];
		char join_species_where_clause[ 512 ];
		char *fishing_trips_table_name;
		char *catch_measurements_table_name;
		char *species_table_name;
		char catch_measurements_select[ 1024 ];

		fishing_trips_table_name =
			get_table_name(	application_name,
					"fishing_trips" );

		catch_measurements_table_name =
			get_table_name(	application_name,
					"catch_measurements" );

		species_table_name =
			get_table_name(	application_name,
					"species" );

		sprintf( catch_measurements_select,
"%s.census_date,%s.interview_number,%s.interview_location,%s.florida_state_code,fishing_area,%s.family,%s.genus,%s.species,%s.common_name,length_millimeters",
			 catch_measurements_table_name,
			 catch_measurements_table_name,
			 catch_measurements_table_name,
			 species_table_name,
			 species_table_name,
			 species_table_name,
			 species_table_name,
			 species_table_name );

		creel_library_get_fishing_trips_census_date_where_clause(
				where_clause,
				application_name,
				fishing_area_list_string,
				begin_date_string,
				end_date_string,
				(char *)0 /* fishing_purpose */ );

		sprintf( join_fishing_trips_where_clause,
			 " and %s.fishing_purpose = %s.fishing_purpose	     "
			 " and %s.census_date = %s.census_date		     "
			 " and %s.interview_location = %s.interview_location "
			 " and %s.interview_number = %s.interview_number     ",
			 catch_measurements_table_name,
			 fishing_trips_table_name,
			 catch_measurements_table_name,
			 fishing_trips_table_name,
			 catch_measurements_table_name,
			 fishing_trips_table_name,
			 catch_measurements_table_name,
			 fishing_trips_table_name );

		sprintf( join_species_where_clause,
			 " and %s.family = %s.family		     "
			 " and %s.genus = %s.genus		     "
			 " and %s.species = %s.species		     ",
			 catch_measurements_table_name,
			 species_table_name,
			 catch_measurements_table_name,
			 species_table_name,
			 catch_measurements_table_name,
			 species_table_name );

		if ( !*family_list_string
		||   strcmp( family_list_string, "family" ) == 0 )
		{
			strcpy( family_list_where_clause,
				" and 1 = 1" );
		}
		else
		{
			creel_library_build_family_list_where_clause(
				family_list_where_clause,
				application_name,
				"species",
				family_list_string,
				genus_list_string,
				species_list_string );
		}

		sprintf(sys_string,
			"echo \"select %s				     "
			"	 from %s,%s,%s				     "
			"	 where 1 = 1 %s %s %s %s		     "
			"	 order by %s;\"				    |"
			"sql.e						     ",
			catch_measurements_select,
			catch_measurements_table_name,
			species_table_name,
			fishing_trips_table_name,
			where_clause,
			join_species_where_clause,
			join_fishing_trips_where_clause,
		       	family_list_where_clause,
			catch_measurements_select );

		input_pipe = popen( sys_string, "r" );
	}

	if ( !get_line( buffer, input_pipe ) )
	{
		pclose( input_pipe );
		input_pipe = (FILE *)0;
		return 0;
	}

/*
"%s.census_date,%s.interview_number,%s.interview_location,%s.florida_state_code,fishing_area,%s.family,%s.genus,%s.species,%s.common_name,length_millimeters",
*/
	piece(	catch_measurements->census_date,
		FOLDER_DATA_DELIMITER,
		buffer,
		0 );

	piece(	catch_measurements->interview_number,
		FOLDER_DATA_DELIMITER,
		buffer,
		1 );

	piece(	catch_measurements->interview_location,
		FOLDER_DATA_DELIMITER,
		buffer,
		2 );

	piece(	catch_measurements->florida_state_code,
		FOLDER_DATA_DELIMITER,
		buffer,
		3 );

	piece(	catch_measurements->fishing_area,
		FOLDER_DATA_DELIMITER,
		buffer,
		4 );
	up_string( catch_measurements->fishing_area );

	piece(	catch_measurements->family,
		FOLDER_DATA_DELIMITER,
		buffer,
		5 );

	piece(	catch_measurements->genus,
		FOLDER_DATA_DELIMITER,
		buffer,
		6 );

	piece(	catch_measurements->species,
		FOLDER_DATA_DELIMITER,
		buffer,
		7 );

	piece(	catch_measurements->common_name,
		FOLDER_DATA_DELIMITER,
		buffer,
		8 );

	piece(	catch_measurements->length_millimeters,
		FOLDER_DATA_DELIMITER,
		buffer,
		9 );
	catch_measurements->length_millimeters_integer =
		atoi( catch_measurements->length_millimeters );

	return 1;

} /* creel_library_date_range_get_catch_measurements() */

boolean creel_library_get_expired_guide_angler(
					GUIDE_ANGLER *guide_angler,
					char *application_name,
					char *days_offset,
					char *mail_merge_letter,
					boolean order_by_expiration_date )
{
	char sys_string[ 65536 ];
	char buffer[ 1024 ];
	static FILE *input_pipe = {0};

	if ( !input_pipe )
	{
		char where_clause[ 2048 ];
		char join_permits_where_clause[ 512 ];
		char *guide_anglers_table_name;
		char *permits_table_name;
		char select[ 256 ];
		char *order_by_clause;

		if ( order_by_expiration_date )
			order_by_clause = "expiration_date desc";
		else
			order_by_clause = "zip_code";

		guide_anglers_table_name =
			get_table_name(	application_name,
					"guide_anglers" );

		permits_table_name =
			get_table_name(	application_name,
					"permits" );

		sprintf( select,
"%s.guide_angler_name,street_address,city,state_code,zip_code,phone_number,email_address,permit_code,issued_date,expiration_date",
			 guide_anglers_table_name );

		creel_library_get_expired_permit_where_clause(
				where_clause,
				days_offset,
				mail_merge_letter );

		sprintf( join_permits_where_clause,
			 " and %s.guide_angler_name = %s.guide_angler_name   ",
			 guide_anglers_table_name,
			 permits_table_name );

		sprintf(sys_string,
			"echo \"select %s				     "
			"	 from %s,%s				     "
			"	 where 1 = 1 %s %s			     "
			"	 order by %s;\"				    |"
			"sql.e 						     ",
			select,
			guide_anglers_table_name,
			permits_table_name,
			where_clause,
			join_permits_where_clause,
			order_by_clause );

		input_pipe = popen( sys_string, "r" );
	}

	if ( !get_line( buffer, input_pipe ) )
	{
		pclose( input_pipe );
		input_pipe = (FILE *)0;
		return 0;
	}

	piece(	guide_angler->guide_angler_name,
		FOLDER_DATA_DELIMITER,
		buffer,
		0 );

	piece(	guide_angler->street_address,
		FOLDER_DATA_DELIMITER,
		buffer,
		1 );

	piece(	guide_angler->city,
		FOLDER_DATA_DELIMITER,
		buffer,
		2 );

	piece(	guide_angler->state_code,
		FOLDER_DATA_DELIMITER,
		buffer,
		3 );

	piece(	guide_angler->zip_code,
		FOLDER_DATA_DELIMITER,
		buffer,
		4 );

	piece(	guide_angler->phone_number,
		FOLDER_DATA_DELIMITER,
		buffer,
		5 );

	piece(	guide_angler->email_address,
		FOLDER_DATA_DELIMITER,
		buffer,
		6 );

	piece(	guide_angler->permit_code,
		FOLDER_DATA_DELIMITER,
		buffer,
		7 );

	piece(	guide_angler->issued_date,
		FOLDER_DATA_DELIMITER,
		buffer,
		8 );

	piece(	guide_angler->expiration_date,
		FOLDER_DATA_DELIMITER,
		buffer,
		9 );

	return 1;

} /* creel_library_get_expired_guide_angler() */

void creel_library_get_expired_permit_where_clause(
				char *where_clause,
				char *days_offset,
				char *mail_merge_letter )
{
	char *begin_date_string;
	char *end_date_string;

	creel_library_get_begin_end_dates(
				&begin_date_string,
				&end_date_string,
				days_offset,
				mail_merge_letter );

	sprintf( where_clause,
		 " and expiration_date between '%s' and '%s'",
		 begin_date_string,
		 end_date_string );

} /* creel_library_get_expired_permit_where_clause() */

void creel_library_get_begin_end_dates(
				char **begin_date_string,
				char **end_date_string,
				char *days_offset,
				char *mail_merge_letter )
{
	DATE *begin_date;
	DATE *end_date;

	begin_date = date_today_new( date_get_utc_offset() );
	end_date = date_today_new( date_get_utc_offset() );

	if ( strcmp( mail_merge_letter, "recently_expired" ) == 0 )
	{
		date_decrement_days(
				begin_date,
				atof( days_offset ),
				0 /* utc_offset */ );
	}
	else
	if ( strcmp( mail_merge_letter, "about_to_expire" ) == 0 )
	{
		date_increment_days(
				end_date,
				atof( days_offset ),
				0 /* utc_offset */ );
	}
	else
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: invalid mail_merge_letter = (%s)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			mail_merge_letter );
		exit( 1 );
	}


	*begin_date_string = strdup( date_display_yyyy_mm_dd( begin_date ) );
	*end_date_string = strdup( date_display_yyyy_mm_dd( end_date ) );

	date_free( begin_date );
	date_free( end_date );

} /* creel_library_get_expired_permit_where_clause() */

boolean creel_library_exists_fishing_area(
				char *application_name,
				char *fishing_area )
{
	static LIST *fishing_area_list = {0};
	char *local_fishing_area;
	char *select = "fishing_area";

	if ( !fishing_area_list )
	{
		char sys_string[ 1024 ];

		sprintf(
		sys_string,
		"get_folder_data	application=%s			 "
		"			select=%s			 "
		"			folder=fishing_area		 ",
		application_name,
		select );

		fishing_area_list = pipe2list( sys_string );
	}

	if ( !list_rewind( fishing_area_list ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: empty fishing_area_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		local_fishing_area = list_get_pointer( fishing_area_list );

		if ( strcasecmp(	local_fishing_area,
					fishing_area ) == 0 )
		{
			return 1;
		}
	} while( list_next( fishing_area_list ) );

	return 0;
} /* creel_library_exists_fishing_area() */

CATCH_PER_UNIT_EFFORT *creel_library_catch_per_unit_effort_new(
				void )
{
	CATCH_PER_UNIT_EFFORT *catch_per_unit_effort;

	catch_per_unit_effort =
		(CATCH_PER_UNIT_EFFORT *)
		calloc( 1, sizeof( CATCH_PER_UNIT_EFFORT ) );

	if ( !catch_per_unit_effort )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	catch_per_unit_effort->results_list = list_new();
	catch_per_unit_effort->results_per_species_per_area_list = list_new();
	catch_per_unit_effort->results_per_species_list = list_new();
	return catch_per_unit_effort;
} /* creel_library_catch_per_unit_effort_new() */

CATCH_PER_UNIT_EFFORT_FISHING_TRIP *catch_per_unit_effort_fishing_trip_new(
						char *fishing_purpose,
						char *census_date,
						char *interview_location,
						int interview_number,
						char *family,
						char *genus,
						char *species_preferred,
						char *fishing_area,
						int hours_fishing,
						int party_count )
{
	CATCH_PER_UNIT_EFFORT_FISHING_TRIP *fishing_trip;

	fishing_trip =
		(CATCH_PER_UNIT_EFFORT_FISHING_TRIP *)
		calloc( 1, sizeof( CATCH_PER_UNIT_EFFORT_FISHING_TRIP ) );

	if ( !fishing_trip )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	fishing_trip->fishing_purpose = fishing_purpose;
	fishing_trip->census_date = census_date;
	fishing_trip->interview_location = interview_location;
	fishing_trip->interview_number = interview_number;
	fishing_trip->family = family;
	fishing_trip->genus = genus;
	fishing_trip->species_preferred = species_preferred;
	fishing_trip->fishing_area = fishing_area;
	fishing_trip->hours_fishing = hours_fishing;
	fishing_trip->party_count = party_count;
	fishing_trip->catch_list = list_new();
	return fishing_trip;
} /* catch_per_unit_effort_fishing_trip_new() */

CATCH_PER_UNIT_EFFORT_CATCH *creel_library_catch_per_unit_effort_catch_new(
				char *family,
				char *genus,
				char *species,
				int caught_or_kept )
{
	CATCH_PER_UNIT_EFFORT_CATCH *catch;

	catch = (CATCH_PER_UNIT_EFFORT_CATCH *)
		calloc( 1, sizeof( CATCH_PER_UNIT_EFFORT_CATCH ) );

	if ( !catch )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	catch->family = family;
	catch->genus = genus;
	catch->species = species;
	catch->caught_or_kept = caught_or_kept;
	return catch;
} /* creel_library_catch_per_unit_effort_catch_new() */

CATCH_PER_UNIT_EFFORT_FISHING_TRIP *
	creel_library_fetch_catch_per_unit_effort_fishing_trip(
				LIST *fishing_trip_list,
				char *fishing_purpose,
				char *census_date,
				char *interview_location,
				int interview_number )
{
	CATCH_PER_UNIT_EFFORT_FISHING_TRIP *fishing_trip;

	if ( list_rewind( fishing_trip_list ) )
	{
		do {
			fishing_trip = list_get_pointer( fishing_trip_list );

			if ( strcmp(	fishing_trip->fishing_purpose, 
					fishing_purpose ) == 0
			&&   strcmp(	fishing_trip->census_date, 
					census_date ) == 0
			&&   strcmp(	fishing_trip->interview_location, 
					interview_location ) == 0
			&&   fishing_trip->interview_number ==
			     interview_number )
			{
				return fishing_trip;
			}
		} while( list_next( fishing_trip_list ) );
	}
	return (CATCH_PER_UNIT_EFFORT_FISHING_TRIP *)0;
} /* creel_library_fetch_catch_per_unit_effort_fishing_trip() */

LIST *creel_library_get_CPUE_fishing_trip_list(
					char *application_name,
					char *begin_date_string,
					char *end_date_string,
					char *fishing_area_list_string,
					char *family_list_string,
					char *genus_list_string,
					char *species_list_string,
					char *fishing_purpose,
					char *catch_harvest,
					boolean exclude_zero_catches )
{
	LIST *fishing_trip_list;
	LIST *preferred_family_list;
	LIST *preferred_genus_list;
	LIST *preferred_species_list;

	fishing_trip_list = list_new();

	preferred_family_list = list_string2list( family_list_string, ',' );
	preferred_genus_list = list_string2list( genus_list_string, ',' );
	preferred_species_list = list_string2list( species_list_string, ',' );

	if ( !list_length( preferred_family_list )
	||   list_length( preferred_family_list ) !=
		list_length( preferred_genus_list )
	||   list_length( preferred_family_list ) !=
		list_length( preferred_species_list ) )
	{
		return fishing_trip_list;
	}

	list_rewind( preferred_family_list );
	list_rewind( preferred_genus_list );
	list_rewind( preferred_species_list );

	do {
		list_append_list(
		     fishing_trip_list,
		     creel_library_get_CPUE_preferred_species_fishing_trip_list(
				application_name,
				begin_date_string,
				end_date_string,
				fishing_area_list_string,
				list_get_pointer( preferred_family_list ),
				list_get_pointer( preferred_genus_list ),
				list_get_pointer( preferred_species_list ),
				fishing_purpose,
				catch_harvest,
				exclude_zero_catches ) );

		list_next( preferred_genus_list );
		list_next( preferred_species_list );

	} while( list_next( preferred_family_list ) );

	return fishing_trip_list;

} /* creel_library_get_CPUE_fishing_trip_list() */

LIST *creel_library_get_CPUE_preferred_species_fishing_trip_list(
					char *application_name,
					char *begin_date_string,
					char *end_date_string,
					char *fishing_area_list_string,
					char *preferred_family_string,
					char *preferred_genus_string,
					char *preferred_species_string,
					char *fishing_purpose,
					char *catch_harvest,
					boolean exclude_zero_catches )
{
	FISHING_TRIPS fishing_trip;
	CATCH_PER_UNIT_EFFORT_FISHING_TRIP *catch_per_unit_effort_fishing_trip;
	CATCHES catches;
	CATCH_PER_UNIT_EFFORT_CATCH *catch_per_unit_effort_catch;
	LIST *fishing_trip_list;

	fishing_trip_list = list_new();

	while( creel_library_date_range_get_fishing_trip(
				&fishing_trip,
				application_name,
				begin_date_string,
				end_date_string,
				fishing_area_list_string,
				fishing_purpose,
				preferred_family_string,
				preferred_genus_string,
				preferred_species_string,
				exclude_zero_catches ) )
	{
		catch_per_unit_effort_fishing_trip =
			catch_per_unit_effort_fishing_trip_new(
				strdup( fishing_trip.fishing_purpose ),
				strdup( fishing_trip.census_date ),
				strdup( fishing_trip.interview_location ),
				atoi( fishing_trip.interview_number ),
				strdup( fishing_trip.family ),
				strdup( fishing_trip.genus ),
				strdup( fishing_trip.species_preferred ),
				strdup( fishing_trip.fishing_area ),
				atoi( fishing_trip.fishing_duration_hours ),
				atoi( fishing_trip.number_of_people_fishing ) );

		list_append_pointer(
			fishing_trip_list,
			catch_per_unit_effort_fishing_trip );
	}

	while( creel_library_date_range_get_catches(
					&catches,
					application_name,
					begin_date_string,
					end_date_string,
					fishing_area_list_string,
					preferred_family_string,
					preferred_genus_string,
					preferred_species_string,
					fishing_purpose ) )
	{
		if ( ! ( catch_per_unit_effort_fishing_trip =
			creel_library_fetch_catch_per_unit_effort_fishing_trip(
				fishing_trip_list,
				catches.fishing_purpose,
				catches.census_date,
				catches.interview_location,
				catches.interview_number ) ) )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: cannot fetch from fishing_trips: %s,%s,%s,%d.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				catches.fishing_purpose,
				catches.census_date,
				catches.interview_location,
				catches.interview_number );

			document_close();
			exit( 1 );
		}

		if ( timlib_strcmp( catch_harvest, "cpue" ) == 0 )
		{
			catch_per_unit_effort_catch =
				creel_library_catch_per_unit_effort_catch_new(
					strdup( catches.family ),
					strdup( catches.genus ),
					strdup( catches.species ),
					catches.kept_integer +
					catches.released_integer );
		}
		else
		{
			catch_per_unit_effort_catch =
				creel_library_catch_per_unit_effort_catch_new(
					strdup( catches.family ),
					strdup( catches.genus ),
					strdup( catches.species ),
					catches.kept_integer );
		}

		list_append_pointer(
			catch_per_unit_effort_fishing_trip->catch_list,
			catch_per_unit_effort_catch );
	}

	return fishing_trip_list;

} /* creel_library_get_CPUE_preferred_species_fishing_trip_list() */


CATCH_PER_UNIT_EFFORT_SPECIES *catch_per_unit_effort_species_new(
					char *family_caught,
					char *genus_caught,
					char *species_caught,
					char *fishing_area )
{
	CATCH_PER_UNIT_EFFORT_SPECIES *species;

	species = (CATCH_PER_UNIT_EFFORT_SPECIES *)
		calloc( 1, sizeof( CATCH_PER_UNIT_EFFORT_SPECIES ) );

	if ( !species )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	species->family_caught = family_caught;
	species->genus_caught = genus_caught;
	species->species_caught = species_caught;
	species->fishing_area = fishing_area;
	return species;
} /* catch_per_unit_effort_species_new() */

CATCH_PER_UNIT_EFFORT_SPECIES *catch_per_unit_effort_species_seek(
					LIST *species_list,
					char *family,
					char *genus,
					char *species_name,
					char *fishing_area )
{
	CATCH_PER_UNIT_EFFORT_SPECIES *species;

	if ( !list_rewind( species_list ) )
		return (CATCH_PER_UNIT_EFFORT_SPECIES *)0;

	do {
		species = list_get_pointer( species_list );

		if ( fishing_area )
		{
			if ( strcmp( species->family_caught, family ) == 0
			&&   strcmp( species->genus_caught, genus ) == 0
			&&   strcmp(	species->species_caught,
					species_name ) == 0
			&&   strcasecmp(species->fishing_area,
					fishing_area ) == 0 )
			{
				return species;
			}
		}
		else
		{
			if ( strcmp( species->family_caught, family ) == 0
			&&   strcmp( species->genus_caught, genus ) == 0
			&&   strcmp(	species->species_caught,
					species_name ) == 0 )
			{
				return species;
			}
		}
	} while( list_next( species_list ) );
	return (CATCH_PER_UNIT_EFFORT_SPECIES *)0;
} /* catch_per_unit_effort_species_seek() */

CATCH_PER_UNIT_EFFORT_CATCH *catch_per_unit_effort_catch_seek(
					LIST *catch_list,
					char *family,
					char *genus,
					char *species_name )
{
	CATCH_PER_UNIT_EFFORT_CATCH *catch;

	if ( !list_rewind( catch_list ) )
		return (CATCH_PER_UNIT_EFFORT_CATCH *)0;

	do {
		catch = list_get_pointer( catch_list );
		if ( strcmp( catch->family, family ) == 0
		&&   strcmp( catch->genus, genus ) == 0
		&&   strcmp( catch->species, species_name ) == 0 )
		{
			return catch;
		}
	} while( list_next( catch_list ) );
	return (CATCH_PER_UNIT_EFFORT_CATCH *)0;
} /* catch_per_unit_effort_catch_seek() */

CATCH_PER_UNIT_EFFORT_SPECIES *catch_per_unit_effort_get_per_species(
					LIST *results_per_species_list,
					char *family,
					char *genus,
					char *species_name,
					char *fishing_area,
					char *appaserver_data_directory )
{
	CATCH_PER_UNIT_EFFORT_SPECIES *species;

	if ( ! ( species = catch_per_unit_effort_species_seek(
					results_per_species_list,
					family,
					genus,
					species_name,
					fishing_area ) ) )
	{
		char sys_string[ 1024 ];

		species = catch_per_unit_effort_species_new(
					family,
					genus,
					species_name,
					fishing_area );

		/* Statistics */
		/* ---------- */
		species->statistics_filename =
		creel_library_get_effort_species_statistics_filename(
				family,
				genus,
				species_name,
				fishing_area,
				appaserver_data_directory );

		sprintf( sys_string,
			 "statistics_weighted.e '|' 0 > %s",
			 species->statistics_filename );

		species->statistics_pipe = popen( sys_string, "w" );

		/* Save the record to the list. */
		/* ---------------------------- */
		list_append_pointer( results_per_species_list, species );
	}

	return species;

} /* catch_per_unit_effort_get_per_species() */

void creel_library_get_CPUE_results_list(
				LIST *results_list,
				LIST *results_per_species_per_area_list,
				LIST *results_per_species_list,
				LIST *fishing_trip_list,
				char *family_list_string,
				char *genus_list_string,
				char *species_list_string,
				boolean summary_only,
				char *appaserver_data_directory )
{
	CATCH_PER_UNIT_EFFORT_SPECIES *species;
	CATCH_PER_UNIT_EFFORT_FISHING_TRIP *fishing_trip;
	CATCH_PER_UNIT_EFFORT_CATCH *catch;
	int effort;

	if ( !list_rewind( fishing_trip_list ) ) return;

	/* Accumulate for those that are caught. */
	/* ------------------------------------- */
	do {
		fishing_trip = list_get_pointer( fishing_trip_list );

		if ( !list_rewind( fishing_trip->catch_list ) ) continue;

		do {
			catch =
				list_get_pointer(
					fishing_trip->catch_list );

			/* Per species per area */
			/* -------------------- */
			species = catch_per_unit_effort_get_per_species(
					results_per_species_per_area_list,
					catch->family,
					catch->genus,
					catch->species,
					fishing_trip->fishing_area,
					appaserver_data_directory );

			effort = fishing_trip->hours_fishing *
				 fishing_trip->party_count;

			species->catches += catch->caught_or_kept;
			species->effort += effort;
			species->catch_records_count++;

			if ( effort )
			{
				fprintf( species->statistics_pipe,
					 "%lf\n",
					(double)catch->caught_or_kept /
					(double)effort );
			}
			else
			{
				fprintf( species->statistics_pipe,
					 "0.0\n" );
			}

			/* Per species */
			/* ----------- */
			species = catch_per_unit_effort_get_per_species(
					results_per_species_list,
					catch->family,
					catch->genus,
					catch->species,
					(char *)0 /* fishing_area */,
					appaserver_data_directory );

			effort = fishing_trip->hours_fishing *
				 fishing_trip->party_count;

			species->catches += catch->caught_or_kept;
			species->effort += effort;
			species->catch_records_count++;

			if ( effort )
			{
				fprintf( species->statistics_pipe,
					 "%lf\n",
					(double)catch->caught_or_kept /
					(double)effort );

/*
				fprintf( species->complete_statistics_pipe,
					 "%lf\n",
					(double)catch->caught_or_kept /
					(double)effort );
*/
			}
			else
			{
				fprintf( species->statistics_pipe,
					 "0.0\n" );
			}

			/* Detail */
			/* ------ */
			if ( !summary_only )
			{
				species = catch_per_unit_effort_species_new(
						catch->family,
						catch->genus,
						catch->species,
						fishing_trip->fishing_area );

				catch_per_unit_effort_set_detail_species(
					species,
					fishing_trip->census_date,
					fishing_trip->fishing_purpose,
					fishing_trip->interview_location,
					fishing_trip->interview_number,
					fishing_trip->family,
					fishing_trip->genus,
					fishing_trip->species_preferred,
					fishing_trip->hours_fishing,
					fishing_trip->party_count,
					catch->family,
					catch->genus,
					catch->species,
					catch->caught_or_kept );

				list_append_pointer(
					results_list,
					species );
			}
		} while( list_next( fishing_trip->catch_list ) );

	} while( list_next( fishing_trip_list ) );

	/* Accumulate for those that are preferred but not caught. */
	/* ------------------------------------------------------- */
	list_rewind( fishing_trip_list );
	do {
		fishing_trip = list_get_pointer( fishing_trip_list );

		if ( !creel_library_exists_selected_species(
				fishing_trip->family,
				fishing_trip->genus,
				fishing_trip->species_preferred,
				family_list_string,
				genus_list_string,
				species_list_string ) )
		{
			continue;
		}

		if ( ! ( catch = catch_per_unit_effort_catch_seek(
				fishing_trip->catch_list,
				fishing_trip->family,
				fishing_trip->genus,
				fishing_trip->species_preferred ) ) )
		{
			/* Per species per area */
			/* -------------------- */
			species = catch_per_unit_effort_get_per_species(
					results_per_species_per_area_list,
					fishing_trip->family,
					fishing_trip->genus,
					fishing_trip->species_preferred,
					fishing_trip->fishing_area,
					appaserver_data_directory );

			species->effort +=
				( fishing_trip->hours_fishing *
			  	  fishing_trip->party_count );

			fprintf( species->statistics_pipe,
				 "0.0\n" );

			/* Per species */
			/* ----------- */
			species = catch_per_unit_effort_get_per_species(
					results_per_species_list,
					fishing_trip->family,
					fishing_trip->genus,
					fishing_trip->species_preferred,
					(char *)0 /* fishing_area */,
					appaserver_data_directory );

			species->effort +=
				( fishing_trip->hours_fishing *
			  	  fishing_trip->party_count );

			fprintf( species->statistics_pipe,
				 "0.0\n" );

			/* Detail */
			/* ------ */
			if ( !summary_only )
			{
				species = catch_per_unit_effort_species_new(
						fishing_trip->family,
						fishing_trip->genus,
						fishing_trip->
							species_preferred,
						fishing_trip->
							fishing_area );

				catch_per_unit_effort_set_detail_species(
					species,
					fishing_trip->census_date,
					fishing_trip->fishing_purpose,
					fishing_trip->interview_location,
					fishing_trip->interview_number,
					fishing_trip->family,
					fishing_trip->genus,
					fishing_trip->species_preferred,
					fishing_trip->hours_fishing,
					fishing_trip->party_count,
					"",
					"",
					"",
					0 );

				list_append_pointer(
					results_list,
					species );
			}
		}
	} while( list_next( fishing_trip_list ) );

} /* creel_library_get_CPUE_results_list() */

void catch_per_unit_effort_set_detail_species(
				CATCH_PER_UNIT_EFFORT_SPECIES *species,
				char *census_date,
				char *fishing_purpose,
				char *interview_location,
				int interview_number,
				char *family_preferred,
				char *genus_preferred,
				char *species_preferred,
				int hours_fishing,
				int party_count,
				char *family_caught,
				char *genus_caught,
				char *species_caught,
				int caught_integer )
{
	species->census_date = census_date;
	species->fishing_purpose = fishing_purpose;
	species->interview_location = interview_location;
	species->interview_number = interview_number;
	species->family_preferred = family_preferred;
	species->genus_preferred = genus_preferred;
	species->species_preferred = species_preferred;
	species->hours_fishing = hours_fishing;
	species->party_count = party_count;
	species->family_caught = family_caught;
	species->genus_caught = genus_caught;
	species->species_caught = species_caught;
	species->catches = caught_integer;
	species->effort = ( hours_fishing * party_count );

} /* catch_per_unit_effort_set_detail_species() */

boolean creel_library_exists_selected_species(
				char *family,
				char *genus,
				char *species,
				char *family_list_string,
				char *genus_list_string,
				char *species_list_string )
{
	static LIST *family_list = {0};
	static LIST *genus_list = {0};
	static LIST *species_list = {0};

	if ( !family_list )
	{
		family_list = list_string2list( family_list_string, ',' );
		genus_list = list_string2list( genus_list_string, ',' );
		species_list = list_string2list( species_list_string, ',' );
	}

	if ( list_exists_string( family_list, family )
	&&   list_exists_string( genus_list, genus )
	&&   list_exists_string( species_list, species ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
} /* creel_library_exists_selected_species() */

char *creel_library_get_effort_species_statistics_filename(
				char *family,
				char *genus,
				char *species,
				char *fishing_area,
				char *appaserver_data_directory )
{
	char filename[ 1024 ];

	if ( !fishing_area ) fishing_area = "fishing_area";

	sprintf( filename,
		 CREEL_LIBRARY_STATISTICS_TEMPLATE,
		 appaserver_data_directory,
		 family,
		 genus,
		 species,
		 fishing_area,
		 getpid() );

	return strdup( filename );

} /* creel_library_get_effort_species_statistics_filename() */

void creel_library_fetch_species_summary_statistics_filename(
				double *cpue_mean,
				double *cpue_standard_deviation,
				double *cpue_standard_error_of_mean,
				int *cpue_sample_size,
				char *statistics_filename )
{
	FILE *input_file;
	char input_buffer[ 256 ];
	char label[ 128 ];
	char results[ 128 ];

	if ( ! ( input_file = fopen( statistics_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 statistics_filename );
		exit( 1 );
	}

	*cpue_mean = 0.0;
	*cpue_standard_deviation = 0.0;
	*cpue_standard_error_of_mean = 0.0;

	while( get_line( input_buffer, input_file ) )
	{
		piece( label, ':', input_buffer, 0 );
		if ( strcmp( label, STATISTICS_WEIGHTED_AVERAGE ) == 0 )
		{
			piece( results, ':', input_buffer, 1 );
			*cpue_mean = atof( results );
		}
		else
		if ( strcmp(	label,
				STATISTICS_WEIGHTED_STANDARD_DEVIATION ) == 0 )
		{
			piece( results, ':', input_buffer, 1 );
			*cpue_standard_deviation = atof( results );
		}
		else
		if ( strcmp(	label,
				STATISTICS_WEIGHTED_COUNT ) == 0 )
		{
			piece( results, ':', input_buffer, 1 );
			*cpue_sample_size = atoi( results );
		}
		else
		if ( strcmp(
			label,
			STATISTICS_WEIGHTED_STANDARD_ERROR_OF_MEAN ) == 0 )
		{
			piece( results, ':', input_buffer, 1 );
			*cpue_standard_error_of_mean = atof( results );
		}
	}
	fclose( input_file );
} /* creel_library_fetch_species_summary_statistics_filename() */

void creel_library_close_statistics_pipes(
				LIST *results_per_species_list )
{
	CATCH_PER_UNIT_EFFORT_SPECIES *species;

	if ( !list_rewind( results_per_species_list ) ) return;

	do {
		species = list_get_pointer(
				results_per_species_list );
		pclose( species->statistics_pipe );
		/* pclose( species->complete_statistics_pipe ); */
	} while( list_next( results_per_species_list ) );

} /* creel_library_close_statistics_pipes() */

void creel_library_remove_statistics_files(
				LIST *results_species_summary_list )
{
	CATCH_PER_UNIT_EFFORT_SPECIES *species_summary;
	char sys_string[ 1024 ];

	if ( !list_rewind( results_species_summary_list ) ) return;

	do {
		species_summary = list_get_pointer(
			results_species_summary_list );

		sprintf( sys_string,
			 "rm -f %s",
			 species_summary->statistics_filename );
		system( sys_string );

/*
		sprintf( sys_string,
			 "rm -f %s",
			 species_summary->complete_statistics_filename );
		system( sys_string );
*/

	} while( list_next( results_species_summary_list ) );

} /* creel_library_remove_statistics_files() */

void creel_library_separate_fishing_area_zone(
				char **fishing_area,
				char **fishing_zone,
				char *fishing_area_zone )
{
	int integer_part;
	static char local_fishing_area[ 16 ];
	static char local_fishing_zone[ 16 ];

	integer_part = atoi( fishing_area_zone );

	if ( !integer_part )
	{
		strcpy( local_fishing_area, fishing_area_zone );
		*fishing_area = local_fishing_area;
		*fishing_zone = "";
	}
	else
	{
		char last_character;

		sprintf( local_fishing_area, "%d", integer_part );
		*fishing_area = local_fishing_area;

		last_character =
			*(fishing_area_zone + strlen( fishing_area_zone ) - 1 );

		if ( isalpha( last_character ) )
		{
			*local_fishing_zone = toupper( last_character );
			*(local_fishing_zone + 1) = '\0';
			*fishing_zone = local_fishing_zone;
		}
		else
		{
			*fishing_zone = "";
		}
	}
} /* creel_library_separate_fishing_area_zone() */

LIST *creel_library_get_measurements_record_list(
				char *application_name,
				char *fishing_purpose,
				char *begin_date_string,
				char *end_date_string )
{
	char sys_string[ 2048 ];
	char catch_measurements_where_clause[ 1024 ];
	char common_name_join_where_clause[ 1024 ];
	char select[ 1024 ];
	char *catch_measurements_table_name;
	char *species_table_name;

	catch_measurements_table_name =
		get_table_name(	application_name,
				"catch_measurements" );

	species_table_name =
		get_table_name(	application_name,
				"species" );

	sprintf( select,
"fishing_purpose,census_date,interview_location,interview_number,%s.family,%s.genus,%s.species,%s.common_name,%s.florida_state_code,catch_measurement_number,length_millimeters",
		 species_table_name,
		 species_table_name,
		 species_table_name,
		 species_table_name,
		 species_table_name );

	if ( !end_date_string
	||   !*end_date_string
	||   strcmp( end_date_string, "end_date" ) == 0 )
	{	
		end_date_string = begin_date_string;
	}

	sprintf( catch_measurements_where_clause,
		 "and fishing_purpose = '%s'		"
		 "and census_date between '%s' and '%s'	",
		 fishing_purpose,
		 begin_date_string,
		 end_date_string );

	sprintf( common_name_join_where_clause,
		 "and %s.family = %s.family		"
		 "and %s.genus = %s.genus		"
		 "and %s.species = %s.species		",
		 catch_measurements_table_name,
		 species_table_name,
		 catch_measurements_table_name,
		 species_table_name,
		 catch_measurements_table_name,
		 species_table_name );

	sprintf( sys_string,
		 "echo	\"select %s				 "
		 "	  from %s,%s				 "
		 "	  where 1 = 1 %s %s			 "
		 "	  order by %s;\"			|"
		 "sql.e '^'					|"
		 "cat						 ",
		 select,
		 catch_measurements_table_name,
		 species_table_name,
		 catch_measurements_where_clause,
		 common_name_join_where_clause,
		 select );

	return pipe2list( sys_string );

} /* creel_library_get_measurements_record_list() */

char *creel_library_get_measurements_record_search_string(
				char *fishing_purpose,
				char *census_date,
				char *interview_location,
				char *interview_number,
				char *family,
				char *genus,
				char *species)
{
	static char search_string[ 512 ];

	sprintf(search_string,
		"%s^%s^%s^%s^%s^%s^%s^",
		fishing_purpose,
		census_date,
		interview_location,
		interview_number,
		family,
		genus,
		species);

	return search_string;
} /* creel_library_get_measurements_record_search_string() */

#ifdef NOT_DEFINED
int creel_library_get_complete_effort(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *fishing_purpose )
{
	char where_clause[ 4096 ];
	char sys_string[ 4096 ];
	char *select;
	char *fishing_trips_table_name;

	select = "sum( hours_fishing * number_of_people_fishing )";

	fishing_trips_table_name =
		get_table_name( application_name, "fishing_trips" );

	creel_library_get_fishing_trips_census_date_where_clause(
			where_clause,
			application_name,
			(char *)0 /* fishing_area_list_string */,
			begin_date_string,
			end_date_string,
			fishing_purpose );

	sprintf( sys_string,
		 "echo \"select %s from %s where 1 = 1 %s;\" | sql.e",
		 select,
		 fishing_trips_table_name,
		 where_clause );

	return atoi( pipe2string( sys_string ) );

} /* creel_library_get_complete_effort() */
#endif

int creel_library_get_complete_sample_size(
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *fishing_purpose )
{
	char where_clause[ 4096 ];
	char sys_string[ 4096 ];
	char *select;
	char *fishing_trips_table_name;

	select = "count(*)";

	fishing_trips_table_name =
		get_table_name( application_name, "fishing_trips" );

	creel_library_get_fishing_trips_census_date_where_clause(
			where_clause,
			application_name,
			(char *)0 /* fishing_area_list_string */,
			begin_date_string,
			end_date_string,
			fishing_purpose );

	sprintf( sys_string,
		 "echo \"select %s from %s where 1 = 1 %s;\" | sql.e",
		 select,
		 fishing_trips_table_name,
		 where_clause );

	return atoi( pipe2string( sys_string ) );

} /* creel_library_get_complete_sample_size() */

CATCHES *creel_library_get_or_set_catches(
				LIST *catches_list,
				char *family,
				char *genus,
				char *species )
{
	CATCHES *catches;

	if ( list_rewind( catches_list ) )
	{
		do {
			catches = list_get_pointer( catches_list );
	
			if ( strcmp( family, catches->family ) == 0
			&&   strcmp( genus, catches->genus ) == 0
			&&   strcmp( species, catches->species ) == 0 )
			{
				return catches;
			}
		} while( list_next( catches_list ) );
	}

	catches = creel_library_catches_new();
	strcpy( catches->family, family );
	strcpy( catches->genus, genus );
	strcpy( catches->species, species );
	list_append_pointer( catches_list, catches );
	return catches;
} /* creel_library_get_or_set_catches() */

FILE *creel_library_get_fishing_trips_input_pipe(
				char *application_name,
				char *fishing_purpose,
				char *begin_date_string,
				char *end_date_string,
				char *family,
				char *genus,
				char *species_preferred )
{
	char sys_string[ 65536 ];
	char select[ 1024 ];
	char where_clause[ 4096 ];
	char *where_preferred_caught;
	char order[ 128 ];
	char *fishing_trips_table;
	char *interview_locations_table;
	char where_fishing_purpose[ 128 ] = {0};

	fishing_trips_table =
		get_table_name( application_name,
				"fishing_trips" );

	interview_locations_table =
		get_table_name( application_name,
				"interview_locations" );

	sprintf( select,
		 CREEL_LIBRARY_FISHING_TRIPS_SELECT,
		 fishing_trips_table );

	if (	fishing_purpose
	&&	*fishing_purpose
	&&	strcmp( fishing_purpose, "fishing_purpose" ) != 0 )
	{
		sprintf(where_fishing_purpose,
	 		"fishing_purpose = '%s'",
			fishing_purpose );
	}
	else
	{
		strcpy( where_fishing_purpose, "1 = 1" );
	}

	if ( end_date_string
	&&   *end_date_string
	&&   strcmp( end_date_string, "end_date" ) != 0 )
	{
		sprintf(where_clause,
	 		"%s and					"
	 		"census_date between '%s' and '%s' and	"
			"%s.interview_location =		"
			"%s.interview_location			",
	 		where_fishing_purpose,
	 		begin_date_string,
			end_date_string,
			fishing_trips_table,
			interview_locations_table );
	}
	else
	{
		sprintf(where_clause,
	 		"%s and					"
	 		"census_date = '%s' and			"
			"%s.interview_location =		"
			"%s.interview_location			",
	 		where_fishing_purpose,
	 		begin_date_string,
			fishing_trips_table,
			interview_locations_table );
	}

	if ( family && *family )
	{
		where_preferred_caught =
			creel_library_get_where_preferred_caught(
				application_name,
				family,
				genus,
				species_preferred );
	}
	else
	{
		where_preferred_caught = "1 = 1";
	}

	sprintf( where_clause + strlen( where_clause ),
		 " and %s",
		 where_preferred_caught );

/*
Jason said:
In the Output Catches Spreadsheet File (Guide), the output should 'output' the data - such that the interview # is sequential for each date (like it is in Sport).

	sprintf( order,
"substr( census_date, 1, 4 ), %s.code,census_date,interview_number",
		 interview_locations_table );
*/
	strcpy( order, "census_date,interview_number" );

	sprintf( sys_string,
		 "get_folder_data				 "
		 "	application=%s		 		 "
		 "	folder=fishing_trips,interview_locations "
		 "	select=%s		 		 "
		 "	where=\"%s\"		 		 "
		 "	order=\"%s\"		 		 ",
		 application_name,
		 select,
		 where_clause,
		 order );


/*
fprintf( stderr, "%s/%s()/%d: %s\n",
__FILE__,
__FUNCTION__,
__LINE__,
sys_string );
*/

	return popen( sys_string, "r" );

} /* creel_library_get_fishing_trips_input_pipe() */

char *creel_library_get_where_preferred_caught(
				char *application_name,
				char *family,
				char *genus,
				char *species_preferred )
{
	char where_clause[ 4096 ];
	char *fishing_trips_table_name;
	char *catches_table_name;
	char *catches_species_where;
	char *fishing_trips_species_where;

	if ( !family && !*family ) return "1 = 1";

	catches_table_name =
		get_table_name( application_name, "catches" );

	fishing_trips_table_name =
		get_table_name( application_name, "fishing_trips" );

	catches_species_where =
		creel_library_get_species_where(
			family,
			genus,
			species_preferred,
			"species",
			catches_table_name );

	fishing_trips_species_where =
		creel_library_get_species_where(
			family,
			genus,
			species_preferred,
			"species_preferred",
			fishing_trips_table_name );

/*
	sprintf( where_clause,
		 "(  (	family = '%s' and				"
		 "	genus = '%s' and				"
		 "	species_preferred = '%s' ) 			"
		 "or exists						"
		 "   (	select *					"
		 "	from %s						"
		 "	where %s.fishing_purpose =			"
		 "		%s.fishing_purpose and			"
		 "	      %s.interview_location =			"
		 "		%s.interview_location and		"
		 "	      %s.census_date = %s.census_date and	"
		 "	      %s.interview_number =			"
		 "		%s.interview_number and			"
		 "	      %s.family = '%s' and			"
		 "	      %s.genus = '%s' and			"
		 "	      %s.species = '%s' ) )			",
		 family,
		 genus,
		 species_preferred,
		 catches_table_name,
		 catches_table_name,
		 fishing_trips_table_name,
		 catches_table_name,
		 fishing_trips_table_name,
		 catches_table_name,
		 fishing_trips_table_name,
		 catches_table_name,
		 fishing_trips_table_name,
		 catches_table_name,
		 family,
		 catches_table_name,
		 genus,
		 catches_table_name,
		 species_preferred );
*/

	sprintf( where_clause,
		 "((%s)							"
		 "or exists						"
		 "   (	select *					"
		 "	from %s						"
		 "	where %s.fishing_purpose =			"
		 "		%s.fishing_purpose and			"
		 "	      %s.interview_location =			"
		 "		%s.interview_location and		"
		 "	      %s.census_date = %s.census_date and	"
		 "	      %s.interview_number =			"
		 "		%s.interview_number and			"
		 "	      %s ) )					",
		 fishing_trips_species_where,
		 catches_table_name,
		 catches_table_name,
		 fishing_trips_table_name,
		 catches_table_name,
		 fishing_trips_table_name,
		 catches_table_name,
		 fishing_trips_table_name,
		 catches_table_name,
		 fishing_trips_table_name,
		 catches_species_where );

	return strdup( where_clause );

} /* creel_library_get_where_preferred_caught() */

char *creel_library_get_species_where(
			char *family,
			char *genus,
			char *species,
			char *species_attribute_name,
			char *catches_table_name )
{
	char where_clause[ 1024 ];
	char family_where[ 256 ];
	char species_where[ 256 ];
	char genus_where[ 256 ];

	if ( species && *species && strcmp( species, "species" ) != 0 )
	{
		sprintf(species_where,
			"%s.%s = '%s'",
			catches_table_name,
			species_attribute_name,
			species );
	}
	else
	{
		strcpy( species_where, "1 = 1" );
	}

	if ( genus && *genus && strcmp( genus, "genus" ) != 0 )
	{
		sprintf(genus_where,
			"%s.genus = '%s'",
			catches_table_name,
			genus );
	}
	else
	{
		strcpy( genus_where, "1 = 1" );
	}

	if ( family && *family && strcmp( family, "family" ) != 0 )
	{
		sprintf(family_where,
			"%s.family = '%s'",
			catches_table_name,
			family );
	}
	else
	{
		strcpy( family_where, "1 = 1" );
	}

	sprintf( where_clause,
		 "%s and %s and %s",
		 family_where,
		 genus_where,
		 species_where );

	return strdup( where_clause );

} /* creel_library_get_species_where() */

char *creel_library_get_fishing_area_where(
				char *fishing_area_list_string )
{
	char *fishing_area_where = "and 1 = 1";

	if (	fishing_area_list_string
	&&	*fishing_area_list_string
	&&	strcmp( fishing_area_list_string, "fishing_area" ) != 0 )
	{
		LIST *column_name_list;
		QUERY_OR_SEQUENCE *query_or_sequence;

		column_name_list = list_new();
		list_append_pointer( column_name_list, "fishing_area" );

		query_or_sequence = query_or_sequence_new( column_name_list );

		query_or_sequence_set_data_list_string(
			query_or_sequence->data_list_list,
			fishing_area_list_string );

		fishing_area_where =
			query_or_sequence_where_clause(
				query_or_sequence->attribute_name_list,
				query_or_sequence->data_list_list,
				1 /* with_and_prefix */ );
	}

	return fishing_area_where;

} /* creel_library_get_fishing_area_where() */

char *creel_library_get_interview_location_where(
				char *interview_location )
{
	char *interview_location_where = "1 = 1";

	if (	interview_location
	&&	*interview_location
	&&	strcmp( interview_location, "interview_location" ) != 0 )
	{
		char buffer[ 128 ];

		sprintf(	buffer,
				"fishing_trips.interview_location = '%s'",
				interview_location );

		interview_location_where = strdup( buffer );
	}

	return interview_location_where;

} /* creel_library_get_interview_location_where() */

static char *fishing_trips_table = "fishing_trips";

char *creel_library_get_fishing_trips_join_where(
				char *related_folder )
{
	char where[ 256 ];

	sprintf( where,
		 "%s.fishing_purpose = %s.fishing_purpose and		"
		 "%s.census_date = %s.census_date and			"
		 "%s.interview_location = %s.interview_location and	"
		 "%s.interview_number = %s.interview_number		",
		 fishing_trips_table,
		 related_folder,
		 fishing_trips_table,
		 related_folder,
		 fishing_trips_table,
		 related_folder,
		 fishing_trips_table,
		 related_folder );

	return strdup( where );

} /* creel_library_get_fishing_trips_join_where() */

