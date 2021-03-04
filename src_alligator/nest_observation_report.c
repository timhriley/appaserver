/* ---------------------------------------------------	*/
/* nest_observation_report.c				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
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
#include "appaserver_link_file.h"
#include "alligator.h"

/* Enumerated Types */
/* ---------------- */
enum output_medium { text_file, table };

/* Constants */
/* --------- */
#define ROWS_BETWEEN_HEADING			20
#define DEFAULT_OUTPUT_MEDIUM			table
#define NEST_FILENAME_STEM			"nest"
#define OBSERVATION_FILENAME_STEM		"observation"

/* Prototypes */
/* ---------- */
void get_title_sub_title(
				char *title,
				char *sub_title,
				char *process_name,
				LIST *alligator_census_list,
				boolean with_html );

enum output_medium get_output_medium(
				char *output_medium_string );

void output_text_file(		char *application_name,
				LIST *alligator_census_list,
				LIST *nest_list,
				char *process_name,
				char *document_root_directory );

void output_table(
				LIST *alligator_census_list,
				LIST *nest_list,
				char *process_name );

void output_table_observation_list(
				LIST *observation_list );

void output_text_file_observation_list(
				FILE *observation_output_pipe,
				char *nest_number,
				LIST *observation_list );

int main( int argc, char **argv )
{
	char *application_name;
	char *discovery_date_list_string;
	char *primary_researcher_list_string;
	char *process_name;
	DOCUMENT *document;
	ALLIGATOR *alligator;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	enum output_medium output_medium;
	char *output_medium_string;

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s application process_name discovery_date primary_researcher output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	discovery_date_list_string = argv[ 3 ];
	primary_researcher_list_string = argv[ 4 ];
	output_medium_string = argv[ 5 ];

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

	if ( !* discovery_date_list_string
	||   strcmp( discovery_date_list_string, "discovery_date" ) == 0 )
	{
		printf(
		"<h3>Please select one or more alligator censuses.</h3>\n" );
		document_close();
		exit( 0 );
	}

	alligator = alligator_new(
				application_name,
				discovery_date_list_string,
				primary_researcher_list_string,
				(char *)0 /* begin_discovery_date */,
				(char *)0 /* end_discovery_date */,
				0 /* not with_secondary_researchers */,
				1 /* with_nest_list */,
				1 /* with_observation_list */ );

	if ( output_medium == table )
	{
		output_table(
			alligator->alligator_census_list,
			alligator->nest_list,
			process_name );
	}
	else
	if ( output_medium == text_file )
	{
		output_text_file(
			application_name,
			alligator->alligator_census_list,
			alligator->nest_list,
			process_name,
			appaserver_parameter_file->
				document_root );
	}
	else
	{
		printf( "<p>Error: invalid output medium of (%s)\n",
			output_medium_string );
	}

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void output_table_observation_list( LIST *observation_list )
{
	OBSERVATION *observation;
	LIST *heading_list = list_new();
	HTML_TABLE *html_table = {0};

	html_table = new_html_table(
				(char *)0 /* title */,
				(char *)0 /* sub_title */ );
	
	list_append_pointer( heading_list, "Observation Date" );
	list_append_pointer( heading_list, "Visit#" );
	list_append_pointer( heading_list, "View" );
	list_append_pointer( heading_list, "Status" );
	list_append_pointer( heading_list, "Behavior" );
	list_append_pointer( heading_list, "Hatchlings" );
	list_append_pointer( heading_list, "Hatched" );
	list_append_pointer( heading_list, "Flooded" );
	list_append_pointer( heading_list, "Predated" );
	list_append_pointer( heading_list, "Crushed" );
	list_append_pointer( heading_list, "Other Egg Mortality" );
	list_append_pointer( heading_list, "Water Depth Avg. (cm)" );
	list_append_pointer( heading_list, "Nest Height Above Water (cm)" );
	
	html_table_set_heading_list( html_table, heading_list );
	html_table->number_left_justified_columns = 5;
	html_table->number_right_justified_columns = 99;

	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );

	do {
		observation = list_get_pointer( observation_list );

		html_table_set_data(
				html_table->data_list,
				strdup( observation->observation_date ) );

		html_table_set_data(
				html_table->data_list,
				strdup( observation->nest_visit_number ) );

		html_table_set_data(
				html_table->data_list,
				strdup( observation->observation_view ) );

		html_table_set_data(
				html_table->data_list,
				strdup( observation->nest_status ) );

		html_table_set_data(
				html_table->data_list,
				strdup( observation->female_behavior ) );

		html_table_set_data(
				html_table->data_list,
				strdup( observation->
					hatchlings_count_string ) );

		html_table_set_data(
				html_table->data_list,
				strdup( observation->hatched_eggs_string ) );

		html_table_set_data(
				html_table->data_list,
				strdup( observation->flooded_eggs_string ) );

		html_table_set_data(
				html_table->data_list,
				strdup( observation->predated_eggs_string ) );

		html_table_set_data(
				html_table->data_list,
				strdup( observation->crushed_eggs_string ) );

		html_table_set_data(
				html_table->data_list,
				strdup( observation->
					other_egg_mortality_string ) );

		html_table_set_data(
				html_table->data_list,
				strdup( observation->
					water_depth_average_cm_string ) );

		html_table_set_data(
				html_table->data_list,
				strdup( observation->
					nest_height_above_water_cm_string ) );

		html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new_list();

	} while( list_next( observation_list ) );

	html_table_close();

} /* output_table_observation_list() */

void output_table(
			LIST *alligator_census_list,
			LIST *nest_list,
			char *process_name )
{
	LIST *heading_list = list_new();
	HTML_TABLE *html_table = {0};
	char title[ 128 ];
	char sub_title[ 128 ];
	char buffer[ 512 ];
	NEST *nest;

	get_title_sub_title(
		title,
		sub_title,
		process_name,
		alligator_census_list,
		1 /* with_html */ );

	html_table = new_html_table( title, sub_title );
	
	list_append_pointer( heading_list, "Nest#" );
	list_append_pointer( heading_list, "UTM Easting" );
	list_append_pointer( heading_list, "UTM Northing" );
	list_append_pointer( heading_list, "Monitor?" );
	list_append_pointer( heading_list, "Materials" );
	list_append_pointer( heading_list, "Basin" );
	list_append_pointer( heading_list, "Habitat" );
	list_append_pointer( heading_list, "Transect" );
	list_append_pointer( heading_list, "In Transect?" );
	list_append_pointer( heading_list, "Total Eggs" );
	list_append_pointer( heading_list, "Banded Eggs" );
	list_append_pointer( heading_list, "Not Banded Eggs" );
	
	html_table_set_heading_list( html_table, heading_list );
	html_table->number_left_justified_columns = 7;
	html_table->number_right_justified_columns = 99;

	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	if ( !list_rewind( nest_list ) )
	{
		html_table_close();
		return;
	}

	do {
		nest = list_get_pointer( nest_list );

		html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );

		html_table_set_data(
				html_table->data_list,
				strdup( nest->nest_number_key ) );

		html_table_set_data(
				html_table->data_list,
				strdup( nest->utm_easting ) );

		html_table_set_data(
				html_table->data_list,
				strdup( nest->utm_northing ) );

		if ( nest->monitor_nest_yn != '\0' )
		{
			sprintf( buffer,
			 	"%c",
			 	toupper( nest->monitor_nest_yn ) );
		}
		else
		{
			*buffer = '\0';
		}
		html_table_set_data(
				html_table->data_list,
				strdup( buffer ) );

		format_initial_capital( nest->materials, nest->materials );
		html_table_set_data(
				html_table->data_list,
				strdup( nest->materials ) );

		format_initial_capital( nest->basin, nest->basin);
		html_table_set_data(
				html_table->data_list,
				strdup( nest->basin ) );

		format_initial_capital( nest->habitat, nest->habitat );
		html_table_set_data(
				html_table->data_list,
				strdup( nest->habitat ) );

		html_table_set_data(
				html_table->data_list,
				strdup( nest->transect_number ) );

		if ( nest->in_transect_boundary_yn != '\0' )
		{
			sprintf( buffer,
			 	"%c",
			 	toupper( nest->in_transect_boundary_yn ) );
		}
		else
		{
			*buffer = '\0';
		}
		html_table_set_data(
				html_table->data_list,
				strdup( buffer ) );

		if ( *(nest->total_eggs_string) )
		{
			strcpy( buffer,
				nest->total_eggs_string );
		}
		else
		{
			*buffer = '\0';
		}
		html_table_set_data(
				html_table->data_list,
				strdup( buffer ) );

		if ( *(nest->banded_eggs_string) )
		{
			strcpy( buffer,
				nest->banded_eggs_string );
		}
		else
		{
			*buffer = '\0';
		}
		html_table_set_data(
				html_table->data_list,
				strdup( buffer ) );

		if ( *(nest->not_banded_eggs_string) )
		{
			strcpy( buffer,
				nest->not_banded_eggs_string );
		}
		else
		{
			*buffer = '\0';
		}
		html_table_set_data(
				html_table->data_list,
				strdup( buffer ) );

/*
		if ( *(nest->female_size_cm_string) )
		{
			strcpy( buffer,
				nest->female_size_cm_string );
		}
		else
		{
			*buffer = '\0';
		}
		html_table_set_data(
				html_table->data_list,
				strdup( buffer ) );

		if ( *(nest->nest_height_cm_string ) )
		{
			strcpy( buffer,
				nest->nest_height_cm_string );
		}
		else
		{
			*buffer = '\0';
		}
		html_table_set_data(
				html_table->data_list,
				strdup( buffer ) );
*/

		html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new_list();

		html_table_close();

		if ( nest->observation_list
		&&   list_rewind( nest->observation_list ) )
		{
			output_table_observation_list(
					nest->observation_list );
		}

		html_table_output_table_heading(
					(char *)0 /* title */,
					(char *)0 /* sub_title */ );

	} while( list_next( nest_list ) );

	html_table_close();

} /* output_table() */

enum output_medium get_output_medium( char *output_medium_string )
{
	if ( strcmp( output_medium_string, "output_medium" ) == 0
	||   !*output_medium_string )
	{
		return DEFAULT_OUTPUT_MEDIUM;
	}
	else
	if ( strcmp( output_medium_string, "text_file" ) == 0
	||   strcmp( output_medium_string, "transmit" ) == 0 )
	{
		return text_file;
	}
	else
	if ( strcmp( output_medium_string, "table" ) == 0 )
	{
		return table;
	}
	else
	{
		return DEFAULT_OUTPUT_MEDIUM;
	}
} /* get_output_medium() */

void get_title_sub_title(
		char *title,
		char *sub_title,
		char *process_name,
		LIST *alligator_census_list,
		boolean with_html )
{
	ALLIGATOR_CENSUS *alligator_census;

	strcpy( title, process_name );
	format_initial_capital( title, title );

	*sub_title = '\0';

	if ( list_rewind( alligator_census_list ) )
	{
		do {
			alligator_census =
				list_get_pointer( alligator_census_list );

			sub_title += sprintf(
					sub_title,
					"%s %s",
					alligator_census->discovery_date,
					alligator_census->primary_researcher );

			if ( with_html )
			{
				sub_title += sprintf(
						sub_title,
						"<br>\n" );
			}
			else
			{
				if ( !list_at_end( alligator_census_list ) )
				{
					sub_title += sprintf(
							sub_title,
							";" );
				}
			}

		} while( list_next( alligator_census_list ) );
	}

} /* get_title_sub_title() */

void output_text_file(	char *application_name,
			LIST *alligator_census_list,
			LIST *nest_list,
			char *process_name,
			char *document_root_directory )
{
	char sys_string[ 65536 ];
	char *nest_output_filename;
	char *nest_ftp_filename;
	char *observation_output_filename;
	char *observation_ftp_filename;
	pid_t process_id = getpid();
	FILE *nest_output_file;
	FILE *observation_output_file;
	FILE *nest_output_pipe;
	FILE *observation_output_pipe;
	char title[ 512 ];
	char sub_title[ 512 ];
	NEST *nest;
	int results;
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			NEST_FILENAME_STEM,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	nest_output_filename =
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

	nest_ftp_filename =
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

	get_title_sub_title(
		title,
		sub_title,
		process_name,
		alligator_census_list,
		0 /* not with_html */ );

/*
	sprintf( nest_output_filename, 
		 NEST_OUTPUT_TEMPLATE,
		 appaserver_mount_point,
		 application_name, 
		 process_id );
*/

	/* Test the nest output file */
	/* ------------------------- */
	if ( ! ( nest_output_file = fopen( nest_output_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			nest_output_filename );
		document_close();
		exit( 1 );
	}
	else
	{
		fclose( nest_output_file );
	}

/*
	sprintf( observation_output_filename, 
		 OBSERVATION_OUTPUT_TEMPLATE,
		 appaserver_mount_point,
		 application_name, 
		 process_id );
*/

	appaserver_link_file->filename_stem = OBSERVATION_FILENAME_STEM;

	observation_output_filename =
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

	observation_ftp_filename =
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

	/* Test the observation output file */
	/* -------------------------------- */
	if ( !( observation_output_file =
				fopen( observation_output_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			observation_output_filename );
		document_close();
		exit( 1 );
	}
	else
	{
		fclose( observation_output_file );
	}

	sprintf( sys_string,
	 "sed 's/^/%c/' | sed 's/|/\\%c,\\%c/g' | sed 's/$/%c/' > %s",
		 '"',
		 '"',
		 '"',
		 '"',
	 	 nest_output_filename );

	nest_output_pipe = popen( sys_string, "w" );

	sprintf( sys_string,
	 "sed 's/^/%c/' | sed 's/|/\\%c,\\%c/g' | sed 's/$/%c/' > %s",
		 '"',
		 '"',
		 '"',
		 '"',
	 	 observation_output_filename );

	observation_output_pipe = popen( sys_string, "w" );

	fprintf( nest_output_pipe,
		 "%s Nests\n%s\n", title, sub_title );
	fprintf( observation_output_pipe,
		 "%s Observations\n%s\n", title, sub_title );

	fprintf( nest_output_pipe,
"Nest#|Easting|Northing|Monitor?|Materials|Basin|Habitat|Transect|InTransect?|Total|Banded|Not-Banded\n" );

	fprintf( observation_output_pipe,
"Nest#|Observation|Visit#|View|Status|Behavior|Hatchlings|Hatched|Flooded|Predated|Crushed|OtherEggMortality|WaterDepthAvg(Cm)|NestHeightAboveWater(Cm)\n" );

	if ( !list_rewind( nest_list ) )
	{
		return;
	}

	do {
		nest = list_get_pointer( nest_list );

		fprintf( nest_output_pipe,
			 "%s|%s|%s|%c|%s|%s|%s|%s|%c|%s|%s|%s\n",
			 nest->nest_number_key,
			 nest->utm_easting,
			 nest->utm_northing,
			 nest->monitor_nest_yn,
			 nest->materials,
			 nest->basin,
			 nest->habitat,
			 nest->transect_number,
			 nest->in_transect_boundary_yn,
			 nest->total_eggs_string,
			 nest->banded_eggs_string,
			 nest->not_banded_eggs_string );

		if ( nest->observation_list
		&&   list_rewind( nest->observation_list ) )
		{
			output_text_file_observation_list(
					observation_output_pipe,
			 		nest->nest_number_key,
					nest->observation_list );
		}

	} while( list_next( nest_list ) );

	pclose( nest_output_pipe );
	pclose( observation_output_pipe );

	printf( "<h1>%s<br></h1>\n", title );
	printf( "<h2>\n" );
	fflush( stdout );
	results = system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );
		
	appaserver_library_output_ftp_prompt(
			nest_ftp_filename,
"Nests: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );
	appaserver_library_output_ftp_prompt(
			observation_ftp_filename,
"Observations: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

} /* output_text_file() */

void output_text_file_observation_list(
			FILE *observation_output_pipe,
			char *nest_number,
			LIST *observation_list )
{
	OBSERVATION *observation;

	do {
		observation = list_get_pointer( observation_list );

		fprintf( observation_output_pipe,
		"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
			 nest_number,
			 observation->observation_date,
			 observation->nest_visit_number,
			 observation->observation_view,
			 observation->nest_status,
			 observation->female_behavior,
			 observation->hatchlings_count_string,
			 observation->hatched_eggs_string,
			 observation->flooded_eggs_string,
			 observation->predated_eggs_string,
			 observation->crushed_eggs_string,
			 observation->other_egg_mortality_string,
			 observation->water_depth_average_cm_string,
			 observation->nest_height_above_water_cm_string );
	} while( list_next( observation_list ) );
} /* output_text_file_observation_list() */

