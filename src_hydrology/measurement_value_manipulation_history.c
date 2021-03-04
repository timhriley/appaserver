/* ----------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_hydrology/measurement_value_manipulation_history.c */
/* ----------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			   */
/* ----------------------------------------------------------------------- */

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "measurement_update_parameter.h"
#include "dictionary.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "measurement_backup.h"
#include "query.h"
#include "appaserver_parameter_file.h"
#include "dictionary_appaserver.h"
#include "html_table.h"
#include "grace.h"
#include "application.h"
#include "environ.h"
#include "list_usage.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "hydrology_library.h"
#include "application_constants.h"
#include "appaserver_link_file.h"
#include "google_chart.h"
#include "measurement.h"

/* Constants */
/* --------- */
#define DEFAULT_OUTPUT_MEDIUM			"table"
#define GRACE_TICKLABEL_ANGLE			90

/* Structures */
/* ---------- */

	
/* Prototypes */
/* ---------- */
LIST *manipulate_history_measurement_backup_list(
			char *station,
			char *datatype,
			char *measurement_date,
			char *measurement_time );

/* -------------------------------------------- */
/* Sets measurement and measurement_backup_list */
/* -------------------------------------------- */
MEASUREMENT_STRUCTURE *manipulation_history_structure_fetch(
			char *station,
			char *datatype,
			char *measurement_date,
			char *measurement_time );

boolean measurement_manipulation_output_table(
			char *station,
			char *datatype,
			char *measurement_date,
			char *measurement_time );

boolean measurement_manipulation_output_transmit(
			FILE *output_pipe,
			char *station,
			char *datatype,
			char *measurement_date,
			char *measurement_time );

/*
void measurement_manipulation_output_googlechart(
			char *application_name,
			char *station,
			char *datatype,
			char *measurement_date,
			char *measurement_time );

void measurement_manipulation_output_gracechart(
			char *application_name,
			char *station,
			char *datatype,
			char *measurement_date,
			char *measurement_time );

*/

int main( int argc, char **argv )
{
	char *application_name;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *process_name;
	char *station;
	char *datatype;
	char *measurement_date;
	char *measurement_time;
	char *output_medium;
	boolean success = 0;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf(stderr,
"Usage: %s process station datatype measurement_date measurement_time output_medium\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	station = argv[ 2 ];
	datatype = argv[ 3 ];
	measurement_date = argv[ 4 ];
	measurement_time = argv[ 5 ];
	output_medium = argv[ 6 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		char buffer[ 128 ];

		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf( "<h1>%s</h1>\n",
			format_initial_capital(
				buffer,
				process_name ) );

		printf( "<h2>\n" );
		fflush( stdout );
		if ( system( timlib_system_date_string() ) ){};
		fflush( stdout );
		printf( "</h2>\n" );
	
	}

	if ( !*output_medium
	||   strcmp( output_medium, "output_medium" ) == 0 )
	{
		output_medium = DEFAULT_OUTPUT_MEDIUM;
	}

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		success =
			measurement_manipulation_output_table(
				station,
				datatype,
				measurement_date,
				measurement_time );
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		char *ftp_filename;
		char *output_pipename;
		pid_t process_id = getpid();
		FILE *output_pipe;
		char sys_string[ 1024 ];
		APPASERVER_LINK_FILE *appaserver_link_file;

		appaserver_link_file =
			appaserver_link_file_new(
				application_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->
					document_root,
				process_name /* filename_stem */,
				application_name,
				process_id,
				(char *)0 /* session */,
				"csv" );

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

		sprintf(sys_string,
			"cat >> %s",
			output_pipename );

		output_pipe = popen( sys_string, "w" );

		success =
			measurement_manipulation_output_transmit(
				output_pipe,
				station,
				datatype,
				measurement_date,
				measurement_time );

		pclose( output_pipe );

		if ( success )
		{
			appaserver_library_output_ftp_prompt(
				ftp_filename,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
		}
	}
	else
	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		FILE *output_pipe;
		char sys_string[ 512 ];

		sprintf(sys_string,
		 	"cat" );

		output_pipe = popen( sys_string, "w" );

		measurement_manipulation_output_transmit(
			output_pipe,
			station,
			datatype,
			measurement_date,
			measurement_time );

		pclose( output_pipe );
	}
	else
	if ( strcmp( output_medium, "gracechart" ) == 0 )
	{
/*
		measurement_manipulation_output_gracechart(
			application_name,
			station,
			datatype,
			measurement_date,
			measurement_time );
*/
	}
	else
	if ( strcmp( output_medium, "googlechart" ) == 0 )
	{
/*
		measurement_manipulation_output_googlechart(
			application_name,
			station,
			datatype,
			measurement_date,
			measurement_time );
*/
	}

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		if ( !success )
		{
			printf(
			"<h3>An internal error occurrred. Check log.</h3>\n" );
		}

		document_close();
	}

	process_increment_execution_count(
		application_name,
		process_name,
		appaserver_parameter_file_get_dbms() );

	exit( 0 );
}

boolean measurement_manipulation_output_transmit(
			FILE *output_pipe,
			char *station,
			char *datatype,
			char *measurement_date,
			char *measurement_time )
{
	MEASUREMENT_STRUCTURE *measurement_structure;

	if ( ! ( measurement_structure =
			/* -------------------------------------------- */
			/* Sets measurement and measurement_backup_list */
			/* -------------------------------------------- */
			manipulation_history_structure_fetch(
				station,
				datatype,
				measurement_date,
				measurement_time ) ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: manipulation_history_structure_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return 0;
	}

	list_set(	measurement_structure->measurement_backup_list,
			measurement_convert_measurement_backup(
				measurement_structure->measurement ) );

	measurement_backup_list_pipe_output(
		output_pipe,
		measurement_structure->
			measurement_backup_list );

	return 1;
}

boolean measurement_manipulation_output_table(
			char *station,
			char *datatype,
			char *measurement_date,
			char *measurement_time )
{
	MEASUREMENT_STRUCTURE *measurement_structure;

	if ( ! ( measurement_structure =
			/* -------------------------------------------- */
			/* Sets measurement and measurement_backup_list */
			/* -------------------------------------------- */
			manipulation_history_structure_fetch(
				station,
				datatype,
				measurement_date,
				measurement_time ) ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: manipulation_history_structure_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return 0;
	}

	list_set(	measurement_structure->measurement_backup_list,
			measurement_convert_measurement_backup(
				measurement_structure->measurement ) );

	measurement_backup_list_table_display(
		measurement_structure->measurement_backup_list );

	return 1;
}

MEASUREMENT_STRUCTURE *manipulation_history_structure_fetch(
			char *station,
			char *datatype,
			char *measurement_date,
			char *measurement_time )
{
	MEASUREMENT_STRUCTURE *measurement_structure;
	MEASUREMENT_STRUCTURE *m;

	m = measurement_structure = measurement_structure_calloc();

	if ( ! ( m->measurement =
			measurement_fetch(
				station,
				datatype,
				measurement_date,
				measurement_time ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: measurement_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (MEASUREMENT_STRUCTURE *)0;
	}

	m->measurement_backup_list =
		manipulate_history_measurement_backup_list(
			station,
			datatype,
			measurement_date,
			measurement_time );

	return measurement_structure;
}

LIST *manipulate_history_measurement_backup_list(
			char *station,
			char *datatype,
			char *measurement_date,
			char *measurement_time )
{
	return measurement_backup_system_list(
		measurement_backup_sys_string(
			measurement_primary_where(
				station,
				datatype,
				measurement_date,
				measurement_time ) ) );
}

