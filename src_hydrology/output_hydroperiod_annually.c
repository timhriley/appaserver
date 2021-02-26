/* ------------------------------------------------------------ 	*/
/* $APPASERVER_HOME/src_hydrology/output_hydroperiod_annually.c	      	*/
/* ------------------------------------------------------------ 	*/
/* Freely available software: see Appaserver.org			*/
/* ------------------------------------------------------------ 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "measurement_update_parameter.h"
#include "hashtbl.h"
#include "dictionary.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "measurement_backup.h"
#include "query.h"
#include "station_datatype.h"
#include "appaserver_parameter_file.h"
#include "grace.h"
#include "application.h"
#include "environ.h"
#include "list.h"
#include "list_usage.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "hydrology_library.h"
#include "station.h"
#include "html_table.h"
#include "annual_hydroperiod.h"

/* Constants */
/* --------- */
#define REPORT_TITLE				"Annual Hydroperiod"
#define DEFAULT_OUTPUT_MEDIUM			"summary_table"
#define FILENAME_STEM				"hydroperiod"

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	int begin_year, end_year;
	char *output_medium;
	char *process_name;
	char *database_string = {0};
	char *station_list_string;
	LIST *station_list;
	char *stage_datatype_list_string;
	LIST *stage_datatype_list;
	double elevation_offset;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	boolean detail;
	DOCUMENT *document = {0};
	ANNUAL_HYDROPERIOD *annual_hydroperiod;
	char *station_elevation_null;
	char begin_year_string[ 16 ];
	char end_year_string[ 16 ];

	if ( argc != 9 )
	{
		fprintf(stderr,
"Usage: %s application process station datatype begin_year end_year elevation_offset output_medium\n",
			argv[ 0 ] );
		fprintf(stderr,
"Note: output_medium = {detail_table,summary_table,detail_text_file,summary_text_file,stdout,summary_stdout}\n" );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	station_list_string = argv[ 3 ];
	stage_datatype_list_string = argv[ 4 ];
	begin_year = atoi( argv[ 5 ] );
	end_year = atoi( argv[ 6 ] );
	elevation_offset = atof( argv[ 7 ] );
	output_medium = argv[ 8 ];

	detail = ( ( instr( "detail", output_medium, 1 ) != -1 ) ||
		 ( strcmp( output_medium, "stdout" ) == 0 ) );

	station_list =
		list_string2list(
			station_list_string, ',' );

	stage_datatype_list =
		list_string2list(
			stage_datatype_list_string, ',' );

	if ( !begin_year )
	{
		begin_year =
			annual_hydroperiod_get_full_period_of_record_year(
				application_name,
				station_list,
				stage_datatype_list,
				"min(measurement_date)",
				"____-01-01",
			        "measurement",
				"measurement_date" );
	}

	if ( !end_year )
	{
		end_year =
			annual_hydroperiod_get_full_period_of_record_year(
				application_name,
				station_list,
				stage_datatype_list,
				"max(measurement_date)",
				"____-12-31",
				"measurement",
				"measurement_date" );
	}

	sprintf( begin_year_string, "%d", begin_year );
	sprintf( end_year_string, "%d", end_year );

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

	annual_hydroperiod =
		annual_hydroperiod_new(
			elevation_offset );

	if ( !begin_year
	||   !*station_list_string
	||   strcmp( station_list_string, "station" ) == 0 )
	{
		document_quick_output_body(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point );

		printf( "<p>ERROR: Insufficient input.\n" );
		document_close();
		exit( 0 );
	}

	if ( ! ( annual_hydroperiod->station_list =
		annual_hydroperiod_get_station_list(
			&station_elevation_null,
			application_name,
			station_list_string,
			stage_datatype_list_string,
			annual_hydroperiod->elevation_offset,
			begin_year,
			end_year,
			detail ) ) )
	{
		document_quick_output_body(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point );

		if ( station_elevation_null )
			printf(
			"<h3>ERROR: Station elevation is null for %s.</h3>\n",
			station_elevation_null );
		else
			printf( "<h3>ERROR: Insufficient input.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !*output_medium
	||   strcmp( output_medium, "output_medium" ) == 0 )
	{
		output_medium = DEFAULT_OUTPUT_MEDIUM;
	}

	if ( strcmp( output_medium, "stdout" ) != 0
	&&   strcmp( output_medium, "summary_stdout" ) != 0 )
	{
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
	}

	if ( strcmp( output_medium, "chart_discontinuous" ) == 0 )
	{
		if ( !annual_hydroperiod_output_chart_discontinuous(
					REPORT_TITLE,
					begin_year,
					end_year,
					annual_hydroperiod,
					argv[ 0 ],
					application_name,
					appaserver_parameter_file->
						document_root,
					annual_hydroperiod->elevation_offset ) )
		{
			printf(
			"<h3>Warning: Nothing selected to chart.</h3>\n" );
			document_close();
			exit( 0 );
		}
	}
	else
	if ( strcmp( output_medium, "summary_table" ) == 0
	||   strcmp( output_medium, "detail_table" ) == 0 )
	{
		if ( detail )
		{
			annual_hydroperiod_output_table_detail(
					REPORT_TITLE,
					begin_year,
					end_year,
					annual_hydroperiod,
					0 /* dont omit elevation */,
					application_name );
		}
		else
		{
			annual_hydroperiod_output_table_summary(
					REPORT_TITLE,
					begin_year,
					end_year,
					annual_hydroperiod,
					0 /* dont omit elevation */,
					application_name );
		}
	}
	else
	if ( strcmp( output_medium, "detail_transmit" ) == 0
	||   strcmp( output_medium, "detail_text_file" ) == 0 )
	{
		char *ftp_filename;
		char *output_pipename;
		pid_t process_id = getpid();
		FILE *output_pipe;
		char sys_string[ 1024 ];
		char title[ 512 ];
		APPASERVER_LINK_FILE *appaserver_link_file;

		appaserver_link_file =
			appaserver_link_file_new(
				application_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->
					document_root,
				FILENAME_STEM,
				application_name,
				process_id,
				(char *)0 /* session */,
				"txt" );

		appaserver_link_file->begin_date_string = begin_year_string;
		appaserver_link_file->end_date_string = end_year_string;

		output_pipename =
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

		annual_hydroperiod_get_report_title(
					title,
					REPORT_TITLE,
					begin_year,
					end_year );

/*
		sprintf( output_pipename, 
			 OUTPUT_FILE_TEXT_FILE,
			 appaserver_parameter_file->appaserver_mount_point,
			 application_name, 
			 begin_year,
			 end_year,
			 process_id );
*/
	
		if ( ! ( output_pipe = fopen( output_pipename, "w" ) ) )
		{
			printf( "<H2>ERROR: Cannot open output file %s\n",
				output_pipename );
			document_close();
			exit( 1 );
		}
		else
		{
			fclose( output_pipe );
		}

/*
		sprintf( sys_string,
			 "delimiter2padded_columns.e '|' 8 > %s",
			 output_pipename );
*/
		sprintf(sys_string,
		 	"tr '|' '%c' > %s",
			OUTPUT_TEXT_FILE_DELIMITER,
		 	output_pipename );

		output_pipe = popen( sys_string, "w" );

		annual_hydroperiod_output_transmit_detail(
				output_pipe,
				title,
				annual_hydroperiod,
				0 /* dont omit elevation */,
				application_name );

		pclose( output_pipe );

		printf( "<h1>%s<br></h1>\n", title );
		printf( "<h1>\n" );
		fflush( stdout );
		if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
		fflush( stdout );
		printf( "</h1>\n" );
	
		appaserver_library_output_ftp_prompt(
				ftp_filename,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}
	else
	if ( strcmp( output_medium, "detail_spreadsheet" ) == 0 )
	{
		char *ftp_filename;
		char *output_pipename;
		pid_t process_id = getpid();
		FILE *output_pipe;
		char sys_string[ 1024 ];
		char title[ 512 ];
		APPASERVER_LINK_FILE *appaserver_link_file;

		appaserver_link_file =
			appaserver_link_file_new(
				application_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->
					document_root,
				FILENAME_STEM,
				application_name,
				process_id,
				(char *)0 /* session */,
				"csv" );

		appaserver_link_file->begin_date_string = begin_year_string;
		appaserver_link_file->end_date_string = end_year_string;

		output_pipename =
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


		annual_hydroperiod_get_report_title(
					title,
					REPORT_TITLE,
					begin_year,
					end_year );

/*
		sprintf( output_pipename, 
			 OUTPUT_FILE_SPREADSHEET,
			 appaserver_parameter_file->appaserver_mount_point,
			 application_name, 
			 begin_year,
			 end_year,
			 process_id );
*/
	
		if ( ! ( output_pipe = fopen( output_pipename, "w" ) ) )
		{
			printf( "<H2>ERROR: Cannot open output file %s\n",
				output_pipename );
			document_close();
			exit( 1 );
		}
		else
		{
			fclose( output_pipe );
		}

		sprintf( sys_string,
			 "tr '|' ',' > %s",
			 output_pipename );

		output_pipe = popen( sys_string, "w" );

		annual_hydroperiod_output_transmit_detail(
				output_pipe,
				title,
				annual_hydroperiod,
				0 /* dont omit elevation */,
				application_name );

		pclose( output_pipe );

		printf( "<h1>%s<br></h1>\n", title );
		printf( "<h1>\n" );
		fflush( stdout );
		if ( system( "date '+%x %H:%M'" ) ) {};
		fflush( stdout );
		printf( "</h1>\n" );
	
		appaserver_library_output_ftp_prompt(
				ftp_filename,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}
	else
	if ( strcmp( output_medium, "summary_spreadsheet" ) == 0 )
	{
		char *ftp_filename;
		char *output_pipename;
		pid_t process_id = getpid();
		FILE *output_pipe;
		char sys_string[ 1024 ];
		char title[ 512 ];
		APPASERVER_LINK_FILE *appaserver_link_file;

		appaserver_link_file =
			appaserver_link_file_new(
				application_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->
					document_root,
				FILENAME_STEM,
				application_name,
				process_id,
				(char *)0 /* session */,
				"csv" );

		appaserver_link_file->begin_date_string = begin_year_string;
		appaserver_link_file->end_date_string = end_year_string;

		output_pipename =
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

		annual_hydroperiod_get_report_title(
					title,
					REPORT_TITLE,
					begin_year,
					end_year );

/*
		sprintf( output_pipename, 
			 OUTPUT_FILE_SPREADSHEET,
			 appaserver_parameter_file->appaserver_mount_point,
			 application_name, 
			 begin_year,
			 end_year,
			 process_id );
*/
	
		if ( ! ( output_pipe = fopen( output_pipename, "w" ) ) )
		{
			printf( "<H2>ERROR: Cannot open output file %s\n",
				output_pipename );
			document_close();
			exit( 1 );
		}
		else
		{
			fclose( output_pipe );
		}

		sprintf( sys_string,
			 "tr '|' ',' > %s",
			 output_pipename );

		output_pipe = popen( sys_string, "w" );

		annual_hydroperiod_output_text_file_summary(
				output_pipe,
				title,
				annual_hydroperiod,
				application_name );

		pclose( output_pipe );

		printf( "<h1>%s<br></h1>\n", title );
		printf( "<h1>\n" );
		fflush( stdout );
		if ( system( "date '+%x %H:%M'" ) ) {};
		fflush( stdout );
		printf( "</h1>\n" );
	
		appaserver_library_output_ftp_prompt(
				ftp_filename,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}
	else
	if ( strcmp( output_medium, "summary_transmit" ) == 0
	||   strcmp( output_medium, "summary_text_file" ) == 0 )
	{
		char *ftp_filename;
		char *output_pipename;
		pid_t process_id = getpid();
		FILE *output_pipe;
		char sys_string[ 1024 ];
		char title[ 512 ];
		APPASERVER_LINK_FILE *appaserver_link_file;

		appaserver_link_file =
			appaserver_link_file_new(
				application_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->
					document_root,
				FILENAME_STEM,
				application_name,
				process_id,
				(char *)0 /* session */,
				"txt" );

		appaserver_link_file->begin_date_string = begin_year_string;
		appaserver_link_file->end_date_string = end_year_string;

		output_pipename =
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

		annual_hydroperiod_get_report_title(
					title,
					REPORT_TITLE,
					begin_year,
					end_year );

/*
		sprintf( output_pipename, 
			 OUTPUT_FILE_TEXT_FILE,
			 appaserver_parameter_file->appaserver_mount_point,
			 application_name, 
			 begin_year,
			 end_year,
			 process_id );
*/
	
		if ( ! ( output_pipe = fopen( output_pipename, "w" ) ) )
		{
			printf( "<H2>ERROR: Cannot open output file %s\n",
				output_pipename );
			document_close();
			exit( 1 );
		}
		else
		{
			fclose( output_pipe );
		}

/*
		sprintf( sys_string,
			 "delimiter2padded_columns.e '|' 8 > %s",
			 output_pipename );
*/
		sprintf(sys_string,
		 	"tr '|' '%c' > %s",
			OUTPUT_TEXT_FILE_DELIMITER,
		 	output_pipename );

		output_pipe = popen( sys_string, "w" );

		annual_hydroperiod_output_text_file_summary(
				output_pipe,
				title,
				annual_hydroperiod,
				application_name );

		pclose( output_pipe );

		printf( "<h1>%s<br></h1>\n", title );
		printf( "<h1>\n" );
		fflush( stdout );
		if ( system( "date '+%x %H:%M'" ) ) {};
		fflush( stdout );
		printf( "</h1>\n" );
	
		appaserver_library_output_ftp_prompt(
				ftp_filename,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}
	else
	if ( strcmp( output_medium, "summary_stdout" ) == 0 )
	{
		char title[ 512 ];

		annual_hydroperiod_get_report_title(
					title,
					REPORT_TITLE,
					begin_year,
					end_year );

		annual_hydroperiod_output_text_file_summary(
				stdout,
				title,
				annual_hydroperiod,
				application_name );
	}
	else
	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		FILE *output_pipe;
		char sys_string[ 512 ];
		char title[ 512 ];

		annual_hydroperiod_get_report_title(
					title,
					REPORT_TITLE,
					begin_year,
					end_year );

/*
		sprintf( sys_string,
			 "delimiter2padded_columns.e '|' 8" );
*/
		sprintf(sys_string,
		 	"tr '|' '%c'",
			OUTPUT_TEXT_FILE_DELIMITER );

		output_pipe = popen( sys_string, "w" );

		annual_hydroperiod_output_transmit_detail(
				output_pipe,
				title,
				annual_hydroperiod,
				0 /* dont omit elevation */,
				application_name );

		pclose( output_pipe );
	}

	if ( strcmp( output_medium, "stdout" ) != 0
	&&   strcmp( output_medium, "summary_stdout" ) != 0 )
	{
		document_close();
	}

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;

} /* main() */

