/* ---------------------------------------------------	*/
/* src_creel/species_preferred_species_caught.c		*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
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
#include "hash_table.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "creel_library.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define GUIDE_FISHING_PURPOSE			"guide"
#define FILENAME_STEM_WITH_CODES		"preferred_caught_with_codes"
#define FILENAME_STEM_NO_CODES			"preferred_caught_no_codes"

/* Prototypes */
/* ---------- */
void output_spreadsheet_fishing_trip_horizontally(
				FILE *output_codes_file,
				FILE *output_no_codes_file,
				FISHING_TRIPS fishing_trips,
				DATE *date,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *species_preferred_code,
				char *species_preferred_display,
				char *fishing_purpose );

void output_spreadsheet_fishing_trip_vertically(
				FILE *output_codes_file,
				FILE *output_no_codes_file,
				FISHING_TRIPS fishing_trips,
				DATE *date,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *species_preferred_code,
				char *species_preferred_display,
				char *fishing_purpose );

void output_spreadsheet_fishing_trip_filtered(
				FILE *output_codes_file,
				FILE *output_no_codes_file,
				FISHING_TRIPS fishing_trips,
				DATE *date,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *species_preferred_code,
				char *species_preferred_display,
				char *fishing_purpose,
				char *family,
				char *genus,
				char *species_preferred );

void get_title_and_sub_title(
				char *title,
				char *sub_title,
				char *begin_date_string,
				char *end_date_string,
				char *process_name );

void species_preferred_species_caught(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *document_root_directory,
				boolean with_between,
				char *display_data,
				pid_t process_id,
				char *process_name,
				char *family,
				char *genus,
				char *species_preferred,
				char *fishing_purpose );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *begin_date_string;
	char *end_date_string;
	boolean with_between;
	char *display_data;
	char *family;
	char *genus;
	char *species_preferred;
	char *fishing_purpose;

	if ( argc != 10 )
	{
		fprintf( stderr,
"Usage: %s application process_name begin_date end_date family genus species_preferred fishing_purpose horizontally|vertically(display_data)\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	process_name = argv[ 2 ];
	begin_date_string = argv[ 3 ];
	end_date_string = argv[ 4 ];

	if ( !*end_date_string
	||   strcmp( end_date_string, "end_date" ) == 0 )
	{
		with_between = 0;
	}
	else
	{
		with_between = 1;
	}

	family = argv[ 5 ];
	genus = argv[ 6 ];
	species_preferred = argv[ 7 ];

	fishing_purpose = argv[ 8 ];

	display_data = argv[ 9 ];

	if (	!*display_data
	||	strcmp( display_data, "display_data" ) == 0 )
	{
		display_data = DISPLAY_CATCHES_HORIZONTALLY;
	}

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

appaserver_parameter_file->appaserver_mount_point = "/appaserver";

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

	if ( !*family || strcmp( family, "family" ) == 0 )
	{
		printf( "<h3>Please select a species.</h3>\n" );
		document_close();
		exit( 0 );
	}

	species_preferred_species_caught(
			application_name,
			begin_date_string,
			end_date_string,
			appaserver_parameter_file->document_root,
			with_between,
			display_data,
			getpid(),
			process_name,
			family,
			genus,
			species_preferred,
			fishing_purpose );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void species_preferred_species_caught(
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *document_root_directory,
			boolean with_between,
			char *display_data,
			pid_t process_id,
			char *process_name,
			char *family,
			char *genus,
			char *species_preferred,
			char *fishing_purpose )
{
	char *ftp_codes_filename;
	char *output_codes_filename;
	FILE *output_codes_file;
	char *ftp_no_codes_filename;
	char *output_no_codes_filename;
	FILE *output_no_codes_file;
	FISHING_TRIPS fishing_trips;
	char title[ 128 ];
	char sub_title[ 128 ];
	DATE *date;
	char species_preferred_display[ 128 ];
	char *species_preferred_code;
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
		   application_http_prefix(
				application_name ),
		   appaserver_library_server_address(),
		   ( application_prepend_http_protocol_yn(
			application_name ) == 'y' ),
		   document_root_directory,
		   (char *)0 /* filename_stem */,
		   application_name,
		   process_id,
		   (char *)0 /* session */,
		   "csv" );

	if ( with_between )
	{
		appaserver_link_file->begin_date_string = begin_date_string;
		appaserver_link_file->end_date_string = end_date_string;
	}
	else
	{
		appaserver_link_file->begin_date_string = begin_date_string;
	}

	/* With codes */
	/* ========== */
	appaserver_link_file->filename_stem = FILENAME_STEM_WITH_CODES;

	output_codes_filename =
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

	ftp_codes_filename =
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

	/* Without codes */
	/* ============= */
	appaserver_link_file->filename_stem = FILENAME_STEM_NO_CODES;

	output_no_codes_filename =
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

	ftp_no_codes_filename =
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

/*
	if ( with_between )
	{
		sprintf(output_codes_filename, 
		 	OUTPUT_CODES_WITH_BETWEEN,
		 	appaserver_mount_point,
		 	application_name, 
		 	begin_date_string,
			end_date_string,
			process_id );

		sprintf(output_no_codes_filename, 
		 	OUTPUT_NO_CODES_WITH_BETWEEN,
		 	appaserver_mount_point,
		 	application_name, 
		 	begin_date_string,
			end_date_string,
			process_id );
	}
	else
	{
		sprintf(output_codes_filename, 
		 	OUTPUT_CODES_NO_BETWEEN,
		 	appaserver_mount_point,
		 	application_name, 
		 	begin_date_string,
			process_id );

		sprintf(output_no_codes_filename, 
		 	OUTPUT_NO_CODES_NO_BETWEEN,
		 	appaserver_mount_point,
		 	application_name, 
		 	begin_date_string,
			process_id );
	}
*/

	if ( ! ( output_codes_file = fopen( output_codes_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			output_codes_filename );
		document_close();
		exit( 1 );
	}

	if ( ! ( output_no_codes_file =
			fopen( output_no_codes_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			output_no_codes_filename );
		document_close();
		exit( 1 );
	}

	if ( !*display_data
	||   strcmp( display_data, DISPLAY_CATCHES_HORIZONTALLY ) == 0 )
	{
		fprintf( output_codes_file,
"REPORT,LOCATION,PERMIT,DATE: MDY,HRS,POB,TARGET,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE\n" );

		fprintf( output_no_codes_file,
"Report,Location,Permit,Date,Fishing Hours,No. Fishing,Preferred,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone\n" );
	}
	else
	if ( strcmp( display_data, DISPLAY_CATCHES_VERTICALLY ) == 0
	||   strcmp( display_data, DISPLAY_CATCHES_FILTERED ) == 0 )
	{
		fprintf( output_codes_file,
"REPORT,LOCATION,PERMIT,DATE: MDY,HRS,POB,TARGET,SPECIES,KEPT,RELEASED,AREA,ZONE\n" );

		fprintf( output_no_codes_file,
"Report,Location,Permit,Date,Fishing Hours,No. Fishing,Preferred,Family/Species,Kept,Released,Area,Zone\n" );
	}
	else
	{
		fprintf( stderr,
			"ERROR in %s/%s()/%d: invalid display_data = (%s)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			display_data );
		exit( 1 );
	}

	while( creel_library_get_fishing_trips_between_dates_preferred_caught(
					&fishing_trips,
					application_name,
					begin_date_string,
					end_date_string,
					family,
					genus,
					species_preferred,
					fishing_purpose ) )
	{
		*(fishing_trips.interview_time + 2) = '\0';

		date =
			date_yyyy_mm_dd_new(
				fishing_trips.census_date );

		species_preferred_code = "";

		if ( *fishing_trips.species_preferred )
		{
			if ( strcmp( fishing_trips.genus, "unknown" ) == 0 )
			{
				strcpy(	species_preferred_display,
					fishing_trips.family );
			}
			else
			{
				sprintf(species_preferred_display,
					"%s %s %s",
					fishing_trips.family,
					fishing_trips.genus,
					fishing_trips.species_preferred );
			}

			if ( ! ( species_preferred_code =
					creel_library_get_species(
					application_name,
					fishing_trips.family,
					fishing_trips.genus,
					fishing_trips.species_preferred,
					(char *)0 /* species_code */ ) ) )
			{
				species_preferred_code =
					species_preferred_display;

				fprintf( stderr,
	"Warning in %s/%s()/%d: cannot find species_code for %s/%s/%s.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__,
					 fishing_trips.family,
					 fishing_trips.genus,
					 fishing_trips.species_preferred );
			}
		}
		else
		{
			*species_preferred_display = '\0';
		}

		if ( !*display_data
		||   strcmp(	display_data,
				DISPLAY_CATCHES_HORIZONTALLY ) == 0 )
		{
			output_spreadsheet_fishing_trip_horizontally(
				output_codes_file,
				output_no_codes_file,
				fishing_trips,
				date,
				application_name,
				begin_date_string,
				end_date_string,
				species_preferred_code,
				species_preferred_display,
				fishing_purpose );
		}
		else
		if ( strcmp( display_data,
				DISPLAY_CATCHES_VERTICALLY ) == 0 )
		{
			output_spreadsheet_fishing_trip_vertically(
				output_codes_file,
				output_no_codes_file,
				fishing_trips,
				date,
				application_name,
				begin_date_string,
				end_date_string,
				species_preferred_code,
				species_preferred_display,
				fishing_purpose );
		}
		else
		if ( strcmp( display_data,
				DISPLAY_CATCHES_FILTERED ) == 0 )
		{
			output_spreadsheet_fishing_trip_filtered(
				output_codes_file,
				output_no_codes_file,
				fishing_trips,
				date,
				application_name,
				begin_date_string,
				end_date_string,
				species_preferred_code,
				species_preferred_display,
				fishing_purpose,
				family,
				genus,
				species_preferred );
		}
		else
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: invalid display_data = (%s)\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				display_data );
			exit( 1 );
		}

	}

	fclose( output_codes_file );
	fclose( output_no_codes_file );

	get_title_and_sub_title(
			title,
			sub_title,
			begin_date_string,
			end_date_string,
			process_name );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );
		
	appaserver_library_output_ftp_prompt(
			ftp_codes_filename,
"With codes: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	appaserver_library_output_ftp_prompt(
			ftp_no_codes_filename,
"Without codes: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );

} /* species_preferred_species_caught() */


void get_title_and_sub_title(
			char *title,
			char *sub_title,
			char *begin_date_string,
			char *end_date_string,
			char *process_name )
{
	char buffer[ 128 ];

	sprintf(title,
		"%s",
		format_initial_capital( buffer, process_name ) );

	sprintf(	sub_title,
			"From %s To %s",
			begin_date_string,
			end_date_string );

} /* get_title_and_sub_title() */

void output_spreadsheet_fishing_trip_horizontally(
			FILE *output_codes_file,
			FILE *output_no_codes_file,
			FISHING_TRIPS fishing_trips,
			DATE *date,
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *species_preferred_code,
			char *species_preferred_display,
			char *fishing_purpose )
{
	CATCHES catches;
	int location_area;
	char location_zone[ 16 ];
	int str_len;
	char *species_code;
	char *interview_location_code;

	interview_location_code =
		creel_library_get_interview_location(
				application_name,
				fishing_trips.interview_location,
				(char *)0 /* interview_location_code */ );

	fprintf(output_codes_file,
		"%s,%s,%s,%s,%s,%s,%s",
		fishing_trips.interview_number,
		interview_location_code,
		fishing_trips.permit_code,
		date_display_mdy( date ),
		fishing_trips.fishing_duration_hours,
		fishing_trips.number_of_people_fishing,
		species_preferred_code );

	fprintf(output_no_codes_file,
		"%s,%s,%s,%s,%s,%s,%s",
		fishing_trips.interview_number,
		fishing_trips.interview_location,
		fishing_trips.permit_code,
		fishing_trips.census_date,
		fishing_trips.fishing_duration_hours,
		fishing_trips.number_of_people_fishing,
		species_preferred_display );

	location_area = atoi( fishing_trips.fishing_area );

	str_len = strlen( fishing_trips.fishing_area );
	if ( str_len
	&&   isalpha( *(fishing_trips.fishing_area +
			str_len - 1 ) ) )
	{
		*location_zone =
			*(fishing_trips.fishing_area +
			str_len - 1 );
		*(location_zone + 1) = '\0';
	}
	else
	{
		*location_zone = '\0';
	}

	while( creel_library_census_date_get_catches(
			&catches,
			application_name,
			fishing_purpose,
			fishing_trips.census_date,
			fishing_trips.interview_location,
			fishing_trips.interview_number,
			begin_date_string,
			end_date_string ) )
	{
		if ( ! ( species_code =
				creel_library_get_species(
				application_name,
			 	catches.family,
			 	catches.genus,
			 	catches.species,
				(char *)0 /* species_code */ ) ) )
		{
			species_code = catches.species;
		}

		fprintf( output_codes_file,
			 ",%s,%d,%d,%d,%s",
			 species_code,
			 catches.kept_integer,
			 catches.released_integer,
			 location_area,
			 location_zone );

		if ( strcmp( catches.genus, "unknown" ) == 0 )
		{
			fprintf( output_no_codes_file,
			 	",%s,%d,%d,%d,%s",
			 	catches.family,
			 	catches.kept_integer,
			 	catches.released_integer,
				location_area,
				location_zone );
		}
		else
		{
			fprintf( output_no_codes_file,
			 	",%s %s %s,%d,%d,%d,%s",
			 	catches.family,
			 	catches.genus,
			 	catches.species,
			 	catches.kept_integer,
			 	catches.released_integer,
				location_area,
				location_zone );
		}
	}

	fprintf(output_codes_file, "\n" );
	fprintf(output_no_codes_file, "\n" );
} /* output_spreadsheet_fishing_trip_horizontally() */

void output_spreadsheet_fishing_trip_vertically(
			FILE *output_codes_file,
			FILE *output_no_codes_file,
			FISHING_TRIPS fishing_trips,
			DATE *date,
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *species_preferred_code,
			char *species_preferred_display,
			char *fishing_purpose )
{
	CATCHES catches;
	int got_one = 0;
	int location_area;
	char location_zone[ 16 ];
	int str_len;
	char *species_code;
	char *interview_location_code;

	location_area = atoi( fishing_trips.fishing_area );

	str_len = strlen( fishing_trips.fishing_area );
	if ( str_len
	&&   isalpha( *(fishing_trips.fishing_area +
			str_len - 1 ) ) )
	{
		*location_zone =
			*(fishing_trips.fishing_area +
			str_len - 1 );
		*(location_zone + 1) = '\0';
	}
	else
	{
		*location_zone = '\0';
	}

	interview_location_code =
		creel_library_get_interview_location(
				application_name,
				fishing_trips.interview_location,
				(char *)0 /* interview_location_code */ );

	while( creel_library_census_date_get_catches(
			&catches,
			application_name,
			fishing_purpose,
			fishing_trips.census_date,
			fishing_trips.interview_location,
			fishing_trips.interview_number,
			begin_date_string,
			end_date_string ) )
	{
		got_one = 1;

		fprintf(output_codes_file,
			"%s,%s,%s,%s,%s,%s,%s",
			fishing_trips.interview_number,
			interview_location_code,
			fishing_trips.permit_code,
			date_display_mdy( date ),
			fishing_trips.fishing_duration_hours,
			fishing_trips.number_of_people_fishing,
			species_preferred_code );

		fprintf(output_no_codes_file,
			"%s,%s,%s,%s,%s,%s,%s",
			fishing_trips.interview_number,
			fishing_trips.interview_location,
			fishing_trips.permit_code,
			fishing_trips.census_date,
			fishing_trips.fishing_duration_hours,
			fishing_trips.number_of_people_fishing,
			species_preferred_display );

		if ( ! ( species_code =
				creel_library_get_species(
				application_name,
			 	catches.family,
			 	catches.genus,
			 	catches.species,
				(char *)0 /* species_code */ ) ) )
		{
			species_code = catches.species;
		}


		fprintf( output_codes_file,
			 ",%s,%d,%d,%d,%s\n",
			 species_code,
			 catches.kept_integer,
			 catches.released_integer,
			 location_area,
			 location_zone );

		if ( strcmp( catches.genus, "unknown" ) == 0 )
		{
			fprintf( output_no_codes_file,
			 	",%s,%d,%d,%d,%s\n",
			 	catches.family,
			 	catches.kept_integer,
			 	catches.released_integer,
				location_area,
				location_zone );
		}
		else
		{
			fprintf( output_no_codes_file,
			 	",%s %s %s,%d,%d,%d,%s\n",
			 	catches.family,
			 	catches.genus,
			 	catches.species,
			 	catches.kept_integer,
			 	catches.released_integer,
				location_area,
				location_zone );
		}
	}

	if ( !got_one )
	{
		fprintf(output_codes_file,
			"%s,%s,%s,%s,%s,%s,%s\n",
			fishing_trips.interview_number,
			interview_location_code,
			fishing_trips.permit_code,
			date_display_mdy( date ),
			fishing_trips.fishing_duration_hours,
			fishing_trips.number_of_people_fishing,
			species_preferred_code );

		fprintf(output_no_codes_file,
			"%s,%s,%s,%s,%s,%s,%s\n",
			fishing_trips.interview_number,
			fishing_trips.interview_location,
			fishing_trips.permit_code,
			fishing_trips.census_date,
			fishing_trips.fishing_duration_hours,
			fishing_trips.number_of_people_fishing,
			species_preferred_display );
	}

} /* output_spreadsheet_fishing_trip_vertically() */

void output_spreadsheet_fishing_trip_filtered(
			FILE *output_codes_file,
			FILE *output_no_codes_file,
			FISHING_TRIPS fishing_trips,
			DATE *date,
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *species_preferred_code,
			char *species_preferred_display,
			char *fishing_purpose,
			char *family,
			char *genus,
			char *species_preferred )
{
	CATCHES catches;
	int got_one = 0;
	int location_area;
	char location_zone[ 16 ];
	int str_len;
	char *species_code;
	char *interview_location_code;

	location_area = atoi( fishing_trips.fishing_area );

	str_len = strlen( fishing_trips.fishing_area );
	if ( str_len
	&&   isalpha( *(fishing_trips.fishing_area +
			str_len - 1 ) ) )
	{
		*location_zone =
			*(fishing_trips.fishing_area +
			str_len - 1 );
		*(location_zone + 1) = '\0';
	}
	else
	{
		*location_zone = '\0';
	}

	interview_location_code =
		creel_library_get_interview_location(
				application_name,
				fishing_trips.interview_location,
				(char *)0 /* interview_location_code */ );

	while( creel_library_census_date_get_catches(
			&catches,
			application_name,
			fishing_purpose,
			fishing_trips.census_date,
			fishing_trips.interview_location,
			fishing_trips.interview_number,
			begin_date_string,
			end_date_string ) )
	{
		if ( strcmp( catches.family, family ) != 0
		||   strcmp( catches.genus, genus ) != 0
		||   strcmp( catches.species, species_preferred ) != 0 )
		{
			continue;
		}

		got_one = 1;

		fprintf(output_codes_file,
			"%s,%s,%s,%s,%s,%s,%s",
			fishing_trips.interview_number,
			interview_location_code,
			fishing_trips.permit_code,
			date_display_mdy( date ),
			fishing_trips.fishing_duration_hours,
			fishing_trips.number_of_people_fishing,
			species_preferred_code );

		fprintf(output_no_codes_file,
			"%s,%s,%s,%s,%s,%s,%s",
			fishing_trips.interview_number,
			fishing_trips.interview_location,
			fishing_trips.permit_code,
			fishing_trips.census_date,
			fishing_trips.fishing_duration_hours,
			fishing_trips.number_of_people_fishing,
			species_preferred_display );

		if ( ! ( species_code =
				creel_library_get_species(
				application_name,
			 	catches.family,
			 	catches.genus,
			 	catches.species,
				(char *)0 /* species_code */ ) ) )
		{
			species_code = catches.species;
		}


		fprintf( output_codes_file,
			 ",%s,%d,%d,%d,%s\n",
			 species_code,
			 catches.kept_integer,
			 catches.released_integer,
			 location_area,
			 location_zone );

		if ( strcmp( catches.genus, "unknown" ) == 0 )
		{
			fprintf( output_no_codes_file,
			 	",%s,%d,%d,%d,%s\n",
			 	catches.family,
			 	catches.kept_integer,
			 	catches.released_integer,
				location_area,
				location_zone );
		}
		else
		{
			fprintf( output_no_codes_file,
			 	",%s %s %s,%d,%d,%d,%s\n",
			 	catches.family,
			 	catches.genus,
			 	catches.species,
			 	catches.kept_integer,
			 	catches.released_integer,
				location_area,
				location_zone );
		}
	}

	if ( !got_one )
	{
		fprintf(output_codes_file,
			"%s,%s,%s,%s,%s,%s,%s",
			fishing_trips.interview_number,
			interview_location_code,
			fishing_trips.permit_code,
			date_display_mdy( date ),
			fishing_trips.fishing_duration_hours,
			fishing_trips.number_of_people_fishing,
			species_preferred_code );

		fprintf(output_codes_file,
		 	",%s,0,0,%d,%s\n",
			species_preferred_code,
			location_area,
			location_zone );

		fprintf(output_no_codes_file,
			"%s,%s,%s,%s,%s,%s,%s",
			fishing_trips.interview_number,
			fishing_trips.interview_location,
			fishing_trips.permit_code,
			fishing_trips.census_date,
			fishing_trips.fishing_duration_hours,
			fishing_trips.number_of_people_fishing,
			species_preferred_display );

		fprintf(output_no_codes_file,
		 	",%s,0,0,%d,%s\n",
			species_preferred_display,
			location_area,
			location_zone );
	}

} /* output_spreadsheet_fishing_trip_filtered() */

