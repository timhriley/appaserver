/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_creel/load_trailer_count.c	*/
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
#include "creel_library.h"
#include "creel_load_library.h"
#include "environ.h"
#include "date_convert.h"
#include "application.h"
#include "process.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */
#define DATE_COLUMN_HEADING		"Date MDY"
#define PARKING_LOT_FLORIDA_BAY		"florida_bay"
#define PARKING_LOT_WHITE_WATER_BAY	"white_water_bay"
#define PARKING_LOT_FLAMINGO		"flamingo"

#define WHITEWATER_BAY_PIECE		2
#define FLORIDA_BAY_PIECE		1
#define FLAMINGO_PIECE			3

/* Prototypes */
/* ---------- */
boolean parking_lot_exists(	char *application_name,
				char *parking_lot );

void delete_trailer_count(
				char *application_name,
				char *input_filename );

void insert_trailer_count(	int *trailer_count_record_count,
				char *application_name,
				char *input_filename,
				char really_yn );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *input_filename;
	char really_yn;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	int trailer_count_record_count = 0;
	int results;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf(stderr, 
			"Usage: %s process filename really_yn\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	input_filename = argv[ 2 ];
	really_yn = *argv[ 3 ];

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

	printf( "<h2>Load Trailer Count\n" );
	fflush( stdout );
	results = system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( !parking_lot_exists(	application_name,
					PARKING_LOT_FLORIDA_BAY ) )
	{
		printf( "<h3>Error: parking lot no longer exists: %s</h3>\n",
			PARKING_LOT_FLORIDA_BAY	);
		document_close();
		exit( 0 );
	}

	if ( !parking_lot_exists(	application_name,
					PARKING_LOT_WHITE_WATER_BAY ) )
	{
		printf( "<h3>Error: parking lot no longer exists: %s</h3>\n",
			PARKING_LOT_WHITE_WATER_BAY );
		document_close();
		exit( 0 );
	}

	if ( !parking_lot_exists(	application_name,
					PARKING_LOT_FLAMINGO ) )
	{
		printf( "<h3>Error: parking lot no longer exists: %s</h3>\n",
			PARKING_LOT_FLAMINGO );
		document_close();
		exit( 0 );
	}

	if ( really_yn == 'y' )
	{
		delete_trailer_count( application_name, input_filename );
	}

	insert_trailer_count(	&trailer_count_record_count,
				application_name,
				input_filename,
				really_yn );

	if ( really_yn == 'y' )
	{
		printf(
		"<p>Process complete with %d trailer records.\n",
			trailer_count_record_count );

		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}
	else
	{
		printf(
		"<p>Process NOT executed with %d trailer records.\n",
			trailer_count_record_count );
	}

	document_close();

	exit( 0 );
} /* main() */

#define DELETE_FIELD_LIST	"date,parking_lot"

void delete_trailer_count(	char *application_name,
				char *input_filename )
{
	FILE *input_file;
	FILE *delete_pipe;
	char sys_string[ 1024 ];
	char input_string[ 1024 ];
	char compressed_date[ 256 ];
	char trailer_count[ 16 ];
	char *american_date_pointer;
	char american_date_string[ 16 ];
	char international_date_string[ 256 ];
	char *table_name;

	table_name =
		get_table_name(
			application_name, "trailer_count" );

	sprintf( sys_string,
"delete_statement.e t=%s f=%s d='|' | sql.e 2>&1 | html_paragraph_wrapper.e",
		 table_name,
		 DELETE_FIELD_LIST );

	delete_pipe = popen( sys_string, "w" );

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	while( get_line( input_string, input_file ) )
	{
		trim_character( input_string, '"', input_string );
		trim_character( input_string, '\'', input_string );

		if ( !piece(	compressed_date,
				',',
				input_string,
				0 ) )
		{
			continue;
		}

		if ( timlib_strcmp(	compressed_date,
					DATE_COLUMN_HEADING ) == 0 )
		{
			continue;
		}

		if ( character_exists( compressed_date, '/' ) )
		{
			strcpy( american_date_string, compressed_date );
		}
		else
		if ( ! ( american_date_pointer =
			date_convert_american_sans_slashes(
				compressed_date ) ) )
		{
			continue;
		}
		else
		{
			strcpy( american_date_string, american_date_pointer );
		}

		*international_date_string = '\0';

		date_convert_source_american(
				international_date_string,
				international,
				american_date_string );

		if ( !*international_date_string )
		{
			continue;
		}

		/* White Water Bay */
		/* --------------- */
		if ( piece(	trailer_count,
				',',
				input_string,
				WHITEWATER_BAY_PIECE ) )
		{
			if ( *trailer_count )
			{
				fprintf(delete_pipe,
					"%s|%s\n",
		     			international_date_string,
					PARKING_LOT_WHITE_WATER_BAY );
			}
		}

		/* Florida Bay */
		/* ----------- */
		if ( piece(	trailer_count,
				',',
				input_string,
				FLORIDA_BAY_PIECE ) )
		{
			if ( *trailer_count )
			{
				fprintf(delete_pipe,
					"%s|%s\n",
		     			international_date_string,
					PARKING_LOT_FLORIDA_BAY );
			}
		}

		/* Flamingo */
		/* -------- */
		if ( piece(	trailer_count,
				',',
				input_string,
				FLAMINGO_PIECE ) )
		{
			if ( *trailer_count )
			{
				fprintf(delete_pipe,
					"%s|%s\n",
		     			international_date_string,
					PARKING_LOT_FLAMINGO );
			}
		}
	}

	fclose( input_file );
	pclose( delete_pipe );

} /* delete_trailer_count() */

#define INSERT_FIELD_LIST	"date,parking_lot,trailer_count"

void insert_trailer_count(	int *trailer_count_record_count,
				char *application_name,
				char *input_filename,
				char really_yn )
{
	FILE *input_file;
	FILE *output_pipe = {0};
	char sys_string[ 1024 ];
	char input_string[ 1024 ];
	char compressed_date[ 256 ];
	char trailer_count[ 16 ];
	char *american_date_pointer;
	char american_date_string[ 16 ];
	char international_date_string[ 256 ];
	char error_filename[ 128 ];
	FILE *error_file;
	int line_number = 0;
	boolean found_one;
	int results;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	sprintf( error_filename, "/tmp/trailer_count_error_%d.txt", getpid() );
	if ( ! ( error_file = fopen( error_filename, "w" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		fclose( input_file );
		exit( 1 );
	}

	if ( really_yn == 'y' )
	{
		char *table_name =
			get_table_name( application_name, "trailer_count" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | html_paragraph_wrapper.e",
			 table_name,
			 INSERT_FIELD_LIST );
		output_pipe = popen( sys_string, "w" );

	}
	else
	{
		sprintf( sys_string,
"queue_top_bottom_lines.e 50 | html_table.e 'Insert into Trailer Count' %s '|' left,left,right",
			 INSERT_FIELD_LIST );
		output_pipe = popen( sys_string, "w" );
	}

	while( get_line( input_string, input_file ) )
	{
		line_number++;
		trim_character( input_string, '"', input_string );
		trim_character( input_string, '\'', input_string );

		if ( !piece(	compressed_date,
				',',
				input_string,
				0 ) )
		{
			fprintf(error_file,
			"Warning in line %d: Cannot piece date in (%s)\n",
				line_number,
				input_string );
			fflush( error_file );
			continue;
		}

		if ( timlib_strcmp(	compressed_date,
					DATE_COLUMN_HEADING ) == 0 )
		{
			fprintf(error_file,
			"Warning in line %d: ignoring heading of (%s)\n",
				line_number,
				input_string );
			fflush( error_file );
			continue;
		}

		if ( character_exists( compressed_date, '/' ) )
		{
			strcpy( american_date_string, compressed_date );
		}
		else
		if ( ! ( american_date_pointer =
			date_convert_american_sans_slashes(
				compressed_date ) ) )
		{
			fprintf(error_file,
			"Warning in line %d: Invalid date (%s)\n",
				line_number,
				input_string );
			fflush( error_file );
			continue;
		}
		else
		{
			strcpy( american_date_string, american_date_pointer );
		}

		*international_date_string = '\0';

		date_convert_source_american(
				international_date_string,
				international,
				american_date_string );

		if ( !*international_date_string )
		{
			fprintf(error_file,
			"Warning in line %d: Invalid date (%s)\n",
				line_number,
				input_string );
			fflush( error_file );
			continue;
		}

		found_one = 0;

		/* White Water Bay */
		/* --------------- */
		if ( piece(	trailer_count,
				',',
				input_string,
				WHITEWATER_BAY_PIECE ) )
		{
			if ( *trailer_count )
			{
				fprintf(output_pipe,
					"%s|%s|%s\n",
		     			international_date_string,
					PARKING_LOT_WHITE_WATER_BAY,
					trailer_count );

				(*trailer_count_record_count)++;
				found_one = 1;
			}
		}

		/* Florida Bay */
		/* ----------- */
		if ( piece(	trailer_count,
				',',
				input_string,
				FLORIDA_BAY_PIECE ) )
		{
			if ( *trailer_count )
			{
				fprintf(output_pipe,
					"%s|%s|%s\n",
		     			international_date_string,
					PARKING_LOT_FLORIDA_BAY,
					trailer_count );
				(*trailer_count_record_count)++;
				found_one = 1;
			}
		}

		/* Flamingo */
		/* -------- */
		if ( piece(	trailer_count,
				',',
				input_string,
				FLAMINGO_PIECE ) )
		{
			if ( *trailer_count )
			{
				fprintf(output_pipe,
					"%s|%s|%s\n",
		     			international_date_string,
					PARKING_LOT_FLAMINGO,
					trailer_count );
				(*trailer_count_record_count)++;
				found_one = 1;
			}
		}

		if ( !found_one )
		{
			fprintf(error_file,
	"Warning in line %d: Cannot get any parking lot count in (%s)\n",
				line_number,
				input_string );
			fflush( error_file );
		}
	}

	fclose( input_file );
	fclose( error_file );
	pclose( output_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'File Errors' '' '|'",
			 error_filename );
		results = system( sys_string );
	}

	sprintf( sys_string, "rm %s", error_filename );
	results = system( sys_string );

} /* insert_trailer_count() */

boolean parking_lot_exists(	char *application_name,
				char *parking_lot )
{
	char sys_string[ 1024 ];
	char where[ 512 ];

	sprintf( where, "parking_lot = '%s'", parking_lot );
	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=count		"
		 "			folder=parking_lot	"
		 "			where=\"%s\"		",
		 application_name,
		 where );
	return atoi( pipe2string( sys_string ) );
} /* parking_lot_exists() */

