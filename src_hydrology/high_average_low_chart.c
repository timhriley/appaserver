/* high_average_low_chart.c		       		*/
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
#include "session.h"
#include "grace.h"
#include "date.h"
#include "application.h"
#include "environ.h"
#include "basename.h"
#include "process.h"
#include "hydrology_library.h"

/* Constants */
/* --------- */
#define PROCESS_NAME		"chart_high_average_low"
#define INFRASTRUCTURE_PROCESS	"%s/%s/datatype_unit_record_list.sh %s %s n average %s | %s"
#define DATA_PROCESS		"%s/%s/measurement_record_list.sh %s %s %s:%s %s %s average:n y \"%s\" \"%s\""

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *role_name;
	int date_piece, time_piece, value_piece;
	int datatype_entity_piece, datatype_piece;
	char title[ 256 ];
	char sub_title[ 256 ];
	char *begin_date_string, *end_date_string;
	char grace_begin_date_string[ 16 ] = {0};
	char grace_end_date_string[ 16 ] = {0};
	char buffer[ 4096 ];
	char *ftp_agr_filename;
	char *ftp_output_filename;
	char *output_filename;
	char *agr_filename;
	char *postscript_filename;
	char *aggregate_level_string;
	enum aggregate_level aggregate_level;
	char *datatype;
	char *station;
	char *units_converted;
	char infrastructure_process[ 512 ];
	char data_process[ 512 ];
	char units_converted_process[ 128 ];
	GRACE *grace;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char graph_identifier[ 128 ];
	char *email_address;
	char *chart_email_command_line;
	char *distill_landscape_flag;
	int page_width_pixels;
	int page_length_pixels;
	char *database_string = {0};
	char *units;
	char *units_display;
	boolean bar_chart;

	output_starting_argv_stderr( argc, argv );

	if ( argc != 12 )
	{
		fprintf(stderr,
"Usage: %s application ignored ignored role station datatype begin_date end_date aggregate_level email_address units_converted\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	/* session = argv[ 2 ]; */
	/* login_name = argv[ 3 ]; */
	role_name = argv[ 4 ];
	station = argv[ 5 ];
	datatype = argv[ 6 ];
	begin_date_string = argv[ 7 ];
	end_date_string = argv[ 8 ];
	aggregate_level_string = argv[ 9 ];
	email_address = argv[ 10 ];
	units_converted = argv[ 11 ];

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

	appaserver_parameter_file = new_appaserver_parameter_file();

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
		aggregate_level_get_aggregate_level( aggregate_level_string );

	if ( aggregate_level == real_time
	||   aggregate_level == half_hour )
	{
		aggregate_level = hourly;
	}

	units = hydrology_library_get_units_string(
				&bar_chart,
				application_name,
				datatype );

	units_display =
		hydrology_library_get_datatype_units_display(
			application_name,
			datatype,
			units,
			units_converted,
			aggregate_statistic_none );

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
			appaserver_parameter_file->appaserver_mount_point,
			application_get_relative_source_directory(
				application_name ),
			application_name,
			station,
			datatype,
			units_converted_process );

	sprintf(	data_process,
			DATA_PROCESS,
			appaserver_parameter_file->appaserver_mount_point,
			application_get_relative_source_directory(
				application_name ),
			application_name,
			station,
			begin_date_string,
			end_date_string,
			aggregate_level_get_string( aggregate_level ),
			"validation_level",
			units_converted,
			datatype );

	if ( aggregate_level == hourly )
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

	sprintf(	title, "%s High/Avg/Low %s Values", 
			station, 
			format_initial_capital(	buffer,
					aggregate_level_get_string(
						aggregate_level ) ) );

	strcpy( grace_begin_date_string,
		begin_date_string );
	strcpy( grace_end_date_string,
		end_date_string );

	sprintf( sub_title, "Beginning: %s Ending: %s",
		 grace_begin_date_string,
		 grace_end_date_string );

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
				strdup( title ),
				strdup( sub_title ),
				1 /* datatype_type_xyhilo */,
				no_cycle_colors );

	if ( ! grace_set_begin_end_date( 	grace,
						grace_begin_date_string,
						grace_end_date_string ) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf(
	"<h2>ERROR: Invalid date format format.</h2>" );
		document_close();
		exit( 1 );
	}

	grace->grace_output =
		application_get_grace_output( application_name );

	sprintf(	graph_identifier,
			"%s_%s_%d",
			station,
			datatype,
			getpid() );

	grace_get_filenames(
			&agr_filename,
			&ftp_agr_filename,
			&postscript_filename,
			&output_filename,
			&ftp_output_filename,
			application_name,
			appaserver_parameter_file->
				document_root,
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
		document_quick_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>Warning: no graphs to display.\n" );
		printf(
"<p>If data exists, check station_datatype.chart_datatype_yn or datatype.units.\n" );
		document_close();
		exit( 0 );
	}

	grace_set_world_min_y_based_on_grace_point_low( grace->graph_list );
	grace_set_world_max_y_based_on_grace_point_high( grace->graph_list );

	chart_email_command_line =
			application_get_chart_email_command_line(
				application_name );

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
				ftp_output_filename,
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

