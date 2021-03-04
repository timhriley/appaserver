/* -----------------------------------------------------------	*/
/* $APPASERVER_HOME/src_creel/estimated_fishing_effort_hours.c	*/
/* -----------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------------	*/

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
#define ROWS_BETWEEN_HEADING			20
#define DEFAULT_OUTPUT_MEDIUM			table

#define TOTALS_FILENAME_LABEL	"monthly_totals"

/* Prototypes */
/* ---------- */
void output_month_sheet_stdout(		char *month_abbreviation,
					MONTH_SHEET *month_sheet );

void output_month_sheet_list_stdout(	LIST *month_sheet_list );

void output_month_sheet_total_row_table(
					FILE *output_pipe,
					MONTH_ROW *total_row,
					LIST *total_row_catch_area_list,
					boolean all_species );

void output_month_sheet_table(		FILE *output_pipe,
					MONTH_SHEET *month_sheet );

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
					LIST *total_row_list );

void output_month_total_sheet_table(	TOTAL_SHEET *total_sheet,
					LIST *species_list,
					int year,
					boolean all_species );

void output_total_sheet_total_row_list_text_file(
					FILE *output_file,
					LIST *total_row_list );

void output_table(			ESTIMATED_TOTAL_CAUGHT_OUTPUT *output,
					int begin_month,
					int end_month,
					int year );

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
					int year,
					char *process_name );

void output_month_total_sheet_text_file(TOTAL_SHEET *total_sheet,
					int begin_month,
					int end_month,
					int year,
					char *document_root_directory,
					pid_t process_id,
					char *application_name,
					char *process_name );

void output_month_sheet_text_file(	FILE *output_file,
					MONTH_SHEET *month_sheet );

void output_text_file(			ESTIMATED_TOTAL_CAUGHT_OUTPUT *output,
					int begin_month,
					int end_month,
					char *application_name,
					char *document_root_directory,
					int year,
					char *process_name );

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
	char *output_medium_string;
	ESTIMATED_TOTAL_CAUGHT *estimated_total_caught;

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s application process_name begin_month end_month year output_medium\n",
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
	output_medium_string = argv[ 6 ];

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

	appaserver_parameter_file = appaserver_parameter_file_new();

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

	estimated_total_caught->input->fishing_trip_hash_table =
		total_caught_get_fishing_trip_hash_table(
			estimated_total_caught->
				input->fishing_trip_list_array,
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
			1 /* omit_catch_area_list */ );

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
		printf( "Sport and Guide Combined\n" );
		fflush( stdout );
		system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
		fflush( stdout );
		printf( "</h2>\n" );
	}

	if ( output_medium == table )
	{
		output_table(	estimated_total_caught->output,
				estimated_total_caught->input->begin_month,
				estimated_total_caught->input->end_month,
				estimated_total_caught->input->year );

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
				estimated_total_caught->input->year,
				process_name );

		document_close();
	}
	else
	if ( output_medium == output_medium_stdout )
	{
		output_month_sheet_list_stdout(
			estimated_total_caught->output->month_sheet_list );
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
			int year )
{
	MONTH_SHEET *month_sheet;
	TOTAL_ROW *total_row;
	FILE *output_pipe;
	char sys_string[ 65536 ];
	char title[ 128 ];
	char begin_month_string[ 8 ];
	char end_month_string[ 8 ];
	char *estimated_park_fishing_vessels_string;
	char *park_effort_hours_day_string;

	if (	output->total_sheet &&
		output->total_sheet->total_row )
	{
		strcpy(	begin_month_string,
			timlib_get_three_character_month_string(
				begin_month - 1 ) );

		strcpy(	end_month_string,
			timlib_get_three_character_month_string(
				end_month - 1 ) );

		sprintf(	title,
				"Estimated Park-wide Total for %s-%s/%d",
				begin_month_string,
				end_month_string,
				year );

		sprintf(sys_string,
			"html_table.e '^^%s' '%s' '^' right",
			title,
			"Estimated Trips Park Fishing,Effort Hours" );

		output_pipe = popen( sys_string, "w" );

		park_effort_hours_day_string =
			strdup( place_commas_in_integer(
		 		(int)round_double(
					output->
						total_sheet->
						total_row->
						park_effort_hours_day ) ) );

		estimated_park_fishing_vessels_string =
			strdup( place_commas_in_integer(
		 		(int)round_double(
		 			output->
					total_sheet->
					total_row->
					estimated_park_fishing_vessels ) ) );

		fprintf(output_pipe,
		 	"%s^%s\n",
			estimated_park_fishing_vessels_string,
			park_effort_hours_day_string );

		free( estimated_park_fishing_vessels_string );
		free( park_effort_hours_day_string );

		pclose( output_pipe );
		fflush( stdout );
	}

	if (	output->total_sheet &&
		list_rewind( output->total_sheet->total_row_list ) )
	{
		sprintf(title,
			"Estimated Park-wide Total for %d",
			year );

		sprintf(sys_string,
			"html_table.e '^^%s' '%s' '^' right",
			title,
			"Month,Estimated Trips Park Fishing,Effort Hours" );

		output_pipe = popen( sys_string, "w" );

		do {
			total_row =
				list_get_pointer(
					output->total_sheet->total_row_list );

			park_effort_hours_day_string =
				strdup( place_commas_in_integer(
		 		(int)round_double(
						total_row->
						park_effort_hours_day ) ) );

			estimated_park_fishing_vessels_string =
				strdup( place_commas_in_integer(
		 		(int)round_double(
					total_row->
					estimated_park_fishing_vessels ) ) );

			fprintf(output_pipe,
		 		"%s^%s^%s\n",
			 	total_row->month,
				estimated_park_fishing_vessels_string,
				park_effort_hours_day_string );

			free( estimated_park_fishing_vessels_string );
			free( park_effort_hours_day_string );

/*
			fprintf(output_pipe,
			 	"%s^%.1lf^%s\n",
			 	total_row->month,
			 	total_row->estimated_park_fishing_vessels,
			 	place_commas_in_integer(
			 		(int)round_double(
						total_row->
						park_effort_hours_day ) ) );
*/

		} while( list_next( output->total_sheet->total_row_list ) );

		pclose( output_pipe );
	}

	if ( list_rewind( output->month_sheet_list ) )
	{
		do {
			month_sheet =
				list_get_pointer(
					output->month_sheet_list );

			sprintf(title,
				"Estimated Park-wide Total for %s/%d",
				timlib_get_three_character_month_string(
					month_sheet->month - 1 ),
				year );

			sprintf(sys_string,
				"html_table.e '^^%s' '%s' '^' right",
				title,
			"Day,Estimated Trips Park Fishing,Effort Hours" );

			output_pipe = popen( sys_string, "w" );

			output_month_sheet_table(
					output_pipe,
					month_sheet );

			pclose( output_pipe );

		} while( list_next( output->month_sheet_list ) );
	}

	printf(
"<br><a href=\"/src_creel/species_estimated_total_caught_algorithm.pdf\">Press to view the algorithm.</a><br>\n" );

} /* output_table() */

void output_text_file(	ESTIMATED_TOTAL_CAUGHT_OUTPUT *output,
			int begin_month,
			int end_month,
			char *application_name,
			char *document_root_directory,
			int year,
			char *process_name )
{
	pid_t process_id = getpid();

	output_month_total_sheet_text_file(
					output->total_sheet,
					begin_month,
					end_month,
					year,
					document_root_directory,
					process_id,
					application_name,
					process_name );

	output_month_sheet_list_text_file(
			output->month_sheet_list,
			document_root_directory,
			process_id,
			application_name,
			year,
			process_name );

	printf(
"<br><a href=\"/src_creel/species_estimated_total_caught_algorithm.pdf\">Press to view the algorithm.</a><br>\n" );

} /* output_text_file() */

void output_month_sheet_list_text_file(
					LIST *month_sheet_list,
					char *document_root_directory,
					pid_t process_id,
					char *application_name,
					int year,
					char *process_name )
{
	MONTH_SHEET *month_sheet;
	char *output_filename;
	char *ftp_filename;
	char ftp_prompt[ 256 ];
	char *month_abbreviation;
	FILE *output_file;
	char year_string[ 8 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			process_name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	sprintf( year_string, "%d", year );
	appaserver_link_file->begin_date_string = year_string;

	if ( !list_rewind( month_sheet_list ) ) return;

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

		fprintf( output_file,
			 "Estimated Park-wide Total for %s/%d\n",
		 	 timlib_integer2full_month( month_sheet->month ),
			 year );

		fprintf( output_file,
			 "Day,Estimated Trips Park Fishing,Effort Hours\n" );

		output_month_sheet_text_file(
				output_file,
				month_sheet );

		if ( month_sheet->total_row )
		{
			fprintf(output_file,
			 	"Total,%.1lf,%.1lf\n",
			 	month_sheet->
					total_row->
					vessel->
					estimated_park_fishing_vessels,
			 	month_sheet->
					total_row->
					fishing_trip->
					park_effort_hours_day );
		}
		else
		{
			fprintf(output_file,
			 	"Total,0.0,0.0\n" );
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

void output_month_sheet_text_file(	FILE *output_file,
					MONTH_SHEET *month_sheet )
{
	MONTH_ROW *month_row;

	if ( !list_rewind( month_sheet->row_list ) ) return;

	do {
		month_row = list_get_pointer( month_sheet->row_list );

		fprintf( output_file,
			 "%d,%.1lf,%.1lf\n",
			 month_row->day,
			 month_row->vessel->estimated_park_fishing_vessels,
			 month_row->fishing_trip->park_effort_hours_day );

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

void output_month_total_sheet_text_file(TOTAL_SHEET *total_sheet,
					int begin_month,
					int end_month,
					int year,
					char *document_root_directory,
					pid_t process_id,
					char *application_name,
					char *process_name )

{
	char *output_filename;
	char *ftp_filename;
	char ftp_prompt[ 256 ];
	FILE *output_file;
	char buffer[ 256 ];
	char *begin_month_string;
	char *end_month_string;
	char end_date_string[ 16 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			process_name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	begin_month_string =
		timlib_integer2full_month(
			begin_month );

	appaserver_link_file->begin_date_string =
		strdup( begin_month_string );

	end_month_string =
		timlib_integer2full_month(
			end_month );

	sprintf( end_date_string, "%s-%d", end_month_string, year );

	appaserver_link_file->end_date_string =
		strdup( end_date_string );

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

	begin_month_string =
		timlib_get_three_character_month_string(
			begin_month - 1 );

	end_month_string =
		timlib_get_three_character_month_string(
			end_month - 1 );

	fprintf(	output_file,
			"Estimated Park-wide Total for %s-%s/%d\n",
			begin_month_string,
			end_month_string,
			year );

	fprintf(	output_file,
			"Month,%s,%s\n",
			"Estimated Trips Park Fishing",
			"Effort Hours" );

	if (	total_sheet &&
		total_sheet->total_row )
	{
		output_total_sheet_total_row_list_text_file(
				output_file,
				total_sheet->total_row_list );

		fprintf(	output_file,
				"Total,%d,%d\n",
			 	(int)round_double(
					total_sheet->
					total_row->
					estimated_park_fishing_vessels ),
			 	(int)round_double(
					total_sheet->
					total_row->
					park_effort_hours_day ) );
	}

	fclose( output_file );

	sprintf( ftp_prompt,
	"%s: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
		 format_initial_capital( buffer,
		 			 TOTALS_FILENAME_LABEL ) );

	appaserver_library_output_ftp_prompt(
		ftp_filename,
		ftp_prompt,
		(char *)0 /* target */,
		(char *)0 /* application_type */ );

	printf( "<br>\n" );

} /* output_month_total_sheet_text_file() */

void output_total_sheet_total_row_list_text_file(
					FILE *output_file,
					LIST *total_row_list )
{
	TOTAL_ROW *total_row;

	if ( !list_rewind( total_row_list ) ) return;

	do {
		total_row = list_get_pointer( total_row_list );

		fprintf( output_file,
			 "%s,%d,%d\n",
			 total_row->month,
			 	(int)round_double(
			 		total_row->
					estimated_park_fishing_vessels ),
			 	(int)round_double(
			 		total_row->
					park_effort_hours_day ) );

	} while( list_next( total_row_list ) );

} /* output_total_sheet_total_row_list_text_file() */

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

	list_rewind( catch_area_list );

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

void output_month_sheet_table(		FILE *output_pipe,
					MONTH_SHEET *month_sheet )
{
	MONTH_ROW *month_row;

	if ( !list_rewind( month_sheet->row_list ) ) return;

	do {
		month_row = list_get_pointer( month_sheet->row_list );

		fprintf( output_pipe,
			 "%d^%.1lf^%s\n",
			 month_row->day,
			 month_row->vessel->estimated_park_fishing_vessels,
			 place_commas_in_integer(
			 	(int)round_double(
					month_row->
						fishing_trip->
						park_effort_hours_day ) ) );

	} while( list_next( month_sheet->row_list ) );

} /* output_month_sheet_table() */

void output_month_sheet_list_stdout(	LIST *month_sheet_list )
{
	MONTH_SHEET *month_sheet;
	char *month_abbreviation;

	if ( !list_rewind( month_sheet_list ) ) return;

	do {
		month_sheet = list_get_pointer( month_sheet_list );

		month_abbreviation =
			timlib_get_three_character_month_string(
				month_sheet->month - 1 );

		output_month_sheet_stdout(
				month_abbreviation,
				month_sheet );

	} while( list_next( month_sheet_list ) );

} /* output_month_sheet_list_stdout() */

void output_month_sheet_stdout(		char *month_abbreviation,
					MONTH_SHEET *month_sheet )
{
	MONTH_ROW *month_row;

	if ( !list_rewind( month_sheet->row_list ) ) return;

	printf( "%s\n", month_abbreviation );

	do {
		month_row = list_get_pointer( month_sheet->row_list );

		printf(
			"%d,%.0lf,%.0lf\n",
			 month_row->day,
			 month_row->vessel->estimated_park_fishing_vessels,
			 month_row->fishing_trip->park_effort_hours_day );

	} while( list_next( month_sheet->row_list ) );

} /* output_month_sheet_stdout() */

