/* ---------------------------------------------------	*/
/* load_srf_spreadsheet.c				*/
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

/* Constants */
/* --------- */
#define AUDIT_REPORT_TRANSECT_COLUMNS		"Nest,UTMNorthing,Discovery,TransectReceived,TransectShouldBe,Distance(Meters)"
#define AUDIT_REPORT_TRANSECT_BOUNDARY_COLUMNS	"Nest,UTMNorthing,UTMEasting,Discovery,TransectReceived,TransectShouldBe,FlagReceived,FlagShouldBe,Distance(Meters)"
#define TRANSECT_JUSTIFY_COMMA_LIST		"left,left,left,right,right,right"
#define TRANSECT_BOUNDARY_JUSTIFY_COMMA_LIST	"left,left,left,left,right,right,left,left,right"
#define AUDIT_REPORT_BASIN_COLUMNS		"Nest,Discovery,UTMNorthing,UTMEasting,CellNumber,BasinReceived,BasinShouldBe"
#define BASIN_JUSTIFY_COMMA_LIST		"left,left,left,left,right,left,left"

/* Prototypes */
/* ---------- */
void audit_report_transect(
				char *application_name,
				char *input_filename );

void audit_report_transect_boundary(
				char *application_name,
				char *input_filename );

void audit_report_basin(
				char *application_name,
				char *input_filename );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *output_medium;
	char *input_filename;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	int load_count = 0;

	if ( argc != 5 )
	{
		fprintf( stderr, 
"Usage: %s application process_name filename output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	input_filename = argv[ 3 ];
	output_medium = argv[ 4 ];

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

	printf( "<h2>Load SRF Spreadsheet\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( strcmp( output_medium, "output_medium" ) == 0
	|| ( strcmp( output_medium, "screen" ) != 0
	&&   strcmp( output_medium, "audit_report" ) != 0
	&&   strcmp( output_medium, "database" ) != 0 ) )
	{
		printf( "<p>ERROR: invalid Output Medium.\n" );
		document_close();
		exit( 0 );
	}

	if ( strcmp( output_medium, "database" ) == 0 )
	{
		delete_nest( application_name, input_filename );
	}

	if ( strcmp( output_medium, "database" ) == 0
	||   strcmp( output_medium, "screen" ) == 0 )
	{
		load_count = insert_nests(
				application_name,
				input_filename,
				output_medium );

		if ( strcmp( output_medium, "database" ) == 0 )
			printf( "<p>Process complete with %d nests.\n",
				load_count );
		else
			printf( "<p>Process did not load %d nests.\n",
				load_count );
	}
	else
	{
		audit_report_transect(
				application_name,
				input_filename );

		audit_report_transect_boundary(
				application_name,
				input_filename );

		audit_report_basin(
				application_name,
				input_filename );
	}

	document_close();

	if ( strcmp( output_medium, "database" ) == 0 )
	{
		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}

	exit( 0 );
} /* main() */

void audit_report_transect(
			char *application_name,
			char *input_filename )
{
	char sys_string[ 1024 ];
	FILE *input_file;
	FILE *error_file;
	char input_string[ 1024 ];
	char error_filename[ 128 ];
	FILE *table_output_pipe = {0};
	char utm_northing_string[ 128 ];
	int utm_northing;
	char discovery_date_american[ 128 ];
	char discovery_date_international[ 128 ];
	char nest_number[ 128 ];
	char transect_number_string[ 128 ];
	int transect_number;
	int transect_number_should_be;
	char *nest_number_key;
	int distance = 0;

	/* Open the input file */
	/* ------------------- */
	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		printf( "<h2>ERROR: cannot open %s for read</h2>\n",
			input_filename );
		exit( 1 );
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

	sprintf( sys_string,
"queue_top_bottom_lines.e 50 | html_table.e 'Audit Report Transect' '%s' '|' '%s'",
		 AUDIT_REPORT_TRANSECT_COLUMNS,
		 TRANSECT_JUSTIFY_COMMA_LIST );

	table_output_pipe = popen( sys_string, "w" );

	/* Skip the first line. */
	/* -------------------- */
	get_line( input_string, input_file );

	while( get_line( input_string, input_file ) )
	{
		trim( input_string );
		if ( !*input_string ) continue;

		if ( !piece_double_quoted(
				utm_northing_string,
				',',
				input_string,
				SRF_UTM_NORTHING ) )
		{
			fprintf( error_file,
				 "Warning: cannot get UTM Northing in %s\n",
				 input_string );
			continue;
		}
		utm_northing = atoi( utm_northing_string );

		if ( !piece_double_quoted(
				discovery_date_american,
				',',
				input_string,
				SRF_DISCOVERY_DATE ) )
		{
			fprintf( error_file,
				 "Warning: cannot get date in %s\n",
				 input_string );
			continue;
		}

		*discovery_date_international = '\0';

		date_convert_source_american(
				discovery_date_international,
				international,
				discovery_date_american );

		if ( !*discovery_date_international )
		{
			fprintf( error_file,
				 "Warning: cannot convert date of (%s) in %s\n",
				 discovery_date_american,
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				nest_number,
				',',
				input_string,
				SRF_NEST_NUMBER ) )
		{
			fprintf( error_file,
				 "Warning: cannot get nest number in %s\n",
				 input_string );
			continue;
		}

		if ( strlen( nest_number ) > 4 )
		{
			fprintf( error_file,
			"Warning: nest number cannot be more than 9999 in %s\n",
				 input_string );
			continue;
		}

		if ( ! ( nest_number_key = 
			alligator_get_nest_key(
					discovery_date_international,
					(char *)0 /* discovery_year */,
					nest_number ) ) )
		{
			fprintf( error_file,
	"Warning: cannot get nest_number_key with (%s) and (%s)\n",
				discovery_date_international,
				nest_number );
			continue;
		}

		if ( !piece_double_quoted(
				transect_number_string,
				',',
				input_string,
				SRF_TRANSECT_NUMBER ) )
		{
			fprintf( error_file,
				 "Warning: cannot get transect number in %s\n",
				 input_string );
			continue;
		}
		transect_number = atoi( transect_number_string );

		transect_number_should_be =
			alligator_get_transect_number_should_be(
				&distance,
				application_name,
				utm_northing );

		if ( transect_number_should_be != transect_number )
		{
			fprintf(table_output_pipe,
			 	"%s|%d|%s|%d|%d|%d\n",
			 	nest_number_key,
			 	utm_northing,
			 	discovery_date_international,
			 	transect_number,
				transect_number_should_be,
				distance );
		}
	}

	fclose( input_file );
	fclose( error_file );
	pclose( table_output_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Nest Load Errors' '' '|'",
			 error_filename );
		system( sys_string );
	}

	sprintf( sys_string, "rm %s", error_filename );
	system( sys_string );

} /* audit_report_transect() */

void audit_report_transect_boundary(
			char *application_name,
			char *input_filename )
{
	char sys_string[ 1024 ];
	FILE *input_file;
	FILE *error_file;
	char input_string[ 1024 ];
	char error_filename[ 128 ];
	FILE *table_output_pipe = {0};
	char utm_northing_string[ 128 ];
	int utm_northing;
	char utm_easting_string[ 128 ];
	int utm_easting;
	char discovery_date_american[ 128 ];
	char discovery_date_international[ 128 ];
	char nest_number[ 128 ];
	char in_transect_boundary_yn[ 128 ];
	char *nest_number_key;
	char in_transect_boundary_should_be_yn;
	double distance;
	int transect_number_should_be;
	int distance_integer = 0;
	char transect_number_string[ 128 ];

	/* Open the input file */
	/* ------------------- */
	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		printf( "<h2>ERROR: cannot open %s for read</h2>\n",
			input_filename );
		exit( 1 );
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

	sprintf( sys_string,
"queue_top_bottom_lines.e 50 | html_table.e 'Audit Report Transect Boundary' '%s' '|' '%s'",
		 AUDIT_REPORT_TRANSECT_BOUNDARY_COLUMNS,
		 TRANSECT_BOUNDARY_JUSTIFY_COMMA_LIST );

	table_output_pipe = popen( sys_string, "w" );

	/* Skip the first line. */
	/* -------------------- */
	get_line( input_string, input_file );

	while( get_line( input_string, input_file ) )
	{
		trim( input_string );
		if ( !*input_string ) continue;

		if ( !piece_double_quoted(
				utm_northing_string,
				',',
				input_string,
				SRF_UTM_NORTHING ) )
		{
			fprintf( error_file,
				 "Warning: cannot get UTM Northing in %s\n",
				 input_string );
			continue;
		}
		utm_northing = atoi( utm_northing_string );

		if ( !piece_double_quoted(
				utm_easting_string,
				',',
				input_string,
				SRF_UTM_EASTING ) )
		{
			fprintf( error_file,
				 "Warning: cannot get UTM Easting in %s\n",
				 input_string );
			continue;
		}
		utm_easting = atoi( utm_easting_string );

		if ( !piece_double_quoted(
				discovery_date_american,
				',',
				input_string,
				SRF_DISCOVERY_DATE ) )
		{
			fprintf( error_file,
				 "Warning: cannot get date in %s\n",
				 input_string );
			continue;
		}

		*discovery_date_international = '\0';

		date_convert_source_american(
				discovery_date_international,
				international,
				discovery_date_american );

		if ( !*discovery_date_international )
		{
			fprintf( error_file,
				 "Warning: cannot convert date of (%s) in %s\n",
				 discovery_date_american,
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				nest_number,
				',',
				input_string,
				SRF_NEST_NUMBER ) )
		{
			fprintf( error_file,
				 "Warning: cannot get nest number in %s\n",
				 input_string );
			continue;
		}

		if ( strlen( nest_number ) > 4 )
		{
			fprintf( error_file,
			"Warning: nest number cannot be more than 9999 in %s\n",
				 input_string );
			continue;
		}

		if ( ! ( nest_number_key = 
			alligator_get_nest_key(
					discovery_date_international,
					(char *)0 /* discovery_year */,
					nest_number ) ) )
		{
			fprintf( error_file,
	"Warning: cannot get nest_number_key with (%s) and (%s)\n",
				discovery_date_international,
				nest_number );
			continue;
		}

		if ( !piece_double_quoted(
				transect_number_string,
				',',
				input_string,
				SRF_TRANSECT_NUMBER ) )
		{
			fprintf( error_file,
				 "Warning: cannot get transect number in %s\n",
				 input_string );
			continue;
		}

		transect_number_should_be =
			alligator_get_transect_number_should_be(
				&distance_integer,
				application_name,
				utm_northing );

		if ( !piece_double_quoted(
				in_transect_boundary_yn,
				',',
				input_string,
				SRF_IN_TRANSECT_BOUNDARY_YN ) )
		{
			fprintf( error_file,
			"Warning: cannot get transect boundary yn in %s\n",
				 input_string );
			continue;
		}

		if ( *in_transect_boundary_yn )
		{
			*in_transect_boundary_yn =
				tolower( *in_transect_boundary_yn );

			if ( *in_transect_boundary_yn != 'y' )
				*in_transect_boundary_yn = 'n';
		}
		else
		{
			*in_transect_boundary_yn = 'n';
		}

/*
		in_transect_boundary_should_be_yn =
			alligator_get_in_transect_boundary_should_be_yn(
				&distance,
				application_name,
				atoi( transect_number_string ),
				utm_northing,
				utm_easting );
*/

		in_transect_boundary_should_be_yn =
			alligator_get_in_transect_boundary_should_be_yn(
				&distance,
				application_name,
				transect_number_should_be,
				utm_northing,
				utm_easting );

		if (	*in_transect_boundary_yn !=
			in_transect_boundary_should_be_yn )
		{
			fprintf(table_output_pipe,
			 	"%s|%d|%d|%s|%s|%d|%c|%c|%.1lf\n",
			 	nest_number_key,
			 	utm_northing,
			 	utm_easting,
			 	discovery_date_international,
				transect_number_string,
			 	transect_number_should_be,
			 	*in_transect_boundary_yn,
			 	in_transect_boundary_should_be_yn,
				distance );
		}
	}

	fclose( input_file );
	fclose( error_file );
	pclose( table_output_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Nest Load Errors' '' '|'",
			 error_filename );
		system( sys_string );
	}

	sprintf( sys_string, "rm %s", error_filename );
	system( sys_string );

} /* audit_report_transect_boundary() */

void audit_report_basin(
			char *application_name,
			char *input_filename )
{
	char sys_string[ 1024 ];
	FILE *input_file;
	FILE *error_file;
	char input_string[ 1024 ];
	char error_filename[ 128 ];
	FILE *table_output_pipe = {0};
	char utm_northing_string[ 128 ];
	double utm_northing;
	char utm_easting_string[ 128 ];
	double utm_easting;
	char discovery_date_american[ 128 ];
	char discovery_date_international[ 128 ];
	char nest_number[ 128 ];
	char *nest_number_key;
	char basin_code[ 128 ];
	char *basin;
	char *basin_should_be;
	char basin1_buffer[ 128 ];
	char basin2_buffer[ 128 ];
	int cell_number;

	/* Open the input file */
	/* ------------------- */
	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		printf( "<h2>ERROR: cannot open %s for read</h2>\n",
			input_filename );
		exit( 1 );
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

	sprintf( sys_string,
"queue_top_bottom_lines.e 50 | html_table.e 'Audit Report Basin' '%s' '|' '%s'",
		 AUDIT_REPORT_BASIN_COLUMNS,
		 BASIN_JUSTIFY_COMMA_LIST );

	table_output_pipe = popen( sys_string, "w" );

	/* Skip the first line. */
	/* -------------------- */
	get_line( input_string, input_file );

	while( get_line( input_string, input_file ) )
	{
		trim( input_string );
		if ( !*input_string ) continue;

		if ( !piece_double_quoted(
				utm_northing_string,
				',',
				input_string,
				SRF_UTM_NORTHING ) )
		{
			fprintf( error_file,
				 "Warning: cannot get UTM Northing in %s\n",
				 input_string );
			continue;
		}
		utm_northing = atof( utm_northing_string );

		if ( !piece_double_quoted(
				utm_easting_string,
				',',
				input_string,
				SRF_UTM_EASTING ) )
		{
			fprintf( error_file,
				 "Warning: cannot get UTM Easting in %s\n",
				 input_string );
			continue;
		}
		utm_easting = atof( utm_easting_string );

		if ( !piece_double_quoted(
				discovery_date_american,
				',',
				input_string,
				SRF_DISCOVERY_DATE ) )
		{
			fprintf( error_file,
				 "Warning: cannot get date in %s\n",
				 input_string );
			continue;
		}

		*discovery_date_international = '\0';

		date_convert_source_american(
				discovery_date_international,
				international,
				discovery_date_american );

		if ( !*discovery_date_international )
		{
			fprintf( error_file,
				 "Warning: cannot convert date of (%s) in %s\n",
				 discovery_date_american,
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				nest_number,
				',',
				input_string,
				SRF_NEST_NUMBER ) )
		{
			fprintf( error_file,
				 "Warning: cannot get nest number in %s\n",
				 input_string );
			continue;
		}

		if ( strlen( nest_number ) > 4 )
		{
			fprintf( error_file,
			"Warning: nest number cannot be more than 9999 in %s\n",
				 input_string );
			continue;
		}

		if ( ! ( nest_number_key = 
			alligator_get_nest_key(
					discovery_date_international,
					(char *)0 /* discovery_year */,
					nest_number ) ) )
		{
			fprintf( error_file,
	"Warning: cannot get nest_number_key with (%s) and (%s)\n",
				discovery_date_international,
				nest_number );
			continue;
		}

		if ( !piece_double_quoted(
				basin_code,
				',',
				input_string,
				SRF_BASIN_CODE ) )
		{
			fprintf( error_file,
			"Warning: cannot get basin code in %s\n",
				 input_string );
			continue;
		}

		if ( ! ( basin = alligator_get_basin(
					application_name,
					basin_code ) ) )
		{
			continue;
		}

		cell_number = 0;

		if ( ! ( basin_should_be =
			alligator_get_basin_should_be(
				&cell_number,
				application_name,
				utm_northing,
				utm_easting ) ) )
		{
			fprintf( error_file,
			"Warning: cannot calculate basin in %s\n",
				 input_string );
			continue;
		}

		if ( strcmp( basin_should_be, basin ) != 0 )
		{
			fprintf(table_output_pipe,
			 	"%s|%s|%.1lf|%.1lf|%d|%s|%s\n",
			 	nest_number_key,
			 	discovery_date_international,
			 	utm_northing,
			 	utm_easting,
				cell_number,
			 	format_initial_capital(
					basin1_buffer, basin ),
			 	format_initial_capital(
					basin2_buffer, basin_should_be ) );
		}
	}

	fclose( input_file );
	fclose( error_file );
	pclose( table_output_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Nest Load Errors' '' '|'",
			 error_filename );
		system( sys_string );
	}

	sprintf( sys_string, "rm %s", error_filename );
	system( sys_string );

} /* audit_report_basin() */

int insert_nests(	char *application_name,
			char *input_filename,
			char *output_medium )
{
	char sys_string[ 1024 ];
	FILE *input_file;
	FILE *error_file;
	char input_string[ 1024 ];
	char *table_name;
	char error_filename[ 128 ];
	FILE *table_output_pipe = {0};
	FILE *nest_insert_pipe = {0};
	FILE *researcher_insert_pipe = {0};
	FILE *secondary_researcher_insert_pipe = {0};
	FILE *pilot_insert_pipe = {0};
	FILE *alligator_census_insert_pipe = {0};
	FILE *transect_insert_pipe = {0};
	char utm_easting_string[ 128 ];
	char utm_northing_string[ 128 ];
	int utm_easting;
	int utm_northing;
	char discovery_date_american[ 128 ];
	char discovery_date_international[ 128 ];
	char nest_number[ 128 ];
	char transect_number[ 128 ];
	char in_transect_boundary_yn[ 128 ];
	char basin_code[ 128 ];
	char *basin;
	char habitat_code[ 128 ];
	char *habitat;
	char near_pond_yn[ 128 ];
	char nest_location_notepad[ 1024 ];
	char nest_notepad[ 1024 ];
	char nest_full_notepad[ 2048 ];
	char primary_researcher[ 128 ];
	char pilot[ 128 ];
	char secondary_researcher[ 128 ];
	char srf_yn[ 128 ];
	char *nest_number_key;
	int load_count = 0;

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

	if ( strcmp( output_medium, "database" ) == 0 )
	{
		/* Open the nest insert pipe */
		/* ------------------------- */
		table_name =
			get_table_name(	application_name,
					"nest" );

		sprintf( sys_string,
"insert_statement.e table=%s field=%s delimiter='|' | sql.e 2>&1 | html_paragraph_wrapper.e",
		 	table_name,
		 	INSERT_NEST );

		nest_insert_pipe = popen( sys_string, "w" );

		/* Open the researcher insert pipe */
		/* -------------------------------- */
		table_name =
			get_table_name(	application_name,
					"researcher" );
	
		sprintf( sys_string,
"sort -u | insert_statement.e %s %s '|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
		 	table_name,
		 	INSERT_RESEARCHERS );

		researcher_insert_pipe = popen( sys_string, "w" );

		/* Open the secondary researcher insert pipe */
		/* ----------------------------------------- */
		table_name =
			get_table_name(	application_name,
					"secondary_researcher" );
	
		sprintf( sys_string,
"sort -u | insert_statement.e %s %s '|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
		 	table_name,
		 	INSERT_SECONDARY_RESEARCHER );

		secondary_researcher_insert_pipe = popen( sys_string, "w" );

		/* Open the pilot insert pipe */
		/* --------------------------- */
		table_name =
			get_table_name(	application_name,
					"pilot" );
	
		sprintf( sys_string,
"sort -u | insert_statement.e %s %s '|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
		 	table_name,
		 	INSERT_PILOT );

		pilot_insert_pipe = popen( sys_string, "w" );

		/* Open the discovery survey insert pipe */
		/* ------------------------------------- */
		table_name =
			get_table_name(	application_name,
					"alligator_census" );
	
		sprintf( sys_string,
"sort -u | insert_statement.e %s %s '|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
		 	table_name,
		 	INSERT_ALLIGATOR_CENSUS );

		alligator_census_insert_pipe = popen( sys_string, "w" );

		/* Open the transect insert pipe */
		/* ------------------------------ */
		table_name =
			get_table_name(	application_name,
					"transect" );

		sprintf( sys_string,
"sort -u | insert_statement.e %s %s '|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
		 	table_name,
		 	INSERT_TRANSECT );

		transect_insert_pipe = popen( sys_string, "w" );
	}
	else
	{
		sprintf( sys_string,
"queue_top_bottom_lines.e 50 | html_table.e 'Insert into Nests' %s '|'",
			 INSERT_NEST );

		table_output_pipe = popen( sys_string, "w" );
	}

	/* Skip the first line. */
	/* -------------------- */
	get_line( input_string, input_file );

	while( get_line( input_string, input_file ) )
	{
		trim( input_string );
		if ( !*input_string ) continue;

		if ( !piece_double_quoted(
				utm_easting_string,
				',',
				input_string,
				SRF_UTM_EASTING ) )
		{
			fprintf( error_file,
				 "Warning: cannot get UTM Easting in %s\n",
				 input_string );
			continue;
		}
		utm_easting = atoi( utm_easting_string );

		if ( !piece_double_quoted(
				utm_northing_string,
				',',
				input_string,
				SRF_UTM_NORTHING ) )
		{
			fprintf( error_file,
				 "Warning: cannot get UTM Northing in %s\n",
				 input_string );
			continue;
		}
		utm_northing = atoi( utm_northing_string );

		if ( !piece_double_quoted(
				discovery_date_american,
				',',
				input_string,
				SRF_DISCOVERY_DATE ) )
		{
			fprintf( error_file,
				 "Warning: cannot get date in %s\n",
				 input_string );
			continue;
		}

		*discovery_date_international = '\0';

		date_convert_source_american(
				discovery_date_international,
				international,
				discovery_date_american );

		if ( !*discovery_date_international )
		{
			fprintf( error_file,
				 "Warning: cannot convert date of (%s) in %s\n",
				 discovery_date_american,
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				nest_number,
				',',
				input_string,
				SRF_NEST_NUMBER ) )
		{
			fprintf( error_file,
				 "Warning: cannot get nest number in %s\n",
				 input_string );
			continue;
		}

		if ( strlen( nest_number ) > 4 )
		{
			fprintf( error_file,
			"Warning: nest number cannot be more than 9999 in %s\n",
				 input_string );
			continue;
		}

		if ( ! ( nest_number_key = 
			alligator_get_nest_key(
					discovery_date_international,
					(char *)0 /* discovery_year */,
					nest_number ) ) )
		{
			fprintf( error_file,
	"Warning: cannot get nest_number_key with (%s) and (%s)\n",
				discovery_date_international,
				nest_number );
			continue;
		}

		if ( !piece_double_quoted(
				transect_number,
				',',
				input_string,
				SRF_TRANSECT_NUMBER ) )
		{
			fprintf( error_file,
				 "Warning: cannot get transect number in %s\n",
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				in_transect_boundary_yn,
				',',
				input_string,
				SRF_IN_TRANSECT_BOUNDARY_YN ) )
		{
			fprintf( error_file,
			"Warning: cannot get transect boundary yn in %s\n",
				 input_string );
			continue;
		}

		if ( *in_transect_boundary_yn )
		{
			*in_transect_boundary_yn =
				tolower( *in_transect_boundary_yn );

			if ( *in_transect_boundary_yn != 'y' )
				*in_transect_boundary_yn = 'n';
		}

		if ( !piece_double_quoted(
				basin_code,
				',',
				input_string,
				SRF_BASIN_CODE ) )
		{
			fprintf( error_file,
			"Warning: cannot get basin code in %s\n",
				 input_string );
			continue;
		}

		if ( ! ( basin = alligator_get_basin(
					application_name,
					basin_code ) ) )
		{
			fprintf( error_file,
"Ignoring: cannot get basin for code = (%s) in %s. Ignoring basin column.\n",
				 basin_code,
				 input_string );
			basin = "";
		}

		if ( !piece_double_quoted(
				habitat_code,
				',',
				input_string,
				SRF_HABITAT_CODE ) )
		{
			fprintf( error_file,
			"Warning: cannot get habitat code in %s\n",
				 input_string );
			continue;
		}

		if ( ! ( habitat = alligator_get_habitat(
					application_name,
					habitat_code ) ) )
		{
			fprintf( error_file,
"Ignoring: cannot get habitat for code = (%s) in %s. Ignoring habitat column.\n",
				 habitat_code,
				 input_string );
			habitat = "";
		}

		if ( !piece_double_quoted(
				near_pond_yn,
				',',
				input_string,
				SRF_NEAR_POND_YN ) )
		{
			fprintf( error_file,
			"Warning: cannot get near pond yn in %s\n",
				 input_string );
			continue;
		}

		if ( *near_pond_yn )
		{
			*near_pond_yn =
				tolower( *near_pond_yn );
			if ( *near_pond_yn != 'y' )
				*near_pond_yn = 'n';
		}

		if ( !piece_double_quoted(
				nest_location_notepad,
				',',
				input_string,
				SRF_LOCATION_NOTEPAD ) )
		{
			fprintf( error_file,
			"Warning: cannot get location description in %s\n",
				 input_string );
			continue;
		}

		if ( strcmp( nest_location_notepad, "NR" ) == 0 )
			*nest_location_notepad = '\0';

		if ( !piece_double_quoted(
				nest_notepad,
				',',
				input_string,
				SRF_NEST_NOTEPAD ) )
		{
/*
			fprintf( error_file,
			"Warning: cannot get notepad in %s\n",
				 input_string );
			continue;
*/
			*nest_notepad = '\0';
		}

		trim( nest_notepad );

		if ( *nest_location_notepad && *nest_notepad )
		{
			sprintf(	nest_full_notepad,
					"%s;%s",
					nest_location_notepad,
					nest_notepad );
		}
		else
		if ( *nest_location_notepad && !*nest_notepad )
		{
			sprintf(	nest_full_notepad,
					"%s",
					nest_location_notepad );
		}
		else
		if ( !*nest_location_notepad && *nest_notepad )
		{
			sprintf(	nest_full_notepad,
					"%s",
					nest_notepad );
		}
		else
		{
			*nest_full_notepad = '\0';
		}

		if ( !piece_double_quoted(
				primary_researcher,
				',',
				input_string,
				SRF_PRIMARY_RESEARCHER ) )
		{
			fprintf( error_file,
			"Warning: cannot get primary researcher in %s\n",
				 input_string );
			continue;
		}

		if ( !*primary_researcher )
			strcpy( primary_researcher, UNKNOWN_RESEARCHER );

		if ( !piece_double_quoted(
				pilot,
				',',
				input_string,
				SRF_PILOT ) )
		{
			fprintf( error_file,
			"Warning: cannot get pilot in %s\n",
				 input_string );
			continue;
		}

		if ( !piece_double_quoted(
				srf_yn,
				',',
				input_string,
				SRF_YN ) )
		{
/*
			fprintf( error_file,
			"Warning: cannot get SRF_YN in %s\n",
				 input_string );
			continue;
*/
			strcpy( srf_yn, "y" );
		}

		if ( *srf_yn ) *srf_yn = tolower( *srf_yn );

		if ( *srf_yn && (*srf_yn != 'y') && (*srf_yn != 'n') )
		{
			fprintf( error_file,
			"Ignoring: invalid SRF_YN in %s. Ignoring column.\n",
				 input_string );
			*srf_yn = '\0';
		}

		if ( !*srf_yn ) *srf_yn = 'n';

/*
#define INSERT_NEST			"nest_number,utm_easting,utm_northing,discovery_date,primary_researcher,transect_number,in_transect_boundary_yn,basin,habitat,near_pond_yn,nest_notepad,SRF_yn"
#define INSERT_TRANSECT			"transect_number"
#define INSERT_DISCOVERY_CENSUS		"discovery_date,primary_researcher,pilot,transect_number"
#define INSERT_PILOT			"pilot"
#define INSERT_SECONDARY_RESEARCHER	"researcher,discovery_date,primary_researcher"
#define INSERT_RESEARCHERS		"researcher"

*/
		if ( strcmp( output_medium, "database" ) == 0 )
		{
			fprintf( nest_insert_pipe,
				 "%s|%d|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
				 nest_number_key,
				 utm_easting,
				 utm_northing,
				 discovery_date_international,
				 primary_researcher,
				 transect_number,
				 in_transect_boundary_yn,
				 basin,
				 habitat,
				 near_pond_yn,
				 nest_full_notepad,
				 srf_yn );

			fprintf( transect_insert_pipe,
				 "%s\n",
				 transect_number );

			fprintf( pilot_insert_pipe,
				 "%s\n",
				 pilot );

			fprintf( alligator_census_insert_pipe,
				 "%s|%s|%s\n",
				 discovery_date_international,
				 primary_researcher,
				 pilot );

			fprintf( researcher_insert_pipe,
				 "%s\n",
				 primary_researcher );

			if ( !piece_double_quoted(
					secondary_researcher,
					',',
					input_string,
					SRF_SECONDARY_RESEARCHER1 ) )
			{
				fprintf( error_file,
			"Warning: cannot get secondary researcher in %s\n",
				 	input_string );
				continue;
			}

			if ( *secondary_researcher
			&&   strcmp( secondary_researcher, "none" ) != 0 )
			{
				fprintf( secondary_researcher_insert_pipe,
				 	"%s|%s|%s\n",
				 	secondary_researcher,
				 	discovery_date_international,
				 	primary_researcher );

				fprintf( researcher_insert_pipe,
				 	"%s\n",
				 	secondary_researcher );
			}

			if ( !piece_double_quoted(
					secondary_researcher,
					',',
					input_string,
					SRF_SECONDARY_RESEARCHER2 ) )
			{
				fprintf( error_file,
			"Warning: cannot get secondary researcher in %s\n",
				 	input_string );
				continue;
			}

			if ( *secondary_researcher
			&&   strcmp( secondary_researcher, "none" ) != 0  )
			{
				fprintf( secondary_researcher_insert_pipe,
				 	"%s|%s|%s\n",
				 	secondary_researcher,
				 	discovery_date_international,
				 	primary_researcher );

				fprintf( researcher_insert_pipe,
				 	"%s\n",
				 	secondary_researcher );
			}
		}
		else
		{
			fprintf( table_output_pipe,
				 "%s|%d|%d|%s|%s|%s|%c|%s|%s|%c|%s|%s\n",
				 nest_number_key,
				 utm_easting,
				 utm_northing,
				 discovery_date_international,
				 primary_researcher,
				 transect_number,
				 tolower( *in_transect_boundary_yn ),
				 basin,
				 habitat,
				 tolower( *near_pond_yn ),
				 nest_full_notepad,
				 srf_yn );
		}
		load_count++;
	}

	fclose( input_file );
	fclose( error_file );

	if ( strcmp( output_medium, "database" ) == 0 )
	{
		pclose( nest_insert_pipe );
		pclose( researcher_insert_pipe );
		pclose( secondary_researcher_insert_pipe );
		pclose( pilot_insert_pipe );
		pclose( alligator_census_insert_pipe );
		pclose( transect_insert_pipe );
	}
	else
	{
		pclose( table_output_pipe );
	}

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Nest Load Errors' '' '|'",
			 error_filename );
		system( sys_string );
	}

	sprintf( sys_string, "rm %s", error_filename );
	system( sys_string );

	return load_count;

} /* insert_nests() */

#define DELETE_FIELD_LIST	"nest_number"

void delete_nest(	char *application_name,
			char *input_filename )
{
	FILE *input_file;
	FILE *nest_delete_pipe;
	char sys_string[ 1024 ];
	char input_string[ 1024 ];
	char *table_name;
	char nest_number[ 128 ];
	char discovery_date_american[ 128 ];
	char discovery_date_international[ 128 ];
	char *nest_number_key;

	/* Open the input file */
	/* ------------------- */
	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	/* Open the nest delete pipe */
	/* ------------------------- */
	table_name =
		get_table_name(
			application_name, "nest" );

	sprintf( sys_string,
"delete_statement.e t=%s f=%s d='|' | sql.e 2>&1",
		 table_name,
		 DELETE_FIELD_LIST );

	nest_delete_pipe = popen( sys_string, "w" );

	/* Skip the first line. */
	/* -------------------- */
	get_line( input_string, input_file );

	while( get_line( input_string, input_file ) )
	{
		trim( input_string );
		if ( !*input_string ) continue;

		if ( !piece_double_quoted(
				discovery_date_american,
				',',
				input_string,
				SRF_DISCOVERY_DATE ) )
		{
			continue;
		}

		if ( !piece_double_quoted(
				nest_number,
				',',
				input_string,
				SRF_NEST_NUMBER ) )
		{
			continue;
		}

		*discovery_date_international = '\0';

		date_convert_source_american(
				discovery_date_international,
				international,
				discovery_date_american );

		if ( !*discovery_date_international )
		{
			continue;
		}

		if ( ! ( nest_number_key = 
			alligator_get_nest_key(
					discovery_date_international,
					(char *)0 /* discovery_year */,
					nest_number ) ) )
		{
			continue;
		}

		fprintf( nest_delete_pipe,
			 "%s\n",
			 nest_number_key );
	}

	fclose( input_file );
	pclose( nest_delete_pipe );

} /* delete_nest() */

