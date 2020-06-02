/* $APPASERVER_HOME/src_hydrology/custom_chart.c	*/
/* --------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "aggregate_statistic.h"
#include "validation_level.h"
#include "grace.h"
#include "date.h"
#include "application.h"
#include "environ.h"
#include "basename.h"
#include "process.h"
#include "hydrology_library.h"
#include "datatype.h"

/* Constants */
/* --------- */
#define PROCESS_NAME		"output_custom_chart"

#define INFRASTRUCTURE_PROCESS	"datatype_unit_record_list.sh %s %s %s %s %s | %s"
#define PADDED_DATA_PROCESS	"measurement_record_list_missing_padded.sh %s %s %s:%s %s %s %s:%s n \"%s\" \"%s\""
#define NON_PADDED_DATA_PROCESS	"measurement_record_list.sh %s %s %s:%s %s %s %s:%s n \"%s\" \"%s\""

/* #define GRACE_TICKLABEL_ANGLE	90 */

#define GRACE_TICKLABEL_ANGLE	45

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *role_name;
	char *session;
	char *validation_level_string;
	char *aggregate_statistic_string;
	char *symbols_yn;
	enum aggregate_statistic aggregate_statistic;
	int date_piece, time_piece, value_piece;
	enum validation_level validation_level;
	int datatype_entity_piece, datatype_piece;
	char title[ 256 ];
	char sub_title[ 256 ];
	char *begin_date_string, *end_date_string;
	char grace_begin_date_string[ 16 ] = {0};
	char grace_end_date_string[ 16 ] = {0};
	char *agr_filename;
	char *ftp_agr_filename;
	char *output_filename;
	char *pdf_output_filename;
	char *postscript_filename;
	char *aggregate_level_string;
	enum aggregate_level aggregate_level;
	char *datatype;
	char *station;
	char infrastructure_process[ 512 ];
	char units_converted_process[ 128 ];
	char *units_display;
	char *units;
	char data_process[ 1024 ];
	GRACE *grace;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char graph_identifier[ 128 ];
	char *email_address;
	char *chart_email_command_line;
	char *distill_landscape_flag;
	char *accumulate_yn;
	int page_width_pixels;
	int page_length_pixels;
	char *ymin;
	char *ymax;
	char *y_tick_major;
	char *y_tick_minor;
	char *gridlines_yn;
	char *plot_for_station_check_yn;
	char *units_converted;
	char *database_string = {0};
	boolean bar_chart = 0;
	boolean force_landscape_mode = 0;
	boolean bypass_data_collection_frequency;

	output_starting_argv_stderr( argc, argv );

	if ( argc < 21 )
	{
		fprintf(stderr,
"Usage: %s application role session station datatype begin_date end_date aggregate_level ymin ymax y_tick_major y_tick_minor gridlines_yn email_address validation_level aggregate_statistic symbols_yn plot_for_station_check_yn units_converted accumulate_yn [force_landscape_yn]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	role_name = argv[ 2 ];
	session = argv[ 3 ];
	station = argv[ 4 ];
	datatype = argv[ 5 ];
	begin_date_string = argv[ 6 ];
	end_date_string = argv[ 7 ];
	aggregate_level_string = argv[ 8 ];
	ymin = argv[ 9 ];
	ymax = argv[ 10 ];
	y_tick_major = argv[ 11 ];
	y_tick_minor = argv[ 12 ];
	gridlines_yn = argv[ 13 ];
	email_address = argv[ 14 ];
	validation_level_string = argv[ 15 ];
	aggregate_statistic_string = argv[ 16 ];
	symbols_yn = argv[ 17 ];
	plot_for_station_check_yn = argv[ 18 ];
	units_converted = argv[ 19 ];
	accumulate_yn = argv[ 20 ];

	if ( argc == 22 )
	{
		force_landscape_mode = (*argv[ 21 ] == 'y');
	}

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

	if ( !*gridlines_yn
	||   strcmp(	gridlines_yn,
			"gridlines_yn" ) == 0 )
	{
		gridlines_yn = "y";
	}

	hydrology_library_get_clean_begin_end_date(
					&begin_date_string,
					&end_date_string,
					application_name,
					station,
					datatype );

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date_string,
					&end_date_string,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	aggregate_level =
		aggregate_level_get_aggregate_level(
			aggregate_level_string );

	validation_level =
		validation_level_get_validation_level(
			validation_level_string);

	bypass_data_collection_frequency =
		datatype_get_bypass_data_collection_frequency(
				application_name,
				station,
				datatype );

	aggregate_statistic = aggregate_statistic_none;

	if ( aggregate_level != real_time )
	{
		aggregate_statistic =
			aggregate_statistic_get_aggregate_statistic(
					aggregate_statistic_string,
					aggregate_level );

		if ( aggregate_statistic == aggregate_statistic_none
		&&   datatype
		&&   *datatype
		&&   strcmp( datatype, "datatype" ) !=  0 )
		{
			aggregate_statistic =
				based_on_datatype_get_aggregate_statistic(
					application_name,
					datatype,
					aggregate_statistic_none );
		}
	}

	if ( *datatype && strcmp( datatype, "datatype" ) != 0 )
	{
		units = hydrology_library_get_units_string(
				&bar_chart,
				application_name,
				datatype );
	}
	else
	{
		units = "";
	}

	units_display =
		hydrology_library_get_datatype_units_display(
			application_name,
			datatype,
			units,
			units_converted,
			aggregate_statistic );

	if ( !units_display )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );
		printf( "%s\n",
			hydrology_library_get_output_invalid_units_error(
				datatype,
				units,
				units_converted ) );
		document_close();
		exit( 0 );
	}

	if ( units_converted
	&&   *units_converted
	&&   strcmp( units_converted, "units_converted" ) != 0 )
	{
		sprintf( units_converted_process,
			 "piece_replace.e '|' 0 \"%s\"",
			 units_converted );
	}
	else
	{
		strcpy( units_converted_process, "cat" );
	}

	sprintf(	infrastructure_process,
			INFRASTRUCTURE_PROCESS,
			application_name,
			station,
			plot_for_station_check_yn,
			aggregate_statistic_get_string(
				aggregate_statistic ),
			datatype,
			units_converted_process );

	if ( !bypass_data_collection_frequency )
	{
		sprintf(	data_process,
				PADDED_DATA_PROCESS,
				application_name,
				station,
				begin_date_string,
				end_date_string,
				aggregate_level_get_string( aggregate_level ),
				validation_level_get_string( validation_level ),
				aggregate_statistic_get_string(
					aggregate_statistic ),
				accumulate_yn,
				units_converted,
				datatype );
	}
	else
	{
		sprintf(	data_process,
				NON_PADDED_DATA_PROCESS,
				application_name,
				station,
				begin_date_string,
				end_date_string,
				aggregate_level_get_string( aggregate_level ),
				validation_level_get_string( validation_level ),
				aggregate_statistic_get_string(
					aggregate_statistic ),
				accumulate_yn,
				units_converted,
				datatype );
	}

	if ( aggregate_level == real_time 
	||   aggregate_level == half_hour
	||   aggregate_level == hourly )
	{
		datatype_entity_piece = 0;
		datatype_piece = 1;
		date_piece = 2;
		time_piece = 3;
		value_piece = 4;
	}
	else
	{
		datatype_entity_piece = 0;
		datatype_piece = 1;
		date_piece = 2;
		time_piece = -1;
		value_piece = 4;
	}

	strcpy( grace_begin_date_string, begin_date_string );
	strcpy( grace_end_date_string, end_date_string );

	if ( *session && strcmp( session, "session" ) != 0 )
	{
		sprintf( graph_identifier,
			 "%s_%s_%s",
			 station,
			 datatype,
			 session );
	}
	else
	{
		sprintf( graph_identifier,
			 "%s_%s_%d",
			 station,
			 datatype,
			 getpid() );
	}

	grace = grace_new_unit_graph_grace(
				application_name,
				role_name,
				infrastructure_process,
				data_process,
				argv[ 0 ],
				datatype_entity_piece,
				datatype_piece,
				date_piece,
				time_piece,
				value_piece,
				(char *)0 /* title */,
				(char *)0 /* sub_title */,
				0 /* not datatype_type_xyhilo */,
				do_cycle_colors );

	if ( list_length( grace->graph_list ) == 1 )
	{
		GRACE_GRAPH *grace_graph;

		grace_graph = list_get_first_pointer( grace->graph_list );

		if ( list_length( grace_graph->datatype_list ) == 1 )
		{
			hydrology_library_get_title(
				title,
				sub_title,
				validation_level,
				aggregate_statistic,
				aggregate_level,
				station,
				datatype,
				grace_begin_date_string,
				grace_end_date_string,
				*accumulate_yn );
		}
		else
		{
			hydrology_library_get_title(
				title,
				sub_title,
				validation_level,
				aggregate_statistic,
				aggregate_level,
				station,
				(char *)0 /* datatype */,
				grace_begin_date_string,
				grace_end_date_string,
				*accumulate_yn );
		}

		grace->title = title;
		grace->sub_title = sub_title;
	}
	else
	{
		hydrology_library_get_title(
				title,
				sub_title,
				validation_level,
				aggregate_statistic,
				aggregate_level,
				station,
				(char *)0 /* datatype */,
				grace_begin_date_string,
				grace_end_date_string,
				*accumulate_yn );

		grace->title = title;
		grace->sub_title = sub_title;
	}

	if ( !grace_set_begin_end_date( 	grace,
						grace_begin_date_string,
						grace_end_date_string ) )
	{
		DOCUMENT *document;

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
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

		document_output_body(
			document->application_name,
			document->onload_control_string );

		printf(
	"<h2>ERROR: Invalid date format format.</h2>" );
		document_close();
		exit( 1 );
	}

	grace->grace_output = application_get_grace_output( application_name );

	grace_get_filenames(
			&agr_filename,
			&ftp_agr_filename,
			&postscript_filename,
			&output_filename,
			&pdf_output_filename,
			application_name,
			appaserver_parameter_file->
				document_root,
			graph_identifier,
			grace->grace_output );

	if ( list_length( grace->graph_list ) < 4 )
	{
		grace_set_xaxis_ticklabel_angle(
					grace->graph_list,
					GRACE_TICKLABEL_ANGLE );
	}

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
				force_landscape_mode ) )
	{
		DOCUMENT *document;

		document = document_new( "", application_name );
		document_set_output_content_type( document );

		document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

		document_output_body(
			document->application_name,
			document->onload_control_string );

		printf( "<p>Warning: no graphs to display.\n" );
		printf(
"<p>If data exists, check station_datatype.plot_for_station_check_yn, datatype.units, or station_datatype.validation_required_yn.\n" );
		document_close();
		exit( 0 );
	}

	if ( *ymin && strcmp( ymin, "ymin" ) != 0 )
	{
		 grace_graph_set_global_world_min_y(	grace->graph_list,
							atof( ymin ) );
	}

	if ( *ymax && strcmp( ymax, "ymax" ) != 0 )
	{
		 grace_graph_set_global_world_max_y(	grace->graph_list,
							atof( ymax ) );
	}

	if ( *y_tick_major && strcmp( y_tick_major, "y_tick_major" ) != 0 )
	{
		 grace_graph_set_global_y_tick_major(	grace->graph_list,
							atof( y_tick_major ) );
	}

	if ( *y_tick_minor && strcmp( y_tick_minor, "y_tick_minor" ) != 0 )
	{
		 grace_graph_set_global_y_tick_minor(	grace->graph_list,
							atoi( y_tick_minor ) );
	}

	if ( *gridlines_yn != 'y' )
	{
		grace_graph_set_no_yaxis_grid_lines_offset(
				grace->graph_list,
				0 /* graph_offset */ );
	}

	chart_email_command_line =
			application_get_chart_email_command_line(
				application_name );

	grace->symbols = (*symbols_yn == 'y');

	if ( list_length( grace->graph_list ) == 1 )
	{
		grace_move_legend_bottom_left(
			list_get_first_pointer( grace->graph_list ),
			grace->landscape_mode );
	}

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
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>Internal ERROR: could not output charts in %s\n",
			argv[ 0 ] );
		document_close();
		exit( 0 );
	}

	if ( !*chart_email_command_line
	||   *chart_email_command_line == '#'
	||   strcmp( email_address, "email_address" ) == 0 )
	{
		grace_output_graph_window(
				application_name,
				pdf_output_filename,
				ftp_agr_filename,
				appaserver_parameter_file->
					appaserver_mount_point,
				1 /* with_document_output */,
				(char *)0 /* where_clause */ );
	}
	else
	{
		grace_email_graph(
				application_name,
				email_address,
				chart_email_command_line,
				output_filename,
				appaserver_parameter_file->
					appaserver_mount_point,
				1 /* with_document_output */,
				(char *)0 /* where_clause */ );
	}
	process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

