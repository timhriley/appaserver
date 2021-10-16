/* ----------------------------------------------------------------------*/
/* $APPASERVER_HOME/src_benthic/output_spss_ready_attempt_success.c	 */
/* ----------------------------------------------------------------------*/
/* Freely available software: see Appaserver.org	 		 */
/* ----------------------------------------------------------------------*/

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
#include "benthic_library.h"
#include "appaserver_link.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define FILENAME_STEM			"spss_ready_attempt"

/* Prototypes */
/* ---------- */
FILE *get_input_pipe(		char *application_name,
				LIST *collection_list,
				LIST *project_list );

char *get_select_string(
				char *application_name,
				LIST *collection_list,
				LIST *project_list );

char *get_query_or_sequence_where_clause(
				char *application_name,
				LIST *collection_list,
				LIST *project_list );

void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name,
				LIST *collection_list,
				LIST *project_list );

void output_spss_ready_attempt_success(
				char *application_name,
				char *document_root_directory,
				int process_id,
				LIST *collection_list,
				LIST *project_list );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *collection_list_string;
	char *project_list_string;
	LIST *collection_list;
	LIST *project_list;
	char title[ 128 ];
	char sub_title[ 65536 ];
	int results;

	if ( argc != 5 )
	{
		fprintf( stderr, 
"Usage: %s application process_name collection_list project_list\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	collection_list_string = argv[ 3 ];
	project_list_string = argv[ 4 ];

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

	collection_list = list_string2list( collection_list_string, ',' );
	project_list = list_string2list( project_list_string, ',' );

	get_title_and_sub_title(
			title,
			sub_title,
			process_name,
			collection_list,
			project_list );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	results = system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

	if ( !*collection_list_string
	||   strcmp( collection_list_string, "collection_name" ) == 0 )
	{
		printf(
		"<h3>Please choose a collection.</h3>\n" );
		document_close();
		exit( 0 );
	}

	output_spss_ready_attempt_success(
				application_name,
				appaserver_parameter_file->
					document_root,
				getpid(),
				collection_list,
				project_list );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
}

void output_spss_ready_attempt_success(
				char *application_name,
				char *document_root_directory,
				int process_id,
				LIST *collection_list,
				LIST *project_list )
{
	char sys_string[ 1024 ];
	char *process_output_filename;
	char *ftp_filename;
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];
	char *datatype_comma_list;
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
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"csv" );

	process_output_filename =
		appaserver_link_output_filename(
			appaserver_link->document_root_directory,
			appaserver_link_output_tail_half(
				appaserver_link->application_name,
				appaserver_link->filename_stem,
				appaserver_link->begin_date_string,
				appaserver_link->end_date_string,
				appaserver_link->process_id,
				appaserver_link->session_key,
				appaserver_link->extension ) );

	datatype_comma_list = "Attempt1,Attempt2,Attempt3,Attempt4,Attempt5";

	sprintf( sys_string,
		 "pivot_table.e \"%s\" ',' > %s",
		 datatype_comma_list,
		 process_output_filename );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
".heading collect_num,site_num,collection,location,region,pink_area,season,cell,lat_sampled,long_sampled,Date,Time,%s\n",
		 datatype_comma_list );

	input_pipe = get_input_pipe( 
				application_name,
				collection_list,
				project_list );

	while( get_line( input_buffer, input_pipe ) )
	{
		fprintf( output_pipe, "%s\n", input_buffer );
	}

	pclose( output_pipe );
	pclose( input_pipe );

	ftp_filename =
		appaserver_link_prompt_filename(
			appaserver_link_prompt_link_half(
				appaserver_link->prepend_http,
				appaserver_link->http_prefix,
				appaserver_link->server_address ),
			appaserver_link->application_name,
			appaserver_link->filename_stem,
			appaserver_link->begin_date_string,
			appaserver_link->end_date_string,
			appaserver_link->process_id,
			appaserver_link->session_key,
			appaserver_link->extension );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
"Attempt Success: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );

}

void get_title_and_sub_title(
			char *title,
			char *sub_title,
			char *process_name,
			LIST *collection_list,
			LIST *project_list )
{
	format_initial_capital( title, process_name );

	strcpy( sub_title,
		benthic_library_get_collection_project_combined_string(
			collection_list,
			project_list ) );

}

FILE *get_input_pipe(	char *application_name,
			LIST *collection_list,
			LIST *project_list )
{
	char *select_string;
	char sys_string[ 65536 ];

	select_string =
		get_select_string(
			application_name,
			collection_list,
			project_list );

	sprintf( sys_string,
		 "echo \"%s\" | sql.e ','",
		 select_string );

	return popen( sys_string, "r" );

}

char *get_query_or_sequence_where_clause(
				char *application_name,
				LIST *collection_list,
				LIST *project_list )
{
	LIST *attribute_name_list;
	char *sampling_point_table_name;
	char attribute_name[ 128 ];
	QUERY_OR_SEQUENCE *query_or_sequence;

	attribute_name_list = list_new();

	sampling_point_table_name =
		get_table_name(
			application_name, "sampling_point" );

	sprintf(	attribute_name,
			"%s.collection_name",
			sampling_point_table_name );

	list_append_pointer( attribute_name_list, strdup( attribute_name ) );

	sprintf(	attribute_name,
			"%s.project",
			sampling_point_table_name );

	list_append_pointer( attribute_name_list, strdup( attribute_name ) );

	query_or_sequence = query_or_sequence_new( attribute_name_list );

	query_or_sequence_set_data_list(
			query_or_sequence->data_list_list,
			collection_list );

	query_or_sequence_set_data_list(
			query_or_sequence->data_list_list,
			project_list );

	return query_or_sequence_where_clause(
				query_or_sequence->attribute_name_list,
				query_or_sequence->data_list_list,
				0 /* not with_and_prefix */ );
}

char *get_select_string(
			char *application_name,
			LIST *collection_list,
			LIST *project_list )
{
	char select_string[ 65536 ];
	char select[ 1024 ];
	char *where_or_clause;
	char where_collection_join[ 256 ];
	char where_pink_assessment_area_join[ 256 ];
	char where_location_join[ 256 ];
	char where_region_join[ 256 ];
	char where_sampling_site_attempt_success_join[ 2048 ];
	char where_clause[ 4096 ];
	char from[ 512 ];
	char *sampling_point_table_name;
	char *collection_table_name;
	char *location_table_name;
	char *region_table_name;
	char *pink_assessment_area_table_name;
	char *sampling_site_attempt_success_table_name;

	sampling_site_attempt_success_table_name =
		get_table_name( application_name,
				"sampling_site_attempt_success" );

	sampling_point_table_name =
		get_table_name( application_name, "sampling_point" );

	collection_table_name =
		get_table_name( application_name, "collection" );

	location_table_name =
		get_table_name( application_name, "location" );

	region_table_name =
		get_table_name( application_name, "region" );

	pink_assessment_area_table_name =
		get_table_name( application_name, "pink_assessment_area" );

	/* Select */
	/* ====== */
	sprintf( select,
"lpad(collection_number,2,'0'),lpad(location_number,2,'0'),%s.collection_name,%s.location,region_code,pink_assessment_area_code,'',lpad(%s.site_number,2,'0'),%s.actual_latitude,%s.actual_longitude,date_format(%s.anchor_date,'%cc/%ce/%cY'),%s.anchor_time,concat( 'Attempt', sample_number ),attempt_success",
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 '%', '%', '%',
		 sampling_point_table_name );

	/* From */
	/* ==== */
	sprintf( from,
		 "%s,%s,%s,%s,%s,%s",
		 sampling_point_table_name,
		 sampling_site_attempt_success_table_name,
		 collection_table_name,
		 location_table_name,
		 region_table_name,
		 pink_assessment_area_table_name );

	/* Where */
	/* ===== */

	/* Where quad_vegetation join */
	/* -------------------------- */
	sprintf( where_sampling_site_attempt_success_join,
		 "%s.anchor_date = %s.anchor_date and			"
		 "%s.anchor_time = %s.anchor_time and			"
		 "%s.location = %s.location and				"
		 "%s.site_number = %s.site_number			",
		 sampling_site_attempt_success_table_name,
		 sampling_point_table_name,
		 sampling_site_attempt_success_table_name,
		 sampling_point_table_name,
		 sampling_site_attempt_success_table_name,
		 sampling_point_table_name,
		 sampling_site_attempt_success_table_name,
		 sampling_point_table_name );

	/* Where collection join */
	/* --------------------- */
	sprintf( where_collection_join,
		 "%s.collection_name = %s.collection_name and	"
		 "%s.project = %s.project 			",
		 sampling_point_table_name,
		 collection_table_name,
		 sampling_point_table_name,
		 collection_table_name );

	/* Where location join */
	/* ------------------- */
	sprintf( where_location_join,
		 "%s.location = %s.location			",
		 sampling_point_table_name,
		 location_table_name );

	/* Where region join */
	/* ----------------- */
	sprintf( where_region_join,
		 "%s.region = %s.region				",
		 location_table_name,
		 region_table_name );

	/* Where pink_assessment_area join */
	/* ------------------------------- */
	sprintf( where_pink_assessment_area_join,
		 "%s.pink_assessment_area = %s.pink_assessment_area	",
		 location_table_name,
		 pink_assessment_area_table_name );

	where_or_clause =
		get_query_or_sequence_where_clause(
			application_name,
			collection_list,
			project_list );

	sprintf( where_clause,
		 "(%s) and %s and %s and %s and %s and %s",
		 where_or_clause,
		 where_collection_join,
		 where_location_join,
		 where_region_join,
		 where_sampling_site_attempt_success_join,
		 where_pink_assessment_area_join );

	sprintf( select_string,
		 "select %s from %s where %s order by %s;",
		 select,
		 from,
		 where_clause,
		 select );

	return strdup( select_string );

}

