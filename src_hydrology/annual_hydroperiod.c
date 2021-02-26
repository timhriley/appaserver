/* src_hydrology/annual_hydroperiod.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include "station.h"
#include "timlib.h"
#include "piece.h"
#include "folder.h"
#include "grace.h"
#include "html_table.h"
#include "application.h"
#include "station_datatype.h"
#include "hydrology_library.h"
#include "annual_hydroperiod.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"

ANNUAL_HYDROPERIOD *annual_hydroperiod_new(
					double elevation_offset )
{
	ANNUAL_HYDROPERIOD *annual_hydroperiod;

	annual_hydroperiod =
		(ANNUAL_HYDROPERIOD *)
			calloc( 1, sizeof( ANNUAL_HYDROPERIOD ) );

	annual_hydroperiod->elevation_offset = elevation_offset;

	return annual_hydroperiod;
} /* annual_hydroperiod_new() */

LIST *annual_hydroperiod_get_station_list(
					char **station_elevation_null,
					char *application_name,
					char *station_list_string,
					char *stage_datatype_list_string,
					double elevation_offset,
					int begin_year,
					int end_year,
					boolean detail )
{
	LIST *station_list;
	ANNUAL_HYDROPERIOD_STATION *station;
	LIST *station_name_list;
	LIST *datatype_name_list;
	char *station_name;
	char *stage_datatype_name;

	station_name_list =
		list_string2list(
			station_list_string, ',' );

	datatype_name_list =
		list_string2list(
			stage_datatype_list_string, ',' );

	if (	!list_length( station_name_list )
	||	( list_length( station_name_list ) !=
		  list_length( datatype_name_list ) ) )
	{
		return (LIST *)0;
	}

	station_list = list_new_list();
	list_rewind( station_name_list );
	list_rewind( datatype_name_list );

	do {
		station_name =
			list_get_pointer(
				station_name_list );

		stage_datatype_name =
			list_get_pointer(
				datatype_name_list );

		station =
			annual_hydroperiod_station_new(
				station_name,
				stage_datatype_name,
				(char *)0 /* model_name */,
				(char *)0 /* alternative_name */ );

		station->elevation =
			station_get_ground_surface_elevation_ft(
				station_elevation_null,
				application_name,
				station_name );

		if ( *station_elevation_null ) return (LIST *)0;

		station->threshold =
			elevation_offset +
			station->elevation;

		station->year_list =
			annual_hydroperiod_get_year_list(
				application_name,
				station->station_name,
				station->stage_datatype_name,
				station->threshold,
				begin_year,
				end_year,
				detail );

		list_append_pointer( station_list, station );

		list_next( datatype_name_list );
	} while( list_next( station_name_list ) );
	return station_list;
} /* annual_hydroperiod_get_station_list() */

#ifdef NOT_DEFINED
LIST *annual_hydroperiod_get_alt_station_list(
					char **station_elevation_null,
					char *application_name,
					LIST *model_list,
					LIST *alternative_list,
					LIST *station_name_list,
					LIST *datatype_name_list,
					double elevation_offset,
					int begin_year,
					int end_year,
					boolean detail )
{
	LIST *station_list;
	ANNUAL_HYDROPERIOD_STATION *station;
	char *model_name;
	char *alternative_name;
	char *station_name;
	char *datatype_name;
	double ground_surface_elevation;
	boolean station_not_found;

	if (	!list_length( station_name_list )
	||	( list_length( station_name_list ) !=
		  list_length( datatype_name_list ) ) )
	{
		return (LIST *)0;
	}

	if (	!list_length( model_list )
	||	( list_length( model_list ) !=
		  list_length( alternative_list ) ) )
	{
		return (LIST *)0;
	}

	station_list = list_new_list();
	list_rewind( model_list );
	list_rewind( alternative_list );

	do {
		model_name = list_get_pointer( model_list );
		alternative_name = list_get_pointer( alternative_list );

		list_rewind( station_name_list );
		list_rewind( datatype_name_list );

		do {
			station_name =
				list_get_pointer(
					station_name_list );

			ground_surface_elevation =
				station_get_ground_surface_elevation(
					&station_not_found,
					station_elevation_null,
					application_name,
					model_name,
					alternative_name,
					station_name );
	
			if ( station_not_found )
			{
				list_next( datatype_name_list );
				continue;
			}

/*
			if ( *station_elevation_null ) return (LIST *)0;
*/
	
			datatype_name =
				list_get_pointer(
					datatype_name_list );
	
			station =
				annual_hydroperiod_station_new(
					station_name,
					datatype_name,
					model_name,
					alternative_name );
	
			station->elevation = ground_surface_elevation;

			station->threshold = elevation_offset;

/* Stub */
/* ---- */
elevation_offset = 0.0;
/*
			station->threshold =
				elevation_offset +
				ground_surface_elevation;
*/
	
			station->year_list =
				annual_hydroperiod_get_alt_year_list(
					application_name,
					model_name,
					alternative_name,
					station_name,
					datatype_name,
					station->threshold,
					begin_year,
					end_year,
					detail );
	
			list_append_pointer( station_list, station );
	
			list_next( datatype_name_list );

		} while( list_next( station_name_list ) );
		list_next( alternative_list );
	} while( list_next( model_list ) );

	return station_list;
} /* annual_hydroperiod_get_alt_station_list() */
#endif

ANNUAL_HYDROPERIOD_DATA *annual_hydroperiod_data_new(
					char *measurement_date,
					double measurement_value )
{
	ANNUAL_HYDROPERIOD_DATA *annual_hydroperiod_data;

	annual_hydroperiod_data =
		(ANNUAL_HYDROPERIOD_DATA *)
			calloc( 1, sizeof( ANNUAL_HYDROPERIOD_DATA ) );

	annual_hydroperiod_data->measurement_date = measurement_date;
	annual_hydroperiod_data->measurement_value = measurement_value;
	return annual_hydroperiod_data;
} /* annual_hydroperiod_data_new() */

ANNUAL_HYDROPERIOD_YEAR *annual_hydroperiod_year_new(
					int year )
{
	ANNUAL_HYDROPERIOD_YEAR *annual_hydroperiod_year;

	annual_hydroperiod_year =
		(ANNUAL_HYDROPERIOD_YEAR *)
			calloc( 1, sizeof( ANNUAL_HYDROPERIOD_YEAR ) );

	annual_hydroperiod_year->year = year;
	return annual_hydroperiod_year;
} /* annual_hydroperiod_year_new() */

ANNUAL_HYDROPERIOD_WETTING_EVENT *annual_hydroperiod_wetting_event_new(
					char *begin_date_string,
					boolean detail )
{
	ANNUAL_HYDROPERIOD_WETTING_EVENT *wetting_event;

	wetting_event =
		(ANNUAL_HYDROPERIOD_WETTING_EVENT *)
			calloc( 1, sizeof( ANNUAL_HYDROPERIOD_WETTING_EVENT ) );

	wetting_event->begin_date = begin_date_string;
	if ( detail ) wetting_event->data_list = list_new();
	return wetting_event;
} /* annual_hydroperiod_wetting_event_new() */

ANNUAL_HYDROPERIOD_STATION *annual_hydroperiod_station_new(
					char *station_name,
					char *stage_datatype_name,
					char *model_name,
					char *alternative_name )
{
	ANNUAL_HYDROPERIOD_STATION *annual_hydroperiod_station;
	char full_station_name[ 128 ];

	if ( model_name && alternative_name )
	{
		sprintf(	full_station_name,
				"%s/%s/%s",
				model_name,
				alternative_name,
				station_name );
	}
	else
	{
		strcpy( full_station_name, station_name );
	}

	annual_hydroperiod_station =
		(ANNUAL_HYDROPERIOD_STATION *)
			calloc( 1, sizeof( ANNUAL_HYDROPERIOD_STATION ) );

	annual_hydroperiod_station->station_name = strdup( full_station_name );
	annual_hydroperiod_station->stage_datatype_name = stage_datatype_name;
	return annual_hydroperiod_station;
} /* annual_hydroperiod_station_new() */

void annual_hydroperiod_display( LIST *station_list )
{
	ANNUAL_HYDROPERIOD_STATION *station;
	ANNUAL_HYDROPERIOD_YEAR *year;
	ANNUAL_HYDROPERIOD_DATA *data;
	ANNUAL_HYDROPERIOD_WETTING_EVENT *wetting_event;

	if ( !list_rewind( station_list ) ) return;
	do {
		station = list_get_pointer( station_list );

		printf("station=%s,datatype=%s,threshold=%lf",
				station->station_name,
				station->stage_datatype_name,
				station->threshold );

		if ( !list_rewind( station->year_list ) ) continue;

		do {
			year = list_get_pointer( station->year_list );

			printf(":year=%d,above=%d",
					year->year,
					year->above_threshold_count );

			if ( !list_rewind( year->wetting_event_list ) )
			{
				continue;
			}

			do {
				wetting_event =
					list_get_pointer(
						year->wetting_event_list );

				printf(":begin=%s,end=%s,count=%d",
						wetting_event->begin_date,
						wetting_event->end_date,
						wetting_event->days_count );

				if ( !wetting_event->data_list ) continue;

				list_rewind( wetting_event->data_list );
				do {
					data =	list_get_pointer(
						wetting_event->data_list );

					printf(":date=%s,value=%.3lf",
						data->measurement_date,
						data->measurement_value );

				} while( list_next(
						wetting_event->data_list ) );

			} while( list_next( year->wetting_event_list ) );

		} while( list_next( station->year_list ) );
		printf( "\n" );
	} while( list_next( station_list ) );
} /* annual_hydroperiod_display() */

LIST *annual_hydroperiod_get_year_list(
				char *application_name,
				char *station_name,
				char *stage_datatype_name,
				double threshold,
				int begin_year,
				int end_year,
				boolean detail )
{
	char *where_clause;
	char sys_string[ 1024 ];
	int this_year;
	ANNUAL_HYDROPERIOD_YEAR *year;
	FILE *input_pipe;
	LIST *year_list = list_new();
	char *real_time_process =
		"real_time2aggregate_value.e average 0 1 2 delimiter daily n";
	char *select = "measurement_date,measurement_time,measurement_value";

	for( this_year = begin_year; this_year <= end_year; this_year++ )
	{
		year = annual_hydroperiod_year_new( this_year );

		where_clause =
			annual_hydroperiod_get_where_clause(
				station_name,
				stage_datatype_name,
				this_year );

		sprintf(sys_string,
		 	"get_folder_data	application=%s		    "
		 	"			select=%s		    "
		 	"			folder=measurement	    "
		 	"			where=\"%s\"		   |"
			"%s						   |"
			"piece_inverse.e 1 '%c'				    ",
		 	application_name,
			select,
		 	where_clause,
			real_time_process,
			FOLDER_DATA_DELIMITER );

		input_pipe = popen( sys_string, "r" );
		year->wetting_event_list =
			annual_hydroperiod_get_wetting_event_list(
				&year->above_threshold_count,
				input_pipe,
				threshold,
				detail );
		pclose( input_pipe );
		list_append_pointer( year_list, year );
	}

	return year_list;
} /* annual_hydroperiod_get_year_list() */

LIST *annual_hydroperiod_get_alt_year_list(
				char *application_name,
				char *model_name,
				char *alternative_name,
				char *station_name,
				char *stage_datatype_name,
				double threshold,
				int begin_year,
				int end_year,
				boolean detail )
{
	char *where_clause;
	char sys_string[ 1024 ];
	int this_year;
	ANNUAL_HYDROPERIOD_YEAR *year;
	FILE *input_pipe;
	LIST *year_list = list_new();
	char *real_time_process =
		"real_time2aggregate_value.e average 0 1 2 delimiter daily n";
	char *select = "date,'null',value";

	for( this_year = begin_year; this_year <= end_year; this_year++ )
	{
		year = annual_hydroperiod_year_new( this_year );

		where_clause =
			annual_hydroperiod_get_alt_where_clause(
				model_name,
				alternative_name,
				station_name,
				stage_datatype_name,
				this_year );

		sprintf(sys_string,
		 	"get_folder_data	application=%s		    "
		 	"			select=%s		    "
		 	"			folder=model_results	    "
		 	"			where=\"%s\"		   |"
			"%s						   |"
			"piece_inverse.e 1 '%c'				    ",
		 	application_name,
			select,
		 	where_clause,
			real_time_process,
			FOLDER_DATA_DELIMITER );

		input_pipe = popen( sys_string, "r" );
		year->wetting_event_list =
			annual_hydroperiod_get_wetting_event_list(
				&year->above_threshold_count,
				input_pipe,
				threshold,
				detail );
		pclose( input_pipe );
		list_append_pointer( year_list, year );
	}

	return year_list;
} /* annual_hydroperiod_get_alt_year_list() */

char *annual_hydroperiod_get_where_clause(
			char *station_name,
			char *stage_datatype_name,
			int year )
{
	static char where_clause[ 1024 ];
	char last_day_previous_year_string[ 16 ];
	char end_date_string[ 16 ];

	sprintf( last_day_previous_year_string, "%d-01-01", year );
	sprintf( end_date_string, "%d-12-31", year );
	sprintf( where_clause,
"station = '%s' and datatype = '%s' and measurement_date between '%s' and '%s'",
		 station_name,
		 stage_datatype_name,
		 last_day_previous_year_string,
		 end_date_string );

	return where_clause;
} /* annual_hydroperiod_get_where_clause() */

char *annual_hydroperiod_get_alt_where_clause(
			char *model_name,
			char *alternative_name,
			char *station_name,
			char *stage_datatype_name,
			int year )
{
	static char where_clause[ 1024 ];
	char last_day_previous_year_string[ 16 ];
	char end_date_string[ 16 ];

	sprintf( last_day_previous_year_string, "%d-01-01", year );
	sprintf( end_date_string, "%d-12-31", year );
	sprintf( where_clause,
"model = '%s' and alt = '%s' and station = '%s' and datatype = '%s' and date between '%s' and '%s'",
		 model_name,
		 alternative_name,
		 station_name,
		 stage_datatype_name,
		 last_day_previous_year_string,
		 end_date_string );

	return where_clause;
} /* annual_hydroperiod_get_alt_where_clause() */

LIST *annual_hydroperiod_get_wetting_event_list(
				int *above_threshold_count,
				FILE *input_pipe,
				double threshold,
				boolean detail )
{
	double average_stage_for_day = 0.0;
	double average_stage_for_yesterday = 0.0;
	char last_date_wetting_event_string[ 32 ];
	char input_buffer[ 128 ];
	char measurement_value_string[ 32 ];
	char measurement_date_string[ 32 ];
	boolean first_time = 1;
	ANNUAL_HYDROPERIOD_WETTING_EVENT *wetting_event = {0};
	ANNUAL_HYDROPERIOD_DATA *data;
	LIST *wetting_event_list = list_new_list();

	*above_threshold_count = 0;

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	measurement_date_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	measurement_value_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		if ( !*measurement_value_string
		||   strcmp( measurement_value_string, "null" ) == 0 )
		{
			continue;
		}

		average_stage_for_day = atof( measurement_value_string );

		if ( average_stage_for_day >= threshold )
			(*above_threshold_count)++;

		if ( first_time )
		{
			if ( average_stage_for_day >= threshold )
			{
				wetting_event =
				annual_hydroperiod_wetting_event_new(
					strdup( measurement_date_string ),
					detail );

				list_append_pointer(
					wetting_event_list,
					wetting_event );
				wetting_event->days_count = 1;
				strcpy(	last_date_wetting_event_string,
					measurement_date_string );
			}
			first_time = 0;
		}
		else
		/* ---------------------- */
		/* If begin wetting event */
		/* ---------------------- */
		if ( average_stage_for_day >= threshold
		&&   average_stage_for_yesterday < threshold )
		{
			if ( wetting_event )
			{
				wetting_event->end_date =
					strdup(
					last_date_wetting_event_string );
			}

			wetting_event =
				annual_hydroperiod_wetting_event_new(
					strdup( measurement_date_string ),
					detail );

			list_append_pointer(
				wetting_event_list,
				wetting_event );

			wetting_event->days_count = 1;

			strcpy(	last_date_wetting_event_string,
				measurement_date_string );
		}
		else
		/* -------------------------------- */
		/* If inside existing wetting event */
		/* -------------------------------- */
		if ( wetting_event
		&&   average_stage_for_yesterday >= threshold
		&&   average_stage_for_day >= threshold )
		{
			wetting_event->days_count++;

			strcpy(	last_date_wetting_event_string,
				measurement_date_string );
		}

		if ( detail
		&&   wetting_event
		&&   average_stage_for_day >= threshold )
		{
			data = annual_hydroperiod_data_new(
					strdup( measurement_date_string ),
					average_stage_for_day );
			list_append_pointer(
				wetting_event->data_list,
				data );
		}

		average_stage_for_yesterday = average_stage_for_day;
	}

	/* ------------------------------------- */
	/* If end wetting event on the last one. */
	/* ------------------------------------- */
	if ( wetting_event
	&&   average_stage_for_yesterday >= threshold
	&&   average_stage_for_day < threshold )
	{
		wetting_event->end_date =
			strdup( measurement_date_string );
	}
	else
	if ( wetting_event
	&&   !wetting_event->end_date
	&&   average_stage_for_day < threshold )
	{
		wetting_event->end_date =
			strdup( last_date_wetting_event_string );
	}

	return wetting_event_list;

} /* annual_hydroperiod_get_wetting_event_list() */

int annual_hydroperiod_get_minimum_days_count(
				       	LIST *wetting_event_list )
{
	ANNUAL_HYDROPERIOD_WETTING_EVENT *wetting_event;
	int minimum_days_count = INT_MAX;

	if ( !list_rewind( wetting_event_list ) )
		return minimum_days_count;

	do {
		wetting_event = list_get_pointer( wetting_event_list );
		if ( wetting_event->days_count < minimum_days_count )
			minimum_days_count = wetting_event->days_count;
	} while( list_next( wetting_event_list ) );

	return minimum_days_count;
} /* annual_hydroperiod_get_minimum_days_count() */

int annual_hydroperiod_get_maximum_days_count(
				       	LIST *wetting_event_list )
{
	ANNUAL_HYDROPERIOD_WETTING_EVENT *wetting_event;
	int maximum_days_count = INT_MIN;

	if ( !list_rewind( wetting_event_list ) )
		return maximum_days_count;

	do {
		wetting_event = list_get_pointer( wetting_event_list );
		if ( wetting_event->days_count > maximum_days_count )
			maximum_days_count = wetting_event->days_count;
	} while( list_next( wetting_event_list ) );

	return maximum_days_count;
} /* annual_hydroperiod_get_maximum_days_count() */

void annual_hydroperiod_output_text_file_summary(
				FILE *output_pipe,
				char *title,
				ANNUAL_HYDROPERIOD *annual_hydroperiod,
				char *application_name )
{
	ANNUAL_HYDROPERIOD_STATION *station;
	ANNUAL_HYDROPERIOD_YEAR *year;

	if ( list_rewind( annual_hydroperiod->station_list ) )
	{
		do {
			station =
				list_get_pointer(
					annual_hydroperiod->station_list );

			if ( !list_rewind( station->year_list ) ) continue;

			hydrology_library_output_station_text_file(
					output_pipe,
					application_name,
					station->station_name );

		} while( list_next( annual_hydroperiod->station_list ) );
	}

	fprintf(output_pipe,
	"#%s\n#Station|Datatype|Elevation|Threshold|Year|WettingEventCount|Discontinuous|Minimum|Average|Maximum\n",
		title );

	if ( !list_rewind( annual_hydroperiod->station_list ) )
	{
		return;
	}

	do {
		station = list_get_pointer( annual_hydroperiod->station_list );

		if ( !list_rewind( station->year_list ) ) continue;

		do {
			year = list_get_pointer( station->year_list );

			fprintf(output_pipe,
				"%s",
				station->station_name );

			fprintf(output_pipe,
				"|%s",
				 station->stage_datatype_name );

			fprintf(output_pipe,
			 	"|%.2lf",
			 	station->elevation );

			fprintf(output_pipe,
			 	"|%.2lf",
			 	station->threshold );

			fprintf(output_pipe,
			 	"|%d",
			 	year->year );

			fprintf(output_pipe,
			 	"|%d",
			 	list_length( year->wetting_event_list ) );

			fprintf(output_pipe,
			 	"|%d",
			 	year->above_threshold_count );

			if ( !list_length( year->wetting_event_list ) )
			{
				fprintf(output_pipe,
			 		"|Null|Null|Null" );
			}
			else
			{
				fprintf(output_pipe,
			 		"|%d",
			 	annual_hydroperiod_get_minimum_days_count(
				       	year->wetting_event_list ) );

				fprintf(output_pipe,
			 		"|%.1lf",
			 		(double)year->above_threshold_count /
			 		(double)
					list_length(
						year->
						wetting_event_list ) );
				fprintf(output_pipe,
			 		"|%d",
			 	annual_hydroperiod_get_maximum_days_count(
				       	year->wetting_event_list ) );

			}
			fprintf(output_pipe, "\n" );

		} while( list_next( station->year_list ) );
	} while( list_next( annual_hydroperiod->station_list ) );

} /* annual_hydroperiod_output_text_file_summary() */

boolean annual_hydroperiod_output_chart_discontinuous(
				char *report_title,
				int begin_year,
				int end_year,
				ANNUAL_HYDROPERIOD *annual_hydroperiod,
				char *argv_0,
				char *application_name,
				char *document_root_directory,
				double elevation_offset )
{
	char title[ 512 ];
	ANNUAL_HYDROPERIOD_STATION *station;
	ANNUAL_HYDROPERIOD_YEAR *year;
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
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];
	char point_list_string[ 1024 ];
	char sub_title[ 128 ];

	annual_hydroperiod_get_report_title(
				title,
				report_title,
				begin_year,
				end_year );

	sprintf(	sub_title,
			"Discontinuations Above Land Surface Plus %.1lf",
			elevation_offset );

	if ( !list_length( annual_hydroperiod->station_list ) )
	{
		return 0;
	}

	grace = grace_new_unit_graph_grace(
				(char *)0 /* application_name */,
				(char *)0 /* role_name */,
				(char *)0 /* infrastructure_process */,
				(char *)0 /* data_process */,
				argv_0,
				GRACE_DATATYPE_ENTITY_PIECE,
				GRACE_DATATYPE_PIECE,
				GRACE_DATE_PIECE,
				GRACE_TIME_PIECE,
				GRACE_VALUE_PIECE,
				strdup( title ),
				sub_title,
				0 /* not datatype_type_xyhilo */,
				no_cycle_colors_if_multiple_datatypes );

	sprintf( begin_date_string, "%d-01-01", begin_year );
	sprintf( end_date_string, "%d-12-31", end_year );

	if ( !grace_set_begin_end_date(	grace,
					begin_date_string,
					end_date_string ) )
	{
		printf(
		"<h3>ERROR: Invalid date format format.</h3>" );
		document_close();
		exit( 1 );
	}

	grace_graph = grace_new_grace_graph();

	grace_graph->units = "count";
	grace_graph->world_min_y = -10;
	grace_graph->y_tick_major = 30;

	list_rewind( annual_hydroperiod->station_list );
	do {
		station = list_get_pointer( annual_hydroperiod->station_list );

		if ( !list_rewind( station->year_list ) ) continue;

		grace_datatype =
			grace_new_grace_datatype(
				station->station_name,
				station->stage_datatype_name );

/*
		grace_graph_set_scale_to_zero( grace_graph, 1 );
*/

		grace_graph->xaxis_ticklabel_angle = GRACE_TICKLABEL_ANGLE;

		sprintf(	legend,
				"%s/%s",
				station->station_name,
				station->stage_datatype_name );

		format_initial_capital( legend, legend );

		grace_datatype->legend = strdup( legend );

		grace_datatype->datatype_type_bar_xy_xyhilo = "xy";

		list_append_pointer(	grace_graph->datatype_list,
					grace_datatype );

	} while( list_next( annual_hydroperiod->station_list ) );

	list_append_pointer( grace->graph_list, grace_graph );

	grace->grace_output =
		application_grace_output(
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

	list_rewind( annual_hydroperiod->station_list );

	do {
		station = list_get_pointer( annual_hydroperiod->station_list );

		if ( !list_rewind( station->year_list ) ) continue;

		do {
			year = list_get_pointer( station->year_list );

			sprintf(point_list_string,
				"%s|%s|%d-01-01|%d",
				station->station_name,
				station->stage_datatype_name,
		 		year->year,
		 		year->above_threshold_count );

			if ( !grace_set_string_to_point_list(
				grace->graph_list, 
				GRACE_DATATYPE_ENTITY_PIECE,
				GRACE_DATATYPE_PIECE,
				GRACE_DATE_PIECE,
				GRACE_TIME_PIECE,
				GRACE_VALUE_PIECE,
				point_list_string,
				unit_graph,
				grace->datatype_type_xyhilo,
				grace->dataset_no_cycle_color,
				(char *)0 /* optional_label */ ) )
			{
				fprintf( stderr,
			"Warning in %s/%s()/%d: grace will now accept (%s).\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__,
					 point_list_string );
			}

		} while( list_next( station->year_list ) );

	} while( list_next( annual_hydroperiod->station_list ) );

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
		printf( "<h3>Warning: no graphs to display.</h3>\n" );
		document_close();
		exit( 0 );
	}

	grace_move_legend_bottom_left(
			list_get_first_pointer( grace->graph_list ),
			grace->landscape_mode );

	grace_increase_legend_char_size(
			list_get_first_pointer(
				grace->graph_list ),
			0.15 );

	/* Make the graph wider -- 97% of the page */
	/* --------------------------------------- */
	grace_set_view_maximum_x(
			(GRACE_GRAPH *)
				list_get_first_pointer(
					grace->graph_list ),
			0.97 );

	/* Move the legend down a little */
	/* ----------------------------- */
	grace_lower_legend(	grace->graph_list,
				0.04 );

	grace->dataset_no_cycle_color = 1;

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
				application_grace_home_directory(
					application_name ),
				application_grace_execution_directory(
					application_name ),
				application_grace_free_option_yn(
					application_name ),
				grace->grace_output,
				application_distill_directory(
					application_name ),
				distill_landscape_flag,
				application_ghost_script_directory(
					application_name ),
				(LIST *)0 /* quantum_datatype_name_list */,
				grace->symbols,
				grace->world_min_x,
				grace->world_max_x,
				grace->xaxis_ticklabel_precision,
				grace->graph_list,
				grace->anchor_graph_list ) )
	{
		printf( "<h3>No data for selected parameters.</h3>\n" );
		document_close();
		exit( 0 );
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
	return 1;

} /* annual_hydroperiod_output_chart_discontinuous() */

void annual_hydroperiod_output_transmit_detail(
				FILE *output_pipe,
				char *title,
				ANNUAL_HYDROPERIOD *annual_hydroperiod,
				boolean omit_elevation,
				char *application_name )
{
	ANNUAL_HYDROPERIOD_STATION *station;
	ANNUAL_HYDROPERIOD_YEAR *year;
	ANNUAL_HYDROPERIOD_WETTING_EVENT *wetting_event;
	ANNUAL_HYDROPERIOD_DATA *data;

	if ( list_rewind( annual_hydroperiod->station_list ) )
	{
		do {
			station =
				list_get_pointer(
					annual_hydroperiod->station_list );

			if ( !list_rewind( station->year_list ) ) continue;

			hydrology_library_output_station_text_file(
					output_pipe,
					application_name,
					station->station_name );

		} while( list_next( annual_hydroperiod->station_list ) );
	}

	if ( !omit_elevation )
	{
		fprintf(output_pipe,
"#%s\n#Station|Datatype|Elevation|Threshold|Year|Begin|End|Count|Date|Value\n",
			title );
	}
	else
	{
		fprintf(output_pipe,
"#%s\n#Station|Datatype|Threshold|Year|Begin|End|Count|Date|Value\n",
			title );
	}

	if ( !list_rewind( annual_hydroperiod->station_list ) )
	{
		return;
	}

	do {
		station = list_get_pointer( annual_hydroperiod->station_list );

		if ( !list_rewind( station->year_list ) )
		{
			continue;
		}

		do {
			year = list_get_pointer( station->year_list );

			if ( !list_rewind( year->wetting_event_list ) )
			{
				continue;
			}

			do {
				wetting_event =
					list_get_pointer(
						year->wetting_event_list );

				if ( !list_rewind( wetting_event->data_list ) )
				{
					fprintf(stderr,
				"ERROR in %s/%s()/%d: empty data_list.\n",
						__FILE__,
						__FUNCTION__,
						__LINE__ );
					exit( 1 );
				}

				do {
					data =
					list_get_pointer(
						wetting_event->data_list );

					fprintf(output_pipe,
						"%s",
						station->station_name );

					fprintf(output_pipe,
						"|%s",
						station->stage_datatype_name );

					if ( !omit_elevation )
					{
						fprintf(output_pipe,
			 				"|%.2lf",
			 				station->elevation );
					}

					fprintf(output_pipe,
			 			"|%.2lf",
			 			station->threshold );

					fprintf(output_pipe,
			 			"|%d",
			 			year->year );

					fprintf(output_pipe,
			 			"|%s",
						wetting_event->begin_date );

					fprintf(output_pipe,
			 			"|%s",
						(wetting_event->end_date)
							? wetting_event->
								end_date
							: "" );

					fprintf(output_pipe,
			 			"|%d",
						wetting_event->days_count );

					fprintf(output_pipe,
			 			"|%s",
						data->measurement_date );

					fprintf(output_pipe,
			 			"|%.2lf\n",
			 			data->measurement_value );

				} while( list_next( wetting_event->data_list ));
			} while( list_next( year->wetting_event_list ) );
		} while( list_next( station->year_list ) );
	} while( list_next( annual_hydroperiod->station_list ) );
} /* annual_hydroperiod_output_transmit_detail() */

void annual_hydroperiod_output_table_summary(
				char *report_title,
				int begin_year,
				int end_year,
				ANNUAL_HYDROPERIOD *annual_hydroperiod,
				boolean omit_elevation,
				char *application_name )
{
	char title[ 512 ];
	LIST *heading_list;
	HTML_TABLE *html_table;
	ANNUAL_HYDROPERIOD_STATION *station;
	ANNUAL_HYDROPERIOD_YEAR *year;
	char elevation_string[ 16 ];
	char threshold_string[ 16 ];
	char year_string[ 16 ];
	char above_threshold_count_string[ 16 ];
	char wetting_event_count_string[ 16 ];
	char minimum_string[ 16 ];
	char average_string[ 16 ];
	char maximum_string[ 16 ];
	int count = 0;

	if ( list_rewind( annual_hydroperiod->station_list ) )
	{
		do {
			station =
				list_get_pointer(
					annual_hydroperiod->station_list );

			if ( !list_rewind( station->year_list ) ) continue;

			hydrology_library_output_station_table(
					application_name,
					station->station_name );

		} while( list_next( annual_hydroperiod->station_list ) );
	}

	annual_hydroperiod_get_report_title(
				title,
				report_title,
				begin_year,
				end_year );

	html_table = new_html_table(
			title,
			(char *)0 /* sub_title */ );

	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	heading_list = list_new();

	list_append_pointer( heading_list, "Station" );
	list_append_pointer( heading_list, "Datatype" );

	if ( !omit_elevation )
		list_append_pointer( heading_list, "Elevation" );

	list_append_pointer( heading_list, "Threshold" );
	list_append_pointer( heading_list, "Year" );
	list_append_pointer( heading_list, "Wetting Event Count" );
	list_append_pointer( heading_list, "Discontinuous" );
	list_append_pointer( heading_list, "Minimum" );
	list_append_pointer( heading_list, "Average" );
	list_append_pointer( heading_list, "Maximum" );

	html_table->number_left_justified_columns = 2;
	html_table->number_right_justified_columns = 99;

	html_table_set_heading_list( html_table, heading_list );

	html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );

	if ( !list_rewind( annual_hydroperiod->station_list ) )
	{
		html_table_close();
		return;
	}

	do {
		station = list_get_pointer( annual_hydroperiod->station_list );

		if ( !list_rewind( station->year_list ) ) continue;

		do {
			year = list_get_pointer( station->year_list );

			html_table_set_data(
				html_table->data_list,
				strdup( station->station_name ) );

			html_table_set_data(
				html_table->data_list,
				strdup( station->stage_datatype_name ) );

			if ( !omit_elevation )
			{
				sprintf( elevation_string,
			 		"%.2lf",
			 		station->elevation );

				html_table_set_data(
					html_table->data_list,
					strdup( elevation_string ) );
			}

			sprintf( threshold_string,
			 	"%.2lf",
			 	station->threshold );

			html_table_set_data(
				html_table->data_list,
				strdup( threshold_string ) );

			sprintf(year_string,
			 	"%d",
			 	year->year );

			html_table_set_data(
				html_table->data_list,
				strdup( year_string ) );

			sprintf(wetting_event_count_string,
			 	"%d",
			 	list_length( year->wetting_event_list ) );

			html_table_set_data(
				html_table->data_list,
				strdup( wetting_event_count_string ) );

			sprintf(above_threshold_count_string,
			 	"%d",
			 	year->above_threshold_count );

			html_table_set_data(
				html_table->data_list,
				strdup( above_threshold_count_string ) );

			if ( !list_length( year->wetting_event_list ) )
			{
				strcpy( minimum_string, "Null" );
				strcpy( average_string, "Null" );
				strcpy( maximum_string, "Null" );
			}
			else
			{
				sprintf(minimum_string,
			 		"%d",
			 	annual_hydroperiod_get_minimum_days_count(
				       	year->wetting_event_list ) );

				sprintf(average_string,
			 		"%.1lf",
			 		(double)year->above_threshold_count /
			 		(double)
					list_length(
						year->
						wetting_event_list ) );
				sprintf(maximum_string,
			 		"%d",
			 	annual_hydroperiod_get_maximum_days_count(
				       	year->wetting_event_list ) );

			}

			html_table_set_data(
				html_table->data_list,
				strdup( minimum_string ) );

			html_table_set_data(
				html_table->data_list,
				strdup( average_string ) );

			html_table_set_data(
				html_table->data_list,
				strdup( maximum_string ) );

			if ( (++count % 2 ) )
				html_table_set_background_shaded(
						html_table );
			else
				html_table_set_background_unshaded(
						html_table );

			if ( !(	count %
				ANNUAL_HYDROPERIOD_ROWS_BETWEEN_HEADING ) )
			{
				html_table_output_data_heading(
					html_table->heading_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->justify_list );
			}

			html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

			list_free_string_list( html_table->data_list );
			html_table->data_list = list_new();
		} while( list_next( station->year_list ) );
	} while( list_next( annual_hydroperiod->station_list ) );

	html_table_close();

} /* annual_hydroperiod_output_table_summary() */

void annual_hydroperiod_output_table_detail(
				char *report_title,
				int begin_year,
				int end_year,
				ANNUAL_HYDROPERIOD *annual_hydroperiod,
				boolean omit_elevation,
				char *application_name )
{
	char title[ 512 ];
	LIST *heading_list;
	HTML_TABLE *html_table;
	ANNUAL_HYDROPERIOD_STATION *station;
	ANNUAL_HYDROPERIOD_YEAR *year;
	ANNUAL_HYDROPERIOD_WETTING_EVENT *wetting_event;
	ANNUAL_HYDROPERIOD_DATA *data;
	char elevation_string[ 16 ];
	char threshold_string[ 16 ];
	char days_count_string[ 16 ];
	char year_string[ 16 ];
	char data_string[ 16 ];
	int first_time;
	int did_output = 0;

	if ( list_rewind( annual_hydroperiod->station_list ) )
	{
		do {
			station =
				list_get_pointer(
					annual_hydroperiod->station_list );

			if ( !list_rewind( station->year_list ) ) continue;

			hydrology_library_output_station_table(
					application_name,
					station->station_name );

		} while( list_next( annual_hydroperiod->station_list ) );
	}

	annual_hydroperiod_get_report_title(
				title,
				report_title,
				begin_year,
				end_year );

	html_table = new_html_table(
			title,
			(char *)0 /* sub_title */ );
	heading_list = list_new();

	list_append_pointer( heading_list, "Station" );
	list_append_pointer( heading_list, "Datatype" );

	if ( !omit_elevation )
		list_append_pointer( heading_list, "Elevation" );

	list_append_pointer( heading_list, "Threshold" );
	list_append_pointer( heading_list, "Year" );
	list_append_pointer( heading_list, "Begin" );
	list_append_pointer( heading_list, "End" );
	list_append_pointer( heading_list, "Count" );
	list_append_pointer( heading_list, "Date" );
	list_append_pointer( heading_list, "Value" );

	html_table->number_left_justified_columns = 2;
	html_table->number_right_justified_columns = 99;

	html_table_set_heading_list( html_table, heading_list );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	if ( !list_rewind( annual_hydroperiod->station_list ) )
	{
		html_table_close();
		return;
	}

	do {
		station = list_get_pointer( annual_hydroperiod->station_list );

		if ( !list_rewind( station->year_list ) )
		{
			continue;
		}

		do {
			year = list_get_pointer( station->year_list );

			if ( !list_rewind( year->wetting_event_list ) )
			{
				continue;
			}

			do {
				wetting_event =
					list_get_pointer(
						year->wetting_event_list );

				if ( !list_rewind( wetting_event->data_list ) )
				{
					fprintf(stderr,
				"ERROR in %s/%s()/%d: empty data_list.\n",
						__FILE__,
						__FUNCTION__,
						__LINE__ );
					exit( 1 );
				}

				html_table_output_data_heading(
					html_table->heading_list,
					html_table->
						number_left_justified_columns,
					html_table->
						number_right_justified_columns,
					html_table->justify_list );

				first_time = 1;
				do {
					data =
					list_get_pointer(
						wetting_event->data_list );

					if ( first_time )
					{
						html_table_set_data(
						html_table->data_list,
						strdup( station->
							station_name ) );

						html_table_set_data(
						html_table->data_list,
						strdup( station->
							stage_datatype_name ) );

						if ( !omit_elevation )
						{
						      sprintf( elevation_string,
			 				       "%.2lf",
			 				       station->
							       elevation );

						      html_table_set_data(
							       html_table->
							       data_list,
							       strdup(
							       elevation_string
							       ) );
						}

						sprintf( threshold_string,
			 				"%.2lf",
			 				station->threshold );

						html_table_set_data(
							html_table->data_list,
							strdup(
							threshold_string ) );

						sprintf(year_string,
			 				"%d",
			 				year->year );

						html_table_set_data(
							html_table->data_list,
							strdup( year_string ) );

						html_table_set_data(
							html_table->data_list,
							strdup(
							wetting_event->
							begin_date ) );

						if ( wetting_event->end_date )
						{
							html_table_set_data(
							html_table->data_list,
							strdup(
							wetting_event->
							end_date ) );
						}
						else
						{
							html_table_set_data(
							html_table->data_list,
								strdup( "" ) );
						}

						sprintf(days_count_string,
			 				"%d",
							wetting_event->
							days_count );

						html_table_set_data(
							html_table->data_list,
							strdup(
							days_count_string ) );
						first_time = 0;
					}
					else
					{
						html_table_set_data(
							html_table->data_list,
							strdup( "" ) );
						html_table_set_data(
							html_table->data_list,
							strdup( "" ) );

						if ( !omit_elevation )
						{
							html_table_set_data(
								html_table->
								data_list,
								strdup( "" ) );
						}

						html_table_set_data(
							html_table->data_list,
							strdup( "" ) );
						html_table_set_data(
							html_table->data_list,
							strdup( "" ) );
						html_table_set_data(
							html_table->data_list,
							strdup( "" ) );
						html_table_set_data(
							html_table->data_list,
							strdup( "" ) );
						html_table_set_data(
							html_table->data_list,
							strdup( "" ) );
					}

					html_table_set_data(
						html_table->data_list,
						strdup( data->
							measurement_date ) );

					sprintf(data_string,
			 			"%.2lf",
			 			data->measurement_value );

					html_table_set_data(
						html_table->data_list,
						strdup( data_string ) );

					html_table_output_data(
						html_table->data_list,
						html_table->
						number_left_justified_columns,
						html_table->
						number_right_justified_columns,
						html_table->background_shaded,
						html_table->justify_list );

					list_free_string_list(
						html_table->data_list );
					html_table->data_list = list_new();
				} while( list_next( wetting_event->data_list ));
				did_output = 1;
			} while( list_next( year->wetting_event_list ) );
		} while( list_next( station->year_list ) );
	} while( list_next( annual_hydroperiod->station_list ) );

	if ( !did_output )
		printf( "<p>No wetting events for year.</p>\n" );

	html_table_close();

} /* annual_hydroperiod_output_table_detail() */

void annual_hydroperiod_get_report_title(
			char *title,
			char *report_title,
			int begin_year,
			int end_year )
{
	sprintf(	title,
			"%s from %d to %d",
			report_title,
			begin_year,
			end_year );
	format_initial_capital( title, title );
} /* annual_hydroperiod_get_report_title() */

int annual_hydroperiod_get_full_period_of_record_year(
				char *application_name,
				LIST *station_list,
				LIST *stage_datatype_list,
				char *aggregation_select,
				char *min_max_date_string,
				char *folder_name,
				char *date_attribute_name )
{
	char sys_string[ 1024 ];
	char where_clause[ 512 ];
	char buffer1[ 512 ];
	char buffer2[ 512 ];

	sprintf(where_clause,
		"station in (%s) and			"
		"datatype in (%s) and			"
		"%s like '%s'				",
		list_display_quote_comma_delimited(
			buffer1, station_list ),
		list_display_quote_comma_delimited(
			buffer2, stage_datatype_list ),
		date_attribute_name,
		min_max_date_string );

	sprintf(sys_string,
		"get_folder_data	application=%s		"
		"			select=\"%s\"		"
		"			folder=%s		"
		"			where=\"%s\"		"
		"			order=none		",
		application_name,
		aggregation_select,
		folder_name,
		where_clause );
	return atoi( pipe2string( sys_string ) );
} /* annual_hydroperiod_get_full_period_of_record_year() */

