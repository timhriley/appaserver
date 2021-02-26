/* ---------------------------------------------------	*/
/* src_wadingbird/bird_observation.c			*/
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
#include "column.h"
#include "date.h"
#include "wadingbird_library.h"
#include "environ.h"
#include "date_convert.h"
#include "application.h"
#include "process.h"
#include "appaserver_link_file.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */
#define DELIMITER	'^'

#define HEADING_TEMPLATE \
"SRF_Date,%s,%s,common_name,L/R,bird_count,bird_estimation,cell_number,hydropattern,basin,transect"

/* Prototypes */
/* ---------- */
char *get_heading_string(	char *heading_template,
				char *location_format );

char *get_input_sys_string(	char *application_name,
				char *initial_takeoff_date_list_string,
				char *common_name_list_string,
				char *location_format );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char buffer[ 128 ];
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *initial_takeoff_date_list_string;
	char *common_name_list_string;
	char *location_format;
	char *output_medium;
	APPASERVER_LINK_FILE *appaserver_link_file;

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s application process initial_takeoff_date common_name location_format output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	initial_takeoff_date_list_string = argv[ 3 ];
	common_name_list_string = argv[ 4 ];
	location_format = argv[ 5 ];
	output_medium = argv[ 6 ];

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
	add_local_bin_to_path();
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

	printf( "<h2>%s for %s<br>\n",
		format_initial_capital( buffer, process_name ),
		initial_takeoff_date_list_string );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( !*initial_takeoff_date_list_string
	||   strcmp( initial_takeoff_date_list_string,
		     "initial_takeoff_date" ) == 0 )
	{
		printf(
	"<h3>Please select a systematic reconnaissance flight.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( strcmp( output_medium, "table" ) == 0
	||   strcmp( output_medium, "output_medium" ) == 0 )
	{
		char *input_sys_string;
		FILE *input_pipe;
		char output_sys_string[ 1024 ];
		FILE *output_pipe;
		char input_buffer[ 1024 ];

		input_sys_string =
			get_input_sys_string(
				application_name,
				initial_takeoff_date_list_string,
				common_name_list_string,
				location_format );

		input_pipe = popen( input_sys_string, "r" );

		sprintf( output_sys_string,
			 "sed 's/ /\\&nbsp;/1'		|"
			 "sed 's/ /\\&nbsp;/1'		|"
			 "html_table.e '' '%s' '^' %s	 ",
			 get_heading_string(	HEADING_TEMPLATE,
						location_format ),
"left,right,right,left,left,right,right,right,left,left,left" );

		output_pipe = popen( output_sys_string, "w" );

		while( get_line( input_buffer, input_pipe ) )
		{
			fprintf( output_pipe, "%s\n", input_buffer );
		}

		pclose( input_pipe );
		pclose( output_pipe );
	}
	else
	if ( strcmp( output_medium, "text_file" ) == 0 )
	{
		FILE *output_file;
		char *output_filename;
		char *ftp_filename;
		APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
		int process_id = getpid();
		char *input_sys_string;
		FILE *input_pipe;
		char input_buffer[ 1024 ];
		char buffer[ 512 ];

		input_sys_string =
			get_input_sys_string(
				application_name,
				initial_takeoff_date_list_string,
				common_name_list_string,
				location_format );

		input_pipe = popen( input_sys_string, "r" );

		appaserver_parameter_file = appaserver_parameter_file_new();

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

/*
		sprintf( output_filename, 
			 OUTPUT_TEMPLATE,
			 appaserver_parameter_file->appaserver_mount_point,
			 application_name, 
			 process_id );
*/
	
		if ( ! ( output_file = fopen( output_filename, "w" ) ) )
		{
			printf(
			"<h3>ERROR: Cannot open output file %s</h3>\n",
				output_filename );
			document_close();
			exit( 0 );
		}
	
		fprintf( output_file,
			 "%s\n",
			 format_initial_capital(
				buffer,
				get_heading_string(
						HEADING_TEMPLATE,
						location_format ) ) );

		while( get_line( input_buffer, input_pipe ) )
		{
			search_replace_string(
				input_buffer,
				"^",
				"\",\"" );
			fprintf( output_file, "\"%s\"\n", input_buffer );
		}

		pclose( input_pipe );

		appaserver_library_output_ftp_prompt(
			ftp_filename,
	"&lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	}
	else
	if ( strcmp( output_medium, "chart" ) == 0
	||   strcmp( output_medium, "map" ) == 0 )
	{
		printf( "<h3>This function is not yet implemented.</h3>\n" );
/*
		output_chart(	application_name,
				appaserver_parameter_file->
					appaserver_mount_point,
				initial_takeoff_date,
				hydropattern_merge->cell_list );
*/
	}

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );

	exit( 0 );
} /* main() */

char *get_input_sys_string(
				char *application_name,
				char *initial_takeoff_date_list_string,
				char *common_name_list_string,
				char *location_format )
{
	static char sys_string[ 5120 ];
	char select[ 1024 ];
	char from[ 512 ];
	char order[ 512 ];
	char *systematic_reconnaissance_flight_table;
	char *bird_observation_table;
	char *cell_hydropattern_table;
	char *cell_table;
	char where[ 4096 ];
	char *where_ptr = where;
	char location_format_process[ 128 ];

	if ( strcmp( location_format, "UTM_NAD83" ) == 0 )
	{
		strcpy( location_format_process, "cat" );
	}
	else
	if ( strcmp( location_format, "WGS84_D_M.decimal" ) == 0 )
	{
		sprintf( location_format_process,
			 "location_convert.e UTM_NAD83 '%s' 1 2 '%c'",
			 location_format,
			 DELIMITER );
	}
	else
	if ( strcmp( location_format, "WGS84_D.decimal" ) == 0 )
	{
		sprintf( location_format_process,
			 "location_convert.e UTM_NAD83 '%s' 1 2 '%c'",
			 location_format,
			 DELIMITER );
	}
	else
	if ( strcmp( location_format, "location_format" ) == 0
	||   !*location_format )
	{
		strcpy( location_format_process, "cat" );
	}
	else
	{
		printf( "<h3>Error: invalid location format = (%s).</h3>\n",
			location_format );
		document_close();
		exit( 1 );
	}

	systematic_reconnaissance_flight_table =
		get_table_name( application_name,
				"systematic_reconnaissance_flight" );

	bird_observation_table =
		get_table_name( application_name, "bird_observation" );

	cell_hydropattern_table =
		get_table_name( application_name, "cell_hydropattern" );

	cell_table =
		get_table_name( application_name, "cell" );

	/* Select */
	/* ------ */
	sprintf( select,
		 "%s.initial_takeoff_date,		"
		 "%s.utm_easting,			"
		 "%s.utm_northing,			"
		 "%s.common_name,			"
		 "%s.left_right,			"
		 "%s.bird_count,			"
		 "%s.bird_estimation,			"
		 "%s.cell_number,			"
		 "%s.hydropattern,			"
		 "%s.basin,				"
		 "%s.transect_number			",
		 bird_observation_table,
		 bird_observation_table,
		 bird_observation_table,
		 bird_observation_table,
		 bird_observation_table,
		 bird_observation_table,
		 bird_observation_table,
		 bird_observation_table,
		 cell_hydropattern_table,
		 cell_table,
		 cell_table );

	/* From */
	/* ---- */
	sprintf( from,
		 "%s,%s,%s,%s",
		 systematic_reconnaissance_flight_table,
		 bird_observation_table,
		 cell_hydropattern_table,
		 cell_table );

	/* Where */
	/* ----- */
	where_ptr += sprintf( where_ptr, "1 = 1" );

	/* Join BIRD_OBSERVATION with SRF */
	/* ------------------------------ */
	where_ptr += sprintf(
		 where_ptr,
		 " and %s.initial_takeoff_date = %s.initial_takeoff_date",
		 bird_observation_table,
		 systematic_reconnaissance_flight_table );

	/* Join SRF with CELL_HYDROPATTERN */
	/* ------------------------------- */
	where_ptr += sprintf(
		 where_ptr,
		 " and %s.initial_takeoff_date = %s.initial_takeoff_date"
		 " and %s.preferred_hydropattern_seat = %s.left_right",
		 systematic_reconnaissance_flight_table,
		 cell_hydropattern_table,
		 systematic_reconnaissance_flight_table,
		 cell_hydropattern_table );

	/* Join BIRD_OBSERVATION with CELL */
	/* ------------------------------- */
	where_ptr += sprintf(
		 where_ptr,
		 " and %s.cell_number = %s.cell_number",
		 bird_observation_table,
		 cell_table );

	/* Join BIRD_OBSERVATION with CELL_HYDROPATTERN */
	/* -------------------------------------------- */
	where_ptr += sprintf(
		 where_ptr,
		 " and %s.initial_takeoff_date = %s.initial_takeoff_date"
		 " and %s.cell_number = %s.cell_number",
		 bird_observation_table,
		 cell_hydropattern_table,
		 bird_observation_table,
		 cell_hydropattern_table );

	/* Query Where */
	/* ----------- */
	where_ptr += sprintf(
		 where_ptr,
		 " and %s.initial_takeoff_date in (%s)",
		 bird_observation_table,
		 timlib_get_in_clause( initial_takeoff_date_list_string ) );

	if ( *common_name_list_string
	&&   strcmp( common_name_list_string, "common_name" ) != 0 )
	{
		where_ptr += sprintf(
		 	where_ptr,
		 	" and %s.common_name in (%s)",
		 	bird_observation_table,
		 	timlib_get_in_clause( common_name_list_string ) );
	}

	/* Order By */
	/* -------- */
	sprintf( order,
		 "%s.initial_takeoff_date,%s.utm_easting",
		 bird_observation_table,
		 bird_observation_table );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\"	|"
		 "sql.e '%c'						|"
		 "%s							 ",
		 select,
		 from,
		 where,
		 order,
		 DELIMITER,
		 location_format_process );
	return sys_string;

} /* get_input_sys_string() */

char *get_heading_string(	char *heading_template,
				char *location_format )
{
	static char heading_string[ 256 ];

	if ( !*location_format
	||   strcmp( location_format, "location_format" ) == 0
	||   instr( "UTM", location_format, 1 ) >= 0 )
	{
		sprintf(	heading_string,
				heading_template,
				"easting",
				"northing" );
	}
	else
	{
		sprintf(	heading_string,
				heading_template,
				"longitude",
				"latitude" );
	}
	return heading_string;
} /* get_heading_string() */

