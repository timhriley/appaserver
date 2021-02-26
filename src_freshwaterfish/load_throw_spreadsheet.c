/* ---------------------------------------------------	*/
/* load_throw_spreadsheet.c				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "freshwaterfish_library.h"
#include "environ.h"
#include "date_convert.h"
#include "application.h"
#include "process.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */
#define THROW_JUSTIFY_COMMA_LIST "right,left,left,right,right,right,right,right,right,right,right,right,right,right,right,right,right,right,right"

/* Prototypes */
/* ---------- */
void delete_throws(
				char *application_name,
				char *input_filename );

int insert_throws(		char *application_name,
				char *input_filename,
				char really_yn );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *input_filename;
	char buffer[ 128 ];
	char really_yn;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	int record_count;

	if ( argc != 5 )
	{
		fprintf( stderr, 
"Usage: %s application process filename really_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	input_filename = argv[ 3 ];
	really_yn = *argv[ 4 ];

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

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	printf( "<h2>%s\n", format_initial_capital( buffer, process_name ) );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( !freshwaterfish_exists_region(
					application_name,
					NPS_REGION_SHARK_RIVER_SLOUGH ) )
	{
		printf(
"<h3>Error. A region name was changed. It is expected to be %s.</h3>\n",
			NPS_REGION_SHARK_RIVER_SLOUGH );
		document_close();
		exit( 0 );
	}

	if ( !freshwaterfish_exists_region(
					application_name,
					NPS_REGION_NORTHEAST_SHARK_SLOUGH ) )
	{
		printf(
"<h3>Error. A region name was changed. It is expected to be %s.</h3>\n",
			NPS_REGION_NORTHEAST_SHARK_SLOUGH );
		document_close();
		exit( 0 );
	}

	if ( !freshwaterfish_exists_agency(
					application_name,
					AGENCY_NPS ) )
	{
		printf(
"<h3>Error. An agency name was changed. It is expected to be %s.</h3>\n",
			AGENCY_NPS );
		document_close();
		exit( 0 );
	}

	if ( really_yn == 'y' )
	{
		delete_throws( application_name, input_filename );
	}

	record_count = insert_throws(
				application_name,
				input_filename,
				really_yn );

	if ( really_yn == 'y' )
	{
		printf( "<p>Process complete with %d records.\n",
			record_count );
	}
	else
	{
		printf( "<p>Process not executed with %d records.\n",
			 record_count );
	}

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

#define DELETE_FIELD_LIST	"site,plot,region,visit_date"

void delete_throws(	char *application_name,
			char *input_filename )
{
	FILE *input_file;
	FILE *site_visit_delete_pipe;
	FILE *throw_delete_pipe;
	FILE *surface_vegetation_delete_pipe;
	FILE *submergent_vegetation_delete_pipe;
	char sys_string[ 1024 ];
	char input_string[ 1024 ];
	char site[ 128 ];
	char plot[ 128 ];
	char *region;
	char visit_date_compressed[ 256 ];
	char *visit_date_international;
	char *table_name;

	table_name =
		get_table_name(
			application_name, "site_visit" );

	sprintf( sys_string,
"sort -u | delete_statement.e t=%s f=%s d='|' | sql.e 2>&1",
		 table_name,
		 DELETE_FIELD_LIST );

	site_visit_delete_pipe = popen( sys_string, "w" );

	table_name =
		get_table_name(
			application_name, "throw" );

	sprintf( sys_string,
"sort -u | delete_statement.e t=%s f=%s d='|' | sql.e 2>&1",
		 table_name,
		 DELETE_FIELD_LIST );

	throw_delete_pipe = popen( sys_string, "w" );

	table_name =
		get_table_name(
			application_name, "surface_vegetation" );

	sprintf( sys_string,
"sort -u | delete_statement.e t=%s f=%s d='|' | sql.e 2>&1",
		 table_name,
		 DELETE_FIELD_LIST );

	surface_vegetation_delete_pipe = popen( sys_string, "w" );

	table_name =
		get_table_name(
			application_name, "submergent_vegetation" );

	sprintf( sys_string,
"sort -u | delete_statement.e t=%s f=%s d='|' | sql.e 2>&1",
		 table_name,
		 DELETE_FIELD_LIST );

	submergent_vegetation_delete_pipe = popen( sys_string, "w" );

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	while( get_line( input_string, input_file ) )
	{
		trim_character( input_string, '"', input_string );
		trim_character( input_string, '\'', input_string );

		if ( !piece(	visit_date_compressed,
				',',
				input_string,
				HABITAT_VISIT_DATE_PIECE ) )
		{
			continue;
		}

		if ( strcmp( visit_date_compressed, "DATE" ) == 0 ) continue;

		visit_date_international =
		     freshwaterfish_get_visit_date_international(
				visit_date_compressed );

		if ( !piece(	site,
				',',
				input_string,
				HABITAT_SITE_PIECE ) )
		{
			continue;
		}

		strcpy( site, freshwaterfish_get_clean_site( site ) );

		if ( !piece(	plot,
				',',
				input_string,
				HABITAT_PLOT_PIECE ) )
		{
			continue;
		}

		if ( ! ( region = freshwaterfish_get_nps_region( site ) ) )
		{
			continue;
		}

		fprintf(site_visit_delete_pipe,
			"%s|%s|%s|%s\n",
			site,
		     	plot,
			region,
			visit_date_international );

		fprintf(throw_delete_pipe,
			"%s|%s|%s|%s\n",
			site,
		     	plot,
			region,
			visit_date_international );

		fprintf(surface_vegetation_delete_pipe,
			"%s|%s|%s|%s\n",
			site,
		     	plot,
			region,
			visit_date_international );

		fprintf(submergent_vegetation_delete_pipe,
			"%s|%s|%s|%s\n",
			site,
		     	plot,
			region,
			visit_date_international );
	}

	fclose( input_file );
	pclose( site_visit_delete_pipe );
	pclose( throw_delete_pipe );
	pclose( surface_vegetation_delete_pipe );
	pclose( submergent_vegetation_delete_pipe );

} /* delete_throws() */

int insert_throws(	char *application_name,
			char *input_filename,
			char really_yn )
{
	FILE *input_file;
	FILE *site_visit_output_pipe = {0};
	FILE *throw_output_pipe = {0};
	FILE *surface_vegetation_output_pipe = {0};
	FILE *submergent_vegetation_output_pipe = {0};
	char input_string[ 1024 ];
	char *scientific_name;
	char fiu_non_fish_old_code[ 128 ];
	char nps_code[ 128 ];
	char *visit_date_international;
	char *now_date_international;
	char site[ 128 ];
	char plot[ 128 ];
	char *region;
	char visit_date_compressed[ 128 ];
	char long_grid_number[ 128 ];
	char short_grid_number[ 128 ];
	char throw_number[ 128 ];
	char throw_time[ 128 ];
	char water_depth_cm[ 128 ];
	char average_plant_height_cm[ 128 ];
	char emergence_plant_cover_percent[ 128 ];
	char total_plant_cover_percent[ 128 ];
	char periphyton_cover_percent[ 128 ];
	char periphyton_volume_milliliter[ 128 ];
	char temperature_celsius[ 128 ];
	char disolved_oxygen_mg_per_liter[ 128 ];
	char conductivity[ 128 ];
	char density_n[ 128 ];
	char density_o[ 128 ];
	char surface_submergent_flag[ 128 ];
	char sys_string[ 1024 ];
	char error_filename[ 128 ];
	FILE *error_file;
	int line_number = 0;
	int record_count = 0;
	char region_buffer[ 128 ];

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	sprintf( error_filename,
		 "/tmp/freshwaterfish_throw_error_%d.txt",
		 getpid() );
	if ( ! ( error_file = fopen( error_filename, "w" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		fclose( input_file );
		exit( 1 );
	}

	if ( really_yn == 'y' )
	{
		char *table_name;

		/* Site Visit */
		/* ---------- */
		table_name =
			get_table_name( application_name, "site_visit" );

		sprintf( sys_string,
"sed 's/|\\./|/g' | insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 SITE_VISIT_FIELD_LIST );
		site_visit_output_pipe = popen( sys_string, "w" );

		/* Throw */
		/* ----- */
		table_name =
			get_table_name( application_name, "throw" );

		sprintf( sys_string,
"sed 's/|\\./|/g' | insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 THROW_FIELD_LIST );
		throw_output_pipe = popen( sys_string, "w" );

		/* Surface Vegetation */
		/* ------------------ */
		table_name =
			get_table_name( application_name,
					"surface_vegetation" );

		sprintf( sys_string,
"sed 's/|\\./|/g' | insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 SURFACE_VEGETATION_FIELD_LIST );
		surface_vegetation_output_pipe = popen( sys_string, "w" );

		/* Submergent Vegetation */
		/* --------------------- */
		table_name =
			get_table_name( application_name,
					"submergent_vegetation");

		sprintf( sys_string,
"sed 's/|\\./|/g' | insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 SUBMERGENT_VEGETATION_FIELD_LIST );
		submergent_vegetation_output_pipe = popen( sys_string, "w" );

	}
	else
	{
		sprintf( sys_string,
"sed 's/|\\./|/g' | sort -u | queue_top_bottom_lines.e 50 | html_table.e 'Insert into Throw' %s '|' %s",
			 THROW_FIELD_LIST,
			 THROW_JUSTIFY_COMMA_LIST );
		throw_output_pipe = popen( sys_string, "w" );
	}

	now_date_international =
		date_get_now_date_yyyy_mm_dd(
			date_get_utc_offset() );

	while( get_line( input_string, input_file ) )
	{
		line_number++;
		trim_character( input_string, '"', input_string );
		trim_character( input_string, '\'', input_string );

		if ( !piece(	visit_date_compressed,
				',',
				input_string,
				HABITAT_VISIT_DATE_PIECE ) )
		{
			continue;
		}

		if ( strcmp( visit_date_compressed, "DATE" ) == 0
		||   !*visit_date_compressed )
		{
			continue;
		}

		visit_date_international =
		     freshwaterfish_get_visit_date_international(
				visit_date_compressed );

		if ( strcmp(	visit_date_international,
				now_date_international ) > 0 )
		{
			fprintf(error_file,
			"Warning in line %d: date is in the future = (%s)\n",
				line_number,
				visit_date_compressed );
			continue;
		}

		if ( !piece(	site,
				',',
				input_string,
				HABITAT_SITE_PIECE ) )
		{
			fprintf(error_file,
			"Warning in line %d: Cannot piece site in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		strcpy( site, freshwaterfish_get_clean_site( site ) );

		if ( !piece(	plot,
				',',
				input_string,
				HABITAT_PLOT_PIECE ) )
		{
			fprintf(error_file,
			"Warning in line %d: Cannot piece plot in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( ! ( region = freshwaterfish_get_nps_region( site ) ) )
		{
			fprintf(error_file,
"Warning in line %d: Cannot get NPS region for site = %s in (%s). This information is hardcoded into the source code. Please contact Tim at (916) 641-7789.\n",
				line_number,
				site,
				input_string );
			continue;
		}

		if ( !piece(	throw_number,
				',',
				input_string,
				HABITAT_THROW_NUMBER_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece throw number in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece(	long_grid_number,
				',',
				input_string,
				HABITAT_LONG_GRID_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece long grid number in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece(	short_grid_number,
				',',
				input_string,
				HABITAT_SHORT_GRID_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece short grid number in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece(	throw_time,
				',',
				input_string,
				HABITAT_THROW_TIME_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece throw time in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		freshwaterfish_library_pad_time_four( throw_time );

		if ( !piece(	water_depth_cm,
				',',
				input_string,
				HABITAT_WATER_DEPTH_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece water depth in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece(	average_plant_height_cm,
				',',
				input_string,
				HABITAT_PLANT_HEIGHT_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece plant height in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece(	emergence_plant_cover_percent,
				',',
				input_string,
				HABITAT_EMERGENCE_PLANT_COVER_PIECE ) )
		{
			fprintf(error_file,
	"Warning in line %d: Cannot piece emergence plant cover in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece(	total_plant_cover_percent,
				',',
				input_string,
				HABITAT_TOTAL_PLANT_COVER_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece total plant cover in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece(	periphyton_cover_percent,
				',',
				input_string,
				HABITAT_PERIPHYTON_COVER_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece periphyton cover in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece(	periphyton_volume_milliliter,
				',',
				input_string,
				HABITAT_PERIPHYTON_VOLUME_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece periphyton volume in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece(	temperature_celsius,
				',',
				input_string,
				HABITAT_TEMPERATURE_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece temperature in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece(	disolved_oxygen_mg_per_liter,
				',',
				input_string,
				HABITAT_OXYGEN_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece oxygen in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece(	conductivity,
				',',
				input_string,
				HABITAT_CONDUCTIVITY_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece conductivity in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece(	density_n,
				',',
				input_string,
				HABITAT_DENSITY_N_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece density N in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece(	density_o,
				',',
				input_string,
				HABITAT_DENSITY_O_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece density O in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece(	surface_submergent_flag,
				',',
				input_string,
				HABITAT_SURFACE_SUBMERGENT_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece flag in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece(	nps_code,
				',',
				input_string,
				HABITAT_NPS_CODE_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece nps code in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece(	fiu_non_fish_old_code,
				',',
				input_string,
				HABITAT_FIU_NON_FISH_OLD_CODE_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece old fiu code in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( ! ( scientific_name =
			freshwaterfish_library_get_scientific_name(
				nps_code,
				(char *)0 /* fiu_fish_old_code */,
				fiu_non_fish_old_code,
				(char *)0 /* fiu_new_code */,
				application_name ) ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot get scientific name for either (%s) or (%s) in (%s)\n",
				line_number,
				nps_code,
				fiu_non_fish_old_code,
				input_string );
			continue;
		}

		if ( really_yn == 'y' )
		{
			fprintf(throw_output_pipe,
				"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
				site,
				plot,
				region,
				visit_date_international,
				throw_number,
				long_grid_number,
				short_grid_number,
				throw_time,
				water_depth_cm,
				average_plant_height_cm,
				emergence_plant_cover_percent,
				total_plant_cover_percent,
				periphyton_cover_percent,
				periphyton_volume_milliliter );
		}
		else
		{
			fprintf(throw_output_pipe,
				"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
				site,
				plot,
				format_initial_capital( region_buffer, region ),
				visit_date_international,
				throw_number,
				long_grid_number,
				short_grid_number,
				throw_time,
				water_depth_cm,
				average_plant_height_cm,
				emergence_plant_cover_percent,
				total_plant_cover_percent,
				periphyton_cover_percent,
				periphyton_volume_milliliter );
		}

		if ( really_yn == 'y' )
		{
			fprintf(site_visit_output_pipe,
				"%s|%s|%s|%s|%s|%s|%s|%s\n",
				site,
				plot,
				region,
				visit_date_international,
				AGENCY_NPS,
				temperature_celsius,
				disolved_oxygen_mg_per_liter,
				conductivity );

			if ( *surface_submergent_flag == 'D' )
			{
				if ( scientific_name )
				{
					fprintf(surface_vegetation_output_pipe,
						"%s|%s|%s|%s|%s|%s|%s|%s\n",
						site,
						plot,
						region,
						visit_date_international,
						throw_number,
						scientific_name,
						density_n,
						density_o );
				}
			}
			else
			if ( *surface_submergent_flag == 'P' )
			{
				if ( scientific_name )
				{
					fprintf(
					      submergent_vegetation_output_pipe,
						"%s|%s|%s|%s|%s|%s|%s\n",
						site,
						plot,
						region,
						visit_date_international,
						throw_number,
						scientific_name,
						density_n );
				}
			}
			else
			{
				fprintf(error_file,
"Warning in line %d: invalid surface/vegetation flag (D/P) of (%s) in (%s)\n",
					line_number,
					surface_submergent_flag,
					input_string );
			}
		}

		record_count++;
	}

	fclose( input_file );
	fclose( error_file );

	pclose( throw_output_pipe );

	if ( really_yn == 'y' )
	{
		pclose( site_visit_output_pipe );
		pclose( surface_vegetation_output_pipe );
		pclose( submergent_vegetation_output_pipe );
	}

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Throw Errors' '' '|'",
			 error_filename );
		system( sys_string );
	}

	sprintf( sys_string, "rm %s", error_filename );
	system( sys_string );
	return record_count;

} /* insert_throws() */

