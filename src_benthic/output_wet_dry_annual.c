/* ---------------------------------------------------	*/
/* src_benthic/output_wet_dry_annual.c			*/
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

/* Structures */
/* ---------- */
typedef struct
{
	int year;
	int wet_total;
	int dry_total;
} YEAR_TOTAL;

/* Prototypes */
/* ---------- */
YEAR_TOTAL *year_total_seek_or_set(
				LIST *year_total_list,
				int year );

YEAR_TOTAL *year_total_seek(	LIST *year_total_list,
				int year );

YEAR_TOTAL *year_total_new(	int year );

LIST *get_year_total_list(	char *application_name,
				char *family_name,
				char *genus_name,
				char *species_name );

FILE *get_input_pipe(		char *application_name,
				char *family_name,
				char *genus_name,
				char *species_name,
				char *season );

void output_wet_dry_annual_stdout(
				char *application_name,
				char *family_name,
				char *genus_name,
				char *species_name );

void output_wet_dry_annual_table(
				char *application_name,
				char *family_name,
				char *genus_name,
				char *species_name );

void output_wet_dry_annual_spreadsheet(
				char *application_name,
				char *family_name,
				char *genus_name,
				char *species_name,
				char *document_root_directory,
				char *sub_title,
				char *process_name );

void output_wet_dry_annual_gracechart(
				char *application_name,
				char *family_name,
				char *genus_name,
				char *species_name,
				char *document_root_directory,
				char *title,
				char *sub_title,
				char *argv_0 );

void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name,
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
	char *family_name;
	char *genus_name;
	char *species_name;
	char *output_medium;
	char title[ 128 ];
	char sub_title[ 128 ];

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s application process_name family genus species output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	family_name = argv[ 3 ];
	genus_name = argv[ 4 ];
	species_name = argv[ 5 ];
	output_medium = argv[ 6 ];

	if ( !*output_medium || strcmp( output_medium, "output_medium" ) == 0 )
		output_medium = "table";

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

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
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

		get_title_and_sub_title(
			title,
			sub_title,
			process_name,
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
	}

	if ( timlib_strcmp( family_name, "family_name" ) == 0 )
	{
		printf( "<h3>Please choose a species.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		output_wet_dry_annual_table(
				application_name,
				family_name,
				genus_name,
				species_name );
	}
	else
	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		output_wet_dry_annual_stdout(
				application_name,
				family_name,
				genus_name,
				species_name );
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		output_wet_dry_annual_spreadsheet(
				application_name,
				family_name,
				genus_name,
				species_name,
				appaserver_parameter_file->
					document_root,
				sub_title,
				process_name );
	}
	else
	if ( strcmp( output_medium, "chart" ) == 0 )
	{
		output_wet_dry_annual_gracechart(
				application_name,
				family_name,
				genus_name,
				species_name,
				appaserver_parameter_file->
					document_root,
				title,
				sub_title,
				argv[ 0 ] );
	}

	if ( strcmp( output_medium, "stdout" ) != 0 ) document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void output_wet_dry_annual_gracechart(
				char *application_name,
				char *family_name,
				char *genus_name,
				char *species_name,
				char *document_root_directory,
				char *title,
				char *sub_title,
				char *argv_0 )
{
	LIST *year_total_list;
	YEAR_TOTAL *year_total;
	GRACE *grace;
	char graph_identifier[ 16 ];
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];
	char point_buffer[ 128 ];
	char optional_label[ 16 ];

	year_total_list =
		get_year_total_list(
			application_name,
			family_name,
			genus_name,
			species_name );

	if ( !list_length( year_total_list ) )
	{
		printf(
	"<h3>Error: the FIAN project has no catches for this species.</h3>\n" );
		return;
	}

	year_total = list_get_first_pointer( year_total_list );
	sprintf( begin_date_string, "%d-01-01", year_total->year );

	year_total = list_get_last_pointer( year_total_list );
	sprintf( end_date_string, "%d-12-31", year_total->year );

	grace = grace_new_unit_graph_grace(
				application_name,
				(char *)0 /* role_name */,
				(char *)0 /* infrastructure_process */,
				(char *)0 /* data_process */,
				argv_0,
				-1 /* grace_datatype_entity_piece */,
				0 /* grace_datatype_piece */,
				1 /* grace_date_piece */,
				-1 /* grace_time_piece */,
				2 /* grace_value_piece */,
				title,
				sub_title,
				0 /* not datatype_type_xyhilo */,
				no_cycle_colors );

	grace_set_begin_end_date(	grace,
					begin_date_string,
					end_date_string );

	grace_graph = grace_new_grace_graph();
	grace_graph->xaxis_ticklabel_angle = 45;
	grace_graph->yaxis_grid_lines = 0;
	grace_graph->yaxis_ticklabel_precision = 0;
	list_append_pointer( grace->graph_list, grace_graph );

	grace_datatype = grace_new_grace_datatype( "", "wet" );
	grace_datatype->legend = "Wet";
	grace_datatype->datatype_type_bar_xy_xyhilo = "xy";
	list_append_pointer(	grace_graph->datatype_list,
				grace_datatype );

	grace_datatype = grace_new_grace_datatype( "", "dry" );
	grace_datatype->legend = "Dry";
	grace_datatype->datatype_type_bar_xy_xyhilo = "xy";
	list_append_pointer(	grace_graph->datatype_list,
				grace_datatype );

	grace->dataset_no_cycle_color = 1;

	list_rewind( year_total_list );

	do {
		year_total = list_get( year_total_list );

		sprintf( optional_label, "%d", year_total->year );

		sprintf( point_buffer,
			 "wet|%d-12-31|%d\n",
			 year_total->year,
			 year_total->wet_total );

		if ( !grace_set_string_to_point_list(
				grace->graph_list, 
				grace->datatype_entity_piece,
				grace->datatype_piece,
				grace->date_piece,
				grace->time_piece,
				grace->value_piece,
				point_buffer,
				grace->grace_graph_type,
				grace->datatype_type_xyhilo,
				grace->dataset_no_cycle_color,
				strdup( optional_label ) ) )
		{
			printf(
			"<h3> Error: cannot find datatype in:%s</h3>\n",
				point_buffer );
		}

		sprintf( point_buffer,
			 "dry|%d-12-31|%d\n",
			 year_total->year,
			 year_total->dry_total );

		if ( !grace_set_string_to_point_list(
				grace->graph_list, 
				grace->datatype_entity_piece,
				grace->datatype_piece,
				grace->date_piece,
				grace->time_piece,
				grace->value_piece,
				point_buffer,
				grace->grace_graph_type,
				grace->datatype_type_xyhilo,
				grace->dataset_no_cycle_color,
				strdup( optional_label ) ) )
		{
			printf(
			"<h3> Error: cannot find datatype in:%s</h3>\n",
				point_buffer );
		}

	} while( list_next( year_total_list ) );

	grace->grace_output =
		application_grace_output( application_name );

	sprintf( graph_identifier, "%d", getpid() );

	grace_get_filenames(
			&grace->agr_filename,
			&grace->ftp_agr_filename,
			&grace->postscript_filename,
			&grace->output_filename,
			&grace->ftp_output_filename,
			application_name,
			document_root_directory,
			graph_identifier,
			grace->grace_output );

	if ( !grace_set_structures(
				&grace->page_width_pixels,
				&grace->page_length_pixels,
				&grace->distill_landscape_flag,
				&grace->landscape_mode,
				grace,
				grace->graph_list,
				grace->anchor_graph_list,
				grace->begin_date_julian,
				grace->end_date_julian,
				grace->number_of_days,
				grace->grace_graph_type,
				0 /* not force_landscape_mode */ ) )
	{
		printf( "<h3>Cannot set structures.</h3>\n" );
		return;
	}

	grace->dataset_no_cycle_color = 1;

	if ( !grace_output_charts(
				grace->output_filename, 
				grace->postscript_filename,
				grace->agr_filename,
				grace->title,
				grace->sub_title,
				grace->xaxis_ticklabel_format,
				grace->grace_graph_type,
				grace->x_label_size,
				grace->page_width_pixels,
				grace->page_length_pixels,
				application_grace_home_directory(
					application_name ),
				application_grace_execution_directory(
					application_name ),
				application_grace_free_option_yn(
					application_name ),
				grace->grace_output,
				application_distill_directory(
					application_name ),
				grace->distill_landscape_flag,
				application_ghost_script_directory(
					application_name ),
				grace_get_optional_label_list(
					grace->graph_list ),
				grace->symbols,
				grace->world_min_x,
				grace->world_max_x,
				grace->xaxis_ticklabel_precision,
				grace->graph_list,
				grace->anchor_graph_list ) )
	{
		printf( "<h3>Cannot output charts.</h3>\n" );
		return;
	}
	else
	{
		grace_output_graph_window(
				application_name,
				grace->ftp_output_filename,
				grace->ftp_agr_filename,
				(char *)0 /* appaserver_mount_point */,
				0 /* with_document_output */,
				(char *)0 /* where_clause */ );
		return;
	}
} /* output_wet_dry_annual_gracechart() */

void output_wet_dry_annual_spreadsheet(
				char *application_name,
				char *family_name,
				char *genus_name,
				char *species_name,
				char *document_root_directory,
				char *sub_title,
				char *process_name )
{
	LIST *year_total_list;
	YEAR_TOTAL *year_total;
	char *output_filename;
	char *ftp_filename;
	FILE *output_file;
	pid_t process_id = getpid();
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
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

	year_total_list =
		get_year_total_list(
			application_name,
			family_name,
			genus_name,
			species_name );

	if ( !list_rewind( year_total_list ) )
	{
		printf(
	"<h3>Error: the database has no catches for this species.</h3>\n" );
		return;
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
		printf( "<H2>ERROR: Cannot open output file %s</H2>\n",
			output_filename );
		document_close();
		exit( 1 );
	}

	fprintf( output_file, "%s\n", sub_title );

	fprintf( output_file, "Year,Wet,Dry,Total\n" );

	do {
		year_total = list_get( year_total_list );

		fprintf( output_file,
			 "%d,%d,%d,%d\n",
			 year_total->year,
			 year_total->wet_total,
			 year_total->dry_total,
			 year_total->wet_total + year_total->dry_total );

	} while( list_next( year_total_list ) );

	fclose( output_file );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
"&lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );

} /* output_wet_dry_annual_spreadsheet() */

void output_wet_dry_annual_stdout(
				char *application_name,
				char *family_name,
				char *genus_name,
				char *species_name )
{
	LIST *year_total_list;
	YEAR_TOTAL *year_total;

	year_total_list =
		get_year_total_list(
			application_name,
			family_name,
			genus_name,
			species_name );

	if ( !list_rewind( year_total_list ) )
	{
		printf(
	"<h3>Error: the database has no catches for this species.</h3>\n" );
		return;
	}

	printf( "Year,Wet,Dry,Total\n" );

	do {
		year_total = list_get( year_total_list );

		printf( "%d,%d,%d,%d\n",
			 year_total->year,
			 year_total->wet_total,
			 year_total->dry_total,
			 year_total->wet_total + year_total->dry_total );

	} while( list_next( year_total_list ) );

} /* output_wet_dry_annual_stdout() */

void output_wet_dry_annual_table(
				char *application_name,
				char *family_name,
				char *genus_name,
				char *species_name )
{
	LIST *year_total_list;
	YEAR_TOTAL *year_total;
	char sys_string[ 1024 ];
	FILE *output_pipe;

	year_total_list =
		get_year_total_list(
			application_name,
			family_name,
			genus_name,
			species_name );

	if ( !list_rewind( year_total_list ) )
	{
		printf(
	"<h3>Error: the database has no catches for this species.</h3>\n" );
		return;
	}


	strcpy(	sys_string,
		"html_table.e '' Year,Wet,Dry,Total '^' left,right,right" );

	output_pipe = popen( sys_string, "w" );

	do {
		year_total = list_get( year_total_list );

		fprintf( output_pipe,
			 "%d^%d^%d^%d\n",
			 year_total->year,
			 year_total->wet_total,
			 year_total->dry_total,
			 year_total->wet_total + year_total->dry_total );

	} while( list_next( year_total_list ) );

	pclose( output_pipe );

} /* output_wet_dry_annual_table() */

void get_title_and_sub_title(
			char *title,
			char *sub_title,
			char *process_name,
			char *family_name,
			char *genus_name,
			char *species_name )
{
	format_initial_capital( title, process_name );

	if ( *species_name && strcmp( species_name, "species_name" ) != 0 )
	{
		sprintf(	sub_title,
				"Species: %s %s %s",
				family_name,
				genus_name,
				species_name );
	}
	else
	if ( *genus_name && strcmp( genus_name, "genus_name" ) != 0 )
	{
		sprintf(	sub_title,
				"Genus: %s %s",
				family_name,
				genus_name );
	}
	else
	if ( *family_name && strcmp( family_name, "family_name" ) != 0 )
	{
		sprintf(	sub_title,
				"Family: %s",
				family_name );
	}
	else
	{
		*sub_title = '\0';
	}

} /* get_title_and_sub_title() */

LIST *get_year_total_list(
			char *application_name,
			char *family_name,
			char *genus_name,
			char *species_name )
{
	LIST *year_total_list = list_new();
	YEAR_TOTAL *year_total;
	FILE *input_pipe;
	char input_buffer[ 256 ];
	char year_string[ 128 ];
	char total_string[ 128 ];

	input_pipe =
		get_input_pipe(
			application_name,
			family_name,
			genus_name,
			species_name,
			"Sp" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( year_string, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		piece( total_string, FOLDER_DATA_DELIMITER, input_buffer, 1 );

		year_total =
			year_total_seek_or_set(
				year_total_list,
				atoi( year_string ) );
		year_total->wet_total = atoi( total_string );
	}

	pclose( input_pipe );

	input_pipe =
		get_input_pipe(
			application_name,
			family_name,
			genus_name,
			species_name,
			"Fa" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( year_string, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		piece( total_string, FOLDER_DATA_DELIMITER, input_buffer, 1 );

		year_total =
			year_total_seek_or_set(
				year_total_list,
				atoi( year_string ) );
		year_total->dry_total = atoi( total_string );
	}

	pclose( input_pipe );

	return year_total_list;

} /* get_year_total_list() */

FILE *get_input_pipe(	char *application_name,
			char *family_name,
			char *genus_name,
			char *species_name,
			char *season )
{
	char sys_string[ 1024 ];
	char select[ 1024 ];
	char where_join[ 1024 ];
	char where_scientific_name[ 1024 ];
	char where_clause[ 2048 ];
	char from[ 1024 ];
	char group_by[ 1024 ];
	char *sampling_point_table_name;
	char *species_count_table_name;

	sampling_point_table_name =
		get_table_name( application_name, "sampling_point" );

	species_count_table_name =
		get_table_name( application_name, "species_count" );

	sprintf( select,
		 "substr( %s.anchor_date, 1, 4), sum( count_per_square_meter )",
		 species_count_table_name );

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

	if ( *species_name && strcmp( species_name, "species_name" ) != 0 )
	{
		sprintf(where_scientific_name,
		 	"%s.family_name = '%s' and			"
		 	"%s.genus_name = '%s' and			"
		 	"%s.species_name = '%s'				",
		 	species_count_table_name,
		 	family_name,
		 	species_count_table_name,
		 	genus_name,
		 	species_count_table_name,
		 	species_name );
	}
	else
	if ( *genus_name && strcmp( genus_name, "genus_name" ) != 0 )
	{
		sprintf(where_scientific_name,
		 	"%s.family_name = '%s' and			"
		 	"%s.genus_name = '%s'				",
		 	species_count_table_name,
		 	family_name,
		 	species_count_table_name,
		 	genus_name );
	}
	else
	{
		sprintf(where_scientific_name,
		 	"%s.family_name = '%s'				",
		 	species_count_table_name,
		 	family_name );
	}

	sprintf( where_clause,
		 "%s and %s and %s.collection_name like '%s%c'",
		 where_scientific_name,
		 where_join,
		 sampling_point_table_name,
		 season,
		 '%' );

	sprintf( group_by,
		 "substr( %s.anchor_date, 1, 4 )",
		 species_count_table_name );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s group by %s;\" | sql.e",
		 select,
		 from,
		 where_clause,
		 group_by );

	return popen( sys_string, "r" );

} /* get_input_pipe() */

YEAR_TOTAL *year_total_new( int year )
{
	YEAR_TOTAL *year_total;

	if ( ! ( year_total = calloc( 1, sizeof( YEAR_TOTAL ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	year_total->year = year;
	return year_total;

} /* year_total_new() */

YEAR_TOTAL *year_total_seek_or_set(
				LIST *year_total_list,
				int year )
{
	YEAR_TOTAL *year_total;

	if ( ( year_total = year_total_seek( year_total_list, year ) ) )
	{
		return year_total;
	}
	else
	{
		year_total = year_total_new( year );
		list_append_pointer( year_total_list, year_total );
		return year_total;
	}

} /* year_total_seek_or_set() */

YEAR_TOTAL *year_total_seek(	LIST *year_total_list,
				int year )
{
	YEAR_TOTAL *year_total;

	if ( !list_rewind( year_total_list ) ) return (YEAR_TOTAL *)0;

	do {
		year_total = list_get( year_total_list );

		if ( year_total->year == year ) return year_total;

	} while( list_next( year_total_list ) );

	return (YEAR_TOTAL *)0;

} /* year_total_seek() */

