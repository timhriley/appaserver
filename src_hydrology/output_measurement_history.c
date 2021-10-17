/* $APPASERVER_HOME/src_hydrology/output_measurement_history.c		*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "hydrology_library.h"
#include "appaserver.h"
#include "appaserver_link.h"

/* Constants */
/* --------- */
#define FILENAME_STEM		"measurement_history"

#define DEFAULT_OUTPUT		"table"
#define HEADING_LIST "measurement_date,measurement_time,measurement_update_date,measurement_update_time,measurement_update_method,value,change,percent_change"
#define MEASUREMENT_BACKUP_SELECT "measurement_date,measurement_time,'aaa',measurement_update_date,measurement_update_time,measurement_update_method,original_value"
#define MEASUREMENT_SELECT "measurement_date,measurement_time,'zzz','','','Currently',measurement_value"

#define HTML_TABLE_JUSTIFY_LIST "left,left,left,left,left,right"

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *station;
	char *datatype;
	char *where_clause;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char sys_string[ 1024 ];
	char report_title[ 512 ];
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];
	char *output_medium;
	char *ftp_filename = {0};
	char *output_filename = {0};

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s application ignored process_name station datatype where output_medium\n",
			 argv[ 0 ] );
		fprintf( stderr,
"Note: output_medium = {table,stdout,text_file,spreadsheet}\n" );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	/* session = argv[ 2 ]; */
	process_name = argv[ 3 ];
	station = argv[ 4 ];
	datatype = argv[ 5 ];
	where_clause = argv[ 6 ];
	output_medium = argv[ 7 ];

	if ( !*output_medium
	||   strcmp( output_medium, "output_medium" ) == 0 )
	{
		output_medium = DEFAULT_OUTPUT;
	}

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

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		document_quick_output_body(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point );
	}

	sprintf(	report_title,
			"Measurement History for %s/%s",
			station,
			datatype );

	format_initial_capital( report_title, report_title );

	sprintf( sys_string,
		 "(get_folder_data	application=%s			 "
		 "			select=\"%s\"			 "
		 "			folder=measurement_backup	 "
		 "			where=\"%s\"			 "
		 "			order=\"none\"			;"
		 "get_folder_data	application=%s			 "
		 "			select=\"%s\"			 "
		 "			folder=measurement		 "
		 "			where=\"%s\"			 "
		 "			order=\"none\")			|"
		 "sort							|"
		 "piece_inverse.e 2 '%c'				|"
		 "group_date_time.e					 "
		 "		value_offset=5 				 "
		 "		delimiter='%c' 			 	 "
		 "		function=percent_change_base_first	 "
		 "cat							 ",
		 application_name,
		 MEASUREMENT_BACKUP_SELECT,
		 where_clause,
		 application_name,
		 MEASUREMENT_SELECT,
		 where_clause,
		 FOLDER_DATA_DELIMITER,
		 FOLDER_DATA_DELIMITER );

	input_pipe = popen( sys_string, "r" );

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		hydrology_library_output_station_table(
			application_name,
			station );

		sprintf(sys_string,
		 	"html_table.e \"%s\" \"%s\" '%c' %s",
		 	report_title,
		 	HEADING_LIST,
		 	FOLDER_DATA_DELIMITER,
		 	HTML_TABLE_JUSTIFY_LIST );
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		pid_t process_id = getpid();
		FILE *output_file;
		APPASERVER_LINK *appaserver_link;

		appaserver_link =
			appaserver_link_new(
				application_http_prefix( application_name ),
				appaserver_library_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
				appaserver_parameter_file->
					document_root,
				FILENAME_STEM,
				application_name,
				process_id,
				(char *)0 /* session */,
				(char *)0 /* begin_date_string */,
				(char *)0 /* end_date_string */,
				"csv" );

		output_filename = appaserver_link->output->filename;
		ftp_filename = appaserver_link->prompt->filename;

		if ( ! ( output_file = fopen( output_filename, "w" ) ) )
		{
			printf( "<H2>ERROR: Cannot open output file %s\n",
				output_filename );
			document_close();
			exit( 1 );
		}
		else
		{
			fclose( output_file );
		}

		hydrology_library_output_station_text_filename(
				output_filename,
				application_name,
				station,
				1 /* with_zap_file */ );

		sprintf( sys_string,
			 "tr '%c' ',' >> %s",
			 FOLDER_DATA_DELIMITER,
			 output_filename );
	}
	else
	if ( strcmp( output_medium, "text_file" ) == 0 )
	{
		pid_t process_id = getpid();
		FILE *output_file;
		APPASERVER_LINK *appaserver_link;

		appaserver_link =
			appaserver_link_new(
				application_http_prefix( application_name ),
				appaserver_library_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
				appaserver_parameter_file->
					document_root,
				FILENAME_STEM,
				application_name,
				process_id,
				(char *)0 /* session */,
				(char *)0 /* begin_date_string */,
				(char *)0 /* end_date_string */,
				"txt" );

		output_filename = appaserver_link->output->filename;
		ftp_filename = appaserver_link->prompt->filename;

		if ( ! ( output_file = fopen( output_filename, "w" ) ) )
		{
			printf( "<H2>ERROR: Cannot open output file %s\n",
				output_filename );
			document_close();
			exit( 1 );
		}
		else
		{
			fclose( output_file );
		}

		hydrology_library_output_station_text_filename(
				output_filename,
				application_name,
				station,
				1 /* with_zap_file */ );

/*
		sprintf( sys_string,
			 "delimiter2padded_columns.e '%c' 3 >> %s",
			 FOLDER_DATA_DELIMITER,
			 output_filename );
*/
		sprintf(sys_string,
		 	"tr '%c' '%c' >> %s",
			FOLDER_DATA_DELIMITER,
			OUTPUT_TEXT_FILE_DELIMITER,
		 	output_filename );

	}
	else
	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
/*
		sprintf( sys_string,
			 "delimiter2padded_columns.e '%c' 3",
			 FOLDER_DATA_DELIMITER );
*/
		sprintf(sys_string,
		 	"tr '%c' '%c'",
			FOLDER_DATA_DELIMITER,
			OUTPUT_TEXT_FILE_DELIMITER );
	}
	else
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: invalid output_medium = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 output_medium );
		exit( 1 );
	}

	output_pipe = popen( sys_string, "w" );

	if ( strcmp( output_medium, "stdout" ) == 0
	||   strcmp( output_medium, "text_file" ) == 0 )
	{
		char heading_list_string[ 256 ];

		strcpy( heading_list_string, HEADING_LIST );
		search_replace_character(
				heading_list_string,
				',',
				FOLDER_DATA_DELIMITER );
		fprintf( output_pipe, "#%s\n", report_title );
		fprintf( output_pipe, "#%s\n", heading_list_string );
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		fprintf( output_pipe, "#%s\n", report_title );
		fprintf( output_pipe, "#%s\n", HEADING_LIST );
	}

	while( get_line( input_buffer, input_pipe ) )
	{
		fprintf( output_pipe, "%s\n", input_buffer );
	}

	pclose( input_pipe );
	pclose( output_pipe );

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		document_close();
	}
	else
	if ( strcmp( output_medium, "text_file" ) == 0
	||   strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		printf( "<h1>Measurement History Transmission<br></h1>\n" );
		printf( "<h2>\n" );
		fflush( stdout );
		system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
		fflush( stdout );
		printf( "</h2>\n" );
	
		appaserver_library_output_ftp_prompt(
				ftp_filename,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;

} /* main() */

