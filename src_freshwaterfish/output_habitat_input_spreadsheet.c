/* ---------------------------------------------------	*/
/* output_habitat_input_spreadsheet.c			*/
/* ---------------------------------------------------	*/
/* This process outputs a spreadsheet file that		*/
/* closely matches the habitat input spreadsheet.	*/
/*							*/
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
#include "hash_table.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "freshwaterfish_library.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void output_throw_first_half(	FILE *output_file,
				THROW *throw,
				char *visit_date_compressed );

void output_throw_second_half(	FILE *output_file,
				THROW *throw );

void get_title_and_sub_title(
				char *title,
				char *sub_title,
				char *begin_date_string,
				char *end_date_string,
				char *process_name );

void output_spreadsheet(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *document_root_directory,
				char *process_name,
				char output_scientific_name_yn );

void output_repeating_throws(	FILE *output_file,
				char *application_name,
				THROW *throw,
				char *visit_date_compressed,
				char output_scientific_name_yn );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *begin_date_string;
	char *end_date_string;
	char output_scientific_name_yn;

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s application process_name begin_date end_date output_scientific_name_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	begin_date_string = argv[ 3 ];
	end_date_string = argv[ 4 ];
	output_scientific_name_yn = *argv[ 5 ];

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

	if ( !*begin_date_string
	||   strcmp( begin_date_string, "begin_date" ) == 0 )
	{
		printf( "<h3>Please enter a begin date.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !*end_date_string
	||   strcmp( end_date_string, "end_date" ) == 0 )
	{
		printf( "<h3>Please enter an end date.</h3>\n" );
		document_close();
		exit( 0 );
	}

	output_spreadsheet(
			application_name,
			begin_date_string,
			end_date_string,
			appaserver_parameter_file->
				document_root,
			process_name,
			output_scientific_name_yn );

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
			char *process_name,
			char output_scientific_name_yn )
{
	char *output_filename;
	char *ftp_filename;
	FILE *output_file;
	THROW throw;
	char *visit_date_compressed;
	char title[ 128 ];
	char sub_title[ 128 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			process_name,
			application_name,
			0 /* process_id */,
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


/*
	sprintf(output_filename, 
	 	OUTPUT_TEMPLATE,
	 	appaserver_mount_point,
	 	application_name, 
	 	begin_date_string,
		end_date_string );
*/

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H3>ERROR: Cannot open output file %s\n",
			output_filename );
		document_close();
		exit( 1 );
	}

	fprintf( output_file,
"DATE,YEAR,MONTH,DAY,SITE,PLOT,THROW,LONGGRID,SHORTGRID,TIME,DEPTH,VEGSP,CODE,DENSITY,DENSITYD,DP,PLHT,PEREMERPLCVR,PERTOPL,PERPERICVR,PERIVOL,TEMP,DO,COND\n" );


	while( freshwaterfish_get_throw_between_dates(
					&throw,
					application_name,
					begin_date_string,
					end_date_string,
					AGENCY_NPS ) )
	{
		if ( ! ( visit_date_compressed =
			freshwaterfish_get_visit_date_compressed(
				throw.visit_date ) ) )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: invalid visit date = (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 throw.visit_date );
		}

		output_repeating_throws(
				output_file,
				application_name,
				&throw,
				visit_date_compressed,
				output_scientific_name_yn );
	}

	fclose( output_file );

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
			ftp_filename,
"&lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

} /* output_spreadsheet() */


void output_repeating_throws(	FILE *output_file,
				char *application_name,
				THROW *throw,
				char *visit_date_compressed,
				char output_scientific_name_yn )
{
	SURFACE_VEGETATION *surface_vegetation;
	SUBMERGENT_VEGETATION *submergent_vegetation;
	char *vegsp_column;
	char vegsp_column_up_string[ 128 ];

	if ( !list_length( throw->surface_vegetation_list )
	&&   !list_length( throw->submergent_vegetation_list ) )
	{
		output_throw_first_half(
				output_file,
				throw,
				visit_date_compressed );

		fprintf(output_file, ",,,,,," );

		output_throw_second_half(
				output_file,
				throw );
		return;
	}

	if ( list_rewind( throw->surface_vegetation_list ) )
	{
		do {
			surface_vegetation =
				list_get_pointer(
					throw->surface_vegetation_list );
	
			output_throw_first_half(
					output_file,
					throw,
					visit_date_compressed );
	
			if ( output_scientific_name_yn == 'y')
			{
				vegsp_column = 
					surface_vegetation->scientific_name;
			}
			else
			{
				vegsp_column = 
					freshwaterfish_get_nps_code(
						application_name,
						surface_vegetation->
							scientific_name );
				strcpy( vegsp_column_up_string, vegsp_column );
				up_string( vegsp_column_up_string );
				vegsp_column = vegsp_column_up_string;
			}

			fprintf(output_file,
				",%s,%s,%s,%s,D,",
				vegsp_column,
				freshwaterfish_get_fiu_non_fish_old_code(
					application_name,
					surface_vegetation->
						scientific_name ),
				(*surface_vegetation->density_alive_count)
					? surface_vegetation->
						density_alive_count
					: ".",
				(*surface_vegetation->density_dead_count)
					? surface_vegetation->
						density_dead_count
					: "." );
	
			output_throw_second_half(
					output_file,
					throw );
		} while( list_next( throw->surface_vegetation_list ) );
	}

	if ( list_rewind( throw->submergent_vegetation_list ) )
	{
		do {
			submergent_vegetation =
				list_get_pointer(
					throw->submergent_vegetation_list );
	
			output_throw_first_half(
					output_file,
					throw,
					visit_date_compressed );
	
			if ( output_scientific_name_yn == 'y')
			{
				vegsp_column = 
					submergent_vegetation->scientific_name;
			}
			else
			{
				vegsp_column = 
					freshwaterfish_get_nps_code(
						application_name,
						submergent_vegetation->
							scientific_name );
				strcpy( vegsp_column_up_string, vegsp_column );
				up_string( vegsp_column_up_string );
				vegsp_column = vegsp_column_up_string;
			}

			fprintf(output_file,
				",%s,%s,%s,.,P,",
				vegsp_column,
				freshwaterfish_get_fiu_non_fish_old_code(
					application_name,
					submergent_vegetation->
						scientific_name ),
				(*submergent_vegetation->density_percent)
					? submergent_vegetation->
						density_percent
					: "." );
	
			output_throw_second_half(
					output_file,
					throw );
		} while( list_next( throw->submergent_vegetation_list ) );
	}

} /* output_repeating_throws() */

void output_throw_first_half(	FILE *output_file,
				THROW *throw,
				char *visit_date_compressed )
{
		fprintf(output_file,
"%s,%d,%d,%d,%s,%c,%d,%d,%d,%s,%s",
			visit_date_compressed,
			freshwaterfish_get_visit_year( throw->visit_date ),
			freshwaterfish_get_visit_month( throw->visit_date ),
			freshwaterfish_get_visit_day( throw->visit_date ),
			throw->site,
			*throw->plot,
			throw->throw_number,
			throw->long_grid_number,
			throw->short_grid_number,
			throw->throw_time,
			(*throw->water_depth_cm)
				? throw->water_depth_cm
				: "." );

} /* output_throw_first_half() */

void output_throw_second_half(	FILE *output_file,
				THROW *throw )
{
		fprintf(output_file,
"%s,%s,%s,%s,%s,%s,%s,%s\n",
			(*throw->average_plant_height_cm)
				? throw->average_plant_height_cm
				: ".",
			(*throw->emergence_plant_cover_percent)
				? throw->emergence_plant_cover_percent
				: ".",
			(*throw->total_plant_cover_percent)
				? throw->total_plant_cover_percent
				: ".",
			(*throw->periphyton_cover_percent)
				? throw->periphyton_cover_percent
				: ".",
			(*throw->periphyton_volume_milliliter)
				? throw->periphyton_volume_milliliter
				: ".",
			(*throw->temperature_celsius)
				? throw->temperature_celsius
				: ".",
			(*throw->disolved_oxygen_mg_per_liter)
				? throw->disolved_oxygen_mg_per_liter
				: ".",
			(*throw->conductivity)
				? throw->conductivity
				: "." );

} /* output_throw_second_half() */

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

