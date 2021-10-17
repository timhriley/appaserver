/* ---------------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/on_the_hour_measurements.c	*/
/* ---------------------------------------------------------	*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ---------------------------------------------------------	*/

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
#include "appaserver_link.h"
#include "application.h"

/* Constants */
/* --------- */
#define FILENAME_STEM		"hour"

/* Prototypes */
/* ---------- */
void on_the_hour_measurements_gracechart(
			LIST *measurement_list );

void on_the_hour_measurements_spreadsheet(
			char *application_name,
			char *station,
			char *begin_date,
			char *end_date,
			char *document_root_directory,
			LIST *measurement_list );

/* Returns static memory */
/* --------------------- */
char *on_the_hour_measurements_where(
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
					on_the_hour_measurements_where(
						station,
						datatype,
						begin_date,
						end_date ) ) ) );
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		on_the_hour_measurements_spreadsheet(
			application_name,
			station,
			begin_date,
			end_date,
			appaserver_parameter_file->
				document_root,
			measurement_system_list(
				measurement_sys_string(
					on_the_hour_measurements_where(
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
		on_the_hour_measurements_gracechart(
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
					on_the_hour_measurements_where(
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

void on_the_hour_measurements_spreadsheet(
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
	APPASERVER_LINK *appaserver_link;

	appaserver_link =
		appaserver_link_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM,
			application_name,
			process_id,
			(char *)0 /* session */,
			begin_date,
			end_date,
			"csv" );

	output_pipename = appaserver_link->output->filename;
	ftp_filename = appaserver_link->prompt->filename;

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

char *on_the_hour_measurements_where(
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
		"measurement_time like '%c00' 			",
		station,
		datatype,
		begin_date,
		end_date,
		'%' );

	return where;
}
