/* --------------------------------------------------- 	*/
/* src_hydrology/load_cr10.c		    	   	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "timlib.h"
#include "basename.h"
#include "piece.h"
#include "list.h"
#include "document.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "process.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define PROCESS_NAME			"load_cr10"
#define YEAR_PIECE			1 
#define DAYOFYEAR_PIECE			2 
#define TIME_PIECE			2
#define DEFAULT_SOURCE_DIRECTORY	"/opt/export/enpdata"
#define TDP_FILENAME_STEM		"load_cr10_bad_tdp"
#define GREP_FILENAME_STEM		"load_cr10_bad_grep"
#define DATE_FILENAME_STEM		"load_cr10_bad_date"
#define MEASUREMENT_ERROR_STEM		"load_cr10_measurement_error"
/* --------------------------------------------------- */
/* Note: TDP ==> Transmission Datatype Position table  */
/*		 Now known as CR10_PARSE or just PARSE */
/* --------------------------------------------------- */

/* --------------------------------------------------------------------------
Sample input:
10,2000,317,230,1.43,0
10,2000,317,240,1.42,0
10,2000,317,250,1.41,0
10,2000,317,300,1.41,0
60,2000,317,300,0,23.85,-6999,24.06,0,48.8,-30.93,31.9,12.7,60,23.82,23.62
^  ^    ^   ^   ^ ^
|  |    |   |   | value of TDP[2]
|  |    |   |   value of TDP[1]
|  |    |   time
|  |    date offset
|  year
record type
-------------------------------------------------------------------------- */

/* Prototypes */
/* ---------- */
void load_cr10(	LIST *file_list, 
		char *tdp_bad_file,
		char *grep_ignore_file,
		char *date_ignore_file,
		char *measurement_error_file,
		char *station,
		char *date_offset_comma_list,
		int with_file_trim_yn,
		int really_yn,
		char *application_name,
		boolean with_shift_left );

int main( int argc, char **argv )
{
	char *application_name;
	char *station;
	char *source_directory;
	char *date_offset_comma_list;
	char *cr10_file_specification;
	char with_file_trim_yn;
	char really_yn;
	DOCUMENT *document;
	char *tdp_bad_file;
	char *grep_ignore_file;
	char *date_ignore_file;
	char *measurement_error_file;
	LIST *file_list;
	char sys_string[ 4096 ];
	char execution_directory[ 128 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	APPASERVER_LINK_FILE *appaserver_link_file;
	boolean with_shift_left = 0;

	if ( argc < 9 )
	{
		fprintf(stderr,
"Usage: %s application source_directory 'file_specification' station date_offset_comma_list with_file_trim_yn ignored really_yn [with_shift_left_yn]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	source_directory = argv[ 2 ];
	cr10_file_specification = argv[ 3 ];
	station = argv[ 4 ];
	date_offset_comma_list = argv[ 5 ];
	with_file_trim_yn = *argv[ 6 ];
	/* email_address = argv[ 7 ]; */
	really_yn = *argv[ 8 ];

	if ( argc == 10 ) with_shift_left = ( *argv[ 9 ] == 'y' );

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );
	add_python_library_path();

	appaserver_parameter_file = appaserver_parameter_file_new();

	/* cd to the execution directory */
	/* ----------------------------- */
	sprintf(execution_directory,
		"%s/%s",
		appaserver_parameter_file->appaserver_mount_point,
		application_relative_source_directory( application_name));

	if ( chdir( execution_directory ) == -1 )
	{
		fprintf(	stderr,
				"Cannot change to %s directory\n",
				execution_directory );
		exit( 1 );
	}

	if ( !*source_directory
	||   strcmp( source_directory, "source_directory" ) == 0 )
	{
		source_directory = DEFAULT_SOURCE_DIRECTORY;
	}

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		appaserver_parameter_file->
				document_root,
			(char *)0 /* filename_stem */,
			application_name,
			getpid(),
			(char *)0 /* session */,
			"dat" );

	appaserver_link_file->filename_stem = TDP_FILENAME_STEM;

	tdp_bad_file =
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

	appaserver_link_file->filename_stem = GREP_FILENAME_STEM;

	grep_ignore_file =
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

	appaserver_link_file->filename_stem = DATE_FILENAME_STEM;

	date_ignore_file =
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

	appaserver_link_file->filename_stem = MEASUREMENT_ERROR_STEM;

	measurement_error_file =
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

	if ( strcmp( cr10_file_specification, "filename" ) == 0 )
	{
		if ( *station && strcmp( station, "station" ) != 0 )
		{
			sprintf( cr10_file_specification,
				 "%s.DAT", 
				 up_string( station ) );
		}
		else
		{
			strcpy( cr10_file_specification, "*.DAT" );
		}
	}

	sprintf( sys_string,
		 "echo %s/%s | tr ' ' '\012' | grep -v '#'",
		  source_directory,
		  cr10_file_specification );

	file_list = pipe2list( sys_string );

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

	load_cr10(	file_list, 
			tdp_bad_file,
			grep_ignore_file,
			date_ignore_file,
			measurement_error_file,
			station,
			date_offset_comma_list,
			with_file_trim_yn,
			really_yn,
			application_name,
			with_shift_left );

	if ( really_yn == 'y' )
	{
		printf( "<p>Process complete.\n" );

		process_increment_execution_count(
			application_name,
			PROCESS_NAME,
			(char *)0 /* database_management_system */ );
	}
	else
	{
		printf( "<p>Process NOT executed.\n" );
	}

	document_close();
	exit( 0 );
} /* main() */

void load_cr10(	LIST *file_list, 
		char *tdp_bad_file,
		char *grep_ignore_file,
		char *date_ignore_file,
		char *measurement_error_file,
		char *station,
		char *date_offset_comma_list,
		int with_file_trim_yn,
		int really_yn,
		char *application_name,
		boolean with_shift_left )
{
	char *filename_to_process;
	char *station_to_process;
	char trim_process[ 512 ];
	char archive_process[ 512 ];
	char grep_process[ 512 ];
	char shift_process[ 512 ];
	char replace_cr10_error_with_null_process[ 512 ];
	char remove_process[ 1024 ];
	char summary_process[ 1024 ];
	char sys_string[ 4096 ];

	sprintf(	replace_cr10_error_with_null_process,
			"%s",
			"sed 's/,6999\\.0*$/,/' | sed 's/,-6999\\.0*$/,/'" );

	sprintf(remove_process, "rm -f %s %s %s",
		tdp_bad_file,
		grep_ignore_file,
		date_ignore_file );

	sprintf(summary_process,
		"echo \"<p>Count of bad records because no parse table entry = "
		"`cat %s | wc -l`"
		"; count because of a bad date = "
		"`cat %s | wc -l`\"",
		tdp_bad_file,
		date_ignore_file );

	if ( !list_rewind( file_list ) )
	{
		printf( "<p>ERROR: file not found\n" );
		return;
	}

	if ( with_shift_left )
	{
		strcpy( shift_process, "piece_shift_left.e ','" );
	}
	else
	{
		strcpy( shift_process, "cat -" );
	}

	do {
		filename_to_process = list_get_string( file_list );

		/* ---------------------------------- */
		/* If passed a station as a parameter */
		/* ---------------------------------- */
		if ( *station && strcmp( station, "station" ) != 0 )
			station_to_process = station;
		else
		{
			char filename_without_path[ 512 ];
			char station_from_filename[ 512 ];

			piece(	filename_without_path, 
				'/',
				filename_to_process,
				count_characters( '/', filename_to_process ) );

			piece( 	station_from_filename, 
				'.', 
				filename_without_path, 
				0 );
			station_to_process = strdup( station_from_filename );
		}

		printf( "<p>Processing: %s\n", filename_to_process );

		if ( really_yn == 'y' && with_file_trim_yn == 'y' )
		{
			sprintf( trim_process,
"date2dayofyear.e ',' %d < %s > %s; cat %s >> %s; cat %s >> %s",
				 DAYOFYEAR_PIECE - 1,
				 tdp_bad_file, 
				 filename_to_process,
				 grep_ignore_file,
				 filename_to_process,
				 date_ignore_file,
				 filename_to_process );
		}
		else
		{
			strcpy( trim_process, "echo > /dev/null" );
		}

		if ( really_yn == 'y' && with_file_trim_yn == 'y' )
		{
			sprintf( archive_process,
			"archive_cr10_file_done.sh %s %s %s",
				 application_name,
				 station_to_process, 
				 filename_to_process );
		}
		else
			strcpy( archive_process, "echo > /dev/null" );

		if ( *date_offset_comma_list
		&&   strcmp( date_offset_comma_list,
			     "date_offset_comma_list" ) != 0 )
		{
			sprintf(	grep_process, 
					"piece_grep ',' %d %s 2>>%s",
					DAYOFYEAR_PIECE,
					date_offset_comma_list,
					grep_ignore_file );
		}
		else
		{
			strcpy( grep_process, "cat -" );
		}

		sprintf( sys_string, 
	"> %s							     ;"
	"> %s							     ;"
	"> %s							     ;"
	"cat %s						      	     |"
	"grep -v '^$'						     |"
	"%s							     |"
	"%s							     |"
	"dayofyear2date.e ',' %d %d y 2>%s		  	     |"
	"piece_inverse.e 1 ','					     |"
	"prepend_zeros_to_piece.e ',' %d 4			     |"
	"apply_position_translation_to_record %s %s \',\' 2>>%s	     |"
	"validate_midnight 2 3 ','				     |"
	"%s							     |"
	"measurement_insert %s cr10 %c 2>>%s	      		     ;"
	"%s 					      		     ;"
	"%s							     ;"
	"%s							     ;"
	"%s							      ",
			 tdp_bad_file,
			 grep_ignore_file,
			 date_ignore_file,
			 filename_to_process,
			 grep_process,
			 shift_process,
			 YEAR_PIECE,
			 DAYOFYEAR_PIECE,
			 date_ignore_file,
			 TIME_PIECE,
			 application_name,
			 station_to_process,
			 tdp_bad_file,
			 replace_cr10_error_with_null_process,
			 application_name,
			 really_yn,
			 measurement_error_file,
			 archive_process,
			 trim_process,
			 summary_process,
			 remove_process );

		fflush( stdout );
		fflush( stderr );

/* fprintf( stderr, "%s\n", sys_string ); */

		system( sys_string );

	} while( list_next( file_list ) );

	if ( timlib_file_populated( measurement_error_file ) )
	{
		char sys_string[ 1024 ];

		sprintf( sys_string,
			 "cat %s			|"
			 "queue_top_bottom_lines.e 50	|"
			 "html_paragraph_wrapper.e	 ",
			 measurement_error_file );
		system( sys_string );
	}

} /* load_cr10() */

