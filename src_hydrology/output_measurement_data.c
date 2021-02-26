/* --------------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/output_measurement_data.c	*/
/* --------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* --------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "list.h"
#include "station.h"
#include "dictionary.h"
#include "folder.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "validation_level.h"
#include "hydrology_library.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "process_generic_output.h"
#include "application.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */
enum output_medium { output_medium_stdout, text_file, table, spreadsheet };

/* Constants */
/* --------- */
#define FILENAME_STEM				"measurements"
#define ZULU_TIME_OFFSET_HOURS			5
#define DEFAULT_OUTPUT_MEDIUM			text_file
#define SOURCE_FOLDER				"measurement"
#define DATE_PIECE				2
#define TIME_PIECE				3
#define VALUE_PIECE				4
#define STATION_PIECE				0
#define SELECT_LIST				 "station,datatype,measurement_date,measurement_time,measurement_value"
#define SELECT_WITH_VALIDATION_LIST		 "station,datatype,measurement_date,measurement_time,measurement_value,last_validation_date"

/* Prototypes */
/* ---------- */
char *get_sys_string(	char *application_name,
			char *output_filename,
			char *where_clause,
			char *sort_process,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *datatype,
			int date_piece,
			int value_piece,
			int station_piece,
			char *appaserver_mount_point,
			char *transmit_validation_info_yn,
			char *units_converted,
			char *end_date,
			boolean accumulate_flag,
			char *format_process,
			char date_time_delimiter,
			boolean zulu_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *begin_date, *end_date;
	char *email_address;
	char *aggregate_level_string;
	char *validation_level_string;
	char *aggregate_statistic_string;
	char *station_name;
	char *datatype;
	char *transmit_validation_info_yn;
	char *output_filename;
	char *ftp_filename;
	char end_date_suffix[ 128 ];
	char where_clause[ 4096 ];
	char *sort_process;
	char *sys_string;
	char html_sys_string[ 1024 ];
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	enum aggregate_level aggregate_level;
	enum aggregate_statistic aggregate_statistic;
	enum validation_level validation_level;
	int process_id = getpid();
	char *units_converted;
	boolean accumulate_flag = 0;
	char *database_string = {0};
	char format_process[ 128 ];
	int right_justified_columns_from_right;
	enum output_medium output_medium = DEFAULT_OUTPUT_MEDIUM;
	char date_time_delimiter;
	boolean zulu_time;
	APPASERVER_LINK_FILE *appaserver_link_file;

	if ( argc < 14 )
	{
		fprintf( stderr, 
	"Usage: %s application station datatype begin_date end_date email_address aggregate_level validation_level aggregate_statistic accumulate_yn transmit_validation_info_yn units_converted zulu_time_yn [output_medium]\n",
			 argv[ 0 ] );
		fprintf(stderr,
	"Note: output_medium = {table,stdout,text_file,spreadsheet}\n" );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	station_name = argv[ 2 ];
	datatype = argv[ 3 ];
	begin_date = argv[ 4 ];
	end_date = argv[ 5 ];
	email_address = argv[ 6 ];
	aggregate_level_string = argv[ 7 ];
	validation_level_string = argv[ 8 ];
	aggregate_statistic_string = argv[ 9 ];
	accumulate_flag = (*argv[ 10 ] == 'y');
	transmit_validation_info_yn = argv[ 11 ];
	units_converted = argv[ 12 ];
	zulu_time = ( *argv[ 13 ] == 'y' );

	if ( argc == 15 )
	{
		if ( strcmp( argv[ 14 ], "stdout" ) == 0 )
			output_medium = output_medium_stdout;
		else
		if ( strcmp( argv[ 14 ], "text_file" ) == 0 )
			output_medium = text_file;
		else
		if ( strcmp( argv[ 14 ], "spreadsheet" ) == 0 )
			output_medium = spreadsheet;
		else
		if ( strcmp( argv[ 14 ], "table" ) == 0 )
			output_medium = table;
		else
		{
			fprintf(stderr,
"ERROR in %s: output_medium must be either stdout, text_file, spreadsheet or table\n",
				argv[ 0 ] );
			exit( 1 );
		}
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

	hydrology_library_get_clean_begin_end_date(
					&begin_date,
					&end_date,
					application_name,
					station_name,
					datatype );

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date,
					&end_date,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	aggregate_level =
		aggregate_level_get_aggregate_level( aggregate_level_string );

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
				aggregate_statistic_string,
				aggregate_level );

	validation_level =
		validation_level_get_validation_level( validation_level_string);

	if ( strcmp( end_date, "end_date" ) != 0 )
		sprintf( end_date_suffix, "%s_", end_date );
	else
		*end_date_suffix = '\0';

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
			(char *)0 /* extension */ );

	if ( output_medium == text_file
	||   output_medium == output_medium_stdout )
	{
		appaserver_link_file->extension = "txt";
	}
	else
	{
		appaserver_link_file->extension = "csv";
	}

	output_filename =
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

	if ( strcmp( end_date, "end_date" ) != 0 )
	{
		sprintf( where_clause,
		 	"station = '%s' and			"
		 	"datatype = '%s' and			"
			"measurement_date between '%s' and '%s'	",
			station_name,
			datatype,
			begin_date,
			end_date );
	}
	else
	{
		sprintf( where_clause,
		 	"station = '%s' and			"
		 	"datatype = '%s' and			"
			"measurement_date = '%s'		",
			station_name,
			datatype,
			begin_date );
	}

	strcat( where_clause,
		/* ----------------------- */
		/* Returns program memory. */
		/* ----------------------- */
		hydrology_library_provisional_where(
			validation_level ) );

	if ( aggregate_level == real_time || aggregate_level == daily )
	{
		sort_process = "sort -f";
	}
	else
	{
		sort_process = "sort -f -r";
	}

	if ( aggregate_level == real_time )
		right_justified_columns_from_right = 1;
	else
		right_justified_columns_from_right = 2;

	if ( accumulate_flag ) right_justified_columns_from_right++;

	if ( output_medium == output_medium_stdout
	||   output_medium == text_file )
	{
		sprintf(format_process,
		 	"tr ',' '%c'",
			OUTPUT_TEXT_FILE_DELIMITER );

		date_time_delimiter = ' ';
	}
	else
	{
		strcpy( format_process, "cat" );
		date_time_delimiter = '/';
	}

	if ( output_medium == table )
	{
		FILE *input_pipe;
		char input_buffer[ 512 ];
		FILE *output_pipe;
		char justify_column_list_string[ 256 ];
		char title[ 512 ];

		sys_string = get_sys_string(	
				application_name,
				(char *)0 /* output_filename */,
				where_clause,
				sort_process,
				aggregate_level,
				aggregate_statistic,
				datatype,
				DATE_PIECE,
				VALUE_PIECE,
				STATION_PIECE,
				appaserver_parameter_file->
					appaserver_mount_point,
				transmit_validation_info_yn,
				units_converted,
				end_date,
				accumulate_flag,
				format_process,
				date_time_delimiter,
				zulu_time );

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

		hydrology_library_output_station_table(
			application_name,
			station_name );

		strcpy( justify_column_list_string, "left,left,left" );

		/* The time */
		/* -------- */
		if ( aggregate_level == real_time
		||   aggregate_level == half_hour
		||   aggregate_level == hourly )
		{
			strcat( justify_column_list_string, ",left" );
		}

		strcat( justify_column_list_string, ",right" );

		if ( accumulate_flag )
		{
			strcat( justify_column_list_string, ",right" );
		}

		/* The frequency per period */
		/* ------------------------ */
		if ( aggregate_level != real_time )
			strcat( justify_column_list_string, ",right" );

		input_pipe = popen( sys_string, "r" );
		get_line( input_buffer, input_pipe );
		search_replace_string( input_buffer, "#stn", "station" );

		sprintf( title,
			 "Measurements: %s",
			 where_clause );

		sprintf(html_sys_string,
			"html_table.e \"%s\" \"%s\" ',' %s",
			title,
		        input_buffer,
			justify_column_list_string );	

		output_pipe = popen( html_sys_string, "w" );

		while( get_line( input_buffer, input_pipe ) )
		{
			fprintf( output_pipe, "%s\n", input_buffer );
		}

		pclose( input_pipe );
		pclose( output_pipe );
		document_close();
	}
	else
	if ( output_medium == output_medium_stdout )
	{
		hydrology_library_output_station_text_filename(
				output_filename,
				application_name,
				station_name,
				1 /* with_zap_file */ );

		sys_string = get_sys_string(	
				application_name,
				output_filename,
				where_clause,
				sort_process,
				aggregate_level,
				aggregate_statistic,
				datatype,
				DATE_PIECE,
				VALUE_PIECE,
				STATION_PIECE,
				appaserver_parameter_file->
					appaserver_mount_point,
				transmit_validation_info_yn,
				units_converted,
				end_date,
				accumulate_flag,
				format_process,
				date_time_delimiter,
				zulu_time );

		if ( system( sys_string ) ){};

		sprintf( sys_string, "cat %s", output_filename );
		if ( system( sys_string ) ){};

		sprintf( sys_string, "rm %s", output_filename );
		if ( system( sys_string ) ){};

	}
	else
	if ( output_medium == text_file
	||   output_medium == spreadsheet )
	{

		hydrology_library_output_station_text_filename(
				output_filename,
				application_name,
				station_name,
				1 /* with_zap_file */ );

		sys_string = get_sys_string(	
				application_name,
				output_filename,
				where_clause,
				sort_process,
				aggregate_level,
				aggregate_statistic,
				datatype,
				DATE_PIECE,
				VALUE_PIECE,
				STATION_PIECE,
				appaserver_parameter_file->
					appaserver_mount_point,
				transmit_validation_info_yn,
				units_converted,
				end_date,
				accumulate_flag,
				format_process,
				date_time_delimiter,
				zulu_time );

		if ( system( sys_string ) ){};

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

		if ( !*email_address
		||   strcmp( email_address, "email_address" ) == 0 ) 
		{
			printf( "<h1>Measurement Transmission<br></h1>\n" );
			printf( "<h2>\n" );
			fflush( stdout );
			if ( system( timlib_system_date_string() ) ){};
			fflush( stdout );
			printf( "</h2>\n" );
		
			printf( "<BR><p>Search criteria: %s<hr>\n",
				where_clause );
		
			appaserver_library_output_ftp_prompt(
					ftp_filename, 
					TRANSMIT_PROMPT,
					(char *)0 /* target */,
					(char *)0 /* application_type */ );
		}
		else
		{
			char sys_string[ 256 ];
	
			sprintf( sys_string,
				 "cat %s				|"
				 "mailx -s \"Measurements %s\" %s	 ",
				 output_filename,
				 where_clause,
				 email_address );
			if ( system( sys_string ) ){};
			printf( "<h1>Measurement Transmission<br></h1>\n" );
			printf( "<h2>\n" );
			fflush( stdout );
			if ( system( timlib_system_date_string() ) ){};
			fflush( stdout );
			printf( "</h2>\n" );
		
			printf( "<BR><p>Search criteria: %s>\n", where_clause );
			printf( "<BR><p>Sent to %s<hr>\n", email_address );
		}
		document_close();
	}

	return 0;

} /* main() */

char *get_sys_string(	char *application_name,
			char *output_filename,
			char *where_clause,
			char *sort_process,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *datatype,
			int date_piece,
			int value_piece,
			int station_piece,
			char *appaserver_mount_point,
			char *transmit_validation_info_yn,
			char *units_converted,
			char *end_date,
			boolean accumulate_flag,
			char *format_process,
			char date_time_delimiter,
			boolean zulu_time )
{
	char sys_string[ 4096 ];
	char aggregation_process[ 256 ];
	char accumulation_process[ 256 ];
	char zulu_process[ 256 ];
	char output_process[ 256 ];
	char trim_time_process[ 256 ];
	char heading[ 256 ];
	char value_label[ 32 ];
	char accumulate_label[ 32 ];
	char *select_list_string;
	char *units;
	char *units_display;
	boolean bar_chart;
	char units_converted_process[ 256 ];

	if ( zulu_time )
	{
		sprintf( zulu_process,
			 "julian_add_hours.e %d %d %d '%c' Z",
			 ZULU_TIME_OFFSET_HOURS,
			 date_piece,
			 date_piece + 1,
			 FOLDER_DATA_DELIMITER );
	}
	else
	{
		strcpy( zulu_process, "cat" );
	}

	units = hydrology_library_get_units_string(
					&bar_chart,
					application_name,
					datatype );

	units_display =
		hydrology_library_get_datatype_units_display(
			application_name,
			datatype,
			units,
			units_converted,
			aggregate_statistic );

	if ( !units_display )
	{
		document_quick_output_body(	application_name,
						appaserver_mount_point );
		printf( "%s\n",
			hydrology_library_get_output_invalid_units_error(
				datatype,
				units,
				units_converted ) );
		document_close();
		exit( 0 );
	}

	if ( *transmit_validation_info_yn == 'y'
	&& ( aggregate_level == real_time
	||   aggregate_level == daily
	||   aggregate_level == half_hour
	||   aggregate_level == hourly ) )
		select_list_string = SELECT_WITH_VALIDATION_LIST;
	else
		select_list_string = SELECT_LIST;

	if ( aggregate_statistic == aggregate_statistic_none )
	{
		aggregate_statistic =
			based_on_datatype_get_aggregate_statistic(
				application_name,
				datatype,
				aggregate_statistic );
	}

	if ( aggregate_level == real_time )
	{
		strcpy( aggregation_process, "cat" );
		sprintf( trim_time_process,
			 "time_hhmm2hhcolonmm.e ',' %d",
			 date_piece + 1 );
		strcpy( value_label, "value" );
	}
	else
	{
		sprintf( aggregation_process, 
			 "real_time2aggregate_value.e %s %d %d %d ',' %s n %s",
			 aggregate_statistic_get_string( aggregate_statistic ),
			 date_piece,
			 date_piece + 1,
			 date_piece + 2,
			 aggregate_level_get_string( aggregate_level ),
			 end_date );

		if ( aggregate_level != half_hour
		&&   aggregate_level != hourly )
		{
			sprintf(trim_time_process,
			 	"piece_inverse.e %d ','",
			 	date_piece + 1 );
		}
		else
		{
			sprintf( trim_time_process,
			 	"time_hhmm2hhcolonmm.e ',' %d",
			 	date_piece + 1 );
		}

		sprintf( value_label,
			 "%s",
			 aggregate_statistic_get_string(
				aggregate_statistic ) );

	}

	if ( accumulate_flag )
	{
		sprintf( accumulation_process, 
			 "sort | accumulate.e %d ',' append",
			 date_piece + 2 );

		strcpy( accumulate_label, ",accumulate" );
	}
	else
	{
		strcpy( accumulation_process, "cat" );
		*accumulate_label = '\0';
	}

	if ( units_converted
	&&   *units_converted
	&&   strcmp( units_converted, "units_converted" ) != 0 )
	{
		sprintf( units_converted_process,
			 "measurement_convert_units.e %s %s %s %d ',' %d",
			 application_name,
			 units,
			 units_converted,
			 value_piece,
			 station_piece );
	}
	else
	{
		strcpy( units_converted_process, "cat" );
	}

	if ( aggregate_level == real_time )
	{
		sprintf(heading,
			"#stn,type,date%ctime,%s(%s)%s",
			date_time_delimiter,
			value_label,
			units_display,
			accumulate_label );

		if ( *transmit_validation_info_yn == 'y' )
		{
			sprintf( heading + strlen( heading ),
			",validation_date" );
		}
	}
	else
	if ( aggregate_level == hourly )
	{
		if ( *transmit_validation_info_yn == 'y' )
		{
			sprintf(heading,
	"#stn,type,date%ctime,%s(%s)%s,validation_date,values/hour",
				date_time_delimiter,
				value_label,
				units_display,
				accumulate_label );
		}
		else
		{
			sprintf(heading,
			"#stn,type,date%ctime,%s(%s)%s,values/hour",
				date_time_delimiter,
				value_label,
				units_display,
				accumulate_label );
		}
	}
	else
	if ( aggregate_level == half_hour )
	{
		if ( *transmit_validation_info_yn == 'y' )
		{
			sprintf(heading,
"#stn,type,date%ctime,%s(%s)%s,validation_date,values/halfhour",
				date_time_delimiter,
				value_label,
				units_display,
				accumulate_label );
		}
		else
		{
			sprintf(heading,
			"#stn,type,date%ctime,%s(%s)%s,values/halfhour",
				date_time_delimiter,
				value_label,
				units_display,
				accumulate_label );
		}
	}
	else
	if ( aggregate_level == daily )
	{
		if ( *transmit_validation_info_yn == 'y' )
		{
			sprintf(heading,
		"#stn,type,date,%s(%s)%s,validation_date,values/day",
				value_label,
				units_display,
				accumulate_label );
		}
		else
		{
			sprintf(heading,
				"#stn,type,date,%s(%s)%s,values/day",
				value_label,
				units_display,
				accumulate_label );
		}
	}
	else
	if ( aggregate_level == weekly )
	{
		sprintf(heading,
			"#stn,type,date,%s(%s)%s,values/week",
			value_label,
			units_display,
			accumulate_label );
	}
	else
	if ( aggregate_level == monthly )
	{
		sprintf(heading,
			"#stn,type,date,%s(%s)%s,values/month",
			value_label,
			units_display,
			accumulate_label );
	}
	else
	if ( aggregate_level == annually )
	{
		sprintf(heading,
			"#stn,type,date,%s(%s)%s,values/year",
			value_label,
			units_display,
			accumulate_label );
	}

	if ( output_filename && *output_filename )
		sprintf( output_process, ">> %s", output_filename );
	else
		*output_process = '\0';

	sprintf( sys_string,
		 "(echo \"%s\"						 ;"
		 "get_folder_data	application=%s			  "
		 "			folder=%s			  "
		 "			select='%s'			  "
		 "			where=\"%s\"			  "
		 "			quick=yes			 |"
		 "tolower.e						 |"
		 "%s							 |"
		 "%s							 |"
		 "tr '%c' ','						 |"
		 "toupper_to_delimiter.e ','				 |"
		 "%s							 |"
		 "%s							 |"
		 "%s							 |"
		 "%s							 |"
		 "sort -f						 |"
		 "cat) | %s %s						  ",
		 heading,
		 application_name,
		 SOURCE_FOLDER,
		 select_list_string,
		 where_clause,
		 zulu_process,
		 sort_process,
		 FOLDER_DATA_DELIMITER,
		 aggregation_process,
		 units_converted_process,
		 accumulation_process,
		 trim_time_process,
		 format_process,
		 output_process );

	return strdup( sys_string );

} /* get_sys_string() */

