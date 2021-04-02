/* ---------------------------------------------------	*/
/* creel_catches2minutes_between_fish.c			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "html_table.h"
#include "hash_table.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "application.h"
#include "date.h"
#include "creel_library.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *census_date;
	double fishing_hours_times_party_count;
} FISHING_HOURS_TIMES_PARTY_COUNT;

/* Prototypes */
/* ---------- */
FISHING_HOURS_TIMES_PARTY_COUNT *get_or_set_fishing_hours_times_party_count(
		HASH_TABLE *fishing_hours_times_party_count_hash_table,
		char *census_date );

FISHING_HOURS_TIMES_PARTY_COUNT *fishing_hours_times_party_count_new(
			char *census_date );

HASH_TABLE *get_fishing_hours_times_party_count_hash_table(
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *fishing_area_string );

int main( int argc, char **argv )
{
	char *application_name;
	char *begin_date_string;
	char *end_date_string;
	char *fishing_area_string;
	char census_date[ 16 ];
	char caught_count_string[ 16 ];
	char count_replace_string[ 16 ];
	int piece_offset;
	char delimiter;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	HASH_TABLE *fishing_hours_times_party_count_hash_table;
	FISHING_HOURS_TIMES_PARTY_COUNT *fishing_hours_times_party_count;
	char input_buffer[ 512 ];

	if ( argc != 7 )
	{
		fprintf( stderr, 
	"Usage: %s application begin_date end_date fishing_area delimiter catches_piece_offset\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	begin_date_string = argv[ 2 ];
	end_date_string = argv[ 3 ];
	fishing_area_string = argv[ 4 ];
	delimiter = *argv[ 5 ];
	piece_offset = atoi( argv[ 6 ] );

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

	fishing_hours_times_party_count_hash_table =
		get_fishing_hours_times_party_count_hash_table(
			application_name,
			begin_date_string,
			end_date_string,
			fishing_area_string );

	while( get_line( input_buffer, stdin ) )
	{
		piece(	census_date,
			delimiter,
			input_buffer,
			0 );

		if ( !piece(	caught_count_string,
				delimiter,
				input_buffer,
				piece_offset ) )
		{
			printf( input_buffer );
		}

		if ( ! ( fishing_hours_times_party_count =
			    hash_table_get_pointer(
				fishing_hours_times_party_count_hash_table,
				census_date ) ) )
		{
			strcpy( count_replace_string, "null" );
		}
		else
		{
			sprintf( count_replace_string,
				 "%.1lf",
				 ( fishing_hours_times_party_count->
				 	fishing_hours_times_party_count /
				   atof( caught_count_string ) ) *
				 60.0 );
		}

		piece_replace(	input_buffer,
				delimiter,
				count_replace_string,
				piece_offset );

		printf( "%s\n", input_buffer );
	}

	return 0;
} /* main() */

FISHING_HOURS_TIMES_PARTY_COUNT *fishing_hours_times_party_count_new(
			char *census_date )
{
	FISHING_HOURS_TIMES_PARTY_COUNT *fishing_hours_times_party_count;

	if ( ! ( fishing_hours_times_party_count =
		(FISHING_HOURS_TIMES_PARTY_COUNT *)
			calloc( 1,
				sizeof( FISHING_HOURS_TIMES_PARTY_COUNT ) ) ) )
	{
		fprintf(stderr,
			"Memory allocation error in %s/%s()/%d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	fishing_hours_times_party_count->census_date = census_date;

	return fishing_hours_times_party_count;

} /* fishing_hours_times_party_count_new() */

HASH_TABLE *get_fishing_hours_times_party_count_hash_table(
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *fishing_area_string )
{
	HASH_TABLE *fishing_hours_times_party_count_hash_table;
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	char where_clause[ 128 ];
	char census_date[ 16 ];
	char fishing_hours[ 16 ];
	char party_count[ 16 ];
	FISHING_HOURS_TIMES_PARTY_COUNT *fishing_hours_times_party_count;
	FILE *input_pipe;
	char *folder = "fishing_trips";
	char *select = "census_date,number_of_people_fishing,hours_fishing";

	fishing_hours_times_party_count_hash_table =
		hash_table_new( hash_table_medium );

	sprintf( where_clause,
		 "fishing_area = %s and			"
		 "census_date between '%s' and '%s'	",
		 fishing_area_string,
		 begin_date_string,
		 end_date_string );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 folder,
		 where_clause );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	census_date,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	fishing_hours,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		if ( !*fishing_hours || !atoi( fishing_hours ) ) continue;

		piece(	party_count,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		if ( !*party_count || !atoi( party_count ) ) continue;

		fishing_hours_times_party_count =
			get_or_set_fishing_hours_times_party_count(
				fishing_hours_times_party_count_hash_table,
				census_date );

		fishing_hours_times_party_count->
			fishing_hours_times_party_count +=
				( atof( fishing_hours ) *
				  atof( party_count ) );
	}

	pclose( input_pipe );
	return fishing_hours_times_party_count_hash_table;
} /* get_fishing_hours_times_party_count_hash_table() */

FISHING_HOURS_TIMES_PARTY_COUNT *get_or_set_fishing_hours_times_party_count(
		HASH_TABLE *fishing_hours_times_party_count_hash_table,
		char *census_date )
{
	FISHING_HOURS_TIMES_PARTY_COUNT *fishing_hours_times_party_count;

	if ( ( fishing_hours_times_party_count =
			hash_table_get_pointer(
				fishing_hours_times_party_count_hash_table,
				census_date ) ) )
	{
		return fishing_hours_times_party_count;
	}

	fishing_hours_times_party_count =
		fishing_hours_times_party_count_new(
			strdup( census_date ) );

	hash_table_set_pointer(
			fishing_hours_times_party_count_hash_table,
			fishing_hours_times_party_count->census_date,
			fishing_hours_times_party_count );

	return fishing_hours_times_party_count;

} /* get_or_set_fishing_hours_times_party_count() */

