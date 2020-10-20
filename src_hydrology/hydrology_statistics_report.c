/* ---------------------------------------------------	*/
/* hydrology_statistics_report.c			*/
/* ---------------------------------------------------	*/
/*							*/
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
#include "list.h"
#include "environ.h"
#include "date.h"
#include "query_attribute_statistics_list.h"
#include "statistics_weighted.h"
#include "validation_level.h"
#include "process.h"
#include "application.h"
#include "session.h"
#include "boolean.h"
#include "hydrology_library.h"
#include "datatype.h"

/* Constants */
/* --------- */
#define PROCESS_NAME					"statistics_report"
#define MEASUREMENT_DATE_PIECE				0
#define MEASUREMENT_TIME_PIECE				1
#define MEASUREMENT_VALUE_PIECE				2
#define FOLDER_NAME					"measurement"

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *station, *datatype, *begin_date, *end_date;
	char *validation_level_string;
	enum validation_level validation_level;
	char measurement_date_string[ 128 ];
	char measurement_time_string[ 128 ];
	char measurement_value_string[ 128 ];
	char expected_count_string[ 128 ];
	char sys_string[ 4096 ];
	char input_buffer[ 1024 ];
	char piece_buffer[ 256 ];
	char label[ 256 ];
	char *measurement_table_name;
	FILE *input_pipe;
	FILE *statistics_output_pipe;
	FILE *date_min_max_output_pipe;
	FILE *input_file;
	char statistics_temp_file[ 128 ];
	char date_min_max_temp_file[ 128 ];
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char where_clause[ 4096 ];
	char *validated_where_clause;
	int process_id = getpid();
	QUERY_ATTRIBUTE_STATISTICS_LIST *query_attribute_statistics_list;
	QUERY_ATTRIBUTE_STATISTICS
		*query_attribute_statistics_measurement_value;
	QUERY_ATTRIBUTE_STATISTICS
		*query_attribute_statistics_measurement_date;
	char *units_converted;
	char *units;
	char *units_display;
	char units_converted_process[ 1024 ];
	char *database_string = {0};
	boolean bar_chart;
	boolean still_getting_null_values = 1;
	boolean bypass_data_collection_frequency;
				
	if ( argc != 11 )
	{
		fprintf( stderr, 
"Usage: %s application ignored ignored ignored station datatype begin_date end_date validation_level units_converted\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	/* session = argv[ 2 ]; */
	/* login_name = argv[ 3 ]; */
	/* role_name = argv[ 4 ]; */
	station = argv[ 5 ];
	datatype = argv[ 6 ];
	begin_date = argv[ 7 ];
	end_date = argv[ 8 ];
	validation_level_string = argv[ 9 ];
	units_converted = argv[ 10 ];

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
					station,
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

	units = hydrology_library_get_units_string(
				&bar_chart,
				application_name,
				datatype );

	validation_level =
		validation_level_get_validation_level(
			validation_level_string);

	measurement_table_name =
		get_table_name( application_name, FOLDER_NAME );

	sprintf( where_clause, 
		 "    %s.station = '%s'				"
		 "and %s.datatype = '%s' 			"
		 "and measurement_date between '%s' and '%s'	",
		 measurement_table_name,
		 station,
		 measurement_table_name,
		 datatype,
		 begin_date,
		 end_date );

	validated_where_clause =
		/* ----------------------- */
		/* Returns program memory. */
		/* ----------------------- */
		hydrology_library_provisional_where(
			validation_level );

	strcat( where_clause, validated_where_clause );

	bypass_data_collection_frequency =
		datatype_get_bypass_data_collection_frequency(
				application_name,
				station,
				datatype );

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	printf( "<h2>Statistics Report\n" );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ){};
	printf( "</h2>\n" );
	fflush( stdout );

	if ( units_converted
	&&   *units_converted
	&&   strcmp( units_converted, "units_converted" ) != 0 )
	{
		sprintf( units_converted_process,
			 "measurement_convert_units.e %s %s %s 2 '^' 3",
			 application_name,
			 units,
			 units_converted );
	}
	else
	{
		strcpy( units_converted_process, "cat" );
	}

	units_display =
		hydrology_library_get_datatype_units_display(
			application_name,
			datatype,
			units,
			units_converted,
			aggregate_statistic_none );

	if ( !units_display )
	{
		printf( "%s\n",
			hydrology_library_get_output_invalid_units_error(
				datatype,
				units,
				units_converted ) );
		document_close();
		exit( 0 );
	}

	if ( !bypass_data_collection_frequency )
	{
		sprintf(sys_string,
	"echo \"select	measurement_date,				 "
	"		measurement_time,				 "
	"		measurement_value,station			 "
	"	from %s							 "
	"	where %s						 "
	"	order by measurement_date;\"				|"
	"sql_quick.e '%c'						|"
	"%s								|"
	"pad_missing_times.e '%c' 0,1,2 real_time %s 0000 %s 2359 0 '%s'|"
	"cat								 ",
			measurement_table_name,
			where_clause,
			FOLDER_DATA_DELIMITER,
			units_converted_process,
			FOLDER_DATA_DELIMITER,
			begin_date,
			end_date,
			hydrology_library_get_expected_count_list_string(
				application_name, station, datatype, '|' ) );
	}
	else
	{
		sprintf(sys_string,
	"echo \"select	measurement_date,				 "
	"		measurement_time,				 "
	"		measurement_value,station			 "
	"	from %s							 "
	"	where %s						 "
	"	order by measurement_date;\"				|"
	"sql_quick.e '%c'						|"
	"%s								|"
	"cat								 ",
			measurement_table_name,
			where_clause,
			FOLDER_DATA_DELIMITER,
			units_converted_process );
	}

	input_pipe = popen( sys_string, "r" );

	sprintf(statistics_temp_file,
		"/tmp/measurement_statistics_%d",
		process_id );

	sprintf(sys_string,
		"statistics_weighted.e '%c' 0 1 > %s",
		FOLDER_DATA_DELIMITER,
		statistics_temp_file );

	statistics_output_pipe = popen( sys_string, "w" );

	sprintf(date_min_max_temp_file,
		"/tmp/date_min_max_%d",
		process_id );

	sprintf(sys_string,
		"date_min_max.e 0 '%c' '%s' > %s",
		FOLDER_DATA_DELIMITER,
		appaserver_parameter_file->database_management_system,
		date_min_max_temp_file );

	date_min_max_output_pipe = popen( sys_string, "w" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	measurement_date_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			MEASUREMENT_DATE_PIECE );

		piece(	measurement_time_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			MEASUREMENT_TIME_PIECE );

		piece(	measurement_value_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			MEASUREMENT_VALUE_PIECE );

		if ( still_getting_null_values )
		{
			if ( *measurement_value_string
			&&   timlib_strcmp(	measurement_value_string,
						"null" ) != 0 )
			{
				still_getting_null_values = 0;
			}
			else
			{
				continue;
			}
		}

		fprintf( statistics_output_pipe,
			 "%s%c%s\n",
			 measurement_value_string,
			 FOLDER_DATA_DELIMITER,
			 expected_count_string );

		fprintf( date_min_max_output_pipe,
			 "%s\n",
			 measurement_date_string );
	}
	pclose( input_pipe );
	pclose( statistics_output_pipe );
	pclose( date_min_max_output_pipe );

	/* Use query_attribute_statistics_list for the output */
	/* -------------------------------------------------- */
	query_attribute_statistics_list =
		query_attribute_statistics_list_new(
					application_name,
					FOLDER_NAME );

	query_attribute_statistics_list->list = list_new_list();

	query_attribute_statistics_measurement_value =
		query_attribute_statistics_new(
					"measurement_value",
					"float",
					0 /* input_piece */ );

	query_attribute_statistics_measurement_date =
		query_attribute_statistics_new(
					"measurement_date",
					"date",
					0 /* input_piece */ );

	list_append_pointer(	query_attribute_statistics_list->list,
				query_attribute_statistics_measurement_value );

	list_append_pointer(	query_attribute_statistics_list->list,
				query_attribute_statistics_measurement_date );

	/* Get statistics */
	/* -------------- */
	input_file = fopen( statistics_temp_file, "r" );

	while( get_line( input_buffer, input_file ) )
	{
		piece( label, ':', input_buffer, 0 );

		if ( !piece( piece_buffer, ':', input_buffer, 1 ) )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot piece(1) in (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );
			exit( 1 );
		}

		if ( strcmp( label, STATISTICS_WEIGHTED_AVERAGE ) == 0 )
		{
			query_attribute_statistics_measurement_value->
				average = atof( piece_buffer );
		}
		else
		if ( strcmp( label, STATISTICS_WEIGHTED_SUM ) == 0 )
		{
			query_attribute_statistics_measurement_value->
				sum = atof( piece_buffer );
		}
		else
		if ( strcmp(
			label,
			STATISTICS_WEIGHTED_STANDARD_DEVIATION ) == 0 )
		{
			query_attribute_statistics_measurement_value->
				standard_deviation = atof( piece_buffer );
		}
		else
		if ( strcmp(
			label,
			STATISTICS_WEIGHTED_COEFFICIENT_OF_VARIATION ) == 0 )
		{
			query_attribute_statistics_measurement_value->
				coefficient_of_variation = atof( piece_buffer );
		}
		else
		if ( strcmp( label, STATISTICS_WEIGHTED_MINIMUM ) == 0 )
		{
			query_attribute_statistics_measurement_value->
				minimum = atof( piece_buffer );
		}
		else
		if ( strcmp( label, STATISTICS_WEIGHTED_MEDIAN ) == 0 )
		{
			query_attribute_statistics_measurement_value->
				median = atof( piece_buffer );
		}
		else
		if ( strcmp( label, STATISTICS_WEIGHTED_MAXIMUM ) == 0 )
		{
			query_attribute_statistics_measurement_value->
				maximum = atof( piece_buffer );
		}
		if ( strcmp( label, STATISTICS_WEIGHTED_RANGE ) == 0 )
		{
			query_attribute_statistics_measurement_value->
				range = atof( piece_buffer );
		}
		else
		if ( strcmp( label, STATISTICS_WEIGHTED_COUNT ) == 0 )
		{
			query_attribute_statistics_measurement_value->
				count = atoi( piece_buffer );
		}
		else
		if ( strcmp( label, STATISTICS_WEIGHTED_PERCENT_MISSING ) == 0 )
		{
			query_attribute_statistics_measurement_value->
				percent_missing = atof( piece_buffer );
		}
	}
	fclose( input_file );

	/* Get date min/max */
	/* ---------------- */
	input_file = fopen( date_min_max_temp_file, "r" );

	while( get_line( input_buffer, input_file ) )
	{
		piece( label, ':', input_buffer, 0 );

		if ( !piece( piece_buffer, ':', input_buffer, 1 ) )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: cannot piece(1) in (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );
			exit( 1 );
		}

		if ( strcmp( label, DATE_MINIMUM ) == 0 )
		{
			query_attribute_statistics_measurement_date->
				begin_date = strdup( piece_buffer );
		}
		else
		if ( strcmp( label, DATE_MAXIMUM ) == 0 )
		{
			query_attribute_statistics_measurement_date->
				end_date = strdup( piece_buffer );
		}
	}
	fclose( input_file );

	query_attribute_statistics_measurement_date->count = -1;

	/* Change the where_clause for output */
	/* ---------------------------------- */
	if ( timlib_strcmp( validated_where_clause, " and 1 = 1" ) == 0 )
	{
		validated_where_clause = "";
	}

	sprintf( where_clause,
"station = %s and datatype = %s and measurement_date between %s and %s %s<hr>\n",
		station,
		datatype,
		begin_date,
		end_date,
		validated_where_clause );

	query_attribute_statistics_list_set_units_string(
		query_attribute_statistics_list->list,
		"measurement_value",
		units_display );

	query_attribute_statistics_list_output_table(
					FOLDER_NAME,
					where_clause,
					query_attribute_statistics_list->list,
					application_name );

	printf(
"<p>Note: If a station has measurements stored as both real time and daily values, then the weight given to each real time measurement is proportionally less.\n" );

	sprintf( sys_string, "/bin/rm -f %s", statistics_temp_file );
	if ( system( sys_string ) ){};

	sprintf( sys_string, "/bin/rm -f %s", date_min_max_temp_file );
	if ( system( sys_string ) ){};

	document_close();
	process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
}

