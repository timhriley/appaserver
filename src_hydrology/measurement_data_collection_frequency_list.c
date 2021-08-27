/* ----------------------------------------------------------- 	*/
/* src_hydrology/measurement_data_collection_frequency_list.c	*/
/* ----------------------------------------------------------- 	*/
/* 						  	     	*/
/* Freely available software: see Appaserver.org		*/
/* ----------------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hydrology_library.h"
#include "appaserver_library.h"
#include "environ.h"
#include "timlib.h"
#include "piece.h"
#include "aggregate_statistic.h"
#include "dictionary.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void populate_measurement_value(
		DICTIONARY *measurement_dictionary,
		char *application_name,
		char *station,
		char *datatype,
		char *begin_measurement_date_string,
		char *end_measurement_date_string,
		char *aggregate_string,
		boolean append_null );

int main( int argc, char **argv )
{
	char *application_name;
	char *station;
	char *datatype;
	char *aggregate_string;
	char *begin_measurement_date_string;
	char *end_measurement_date_string;
	char sys_string[ 1024 ];
	DICTIONARY *measurement_dictionary;
	char input_buffer[ 128 ];
	FILE *input_pipe;
	FILE *output_pipe;
	LIST *key_list;
	char *key;
	char *measurement_data;
	boolean append_null = 0;

	output_starting_argv_stderr( argc, argv );

	if ( argc < 7 )
	{
		fprintf(stderr,
"Usage: %s application station datatype aggregate_level begin_measurement_date end_measurement_date [append_null_yn]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	station = argv[ 2 ];
	datatype = argv[ 3 ];
	aggregate_string = argv[ 4 ];
	begin_measurement_date_string = argv[ 5 ];
	end_measurement_date_string = argv[ 6 ];

	if ( argc == 8 ) append_null = ( *argv[ 7 ] == 'y' );

	if ( timlib_strncmp( begin_measurement_date_string, "begin_date" ) == 0
	&&   timlib_strncmp( end_measurement_date_string, "end_date" ) == 0 )
	{
		hydrology_library_get_period_of_record_begin_end_dates(
					&begin_measurement_date_string,
					&end_measurement_date_string,
					application_name,
					station,
					datatype );
	}

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	sprintf(sys_string,
		"data_collection_frequency_list %s %s %s %s %s %s",
		application_name,
		station,
		datatype,
		aggregate_string,
		begin_measurement_date_string,
		end_measurement_date_string );

	output_pipe = popen( "sort", "w" );
	measurement_dictionary = dictionary_large();
	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		dictionary_set_pointer(
				measurement_dictionary,
				strdup( input_buffer ),
				"" );
	}

	pclose( input_pipe );

	if ( !dictionary_length( measurement_dictionary ) ) exit( 0 );

	populate_measurement_value(
		measurement_dictionary,
		application_name,
		station,
		datatype,
		begin_measurement_date_string,
		end_measurement_date_string,
		aggregate_string,
		append_null );

	key_list = dictionary_get_key_list( measurement_dictionary );

	list_rewind( key_list );
	do {
		key = list_get_pointer( key_list );
		measurement_data =
			dictionary_get_pointer(
				measurement_dictionary,
				key );

		fprintf(output_pipe,
			"%s^%s^%s^",
			station,
			datatype,
			key );

		if ( measurement_data && *measurement_data )
			fprintf( output_pipe, "%s", measurement_data );

		fprintf( output_pipe, "\n" );

	} while( list_next( key_list ) );
	pclose( output_pipe );

	exit( 0 );
} /* main() */

void populate_measurement_value(
		DICTIONARY *measurement_dictionary,
		char *application_name,
		char *station,
		char *datatype,
		char *begin_measurement_date_string,
		char *end_measurement_date_string,
		char *aggregate_string,
		boolean append_null )
{
	char sys_string[ 1024 ];
	char input_buffer[ 128 ];
	char *select = "measurement_date,measurement_time,measurement_value";
	char where_clause[ 512 ];
	FILE *input_pipe;
	char measurement_date_string[ 128 ];
	char measurement_time_string[ 128 ];
	char measurement_value_string[ 128 ];
	char aggregation_process[ 256 ];
	char key[ 256 ];
	char buffer1[ 16 ];
	char buffer2[ 16 ];

	sprintf( where_clause,
"station = '%s' and datatype = '%s' and measurement_date between '%s' and '%s'",
		 station,
		 datatype,
		 piece( buffer1, ':', begin_measurement_date_string, 0 ),
		 piece( buffer2, ':', end_measurement_date_string, 0 ) );

	if ( strcmp( aggregate_string, "real_time" ) == 0 )
	{
		strcpy( aggregation_process, "cat" );
	}
	else
	{
		enum aggregate_statistic aggregate_statistic =
				based_on_datatype_get_aggregate_statistic(
					application_name,
					datatype,
					aggregate_statistic_none );

		sprintf( aggregation_process, 
			 "real_time2aggregate_value.e %s 0 1 2 '^' %s n %s",
			 aggregate_statistic_get_string( aggregate_statistic ),
			 aggregate_string,
			 end_measurement_date_string );
	}

	sprintf(sys_string,
		"get_folder_data	application=%s			 "
		"			folder=measurement		 "
		"			select=%s			 "
		"			where=\"%s\"			|"
		"%s							 ",
		application_name,
		select,
		where_clause,
		aggregation_process );

	input_pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	measurement_date_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	measurement_time_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	measurement_value_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		sprintf(	key,
				"%s^%s",
				measurement_date_string,
				measurement_time_string );

		if ( !dictionary_exists_key( measurement_dictionary, key ) )
			continue;

		if ( *measurement_value_string )
		{
			dictionary_set_pointer(
				measurement_dictionary,
				key,
				strdup( measurement_value_string ) );
		}
		else
		if ( append_null )
		{
			dictionary_set_pointer(
				measurement_dictionary,
				key,
				"null" );
		}
	}
	pclose( input_pipe );

} /* populate_measurement_value() */

