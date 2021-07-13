/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_creel/calculate_species_cpue.c	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"
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
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "creel_library.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */
enum output_medium { output_medium_stdout, spreadsheet, table };

/* Constants */
/* --------- */
#define ROWS_BETWEEN_HEADING			20
#define DEFAULT_OUTPUT_MEDIUM			table

#define FILENAME_STEM_SPECIES_DETAIL	"creel_detail_cpue"
#define FILENAME_STEM_PER_SPECIES	"creel_cpue_per_species"
#define FILENAME_STEM_SPECIES_PER_AREA	"creel_cpue_per_species_per_area"
#define FILENAME_STEM_DESTINATION	"creel_cpue_annually"

/* Prototypes */
/* ---------- */
void append_spreadsheet(		char *destination_filename,
					char *source_filename,
					char *column_a_datum );

char *get_summary_heading(		char *catch_harvest,
					char *exclude_zero_catches_label );

char *get_per_species_output_filename(
					char *document_root_directory,
					char *application_name,
					char *begin_date_string,
					char *end_date_string,
					pid_t process_id,
					char *filename_stem );

void calculate_species_cpue_group_monthly(
					char *application_name,
					int begin_year,
					int end_year,
					char *fishing_area_list_string,
					char *family_list_string,
					char *genus_list_string,
					char *species_list_string,
					char *fishing_purpose,
					char *catch_harvest,
					boolean exclude_zero_catches,
					char *exclude_zero_catches_label,
					char *document_root_directory,
					char *process_name,
					char *appaserver_data_directory );

void calculate_species_cpue_group_annually(
					char *application_name,
					int begin_year,
					int end_year,
					char *fishing_area_list_string,
					char *family_list_string,
					char *genus_list_string,
					char *species_list_string,
					char *fishing_purpose,
					char *catch_harvest,
					boolean exclude_zero_catches,
					char *exclude_zero_catches_label,
					char *document_root_directory,
					char *process_name,
					char *appaserver_data_directory );

void calculate_species_cpue(		char *application_name,
					char *begin_date_string,
					char *end_date_string,
					char *fishing_area_list_string,
					char *family_list_string,
					char *genus_list_string,
					char *species_list_string,
					boolean summary_only,
					char *fishing_purpose,
					char *catch_harvest,
					boolean exclude_zero_catches,
					char *exclude_zero_catches_label,
					enum output_medium output_medium,
					char *document_root_directory,
					char *process_name,
					char *appaserver_data_directory );

void creel_catches_report_catch_per_unit_effort_spreadsheet(
					char *application_name,
					char *begin_date_string,
					char *end_date_string,
					char *fishing_area_list_string,
					char *family_list_string,
					char *genus_list_string,
					char *species_list_string,
					char *document_root_directory,
					boolean summary_only,
					char *fishing_purpose,
					char *sub_title,
					char *catch_harvest,
					boolean exclude_zero_catches,
					char *exclude_zero_catches_label,
					boolean omit_links,
					char *appaserver_data_directory );

CATCH_PER_UNIT_EFFORT *get_catch_per_unit_effort(
					char *application_name,
					char *begin_date_string,
					char *end_date_string,
					char *fishing_area_list_string,
					char *family_list_string,
					char *genus_list_string,
					char *species_list_string,
					boolean summary_only,
					char *fishing_purpose,
					char *catch_harvest,
					boolean exclude_zero_catches,
					char *appaserver_data_directory );

void creel_catches_report_catch_per_unit_effort_output_table(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *fishing_area_list_string,
				char *family_list_string,
				char *genus_list_string,
				char *species_list_string,
				boolean summary_only,
				char *fishing_purpose,
				char *catch_harvest,
				boolean exclude_zero_catches,
				char *exclude_zero_catches_label,
				char *appaserver_data_directory );

void get_title_and_sub_title(
				char *title,
				char *sub_title,
				char *begin_date_string,
				char *end_date_string,
				char *process_name,
				char *fishing_purpose );

enum output_medium get_output_medium(
				char *output_medium_string );

int main( int argc, char **argv )
{
	char *application_name;
	char *fishing_area_list_string;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	enum output_medium output_medium;
	char *fishing_purpose;
	boolean summary_only;
	char *begin_date_string;
	char *end_date_string;
	char *family_list_string;
	char *genus_list_string;
	char *species_list_string;
	char *catch_harvest;
	boolean exclude_zero_catches;
	char *aggregate_period;
	char *exclude_zero_catches_label;
	char *output_medium_string;

	if ( argc != 15 )
	{
		fprintf( stderr,
"Usage: %s application process_name fishing_area begin_date end_date family genus species summary_only_yn fishing_purpose catch_harvest exclude_zero_catches_yn aggregate_period output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	fishing_area_list_string = argv[ 3 ];
	begin_date_string = argv[ 4 ];
	end_date_string = argv[ 5 ];

	if ( !*end_date_string || strcmp( end_date_string, "end_date" ) == 0 )
		end_date_string = begin_date_string;

	family_list_string = argv[ 6 ];
	genus_list_string = argv[ 7 ];
	species_list_string = argv[ 8 ];
	summary_only = (*argv[ 9 ] == 'y');
	fishing_purpose = argv[ 10 ];

	catch_harvest = argv[ 11 ];

	if ( !*catch_harvest
	||   timlib_strcmp( catch_harvest, "catch_harvest" ) == 0 )
	{
		catch_harvest = "cpue";
	}

	exclude_zero_catches = (*argv[ 12 ] == 'y');

	if ( exclude_zero_catches )
		exclude_zero_catches_label = "(Excluding Zero Catches)";
	else
		exclude_zero_catches_label = "(Including Zero Catches)";

	aggregate_period = argv[ 13 ];
	output_medium_string = argv[ 14 ];

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

	output_medium = get_output_medium( output_medium_string );

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

	if ( !*family_list_string
	||   strcmp( family_list_string, "select" ) == 0
	||   strcmp( family_list_string, "family" ) == 0 )
	{
		printf( "<h3>Please select a species.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( strcmp( aggregate_period, "annually" ) == 0 )
	{
		calculate_species_cpue_group_annually(
			application_name,
			atoi( begin_date_string ),
			atoi( end_date_string ),
			fishing_area_list_string,
			family_list_string,
			genus_list_string,
			species_list_string,
			fishing_purpose,
			catch_harvest,
			exclude_zero_catches,
			exclude_zero_catches_label,
			appaserver_parameter_file->
				document_root,
			process_name,
			appaserver_parameter_file->
				appaserver_data_directory );
	}
	else
	if ( strcmp( aggregate_period, "monthly" ) == 0 )
	{
		calculate_species_cpue_group_monthly(
				application_name,
				atoi( begin_date_string ),
				atoi( end_date_string ),
				fishing_area_list_string,
				family_list_string,
				genus_list_string,
				species_list_string,
				fishing_purpose,
				catch_harvest,
				exclude_zero_catches,
				exclude_zero_catches_label,
				appaserver_parameter_file->
					document_root,
				process_name,
				appaserver_parameter_file->
					appaserver_data_directory );
	}
	else
	{
		calculate_species_cpue(
				application_name,
				begin_date_string,
				end_date_string,
				fishing_area_list_string,
				family_list_string,
				genus_list_string,
				species_list_string,
				summary_only,
				fishing_purpose,
				catch_harvest,
				exclude_zero_catches,
				exclude_zero_catches_label,
				output_medium,
				appaserver_parameter_file->
					document_root,
				process_name,
				appaserver_parameter_file->
					appaserver_data_directory );
	}

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void calculate_species_cpue_group_annually(
				char *application_name,
				int begin_year,
				int end_year,
				char *fishing_area_list_string,
				char *family_list_string,
				char *genus_list_string,
				char *species_list_string,
				char *fishing_purpose,
				char *catch_harvest,
				boolean exclude_zero_catches,
				char *exclude_zero_catches_label,
				char *document_root_directory,
				char *process_name,
				char *appaserver_data_directory )
{
	char title[ 512 ];
	char sub_title[ 512 ];
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];
	pid_t process_id = getpid();
	int year;
	char *spreadsheet_cpue_per_species_source_filename;
	char *spreadsheet_cpue_per_species_destination_filename;
	char *ftp_summary_filename;
	FILE *output_file;
	char column_a_datum[ 16 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM_PER_SPECIES,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	sprintf( begin_date_string,
		 "%d-01-01",
		 begin_year );

	sprintf( end_date_string,
		 "%d-12-31",
		 end_year );

	get_title_and_sub_title(
			title,
			sub_title,
			begin_date_string,
			end_date_string,
			process_name,
			fishing_purpose );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	system( "/bin/date '+%m/%d/%Y %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

	if ( begin_year > end_year ) return;

	spreadsheet_cpue_per_species_destination_filename =
		get_per_species_output_filename(
				document_root_directory,
				application_name,
				begin_date_string,
				end_date_string,
				process_id,
				FILENAME_STEM_DESTINATION );

	if ( ! ( output_file =
			fopen(
			spreadsheet_cpue_per_species_destination_filename,
			"w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			spreadsheet_cpue_per_species_destination_filename );
		document_close();
		exit( 1 );
	}
	else
	{
		fprintf( output_file,
			 "Year,%s\n",
			 search_replace_character(
			 	get_summary_heading(
					catch_harvest,
					exclude_zero_catches_label ),
				'|' /* search_character */, 
				',' /* replace_character */ ) );

		fclose( output_file );
	}

	appaserver_link_file->begin_date_string = begin_date_string;
	appaserver_link_file->end_date_string = end_date_string;

	ftp_summary_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			FILENAME_STEM_DESTINATION,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	for (	year = begin_year;
		year <= end_year;
		year++ )
	{
		sprintf( begin_date_string,
			 "%d-01-01",
			 year );

		sprintf( end_date_string,
			 "%d-12-31",
			 year );

		creel_catches_report_catch_per_unit_effort_spreadsheet(
			application_name,
			begin_date_string,
			end_date_string,
			fishing_area_list_string,
			family_list_string,
			genus_list_string,
			species_list_string,
			document_root_directory,
			1 /* summary_only */,
			fishing_purpose,
			(char *)0 /* sub_title */,
			catch_harvest,
			exclude_zero_catches,
			exclude_zero_catches_label,
			1 /* omit_links */,
			appaserver_data_directory );

		spreadsheet_cpue_per_species_source_filename =
			get_per_species_output_filename(
				document_root_directory,
				application_name,
				begin_date_string,
				end_date_string,
				process_id,
				FILENAME_STEM_PER_SPECIES );

		sprintf( column_a_datum,
			 "%d",
			 year );

		append_spreadsheet(
			spreadsheet_cpue_per_species_destination_filename,
			spreadsheet_cpue_per_species_source_filename,
			column_a_datum );
	}

	printf( "<br>\n" );

	if ( timlib_strcmp( catch_harvest, "cpue" ) == 0 )
	{
		appaserver_library_output_ftp_prompt(
			ftp_summary_filename,
"Species CPUE Per Species: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );
	}
	else
	{
		appaserver_library_output_ftp_prompt(
			ftp_summary_filename,
"Species HPUE Per Species: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );
	}

} /* calculate_species_cpue_group_annually() */

void calculate_species_cpue_group_monthly(
				char *application_name,
				int begin_year,
				int end_year,
				char *fishing_area_list_string,
				char *family_list_string,
				char *genus_list_string,
				char *species_list_string,
				char *fishing_purpose,
				char *catch_harvest,
				boolean exclude_zero_catches,
				char *exclude_zero_catches_label,
				char *document_root_directory,
				char *process_name,
				char *appaserver_data_directory )
{
	char title[ 512 ];
	char sub_title[ 512 ];
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];
	pid_t process_id = getpid();
	int year;
	int month;
	char *spreadsheet_cpue_per_species_source_filename;
	char *spreadsheet_cpue_per_species_destination_filename;
	char *ftp_summary_filename;
	FILE *output_file;
	char column_a_datum[ 16 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM_PER_SPECIES,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	sprintf( begin_date_string,
		 "%d-01-01",
		 begin_year );

	sprintf( end_date_string,
		 "%d-12-31",
		 end_year );

	get_title_and_sub_title(
			title,
			sub_title,
			begin_date_string,
			end_date_string,
			process_name,
			fishing_purpose );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	system( "/bin/date '+%m/%d/%Y %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

	if ( begin_year > end_year ) return;

	spreadsheet_cpue_per_species_destination_filename =
		get_per_species_output_filename(
				document_root_directory,
				application_name,
				begin_date_string,
				end_date_string,
				process_id,
				FILENAME_STEM_PER_SPECIES );

	if ( ! ( output_file =
			fopen(
			spreadsheet_cpue_per_species_destination_filename,
			"w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			spreadsheet_cpue_per_species_destination_filename );
		document_close();
		exit( 1 );
	}
	else
	{
		fprintf( output_file,
			 "Month,%s\n",
			 search_replace_character(
			 	get_summary_heading(
					catch_harvest,
					exclude_zero_catches_label ),
				'|' /* search_character */, 
				',' /* replace_character */ ) );

		fclose( output_file );
	}

	appaserver_link_file->begin_date_string = begin_date_string;
	appaserver_link_file->end_date_string = end_date_string;

	ftp_summary_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	for (	year = begin_year;
		year <= end_year;
		year++ )
	{
		for(	month = 1;
			month <= 12;
			month++ )
		{

			sprintf(begin_date_string,
			 	"%d-%.2d-01",
			 	year,
				month );

			sprintf(end_date_string,
			 	"%d-%.2d-%d",
			 	year,
				month,
				date_days_in_month(
					month,
					year ) );

			creel_catches_report_catch_per_unit_effort_spreadsheet(
				application_name,
				begin_date_string,
				end_date_string,
				fishing_area_list_string,
				family_list_string,
				genus_list_string,
				species_list_string,
				document_root_directory,
				1 /* summary_only */,
				fishing_purpose,
				(char *)0 /* sub_title */,
				catch_harvest,
				exclude_zero_catches,
				exclude_zero_catches_label,
				1 /* omit_links */,
				appaserver_data_directory );

			spreadsheet_cpue_per_species_source_filename =
				get_per_species_output_filename(
					document_root_directory,
					application_name,
					begin_date_string,
					end_date_string,
					process_id,
					FILENAME_STEM_PER_SPECIES );

			sprintf(column_a_datum,
			 	"%s/%d",
				timlib_get_three_character_month_string(
					month - 1 ),
			 	year );

			append_spreadsheet(
			spreadsheet_cpue_per_species_destination_filename,
			spreadsheet_cpue_per_species_source_filename,
			column_a_datum );
		}
	}

	printf( "<br>\n" );

	if ( timlib_strcmp( catch_harvest, "cpue" ) == 0 )
	{
		appaserver_library_output_ftp_prompt(
			ftp_summary_filename,
"Species CPUE Per Species: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );
	}
	else
	{
		appaserver_library_output_ftp_prompt(
			ftp_summary_filename,
"Species HPUE Per Species: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );
	}

} /* calculate_species_cpue_group_monthly() */

void calculate_species_cpue(	char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *fishing_area_list_string,
				char *family_list_string,
				char *genus_list_string,
				char *species_list_string,
				boolean summary_only,
				char *fishing_purpose,
				char *catch_harvest,
				boolean exclude_zero_catches,
				char *exclude_zero_catches_label,
				enum output_medium output_medium,
				char *document_root_directory,
				char *process_name,
				char *appaserver_data_directory )
{
	char title[ 512 ];
	char sub_title[ 512 ];

	get_title_and_sub_title(
			title,
			sub_title,
			begin_date_string,
			end_date_string,
			process_name,
			fishing_purpose );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	system( "/bin/date '+%m/%d/%Y %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

	if ( output_medium == table )
	{
		creel_catches_report_catch_per_unit_effort_output_table(
			application_name,
			begin_date_string,
			end_date_string,
			fishing_area_list_string,
			family_list_string,
			genus_list_string,
			species_list_string,
			summary_only,
			fishing_purpose,
			catch_harvest,
			exclude_zero_catches,
			exclude_zero_catches_label,
			appaserver_data_directory );

	}
	else
	if ( output_medium == spreadsheet )
	{
		creel_catches_report_catch_per_unit_effort_spreadsheet(
			application_name,
			begin_date_string,
			end_date_string,
			fishing_area_list_string,
			family_list_string,
			genus_list_string,
			species_list_string,
			document_root_directory,
			summary_only,
			fishing_purpose,
			sub_title,
			catch_harvest,
			exclude_zero_catches,
			exclude_zero_catches_label,
			0 /* not omit_links */,
			appaserver_data_directory );
	}

} /* calculate_species_cpue() */

enum output_medium get_output_medium( char *output_medium_string )
{
	if ( strcmp( output_medium_string, "output_medium" ) == 0
	||   !*output_medium_string )
	{
		return DEFAULT_OUTPUT_MEDIUM;
	}
	else
	if ( strcmp( output_medium_string, "text_file" ) == 0
	||   strcmp( output_medium_string, "transmit" ) == 0
	||   strcmp( output_medium_string, "spreadsheet" ) == 0 )
	{
		return spreadsheet;
	}
	else
	if ( strcmp( output_medium_string, "table" ) == 0 )
	{
		return table;
	}
	else
	if ( strcmp( output_medium_string, "stdout" ) == 0 )
	{
		return output_medium_stdout;
	}
	else
	{
		return DEFAULT_OUTPUT_MEDIUM;
	}
} /* get_output_medium_string() */

CATCH_PER_UNIT_EFFORT *get_catch_per_unit_effort(
					char *application_name,
					char *begin_date_string,
					char *end_date_string,
					char *fishing_area_list_string,
					char *family_list_string,
					char *genus_list_string,
					char *species_list_string,
					boolean summary_only,
					char *fishing_purpose,
					char *catch_harvest,
					boolean exclude_zero_catches,
					char *appaserver_data_directory )
{
	CATCH_PER_UNIT_EFFORT *catch_per_unit_effort;

	catch_per_unit_effort =
		creel_library_catch_per_unit_effort_new();

	catch_per_unit_effort->fishing_trip_list =
		creel_library_get_CPUE_fishing_trip_list(
					application_name,
					begin_date_string,
					end_date_string,
					fishing_area_list_string,
					family_list_string,
					genus_list_string,
					species_list_string,
					fishing_purpose,
					catch_harvest,
					exclude_zero_catches );

	creel_library_get_CPUE_results_list(
		catch_per_unit_effort->results_list,
		catch_per_unit_effort->results_per_species_per_area_list,
		catch_per_unit_effort->results_per_species_list,
		catch_per_unit_effort->fishing_trip_list,
		family_list_string,
		genus_list_string,
		species_list_string,
		summary_only,
		appaserver_data_directory );

	creel_library_close_statistics_pipes(
			catch_per_unit_effort->
			results_per_species_per_area_list );

	creel_library_close_statistics_pipes(
			catch_per_unit_effort->
			results_per_species_list );

	return catch_per_unit_effort;

} /* get_catch_per_unit_effort() */

void creel_catches_report_catch_per_unit_effort_output_table(
					char *application_name,
					char *begin_date_string,
					char *end_date_string,
					char *fishing_area_list_string,
					char *family_list_string,
					char *genus_list_string,
					char *species_list_string,
					boolean summary_only,
					char *fishing_purpose,
					char *catch_harvest,
					boolean exclude_zero_catches,
					char *exclude_zero_catches_label,
					char *appaserver_data_directory )
{
	CATCH_PER_UNIT_EFFORT *catch_per_unit_effort;
	LIST *heading_list;
	CATCH_PER_UNIT_EFFORT_SPECIES *species;
	char buffer[ 128 ];
	double cpue_mean = 0.0;
	double cpue_standard_deviation = 0.0;
	double cpue_standard_error_of_mean = 0.0;
	int cpue_sample_size = 0;
	HTML_TABLE *html_table = {0};
	int count;
	char cpue_heading[ 256 ];

	catch_per_unit_effort =
		get_catch_per_unit_effort(
			application_name,
			begin_date_string,
			end_date_string,
			fishing_area_list_string,
			family_list_string,
			genus_list_string,
			species_list_string,
			summary_only,
			fishing_purpose,
			catch_harvest,
			exclude_zero_catches,
			appaserver_data_directory );

	if ( !summary_only )
	{
		FILE *output_pipe;
		char sys_string[ 1024 ];
		char *heading_column_list_string;
		char *justify_list_string;
		char *title_sub_title;

		if ( timlib_strcmp( catch_harvest, "cpue" ) == 0 )
		{
			heading_column_list_string =
"Census Date,Fishing Purpose,Interview Location,Interview Number,Fishing Area,Species Preferred,Species Caught,Hours Fishing,Party Count,Catches,Effort Hours";
		}
		else
		{
			heading_column_list_string =
"Census Date,Fishing Purpose,Interview Location,Interview Number,Fishing Area,Species Preferred,Species Caught,Hours Fishing,Party Count,Kept,Effort Hours";
		}

		justify_list_string =
"left,left,left,right,left,left,left,right,right,right,right";

		title_sub_title = "^Detail";

		sprintf( sys_string,
			 "html_table.e '%s' '%s' '|' '%s'",
			 title_sub_title,
			 heading_column_list_string,
			 justify_list_string );

		fflush( stdout );
		output_pipe = popen( sys_string, "w" );

		if ( list_rewind( catch_per_unit_effort->results_list ) )
		{
			do {
				species =
					list_get_pointer(
						catch_per_unit_effort->
						results_list );

				fprintf( output_pipe,
			"%s|%s|%s|%d|%s|%s %s %s|%s %s %s|%d|%d|%d|%d\n",
					 species->census_date,
					 species->fishing_purpose,
					 species->interview_location,
					 species->interview_number,
					 species->fishing_area,
			 		 species->family_preferred,
					 species->genus_preferred,
					 species->species_preferred,
			 		 species->family_caught,
					 species->genus_caught,
					 species->species_caught,
			 		 species->hours_fishing,
			 		 species->party_count,
			 		 species->catches,
			 		 species->effort );

			} while( list_next(
				catch_per_unit_effort->results_list ) );
		}
		pclose( output_pipe );
		fflush( stdout );
	} /* if not summary_only */

	/* Per Species Per Area */
	/* -------------------- */
	html_table = new_html_table(
		"", "Per Species Per Area" );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	heading_list = list_new();
	list_append_pointer( heading_list, "Family" );
	list_append_pointer( heading_list, "Genus" );
	list_append_pointer( heading_list, "Species" );
	list_append_pointer( heading_list, "Fishing Area" );

	if ( timlib_strcmp( catch_harvest, "cpue" ) == 0 )
	{
		list_append_pointer( heading_list, "Sum(Catches)" );
	}
	else
	{
		list_append_pointer( heading_list, "Sum(Kept)" );
	}

	list_append_pointer( heading_list, "Sum(Effort Hours)" );
	list_append_pointer( heading_list, "Fishing Trip Count" );

	if ( timlib_strcmp( catch_harvest, "cpue" ) == 0 )
	{
		sprintf( cpue_heading,
		"CPUE Weighted Effort %s:<br>Sum(Catches) / <nobr>Sum(Effort Hours)</nobr>",
			 exclude_zero_catches_label );
		list_append_pointer( heading_list, strdup( cpue_heading ) );

		sprintf( cpue_heading,
		"CPUE Non-weighted Effort %s:<br>Sum(Catches/Effort Hours)</nobr> / Fishing Trip Count",
			 exclude_zero_catches_label );
		list_append_pointer( heading_list, strdup( cpue_heading ) );

		list_append_pointer(	heading_list,
					"CPUE Std. Dev." );
		list_append_pointer(	heading_list,
					"CPUE Std. Err. Mean" );
	}
	else
	{
		sprintf( cpue_heading,
		"HPUE Weighted Effort %s:<br>Sum(Kept) / <nobr>Sum(Effort Hours)</nobr>",
			 exclude_zero_catches_label );
		list_append_pointer( heading_list, strdup( cpue_heading ) );

		sprintf( cpue_heading,
		"HPUE Non-weighted Effort %s:<br>Sum(Kept/Effort Hours)</nobr> / Fishing Trip Count",
			 exclude_zero_catches_label );
		list_append_pointer( heading_list, strdup( cpue_heading ) );

		list_append_pointer(	heading_list,
					"HPUE Std. Dev." );
		list_append_pointer(	heading_list,
					"HPUE Std. Err. Mean" );
	}
		
	html_table_set_heading_list( html_table, heading_list );
	
	html_table->number_left_justified_columns = 4;
	html_table->number_right_justified_columns = 99;

	html_table_output_data_heading(
			html_table->heading_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->justify_list );

	if ( !list_rewind( catch_per_unit_effort->
				results_per_species_per_area_list ) )
	{
		return;
	}

	count = 0;
	do {
		if ( !(++count % ROWS_BETWEEN_HEADING ) )
		{
			html_table_output_data_heading(
				html_table->heading_list,
				html_table->
					number_left_justified_columns,
				html_table->
					number_right_justified_columns,
				html_table->justify_list );
		}

		species = list_get_pointer( 
			catch_per_unit_effort->
				results_per_species_per_area_list );

		html_table_set_data(
			html_table->data_list,
			strdup( species->family_caught ) );

		html_table_set_data(
			html_table->data_list,
			strdup( species->genus_caught ) );

		html_table_set_data(
			html_table->data_list,
			strdup( species->species_caught ) );

		html_table_set_data(
			html_table->data_list,
			strdup( species->fishing_area ) );

		sprintf( buffer,
			 "%d",
			 species->catches );
		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		sprintf( buffer,
			 "%d",
			 species->effort );
		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		creel_library_fetch_species_summary_statistics_filename(
				&cpue_mean,
				&cpue_standard_deviation,
				&cpue_standard_error_of_mean,
				&cpue_sample_size,
				species->statistics_filename );

		sprintf( buffer, "%d", cpue_sample_size );
		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		if ( species->effort )
		{
			sprintf( buffer,
				 "%.4lf",
				 (double)species->catches /
				 (double)species->effort );
		}
		else
		{
			strcpy( buffer, "0.0" );
		}
		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		sprintf( buffer, "%.4lf", cpue_mean );
		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		sprintf( buffer, "%.4lf", cpue_standard_deviation );
		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		sprintf( buffer, "%.4lf", cpue_standard_error_of_mean );
		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new_list();
	} while ( list_next( catch_per_unit_effort->
				results_per_species_per_area_list ) );

	html_table_close();

	/* Per Species */
	/* ----------- */
	html_table = new_html_table(
		"", "Per Species" );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	heading_list = list_new();
	list_append_pointer( heading_list, "Family" );
	list_append_pointer( heading_list, "Genus" );
	list_append_pointer( heading_list, "Species" );

	if ( timlib_strcmp( catch_harvest, "cpue" ) == 0 )
	{
		list_append_pointer( heading_list, "Sum(Catches)" );
	}
	else
	{
		list_append_pointer( heading_list, "Sum(Kept)" );
	}

	list_append_pointer( heading_list, "Sum(Effort Hours)" );
	list_append_pointer( heading_list, "Fishing Trip Count" );

	if ( timlib_strcmp( catch_harvest, "cpue" ) == 0 )
	{
		sprintf( cpue_heading,
"CPUE Weighted Effort %s:<br>Sum(Catches) / <nobr>Sum(Effort Hours)</nobr>",
			 exclude_zero_catches_label );
		list_append_pointer( heading_list, strdup( cpue_heading ) );

		sprintf( cpue_heading,
"CPUE Non-weighted Effort %s:<br>Sum(Catches/<nobr>Effort Hours</nobr>) / Fishing Trip Count",
			 exclude_zero_catches_label );
		list_append_pointer( heading_list, strdup( cpue_heading ) );

		list_append_pointer(	heading_list,
					"CPUE Std. Dev." );
		list_append_pointer(	heading_list,
					"CPUE Std. Err. Mean" );
	}
	else
	{
		sprintf( cpue_heading,
"HPUE Weighted Effort %s:<br>Sum(Kept) / <nobr>Sum(Effort Hours)</nobr>",
			 exclude_zero_catches_label );
		list_append_pointer( heading_list, strdup( cpue_heading ) );

		sprintf( cpue_heading,
"HPUE Non-weighted Effort %s:<br>Sum(Kept/<nobr>Effort Hours</nobr>) / Fishing Trip Count",
			 exclude_zero_catches_label );
		list_append_pointer( heading_list, strdup( cpue_heading ) );

		list_append_pointer(	heading_list,
					"HPUE Std. Dev." );
		list_append_pointer(	heading_list,
					"HPUE Std. Err. Mean" );
	}
		
	html_table_set_heading_list( html_table, heading_list );
	
	html_table->number_left_justified_columns = 3;
	html_table->number_right_justified_columns = 99;

	html_table_output_data_heading(
			html_table->heading_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->justify_list );

	if ( !list_rewind( catch_per_unit_effort->
				results_per_species_list ) )
	{
		return;
	}

	count = 0;
	do {
		if ( !(++count % ROWS_BETWEEN_HEADING ) )
		{
			html_table_output_data_heading(
				html_table->heading_list,
				html_table->
					number_left_justified_columns,
				html_table->
					number_right_justified_columns,
				html_table->justify_list );
		}

		species = list_get_pointer( 
			catch_per_unit_effort->
				results_per_species_list );

		creel_library_fetch_species_summary_statistics_filename(
				&cpue_mean,
				&cpue_standard_deviation,
				&cpue_standard_error_of_mean,
				&cpue_sample_size,
				species->statistics_filename );

		html_table_set_data(
			html_table->data_list,
			strdup( species->family_caught ) );

		html_table_set_data(
			html_table->data_list,
			strdup( species->genus_caught ) );

		html_table_set_data(
			html_table->data_list,
			strdup( species->species_caught ) );

		sprintf( buffer,
			 "%d",
			 species->catches );
		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		sprintf( buffer,
			 "%d",
			 species->effort );
		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		sprintf( buffer,
			 "%d",
			 cpue_sample_size );
		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		if ( species->effort )
		{
			sprintf( buffer,
				 "%.4lf",
				 (double)species->catches /
				 (double)species->effort );
		}
		else
		{
			strcpy( buffer, "0.0" );
		}
		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		sprintf( buffer, "%.4lf", cpue_mean );
		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		sprintf( buffer, "%.4lf", cpue_standard_deviation );
		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		sprintf( buffer, "%.4lf", cpue_standard_error_of_mean );
		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new_list();
	} while ( list_next( catch_per_unit_effort->
				results_per_species_list ) );

	html_table_close();

	creel_library_remove_statistics_files(
				catch_per_unit_effort->
					results_per_species_per_area_list );

	creel_library_remove_statistics_files(
				catch_per_unit_effort->
					results_per_species_list );

} /* creel_catches_report_catch_per_unit_effort_output_table() */

void creel_catches_report_catch_per_unit_effort_spreadsheet(
					char *application_name,
					char *begin_date_string,
					char *end_date_string,
					char *fishing_area_list_string,
					char *family_list_string,
					char *genus_list_string,
					char *species_list_string,
					char *document_root_directory,
					boolean summary_only,
					char *fishing_purpose,
					char *sub_title,
					char *catch_harvest,
					boolean exclude_zero_catches,
					char *exclude_zero_catches_label,
					boolean omit_links,
					char *appaserver_data_directory )
{
	CATCH_PER_UNIT_EFFORT *catch_per_unit_effort;
	CATCH_PER_UNIT_EFFORT_SPECIES *species;
	char *output_filename = {0};
	char *ftp_detail_filename = {0};
	char *ftp_per_species_per_area_filename = {0};
	char *ftp_per_species_filename = {0};
	pid_t process_id = getpid();
	char output_sys_string[ 1024 ];
	FILE *output_file;
	FILE *output_pipe;
	double cpue_mean = 0.0;
	double cpue_standard_deviation = 0.0;
	double cpue_standard_error_of_mean = 0.0;
	int cpue_sample_size = 0;
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			(char *)0 /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	appaserver_link_file->begin_date_string = begin_date_string;
	appaserver_link_file->end_date_string = end_date_string;

	catch_per_unit_effort =
		get_catch_per_unit_effort(
			application_name,
			begin_date_string,
			end_date_string,
			fishing_area_list_string,
			family_list_string,
			genus_list_string,
			species_list_string,
			summary_only,
			fishing_purpose,
			catch_harvest,
			exclude_zero_catches,
			appaserver_data_directory );

	if ( !summary_only )
	{
		appaserver_link_file->filename_stem =
			FILENAME_STEM_SPECIES_DETAIL;

		output_filename =
			appaserver_link_get_output_filename(
				appaserver_link_file->
					output_file->
					document_root_directory,
				appaserver_link_file->application_name,
				appaserver_link_file->filename_stem,
				appaserver_link_file->begin_date_string,
				appaserver_link_file->end_date_string,
				appaserver_link_file->process_id,
				appaserver_link_file->session,
				appaserver_link_file->extension );

		if ( ! ( output_file = fopen( output_filename, "w" ) ) )
		{
			printf( "<H2>ERROR: Cannot open output file %s\n",
				output_filename );
			document_close();
			exit( 1 );
		}
		else
		{
			fclose( output_file );
		}

		ftp_detail_filename =
			appaserver_link_get_link_prompt(
				appaserver_link_file->
					link_prompt->
					prepend_http_boolean,
				appaserver_link_file->
					link_prompt->
					http_prefix,
				appaserver_link_file->
					link_prompt->server_address,
				appaserver_link_file->application_name,
				appaserver_link_file->filename_stem,
				appaserver_link_file->begin_date_string,
				appaserver_link_file->end_date_string,
				appaserver_link_file->process_id,
				appaserver_link_file->session,
				appaserver_link_file->extension );

		sprintf(output_sys_string,
		 	"tr '|' ',' > %s",
		 	output_filename );
		output_pipe = popen( output_sys_string, "w" );

		if ( sub_title )
		{
			if ( timlib_strcmp( catch_harvest, "cpue" ) == 0 )
			{
				fprintf( output_pipe,
		 	 		"Species CPUE Detail\n%s\n",
		 	 		sub_title );
			}
			else
			{
				fprintf( output_pipe,
		 	 		"Species HPUE Detail\n%s\n",
		 	 		sub_title );
			}
		}

		if ( timlib_strcmp( catch_harvest, "cpue" ) == 0 )
		{
			fprintf( output_pipe,
"Date|Purpose|Location|Interview|FishingArea|Preferred|Caught|Hours Fishing|Party Count|Catches|Effort Hours\n" );
		}
		else
		{
			fprintf( output_pipe,
"Date|Purpose|Location|Interview|FishingArea|Preferred|Caught|Hours Fishing|Party Count|Kept|Effort Hours\n" );
		}

		pclose( output_pipe );

		sprintf(output_sys_string,
	 		"tr '|' ',' >> %s",
	 		output_filename );

		output_pipe = popen( output_sys_string, "w" );

		if ( list_rewind( catch_per_unit_effort->
					results_list ) )
		{
			do {
				species = list_get_pointer(
						catch_per_unit_effort->
						results_list );

				fprintf( output_pipe,
"%s|%s|%s|%d|%s|%s %s %s|%s %s %s|%d|%d|%d|%d\n",
			 		species->census_date,
			 		species->fishing_purpose,
			 		species->interview_location,
			 		species->interview_number,
			 		species->fishing_area,
			 		species->family_preferred,
			 		species->genus_preferred,
			 		species->species_preferred,
			 		species->family_caught,
			 		species->genus_caught,
			 		species->species_caught,
			 		species->hours_fishing,
			 		species->party_count,
			 		species->catches,
			 		species->effort );

			} while( list_next(
					catch_per_unit_effort->results_list ) );
		}

		pclose( output_pipe );

		if ( !omit_links )
		{
			printf( "<br>\n" );

			if ( timlib_strcmp( catch_harvest, "cpue" ) == 0 )
			{
				appaserver_library_output_ftp_prompt(
					ftp_detail_filename,
"Species CPUE Detail: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
					(char *)0 /* target */,
					(char *)0 /* application_type */ );
			}
			else
			{
				appaserver_library_output_ftp_prompt(
					ftp_detail_filename,
"Species HPUE Detail: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
					(char *)0 /* target */,
					(char *)0 /* application_type */ );
			}
		}
	} /* if (!summary_only ) */

	/* Output CPUE Per Species Per Area */
	/* -------------------------------- */
	appaserver_link_file->filename_stem =
		FILENAME_STEM_SPECIES_PER_AREA;

	output_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			output_filename );
		document_close();
		exit( 1 );
	}
	else
	{
		fclose( output_file );
	}

	sprintf( output_sys_string,
		 "tr '|' ',' > %s",
		 output_filename );

	ftp_per_species_per_area_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	output_pipe = popen( output_sys_string, "w" );

	if ( sub_title )
	{
		if ( timlib_strcmp( catch_harvest, "cpue" ) == 0 )
		{
			fprintf( output_pipe,
		 	 	"Species CPUE Per Species Per Area\n%s\n",
		 	 	sub_title );
		}
		else
		{
			fprintf( output_pipe,
		 	 	"Species HPUE Per Species Per Area\n%s\n",
		 	 	sub_title );
		}
	}

	if ( timlib_strcmp( catch_harvest, "cpue" ) == 0 )
	{
		fprintf( output_pipe,
"Family|Genus|Species|Fishing Area|Sum(Catches)|Sum(Effort Hours)|Fishing Trip Count|CPUE Weighted Effort %s|CPUE Non-Weighted Effort %s|CPUE Std. Dev.|CPUE Std. Err. Mean\n",
			 exclude_zero_catches_label,
			 exclude_zero_catches_label );
	}
	else
	{
		fprintf( output_pipe,
"Family|Genus|Species|Fishing Area|Sum(Kept)|Sum(Effort Hours)|Fishing Trip Count|HPUE Weighted Effort %s|HPUE Non-Weighted Effort %s|HPUE Std. Dev.|HPUE Std. Err. Mean\n",
			 exclude_zero_catches_label,
			 exclude_zero_catches_label );
	}

	if ( list_rewind( catch_per_unit_effort->
		results_per_species_per_area_list ) )
	{
		do {
			species = list_get_pointer( 
					catch_per_unit_effort->
					results_per_species_per_area_list );

			creel_library_fetch_species_summary_statistics_filename(
				&cpue_mean,
				&cpue_standard_deviation,
				&cpue_standard_error_of_mean,
				&cpue_sample_size,
				species->statistics_filename );

			fprintf( output_pipe,
				 "%s|%s|%s|%s|%d|%d|%d",
				 species->family_caught,
				 species->genus_caught,
				 species->species_caught,
				 species->fishing_area,
				 species->catches,
				 species->effort,
				 cpue_sample_size );
	
			if ( species->effort )
			{
				fprintf( output_pipe,
					 "|%.4lf",
					 (double)species->catches /
					 (double)species->effort );
			}
			else
			{
				fprintf( output_pipe,
					 "|0.0" );
			}
	
			fprintf( output_pipe,
				 "|%.4lf|%.4lf|%.4lf\n",
				 cpue_mean,
				 cpue_standard_deviation,
				 cpue_standard_error_of_mean );

		} while ( list_next( catch_per_unit_effort->
					results_per_species_per_area_list ) );
	}

	pclose( output_pipe );

	if ( !omit_links )
	{
		printf( "<br>\n" );

		if ( timlib_strcmp( catch_harvest, "cpue" ) == 0 )
		{
			appaserver_library_output_ftp_prompt(
				ftp_per_species_per_area_filename,
"Species CPUE Per Species Per Area: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
		}
		else
		{
			appaserver_library_output_ftp_prompt(
				ftp_per_species_per_area_filename,
"Species HPUE Per Species Per Area: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
		}
	}

	/* Output CPUE Per Species */
	/* ----------------------- */
	appaserver_link_file->filename_stem =
		FILENAME_STEM_PER_SPECIES;

	output_filename =
		get_per_species_output_filename(
			document_root_directory,
			application_name,
			begin_date_string,
			end_date_string,
			process_id,
			appaserver_link_file->filename_stem );

	if ( ! ( output_file =
			fopen(	output_filename,
				"w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			output_filename );
		document_close();
		exit( 1 );
	}
	else
	{
		fclose( output_file );
	}

	ftp_per_species_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	sprintf( output_sys_string,
		 "tr '|' ',' > %s",
		 output_filename );

	output_pipe = popen( output_sys_string, "w" );

	if ( sub_title )
	{
		if ( timlib_strcmp( catch_harvest, "cpue" ) == 0 )
		{
			fprintf(output_pipe,
		 	 	"Species CPUE Per Species\n%s\n",
		 	 	sub_title );
		}
		else
		{
			fprintf(output_pipe,
		 	 	"Species HPUE Per Species\n%s\n",
		 	 	sub_title );
		}
	}

	fprintf( output_pipe,
		 "%s\n",
		 get_summary_heading(
			catch_harvest,
			exclude_zero_catches_label ) );

	if ( list_rewind(
		catch_per_unit_effort->
		results_per_species_list ) )
	{
		do {
			species = list_get_pointer( 
					catch_per_unit_effort->
					results_per_species_list );

			creel_library_fetch_species_summary_statistics_filename(
				&cpue_mean,
				&cpue_standard_deviation,
				&cpue_standard_error_of_mean,
				&cpue_sample_size,
				species->statistics_filename );

			fprintf( output_pipe,
				 "%s|%s|%s|%d|%d|%d",
				 species->family_caught,
				 species->genus_caught,
				 species->species_caught,
				 species->catches,
				 species->effort,
				 cpue_sample_size );

			if ( species->effort )
			{
				fprintf( output_pipe,
					 "|%.4lf",
					 (double)species->catches /
					 (double)species->effort );
			}
			else
			{
				fprintf( output_pipe,
					 "|0.0" );
			}

			fprintf( output_pipe,
				 "|%.4lf|%.4lf|%.4lf\n",
				 cpue_mean,
				 cpue_standard_deviation,
				 cpue_standard_error_of_mean );

		} while ( list_next( catch_per_unit_effort->
					results_per_species_list ) );
	}

	pclose( output_pipe );

	if ( !omit_links )
	{
		printf( "<br>\n" );

		if ( timlib_strcmp( catch_harvest, "cpue" ) == 0 )
		{
			appaserver_library_output_ftp_prompt(
				ftp_per_species_filename,
"Species CPUE Per Species: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
		}
		else
		{
			appaserver_library_output_ftp_prompt(
				ftp_per_species_filename,
"Species HPUE Per Species: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
		}
	}

	creel_library_remove_statistics_files(
		catch_per_unit_effort->
			results_per_species_per_area_list );

	creel_library_remove_statistics_files(
		catch_per_unit_effort->
			results_per_species_list );

} /* creel_catches_report_catch_per_unit_effort_spreadsheet() */

void get_title_and_sub_title(
			char *title,
			char *sub_title,
			char *begin_date_string,
			char *end_date_string,
			char *process_name,
			char *fishing_purpose )
{
	format_initial_capital( title, process_name );

	if ( fishing_purpose
	&&   *fishing_purpose
	&&   strcmp( fishing_purpose, "fishing_purpose" ) != 0 )
	{
		sprintf(	sub_title,
				"Purpose: %s From %s To %s",
				fishing_purpose,
				begin_date_string,
				end_date_string );
	}
	else
	{
		sprintf(	sub_title,
				"From %s To %s",
				begin_date_string,
				end_date_string );
	}
	format_initial_capital( sub_title, sub_title );
} /* get_title_and_sub_title() */

char *get_per_species_output_filename(
			char *document_root_directory,
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			pid_t process_id,
			char *filename_stem )
{
	char *output_filename;
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			filename_stem,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	appaserver_link_file->begin_date_string = begin_date_string;
	appaserver_link_file->end_date_string = end_date_string;

	output_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	return output_filename;

} /* get_per_species_output_filename() */

char *get_summary_heading(
			char *catch_harvest,
			char *exclude_zero_catches_label )
{
	static char summary_heading[ 1024 ];

	if ( timlib_strcmp( catch_harvest, "cpue" ) == 0 )
	{
		sprintf( summary_heading,
"Family|Genus|Species|Sum(Catches)|Sum(Effort Hours)|Fishing Trip Count|CPUE Weighted Effort %s|CPUE Non-Weighted Effort %s|CPUE Std. Dev.|CPUE Std. Err. Mean",
			 exclude_zero_catches_label,
			 exclude_zero_catches_label );
	}
	else
	{
		sprintf( summary_heading,
"Family|Genus|Species|Sum(Kept)|Sum(Effort Hours)|Fishing Trip Count|HPUE Weighted Effort %s|HPUE Non-Weighted Effort %s|HPUE Std. Dev.|HPUE Std. Err. Mean",
			 exclude_zero_catches_label,
			 exclude_zero_catches_label );
	}

	return summary_heading;

} /* get_summary_heading() */

void append_spreadsheet(	char *destination_filename,
				char *source_filename,
				char *column_a_datum )
{
	FILE *input_file;
	FILE *output_file;
	char input_buffer[ 1024 ];
	boolean first_time = 1;

	if ( ! ( input_file = fopen( source_filename, "r" ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 source_filename );
		exit( 1 );
	}

	if ( ! ( output_file = fopen( destination_filename, "a" ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot open %s for append.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 destination_filename );
		exit( 1 );
	}

	while( get_line( input_buffer, input_file ) )
	{
		if ( first_time )
			first_time = 0;
		else
		{
			fprintf(	output_file,
					"%s,%s\n",
					column_a_datum,
					input_buffer );
		}
	}

	fclose( input_file );
	fclose( output_file );

} /* append_spreadsheet() */

