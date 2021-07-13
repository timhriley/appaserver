/* ---------------------------------------------------	*/
/* src_benthic/output_quick_count.c			*/
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
#include "column.h"
#include "grace.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
FILE *get_input_pipe(		char *application_name,
				char *project_list_string,
				char *collection_list_string,
				char *location_list_string,
				char *family_name,
				char *genus_name,
				char *species_name );

void output_quick_count_statistics(
				char *application_name,
				char *project_list_string,
				char *collection_list_string,
				char *location_list_string,
				char *family_name,
				char *genus_name,
				char *species_name,
				pid_t process_id );

void output_quick_count_histogram(
				char *application_name,
				char *project_list_string,
				char *collection_list_string,
				char *location_list_string,
				char *family_name,
				char *genus_name,
				char *species_name,
				char *document_root_directory,
				pid_t process_id,
				char *title,
				char *sub_title );

void output_quick_count_text_file(
				char *application_name,
				char *project_list_string,
				char *collection_list_string,
				char *location_list_string,
				char *family_name,
				char *genus_name,
				char *species_name,
				char *document_root_directory,
				pid_t process_id,
				char *sub_title,
				char *process_name );

void output_quick_count(
				char *application_name,
				char *project_list_string,
				char *collection_list_string,
				char *location_list_string,
				char *family_name,
				char *genus_name,
				char *species_name,
				char *document_root_directory,
				pid_t process_id,
				char *process_name,
				char *output_medium );

void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name,
				char *collection_list_string,
				char *location_list_string,
				char *family_name,
				char *genus_name,
				char *species_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *project_list_string;
	char *collection_list_string;
	char *location_list_string;
	char *family_name;
	char *genus_name;
	char *species_name;
	char *output_medium;

	if ( argc != 10 )
	{
		fprintf( stderr, 
"Usage: %s application process_name project collection location family genus species output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	project_list_string = argv[ 3 ];
	collection_list_string = argv[ 4 ];
	location_list_string = argv[ 5 ];
	family_name = argv[ 6 ];
	genus_name = argv[ 7 ];
	species_name = argv[ 8 ];
	output_medium = argv[ 9 ];

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

	output_quick_count(
				application_name,
				project_list_string,
				collection_list_string,
				location_list_string,
				family_name,
				genus_name,
				species_name,
				appaserver_parameter_file->
					document_root,
				getpid(),
				process_name,
				output_medium );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void output_quick_count(
				char *application_name,
				char *project_list_string,
				char *collection_list_string,
				char *location_list_string,
				char *family_name,
				char *genus_name,
				char *species_name,
				char *document_root_directory,
				pid_t process_id,
				char *process_name,
				char *output_medium )
{
	char title[ 128 ];
	char sub_title[ 128 ];

	get_title_and_sub_title(
			title,
			sub_title,
			process_name,
			collection_list_string,
			location_list_string,
			family_name,
			genus_name,
			species_name );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( timlib_strcmp( family_name, "family_name" ) == 0 )
	{
		printf(
	"<h3>Please choose either a family, genus, or species.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( strcmp( output_medium, "statistics" ) == 0 )
	{
		output_quick_count_statistics(
				application_name,
				project_list_string,
				collection_list_string,
				location_list_string,
				family_name,
				genus_name,
				species_name,
				process_id );
	}
	else
	if ( strcmp( output_medium, "text_file" ) == 0
	||   strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		output_quick_count_text_file(
				application_name,
				project_list_string,
				collection_list_string,
				location_list_string,
				family_name,
				genus_name,
				species_name,
				document_root_directory,
				process_id,
				sub_title,
				process_name );
	}
	else
	if ( strcmp( output_medium, "histogram" ) == 0 )
	{
		output_quick_count_histogram(
				application_name,
				project_list_string,
				collection_list_string,
				location_list_string,
				family_name,
				genus_name,
				species_name,
				document_root_directory,
				process_id,
				title,
				sub_title );
	}
	else
	{
		output_quick_count_statistics(
				application_name,
				project_list_string,
				collection_list_string,
				location_list_string,
				family_name,
				genus_name,
				species_name,
				process_id );
	}
} /* output_quick_count() */

void output_quick_count_statistics(
				char *application_name,
				char *project_list_string,
				char *collection_list_string,
				char *location_list_string,
				char *family_name,
				char *genus_name,
				char *species_name,
				pid_t process_id )
{
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];
	char statistics_temp_file[ 128 ];
	char sys_string[ 1024 ];

	sprintf(	statistics_temp_file,
			"/tmp/output_quick_count_%d",
			process_id );

	input_pipe = get_input_pipe(
			application_name,
			project_list_string,
			collection_list_string,
			location_list_string,
			family_name,
			genus_name,
			species_name );

	sprintf(sys_string,
		"statistics_weighted.e '|' 0 -1 > %s",
		statistics_temp_file );

	output_pipe = popen( sys_string, "w" );

	while( get_line( input_buffer, input_pipe ) )
	{
		fprintf( output_pipe, "%s\n", input_buffer );
	}
	pclose( input_pipe );
	pclose( output_pipe );

	sprintf(	sys_string,
			"cat %s					|"
"html_table.e '' '' ':' left,right",
			statistics_temp_file );

	system( sys_string );

	sprintf( sys_string, "rm -f %s", statistics_temp_file );
	system( sys_string );

} /* output_quick_count_statistics() */

void output_quick_count_histogram(
				char *application_name,
				char *project_list_string,
				char *collection_list_string,
				char *location_list_string,
				char *family_name,
				char *genus_name,
				char *species_name,
				char *document_root_directory,
				pid_t process_id,
				char *title,
				char *sub_title )
{
	char output_filename[ 256 ];
	FILE *output_pipe;
	FILE *input_pipe;
	FILE *input_file;
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	char ftp_agr_filename[ 256 ];
	char ftp_output_filename[ 256 ];
	char grace_histogram_filename[ 256 ];

/*
	FILE *output_file;
	sprintf(output_filename, 
	 	OUTPUT_TEMPLATE,
	 	appaserver_mount_point,
	 	application_name,
	 	process_id );

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s</H2>\n",
			output_filename );
		document_close();
		exit( 1 );
	}

	fprintf( output_file, "%s\n", sub_title );
	fclose( output_file );
*/

	input_pipe = get_input_pipe(
			application_name,
			project_list_string,
			collection_list_string,
			location_list_string,
			family_name,
			genus_name,
			species_name );

	sprintf( grace_histogram_filename,
		 "%s/%s/grace_histogram_%d.dat",
		 document_root_directory,
		 application_name,
		 process_id );

	search_replace_string( sub_title, "<br>", "/" );
	if ( *sub_title == '/' ) timlib_strcpy( sub_title, sub_title + 1, 0 );

	sprintf(	sys_string,
			"grace_histogram.e %s %d \"%s\" \"%s\" \"%s\" > %s",
			application_name,
			process_id,
			title,
			sub_title,
			"",
			grace_histogram_filename );

	output_pipe = popen( sys_string, "w" );

	while( get_line( input_buffer, input_pipe ) )
	{
		fprintf( output_pipe, "%s\n", input_buffer );
	}
	pclose( input_pipe );
	pclose( output_pipe );

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
		fprintf( stderr,
	 	"%s/%s(): unexpected end of file in %s\n",
	 		__FILE__,
	 		__FUNCTION__,
	 		grace_histogram_filename );
		fclose( input_file );
		exit( 1 );
	}

	column( ftp_agr_filename, 0, input_buffer );
	column( ftp_output_filename, 1, input_buffer );
	column( output_filename, 2, input_buffer );

	fclose( input_file );
	sprintf( sys_string,
		 "rm -f %s",
		 grace_histogram_filename );
	system( sys_string );

	grace_output_graph_window(
				application_name,
				ftp_output_filename,
				ftp_agr_filename,
				(char *)0 /* appaserver_mount_point */,
				0 /* not with_document_output */,
				(char *)0 /* where_clause */ );

} /* output_quick_count_histogram() */

void output_quick_count_text_file(
				char *application_name,
				char *project_list_string,
				char *collection_list_string,
				char *location_list_string,
				char *family_name,
				char *genus_name,
				char *species_name,
				char *document_root_directory,
				pid_t process_id,
				char *sub_title,
				char *process_name )
{
	char *ftp_filename;
	char *output_filename;
	FILE *output_file;
	FILE *output_pipe;
	FILE *input_pipe;
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

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

/*
	sprintf(output_filename, 
	 	OUTPUT_TEMPLATE,
	 	appaserver_mount_point,
	 	application_name,
	 	process_id );
*/

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s</H2>\n",
			output_filename );
		document_close();
		exit( 1 );
	}

	search_replace_string( sub_title, "<br>", "/" );
	if ( *sub_title == '/' ) timlib_strcpy( sub_title, sub_title + 1, 0 );

	fprintf( output_file, "%s\n", sub_title );

	fclose( output_file );

	input_pipe = get_input_pipe(
			application_name,
			project_list_string,
			collection_list_string,
			location_list_string,
			family_name,
			genus_name,
			species_name );

	sprintf(sys_string,
		"statistics_weighted.e '|' 0 -1 | tr ':' ',' >> %s",
		output_filename );

	output_pipe = popen( sys_string, "w" );

	while( get_line( input_buffer, input_pipe ) )
	{
		fprintf( output_pipe, "%s\n", input_buffer );
	}
	pclose( input_pipe );
	pclose( output_pipe );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
"&lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );

} /* output_quick_count_text_file() */

void get_title_and_sub_title(
			char *title,
			char *sub_title,
			char *process_name,
			char *collection_list_string,
			char *location_list_string,
			char *family_name,
			char *genus_name,
			char *species_name )
{
	char project_sub_title[ 4096 ];
	char species_sub_title[ 1024 ];
	char *ptr = project_sub_title;

	format_initial_capital( title, process_name );

	*ptr = '\0';

	if ( *collection_list_string
	&&   strcmp( collection_list_string, "collection_name" ) != 0 )
	{
		ptr += sprintf( ptr, "%s", collection_list_string );
	}

	if ( *location_list_string
	&&   strcmp( location_list_string, "location" ) != 0 )
	{
		if ( ptr != project_sub_title )
			ptr += sprintf( ptr, "<br>" );

		ptr += sprintf( ptr, "%s", location_list_string );
	}

	ptr = species_sub_title;

	*ptr = '\0';

	if ( *family_name && strcmp( family_name, "family_name" ) != 0 )
	{
		ptr += sprintf( ptr, "%s", family_name );
	}

	if ( *genus_name && strcmp( genus_name, "genus_name" ) != 0 )
	{
		ptr += sprintf( ptr, " %s", genus_name );
	}

	if ( *species_name && strcmp( species_name, "species_name" ) != 0 )
	{
		ptr += sprintf( ptr, " %s", species_name );
	}

	sprintf(	sub_title,
			"%s<br>%s",
			project_sub_title,
			species_sub_title );

} /* get_title_and_sub_title() */

FILE *get_input_pipe(	char *application_name,
			char *project_list_string,
			char *collection_list_string,
			char *location_list_string,
			char *family_name,
			char *genus_name,
			char *species_name )
{
	char sys_string[ 65536 ];
	char select[ 1024 ];
	char where_join[ 1024 ];
	char where_clause[ 65536 ];
	char where_benthic_species[ 512 ];
	char where_collection[ 65536 ];
	char where_location[ 65536 ];
	char from[ 1024 ];
	char group_by[ 1024 ];
	char *sampling_point_table_name;
	char *species_count_table_name;

	sampling_point_table_name =
		get_table_name( application_name, "sampling_point" );

	species_count_table_name =
		get_table_name( application_name, "species_count" );

	sprintf( select,
		 "sum( count_per_square_meter )" );

	sprintf( from,
		 "%s,%s",
		 sampling_point_table_name,
		 species_count_table_name );

	sprintf( where_join,
		 "%s.anchor_date = %s.anchor_date and			"
		 "%s.anchor_time = %s.anchor_time and			"
		 "%s.location = %s.location and				"
		 "%s.site_number = %s.site_number 			",
		 sampling_point_table_name,
		 species_count_table_name,
		 sampling_point_table_name,
		 species_count_table_name,
		 sampling_point_table_name,
		 species_count_table_name,
		 sampling_point_table_name,
		 species_count_table_name );

	if ( strcmp( genus_name, "genus_name" ) == 0 )
	{
		sprintf( where_benthic_species,
		 "%s.family_name = '%s'",
		 	species_count_table_name,
		 	family_name );
	}
	else
	if ( strcmp( species_name, "species_name" ) == 0 )
	{
		sprintf( where_benthic_species,
		 "%s.family_name = '%s' and	"
		 "%s.genus_name = '%s'",
		 	species_count_table_name,
		 	family_name,
		 	species_count_table_name,
		 	genus_name );
	}
	else
	{
		sprintf( where_benthic_species,
		 "%s.family_name = '%s' and				"
		 "%s.genus_name = '%s' and				"
		 "%s.species_name = '%s'				",
		 	species_count_table_name,
		 	family_name,
		 	species_count_table_name,
		 	genus_name,
		 	species_count_table_name,
		 	species_name );
	}

	if ( *collection_list_string
	&&   strcmp( collection_list_string, "collection_name" ) != 0 )
	{
		sprintf( where_collection,
		 "%s.collection_name in (%s) and			"
		 "%s.project in (%s)					",
		 sampling_point_table_name,
		 timlib_get_in_clause( collection_list_string ),
		 sampling_point_table_name,
		 timlib_get_in_clause( project_list_string ) );
	}
	else
	{
		strcpy( where_collection, "1 = 1" );
	}

	if ( *location_list_string
	&&   strcmp( location_list_string, "location" ) != 0 )
	{
		sprintf( where_location,
		 "%s.location in (%s)",
		 sampling_point_table_name,
		 timlib_get_in_clause( location_list_string ) );
	}
	else
	{
		strcpy( where_location, "1 = 1" );
	}

	sprintf( where_clause,
		 "%s and %s and %s and %s",
		 where_benthic_species,
		 where_collection,
		 where_location,
		 where_join );

	sprintf( group_by,
"%s.anchor_date,%s.anchor_time,%s.location,%s.site_number",
		 species_count_table_name,
		 species_count_table_name,
		 species_count_table_name,
		 species_count_table_name );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s group by %s;\" | sql.e",
		 select,
		 from,
		 where_clause,
		 group_by );

/* fprintf( stderr, "%s\n", sys_string ); */

	return popen( sys_string, "r" );

} /* get_input_pipe() */

