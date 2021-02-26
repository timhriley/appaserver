/* ---------------------------------------------------	*/
/* src_wadingbird/load_tablet_file.c			*/
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
#include "column.h"
#include "date.h"
#include "wadingbird_library.h"
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
void populate_hydropattern_from_file(
				char *application_name,
				LIST *transect_list,
				char *input_filename,
				FILE *error_file );

void populate_hydropattern_fill_in_blanks(
				LIST *transect_list );

char *get_left_right(		char *left_right_code,
				char *previous_left_right );

void delete_hydropattern(	char *application_name,
				char *input_filename );

void insert_each_species(	int *observation_count,
				FILE *bird_observation_output_pipe,
				char *initial_takeoff_date_international,
		     		char *utm_easting,
				char *utm_northing,
				char *left_right,
				char *cell_number,
				char *application_name,
				char *input_string,
				LIST *transects_with_one_observer_list );

int insert_bird_observations(	int *record_count,
				FILE *error_file,
				char *application_name,
				char *input_filename,
				char *utm_filename,
				char really_yn,
				char *transects_with_one_observer_list_string,
				char *preferred_hydropattern_seat,
				char non_conventional_flight_yn );

int insert_cell_hydropattern(	char *application_name,
				char *input_filename,
				char really_yn,
				FILE *error_file );

void build_utm_file(		char *utm_filename,
				char *input_filename );

void delete_each_species(
				FILE *bird_observation_delete_pipe,
				char *initial_takeoff_date_international,
		     		char *utm_easting,
				char *utm_northing,
				char *left_right,
				char *application_name,
				char *input_string );

void delete_bird_observation(
				char *application_name,
				char *input_filename,
				char *utm_filename );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *input_filename;
	char *transects_with_one_observer_list_string;
	char buffer[ 128 ];
	char non_conventional_flight_yn;
	char really_yn;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	int record_count = 0;
	int observation_count = 0;
	char utm_filename[ 128 ];
	char sys_string[ 1024 ];
	char *preferred_hydropattern_seat;
	char error_filename[ 128 ];
	FILE *error_file;
	int process_id = getpid();
	int hydropattern_count;
	int results;

	if ( argc != 8 )
	{
		fprintf( stderr, 
"Usage: %s application process filename preferred_hydropattern_seat transects_with_one_observer non_conventional_flight_yn really_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	input_filename = argv[ 3 ];
	preferred_hydropattern_seat = argv[ 4 ];
	transects_with_one_observer_list_string = argv[ 5 ];
	non_conventional_flight_yn = *argv[ 6 ];
	really_yn = *argv[ 7 ];

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
	add_local_bin_to_path();
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
	results = system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( !*preferred_hydropattern_seat
	||   strcmp( preferred_hydropattern_seat,
		     "preferred_hydropattern_seat" ) == 0 )
	{
		printf(
		"<h3>Please select a preferred hydropattern seat.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !*input_filename || strcmp( input_filename, "filename" ) == 0 )
	{
		printf( "<h3>Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	sprintf( utm_filename, "/tmp/append_utm_%d.dat", process_id );
	build_utm_file( utm_filename, input_filename );

	sprintf( error_filename,
		 "/tmp/wadingbird_load_tablet_error_%d.txt",
		 process_id );
	if ( ! ( error_file = fopen( error_filename, "w" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		exit( 1 );
	}

	if ( really_yn == 'y' )
	{
		delete_bird_observation(
			application_name,
			input_filename,
			utm_filename );

		delete_hydropattern(
			application_name,
			input_filename );
	}

	observation_count = insert_bird_observations(
				&record_count,
				error_file,
				application_name,
				input_filename,
				utm_filename,
				really_yn,
				transects_with_one_observer_list_string,
				preferred_hydropattern_seat,
				non_conventional_flight_yn );

	hydropattern_count = insert_cell_hydropattern(
				application_name,
				input_filename,
				really_yn,
				error_file );

	if ( really_yn == 'y' )
	{
		printf(
	"<p>Process complete with %d observations from %d input records.Hydropattern count = %d.\n",
			observation_count,
			record_count,
			hydropattern_count );
	}
	else
	{
		printf(
	"<p>Process not executed with %d observations from %d input records. Hydropattern count = %d.\n",
			 observation_count,
			 record_count,
			 hydropattern_count );
	}

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );

	sprintf( sys_string, "rm -f %s", utm_filename );
	results = system( sys_string );

	fclose( error_file );
	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Load Errors' '' '|'",
			 error_filename );
		results = system( sys_string );
	}
	sprintf( sys_string, "rm %s", error_filename );
	results = system( sys_string );

	exit( 0 );
} /* main() */

#define BIRD_DELETE_FIELD_LIST	"initial_takeoff_date,utm_easting,utm_northing,common_name,left_right"

void delete_bird_observation(	char *application_name,
				char *input_filename,
				char *utm_filename )
{
	FILE *input_pipe;
	FILE *utm_input_file;
	FILE *bird_observation_delete_pipe;
	char sys_string[ 1024 ];
	char input_string[ 1024 ];
	char utm_input_string[ 256 ];
	char initial_takeoff_date_american[ 128 ] = {0};
	char *initial_takeoff_date_international = {0};
	char utm_easting[ 128 ];
	char utm_northing[ 128 ];
	char left_right_code[ 16 ];
	char *left_right = {0};
	char *table_name;

	if ( ! ( utm_input_file = fopen( utm_filename, "r" ) ) )
	{
		fprintf(stderr,
			"Error in %s/%s()/%d: cannot open %s for read.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			utm_filename );
		exit( 1 );
	}

	input_pipe = wadingbird_get_tablet_input_pipe( input_filename );

	table_name =
		get_table_name(
			application_name, "bird_observation" );

	sprintf( sys_string,
"delete_statement.e t=%s f=%s d='|' | sql.e 2>&1 | html_paragraph_wrapper.e",
		 table_name,
		 BIRD_DELETE_FIELD_LIST );

	bird_observation_delete_pipe = popen( sys_string, "w" );

	while( get_line( input_string, input_pipe ) )
	{
		get_line( utm_input_string, utm_input_file );
		column( utm_easting, 0, utm_input_string );
		column( utm_northing, 1, utm_input_string );

		trim_character( input_string, '"', input_string );
		trim_character( input_string, '\'', input_string );

		if ( !piece(	left_right_code,
				',',
				input_string,
				TABLET_LEFT_RIGHT_CODE_PIECE ) )
		{
			continue;
		}

		if ( ! ( left_right = get_left_right(
					left_right_code,
					left_right ) ) )
		{
			continue;
		}

		if ( !*initial_takeoff_date_american )
		{
			if ( !piece(	initial_takeoff_date_american,
					',',
					input_string,
					TABLET_INITIAL_TAKEOFF_DATE_PIECE ) )
			{
				continue;
			}
	
			initial_takeoff_date_international =
			     wadingbird_get_initial_takeoff_date_international(
					initial_takeoff_date_american );
			continue;
		}

		delete_each_species(
			bird_observation_delete_pipe,
			initial_takeoff_date_international,
		     	utm_easting,
			utm_northing,
			left_right,
			application_name,
			input_string );
	}

	pclose( input_pipe );
	pclose( bird_observation_delete_pipe );
	fclose( utm_input_file );

} /* delete_bird_observation() */

#define HYDROPATTERN_DELETE_FIELD_LIST	"initial_takeoff_date,cell_number,left_right"
#define PREFERRED_HYDROPATTERN_DELETE_FIELD_LIST "initial_takeoff_date"

void delete_hydropattern(	char *application_name,
				char *input_filename )
{
	FILE *cell_hydropattern_delete_pipe;
	FILE *preferred_hydropattern_delete_pipe;
	char sys_string[ 1024 ];
	char *table_name;
	SRF_CENSUS *srf_census;
	TRANSECT *transect;
	SRF_CELL *cell;

	/* Cell Hydropattern */
	/* ----------------- */
	table_name =
		get_table_name(
			application_name, "cell_hydropattern" );

	sprintf( sys_string,
"delete_statement.e t=%s f=%s d='|' | sql.e 2>&1 | html_paragraph_wrapper.e",
		 table_name,
		 HYDROPATTERN_DELETE_FIELD_LIST );

	cell_hydropattern_delete_pipe = popen( sys_string, "w" );

	/* Preferred Hydropattern */
	/* ---------------------- */
	table_name =
		get_table_name(
			application_name, "preferred_hydropattern" );

	sprintf( sys_string,
"delete_statement.e t=%s f=%s d='|' | sql.e 2>&1 | html_paragraph_wrapper.e",
		 table_name,
		PREFERRED_HYDROPATTERN_DELETE_FIELD_LIST );

	preferred_hydropattern_delete_pipe = popen( sys_string, "w" );

	srf_census = wadingbird_srf_census_new(
				application_name,
				input_filename );

	fprintf(preferred_hydropattern_delete_pipe,
		"%s\n",
		srf_census->initial_takeoff_date );

	if ( !list_rewind( srf_census->transect_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty transect_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		transect = list_get_pointer( srf_census->transect_list );

		if ( list_rewind( transect->left_cell_list ) )
		{
			do {
				cell = list_get_pointer(
						transect->left_cell_list );

				fprintf(cell_hydropattern_delete_pipe,
					"%s|%d|left\n",
					srf_census->
						initial_takeoff_date,
		     			cell->cell_number );

			} while( list_next( transect->left_cell_list ) );
		}

		if ( list_rewind( transect->right_cell_list ) )
		{
			do {
				cell = list_get_pointer(
						transect->right_cell_list );

				fprintf(cell_hydropattern_delete_pipe,
					"%s|%d|right\n",
					srf_census->
						initial_takeoff_date,
		     			cell->cell_number );

			} while( list_next( transect->right_cell_list ) );
		}

	} while( list_next( srf_census->transect_list ) );

	pclose( cell_hydropattern_delete_pipe );
	pclose( preferred_hydropattern_delete_pipe );

} /* delete_hydropattern() */

void delete_each_species(
			FILE *bird_observation_delete_pipe,
			char *initial_takeoff_date_international,
		     	char *utm_easting,
			char *utm_northing,
			char *left_right,
			char *application_name,
			char *input_string )
{
	char count[ 16 ];
	char *common_name;

	/* Great Egret */
	/* ----------- */
	if ( !piece(	count,
			',',
			input_string,
			TABLET_GREAT_EGRET_COUNT_PIECE ) )
	{
		return;
	}

	if ( atoi( count ) )
	{
		wadingbird_get_species(
				&common_name,
				application_name,
				GREAT_EGRET_SPECIES_CODE );

		fprintf(bird_observation_delete_pipe,
			"%s|%s|%s|%s|%s\n",
			initial_takeoff_date_international,
		     	utm_easting,
			utm_northing,
			common_name,
			left_right );
	}

	/* White Ibis */
	/* ----------- */
	if ( !piece(	count,
			',',
			input_string,
			TABLET_WHITE_IBIS_COUNT_PIECE ) )
	{
		return;
	}

	if ( atoi( count ) )
	{
		wadingbird_get_species(
				&common_name,
				application_name,
				WHITE_IBIS_SPECIES_CODE );

		fprintf(bird_observation_delete_pipe,
			"%s|%s|%s|%s|%s\n",
			initial_takeoff_date_international,
		     	utm_easting,
			utm_northing,
			common_name,
			left_right );
	}


	/* Great Blue Heron */
	/* ---------------- */
	if ( !piece(	count,
			',',
			input_string,
			TABLET_GREAT_BLUE_HERON_COUNT_PIECE ) )
	{
		return;
	}

	if ( atoi( count ) )
	{
		wadingbird_get_species(
				&common_name,
				application_name,
				GREAT_BLUE_HERON_SPECIES_CODE );

		fprintf(bird_observation_delete_pipe,
			"%s|%s|%s|%s|%s\n",
			initial_takeoff_date_international,
		     	utm_easting,
			utm_northing,
			common_name,
			left_right );
	}

	/* Small White Heron */
	/* ----------------- */
	if ( !piece(	count,
			',',
			input_string,
			TABLET_SMALL_WHITE_HERON_COUNT_PIECE ) )
	{
		return;
	}

	if ( atoi( count ) )
	{
		wadingbird_get_species(
				&common_name,
				application_name,
				SMALL_WHITE_HERON_SPECIES_CODE );

		fprintf(bird_observation_delete_pipe,
			"%s|%s|%s|%s|%s\n",
			initial_takeoff_date_international,
		     	utm_easting,
			utm_northing,
			common_name,
			left_right );
	}


	/* Small Dark Heron */
	/* ---------------- */
	if ( !piece(	count,
			',',
			input_string,
			TABLET_SMALL_DARK_HERON_COUNT_PIECE ) )
	{
		return;
	}

	if ( atoi( count ) )
	{
		wadingbird_get_species(
				&common_name,
				application_name,
				SMALL_DARK_HERON_SPECIES_CODE );

		fprintf(bird_observation_delete_pipe,
			"%s|%s|%s|%s|%s\n",
			initial_takeoff_date_international,
		     	utm_easting,
			utm_northing,
			common_name,
			left_right );
	}


	/* Wood Stork */
	/* ---------- */
	if ( !piece(	count,
			',',
			input_string,
			TABLET_WOOD_STORK_COUNT_PIECE ) )
	{
		return;
	}

	if ( atoi( count ) )
	{
		wadingbird_get_species(
				&common_name,
				application_name,
				WOOD_STORK_SPECIES_CODE );

		fprintf(bird_observation_delete_pipe,
			"%s|%s|%s|%s|%s\n",
			initial_takeoff_date_international,
		     	utm_easting,
			utm_northing,
			common_name,
			left_right );
	}


	/* Roseate Spoonbill */
	/* ----------------- */
	if ( !piece(	count,
			',',
			input_string,
			TABLET_ROSEATE_SPOONBILL_COUNT_PIECE ) )
	{
		return;
	}

	if ( atoi( count ) )
	{
		wadingbird_get_species(
				&common_name,
				application_name,
				ROSEATE_SPOONBILL_SPECIES_CODE );

		fprintf(bird_observation_delete_pipe,
			"%s|%s|%s|%s|%s\n",
			initial_takeoff_date_international,
		     	utm_easting,
			utm_northing,
			common_name,
			left_right );
	}


	/* Glossy Ibis */
	/* ----------- */
	if ( !piece(	count,
			',',
			input_string,
			TABLET_GLOSSY_IBIS_COUNT_PIECE ) )
	{
		return;
	}

	if ( atoi( count ) )
	{
		wadingbird_get_species(
				&common_name,
				application_name,
				GLOSSY_IBIS_SPECIES_CODE );

		fprintf(bird_observation_delete_pipe,
			"%s|%s|%s|%s|%s\n",
			initial_takeoff_date_international,
		     	utm_easting,
			utm_northing,
			common_name,
			left_right );
	}

	/* Great White Heron */
	/* ----------------- */
	if ( !piece(	count,
			',',
			input_string,
			TABLET_GREAT_WHITE_HERON_COUNT_PIECE ) )
	{
		return;
	}

	if ( atoi( count ) )
	{
		wadingbird_get_species(
				&common_name,
				application_name,
				GREAT_WHITE_HERON_SPECIES_CODE );

		fprintf(bird_observation_delete_pipe,
			"%s|%s|%s|%s|%s\n",
			initial_takeoff_date_international,
		     	utm_easting,
			utm_northing,
			common_name,
			left_right );
	}

} /* delete_each_species() */

int insert_bird_observations(	int *record_count,
				FILE *error_file,
				char *application_name,
				char *input_filename,
				char *utm_filename,
				char really_yn,
				char *transects_with_one_observer_list_string,
				char *preferred_hydropattern_seat,
				char non_conventional_flight_yn )
{
	FILE *srf_output_pipe = {0};
	FILE *bird_observation_output_pipe = {0};
	FILE *input_pipe;
	FILE *utm_input_file;
	char sys_string[ 1024 ];
	char input_string[ 1024 ];
	char utm_input_string[ 256 ];
	char initial_takeoff_date_american[ 128 ] = {0};
	char *initial_takeoff_date_international = {0};
	char utm_easting[ 128 ];
	char utm_northing[ 128 ];
	char left_right_code[ 16 ];
	char *left_right = {0};
	char cell_number[ 16 ];
	int line_number = 0;
	int observation_count = 0;
	LIST *transects_with_one_observer_list;

	transects_with_one_observer_list =
		list_string_to_list(
			transects_with_one_observer_list_string,
			',' );

	if ( ! ( utm_input_file = fopen( utm_filename, "r" ) ) )
	{
		fprintf(stderr,
			"Error in %s/%s()/%d: cannot open %s for read.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			utm_filename );
		exit( 1 );
	}

	input_pipe = wadingbird_get_tablet_input_pipe( input_filename );

	if ( really_yn == 'y' )
	{
		char *table_name;

		/* Systematic Reconnaissance Flight */
		/* -------------------------------- */
		table_name =
			get_table_name(
				application_name,
				"systematic_reconnaissance_flight" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
			 table_name,
			 SRF_FIELD_LIST );
		srf_output_pipe = popen( sys_string, "w" );

		/* Bird Observation */
		/* ---------------- */
		table_name =
			get_table_name( application_name, "bird_observation" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | html_paragraph_wrapper.e",
			 table_name,
			 BIRD_OBSERVATION_FIELD_LIST );
		bird_observation_output_pipe = popen( sys_string, "w" );
	}
	else
	{
		sprintf( sys_string,
"queue_top_bottom_lines.e 50 | html_table.e 'Insert into Bird Observation' %s '|' %s",
			 BIRD_OBSERVATION_FIELD_LIST,
			 BIRD_OBSERVATION_JUSTIFY_COMMA_LIST );
		bird_observation_output_pipe = popen( sys_string, "w" );
	}

	while( get_line( input_string, input_pipe ) )
	{
		get_line( utm_input_string, utm_input_file );
		column( utm_easting, 0, utm_input_string );
		column( utm_northing, 1, utm_input_string );

		line_number++;
		trim_character( input_string, '"', input_string );
		trim_character( input_string, '\'', input_string );

		if ( !*initial_takeoff_date_american )
		{
			if ( !piece(	initial_takeoff_date_american,
					',',
					input_string,
					TABLET_INITIAL_TAKEOFF_DATE_PIECE ) )
			{
				fprintf(error_file,
			"ERROR: cannot piece(%d) the takeoff date in (%s).\n",
					TABLET_INITIAL_TAKEOFF_DATE_PIECE,
					input_string );
				continue;
			}
	
			initial_takeoff_date_international =
			     wadingbird_get_initial_takeoff_date_international(
					initial_takeoff_date_american );

			if ( srf_output_pipe )
			{
				fprintf(srf_output_pipe,
					"%s|%c|%s\n",
					initial_takeoff_date_international,
					non_conventional_flight_yn,
					preferred_hydropattern_seat );
			}
			(*record_count)++;

			/* If the left/right is in the date row */
			/* ------------------------------------ */
			if ( !piece(	left_right_code,
					',',
					input_string,
					TABLET_LEFT_RIGHT_CODE_PIECE ) )
			{
				fprintf(error_file,
			"ERROR: cannot piece(%d) left/right in (%s).\n",
						TABLET_LEFT_RIGHT_CODE_PIECE,
						input_string );
				continue;
			}

			if ( ! ( left_right = get_left_right(
						left_right_code,
						left_right ) ) )
			{
				fprintf(error_file,
			"ERROR: invalid left/right code of (%s) in (%s).\n",
						left_right_code,
						input_string );
				continue;
			}

			continue;
		}

		(*record_count)++;

		if ( !piece(	left_right_code,
				',',
				input_string,
				TABLET_LEFT_RIGHT_CODE_PIECE ) )
		{
			fprintf(error_file,
			"ERROR: cannot piece(%d) left/right in (%s).\n",
					TABLET_LEFT_RIGHT_CODE_PIECE,
					input_string );
			continue;
		}

		if ( ! ( left_right = get_left_right(
					left_right_code,
					left_right ) ) )
		{
			fprintf(error_file,
			"ERROR: invalid left/right code of (%s) in (%s).\n",
					left_right_code,
					input_string );
			continue;
		}

		if ( !piece(	cell_number,
				',',
				input_string,
				TABLET_CELL_NUMBER_PIECE ) )
		{
			fprintf(error_file,
			"ERROR: cannot piece(%d) cell number in (%s).\n",
					TABLET_CELL_NUMBER_PIECE,
					input_string );
			continue;
		}

		if ( !wadingbird_given_cell_number_get_transect_number(
					application_name,
					atoi( cell_number ) ) )
		{
			fprintf(error_file,
			"Warning: invalid cell number of (%s) in (%s).\n",
					cell_number,
					input_string );
			continue;
		}

		insert_each_species(
			&observation_count,
			bird_observation_output_pipe,
			initial_takeoff_date_international,
		     	utm_easting,
			utm_northing,
			left_right,
			cell_number,
			application_name,
			input_string,
			transects_with_one_observer_list );
	}

	pclose( input_pipe );
	pclose( bird_observation_output_pipe );
	fclose( utm_input_file );

	if ( srf_output_pipe )
	{
		pclose( srf_output_pipe );
	}

	return observation_count;

} /* insert_bird_observations() */

void build_utm_file( char *utm_filename, char *input_filename )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	FILE *output_pipe;
	char input_string[ 1024 ];
	char latitude_decimal[ 128 ];
	char longitude_decimal[ 128 ];

	input_pipe = wadingbird_get_tablet_input_pipe( input_filename );

	sprintf( sys_string,
		 "cs2cs_lonlat.sh > %s",
		 utm_filename );

	output_pipe = popen( sys_string, "w" );

	while( get_line( input_string, input_pipe ) )
	{
		trim_character( input_string, '"', input_string );
		trim_character( input_string, '\'', input_string );

		if ( !piece(	latitude_decimal,
				',',
				input_string,
				TABLET_LATITUDE_PIECE ) )
		{
			fprintf( output_pipe, "\n" );
			continue;
		}

		if ( !piece(	longitude_decimal,
				',',
				input_string,
				TABLET_LONGITUDE_PIECE ) )
		{
			fprintf( output_pipe, "\n" );
			continue;
		}

		fprintf(	output_pipe,
				"%s %s\n",
				longitude_decimal,
				latitude_decimal );
	}

	pclose( output_pipe );
	pclose( input_pipe );

} /* build_utm_file() */

void insert_each_species(
			int *observation_count,
			FILE *bird_observation_output_pipe,
			char *initial_takeoff_date_international,
		     	char *utm_easting,
			char *utm_northing,
			char *left_right,
			char *cell_number,
			char *application_name,
			char *input_string,
			LIST *transects_with_one_observer_list )
{
	char bird_count_string[ 16 ];
	int bird_count;
	int bird_estimation;
	char *common_name;

	/* Great Egret */
	/* ----------- */
	if ( !piece(	bird_count_string,
			',',
			input_string,
			TABLET_GREAT_EGRET_COUNT_PIECE ) )
	{
		return;
	}

	bird_count = atoi( bird_count_string );
	if ( bird_count )
	{
		bird_estimation =
			wadingbird_get_bird_estimation(
				application_name,
				atoi( cell_number ),
				bird_count,
				transects_with_one_observer_list );

		wadingbird_get_species(
				&common_name,
				application_name,
				GREAT_EGRET_SPECIES_CODE );

		fprintf(bird_observation_output_pipe,
			"%s|%s|%s|%s|%s|%d|%d|%s\n",
			initial_takeoff_date_international,
		     	utm_easting,
			utm_northing,
			common_name,
			left_right,
			bird_count,
			bird_estimation,
			cell_number );
		(*observation_count)++;
	}

	/* White Ibis */
	/* ----------- */
	if ( !piece(	bird_count_string,
			',',
			input_string,
			TABLET_WHITE_IBIS_COUNT_PIECE ) )
	{
		return;
	}

	bird_count = atoi( bird_count_string );
	if ( bird_count )
	{
		bird_estimation =
			wadingbird_get_bird_estimation(
				application_name,
				atoi( cell_number ),
				bird_count,
				transects_with_one_observer_list );

		wadingbird_get_species(
				&common_name,
				application_name,
				WHITE_IBIS_SPECIES_CODE );

		fprintf(bird_observation_output_pipe,
			"%s|%s|%s|%s|%s|%d|%d|%s\n",
			initial_takeoff_date_international,
		     	utm_easting,
			utm_northing,
			common_name,
			left_right,
			bird_count,
			bird_estimation,
			cell_number );
		(*observation_count)++;
	}


	/* Great Blue Heron */
	/* ---------------- */
	if ( !piece(	bird_count_string,
			',',
			input_string,
			TABLET_GREAT_BLUE_HERON_COUNT_PIECE ) )
	{
		return;
	}

	bird_count = atoi( bird_count_string );
	if ( bird_count )
	{
		bird_estimation =
			wadingbird_get_bird_estimation(
				application_name,
				atoi( cell_number ),
				bird_count,
				transects_with_one_observer_list );

		wadingbird_get_species(
				&common_name,
				application_name,
				GREAT_BLUE_HERON_SPECIES_CODE );

		fprintf(bird_observation_output_pipe,
			"%s|%s|%s|%s|%s|%d|%d|%s\n",
			initial_takeoff_date_international,
		     	utm_easting,
			utm_northing,
			common_name,
			left_right,
			bird_count,
			bird_estimation,
			cell_number );
		(*observation_count)++;
	}

	/* Small White Heron */
	/* ----------------- */
	if ( !piece(	bird_count_string,
			',',
			input_string,
			TABLET_SMALL_WHITE_HERON_COUNT_PIECE ) )
	{
		return;
	}

	bird_count = atoi( bird_count_string );
	if ( bird_count )
	{
		bird_estimation =
			wadingbird_get_bird_estimation(
				application_name,
				atoi( cell_number ),
				bird_count,
				transects_with_one_observer_list );

		wadingbird_get_species(
				&common_name,
				application_name,
				SMALL_WHITE_HERON_SPECIES_CODE );

		fprintf(bird_observation_output_pipe,
			"%s|%s|%s|%s|%s|%d|%d|%s\n",
			initial_takeoff_date_international,
		     	utm_easting,
			utm_northing,
			common_name,
			left_right,
			bird_count,
			bird_estimation,
			cell_number );
		(*observation_count)++;
	}


	/* Small Dark Heron */
	/* ---------------- */
	if ( !piece(	bird_count_string,
			',',
			input_string,
			TABLET_SMALL_DARK_HERON_COUNT_PIECE ) )
	{
		return;
	}

	bird_count = atoi( bird_count_string );
	if ( bird_count )
	{
		bird_estimation =
			wadingbird_get_bird_estimation(
				application_name,
				atoi( cell_number ),
				bird_count,
				transects_with_one_observer_list );

		wadingbird_get_species(
				&common_name,
				application_name,
				SMALL_DARK_HERON_SPECIES_CODE );

		fprintf(bird_observation_output_pipe,
			"%s|%s|%s|%s|%s|%d|%d|%s\n",
			initial_takeoff_date_international,
		     	utm_easting,
			utm_northing,
			common_name,
			left_right,
			bird_count,
			bird_estimation,
			cell_number );
		(*observation_count)++;
	}


	/* Wood Stork */
	/* ---------- */
	if ( !piece(	bird_count_string,
			',',
			input_string,
			TABLET_WOOD_STORK_COUNT_PIECE ) )
	{
		return;
	}

	bird_count = atoi( bird_count_string );
	if ( bird_count )
	{
		bird_estimation =
			wadingbird_get_bird_estimation(
				application_name,
				atoi( cell_number ),
				bird_count,
				transects_with_one_observer_list );

		wadingbird_get_species(
				&common_name,
				application_name,
				WOOD_STORK_SPECIES_CODE );

		fprintf(bird_observation_output_pipe,
			"%s|%s|%s|%s|%s|%d|%d|%s\n",
			initial_takeoff_date_international,
		     	utm_easting,
			utm_northing,
			common_name,
			left_right,
			bird_count,
			bird_estimation,
			cell_number );
		(*observation_count)++;
	}


	/* Roseate Spoonbill */
	/* ----------------- */
	if ( !piece(	bird_count_string,
			',',
			input_string,
			TABLET_ROSEATE_SPOONBILL_COUNT_PIECE ) )
	{
		return;
	}

	bird_count = atoi( bird_count_string );
	if ( bird_count )
	{
		bird_estimation =
			wadingbird_get_bird_estimation(
				application_name,
				atoi( cell_number ),
				bird_count,
				transects_with_one_observer_list );

		wadingbird_get_species(
				&common_name,
				application_name,
				ROSEATE_SPOONBILL_SPECIES_CODE );

		fprintf(bird_observation_output_pipe,
			"%s|%s|%s|%s|%s|%d|%d|%s\n",
			initial_takeoff_date_international,
		     	utm_easting,
			utm_northing,
			common_name,
			left_right,
			bird_count,
			bird_estimation,
			cell_number );
		(*observation_count)++;
	}


	/* Glossy Ibis */
	/* ----------- */
	if ( !piece(	bird_count_string,
			',',
			input_string,
			TABLET_GLOSSY_IBIS_COUNT_PIECE ) )
	{
		return;
	}

	bird_count = atoi( bird_count_string );
	if ( bird_count )
	{
		bird_estimation =
			wadingbird_get_bird_estimation(
				application_name,
				atoi( cell_number ),
				bird_count,
				transects_with_one_observer_list );

		wadingbird_get_species(
				&common_name,
				application_name,
				GLOSSY_IBIS_SPECIES_CODE );

		fprintf(bird_observation_output_pipe,
			"%s|%s|%s|%s|%s|%d|%d|%s\n",
			initial_takeoff_date_international,
		     	utm_easting,
			utm_northing,
			common_name,
			left_right,
			bird_count,
			bird_estimation,
			cell_number );
		(*observation_count)++;
	}

	/* Great White Heron */
	/* ----------------- */
	if ( !piece(	bird_count_string,
			',',
			input_string,
			TABLET_GREAT_WHITE_HERON_COUNT_PIECE ) )
	{
		return;
	}

	bird_count = atoi( bird_count_string );
	if ( bird_count )
	{
		bird_estimation =
			wadingbird_get_bird_estimation(
				application_name,
				atoi( cell_number ),
				bird_count,
				transects_with_one_observer_list );

		wadingbird_get_species(
				&common_name,
				application_name,
				GREAT_WHITE_HERON_SPECIES_CODE );

		fprintf(bird_observation_output_pipe,
			"%s|%s|%s|%s|%s|%d|%d|%s\n",
			initial_takeoff_date_international,
		     	utm_easting,
			utm_northing,
			common_name,
			left_right,
			bird_count,
			bird_estimation,
			cell_number );
		(*observation_count)++;
	}

} /* insert_each_species() */

int insert_cell_hydropattern(	char *application_name,
				char *input_filename,
				char really_yn,
				FILE *error_file )
{
	FILE *cell_hydropattern_output_pipe;
	char sys_string[ 1024 ];
	SRF_CENSUS *srf_census;
	TRANSECT *transect;
	SRF_CELL *cell;
	int hydropattern_count = 0;

	if ( really_yn == 'y' )
	{
		char *table_name;

		/* Cell Hydropattern */
		/* ----------------- */
		table_name =
			get_table_name( application_name, "cell_hydropattern" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e | html_paragraph_wrapper.e",
			 table_name,
			 CELL_HYDROPATTERN_FIELD_LIST );
		cell_hydropattern_output_pipe = popen( sys_string, "w" );
	}
	else
	{
/*
		sprintf( sys_string,
"queue_top_bottom_lines.e 50 | html_table.e 'Insert into Cell Hydropattern' %s '|' %s",
*/
		sprintf( sys_string,
"html_table.e 'Insert into Cell Hydropattern' %s '|' %s",
			 CELL_HYDROPATTERN_FIELD_LIST,
			 CELL_HYDROPATTERN_JUSTIFY_COMMA_LIST );
		cell_hydropattern_output_pipe = popen( sys_string, "w" );
	}

	srf_census = wadingbird_srf_census_new(
				application_name,
				input_filename );

	populate_hydropattern_from_file(
				application_name,
				srf_census->transect_list,
				input_filename,
				error_file );

	populate_hydropattern_fill_in_blanks(
				srf_census->transect_list );

	if ( !list_rewind( srf_census->transect_list ) )
	{
		printf( "<h3>Error: bad input file. It seems like there is no starting transect number.</h3>\n" );
		document_close();
		exit( 0 );
	}

	do {
		transect = list_get_pointer( srf_census->transect_list );

		if ( list_length( transect->left_cell_list )
		&&   list_length( transect->right_cell_list )
		&& ( list_length( transect->left_cell_list ) !=
		     list_length( transect->right_cell_list ) ) )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: left and right cell list lengths are different. Left=%d, Right=%d\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				list_length( transect->left_cell_list ),
				list_length( transect->right_cell_list ) );
			exit( 1 );
		}

		list_rewind( transect->left_cell_list );
		list_rewind( transect->right_cell_list );

		while( 1 )
		{
			if ( list_length( transect->left_cell_list ) )
			{
				cell = list_get_pointer(
						transect->left_cell_list );
	
				fprintf(cell_hydropattern_output_pipe,
					"%s|%d|left|%s\n",
					srf_census->
						initial_takeoff_date,
			     		cell->cell_number,
					(cell->hydropattern)
						? cell->hydropattern
						: "" );

				hydropattern_count++;
			}
	
			if ( list_length( transect->right_cell_list ) )
			{
				cell = list_get_pointer(
						transect->right_cell_list );
	
				fprintf(cell_hydropattern_output_pipe,
					"%s|%d|right|%s\n",
					srf_census->
						initial_takeoff_date,
			     		cell->cell_number,
					(cell->hydropattern)
						? cell->hydropattern
						: "" );

				hydropattern_count++;
			}
	
			if ( list_length( transect->left_cell_list ) )
			{
				if ( !list_next( transect->left_cell_list ) )
					break;
			}

			if ( list_length( transect->right_cell_list ) )
			{
				if ( !list_next( transect->right_cell_list ) )
					break;
			}
		}

	} while( list_next( srf_census->transect_list ) );

	pclose( cell_hydropattern_output_pipe );

	return hydropattern_count;

} /* insert_cell_hydropattern() */

void populate_hydropattern_from_file(
				char *application_name,
				LIST *transect_list,
				char *input_filename,
				FILE *error_file )
{
	FILE *input_pipe;
	char input_string[ 1024 ];
	char left_right_code[ 16 ];
	char *left_right = {0};
	char cell_number[ 16 ];
	char hydropattern_numeric_code[ 16 ];
	char *hydropattern;
	SRF_CELL *cell;

	input_pipe = wadingbird_get_tablet_input_pipe( input_filename );

	while( get_line( input_string, input_pipe ) )
	{
		trim_character( input_string, '"', input_string );
		trim_character( input_string, '\'', input_string );

		if ( !piece(	left_right_code,
				',',
				input_string,
				TABLET_LEFT_RIGHT_CODE_PIECE ) )
		{
			continue;
		}

		if ( ! ( left_right = get_left_right(
					left_right_code,
					left_right ) ) )
			continue;

		if ( !piece(	cell_number,
				',',
				input_string,
				TABLET_CELL_NUMBER_PIECE ) )
		{
			continue;
		}

		if ( ! ( cell = wadingbird_get_cell(
					transect_list,
					atoi( cell_number ),
					left_right ) ) )
		{
			fprintf(error_file,
			"ERROR: cannot get cell %s in (%s).\n",
					cell_number,
					input_string );
			continue;
		}

		if ( !piece(	hydropattern_numeric_code,
				',',
				input_string,
				TABLET_HYDROPATTERN_PIECE ) )
		{
			continue;
		}

		if ( *hydropattern_numeric_code )
		{
			if ( ! wadingbird_get_hydropattern(
					&hydropattern,
					hydropattern_numeric_code,
					application_name ) )
			{
				fprintf(error_file,
	"ERROR: cannot get hydropattern for code = (%s) in (%s).\n",
					hydropattern_numeric_code,
					input_string );
			}
	
			if ( cell->hydropattern
			&&   strcmp( cell->hydropattern, hydropattern ) != 0 )
			{
				fprintf(error_file,
	"Warning: hydropattern for cell %d is already set to %s. Line (%s) is trying to set this hydropattern again to %s.\n",
					cell->cell_number,
					cell->hydropattern,
					input_string,
					hydropattern );
			}

			cell->hydropattern = strdup( hydropattern );
		}
	}
	pclose( input_pipe );

} /* populate_hydropattern_from_file() */


void populate_hydropattern_fill_in_blanks(
				LIST *transect_list )
{
	char *left_hydropattern = {0};
	char *right_hydropattern = {0};
	TRANSECT *transect;
	SRF_CELL *cell;

	if ( !list_rewind( transect_list ) ) return;

	do {
		transect = list_get_pointer( transect_list );

		if ( list_rewind( transect->left_cell_list ) )
		{
			do {
				cell = list_get_pointer(
						transect->left_cell_list );
				if ( cell->hydropattern )
				{
					left_hydropattern =
						cell->hydropattern;
				}
				else
				{
					cell->hydropattern = left_hydropattern;
				}
			} while( list_next( transect->left_cell_list ) );
		}

		if ( list_rewind( transect->right_cell_list ) )
		{
			do {
				cell = list_get_pointer(
						transect->right_cell_list );
				if ( cell->hydropattern )
				{
					right_hydropattern =
						cell->hydropattern;
				}
				else
				{
					cell->hydropattern = right_hydropattern;
				}
			} while( list_next( transect->right_cell_list ) );
		}

	} while( list_next( transect_list ) );

} /* populate_hydropattern_fill_in_blanks() */


char *get_left_right( char *left_right_code, char *previous_left_right )
{
	char *left_right;

	if ( *left_right_code == 'L' )
		left_right = "left";
	else
	if ( *left_right_code == 'R' )
		left_right = "right";
	else
		left_right = previous_left_right;

	return left_right;
} /* get_left_right() */

