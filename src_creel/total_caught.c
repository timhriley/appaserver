/* ---------------------------------------------------	*/
/* src_creel/total_caught.c				*/
/* ---------------------------------------------------	*/
/* This is the header file for the Species Estimated    */
/* Total Caught Algorithm.				*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "total_caught.h"
#include "list.h"
#include "piece.h"
#include "timlib.h"
#include "query.h"
#include "appaserver_library.h"
#include "application_constants.h"

CATCH_AREA *total_caught_catch_area_new( void )
{
	CATCH_AREA *catch_area;

	if ( ! ( catch_area = calloc( 1, sizeof( CATCH_AREA ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return catch_area;
} /* total_caught_catch_area_new() */

MONTH_ROW_VESSEL *total_caught_month_row_vessel_new( void )
{
	MONTH_ROW_VESSEL *month_row_vessel;

	if ( ! ( month_row_vessel = calloc( 1, sizeof( MONTH_ROW_VESSEL ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return month_row_vessel;

} /* total_caught_month_row_vessel_new() */

MONTH_ROW_FISHING_TRIP *total_caught_month_row_fishing_trip_new( void )
{
	MONTH_ROW_FISHING_TRIP *month_row_fishing_trip;

	if ( ! ( month_row_fishing_trip =
			calloc( 1, sizeof( MONTH_ROW_FISHING_TRIP ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return month_row_fishing_trip;

} /* total_caught_month_row_fishing_trip_new() */

CATCH *total_caught_catch_new( void )
{
	CATCH *catch;

	if ( ! ( catch = calloc( 1, sizeof( CATCH ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return catch;

} /* total_caught_catch_new() */

SPECIES *total_caught_species_new(
				char *family,
				char *genus,
				char *species_name )
{
	SPECIES *species;

	if ( ! ( species = calloc( 1, sizeof( SPECIES ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	species->family = family;
	species->genus = genus;
	species->species_name = species_name;
	return species;
}

FISHING_TRIP *total_caught_fishing_trip_new(
				char *fishing_purpose,
				char *census_date_string,
				char *interview_location,
				int interview_number,
				char *trip_origin_location )
{
	FISHING_TRIP *fishing_trip;

	if ( ! ( fishing_trip = calloc( 1, sizeof( FISHING_TRIP ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	fishing_trip->fishing_purpose = fishing_purpose;

	fishing_trip->census_date =
		date_new_yyyy_mm_dd_date(
			census_date_string );

	fishing_trip->interview_location = interview_location;
	fishing_trip->interview_number = interview_number;
	fishing_trip->trip_origin_location = trip_origin_location;
	fishing_trip->catch_list = list_new();

	return fishing_trip;
}

ESTIMATED_TOTAL_CAUGHT *estimated_total_caught_new(
				int begin_month,
				int end_month,
				int year,
				char *application_name )
{
	ESTIMATED_TOTAL_CAUGHT *caught;

	if ( ! ( caught = calloc( 1, sizeof( ESTIMATED_TOTAL_CAUGHT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	caught->input =
		total_caught_input_new(
			begin_month,
			end_month,
			year,
			application_name );

	return caught;
}

ESTIMATED_TOTAL_CAUGHT_INPUT *total_caught_input_new(
				int begin_month,
				int end_month,
				int year,
				char *application_name )
{
	ESTIMATED_TOTAL_CAUGHT_INPUT *input;

	if ( ! ( input = calloc( 1, sizeof( ESTIMATED_TOTAL_CAUGHT_INPUT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	input->begin_month = begin_month;
	input->end_month = end_month;
	input->year = year;

	if ( !total_caught_get_coefficients(
				&input->areas_1_5_coefficient,
				&input->areas_1_5_y_intercept,
				&input->area_6_coefficient,
				&input->area_6_y_intercept,
				application_name ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot get coefficients.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	input->trailer_count_dictionary =
		total_caught_get_trailer_count_dictionary(
				application_name,
				begin_month,
				end_month,
				year );

	input->weekend_creel_census_dictionary =
		total_caught_get_weekend_creel_census_dictionary(
				application_name,
				begin_month,
				end_month,
				year );

	return input;
}

DICTIONARY *total_caught_get_weekend_creel_census_dictionary(
				char *application_name,
				int begin_month,
				int end_month,
				int year )
{
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];
	char where_clause[ 1024 ];
	char sys_string[ 1024 ];
	char select[ 128 ];
	char interview_location_where[ 128 ];

	sprintf(
	select,
	"concat(census_date,'%c',interview_location),dayname(census_date)",
	DICTIONARY_KEY_DELIMITER );

	total_caught_get_begin_end_date_string(
			begin_date_string,
			end_date_string,
			begin_month,
			end_month,
			year );

	sprintf( interview_location_where,
		 "interview_location in ('%s','%s')",
		 INTERVIEW_LOCATION_FLAMINGO,
		 INTERVIEW_LOCATION_EVERGLADES_CITY );

	sprintf( where_clause,
		 "census_date between '%s' and '%s'"
		 " and %s"
	  	 " and ( dayname( census_date ) = 'Saturday'"
	   	 " or   dayname( census_date ) = 'Sunday' )",
		 begin_date_string,
		 end_date_string,
		 interview_location_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=creel_census	"
		 "			where=\"%s\"		"
		 "			order=census_date	",
		 application_name,
		 select,
		 where_clause );

	return pipe2dictionary( sys_string, FOLDER_DATA_DELIMITER );
}

DICTIONARY *total_caught_get_trailer_count_dictionary(
				char *application_name,
				int begin_month,
				int end_month,
				int year )
{
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];
	char where_clause[ 128 ];
	char sys_string[ 1024 ];
	char select[ 128 ];

	sprintf( select,
		 "concat(date,'%c',parking_lot),trailer_count",
		 DICTIONARY_KEY_DELIMITER );

	total_caught_get_begin_end_date_string(
			begin_date_string,
			end_date_string,
			begin_month,
			end_month,
			year );

	sprintf( where_clause,
		 "date between '%s' and '%s'",
		 begin_date_string,
		 end_date_string );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=trailer_count	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where_clause );

	return pipe2dictionary( sys_string, FOLDER_DATA_DELIMITER );
}

LIST *total_caught_get_species_list(
			boolean *all_species,
			char *application_name,
			char *family_list_string,
			char *genus_list_string,
			char *species_list_string )
{
	QUERY_OR_SEQUENCE *query_or_sequence = {0};
	char *query_or_sequence_where_clause;
	LIST *column_name_list;
	char sys_string[ 1024 ];
	char *select;
	char input_buffer[ 512 ];
	char family[ 128 ];
	char genus[ 128 ];
	char species_name[ 128 ];
	char florida_state_code[ 128 ];
	SPECIES *species;
	FILE *input_pipe;
	LIST *species_list;

	if ( !*family_list_string
	||   strcmp( family_list_string, "family" ) == 0 )
	{
		query_or_sequence_where_clause = "1 = 1";
		*all_species = 1;
	}
	else
	{
		column_name_list = list_new();
		list_append_pointer( column_name_list, "family" );
		list_append_pointer( column_name_list, "genus" );
		list_append_pointer( column_name_list, "species" );
	
		query_or_sequence = query_or_sequence_new( column_name_list );
	
		query_or_sequence_set_data_list_string(
			query_or_sequence->data_list_list,
			family_list_string );
	
		query_or_sequence_set_data_list_string(
			query_or_sequence->data_list_list,
			genus_list_string );
	
		query_or_sequence_set_data_list_string(
			query_or_sequence->data_list_list,
			species_list_string );
	
		query_or_sequence_where_clause =
			query_or_sequence_get_where_clause(
					query_or_sequence->attribute_name_list,
				query_or_sequence->data_list_list,
				0 /* not with_and_prefix */ );
	}

	select = "family,genus,species,florida_state_code";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=species			"
		 "			where=\"%s\"			"
		 "			order=florida_state_code	",
		 application_name,
		 select,
		 query_or_sequence_where_clause );

	input_pipe = popen( sys_string, "r" );

	species_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	family,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	genus,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	species_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	florida_state_code,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		species = total_caught_species_new(
				strdup( family ),
				strdup( genus ),
				strdup( species_name ) );

		species->florida_state_code = atoi( florida_state_code );

		list_append_pointer( species_list, species );
	}
	pclose( input_pipe );

	return species_list;
}

HASH_TABLE *total_caught_get_fishing_trip_hash_table(
			FISHING_TRIP_LIST *fishing_trip_list_array
							[ MONTHS_PER_YEAR ]
							[ DAYS_PER_MONTH ],
			char *application_name,
			int begin_month,
			int end_month,
			int year )
{
	char sys_string[ 1024 ];
	char *select;
	char input_buffer[ 512 ];
	char fishing_purpose[ 128 ];
	char census_date_string[ 128 ];
	char interview_location[ 128 ];
	char interview_number_string[ 128 ];
	char number_of_people_fishing_string[ 128 ];
	char hours_fishing_string[ 128 ];
	char fishing_area_string[ 128 ];
	char trip_origin_location[ 128 ];
	FILE *input_pipe;
	FISHING_TRIP *fishing_trip;
	HASH_TABLE *fishing_trip_hash_table;
	char *fishing_trip_key;
	char where[ 256 ];
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];
	int census_date_month;
	int census_date_day;
	LIST *fishing_trip_list;

	select =
"fishing_purpose,census_date,interview_location,interview_number,number_of_people_fishing,hours_fishing,fishing_area,trip_origin_location";

	total_caught_get_begin_end_date_string(
			begin_date_string,
			end_date_string,
			begin_month,
			end_month,
			year );

	sprintf( where,
		 "census_date between '%s' and '%s' and			"
		 "dayname( census_date ) in ('Saturday','Sunday' ) 	",
		 begin_date_string,
		 end_date_string );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=fishing_trips	"
		 "			where=\"%s\"		"
		 "			order=census_date	",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	fishing_trip_hash_table = hash_table_new( hash_table_large );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	fishing_purpose,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	census_date_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	interview_location,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	interview_number_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		piece(	number_of_people_fishing_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			4 );

		piece(	hours_fishing_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			5 );

		piece(	fishing_area_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			6 );

		piece(	trip_origin_location,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			7 );

		fishing_trip =
			total_caught_fishing_trip_new(
				strdup( fishing_purpose ),
				census_date_string,
				strdup( interview_location ),
				atoi( interview_number_string ),
				strdup( trip_origin_location ) );

		fishing_trip->number_of_people_fishing =
			atoi( number_of_people_fishing_string );

		fishing_trip->hours_fishing = atoi( hours_fishing_string );

		*(fishing_area_string + 1) = '\0';
		fishing_trip->fishing_area_integer =
				atoi( fishing_area_string );

		fishing_trip_key =
			total_caught_get_fishing_trip_key(
				fishing_purpose,
				census_date_string,
				interview_location,
				interview_number_string );

		hash_table_set_pointer(
				fishing_trip_hash_table,
				strdup( fishing_trip_key ),
				fishing_trip );

		if ( !total_caught_get_census_date_month_day(
			&census_date_month,
			&census_date_day,
			census_date_string ) )
		{
			continue;
		}

		if ( !fishing_trip_list_array	[ census_date_month - 1 ]
						[ census_date_day - 1 ] )
		{
			fishing_trip_list_array	[ census_date_month - 1 ]
						[ census_date_day - 1 ] =
				total_caught_fishing_trip_list_new();
		}

		fishing_trip_list =
			fishing_trip_list_array	[ census_date_month - 1 ]
						[ census_date_day - 1 ]->
				fishing_trip_list;

		list_append_pointer( fishing_trip_list, fishing_trip );

	}
	pclose( input_pipe );

	return fishing_trip_hash_table;
}

void total_caught_populate_catch_list(
				HASH_TABLE *fishing_trip_hash_table,
				LIST *species_list,
				char *application_name,
				int begin_month,
				int end_month,
				int year )
{
	char sys_string[ 1024 ];
	char *select;
	char input_buffer[ 512 ];
	char fishing_purpose[ 128 ];
	char census_date_string[ 128 ];
	char interview_location[ 128 ];
	char interview_number_string[ 128 ];
	char family[ 128 ];
	char genus[ 128 ];
	char species_name[ 128 ];
	char kept_count_string[ 128 ];
	char released_count_string[ 128 ];
	FILE *input_pipe;
	FISHING_TRIP *fishing_trip;
	CATCH *catch;
	SPECIES *species;
	char *key;
	char where[ 256 ];
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];

	select =
"fishing_purpose,census_date,interview_location,interview_number,family,genus,species,kept_count,released_count";

	total_caught_get_begin_end_date_string(
			begin_date_string,
			end_date_string,
			begin_month,
			end_month,
			year );

	sprintf( where,
		 "census_date between '%s' and '%s' and 		"
		 "dayname( census_date ) in ('Saturday','Sunday' )	",
		 begin_date_string,
		 end_date_string );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=catches		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

/* fprintf( stderr, "%s\n", sys_string ); */

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
/*
	select =
"fishing_purpose,census_date,interview_location,interview_number,family,genus,species,kept_count,released_count";
*/

		piece(	family,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			4 );

		piece(	genus,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			5 );

		piece(	species_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			6 );

		if ( ! ( species = total_caught_seek_species(
					species_list,
					family,
					genus,
					species_name ) ) )
		{
			continue;
		}

		piece(	fishing_purpose,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	census_date_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	interview_location,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	interview_number_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		key = total_caught_get_fishing_trip_key(
				fishing_purpose,
				census_date_string,
				interview_location,
				interview_number_string );

		if ( ! ( fishing_trip = hash_table_fetch(
					fishing_trip_hash_table,
					key ) ) )
		{
			fprintf(stderr,
"Ignoring in %s/%s()/%d: orphaned fishing trip for primary key = (%s,%s,%s,%s).\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				fishing_purpose,
				census_date_string,
				interview_location,
				interview_number_string );
			continue;
		}

		catch = total_caught_get_or_set_fishing_trip_catch(
					fishing_trip->catch_list,
					species->family,
					species->genus,
					species->species_name,
					species );

		piece(	kept_count_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			7 );

		piece(	released_count_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			8 );

		catch->kept += atoi( kept_count_string );
		catch->released += atoi( released_count_string );
	}
	pclose( input_pipe );
}

CATCH *total_caught_get_or_set_fishing_trip_catch(
					LIST *catch_list,
					char *family,
					char *genus,
					char *species_name,
					SPECIES *species )
{
	CATCH *catch;

	if ( list_rewind( catch_list ) )
	{
		do {
			catch = list_get_pointer( catch_list );

			if ( strcmp(	catch->species->family,
					family ) == 0
			&&   strcmp(	catch->species->genus,
					genus ) == 0
			&&   strcmp(	catch->species->species_name,
					species_name ) == 0 )
			{
				return catch;
			}
		} while( list_next( catch_list ) );
	}

	catch = total_caught_catch_new();
	catch->species = species;
	list_append_pointer( catch_list, catch );
	return catch;
}

char *total_caught_get_trailer_key(
				char *trailer_count_date_string,
				char *parking_lot )
{
	static char key[ 256 ];

	sprintf( key,
		 "%s%c%s",
		 trailer_count_date_string,
		 DICTIONARY_KEY_DELIMITER,
		 parking_lot );

	return key;
}

char *total_caught_get_fishing_trip_key(
				char *fishing_purpose,
				char *census_date_string,
				char *interview_location,
				char *interview_number_string )
{
	static char key[ 256 ];

	sprintf( key,
		 "%s%c%s%c%s%c%s",
		 fishing_purpose,
		 DICTIONARY_KEY_DELIMITER,
		 census_date_string,
		 DICTIONARY_KEY_DELIMITER,
		 interview_location,
		 DICTIONARY_KEY_DELIMITER,
		 interview_number_string );

	return key;
}

char *total_caught_get_weekend_key(
				char *census_date_string,
				char *interview_location )
{
	static char key[ 256 ];

	sprintf( key,
		 "%s%c%s",
		 census_date_string,
		 DICTIONARY_KEY_DELIMITER,
		 interview_location );

	return key;
}

char *total_caught_get_species_key(
				char *family,
				char *genus,
				char *species_name )
{
	static char key[ 256 ];

	sprintf( key,
		 "%s%c%s%c%s",
		 family,
		 DICTIONARY_KEY_DELIMITER,
		 genus,
		 DICTIONARY_KEY_DELIMITER,
		 species_name );

	return key;
}

void total_caught_get_begin_end_date_string(
			char *begin_date_string,
			char *end_date_string,
			int begin_month,
			int end_month,
			int year )
{
	DATE *last_week;

	sprintf( begin_date_string,
		 "%d-%.2d-01",
		 year,
		 begin_month );

	last_week =
		date_new_yyyy_mm_dd_date(
			begin_date_string );

	date_decrement_days(
		last_week,
		WEEKENDS_TO_GO_BACK * 7 );

	date_get_yyyy_mm_dd( begin_date_string, last_week );

	date_free( last_week );

	sprintf( end_date_string,
		 "%d-%.2d-31",
		 year,
		 end_month );
}

MONTH_SHEET *total_caught_month_sheet_new( int month )
{
	MONTH_SHEET *month_sheet;

	if ( ! ( month_sheet = calloc( 1, sizeof( MONTH_SHEET ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	month_sheet->month = month;
	month_sheet->row_list = list_new();
	return month_sheet;
}

TOTAL_SHEET *total_caught_total_sheet_new( int year )
{
	TOTAL_SHEET *total_sheet;

	if ( ! ( total_sheet = calloc( 1, sizeof( TOTAL_SHEET ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	total_sheet->year = year;
	total_sheet->total_row_list = list_new();
	return total_sheet;
}

MONTH_ROW *total_caught_month_row_new( int day )
{
	MONTH_ROW *month_row;

	if ( ! ( month_row = calloc( 1, sizeof( MONTH_ROW ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	month_row->day = day;
	return month_row;

} /* total_caught_month_row_new() */

ESTIMATED_TOTAL_CAUGHT_OUTPUT *total_caught_output_new( void )
{
	ESTIMATED_TOTAL_CAUGHT_OUTPUT *output;

	if ( ! ( output = calloc( 1,
				  sizeof( ESTIMATED_TOTAL_CAUGHT_OUTPUT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return output;

} /* total_caught_output_new() */

ESTIMATED_TOTAL_CAUGHT_OUTPUT *total_caught_get_output(
			int begin_month,
			int end_month,
			int year,
			LIST *species_list,
			DICTIONARY *trailer_count_dictionary,
			FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
			DICTIONARY *weekend_creel_census_dictionary,
			double areas_1_5_coefficient,
			double areas_1_5_y_intercept,
			double area_6_coefficient,
			double area_6_y_intercept,
			boolean omit_catch_area_list )
{
	ESTIMATED_TOTAL_CAUGHT_OUTPUT *output;

	output = total_caught_output_new();

	output->month_sheet_list =
		total_caught_get_month_sheet_list(
			begin_month,
			end_month,
			year,
			trailer_count_dictionary,
			fishing_trip_list_array,
			species_list,
			weekend_creel_census_dictionary,
			areas_1_5_coefficient,
			areas_1_5_y_intercept,
			area_6_coefficient,
			area_6_y_intercept,
			omit_catch_area_list );

	output->total_sheet =
		total_caught_get_total_sheet(
			output->month_sheet_list,
			begin_month,
			year );

	if ( output->total_sheet &&
	     output->total_sheet->total_row &&
	     list_length( output->total_sheet->total_row->catch_area_list ) )
	{
		total_caught_populate_grand_totals(
			&output->grand_areas_1_5_estimated_kept,
			&output->grand_areas_1_5_estimated_released,
			&output->grand_areas_1_5_estimated_caught,
			&output->grand_areas_1_6_estimated_kept,
			&output->grand_areas_1_6_estimated_released,
			&output->grand_areas_1_6_estimated_caught,
			output->total_sheet->total_row->catch_area_list );
	}

	return output;

} /* total_caught_get_output() */

LIST *total_caught_get_month_sheet_list(
			int begin_month,
			int end_month,
			int year,
			DICTIONARY *trailer_count_dictionary,
			FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
			LIST *species_list,
			DICTIONARY *weekend_creel_census_dictionary,
			double areas_1_5_coefficient,
			double areas_1_5_y_intercept,
			double area_6_coefficient,
			double area_6_y_intercept,
			boolean omit_catch_area_list )
{
	MONTH_SHEET *month_sheet;
	LIST *month_sheet_list = {0};
	int month;

	for( month = begin_month; month <= end_month; month++ )
	{
		month_sheet =
			total_caught_get_month_sheet(
				month,
				year,
				trailer_count_dictionary,
				fishing_trip_list_array,
				species_list,
				weekend_creel_census_dictionary,
				areas_1_5_coefficient,
				areas_1_5_y_intercept,
				area_6_coefficient,
				area_6_y_intercept,
				omit_catch_area_list );

		if ( month_sheet )
		{
			if ( !month_sheet_list ) month_sheet_list = list_new();

			list_append_pointer( month_sheet_list, month_sheet );
		}
	}

	return month_sheet_list;

} /* total_caught_get_month_sheet_list() */

MONTH_SHEET *total_caught_get_month_sheet(
				int month,
				int year,
				DICTIONARY *trailer_count_dictionary,
				FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
				LIST *species_list,
				DICTIONARY *weekend_creel_census_dictionary,
				double areas_1_5_coefficient,
				double areas_1_5_y_intercept,
				double area_6_coefficient,
				double area_6_y_intercept,
				boolean omit_catch_area_list )
{
	MONTH_SHEET *month_sheet;
	MONTH_ROW *month_row;
	int days_in_month;
	int day;
	char row_date_string[ 16 ];

	if ( month < 1 || month > 12 ) return (MONTH_SHEET *)0;

	month_sheet = total_caught_month_sheet_new( month );

	days_in_month = date_days_in_month( month, year );

	for( day = 1; day <= days_in_month; day++ )
	{
		sprintf( row_date_string,
			 "%d-%.2d-%.2d",
			 year,
			 month,
			 day );

		month_row =
			total_caught_get_month_row(
				day,
				row_date_string,
				trailer_count_dictionary,
				fishing_trip_list_array,
				species_list,
				weekend_creel_census_dictionary,
				areas_1_5_coefficient,
				areas_1_5_y_intercept,
				area_6_coefficient,
				area_6_y_intercept,
				omit_catch_area_list );

		if ( month_row )
		{
			list_append_pointer(
				month_sheet->row_list,
				month_row );
		}
	}

	month_sheet->total_row =
		total_caught_get_month_sheet_total_row(
			month_sheet->row_list );

	return month_sheet;

} /* total_caught_get_month_sheet() */

MONTH_ROW *total_caught_get_month_row(
				int day,
				char *row_date_string,
				DICTIONARY *trailer_count_dictionary,
				FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
				LIST *species_list,
				DICTIONARY *weekend_creel_census_dictionary,
				double areas_1_5_coefficient,
				double areas_1_5_y_intercept,
				double area_6_coefficient,
				double area_6_y_intercept,
				boolean omit_catch_area_list )
{
	MONTH_ROW *month_row;

	month_row = total_caught_month_row_new( day );

	month_row->row_date =
		date_yyyy_mm_dd_new(
			row_date_string );

	/* Get enum performed_census_weekend */
	/* --------------------------------- */
	month_row->performed_flamingo_census_weekend =
		total_caught_get_performed_census_weekend(
			month_row->row_date,
			weekend_creel_census_dictionary,
			INTERVIEW_LOCATION_FLAMINGO );

	month_row->performed_everglades_city_census_weekend =
		total_caught_get_performed_census_weekend(
			month_row->row_date,
			weekend_creel_census_dictionary,
			INTERVIEW_LOCATION_EVERGLADES_CITY );

	/* Get the Saturday/Sunday interview dates */
	/* --------------------------------------- */
	if ( !total_caught_get_saturday_sunday_interview_dates(
		&month_row->flamingo_interview_date_saturday,
		&month_row->flamingo_interview_date_sunday,
		month_row->row_date,
		month_row->performed_flamingo_census_weekend,
		weekend_creel_census_dictionary,
		INTERVIEW_LOCATION_FLAMINGO ) )
	{
		return (MONTH_ROW *)0;
	}

	if ( !total_caught_get_saturday_sunday_interview_dates(
		&month_row->everglades_city_interview_date_saturday,
		&month_row->everglades_city_interview_date_sunday,
		month_row->row_date,
		month_row->performed_everglades_city_census_weekend,
		weekend_creel_census_dictionary,
		INTERVIEW_LOCATION_EVERGLADES_CITY ) )
	{
		return (MONTH_ROW *)0;
	}

	/* Get MONTH_ROW_FISHING_TRIP */
	/* -------------------------- */
	month_row->fishing_trip =
		total_caught_month_row_get_fishing_trip(
			month_row->flamingo_interview_date_saturday,
			month_row->flamingo_interview_date_sunday,
			month_row->everglades_city_interview_date_saturday,
			month_row->everglades_city_interview_date_sunday,
	  		fishing_trip_list_array,
			month_row->performed_flamingo_census_weekend,
			month_row->performed_everglades_city_census_weekend );

	/* Get MONTH_ROW_FISHING_VESSEL */
	/* ---------------------------- */
	month_row->vessel =
		total_caught_month_row_get_vessel(
			month_row->row_date,
			trailer_count_dictionary,
			areas_1_5_coefficient,
			areas_1_5_y_intercept,
			area_6_coefficient,
			area_6_y_intercept,
			month_row->fishing_trip->flamingo_fishing_day,
			month_row->fishing_trip->flamingo_boating_day,
			month_row->fishing_trip->everglades_city_fishing_day,
			month_row->fishing_trip->everglades_city_boating_day );

	/* Calculate Park Effort Hours Day */
	/* ------------------------------- */
	month_row->fishing_trip->flamingo_fishing_effort_hours_day =
		month_row->fishing_trip->flamingo_angler_count_day *
		month_row->fishing_trip->flamingo_fishing_hours_day;

	if ( month_row->fishing_trip->flamingo_fishing_day )
	{
		month_row->fishing_trip->
			average_effort_hours_per_flamingo_fishing_vessel =
			month_row->fishing_trip->
				flamingo_fishing_effort_hours_day /
			month_row->fishing_trip->flamingo_fishing_day;
	}

	month_row->fishing_trip->estimated_flamingo_effort_hours =
		month_row->fishing_trip->
			average_effort_hours_per_flamingo_fishing_vessel *
		month_row->vessel->estimated_flamingo_fishing_vessels;

	month_row->fishing_trip->everglades_city_fishing_effort_hours_day =
		month_row->fishing_trip->everglades_city_angler_count_day *
		month_row->fishing_trip->everglades_city_fishing_hours_day;

	if ( month_row->fishing_trip->everglades_city_fishing_day )
	{
		month_row->fishing_trip->
		       average_effort_hours_per_everglades_city_fishing_vessel =
			month_row->fishing_trip->
			everglades_city_fishing_effort_hours_day /
			month_row->fishing_trip->everglades_city_fishing_day;
	}

	month_row->fishing_trip->estimated_everglades_city_effort_hours =
		month_row->fishing_trip->
		       average_effort_hours_per_everglades_city_fishing_vessel *
		month_row->vessel->estimated_everglades_city_fishing_vessels;

	month_row->fishing_trip->park_effort_hours_day =
		month_row->fishing_trip->estimated_flamingo_effort_hours +
		month_row->fishing_trip->
			estimated_everglades_city_effort_hours;

	/* Get Catch Area List */
	/* ------------------- */
	if ( !omit_catch_area_list )
	{
		month_row->catch_area_list =
	   	total_caught_get_catch_area_list(
			fishing_trip_list_array,
			month_row->flamingo_interview_date_saturday,
			month_row->flamingo_interview_date_sunday,
			month_row->everglades_city_interview_date_saturday,
			month_row->everglades_city_interview_date_sunday,
			species_list,
			month_row->fishing_trip->flamingo_fishing_day,
			month_row->fishing_trip->everglades_city_fishing_day,
			month_row->vessel->estimated_flamingo_fishing_vessels,
			month_row->vessel->
				estimated_everglades_city_fishing_vessels,
			month_row->performed_flamingo_census_weekend,
			month_row->performed_everglades_city_census_weekend );
	}

	return month_row;

} /* total_caught_get_month_row() */

int total_caught_get_fishing_interviews_count(
			DATE *search_date,
			FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
			boolean is_fishing_trips,
			boolean is_flamingo )
{
	FISHING_TRIP *fishing_trip;
	int fishing_interviews_count = 0;
	LIST *fishing_trip_list;

	if ( !search_date ) return 0;

	fishing_trip_list =
		total_caught_with_date_get_fishing_trip_list(
			fishing_trip_list_array,
			date_display( search_date ) );

	if ( !list_rewind( fishing_trip_list ) ) return 0;

	do {
		fishing_trip = list_get_pointer( fishing_trip_list );

		if ( is_fishing_trips && !fishing_trip->hours_fishing )
		{
			continue;
		}
		else
		if ( !is_fishing_trips && fishing_trip->hours_fishing )
		{
			continue;
		}
		else
		if ( is_flamingo
		&&   ( fishing_trip->fishing_area_integer < 1
		||     fishing_trip->fishing_area_integer > 5 ) )
		{
			continue;
		}
		else
		if ( !is_flamingo
		&&   fishing_trip->fishing_area_integer != 6 )
		{
			continue;
		}

		fishing_interviews_count++;

	} while( list_next( fishing_trip_list ) );

	return fishing_interviews_count;

} /* total_caught_get_fishing_interviews_count() */

char *total_caught_get_trailer_count_weekend_string(
			DATE *row_date,
			boolean is_saturday )
{
	static char trailer_count_weekend_string[ 16 ];
	int day_of_week;

	day_of_week = date_get_day_of_week( row_date );

	if ( is_saturday /* request Saturday's trailer count */ )
	{
		if ( day_of_week == WDAY_SATURDAY )
		{
			date_get_yyyy_mm_dd(
				trailer_count_weekend_string,
				row_date );
		}
		else
		if ( day_of_week == WDAY_SUNDAY )
		{
			DATE *tomorrow;

			tomorrow = date_new( 0, 0, 0 );

			date_copy( tomorrow, row_date );

			date_increment_days(
				tomorrow,
				1.0 );

			date_get_yyyy_mm_dd(
				trailer_count_weekend_string,
				tomorrow );

			date_free( tomorrow );
		}
		else
		{
			strcpy( trailer_count_weekend_string,
				date_prior_day_of_week_yyyy_mm_dd_string(
					row_date,
					WDAY_SATURDAY,
					date_get_utc_offset() ) );
		}
	}
	else
	/* ------------------------------ */
	/* request Sunday's trailer count */
	/* ------------------------------ */
	{
		if ( day_of_week == WDAY_SUNDAY )
		{
			date_get_yyyy_mm_dd(
				trailer_count_weekend_string,
				row_date );
		}
		else
		if ( day_of_week == WDAY_SATURDAY )
		{
			DATE *yesterday;

			yesterday = date_new( 0, 0, 0 );

			date_copy( yesterday, row_date );

			date_decrement_days(
				yesterday,
				1.0 );

			date_get_yyyy_mm_dd(
				trailer_count_weekend_string,
				yesterday );
			date_free( yesterday );
		}
		else
		{
			strcpy( trailer_count_weekend_string,
				date_prior_day_of_week_yyyy_mm_dd_string(
					row_date,
					WDAY_SUNDAY,
					date_get_utc_offset() ) );
		}
	}

	return trailer_count_weekend_string;

} /* total_caught_get_trailer_count_weekend_string() */

LIST *total_caught_get_catch_area_list(
	FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
	DATE *flamingo_interview_date_saturday,
	DATE *flamingo_interview_date_sunday,
	DATE *everglades_city_interview_date_saturday,
	DATE *everglades_city_interview_date_sunday,
	LIST *species_list,
	double flamingo_fishing_day,
	double everglades_city_fishing_day,
	double estimated_flamingo_fishing_vessels,
	double estimated_everglades_city_fishing_vessels,
	enum performed_census_weekend performed_flamingo_census_weekend,
	enum performed_census_weekend performed_everglades_city_census_weekend )
{
	CATCH_AREA *catch_area;
	SPECIES *species;
	LIST *catch_area_list;
	LIST *flamingo_fishing_trip_list_saturday = {0};
	LIST *flamingo_fishing_trip_list_sunday = {0};
	LIST *everglades_city_fishing_trip_list_saturday = {0};
	LIST *everglades_city_fishing_trip_list_sunday = {0};

	if ( !list_rewind( species_list ) ) return (LIST *)0;

	catch_area_list = list_new();

	if ( flamingo_interview_date_saturday )
	{
		flamingo_fishing_trip_list_saturday =
			total_caught_with_date_get_fishing_trip_list(
				fishing_trip_list_array,
				date_display(
					flamingo_interview_date_saturday ) );
	}

	if ( flamingo_interview_date_sunday )
	{
		flamingo_fishing_trip_list_sunday =
			total_caught_with_date_get_fishing_trip_list(
				fishing_trip_list_array,
				date_display(
					flamingo_interview_date_sunday ) );
	}

	if ( everglades_city_interview_date_saturday )
	{
		everglades_city_fishing_trip_list_saturday =
			total_caught_with_date_get_fishing_trip_list(
				fishing_trip_list_array,
				date_display(
				everglades_city_interview_date_saturday ) );
	}

	if ( everglades_city_interview_date_sunday )
	{
		everglades_city_fishing_trip_list_sunday =
			total_caught_with_date_get_fishing_trip_list(
				fishing_trip_list_array,
				date_display(
				everglades_city_interview_date_sunday ) );
	}

	do {
		species = list_get_pointer( species_list );

		catch_area = total_caught_catch_area_new();
		catch_area->species = species;
		list_append_pointer( catch_area_list, catch_area );

		/* Areas 1-5 variables */
		/* ------------------- */
		if ( list_length( flamingo_fishing_trip_list_saturday ) )
		{
			catch_area->areas_1_5_sample_kept_saturday =
				total_caught_get_catches_weekend(
					flamingo_fishing_trip_list_saturday,
					species->family,
					species->genus,
					species->species_name,
					1 /* is_areas_1_5 */,
					1 /* is_kept */ );

			catch_area->areas_1_5_sample_released_saturday =
				total_caught_get_catches_weekend(
					flamingo_fishing_trip_list_saturday,
					species->family,
					species->genus,
					species->species_name,
					1 /* is_areas_1_5 */,
					0 /* not is_kept */ );
		}

		if ( list_length( flamingo_fishing_trip_list_sunday ) )
		{
			catch_area->areas_1_5_sample_kept_sunday =
				total_caught_get_catches_weekend(
					flamingo_fishing_trip_list_sunday,
					species->family,
					species->genus,
					species->species_name,
					1 /* is_areas_1_5 */,
					1 /* is_kept */ );

			catch_area->areas_1_5_sample_released_sunday =
				total_caught_get_catches_weekend(
					flamingo_fishing_trip_list_sunday,
					species->family,
					species->genus,
					species->species_name,
					1 /* is_areas_1_5 */,
					0 /* not is_kept */ );
		}

		/* Area 6 variables */
		/* ---------------- */
		if ( list_length( everglades_city_fishing_trip_list_saturday ) )
		{
			catch_area->area_6_sample_kept_saturday =
				total_caught_get_catches_weekend(
				everglades_city_fishing_trip_list_saturday,
				species->family,
				species->genus,
				species->species_name,
				0 /* not is_areas_1_5 */,
				1 /* is_kept */ );

			catch_area->area_6_sample_released_saturday =
				total_caught_get_catches_weekend(
				everglades_city_fishing_trip_list_saturday,
				species->family,
				species->genus,
				species->species_name,
				0 /* not is_areas_1_5 */,
				0 /* not is_kept */ );
		}

		if ( list_length( everglades_city_fishing_trip_list_sunday ) )
		{
			catch_area->area_6_sample_kept_sunday =
				total_caught_get_catches_weekend(
				everglades_city_fishing_trip_list_sunday,
				species->family,
				species->genus,
				species->species_name,
				0 /* not is_areas_1_5 */,
				1 /* is_kept */ );

			catch_area->area_6_sample_released_sunday =
				total_caught_get_catches_weekend(
				everglades_city_fishing_trip_list_sunday,
				species->family,
				species->genus,
				species->species_name,
				0 /* not is_areas_1_5 */,
				0 /* not is_kept */ );
		}

		/* Perform arithmetic areas 1-5 kept (M) and released (N) */
		/* ------------------------------------------------------ */
		if (	performed_flamingo_census_weekend ==
			day_saturday_census_saturday )
		{
			catch_area->areas_1_5_sample_kept =
			(double)catch_area->areas_1_5_sample_kept_saturday;

			catch_area->areas_1_5_sample_released =
			(double)catch_area->areas_1_5_sample_released_saturday;
		}
		else
		if (	performed_flamingo_census_weekend ==
			day_sunday_census_sunday )
		{
			catch_area->areas_1_5_sample_kept =
			(double)catch_area->areas_1_5_sample_kept_sunday;

			catch_area->areas_1_5_sample_released =
			(double)catch_area->areas_1_5_sample_released_sunday;
		}
		else
		if (	performed_flamingo_census_weekend ==
			day_saturday_census_missed )
		{
			catch_area->areas_1_5_sample_kept_weekend =
			catch_area->areas_1_5_sample_kept_saturday +
			catch_area->areas_1_5_sample_kept_sunday;

			catch_area->areas_1_5_sample_kept =
			(double)catch_area->areas_1_5_sample_kept_weekend /
			2.0;

			catch_area->areas_1_5_sample_released_weekend =
			catch_area->areas_1_5_sample_released_saturday +
			catch_area->areas_1_5_sample_released_sunday;

			catch_area->areas_1_5_sample_released =
			(double)catch_area->areas_1_5_sample_released_weekend /
			2.0;
		}
		else
		if (	performed_flamingo_census_weekend ==
			day_sunday_census_missed_saturday_not_missed )
		{
			catch_area->areas_1_5_sample_kept =
			(double)catch_area->areas_1_5_sample_kept_saturday;

			catch_area->areas_1_5_sample_released =
			(double)catch_area->areas_1_5_sample_released_saturday;
		}
		else
		if (	performed_flamingo_census_weekend ==
			day_sunday_census_weekend_missed )
		{
			catch_area->areas_1_5_sample_kept_weekend =
			catch_area->areas_1_5_sample_kept_saturday +
			catch_area->areas_1_5_sample_kept_sunday;

			catch_area->areas_1_5_sample_kept =
			(double)catch_area->areas_1_5_sample_kept_weekend /
			2.0;

			catch_area->areas_1_5_sample_released_weekend =
			catch_area->areas_1_5_sample_released_saturday +
			catch_area->areas_1_5_sample_released_sunday;

			catch_area->areas_1_5_sample_released =
			(double)catch_area->areas_1_5_sample_released_weekend /
			2.0;
		}
		else
		if (	performed_flamingo_census_weekend ==
			day_weekday_census_saturday_only )
		{
			catch_area->areas_1_5_sample_kept =
			(double)catch_area->areas_1_5_sample_kept_saturday;

			catch_area->areas_1_5_sample_released =
			(double)catch_area->areas_1_5_sample_released_saturday;
		}
		else
		if (	performed_flamingo_census_weekend ==
			day_weekday_census_sunday_only )
		{
			catch_area->areas_1_5_sample_kept =
			(double)catch_area->areas_1_5_sample_kept_sunday;

			catch_area->areas_1_5_sample_released =
			(double)catch_area->areas_1_5_sample_released_sunday;
		}
		else
		if (	performed_flamingo_census_weekend ==
			day_weekday_census_saturday_and_sunday )
		{
			catch_area->areas_1_5_sample_kept_weekend =
			catch_area->areas_1_5_sample_kept_saturday +
			catch_area->areas_1_5_sample_kept_sunday;

			catch_area->areas_1_5_sample_kept =
			(double)catch_area->areas_1_5_sample_kept_weekend /
			2.0;

			catch_area->areas_1_5_sample_released_weekend =
			catch_area->areas_1_5_sample_released_saturday +
			catch_area->areas_1_5_sample_released_sunday;

			catch_area->areas_1_5_sample_released =
			(double)catch_area->areas_1_5_sample_released_weekend /
			2.0;
		}
		else
		if (	performed_flamingo_census_weekend ==
			day_weekday_census_missed_saturday_and_sunday )
		{
			catch_area->areas_1_5_sample_kept_weekend =
			catch_area->areas_1_5_sample_kept_saturday +
			catch_area->areas_1_5_sample_kept_sunday;

			catch_area->areas_1_5_sample_kept =
			(double)catch_area->areas_1_5_sample_kept_weekend /
			2.0;

			catch_area->areas_1_5_sample_released_weekend =
			catch_area->areas_1_5_sample_released_saturday +
			catch_area->areas_1_5_sample_released_sunday;

			catch_area->areas_1_5_sample_released =
			(double)catch_area->areas_1_5_sample_released_weekend /
			2.0;
		}
		else
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: invalid performed_flamingo_census_weekend = %d.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 performed_flamingo_census_weekend );
			exit( 1 );
		}

		/* Perform arithmetic area 6 kept and released */
		/* ------------------------------------------- */
		if (	performed_everglades_city_census_weekend ==
			day_saturday_census_saturday )
		{
			catch_area->area_6_sample_kept =
			(double)catch_area->area_6_sample_kept_saturday;

			catch_area->area_6_sample_released =
			(double)catch_area->area_6_sample_released_saturday;
		}
		else
		if (	performed_everglades_city_census_weekend ==
			day_sunday_census_sunday )
		{
			catch_area->area_6_sample_kept =
			(double)catch_area->area_6_sample_kept_sunday;

			catch_area->area_6_sample_released =
			(double)catch_area->area_6_sample_released_sunday;
		}
		else
		if (	performed_everglades_city_census_weekend ==
			day_saturday_census_missed )
		{
			catch_area->area_6_sample_kept_weekend =
			catch_area->area_6_sample_kept_saturday +
			catch_area->area_6_sample_kept_sunday;

			catch_area->area_6_sample_kept =
			(double)catch_area->area_6_sample_kept_weekend /
			2.0;

			catch_area->area_6_sample_released_weekend =
			catch_area->area_6_sample_released_saturday +
			catch_area->area_6_sample_released_sunday;

			catch_area->area_6_sample_released =
			(double)catch_area->area_6_sample_released_weekend /
			2.0;
		}
		else
		if (	performed_everglades_city_census_weekend ==
			day_sunday_census_missed_saturday_not_missed )
		{
			catch_area->area_6_sample_kept =
			(double)catch_area->area_6_sample_kept_saturday;

			catch_area->area_6_sample_released =
			(double)catch_area->area_6_sample_released_saturday;
		}
		else
		if (	performed_everglades_city_census_weekend ==
			day_sunday_census_weekend_missed )
		{
			catch_area->area_6_sample_kept_weekend =
			catch_area->area_6_sample_kept_saturday +
			catch_area->area_6_sample_kept_sunday;

			catch_area->area_6_sample_kept =
			(double)catch_area->area_6_sample_kept_weekend /
			2.0;

			catch_area->area_6_sample_released_weekend =
			catch_area->area_6_sample_released_saturday +
			catch_area->area_6_sample_released_sunday;

			catch_area->area_6_sample_released =
			(double)catch_area->area_6_sample_released_weekend /
			2.0;
		}
		else
		if (	performed_everglades_city_census_weekend ==
			day_weekday_census_saturday_only )
		{
			catch_area->area_6_sample_kept =
			(double)catch_area->area_6_sample_kept_saturday;

			catch_area->area_6_sample_released =
			(double)catch_area->area_6_sample_released_saturday;
		}
		else
		if (	performed_everglades_city_census_weekend ==
			day_weekday_census_sunday_only )
		{
			catch_area->area_6_sample_kept =
			(double)catch_area->area_6_sample_kept_sunday;

			catch_area->area_6_sample_released =
			(double)catch_area->area_6_sample_released_sunday;
		}
		else
		if (	performed_everglades_city_census_weekend ==
			day_weekday_census_saturday_and_sunday )
		{
			catch_area->area_6_sample_kept_weekend =
			catch_area->area_6_sample_kept_saturday +
			catch_area->area_6_sample_kept_sunday;

			catch_area->area_6_sample_kept =
			(double)catch_area->area_6_sample_kept_weekend /
			2.0;

			catch_area->area_6_sample_released_weekend =
			catch_area->area_6_sample_released_saturday +
			catch_area->area_6_sample_released_sunday;

			catch_area->area_6_sample_released =
			(double)catch_area->area_6_sample_released_weekend /
			2.0;
		}
		else
		if (	performed_everglades_city_census_weekend ==
			day_weekday_census_missed_saturday_and_sunday )
		{
			catch_area->area_6_sample_kept_weekend =
			catch_area->area_6_sample_kept_saturday +
			catch_area->area_6_sample_kept_sunday;

			catch_area->area_6_sample_kept =
			(double)catch_area->area_6_sample_kept_weekend /
			2.0;

			catch_area->area_6_sample_released_weekend =
			catch_area->area_6_sample_released_saturday +
			catch_area->area_6_sample_released_sunday;

			catch_area->area_6_sample_released =
			(double)catch_area->area_6_sample_released_weekend /
			2.0;
		}
		else
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: invalid performed_everglades_city_census_weekend = %d.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 performed_everglades_city_census_weekend );
			exit( 1 );
		}

		/* Perform arithmetic areas 1-5 caught (O) */
		/* --------------------------------------- */
		catch_area->areas_1_5_sample_caught =
			catch_area->areas_1_5_sample_kept +
			catch_area->areas_1_5_sample_released;

		catch_area->area_6_sample_caught =
			catch_area->area_6_sample_kept +
			catch_area->area_6_sample_released;

		/* Perform arithmetic areas 1-6 kept (P) */
		/* ------------------------------------- */
		catch_area->areas_1_6_sample_kept =
			catch_area->areas_1_5_sample_kept +
			catch_area->area_6_sample_kept;

		/* Perform arithmetic areas 1-6 released (Q) */
		/* ----------------------------------------- */
		catch_area->areas_1_6_sample_released =
			catch_area->areas_1_5_sample_released +
			catch_area->area_6_sample_released;

		/* Perform arithmetic areas 1-6 caught (R) */
		/* --------------------------------------- */
		catch_area->areas_1_6_sample_caught =
			catch_area->areas_1_6_sample_kept +
			catch_area->areas_1_6_sample_released;

		/* Perform arithmetic on areas 1-5 estimated kept/released */
		/* ------------------------------------------------------- */
		if ( flamingo_fishing_day )
		{
			catch_area->
			average_kept_per_flamingo_fishing_vessel =
				catch_area->areas_1_5_sample_kept /
				flamingo_fishing_day;

			catch_area->
			average_released_per_flamingo_fishing_vessel =
				catch_area->areas_1_5_sample_released /
				flamingo_fishing_day;
		}

		/* Perform arithmetic areas 1-5 estimated kept (S) */
		/* ----------------------------------------------- */
		catch_area->areas_1_5_estimated_kept =
			catch_area->average_kept_per_flamingo_fishing_vessel *
			estimated_flamingo_fishing_vessels;

		/* Perform arithmetic areas 1-5 estimated released (T) */
		/* --------------------------------------------------- */
		catch_area->areas_1_5_estimated_released =
			catch_area->
				average_released_per_flamingo_fishing_vessel *
			estimated_flamingo_fishing_vessels;

		/* Perform arithmetic areas 1-5 estimated caught (U) */
		/* ------------------------------------------------- */
		catch_area->areas_1_5_estimated_caught =
			catch_area->areas_1_5_estimated_kept +
			catch_area->areas_1_5_estimated_released;

		/* Perform arithmetic on area 6 estimated kept/released */
		/* ---------------------------------------------------- */
		if ( everglades_city_fishing_day )
		{
			catch_area->
			average_kept_per_everglades_city_fishing_vessel =
				catch_area->area_6_sample_kept /
				everglades_city_fishing_day;

			catch_area->
			average_released_per_everglades_city_fishing_vessel =
				catch_area->area_6_sample_released /
				everglades_city_fishing_day;
		}

		catch_area->area_6_estimated_kept =
			catch_area->
			average_kept_per_everglades_city_fishing_vessel *
			estimated_everglades_city_fishing_vessels;

		catch_area->area_6_estimated_released =
			catch_area->
			average_released_per_everglades_city_fishing_vessel *
			estimated_everglades_city_fishing_vessels;

		catch_area->area_6_estimated_caught =
			catch_area->area_6_estimated_kept +
			catch_area->area_6_estimated_released;

		/* Perform arithmetic areas 1-6 estimated kept (V) */
		/* ----------------------------------------------- */
		catch_area->areas_1_6_estimated_kept =
			catch_area->areas_1_5_estimated_kept +
			catch_area->area_6_estimated_kept;

		/* Perform arithmetic areas 1-6 estimated released (W) */
		/* --------------------------------------------------- */
		catch_area->areas_1_6_estimated_released =
			catch_area->areas_1_5_estimated_released +
			catch_area->area_6_estimated_released;

		/* Perform arithmetic areas 1-6 estimated caught (X) */
		/* ------------------------------------------------- */
		catch_area->areas_1_6_estimated_caught =
			catch_area->areas_1_6_estimated_kept +
			catch_area->areas_1_6_estimated_released;

	} while( list_next( species_list ) );

	return catch_area_list;

} /* total_caught_get_catch_area_list() */

SPECIES *total_caught_seek_species(
					LIST *species_list,
					char *family,
					char *genus,
					char *species_name )
{
	SPECIES *species;

	if ( list_rewind( species_list ) )
	{
		do {
			species = list_get_pointer( species_list );

			if ( strcmp( species->family, family ) == 0
			&&   strcmp( species->genus, genus ) == 0
			&&   strcmp( species->species_name, species_name ) == 0)
			{
				return species;
			}
		} while( list_next( species_list ) );
	}

	return (SPECIES *)0;
} /* total_caught_seek_species() */

CATCH_AREA *total_caught_get_or_set_catch_area(
					LIST *catch_area_list,
					char *family,
					char *genus,
					char *species_name,
					SPECIES *species)
{
	CATCH_AREA *catch_area;

	if ( list_rewind( catch_area_list ) )
	{
		do {
			catch_area = list_get_pointer( catch_area_list );

			if ( strcmp(	catch_area->species->family,
					family ) == 0
			&&   strcmp(	catch_area->species->genus,
					genus ) == 0
			&&   strcmp(	catch_area->species->species_name,
					species_name ) == 0 )
			{
				return catch_area;
			}
		} while( list_next( catch_area_list ) );
	}

	catch_area = total_caught_catch_area_new();
	catch_area->species = species;
	list_append_pointer( catch_area_list, catch_area );
	return catch_area;
	
} /* total_caught_get_or_set_catch_area() */

MONTH_ROW *total_caught_get_month_sheet_total_row(
			LIST *month_sheet_row_list )
{
	MONTH_ROW *total_row;
	MONTH_ROW *month_row;
	CATCH_AREA *total_row_catch_area;
	CATCH_AREA *month_row_catch_area;

	if ( !list_rewind( month_sheet_row_list ) ) return (MONTH_ROW *)0;

	total_row = total_caught_month_row_new( 0 );
	total_row->vessel = total_caught_month_row_vessel_new();
	total_row->fishing_trip = total_caught_month_row_fishing_trip_new();
	total_row->catch_area_list = list_new();

	do {
		month_row = list_get_pointer( month_sheet_row_list );

		total_row->vessel->florida_bay_trailer_count +=
			month_row->vessel->florida_bay_trailer_count;

		total_row->vessel->whitewater_bay_trailer_count +=
			month_row->vessel->whitewater_bay_trailer_count;

		total_row->vessel->total_trailer_count +=
			month_row->vessel->total_trailer_count;

		total_row->vessel->estimated_areas_1_5_vessels +=
			month_row->vessel->estimated_areas_1_5_vessels;

		total_row->vessel->estimated_area_6_vessels +=
			month_row->vessel->estimated_area_6_vessels;

		total_row->vessel->estimated_areas_1_6_vessels +=
			month_row->vessel->estimated_areas_1_6_vessels;

		total_row->vessel->estimated_flamingo_fishing_vessels +=
		    month_row->vessel->estimated_flamingo_fishing_vessels;

		total_row->vessel->estimated_everglades_city_fishing_vessels +=
		    month_row->
			vessel->
			estimated_everglades_city_fishing_vessels;

		total_row->fishing_trip->flamingo_fishing_day +=
			month_row->fishing_trip->flamingo_fishing_day;

		total_row->fishing_trip->flamingo_boating_day +=
			month_row->fishing_trip->flamingo_boating_day;

		total_row->fishing_trip->park_fishing_day +=
			month_row->fishing_trip->park_fishing_day;

		total_row->fishing_trip->park_boating_day +=
			month_row->fishing_trip->park_boating_day;

		total_row->vessel->estimated_park_fishing_vessels +=
			month_row->vessel->estimated_park_fishing_vessels;

		total_row->fishing_trip->park_effort_hours_day +=
			month_row->fishing_trip->park_effort_hours_day;

		if ( list_rewind( month_row->catch_area_list ) )
		{
			do {
				month_row_catch_area =
					list_get_pointer(
						month_row->catch_area_list );

				total_row_catch_area =
					total_caught_get_or_set_catch_area(
					total_row->catch_area_list,
					month_row_catch_area->species->family,
					month_row_catch_area->species->genus,
					month_row_catch_area->species->
						species_name,
					month_row_catch_area->species );

				total_row_catch_area->
					areas_1_5_estimated_kept +=
					month_row_catch_area->
						areas_1_5_estimated_kept;

				total_row_catch_area->
					areas_1_5_estimated_released +=
					month_row_catch_area->
				     	areas_1_5_estimated_released;

				total_row_catch_area->
					areas_1_5_estimated_caught +=
					month_row_catch_area->
				     	areas_1_5_estimated_caught;

				total_row_catch_area->
					areas_1_6_estimated_kept +=
					month_row_catch_area->
						areas_1_6_estimated_kept;

				total_row_catch_area->
					areas_1_6_estimated_released +=
					month_row_catch_area->
				     	areas_1_6_estimated_released;

				total_row_catch_area->
					areas_1_6_estimated_caught +=
					month_row_catch_area->
				     	areas_1_6_estimated_caught;

			} while( list_next( month_row->catch_area_list ) );
		}

	} while( list_next( month_sheet_row_list ) );

	return total_row;

} /* total_caught_get_month_sheet_total_row() */

MONTH_ROW_VESSEL *total_caught_month_row_get_vessel(
			DATE *row_date,
			DICTIONARY *trailer_count_dictionary,
			double areas_1_5_coefficient,
			double areas_1_5_y_intercept,
			double area_6_coefficient,
			double area_6_y_intercept,
			double flamingo_fishing_day,
			double flamingo_boating_day,
			double everglades_city_fishing_day,
			double everglades_city_boating_day )
{
	MONTH_ROW_VESSEL *vessel;

	vessel = total_caught_month_row_vessel_new();

	/* Get total_trailer_count (D) */
	/* --------------------------- */
	vessel->total_trailer_count =
		total_caught_get_total_trailer_count(
			&vessel->florida_bay_trailer_count,
			&vessel->whitewater_bay_trailer_count,
			row_date,
			trailer_count_dictionary );

	/* Calculate (E) */
	/* ------------- */
	vessel->estimated_areas_1_5_vessels =
		areas_1_5_y_intercept +
		areas_1_5_coefficient *
		(double)vessel->florida_bay_trailer_count;

	/* Calculate (F) */
	/* ------------- */
	vessel->estimated_area_6_vessels =
		area_6_y_intercept +
		area_6_coefficient *
		(double)vessel->whitewater_bay_trailer_count;

	/* Calculate (G) */
	/* ------------- */
	vessel->estimated_areas_1_6_vessels =
		vessel->estimated_areas_1_5_vessels +
		vessel->estimated_area_6_vessels;

	if ( flamingo_boating_day )
	{
		vessel->flamingo_fishing_boating_ratio =
			flamingo_fishing_day /
			flamingo_boating_day;

		vessel->estimated_flamingo_fishing_vessels =
			vessel->estimated_areas_1_5_vessels *
			vessel->flamingo_fishing_boating_ratio;
	}

	if ( everglades_city_boating_day )
	{
		vessel->everglades_city_fishing_boating_ratio =
			everglades_city_fishing_day /
			everglades_city_boating_day;

		vessel->estimated_everglades_city_fishing_vessels =
			vessel->estimated_area_6_vessels *
			vessel->everglades_city_fishing_boating_ratio;
	}

	/* Calculate (L) */
	/* ------------- */
	vessel->estimated_park_fishing_vessels =
		vessel->estimated_flamingo_fishing_vessels +
		vessel->estimated_everglades_city_fishing_vessels;

	return vessel;

} /* total_caught_month_row_get_vessel() */

MONTH_ROW_FISHING_TRIP *total_caught_month_row_get_fishing_trip(
			DATE *flamingo_interview_date_saturday,
			DATE *flamingo_interview_date_sunday,
			DATE *everglades_city_interview_date_saturday,
			DATE *everglades_city_interview_date_sunday,
			FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
			enum performed_census_weekend
				performed_flamingo_census_weekend,
			enum performed_census_weekend
				performed_everglades_city_census_weekend )
{
	MONTH_ROW_FISHING_TRIP *fishing_trip;
 
	fishing_trip = total_caught_month_row_fishing_trip_new();

	if ( performed_flamingo_census_weekend == day_saturday_census_saturday )
	{
		fishing_trip->flamingo_fishing_saturday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_pleasure_saturday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_day =
			(double)fishing_trip->flamingo_fishing_saturday;

		fishing_trip->flamingo_boating_day =
			(double)fishing_trip->flamingo_fishing_saturday +
			(double)fishing_trip->flamingo_pleasure_saturday;

		fishing_trip->flamingo_fishing_hours_saturday =
			total_caught_get_fishing_hours_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_hours_day =
			(double)fishing_trip->flamingo_fishing_hours_saturday;

		fishing_trip->flamingo_angler_count_saturday =
			total_caught_get_angler_count_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_angler_count_day =
			(double)fishing_trip->flamingo_angler_count_saturday;
	}
	else
	if ( performed_flamingo_census_weekend == day_sunday_census_sunday )
	{
		fishing_trip->flamingo_fishing_sunday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_pleasure_sunday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_day =
			(double)fishing_trip->flamingo_fishing_sunday;

		fishing_trip->flamingo_boating_day =
			(double)fishing_trip->flamingo_fishing_sunday +
			(double)fishing_trip->flamingo_pleasure_sunday;

		fishing_trip->flamingo_fishing_hours_sunday =
			total_caught_get_fishing_hours_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_hours_day =
			(double)fishing_trip->flamingo_fishing_hours_sunday;

		fishing_trip->flamingo_angler_count_sunday =
			total_caught_get_angler_count_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_angler_count_day =
			(double)fishing_trip->flamingo_angler_count_sunday;
	}
	else
	if ( performed_flamingo_census_weekend == day_saturday_census_missed )
	{
		fishing_trip->flamingo_fishing_saturday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_sunday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_pleasure_saturday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_pleasure_sunday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_weekend =
			fishing_trip->flamingo_fishing_saturday +
			fishing_trip->flamingo_fishing_sunday;

		fishing_trip->flamingo_boating_weekend =
			fishing_trip->flamingo_fishing_saturday +
			fishing_trip->flamingo_pleasure_saturday +
			fishing_trip->flamingo_fishing_sunday +
			fishing_trip->flamingo_pleasure_sunday;

		fishing_trip->flamingo_fishing_day =
			(double)fishing_trip->flamingo_fishing_weekend / 2.0;

		fishing_trip->flamingo_boating_day =
			(double)fishing_trip->flamingo_boating_weekend / 2.0;

		fishing_trip->flamingo_fishing_hours_saturday =
			total_caught_get_fishing_hours_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_hours_sunday =
			total_caught_get_fishing_hours_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_hours_weekend =
			fishing_trip->flamingo_fishing_hours_saturday +
			fishing_trip->flamingo_fishing_hours_sunday;

		fishing_trip->flamingo_fishing_hours_day =
			(double)fishing_trip->flamingo_fishing_hours_weekend /
			2.0;

		fishing_trip->flamingo_angler_count_saturday =
			total_caught_get_angler_count_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_angler_count_sunday =
			total_caught_get_angler_count_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_angler_count_weekend =
			fishing_trip->flamingo_angler_count_saturday +
			fishing_trip->flamingo_angler_count_sunday;

		fishing_trip->flamingo_angler_count_day =
			(double)fishing_trip->flamingo_angler_count_weekend /
			2.0;
	}
	else
	if (	performed_flamingo_census_weekend ==
		day_sunday_census_missed_saturday_not_missed )
	{
		fishing_trip->flamingo_fishing_saturday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_pleasure_saturday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_day =
			(double)fishing_trip->flamingo_fishing_saturday;

		fishing_trip->flamingo_boating_day =
			(double)fishing_trip->flamingo_fishing_saturday +
			(double)fishing_trip->flamingo_pleasure_saturday;

		fishing_trip->flamingo_fishing_hours_saturday =
			total_caught_get_fishing_hours_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_hours_day =
			(double)fishing_trip->flamingo_fishing_hours_saturday;

		fishing_trip->flamingo_angler_count_saturday =
			total_caught_get_angler_count_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_angler_count_day =
			(double)fishing_trip->flamingo_angler_count_saturday;
	}
	else
	if (	performed_flamingo_census_weekend ==
		day_sunday_census_weekend_missed )
	{
		fishing_trip->flamingo_fishing_saturday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_sunday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_pleasure_saturday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_pleasure_sunday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_weekend =
			fishing_trip->flamingo_fishing_saturday +
			fishing_trip->flamingo_fishing_sunday;

		fishing_trip->flamingo_boating_weekend =
			fishing_trip->flamingo_fishing_saturday +
			fishing_trip->flamingo_pleasure_saturday +
			fishing_trip->flamingo_fishing_sunday +
			fishing_trip->flamingo_pleasure_sunday;

		fishing_trip->flamingo_fishing_day =
			(double)fishing_trip->flamingo_fishing_weekend / 2.0;

		fishing_trip->flamingo_boating_day =
			(double)fishing_trip->flamingo_boating_weekend / 2.0;

		fishing_trip->flamingo_fishing_hours_saturday =
			total_caught_get_fishing_hours_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_hours_sunday =
			total_caught_get_fishing_hours_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_hours_weekend =
			fishing_trip->flamingo_fishing_hours_saturday +
			fishing_trip->flamingo_fishing_hours_sunday;

		fishing_trip->flamingo_fishing_hours_day =
			(double)fishing_trip->flamingo_fishing_hours_weekend /
			2.0;

		fishing_trip->flamingo_angler_count_saturday =
			total_caught_get_angler_count_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_angler_count_sunday =
			total_caught_get_angler_count_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_angler_count_weekend =
			fishing_trip->flamingo_angler_count_saturday +
			fishing_trip->flamingo_angler_count_sunday;

		fishing_trip->flamingo_angler_count_day =
			(double)fishing_trip->flamingo_angler_count_weekend /
			2.0;
	}
	else
	if (	performed_flamingo_census_weekend ==
		day_weekday_census_saturday_only )
	{
		fishing_trip->flamingo_fishing_saturday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_pleasure_saturday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_day =
			(double)fishing_trip->flamingo_fishing_saturday;

		fishing_trip->flamingo_boating_day =
			(double)fishing_trip->flamingo_fishing_saturday +
			(double)fishing_trip->flamingo_pleasure_saturday;

		fishing_trip->flamingo_fishing_hours_saturday =
			total_caught_get_fishing_hours_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_hours_day =
			(double)fishing_trip->flamingo_fishing_hours_saturday;

		fishing_trip->flamingo_angler_count_saturday =
			total_caught_get_angler_count_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_angler_count_day =
			(double)fishing_trip->flamingo_angler_count_saturday;
	}
	else
	if (	performed_flamingo_census_weekend ==
		day_weekday_census_sunday_only )
	{
		fishing_trip->flamingo_fishing_sunday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_pleasure_sunday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_day =
			(double)fishing_trip->flamingo_fishing_sunday;

		fishing_trip->flamingo_boating_day =
			(double)fishing_trip->flamingo_fishing_sunday +
			(double)fishing_trip->flamingo_pleasure_sunday;

		fishing_trip->flamingo_fishing_hours_sunday =
			total_caught_get_fishing_hours_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_hours_day =
			(double)fishing_trip->flamingo_fishing_hours_sunday;

		fishing_trip->flamingo_angler_count_sunday =
			total_caught_get_angler_count_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_angler_count_day =
			(double)fishing_trip->flamingo_angler_count_sunday;
	}
	else
	if (	performed_flamingo_census_weekend ==
		day_weekday_census_saturday_and_sunday )
	{
		fishing_trip->flamingo_fishing_saturday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_sunday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_pleasure_saturday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_pleasure_sunday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_weekend =
			fishing_trip->flamingo_fishing_saturday +
			fishing_trip->flamingo_fishing_sunday;

		fishing_trip->flamingo_boating_weekend =
			fishing_trip->flamingo_fishing_saturday +
			fishing_trip->flamingo_pleasure_saturday +
			fishing_trip->flamingo_fishing_sunday +
			fishing_trip->flamingo_pleasure_sunday;

		fishing_trip->flamingo_fishing_day =
			(double)fishing_trip->flamingo_fishing_weekend / 2.0;

		fishing_trip->flamingo_boating_day =
			(double)fishing_trip->flamingo_boating_weekend / 2.0;

		fishing_trip->flamingo_fishing_hours_saturday =
			total_caught_get_fishing_hours_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_hours_sunday =
			total_caught_get_fishing_hours_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_hours_weekend =
			fishing_trip->flamingo_fishing_hours_saturday +
			fishing_trip->flamingo_fishing_hours_sunday;

		fishing_trip->flamingo_fishing_hours_day =
			(double)fishing_trip->flamingo_fishing_hours_weekend /
			2.0;

		fishing_trip->flamingo_angler_count_saturday =
			total_caught_get_angler_count_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_angler_count_sunday =
			total_caught_get_angler_count_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_angler_count_weekend =
			fishing_trip->flamingo_angler_count_saturday +
			fishing_trip->flamingo_angler_count_sunday;

		fishing_trip->flamingo_angler_count_day =
			(double)fishing_trip->flamingo_angler_count_weekend /
			2.0;
	}
	else
	if (	performed_flamingo_census_weekend ==
		day_weekday_census_missed_saturday_and_sunday )
	{
		fishing_trip->flamingo_fishing_saturday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_sunday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_pleasure_saturday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_pleasure_sunday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_weekend =
			fishing_trip->flamingo_fishing_saturday +
			fishing_trip->flamingo_fishing_sunday;

		fishing_trip->flamingo_boating_weekend =
			fishing_trip->flamingo_fishing_saturday +
			fishing_trip->flamingo_pleasure_saturday +
			fishing_trip->flamingo_fishing_sunday +
			fishing_trip->flamingo_pleasure_sunday;

		fishing_trip->flamingo_fishing_day =
			(double)fishing_trip->flamingo_fishing_weekend / 2.0;

		fishing_trip->flamingo_boating_day =
			(double)fishing_trip->flamingo_boating_weekend / 2.0;

		fishing_trip->flamingo_fishing_hours_saturday =
			total_caught_get_fishing_hours_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_hours_sunday =
			total_caught_get_fishing_hours_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_fishing_hours_weekend =
			fishing_trip->flamingo_fishing_hours_saturday +
			fishing_trip->flamingo_fishing_hours_sunday;

		fishing_trip->flamingo_fishing_hours_day =
			(double)fishing_trip->flamingo_fishing_hours_weekend /
			2.0;

		fishing_trip->flamingo_angler_count_saturday =
			total_caught_get_angler_count_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_angler_count_sunday =
			total_caught_get_angler_count_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_flamingo */ );

		fishing_trip->flamingo_angler_count_weekend =
			fishing_trip->flamingo_angler_count_saturday +
			fishing_trip->flamingo_angler_count_sunday;

		fishing_trip->flamingo_angler_count_day =
			(double)fishing_trip->flamingo_angler_count_weekend /
			2.0;
	}
	else
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: invalid performed_flamingo_census_weekend = %d.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 performed_flamingo_census_weekend );
		exit( 1 );
	}

	if (	performed_everglades_city_census_weekend ==
		day_saturday_census_saturday )
	{
		fishing_trip->everglades_city_fishing_saturday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_pleasure_saturday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_day =
			(double)fishing_trip->everglades_city_fishing_saturday;

		fishing_trip->everglades_city_boating_day =
			(double)fishing_trip->everglades_city_fishing_saturday +
			(double)fishing_trip->everglades_city_pleasure_saturday;

		fishing_trip->everglades_city_fishing_hours_saturday =
			total_caught_get_fishing_hours_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* is_flamingo */ );

		fishing_trip->everglades_city_fishing_hours_day =
			(double)fishing_trip->
				everglades_city_fishing_hours_saturday;

		fishing_trip->everglades_city_angler_count_saturday =
			total_caught_get_angler_count_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_angler_count_day =
			(double)fishing_trip->
				everglades_city_angler_count_saturday;
	}
	else
	if (	performed_everglades_city_census_weekend ==
		day_sunday_census_sunday )
	{
		fishing_trip->everglades_city_fishing_sunday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_pleasure_sunday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_day =
			(double)fishing_trip->everglades_city_fishing_sunday;

		fishing_trip->everglades_city_boating_day =
			(double)fishing_trip->everglades_city_fishing_sunday +
			(double)fishing_trip->everglades_city_pleasure_sunday;

		fishing_trip->everglades_city_fishing_hours_sunday =
			total_caught_get_fishing_hours_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				0 /* is_flamingo */ );

		fishing_trip->everglades_city_fishing_hours_day =
			(double)fishing_trip->
				everglades_city_fishing_hours_sunday;

		fishing_trip->everglades_city_angler_count_sunday =
			total_caught_get_angler_count_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_angler_count_day =
			(double)fishing_trip->
				everglades_city_angler_count_sunday;
	}
	else
	if (	performed_everglades_city_census_weekend ==
		day_saturday_census_missed )
	{
		fishing_trip->everglades_city_fishing_saturday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_sunday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_pleasure_saturday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_pleasure_sunday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_weekend =
			fishing_trip->everglades_city_fishing_saturday +
			fishing_trip->everglades_city_fishing_sunday;

		fishing_trip->everglades_city_boating_weekend =
			fishing_trip->everglades_city_fishing_saturday +
			fishing_trip->everglades_city_pleasure_saturday +
			fishing_trip->everglades_city_fishing_sunday +
			fishing_trip->everglades_city_pleasure_sunday;

		fishing_trip->everglades_city_fishing_day =
			(double)fishing_trip->everglades_city_fishing_weekend /
			2.0;

		fishing_trip->everglades_city_boating_day =
			(double)fishing_trip->everglades_city_boating_weekend /
			2.0;

		fishing_trip->everglades_city_fishing_hours_saturday =
			total_caught_get_fishing_hours_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_hours_sunday =
			total_caught_get_fishing_hours_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_hours_weekend =
			fishing_trip->everglades_city_fishing_hours_saturday +
			fishing_trip->everglades_city_fishing_hours_sunday;

		fishing_trip->everglades_city_fishing_hours_day =
			(double)fishing_trip->
				everglades_city_fishing_hours_weekend /
			2.0;

		fishing_trip->everglades_city_angler_count_saturday =
			total_caught_get_angler_count_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_angler_count_sunday =
			total_caught_get_angler_count_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_angler_count_weekend =
			fishing_trip->everglades_city_angler_count_saturday +
			fishing_trip->everglades_city_angler_count_sunday;

		fishing_trip->everglades_city_angler_count_day =
			(double)fishing_trip->
				everglades_city_angler_count_weekend /
			2.0;
	}
	else
	if (	performed_everglades_city_census_weekend ==
		day_sunday_census_missed_saturday_not_missed )
	{
		fishing_trip->everglades_city_fishing_saturday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_pleasure_saturday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_day =
			(double)fishing_trip->everglades_city_fishing_saturday;

		fishing_trip->everglades_city_boating_day =
			(double)fishing_trip->everglades_city_fishing_saturday +
			(double)fishing_trip->everglades_city_pleasure_saturday;

		fishing_trip->everglades_city_fishing_hours_saturday =
			total_caught_get_fishing_hours_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_hours_day =
			(double)fishing_trip->
				everglades_city_fishing_hours_saturday;

		fishing_trip->everglades_city_angler_count_saturday =
			total_caught_get_angler_count_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_angler_count_day =
			(double)fishing_trip->
				everglades_city_angler_count_saturday;
	}
	else
	if (	performed_everglades_city_census_weekend ==
		day_sunday_census_weekend_missed )
	{
		fishing_trip->everglades_city_fishing_saturday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_sunday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_pleasure_saturday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_pleasure_sunday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_weekend =
			fishing_trip->everglades_city_fishing_saturday +
			fishing_trip->everglades_city_fishing_sunday;

		fishing_trip->everglades_city_boating_weekend =
			fishing_trip->everglades_city_fishing_saturday +
			fishing_trip->everglades_city_pleasure_saturday +
			fishing_trip->everglades_city_fishing_sunday +
			fishing_trip->everglades_city_pleasure_sunday;

		fishing_trip->everglades_city_fishing_day =
			(double)fishing_trip->everglades_city_fishing_weekend /
			2.0;

		fishing_trip->everglades_city_boating_day =
			(double)fishing_trip->everglades_city_boating_weekend /
			2.0;

		fishing_trip->everglades_city_fishing_hours_saturday =
			total_caught_get_fishing_hours_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_hours_sunday =
			total_caught_get_fishing_hours_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_hours_weekend =
			fishing_trip->everglades_city_fishing_hours_saturday +
			fishing_trip->everglades_city_fishing_hours_sunday;

		fishing_trip->everglades_city_fishing_hours_day =
			(double)fishing_trip->
				everglades_city_fishing_hours_weekend /
			2.0;

		fishing_trip->everglades_city_angler_count_saturday =
			total_caught_get_angler_count_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_angler_count_sunday =
			total_caught_get_angler_count_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_angler_count_weekend =
			fishing_trip->everglades_city_angler_count_saturday +
			fishing_trip->everglades_city_angler_count_sunday;

		fishing_trip->everglades_city_angler_count_day =
			(double)fishing_trip->
				everglades_city_angler_count_weekend /
			2.0;
	}
	else
	if (	performed_everglades_city_census_weekend ==
		day_weekday_census_saturday_only )
	{
		fishing_trip->everglades_city_fishing_saturday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_pleasure_saturday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_day =
			(double)fishing_trip->everglades_city_fishing_saturday;

		fishing_trip->everglades_city_boating_day =
			(double)fishing_trip->everglades_city_fishing_saturday +
			(double)fishing_trip->everglades_city_pleasure_saturday;

		fishing_trip->everglades_city_fishing_hours_saturday =
			total_caught_get_fishing_hours_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_hours_day =
			(double)fishing_trip->
				everglades_city_fishing_hours_saturday;

		fishing_trip->everglades_city_angler_count_saturday =
			total_caught_get_angler_count_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_angler_count_day =
			(double)fishing_trip->
				everglades_city_angler_count_saturday;
	}
	else
	if (	performed_everglades_city_census_weekend ==
		day_weekday_census_sunday_only )
	{
		fishing_trip->everglades_city_fishing_sunday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_pleasure_sunday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_day =
			(double)fishing_trip->everglades_city_fishing_sunday;

		fishing_trip->everglades_city_boating_day =
			(double)fishing_trip->everglades_city_fishing_sunday +
			(double)fishing_trip->everglades_city_pleasure_sunday;

		fishing_trip->everglades_city_fishing_hours_sunday =
			total_caught_get_fishing_hours_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_hours_day =
			(double)fishing_trip->
				everglades_city_fishing_hours_sunday;

		fishing_trip->everglades_city_angler_count_sunday =
			total_caught_get_angler_count_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_angler_count_day =
			(double)fishing_trip->
				everglades_city_angler_count_sunday;
	}
	else
	if (	performed_everglades_city_census_weekend ==
		day_weekday_census_saturday_and_sunday )
	{
		fishing_trip->everglades_city_fishing_saturday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_sunday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_pleasure_saturday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_pleasure_sunday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_weekend =
			fishing_trip->everglades_city_fishing_saturday +
			fishing_trip->everglades_city_fishing_sunday;

		fishing_trip->everglades_city_boating_weekend =
			fishing_trip->everglades_city_fishing_saturday +
			fishing_trip->everglades_city_pleasure_saturday +
			fishing_trip->everglades_city_fishing_sunday +
			fishing_trip->everglades_city_pleasure_sunday;

		fishing_trip->everglades_city_fishing_day =
			(double)fishing_trip->everglades_city_fishing_weekend /
			2.0;

		fishing_trip->everglades_city_boating_day =
			(double)fishing_trip->everglades_city_boating_weekend /
			2.0;

		fishing_trip->everglades_city_fishing_hours_saturday =
			total_caught_get_fishing_hours_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_hours_sunday =
			total_caught_get_fishing_hours_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_hours_weekend =
			fishing_trip->everglades_city_fishing_hours_saturday +
			fishing_trip->everglades_city_fishing_hours_sunday;

		fishing_trip->everglades_city_fishing_hours_day =
			(double)fishing_trip->
				everglades_city_fishing_hours_weekend /
			2.0;

		fishing_trip->everglades_city_angler_count_saturday =
			total_caught_get_angler_count_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_angler_count_sunday =
			total_caught_get_angler_count_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_angler_count_weekend =
			fishing_trip->everglades_city_angler_count_saturday +
			fishing_trip->everglades_city_angler_count_sunday;

		fishing_trip->everglades_city_angler_count_day =
			(double)fishing_trip->
				everglades_city_angler_count_weekend /
			2.0;
	}
	else
	if (	performed_everglades_city_census_weekend ==
		day_weekday_census_missed_saturday_and_sunday )
	{
		fishing_trip->everglades_city_fishing_saturday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_sunday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_pleasure_saturday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_pleasure_sunday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_weekend =
			fishing_trip->everglades_city_fishing_saturday +
			fishing_trip->everglades_city_fishing_sunday;

		fishing_trip->everglades_city_boating_weekend =
			fishing_trip->everglades_city_fishing_saturday +
			fishing_trip->everglades_city_pleasure_saturday +
			fishing_trip->everglades_city_fishing_sunday +
			fishing_trip->everglades_city_pleasure_sunday;

		fishing_trip->everglades_city_fishing_day =
			(double)fishing_trip->everglades_city_fishing_weekend /
			2.0;

		fishing_trip->everglades_city_boating_day =
			(double)fishing_trip->everglades_city_boating_weekend /
			2.0;

		fishing_trip->everglades_city_fishing_hours_saturday =
			total_caught_get_fishing_hours_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_hours_sunday =
			total_caught_get_fishing_hours_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_fishing_hours_weekend =
			fishing_trip->everglades_city_fishing_hours_saturday +
			fishing_trip->everglades_city_fishing_hours_sunday;

		fishing_trip->everglades_city_fishing_hours_day =
			(double)fishing_trip->
				everglades_city_fishing_hours_weekend /
			2.0;

		fishing_trip->everglades_city_angler_count_saturday =
			total_caught_get_angler_count_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_angler_count_sunday =
			total_caught_get_angler_count_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_flamingo */ );

		fishing_trip->everglades_city_angler_count_weekend =
			fishing_trip->everglades_city_angler_count_saturday +
			fishing_trip->everglades_city_angler_count_sunday;

		fishing_trip->everglades_city_angler_count_day =
			(double)fishing_trip->
				everglades_city_angler_count_weekend /
			2.0;

	}
	else
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: invalid performed_everglades_city_census_weekend = %d.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 performed_everglades_city_census_weekend );
		exit( 1 );
	}

	/* Calculate interviews park fishing day (J) */
	/* ----------------------------------------- */
	fishing_trip->park_fishing_day =
		fishing_trip->flamingo_fishing_day +
		fishing_trip->everglades_city_fishing_day;

	/* Calculate park boating trips day (K) */
	/* ------------------------------------ */
	fishing_trip->park_boating_day =
		fishing_trip->flamingo_boating_day +
		fishing_trip->everglades_city_boating_day;

	return fishing_trip;

} /* total_caught_month_row_get_fishing_trip() */

int total_caught_get_catches_weekend(
			LIST *fishing_trip_list,
			char *family,
			char *genus,
			char *species_name,
			boolean is_areas_1_5,
			boolean is_kept )
{
	FISHING_TRIP *fishing_trip;
	CATCH *catch;
	int catches_weekend = 0;

	if ( !list_rewind( fishing_trip_list ) )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty fishing_trip_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return 0;
	}

	do {
		fishing_trip = list_get_pointer( fishing_trip_list );

		if ( is_areas_1_5
		&&   ( fishing_trip->fishing_area_integer < 1
		||     fishing_trip->fishing_area_integer > 5 ) )
		{
			continue;
		}

		if ( !is_areas_1_5
		&&   fishing_trip->fishing_area_integer != 6 )
		{
			continue;
		}

		if ( !list_rewind( fishing_trip->catch_list ) )
		{
			continue;
		}

		do {
			catch = list_get_pointer( fishing_trip->catch_list );

			if ( strcmp(	catch->species->family,
					family ) != 0
			||   strcmp(	catch->species->genus,
					genus ) != 0
			||   strcmp(	catch->species->species_name,
					species_name ) != 0 )
			{
				continue;
			}

			if ( is_kept )
				catches_weekend += catch->kept;
			else
				catches_weekend += catch->released;

		} while( list_next( fishing_trip->catch_list ) );

	} while( list_next( fishing_trip_list ) );

	return catches_weekend;

} /* total_caught_get_catches_weekend() */

TOTAL_SHEET *total_caught_get_total_sheet(
			LIST *month_sheet_list,
			int begin_month,
			int year )
{
	TOTAL_SHEET *total_sheet;
	MONTH_SHEET *month_sheet;
	TOTAL_ROW *total_row;
	int month_integer;

	if ( !list_rewind( month_sheet_list ) ) return (TOTAL_SHEET *)0;

	month_integer = begin_month;
	total_sheet = total_caught_total_sheet_new( year );

	do {
		month_sheet = list_get_pointer( month_sheet_list );

		total_row = total_caught_total_row_new(
				timlib_integer2full_month( month_integer ) );

		total_caught_populate_total_row(
					total_row,
					month_sheet->row_list );

		list_append_pointer( total_sheet->total_row_list, total_row );
		month_integer++;

	} while( list_next( month_sheet_list ) );

/*
	if ( ! ( total_sheet->total_row =
		total_caught_get_total_sheet_total_row(
			total_sheet->total_row_list ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty total sheet row list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
*/

	total_sheet->total_row =
		total_caught_get_total_sheet_total_row(
			total_sheet->total_row_list );

	return total_sheet;

} /* total_caught_get_total_sheet() */

TOTAL_ROW *total_caught_total_row_new( char *month )
{
	TOTAL_ROW *total_row;

	if ( ! ( total_row = calloc( 1, sizeof( TOTAL_ROW ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	total_row->month = month;
	return total_row;

} /* total_caught_total_row_new() */

void total_caught_populate_total_row(	TOTAL_ROW *total_row,
					LIST *month_row_list )
{
	MONTH_ROW *month_row;
	CATCH_AREA *catch_area;
	CATCH_AREA *month_row_catch_area;

	total_row->catch_area_list = list_new();

	if ( !list_rewind( month_row_list ) ) return;

	do {
		month_row = list_get_pointer( month_row_list );

		total_row->florida_bay_trailer_count +=
		month_row->vessel->florida_bay_trailer_count;

		total_row->whitewater_bay_trailer_count +=
		month_row->vessel->whitewater_bay_trailer_count;

		total_row->total_trailer_count +=
		month_row->vessel->total_trailer_count;

		total_row->estimated_areas_1_5_vessels +=
		month_row->vessel->estimated_areas_1_5_vessels;

		total_row->estimated_area_6_vessels +=
		month_row->vessel->estimated_area_6_vessels;

		total_row->estimated_areas_1_6_vessels +=
		month_row->vessel->estimated_areas_1_6_vessels;

		total_row->flamingo_fishing_day +=
		month_row->fishing_trip->flamingo_fishing_day;

		total_row->flamingo_boating_day +=
		month_row->fishing_trip->flamingo_boating_day;

		total_row->park_fishing_day +=
		month_row->fishing_trip->park_fishing_day;

		total_row->park_boating_day +=
		month_row->fishing_trip->park_boating_day;

		total_row->estimated_park_fishing_vessels +=
		month_row->vessel->estimated_park_fishing_vessels;

		total_row->park_effort_hours_day +=
			month_row->fishing_trip->park_effort_hours_day;


		if ( list_rewind( month_row->catch_area_list ) )
		{
			do {
				month_row_catch_area =
					list_get_pointer(
						month_row->catch_area_list );

				catch_area =
				total_caught_get_or_set_catch_area(
				total_row->catch_area_list,
				month_row_catch_area->species->family,
				month_row_catch_area->species->genus,
				month_row_catch_area->species->species_name,
				month_row_catch_area->species );

				catch_area->areas_1_5_estimated_kept +=
				month_row_catch_area->areas_1_5_estimated_kept;

				catch_area->areas_1_5_estimated_released +=
				month_row_catch_area->
					areas_1_5_estimated_released;

				catch_area->areas_1_5_estimated_caught +=
				month_row_catch_area->
					areas_1_5_estimated_caught;

				catch_area->areas_1_6_estimated_kept +=
				month_row_catch_area->areas_1_6_estimated_kept;

				catch_area->areas_1_6_estimated_released +=
				month_row_catch_area->
					areas_1_6_estimated_released;

				catch_area->areas_1_6_estimated_caught +=
				month_row_catch_area->
					areas_1_6_estimated_caught;

			} while( list_next( month_row->catch_area_list ) );
		}

	} while( list_next( month_row_list ) );

} /* total_caught_populate_total_row() */

TOTAL_ROW *total_caught_get_total_sheet_total_row(
			LIST *total_row_list )
{
	TOTAL_ROW *total_sheet_total_row;
	TOTAL_ROW *total_row;
	CATCH_AREA *total_sheet_total_row_catch_area;
	CATCH_AREA *total_row_catch_area;

	if ( !list_rewind( total_row_list ) ) return (TOTAL_ROW *)0;

	total_sheet_total_row = total_caught_total_row_new( "Total" );
	total_sheet_total_row->catch_area_list = list_new();

	do {
		total_row = list_get_pointer( total_row_list );

		total_sheet_total_row->florida_bay_trailer_count +=
			total_row->florida_bay_trailer_count;

		total_sheet_total_row->whitewater_bay_trailer_count +=
			total_row->whitewater_bay_trailer_count;

		total_sheet_total_row->total_trailer_count +=
			total_row->total_trailer_count;

		total_sheet_total_row->estimated_areas_1_5_vessels +=
			total_row->estimated_areas_1_5_vessels;

		total_sheet_total_row->estimated_area_6_vessels +=
			total_row->estimated_area_6_vessels;

		total_sheet_total_row->estimated_areas_1_6_vessels +=
			total_row->estimated_areas_1_6_vessels;

		total_sheet_total_row->estimated_park_fishing_vessels +=
			total_row->estimated_park_fishing_vessels;

		total_sheet_total_row->park_effort_hours_day +=
			total_row->park_effort_hours_day;


		total_sheet_total_row->flamingo_fishing_day +=
			total_row->flamingo_fishing_day;

		total_sheet_total_row->flamingo_boating_day +=
			total_row->flamingo_boating_day;

		total_sheet_total_row->park_fishing_day +=
			total_row->park_fishing_day;

		total_sheet_total_row->park_boating_day +=
			total_row->park_boating_day;

		if ( list_rewind( total_row->catch_area_list ) )
		{

			do {
				total_row_catch_area =
					list_get_pointer(
						total_row->catch_area_list );

				total_sheet_total_row_catch_area =
				total_caught_get_or_set_catch_area(
				total_sheet_total_row->catch_area_list,
				total_row_catch_area->species->family,
				total_row_catch_area->species->genus,
				total_row_catch_area->species->
					species_name,
				total_row_catch_area->species );

				total_sheet_total_row_catch_area->
				areas_1_5_estimated_kept +=
				total_row_catch_area->
						areas_1_5_estimated_kept;

				total_sheet_total_row_catch_area->
				areas_1_5_estimated_released +=
				total_row_catch_area->
				     areas_1_5_estimated_released;

				total_sheet_total_row_catch_area->
				areas_1_5_estimated_caught +=
				total_row_catch_area->
				     areas_1_5_estimated_caught;

				total_sheet_total_row_catch_area->
				areas_1_6_estimated_kept +=
				total_row_catch_area->
					areas_1_6_estimated_kept;

				total_sheet_total_row_catch_area->
				areas_1_6_estimated_released +=
				total_row_catch_area->
				     areas_1_6_estimated_released;

				total_sheet_total_row_catch_area->
				areas_1_6_estimated_caught +=
				total_row_catch_area->
				     areas_1_6_estimated_caught;

			} while( list_next( total_row->catch_area_list ) );
		}

	} while( list_next( total_row_list ) );

	return total_sheet_total_row;

} /* total_caught_get_total_sheet_total_row() */

void total_caught_populate_grand_totals(
			double *grand_areas_1_5_estimated_kept,
			double *grand_areas_1_5_estimated_released,
			double *grand_areas_1_5_estimated_caught,
			double *grand_areas_1_6_estimated_kept,
			double *grand_areas_1_6_estimated_released,
			double *grand_areas_1_6_estimated_caught,
			LIST *catch_area_list )
{
	CATCH_AREA *catch_area;

	if ( !list_rewind( catch_area_list ) ) return;

	do {
		catch_area = list_get_pointer( catch_area_list );

		*grand_areas_1_5_estimated_kept +=
			catch_area->areas_1_5_estimated_kept;

		*grand_areas_1_5_estimated_released +=
			catch_area->areas_1_5_estimated_released;

		*grand_areas_1_5_estimated_caught +=
			catch_area->areas_1_5_estimated_caught;

		*grand_areas_1_6_estimated_kept +=
			catch_area->areas_1_6_estimated_kept;

		*grand_areas_1_6_estimated_released +=
			catch_area->areas_1_6_estimated_released;

		*grand_areas_1_6_estimated_caught +=
			catch_area->areas_1_6_estimated_caught;

	} while( list_next( catch_area_list ) );

} /* total_caught_populate_grand_totals() */

enum performed_census_weekend total_caught_get_performed_census_weekend(
				DATE *row_date,
				DICTIONARY *weekend_creel_census_dictionary,
				char *interview_location )
{
	DATE *prior_saturday;
	DATE *prior_sunday;
	boolean exists_saturday;
	boolean exists_sunday;

	if ( date_get_day_of_week( row_date ) == WDAY_SATURDAY )
	{
		if ( total_caught_exists_census(
				date_yyyy_mm_dd( row_date ),
				weekend_creel_census_dictionary,
				interview_location ) )
		{
			return day_saturday_census_saturday;
		}
		else
		{
			return day_saturday_census_missed;
		}
	}

	if ( date_get_day_of_week( row_date ) == WDAY_SUNDAY )
	{
		if ( total_caught_exists_census(
				date_yyyy_mm_dd( row_date ),
				weekend_creel_census_dictionary,
				interview_location ) )
		{
			return day_sunday_census_sunday;
		}

		prior_saturday =
			date_get_prior_saturday(
				row_date,
				date_get_utc_offset() );

		if ( total_caught_exists_census(
				date_yyyy_mm_dd( prior_saturday ),
				weekend_creel_census_dictionary,
				interview_location ) )
		{
			return day_sunday_census_missed_saturday_not_missed;
		}

		return day_sunday_census_weekend_missed;
	}

	/* Must be weekday */
	/* --------------- */
	prior_saturday =
		date_get_prior_saturday(
			row_date,
			date_get_utc_offset() );

	prior_sunday = date_new( 0, 0, 0 );
	date_copy( prior_sunday, prior_saturday );

	date_increment_days(
		prior_sunday,
		1.0 );

	exists_saturday =
		total_caught_exists_census(
			date_yyyy_mm_dd( prior_saturday ),
			weekend_creel_census_dictionary,
			interview_location );

	exists_sunday =
		total_caught_exists_census(
			date_yyyy_mm_dd( prior_sunday ),
			weekend_creel_census_dictionary,
			interview_location );

	if ( exists_saturday && !exists_sunday )
		return day_weekday_census_saturday_only;
	else
	if ( !exists_saturday && exists_sunday )
		return day_weekday_census_sunday_only;
	else
	if ( exists_saturday && exists_sunday )
		return day_weekday_census_saturday_and_sunday;
	else
		return day_weekday_census_missed_saturday_and_sunday;

} /* total_caught_get_performed_census_weekend() */

boolean total_caught_exists_census(
				char *census_date_string,
				DICTIONARY *weekend_creel_census_dictionary,
				char *interview_location )
{
	char *key;

	key = total_caught_get_weekend_key(
			census_date_string,
			interview_location );

	if ( dictionary_fetch(
			weekend_creel_census_dictionary,
			key ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

FISHING_TRIP_LIST *total_caught_fishing_trip_list_new( void )
{
	FISHING_TRIP_LIST *fishing_trip_list;

	if ( ! ( fishing_trip_list =
			calloc( 1, sizeof( FISHING_TRIP_LIST ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	fishing_trip_list->fishing_trip_list = list_new();

	return fishing_trip_list;

} /* total_caught_fishing_trip_list_new() */

boolean	total_caught_get_census_date_month_day(
			int *census_date_month,
			int *census_date_day,
			char *census_date_string )
{
	char piece_buffer[ 128 ];

	/* Get census date month */
	/* --------------------- */
	piece( piece_buffer, '-', census_date_string, 1 );
	*census_date_month = atoi( piece_buffer );

	if ( *census_date_month < 1
	||   *census_date_month > MONTHS_PER_YEAR )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: invalid census_date = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 census_date_string );
		return 0;
	}

	/* Get census date day */
	/* ------------------- */
	piece( piece_buffer, '-', census_date_string, 2 );
	*census_date_day = atoi( piece_buffer );

	if ( *census_date_day < 1
	||   *census_date_day > DAYS_PER_MONTH )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: invalid census_date = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 census_date_string );
		return 0;
	}
	return 1;
} /* total_caught_get_census_date_month_day() */

LIST *total_caught_with_date_get_fishing_trip_list(
			FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
			char *date_string )
{
	int census_date_month;
	int census_date_day;

	if ( !total_caught_get_census_date_month_day(
		&census_date_month,
		&census_date_day,
		date_string ) )
	{
		return (LIST *)0;
	}

	if ( !fishing_trip_list_array	[ census_date_month - 1 ]
					[ census_date_day - 1 ] )
	{
		return (LIST *)0;
	}

	return fishing_trip_list_array	[ census_date_month - 1 ]
					[ census_date_day - 1 ]->
			fishing_trip_list;

} /* total_caught_with_date_get_fishing_trip_list() */

boolean total_caught_get_coefficients(
				double *areas_1_5_coefficient,
				double *areas_1_5_y_intercept,
				double *area_6_coefficient,
				double *area_6_y_intercept,
				char *application_name )
{
	APPLICATION_CONSTANTS *application_constants;
	char *value_string;

	application_constants = application_constants_new();

	application_constants->dictionary =
			application_constants_get_dictionary(
				application_name );

	if ( ! ( value_string =
			application_constants_fetch(
				application_constants->dictionary,
				"areas_1_5_coefficient" ) ) )
	{
		return 0;
	}
	*areas_1_5_coefficient = atof( value_string );

	if ( ! ( value_string =
			application_constants_fetch(
				application_constants->dictionary,
				"areas_1_5_y_intercept" ) ) )
	{
		return 0;
	}
	*areas_1_5_y_intercept = atof( value_string );

	if ( ! ( value_string =
			application_constants_fetch(
				application_constants->dictionary,
				"area_6_coefficient" ) ) )
	{
		return 0;
	}
	*area_6_coefficient = atof( value_string );

	if ( ! ( value_string =
			application_constants_fetch(
				application_constants->dictionary,
				"area_6_y_intercept" ) ) )
	{
		return 0;
	}
	*area_6_y_intercept = atof( value_string );

	application_constants_free( application_constants );
	return 1;

} /* total_caught_get_coefficients() */

int total_caught_get_total_trailer_count(
			int *florida_bay_trailer_count,
			int *whitewater_bay_trailer_count,
			DATE *row_date,
			DICTIONARY *trailer_count_dictionary )
{
	char *trailer_count_key;
	char *trailer_count_string;
	char buffer[ 16 ];

	/* Get florida_bay_trailer_count */
	/* ----------------------------- */
	trailer_count_key =
		total_caught_get_trailer_key(
	 		date_get_yyyy_mm_dd(	buffer,
						row_date ),
			PARKING_LOT_FLORIDA_BAY );

	if ( ( trailer_count_string =
			dictionary_get_pointer(
				trailer_count_dictionary,
				trailer_count_key ) ) )
	{
		*florida_bay_trailer_count =
			atoi( trailer_count_string );
	}

	/* Get whitewater_bay_trailer_count */
	/* -------------------------------- */
	trailer_count_key =
		total_caught_get_trailer_key(
	 		date_get_yyyy_mm_dd(	buffer,
						row_date ),
			PARKING_LOT_WHITEWATER_BAY );

	if ( ( trailer_count_string =
			dictionary_get_pointer(
				trailer_count_dictionary,
				trailer_count_key ) ) )
	{
		*whitewater_bay_trailer_count =
			atoi( trailer_count_string );
	}

	return  *florida_bay_trailer_count +
		*whitewater_bay_trailer_count;

} /* total_caught_get_total_trailer_count() */

boolean total_caught_get_saturday_sunday_interview_dates(
		DATE **interview_date_saturday,
		DATE **interview_date_sunday,
		DATE *row_date,
		enum performed_census_weekend performed_census_weekend,
		DICTIONARY *weekend_creel_census_dictionary,
		char *interview_location )
{
	*interview_date_saturday = (DATE *)0;
	*interview_date_sunday = (DATE *)0;

	if ( performed_census_weekend == day_saturday_census_saturday )
	{
		*interview_date_saturday = row_date;
	}
	else
	if ( performed_census_weekend == day_sunday_census_sunday )
	{
		*interview_date_sunday = row_date;
	}
	else
	if ( performed_census_weekend == day_saturday_census_missed )
	{
		*interview_date_saturday =
			total_caught_get_preceeding_full_census_saturday(
				row_date,
				weekend_creel_census_dictionary,
				interview_location );

		if ( !*interview_date_saturday ) return 0;

		*interview_date_sunday =
			date_new( 0, 0, 0 );

		date_copy( *interview_date_sunday, *interview_date_saturday );

		date_increment_days(
			*interview_date_sunday,
			1.0 );
	}
	else
	if (	performed_census_weekend ==
		day_sunday_census_missed_saturday_not_missed )
	{
		*interview_date_saturday =
			date_new( 0, 0, 0 );

		date_copy( *interview_date_saturday, row_date );

		date_increment_days(
			*interview_date_saturday,
			-1.0 );
	}
	else
	if ( performed_census_weekend == day_sunday_census_weekend_missed )
	{
		*interview_date_saturday =
			total_caught_get_preceeding_full_census_saturday(
				row_date,
				weekend_creel_census_dictionary,
				interview_location );

		if ( !*interview_date_saturday ) return 0;

		*interview_date_sunday = date_new( 0, 0, 0 );

		if ( !*interview_date_saturday )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: empty interview_date_saturday for row_date = (%s).\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 date_display( row_date ) );
			exit( 1 );
		}

		if ( !date_copy(	*interview_date_sunday,
					*interview_date_saturday ) )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: cannot copy source date = (%s).\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 date_display( *interview_date_saturday ) );
			exit( 1 );
		}

		date_increment_days(
			*interview_date_sunday,
			1.0 );
	}
	else
	if ( performed_census_weekend == day_weekday_census_saturday_only )
	{
		*interview_date_saturday =
			date_get_prior_saturday(
				row_date,
				date_get_utc_offset() );
	}
	else
	if ( performed_census_weekend == day_weekday_census_sunday_only )
	{
		*interview_date_sunday =
			date_get_prior_saturday(
				row_date,
				date_get_utc_offset() );

		date_increment_days(
			*interview_date_sunday,
			1.0 );
	}
	else
	if (	performed_census_weekend ==
		day_weekday_census_saturday_and_sunday )
	{
		*interview_date_saturday =
			date_get_prior_saturday(
				row_date,
				date_get_utc_offset() );

		*interview_date_sunday =
			date_new( 0, 0, 0 );

		date_copy( *interview_date_sunday, *interview_date_saturday );

		date_increment_days(
			*interview_date_sunday,
			1.0 );
	}
	else
	if (	performed_census_weekend ==
		day_weekday_census_missed_saturday_and_sunday )
	{
		*interview_date_saturday =
			total_caught_get_preceeding_full_census_saturday(
				row_date,
				weekend_creel_census_dictionary,
				interview_location );

		if ( !*interview_date_saturday ) return 0;

		*interview_date_sunday =
			date_new( 0, 0, 0 );

		date_copy( *interview_date_sunday, *interview_date_saturday );

		date_increment_days(
			*interview_date_sunday,
			1.0 );
	}
	else
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: invalid performed_census_weekend = %d.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 performed_census_weekend );
		exit( 1 );
	}

	return 1;

} /* total_caught_get_saturday_sunday_interview_dates() */

DATE *total_caught_get_preceeding_full_census_saturday(
				DATE *row_date,
				DICTIONARY *weekend_creel_census_dictionary,
				char *interview_location )
{
	DATE *preceeding_saturday;
	enum performed_census_weekend performed_census_weekend;
	int i;

	preceeding_saturday =
		date_get_prior_saturday(
			row_date,
			date_get_utc_offset() );

	for( i = 0; i < WEEKENDS_TO_GO_BACK; i++ )
	{
		performed_census_weekend =
		total_caught_get_performed_census_weekend(
			preceeding_saturday,
			weekend_creel_census_dictionary,
			interview_location );

		if ( performed_census_weekend == day_saturday_census_saturday )
		{
			DATE *preceeding_sunday;

			preceeding_sunday =
				date_new( 0, 0, 0 );

			date_copy( preceeding_sunday, preceeding_saturday );

			date_increment_days(
				preceeding_sunday,
				1.0 );

			performed_census_weekend =
			total_caught_get_performed_census_weekend(
				preceeding_sunday,
				weekend_creel_census_dictionary,
				interview_location );

			if (	performed_census_weekend ==
				day_sunday_census_sunday )
			{
				return preceeding_saturday;
			}
		}
		date_decrement_days(
			preceeding_saturday,
			7.0 );
	}

	return (DATE *)0;

} /* total_caught_get_preceeding_full_census_saturday() */

int total_caught_get_fishing_trips_weekend(
			DATE *search_date,
			FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
			boolean is_fishing_trips,
			boolean is_flamingo )
{
	FISHING_TRIP *fishing_trip;
	int fishing_trips_weekend = 0;
	LIST *fishing_trip_list;

	if ( !search_date ) return 0;

	fishing_trip_list =
		total_caught_with_date_get_fishing_trip_list(
			fishing_trip_list_array,
			date_display( search_date ) );

	if ( !list_rewind( fishing_trip_list ) ) return 0;

	do {
		fishing_trip = list_get_pointer( fishing_trip_list );

		if ( is_fishing_trips )
		{
			if ( !fishing_trip->hours_fishing ) continue;
		}
		else
		{
			if ( fishing_trip->hours_fishing ) continue;
		}

		if ( is_flamingo )
		{
			/* ------------------------------------- */
			/* If no fishing area, then consider the */
			/* trip origin location.		 */
			/* ------------------------------------- */
			if ( fishing_trip->fishing_area_integer == 0 )
			{
				if ( timlib_strcmp(
					fishing_trip->
						trip_origin_location,
					INTERVIEW_LOCATION_FLAMINGO )
					!= 0 )
				{
					continue;
				}
			}
			else
			if (	fishing_trip->fishing_area_integer <
				FLAMINGO_BEGINNING_FISHING_AREA
			||	fishing_trip->fishing_area_integer >
				FLAMINGO_ENDING_FISHING_AREA )
			{
				continue;
			}
		}
		else
		{
			/* ------------------------------------- */
			/* If no fishing area, then consider the */
			/* trip origin location.		 */
			/* ------------------------------------- */
			if ( fishing_trip->fishing_area_integer == 0 )
			{
				if ( timlib_strcmp(
					fishing_trip->
						trip_origin_location,
					INTERVIEW_LOCATION_EVERGLADES_CITY )
					!= 0 )
				{
					continue;
				}
			}
			else
			if (	fishing_trip->fishing_area_integer !=
				EVERGLADES_CITY_FISHING_AREA )
			{
					continue;
			}
		}

		fishing_trips_weekend++;

	} while( list_next( fishing_trip_list ) );

	return fishing_trips_weekend;

} /* total_caught_get_fishing_trips_weekend() */

int total_caught_get_fishing_hours_weekend(
			DATE *search_date,
			FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
			boolean is_flamingo )
{
	FISHING_TRIP *fishing_trip;
	int fishing_hours_weekend = 0;
	LIST *fishing_trip_list;

	if ( !search_date ) return 0;

	fishing_trip_list =
		total_caught_with_date_get_fishing_trip_list(
			fishing_trip_list_array,
			date_display( search_date ) );

	if ( !list_rewind( fishing_trip_list ) ) return 0;

	do {
		fishing_trip = list_get_pointer( fishing_trip_list );

		if ( !fishing_trip->hours_fishing ) continue;

		if ( is_flamingo )
		{
			/* ------------------------------------- */
			/* If no fishing area, then consider the */
			/* trip origin location.		 */
			/* ------------------------------------- */
			if ( fishing_trip->fishing_area_integer == 0 )
			{
				if ( timlib_strcmp(
					fishing_trip->
						trip_origin_location,
					INTERVIEW_LOCATION_FLAMINGO )
					!= 0 )
				{
					continue;
				}
			}
			else
			if (	fishing_trip->fishing_area_integer <
				FLAMINGO_BEGINNING_FISHING_AREA
			||	fishing_trip->fishing_area_integer >
				FLAMINGO_ENDING_FISHING_AREA )
			{
				continue;
			}
		}
		else
		{
			/* ------------------------------------- */
			/* If no fishing area, then consider the */
			/* trip origin location.		 */
			/* ------------------------------------- */
			if ( fishing_trip->fishing_area_integer == 0 )
			{
				if ( timlib_strcmp(
					fishing_trip->
						trip_origin_location,
					INTERVIEW_LOCATION_EVERGLADES_CITY )
					!= 0 )
				{
					continue;
				}
			}
			else
			if (	fishing_trip->fishing_area_integer !=
				EVERGLADES_CITY_FISHING_AREA )
			{
					continue;
			}
		}

		fishing_hours_weekend += fishing_trip->hours_fishing;

	} while( list_next( fishing_trip_list ) );

	return fishing_hours_weekend;

} /* total_caught_get_fishing_hours_weekend() */

int total_caught_get_angler_count_weekend(
			DATE *search_date,
			FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
			boolean is_flamingo )
{
	FISHING_TRIP *fishing_trip;
	int angler_count = 0;
	LIST *fishing_trip_list;

	if ( !search_date ) return 0;

	fishing_trip_list =
		total_caught_with_date_get_fishing_trip_list(
			fishing_trip_list_array,
			date_display( search_date ) );

	if ( !list_rewind( fishing_trip_list ) ) return 0;

	do {
		fishing_trip = list_get_pointer( fishing_trip_list );

		if ( !fishing_trip->hours_fishing ) continue;

		if ( is_flamingo )
		{
			/* ------------------------------------- */
			/* If no fishing area, then consider the */
			/* trip origin location.		 */
			/* ------------------------------------- */
			if ( fishing_trip->fishing_area_integer == 0 )
			{
				if ( timlib_strcmp(
					fishing_trip->
						trip_origin_location,
					INTERVIEW_LOCATION_FLAMINGO )
					!= 0 )
				{
					continue;
				}
			}
			else
			if (	fishing_trip->fishing_area_integer <
				FLAMINGO_BEGINNING_FISHING_AREA
			||	fishing_trip->fishing_area_integer >
				FLAMINGO_ENDING_FISHING_AREA )
			{
				continue;
			}
		}
		else
		{
			/* ------------------------------------- */
			/* If no fishing area, then consider the */
			/* trip origin location.		 */
			/* ------------------------------------- */
			if ( fishing_trip->fishing_area_integer == 0 )
			{
				if ( timlib_strcmp(
					fishing_trip->
						trip_origin_location,
					INTERVIEW_LOCATION_EVERGLADES_CITY )
					!= 0 )
				{
					continue;
				}
			}
			else
			if (	fishing_trip->fishing_area_integer != 
				EVERGLADES_CITY_FISHING_AREA )
			{
					continue;
			}
		}

		angler_count += fishing_trip->number_of_people_fishing;

	} while( list_next( fishing_trip_list ) );

	return angler_count;

} /* total_caught_get_angler_count_weekend() */

