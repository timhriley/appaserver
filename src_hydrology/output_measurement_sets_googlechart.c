/* -------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/output_measurement_set_googlechart.c	*/
/* -------------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "folder.h"
#include "query.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "aggregate_level.h"
#include "validation_level.h"
#include "aggregate_statistic.h"
#include "hydrology_library.h"
#include "document.h"
#include "google_chart.h"
#include "appaserver_parameter_file.h"
#include "appaserver_link_file.h"
#include "environ.h"
#include "application_constants.h"
#include "datatype.h"

/* Constants */
/* --------- */
#define CHART_HEIGHT				200
#define SOURCE_FOLDER				"measurement"
#define DATE_PIECE				0
#define SELECT_LIST				"measurement_date,measurement_time,measurement_value"

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
boolean populate_unit_chart_data(
			LIST *unit_chart_datatype_list,
			DICTIONARY *date_time_dictionary,
			char *application_name,
			char *station_name,
			char *begin_date,
			char *end_date,
			enum validation_level validation_level,
			enum aggregate_level aggregate_level );

LIST *get_unit_chart_list(
			LIST *datatype_list );

boolean populate_unit_chart_list_data(
			LIST *unit_chart_list,
			char *application_name,
			char *station_name,
			char *begin_date,
			char *end_date,
			enum validation_level validation_level,
			enum aggregate_level aggregate_level );

char *get_sys_string(	char *application_name,
			char *where_clause,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *datatype_name,
			int date_piece,
			char *end_date_string );

int main( int argc, char **argv )
{
	char *application_name;
	char *begin_date, *end_date;
	char *aggregate_level_string;
	char *validation_level_string;
	char *station_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	enum aggregate_level aggregate_level;
	enum validation_level validation_level;
	GOOGLE_CHART *google_chart;
	char *chart_filename;
	char *prompt_filename;
	FILE *chart_file;
	char title[ 256 ];
	char sub_title[ 256 ];
	char *process_name;
	char plot_for_station_check_yn;
	char *database_string = {0};
	LIST *datatype_list;

	if ( argc != 9 )
	{
		fprintf( stderr, 
	"Usage: %s application process station begin_date end_date aggregate_level validation_level plot_for_station_check_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	station_name = argv[ 3 ];
	begin_date = argv[ 4 ];
	end_date = argv[ 5 ];
	aggregate_level_string = argv[ 6 ];
	validation_level_string = argv[ 7 ];
	if ( ( plot_for_station_check_yn = *argv[ 8 ] ) ) {};

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

	appaserver_parameter_file = new_appaserver_parameter_file();

	aggregate_level =
		aggregate_level_get_aggregate_level( aggregate_level_string );

	hydrology_library_get_clean_begin_end_date(
					&begin_date,
					&end_date,
					application_name,
					station_name,
					(char *)0 /* datatype_name */ );

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date,
					&end_date,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	if ( !*station_name || strcmp( station_name, "station" ) == 0 )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf(
		"<p>ERROR: Please select a station\n" );
		document_close();
		exit( 0 );
	}

	validation_level =
		validation_level_get_validation_level(
			validation_level_string);

	datatype_list =
		datatype_get_datatype_list(
					application_name,
					station_name,
					0 /* plot_for_station_check_yn */,
					aggregate_statistic_none );

	if ( !list_length( datatype_list ) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>Warning: datatypes for this station.\n" );
		document_close();
		exit( 0 );
	}

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

	appaserver_link_get_pid_filename(
			&chart_filename,
			&prompt_filename,
			application_name,
			appaserver_parameter_file->
				document_root,
			getpid(),
			process_name /* filename_stem */,
			"html" /* extension */ );

	chart_file = fopen( chart_filename, "w" );

	if ( !chart_file )
	{
		fprintf(stderr,
			"ERROR in %s/%s(): cannot open %s\n",
			__FILE__,
			__FUNCTION__,
			chart_filename );
		exit( 1 );
	}

	hydrology_library_get_title(
		title,
		sub_title,
		validation_level,
		aggregate_statistic_none,
		aggregate_level,
		station_name,
		(char *)0 /* datatype */,
		begin_date,
		end_date,
		'n' /* accumulate_yn */ );
	
	sprintf( title + strlen( title ),
		 " %s",
		 sub_title );

	google_chart = google_chart_new();

	google_chart->title = title;

	if ( ! ( google_chart->unit_chart_list =
			get_unit_chart_list(
				datatype_list ) ) )
	{
		printf(
		"<h2>Warning: no datatypes to display.</h2>\n" );
		document_close();
		exit( 0 );
	}

	if ( !populate_unit_chart_list_data(
			google_chart->unit_chart_list,
			application_name,
			station_name,
			begin_date,
			end_date,
			validation_level,
			aggregate_level ) )
	{
		printf(
		"<h2>Warning: nothing was selected to display.</h2>\n" );
		document_close();
		exit( 0 );
	}

	google_chart->output_chart_list =
		google_chart_unit_get_output_chart_list(
			google_chart->unit_chart_list,
			GOOGLE_CHART_WIDTH,
			CHART_HEIGHT );

	google_chart_output_all_charts(
			chart_file,
			google_chart->output_chart_list,
			google_chart->title,
			google_chart->sub_title,
			google_chart->stylesheet );

	fclose( chart_file );

	google_chart_output_graph_window(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point,
				0 /* not with_document_output */,
				process_name,
				prompt_filename,
				(char *)0 /* where_clause */ );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );

	return 0;

} /* main() */

char *get_sys_string(	char *application_name,
			char *where_clause,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *datatype_name,
			int date_piece,
			char *end_date_string )
{
	char sys_string[ 4096 ];
	char intermediate_process[ 1024 ];
	char trim_time_process[ 1024 ];
	char *select_list_string;

	select_list_string = SELECT_LIST;

	if ( aggregate_statistic == aggregate_statistic_none )
	{
		aggregate_statistic =
			based_on_datatype_get_aggregate_statistic(
				application_name,
				datatype_name,
				aggregate_statistic );
	}

	if ( aggregate_level == real_time )
	{
		strcpy( intermediate_process, "cat" );
	}
	else
	{
		sprintf( intermediate_process, 
			 "real_time2aggregate_value.e %s %d %d %d '^' %s n %s",
			 aggregate_statistic_get_string( aggregate_statistic ),
			 date_piece,
			 date_piece + 1,
			 date_piece + 2,
			 aggregate_level_get_string( aggregate_level ),
			 end_date_string );

		if ( aggregate_level != half_hour
		&&   aggregate_level != hourly )
		{
			sprintf( trim_time_process,
			 	"piece_inverse.e %d ','",
			 	date_piece + 1 );
		}
		else
		{
			strcpy( trim_time_process, "cat" );
		}
	}

/*
	sprintf( sys_string,
		 "get_folder_data	application=%s			  "
		 "			folder=%s			  "
		 "			select='%s'			  "
		 "			where=\"%s\"			 |"
		 "%s							 |"
		 "pad_missing_times.e '^' 0,1,2 %s %s 0000 %s 2359 0 '%s'|"
		 "cat							  ",
		 application_name,
		 SOURCE_FOLDER,
		 select_list_string,
		 where_clause,
		 intermediate_process,
		 aggregate_level_get_string( aggregate_level ),
		 begin_date_string,
		 end_date_string,
		 hydrology_library_get_expected_count_list_string(
			application_name, station_name, datatype_name, '|' ) );
*/

	sprintf( sys_string,
		 "get_folder_data	application=%s			  "
		 "			folder=%s			  "
		 "			select='%s'			  "
		 "			where=\"%s\"			 |"
		 "%s							 |"
		 "cat							  ",
		 application_name,
		 SOURCE_FOLDER,
		 select_list_string,
		 where_clause,
		 intermediate_process );

	return strdup( sys_string );

} /* get_sys_string() */

LIST *get_unit_chart_list( LIST *datatype_list )
{
	DATATYPE *datatype;
	char yaxis_label[ 128 ];
	GOOGLE_UNIT_CHART *unit_chart;
	GOOGLE_UNIT_DATATYPE *unit_datatype;
	LIST *unit_list;
	char *unit;
	LIST *unit_chart_list;
	LIST *datatypes_for_unit_list;

	if ( !list_length( datatype_list ) ) return (LIST *)0;

	unit_list =
		datatype_list_get_unique_unit_list(
			datatype_list );

	if ( !list_rewind( unit_list ) ) return (LIST *)0;

	unit_chart_list = list_new();

	do {
		unit = list_get_pointer( unit_list );

		datatypes_for_unit_list =
			datatype_get_datatypes_for_unit(
				datatype_list,
				unit );

		if ( !list_rewind( datatypes_for_unit_list ) )
			continue;

		unit_chart = google_unit_chart_new( unit );

		list_append_pointer(
			unit_chart_list,
			unit_chart );
			
		do {
			datatype =
				list_get_pointer(
					datatypes_for_unit_list );

			unit_datatype =
				google_unit_datatype_new(
					datatype->datatype_name );

			list_append_pointer(	unit_chart->unit_datatype_list,
						unit_datatype );

			/* unit_chart->bar_chart = unit_datatype->bar_chart; */

			sprintf(yaxis_label,
				"%s (%s)",
				datatype->datatype_name,
				datatype->units->units_name );
	
			unit_chart->yaxis_label = strdup( yaxis_label );
	
		} while( list_next( datatypes_for_unit_list ) );

	} while( list_next( unit_list ) );

	return unit_chart_list;

} /* get_unit_chart_list() */

boolean populate_unit_chart_list_data(
			LIST *unit_chart_list,
			char *application_name,
			char *station_name,
			char *begin_date,
			char *end_date,
			enum validation_level validation_level,
			enum aggregate_level aggregate_level )
{
	GOOGLE_UNIT_CHART *unit_chart;

	if ( !list_rewind( unit_chart_list ) ) return 0;

	do {
		unit_chart = list_get_pointer( unit_chart_list );

		populate_unit_chart_data(
			unit_chart->unit_datatype_list,
			unit_chart->date_time_dictionary,
			application_name,
			station_name,
			begin_date,
			end_date,
			validation_level,
			aggregate_level );

	} while( list_next( unit_chart_list ) );

	return 1;

} /* populate_unit_chart_list_data() */

boolean populate_unit_chart_data(
			LIST *unit_chart_datatype_list,
			DICTIONARY *date_time_dictionary,
			char *application_name,
			char *station_name,
			char *begin_date,
			char *end_date,
			enum validation_level validation_level,
			enum aggregate_level aggregate_level )
{
	int got_input = 0;
	GOOGLE_UNIT_DATATYPE *datatype;
	char where_clause[ 1024 ];
	char *sys_string;

	if ( !list_rewind( unit_chart_datatype_list ) ) return 0;

	do {
		datatype = list_get_pointer( unit_chart_datatype_list );

		if ( *end_date
		&&   strcmp( end_date, "end_date" ) != 0 )
		{
			sprintf(where_clause,
			 	"station = '%s' and			"
			 	"datatype = '%s' and			"
				"measurement_date between '%s' and '%s'	",
				station_name,
				datatype->datatype_name,
				begin_date,
				end_date );
		}
		else
		{
			sprintf(where_clause,
			 	"station = '%s' and			"
			 	"datatype = '%s' and			"
				"measurement_date = '%s'		",
				station_name,
				datatype->datatype_name,
				begin_date );
		}
	
		strcat( where_clause,
			/* ----------------------- */
			/* Returns program memory. */
			/* ----------------------- */
			hydrology_library_provisional_where(
				validation_level ) );

		sys_string =
			get_sys_string(	
				application_name,
				where_clause,
				aggregate_level,
				aggregate_statistic_none,
				datatype->datatype_name,
				DATE_PIECE,
				end_date );

		if ( google_chart_value_hash_table_set(
				datatype->value_hash_table,
				date_time_dictionary,
				sys_string,
				0  /* date_piece */,
				1  /* time_piece */,
				2  /* value_piece */,
				'^'/* delimiter */ ) )
		{
			got_input = 1;
		}

		free( sys_string );

	} while( list_next( unit_chart_datatype_list ) );

	return got_input;

} /* populate_unit_chart_data() */

