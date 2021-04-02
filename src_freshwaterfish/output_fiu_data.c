/* ---------------------------------------------------	*/
/* src_freshwaterfish/output_fiu_data.c			*/
/* ---------------------------------------------------	*/
/* This process outputs a series of spreadsheet files	*/
/* that are sent to FIU.				*/
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
#define FILENAME_STEM_FISH			"Tfish_for_FIU"
#define FILENAME_STEM_CRAYFISH			"TCrayfish_for_FIU"
#define FILENAME_STEM_VEGETATION		"TVeg_for_FIU"
#define FILENAME_STEM_PERIPHYTON		"TPHYS_for_FIU"
#define FILENAME_STEM_NONFISH			"TINVandOV_for_FIU"
#define FILENAME_STEM_WETWEIGHT			"TWetweight_for_FIU"

/* Prototypes */
/* ---------- */
void output_species_group_weight(
				FILE *output_file,
				LIST *species_group_list,
				THROW_WEIGHT_COUNT *throw_count );

void output_wet_weights_data(
				char *application_name,
				int year,
				int begin_month_integer,
				int end_month_integer,
				char *document_root_directory );

void output_throw_counts(
				FILE *output_file,
				LIST *species_list,
				THROW_WEIGHT_COUNT *throw_count,
				boolean with_extra_heading_comma );

boolean is_ignore_vegetation(	char *vegetation_scientific_name );

void output_fish_data(		char *application_name,
				int year,
				int begin_month_integer,
				int end_month_integer,
				char *document_root_directory );

void output_inverts_verts_data(	char *application_name,
				int year,
				int begin_month_integer,
				int end_month_integer,
				char *document_root_directory );

void output_fish_measurements(	FILE *output_file,
				char *application_name,
				THROW *throw );

void output_crayfish_data(	char *application_name,
				int year,
				int begin_month_integer,
				int end_month_integer,
				char *document_root_directory );

void output_crayfish_measurements(
				FILE *output_file,
				char *application_name,
				THROW *throw );

void output_vegetation_data(	char *application_name,
				int year,
				int begin_month_integer,
				int end_month_integer,
				char *document_root_directory );

void output_vegetation_densities(
				FILE *output_file,
				char *application_name,
				THROW *throw );

void output_periphyton_data(	char *application_name,
				int year,
				int begin_month_integer,
				int end_month_integer,
				char *document_root_directory );

char *get_length_column(	char *length_millimeters,
				char *no_measurement_count );

char *get_region_column(	char *site );

char *get_event_column(		int month_integer );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *year_integer_string;
	char *begin_month_integer_string;
	char *end_month_integer_string;
	char buffer[ 128 ];

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s application process_name year begin_month_integer end_month_integer\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	year_integer_string = argv[ 3 ];
	begin_month_integer_string = argv[ 4 ];
	end_month_integer_string = argv[ 5 ];

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

	if ( !*year_integer_string
	||   strcmp( year_integer_string, "year" ) == 0
	||   atoi( year_integer_string ) < 1950
	||   atoi( year_integer_string ) > 2050 )
	{
		printf( "<h3>Please enter a valid year integer.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !*begin_month_integer_string
	||   strcmp( begin_month_integer_string, "begin_month" ) == 0
	||   atoi( begin_month_integer_string ) < 1
	||   atoi( begin_month_integer_string ) > 12 )
	{
		begin_month_integer_string = "1";
	}

	if ( !*end_month_integer_string
	||   strcmp( end_month_integer_string, "end_month" ) == 0
	||   atoi( end_month_integer_string ) < 1
	||   atoi( end_month_integer_string ) > 12 )
	{
		end_month_integer_string = "12";
	}

	printf( "<h1>%s</h1>\n",
		format_initial_capital( buffer, process_name ) );
	printf( "<h2>\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

	output_fish_data(
			application_name,
			atoi( year_integer_string ),
			atoi( begin_month_integer_string ),
			atoi( end_month_integer_string ),
			appaserver_parameter_file->
				document_root );

	output_crayfish_data(
			application_name,
			atoi( year_integer_string ),
			atoi( begin_month_integer_string ),
			atoi( end_month_integer_string ),
			appaserver_parameter_file->
				document_root );

	output_inverts_verts_data(
			application_name,
			atoi( year_integer_string ),
			atoi( begin_month_integer_string ),
			atoi( end_month_integer_string ),
			appaserver_parameter_file->
				document_root );

	output_wet_weights_data(
			application_name,
			atoi( year_integer_string ),
			atoi( begin_month_integer_string ),
			atoi( end_month_integer_string ),
			appaserver_parameter_file->
				document_root );

	output_vegetation_data(
			application_name,
			atoi( year_integer_string ),
			atoi( begin_month_integer_string ),
			atoi( end_month_integer_string ),
			appaserver_parameter_file->
				document_root );

	output_periphyton_data(
			application_name,
			atoi( year_integer_string ),
			atoi( begin_month_integer_string ),
			atoi( end_month_integer_string ),
			appaserver_parameter_file->
				document_root );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;

} /* main() */


void output_fish_data(
			char *application_name,
			int year,
			int begin_month_integer,
			int end_month_integer,
			char *document_root_directory )
{
	char *output_filename;
	char *ftp_filename;
	FILE *output_file;
	char *begin_month_string;
	char *end_month_string;
	THROW throw;
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM_FISH,
			application_name,
			year /* process_id */,
			(char *)0 /* session */,
			"csv" );

	begin_month_string =
		timlib_integer2full_month(
			begin_month_integer );

	appaserver_link_file->begin_date_string = begin_month_string;

	end_month_string =
		timlib_integer2full_month(
			end_month_integer );

	appaserver_link_file->end_date_string = end_month_string;

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

	sprintf(	begin_date_string,
			"%d-%d-01",
			year,
			begin_month_integer );

	sprintf(	end_date_string,
			"%d-%d-31",
			year,
			end_month_integer );

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H3>ERROR: Cannot open output file %s\n",
			output_filename );
		document_close();
		exit( 1 );
	}

	fprintf( output_file,
"REGION,CUM,YEAR,PERIOD,MONTH,DAY,SITE,PLOT,THROW,SPECIES,LENGTH,SEX,COMMENT\n" );

	while( freshwaterfish_get_throw_between_dates(
					&throw,
					application_name,
					begin_date_string,
					end_date_string,
					AGENCY_NPS ) )
	{
		output_fish_measurements(
				output_file,
				application_name,
				&throw );
	}

	fclose( output_file );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
"Tfish: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

} /* output_fish_data() */

void output_fish_measurements(	FILE *output_file,
				char *application_name,
				THROW *throw )
{
	MEASUREMENT *measurement;
	char *fiu_new_code;
	char *length_column;
	char *sex_fiu_code;
	char *region_column;
	char *event_column;
	LIST *species_group_name_list = list_new();

	list_append_pointer( species_group_name_list, "fish" );

	if ( list_rewind( throw->measurement_list ) )
	{
		do {
			measurement =
				list_get_pointer(
					throw->measurement_list );
	
			if ( strcmp(	measurement->scientific_name,
					"no_fish_caught" ) == 0 )
			{
				continue;
			}

			if ( !freshwaterfish_in_species_membership(
					application_name,
					species_group_name_list,
					measurement->scientific_name ) )
			{
				continue;
			}

			region_column =
				get_region_column( throw->site );

			fiu_new_code =
				freshwaterfish_get_fiu_new_code(
					application_name,
					measurement->scientific_name );

			event_column =
				get_event_column(
					freshwaterfish_get_visit_month(
						throw->visit_date ) );

			fprintf(output_file,
				"%s,%d,%d,%s,%d,%d,%s,%c,%d",
				region_column,
				freshwaterfish_get_visit_year(
					throw->visit_date ) - 1994,
				freshwaterfish_get_visit_year(
					throw->visit_date ),
				event_column,
				freshwaterfish_get_visit_month(
					throw->visit_date ),
				freshwaterfish_get_visit_day(
					throw->visit_date ),
				throw->site,
				*throw->plot,
				throw->throw_number );

			length_column =
				get_length_column(
					measurement->length_millimeters,
					measurement->no_measurement_count );

			sex_fiu_code =
				freshwaterfish_get_sex_fiu_code(
					application_name,
					measurement->sex );

			fprintf(output_file,
				",%s,%s,%s,%s\n",
				(*fiu_new_code != '.')
					? fiu_new_code
					: measurement->scientific_name,
				length_column,
				(sex_fiu_code) ? sex_fiu_code : "",
				(*measurement->comments == '.')
					? ""
					: measurement->comments );

		} while( list_next( throw->measurement_list ) );
	}

} /* output_fish_measurements() */

char *get_length_column(	char *length_millimeters,
				char *no_measurement_count )
{
	if ( length_millimeters && *length_millimeters != '.' )
		return length_millimeters;
	else
	if ( no_measurement_count && *no_measurement_count != '.' )
		return no_measurement_count;
	else
		return ".";
}

char *get_region_column( char *site )
{
	if ( strcmp( site, "6" ) == 0 || strcmp( site, "50" ) == 0 )
		return "SRS";
	else
	if ( strcmp( site, "23" ) == 0 )
		return "NESS";
	else
		return "";
}

char *get_event_column( int month_integer )
{
	if ( month_integer == 2 )
		return "1";
	else
	if ( month_integer == 4 )
		return "2";
	else
	if ( month_integer == 7 )
		return "3";
	else
	if ( month_integer == 10 )
		return "4";
	else
	if ( month_integer == 12 )
		return "5";
	else
		return "";
}

void output_crayfish_data(
			char *application_name,
			int year,
			int begin_month_integer,
			int end_month_integer,
			char *document_root_directory )
{
	char *output_filename;
	char *ftp_filename;
	FILE *output_file;
	char *begin_month_string;
	char *end_month_string;
	THROW throw;
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM_CRAYFISH,
			application_name,
			year /* process_id */,
			(char *)0 /* session */,
			"csv" );

	begin_month_string =
		timlib_integer2full_month(
			begin_month_integer );

	appaserver_link_file->begin_date_string = begin_month_string;

	end_month_string =
		timlib_integer2full_month(
			end_month_integer );

	appaserver_link_file->end_date_string = end_month_string;

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

	sprintf(	begin_date_string,
			"%d-%d-01",
			year,
			begin_month_integer );

	sprintf(	end_date_string,
			"%d-%d-31",
			year,
			end_month_integer );

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H3>ERROR: Cannot open output file %s\n",
			output_filename );
		document_close();
		exit( 1 );
	}

	fprintf( output_file,
"REGION,CUM,YEAR,PERIOD,MONTH,DAY,SITE,PLOT,THROW,SPECIES,SEX,FORM,LENGTH,COMMENT\n" );

	while( freshwaterfish_get_throw_between_dates(
					&throw,
					application_name,
					begin_date_string,
					end_date_string,
					AGENCY_NPS ) )
	{
		output_crayfish_measurements(
				output_file,
				application_name,
				&throw );
	}

	fclose( output_file );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
"TCrayfish: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

} /* output_crayfish_data() */

void output_crayfish_measurements(
				FILE *output_file,
				char *application_name,
				THROW *throw )
{
	MEASUREMENT *measurement;
	char *length_column;
	char *sex_fiu_code;
	char *fiu_new_code;
	char *region_column;
	char *event_column;
	LIST *species_group_name_list = list_new();

	list_append_pointer( species_group_name_list, "crayfish" );

	if ( list_rewind( throw->measurement_list ) )
	{
		do {
			measurement =
				list_get_pointer(
					throw->measurement_list );
	
			if ( !freshwaterfish_in_species_membership(
					application_name,
					species_group_name_list,
					measurement->scientific_name ) )
			{
				continue;
			}

			region_column =
				get_region_column( throw->site );

			event_column =
				get_event_column(
					freshwaterfish_get_visit_month(
						throw->visit_date ) );

			fprintf(output_file,
				"%s,%d,%d,%s,%d,%d,%s,%c,%d",
				region_column,
				freshwaterfish_get_visit_year(
					throw->visit_date ) - 1994,
				freshwaterfish_get_visit_year(
					throw->visit_date ),
				event_column,
				freshwaterfish_get_visit_month(
					throw->visit_date ),
				freshwaterfish_get_visit_day(
					throw->visit_date ),
				throw->site,
				*throw->plot,
				throw->throw_number );

			length_column =
				get_length_column(
					measurement->length_millimeters,
					measurement->no_measurement_count );

			sex_fiu_code =
				freshwaterfish_get_sex_fiu_code(
					application_name,
					measurement->sex );

			fiu_new_code =
				freshwaterfish_get_fiu_new_code(
					application_name,
					measurement->scientific_name );

			fprintf(output_file,
				",%s,%s,,%s,%s\n",
				(*fiu_new_code != '.')
					? fiu_new_code
					: measurement->scientific_name,
				(sex_fiu_code) ? sex_fiu_code : "",
				length_column,
				(*measurement->comments == '.')
					? ""
					: measurement->comments );

		} while( list_next( throw->measurement_list ) );
	}

} /* output_crayfish_measurements() */

void output_vegetation_data(
			char *application_name,
			int year,
			int begin_month_integer,
			int end_month_integer,
			char *document_root_directory )
{
	char *output_filename;
	char *ftp_filename;
	FILE *output_file;
	char *begin_month_string;
	char *end_month_string;
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];
	MEASUREMENT_COUNT *measurement_count;
	SPECIES *species;
	LIST *species_group_name_list;
	THROW_WEIGHT_COUNT *throw_count;
	char output_string[ 128 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM_VEGETATION,
			application_name,
			year /* process_id */,
			(char *)0 /* session */,
			"csv" );

	begin_month_string =
		timlib_integer2full_month(
			begin_month_integer );

	appaserver_link_file->begin_date_string = begin_month_string;

	end_month_string =
		timlib_integer2full_month(
			end_month_integer );

	appaserver_link_file->end_date_string = end_month_string;

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

	sprintf(	begin_date_string,
			"%d-%d-01",
			year,
			begin_month_integer );

	sprintf(	end_date_string,
			"%d-%d-31",
			year,
			end_month_integer );

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H3>ERROR: Cannot open output file %s\n",
			output_filename );
		document_close();
		exit( 1 );
	}


	species_group_name_list = list_new();
	list_append_pointer(	species_group_name_list,
				"surface_vegetation" );
	list_append_pointer(	species_group_name_list,
				"submergent_vegetation" );

	measurement_count = freshwaterfish_measurement_count_new(
				application_name,
				species_group_name_list );

	measurement_count->throw_list =
		freshwaterfish_get_species_throw_list(
			application_name,
			begin_date_string,
			end_date_string,
			"select_vegetation.sh",
			measurement_count->species_comprised_list );

	/* Output the header */
	/* ----------------- */
	fprintf( output_file,
"REGION,CUM,YEAR,PERIOD,MONTH,DAY,SITE,PLOT,THROW" );

	if ( list_rewind( measurement_count->species_list ) )
	{
		do {
			species = list_get_pointer(
					measurement_count->species_list );

			strcpy( output_string, species->fiu_new_code );
			up_string( output_string );
			fprintf(	output_file,
					",%s",
					output_string );
		} while( list_next( measurement_count->species_list ) );
	}

	fprintf( output_file, "\n" );

	/* Output the data */
	/* --------------- */
	if ( list_rewind( measurement_count->throw_list ) )
	{
		do {
			throw_count =
				list_get_pointer(
					measurement_count->throw_list );

			output_throw_counts(
				output_file,
				measurement_count->species_list,
				throw_count,
				0 /* not with_extra_heading_comma */ );

		} while( list_next( measurement_count->throw_list ) );
	}

	fclose( output_file );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
"TVeg: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

} /* output_vegetation_data() */

void output_vegetation_densities(
				FILE *output_file,
				char *application_name,
				THROW *throw )
{
	SURFACE_VEGETATION *surface_vegetation;
	SUBMERGENT_VEGETATION *submergent_vegetation;
	char *fiu_new_code;

	if ( list_rewind( throw->surface_vegetation_list ) )
	{
		do {
			surface_vegetation =
				list_get_pointer(
					throw->surface_vegetation_list );

			if ( is_ignore_vegetation(
				surface_vegetation->scientific_name ) )
			{
				continue;
			}

			fiu_new_code =
				freshwaterfish_get_fiu_new_code(
					application_name,
					surface_vegetation->scientific_name );

			if ( !fiu_new_code
			||   !*fiu_new_code
			||   *fiu_new_code == '.' )
			{
				fiu_new_code = 
					surface_vegetation->
						scientific_name;
			}

			fprintf(output_file,
				"%d,%d,%d,%s,%c,%d",
				freshwaterfish_get_visit_year(
					throw->visit_date ),
				freshwaterfish_get_visit_month(
					throw->visit_date ),
				freshwaterfish_get_visit_day(
					throw->visit_date ),
				throw->site,
				*throw->plot,
				throw->throw_number );

			fprintf(output_file,
				",%s,%s,%s\n",
				fiu_new_code,
				surface_vegetation->
					density_alive_count,
				surface_vegetation->
					density_dead_count );

		} while( list_next( throw->surface_vegetation_list ) );
	}

	if ( list_rewind( throw->submergent_vegetation_list ) )
	{
		do {
			submergent_vegetation =
				list_get_pointer(
					throw->submergent_vegetation_list );

			if ( is_ignore_vegetation(
				submergent_vegetation->scientific_name ) )
			{
				continue;
			}

			fiu_new_code =
				freshwaterfish_get_fiu_new_code(
					application_name,
					submergent_vegetation->
						scientific_name );

			if ( !fiu_new_code
			||   !*fiu_new_code
			||   *fiu_new_code == '.' )
			{
				fiu_new_code = 
					submergent_vegetation->
						scientific_name;
			}

			fprintf(output_file,
				"%d,%d,%d,%s,%c,%d",
				freshwaterfish_get_visit_year(
					throw->visit_date ),
				freshwaterfish_get_visit_month(
					throw->visit_date ),
				freshwaterfish_get_visit_day(
					throw->visit_date ),
				throw->site,
				*throw->plot,
				throw->throw_number );

			fprintf(output_file,
				",%s,%s,\n",
				fiu_new_code,
				submergent_vegetation->
					density_percent );

		} while( list_next( throw->submergent_vegetation_list ) );
	}

} /* output_vegetation_densities() */

void output_periphyton_data(
			char *application_name,
			int year,
			int begin_month_integer,
			int end_month_integer,
			char *document_root_directory )
{
	char *output_filename;
	char *ftp_filename;
	FILE *output_file;
	char *begin_month_string;
	char *end_month_string;
	THROW throw;
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM_PERIPHYTON,
			application_name,
			year /* process_id */,
			(char *)0 /* session */,
			"csv" );

	begin_month_string =
		timlib_integer2full_month(
			begin_month_integer );

	appaserver_link_file->begin_date_string = begin_month_string;

	end_month_string =
		timlib_integer2full_month(
			end_month_integer );

	appaserver_link_file->end_date_string = end_month_string;

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


	sprintf(	begin_date_string,
			"%d-%d-01",
			year,
			begin_month_integer );

	sprintf(	end_date_string,
			"%d-%d-31",
			year,
			end_month_integer );

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H3>ERROR: Cannot open output file %s\n",
			output_filename );
		document_close();
		exit( 1 );
	}

	fprintf( output_file,
"YEAR,MONTH,DAY,SITE,PLOT,THROW,PEPLCVR,PLHT,PPERICVR,PERIVOL,DEPTH\n" );

	while( freshwaterfish_get_throw_between_dates(
					&throw,
					application_name,
					begin_date_string,
					end_date_string,
					AGENCY_NPS ) )
	{
		fprintf(output_file,
			"%d,%d,%d,%s,%c,%d,%s,%s,%s,%s,%s\n",
			freshwaterfish_get_visit_year(
				throw.visit_date ),
			freshwaterfish_get_visit_month(
				throw.visit_date ),
			freshwaterfish_get_visit_day(
				throw.visit_date ),
			throw.site,
			*throw.plot,
			throw.throw_number,
			throw.emergence_plant_cover_percent,
			throw.average_plant_height_cm,
			throw.periphyton_cover_percent,
			throw.periphyton_volume_milliliter,
			throw.water_depth_cm );
	}

	fclose( output_file );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
"TPHYS: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

} /* output_periphyton_data() */

boolean is_ignore_vegetation( char *vegetation_scientific_name )
{
	if ( strcasecmp(	vegetation_scientific_name,
				"periphyton" ) == 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
} /* is_ignore_vegetation() */

void output_inverts_verts_data(
			char *application_name,
			int year,
			int begin_month_integer,
			int end_month_integer,
			char *document_root_directory )
{
	char *output_filename;
	char *ftp_filename;
	FILE *output_file;
	char *begin_month_string;
	char *end_month_string;
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];
	MEASUREMENT_COUNT *measurement_count;
	SPECIES *species;
	THROW_WEIGHT_COUNT *throw_count;
	char output_string[ 128 ];
	LIST *species_group_name_list;
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM_NONFISH,
			application_name,
			year /* process_id */,
			(char *)0 /* session */,
			"csv" );

	begin_month_string =
		timlib_integer2full_month(
			begin_month_integer );

	appaserver_link_file->begin_date_string = begin_month_string;

	end_month_string =
		timlib_integer2full_month(
			end_month_integer );

	appaserver_link_file->end_date_string = end_month_string;

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

	sprintf(	begin_date_string,
			"%d-%d-01",
			year,
			begin_month_integer );

	sprintf(	end_date_string,
			"%d-%d-31",
			year,
			end_month_integer );

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H3>ERROR: Cannot open output file %s\n",
			output_filename );
		document_close();
		exit( 1 );
	}

	species_group_name_list = list_new();
	list_append_pointer(	species_group_name_list,
				"other_invertebrate" );
	list_append_pointer(	species_group_name_list,
				"other_vertebrate" );

	measurement_count = freshwaterfish_measurement_count_new(
				application_name,
				species_group_name_list );

	measurement_count->throw_list =
		freshwaterfish_get_species_throw_list(
			application_name,
			begin_date_string,
			end_date_string,
			"select_inverts_other_verts.sh",
			measurement_count->species_comprised_list );

	/* Output the header */
	/* ----------------- */
	fprintf( output_file,
"REGION,CUM,YEAR,PERIOD,MONTH,DAY,SITE,PLOT,THROW,DATA_SETS" );

	if ( list_rewind( measurement_count->species_list ) )
	{
		do {
			species = list_get_pointer(
					measurement_count->species_list );

			strcpy( output_string, species->fiu_new_code );
			up_string( output_string );
			fprintf(	output_file,
					",%s",
					output_string );
		} while( list_next( measurement_count->species_list ) );
	}

	fprintf( output_file, "\n" );

	/* Output the data */
	/* --------------- */
	if ( list_rewind( measurement_count->throw_list ) )
	{
		do {
			throw_count =
				list_get_pointer(
					measurement_count->throw_list );

			output_throw_counts(
				output_file,
				measurement_count->species_list,
				throw_count,
				1 /* with_extra_heading_comma */ );

		} while( list_next( measurement_count->throw_list ) );
	}

	fclose( output_file );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
"TINVandOV: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

} /* output_inverts_verts_data() */

void output_throw_counts(
				FILE *output_file,
				LIST *species_list,
				THROW_WEIGHT_COUNT *throw_count,
				boolean with_extra_heading_comma )
{
	SPECIES *species;
	SPECIES *throw_count_species;
	char *region_column;
	char *event_column;

	region_column = get_region_column( throw_count->site );

	event_column = get_event_column(
				freshwaterfish_get_visit_month(
					throw_count->visit_date ) );

	fprintf(	output_file,
			"%s,%d,%d,%s,%d,%d,%s,%s,%d",
			region_column,
			freshwaterfish_get_visit_year(
					throw_count->visit_date ) - 1994,
			freshwaterfish_get_visit_year(
					throw_count->visit_date ),
			event_column,
			freshwaterfish_get_visit_month(
					throw_count->visit_date ),
			freshwaterfish_get_visit_day(
					throw_count->visit_date ),
			throw_count->site,
			throw_count->plot,
			throw_count->throw_number );

	if ( with_extra_heading_comma ) fprintf( output_file, "," );

	if ( list_rewind( species_list ) )
	{
		do {
			species = list_get_pointer( species_list );

			if ( ( throw_count_species =
				hash_table_get_pointer(
					throw_count->species_hash_table,
					species->scientific_name ) ) )
			{
				fprintf(	output_file,
						",%.1lf",
						throw_count_species->
							aggregation );
			}
			else
			{
				fprintf(	output_file,
						"," );
			}
		} while( list_next( species_list ) );
	}

	fprintf( output_file, "\n" );

} /* output_throw_counts() */

void output_species_group_weight(
				FILE *output_file,
				LIST *species_group_list,
				THROW_WEIGHT_COUNT *throw_count )
{
	SPECIES_GROUP *species_group;
	SPECIES_GROUP *throw_count_species_group;
	char *region_column;

	region_column = get_region_column( throw_count->site );

	fprintf(	output_file,
			"%s,%d,%d,%d,%d,%s,%s,%d",
			region_column,
			freshwaterfish_get_visit_year(
					throw_count->visit_date ) - 1994,
			freshwaterfish_get_visit_year(
					throw_count->visit_date ),
			freshwaterfish_get_visit_month(
					throw_count->visit_date ),
			freshwaterfish_get_visit_day(
					throw_count->visit_date ),
			throw_count->site,
			throw_count->plot,
			throw_count->throw_number );

	if ( list_rewind( species_group_list ) )
	{
		do {
			species_group = list_get_pointer( species_group_list );

			if ( ( throw_count_species_group =
				hash_table_get_pointer(
					throw_count->species_group_hash_table,
					species_group->species_group_name ) ) )
			{
				fprintf(	output_file,
						",%.2lf",
						throw_count_species_group->
							weight_sum );
			}
			else
			{
				fprintf(	output_file,
						"," );
			}
		} while( list_next( species_group_list ) );
	}

	if ( ( throw_count_species_group =
		hash_table_get_pointer(
			throw_count->species_group_hash_table,
			"fish" ) ) )
	{
		fprintf(	output_file,
				",,%d",
				throw_count_species_group->
					count );
	}
	else
	{
		fprintf(	output_file,
				"," );
	}
	fprintf( output_file, "\n" );

} /* output_species_group_weight() */

void output_wet_weights_data(
			char *application_name,
			int year,
			int begin_month_integer,
			int end_month_integer,
			char *document_root_directory )
{
	char *output_filename;
	char *ftp_filename;
	FILE *output_file;
	char *begin_month_string;
	char *end_month_string;
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];
	LIST *species_group_name_list;
	SPECIES_GROUP_WEIGHT *species_group_weight;
	THROW_WEIGHT_COUNT *throw_count;
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM_NONFISH,
			application_name,
			year /* process_id */,
			(char *)0 /* session */,
			"csv" );

	begin_month_string =
		timlib_integer2full_month(
			begin_month_integer );

	appaserver_link_file->begin_date_string = begin_month_string;

	end_month_string =
		timlib_integer2full_month(
			end_month_integer );

	appaserver_link_file->end_date_string = end_month_string;

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

	sprintf(	begin_date_string,
			"%d-%d-01",
			year,
			begin_month_integer );

	sprintf(	end_date_string,
			"%d-%d-31",
			year,
			end_month_integer );

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H3>ERROR: Cannot open output file %s\n",
			output_filename );
		document_close();
		exit( 1 );
	}

	species_group_name_list = list_new();
	list_append_pointer(	species_group_name_list,
				"fish" );
	list_append_pointer(	species_group_name_list,
				"crayfish" );
	list_append_pointer(	species_group_name_list,
				"shrimp" );
	list_append_pointer(	species_group_name_list,
				"mollusc" );
	list_append_pointer(	species_group_name_list,
				"other_invertebrate" );
	list_append_pointer(	species_group_name_list,
				"other_vertebrate" );

	species_group_weight = freshwaterfish_species_group_weight_new(
					application_name,
					species_group_name_list );

	species_group_weight->throw_list =
		freshwaterfish_get_species_group_throw_list(
			application_name,
			begin_date_string,
			end_date_string );

	/* Output the header */
	/* ----------------- */
	fprintf( output_file,
"REGION,CUM,YEAR,MONTH,DAY,SITE,PLOT,THROW,FISWGT,CRYWGT,SHPWGT,MOLWGT,OITWGT,OVTWGT,COMMENT,TOTFISH" );

	fprintf( output_file, "\n" );

	/* Output the data */
	/* --------------- */
	if ( list_rewind( species_group_weight->throw_list ) )
	{
		do {
			throw_count =
				list_get_pointer(
					species_group_weight->throw_list );

			output_species_group_weight(
				output_file,
				species_group_weight->species_group_list,
				throw_count );

		} while( list_next( species_group_weight->throw_list ) );
	}

	fclose( output_file );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
"TWetweight: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

} /* output_wet_weights_data() */

