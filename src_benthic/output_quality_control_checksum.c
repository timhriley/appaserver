/* ---------------------------------------------------	*/
/* src_benthic/output_quality_control_checksum.c	*/
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
#include "appaserver_link.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define FILENAME_STEM_PER_SITE			"checksum_per_site"
#define FILENAME_STEM_PER_SWEEP			"checksum_per_sweep"

/* Prototypes */
/* ---------- */
char *get_count_sys_string(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *heading );

char *get_measurement_sys_string(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *heading );

char *get_per_site_sys_string(	char *application_name,
				char *project,
				char *collection_name,
				char *location_name );

char *get_process_time_sys_string(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *heading );

char *get_substrate_sys_string(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *heading );

char *get_abundance_rating_sys_string(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *heading );

char *get_seagrass_height_sys_string(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *heading );

char *get_environment_sys_string(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *heading );

char *get_per_site_sys_string(	char *application_name,
				char *project,
				char *collection_name,
				char *location_name );

char *get_per_sweep_sys_string(	char *application_name,
				char *project,
				char *collection_name,
				char *location_name );

void output_checksum_table(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *process_name );

void output_checksum_text_file(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *document_root_directory,
				pid_t process_id,
				char *process_name );

void output_quality_control_checksum(
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

	output_quality_control_checksum(
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

void output_quality_control_checksum(
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
		output_checksum_text_file(
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
		output_checksum_table(
				application_name,
				project,
				collection_name,
				location_name,
				process_name );
	}
	else
	{
		output_checksum_table(
				application_name,
				project,
				collection_name,
				location_name,
				process_name );
	}
} /* output_quality_control_checksum() */

void output_checksum_text_file(
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
	char *input_sys_string;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	APPASERVER_LINK *appaserver_link;

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

	appaserver_link =
		appaserver_link_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			(char *)0 /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"csv" );

	/* Per site */
	/* -------- */
	appaserver_link->filename_stem = FILENAME_STEM_PER_SITE;

	output_filename =
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

/*
	sprintf(output_filename, 
	 	PER_SITE_OUTPUT_TEMPLATE,
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

	fprintf( output_file, "%s\n", sub_title );

	fprintf( output_file,
"Site,Environment,Seagrass Height,Abundance Rating,Substrate\n" );

	input_sys_string = get_per_site_sys_string(
				application_name,
				project,
				collection_name,
				location_name );

	input_pipe = popen( input_sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		search_replace_character(
				input_buffer,
		 		FOLDER_DATA_DELIMITER,
				',' );

		fprintf( output_file, "%s\n", input_buffer );
	}

	pclose( input_pipe );
	fclose( output_file );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
"Per site: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );

	/* Per sweep */
	/* --------- */
	appaserver_link->filename_stem = FILENAME_STEM_PER_SWEEP;

	output_filename =
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

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			output_filename );
		document_close();
		exit( 1 );
	}

	fprintf( output_file, "%s\n", sub_title );

	fprintf( output_file,
"Site,Sweep,Process Time,Species Count,Species Measurement\n" );

	input_sys_string = get_per_sweep_sys_string(
				application_name,
				project,
				collection_name,
				location_name );

	input_pipe = popen( input_sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		search_replace_character(
				input_buffer,
		 		FOLDER_DATA_DELIMITER,
				',' );

		fprintf( output_file, "%s\n", input_buffer );
	}

	pclose( input_pipe );
	fclose( output_file );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
"Per sweep: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );

} /* output_checksum_text_file() */

void output_checksum_table(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *process_name )
{
	char title[ 128 ];
	char sub_title[ 128 ];
	int results;
	char *input_sys_string;
	char output_sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	FILE *output_pipe;
	char *heading;
	char *format;

	get_title_and_sub_title(	title,
					sub_title,
					process_name,
					project,
					collection_name,
					location_name );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	results = system( "date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

	/* Per site. */
	/* --------- */
	heading =
"site,environment,vegetation_height,abundance_rating,substrate";

	format = "right,right,right,right,right";

	sprintf( output_sys_string,
		 "html_table.e '^Per Site' '%s' '^' %s",
		 heading,
		 format );

	output_pipe = popen( output_sys_string, "w" );

	input_sys_string = get_per_site_sys_string(
				application_name,
				project,
				collection_name,
				location_name );

	input_pipe = popen( input_sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		fprintf( output_pipe, "%s\n", input_buffer );
	}

	pclose( input_pipe );
	pclose( output_pipe );

	/* Per sweep. */
	/* ---------- */
	heading =
"site,sweep,process_time,species_count,species_measurement";

	format = "right,right,right,right,right";

	sprintf( output_sys_string,
		 "html_table.e '^Per Sweep' '%s' '^' %s",
		 heading,
		 format );

	output_pipe = popen( output_sys_string, "w" );

	input_sys_string = get_per_sweep_sys_string(
				application_name,
				project,
				collection_name,
				location_name );

	input_pipe = popen( input_sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		fprintf( output_pipe, "%s\n", input_buffer );
	}

	pclose( input_pipe );
	pclose( output_pipe );

} /* output_checksum_table() */

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

char *get_per_sweep_sys_string(	char *application_name,
				char *project,
				char *collection_name,
				char *location_name )
{
	static char sys_string[ 65536 ];
	char *heading;
	char *process_time_sys_string;
	char *count_sys_string;
	char *measurement_sys_string;

	heading = "process_time,species_count,species_measurement";

	process_time_sys_string =
		get_process_time_sys_string(
				application_name,
				project,
				collection_name,
				location_name,
				"process_time" );

	count_sys_string = get_count_sys_string(
				application_name,
				project,
				collection_name,
				location_name,
				"species_count" );

	measurement_sys_string =
			get_measurement_sys_string(
				application_name,
				project,
				collection_name,
				location_name,
				"species_measurement" );

	sprintf( sys_string,
		 "(%s;%s;%s)						|"
		 "sql.e '%c'						|"
		 "sort -n						|"
		 "pivot_table.e %s '^'					|"
		 "cat							 ",
		 process_time_sys_string,
		 count_sys_string,
		 measurement_sys_string,
		 FOLDER_DATA_DELIMITER,
		 heading );

	return sys_string;

} /* get_per_sweep_sys_string() */

char *get_per_site_sys_string(	char *application_name,
				char *project,
				char *collection_name,
				char *location_name )
{
	static char sys_string[ 65536 ];
	char *heading;
	char *environment_sys_string;
	char *seagrass_height_sys_string;
	char *abundance_rating_sys_string;
	char *substrate_sys_string;

	environment_sys_string =
		get_environment_sys_string(
				application_name,
				project,
				collection_name,
				location_name,
				"abiotic" );

	seagrass_height_sys_string =
		get_seagrass_height_sys_string(
				application_name,
				project,
				collection_name,
				location_name,
				"vegetation_height" );

	abundance_rating_sys_string =
		get_abundance_rating_sys_string(
				application_name,
				project,
				collection_name,
				location_name,
				"abundance_rating" );

	substrate_sys_string = get_substrate_sys_string(
				application_name,
				project,
				collection_name,
				location_name,
				"substrate" );

	heading =
"abiotic,vegetation_height,abundance_rating,substrate";

	sprintf( sys_string,
		 "(%s;%s;%s;%s)						|"
		 "sql.e							|"
		 "sort -n						|"
		 "pivot_table.e %s '^'					|"
		 "cat							 ",
		 environment_sys_string,
		 seagrass_height_sys_string,
		 abundance_rating_sys_string,
		 substrate_sys_string,
		 heading );

	return sys_string;

} /* get_per_site_sys_string() */

char *get_environment_sys_string(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *heading )
{
	static char sys_string[ 1024 ];
	char *sampling_point;
	char *abiotic_measurement;
	char select[ 128 ];
	char *group_by;
	char where[ 1024 ];

	sampling_point =
		get_table_name( application_name, "sampling_point" );
	abiotic_measurement =
		get_table_name( application_name, "abiotic_measurement" );

	sprintf( select,
		 "site_number,'%s',sum(value)",
		 heading );

	group_by = "site_number";

	sprintf( where,
		 "%s.project = '%s' and				"
		 "collection_name = '%s' and			"
		 "location = '%s' and				"
		 "%s.anchor_date = %s.anchor_date and		"
		 "%s.anchor_time = %s.anchor_time and		"
		 "%s.actual_latitude = %s.actual_latitude and	"
		 "%s.actual_longitude = %s.actual_longitude and	"
		 "%s.project = %s.project 			",
		 sampling_point,
		 project,
		 collection_name,
		 location_name,
		 sampling_point,
		 abiotic_measurement,
		 sampling_point,
		 abiotic_measurement,
		 sampling_point,
		 abiotic_measurement,
		 sampling_point,
		 abiotic_measurement,
		 sampling_point,
		 abiotic_measurement );

	sprintf( sys_string,
		 "echo \"select %s from %s,%s where %s group by %s;\"",
		 select,
		 sampling_point,
		 abiotic_measurement,
		 where,
		 group_by );

	return sys_string;
} /* get_environment_sys_string() */

char *get_seagrass_height_sys_string(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *heading )
{
	static char sys_string[ 1024 ];
	char *sampling_point;
	char *quad_vegetation;
	char select[ 128 ];
	char *group_by;
	char where[ 1024 ];

	sampling_point =
		get_table_name( application_name, "sampling_point" );
	quad_vegetation =
		get_table_name( application_name, "quad_vegetation" );

	sprintf( select,
		 "site_number,'%s',sum(vegetation_height_cm)",
		 heading );

	group_by = "site_number";

	sprintf( where,
		 "%s.project = '%s' and				"
		 "collection_name = '%s' and			"
		 "location = '%s' and				"
		 "%s.anchor_date = %s.anchor_date and		"
		 "%s.anchor_time = %s.anchor_time and		"
		 "%s.actual_latitude = %s.actual_latitude and	"
		 "%s.actual_longitude = %s.actual_longitude and	"
		 "%s.project = %s.project 			",
		 sampling_point,
		 project,
		 collection_name,
		 location_name,
		 sampling_point,
		 quad_vegetation,
		 sampling_point,
		 quad_vegetation,
		 sampling_point,
		 quad_vegetation,
		 sampling_point,
		 quad_vegetation,
		 sampling_point,
		 quad_vegetation );

	sprintf( sys_string,
		 "echo \"select %s from %s,%s where %s group by %s;\"",
		 select,
		 sampling_point,
		 quad_vegetation,
		 where,
		 group_by );

	return sys_string;
} /* get_seagrass_height_sys_string() */

char *get_abundance_rating_sys_string(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *heading )
{
	static char sys_string[ 1024 ];
	char *sampling_point;
	char *quad_vegetation;
	char *abundance_rating;
	char select[ 128 ];
	char *group_by;
	char where[ 1024 ];

	sampling_point =
		get_table_name( application_name, "sampling_point" );
	quad_vegetation =
		get_table_name( application_name, "quad_vegetation" );
	abundance_rating =
		get_table_name( application_name, "abundance_rating" );

	sprintf( select,
		 "site_number,'%s',sum(abundance_rating_code)",
		 heading );

	group_by = "site_number";

	sprintf( where,
		 "%s.project = '%s' and				"
		 "collection_name = '%s' and			"
		 "location = '%s' and				"
		 "%s.anchor_date = %s.anchor_date and		"
		 "%s.anchor_time = %s.anchor_time and		"
		 "%s.actual_latitude = %s.actual_latitude and	"
		 "%s.actual_longitude = %s.actual_longitude and	"
		 "%s.project = %s.project and			"
		 "%s.abundance_rating = %s.abundance_rating 	",
		 sampling_point,
		 project,
		 collection_name,
		 location_name,
		 sampling_point,
		 quad_vegetation,
		 sampling_point,
		 quad_vegetation,
		 sampling_point,
		 quad_vegetation,
		 sampling_point,
		 quad_vegetation,
		 sampling_point,
		 quad_vegetation,
		 quad_vegetation,
		 abundance_rating );

	sprintf( sys_string,
		 "echo \"select %s from %s,%s,%s where %s group by %s;\"",
		 select,
		 sampling_point,
		 quad_vegetation,
		 abundance_rating,
		 where,
		 group_by );

	return sys_string;
} /* get_abundance_rating_sys_string() */

char *get_substrate_sys_string(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *heading )
{
	static char sys_string[ 1024 ];
	char *sampling_point;
	char *quad_substrate;
	char select[ 128 ];
	char *group_by;
	char where[ 1024 ];

	sampling_point =
		get_table_name( application_name, "sampling_point" );
	quad_substrate =
		get_table_name( application_name, "quad_substrate" );

	sprintf( select,
		 "site_number,'%s',count(*)",
		 heading );

	group_by = "site_number";

	sprintf( where,
		 "%s.project = '%s' and				"
		 "collection_name = '%s' and			"
		 "location = '%s' and				"
		 "%s.anchor_date = %s.anchor_date and		"
		 "%s.anchor_time = %s.anchor_time and		"
		 "%s.actual_latitude = %s.actual_latitude and	"
		 "%s.actual_longitude = %s.actual_longitude and	"
		 "%s.project = %s.project 			",
		 sampling_point,
		 project,
		 collection_name,
		 location_name,
		 sampling_point,
		 quad_substrate,
		 sampling_point,
		 quad_substrate,
		 sampling_point,
		 quad_substrate,
		 sampling_point,
		 quad_substrate,
		 sampling_point,
		 quad_substrate );

	sprintf( sys_string,
		 "echo \"select %s from %s,%s where %s group by %s;\"",
		 select,
		 sampling_point,
		 quad_substrate,
		 where,
		 group_by );

	return sys_string;
} /* get_substrate_sys_string() */

char *get_process_time_sys_string(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *heading )
{
	static char sys_string[ 1024 ];
	char *sampling_point;
	char *sweep;
	char select[ 256 ];
	char where[ 1024 ];

	sampling_point =
		get_table_name( application_name, "sampling_point" );
	sweep =
		get_table_name( application_name, "sweep" );

	sprintf( select,
"site_number,sweep_number,'%s',wash_time_minutes + pick_time_minutes + quality_control_time_minutes + sort_minutes + identify_penaeid_minutes + identify_fish_minutes + identify_caridean_minutes + identify_crabs_minutes",
		 heading );

	sprintf( where,
		 "%s.project = '%s' and				"
		 "collection_name = '%s' and			"
		 "location = '%s' and				"
		 "%s.anchor_date = %s.anchor_date and		"
		 "%s.anchor_time = %s.anchor_time and		"
		 "%s.actual_latitude = %s.actual_latitude and	"
		 "%s.actual_longitude = %s.actual_longitude and	"
		 "%s.project = %s.project 			",
		 sampling_point,
		 project,
		 collection_name,
		 location_name,
		 sampling_point,
		 sweep,
		 sampling_point,
		 sweep,
		 sampling_point,
		 sweep,
		 sampling_point,
		 sweep,
		 sampling_point,
		 sweep );

	sprintf( sys_string,
		 "echo \"select %s from %s,%s where %s;\"",
		 select,
		 sampling_point,
		 sweep,
		 where );

	return sys_string;
} /* get_process_time_sys_string() */

char *get_count_sys_string(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *heading )
{
	static char sys_string[ 1024 ];
	char *sampling_point;
	char *species_count;
	char select[ 256 ];
	char where[ 1024 ];
	char *group_by;

	sampling_point =
		get_table_name( application_name, "sampling_point" );
	species_count =
		get_table_name( application_name, "species_count" );

	sprintf( select,
	"site_number,sweep_number,'%s',sum(count_per_square_meter)",
		 heading );

	group_by = "site_number,sweep_number";

	sprintf( where,
		 "%s.project = '%s' and				"
		 "collection_name = '%s' and			"
		 "location = '%s' and				"
		 "%s.anchor_date = %s.anchor_date and		"
		 "%s.anchor_time = %s.anchor_time and		"
		 "%s.actual_latitude = %s.actual_latitude and	"
		 "%s.actual_longitude = %s.actual_longitude and	"
		 "%s.project = %s.project 			",
		 sampling_point,
		 project,
		 collection_name,
		 location_name,
		 sampling_point,
		 species_count,
		 sampling_point,
		 species_count,
		 sampling_point,
		 species_count,
		 sampling_point,
		 species_count,
		 sampling_point,
		 species_count );

	sprintf( sys_string,
		 "echo \"select %s from %s,%s where %s group by %s;\"",
		 select,
		 sampling_point,
		 species_count,
		 where,
		 group_by );

	return sys_string;
} /* get_count_sys_string() */

char *get_measurement_sys_string(
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name,
				char *heading )
{
	static char sys_string[ 1024 ];
	char *sampling_point;
	char *species_measurement;
	char select[ 256 ];
	char where[ 1024 ];
	char *group_by;

	sampling_point =
		get_table_name( application_name, "sampling_point" );
	species_measurement =
		get_table_name( application_name, "species_measurement" );

	sprintf( select,
	"site_number,sweep_number,'%s',sum(length_millimeters)",
		 heading );

	group_by = "site_number,sweep_number";

	sprintf( where,
		 "%s.project = '%s' and				"
		 "collection_name = '%s' and			"
		 "location = '%s' and				"
		 "%s.anchor_date = %s.anchor_date and		"
		 "%s.anchor_time = %s.anchor_time and		"
		 "%s.actual_latitude = %s.actual_latitude and	"
		 "%s.actual_longitude = %s.actual_longitude and	"
		 "%s.project = %s.project 			",
		 sampling_point,
		 project,
		 collection_name,
		 location_name,
		 sampling_point,
		 species_measurement,
		 sampling_point,
		 species_measurement,
		 sampling_point,
		 species_measurement,
		 sampling_point,
		 species_measurement,
		 sampling_point,
		 species_measurement );

	sprintf( sys_string,
		 "echo \"select %s from %s,%s where %s group by %s;\"",
		 select,
		 sampling_point,
		 species_measurement,
		 where,
		 group_by );

	return sys_string;
} /* get_measurement_sys_string() */

