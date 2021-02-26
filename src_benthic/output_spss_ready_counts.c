/* ---------------------------------------------------	*/
/* src_benthic/output_spss_ready_counts.c		*/
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
#include "hashtbl.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "benthic_species.h"
#include "benthic_library.h"
#include "benthic_spss.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define DEBUG 0
#define FILENAME_STEM_SHRIMP		"spss_ready_shrimp_crab_counts"
#define FILENAME_STEM_FISH		"spss_ready_fish_counts"
#define TEMP_FILE			"%s/spss_ready_temp_file_%d.csv"

/* Prototypes */
/* ---------- */
char *create_rows(		char *application_name,
				LIST *collection_list,
				LIST *project_list,
				char sum_sweeps_yn,
				char *appaserver_data_directory,
				pid_t process_id );

void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name,
				LIST *collection_list,
				LIST *project_list,
				char sum_sweeps_yn );

void output_spss_ready_counts_shrimp_crab(
				char *application_name,
				char *document_root_directory,
				int process_id,
				char sum_sweeps_yn,
				char *select_output_filename );

void output_spss_ready_counts_fish(
				char *application_name,
				char *document_root_directory,
				int process_id,
				char sum_sweeps_yn,
				char *select_output_filename );

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
	char sum_sweeps_yn;
	char *select_output_filename;

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s application process_name collection_list project_list sum_sweeps_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	collection_list_string = argv[ 3 ];
	project_list_string = argv[ 4 ];
	sum_sweeps_yn = *argv[ 5 ];

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
			sum_sweeps_yn );

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

	select_output_filename =
		create_rows(
			application_name,
			collection_list,
			project_list,
			sum_sweeps_yn,
			appaserver_parameter_file->
				appaserver_data_directory,
			getpid() );

	output_spss_ready_counts_shrimp_crab(
				application_name,
				appaserver_parameter_file->
					document_root,
				getpid(),
				sum_sweeps_yn,
				select_output_filename );

	output_spss_ready_counts_fish(
				application_name,
				appaserver_parameter_file->
					document_root,
				getpid(),
				sum_sweeps_yn,
				select_output_filename );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;

} /* main() */

void get_title_and_sub_title(
			char *title,
			char *sub_title,
			char *process_name,
			LIST *collection_list,
			LIST *project_list,
			char sum_sweeps_yn )
{
	sprintf( title,
		 "%s: %s",
		 format_initial_capital( title, process_name ),
		 (sum_sweeps_yn == 'y') ? "Sum Sweeps" : "Display Sweeps" );

	strcpy( sub_title,
		benthic_library_get_collection_project_combined_string(
			collection_list,
			project_list ) );

} /* get_title_and_sub_title() */

char *create_rows(	char *application_name,
			LIST *collection_list,
			LIST *project_list,
			char sum_sweeps_yn,
			char *appaserver_data_directory,
			pid_t process_id )
{
	static char select_output_filename[ 128 ];
	char sys_string[ 65536 ];

	/* Create the rows */
	/* --------------- */
	sprintf(select_output_filename,
	 	TEMP_FILE,
	 	appaserver_data_directory,
	 	process_id );

	sprintf( sys_string,
"select_spss_ready_counts \"%s\" \"%s\" \"%s\" \"%c\" > %s",
		 application_name,
		 list_display_delimited( collection_list, ',' ),
		 list_display_delimited( project_list, ',' ),
		 sum_sweeps_yn,
		 select_output_filename );

	system( sys_string );
	return select_output_filename;

} /* create_rows() */

void output_spss_ready_counts_shrimp_crab(
				char *application_name,
				char *document_root_directory,
				int process_id,
				char sum_sweeps_yn,
				char *select_output_filename )
{
	char *process_output_filename;
	char *ftp_filename;
	FILE *output_file;
	BENTHIC_SPSS *benthic_spss;
	LIST *faunal_group_name_list;
	SPSS_SAMPLING_POINT *spss_sampling_point;
	int all_done = 0;
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
		   application_http_prefix(
				application_name ),
		   appaserver_library_get_server_address(),
		   ( application_prepend_http_protocol_yn(
			application_name ) == 'y' ),
		   document_root_directory,
		   FILENAME_STEM_SHRIMP,
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

	faunal_group_name_list = list_new();
	list_append_pointer( faunal_group_name_list, "penaeid_shrimp" );
	list_append_pointer( faunal_group_name_list, "caridean_shrimp" );
	list_append_pointer( faunal_group_name_list, "crab" );

	benthic_spss = benthic_spss_new(
				application_name,
				faunal_group_name_list,
				select_output_filename );

/*
	sprintf(process_output_filename,
	 	OUTPUT_SHRIMP_CRAB,
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

	fprintf(	output_file,
			"%s", 
			benthic_spss_get_heading_string(
				benthic_spss->faunal_group_list,
				sum_sweeps_yn ) );

	while( ( spss_sampling_point =
		benthic_spss_get_spss_sampling_point(
			&all_done,
			benthic_spss->input_file,
			benthic_spss->faunal_group_list,
			sum_sweeps_yn ) ) )
	{
		fprintf(output_file,
			"%s\n", 
			benthic_spss_get_sampling_point_output_line(
				spss_sampling_point,
				benthic_spss->faunal_group_list,
				sum_sweeps_yn ) );

		benthic_spss_sampling_point_free(
			benthic_spss->spss_sampling_point );
	}

	fclose( output_file );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
"Shrimp and crab: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );

} /* output_spss_ready_counts_shrimp_crab() */

void output_spss_ready_counts_fish(
				char *application_name,
				char *document_root_directory,
				int process_id,
				char sum_sweeps_yn,
				char *select_output_filename )
{
	char *process_output_filename;
	char *ftp_filename;
	FILE *output_file;
	BENTHIC_SPSS *benthic_spss;
	LIST *faunal_group_name_list;
	SPSS_SAMPLING_POINT *spss_sampling_point;
	int all_done = 0;
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
		   application_http_prefix(
				application_name ),
		   appaserver_library_get_server_address(),
		   ( application_prepend_http_protocol_yn(
			application_name ) == 'y' ),
		   document_root_directory,
		   FILENAME_STEM_FISH,
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

	faunal_group_name_list = list_new();
	list_append_pointer( faunal_group_name_list, "fish" );

	benthic_spss = benthic_spss_new(
				application_name,
				faunal_group_name_list,
				select_output_filename );

/*
	sprintf(process_output_filename,
	 	OUTPUT_FISH,
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

	fprintf(	output_file,
			"%s", 
			benthic_spss_get_heading_string(
				benthic_spss->faunal_group_list,
				sum_sweeps_yn ) );

	while( ( spss_sampling_point =
		benthic_spss_get_spss_sampling_point(
			&all_done,
			benthic_spss->input_file,
			benthic_spss->faunal_group_list,
			sum_sweeps_yn ) ) )
	{
		fprintf(output_file,
			"%s\n", 
			benthic_spss_get_sampling_point_output_line(
				spss_sampling_point,
				benthic_spss->faunal_group_list,
				sum_sweeps_yn ) );

		benthic_spss_sampling_point_free(
			benthic_spss->spss_sampling_point );
	}

	fclose( output_file );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
"Fish: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );

} /* output_spss_ready_counts_fish() */

