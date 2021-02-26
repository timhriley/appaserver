/* ---------------------------------------------------	*/
/* load_lab_spreadsheet.c				*/
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

/* Prototypes */
/* ---------- */
void delete_measurements(
				char *application_name,
				char *input_filename );

int insert_measurements(	char *application_name,
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

	if ( !freshwaterfish_exists_sex(
					application_name,
					UNKNOWN_SEX ) )
	{
		printf(
"<h3>Error. A sex name was changed. It is expected to be %s.</h3>\n",
			UNKNOWN_SEX );
		document_close();
		exit( 0 );
	}

	if ( really_yn == 'y' )
	{
		delete_measurements( application_name, input_filename );
	}

	record_count = insert_measurements(
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

void delete_measurements(	char *application_name,
				char *input_filename )
{
	FILE *input_file;
	FILE *measurement_delete_pipe;
	char sys_string[ 1024 ];
	char input_string[ 1024 ];
	char site[ 128 ];
	char plot[ 128 ];
	char *region;
	char visit_date_compressed[ 256 ];
	char *visit_date_international;
	char *table_name;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	table_name =
		get_table_name(
			application_name, "measurement" );

	sprintf( sys_string,
"delete_statement.e t=%s f=%s d='|' | sql.e 2>&1",
		 table_name,
		 DELETE_FIELD_LIST );

	measurement_delete_pipe = popen( sys_string, "w" );

	while( get_line( input_string, input_file ) )
	{
		trim_character( input_string, '"', input_string );
		trim_character( input_string, '\'', input_string );

		if ( !piece(	visit_date_compressed,
				',',
				input_string,
				MEASUREMENT_VISIT_DATE_PIECE ) )
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
				MEASUREMENT_SITE_PIECE ) )
		{
			continue;
		}

		strcpy( site, freshwaterfish_get_clean_site( site ) );

		if ( ! ( region = freshwaterfish_get_nps_region( site ) ) )
		{
			continue;
		}

		if ( !piece(	plot,
				',',
				input_string,
				MEASUREMENT_PLOT_PIECE ) )
		{
			continue;
		}

		fprintf(measurement_delete_pipe,
			"%s|%s|%s|%s\n",
			site,
		     	plot,
			region,
			visit_date_international );
	}

	fclose( input_file );
	pclose( measurement_delete_pipe );

} /* delete_measurements() */

int insert_measurements(char *application_name,
			char *input_filename,
			char really_yn )
{
	FILE *input_file;
	FILE *site_visit_output_pipe = {0};
	FILE *throw_output_pipe = {0};
	FILE *measurement_output_pipe = {0};
	char input_string[ 1024 ];
	char *scientific_name;
	char fiu_fish_old_code[ 128 ];
	char fiu_non_fish_old_code[ 128 ];
	char nps_code[ 128 ];
	char *visit_date_international;
	char *now_date_international;
	char site[ 128 ];
	char plot[ 128 ];
	char *region;
	char visit_date_compressed[ 128 ];
	char throw_number[ 128 ];
	char length_millimeters[ 128 ];
	char sex_alpha_code[ 128 ];
	char *sex;
	char weight_grams[ 128 ];
	char no_measurement_count[ 128 ];
	char no_measurement_total_weight[ 128 ];
	char comments[ 128 ];
	char measurement_number_string[ 128 ];
	char sys_string[ 1024 ];
	char error_filename[ 128 ];
	FILE *error_file;
	int measurement_number = 0;
	int line_number = 0;
	int record_count = 0;
	char region_buffer[ 128 ];

	if ( really_yn == 'y' )
	{
		measurement_number =
			freshwaterfish_get_measurement_number(
				application_name,
				input_filename );
	}

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	sprintf( error_filename,
		 "/tmp/freshwaterfish_measurement_error_%d.txt",
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

		/* Insert SITE_VISIT */
		/* ----------------- */
		table_name =
			get_table_name( application_name, "site_visit" );

		sprintf( sys_string,
"sed 's/|\\./|/g' | insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 LAB_SITE_VISIT_FIELD_LIST );
		site_visit_output_pipe = popen( sys_string, "w" );

		/* Insert THROW */
		/* ------------ */
		table_name =
			get_table_name( application_name, "throw" );

		sprintf( sys_string,
"sed 's/|\\./|/g' | insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 LAB_THROW_FIELD_LIST );
		throw_output_pipe = popen( sys_string, "w" );

		/* Insert MEASUREMENT */
		/* ------------------ */
		table_name =
			get_table_name( application_name, "measurement" );

		sprintf( sys_string,
"sed 's/|\\./|/g' | insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 MEASUREMENT_FIELD_LIST );
		measurement_output_pipe = popen( sys_string, "w" );
	}
	else
	{
		sprintf( sys_string,
"sed 's/|\\./|/g' | queue_top_bottom_lines.e 50 | html_table.e 'Insert into Measurement' %s '|' %s",
			 MEASUREMENT_FIELD_LIST,
			 MEASUREMENT_JUSTIFY_COMMA_LIST );
		measurement_output_pipe = popen( sys_string, "w" );
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
				MEASUREMENT_VISIT_DATE_PIECE ) )
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
				MEASUREMENT_SITE_PIECE ) )
		{
			fprintf(error_file,
			"Warning in line %d: Cannot piece site in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		strcpy( site, freshwaterfish_get_clean_site( site ) );

		if ( strcmp( site, "99" ) == 0 ) continue;

		if ( !piece(	plot,
				',',
				input_string,
				MEASUREMENT_PLOT_PIECE ) )
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
				MEASUREMENT_THROW_NUMBER_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece throw number in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece(	nps_code,
				',',
				input_string,
				MEASUREMENT_NPS_CODE_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece nps code in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece(	fiu_fish_old_code,
				',',
				input_string,
				MEASUREMENT_FIU_FISH_OLD_CODE_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece old fiu code in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		strcpy( fiu_non_fish_old_code, fiu_fish_old_code );

		if ( ! ( scientific_name =
			freshwaterfish_library_get_scientific_name(
				nps_code,
				fiu_fish_old_code,
				fiu_non_fish_old_code,
				(char *)0 /* fiu_new_code */,
				application_name ) ) )
		{
			fprintf(error_file,
"Warning in line %d: Cannot get scientific name for either code (%s) or (%s) in (%s)\n",
				line_number,
				nps_code,
				fiu_non_fish_old_code,
				input_string );
			continue;
		}

		if ( !piece(	length_millimeters,
				',',
				input_string,
				MEASUREMENT_LENGTH_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece length in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece(	sex_alpha_code,
				',',
				input_string,
				MEASUREMENT_SEX_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece sex code in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( ! ( sex = freshwaterfish_get_sex(
				application_name,
				sex_alpha_code ) ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot get sex using code of %s in (%s)\n",
				line_number,
				sex_alpha_code,
				input_string );
			continue;
		}

		if ( !piece(	weight_grams,
				',',
				input_string,
				MEASUREMENT_WEIGHT_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece weight in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece(	comments,
				',',
				input_string,
				MEASUREMENT_COMMENTS_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece comments in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( strcmp( comments, "ADD" ) == 0 )
		{
			strcpy( no_measurement_count, length_millimeters );
			strcpy( no_measurement_total_weight, weight_grams );
			*length_millimeters = '\0';
			*weight_grams = '\0';
		}
		else
		{
			*no_measurement_count = '\0';
			*no_measurement_total_weight = '\0';
		}

		if ( really_yn == 'y' )
		{
			fprintf(site_visit_output_pipe,
				"%s|%s|%s|%s|%s\n",
				site,
				plot,
				region,
				visit_date_international,
				AGENCY_NPS );

			fprintf(throw_output_pipe,
				"%s|%s|%s|%s|%s\n",
				site,
				plot,
				region,
				visit_date_international,
				throw_number );

			sprintf(	measurement_number_string,
					"%d",
					measurement_number++ );
		}
		else
		{
			strcpy(	measurement_number_string,
				"To be determined" );
		}

		if ( really_yn == 'y' )
		{
			fprintf(measurement_output_pipe,
				"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
				site,
				plot,
				region,
				visit_date_international,
				throw_number,
				scientific_name,
				measurement_number_string,
				length_millimeters,
				sex,
				weight_grams,
				no_measurement_count,
				no_measurement_total_weight,
				comments );
		}
		else
		{
			fprintf(measurement_output_pipe,
				"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
				site,
				plot,
				format_initial_capital( region_buffer, region ),
				visit_date_international,
				throw_number,
				scientific_name,
				measurement_number_string,
				length_millimeters,
				sex,
				weight_grams,
				no_measurement_count,
				no_measurement_total_weight,
				comments );
		}

		record_count++;
	}

	fclose( input_file );
	fclose( error_file );

	pclose( measurement_output_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Lab Errors' '' '|'",
			 error_filename );
		system( sys_string );
	}

	sprintf( sys_string, "rm %s", error_filename );
	system( sys_string );
	return record_count;

} /* insert_measurements() */

