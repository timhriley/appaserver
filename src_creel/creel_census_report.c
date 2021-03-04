/* ---------------------------------------------------	*/
/* src_creel/creel_census_report.c			*/
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
#include "hashtbl.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "application.h"
#include "date.h"
#include "creel_library.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */
enum output_medium { output_medium_stdout, table };

/* Constants */
/* --------- */
#define ROWS_BETWEEN_HEADING			20
#define DEFAULT_OUTPUT_MEDIUM			table

/* Prototypes */
/* ---------- */
void creel_census_report_output_table(
				char *application_name,
				char *fishing_purpose,
				char *census_date_string,
				char *interview_location,
				boolean summary_only );

int main( int argc, char **argv )
{
	char *application_name;
	char *fishing_purpose;
	char *census_date_string;
	char *interview_location;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	enum output_medium output_medium = DEFAULT_OUTPUT_MEDIUM;
	boolean summary_only;

	if ( argc != 7 )
	{
		fprintf( stderr, 
	"Usage: %s application process_name fishing_purpose census_date interview_location summary_only_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	fishing_purpose = argv[ 3 ];
	census_date_string = argv[ 4 ];
	interview_location = argv[ 5 ];
	summary_only = (*argv[ 6 ] == 'y');

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

	if ( output_medium == table )
	{

		document = document_new( "", application_name );
		document_set_output_content_type( document );
	
		document_output_head(
				document->application_name,
				document->title,
				document->output_content_type,
				appaserver_parameter_file->
					appaserver_mount_point,
				document->javascript_module_list,
				document->stylesheet_filename,
				application_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );
	
		document_output_body(
				document->application_name,
				document->onload_control_string );

		creel_census_report_output_table(
			application_name,
			fishing_purpose,
			census_date_string,
			interview_location,
			summary_only );

		document_close();
	}

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void creel_census_report_output_table(
			char *application_name,
			char *fishing_purpose,
			char *census_date_string,
			char *interview_location,
			boolean summary_only )
{
	LIST *heading_list = list_new();
	HTML_TABLE *html_table = {0};
	char title[ 512 ];
	char sub_title[ 512 ];
	FISHING_TRIPS fishing_trips;
	CATCHES catches;
	CATCH_MEASUREMENTS catch_measurements;
	CATCHES *summary_catches;
	CATCH_MEASUREMENTS *summary_catch_measurements;
	int fishing_trips_count = 0;
	int catch_records_count = 0;
	double total_fishing_hours_times_party_count = 0.0;
	double fishing_hours;
	double party_count;
	int caught;
	int total_kept = 0;
	int total_released = 0;
	int total_caught = 0;
	int first_time;
	HASH_TABLE *catches_hash_table;
	HASH_TABLE *catch_measurements_hash_table;
	LIST *key_list;
	char *key;
	char targeted_species[ 128 ];
	DATE *census_date;

	census_date =
		date_yyyy_mm_dd_new(
			census_date_string );

	catches_hash_table =
		hash_table_new( hash_table_medium );

	catch_measurements_hash_table =
		hash_table_new( hash_table_medium );

	strcpy( title, "Creel Census Report" );

	sprintf(	sub_title,
			"%s %s %s (Day = %s)",
			fishing_purpose,
			census_date_string,
			interview_location,
			date_get_day_of_week_string( census_date ) );

	format_initial_capital( sub_title, sub_title );

	html_table = new_html_table( title, sub_title );
	
	if ( !summary_only )
	{
		list_append_pointer( heading_list, "Number" );
		list_append_pointer( heading_list, "Time" );
		list_append_pointer( heading_list, "Origin" );
		list_append_pointer( heading_list, "Purpose" );
		list_append_pointer( heading_list, "Species Preferred" );
		list_append_pointer( heading_list, "Fishing Area" );
		list_append_pointer( heading_list, "Composition" );
		list_append_pointer( heading_list, "Trip (hours)" );
		list_append_pointer( heading_list, "Fishing (hours)" );
		list_append_pointer( heading_list, "Party Count" );
		list_append_pointer( heading_list, "Permit" );
	
		html_table_set_heading_list( html_table, heading_list );
		html_table->number_left_justified_columns = 99;
	}

	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	while( creel_library_get_fishing_trip(
					&fishing_trips,
					application_name,
					fishing_purpose,
					census_date_string,
					interview_location ) )
	{
		if ( !summary_only )
		{
			html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );

			html_table_set_data(
				html_table->data_list,
				strdup( fishing_trips.interview_number ) );

			html_table_set_data(
				html_table->data_list,
				strdup( fishing_trips.interview_time ) );

			html_table_set_data(
				html_table->data_list,
				strdup( fishing_trips.trip_origin_location ) );

			html_table_set_data(
				html_table->data_list,
				strdup( fishing_trips.fishing_purpose ) );

			if ( *fishing_trips.family )
			{
				sprintf( targeted_species,
				 	"%s %s %s",
				 	fishing_trips.family,
				 	fishing_trips.genus,
				 	fishing_trips.species_preferred );
			}
			else
			{
				*targeted_species = '\0';
			}

			html_table_set_data(
				html_table->data_list,
				strdup( targeted_species ) );

			html_table_set_data(
				html_table->data_list,
				strdup( fishing_trips.fishing_area ) );

			html_table_set_data(
				html_table->data_list,
				strdup( fishing_trips.
					fishing_party_composition ) );

			html_table_set_data(
				html_table->data_list,
				strdup( fishing_trips.
					fishing_trip_duration_hours ) );

			html_table_set_data(
				html_table->data_list,
				strdup( fishing_trips.
					fishing_duration_hours ) );

			html_table_set_data(
				html_table->data_list,
				strdup( fishing_trips.
					number_of_people_fishing ) );

			html_table_set_data(
				html_table->data_list,
				strdup( fishing_trips.permit_code ) );

			html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

			list_free_string_list( html_table->data_list );
			html_table->data_list = list_new_list();
		}

		fishing_hours = atof(	fishing_trips.
					fishing_duration_hours );

		party_count = atoi(	fishing_trips.
					number_of_people_fishing );

		total_fishing_hours_times_party_count +=
			( fishing_hours * party_count );

		first_time = 1;

		while(
		creel_library_census_date_get_catches_fresh_database_fetch(
				&catches,
				application_name,
				fishing_purpose,
				census_date_string,
				interview_location,
				fishing_trips.interview_number ) )
		{
			if ( !summary_only && first_time )
			{
				printf( "<tr><td colspan=99>\n" );
				printf( "<table border=0>\n" );
				printf( "<tr>\n" );
				printf( "<th align=left>Family</th>\n" );
				printf( "<th align=left>Genus</th>\n" );
				printf( "<th align=left>Species</th>\n" );
				printf( "<th align=left>Common</th>\n" );
				printf( "<th align=left>Code</th>\n" );
				printf( "<th align=left>Caught</th>\n" );
				printf( "<th align=right>Kept</th>\n" );
				printf( "<th align=right>Released</th>\n" );
				first_time = 0;
			}

			caught =
				atoi( catches.kept_count ) +
				atoi( catches.released_count );

			if ( !summary_only )
			{
				printf( "<tr>\n" );
				printf( "<td align=left>%s</td>\n",
					catches.family );
				printf( "<td align=left>%s</td>\n",
					catches.genus );
				printf( "<td align=left>%s</td>\n",
					catches.species );
				printf( "<td align=left>%s</td>\n",
					catches.common_name );
				printf( "<td align=right>%s</td>\n",
					catches.florida_state_code );
				printf( "<td align=right>%d</td>\n",
					caught );
				printf( "<td align=right>%s</td>\n",
					catches.kept_count );
				printf( "<td align=right>%s</td>\n",
					catches.released_count );
			}

			total_caught += caught;
			total_kept += atoi( catches.kept_count );
			total_released +=
				atoi( catches.released_count );
			catch_records_count++;

			summary_catches =
			creel_library_get_summary_catches(
				catches_hash_table,
				catches.family,
				catches.genus,
				catches.species,
				catches.common_name,
				catches.florida_state_code );

			summary_catches->caught_integer += caught;
			summary_catches->kept_integer +=
				atoi( catches.kept_count );
			summary_catches->released_integer +=
				atoi( catches.released_count );
		}

		if ( !summary_only && !first_time )
		{
			printf( "</table>\n" );
			printf( "</td>\n" );
		}

		fishing_trips_count++;

		first_time = 1;

		while( creel_library_get_catch_measurements(
				&catch_measurements,
				application_name,
				fishing_purpose,
				census_date_string,
				interview_location,
				fishing_trips.interview_number ) )
		{
			if ( !summary_only && first_time )
			{
				printf( "<tr><td colspan=99>\n" );
				printf( "<table border=0>\n" );
				printf( "<tr>\n" );
				printf( "<th align=left>Family</th>\n" );
				printf( "<th align=left>Genus</th>\n" );
				printf( "<th align=left>Species</th>\n" );
				printf( "<th align=left>Common</th>\n" );
				printf( "<th align=left>Code</th>\n" );
				printf( "<th align=right>Key</th>\n" );
				printf( "<th align=right>Length</th>\n" );
				first_time = 0;
			}

			if ( !summary_only )
			{
				printf( "<tr>\n" );
				printf( "<td align=left>%s</td>\n",
					catch_measurements.
						family );
				printf( "<td align=left>%s</td>\n",
					catch_measurements.
						genus );
				printf( "<td align=left>%s</td>\n",
					catch_measurements.
						species );
				printf( "<td align=left>%s</td>\n",
					catch_measurements.
					common_name );
				printf( "<td align=right>%s</td>\n",
					catch_measurements.
						florida_state_code );
				printf( "<td align=right>%s</td>\n",
					catch_measurements.
						catch_measurement_number );
				printf( "<td align=right>%s</td>\n",
					catch_measurements.
						length_millimeters );
			}

			summary_catch_measurements =
			creel_library_get_summary_catch_measurements(
				catch_measurements_hash_table,
				catch_measurements.family,
				catch_measurements.genus,
				catch_measurements.species,
				catch_measurements.common_name,
				catch_measurements.
					florida_state_code );

			summary_catch_measurements->
				length_millimeters_integer +=
					catch_measurements.
						length_millimeters_integer;
			summary_catch_measurements->
				summary_count++;
		}

		if ( !summary_only && !first_time )
		{
			printf( "</table>\n" );
			printf( "</td>\n" );
		}

	} /* while( creel_library_get_fishing_trip( &fishing_trips ) ) */

	key_list =
		hash_table_get_ordered_key_list(
			catches_hash_table );

	if ( list_rewind( key_list ) )
	{
		printf( "<tr><td colspan=99>\n" );
		printf( "<h1>Caught Species Summary</h1>\n" );
		printf( "<table border=0>\n" );
		printf( "<tr>\n" );
		printf( "<th align=left>Family</th>\n" );
		printf( "<th align=left>Genus</th>\n" );
		printf( "<th align=left>Species</th>\n" );
		printf( "<th align=left>Common</th>\n" );
		printf( "<th align=left>Code</th>\n" );
		printf( "<th align=left>Caught</th>\n" );
		printf( "<th align=right>Kept</th>\n" );
		printf( "<th align=right>Released</th>\n" );
		do {
			key = list_get_pointer( key_list );
			summary_catches =
				hash_table_get_pointer(
					catches_hash_table,
					key );

			printf( "<tr>\n" );
			printf( "<td align=left>%s</td>\n",
				summary_catches->family );
			printf( "<td align=left>%s</td>\n",
				summary_catches->genus );
			printf( "<td align=left>%s</td>\n",
				summary_catches->species );
			printf( "<td align=left>%s</td>\n",
				summary_catches->common_name );
			printf( "<td align=right>%s</td>\n",
				summary_catches->
					florida_state_code );
			printf( "<td align=right>%d</td>\n",
				summary_catches->caught_integer );
			printf( "<td align=right>%d</td>\n",
				summary_catches->kept_integer );
			printf( "<td align=right>%d</td>\n",
				summary_catches->
					released_integer );

		} while( list_next( key_list ) );
		printf( "</table>\n" );
	} /* if ( list_rewind( key_list ) ) */

	key_list =
		hash_table_get_ordered_key_list(
			catch_measurements_hash_table );

	if ( list_rewind( key_list ) )
	{
		printf( "<tr><td colspan=99>\n" );
		printf( "<h1>Measured Species Summary</h1>\n" );
		printf( "<table border=0>\n" );
		printf( "<tr>\n" );
		printf( "<th align=left>Family</th>\n" );
		printf( "<th align=left>Genus</th>\n" );
		printf( "<th align=left>Species</th>\n" );
		printf( "<th align=left>Common</th>\n" );
		printf( "<th align=left>Code</th>\n" );
		printf( "<th align=right>Count</th>\n" );
		printf( "<th align=right>Average</th>\n" );
		do {
			key = list_get_pointer( key_list );
			summary_catch_measurements =
				hash_table_get_pointer(
				catch_measurements_hash_table,
				key );

			printf( "<tr>\n" );
			printf( "<td align=left>%s</td>\n",
				summary_catch_measurements->
					family );
			printf( "<td align=left>%s</td>\n",
				summary_catch_measurements->
					genus );
			printf( "<td align=left>%s</td>\n",
				summary_catch_measurements->
					species );
			printf( "<td align=left>%s</td>\n",
				summary_catch_measurements->
					common_name );
			printf( "<td align=right>%s</td>\n",
				summary_catch_measurements->
					florida_state_code );
			printf( "<td align=right>%d</td>\n",
				summary_catch_measurements->
					summary_count );
			printf( "<td align=right>%d</td>\n",
				summary_catch_measurements->
					length_millimeters_integer /
				summary_catch_measurements->
					summary_count );

		} while( list_next( key_list ) );
		printf( "</table>\n" );
	} /* if ( list_rewind( key_list ) ) */

	printf( "<tr><td colspan=99>\n" );
	printf( "<table border=1>\n" );
	printf( "<tr>\n" );
	printf( "<th align=left></th>\n" );
	printf( "<th align=left>Caught</th>\n" );
	printf( "<th align=right>Kept</th>\n" );
	printf( "<th align=right>Released</th>\n" );
	printf( "<th align=right>Fishing Hours X Party Count</th>\n" );
	printf( "<th align=right>Unit Effort<sup>*</sup></th>\n" );
	printf( "<th align=right>Minutes Between Catch<sup>**</sup></th>\n" );
	printf( "<th align=right>Trips Count</th>\n" );
	printf( "<th align=right>Catch Records Count</th>\n" );

	printf( "<tr>\n" );
	printf( "<td>Total</td>\n" );
	printf( "<td align=right>%d</td>\n",
		total_caught );
	printf( "<td align=right>%d</td>\n",
		total_kept );
	printf( "<td align=right>%d</td>\n",
		total_released );
	printf( "<td align=right>%.2lf</td>\n",
		total_fishing_hours_times_party_count );

	if ( total_fishing_hours_times_party_count )
	{
		double unit_effort =
			(double)
			((double)total_caught /
			total_fishing_hours_times_party_count );

		printf( "<td align=right>%.2lf</td>\n",
			unit_effort );
		printf( "<td align=right>%.0lf</td>\n",
			1.0 / unit_effort * 60.0 );

	}
	else
	{
		printf( "<td align=right></td>\n" );
		printf( "<td align=right></td>\n" );
	}

	printf( "<td align=right>%d</td>\n",
		fishing_trips_count );
	printf( "<td align=right>%d</td>\n",
		catch_records_count );

	printf( "</table>\n" );

	printf(
"<tr><td colspan=99><sup>*</sup>The Unit Effort is the number of fish caught per human fishing hour. Unit Effort = Caught / (Fishing Hours X Party Count).</td>\n" );

	printf(
"<tr><td colspan=99><sup>**</sup>The Minutes Between Catch is the average number of minutes between catches. Minutes Between Catch = (1 / Unit Effort) X 60.</td>\n" );

	if ( !summary_only ) html_table_close();

} /* creel_census_report_output_table() */

