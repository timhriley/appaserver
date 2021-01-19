/* ------------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/midnight_measurements.c	*/
/* ------------------------------------------------------	*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------	*/

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
#include "measurement.h"
#include "process.h"
#include "hydrology_library.h"
#include "boolean.h"
#include "application.h"

/* Constants */
/* --------- */
#define FILENAME_STEM		"midnight"

/* Prototypes */
/* ---------- */
void midnight_measurements_gracechart(
			LIST *measurement_list );

void midnight_measurements_spreadsheet(
			char *application_name,
			char *station,
			char *begin_date,
			char *end_date,
			char *document_root_directory,
			LIST *measurement_list );

/* Returns static memory */
/* --------------------- */
char *midnight_measurements_where(
			char *station,
			char *datatype,
			char *begin_date,
			char *end_date );

int main( int argc, char **argv )
{
	char *application_name;
	char *station;
	char *datatype;
	char *begin_date;
	char *end_date;
	char *process;
	char *output_medium;
	char buffer[ 128 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf(stderr, 
"Usage: %s process station datatype begin_date end_date output_medium\n",
			argv[ 0 ] );

		fprintf(stderr,
"\nNote: Output medium in {table,spreadsheet,chart,stdout}\n" );

		exit ( 1 );
	}

	process = argv[ 1 ];
	station = argv[ 2 ];
	datatype = argv[ 3 ];
	begin_date = argv[ 4 ];
	end_date = argv[ 5 ];
	output_medium = argv[ 6 ];

	if ( !*output_medium || strcmp( output_medium, "output_medium" ) == 0 )
		output_medium = "table";

	appaserver_parameter_file =
		appaserver_parameter_file_new();

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf( "<h1>%s<br></h1>\n",
			format_initial_capital(
				buffer,
				process ) );

		printf( "<h2>\n" );
		fflush( stdout );
		if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
		fflush( stdout );
		printf( "</h2>\n" );

	}

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
		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		hydrology_library_output_station_table(
			application_name,
			station );

		measurement_list_table_display(
			measurement_system_list(
				measurement_sys_string(
					midnight_measurements_where(
						station,
						datatype,
						begin_date,
						end_date ) ) ) );
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		midnight_measurements_spreadsheet(
			application_name,
			station,
			begin_date,
			end_date,
			appaserver_parameter_file->
				document_root,
			measurement_system_list(
				measurement_sys_string(
					midnight_measurements_where(
						station,
						datatype,
						begin_date,
						end_date ) ) ) );
	}
	else
	if ( strcmp( output_medium, "chart" ) == 0 )
	{
		printf( "<h3>Still in progress.</h3>\n" );
/*
		midnight_measurements_gracechart(
			station,
			datatype,
			begin_date,
			end_date );
*/
	}
	else
	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		hydrology_library_output_station_text_filename(
			"stdout"
				/* output_filename */,
			application_name,
			station,
			0 /* not with_zap_file */ );

		measurement_list_stdout(
			measurement_system_list(
				measurement_sys_string(
					midnight_measurements_where(
						station,
						datatype,
						begin_date,
						end_date ) ) ) );
	}
	else
	{
		fprintf(stderr,
			"Error in %s: invalid output_medium.\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_increment_execution_count(
		application_name,
		process,
		(char *)0 );

	if ( strcmp( output_medium, "stdout" ) != 0 )
		document_close();

	return 0;
}

void midnight_measurements_spreadsheet(
			char *application_name,
			char *station,
			char *begin_date,
			char *end_date,
			char *document_root_directory,
			LIST *measurement_list )
{
	char *output_pipename;
	char *ftp_filename;
	pid_t process_id = getpid();
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char buffer[ 256 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	appaserver_link_file->begin_date_string = begin_date;
	appaserver_link_file->end_date_string = end_date;

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

	hydrology_library_output_station_text_filename(
		output_pipename,
		application_name,
		station,
		0 /* not with_zap_file */ );

	sprintf( sys_string,
		 "tr '|' ',' >> %s",
		 output_pipename );

	output_pipe = popen( sys_string, "w" );

	fprintf(output_pipe,
		"%s\n",
		format_initial_capital(
			buffer,
			MEASUREMENT_SELECT_LIST ) );

	measurement_list_output_pipe(
		output_pipe,
		measurement_list );

	pclose( output_pipe );

	appaserver_library_output_ftp_prompt(
		ftp_filename,
		TRANSMIT_PROMPT,
		(char *)0 /* target */,
		(char *)0 /* application_type */ );
}

char *midnight_measurements_where(
			char *station,
			char *datatype,
			char *begin_date,
			char *end_date )
{
	static char where[ 512 ];

	sprintf(where, 
		"station = '%s'					"
		"and datatype = '%s' 				"
		"and measurement_date between '%s' and '%s' and	"
		"measurement_time = '0000' 			",
		station,
		datatype,
		begin_date,
		end_date );

	return where;
}

#ifdef NOT_DEFINED
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
			application_get_relative_source_directory(
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
			 "measurement_convert_units.e %s %s %s %d '^' %d",
			 application_name,
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
	results = system( sys_string );

	chart_email_command_line =
			application_get_chart_email_command_line(
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
#endif
