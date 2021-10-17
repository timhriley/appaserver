/* ---------------------------------------------------	*/
/* output_lab_input_spreadsheet.c			*/
/* ---------------------------------------------------	*/
/* This process outputs a spreadsheet file that		*/
/* closely matches the lab input spreadsheet.		*/
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
#include "appaserver_link.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
char *get_length_column(	char *length_millimeters,
				char *no_measurement_count );

char *get_sex_column(		char *application_name,
				char *sex );

char *get_weight_column(	char *weight_grams,
				char *no_measurement_total_weight );

void output_throw_first_half(	FILE *output_file,
				THROW *throw,
				char *visit_date_compressed );

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
	APPASERVER_LINK *appaserver_link;

	appaserver_link =
		appaserver_link_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			process_name /* filename_stem */,
			application_name,
			0 /* process_id */,
			(char *)0 /* session */,
			begin_date_string,
			end_date_string,
			"csv" );

	output_filename = appaserver_link->output->filename;
	ftp_filename = appaserver_link->prompt->filename;

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H3>ERROR: Cannot open output file %s\n",
			output_filename );
		document_close();
		exit( 1 );
	}

	fprintf( output_file,
"DATE,YEAR,MONTH,DAY,SITE,PLOT,THROW,SPECIES,CODE,LENGTH,SEX,WEIGHT,COMMENTS\n" );


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
	MEASUREMENT *measurement;
	char *species_column;
	char species_column_up_string[ 128 ];
	char *code_column;
	char *length_column;
	char *sex_column;
	char *weight_column;

	if ( !list_length( throw->measurement_list ) )
	{
		output_throw_first_half(
				output_file,
				throw,
				visit_date_compressed );

		fprintf(output_file, ",.,.,.,.,.,.\n" );
		return;
	}

	if ( list_rewind( throw->measurement_list ) )
	{
		do {
			measurement =
				list_get_pointer(
					throw->measurement_list );
	
			output_throw_first_half(
					output_file,
					throw,
					visit_date_compressed );
	
			if ( output_scientific_name_yn == 'y')
			{
				species_column = 
					measurement->scientific_name;
			}
			else
			{
				species_column = 
					freshwaterfish_get_nps_code(
						application_name,
						measurement->
							scientific_name );
				strcpy(	species_column_up_string,
					species_column );
				up_string( species_column_up_string );
				species_column = species_column_up_string;
			}

			code_column =
				freshwaterfish_get_fiu_non_fish_old_code(
					application_name,
					measurement->scientific_name );

			if ( !code_column || *code_column == '.' )
			{
				code_column =
					freshwaterfish_get_fiu_fish_old_code(
						application_name,
						measurement->scientific_name );
			}

			if ( !code_column || *code_column == '.' )
			{
				code_column = "9999";
			}

			length_column =
				get_length_column(
					measurement->length_millimeters,
					measurement->no_measurement_count );

			sex_column =
				get_sex_column(
					application_name,
					measurement->sex );

			weight_column =
				get_weight_column(
					measurement->weight_grams,
					measurement->
						no_measurement_total_weight );

			fprintf(output_file,
				",%s,%s,%s,%s,%s,%s\n",
				species_column,
				code_column,
				length_column,
				sex_column,
				weight_column,
				measurement->comments );

		} while( list_next( throw->measurement_list ) );
	}

} /* output_repeating_throws() */

void output_throw_first_half(	FILE *output_file,
				THROW *throw,
				char *visit_date_compressed )
{
		fprintf(output_file,
"%s,%d,%d,%d,%s,%c,%d",
			visit_date_compressed,
			freshwaterfish_get_visit_year( throw->visit_date ),
			freshwaterfish_get_visit_month( throw->visit_date ),
			freshwaterfish_get_visit_day( throw->visit_date ),
			throw->site,
			*throw->plot,
			throw->throw_number );

} /* output_throw_first_half() */

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

char *get_sex_column(	char *application_name,
			char *sex )
{
	char *sex_alpha_code;

	if ( ! ( sex_alpha_code =
			freshwaterfish_get_sex_alpha_code(
				application_name,
				sex ) ) )
	{
		return ".";
	}
	else
	{
		if ( !*sex_alpha_code )
			return ".";
		else
			return sex_alpha_code;
	}
}

char *get_weight_column(	char *weight_grams,
				char *no_measurement_total_weight )
{
	if ( weight_grams && *weight_grams != '.' )
		return weight_grams;
	else
	if ( no_measurement_total_weight
	&&   *no_measurement_total_weight != '.' )
		return no_measurement_total_weight;
	else
		return ".";
}

