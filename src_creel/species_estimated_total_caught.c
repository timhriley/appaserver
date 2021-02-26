/* ---------------------------------------------------	*/
/* src_creel/species_estimated_total_caught.c		*/
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
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "total_caught.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */
enum output_medium { output_medium_stdout, text_file, table };

/* Constants */
/* --------- */
#define ROWS_BETWEEN_HEADING		20
#define DEFAULT_OUTPUT_MEDIUM		text_file
#define FILENAME_STEM_MONTHLY		"estimated_total_caught_monthly"
#define FILENAME_STEM_GRAND		"estimated_total_caught_grand"

/* Prototypes */
/* ---------- */
void output_month_sheet_total_row_table(
					FILE *output_pipe,
					MONTH_ROW *total_row,
					LIST *total_row_catch_area_list,
					boolean all_species );

void output_month_sheet_table(		FILE *output_pipe,
					MONTH_SHEET *month_sheet,
					LIST *total_row_catch_area_list,
					boolean all_species );

char *get_month_sheet_table_heading(
					LIST *species_list,
					LIST *total_row_catch_area_list,
					boolean all_species );

void output_month_sheet_list_table(
					LIST *month_sheet_list,
					LIST *species_list,
					LIST *total_row_catch_area_list,
					boolean all_species );

void output_total_sheet_total_row_table(
					FILE *output_pipe,
					TOTAL_ROW *total_row,
					LIST *catch_area_list,
					boolean all_species );

void output_total_sheet_total_row_list_table(
					FILE *output_pipe,
					LIST *total_row_list,
					LIST *total_row_catch_area_list,
					boolean all_species );

char *get_month_total_sheet_table_heading(
					LIST *species_list,
					LIST *catch_area_list,
					int year,
					boolean all_species );

void output_month_total_sheet_table(	TOTAL_SHEET *total_sheet,
					LIST *species_list,
					int year,
					boolean all_species );

void output_total_sheet_total_row_list_text_file(
					FILE *output_file,
					LIST *total_row_list,
					LIST *total_row_catch_area_list );

void output_month_sheet_text_file_heading(
					FILE *output_file,
					LIST *species_list,
					LIST *total_row_catch_area_list,
					int this_month,
					boolean all_species );

void output_month_total_sheet_text_file_heading(
					FILE *output_file,
					LIST *species_list,
					LIST *catch_area_list,
					int year );

void output_grand_total_table(	double grand_areas_1_5_estimated_kept,
				double grand_areas_1_5_estimated_released,
				double grand_areas_1_5_estimated_caught,
				double grand_areas_1_6_estimated_kept,
				double grand_areas_1_6_estimated_released,
				double grand_areas_1_6_estimated_caught,
				int year,
				int begin_month,
				int end_month,
				LIST *species_list,
				boolean all_species );

void output_table(			ESTIMATED_TOTAL_CAUGHT_OUTPUT *output,
					int begin_month,
					int end_month,
					LIST *species_list,
					int year,
					boolean all_species );

void output_total_sheet_total_row_text_file(
					FILE *output_file,
					TOTAL_ROW *total_row,
					LIST *catch_area_list );

void output_month_sheet_total_row_text_file(
					FILE *output_file,
					MONTH_ROW *total_row,
					LIST *total_row_catch_area_list,
					boolean all_species );

void output_month_sheet_list_text_file(
					LIST *month_sheet_list,
					char *document_root_directory,
					pid_t process_id,
					char *application_name,
					LIST *species_list,
					int begin_month,
					int year,
					LIST *catch_area_list,
					boolean all_species );

void output_grand_total_sheet_text_file(
				double grand_areas_1_5_estimated_kept,
				double grand_areas_1_5_estimated_released,
				double grand_areas_1_5_estimated_caught,
				double grand_areas_1_6_estimated_kept,
				double grand_areas_1_6_estimated_released,
				double grand_areas_1_6_estimated_caught,
				int year,
				char *document_root_directory,
				pid_t process_id,
				char *application_name,
				int begin_month,
				int end_month,
				LIST *species_list,
				boolean all_species );

void output_month_total_sheet_text_file(TOTAL_SHEET *total_sheet,
					int year,
					char *appaserver_mount_point,
					pid_t process_id,
					char *application_name,
					LIST *species_list );

void output_month_sheet_text_file(	FILE *output_file,
					MONTH_SHEET *month_sheet,
					LIST *total_row_catch_area_list,
					boolean all_species );

void output_text_file(		ESTIMATED_TOTAL_CAUGHT_OUTPUT *output,
				int begin_month,
				int end_month,
				char *application_name,
				char *document_root_directory,
				LIST *species_list,
				int year,
				boolean all_species );

enum output_medium get_output_medium(
				char *output_medium_string );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	enum output_medium output_medium;
	char *begin_month_string;
	char *end_month_string;
	int begin_month_integer;
	int end_month_integer;
	char *year_string;
	char *family_list_string;
	char *genus_list_string;
	char *species_list_string;
	boolean all_species = 0;
	char *output_medium_string;
	ESTIMATED_TOTAL_CAUGHT *estimated_total_caught;

	if ( argc != 10 )
	{
		fprintf( stderr,
"Usage: %s application process_name begin_month end_month year family genus species output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];

	begin_month_string = argv[ 3 ];

	if ( strcmp( begin_month_string, "begin_month" ) == 0 )
		begin_month_integer = 1;
	else
		begin_month_integer = atoi( begin_month_string );

	end_month_string = argv[ 4 ];

	if ( strcmp( end_month_string, "end_month" ) == 0 )
		end_month_integer = 12;
	else
		end_month_integer = atoi( end_month_string );

	year_string = argv[ 5 ];
	family_list_string = argv[ 6 ];
	genus_list_string = argv[ 7 ];
	species_list_string = argv[ 8 ];
	output_medium_string = argv[ 9 ];

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

	if ( begin_month_integer < 1
	||   end_month_integer > 12
	||   begin_month_integer > end_month_integer )
	{
		document_quick_output_body(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point );
		printf( "<h3>Error: invalid month integers.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( atoi( year_string ) < AERIAL_SURVEY_ACCURACY_YEAR )
	{
		document_quick_output_body(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point );
		printf(
"<h3>Error: invalid year. The aerial surveys are accurate as of %d.</h3>\n",
			AERIAL_SURVEY_ACCURACY_YEAR );
		document_close();
		exit( 0 );
	}

	output_medium = get_output_medium( output_medium_string );

	estimated_total_caught =
		estimated_total_caught_new(
				begin_month_integer,
				end_month_integer,
				atoi( year_string ),
				application_name );

	estimated_total_caught->input->species_list =
		total_caught_get_species_list(
			&all_species,
			application_name,
			family_list_string,
			genus_list_string,
			species_list_string );

	if ( !list_length( estimated_total_caught->input->species_list ) )
	{
		document_quick_output_body(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point );
		printf( "<h3>Error: cannot get species list.</h3>\n" );
		document_close();
		exit( 0 );
	}

	estimated_total_caught->input->fishing_trip_hash_table =
		total_caught_get_fishing_trip_hash_table(
			estimated_total_caught->
				input->fishing_trip_list_array,
			application_name,
			estimated_total_caught->input->begin_month,
			estimated_total_caught->input->end_month,
			estimated_total_caught->input->year );

	total_caught_populate_catch_list(
		estimated_total_caught->input->fishing_trip_hash_table,
		estimated_total_caught->input->species_list,
			application_name,
			estimated_total_caught->input->begin_month,
			estimated_total_caught->input->end_month,
			estimated_total_caught->input->year );

	estimated_total_caught->output =
		total_caught_get_output(
			estimated_total_caught->input->begin_month,
			estimated_total_caught->input->end_month,
			estimated_total_caught->input->year,
			estimated_total_caught->input->species_list,
			estimated_total_caught->input->
				trailer_count_dictionary,
			estimated_total_caught->input->
				fishing_trip_list_array,
			estimated_total_caught->input->
				weekend_creel_census_dictionary,
			estimated_total_caught->input->areas_1_5_coefficient,
			estimated_total_caught->input->areas_1_5_y_intercept,
			estimated_total_caught->input->area_6_coefficient,
			estimated_total_caught->input->area_6_y_intercept,
			0 /* not omit_catch_area_list */ );

	if ( output_medium != output_medium_stdout )
	{
		char buffer[ 1024 ];

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
	
		printf( "<h1>%s<br></h1>\n",
			format_initial_capital( buffer, process_name ) );
		printf( "<h2>\n" );
		fflush( stdout );
		system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
		fflush( stdout );
		printf( "</h2>\n" );
	}

#ifdef NOT_DEFINED
fprintf( stderr, "%s/%s()/%d: length( species_list ) = %d, length( catch_area_list ) = %d\n",
__FILE__,
__FUNCTION__,
__LINE__,
list_length( estimated_total_caught->
				input->
				species_list ),
list_length( estimated_total_caught->
				output->
				total_sheet->
				total_row->
				catch_area_list ) );

	if (	!list_length( estimated_total_caught->input->species_list )
	||	( list_length( estimated_total_caught->
				input->
				species_list ) !=
		  list_length( estimated_total_caught->
				output->
				total_sheet->
				total_row->
				catch_area_list ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: lists are invalid lengths: %d and %d.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 list_length( estimated_total_caught->
					input->
					species_list ),
			 list_length( estimated_total_caught->
					output->
					total_sheet->
					total_row->
					catch_area_list ) );
		printf(
		"<h3>An internal error occurred. See the log file.</h3>\n" );

		document_close();

		exit( 1 );
	}
#endif

	if ( output_medium == table )
	{
		output_table(
				estimated_total_caught->output,
				estimated_total_caught->input->begin_month,
				estimated_total_caught->input->end_month,
				estimated_total_caught->input->species_list,
				estimated_total_caught->input->year,
				all_species );

		document_close();
	}
	else
	if ( output_medium == text_file )
	{
		output_text_file(
				estimated_total_caught->output,
				estimated_total_caught->input->begin_month,
				estimated_total_caught->input->end_month,
				application_name,
				appaserver_parameter_file->
					document_root,
				estimated_total_caught->input->species_list,
				estimated_total_caught->input->year,
				all_species );

		document_close();
	}
	else
	if ( output_medium == output_medium_stdout )
	{
	}

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

enum output_medium get_output_medium( char *output_medium_string )
{
	if ( strcmp( output_medium_string, "text_file" ) == 0
	||   strcmp( output_medium_string, "transmit" ) == 0
	||   strcmp( output_medium_string, "spreadsheet" ) == 0 )
	{
		return text_file;
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

void output_table(	ESTIMATED_TOTAL_CAUGHT_OUTPUT *output,
			int begin_month,
			int end_month,
			LIST *species_list,
			int year,
			boolean all_species )
{
	output_grand_total_table(
				output->grand_areas_1_5_estimated_kept,
				output->grand_areas_1_5_estimated_released,
				output->grand_areas_1_5_estimated_caught,
				output->grand_areas_1_6_estimated_kept,
				output->grand_areas_1_6_estimated_released,
				output->grand_areas_1_6_estimated_caught,
				year,
				begin_month,
				end_month,
				species_list,
				all_species );

	output_month_total_sheet_table( output->total_sheet,
					species_list,
					year,
					all_species );

	output_month_sheet_list_table(	output->month_sheet_list,
					species_list,
					output->
						total_sheet->
						total_row->
						catch_area_list,
					all_species );

	printf(
"<br><a href=\"/src_creel/species_estimated_total_caught_algorithm.pdf\">Press to view the algorithm.</a><br>\n" );

} /* output_table() */

void output_text_file(	ESTIMATED_TOTAL_CAUGHT_OUTPUT *output,
			int begin_month,
			int end_month,
			char *application_name,
			char *document_root_directory,
			LIST *species_list,
			int year,
			boolean all_species )
{
	pid_t process_id = getpid();

	output_grand_total_sheet_text_file(
		output->grand_areas_1_5_estimated_kept,
		output->grand_areas_1_5_estimated_released,
		output->grand_areas_1_5_estimated_caught,
		output->grand_areas_1_6_estimated_kept,
		output->grand_areas_1_6_estimated_released,
		output->grand_areas_1_6_estimated_caught,
		year,
		document_root_directory,
		process_id,
		application_name,
		begin_month,
		end_month,
		species_list,
		all_species );

	if ( output->total_sheet
	&&   output->total_sheet->total_row )
	{
		output_month_total_sheet_text_file(
			output->total_sheet,
			year,
			document_root_directory,
			process_id,
			application_name,
			species_list );

		output_month_sheet_list_text_file(
			output->month_sheet_list,
			document_root_directory,
			process_id,
			application_name,
			species_list,
			begin_month,
			year,
			output->total_sheet->total_row->catch_area_list,
			all_species );
	}

	printf(
"<br><a href=\"/src_creel/species_estimated_total_caught_algorithm.pdf\">Press to view the algorithm.</a><br>\n" );

} /* output_text_file() */

void output_month_sheet_list_text_file(
					LIST *month_sheet_list,
					char *document_root_directory,
					pid_t process_id,
					char *application_name,
					LIST *species_list,
					int begin_month,
					int year,
					LIST *total_row_catch_area_list,
					boolean all_species )
{
	MONTH_SHEET *month_sheet;
	char *output_filename;
	char *ftp_filename;
	char ftp_prompt[ 256 ];
	char *month_abbreviation;
	char year_string[ 8 ];
	FILE *output_file;
	APPASERVER_LINK_FILE *appaserver_link_file;

	if ( !list_rewind( month_sheet_list ) ) return;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM_MONTHLY,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	sprintf( year_string, "%d", year );
	appaserver_link_file->begin_date_string = year_string;

	do {
		month_sheet = list_get_pointer( month_sheet_list );

		month_abbreviation =
			timlib_get_three_character_month_string(
				month_sheet->month - 1 );

		appaserver_link_file->end_date_string = month_abbreviation;

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

		ftp_filename =
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

		output_month_sheet_text_file_heading(
				output_file,
				species_list,
				total_row_catch_area_list,
				begin_month++,
				all_species );

		output_month_sheet_text_file(
				output_file,
				month_sheet,
				total_row_catch_area_list,
				all_species );

		if ( month_sheet->total_row )
		{
			output_month_sheet_total_row_text_file(
				output_file,
				month_sheet->total_row,
				total_row_catch_area_list,
				all_species );
		}

		fclose( output_file );

		sprintf( ftp_prompt,
	"%s: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			 month_abbreviation );

		appaserver_library_output_ftp_prompt(
			ftp_filename,
			ftp_prompt,
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

		printf( "<br>\n" );

	} while( list_next( month_sheet_list ) );

} /* output_month_sheet_list_text_file() */

void output_month_sheet_text_file_heading(
					FILE *output_file,
					LIST *species_list,
					LIST *total_row_catch_area_list,
					int this_month,
					boolean all_species )
{
	SPECIES *species;
	CATCH_AREA *catch_area;

	fprintf( output_file,
		 "%s,Trailers,,,Vessels,,,Interviews,,,,Trips",
		 timlib_integer2full_month( this_month ) );

	if ( !list_rewind( species_list )
	||   !list_rewind( total_row_catch_area_list ) )
	{
		fprintf( output_file, "\n" );
		return;
	}

	do {
		species = list_get_pointer( species_list );

		catch_area = list_get_pointer(
				total_row_catch_area_list );

		if ( catch_area->areas_1_6_estimated_caught )
		{
			if ( !all_species )
			{
				fprintf( output_file,
				",Sample Areas 1-5,,,Sample Areas 1-6,," );
			}

			fprintf( output_file,
				",Estimated Areas 1-5,,,Estimated Areas 1-6,,");
		}

		if ( !list_next( total_row_catch_area_list ) ) break;

	} while( list_next( species_list ) );

	fprintf( output_file, "\n" );

	fprintf( output_file,
"Day,Florida Bay,Whitewater Bay,Total,Area 1-5,Area 6,Area 1-6,Flamingo Fishing,Flamingo Boating,Park Fishing,Park Boating,Estimated Park Fishing" );

	list_rewind( species_list );
	list_rewind( total_row_catch_area_list );

	do {
		species = list_get_pointer( species_list );

		catch_area = list_get_pointer(
				total_row_catch_area_list );

		if ( catch_area->areas_1_6_estimated_caught )
		{
			if ( !all_species )
			{
				/* Output actual */
				/* ------------- */
				fprintf( output_file,
		",%d Kept,%d Released,%d Caught,%d Kept,%d Released,%d Caught",
				 	species->florida_state_code,
				 	species->florida_state_code,
				 	species->florida_state_code,
				 	species->florida_state_code,
				 	species->florida_state_code,
				 	species->florida_state_code );
			}

			/* Output estimated total */
			/* ---------------------- */
			fprintf( output_file,
		",%d Kept,%d Released,%d Caught,%d Kept,%d Released,%d Caught",
				 species->florida_state_code,
				 species->florida_state_code,
				 species->florida_state_code,
				 species->florida_state_code,
				 species->florida_state_code,
				 species->florida_state_code );
		}

		if ( !list_next( total_row_catch_area_list ) ) break;

	} while( list_next( species_list ) );

	fprintf( output_file, "\n" );

} /* output_month_sheet_text_file_heading() */

void output_month_sheet_text_file(	FILE *output_file,
					MONTH_SHEET *month_sheet,
					LIST *total_row_catch_area_list,
					boolean all_species )
{
	MONTH_ROW *month_row;
	CATCH_AREA *catch_area;
	CATCH_AREA *total_row_catch_area;

	if ( !list_rewind( month_sheet->row_list ) ) return;

	do {
		month_row = list_get_pointer( month_sheet->row_list );

		fprintf( output_file,
"%d,%d,%d,%d,%.0lf,%.0lf,%.0lf,%.1lf,%.1lf,%.1lf,%.1lf,%.1lf",
			 month_row->day,
			 month_row->vessel->florida_bay_trailer_count,
			 month_row->vessel->whitewater_bay_trailer_count,
			 month_row->vessel->total_trailer_count,
			 month_row->vessel->estimated_areas_1_5_vessels,
			 month_row->vessel->estimated_area_6_vessels,
			 month_row->vessel->estimated_areas_1_6_vessels,
			 month_row->fishing_trip->flamingo_fishing_day,
			 month_row->fishing_trip->flamingo_boating_day,
			 month_row->fishing_trip->park_fishing_day,
			 month_row->fishing_trip->park_boating_day,
			 month_row->vessel->estimated_park_fishing_vessels );

		if ( !list_rewind( month_row->catch_area_list ) )
			continue;

		list_rewind( total_row_catch_area_list );

		do {
			catch_area = list_get_pointer(
					month_row->catch_area_list );

			total_row_catch_area =
				list_get_pointer(
					total_row_catch_area_list );

			if ( total_row_catch_area->
				areas_1_6_estimated_caught )
			{
				if ( !all_species )
				{
					fprintf( output_file,
				 ",%.1lf,%.1lf,%.1lf,%.1lf,%.1lf,%.1lf",
				 	catch_area->
						areas_1_5_sample_kept,
				 	catch_area->
						areas_1_5_sample_released,
				 	catch_area->
						areas_1_5_sample_caught,
				 	catch_area->
						areas_1_6_sample_kept,
				 	catch_area->
						areas_1_6_sample_released,
				 	catch_area->
						areas_1_6_sample_caught );
				}

				fprintf( output_file,
				 ",%.2lf,%.2lf,%.2lf,%.2lf,%.2lf,%.2lf",
				 catch_area->
					areas_1_5_estimated_kept,
				 catch_area->
					areas_1_5_estimated_released,
				 catch_area->
					areas_1_5_estimated_caught,
				 catch_area->
					areas_1_6_estimated_kept,
				 catch_area->
					areas_1_6_estimated_released,
				 catch_area->
					areas_1_6_estimated_caught );

			}

			list_next( total_row_catch_area_list );

		} while( list_next( month_row->catch_area_list ) );

		fprintf( output_file, "\n" );

	} while( list_next( month_sheet->row_list ) );

} /* output_month_sheet_text_file() */

void output_month_sheet_total_row_text_file(
				FILE *output_file,
				MONTH_ROW *total_row,
				LIST *total_row_catch_area_list,
				boolean all_species )
{
	CATCH_AREA *catch_area;
	CATCH_AREA *total_row_catch_area;

	if ( !total_row
	||   !total_row->vessel
	||   !total_row->fishing_trip )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty total_row.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	fprintf( output_file,
	 "Total,%d,%d,%d,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf",
	 total_row->vessel->florida_bay_trailer_count,
	 total_row->vessel->whitewater_bay_trailer_count,
	 total_row->vessel->total_trailer_count,
	 total_row->vessel->estimated_areas_1_5_vessels,
	 total_row->vessel->estimated_area_6_vessels,
	 total_row->vessel->estimated_areas_1_6_vessels,
	 total_row->fishing_trip->flamingo_fishing_day,
	 total_row->fishing_trip->flamingo_boating_day,
	 total_row->fishing_trip->park_fishing_day,
	 total_row->fishing_trip->park_boating_day,
	 total_row->vessel->estimated_park_fishing_vessels );

	if ( !list_rewind( total_row->catch_area_list ) )
	{
		fprintf( output_file, "\n" );
		return;
	}

	list_rewind( total_row_catch_area_list );

	do {
		catch_area =
			list_get_pointer(
				total_row->catch_area_list );

		total_row_catch_area =
			list_get_pointer(
				total_row_catch_area_list );

		if ( total_row_catch_area->areas_1_6_estimated_caught )
		{
			if ( !all_species )
			{
				fprintf( output_file,
			 		 ",,,,,," );
			}

			fprintf( output_file,
			 ",%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf",
			 catch_area->
				areas_1_5_estimated_kept,
			 catch_area->
				areas_1_5_estimated_released,
			 catch_area->
				areas_1_5_estimated_caught,
			 catch_area->
				areas_1_6_estimated_kept,
			 catch_area->
				areas_1_6_estimated_released,
			 catch_area->
				areas_1_6_estimated_caught );
		}

		list_next( total_row_catch_area_list );

	} while( list_next( total_row->catch_area_list ) );

	fprintf( output_file, "\n" );

} /* output_month_sheet_total_row_text_file() */

void output_month_total_sheet_text_file(
					TOTAL_SHEET *total_sheet,
					int year,
					char *document_root_directory,
					pid_t process_id,
					char *application_name,
					LIST *species_list )

{
	char *output_filename;
	char *ftp_filename;
	char ftp_prompt[ 256 ];
	char year_string[ 8 ];
	FILE *output_file;
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM_GRAND,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	sprintf( year_string, "%d", year );
	appaserver_link_file->begin_date_string = year_string;

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

	ftp_filename =
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

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			output_filename );
		document_close();
		exit( 1 );
	}

	output_month_total_sheet_text_file_heading(
				output_file,
				species_list,
				total_sheet->total_row->catch_area_list,
				year );

	output_total_sheet_total_row_list_text_file(
				output_file,
				total_sheet->total_row_list,
				total_sheet->total_row->catch_area_list );

	output_total_sheet_total_row_text_file(
				output_file,
				total_sheet->total_row,
				total_sheet->total_row->catch_area_list );

	fclose( output_file );

	sprintf( ftp_prompt,
"Monthly Totals: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save." );

	appaserver_library_output_ftp_prompt(
		ftp_filename,
		ftp_prompt,
		(char *)0 /* target */,
		(char *)0 /* application_type */ );

	printf( "<br>\n" );

} /* output_month_total_sheet_text_file() */

void output_total_sheet_total_row_list_text_file(
					FILE *output_file,
					LIST *total_row_list,
					LIST *total_row_catch_area_list )
{
	TOTAL_ROW *total_row;
	CATCH_AREA *catch_area;
	CATCH_AREA *total_row_catch_area;

	if ( !list_rewind( total_row_list ) ) return;

	do {
		total_row = list_get_pointer( total_row_list );

		fprintf( output_file,
		"%s,%d,%d,%d,%.1lf,%.1lf,%.1lf,%1lf,%1lf,%1lf,%1lf,%.1lf",
			 total_row->month,
			 total_row->florida_bay_trailer_count,
			 total_row->whitewater_bay_trailer_count,
			 total_row->total_trailer_count,
			 total_row->estimated_areas_1_5_vessels,
			 total_row->estimated_area_6_vessels,
			 total_row->estimated_areas_1_6_vessels,
			 total_row->flamingo_fishing_day,
			 total_row->flamingo_boating_day,
			 total_row->park_fishing_day,
			 total_row->park_boating_day,
			 total_row->estimated_park_fishing_vessels );

		if ( !list_rewind( total_row->catch_area_list ) )
		{
			fprintf( output_file, "\n" );
			continue;
		}

		if ( list_rewind( total_row_catch_area_list ) )
		{
			do {
				catch_area =
					list_get_pointer(
						total_row->catch_area_list );

				total_row_catch_area =
					list_get_pointer(
						total_row_catch_area_list );

				if ( total_row_catch_area->
					areas_1_6_estimated_caught )
				{
					fprintf( output_file,
				 ",%.1lf,%.1lf,%.1lf,%.1lf,%.1lf,%.1lf",
				 	catch_area->
						areas_1_5_estimated_kept,
				 	catch_area->
						areas_1_5_estimated_released,
				 	catch_area->
						areas_1_5_estimated_caught,
				 	catch_area->
						areas_1_6_estimated_kept,
				 	catch_area->
						areas_1_6_estimated_released,
				 	catch_area->
						areas_1_6_estimated_caught );
				}

				if ( !list_next( total_row_catch_area_list ) )
					break;

			} while( list_next( total_row->catch_area_list ) );
		}

		fprintf( output_file, "\n" );

	} while( list_next( total_row_list ) );

} /* output_total_sheet_total_row_list_text_file() */

void output_month_total_sheet_text_file_heading(
				FILE *output_file,
				LIST *species_list,
				LIST *catch_area_list,
				int year )
{
	SPECIES *species;
	CATCH_AREA *catch_area;

	fprintf( output_file,
		 ",Trailers,,,Vessels,,,Interviews,,,,Trips" );

	if ( !list_rewind( species_list )
	||   !list_rewind( catch_area_list ) )
	{
		fprintf( output_file, "\n" );
		return;
	}

	do {
		species = list_get_pointer( species_list );
		catch_area = list_get_pointer( catch_area_list );

		if ( catch_area->areas_1_6_estimated_caught )
		{
		       fprintf( output_file,
		       ",Estimated Areas 1-5,,,Estimated Areas 1-6,," );
		}

		if ( !list_next( catch_area_list ) ) break;

	} while( list_next( species_list ) );

	fprintf( output_file, "\n" );

	fprintf( output_file,
"%d,Florida Bay,Whitewater Bay,Total,Area 1-5,Area 6,Area 1-6,Flamingo Fishing,Flamingo Boating,Park Fishing,Park Boating,Estimated Fishing",
		 year );

	list_rewind( species_list );
	list_rewind( catch_area_list );

	do {
		species = list_get_pointer( species_list );
		catch_area = list_get_pointer( catch_area_list );

		/* Output estimated total */
		/* ---------------------- */
		if ( catch_area->areas_1_6_estimated_caught )
		{
			fprintf( output_file,
		",%d Kept,%d Released,%d Caught,%d Kept,%d Released,%d Caught",
		 		species->florida_state_code,
		 		species->florida_state_code,
		 		species->florida_state_code,
		 		species->florida_state_code,
		 		species->florida_state_code,
		 		species->florida_state_code );
		}

		if ( !list_next( catch_area_list ) ) break;

	} while( list_next( species_list ) );

	fprintf( output_file, "\n" );

} /* output_month_total_sheet_text_file_heading() */

void output_total_sheet_total_row_text_file(
					FILE *output_file,
					TOTAL_ROW *total_row,
					LIST *catch_area_list )
{
	CATCH_AREA *catch_area;

	fprintf( output_file,
	 "Total,%d,%d,%d,%.1lf,%.1lf,%.1lf,%.1lf,%.1lf,%.1lf,%.1lf,%.1lf",
	 total_row->florida_bay_trailer_count,
	 total_row->whitewater_bay_trailer_count,
	 total_row->total_trailer_count,
	 total_row->estimated_areas_1_5_vessels,
	 total_row->estimated_area_6_vessels,
	 total_row->estimated_areas_1_6_vessels,
	 total_row->flamingo_fishing_day,
	 total_row->flamingo_boating_day,
	 total_row->park_fishing_day,
	 total_row->park_boating_day,
	 total_row->estimated_park_fishing_vessels );

	if ( !list_rewind( catch_area_list ) )
	{
		fprintf( output_file, "\n" );
		return;
	}

	do {
		catch_area =
			list_get_pointer(
				catch_area_list );

		if ( catch_area->areas_1_6_estimated_caught )
		{
			fprintf( output_file,
			 ",%.1lf,%.1lf,%.1lf,%.1lf,%.1lf,%.1lf",
			 catch_area->
				areas_1_5_estimated_kept,
			 catch_area->
				areas_1_5_estimated_released,
			 catch_area->
				areas_1_5_estimated_caught,
			 catch_area->
				areas_1_6_estimated_kept,
			 catch_area->
				areas_1_6_estimated_released,
			 catch_area->
				areas_1_6_estimated_caught );
		}

	} while( list_next( catch_area_list ) );

	fprintf( output_file, "\n" );

} /* output_total_sheet_total_row_text_file() */

void output_grand_total_table(	double grand_areas_1_5_estimated_kept,
				double grand_areas_1_5_estimated_released,
				double grand_areas_1_5_estimated_caught,
				double grand_areas_1_6_estimated_kept,
				double grand_areas_1_6_estimated_released,
				double grand_areas_1_6_estimated_caught,
				int year,
				int begin_month,
				int end_month,
				LIST *species_list,
				boolean all_species )
{
	FILE *output_pipe;
	SPECIES *species;
	char sys_string[ 1024 ];

	if ( !list_rewind( species_list ) ) return;

	sprintf(sys_string,
		"html_table.e '^^Grand Total' '' '^' left,right" );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
		 "Year^%d\n",
		 year );

	fprintf( output_pipe,
		 "Begin Month^%s\n",
		 timlib_integer2full_month( begin_month ) );

	fprintf( output_pipe,
		 "End Month^%s\n",
		 timlib_integer2full_month( end_month ) );

	fprintf( output_pipe,
		 "Species" );

	if ( all_species )
	{
		fprintf( output_pipe, "^All species\n" );
	}
	else
	{
		do {
			species = list_get_pointer( species_list );
	
			if ( list_at_first( species_list ) )
				fprintf( output_pipe, "^" );
			else
				fprintf( output_pipe, "," );
	
			fprintf( output_pipe,
				 "%d",
				 species->florida_state_code );
	
		} while( list_next( species_list ) );

		fprintf( output_pipe, "\n" );
	}

	fprintf( output_pipe,
		 "Areas 1-5 Estimated Kept^%.1lf\n",
		 grand_areas_1_5_estimated_kept );

	fprintf( output_pipe,
		 "Areas 1-5 Estimated Released^%.1lf\n",
		 grand_areas_1_5_estimated_released );

	fprintf( output_pipe,
		 "Areas 1-5 Estimated Caught^%.1lf\n",
		 grand_areas_1_5_estimated_caught );

	fprintf( output_pipe,
		 "Areas 1-6 Estimated Kept^%.1lf\n",
		 grand_areas_1_6_estimated_kept );

	fprintf( output_pipe,
		 "Areas 1-6 Estimated Released^%.1lf\n",
		 grand_areas_1_6_estimated_released );

	fprintf( output_pipe,
		 "Areas 1-6 Estimated Caught^%.1lf\n",
		 grand_areas_1_6_estimated_caught );

	pclose( output_pipe );

} /* output_grand_total_table() */

void output_grand_total_sheet_text_file(
				double grand_areas_1_5_estimated_kept,
				double grand_areas_1_5_estimated_released,
				double grand_areas_1_5_estimated_caught,
				double grand_areas_1_6_estimated_kept,
				double grand_areas_1_6_estimated_released,
				double grand_areas_1_6_estimated_caught,
				int year,
				char *document_root_directory,
				pid_t process_id,
				char *application_name,
				int begin_month,
				int end_month,
				LIST *species_list,
				boolean all_species )
{
	char *output_filename;
	char *ftp_filename;
	char ftp_prompt[ 256 ];
	FILE *output_file;
	SPECIES *species;
	char year_string[ 8 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	if ( !list_rewind( species_list ) ) return;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM_GRAND,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	sprintf( year_string, "%d", year );
	appaserver_link_file->begin_date_string = year_string;

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

	ftp_filename =
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

	fprintf( output_file,
		 "Year,%d\n",
		 year );

	fprintf( output_file,
		 "Begin Month,%s\n",
		 timlib_integer2full_month( begin_month ) );

	fprintf( output_file,
		 "End Month,%s\n",
		 timlib_integer2full_month( end_month ) );

	fprintf( output_file,
		 "Species" );

	if ( all_species )
	{
		fprintf( output_file, ",All species\n" );
	}
	else
	{
		do {
			species = list_get_pointer( species_list );
	
			fprintf( output_file,
				 ",%d",
				 species->florida_state_code );

		} while( list_next( species_list ) );
	
		fprintf( output_file, "\n" );
	}

	fprintf( output_file,
		 "Areas 1-5 Estimated Kept,%.1lf\n",
		 grand_areas_1_5_estimated_kept );

	fprintf( output_file,
		 "Areas 1-5 Estimated Released,%.1lf\n",
		 grand_areas_1_5_estimated_released );

	fprintf( output_file,
		 "Areas 1-5 Estimated Caught,%.1lf\n",
		 grand_areas_1_5_estimated_caught );

	fprintf( output_file,
		 "Areas 1-6 Estimated Kept,%.1lf\n",
		 grand_areas_1_6_estimated_kept );

	fprintf( output_file,
		 "Areas 1-6 Estimated Released,%.1lf\n",
		 grand_areas_1_6_estimated_released );

	fprintf( output_file,
		 "Areas 1-6 Estimated Caught,%.1lf\n",
		 grand_areas_1_6_estimated_caught );

	fclose( output_file );

	sprintf( ftp_prompt,
	"Grand: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save." );

	appaserver_library_output_ftp_prompt(
		ftp_filename,
		ftp_prompt,
		(char *)0 /* target */,
		(char *)0 /* application_type */ );

	printf( "<br>\n" );

} /* output_grand_total_sheet_text_file() */

void output_month_total_sheet_table(	TOTAL_SHEET *total_sheet,
					LIST *species_list,
					int year,
					boolean all_species )

{
	FILE *output_pipe;
	char sys_string[ 65536 ];
	char *heading;

	heading = get_month_total_sheet_table_heading(
				species_list,
				total_sheet->total_row->catch_area_list,
				year,
				all_species );

	sprintf(sys_string,
		"html_table.e '^^Monthly Total' '%s' '^' left,right",
		heading );

	output_pipe = popen( sys_string, "w" );

	output_total_sheet_total_row_list_table(
				output_pipe,
				total_sheet->total_row_list,
				total_sheet->total_row->catch_area_list,
				all_species );

	output_total_sheet_total_row_table(
				output_pipe,
				total_sheet->total_row,
				total_sheet->total_row->catch_area_list,
				all_species );

	pclose( output_pipe );

} /* output_month_total_sheet_table() */

void output_total_sheet_total_row_list_table(
					FILE *output_pipe,
					LIST *total_row_list,
					LIST *total_row_catch_area_list,
					boolean all_species )
{
	TOTAL_ROW *total_row;
	CATCH_AREA *catch_area;
	CATCH_AREA *total_row_catch_area;

	if ( !list_rewind( total_row_list ) ) return;

	do {
		total_row = list_get_pointer( total_row_list );

		fprintf( output_pipe,
	"%s^%d^%d^%d^%.1lf^%.1lf^%.1lf^%.1lf^%.1lf^%.1lf^%.1lf^%.1lf",
			 total_row->month,
			 total_row->florida_bay_trailer_count,
			 total_row->whitewater_bay_trailer_count,
			 total_row->total_trailer_count,
			 total_row->estimated_areas_1_5_vessels,
			 total_row->estimated_area_6_vessels,
			 total_row->estimated_areas_1_6_vessels,
			 total_row->flamingo_fishing_day,
			 total_row->flamingo_boating_day,
			 total_row->park_fishing_day,
			 total_row->park_boating_day,
			 total_row->estimated_park_fishing_vessels );

		if ( all_species
		||   !list_rewind( total_row->catch_area_list )
		||   !list_rewind( total_row_catch_area_list ) )
		{
			fprintf( output_pipe, "\n" );
			continue;
		}

		do {
			catch_area =
				list_get_pointer(
					total_row->catch_area_list );

			total_row_catch_area =
				list_get_pointer(
					total_row_catch_area_list );

			if ( total_row_catch_area->areas_1_6_estimated_caught )
			{
				fprintf( output_pipe,
				 "^%.1lf^%.1lf^%.1lf^%.1lf^%.1lf^%.1lf",
				 catch_area->
					areas_1_5_estimated_kept,
				 catch_area->
					areas_1_5_estimated_released,
				 catch_area->
					areas_1_5_estimated_caught,
				 catch_area->
					areas_1_6_estimated_kept,
				 catch_area->
					areas_1_6_estimated_released,
				 catch_area->
					areas_1_6_estimated_caught );
			}

			if ( !list_next( total_row_catch_area_list ) )
				break;

		} while( list_next( total_row->catch_area_list ) );

		fprintf( output_pipe, "\n" );

	} while( list_next( total_row_list ) );

} /* output_total_sheet_total_row_list_table() */

void output_total_sheet_total_row_table(
					FILE *output_pipe,
					TOTAL_ROW *total_row,
					LIST *catch_area_list,
					boolean all_species )
{
	CATCH_AREA *catch_area;

	if ( !total_row ) return;

	fprintf( output_pipe,
	 "Total^%d^%d^%d^%.1lf^%.1lf^%.1lf^%.1lf^%.1lf^%.1lf^%.1lf^%.1lf",
	 total_row->florida_bay_trailer_count,
	 total_row->whitewater_bay_trailer_count,
	 total_row->total_trailer_count,
	 total_row->estimated_areas_1_5_vessels,
	 total_row->estimated_area_6_vessels,
	 total_row->estimated_areas_1_6_vessels,
	 total_row->flamingo_fishing_day,
	 total_row->flamingo_boating_day,
	 total_row->park_fishing_day,
	 total_row->park_boating_day,
	 total_row->estimated_park_fishing_vessels );

	if ( all_species
	||   !list_rewind( catch_area_list ) )
	{
		fprintf( output_pipe, "\n" );
		return;
	}

	do {
		catch_area =
			list_get_pointer(
				catch_area_list );

		if ( catch_area->areas_1_6_estimated_caught )
		{
			fprintf( output_pipe,
			 "^%.1lf^%.1lf^%.1lf^%.1lf^%.1lf^%.1lf",
			 catch_area->
				areas_1_5_estimated_kept,
			 catch_area->
				areas_1_5_estimated_released,
			 catch_area->
				areas_1_5_estimated_caught,
			 catch_area->
				areas_1_6_estimated_kept,
			 catch_area->
				areas_1_6_estimated_released,
			 catch_area->
				areas_1_6_estimated_caught );
		}

	} while( list_next( catch_area_list ) );

	fprintf( output_pipe, "\n" );

} /* output_total_sheet_total_row_table() */

char *get_month_total_sheet_table_heading(
				LIST *species_list,
				LIST *catch_area_list,
				int year,
				boolean all_species )
{
	SPECIES *species;
	CATCH_AREA *catch_area;
	char heading[ 65536 ];
	char *ptr = heading;

	ptr += sprintf( ptr,
"%d,Trailers<br>Florida Bay,Trailers<br>Whitewater Bay,Trailers<br>Total,Estimated Vessels<br>Area 1-5,Estimated Vessels<br>Area 6,Estimated Vessels<br>Area 1-6,Sample Interviews<br>Flamingo Fishing,Sample Interviews<br>Flamingo Boating,Sample Interviews<br>Park Fishing,Sample Interviews<br>Park Boating,Estimated Trips<br>Park Fishing",
		 year );

	if ( all_species
	||   !list_rewind( species_list )
	||   !list_rewind( catch_area_list ) )
	{
		return strdup( heading );
	}

	do {
		species = list_get_pointer( species_list );
		catch_area = list_get_pointer( catch_area_list );

		/* Output estimated total */
		/* ---------------------- */
		if ( catch_area->areas_1_6_estimated_caught )
		{
			ptr += sprintf( ptr,
		",Estimated Areas&nbsp;1-5<br>%04d Kept,Estimated Areas&nbsp;1-5<br>%04d Released,Estimated Areas&nbsp;1-5<br>%04d Caught,Estimated Areas&nbsp;1-6<br>%04d Kept,Estimated Areas&nbsp;1-6<br>%04d Released,Estimated Areas&nbsp;1-6<br>%04d Caught",
			 	species->florida_state_code,
			 	species->florida_state_code,
			 	species->florida_state_code,
			 	species->florida_state_code,
			 	species->florida_state_code,
			 	species->florida_state_code );
		}

		if ( !list_next( catch_area_list ) ) break;

	} while( list_next( species_list ) );

	return strdup( heading );

} /* get_month_total_sheet_table_heading() */

void output_month_sheet_list_table(	LIST *month_sheet_list,
					LIST *species_list,
					LIST *total_row_catch_area_list,
					boolean all_species )
{
	MONTH_SHEET *month_sheet;
	FILE *output_pipe;
	char sys_string[ 65536 ];
	char title[ 128 ];
	char *heading;

	if ( !list_rewind( month_sheet_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty month_sheet_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		month_sheet = list_get_pointer( month_sheet_list );

		heading =
			get_month_sheet_table_heading(
				species_list,
				total_row_catch_area_list,
				all_species );

		sprintf(	title,
				"Daily Total for %s",
				timlib_get_three_character_month_string(
					month_sheet->month - 1 ) );

		sprintf(sys_string,
			"html_table.e '^^%s' '%s' '^' left,right",
			title,
			heading );

		output_pipe = popen( sys_string, "w" );

		output_month_sheet_table(
				output_pipe,
				month_sheet,
				total_row_catch_area_list,
				all_species );

		if ( month_sheet->total_row
		&&   month_sheet->total_row->fishing_trip
		&&   month_sheet->total_row->vessel )
		{
			output_month_sheet_total_row_table(
				output_pipe,
				month_sheet->total_row,
				total_row_catch_area_list,
				all_species );
		}

		pclose( output_pipe );

	} while( list_next( month_sheet_list ) );

} /* output_month_sheet_list_table() */

char *get_month_sheet_table_heading(
					LIST *species_list,
					LIST *total_row_catch_area_list,
					boolean all_species )
{
	SPECIES *species;
	CATCH_AREA *catch_area;
	char heading[ 65536 ];
	char *ptr = heading;

	ptr += sprintf( ptr,
"Day,Trailers<br>Florida Bay,Trailers<br>Whitewater Bay,Trailers<br>Total,Estimated Vessels<br>Area 1-5,Estimated Vessels<br>Area 6,Estimated Vessels<br>Area 1-6,Sample Interviews<br>Flamingo Fishing,Sample Interviews<br>Flamingo Boating,Sample Interviews<br>Park Fishing,Sample Interviews<br>Park Boating,Estimated Trips<br>Park Fishing" );

	if ( all_species
	||   !list_rewind( species_list )
	||   !list_rewind( total_row_catch_area_list ) )
	{
		return strdup( heading );
	}

	do {
		species = list_get_pointer( species_list );

		catch_area =
			list_get_pointer(
				total_row_catch_area_list );

		if ( catch_area->areas_1_6_estimated_caught )
		{
			ptr += sprintf( ptr,
		",Estimated Areas 1-5 %04d Kept,Estimated Areas 1-5 %04d Released,Estimated Areas 1-5 %04d Caught,Estimated Areas 1-6 %04d Kept,Estimated Areas 1-6 %04d Released,Estimated Areas 1-6 %04d Caught",
				 	species->florida_state_code,
				 	species->florida_state_code,
				 	species->florida_state_code,
				 	species->florida_state_code,
				 	species->florida_state_code,
				 	species->florida_state_code );
		}

		if ( !list_next( total_row_catch_area_list ) ) break;

	} while( list_next( species_list ) );

	return strdup( heading );

} /* get_month_sheet_table_heading() */

void output_month_sheet_table(		FILE *output_pipe,
					MONTH_SHEET *month_sheet,
					LIST *total_row_catch_area_list,
					boolean all_species )
{
	MONTH_ROW *month_row;
	CATCH_AREA *catch_area;
	CATCH_AREA *total_row_catch_area;

	if ( !list_rewind( month_sheet->row_list ) ) return;

	do {
		month_row = list_get_pointer( month_sheet->row_list );

		fprintf( output_pipe,
"%d^%d^%d^%d^%.0lf^%.0lf^%.0lf^%.1lf^%.1lf^%.1lf^%.1lf^%.1lf",
			 month_row->day,
			 month_row->vessel->florida_bay_trailer_count,
			 month_row->vessel->whitewater_bay_trailer_count,
			 month_row->vessel->total_trailer_count,
			 month_row->vessel->estimated_areas_1_5_vessels,
			 month_row->vessel->estimated_area_6_vessels,
			 month_row->vessel->estimated_areas_1_6_vessels,
			 month_row->fishing_trip->flamingo_fishing_day,
			 month_row->fishing_trip->flamingo_boating_day,
			 month_row->fishing_trip->park_fishing_day,
			 month_row->fishing_trip->park_boating_day,
			 month_row->vessel->estimated_park_fishing_vessels );

		if ( all_species
		||   !list_rewind( month_row->catch_area_list )
		||   !list_rewind( total_row_catch_area_list ) )
		{
			fprintf( output_pipe, "\n" );
			continue;
		}

		do {
			catch_area =
				list_get_pointer(
					month_row->catch_area_list );

			total_row_catch_area =
				list_get_pointer(
					total_row_catch_area_list );

			if ( total_row_catch_area->
				areas_1_6_estimated_caught )
			{
				fprintf( output_pipe,
				 "^%.1lf^%.1lf^%.1lf^%.1lf^%.1lf^%.1lf",
				 	catch_area->
					   areas_1_5_estimated_kept,
				 	catch_area->
					   areas_1_5_estimated_released,
				 	catch_area->
					   areas_1_5_estimated_caught,
				 	catch_area->
					   areas_1_6_estimated_kept,
				 	catch_area->
					   areas_1_6_estimated_released,
				 	catch_area->
					   areas_1_6_estimated_caught );
			}

			if ( !list_next( total_row_catch_area_list ) )
				break;

		} while( list_next( month_row->catch_area_list ) );

		fprintf( output_pipe, "\n" );

	} while( list_next( month_sheet->row_list ) );

} /* output_month_sheet_table() */

void output_month_sheet_total_row_table(
				FILE *output_pipe,
				MONTH_ROW *total_row,
				LIST *total_row_catch_area_list,
				boolean all_species )
{
	CATCH_AREA *catch_area;
	CATCH_AREA *total_row_catch_area;

	if ( !total_row
	||   !total_row->vessel
	||   !total_row->fishing_trip )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty total_row.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	fprintf( output_pipe,
	 "Total^%d^%d^%d^%.0lf^%.0lf^%.0lf^%.0lf^%.0lf^%.0lf^%.0lf^%.0lf",
	 total_row->vessel->florida_bay_trailer_count,
	 total_row->vessel->whitewater_bay_trailer_count,
	 total_row->vessel->total_trailer_count,
	 total_row->vessel->estimated_areas_1_5_vessels,
	 total_row->vessel->estimated_area_6_vessels,
	 total_row->vessel->estimated_areas_1_6_vessels,
	 total_row->fishing_trip->flamingo_fishing_day,
	 total_row->fishing_trip->flamingo_boating_day,
	 total_row->fishing_trip->park_fishing_day,
	 total_row->fishing_trip->park_boating_day,
	 total_row->vessel->estimated_park_fishing_vessels );

	if ( all_species
	||   !list_rewind( total_row_catch_area_list )
	||   !list_rewind( total_row->catch_area_list ) )
	{
		fprintf( output_pipe, "\n" );
		return;
	}

	do {
		catch_area =
			list_get_pointer(
				total_row->catch_area_list );

		total_row_catch_area =
			list_get_pointer(
				total_row_catch_area_list );

		if ( total_row_catch_area->areas_1_6_estimated_caught )
		{
			/* fprintf( output_pipe, "^^^^^^" ); */

			fprintf( output_pipe,
			 "^%.0lf^%.0lf^%.0lf^%.0lf^%.0lf^%.0lf",
		 	catch_area->
				areas_1_5_estimated_kept,
		 	catch_area->
				areas_1_5_estimated_released,
		 	catch_area->
				areas_1_5_estimated_caught,
		 	catch_area->
				areas_1_6_estimated_kept,
		 	catch_area->
				areas_1_6_estimated_released,
		 	catch_area->
				areas_1_6_estimated_caught );
		}

		if ( !list_next( total_row_catch_area_list ) ) break;

	} while( list_next( total_row->catch_area_list ) );

	fprintf( output_pipe, "\n" );

} /* output_month_sheet_total_row_table() */

