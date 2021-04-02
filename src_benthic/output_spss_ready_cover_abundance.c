/* ---------------------------------------------------	*/
/* src_benthic/output_spss_ready_cover_abundance.c	*/
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
#include "benthic_species.h"
#include "benthic_library.h"
#include "braun_blanque.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define FILENAME_STEM			"spss_ready_bbq"

/* Prototypes */
/* ---------- */
FILE *get_quad_input_pipe(
				char *application_name,
				LIST *collection_list,
				LIST *project_list );

FILE *get_avg_input_pipe(
				char *application_name,
				LIST *collection_list,
				LIST *project_list );

char *get_select_string(	char *select,
				char *from,
				char *where_clause,
				char *group_by );

FILE *get_input_pipe(
				char *application_name,
				LIST *collection_list,
				LIST *project_list,
				boolean avg_quads_boolean );

char *get_quad_substrate_select_string(
				char *application_name,
				LIST *collection_list,
				LIST *project_list );

char *get_sampling_point_substrate_select_string(
				char *application_name,
				LIST *collection_list,
				LIST *project_list );

char *get_quad_database_select_string(
				char *application_name,
				LIST *collection_list,
				LIST *project_list,
				char *folder_name,
				char *select_column_name_list_string );

char *get_avg_database_select_string(
				char *application_name,
				LIST *collection_list,
				LIST *project_list,
				char *folder_name,
				char *select_column_name_list_string );

void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name,
				LIST *collection_list,
				LIST *project_list,
				boolean avg_quads_boolean );

void output_spss_ready_cover_abundance(
				char *application_name,
				char *document_root_directory,
				int process_id,
				LIST *collection_list,
				LIST *project_list,
				boolean avg_quads_boolean );

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
	boolean avg_quads_boolean = 0;

	if ( argc < 5 )
	{
		fprintf( stderr, 
"Usage: %s application process_name collection_list project_list [avg_quads_yn]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	collection_list_string = argv[ 3 ];
	project_list_string = argv[ 4 ];

	if ( argc == 6 )
	{
		avg_quads_boolean = ( *argv[ 5 ] == 'y' );
	}

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
			project_list,
			avg_quads_boolean );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
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

	output_spss_ready_cover_abundance(
				application_name,
				appaserver_parameter_file->
					document_root,
				getpid(),
				collection_list,
				project_list,
				avg_quads_boolean );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void output_spss_ready_cover_abundance(
				char *application_name,
				char *document_root_directory,
				int process_id,
				LIST *collection_list,
				LIST *project_list,
				boolean avg_quads_boolean )
{
	char *process_output_filename;
	char *ftp_filename;
	FILE *input_pipe;
	FILE *output_file;
	char input_buffer[ 1024 ];
	BRAUN_BLANQUE *braun_blanque;
	boolean extra_output_needed = 0;
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
		   application_http_prefix(
				application_name ),
		   appaserver_library_get_server_address(),
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

	braun_blanque = braun_blanque_new( application_name );

/*
	sprintf(process_output_filename,
	 	OUTPUT_FILE,
	 	appaserver_mount_point,
	 	application_name,
	 	process_id );
*/

	if ( ! ( output_file = fopen( process_output_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			process_output_filename );
		document_close();
		exit( 1 );
	}

	if ( !avg_quads_boolean )
	{
		fprintf( output_file,
"collect_num,site_num,collection,location,latitude,longitude,region,pink_area,cell,Quad,season,Substrate,MAXcanopy,AVGcanopy,%s,%s\n",
		 	braun_blanque_group_heading_label_list_display(
				braun_blanque->vegetation_group_list ),
		 	braun_blanque_heading_label_list_display(
				braun_blanque->vegetation_list ) );
	}
	else
	{
		fprintf( output_file,
"collect_num,site_num,collection,location,latitude,longitude,region,pink_area,cell,season,Substrate,MAXavgcanopy,AVGavgcanopy,%s,%s\n",
		 	braun_blanque_group_heading_label_list_display(
				braun_blanque->vegetation_group_list ),
		 	braun_blanque_heading_label_list_display(
				braun_blanque->vegetation_list ) );
	}

	input_pipe = get_input_pipe(
				application_name,
				collection_list,
				project_list,
				avg_quads_boolean );

	while( get_line( input_buffer, input_pipe ) )
	{
		extra_output_needed =
			braun_blanque_set_input_buffer(
				output_file,
				braun_blanque->output_record,
				braun_blanque->input_record,
				input_buffer,
				braun_blanque->vegetation_list,
				braun_blanque->vegetation_group_list,
				braun_blanque->master_substrate_list,
				braun_blanque->vegetation_array,
				avg_quads_boolean );

	}

	if ( extra_output_needed )
	{
		braun_blanque_output_record(
			output_file,
			braun_blanque->output_record,
			list_length( braun_blanque->vegetation_list ),
			list_length( braun_blanque->vegetation_group_list ),
			braun_blanque->vegetation_array,
			avg_quads_boolean );
	}

	fclose( output_file );
	pclose( input_pipe );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
"Cover Abundance: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );

} /* output_spss_ready_cover_abundance() */

void get_title_and_sub_title(
			char *title,
			char *sub_title,
			char *process_name,
			LIST *collection_list,
			LIST *project_list,
			boolean avg_quads_boolean )
{
	format_initial_capital( title, process_name );

	strcpy( sub_title,
		benthic_library_get_collection_project_combined_string(
			collection_list,
			project_list ) );

	if ( avg_quads_boolean )
	{
		sprintf( sub_title + strlen( sub_title ),
			 " (Average Quads Per Sampling Point)" );
	}

} /* get_title_and_sub_title() */

char *get_quad_database_select_string(
			char *application_name,
			LIST *collection_list,
			LIST *project_list,
			char *folder_name,
			char *select_column_name_list_string )
{
	char select[ 2048 ];
	char *where_or_clause;
	char where_collection_join[ 256 ];
	char where_pink_assessment_area_join[ 256 ];
	char where_location_join[ 256 ];
	char where_region_join[ 256 ];
	char where_quad_vegetation_join[ 2048 ];
	char where_clause[ 4096 ];
	char from[ 512 ];
	char *sampling_point_table_name;
	char *table_name;
	char *collection_table_name;
	char *location_table_name;
	char *region_table_name;
	char *pink_assessment_area_table_name;

	sampling_point_table_name =
		get_table_name( application_name, "sampling_point" );

	table_name =
		get_table_name( application_name, folder_name );

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
"lpad(collection_number,2,'0'),lpad(location_number,2,'0'),%s.collection_name,%s.location,lpad(%s.site_number,2,'0'),lpad(%s.quad,2,'0'),%s.actual_latitude,%s.actual_longitude,region_code,pink_assessment_area_code,%s.vegetation_name,%s",
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 table_name,
		 select_column_name_list_string );

	/* From */
	/* ==== */
	sprintf( from,
		 "%s,%s,%s,%s,%s,%s",
		 sampling_point_table_name,
		 table_name,
		 collection_table_name,
		 location_table_name,
		 region_table_name,
		 pink_assessment_area_table_name );

	/* Where */
	/* ===== */

	/* Where quad_vegetation join */
	/* -------------------------- */
	sprintf( where_quad_vegetation_join,
		 "%s.anchor_date = %s.anchor_date and			"
		 "%s.anchor_time = %s.anchor_time and			"
		 "%s.location = %s.location and				"
		 "%s.site_number = %s.site_number 			",
		 table_name,
		 sampling_point_table_name,
		 table_name,
		 sampling_point_table_name,
		 table_name,
		 sampling_point_table_name,
		 table_name,
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
		braun_blanque_or_sequence_get_where_clause(
			application_name,
			collection_list,
			project_list );

	sprintf( where_clause,
		 "(%s) and %s and %s and %s and %s and %s",
		 where_or_clause,
		 where_collection_join,
		 where_location_join,
		 where_region_join,
		 where_quad_vegetation_join,
		 where_pink_assessment_area_join );

	return get_select_string(
			select,
			from,
			where_clause,
			(char *)0 /* group_by */ );

} /* get_quad_database_select_string() */

char *get_select_string(	char *select,
				char *from,
				char *where_clause,
				char *group_by )
{
	char select_string[ 65536 ];

	if ( !group_by )
	{
		sprintf( select_string,
		 	"select %s from %s where %s;",
		 	select,
		 	from,
		 	where_clause );
	}
	else
	{
		sprintf( select_string,
		 	"select %s from %s where %s group by %s;",
		 	select,
		 	from,
		 	where_clause,
			group_by );
	}

	return strdup( select_string );

} /* get_select_string() */

char *get_quad_substrate_select_string(
			char *application_name,
			LIST *collection_list,
			LIST *project_list )
{
	char select[ 2048 ];
	char *where_or_clause;
	char where_collection_join[ 256 ];
	char where_pink_assessment_area_join[ 256 ];
	char where_location_join[ 256 ];
	char where_region_join[ 256 ];
	char where_quad_substrate_join[ 2048 ];
	char where_clause[ 4096 ];
	char from[ 512 ];
	char *sampling_point_table_name;
	char *quad_substrate_table_name;
	char *collection_table_name;
	char *location_table_name;
	char *region_table_name;
	char *pink_assessment_area_table_name;

	sampling_point_table_name =
		get_table_name( application_name, "sampling_point" );

	quad_substrate_table_name =
		get_table_name( application_name, "quad_substrate" );

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
"lpad(collection_number,2,'0'),lpad(location_number,2,'0'),%s.collection_name,%s.location,lpad(%s.site_number,2,'0'),lpad(%s.quad,2,'0'),%s.actual_latitude,%s.actual_longitude,region_code,pink_assessment_area_code,'%s',%s.substrate",
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 quad_substrate_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 BRAUN_BLANQUE_SUBSTRATE_KEY,
		 quad_substrate_table_name );

	/* From */
	/* ==== */
	sprintf( from,
		 "%s,%s,%s,%s,%s,%s",
		 sampling_point_table_name,
		 quad_substrate_table_name,
		 collection_table_name,
		 location_table_name,
		 region_table_name,
		 pink_assessment_area_table_name );

	/* Where */
	/* ===== */

	/* Where quad_substrate join */
	/* -------------------------- */
	sprintf( where_quad_substrate_join,
		 "%s.anchor_date = %s.anchor_date and			"
		 "%s.anchor_time = %s.anchor_time and			"
		 "%s.location = %s.location and				"
		 "%s.site_number = %s.site_number			",
		 quad_substrate_table_name,
		 sampling_point_table_name,
		 quad_substrate_table_name,
		 sampling_point_table_name,
		 quad_substrate_table_name,
		 sampling_point_table_name,
		 quad_substrate_table_name,
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
		braun_blanque_or_sequence_get_where_clause(
			application_name,
			collection_list,
			project_list );

	sprintf( where_clause,
		 "(%s) and %s and %s and %s and %s and %s",
		 where_or_clause,
		 where_collection_join,
		 where_location_join,
		 where_region_join,
		 where_quad_substrate_join,
		 where_pink_assessment_area_join );

	return get_select_string(
			select,
			from,
			where_clause,
			(char *)0 /* group_by */ );

} /* get_quad_substrate_select_string() */

FILE *get_input_pipe(
			char *application_name,
			LIST *collection_list,
			LIST *project_list,
			boolean avg_quads_boolean )
{
	if ( avg_quads_boolean )
	{
		return get_avg_input_pipe(
			application_name,
			collection_list,
			project_list );
	}
	else
	{
		return get_quad_input_pipe(
			application_name,
			collection_list,
			project_list );
	}
} /* get_input_pipe() */

FILE *get_quad_input_pipe(
			char *application_name,
			LIST *collection_list,
			LIST *project_list )
{
	char *quad_vegetation_select_string;
	char *quad_vegetation_total_select_string;
	char *substrate_select_string;
	char sys_string[ 65536 ];

	quad_vegetation_select_string =
		get_quad_database_select_string(
			application_name,
			collection_list,
			project_list,
			"quad_vegetation",
			"abundance_rating,vegetation_height_cm" );

	quad_vegetation_total_select_string =
		get_quad_database_select_string(
			application_name,
			collection_list,
			project_list,
			"quad_vegetation_total",
			"abundance_rating" );

	substrate_select_string =
		get_quad_substrate_select_string(
				application_name,
				collection_list,
				project_list );

	sprintf( sys_string,
"(echo \"%s\" | sql.e ','; echo \"%s\" | sql.e ','; echo \"%s\" | sql.e ',') | sort",
		 quad_vegetation_select_string,
		 quad_vegetation_total_select_string,
		 substrate_select_string );

	return popen( sys_string, "r" );
} /* get_quad_input_pipe() */

FILE *get_avg_input_pipe(
			char *application_name,
			LIST *collection_list,
			LIST *project_list )
{
	char *avg_vegetation_select_string;
	char *avg_vegetation_total_select_string;
	char *substrate_select_string;
	char sys_string[ 65536 ];

	avg_vegetation_select_string =
		get_avg_database_select_string(
			application_name,
			collection_list,
			project_list,
			"quad_vegetation",
			"avg(abundance_rating),avg(vegetation_height_cm)" );

	avg_vegetation_total_select_string =
		get_avg_database_select_string(
			application_name,
			collection_list,
			project_list,
			"quad_vegetation_total",
			"avg(abundance_rating)" );

	substrate_select_string =
		get_sampling_point_substrate_select_string(
				application_name,
				collection_list,
				project_list );

	sprintf( sys_string,
"(echo \"%s\" | sql.e ','; echo \"%s\" | sql.e ','; echo \"%s\" | sql.e ',') | sort",
		 avg_vegetation_select_string,
		 avg_vegetation_total_select_string,
		 substrate_select_string );

	return popen( sys_string, "r" );

} /* get_avg_input_pipe() */

char *get_sampling_point_substrate_select_string(
			char *application_name,
			LIST *collection_list,
			LIST *project_list )
{
	char select[ 2048 ];
	char *where_or_clause;
	char where_collection_join[ 256 ];
	char where_pink_assessment_area_join[ 256 ];
	char where_location_join[ 256 ];
	char where_region_join[ 256 ];
	char where_quad_substrate_join[ 2048 ];
	char where_clause[ 4096 ];
	char from[ 512 ];
	char *sampling_point_table_name;
	char *quad_substrate_table_name;
	char *collection_table_name;
	char *location_table_name;
	char *region_table_name;
	char *pink_assessment_area_table_name;

	sampling_point_table_name =
		get_table_name( application_name, "sampling_point" );

	quad_substrate_table_name =
		get_table_name( application_name, "quad_substrate" );

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
"distinct lpad(collection_number,2,'0'),lpad(location_number,2,'0'),%s.collection_name,%s.location,lpad(%s.site_number,2,'0'),%s.actual_latitude,%s.actual_longitude,region_code,pink_assessment_area_code,'%s',%s.substrate",
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 BRAUN_BLANQUE_SUBSTRATE_KEY,
		 quad_substrate_table_name );

	/* From */
	/* ==== */
	sprintf( from,
		 "%s,%s,%s,%s,%s,%s",
		 sampling_point_table_name,
		 quad_substrate_table_name,
		 collection_table_name,
		 location_table_name,
		 region_table_name,
		 pink_assessment_area_table_name );

	/* Where */
	/* ===== */

	/* Where quad_substrate join */
	/* -------------------------- */
	sprintf( where_quad_substrate_join,
		 "%s.anchor_date = %s.anchor_date and			"
		 "%s.anchor_time = %s.anchor_time and			"
		 "%s.location = %s.location and				"
		 "%s.site_number = %s.site_number			",
		 quad_substrate_table_name,
		 sampling_point_table_name,
		 quad_substrate_table_name,
		 sampling_point_table_name,
		 quad_substrate_table_name,
		 sampling_point_table_name,
		 quad_substrate_table_name,
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
		braun_blanque_or_sequence_get_where_clause(
			application_name,
			collection_list,
			project_list );

	sprintf( where_clause,
		 "(%s) and %s and %s and %s and %s and %s",
		 where_or_clause,
		 where_collection_join,
		 where_location_join,
		 where_region_join,
		 where_quad_substrate_join,
		 where_pink_assessment_area_join );

	return get_select_string(
			select,
			from,
			where_clause,
			(char *)0 /* group_by */ );

} /* get_sampling_point_substrate_select_string() */

char *get_avg_database_select_string(
			char *application_name,
			LIST *collection_list,
			LIST *project_list,
			char *folder_name,
			char *select_column_name_list_string )
{
	char select[ 2048 ];
	char *where_or_clause;
	char where_collection_join[ 256 ];
	char where_pink_assessment_area_join[ 256 ];
	char where_location_join[ 256 ];
	char where_region_join[ 256 ];
	char where_quad_vegetation_join[ 2048 ];
	char where_clause[ 4096 ];
	char from[ 512 ];
	char group_by[ 512 ];
	char *sampling_point_table_name;
	char *table_name;
	char *collection_table_name;
	char *location_table_name;
	char *region_table_name;
	char *pink_assessment_area_table_name;

	sampling_point_table_name =
		get_table_name( application_name, "sampling_point" );

	table_name =
		get_table_name( application_name, folder_name );

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
"lpad(collection_number,2,'0'),lpad(location_number,2,'0'),%s.collection_name,%s.location,lpad(%s.site_number,2,'0'),%s.actual_latitude,%s.actual_longitude,region_code,pink_assessment_area_code,%s.vegetation_name,%s",
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 table_name,
		 select_column_name_list_string );

	/* From */
	/* ==== */
	sprintf( from,
		 "%s,%s,%s,%s,%s,%s",
		 sampling_point_table_name,
		 table_name,
		 collection_table_name,
		 location_table_name,
		 region_table_name,
		 pink_assessment_area_table_name );

	/* Where */
	/* ===== */

	/* Where quad_vegetation join */
	/* -------------------------- */
	sprintf( where_quad_vegetation_join,
		 "%s.anchor_date = %s.anchor_date and			"
		 "%s.anchor_time = %s.anchor_time and			"
		 "%s.location = %s.location and				"
		 "%s.site_number = %s.site_number			",
		 table_name,
		 sampling_point_table_name,
		 table_name,
		 sampling_point_table_name,
		 table_name,
		 sampling_point_table_name,
		 table_name,
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
		braun_blanque_or_sequence_get_where_clause(
			application_name,
			collection_list,
			project_list );

	sprintf( where_clause,
		 "(%s) and %s and %s and %s and %s and %s",
		 where_or_clause,
		 where_collection_join,
		 where_location_join,
		 where_region_join,
		 where_quad_vegetation_join,
		 where_pink_assessment_area_join );

	/* Group By */
	/* ======== */
	sprintf( group_by,
"lpad(collection_number,2,'0'),lpad(location_number,2,'0'),%s.collection_name,%s.location,lpad(%s.site_number,2,'0'),%s.vegetation_name",
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 table_name );

	return get_select_string(
			select,
			from,
			where_clause,
			group_by );

} /* get_avg_database_select_string() */

