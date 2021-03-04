/* ---------------------------------------------------	*/
/* load_visit_spreadsheet.c				*/
/* ---------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "list.h"
#include "environ.h"
#include "date.h"
#include "process.h"
#include "date_convert.h"
#include "application.h"
#include "load_spreadsheets.h"
#include "alligator.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char really_yn;
	char *input_filename;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	int load_count;

	if ( argc != 5 )
	{
		fprintf( stderr, 
"Usage: %s application process_name filename really_yn\n",
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

	printf( "<h2>Load Visit Spreadsheet\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( really_yn == 'y' )
	{
		delete_observation( application_name, input_filename );
	}

	load_count = insert_observations(
			application_name,
			input_filename,
			really_yn );

	if ( really_yn == 'y' )
		printf( "<p>Process complete with %d nest observations.\n",
			load_count );
	else
		printf( "<p>Process not load %d nest observations.\n",
			load_count );

	document_close();

	if ( really_yn == 'y' )
	{
		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}

	exit( 0 );
} /* main() */

int insert_observations(	char *application_name,
				char *input_filename,
				char really_yn )
{
	char sys_string[ 1024 ];
	FILE *input_file;
	FILE *error_file;
	char input_string[ 1024 ];
	char *table_name;
	char error_filename[ 128 ];
	FILE *table_output_pipe = {0};
	FILE *nest_observation_insert_pipe = {0};
	FILE *water_depth_insert_pipe = {0};
	FILE *nest_update_pipe = {0};
	FILE *output_pipe;
	char discovery_year[ 128 ];
	char observation_date_american[ 128 ];
	char observation_date_international[ 128 ];
	char observation_view[ 128 ];
	char nest_number[ 128 ];
	char total_eggs[ 128 ];
	char banded_eggs[ 128 ];
	char not_banded_eggs[ 128 ];
	char hatched_eggs[ 128 ];
	char flooded_eggs[ 128 ];
	char predated_eggs[ 128 ];
	char crushed_eggs[ 128 ];
	char other_egg_mortality[ 128 ];
	char removed_viable_eggs[ 128 ];
	char removed_non_viable_eggs[ 128 ];
	char female_behavior_code[ 128 ];
	char *female_behavior;
	char nest_status_code[ 128 ];
	char *nest_status;
	char hatchlings_count[ 128 ];
	char water_depth_average_cm[ 128 ];
	char nest_height_above_water_cm[ 128 ];
	char observation_notepad[ 512 ];
	char materials_code[ 128 ];
	char *materials;
	char monitor_nest_yn[ 128 ];
	char nest_visit_number[ 128 ];
	char female_size_cm[ 128 ];
	char nest_height_cm[ 128 ];
	char water_depth[ 128 ];
	int load_count = 0;
	int line_in_file = 0;
	char habitat_code[ 128 ];
	char *habitat;
	char *nest_number_key;

	/* Open the input file */
	/* ------------------- */
	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		printf( "<h2>ERROR: cannot open %s for read</h2>\n",
			input_filename );
		return 0;
	}

	/* Open the error file */
	/* ------------------- */
	sprintf(error_filename,
		"/tmp/load_srf_spreadsheet_file_error_%d.txt", getpid() );
	if ( ! ( error_file = fopen( error_filename, "w" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		fclose( input_file );
		exit( 1 );
	}

	if ( really_yn == 'y' )
	{
		/* Open the nest observation insert pipe */
		/* ------------------------------------- */
		table_name =
			get_table_name(	application_name,
					"nest_visit" );

		sprintf( sys_string,
"insert_statement.e table=%s field=%s delimiter='|' | sql.e 2>&1 | html_paragraph_wrapper.e",
		 	table_name,
		 	INSERT_NEST_OBSERVATION );

		nest_observation_insert_pipe = popen( sys_string, "w" );

		/* Open the nest update pipe */
		/* ------------------------- */
		table_name =
			get_table_name(	application_name,
					"nest" );

		sprintf( sys_string,
"update_statement.e table=%s key_field=%s carrot=y | sql.e 2>&1 | html_paragraph_wrapper.e",
		 	table_name,
		 	NEST_PRIMARY_KEY );

		nest_update_pipe = popen( sys_string, "w" );

		/* Open the water depth insert pipe */
		/* -------------------------------- */
		table_name =
			get_table_name(	application_name,
					"water_depth" );

		sprintf( sys_string,
"insert_statement.e table=%s field=%s delimiter='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
		 	table_name,
		 	INSERT_WATER_DEPTH );

		water_depth_insert_pipe = popen( sys_string, "w" );
	}
	else
	{
		char *justify_list_string =
"left,left,left,left,left,left,left,left,right,right,right,right,right,right,right,right,right,right,right,right,right,right";

		sprintf( sys_string,
"queue_top_bottom_lines.e 50 | html_table.e 'Insert into Nest Observations' %s '|' %s",
			 INSERT_NEST_OBSERVATION,
			 justify_list_string );

		table_output_pipe = popen( sys_string, "w" );
	}

	/* Skip the first line. */
	/* -------------------- */
	get_line( input_string, input_file );
	line_in_file++;

	while( get_line( input_string, input_file ) )
	{
		line_in_file++;
		trim( input_string );
		if ( !*input_string ) continue;

		if ( !piece_double_quoted(
				female_behavior_code,
				',',
				input_string,
				OBSERVATION_FEMALE_BEHAVIOR_CODE ) )
		{
			fprintf( error_file,
			"Warning: cannot get female behavior code in %s\n",
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				discovery_year,
				',',
				input_string,
				OBSERVATION_DISCOVERY_YEAR ) )
		{
			fprintf( error_file,
				 "Warning: cannot discovery year in %s\n",
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				nest_number,
				',',
				input_string,
				OBSERVATION_NEST_NUMBER ) )
		{
			fprintf( error_file,
				 "Warning: cannot nest number in %s\n",
				 input_string );
			continue;
		}

		if ( !*nest_number && !*discovery_year ) continue;

		nest_number_key =
			alligator_get_nest_key(
				(char *)0 /* discovery_date_international */,
				discovery_year,
				nest_number );

		if ( !piece_double_quoted(
				observation_date_american,
				',',
				input_string,
				OBSERVATION_DATE ) )
		{
			fprintf( error_file,
				 "Warning: cannot get observation date in %s\n",
				 input_string );
			continue;
		}

		*observation_date_international = '\0';

		date_convert_source_american(
				observation_date_international,
				international,
				observation_date_american );

		if ( !*observation_date_international )
		{
			fprintf( error_file,
				 "Warning: cannot convert date of (%s) in %s\n",
				 observation_date_american,
				 input_string );
			continue;
		}

		if ( !*female_behavior_code )
			female_behavior = "";
		else
		if ( ! ( female_behavior =
				alligator_get_female_behavior(
					application_name,
					female_behavior_code ) ) )
		{
			fprintf( error_file,
"Ignoring: in line = %d, cannot get behavior using code of (%s) in %s. Ignoring column.\n",
				 line_in_file,
				 female_behavior_code,
				 input_string );
			female_behavior = "";
		}

		if ( !piece_double_quoted(
				habitat_code,
				',',
				input_string,
				OBSERVATION_NEST_HABITAT_CODE ) )
		{
			fprintf( error_file,
			"Warning: cannot get nest habitat code in %s\n",
				 input_string );
			continue;
		}

		if ( !*habitat_code )
			habitat = "";
		else
		if ( ! ( habitat =
				alligator_get_habitat(
					application_name,
					habitat_code ) ) )
		{
			fprintf( error_file,
"Ignoring in line = %d, cannot nest habitat using code of (%s) in %s. Ignoring column.\n",
				 line_in_file,
				 habitat_code,
				 input_string );
			habitat = "";
		}

		if ( !piece_double_quoted(
				nest_status_code,
				',',
				input_string,
				OBSERVATION_NEST_STATUS_CODE ) )
		{
			fprintf( error_file,
		"Warning: cannot get nest status code in %s\n",
				 input_string );
			continue;
		}

		if ( ! ( nest_status =
				alligator_get_nest_status(
					application_name,
					nest_status_code ) ) )
		{
			fprintf( error_file,
"Ignoring: in line %d, cannot get nest status using code of (%s) in %s. Ignoring column.\n",
				 line_in_file,
				 nest_status_code,
				 input_string );
			nest_status = "";
		}

		if ( !piece_double_quoted(
				materials_code,
				',',
				input_string,
				OBSERVATION_MATERIALS_CODE ) )
		{
			fprintf( error_file,
			"Warning: cannot get materials code in %s\n",
				 input_string );
			continue;
		}

		if ( *materials_code )
		{
			if ( ! ( materials =
					alligator_get_materials(
						application_name,
						materials_code ) ) )
			{
				fprintf( error_file,
"Ignoring: in line = %d,, cannot materials using code of (%s) in %s. Ignoring column.\n",
					 line_in_file,
					 materials_code,
					 input_string );
				materials = "";
			}
		}
		else
		{
			materials = (char *)0;
		}

		if ( !piece_double_quoted(
				observation_view,
				',',
				input_string,
				OBSERVATION_VIEW ) )
		{
			fprintf( error_file,
			"Warning: cannot get observation view in %s\n",
				 input_string );
			continue;
		}

		if ( *observation_view )
		{
			if ( strcmp( observation_view, "ground" ) != 0
			&&   strcmp( observation_view, "aerial" ) != 0 )
			{
				fprintf( error_file,
"Ignoring: in line = %d, invalid observation view of (%s) in %s. Ignoring column.\n",
					line_in_file,
					observation_view,
				 	input_string );
				*observation_view = '\0';
			}
		}

		if ( !piece_double_quoted(
				monitor_nest_yn,
				',',
				input_string,
				OBSERVATION_MONITOR_NEST_YN ) )
		{
			fprintf( error_file,
			"Warning: cannot get monitor nest in %s\n",
				 input_string );
			continue;
		}

		if ( *monitor_nest_yn )
		{
			*monitor_nest_yn = tolower( *monitor_nest_yn );

			if ( *monitor_nest_yn != 'y' )
				*monitor_nest_yn = 'n';
		}

		if ( !piece_double_quoted(
				nest_visit_number,
				',',
				input_string,
				OBSERVATION_NEST_VISIT_NUMBER ) )
		{
			fprintf( error_file,
			"Warning: cannot get nest visit number in %s\n",
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				female_size_cm,
				',',
				input_string,
				OBSERVATION_FEMALE_SIZE_CM ) )
		{
			fprintf( error_file,
			"Warning: cannot get female size in %s\n",
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				nest_height_cm,
				',',
				input_string,
				OBSERVATION_NEST_HEIGHT_CM ) )
		{
			fprintf( error_file,
			"Warning: cannot get nest height in %s\n",
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				hatchlings_count,
				',',
				input_string,
				OBSERVATION_HATCHLINGS_COUNT ) )
		{
			fprintf( error_file,
			"Warning: cannot get hatchlings count in %s\n",
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				total_eggs,
				',',
				input_string,
				OBSERVATION_TOTAL_EGGS ) )
		{
			fprintf( error_file,
			"Warning: cannot get total eggs in %s\n",
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				banded_eggs,
				',',
				input_string,
				OBSERVATION_BANDED_EGGS ) )
		{
			fprintf( error_file,
			"Warning: cannot get banded eggs in %s\n",
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				not_banded_eggs,
				',',
				input_string,
				OBSERVATION_NOT_BANDED_EGGS ) )
		{
			fprintf( error_file,
			"Warning: cannot get not banded eggs in %s\n",
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				hatched_eggs,
				',',
				input_string,
				OBSERVATION_HATCHED_EGGS ) )
		{
			fprintf( error_file,
			"Warning: cannot get hatched eggs in %s\n",
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				flooded_eggs,
				',',
				input_string,
				OBSERVATION_FLOODED_EGGS ) )
		{
			fprintf( error_file,
			"Warning: cannot get flooded eggs in %s\n",
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				predated_eggs,
				',',
				input_string,
				OBSERVATION_PREDATED_EGGS ) )
		{
			fprintf( error_file,
			"Warning: cannot get predated eggs in %s\n",
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				crushed_eggs,
				',',
				input_string,
				OBSERVATION_CRUSHED_EGGS ) )
		{
			fprintf( error_file,
			"Warning: cannot get crushed eggs in %s\n",
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				other_egg_mortality,
				',',
				input_string,
				OBSERVATION_OTHER_EGG_MORTALITY ) )
		{
			fprintf( error_file,
			"Warning: cannot get other egg mortality in %s\n",
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				removed_viable_eggs,
				',',
				input_string,
				OBSERVATION_REMOVED_VIABLE_EGGS ) )
		{
			fprintf( error_file,
			"Warning: cannot get removed viable eggs in %s\n",
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				removed_non_viable_eggs,
				',',
				input_string,
				OBSERVATION_REMOVED_NON_VIABLE_EGGS ) )
		{
			fprintf( error_file,
			"Warning: cannot get removed non viable eggs in %s\n",
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				water_depth_average_cm,
				',',
				input_string,
				OBSERVATION_WATER_DEPTH_AVERAGE_CM ) )
		{
			fprintf( error_file,
			"Warning: cannot get water depth average in %s\n",
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				nest_height_above_water_cm,
				',',
				input_string,
				OBSERVATION_NEST_HEIGHT_ABOVE_WATER_CM ) )
		{
			fprintf( error_file,
			"Warning: cannot get nest height above water in %s\n",
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				observation_notepad,
				',',
				input_string,
				OBSERVATION_NOTEPAD ) )
		{
			fprintf( error_file,
			"Warning: cannot get observation notepad in %s\n",
				 input_string );
			continue;
		}

		if ( really_yn == 'y' )
		{
			output_pipe = nest_observation_insert_pipe;

			if ( materials && *materials )
			{
				fprintf(nest_update_pipe,
					"%s^materials^%s\n",
					nest_number_key,
					materials );
			}

			if ( *monitor_nest_yn )
			{
				fprintf(nest_update_pipe,
					"%s^monitor_nest_yn^%s\n",
					nest_number_key,
					monitor_nest_yn );
			}

			if ( *female_size_cm )
			{
				fprintf(nest_update_pipe,
					"%s^female_size_cm^%s\n",
					nest_number_key,
					female_size_cm );
			}

			if ( *nest_height_cm )
			{
				fprintf(nest_update_pipe,
					"%s^nest_height_cm^%s\n",
					nest_number_key,
					nest_height_cm );
			}

			if ( *total_eggs )
			{
				fprintf(nest_update_pipe,
					"%s^total_eggs^%s\n",
					nest_number_key,
					total_eggs );
			}

			if ( *banded_eggs )
			{
				fprintf(nest_update_pipe,
					"%s^banded_eggs^%s\n",
					nest_number_key,
					banded_eggs );
			}

			if ( *not_banded_eggs )
			{
				fprintf(nest_update_pipe,
					"%s^not_banded_eggs^%s\n",
					nest_number_key,
					not_banded_eggs );
			}

			if ( habitat && *habitat )
			{
				fprintf(nest_update_pipe,
					"%s^habitat^%s\n",
					nest_number_key,
					habitat );
			}

			/* Output the many water depths */
			/* ---------------------------- */
			if ( !piece_double_quoted(
					water_depth,
					',',
					input_string,
					OBSERVATION_WATER_DEPTH_1 ) )
			{
				fprintf( error_file,
			"Warning: cannot get water depth in %s\n",
				 	input_string );
				continue;
			}

			if ( *water_depth )
			{
				fprintf(water_depth_insert_pipe,
					"%s|%s|1|%s\n",
					nest_number_key,
					observation_date_international,
					water_depth );
			}

			if ( !piece_double_quoted(
					water_depth,
					',',
					input_string,
					OBSERVATION_WATER_DEPTH_2 ) )
			{
				fprintf( error_file,
			"Warning: cannot get water depth in %s\n",
				 	input_string );
				continue;
			}

			if ( *water_depth )
			{
				fprintf(water_depth_insert_pipe,
					"%s|%s|2|%s\n",
					nest_number_key,
					observation_date_international,
					water_depth );
			}

			if ( !piece_double_quoted(
					water_depth,
					',',
					input_string,
					OBSERVATION_WATER_DEPTH_3 ) )
			{
				fprintf( error_file,
			"Warning: cannot get water depth in %s\n",
				 	input_string );
				continue;
			}

			if ( *water_depth )
			{
				fprintf(water_depth_insert_pipe,
					"%s|%s|3|%s\n",
					nest_number_key,
					observation_date_international,
					water_depth );
			}

			if ( !piece_double_quoted(
					water_depth,
					',',
					input_string,
					OBSERVATION_WATER_DEPTH_4 ) )
			{
				fprintf( error_file,
			"Warning: cannot get water depth in %s\n",
				 	input_string );
				continue;
			}

			if ( *water_depth )
			{
				fprintf(water_depth_insert_pipe,
					"%s|%s|4|%s\n",
					nest_number_key,
					observation_date_international,
					water_depth );
			}
		}
		else
		{
			output_pipe = table_output_pipe;
		}

/*
#define INSERT_NEST_OBSERVATION		"nest_number,observation_date,observation_view,nest_status,female_behavior,observation_notepad,nest_visit_number,hatchlings_count,hatched_eggs,flooded_eggs,predated_eggs,crushed_eggs,other_egg_mortality,removed_viable_eggs,removed_non_viable_eggs,water_depth_average,nest_height_above_water_cm"
*/
		fprintf( output_pipe,
"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
			 nest_number_key,
			 observation_date_international,
			 observation_view,
			 nest_status,
			 female_behavior,
			 observation_notepad,
			 nest_visit_number,
			 hatchlings_count,
			 hatched_eggs,
			 flooded_eggs,
			 predated_eggs,
			 crushed_eggs,
			 other_egg_mortality,
			 removed_viable_eggs,
			 removed_non_viable_eggs,
			 water_depth_average_cm,
			 nest_height_above_water_cm );
		load_count++;
	}

	fclose( input_file );
	fclose( error_file );

	if ( really_yn == 'y' )
	{
		pclose( nest_observation_insert_pipe );
		pclose( water_depth_insert_pipe );
		pclose( nest_update_pipe );
	}
	else
	{
		pclose( table_output_pipe );
	}

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Nest Visit Load Errors' '' '|'",
			 error_filename );
		system( sys_string );
	}

	sprintf( sys_string, "rm %s", error_filename );
	system( sys_string );

	return load_count;

} /* insert_observations() */

#define DELETE_FIELD_LIST	"nest_number"

void delete_observation(	char *application_name,
				char *input_filename )
{
	FILE *input_file;
	FILE *nest_observation_delete_pipe;
	char sys_string[ 1024 ];
	char input_string[ 1024 ];
	char *table_name;
	char discovery_year[ 128 ];
	char nest_number[ 128 ];
	char *nest_number_key;
	char observation_date_american[ 128 ];
	char observation_date_international[ 128 ];

	/* Open the input file */
	/* ------------------- */
	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	/* Open the observation nest delete pipe */
	/* ------------------------------------- */
	table_name =
		get_table_name(
			application_name, "nest_visit" );

	sprintf( sys_string,
"delete_statement.e t=%s f=%s d='|' | sql.e 2>&1",
		 table_name,
		 DELETE_FIELD_LIST );

	nest_observation_delete_pipe = popen( sys_string, "w" );

	/* Skip the first line. */
	/* -------------------- */
	get_line( input_string, input_file );

	while( get_line( input_string, input_file ) )
	{
		trim( input_string );
		if ( !*input_string ) continue;

		if ( !piece_double_quoted(
				discovery_year,
				',',
				input_string,
				OBSERVATION_DISCOVERY_YEAR ) )
		{
			continue;
		}

		if ( !piece_double_quoted(
				nest_number,
				',',
				input_string,
				OBSERVATION_NEST_NUMBER ) )
		{
			continue;
		}

		nest_number_key =
			alligator_get_nest_key(
				(char *)0 /* discovery_date_international */,
				discovery_year,
				nest_number );

		if ( !piece_double_quoted(
				observation_date_american,
				',',
				input_string,
				OBSERVATION_DATE ) )
		{
			continue;
		}

		*observation_date_international = '\0';

		date_convert_source_american(
				observation_date_international,
				international,
				observation_date_american );

		if ( !*observation_date_international )
		{
			continue;
		}

		fprintf(nest_observation_delete_pipe,
		 	"%s|%s\n",
		 	nest_number_key,
		 	observation_date_international );
	}

	fclose( input_file );
	pclose( nest_observation_delete_pipe );

} /* delete_observation() */

