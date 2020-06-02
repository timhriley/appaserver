/* --------------------------------------------------- 	*/
/* catches_salinity.c				       	*/
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
#include "application.h"
#include "query.h"
#include "piece.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "hydrology_library.h"

/* Constants */
/* --------- */
#define MEASUREMENT_SELECT_LIST		"measurement_date,measurement_time,'aaaa',datatype,measurement_value"

#define CATCHES_SELECT_LIST		"%s.census_date,'null','zzzz','minutes_between',kept_count + released_count"

#define INPUT_DELIMITER					','
#define DATE_PIECE					0
#define TIME_PIECE					1
#define ORDER_COORDINATOR_PIECE				2
#define DATATYPE_PIECE					3
#define VALUE_PIECE					4

/* Prototypes */
/* ---------- */
int build_sys_string(
		char *sys_string,
		char *application_name,
		char *creel_application_name,
		char *location_area_string,
		char *begin_date_string,
		char *end_date_string );

int main( int argc, char **argv )
{
	char *creel_application_name;
	char *everglades_application_name;
	char *location_area_string;
	char *begin_date_string;
	char *end_date_string;
	char sys_string[ 65536 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	int build_results;
	char *database_string = {0};

	if ( argc != 6 )
	{
		fprintf(stderr,
"Usage: %s creel_application everglades_application location_area begin_date end_date\n",
			argv[ 0 ] );
		exit( 1 );
	}

	creel_application_name = argv[ 1 ];
	everglades_application_name = argv[ 2 ];
	location_area_string = argv[ 3 ];
	begin_date_string = argv[ 4 ];
	end_date_string = argv[ 5 ];

	if ( timlib_parse_database_string(	&database_string,
						creel_application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				creel_application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( creel_application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date_string,
					&end_date_string,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	build_results = build_sys_string(
			sys_string,
			creel_application_name,
			everglades_application_name,
			location_area_string,
			begin_date_string,
			end_date_string );

	if ( !build_results )
	{
		fprintf( stderr, "%s: Parameter error.\n", argv[ 0 ] );
		exit( 1 );
	}

/*
fprintf( stderr, "%s\n", sys_string );
*/

	system( sys_string );
	exit( 0 );
} /* main() */

int build_sys_string(
		char *sys_string,
		char *creel_application_name,
		char *everglades_application_name,
		char *location_area_string,
		char *begin_date_string,
		char *end_date_string )
{
	char measurement_where_clause[ 1024 ];
	char fishing_trips_species_where_clause[ 1024 ];
	char fishing_trips_family_where_clause[ 1024 ];
	char aggregate_process[ 1024 ];
	char *measurement_table_name;
	char *station_table_name;
	char *catches_species_table_name;
	char *catches_family_table_name;
	char *fishing_trips_table_name;
	char catches_species_select_list_string[ 1024 ];
	char catches_family_select_list_string[ 1024 ];

	application_reset();

	measurement_table_name =
		get_table_name( everglades_application_name, "measurement" );

	station_table_name =
		get_table_name( everglades_application_name, "station" );

	application_reset();

	fishing_trips_table_name =
		get_table_name( creel_application_name, "fishing_trips" );

	catches_species_table_name =
		get_table_name( creel_application_name,
				"catches_species" );

	catches_family_table_name =
		get_table_name( creel_application_name,
				"catches_family" );

	sprintf( measurement_where_clause,
		 "%s.location_area = %s and 				"
		 "measurement_date >= '%s' and				"
		 "measurement_date <= '%s' and				"
		 "datatype = 'salinity'	and				"
		 "%s.station = %s.station and				"
		 "%s.measurement_date = %s.census_date			",
		 station_table_name,
		 location_area_string,
		 begin_date_string,
		 end_date_string,
		 measurement_table_name,
		 station_table_name,
		 measurement_table_name,
		 fishing_trips_table_name );

	sprintf( fishing_trips_species_where_clause,
		 "%s.login_name = %s.login_name and			"
		 "%s.census_date = %s.census_date and			"
		 "%s.census_time = %s.census_time and			"
		 "%s.interview_location = %s.interview_location	and	"
		 "%s.interview_number = %s.interview_number and		"
		 "%s.census_date >= '%s' and 				"
		 "%s.census_date <= '%s' and				"
		 "%s.location_area = %s 				",
		 fishing_trips_table_name,
		 catches_species_table_name,
		 fishing_trips_table_name,
		 catches_species_table_name,
		 fishing_trips_table_name,
		 catches_species_table_name,
		 fishing_trips_table_name,
		 catches_species_table_name,
		 fishing_trips_table_name,
		 catches_species_table_name,
		 fishing_trips_table_name,
		 begin_date_string,
		 fishing_trips_table_name,
		 end_date_string,
		 fishing_trips_table_name,
		 location_area_string );

	sprintf( fishing_trips_family_where_clause,
		 "%s.login_name = %s.login_name and			"
		 "%s.census_date = %s.census_date and			"
		 "%s.census_time = %s.census_time and			"
		 "%s.interview_location = %s.interview_location	and	"
		 "%s.interview_number = %s.interview_number and		"
		 "%s.census_date >= '%s' and 				"
		 "%s.census_date <= '%s' and				"
		 "%s.location_area = %s 				",
		 fishing_trips_table_name,
		 catches_family_table_name,
		 fishing_trips_table_name,
		 catches_family_table_name,
		 fishing_trips_table_name,
		 catches_family_table_name,
		 fishing_trips_table_name,
		 catches_family_table_name,
		 fishing_trips_table_name,
		 catches_family_table_name,
		 fishing_trips_table_name,
		 begin_date_string,
		 fishing_trips_table_name,
		 end_date_string,
		 fishing_trips_table_name,
		 location_area_string );

	sprintf( aggregate_process,
		 "real_time2aggregate_value.e average %d %d %d ',' daily n %s",
		 DATE_PIECE,
		 TIME_PIECE,
		 VALUE_PIECE,
		 end_date_string );

	sys_string += sprintf( sys_string,
	"(								  "
	"echo \"	select distinct %s				  "
	"		from %s,%s,%s,%s				  "
	"		where %s and %s					  "
	"		order by %s;\"					 |"
	"sql.e '%c'							 |"
	"tr '%c' '%c'							 |"
	"%s								 |"
	"sed 's/,null,/,/'						 |"
	"sort								  ",
		MEASUREMENT_SELECT_LIST,
		measurement_table_name,
		station_table_name,
		fishing_trips_table_name,
		catches_species_table_name,
		measurement_where_clause,
		fishing_trips_species_where_clause,
		MEASUREMENT_SELECT_LIST,
		FOLDER_DATA_DELIMITER,
		FOLDER_DATA_DELIMITER,
		INPUT_DELIMITER,
		aggregate_process );

	sprintf( aggregate_process,
		 "real_time2aggregate_value.e sum %d %d %d ',' daily n %s",
		 DATE_PIECE,
		 TIME_PIECE,
		 VALUE_PIECE,
		 end_date_string );

	sprintf( catches_species_select_list_string,
		 CATCHES_SELECT_LIST,
		 catches_species_table_name );

	sprintf( catches_family_select_list_string,
		 CATCHES_SELECT_LIST,
		 catches_family_table_name );

	sys_string += sprintf( sys_string,
	";								  "
	"( echo \"	select %s					  "
	"		from %s,%s					  "
	"		where %s					  "
	"		order by %s;\"					 |"
	"sql.e '%c'							 |"
	"tr '%c' '%c'							  ",
		catches_species_select_list_string,
		fishing_trips_table_name,
		catches_species_table_name,
		fishing_trips_species_where_clause,
		catches_species_select_list_string,
		FOLDER_DATA_DELIMITER,
		FOLDER_DATA_DELIMITER,
		INPUT_DELIMITER );

	sys_string += sprintf( sys_string,
	";								  "
	"echo \"	select %s					  "
	"		from %s,%s					  "
	"		where %s					  "
	"		order by %s;\"					 |"
	"sql.e '%c'							 |"
	"tr '%c' '%c'							  ",
		catches_family_select_list_string,
		fishing_trips_table_name,
		catches_family_table_name,
		fishing_trips_family_where_clause,
		catches_family_select_list_string,
		FOLDER_DATA_DELIMITER,
		FOLDER_DATA_DELIMITER,
		INPUT_DELIMITER );

	sys_string += sprintf( sys_string,
	") | sort | %s							 |"
	"sed 's/,null,/,/'						 |"
	"creel_catches2minutes_between_fish %s %s %s %s '%c' %d		  ",
		aggregate_process,
		creel_application_name,
		begin_date_string,
		end_date_string,
		location_area_string,
		INPUT_DELIMITER,
		VALUE_PIECE - 1 );

	sprintf( sys_string, ") | sort | sed 's/zzzz,//' | sed 's/aaaa,//'" );
	return 1;

} /* build_sys_string() */

