/* ----------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_hydrology/output_merged_waterquality.c 	*/
/* ----------------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org		*/
/* ----------------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "measurement_update_parameter.h"
#include "hashtbl.h"
#include "dictionary.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "measurement_backup.h"
#include "query.h"
#include "date.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "grace.h"
#include "application.h"
#include "environ.h"
#include "list_usage.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "session.h"
#include "hydrology_library.h"
#include "application_constants.h"
#include "appaserver_link_file.h"
#include "google_chart.h"
#include "dictionary_appaserver.h"
#include "merged.h"

/* Constants */
/* --------- */
#define GRACE_DATATYPE_ENTITY_PIECE		0
#define GRACE_DATATYPE_PIECE			1
#define GRACE_DATE_PIECE			2
#define GRACE_TIME_PIECE			-1
#define GRACE_VALUE_PIECE			3
#define GRACE_TICKLABEL_ANGLE			90

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
void output_merged_waterquality_textfile(
				FILE *output_pipe,
				LIST *waterquality_station_datatype_list,
				LIST *hydrology_station_datatype_list,
				LIST *date_day_key_list,
				char delimiter );

void output_merged_waterquality_spreadsheet(
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *begin_date,
				char *end_date,
				LIST *waterquality_station_datatype_list,
				LIST *hydrology_station_datatype_list,
				LIST *date_day_key_list );

void output_merged_waterquality_table(
				char *begin_date,
				char *end_date,
				LIST *waterquality_station_datatype_list,
				LIST *hydrology_station_datatype_list,
				LIST *date_day_key_list );

void output_merged_waterquality_gracechart(
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *argv_0,
				char *begin_date,
				char *end_date,
				LIST *waterquality_station_datatype_list,
				LIST *hydrology_station_datatype_list,
				LIST *date_day_key_list );

boolean output_merged_waterquality(
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *argv_0,
				char *begin_date,
				char *end_date,
				char *output_medium,
				MERGED_INPUT *merged_input );

DICTIONARY *output_merged_parameter_dictionary(
				char *parameter_dictionary_string );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *begin_date;
	char *end_date;
	char *output_medium;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DICTIONARY *parameter_dictionary;
	MERGED_INPUT *merged_input;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 6 )
	{
		fprintf( stderr,
	"Usage: %s process begin_date end_date output_medium dictionary\n",
			argv[ 0 ] );

		fprintf( stderr,
	"Note: output_medium = {table,spreadsheet,gracechart,stdout}\n" );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	begin_date = argv[ 2 ];
	end_date = argv[ 3 ];
	output_medium = argv[ 4 ];

	if ( !*output_medium || strcmp( output_medium, "output_medium" ) == 0 )
	{
		output_medium = "table";
	}

	parameter_dictionary =
		output_merged_parameter_dictionary(
			argv[ 5 ]
				/* parameter_dictionary_string */ );

	/* Does exit(1) on errors. */
	/* ----------------------- */
	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		char buffer[ 128 ];

		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf( "<h1>%s</h1>\n<h2>",
			format_initial_capital(
				buffer,
				process_name ) );
		fflush( stdout );

		if ( strcmp( output_medium, "gracechart" ) == 0 )
		{
		}
		else
		{
			if ( system( timlib_system_date_string() ) ) {};

			printf( "</h2>\n" );
		}
		fflush( stdout );
	}

	if ( ! ( merged_input = merged_input_new( parameter_dictionary ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: merged_input_new() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !output_merged_waterquality(
		application_name,
		process_name,
		appaserver_parameter_file->
			document_root,
		argv[ 0 ],
		begin_date,
		end_date,
		output_medium,
		merged_input ) )
	{
		if ( strcmp( output_medium, "stdout" ) != 0 )
		{
			printf(
	"<h3>Warning: no water quality concentrations selected.</h3>\n" );
		}
	}

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
		document_close();
	}

	process_increment_execution_count(
		application_name,
		process_name,
		appaserver_parameter_file_get_dbms() );

	return 0;

} /* main() */

DICTIONARY *output_merged_parameter_dictionary(
				char *parameter_dictionary_string )
{
	DICTIONARY *parameter_dictionary;

	parameter_dictionary = 
		dictionary_index_string2dictionary(
			parameter_dictionary_string );

	dictionary_appaserver_parse_multi_attribute_keys(
		parameter_dictionary,
		(char *)0 /* prefix */ );

	dictionary_add_elements_by_removing_prefix(
		parameter_dictionary,
		QUERY_STARTING_LABEL );

	dictionary_add_elements_by_removing_prefix(
		parameter_dictionary,
		QUERY_FROM_STARTING_LABEL );

	return parameter_dictionary;

} /* output_merged_parameter_dictionary() */

boolean output_merged_waterquality(
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *argv_0,
				char *begin_date,
				char *end_date,
				char *output_medium,
				MERGED_INPUT *merged_input )
{
	MERGED *merged;
	HASH_TABLE *key_hash_table;
	LIST *date_day_key_list = {0};

	merged = merged_new( merged_input );

	if ( !list_length( merged->waterquality_station_datatype_list ) )
	{
		return 0;
	}

	merged_results_hash_table(
		merged->waterquality_station_datatype_list,
		begin_date,
		end_date );

	/* Get the time sequence from waterquality */
	/* --------------------------------------- */
	if ( !list_length( merged->waterquality_station_datatype_list ) )
	{
		return 0;
	}

	key_hash_table =
		merged_key_hash_table(
			merged->waterquality_station_datatype_list );

	if ( !key_hash_table )
	{
		return 0;
	}

	date_day_key_list =
	 	hash_table_get_ordered_key_list(
			key_hash_table );

	if ( !list_length( date_day_key_list ) )
	{
		return 0;
	}

	merged_measurement_hash_table(
		merged->hydrology_station_datatype_list,
		application_name,
		begin_date,
		end_date );

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		output_merged_waterquality_table(
			begin_date,
			end_date,
			merged->waterquality_station_datatype_list,
			merged->hydrology_station_datatype_list,
			date_day_key_list );
	}
	else
	if ( strcmp( output_medium, "gracechart" ) == 0 )
	{
		output_merged_waterquality_gracechart(
			application_name,
			process_name,
			document_root_directory,
			argv_0,
			begin_date,
			end_date,
			merged->waterquality_station_datatype_list,
			merged->hydrology_station_datatype_list,
			date_day_key_list );
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		output_merged_waterquality_spreadsheet(
			application_name,
			process_name,
			document_root_directory,
			begin_date,
			end_date,
			merged->waterquality_station_datatype_list,
			merged->hydrology_station_datatype_list,
			date_day_key_list );
	}
	else
	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		FILE *output_pipe;
		LIST *heading_list;

		output_pipe = popen( "cat", "w" );

		heading_list =
			merged_heading_list(
				merged->waterquality_station_datatype_list,
				merged->hydrology_station_datatype_list );

		fprintf( output_pipe,
			 "%s\n",
			 list_display_delimited( heading_list, ',' ) );

		output_merged_waterquality_textfile(
			output_pipe,
			merged->waterquality_station_datatype_list,
			merged->hydrology_station_datatype_list,
			date_day_key_list,
			',' /* delimiter */ );

		pclose( output_pipe );
	}

	return 1;

} /* output_merged_waterquality() */

void output_merged_waterquality_table(
			char *begin_date,
			char *end_date,
			LIST *waterquality_station_datatype_list,
			LIST *hydrology_station_datatype_list,
			LIST *date_day_key_list )
{
	LIST *heading_list;
	char *sub_title;
	char sys_string[ 1024 ];
	FILE *output_pipe;

	sub_title = merged_subtitle( begin_date, end_date );

	heading_list =
		merged_heading_list(
			waterquality_station_datatype_list,
			hydrology_station_datatype_list );

	sprintf( sys_string,
		 "html_table.e \"^%s\" \"%s\" ',' right",
		 sub_title,
		 list_display_delimited( heading_list, ',' ) );

	output_pipe = popen( sys_string, "w" );

	if ( !list_rewind( date_day_key_list ) )
	{
		pclose( output_pipe );
		return;
	}

	output_merged_waterquality_textfile(
		output_pipe,
		waterquality_station_datatype_list,
		hydrology_station_datatype_list,
		date_day_key_list,
		',' /* delimiter */ );

	pclose( output_pipe );

} /* output_merged_waterquality_table() */

void output_merged_waterquality_spreadsheet(
				char *application_name,
				char *process_name,
				char *document_root_directory,
				char *begin_date,
				char *end_date,
				LIST *waterquality_station_datatype_list,
				LIST *hydrology_station_datatype_list,
				LIST *date_day_key_list )
{
	char sys_string[ 1024 ];
	char *output_filename;
	char *ftp_filename;
	pid_t process_id = getpid();
	FILE *output_pipe;
	FILE *output_file;
	APPASERVER_LINK_FILE *appaserver_link_file;
	LIST *heading_list;

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			process_name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	appaserver_link_file->begin_date_string = begin_date;
	appaserver_link_file->end_date_string = end_date;

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
		printf( "<h3>ERROR: Cannot open output file %s</h3>\n",
			output_filename );
		document_close();
		exit( 1 );
	}
	else
	{
		fclose( output_file );
	}

	sprintf( sys_string,
		 "cat > %s",
		 output_filename );

	output_pipe = popen( sys_string, "w" );

	heading_list =
		merged_heading_list(
			waterquality_station_datatype_list,
			hydrology_station_datatype_list );

	fprintf( output_pipe,
		 "%s\n",
		 list_display_delimited( heading_list, ',' ) );

	output_merged_waterquality_textfile(
				output_pipe,
				waterquality_station_datatype_list,
				hydrology_station_datatype_list,
				date_day_key_list,
				',' /* delimiter */ );

	pclose( output_pipe );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
			TRANSMIT_PROMPT,
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

} /* output_merged_waterquality_spreadsheet() */

void output_merged_waterquality_textfile(
			FILE *output_pipe,
			LIST *waterquality_station_datatype_list,
			LIST *hydrology_station_datatype_list,
			LIST *date_day_key_list,
			char delimiter )
{
	char *date_day;
	MERGED_STATION_DATATYPE *station_datatype;
	MERGED_MEASUREMENT *measurement;

	if ( !list_rewind( date_day_key_list ) ) return;

	do {
		date_day =
			list_get_pointer(
				date_day_key_list );

		fprintf( output_pipe, "%s", date_day );

		/* Output waterquality */
		/* ------------------- */
		if ( list_rewind( waterquality_station_datatype_list ) )
		{
			do {
				station_datatype =
					list_get_pointer(
					 waterquality_station_datatype_list );

				measurement =
					hash_table_get_pointer(
						station_datatype->
							measurement_hash_table,
						date_day );

				fprintf( output_pipe,
					 "%c%s",
					 delimiter,
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					merged_measurement_buffer(
						measurement ) );

			} while( list_next(
					waterquality_station_datatype_list ) );

		} /* If */

		/* Output hydrology */
		/* ---------------- */
		if ( list_rewind( hydrology_station_datatype_list ) )
		{
			do {
				station_datatype =
					list_get_pointer(
					 hydrology_station_datatype_list );

				measurement =
					hash_table_get_pointer(
						station_datatype->
							measurement_hash_table,
						date_day );

				fprintf( output_pipe,
					 "%c%s",
					 delimiter,
					 /* --------------------- */
					 /* Returns static memory */
					 /* --------------------- */
					 merged_measurement_buffer(
						measurement ) );

			} while( list_next(
					hydrology_station_datatype_list ) );

		} /* If */

		fprintf( output_pipe, "\n" );

	} while( list_next( date_day_key_list ) );	

} /* output_merged_waterquality_textfile() */

void output_merged_waterquality_gracechart(
			char *application_name,
			char *process_name,
			char *document_root_directory,
			char *argv_0,
			char *begin_date,
			char *end_date,
			LIST *waterquality_station_datatype_list,
			LIST *hydrology_station_datatype_list,
			LIST *date_day_key_list )
{
	char station_datatype_input_buffer[ 512 ];
	char title[ 128 ];
	char *sub_title;
	GRACE *grace;
	char graph_identifier[ 16 ];
	char *agr_filename;
	char *ftp_agr_filename;
	char *postscript_filename;
	char *output_filename;
	char *ftp_output_filename;
	int page_width_pixels;
	int page_length_pixels;
	char *distill_landscape_flag;
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;
	char legend[ 128 ];
	char *date_day;
	char buffer[ 512 ];
	MERGED_STATION_DATATYPE *station_datatype;
	MERGED_MEASUREMENT *measurement;

	sub_title = merged_subtitle( begin_date, end_date );

	grace = grace_new_unit_graph_grace(
				application_name,
				(char *)0 /* role_name */,
				(char *)0 /* infrastructure_process */,
				(char *)0 /* data_process */,
				argv_0,
				GRACE_DATATYPE_ENTITY_PIECE,
				GRACE_DATATYPE_PIECE,
				GRACE_DATE_PIECE,
				GRACE_TIME_PIECE,
				GRACE_VALUE_PIECE,
				format_initial_capital(
					title,
					process_name  ),
				sub_title,
				0 /* not datatype_type_xyhilo */,
				no_cycle_colors_if_multiple_datatypes );

	grace->dataset_no_cycle_color = 1;

	if ( !grace_set_begin_end_date(
		grace,
		begin_date,
		end_date ) )
	{
		return;
	}

	if ( !list_length( date_day_key_list ) ) return;

	/* Output waterquality */
	/* ------------------- */
	if ( list_rewind( waterquality_station_datatype_list ) )
	{
		do {
			station_datatype =
				list_get_pointer(
				 waterquality_station_datatype_list );

			grace_graph = grace_new_grace_graph();

			grace_graph->xaxis_ticklabel_angle =
				GRACE_TICKLABEL_ANGLE;

			list_append_pointer(
				grace->graph_list,
				grace_graph );

			grace_datatype =
				grace_new_grace_datatype(
					station_datatype->station,
					station_datatype->datatype);

			sprintf(legend,
				"%s/%s (%s)",
				station_datatype->station,
				station_datatype->datatype,
				station_datatype->units );

			strcpy(	legend,
				format_initial_capital(
					buffer,
					legend ) );

			grace_datatype->legend = strdup( legend );

			if ( station_datatype->bar_graph )
			{
				grace_datatype->datatype_type_bar_xy_xyhilo =
					"bar";
				grace_datatype->line_linestyle = 0;
			}
			else
			{
				grace_datatype->datatype_type_bar_xy_xyhilo =
					"xy";
			}

			list_append_pointer(	grace_graph->datatype_list,
						grace_datatype );

			list_rewind( date_day_key_list );

			do {
				date_day =
					list_get_pointer(
						date_day_key_list );

				measurement =
					hash_table_get_pointer(
						station_datatype->
							measurement_hash_table,
						date_day );

				if ( !measurement || measurement->is_null )
					continue;

				sprintf(station_datatype_input_buffer,
		 			"%s|%s|%s|%.3lf",
		 			station_datatype->station,
		 			station_datatype->datatype,
		 			date_day,
					measurement->measurement_value );

				grace_set_string_to_point_list(
					grace->graph_list, 
					GRACE_DATATYPE_ENTITY_PIECE,
					GRACE_DATATYPE_PIECE,
					GRACE_DATE_PIECE,
					GRACE_TIME_PIECE,
					GRACE_VALUE_PIECE,
					station_datatype_input_buffer,
					unit_graph,
					grace->datatype_type_xyhilo,
					grace->dataset_no_cycle_color,
					(char *)0 /* optional_label */ );

			} while( list_next( date_day_key_list ) );	

		} while( list_next( waterquality_station_datatype_list ) );
	} /* If */

	/* Output hydrology */
	/* ---------------- */
	if ( list_rewind( hydrology_station_datatype_list ) )
	{
		do {
			station_datatype =
				list_get_pointer(
				 hydrology_station_datatype_list );

			grace_graph = grace_new_grace_graph();

			grace_graph->xaxis_ticklabel_angle =
				GRACE_TICKLABEL_ANGLE;

			list_append_pointer(
				grace->graph_list,
				grace_graph );

			grace_datatype =
				grace_new_grace_datatype(
					station_datatype->station,
					station_datatype->datatype);

			sprintf(legend,
				"%s/%s (%s)",
				station_datatype->station,
				station_datatype->datatype,
				station_datatype->units );

			strcpy(	legend,
				format_initial_capital(
					buffer,
					legend ) );

			grace_datatype->legend = strdup( legend );

			if ( station_datatype->bar_graph )
			{
				grace_datatype->datatype_type_bar_xy_xyhilo =
					"bar";
				grace_datatype->line_linestyle = 0;
			}
			else
			{
				grace_datatype->datatype_type_bar_xy_xyhilo =
					"xy";
			}

			list_append_pointer(	grace_graph->datatype_list,
						grace_datatype );

			list_rewind( date_day_key_list );

			do {
				date_day =
					list_get_pointer(
						date_day_key_list );

				measurement =
					hash_table_get_pointer(
						station_datatype->
							measurement_hash_table,
						date_day );

				if ( !measurement || measurement->is_null )
					continue;

				sprintf(station_datatype_input_buffer,
		 			"%s|%s|%s|%.3lf",
		 			station_datatype->station,
		 			station_datatype->datatype,
		 			date_day,
					measurement->measurement_value );

				grace_set_string_to_point_list(
					grace->graph_list, 
					GRACE_DATATYPE_ENTITY_PIECE,
					GRACE_DATATYPE_PIECE,
					GRACE_DATE_PIECE,
					GRACE_TIME_PIECE,
					GRACE_VALUE_PIECE,
					station_datatype_input_buffer,
					unit_graph,
					grace->datatype_type_xyhilo,
					grace->dataset_no_cycle_color,
					(char *)0 /* optional_label */ );

			} while( list_next( date_day_key_list ) );	

		} while( list_next( hydrology_station_datatype_list ) );
	} /* If */

	grace->grace_output =
		application_get_grace_output(
			application_name );

	sprintf( graph_identifier, "%d", getpid() );

	grace_get_filenames(
			&agr_filename,
			&ftp_agr_filename,
			&postscript_filename,
			&output_filename,
			&ftp_output_filename,
			application_name,
			document_root_directory,
			graph_identifier,
			grace->grace_output );

	if ( !grace_set_structures(
				&page_width_pixels,
				&page_length_pixels,
				&distill_landscape_flag,
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
		return;
	}

	grace_move_legend_bottom_left(
			(GRACE_GRAPH *)
				list_get_first_pointer(
					grace->graph_list ),
			grace->landscape_mode );

	grace_increase_legend_char_size(
			(GRACE_GRAPH *)
				list_get_first_pointer(
					grace->graph_list ),
			0.15 );

	/* Make the graph wider -- 95% of the page */
	/* --------------------------------------- */
	grace_set_view_maximum_x(
			(GRACE_GRAPH *)
				list_get_first_pointer(
					grace->graph_list ),
			0.95 );

	/* Move the legend down a little */
	/* ----------------------------- */
	grace_lower_legend(	grace->graph_list,
				0.04 );

	if ( !grace_output_charts(
				output_filename, 
				postscript_filename,
				agr_filename,
				grace->title,
				grace->sub_title,
				grace->xaxis_ticklabel_format,
				grace->grace_graph_type,
				grace->x_label_size,
				page_width_pixels,
				page_length_pixels,
				application_get_grace_home_directory(
					application_name ),
				application_get_grace_execution_directory(
					application_name ),
				application_get_grace_free_option_yn(
					application_name ),
				grace->grace_output,
				application_get_distill_directory(
					application_name ),
				distill_landscape_flag,
				application_get_ghost_script_directory(
					application_name ),
				(LIST *)0 /* quantum_datatype_name_list */,
				grace->symbols,
				grace->world_min_x,
				grace->world_max_x,
				grace->xaxis_ticklabel_precision,
				grace->graph_list,
				grace->anchor_graph_list ) )
	{
		return;
	}
	else
	{
		grace_output_graph_window(
				application_name,
				ftp_output_filename,
				ftp_agr_filename,
				(char *)0 /* appaserver_mount_point */,
				0 /* not with_document_output */,
				(char *)0 /* where_clause */ );

	}

} /* output_merged_waterquality_gracechart() */

/* ----------------------------------------------------------- 	*/
