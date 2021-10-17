/* ---------------------------------------------------	*/
/* src_creel/creel_guide_spreadsheet_file.c		*/
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
#include "appaserver_link.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define GUIDE_FISHING_PURPOSE			"guide"
#define FILENAME_STEM_WITH_CODES		"guide_spreadsheet_with_codes"
#define FILENAME_STEM_NO_CODES			"guide_spreadsheet_no_codes"

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
				char *species_code,
				char *species_display );

void output_spreadsheet_fishing_trip_vertically(
				FILE *output_codes_file,
				FILE *output_no_codes_file,
				FISHING_TRIPS fishing_trips,
				DATE *date,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *species_code,
				char *species_display );

void get_title_and_sub_title(
				char *title,
				char *sub_title,
				char *begin_date_string,
				char *end_date_string );

void output_spreadsheet(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *document_root_directory,
				boolean with_between,
				char *display_data,
				pid_t process_id );

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

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s application process_name begin_date end_date horizontally|vertically(display_data)\n",
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

	display_data = argv[ 5 ];

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

	output_spreadsheet(
			application_name,
			begin_date_string,
			end_date_string,
			appaserver_parameter_file->
				document_root,
			with_between,
			display_data,
			getpid() );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */


void output_spreadsheet(
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *document_root_directory,
			boolean with_between,
			char *display_data,
			pid_t process_id )
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
	char species_display[ 128 ];
	char *species_code;
	int results;
	APPASERVER_LINK *appaserver_link;

	appaserver_link =
		appaserver_link_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			(char *)0 /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"csv" );

	if ( with_between )
	{
		appaserver_link->begin_date_string = begin_date_string;
		appaserver_link->end_date_string = end_date_string;
	}
	else
	{
		appaserver_link->begin_date_string = begin_date_string;
	}

	/* With codes */
	/* ========== */
	appaserver_link->filename_stem = FILENAME_STEM_WITH_CODES;

	output_codes_filename =
		appaserver_link_output_filename(
			appaserver_link->document_root_directory,
			appaserver_link_output_tail_half(
				appaserver_link->application_name,
				appaserver_link->filename_stem,
				appaserver_link->begin_date_string,
				appaserver_link->end_date_string,
				appaserver_link->process_id,
				appaserver_link->session_key,
				appaserver_link->extension ) );

	ftp_codes_filename =
		appaserver_link_prompt_filename(
			appaserver_link_prompt_link_half(
				appaserver_link->prepend_http,
				appaserver_link->http_prefix,
				appaserver_link->server_address ),
			appaserver_link->application_name,
			appaserver_link->filename_stem,
			appaserver_link->begin_date_string,
			appaserver_link->end_date_string,
			appaserver_link->process_id,
			appaserver_link->session_key,
			appaserver_link->extension );

	/* Without codes */
	/* ============= */
	appaserver_link->filename_stem = FILENAME_STEM_NO_CODES;

	output_no_codes_filename =
		appaserver_link_output_filename(
			appaserver_link->document_root_directory,
			appaserver_link_output_tail_half(
				appaserver_link->application_name,
				appaserver_link->filename_stem,
				appaserver_link->begin_date_string,
				appaserver_link->end_date_string,
				appaserver_link->process_id,
				appaserver_link->session_key,
				appaserver_link->extension ) );

	ftp_no_codes_filename =
		appaserver_link_prompt_filename(
			appaserver_link_prompt_link_half(
				appaserver_link->prepend_http,
				appaserver_link->http_prefix,
				appaserver_link->server_address ),
			appaserver_link->application_name,
			appaserver_link->filename_stem,
			appaserver_link->begin_date_string,
			appaserver_link->end_date_string,
			appaserver_link->process_id,
			appaserver_link->session_key,
			appaserver_link->extension );

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
"REPORT,PERMIT,DATE: MDY,HRS,POB,TARGET,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE,SPECIES,KEPT,RELEASED,AREA,ZONE\n" );

		fprintf( output_no_codes_file,
"Report,Permit,Date,Fishing Hours,No. People,Preferred,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone,Family/Species,Kept,Released,Area,Zone\n" );
	}
	else
	if ( strcmp( display_data, DISPLAY_CATCHES_VERTICALLY ) == 0 )
	{
		fprintf( output_codes_file,
"REPORT,PERMIT,DATE: MDY,HRS,POB,TARGET,SPECIES,KEPT,RELEASED,AREA,ZONE\n" );

		fprintf( output_no_codes_file,
"Report,Permit,Date,Fishing Hours,No. People,Preferred,Family/Species,Kept,Released,Area,Zone\n" );
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

	while( creel_library_get_fishing_trips_between_dates(
					&fishing_trips,
					application_name,
					GUIDE_FISHING_PURPOSE,
					begin_date_string,
					end_date_string ) )
	{
		*(fishing_trips.interview_time + 2) = '\0';

		date =
			date_yyyy_mm_dd_new(
				fishing_trips.census_date );

		species_code = "";

		if ( *fishing_trips.species_preferred )
		{
			if ( strcmp( fishing_trips.genus, "unknown" ) == 0 )
			{
				strcpy(	species_display,
					fishing_trips.family );
			}
			else
			{
				sprintf(species_display,
					"%s %s %s",
					fishing_trips.family,
					fishing_trips.genus,
					fishing_trips.species_preferred );
			}

			if ( ! ( species_code =
					creel_library_get_species(
					application_name,
					fishing_trips.family,
					fishing_trips.genus,
					fishing_trips.species_preferred,
					(char *)0 /* species_code */ ) ) )
			{
				species_code = species_display;
			}
		}
		else
		{
			*species_display = '\0';
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
				species_code,
				species_display );
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
				species_code,
				species_display );
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

#ifdef NOT_DEFINED
		fprintf(output_codes_file,
			"%s,%s,%s,%s,%s,%s",
			fishing_trips.interview_number,
			fishing_trips.permit_code,
			date_display_mdy( date ),
			fishing_trips.fishing_duration_hours,
			fishing_trips.number_of_people_fishing,
			species_code );

		fprintf(output_no_codes_file,
			"%s,%s,%s,%s,%s,%s",
			fishing_trips.interview_number,
			fishing_trips.permit_code,
			fishing_trips.census_date,
			fishing_trips.fishing_duration_hours,
			fishing_trips.number_of_people_fishing,
			species_display );

		date_free( date );

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
				GUIDE_FISHING_PURPOSE,
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
#endif

	} /* while( creel_library_get_fishing_trips_between_dates() ) */

	fclose( output_codes_file );
	fclose( output_no_codes_file );

	get_title_and_sub_title(
			title,
			sub_title,
			begin_date_string,
			end_date_string );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	results = system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
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

} /* output_spreadsheet() */


void get_title_and_sub_title(
			char *title,
			char *sub_title,
			char *begin_date_string,
			char *end_date_string )
{
	char buffer[ 128 ];

	sprintf(title,
		"Output %s Spreadsheet File",
		format_initial_capital( buffer, GUIDE_FISHING_PURPOSE ) );

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
			char *species_code,
			char *species_display )
{
	CATCHES catches;
	int location_area;
	char location_zone[ 16 ];
	int str_len;

	fprintf(output_codes_file,
		"%s,%s,%s,%s,%s,%s",
		fishing_trips.interview_number,
		fishing_trips.permit_code,
		date_display_mdy( date ),
		fishing_trips.fishing_duration_hours,
		fishing_trips.number_of_people_fishing,
		species_code );

	fprintf(output_no_codes_file,
		"%s,%s,%s,%s,%s,%s",
		fishing_trips.interview_number,
		fishing_trips.permit_code,
		fishing_trips.census_date,
		fishing_trips.fishing_duration_hours,
		fishing_trips.number_of_people_fishing,
		species_display );

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
			GUIDE_FISHING_PURPOSE,
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
			char *species_code,
			char *species_display )
{
	CATCHES catches;
	int got_one = 0;
	int location_area;
	char location_zone[ 16 ];
	int str_len;

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
			fishing_trips.fishing_purpose,
			fishing_trips.census_date,
			fishing_trips.interview_location,
			fishing_trips.interview_number,
			begin_date_string,
			end_date_string ) )
	{
		got_one = 1;

		fprintf(output_codes_file,
			"%s,%s,%s,%s,%s,%s",
			fishing_trips.interview_number,
			fishing_trips.permit_code,
			date_display_mdy( date ),
			fishing_trips.fishing_duration_hours,
			fishing_trips.number_of_people_fishing,
			species_code );

		fprintf(output_no_codes_file,
			"%s,%s,%s,%s,%s,%s",
			fishing_trips.interview_number,
			fishing_trips.permit_code,
			fishing_trips.census_date,
			fishing_trips.fishing_duration_hours,
			fishing_trips.number_of_people_fishing,
			species_display );

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
			"%s,%s,%s,%s,%s,%s\n",
			fishing_trips.interview_number,
			fishing_trips.permit_code,
			date_display_mdy( date ),
			fishing_trips.fishing_duration_hours,
			fishing_trips.number_of_people_fishing,
			species_code );

		fprintf(output_no_codes_file,
			"%s,%s,%s,%s,%s,%s\n",
			fishing_trips.interview_number,
			fishing_trips.permit_code,
			fishing_trips.census_date,
			fishing_trips.fishing_duration_hours,
			fishing_trips.number_of_people_fishing,
			species_display );
	}

} /* output_spreadsheet_fishing_trip_vertically() */

