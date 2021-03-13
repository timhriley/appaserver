/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/hydrology_histogram.c	*/
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
#include "grace.h"
#include "column.h"
#include "validation_level.h"
#include "hydrology_library.h"
#include "process.h"
#include "session.h"
#include "boolean.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "application.h"

/* Constants */
/* --------- */
#define PROCESS_NAME					"histogram"
#define FOLDER_NAME					"measurement"
#define REAL_TIME_SELECT_LIST "measurement_value,station"
#define AGGREGATION_SELECT_LIST "measurement_date,measurement_time,measurement_value,station"

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *station, *datatype, *begin_date, *end_date;
	char grace_begin_date_string[ 16 ] = {0};
	char grace_end_date_string[ 16 ] = {0};
	char *validation_level_string;
	enum validation_level validation_level;
	char sys_string[ 4096 ];
	char aggregate_process[ 1024 ];
	char input_buffer[ 1024 ];
	char *measurement_table_name;
	FILE *input_pipe;
	FILE *histogram_pipe;
	FILE *input_file;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char where_clause[ 4096 ];
	char *validated_where_clause;
	char *session;
	char *aggregate_level_string;
	char *aggregate_statistic_string;
	enum aggregate_level aggregate_level;
	enum aggregate_statistic aggregate_statistic;
	char grace_histogram_filename[ 128 ];
	char ftp_output_filename[ 128 ];
	char ftp_agr_filename[ 128 ];
	char output_filename[ 128 ];
	char initial_capital_buffer[ 128 ];
	char title[ 128 ];
	char sub_title[ 128 ];
	char *select_list;
	char legend[ 128 ];
	char *email_address;
	char *chart_email_command_line;
	char *units_converted;
	char *units;
	char units_converted_process[ 1024 ];
	boolean bar_chart;
	char *units_display;
	int measurement_value_piece;
	int station_piece;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 12 )
	{
		fprintf( stderr, 
"Usage: %s application session station datatype begin_date end_date aggregate_level aggregate_statistic validation_level email_address units_converted\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session = argv[ 2 ];
	station = argv[ 3 ];
	datatype = argv[ 4 ];
	begin_date = argv[ 5 ];
	end_date = argv[ 6 ];
	aggregate_level_string = argv[ 7 ];
	aggregate_statistic_string = argv[ 8 ];
	validation_level_string = argv[ 9 ];
	email_address = argv[ 10 ];
	units_converted = argv[ 11 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

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

	validation_level =
		validation_level_get_validation_level( validation_level_string);

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
		hydrology_library_provisional_where(
			validation_level );

	strcat( where_clause, validated_where_clause );

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

	printf( "<h1>Histogram</h1>\n" );
	fflush( stdout );

	strcpy( title, "Histogram" );

	aggregate_level =
		aggregate_level_get_aggregate_level( aggregate_level_string );

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
			aggregate_statistic_string,
			aggregate_level );

	if ( aggregate_statistic == aggregate_statistic_none )
	{
		aggregate_statistic =
			based_on_datatype_get_aggregate_statistic(
				application_name,
				datatype,
				aggregate_statistic );
	}

	strcpy( grace_begin_date_string,
		begin_date );
	strcpy( grace_end_date_string,
		end_date );

	if ( aggregate_level == real_time )
	{
		strcpy( aggregate_process, "cat" );
		select_list = REAL_TIME_SELECT_LIST;
		measurement_value_piece = 0;
		station_piece = 1;

		sprintf(sub_title,
			"%s/%s from %s to %s",
			station,
			format_initial_capital(	initial_capital_buffer,
						datatype ),
			grace_begin_date_string,
			grace_end_date_string );
	}
	else
	{
		sprintf( aggregate_process,
		"real_time2aggregate_value.e %s 0 1 2 '%c' '%s' n %s |"
		"piece.e '%c' 2					     |"
		"cat						      ",
		aggregate_statistic_get_string( aggregate_statistic ),
		FOLDER_DATA_DELIMITER,
		aggregate_level_get_string( aggregate_level ),
		end_date,
		FOLDER_DATA_DELIMITER );

		select_list = AGGREGATION_SELECT_LIST;
		measurement_value_piece = 2;
		station_piece = 3;

		sprintf(sub_title,
			"%s %s %s from %s to %s",
			station,
			aggregate_level_get_string(
					aggregate_level ),
			aggregate_statistic_get_string(
					aggregate_statistic ),
			grace_begin_date_string,
			grace_end_date_string );

		strcpy( sub_title,
			format_initial_capital(
				initial_capital_buffer,
				sub_title ) );
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
		printf( "%s\n",
			hydrology_library_get_output_invalid_units_error(
				datatype,
				units,
				units_converted ) );
		document_close();
		exit( 0 );
	}

	if ( units_converted
	&&   *units_converted
	&&   strcmp( units_converted, "units_converted" ) != 0 )
	{
		sprintf( units_converted_process,
			 "measurement_convert_units.e %s %s %d '^' %d",
			 units,
			 units_converted,
			 measurement_value_piece,
			 station_piece );
	}
	else
	{
		strcpy( units_converted_process, "cat" );
	}

	sprintf(sys_string,
		"echo \"select %s				 "
		"	from %s					 "
		"	where %s;\"				|"
		"sql_quick.e '%c'				|"
		"%s						|"
		"%s						 ",
		select_list,
		measurement_table_name,
		where_clause,
		FOLDER_DATA_DELIMITER,
		aggregate_process,
		units_converted_process );

	input_pipe = popen( sys_string, "r" );

	sprintf(	legend,
			"%s (%s)",
			datatype,
			units_display );

	strcpy(	legend,
		format_initial_capital(
			initial_capital_buffer,
			legend ) );

	sprintf( grace_histogram_filename,
		 "%s/appaserver/%s/data/grace_histogram_%d.dat",
		 appaserver_parameter_file->document_root,
		 application_name,
		 getpid() );

	sprintf(	sys_string,
			"grace_histogram.e %s %s \"%s\" \"%s\" \"%s\" > %s",
			application_name,
			session,
			title,
			sub_title,
			legend,
			grace_histogram_filename );

	histogram_pipe = popen( sys_string, "w" );
	while( get_line( input_buffer, input_pipe ) )
	{
		fprintf( histogram_pipe, "%s\n", input_buffer );
	}
	pclose( input_pipe );
	pclose( histogram_pipe );

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
		grace_begin_date_string,
		grace_end_date_string,
		validated_where_clause );

	input_file = fopen( grace_histogram_filename, "r" );
	if ( !input_file )
	{
		fprintf( stderr,
			 "%s/%s(): cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 grace_histogram_filename );
		exit( 1 );
	}

	if ( !get_line( input_buffer, input_file ) )
	{
		printf( "<h3>Warning: no records were selected.</h3>\n" );
		document_close();
		exit( 0 );
	}

	column( ftp_agr_filename, 0, input_buffer );
	column( ftp_output_filename, 1, input_buffer );
	column( output_filename, 2, input_buffer );

	fclose( input_file );
	sprintf( sys_string, "rm -f %s", grace_histogram_filename );
	if ( system( sys_string ) ){};

	chart_email_command_line =
			application_chart_email_command_line(
				application_name );

	if ( !*chart_email_command_line
	||   *chart_email_command_line == '#'
	||   strcmp( email_address, "email_address" ) == 0 )
	{
		grace_output_graph_window(
				application_name,
				ftp_output_filename,
				ftp_agr_filename,
				appaserver_parameter_file->
					appaserver_mount_point,
				0 /* not with_document_output */,
				(char *)0 /* where_clause */ );
	}
	else
	{
		grace_email_graph(
				application_name,
				email_address,
				chart_email_command_line,
				output_filename,
				appaserver_parameter_file->
					appaserver_mount_point,
				0 /* not with_document_output */,
				(char *)0 /* where_clause */ );
	}

	document_close();
	process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

