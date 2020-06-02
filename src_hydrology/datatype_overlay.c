/* --------------------------------------------------- 	*/
/* datatype_overlay.c				       	*/
/* --------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "measurement_update_parameter.h"
#include "query.h"
#include "piece.h"
#include "document.h"
#include "measurement_backup.h"
#include "appaserver_parameter_file.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "environ.h"
#include "hydrology_library.h"

/* Constants */
/* --------- */
#define MEASUREMENT_SELECT_LIST				 "measurement_date,measurement_time,station,datatype,measurement_value"

#define ALWAYS_FOLLOWS_MEASUREMENT_SELECT_LIST				 "measurement_date,measurement_time,'zzzz',station,datatype,measurement_value"

#define INPUT_DELIMITER					','
#define DATE_PIECE					0
#define TIME_PIECE					1
#define VALUE_PIECE					4
#define ALWAYS_FOLLOWS_VALUE_PIECE			5

/* Prototypes */
/* ---------- */
int build_sys_string(
		char *sys_string,
		char *application_name,
		char *station,
		char *datatype,
		char *begin_date,
		char *end_date,
		LIST *compare_station_list,
		LIST *compare_datatype_list,
		enum aggregate_level aggregate_level,
		char *appaserver_mount_point );

int main( int argc, char **argv )
{
	char *application_name;
	char *station;
	char *datatype;
	char *compare_station_list_string;
	char *compare_datatype_list_string;
	char *begin_date, *end_date;
	char sys_string[ 65536 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	int build_results;
	char *aggregate_level_string;
	char *database_string = {0};

	if ( argc != 9 )
	{
		fprintf(stderr,
"Usage: %s application station datatype begin_date end_date 'compare_station_comma_list' 'compare_datatype_comma_list' aggregate_level\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	station = argv[ 2 ];
	datatype = argv[ 3 ];
	begin_date = argv[ 4 ];
	end_date = argv[ 5 ];
	compare_station_list_string = argv[ 6 ];
	compare_datatype_list_string = argv[ 7 ];
	aggregate_level_string = argv[ 8 ];

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

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date,
					&end_date,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	build_results = build_sys_string(
			sys_string,
			application_name,
			station,
			datatype,
			begin_date,
			end_date,
			string2list( compare_station_list_string, ',' ),
			string2list( compare_datatype_list_string, ',' ),
			aggregate_level_get_aggregate_level(
				aggregate_level_string ),
			appaserver_parameter_file->appaserver_mount_point );

	if ( !build_results )
	{
		fprintf( stderr, "%s: Parameter error.\n", argv[ 0 ] );
		exit( 1 );
	}

	system( sys_string );
	exit( 0 );
} /* main() */

int build_sys_string(
		char *sys_string,
		char *application_name,
		char *station,
		char *datatype,
		char *begin_date,
		char *end_date,
		LIST *compare_station_list,
		LIST *compare_datatype_list,
		enum aggregate_level aggregate_level,
		char *appaserver_mount_point )
{
	char where_clause[ 1024 ];
	char *compare_station, *compare_datatype;
	int terminating_character;
	int first_time = 1;
	char intermediate_process[ 1024 ];
	char *pre_intermediate_process_sort = {0};
	enum aggregate_statistic anchor_aggregate_statistic;
	enum aggregate_statistic compare_aggregate_statistic;

	anchor_aggregate_statistic =
		based_on_datatype_get_aggregate_statistic(
				application_name,
				datatype,
				aggregate_statistic_none );

	if ( list_length( compare_station_list ) !=
	     list_length( compare_datatype_list ) )
		return 0;

	sprintf( where_clause,
		 "station = '%s' and 					"
		 "datatype = '%s' and 					"
		 "measurement_date >= '%s' and measurement_date <= '%s'	",
		station,
		datatype,
		begin_date,
		end_date );

	if ( aggregate_level == real_time )
	{
		strcpy( intermediate_process, "cat" );
	}
	else
	{
		sprintf( intermediate_process, 
			 "real_time2aggregate_value.e %s %d %d %d ',' %s n %s",
			 aggregate_statistic_get_string(
				anchor_aggregate_statistic ),
			 DATE_PIECE,
			 TIME_PIECE,
			 VALUE_PIECE,
			 aggregate_level_get_string( aggregate_level ),
			 end_date );
	}

	if ( aggregate_level == real_time )
	{
		pre_intermediate_process_sort = "cat";
	}
	else
	if ( aggregate_level == half_hour
	||   aggregate_level == hourly
	||   aggregate_level == daily )
	{
		/* -------------------------------------------------- */
		/* real_time2aggregate_process.e needs grouped input. */
		/* -------------------------------------------------- */
		pre_intermediate_process_sort = "sort";
	}
	else
	{
		/* ------------------------------------ */
		/* Accumulate to the end of the period. */
		/* ------------------------------------ */
		pre_intermediate_process_sort = "sort -r";
	}

/*
	if ( aggregate_level == real_time || aggregate_level == daily )
	{
		pre_intermediate_process_sort = "cat";
	}
	else
	{
		pre_intermediate_process_sort = "sort -r";
	}
*/

	sys_string += sprintf( sys_string,
	"(								  "
	"get_folder_data	application=%s				  "
	"			folder=measurement			  "
	"			select=\"%s\"				  "
	"			where=\"%s\"				 |"
	"tr '%c' '%c'							 |"
	"%s								 |"
	"%s								 |"
	"sort								 ;",
		application_name,
		MEASUREMENT_SELECT_LIST,
		where_clause,
		FOLDER_DATA_DELIMITER,
		INPUT_DELIMITER,
		pre_intermediate_process_sort,
		intermediate_process );

	list_rewind( compare_datatype_list );
	if ( list_rewind( compare_station_list ) )
	{
		do {
			compare_station = 
				list_get_string( compare_station_list );

			compare_datatype = 
				list_get_string( compare_datatype_list );

			compare_aggregate_statistic =
				based_on_datatype_get_aggregate_statistic(
						application_name,
						compare_datatype,
						aggregate_statistic_none );

			sprintf( where_clause,
		 	"station = '%s' and 				      "
		 	"datatype = '%s' and 				      "
		 	"measurement_date >= '%s' and measurement_date <= '%s'",
				compare_station,
				compare_datatype,
				begin_date,
				end_date );

			if ( first_time )
			{
				first_time = 0;
				terminating_character = ' ';
			}
			else
			{
				terminating_character = ';';
			}

			if ( aggregate_level == real_time )
			{
				strcpy( intermediate_process, "cat" );
			}
			else
			{
				sprintf( intermediate_process, 
			 "real_time2aggregate_value.e %s %d %d %d ',' %s n %s",
			 		aggregate_statistic_get_string(
						compare_aggregate_statistic ),
			 		DATE_PIECE,
			 		TIME_PIECE,
			 		ALWAYS_FOLLOWS_VALUE_PIECE,
			 		aggregate_level_get_string(
						aggregate_level ),
					end_date );
			}

			sys_string += sprintf( sys_string,
			"%cget_folder_data	application=%s		    "
			"			folder=measurement	    "
			"			select=\"%s\"		    "
			"			where=\"%s\"		   |"
			"tr '%c' '%c' 					   |"
			"%s	 					   |"
			"%s	 					   |"
			"sort						    ",
				terminating_character,
				application_name,
				ALWAYS_FOLLOWS_MEASUREMENT_SELECT_LIST,
				where_clause,
				FOLDER_DATA_DELIMITER,
				INPUT_DELIMITER,
				pre_intermediate_process_sort,
				intermediate_process );

			list_next( compare_datatype_list );
		} while( list_next( compare_station_list ) );
	}

	sprintf( sys_string, ") | sort | sed 's/\\zzzz,//'" );
	return 1;

} /* build_sys_string() */

