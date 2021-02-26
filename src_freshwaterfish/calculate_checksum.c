/* ---------------------------------------------------	*/
/* src_freshwaterfish/calculate_checksum.c		*/
/* ---------------------------------------------------	*/
/* This process outputs a checksum for each section	*/
/* of the field sheet. By also manually calculating	*/
/* checksum, the manual value can be compared with	*/
/* the computer generated value to provide reasonable	*/
/* assurance that the data entry is correct.		*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "hashtbl.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "freshwaterfish_library.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
double get_measurement_checksum(char *application_name,
				char *site,
				char *plot,
				char *region,
				char *visit_date,
				int throw_number );

int get_surface_checksum(	char *application_name,
				char *site,
				char *plot,
				char *region,
				char *visit_date,
				int throw_number );

int get_submergent_checksum(	char *application_name,
				char *site,
				char *plot,
				char *region,
				char *visit_date,
				int throw_number );

int get_throw_checksum(		char *application_name,
				char *site,
				char *plot,
				char *region,
				char *visit_date,
				int throw_number );

char *get_where_clause(		char *site,
				char *plot,
				char *region,
				char *visit_date,
				int throw_number );

void calculate_checksum(	char *application_name,
				char *site,
				char *plot,
				char *region,
				char *visit_date );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *site;
	char *plot;
	char *region;
	char *visit_date;
	char buffer[ 128 ];

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s application process_name site plot region visit_date\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	site = argv[ 3 ];
	plot = argv[ 4 ];
	region = argv[ 5 ];
	visit_date = argv[ 6 ];

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

	printf( "<h1>%s</h1>\n",
		format_initial_capital( buffer, process_name ) );

	printf( "<h2>\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

	if ( !*site || strcmp( site, "site" ) == 0 )
	{
		printf( "<h3>Please choose a site visit.</h3>\n" );
		document_close();
		exit( 0 );
	}

	calculate_checksum(
			application_name,
			site,
			plot,
			region,
			visit_date );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void calculate_checksum(	char *application_name,
				char *site,
				char *plot,
				char *region,
				char *visit_date )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *justify_comma_list_string;

	justify_comma_list_string =
"left,right,right,right,right,right,right,right,left";

	sprintf(sys_string,
		"html_table.e '' 'Section,1,2,3,4,5,6,7,Notes' '|' '%s'",
		justify_comma_list_string );

	output_pipe = popen( sys_string, "w" );

	fprintf(	output_pipe,
"Throw|%d|%d|%d|%d|%d|%d|%d|For each throw, sum the average plant height + emergence plant cover percent + total plant cover percent + periphyton cover percent + periphyton volume.\n",
			get_throw_checksum(	application_name,
						site,
						plot,
						region,
						visit_date,
						1 ),
			get_throw_checksum(	application_name,
						site,
						plot,
						region,
						visit_date,
						2 ),
			get_throw_checksum(	application_name,
						site,
						plot,
						region,
						visit_date,
						3 ),
			get_throw_checksum(	application_name,
						site,
						plot,
						region,
						visit_date,
						4 ),
			get_throw_checksum(	application_name,
						site,
						plot,
						region,
						visit_date,
						5 ),
			get_throw_checksum(	application_name,
						site,
						plot,
						region,
						visit_date,
						6 ),
			get_throw_checksum(	application_name,
						site,
						plot,
						region,
						visit_date,
						7 )
		);

	fprintf( output_pipe,
"Surface Vegetation|%d|%d|%d|%d|%d|%d|%d|For each throw, sum the density alive + density dead.\n",
			get_surface_checksum(	application_name,
						site,
						plot,
						region,
						visit_date,
						1 ),
			get_surface_checksum(	application_name,
						site,
						plot,
						region,
						visit_date,
						2 ),
			get_surface_checksum(	application_name,
						site,
						plot,
						region,
						visit_date,
						3 ),
			get_surface_checksum(	application_name,
						site,
						plot,
						region,
						visit_date,
						4 ),
			get_surface_checksum(	application_name,
						site,
						plot,
						region,
						visit_date,
						5 ),
			get_surface_checksum(	application_name,
						site,
						plot,
						region,
						visit_date,
						6 ),
			get_surface_checksum(	application_name,
						site,
						plot,
						region,
						visit_date,
						7 )
		);

	fprintf( output_pipe,
"Submergent Vegetation|%d|%d|%d|%d|%d|%d|%d|For each throw, sum the density percent.\n",
			get_submergent_checksum(application_name,
						site,
						plot,
						region,
						visit_date,
						1 ),
			get_submergent_checksum(application_name,
						site,
						plot,
						region,
						visit_date,
						2 ),
			get_submergent_checksum(application_name,
						site,
						plot,
						region,
						visit_date,
						3 ),
			get_submergent_checksum(application_name,
						site,
						plot,
						region,
						visit_date,
						4 ),
			get_submergent_checksum(application_name,
						site,
						plot,
						region,
						visit_date,
						5 ),
			get_submergent_checksum(application_name,
						site,
						plot,
						region,
						visit_date,
						6 ),
			get_submergent_checksum(application_name,
						site,
						plot,
						region,
						visit_date,
						7 )
		);

	fprintf( output_pipe,
"Measurement|%.3lf|%.3lf|%.3lf|%.3lf|%.3lf|%.3lf|%.3lf|For each throw, sum the length + weight.\n",
			get_measurement_checksum(application_name,
						site,
						plot,
						region,
						visit_date,
						1 ),
			get_measurement_checksum(application_name,
						site,
						plot,
						region,
						visit_date,
						2 ),
			get_measurement_checksum(application_name,
						site,
						plot,
						region,
						visit_date,
						3 ),
			get_measurement_checksum(application_name,
						site,
						plot,
						region,
						visit_date,
						4 ),
			get_measurement_checksum(application_name,
						site,
						plot,
						region,
						visit_date,
						5 ),
			get_measurement_checksum(application_name,
						site,
						plot,
						region,
						visit_date,
						6 ),
			get_measurement_checksum(application_name,
						site,
						plot,
						region,
						visit_date,
						7 )
		);

	pclose( output_pipe );

} /* calculate_checksum() */

double get_measurement_checksum(char *application_name,
				char *site,
				char *plot,
				char *region,
				char *visit_date,
				int throw_number )
{
	char sys_string[ 1024 ];
	char *where_clause;
	char *select;
	char *table_name;

	select = "sum(length_millimeters + weight_grams)";

	table_name = get_table_name(	application_name,
					"measurement" );

	where_clause = get_where_clause(	site,
						plot,
						region,
						visit_date,
						throw_number );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s\" | sql.e",
		 select,
		 table_name,
		 where_clause );
	return atof( pipe2string( sys_string ) );
} /* get_measurement_checksum() */

int get_submergent_checksum(	char *application_name,
				char *site,
				char *plot,
				char *region,
				char *visit_date,
				int throw_number )
{
	char sys_string[ 1024 ];
	char *where_clause;
	char *select;
	char *table_name;

	select = "sum(density_percent)";

	table_name = get_table_name(	application_name,
					"submergent_vegetation" );

	where_clause = get_where_clause(	site,
						plot,
						region,
						visit_date,
						throw_number );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s\" | sql.e",
		 select,
		 table_name,
		 where_clause );
	return atoi( pipe2string( sys_string ) );
} /* get_submergent_checksum() */

int get_surface_checksum(	char *application_name,
				char *site,
				char *plot,
				char *region,
				char *visit_date,
				int throw_number )
{
	char sys_string[ 1024 ];
	char *where_clause;
	char *select;
	char *table_name;

	select = "sum(density_alive_count +			"
		 "density_dead_count)				";

	table_name = get_table_name( application_name, "surface_vegetation" );

	where_clause = get_where_clause(	site,
						plot,
						region,
						visit_date,
						throw_number );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s\" | sql.e",
		 select,
		 table_name,
		 where_clause );
	return atoi( pipe2string( sys_string ) );
} /* get_surface_checksum() */

int get_throw_checksum(	char *application_name,
			char *site,
			char *plot,
			char *region,
			char *visit_date,
			int throw_number )
{
	char sys_string[ 1024 ];
	char *where_clause;
	char *select;
	char *table_name;

	select = "sum(average_plant_height_cm +			"
		 "emergence_plant_cover_percent +		"
		 "total_plant_cover_percent +			"
		 "periphyton_cover_percent +			"
		 "periphyton_volume_milliliter)			";

	table_name = get_table_name( application_name, "throw" );

	where_clause = get_where_clause(	site,
						plot,
						region,
						visit_date,
						throw_number );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s\" | sql.e",
		 select,
		 table_name,
		 where_clause );
	return atoi( pipe2string( sys_string ) );
} /* get_throw_checksum() */

char *get_where_clause(	char *site,
			char *plot,
			char *region,
			char *visit_date,
			int throw_number )
{
	static char where_clause[ 1024 ];

	sprintf( where_clause,
		 "site = '%s' and			"
		 "plot = '%s' and			"
		 "region = '%s' and			"
		 "visit_date = '%s' and			"
		 "throw_number = %d			",
		 site,
		 plot,
		 region,
		 visit_date,
		 throw_number );

	return where_clause;

} /* get_where_clause() */

