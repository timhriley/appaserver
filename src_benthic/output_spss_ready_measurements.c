/* ---------------------------------------------------	*/
/* src_benthic/output_spss_ready_measurements.c		*/
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
#include "benthic_library.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define PROJECT			"FIAN"

/* Prototypes */
/* ---------- */
void output_spss_ready_measurements_sys_string(
				char *output_filename,
				char *sys_string,
				LIST *sampling_point_datatype_list,
				DICTIONARY *abiotic_dictionary );

DICTIONARY *get_sys_string_abiotic_dictionary(
				char *sys_string );

LIST *get_sampling_point_datatype_list(
				char *application_name );

char *get_where_collection_project(
				char *application_name,
				LIST *collection_list,
				LIST *project_list );

DICTIONARY *get_abiotic_dictionary(
				char *application_name,
				LIST *collection_list,
				LIST *project_list );

char *get_abiotic_measurement_key(
				char *location,
				char *site_number,
				char *project,
				char *collection_name,
				char *datatype );

char *get_sys_string(		char *application_name,
				LIST *collection_list,
				LIST *project_list );

void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name,
				LIST *collection_list,
				LIST *project_list );

void output_spss_ready_measurements(
				char *application_name,
				char *collection_list_string,
				char *project_list_string,
				char *document_root_directory,
				int process_id,
				char *process_name,
				char join_abiotic_yn );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *collection_list_string;
	char *project_list_string;
	char join_abiotic_yn;

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s application process_name collection_list project_list join_aboitic_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	collection_list_string = argv[ 3 ];
	project_list_string = argv[ 4 ];
	join_abiotic_yn = *argv[ 5 ];

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

	output_spss_ready_measurements(
				application_name,
				collection_list_string,
				project_list_string,
				appaserver_parameter_file->
					document_root,
				getpid(),
				process_name,
				join_abiotic_yn );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void output_spss_ready_measurements(
				char *application_name,
				char *collection_list_string,
				char *project_list_string,
				char *document_root_directory,
				int process_id,
				char *process_name,
				char join_abiotic_yn )
{
	char *ftp_filename;
	char *output_filename;
	FILE *output_file;
	char title[ 128 ];
	char sub_title[ 65536 ];
	LIST *collection_list;
	LIST *project_list;
	char *sys_string;
	DICTIONARY *abiotic_dictionary = {0};
	LIST *sampling_point_datatype_list = {0};
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

	if ( join_abiotic_yn == 'y' )
	{
		abiotic_dictionary =
			get_abiotic_dictionary(
				application_name,
				collection_list,
				project_list );

		sampling_point_datatype_list =
			get_sampling_point_datatype_list(
				application_name );

	}

/*
	sprintf(output_filename, 
	 	OUTPUT_TEMPLATE,
	 	appaserver_mount_point,
	 	application_name,
	 	process_id );
*/

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			output_filename );
		document_close();
		exit( 1 );
	}

	fprintf( output_file,
"collect_num,site_num,collection,location,region,pink_area,cell,sweep,species,size,damaged,gender,total_ind,total_gravid" );

	if ( list_length( sampling_point_datatype_list ) )
	{
		fprintf(	output_file,
				",%s\n",
				list_display_delimited(
					sampling_point_datatype_list, ',' ) );
	}
	else
	{
		fprintf( output_file, "\n" );
	}

	fclose( output_file );

	sys_string = 
		get_sys_string(	application_name,
				collection_list,
				project_list );

	output_spss_ready_measurements_sys_string(
		output_filename,
		sys_string,
		sampling_point_datatype_list,
		abiotic_dictionary );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
"&lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );

} /* output_spss_ready_measurements() */

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

} /* get_title_and_sub_title() */

char *get_sys_string(	char *application_name,
			LIST *collection_list,
			LIST *project_list )
{
	static char sys_string[ 65536 ];
	char where_clause[ 65536 ];
	char select[ 4096 ];
	char *where_collection_project;
	char where_collection_join[ 4096 ];
	char where_pink_assessment_area_join[ 4096 ];
	char where_location_join[ 4096 ];
	char where_region_join[ 4096 ];
	char where_sweep_join[ 4096 ];
	char where_replicate_join[ 4096 ];
	char where_species_measurement_join[ 4096 ];
	char where_species_count_join[ 4096 ];
	char from[ 512 ];
	char order_clause[ 512 ];
	char *sweep_table_name;
	char *throwtrap_replicate_table_name;
	char *species_count_table_name;
	char *species_measurement_table_name;
	char *collection_table_name;
	char *location_table_name;
	char *region_table_name;
	char *pink_assessment_area_table_name;
	char *sampling_point_table_name;

	sampling_point_table_name =
		get_table_name( application_name, "sampling_point" );

	sweep_table_name =
		get_table_name( application_name, "sweep" );

	throwtrap_replicate_table_name =
		get_table_name( application_name, "throwtrap_replicate" );

	species_count_table_name =
		get_table_name( application_name, "species_count" );

	species_measurement_table_name =
		get_table_name( application_name, "species_measurement" );

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
"collection_number,location_number,%s.collection_name,%s.location,region_code,pink_assessment_area_code,%s.site_number,%s.sweep_number,concat( %s.family_name, ' ', %s.genus_name, ' ', %s.species_name ), length_millimeters,damaged_yn,gender,count_per_square_meter,gravid_count",
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 species_measurement_table_name,
		 species_measurement_table_name,
		 species_measurement_table_name,
		 species_measurement_table_name );

	/* From */
	/* ==== */
	sprintf( from,
		 "%s,%s,%s,%s,%s,%s,%s,%s,%s",
		 sampling_point_table_name,
		 throwtrap_replicate_table_name,
		 sweep_table_name,
		 species_count_table_name,
		 species_measurement_table_name,
		 collection_table_name,
		 location_table_name,
		 region_table_name,
		 pink_assessment_area_table_name );

	/* Where */
	/* ===== */

	/* Where species_count join */
	/* ------------------------ */
	sprintf( where_species_count_join,
		 "%s.anchor_date = %s.anchor_date and			"
		 "%s.anchor_time = %s.anchor_time and			"
		 "%s.location = %s.location and				"
		 "%s.site_number = %s.site_number and			"
		 "%s.sweep_number = %s.sweep_number and			"
		 "%s.family_name = %s.family_name and			"
		 "%s.genus_name = %s.genus_name and			"
		 "%s.species_name = %s.species_name			",
		 sweep_table_name,
		 species_count_table_name,
		 sweep_table_name,
		 species_count_table_name,
		 sweep_table_name,
		 species_count_table_name,
		 sweep_table_name,
		 species_count_table_name,
		 sweep_table_name,
		 species_count_table_name,
		 species_count_table_name,
		 species_measurement_table_name,
		 species_count_table_name,
		 species_measurement_table_name,
		 species_count_table_name,
		 species_measurement_table_name );

	/* Where species_measurement join */
	/* ------------------------------ */
	sprintf( where_species_measurement_join,
		 "%s.anchor_date = %s.anchor_date and			"
		 "%s.anchor_time = %s.anchor_time and			"
		 "%s.location = %s.location and				"
		 "%s.site_number = %s.site_number and			"
		 "%s.sweep_number = %s.sweep_number			",
		 sweep_table_name,
		 species_measurement_table_name,
		 sweep_table_name,
		 species_measurement_table_name,
		 sweep_table_name,
		 species_measurement_table_name,
		 sweep_table_name,
		 species_measurement_table_name,
		 sweep_table_name,
		 species_measurement_table_name );

	/* Where replicate join */
	/* -------------------- */
	sprintf( where_replicate_join,
		 "%s.anchor_date = %s.anchor_date and			"
		 "%s.anchor_time = %s.anchor_time and			"
		 "%s.location = %s.location and				"
		 "%s.site_number = %s.site_number			",
		 sweep_table_name,
		 sampling_point_table_name,
		 sweep_table_name,
		 sampling_point_table_name,
		 sweep_table_name,
		 sampling_point_table_name,
		 sweep_table_name,
		 sampling_point_table_name );

	/* Where sweep join */
	/* ---------------- */
	sprintf( where_sweep_join,
		 "%s.anchor_date = %s.anchor_date and			"
		 "%s.anchor_time = %s.anchor_time and			"
		 "%s.location = %s.location and				"
		 "%s.site_number = %s.site_number and			"
		 "%s.replicate_number = %s.replicate_number		",
		 sweep_table_name,
		 throwtrap_replicate_table_name,
		 sweep_table_name,
		 throwtrap_replicate_table_name,
		 sweep_table_name,
		 throwtrap_replicate_table_name,
		 sweep_table_name,
		 throwtrap_replicate_table_name,
		 sweep_table_name,
		 throwtrap_replicate_table_name );

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

	where_collection_project =
		get_where_collection_project(
			application_name,
			collection_list,
			project_list );

	sprintf( where_clause,
		 "(%s) and %s and %s and %s and %s and %s and %s and %s and %s",
		 where_collection_project,
		 where_replicate_join,
		 where_sweep_join,
		 where_collection_join,
		 where_location_join,
		 where_region_join,
		 where_species_measurement_join,
		 where_species_count_join,
		 where_pink_assessment_area_join );

	sprintf( order_clause,
"collection_number,location_number,%s.collection_name,%s.location,%s.site_number,%s.sweep_number,%s.family_name,%s.genus_name,%s.species_name",
		 sampling_point_table_name,
		 sampling_point_table_name,
		 sampling_point_table_name,
		 species_measurement_table_name,
		 species_measurement_table_name,
		 species_measurement_table_name,
		 species_measurement_table_name );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\"	|"
		 "sql.e ','						|"
		 "sed 's/no_measurement//'				|"
		 "cat 							 ",
		 select,
		 from,
		 where_clause,
		 order_clause );

	return sys_string;

} /* get_sys_string() */

char *get_abiotic_measurement_key(
				char *location,
				char *site_number,
				char *project,
				char *collection_name,
				char *datatype )
{
	static char key[ 256 ];

	sprintf( key,
		 "%s|%s|%s|%s|%s",
		 location,
		 site_number,
		 project,
		 collection_name,
		 datatype );
	return key;

} /* get_abiotic_measurement_key() */

DICTIONARY *get_abiotic_dictionary(
				char *application_name,
				LIST *collection_list,
				LIST *project_list )
{
	char sys_string[ 4096 ];
	char select[ 512 ];
	char where_join[ 512 ];
	char *sampling_point_abiotic_measurement;
	char *sampling_point;
	char *where_collection_project;

	sampling_point_abiotic_measurement =
		"sampling_point_abiotic_measurement";

	sampling_point = "sampling_point";

	sprintf( select,
"%s.location,%s.site_number,%s.project,%s.collection_name,%s.datatype,value",
	sampling_point,
	sampling_point,
	sampling_point,
	sampling_point,
	sampling_point_abiotic_measurement );

	sprintf( where_join,
		 "%s.anchor_date = %s.anchor_date and		"
		 "%s.anchor_time = %s.anchor_time and		"
		 "%s.location = %s.location and			"
		 "%s.site_number = %s.site_number 		",
		 sampling_point_abiotic_measurement,
		 sampling_point,
		 sampling_point_abiotic_measurement,
		 sampling_point,
		 sampling_point_abiotic_measurement,
		 sampling_point,
		 sampling_point_abiotic_measurement,
		 sampling_point );

	where_collection_project =
		get_where_collection_project(
			application_name,
			collection_list,
			project_list );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s,%s			"
		 "			where=\"%s and %s\"		",
		 application_name,
		 select,
		 sampling_point_abiotic_measurement,
		 sampling_point,
		 where_join,
		 where_collection_project );

	return get_sys_string_abiotic_dictionary( sys_string );

} /* get_abiotic_dictionary() */

DICTIONARY *get_sys_string_abiotic_dictionary( char *sys_string )
{
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	char location[ 128 ];
	char site_number[ 128 ];
	char project[ 128 ];
	char collection_name[ 128 ];
	char datatype[ 128 ];
	char value[ 128 ];
	char *key;
	DICTIONARY *dictionary = dictionary_large_new();

	input_pipe = popen( sys_string, "r" );

/*
"%s.location,%s.site_number,%s.project,%s.collection_name,%s.datatype,value",
*/
	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	location,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	site_number,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	project,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	collection_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		piece(	datatype,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			4 );

		piece(	value,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			5 );

		key = get_abiotic_measurement_key(
				location,
				site_number,
				project,
				collection_name,
				datatype );

		dictionary_set_pointer(
			dictionary,
			strdup( key ),
			strdup( value ) );
	}

	pclose( input_pipe );

	return dictionary;

} /* get_sys_string_abiotic_dictionary() */

char *get_where_collection_project(
			char *application_name,
			LIST *collection_list,
			LIST *project_list )
{
	QUERY_OR_SEQUENCE *query_or_sequence;
	LIST *attribute_name_list;
	char attribute_name[ 128 ];
	attribute_name_list = list_new();
	char *sampling_point_table_name;

	sampling_point_table_name =
		get_table_name( application_name, "sampling_point" );

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

} /* get_where_collection_project() */

LIST *get_sampling_point_datatype_list(
				char *application_name )
{
	char sys_string[ 1024 ];
	char *select = "datatype.datatype";
	char *where = "sampling_point_datatype.datatype = datatype.datatype";
	char *folder = "sampling_point_datatype,datatype";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			"
		 "			order=display_order		",
		 application_name,
		 select,
		 folder,
		 where );

	return pipe2list( sys_string );

} /* get_sampling_point_datatype_list() */

void output_spss_ready_measurements_sys_string(
		char *output_filename,
		char *sys_string,
		LIST *sampling_point_datatype_list,
		DICTIONARY *abiotic_dictionary )
{
	char output_sys_string[ 256 ];
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];
	char *datatype;
	char *value;
	char location[ 128 ];
	char site_number[ 128 ];
	char collection_name[ 128 ];
	char *key;

	input_pipe = popen( sys_string, "r" );

	sprintf( output_sys_string,
		 "cat >> %s",
		 output_filename );

	output_pipe = popen( output_sys_string, "w" );

	while( get_line( input_buffer, input_pipe ) )
	{
		fprintf( output_pipe, "%s", input_buffer );

		if ( list_rewind( sampling_point_datatype_list ) )
		{
			if ( !piece(	location,
					',',
					input_buffer,
					3 ) )
			{
				fprintf( stderr,
"ERROR in %s/%s()/%d: cannot piece(3) in %s\n",
				 	__FILE__,
				 	__FUNCTION__,
				 	__LINE__,
				 	input_buffer );
				pclose( input_pipe );
				pclose( output_pipe );
				exit( 1 );
			}

			if ( !piece(	site_number,
					',',
					input_buffer,
					6 ) )
			{
				fprintf( stderr,
"ERROR in %s/%s()/%d: cannot piece(6) in %s\n",
				 	__FILE__,
				 	__FUNCTION__,
				 	__LINE__,
				 	input_buffer );
				pclose( input_pipe );
				pclose( output_pipe );
				exit( 1 );
			}

			if ( !piece(	collection_name,
					',',
					input_buffer,
					2 ) )
			{
				fprintf( stderr,
"ERROR in %s/%s()/%d: cannot piece(2) in %s\n",
				 	__FILE__,
				 	__FUNCTION__,
				 	__LINE__,
				 	input_buffer );
				pclose( input_pipe );
				pclose( output_pipe );
				exit( 1 );
			}

			do {
				datatype = list_get_pointer(
						sampling_point_datatype_list );

				key = get_abiotic_measurement_key(
					location,
					site_number,
					PROJECT,
					collection_name,
					datatype );

				value = dictionary_fetch(
						abiotic_dictionary,
						key );

				if ( value && *value )
				{
					fprintf( output_pipe, ",%s", value );
				}
				else
				{
					fprintf( output_pipe, "," );
				}

			} while( list_next( sampling_point_datatype_list ) );
			fprintf( output_pipe, "\n" );
		}
		else
		{
			fprintf( output_pipe, "\n" );
		}
	}

	pclose( output_pipe );
	pclose( input_pipe );

} /* output_spss_ready_measurements_sys_string() */

