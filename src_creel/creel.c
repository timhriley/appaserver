/* ---------------------------------------------------	*/
/* src_creel/creel.c					*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "creel.h"
#include "piece.h"
#include "timlib.h"
#include "appaserver_library.h"

CREEL *creel_new(			char *application_name,
					char *begin_census_date,
					char *end_census_date,
					char *fishing_purpose )
{
	CREEL *creel;

	if ( ! ( creel = (CREEL *) calloc( 1, sizeof( CREEL ) ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	creel->creel_census_hash_table =
		creel_get_census_hash_table(
			&creel->creel_census_list,
			application_name,
			begin_census_date,
			end_census_date,
			fishing_purpose );

	creel->fishing_trips_hash_table =
		creel_get_fishing_trips_hash_table(
			application_name,
			begin_census_date,
			end_census_date,
			fishing_purpose );

	creel->catches_hash_table =
		creel_get_catches_hash_table(
			application_name,
			begin_census_date,
			end_census_date,
			fishing_purpose );

	creel_append_fishing_trips(
		creel->creel_census_hash_table,
		creel->fishing_trips_hash_table );

	creel_append_catches(
		creel->fishing_trips_hash_table,
		creel->catches_hash_table );

	return creel;

} /* creel_new() */

HASH_TABLE *creel_get_census_hash_table(LIST **creel_census_list,
					char *application_name,
					char *begin_census_date,
					char *end_census_date,
					char *fishing_purpose )
{
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	char local_fishing_purpose[ 64 ];
	char census_date[ 64 ];
	char interview_location[ 64 ];
	char researcher[ 64 ];
	char *select;
	char fishing_purpose_where[ 256 ];
	char where[ 256 ];
	FILE *input_pipe;
	HASH_TABLE *creel_census_hash_table;
	char *key;
	CREEL_CENSUS *creel_census;

	select = "fishing_purpose,census_date,interview_location,researcher";

	if (	fishing_purpose
	&&      *fishing_purpose
	&&      strcmp( fishing_purpose, "fishing_purpose" ) != 0 )
	{
		sprintf(fishing_purpose_where,
		 	" and fishing_purpose = '%s'",
			fishing_purpose );
	}
	else
	{
		strcpy( fishing_purpose_where, " and 1 = 1" );
	}

	sprintf( where,
		 "census_date between '%s' and '%s' %s",
		 begin_census_date,
		 end_census_date,
		 fishing_purpose_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=creel_census	"
		 "			where=\"%s\"		"
		 "			order=select		"
		 "			quick=y			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	creel_census_hash_table = hash_table_new( hash_table_medium );
	*creel_census_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	census_date,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	interview_location,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		if (	fishing_purpose
		&&      *fishing_purpose
		&&      strcmp( fishing_purpose, "fishing_purpose" ) != 0 )
		{
			creel_census =
				creel_census_new(
					fishing_purpose,
					strdup( census_date ),
					strdup( interview_location ) );
		}
		else
		{
			piece(	local_fishing_purpose,
				FOLDER_DATA_DELIMITER,
				input_buffer,
				0 );

			creel_census =
				creel_census_new(
					strdup( local_fishing_purpose ),
					strdup( census_date ),
					strdup( interview_location ) );
		}

		piece(	researcher,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		creel_census->researcher = strdup( researcher );

		key = creel_census_get_key(
				creel_census->fishing_purpose,
				creel_census->census_date,
				creel_census->interview_location );

		hash_table_set_pointer(
			creel_census_hash_table,
			strdup( key ),
			creel_census );

		list_append_pointer( *creel_census_list, creel_census );
	}

	pclose( input_pipe );

	return creel_census_hash_table;

} /* creel_get_census_hash_table() */

HASH_TABLE *creel_get_fishing_trips_hash_table(
					char *application_name,
					char *begin_census_date,
					char *end_census_date,
					char *fishing_purpose )
{
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	char local_fishing_purpose[ 64 ];
	char census_date[ 64 ];
	char interview_location[ 64 ];
	char interview_number[ 64 ];
	char piece_buffer[ 64 ];
	char *select;
	char fishing_purpose_where[ 256 ];
	char where[ 256 ];
	FILE *input_pipe;
	HASH_TABLE *fishing_trips_hash_table;
	char *key;
	CREEL_FISHING_TRIPS *fishing_trips;

	select =
"fishing_purpose,census_date,interview_location,interview_number,interview_time,trip_origin_location,family,genus,species_preferred,fishing_area,fishing_party_composition,trip_hours,hours_fishing,permit_code,recreational_angler_reside,number_of_people_fishing";

	if (	fishing_purpose
	&&      *fishing_purpose
	&&      strcmp( fishing_purpose, "fishing_purpose" ) != 0 )
	{
		sprintf(fishing_purpose_where,
		 	" and fishing_purpose = '%s'",
			fishing_purpose );
	}
	else
	{
		strcpy( fishing_purpose_where, " and 1 = 1" );
	}

	sprintf( where,
		 "census_date between '%s' and '%s' %s",
		 begin_census_date,
		 end_census_date,
		 fishing_purpose_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=fishing_trips	"
		 "			where=\"%s\"		"
		 "			quick=y			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	fishing_trips_hash_table = hash_table_new( hash_table_large );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	census_date,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	interview_location,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	interview_number,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		fishing_trips =
			creel_fishing_trips_new(
				atoi( interview_number ) );

		if (	fishing_purpose
		&&      *fishing_purpose
		&&      strcmp( fishing_purpose, "fishing_purpose" ) != 0 )
		{
			key = creel_fishing_trips_get_key(
					fishing_purpose,
					census_date,
					interview_location,
					fishing_trips->interview_number );
		}
		else
		{
			piece(	local_fishing_purpose,
				FOLDER_DATA_DELIMITER,
				input_buffer,
				0 );

			key = creel_fishing_trips_get_key(
					local_fishing_purpose,
					census_date,
					interview_location,
					fishing_trips->interview_number );
		}

		hash_table_set_pointer(
			fishing_trips_hash_table,
			strdup( key ),
			fishing_trips );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			4 );
		fishing_trips->interview_time = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			5 );
		fishing_trips->trip_origin_location = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			6 );
		fishing_trips->family = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			7 );
		fishing_trips->genus = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			8 );
		fishing_trips->species_preferred = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			9 );
		fishing_trips->fishing_area = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			10 );
		fishing_trips->fishing_party_composition =
			strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			11 );
		fishing_trips->trip_hours = atoi( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			12 );
		fishing_trips->hours_fishing = atoi( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			13 );
		fishing_trips->permit_code = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			14 );
		fishing_trips->recreational_angler_reside =
			strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			15 );
		fishing_trips->number_of_people_fishing = atoi( piece_buffer );

	}

	pclose( input_pipe );

	return fishing_trips_hash_table;

} /* creel_get_fishing_trips_hash_table() */

HASH_TABLE *creel_get_catches_hash_table(
					char *application_name,
					char *begin_census_date,
					char *end_census_date,
					char *fishing_purpose )
{
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	char local_fishing_purpose[ 64 ];
	char census_date[ 64 ];
	char interview_location[ 64 ];
	char interview_number[ 64 ];
	char family[ 64 ];
	char genus[ 64 ];
	char species[ 64 ];
	char piece_buffer[ 64 ];
	char *select;
	char fishing_purpose_where[ 256 ];
	char where[ 256 ];
	FILE *input_pipe;
	HASH_TABLE *catches_hash_table;
	char *key;
	CREEL_CATCHES *catches;

	select =
"fishing_purpose,census_date,interview_location,interview_number,family,genus,species,kept_count,released_count";

	if (	fishing_purpose
	&&      *fishing_purpose
	&&      strcmp( fishing_purpose, "fishing_purpose" ) != 0 )
	{
		sprintf(fishing_purpose_where,
		 	" and fishing_purpose = '%s'",
			fishing_purpose );
	}
	else
	{
		strcpy( fishing_purpose_where, " and 1 = 1" );
	}

	sprintf( where,
		 "census_date between '%s' and '%s' %s",
		 begin_census_date,
		 end_census_date,
		 fishing_purpose_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=catches		"
		 "			where=\"%s\"		"
		 "			quick=y			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	catches_hash_table = hash_table_new( hash_table_large );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	census_date,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	interview_location,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	interview_number,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		piece(	family,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			4 );

		piece(	genus,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			5 );

		piece(	species,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			6 );

		catches = creel_catches_new(
				strdup( family ),
				strdup( genus ),
				strdup( species ) );

		if (	fishing_purpose
		&&      *fishing_purpose
		&&      strcmp( fishing_purpose, "fishing_purpose" ) != 0 )
		{
			key = creel_catches_get_key(
					fishing_purpose,
					census_date,
					interview_location,
					atoi( interview_number ),
					catches->family,
					catches->genus,
					catches->species );
		}
		else
		{
			piece(	local_fishing_purpose,
				FOLDER_DATA_DELIMITER,
				input_buffer,
				0 );

			key = creel_catches_get_key(
					local_fishing_purpose,
					census_date,
					interview_location,
					atoi( interview_number ),
					catches->family,
					catches->genus,
					catches->species );
		}

		hash_table_set_pointer(
			catches_hash_table,
			strdup( key ),
			catches );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			7 );
		catches->kept_count = atoi( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			8 );
		catches->released_count = atoi( piece_buffer );
	}

	pclose( input_pipe );

	return catches_hash_table;

} /* creel_get_catches_hash_table() */

CREEL_CENSUS *creel_census_new(		char *fishing_purpose,
					char *census_date,
					char *interview_location )
{
	CREEL_CENSUS *creel_census;

	if ( ! ( creel_census =
			(CREEL_CENSUS *) calloc(
				1,
				sizeof( CREEL_CENSUS ) ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	creel_census->fishing_purpose = fishing_purpose;
	creel_census->census_date = census_date;
	creel_census->interview_location = interview_location;
	creel_census->fishing_trips_list = list_new();

	return creel_census;

} /* creel_census_new() */

CREEL_FISHING_TRIPS *creel_fishing_trips_new(
					int interview_number )
{
	CREEL_FISHING_TRIPS *fishing_trips;

	if ( ! ( fishing_trips =
			(CREEL_FISHING_TRIPS *) calloc(
				1,
				sizeof( CREEL_FISHING_TRIPS ) ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	fishing_trips->interview_number = interview_number;
	fishing_trips->catches_list = list_new();

	return fishing_trips;

} /* creel_fishing_trips_new() */

CREEL_CATCHES *creel_catches_new(	char *family,
					char *genus,
					char *species )
{
	CREEL_CATCHES *catches;

	if ( ! ( catches =
			(CREEL_CATCHES *) calloc(
				1,
				sizeof( CREEL_CATCHES ) ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	catches->family = family;
	catches->genus = genus;
	catches->species = species;

	return catches;

} /* creel_catches_new() */

char *creel_census_get_key(		char *fishing_purpose,
					char *census_date,
					char *interview_location )
{
	static char key[ 256 ];

	sprintf( key,
		 "%s^%s^%s",
		 fishing_purpose,
		 census_date,
		 interview_location );

	return key;

} /* creel_census_get_key() */

char *creel_fishing_trips_get_key(	char *fishing_purpose,
					char *census_date,
					char *interview_location,
					int interview_number )
{
	static char key[ 256 ];

	sprintf( key,
		 "%s^%s^%s^%d",
		 fishing_purpose,
		 census_date,
		 interview_location,
		 interview_number );

	return key;

} /* creel_fishing_trips_get_key() */

char *creel_catches_get_key(		char *fishing_purpose,
					char *census_date,
					char *interview_location,
					int interview_number,
					char *family,
					char *genus,
					char *species )
{
	static char key[ 512 ];

	sprintf( key,
		 "%s^%s^%s^%d^%s^%s^%s",
		 fishing_purpose,
		 census_date,
		 interview_location,
		 interview_number,
		 family,
		 genus,
		 species );

	return key;

} /* creel_catches_get_key() */

CREEL_CENSUS *creel_census_fetch(	HASH_TABLE *creel_census_hash_table,
					char *fishing_purpose,
					char *census_date,
					char *interview_location )
{
	char *key;
	CREEL_CENSUS *creel_census;

	key = creel_census_get_key(
			fishing_purpose,
			census_date,
			interview_location );

	if ( ! ( creel_census =
			hash_table_fetch( creel_census_hash_table, key ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot fetch using key = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 key );
		exit( 1 );
	}

	return creel_census;

}

CREEL_FISHING_TRIPS *creel_fishing_trips_fetch(
					HASH_TABLE *fishing_trips_hash_table,
					char *fishing_purpose,
					char *census_date,
					char *interview_location,
					int interview_number )
{
	char *key;
	CREEL_FISHING_TRIPS *fishing_trips;

	key = creel_fishing_trips_get_key(
			fishing_purpose,
			census_date,
			interview_location,
			interview_number );

	if ( ! ( fishing_trips =
			hash_table_fetch( fishing_trips_hash_table, key ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot fetch using key = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 key );
		exit( 1 );
	}

	return fishing_trips;
}

CREEL_CATCHES *creel_catches_fetch(	HASH_TABLE *catches_hash_table,
					char *fishing_purpose,
					char *census_date,
					char *interview_location,
					int interview_number,
					char *family,
					char *genus,
					char *species )
{
	char *key;
	CREEL_CATCHES *catches;

	key = creel_catches_get_key(
			fishing_purpose,
			census_date,
			interview_location,
			interview_number,
			family,
			genus,
			species );

	if ( ! ( catches = hash_table_fetch( catches_hash_table, key ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot fetch using key = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 key );
		exit( 1 );
	}

	return catches;
}

void creel_append_fishing_trips(	HASH_TABLE *creel_census_hash_table,
					HASH_TABLE *fishing_trips_hash_table )
{
	LIST *key_list;
	CREEL_FISHING_TRIPS *fishing_trips;
	CREEL_CENSUS *creel_census;
	char *key;
	char fishing_purpose[ 64 ];
	char census_date[ 64 ];
	char interview_location[ 64 ];
	char interview_number[ 64 ];

	key_list = hash_table_get_key_list( fishing_trips_hash_table );

	if ( !list_rewind( key_list ) ) return;

	do {
		key = list_get_pointer( key_list );

		piece( fishing_purpose, '^', key, 0 );
		piece( census_date, '^', key, 1 );
		piece( interview_location, '^', key, 2 );
		piece( interview_number, '^', key, 3 );

		key = creel_census_get_key(
			fishing_purpose,
			census_date,
			interview_location );

		if ( ! ( creel_census =
			hash_table_fetch(
				creel_census_hash_table, key ) ) )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: cannot fetch = (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 key );
			exit( 1 );
		}

		key = creel_fishing_trips_get_key(
			fishing_purpose,
			census_date,
			interview_location,
			atoi( interview_number ) );

		if ( ! ( fishing_trips =
			hash_table_fetch(
				fishing_trips_hash_table, key ) ) )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: cannot fetch = (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 key );
			exit( 1 );
		}

		list_append_pointer(	creel_census->fishing_trips_list,
					fishing_trips );

	} while( list_next( key_list ) );
}

void creel_append_catches(		HASH_TABLE *fishing_trips_hash_table,
					HASH_TABLE *catches_hash_table )
{
	LIST *key_list;
	CREEL_CATCHES *catches;
	CREEL_FISHING_TRIPS *fishing_trips;
	char *key;
	char fishing_purpose[ 64 ];
	char census_date[ 64 ];
	char interview_location[ 64 ];
	char interview_number[ 64 ];
	char family[ 64 ];
	char genus[ 64 ];
	char species[ 64 ];

	key_list = hash_table_get_key_list( catches_hash_table );

	if ( !list_rewind( key_list ) ) return;

	do {
		key = list_get_pointer( key_list );

		piece( fishing_purpose, '^', key, 0 );
		piece( census_date, '^', key, 1 );
		piece( interview_location, '^', key, 2 );
		piece( interview_number, '^', key, 3 );
		piece( family, '^', key, 4 );
		piece( genus, '^', key, 5 );
		piece( species, '^', key, 6 );

		key = creel_fishing_trips_get_key(
			fishing_purpose,
			census_date,
			interview_location,
			atoi( interview_number ) );

		if ( ! ( fishing_trips =
			hash_table_fetch(
				fishing_trips_hash_table, key ) ) )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: cannot fetch = (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 key );
			exit( 1 );
		}

		key = creel_catches_get_key(
			fishing_purpose,
			census_date,
			interview_location,
			atoi( interview_number ),
			family,
			genus,
			species );

		if ( ! ( catches =
			hash_table_fetch(
				catches_hash_table, key ) ) )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: cannot fetch = (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 key );
			exit( 1 );
		}

		list_append_pointer(	fishing_trips->catches_list,
					catches );

	} while( list_next( key_list ) );
}

