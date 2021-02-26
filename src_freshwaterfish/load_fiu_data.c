/* ---------------------------------------------------	*/
/* load_fiu_data.c					*/
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
typedef struct
{
	SURFACE_VEGETATION *surface_vegetation;
	SUBMERGENT_VEGETATION *submergent_vegetation;
} LOAD_FIU_VEGETATION_SPECIES;

/* Constants */
/* --------- */
#define FIU_AGENCY			"florida_international_university"
#define PARK_AGENCY			"national_park_service"

#define FIU_SITE_FIELD_LIST		"site,plot,region"
#define FIU_SITE_VISIT_FIELD_LIST	"site,plot,region,visit_date,agency"
#define FIU_THROW_FIELD_LIST		"site,plot,region,visit_date,throw_number"
#define FIU_THROW_FULL_FIELD_LIST	"site,plot,region,visit_date,throw_number,water_depth_cm,average_plant_height_cm,emergence_plant_cover_percent,periphyton_cover_percent,periphyton_volume_milliliter"
#define FIU_THROW_KEY_FIELD_LIST	"site,plot,region,visit_date,throw_number"
#define FIU_MEASUREMENT_FIELD_LIST	"site,plot,region,visit_date,throw_number,scientific_name,measurement_number,length_millimeters,sex,no_measurement_count"
#define FIU_SURFACE_VEGETATION_FIELD_LIST	"site,plot,region,visit_date,throw_number,scientific_name,density_alive_count"
#define FIU_SUBMERGENT_VEGETATION_FIELD_LIST	"site,plot,region,visit_date,throw_number,scientific_name,density_percent"
#define FIU_VEGETATION_FIELD_LIST	"site,plot,region,visit_date,throw_number,scientific_name,density"
#define FIU_MEASUREMENT_JUSTIFY_COMMA_LIST	"left,left,left,left,right,left,right,right,right,right"
#define FIU_VEGETATION_JUSTIFY_COMMA_LIST	"left,left,left,left,right,left,right,right,right,right"
#define FIU_THROW_FULL_JUSTIFY_COMMA_LIST	"left,left,left,left,right,right,right,right,right,right,right,right,right"

/* Prototypes */
/* ---------- */
FILE *open_error_file(		char **error_filename );

int insert_inverts_verts(	char *application_name,
				char *input_filename,
				char really_yn );

void output_vegetation_data(
				FILE *site_output_pipe,
				FILE *site_visit_output_pipe,
				FILE *throw_output_pipe,
				FILE *surface_vegetation_output_pipe,
				FILE *submergent_vegetation_output_pipe,
				int really_yn,
				char *site,
				char *plot,
				char *region,
				char *throw_number,
				char *visit_date_international,
				char *scientific_name,
				char *density,
				boolean is_surface );

MEASUREMENT **get_inverts_verts_species_array(
				int *array_length,
				char *application_name,
				char *input_filename );

LOAD_FIU_VEGETATION_SPECIES **get_vegetation_species_array(
				int *array_length,
				char *application_name,
				char *input_filename );

void output_measurement_output_pipe(
				FILE *measurement_output_pipe,
				char *site,
				char *plot,
				char *region,
				char *visit_date_international,
				char *throw_number,
				char *scientific_name,
				char *measurement_number,
				char *length_millimeters,
				char *sex,
				char *no_measurement_count,
				char really_yn );

char *get_measurement_number(	char *application_name,
				char *input_filename,
				char really_yn );

void delete_species_category(	char *application_name,
				char *species_category_list_string );

void delete_vegetation(		char *application_name );

int insert_update_physical(	char *application_name,
				char *input_filename,
				char really_yn );

int insert_crayfish(		char *application_name,
				char *input_filename,
				char really_yn );

int insert_vegetation(		char *application_name,
				char *input_filename,
				char really_yn );

int insert_other_inverts_verts(	char *application_name,
				char *input_filename,
				char really_yn );

int insert_fish(		char *application_name,
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
	int record_count = 0;
	char *fiu_load_group;
	char *species_category_list_string;
	FILE *input_file;

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s application process filename fiu_load_group really_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	input_filename = argv[ 3 ];
	fiu_load_group = argv[ 4 ];
	really_yn = *argv[ 5 ];

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

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		printf( "<h3>Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}
	else
	{
		fclose( input_file );
	}

	if ( strcmp( fiu_load_group, "vegetation" ) == 0 )
	{
		if ( really_yn == 'y' )
		{
			delete_vegetation( application_name );
			printf( "<p>Inserting FIU: vegetation...\n" );
			fflush( stdout );
		}
		record_count = insert_vegetation(
					application_name,
					input_filename,
					really_yn );
	}
	else
	if ( strcmp( fiu_load_group, "other_inverts_verts" ) == 0 )
	{
	
		if ( really_yn == 'y' )
		{
			species_category_list_string =
				"other_invertebrate,other_vertebrate";

			delete_species_category(application_name,
						species_category_list_string );
			printf(
			"<p>Inserting FIU: other inverts and verts...\n" );
			fflush( stdout );
		}

		record_count = insert_inverts_verts(
					application_name,
					input_filename,
					really_yn );
	}
	else
	if ( strcmp( fiu_load_group, "crayfish" ) == 0 )
	{
	
		if ( really_yn == 'y' )
		{
			species_category_list_string = "crayfish";

			delete_species_category(application_name,
						species_category_list_string );
			printf( "<p>Inserting FIU: crayfish...\n" );
			fflush( stdout );
		}

		record_count = insert_crayfish(
					application_name,
					input_filename,
					really_yn );
	}
	else
	if ( strcmp( fiu_load_group, "physical" ) == 0 )
	{
		if ( really_yn == 'y' )
		{
			printf( "<p>Updating: physical...\n" );
			fflush( stdout );
		}
		record_count = insert_update_physical(
					application_name,
					input_filename,
					really_yn );
	}
	else
	if ( strcmp( fiu_load_group, "fish" ) == 0 )
	{
	
		if ( really_yn == 'y' )
		{
			species_category_list_string = "fish";

			delete_species_category(application_name,
						species_category_list_string );
			printf( "<p>Inserting FIU: fish...\n" );
			fflush( stdout );
		}

		record_count = insert_fish(
					application_name,
					input_filename,
					really_yn );
	}

	if ( really_yn == 'y' )
	{
		printf( "<p>Process complete with %s records.\n",
			place_commas_in_long( record_count ) );
	}
	else
	{
		printf( "<p>Process not executed with %s records.\n",
			place_commas_in_long( record_count ) );
	}

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

void delete_vegetation(	char *application_name )
{
	char sys_string[ 1024 ];
	char input_string[ 2048 ];
	char site[ 128 ];
	char plot[ 128 ];
	char region[ 128 ];
	char visit_date[ 128 ];
	char *site_visit_table_name;
	char *throw_table_name;
	char *surface_vegetation_table_name;
	char *submergent_vegetation_table_name;
	char where[ 1024 ];
	FILE *input_pipe;
	FILE *throw_delete_pipe;
	FILE *site_visit_delete_pipe;
	char *select = "site,plot,region,visit_date";

	printf( "<p>Deleting FIU: vegetation...\n" );
	fflush( stdout );

	surface_vegetation_table_name =
		get_table_name(	application_name,
				"surface_vegetation" );

	submergent_vegetation_table_name =
		get_table_name(	application_name,
				"submergent_vegetation" );

	throw_table_name =
		get_table_name(	application_name,
				"throw" );

	site_visit_table_name =
		get_table_name(	application_name,
				"site_visit" );

	/* Open the input pipe */
	/* ------------------- */
	sprintf(where, "agency = '%s'", FIU_AGENCY );

	sprintf(sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=site_visit		"
		"			where=\"%s\"			",
		application_name,
		select,
		where );

	input_pipe = popen( sys_string, "r" );

	/* Open the throw delete pipe */
	/* -------------------------- */
	sprintf(sys_string,
		"delete_statement.e			 "
		"	table_name=%s			 "
		"	field_list=%s			 "
		"	delimiter='|'			|"
		"sql.e					 ",
		throw_table_name,
		select );
	throw_delete_pipe = popen( sys_string, "w" );

	/* Open the site_visit delete pipe */
	/* ------------------------------- */
	sprintf(sys_string,
		"delete_statement.e			 "
		"	table_name=%s			 "
		"	field_list=%s			 "
		"	delimiter='|'			|"
		"sql.e					 ",
		site_visit_table_name,
		select );
	site_visit_delete_pipe = popen( sys_string, "w" );

	while( get_line( input_string, input_pipe ) )
	{
		piece(	site,
			FOLDER_DATA_DELIMITER,
			input_string,
			0 );

		strcpy( site, freshwaterfish_get_clean_site( site ) );

		piece(	plot,
			FOLDER_DATA_DELIMITER,
			input_string,
			1 );

		piece(	region,
			FOLDER_DATA_DELIMITER,
			input_string,
			2 );

		piece(	visit_date,
			FOLDER_DATA_DELIMITER,
			input_string,
			3 );

		sprintf(where,
			"site = '%s' and			"
			"plot = '%s' and			"
			"region = '%s' and			"
			"visit_date = '%s'			",
			site,
			plot,
			region,
			visit_date );

		sprintf(sys_string,
			"echo \"delete from %s where %s;\"	|"
			"sql.e					 ",
			surface_vegetation_table_name,
			where );
		system( sys_string );

		sprintf(sys_string,
			"echo \"delete from %s where %s;\"	|"
			"sql.e					 ",
			submergent_vegetation_table_name,
			where );
		system( sys_string );

		fprintf( throw_delete_pipe,
			 "%s|%s|%s|%s\n",
			 site,
			 plot,
			 region,
			 visit_date );

		fprintf( site_visit_delete_pipe,
			 "%s|%s|%s|%s\n",
			 site,
			 plot,
			 region,
			 visit_date );
	}
	pclose( input_pipe );
	pclose( throw_delete_pipe );
	pclose( site_visit_delete_pipe );
	printf( "<p>Deleting FIU: vegetation completed.\n" );
	fflush( stdout );

} /* delete_vegetation() */

void delete_species_category(	char *application_name,
				char *species_category_list_string )
{
	char sys_string[ 1024 ];
	char input_string[ 2048 ];
	char site[ 128 ];
	char plot[ 128 ];
	char region[ 128 ];
	char visit_date[ 128 ];
	char *site_visit_table_name;
	char *throw_table_name;
	char *measurement_table_name;
	char where[ 1024 ];
	FILE *input_pipe;
	FILE *throw_delete_pipe;
	FILE *site_visit_delete_pipe;
	char *select = "site,plot,region,visit_date";

	printf( "<p>Deleting FIU: %s...\n", species_category_list_string );
	fflush( stdout );

	measurement_table_name =
		get_table_name(	application_name,
				"measurement" );

	throw_table_name =
		get_table_name(	application_name,
				"throw" );

	site_visit_table_name =
		get_table_name(	application_name,
				"site_visit" );

	/* Open the input pipe */
	/* ------------------- */
	sprintf(where, "agency = '%s'", FIU_AGENCY );

	sprintf(sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=site_visit		"
		"			where=\"%s\"			",
		application_name,
		select,
		where );

	input_pipe = popen( sys_string, "r" );

	/* Open the throw delete pipe */
	/* -------------------------- */
	sprintf(sys_string,
		"delete_statement.e			 "
		"	table_name=%s			 "
		"	field_list=%s			 "
		"	delimiter='|'			|"
		"sql.e					 ",
		throw_table_name,
		select );
	throw_delete_pipe = popen( sys_string, "w" );

	/* Open the site_visit delete pipe */
	/* ------------------------------- */
	sprintf(sys_string,
		"delete_statement.e			 "
		"	table_name=%s			 "
		"	field_list=%s			 "
		"	delimiter='|'			|"
		"sql.e					 ",
		site_visit_table_name,
		select );
	site_visit_delete_pipe = popen( sys_string, "w" );

	while( get_line( input_string, input_pipe ) )
	{
		piece(	site,
			FOLDER_DATA_DELIMITER,
			input_string,
			0 );

		strcpy( site, freshwaterfish_get_clean_site( site ) );

		piece(	plot,
			FOLDER_DATA_DELIMITER,
			input_string,
			1 );

		piece(	region,
			FOLDER_DATA_DELIMITER,
			input_string,
			2 );

		piece(	visit_date,
			FOLDER_DATA_DELIMITER,
			input_string,
			3 );

		sprintf(where,
			"site = '%s' and			"
			"plot = '%s' and			"
			"region = '%s' and			"
			"visit_date = '%s'			",
			site,
			plot,
			region,
			visit_date );

		sprintf(sys_string,
			"echo \"delete from %s where %s;\"	|"
			"sql.e					 ",
			measurement_table_name,
			where );
		system( sys_string );

		fprintf( throw_delete_pipe,
			 "%s|%s|%s|%s\n",
			 site,
			 plot,
			 region,
			 visit_date );

		fprintf( site_visit_delete_pipe,
			 "%s|%s|%s|%s\n",
			 site,
			 plot,
			 region,
			 visit_date );
	}
	pclose( input_pipe );
	pclose( throw_delete_pipe );
	pclose( site_visit_delete_pipe );
	printf( "<p>Deleting FIU: %s completed.\n",
		species_category_list_string );
	fflush( stdout );

} /* delete_species_category() */

int insert_inverts_verts(	char *application_name,
				char *input_filename,
				char really_yn )
{
	int record_count = 0;
	int array_length = 0;
	MEASUREMENT **species_array;
	FILE *input_file;
	FILE *site_output_pipe = {0};
	FILE *site_visit_output_pipe = {0};
	FILE *throw_output_pipe = {0};
	FILE *measurement_output_pipe = {0};
	char input_string[ 2048 ];
	char *scientific_name;
	char year[ 16 ];
	char month[ 16 ];
	char day[ 16 ];
	char site[ 128 ];
	char plot[ 128 ];
	char throw_number[ 128 ];
	char region_code[ 16 ];
	char *region;
	char visit_date_international[ 16 ];
	char no_measurement_count[ 128 ];
	char sys_string[ 1024 ];
	char *error_filename;
	FILE *error_file;
	int line_number = 0;
	int array_offset;
	int spreadsheet_offset;
	char *measurement_number;

	if ( ! ( species_array = get_inverts_verts_species_array(
				&array_length,
				application_name,
				input_filename ) ) )
	{
		return 0;
	}

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	if ( ! ( error_file = open_error_file( &error_filename ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		fclose( input_file );
		exit( 1 );
	}

	if ( really_yn == 'y' )
	{
		char *table_name;

		/* Site */
		/* ---- */
		table_name =
			get_table_name( application_name, "site" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 FIU_SITE_FIELD_LIST );
		site_output_pipe = popen( sys_string, "w" );

		/* Site Visit */
		/* ---------- */
		table_name =
			get_table_name( application_name, "site_visit" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 FIU_SITE_VISIT_FIELD_LIST );
		site_visit_output_pipe = popen( sys_string, "w" );

		/* Throw */
		/* ----- */
		table_name =
			get_table_name( application_name, "throw" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 FIU_THROW_FIELD_LIST );
		throw_output_pipe = popen( sys_string, "w" );

		/* Measurement */
		/* ----------- */
		table_name =
			get_table_name( application_name,
					"measurement" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | html_paragraph_wrapper.e",
			 table_name,
			 FIU_MEASUREMENT_FIELD_LIST );
		measurement_output_pipe = popen( sys_string, "w" );

	}
	else
	{
		sprintf( sys_string,
"queue_top_bottom_lines.e 50 | html_table.e 'Insert into Measurement' %s '|' %s",
			 FIU_MEASUREMENT_FIELD_LIST,
			 FIU_MEASUREMENT_JUSTIFY_COMMA_LIST );
		measurement_output_pipe = popen( sys_string, "w" );
	}

	while( get_line( input_string, input_file ) )
	{
		line_number++;

		if ( !piece_quoted( region_code, ',', input_string, 0, '"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece region code in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( strcmp( region_code, "REGION" ) == 0 ) continue;

		if ( ! ( region =
				freshwaterfish_get_region(
					application_name,
					region_code ) ) )
		{
			fprintf(error_file,
"Warning in line %d: Cannot get FIU region for region code = %s in (%s). This information is hardcoded into the source code. Please contact Tim at (916) 641-7789.\n",
				line_number,
				region_code,
				input_string );
			continue;
		}

		if ( !piece_quoted( site, ',', input_string, 6, '"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece site in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		strcpy( site, freshwaterfish_get_clean_site( site ) );

		if ( !piece_quoted( plot, ',', input_string, 7, '"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece plot in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	year,
					',',
					input_string,
					2,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece year in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	month,
					',',
					input_string,
					4,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece month in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	day,
					',',
					input_string,
					5,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece day in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		sprintf(	visit_date_international,
				"%s-%s-%s",
				year,
				month,
				day );

		if ( !piece_quoted(	throw_number,
					',',
					input_string,
					8,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece throw number in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		/* Species start at piece = 10 */
		/* --------------------------- */
		spreadsheet_offset = 10;

		array_offset = 0;

		while( piece(	no_measurement_count,
				',',
				input_string,
				spreadsheet_offset ) )
		{
			if ( array_offset == array_length ) break;
	
			scientific_name =
				species_array[ array_offset ]->
				scientific_name;

/*
			if ( !atoi( no_measurement_count ) )
			{
				spreadsheet_offset++;
				array_offset++;
				continue;
			}
*/
			if ( !*no_measurement_count )
			{
				spreadsheet_offset++;
				array_offset++;
				continue;
			}

			if ( really_yn == 'y' )
			{
				fprintf(site_output_pipe,
					"%s|%s|%s\n",
					site,
					plot,
					region );
	
				fprintf(site_visit_output_pipe,
					"%s|%s|%s|%s|%s\n",
					site,
					plot,
					region,
					visit_date_international,
					AGENCY_FIU );
	
				fprintf(throw_output_pipe,
					"%s|%s|%s|%s|%s\n",
					site,
					plot,
					region,
					visit_date_international,
					throw_number );

			}

			measurement_number =
				get_measurement_number(
					application_name,
					input_filename,
					really_yn );

			output_measurement_output_pipe(
				measurement_output_pipe,
				site,
				plot,
				region,
				visit_date_international,
				throw_number,
				scientific_name,
				measurement_number,
				"" /* length_millimeters */,
				"" /* sex */,
				no_measurement_count,
				really_yn );

			record_count++;
			spreadsheet_offset++;
			array_offset++;
		}
	}

	fclose( input_file );
	fclose( error_file );
	pclose( measurement_output_pipe );

	if ( really_yn == 'y' )
	{
		pclose( site_output_pipe );
		pclose( site_visit_output_pipe );
		pclose( throw_output_pipe );
	}

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Measurement Errors' '' '|'",
			 error_filename );
		system( sys_string );
	}

	sprintf( sys_string, "rm %s", error_filename );
	system( sys_string );

	return record_count;
} /* insert_inverts_verts() */

int insert_vegetation(	char *application_name,
			char *input_filename,
			char really_yn )
{
	int record_count = 0;
	int array_length = 0;
	LOAD_FIU_VEGETATION_SPECIES **species_array;
	FILE *input_file;
	FILE *site_output_pipe = {0};
	FILE *site_visit_output_pipe = {0};
	FILE *throw_output_pipe = {0};
	FILE *surface_vegetation_output_pipe = {0};
	FILE *submergent_vegetation_output_pipe = {0};
	char input_string[ 2048 ];
	char *scientific_name;
	char year[ 16 ];
	char month[ 16 ];
	char day[ 16 ];
	char site[ 128 ];
	char plot[ 128 ];
	char throw_number[ 128 ];
	char region_code[ 16 ];
	char *region;
	char visit_date_international[ 16 ];
	char density[ 128 ];
	char sys_string[ 1024 ];
	char *error_filename;
	FILE *error_file;
	int line_number = 0;
	int array_offset;
	int spreadsheet_offset;

	if ( ! ( species_array = get_vegetation_species_array(
				&array_length,
				application_name,
				input_filename ) ) )
	{
		return 0;
	}

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	if ( ! ( error_file = open_error_file( &error_filename ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		fclose( input_file );
		exit( 1 );
	}

	if ( really_yn == 'y' )
	{
		char *table_name;

		/* Site */
		/* ---- */
		table_name =
			get_table_name( application_name, "site" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 FIU_SITE_FIELD_LIST );
		site_output_pipe = popen( sys_string, "w" );

		/* Site Visit */
		/* ---------- */
		table_name =
			get_table_name( application_name, "site_visit" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | tee /tmp/tim.sql | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 FIU_SITE_VISIT_FIELD_LIST );
		site_visit_output_pipe = popen( sys_string, "w" );

		/* Throw */
		/* ----- */
		table_name =
			get_table_name( application_name, "throw" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 FIU_THROW_FIELD_LIST );
		throw_output_pipe = popen( sys_string, "w" );

		/* Surface Vegetation */
		/* ------------------ */
		table_name =
			get_table_name( application_name,
					"surface_vegetation" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | html_paragraph_wrapper.e",
			 table_name,
			 FIU_SURFACE_VEGETATION_FIELD_LIST );
		surface_vegetation_output_pipe = popen( sys_string, "w" );

		/* Submergent Vegetation */
		/* --------------------- */
		table_name =
			get_table_name( application_name,
					"submergent_vegetation" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | html_paragraph_wrapper.e",
			 table_name,
			 FIU_SUBMERGENT_VEGETATION_FIELD_LIST );
		submergent_vegetation_output_pipe = popen( sys_string, "w" );

	}
	else
	{
		sprintf( sys_string,
"queue_top_bottom_lines.e 50 | html_table.e 'Insert into Surface/Submergent Vegetation' %s '|' %s",
			 FIU_VEGETATION_FIELD_LIST,
			 FIU_VEGETATION_JUSTIFY_COMMA_LIST );
		surface_vegetation_output_pipe = popen( sys_string, "w" );
	}

	while( get_line( input_string, input_file ) )
	{
		line_number++;

		if ( !piece_quoted( region_code, ',', input_string, 0, '"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece region code in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( strcmp( region_code, "REGION" ) == 0 ) continue;

		if ( ! ( region =
				freshwaterfish_get_region(
					application_name,
					region_code ) ) )
		{
			fprintf(error_file,
"Warning in line %d: Cannot get FIU region for region code = %s in (%s). This information is hardcoded into the source code. Please contact Tim at (916) 641-7789.\n",
				line_number,
				region_code,
				input_string );
			continue;
		}

		if ( !piece_quoted( site, ',', input_string, 6, '"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece site in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		strcpy( site, freshwaterfish_get_clean_site( site ) );

		if ( !piece_quoted( plot, ',', input_string, 7, '"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece plot in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	year,
					',',
					input_string,
					2,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece year in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	month,
					',',
					input_string,
					4,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece month in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	day,
					',',
					input_string,
					5,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece day in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		sprintf(	visit_date_international,
				"%s-%s-%s",
				year,
				month,
				day );

		if ( !piece_quoted(	throw_number,
					',',
					input_string,
					8,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece throw number in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		/* Species start at piece = 9 */
		/* -------------------------- */
		spreadsheet_offset = 9;

		array_offset = 0;

		while( piece( density, ',', input_string, spreadsheet_offset ) )
		{
			if ( array_offset == array_length ) break;
	
			if ( species_array[ array_offset ]->surface_vegetation )
			{
				scientific_name =
					species_array[ array_offset ]->
					surface_vegetation->
					scientific_name;
			}
			else
			if ( species_array[ array_offset ]->
							submergent_vegetation )
			{
				scientific_name =
					species_array[ array_offset ]->
					submergent_vegetation->
					scientific_name;
			}
			else
			{
				fprintf( stderr,
"ERROR in %s/%s()/%d: both surface and submergent vegetation structures are empty.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

/*
if ( strcmp( site, "6" ) == 0
&&   strcmp( plot, "A" ) == 0
&&   strcmp( region, "shark_river_slough" ) == 0
&&   strcmp( visit_date_international, "2008-2-14" ) == 0
&&   strcmp( throw_number, "1" ) == 0
&&   strcmp( scientific_name, "Utricularia spp." ) == 0 )
{
*/

			if ( atoi( density ) )
			{
				output_vegetation_data(
					site_output_pipe,
					site_visit_output_pipe,
					throw_output_pipe,
					surface_vegetation_output_pipe,
					submergent_vegetation_output_pipe,
					really_yn,
					site,
					plot,
					region,
					throw_number,
					visit_date_international,
					scientific_name,
					density,
					(boolean)
					species_array[ array_offset ]->
						surface_vegetation );
				record_count++;
			}
/*
}
*/
			spreadsheet_offset++;
			array_offset++;
		}
	}

	fclose( input_file );
	fclose( error_file );
	pclose( surface_vegetation_output_pipe );

	if ( really_yn == 'y' )
	{
		pclose( site_output_pipe );
		pclose( site_visit_output_pipe );
		pclose( throw_output_pipe );
		pclose( submergent_vegetation_output_pipe );
	}

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Vegetation Errors' '' '|'",
			 error_filename );
		system( sys_string );
	}

	sprintf( sys_string, "rm %s", error_filename );
	system( sys_string );

	return record_count;
} /* insert_vegetation() */

int insert_crayfish(	char *application_name,
			char *input_filename,
			char really_yn )
{
	FILE *input_file;
	FILE *site_output_pipe = {0};
	FILE *site_visit_output_pipe = {0};
	FILE *throw_output_pipe = {0};
	FILE *measurement_output_pipe = {0};
	char input_string[ 2048 ];
	char *scientific_name;
	char fiu_new_code[ 16 ];
	char region_code[ 16 ];
	char *region;
	char year[ 16 ];
	char month[ 16 ];
	char day[ 16 ];
	char site[ 128 ];
	char plot[ 128 ];
	char throw_number[ 128 ];
	char visit_date_international[ 16 ];
	char length_millimeters[ 128 ];
	char sex_fiu_code[ 128 ];
	char *sex;
	char sys_string[ 1024 ];
	char *error_filename;
	FILE *error_file;
	char *measurement_number;
	int line_number = 0;
	int record_count = 0;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	if ( ! ( error_file = open_error_file( &error_filename ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		fclose( input_file );
		exit( 1 );
	}

	if ( really_yn == 'y' )
	{
		char *table_name;

		/* Site */
		/* ---- */
		table_name =
			get_table_name( application_name, "site" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 FIU_SITE_FIELD_LIST );
		site_output_pipe = popen( sys_string, "w" );

		/* Site Visit */
		/* ---------- */
		table_name =
			get_table_name( application_name, "site_visit" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 FIU_SITE_VISIT_FIELD_LIST );
		site_visit_output_pipe = popen( sys_string, "w" );

		/* Throw */
		/* ----- */
		table_name =
			get_table_name( application_name, "throw" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 FIU_THROW_FIELD_LIST );
		throw_output_pipe = popen( sys_string, "w" );

		/* Measurement */
		/* ----------- */
		table_name =
			get_table_name( application_name, "measurement" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | html_paragraph_wrapper.e",
			 table_name,
			 FIU_MEASUREMENT_FIELD_LIST );
		measurement_output_pipe = popen( sys_string, "w" );
	}
	else
	{
		sprintf( sys_string,
"queue_top_bottom_lines.e 50 | html_table.e 'Insert into Measurement' %s '|' %s",
			 FIU_MEASUREMENT_FIELD_LIST,
			 FIU_MEASUREMENT_JUSTIFY_COMMA_LIST );
		measurement_output_pipe = popen( sys_string, "w" );
	}

	while( get_line( input_string, input_file ) )
	{
		line_number++;

		if ( !piece_quoted( region_code, ',', input_string, 0, '"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece region code in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( strcmp( region_code, "REGION" ) == 0 ) continue;

		if ( ! ( region =
				freshwaterfish_get_region(
					application_name,
					region_code ) ) )
		{
			fprintf(error_file,
"Warning in line %d: Cannot get FIU region for region code = %s in (%s). This information is hardcoded into the source code. Please contact Tim at (916) 641-7789.\n",
				line_number,
				region_code,
				input_string );
			continue;
		}

		if ( !piece_quoted( site, ',', input_string, 6, '"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece site in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		strcpy( site, freshwaterfish_get_clean_site( site ) );

		if ( !piece_quoted( plot, ',', input_string, 7, '"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece plot in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	fiu_new_code,
					',',
					input_string,
					9,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece fiu new code in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( strcasecmp( fiu_new_code, "nocray" ) == 0 )
			continue;
		else
		if ( !*fiu_new_code )
		{
			continue;
		}
		else
		if ( ! ( scientific_name =
			freshwaterfish_library_get_scientific_name(
				(char *)0 /* nps_code */,
				(char *)0 /* fiu_fish_old_code */,
				(char *)0 /* fiu_non_fish_old_code */,
				fiu_new_code,
				application_name ) ) )
		{
			fprintf(error_file,
"Warning in line %d: Cannot get scientific name for FIU new code = (%s) in (%s).\n",
				line_number,
				fiu_new_code,
				input_string );
			continue;
		}

		if ( !piece_quoted(	year,
					',',
					input_string,
					2,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece year in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	month,
					',',
					input_string,
					4,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece month in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	day,
					',',
					input_string,
					5,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece day in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		sprintf(	visit_date_international,
				"%s-%s-%s",
				year,
				month,
				day );

		if ( !piece_quoted(	throw_number,
					',',
					input_string,
					8,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece throw number in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	length_millimeters,
					',',
					input_string,
					12,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece length in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	sex_fiu_code,
					',',
					input_string,
					10,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece sex code in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !*sex_fiu_code )
		{
			sex = "";
		}
		else
		if ( ! ( sex = freshwaterfish_with_fiu_code_get_sex(
					application_name,
					sex_fiu_code ) ) )
		{
			fprintf(error_file,
		"Ignoring in line %d: Cannot get sex using code = %s in (%s). Ignoring column.\n",
				line_number,
				sex_fiu_code,
				input_string );
			sex = "";
		}

		measurement_number =
			get_measurement_number(
				application_name,
				input_filename,
				really_yn );

		output_measurement_output_pipe(
			measurement_output_pipe,
			site,
			plot,
			region,
			visit_date_international,
			throw_number,
			scientific_name,
			measurement_number,
			length_millimeters,
			sex,
			"" /* no_measurement_count */,
			really_yn );

		if ( really_yn == 'y' )
		{
			fprintf(site_output_pipe,
				"%s|%s|%s\n",
				site,
				plot,
				region );

			fprintf(site_visit_output_pipe,
				"%s|%s|%s|%s|%s\n",
				site,
				plot,
				region,
				visit_date_international,
				AGENCY_FIU );

			fprintf(throw_output_pipe,
				"%s|%s|%s|%s|%s\n",
				site,
				plot,
				region,
				visit_date_international,
				throw_number );
		}
		record_count++;
	}

	fclose( input_file );
	fclose( error_file );
	pclose( measurement_output_pipe );

	if ( really_yn == 'y' )
	{
		pclose( site_output_pipe );
		pclose( site_visit_output_pipe );
		pclose( throw_output_pipe );
	}

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Measurement Errors' '' '|'",
			 error_filename );
		system( sys_string );
	}

	sprintf( sys_string, "rm %s", error_filename );
	system( sys_string );
	return record_count;

} /* insert_crayfish() */

int insert_fish(	char *application_name,
			char *input_filename,
			char really_yn )
{
	FILE *input_file;
	FILE *site_output_pipe = {0};
	FILE *site_visit_output_pipe = {0};
	FILE *throw_output_pipe = {0};
	FILE *measurement_output_pipe = {0};
	char input_string[ 2048 ];
	char *scientific_name;
	char fiu_new_code[ 16 ];
	char region_code[ 16 ];
	char *region;
	char year[ 16 ];
	char month[ 16 ];
	char day[ 16 ];
	char site[ 128 ];
	char plot[ 128 ];
	char throw_number[ 128 ];
	char visit_date_international[ 16 ];
	char length_millimeters[ 128 ];
	char sex_fiu_code[ 128 ];
	char *sex;
	char sys_string[ 1024 ];
	char *error_filename;
	FILE *error_file;
	char *measurement_number;
	int line_number = 0;
	int record_count = 0;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	if ( ! ( error_file = open_error_file( &error_filename ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		fclose( input_file );
		exit( 1 );
	}

	if ( really_yn == 'y' )
	{
		char *table_name;

		/* Site */
		/* ---- */
		table_name =
			get_table_name( application_name, "site" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 FIU_SITE_FIELD_LIST );
		site_output_pipe = popen( sys_string, "w" );

		/* Site Visit */
		/* ---------- */
		table_name =
			get_table_name( application_name, "site_visit" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 FIU_SITE_VISIT_FIELD_LIST );
		site_visit_output_pipe = popen( sys_string, "w" );

		/* Throw */
		/* ----- */
		table_name =
			get_table_name( application_name, "throw" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 FIU_THROW_FIELD_LIST );
		throw_output_pipe = popen( sys_string, "w" );

		/* Measurement */
		/* ----------- */
		table_name =
			get_table_name( application_name, "measurement" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | html_paragraph_wrapper.e",
			 table_name,
			 FIU_MEASUREMENT_FIELD_LIST );
		measurement_output_pipe = popen( sys_string, "w" );
	}
	else
	{
		sprintf( sys_string,
"queue_top_bottom_lines.e 50 | html_table.e 'Insert into Measurement' %s '|' %s",
			 FIU_MEASUREMENT_FIELD_LIST,
			 FIU_MEASUREMENT_JUSTIFY_COMMA_LIST );
		measurement_output_pipe = popen( sys_string, "w" );
	}

	while( get_line( input_string, input_file ) )
	{
		line_number++;

		if ( !piece_quoted( region_code, ',', input_string, 0, '"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece region code in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( strcmp( region_code, "REGION" ) == 0 ) continue;

		if ( ! ( region =
				freshwaterfish_get_region(
					application_name,
					region_code ) ) )
		{
			fprintf(error_file,
"Warning in line %d: Cannot get FIU region for region code = %s in (%s). This information is hardcoded into the source code. Please contact Tim at (916) 641-7789.\n",
				line_number,
				region_code,
				input_string );
			continue;
		}

		if ( !piece_quoted( site, ',', input_string, 6, '"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece site in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		strcpy( site, freshwaterfish_get_clean_site( site ) );

		if ( !piece_quoted( plot, ',', input_string, 7, '"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece plot in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	fiu_new_code,
					',',
					input_string,
					9,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece fiu new code in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !*fiu_new_code )
		{
			continue;
		}
		else
		if ( ! ( scientific_name =
			freshwaterfish_library_get_scientific_name(
				(char *)0 /* nps_code */,
				(char *)0 /* fiu_fish_old_code */,
				(char *)0 /* fiu_non_fish_old_code */,
				fiu_new_code,
				application_name ) ) )
		{
			fprintf(error_file,
"Warning in line %d: Cannot get scientific name for FIU new code = (%s) in (%s).\n",
				line_number,
				fiu_new_code,
				input_string );
			continue;
		}

		if ( !piece_quoted(	year,
					',',
					input_string,
					2,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece year in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	month,
					',',
					input_string,
					4,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece month in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	day,
					',',
					input_string,
					5,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece day in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		sprintf(	visit_date_international,
				"%s-%s-%s",
				year,
				month,
				day );

		if ( !piece_quoted(	throw_number,
					',',
					input_string,
					8,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece throw number in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	length_millimeters,
					',',
					input_string,
					10,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece length in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	sex_fiu_code,
					',',
					input_string,
					11,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece sex code in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !*sex_fiu_code )
		{
			sex = "";
		}
		else
		if ( ! ( sex = freshwaterfish_with_fiu_code_get_sex(
					application_name,
					sex_fiu_code ) ) )
		{
			fprintf(error_file,
"Ignoring in line %d: Cannot get sex using code = %s in (%s). Ignoring column.\n",
				line_number,
				sex_fiu_code,
				input_string );
			sex = "";
		}

		measurement_number =
			get_measurement_number(
				application_name,
				input_filename,
				really_yn );

		output_measurement_output_pipe(
			measurement_output_pipe,
			site,
			plot,
			region,
			visit_date_international,
			throw_number,
			scientific_name,
			measurement_number,
			length_millimeters,
			sex,
			"" /* no_measurement_count */,
			really_yn );

		if ( really_yn == 'y' )
		{
			fprintf(site_output_pipe,
				"%s|%s|%s\n",
				site,
				plot,
				region );

			fprintf(site_visit_output_pipe,
				"%s|%s|%s|%s|%s\n",
				site,
				plot,
				region,
				visit_date_international,
				AGENCY_FIU );

			fprintf(throw_output_pipe,
				"%s|%s|%s|%s|%s\n",
				site,
				plot,
				region,
				visit_date_international,
				throw_number );
		}
		record_count++;
	}

	fclose( input_file );
	fclose( error_file );
	pclose( measurement_output_pipe );

	if ( really_yn == 'y' )
	{
		pclose( site_output_pipe );
		pclose( site_visit_output_pipe );
		pclose( throw_output_pipe );
	}

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Measurement Errors' '' '|'",
			 error_filename );
		system( sys_string );
	}

	sprintf( sys_string, "rm %s", error_filename );
	system( sys_string );
	return record_count;

} /* insert_fish() */

char *get_measurement_number(	char *application_name,
				char *input_filename,
				char really_yn )
{
	static boolean first_time = 1;
	static int next_measurement_number = 0;
	static char measurement_number[ 16 ];

	if ( really_yn != 'y' ) return "To Be Determined";

	if ( first_time )
	{
		next_measurement_number =
			freshwaterfish_get_measurement_number(
					application_name,
					input_filename );
		first_time = 0;
	}
	sprintf( measurement_number, "%d", next_measurement_number++ );
	return measurement_number;
} /* get_measurement_number() */

void output_measurement_output_pipe(
			FILE *measurement_output_pipe,
			char *site,
			char *plot,
			char *region,
			char *visit_date_international,
			char *throw_number,
			char *scientific_name,
			char *measurement_number,
			char *length_millimeters,
			char *sex,
			char *no_measurement_count,
			char really_yn )
{
	char region_buffer[ 128 ];
	char sex_buffer[ 128 ];

/*
#define FIU_MEASUREMENT_FIELD_LIST	"site,plot,region,visit_date,throw_number,scientific_name,measurement_number,length_millimeters,sex,no_measurement_count"
*/
	if ( really_yn == 'y' )
	{
		fprintf(measurement_output_pipe,
			"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
			site,
			plot,
			region,
			visit_date_international,
			throw_number,
			scientific_name,
			measurement_number,
			length_millimeters,
			sex,
			no_measurement_count );
	}
	else
	{
		fprintf(measurement_output_pipe,
			"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
			site,
			plot,
			format_initial_capital( region_buffer, region ),
			visit_date_international,
			throw_number,
			scientific_name,
			measurement_number,
			length_millimeters,
			format_initial_capital( sex_buffer, sex ),
			no_measurement_count );
	}

} /* output_measurement_output_pipe() */

int insert_update_physical(	char *application_name,
				char *input_filename,
				char really_yn )
{
	FILE *input_file;
	FILE *throw_update_pipe = {0};
	FILE *throw_output_pipe = {0};
	FILE *site_visit_insert_pipe = {0};
	char input_string[ 2048 ];
	char region_code[ 16 ];
	char *region;
	char region_buffer[ 128 ];
	char year[ 16 ];
	char month[ 16 ];
	char day[ 16 ];
	char site[ 128 ];
	char plot[ 128 ];
	char throw_number[ 128 ];
	char visit_date_international[ 16 ];
	char water_depth_cm[ 128 ];
	char average_plant_height_cm[ 128 ];
	char emergence_plant_cover_percent[ 128 ];
	char periphyton_cover_percent[ 128 ];
	char periphyton_volume_milliliter[ 128 ];
	char sys_string[ 1024 ];
	char *error_filename;
	FILE *error_file;
	int line_number = 0;
	int record_count = 0;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	if ( ! ( error_file = open_error_file( &error_filename ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		fclose( input_file );
		exit( 1 );
	}

	if ( really_yn == 'y' )
	{
		char *table_name;

		/* Site visit insert */
		/* ----------------- */
		table_name =
			get_table_name( application_name, "site_visit" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 FIU_SITE_VISIT_FIELD_LIST );
		site_visit_insert_pipe = popen( sys_string, "w" );

		/* Throw update */
		/* ------------ */
		table_name =
			get_table_name( application_name, "throw" );

		sprintf( sys_string,
"update_statement.e t=%s k=%s carrot=y | sql.e 2>&1 | html_paragraph_wrapper.e",
			 table_name,
			 FIU_THROW_KEY_FIELD_LIST );
		throw_update_pipe = popen( sys_string, "w" );

		/* Throw insert */
		/* ------------ */
		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 FIU_THROW_FULL_FIELD_LIST );
		throw_output_pipe = popen( sys_string, "w" );
	}
	else
	{
		sprintf( sys_string,
"queue_top_bottom_lines.e 50 | html_table.e 'Update Throw' %s '|' %s",
			 FIU_THROW_FULL_FIELD_LIST,
			 FIU_THROW_FULL_JUSTIFY_COMMA_LIST );
		throw_output_pipe = popen( sys_string, "w" );
	}

	while( get_line( input_string, input_file ) )
	{
		line_number++;

		if ( !piece_quoted( region_code, ',', input_string, 0, '"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece region code in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( strcmp( region_code, "REGION" ) == 0 ) continue;

		if ( ! ( region =
				freshwaterfish_get_region(
					application_name,
					region_code ) ) )
		{
			fprintf(error_file,
"Warning in line %d: Cannot get FIU region for region code = %s in (%s). This information is hardcoded into the source code. Please contact Tim at (916) 641-7789.\n",
				line_number,
				region_code,
				input_string );
			continue;
		}

		if ( !piece_quoted( site, ',', input_string, 6, '"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece site in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		strcpy( site, freshwaterfish_get_clean_site( site ) );

		if ( !piece_quoted( plot, ',', input_string, 7, '"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece plot in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	year,
					',',
					input_string,
					2,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece year in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	month,
					',',
					input_string,
					4,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece month in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	day,
					',',
					input_string,
					5,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece day in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		sprintf(	visit_date_international,
				"%s-%s-%s",
				year,
				month,
				day );

		if ( !piece_quoted(	throw_number,
					',',
					input_string,
					8,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece throw number in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	emergence_plant_cover_percent,
					',',
					input_string,
					9,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece plant height in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	average_plant_height_cm,
					',',
					input_string,
					10,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece plant height in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	periphyton_cover_percent,
					',',
					input_string,
					11,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece periphyton cover in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	periphyton_volume_milliliter,
					',',
					input_string,
					12,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece periphyton volume in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( !piece_quoted(	water_depth_cm,
					',',
					input_string,
					13,
					'"' ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece water depth in (%s).\n",
				line_number,
				input_string );
			continue;
		}

		if ( really_yn == 'y' )
		{
			if ( *average_plant_height_cm )
			{
				fprintf(throw_update_pipe,
				"%s^%s^%s^%s^%s^average_plant_height_cm^%s\n",
				site,
				plot,
				region,
				visit_date_international,
				throw_number,
				average_plant_height_cm );
			}

			if ( *emergence_plant_cover_percent )
			{
				fprintf(throw_update_pipe,
			"%s^%s^%s^%s^%s^emergence_plant_cover_percent^%s\n",
				site,
				plot,
				region,
				visit_date_international,
				throw_number,
				emergence_plant_cover_percent );
			}

			if ( *emergence_plant_cover_percent )
			{
				fprintf(throw_update_pipe,
			"%s^%s^%s^%s^%s^emergence_plant_cover_percent^%s\n",
				site,
				plot,
				region,
				visit_date_international,
				throw_number,
				emergence_plant_cover_percent );
			}

			if ( *periphyton_cover_percent )
			{
				fprintf(throw_update_pipe,
				"%s^%s^%s^%s^%s^periphyton_cover_percent^%s\n",
				site,
				plot,
				region,
				visit_date_international,
				throw_number,
				periphyton_cover_percent );
			}

			if ( *periphyton_volume_milliliter )
			{
				fprintf(throw_update_pipe,
			"%s^%s^%s^%s^%s^periphyton_volume_milliliter^%s\n",
				site,
				plot,
				region,
				visit_date_international,
				throw_number,
				periphyton_volume_milliliter );
			}

			if ( *water_depth_cm )
			{
				fprintf(throw_update_pipe,
				"%s^%s^%s^%s^%s^water_depth_cm^%s\n",
				site,
				plot,
				region,
				visit_date_international,
				throw_number,
				water_depth_cm );
			}

			fprintf(site_visit_insert_pipe,
				"%s|%s|%s|%s|%s\n",
				site,
				plot,
				region,
				visit_date_international,
				AGENCY_FIU );

			fprintf(throw_output_pipe,
				"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
				site,
				plot,
				region,
				visit_date_international,
				throw_number,
				water_depth_cm,
				average_plant_height_cm,
				emergence_plant_cover_percent,
				periphyton_cover_percent,
				periphyton_volume_milliliter );
		}
		else
		{
			fprintf(throw_output_pipe,
				"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
				site,
				plot,
				format_initial_capital(
					region_buffer, region ),
				visit_date_international,
				throw_number,
				water_depth_cm,
				average_plant_height_cm,
				emergence_plant_cover_percent,
				periphyton_cover_percent,
				periphyton_volume_milliliter );
		}
		record_count++;
	}

	fclose( input_file );
	fclose( error_file );
	pclose( throw_output_pipe );

	if ( really_yn == 'y' )
	{
		pclose( throw_update_pipe );
		pclose( site_visit_insert_pipe );
	}

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Measurement Errors' '' '|'",
			 error_filename );
		system( sys_string );
	}

	sprintf( sys_string, "rm %s", error_filename );
	system( sys_string );
	return record_count;

} /* insert_update_physical() */

MEASUREMENT **get_inverts_verts_species_array(
				int *array_length,
				char *application_name,
				char *input_filename )
{
	FILE *input_file;
	MEASUREMENT **species_array = {0};
	char input_string[ 2048 ];
	int number_commas;
	char fiu_new_code[ 128 ];
	char *scientific_name;
	int i;
	/* --------------------------------------- */
	/* The array might not be fully populated. */
	/* --------------------------------------- */
	int local_array_length;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot open file for read: %s.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			input_filename );
		exit( 1 );
	}

	*array_length = 0;

	if( !get_line( input_string, input_file ) )
	{
		printf( "<h3>ERROR: Empty input file.</h3>\n" );
		return (MEASUREMENT **)0;
	}

	number_commas = character_count( ',', input_string );

	/* 10 columns before the first species, minus 1. */
	/* --------------------------------------------- */
	local_array_length = number_commas - 9;

	if ( local_array_length < 1 )
	{
		fclose( input_file );
		return species_array;
	}

	species_array =
		(MEASUREMENT **)
			timlib_allocate_pointer_array(
				sizeof( MEASUREMENT ** ),
				sizeof( MEASUREMENT * ),
				local_array_length );

	/* 10 columns before the first species */
	/* ----------------------------------- */
	i = 10;

	while( piece( fiu_new_code, ',', input_string, i++ ) )
	{
		if( strcasecmp( fiu_new_code, "COMMENT" ) == 0 )
		{
			break;
		}

		scientific_name =
			freshwaterfish_library_get_scientific_name(
				(char *)0 /* nps_code */,
				(char *)0 /* fiu_fish_old_code */,
				(char *)0 /* fiu_non_fish_old_code */,
				fiu_new_code,
				application_name );

		if ( !scientific_name )
		{
			printf(
"<h3>ERROR: cannot find scientific name for FIU new code = %s.</h3>\n",
				fiu_new_code );
			return (MEASUREMENT **)0;
		}

		species_array[ *array_length ] =
			freshwaterfish_measurement_new(
				0 /* measurement_number */,
				scientific_name,
				(char *)0 /* length_millimeters */,
				(char *)0 /* sex */,
				(char *)0 /* weight_grams */,
				(char *)0 /* no_measurement_count */,
				(char *)0 /* no_measurement_total_weight */,
				(char *)0 /* comments */ );

		(*array_length)++;
	}

	fclose( input_file );
	return species_array;
} /* get_inverts_verts_species_array() */

LOAD_FIU_VEGETATION_SPECIES **get_vegetation_species_array(
				int *array_length,
				char *application_name,
				char *input_filename )
{
	FILE *input_file;
	LOAD_FIU_VEGETATION_SPECIES **species_array = {0};
	char input_string[ 2048 ];
	int number_commas;
	char fiu_new_code[ 128 ];
	char *scientific_name;
	int i;
	char *species_category;
	/* --------------------------------------- */
	/* The array might not be fully populated. */
	/* --------------------------------------- */
	int local_array_length;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot open file for read: %s.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			input_filename );
		exit( 1 );
	}

	*array_length = 0;

	if( !get_line( input_string, input_file ) )
	{
		printf( "<h3>ERROR: Empty input file.</h3>\n" );
		return (LOAD_FIU_VEGETATION_SPECIES **)0;
	}

	number_commas = character_count( ',', input_string );

	/* 9 columns before the first species, minus 1. */
	/* -------------------------------------------- */
	local_array_length = number_commas - 8;

	if ( local_array_length < 1 )
	{
		fclose( input_file );
		return species_array;
	}

	species_array =
		(LOAD_FIU_VEGETATION_SPECIES **)
			timlib_allocate_pointer_array(
				sizeof( LOAD_FIU_VEGETATION_SPECIES ** ),
				sizeof( LOAD_FIU_VEGETATION_SPECIES * ),
				local_array_length );

	/* 9 columns before the first species */
	/* ---------------------------------- */
	i = 9;

	while( piece( fiu_new_code, ',', input_string, i++ ) )
	{
		if( strcasecmp( fiu_new_code, "COMMENT" ) == 0 )
		{
			break;
		}

		scientific_name =
			freshwaterfish_library_get_scientific_name(
				(char *)0 /* nps_code */,
				(char *)0 /* fiu_fish_old_code */,
				(char *)0 /* fiu_non_fish_old_code */,
				fiu_new_code,
				application_name );

		if ( !scientific_name )
		{
			printf(
"<h3>ERROR: cannot find scientific name for FIU new code = %s.</h3>\n",
				fiu_new_code );
			return (LOAD_FIU_VEGETATION_SPECIES **)0;
		}

		if ( ! ( species_category =
			freshwaterfish_library_get_species_category(
				scientific_name,
				application_name ) ) )
		{
			printf(
"<h3>ERROR: cannot find species category for scientific name = %s.</h3>\n",
				scientific_name );
			return (LOAD_FIU_VEGETATION_SPECIES **)0;
		}

		if ( strcmp(	species_category,
				"surface_vegetation" ) == 0 )
		{
			species_array[ *array_length ]->
				surface_vegetation =
			freshwaterfish_surface_vegetation_new(
				scientific_name,
				(char *)0 /* density_alive_count */,
				(char *)0 /* density_dead_count */ );
		}
		else
		if ( strcmp(	species_category,
				"submergent_vegetation" ) == 0 )
		{
			species_array[ *array_length ]->
				submergent_vegetation =
			freshwaterfish_submergent_vegetation_new(
				scientific_name,
				(char *)0 /* density_percent */ );
		}
		else
		{
			printf(
"<h3>ERROR: invalid species category of %s for scientific name = %s.</h3>\n",
				species_category,
				scientific_name );
			return (LOAD_FIU_VEGETATION_SPECIES **)0;
		}

		(*array_length)++;
	}

	fclose( input_file );
	return species_array;
} /* get_vegetation_species_array() */

void output_vegetation_data(
					FILE *site_output_pipe,
					FILE *site_visit_output_pipe,
					FILE *throw_output_pipe,
					FILE *surface_vegetation_output_pipe,
					FILE *submergent_vegetation_output_pipe,
					int really_yn,
					char *site,
					char *plot,
					char *region,
					char *throw_number,
					char *visit_date_international,
					char *scientific_name,
					char *density,
					boolean is_surface )
{
	char region_buffer[ 128 ];

	if ( really_yn == 'y' )
	{
		fprintf(site_output_pipe,
			"%s|%s|%s\n",
			site,
			plot,
			region );
	
		fprintf(site_visit_output_pipe,
			"%s|%s|%s|%s|%s\n",
			site,
			plot,
			region,
			visit_date_international,
			AGENCY_FIU );

		fprintf(throw_output_pipe,
			"%s|%s|%s|%s|%s\n",
			site,
			plot,
			region,
			visit_date_international,
			throw_number );

		if ( is_surface )
		{
			fprintf(surface_vegetation_output_pipe,
				"%s|%s|%s|%s|%s|%s|%s\n",
				site,
				plot,
				region,
				visit_date_international,
				throw_number,
				scientific_name,
				density );
		}
		else
		{
			fprintf(submergent_vegetation_output_pipe,
				"%s|%s|%s|%s|%s|%s|%s\n",
				site,
				plot,
				region,
				visit_date_international,
				throw_number,
				scientific_name,
				density );
		}
	}
	else
	/* -------------------- */
	/* Not really_yn == 'y' */
	/* -------------------- */
	{
		/* They both go to one display table */
		/* --------------------------------- */
		fprintf(surface_vegetation_output_pipe,
			"%s|%s|%s|%s|%s|%s|%s\n",
			site,
			plot,
			format_initial_capital( region_buffer, region ),
			visit_date_international,
			throw_number,
			scientific_name,
			density );
	}
} /* output_vegetation_data() */

FILE *open_error_file( char **error_filename )
{
	static char local_error_filename[ 128 ];

	sprintf( local_error_filename,
		 "/tmp/load_fiu_data_error_%d.txt",
		 getpid() );
	*error_filename = local_error_filename;
	return fopen( local_error_filename, "w" );
} /* open_error_file() */

