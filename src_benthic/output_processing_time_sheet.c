/* ---------------------------------------------------	*/
/* src_benthic/output_processing_time_sheet.c		*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"
#include "date_convert.h"
#include "piece.h"
#include "query.h"
#include "list.h"
#include "dictionary.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "hash_table.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "location.h"
#include "site.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void output_table_heading(	void );

void output_processing_time_sheet_table(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *process_name );

void output_processing_time_sheet_text_file(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *document_root_directory,
				pid_t process_id,
				char *process_name );

void output_processing_time_sheet(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *output_medium,
				char *document_root_directory,
				pid_t process_id,
				char *process_name );

void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name,
				char *project,
				char *collection_name,
				char *location_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *project;
	char *collection_name;
	char *location_name;
	char *output_medium;

	output_starting_argv_stderr( argc, argv );

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s application process_name project collection location output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	project = argv[ 3 ];
	collection_name = argv[ 4 ];
	location_name = argv[ 5 ];
	output_medium = argv[ 6 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

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

	output_processing_time_sheet(
				application_name,
				project,
				collection_name,
				location_name,
				output_medium,
				appaserver_parameter_file->
					document_root,
				getpid(),
				process_name );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void output_processing_time_sheet(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *output_medium,
				char *document_root_directory,
				pid_t process_id,
				char *process_name )
{
	if ( strcmp( output_medium, "text_file" ) == 0 )
	{
		output_processing_time_sheet_text_file(
				application_name,
				project,
				collection_name,
				location_name,
				document_root_directory,
				process_id,
				process_name );
	}
	else
	if ( strcmp( output_medium, "table" ) == 0 )
	{
		output_processing_time_sheet_table(
				application_name,
				project,
				collection_name,
				location_name,
				process_name );
	}
	else
	{
		output_processing_time_sheet_table(
				application_name,
				project,
				collection_name,
				location_name,
				process_name );
	}
} /* output_processing_time_sheet() */

void output_processing_time_sheet_text_file(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *document_root_directory,
				pid_t process_id,
				char *process_name )
{
	char *ftp_filename;
	char *output_filename;
	FILE *output_file;
	char title[ 128 ];
	char sub_title[ 128 ];
	int results;
	LOCATION *location;
	SITE *site;
	SWEEP *sweep;
	int sweep_index;
	APPASERVER_LINK_FILE *appaserver_link_file;

	location = location_new( project, location_name );

	if ( ! ( location->site_list = location_get_site_list(
						&location->grand_total_time,
						application_name,
						project,
						collection_name,
						location_name ) )
	||      !list_length( location->site_list ) )
	{
		printf(
"<h3>Error: there is nothing to report for this location and time.</h3>\n" );
		document_close();
		exit( 0 );
	}

	get_title_and_sub_title(	title,
					sub_title,
					process_name,
					project,
					collection_name,
					location_name );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	results = system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

/*
	sprintf(output_filename, 
	 	OUTPUT_TEMPLATE,
	 	appaserver_mount_point,
	 	application_name,
	 	process_id );
*/

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
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

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			output_filename );
		document_close();
		exit( 1 );
	}

	if ( !list_rewind( location->site_list ) )
	{
		printf( "<H2>ERROR: No sites to output.\n" );
		document_close();
		exit( 1 );
	}

	fprintf( output_file, "%s\n", sub_title );

	fprintf( output_file,
"Site,Total,Pick,,,,,Sort,,,,,Pink,,,,,Fish,,,,,Caridean,,,,,Crab\n" );

	fprintf( output_file,
",,1,2,3,4,5,1,2,3,4,5,1,2,3,4,5,1,2,3,4,5,1,2,3,4,5,1,2,3,4,5\n" );

	do {
		site = list_get_pointer( location->site_list );

		fprintf( output_file,
			 "%d,%d",
			 site->site_number,
			 site->total_time );

		/* Pick time */
		/* --------- */
		for (	sweep_index = 0;
			sweep_index < SWEEP_COUNT;
			sweep_index++ )
		{
			sweep = site->sweep_array[ sweep_index ];

			if ( !sweep )
			{
				fprintf( output_file, ",x" );
			}
			else
			{
				fprintf(output_file,
					",%d",
					sweep->wash_time_minutes +
					sweep->pick_time_minutes +
					sweep->quality_control_time_minutes );
			}
		}

		/* Sort time */
		/* --------- */
		for (	sweep_index = 0;
			sweep_index < SWEEP_COUNT;
			sweep_index++ )
		{
			sweep = site->sweep_array[ sweep_index ];

			if ( !sweep )
			{
				fprintf( output_file, ",x" );
			}
			else
			{
				fprintf(output_file,
					",%d",
					sweep->sort_minutes );
			}
		}

		/* Penaeid time */
		/* ------------ */
		for (	sweep_index = 0;
			sweep_index < SWEEP_COUNT;
			sweep_index++ )
		{
			sweep = site->sweep_array[ sweep_index ];

			if ( !sweep )
			{
				fprintf( output_file, ",x" );
			}
			else
			{
				fprintf(output_file,
					",%d",
					sweep->identify_penaeid_minutes );
			}
		}

		/* Fish time */
		/* --------- */
		for (	sweep_index = 0;
			sweep_index < SWEEP_COUNT;
			sweep_index++ )
		{
			sweep = site->sweep_array[ sweep_index ];

			if ( !sweep )
			{
				fprintf( output_file, ",x" );
			}
			else
			{
				fprintf(output_file,
					",%d",
					sweep->identify_fish_minutes );
			}
		}

		/* Caridean time */
		/* ------------- */
		for (	sweep_index = 0;
			sweep_index < SWEEP_COUNT;
			sweep_index++ )
		{
			sweep = site->sweep_array[ sweep_index ];

			if ( !sweep )
			{
				fprintf( output_file, ",x" );
			}
			else
			{
				fprintf(output_file,
					",%d",
					sweep->identify_caridean_minutes );
			}
		}

		/* Crabs time */
		/* ---------- */
		for (	sweep_index = 0;
			sweep_index < SWEEP_COUNT;
			sweep_index++ )
		{
			sweep = site->sweep_array[ sweep_index ];

			if ( !sweep )
			{
				fprintf( output_file, ",x" );
			}
			else
			{
				fprintf(output_file,
					",%d",
					sweep->identify_crabs_minutes );
			}
		}

		fprintf( output_file, "\n" );
	} while( list_next( location->site_list ) );

	fprintf( output_file,
		 ",%d\n",
		 location->grand_total_time );

	fclose( output_file );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
"&lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );

} /* output_processing_time_sheet_text_file() */

void output_processing_time_sheet_table(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *process_name )
{
	char title[ 128 ];
	char sub_title[ 128 ];
	int results;
	LOCATION *location;
	SITE *site;
	SWEEP *sweep;
	int sweep_index;

	location = location_new( project, location_name );

	if ( ! ( location->site_list = location_get_site_list(
						&location->grand_total_time,
						application_name,
						project,
						collection_name,
						location_name ) )
	||      !list_length( location->site_list ) )
	{
		printf(
	"<h3>Error: there are no sites assigned to this location.</h3>\n" );
		document_close();
		exit( 0 );
	}

	get_title_and_sub_title(	title,
					sub_title,
					process_name,
					project,
					collection_name,
					location_name );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	results = system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

	if ( !list_rewind( location->site_list ) )
	{
		printf( "<H2>ERROR: No sites to output.\n" );
		document_close();
		exit( 1 );
	}

	printf( "<table border>\n" );

	output_table_heading();

	do {
		site = list_get_pointer( location->site_list );

		if ( site->site_number == 16 )
			output_table_heading();

		printf( "<tr><td align=right>%d<td align=right>%d",
			 site->site_number,
			 site->total_time );

		/* Pick time */
		/* --------- */
		for (	sweep_index = 0;
			sweep_index < SWEEP_COUNT;
			sweep_index++ )
		{
			sweep = site->sweep_array[ sweep_index ];

			if ( !sweep )
			{
				printf( "<td align=right>x" );
			}
			else
			{
				printf( "<td align=right>%d",
					sweep->wash_time_minutes +
					sweep->pick_time_minutes +
					sweep->quality_control_time_minutes );
			}
		}

		/* Sort time */
		/* --------- */
		for (	sweep_index = 0;
			sweep_index < SWEEP_COUNT;
			sweep_index++ )
		{
			sweep = site->sweep_array[ sweep_index ];

			if ( !sweep )
			{
				printf( "<td align=right>x" );
			}
			else
			{
				printf( "<td align=right>%d",
					sweep->sort_minutes );
			}
		}

		/* Penaeid time */
		/* ------------ */
		for (	sweep_index = 0;
			sweep_index < SWEEP_COUNT;
			sweep_index++ )
		{
			sweep = site->sweep_array[ sweep_index ];

			if ( !sweep )
			{
				printf( "<td align=right>x" );
			}
			else
			{
				printf( "<td align=right>%d",
					sweep->identify_penaeid_minutes );
			}
		}

		/* Fish time */
		/* --------- */
		for (	sweep_index = 0;
			sweep_index < SWEEP_COUNT;
			sweep_index++ )
		{
			sweep = site->sweep_array[ sweep_index ];

			if ( !sweep )
			{
				printf( "<td align=right>x" );
			}
			else
			{
				printf( "<td align=right>%d",
					sweep->identify_fish_minutes );
			}
		}

		/* Caridean time */
		/* ------------- */
		for (	sweep_index = 0;
			sweep_index < SWEEP_COUNT;
			sweep_index++ )
		{
			sweep = site->sweep_array[ sweep_index ];

			if ( !sweep )
			{
				printf( "<td align=right>x" );
			}
			else
			{
				printf( "<td align=right>%d",
					sweep->identify_caridean_minutes );
			}
		}

		/* Crabs time */
		/* ---------- */
		for (	sweep_index = 0;
			sweep_index < SWEEP_COUNT;
			sweep_index++ )
		{
			sweep = site->sweep_array[ sweep_index ];

			if ( !sweep )
			{
				printf( "<td align=right>x" );
			}
			else
			{
				printf( "<td align=right>%d",
					sweep->identify_crabs_minutes );
			}
		}

		printf( "\n" );
	} while( list_next( location->site_list ) );

	printf( "<tr><td align=right><td align=right>%d\n",
		 location->grand_total_time );

	printf( "</table>\n" );

} /* output_processing_time_sheet_table() */

void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name,
				char *project,
				char *collection_name,
				char *location_name )
{
	format_initial_capital( title, process_name );

	sprintf(	sub_title,
			"%s %s at %s",
			project,
			collection_name,
			location_name );
	format_initial_capital( sub_title, sub_title );

} /* get_title_and_sub_title() */

void output_table_heading( void )
{
	printf(
"<tr><th>Site<th>Total<th colspan=5>Pick<th colspan=5>Sort<th colspan=5>Pink<th colspan=5>Fish<th colspan=5>Caridean<th colspan=5>Crab\n" );

	printf(
"<tr><th><th><th>1<th>2<th>3<th>4<th>5<th>1<th>2<th>3<th>4<th>5<th>1<th>2<th>3<th>4<th>5<th>1<th>2<th>3<th>4<th>5<th>1<th>2<th>3<th>4<th>5<th>1<th>2<th>3<th>4<th>5\n" );

} /* output_table_heading() */

