/* ------------------------------------------------------*/
/* src_benthic/output_spss_ready_sampling_point_abiotic.c*/
/* ------------------------------------------------------*/
/* Freely available software: see Appaserver.org	 */
/* ------------------------------------------------------*/

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
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define FILENAME_STEM			"spss_ready_abiotic"

/* Prototypes */
/* ---------- */
FILE *get_input_pipe(		char *application_name,
				LIST *collection_list,
				LIST *project_list );

char *get_sampling_select_string(
				char *application_name,
				LIST *collection_list,
				LIST *project_list );

char *get_throwtrap_select_string(
				char *application_name,
				LIST *collection_list,
				LIST *project_list );

char *get_query_or_sequence_where_clause(
				char *application_name,
				LIST *collection_list,
				LIST *project_list );

LIST *get_master_datatype_name_list(
				char *application_name );

void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name,
				LIST *collection_list,
				LIST *project_list );

void output_spss_ready_sampling_point_abiotic(
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

	output_spss_ready_sampling_point_abiotic(
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

void output_spss_ready_sampling_point_abiotic(
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
	LIST *master_datatype_name_list;
	char input_buffer[ 1024 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
		   application_http_prefix(
				application_name ),
		   appaserver_library_server_address(),
		   ( application_prepend_http_protocol_yn(
			application_name ) == 'y' ),
		   document_root_directory,
		   FILENAME_STEM,
		   application_name,
		   process_id,
		   (char *)0 /* session */,
		   "csv" );

	process_output_filename =
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

	master_datatype_name_list = 
		get_master_datatype_name_list(
			application_name );

/*
	sprintf(process_output_filename,
	 	OUTPUT_FILE,
	 	appaserver_mount_point,
	 	application_name,
	 	process_id );
*/

	sprintf( sys_string,
		 "pivot_table.e \"%s\" ',' > %s",
		 list_display_delimited( master_datatype_name_list, ',' ),
		 process_output_filename );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
".heading collect_num,site_num,collection,location,region,pink_area,season,cell,lat_sampled,long_sampled,alt_site,random_Lat,random_Long,corr_Lat,corr_Long,proc_time,date,time,%s\n",
		 list_display_delimited( master_datatype_name_list, ',' ) );

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

	appaserver_library_output_ftp_prompt(
			ftp_filename,
"Sampling Point/Abiotic: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
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
	char *sampling_select_string;
	char *throwtrap_select_string;
	char sys_string[ 65536 ];

	sampling_select_string =
		get_sampling_select_string(
			application_name,
			collection_list,
			project_list );

	throwtrap_select_string =
		get_throwtrap_select_string(
			application_name,
			collection_list,
			project_list );

	sprintf( sys_string,
		 "(echo \"%s\" | sql.e ',' && echo \"%s\" | sql.e ',') | sort",
		 sampling_select_string,
		 throwtrap_select_string );

/*
{
char msg[ 10000 ];
sprintf( msg, "\n%s/%s()/%d: got sys_string = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
sys_string );
m2( "benthic", msg );
}
*/
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

	return query_or_sequence_get_where_clause(
			query_or_sequence->attribute_name_list,
			query_or_sequence->data_list_list,
			0 /* not with_and_prefix */ );
}

LIST *get_master_datatype_name_list(
			char *application_name )
{
	char sys_string[ 1024 ];
	char *where;

	where = "omit_insert_yn is null or omit_insert_yn = 'n'";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=datatype		"
		 "			folder=datatype		"
		 "			where=\"%s\"		",
		 application_name,
		 where );
	return pipe2list( sys_string );
	
}

char *get_sampling_select_string(
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
	char where_abiotic_measurement_join[ 2048 ];
	char where_clause[ 4096 ];
	char from[ 512 ];
	char *sampling_point_table_name;
	char *collection_table_name;
	char *location_table_name;
	char *region_table_name;
	char *pink_assessment_area_table_name;
	char *abiotic_measurement_table_name;

	abiotic_measurement_table_name =
		get_table_name(	application_name,
				"sampling_point_abiotic_measurement" );

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
"lpad(collection_number,2,'0'),lpad(location_number,2,'0'),%s.collection_name,%s.location,region_code,pink_assessment_area_code,'',lpad(%s.site_number,2,'0'),%s.actual_latitude,%s.actual_longitude,'','','','','','',date_format(%s.anchor_date,'%cc/%ce/%cY'),%s.anchor_time,datatype,value",
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
		 abiotic_measurement_table_name,
		 collection_table_name,
		 location_table_name,
		 region_table_name,
		 pink_assessment_area_table_name );

	/* Where */
	/* ===== */

	/* Where quad_vegetation join */
	/* -------------------------- */
	sprintf( where_abiotic_measurement_join,
		 "%s.anchor_date = %s.anchor_date and			"
		 "%s.anchor_time = %s.anchor_time and			"
		 "%s.location = %s.location and				"
		 "%s.site_number = %s.site_number			",
		 abiotic_measurement_table_name,
		 sampling_point_table_name,
		 abiotic_measurement_table_name,
		 sampling_point_table_name,
		 abiotic_measurement_table_name,
		 sampling_point_table_name,
		 abiotic_measurement_table_name,
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
		 where_abiotic_measurement_join,
		 where_pink_assessment_area_join );

	sprintf( select_string,
		 "select %s from %s where %s;",
		 select,
		 from,
		 where_clause );

	return strdup( select_string );

}

char *get_throwtrap_select_string(
			char *application_name,
			LIST *collection_list,
			LIST *project_list )
{
	char select_string[ 65536 ];
	char select[ 1024 ];
	char group[ 1024 ];
	char *where_or_clause;
	char where_collection_join[ 256 ];
	char where_pink_assessment_area_join[ 256 ];
	char where_location_join[ 256 ];
	char where_region_join[ 256 ];
	char where_abiotic_measurement_join[ 2048 ];
	char where_clause[ 4096 ];
	char from[ 512 ];
	char *sampling_point_table_name;
	char *collection_table_name;
	char *location_table_name;
	char *region_table_name;
	char *pink_assessment_area_table_name;
	char *abiotic_measurement_table_name;

	abiotic_measurement_table_name =
		get_table_name(	application_name,
				"throwtrap_abiotic_measurement" );

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
"lpad(collection_number,2,'0'),lpad(location_number,2,'0'),%s.collection_name,%s.location,region_code,pink_assessment_area_code,'',lpad(%s.site_number,2,'0'),%s.actual_latitude,%s.actual_longitude,'','','','','','',date_format(%s.anchor_date,'%cc/%ce/%cY'),%s.anchor_time,datatype,avg(value)",
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
		 abiotic_measurement_table_name,
		 collection_table_name,
		 location_table_name,
		 region_table_name,
		 pink_assessment_area_table_name );

	/* Where */
	/* ===== */

	/* Where quad_vegetation join */
	/* -------------------------- */
	sprintf( where_abiotic_measurement_join,
		 "%s.anchor_date = %s.anchor_date and			"
		 "%s.anchor_time = %s.anchor_time and			"
		 "%s.location = %s.location and				"
		 "%s.site_number = %s.site_number			",
		 abiotic_measurement_table_name,
		 sampling_point_table_name,
		 abiotic_measurement_table_name,
		 sampling_point_table_name,
		 abiotic_measurement_table_name,
		 sampling_point_table_name,
		 abiotic_measurement_table_name,
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
		 where_abiotic_measurement_join,
		 where_pink_assessment_area_join );

	/* Group */
	/* ===== */
	sprintf( group,
"lpad(collection_number,2,'0'),lpad(location_number,2,'0'),%s.collection_name,%s.location,region_code,pink_assessment_area_code,'',lpad(%s.site_number,2,'0'),%s.actual_latitude,%s.actual_longitude,'','','','','','',date_format(%s.anchor_date,'%cc/%ce/%cY'),%s.anchor_time,datatype",
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 '%', '%', '%',
		 sampling_point_table_name );

	sprintf( select_string,
		 "select %s from %s where %s group by %s;",
		 select,
		 from,
		 where_clause,
		 group );

	return strdup( select_string );

}

