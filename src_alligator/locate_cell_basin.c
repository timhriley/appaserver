/* ---------------------------------------------------	*/
/* locate_cell_basin.c					*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "html_table.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "alligator.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *utm_easting;
	char *utm_northing;
	char *transect_number_string;
	int cell_number = 0;
	char *basin;
	char basin_buffer[ 128 ];
	char in_transect_boundary_yn;
	double distance = 0.0;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char title[ 128 ];

	if ( argc != 5 )
	{
		fprintf( stderr, 
	"Usage: %s application process_name utm_easting utm_northing\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	utm_easting = argv[ 3 ];
	utm_northing = argv[ 4 ];

	appaserver_parameter_file = new_appaserver_parameter_file();

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

	strcpy( title, process_name );
	format_initial_capital( title, title );
	printf( "<h2>%s</h2>\n", title );
	fflush( stdout );
	printf( "<h2>\n" );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) );
	fflush( stdout );
	printf( "</h2>\n" );

	transect_number_string =
		alligator_get_transect_number_string(
				application_name,
				atoi( utm_northing ) );

	basin = alligator_get_basin_should_be(
					&cell_number,
					application_name,
					atof( utm_northing ),
					atof( utm_easting ) );

	in_transect_boundary_yn =
		alligator_get_in_transect_boundary_should_be_yn(
				&distance,
				application_name,
				(transect_number_string)
				? atoi( transect_number_string )
				: 0,
				atoi( utm_northing ),
				atoi( utm_easting ) );

	printf( "<table border=1>\n" );
	printf( "<tr><td>UTM Easting<td align=right>%s\n", utm_easting );
	printf( "<tr><td>UTM Northing<td align=right>%s\n", utm_northing );

	printf( "<tr><td>Cell\n" );
	if ( cell_number )
		printf( "<td align=right>%d\n", cell_number );
	else
		printf( "\n" );

	printf( "<tr><td>Basin\n" );
	if ( basin )
	{
		printf( "<td align=right>%s\n",
			format_initial_capital( basin_buffer, basin ) );
	}
	else
	{
		printf( "\n" );
	}

	printf( "<tr><td>Transect\n" );
	if ( transect_number_string )
		printf( "<td align=right>%s\n", transect_number_string );
	else
		printf( "\n" );

	printf( "<tr><td>In Transect Boundary\n" );
	if ( in_transect_boundary_yn == 'y' )
	{
		printf( "<td align=right>Yes (%.1lf)\n", distance );
	}
	else
	{
		printf( "<td align=right>No (%.1lf)\n", distance );
	}

	printf( "</table>\n" );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

