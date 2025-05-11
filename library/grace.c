/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/grace.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <float.h>
#include "String.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "application.h"
#include "document.h"
#include "julian.h"
#include "date.h"
#include "float.h"
#include "column.h"
#include "chart.h"
#include "grace.h"

GRACE_POINT *grace_point_new(
		double horizontal_double,
		double vertical_double )
{
	GRACE_POINT *grace_point;

	grace_point = grace_point_calloc();
	grace_point->horizontal_double = horizontal_double;
	grace_point->vertical_double = vertical_double;

	return grace_point;
}

GRACE_POINT *grace_point_calloc( void )
{
	GRACE_POINT *grace_point;

	if ( ! ( grace_point = calloc( 1, sizeof ( GRACE_POINT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_point;
}

double grace_point_minimum_horizontal(
		LIST *grace_point_list )
{
	GRACE_POINT *grace_point;

	if ( ! ( grace_point = list_first( grace_point_list ) ) )
		return 0.0;

	return grace_point->horizontal_double;
}

double grace_point_minimum_vertical( LIST *grace_point_list )
{
	GRACE_POINT *grace_point;
	double minimum_vertical = FLOAT_MAXIMUM_DOUBLE;

	if ( !list_rewind( grace_point_list ) )
		return minimum_vertical;

	do {
		grace_point = list_get( grace_point_list );

		if ( grace_point->vertical_double < minimum_vertical )
		{
			minimum_vertical = grace_point->vertical_double;
		}

	} while ( list_next( grace_point_list ) );

	return minimum_vertical;
}

double grace_point_maximum_horizontal( LIST *grace_point_list )
{
	GRACE_POINT *grace_point;

	if ( ! ( grace_point = list_last( grace_point_list ) ) )
		return 0.0;

	return grace_point->horizontal_double;
}

double grace_point_maximum_vertical( LIST *grace_point_list )
{
	GRACE_POINT *grace_point;
	double maximum_vertical = FLOAT_MINIMUM_DOUBLE;

	if ( !list_rewind( grace_point_list ) )
		return maximum_vertical;

	do {
		grace_point = list_get( grace_point_list );

		if (	grace_point->vertical_double >
			maximum_vertical )
		{
			maximum_vertical =
				grace_point->vertical_double;
		}

	} while ( list_next( grace_point_list ) );

	return maximum_vertical;
}

GRACE_DATASET *grace_dataset_new(
		char *datatype_name,
		LIST *grace_point_list,
		enum grace_dataset_enum grace_dataset_enum,
		int grace_dataset_color_number,
		double symbol_size )
{
	GRACE_DATASET *grace_dataset;

	grace_dataset = grace_dataset_calloc();

	grace_dataset->datatype_name = datatype_name;
	grace_dataset->grace_point_list = grace_point_list;
	grace_dataset->grace_dataset_enum = grace_dataset_enum;
	grace_dataset->color_number = grace_dataset_color_number;
	grace_dataset->symbol_size = symbol_size;

	grace_dataset->legend =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		grace_dataset_legend(
			datatype_name );

	grace_dataset->dataset_number = grace_dataset_number();

	grace_dataset->line_type =
		grace_dataset_line_type(
			grace_dataset->grace_dataset_enum );

	grace_dataset->graph_type =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		grace_dataset_graph_type(
			grace_dataset->grace_dataset_enum );

	return grace_dataset;
}

GRACE_DATASET *grace_dataset_calloc( void )
{
	GRACE_DATASET *grace_dataset;

	if ( ! ( grace_dataset = calloc( 1, sizeof ( GRACE_DATASET ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_dataset;
}

char *grace_dataset_legend( char *datatype_name )
{
	char legend[ 128 ];;

	if ( !datatype_name ) return "";

	string_initial_capital(
		legend,
		datatype_name );

	return strdup( legend );
}

int grace_dataset_number( void )
{
	static int dataset_number;

	return dataset_number++;
}

GRACE_DATASET *grace_dataset_graph_list_seek(
		char *datatype_name,
		LIST *grace_graph_list )
{
	GRACE_GRAPH *grace_graph;
	GRACE_DATASET *grace_dataset;

	if ( !datatype_name )
	{
		char message[ 128 ];

		sprintf(message, "datatype_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( grace_graph_list ) ) return (GRACE_DATASET *)0;

	do {
		grace_graph = list_get( grace_graph_list );

		if ( ( grace_dataset =
			grace_dataset_seek(
				datatype_name,
				grace_graph->grace_dataset_list ) ) )
		{
			return grace_dataset;
		}

	} while ( list_next( grace_graph_list ) );

	return (GRACE_DATASET *)0;
}

GRACE_DATASET *grace_dataset_seek(
		char *datatype_name,
		LIST *grace_dataset_list )
{
	GRACE_DATASET *grace_dataset;

	if ( !datatype_name )
	{
		char message[ 128 ];

		sprintf(message, "datatype_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( grace_dataset_list ) ) return (GRACE_DATASET *)0;

	do {
		grace_dataset = list_get( grace_dataset_list );

		if ( strcmp(
			grace_dataset->datatype_name,
			datatype_name ) == 0 )
		{
			return grace_dataset;
		}

	} while ( list_next( grace_dataset_list ) );

	return (GRACE_DATASET *)0;
}

char *grace_graph_xaxis_tick_on_off(
		boolean xaxis_tick_on_boolean )
{
	if ( xaxis_tick_on_boolean )
		return "on";
	else
		return "off";
}

GRACE_GRAPH *grace_graph_new(
		int graph_number,
		boolean xaxis_tick_on_boolean,
		int horizontal_range,
		char *yaxis_heading,
		GRACE_VIEW *grace_view )
{
	GRACE_GRAPH *grace_graph;

	grace_graph = grace_graph_calloc();

	grace_graph->graph_number = graph_number;
	grace_graph->yaxis_heading = yaxis_heading;
	grace_graph->grace_view = grace_view;
	grace_graph->grace_dataset_list = list_new();

	grace_graph->xaxis_tick_on_off =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		grace_graph_xaxis_tick_on_off(
			xaxis_tick_on_boolean );

	grace_graph->xaxis_ticklabel_format =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		grace_graph_xaxis_ticklabel_format(
			horizontal_range );

	grace_graph->xaxis_ticklabel_angle =
		grace_graph_xaxis_ticklabel_angle(
			horizontal_range );

	return grace_graph;
}

GRACE_GRAPH *grace_graph_calloc( void )
{
	GRACE_GRAPH *grace_graph;

	if ( ! ( grace_graph = calloc( 1, sizeof ( GRACE_GRAPH ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_graph;
}

GRACE_UNITLESS *grace_unitless_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		char *sub_title,
		char *date_attribute_name,
		char *time_attribute_name,
		LIST *query_fetch_row_list,
		int horizontal_range_integer,
		char *title,
		char *grace_datatype_name,
		char *filename_key )
{
	GRACE_UNITLESS *grace_unitless;
	LIST *point_list;

	if ( !application_name
	||   !session_key
	||   !data_directory
	||   !title
	||   !date_attribute_name
	||   !list_length( query_fetch_row_list )
	||   !horizontal_range_integer
	||   !grace_datatype_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	grace_unitless = grace_unitless_calloc();

	grace_unitless->grace_setup =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_setup_new(
			application_name,
			session_key,
			data_directory,
			title,
			sub_title,
			filename_key,
			1 /* grace-graph_list_length */ );

	grace_unitless->grace_view =
		grace_view_new(
			GRACE_VIEW_MIN_X,
			GRACE_VIEW_PORTRAIT_X_OFFSET,
			GRACE_VIEW_LANDSCAPE_X_OFFSET,
			1 /* grace_graph_list_length */,
			horizontal_range_integer,
			grace_unitless->
				grace_setup->
				grace_page->
				landscape_boolean );

	if ( !grace_unitless->grace_view )
	{
		char message[ 128 ];

		sprintf(message, "grace_view_new() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	point_list =
		grace_dataset_point_list(
			date_attribute_name,
			time_attribute_name,
			query_fetch_row_list,
			grace_datatype_name );

	grace_unitless->grace_dataset =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_dataset_new(
			(char *)0 /* datatype_name */,
			point_list,
			grace_dataset_bar,
			grace_dataset_color_number(
				GRACE_DATASET_MAX_COLOR_NUMBER,
				0 /* not cycle_colors_boolean */ ),
			GRACE_DATASET_LINE_SYMBOL_SIZE );

	grace_unitless->grace_graph =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_unitless_grace_graph(
			horizontal_range_integer,
			grace_unitless->grace_view,
			grace_unitless->grace_dataset );

	if ( ! ( grace_unitless->grace_graph->grace_world =
			grace_world_new(
				grace_unitless->
					grace_graph->
					grace_dataset_list ) ) )
	{
		char message[ 128 ];

		sprintf(message, "grace_world_new() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_unitless;
}

GRACE_GRAPH *grace_unitless_grace_graph(
		int horizontal_range,
		GRACE_VIEW *grace_view,
		GRACE_DATASET *grace_dataset )
{
	GRACE_GRAPH *grace_graph;

	grace_graph =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_graph_new(
			0 /* graph_number */,
			1 /* xaxis_tick_on_boolean */,
			horizontal_range,
			(char *)0 /* yaxis_heading */,
			grace_view );

	list_set(
		grace_graph->grace_dataset_list,
		grace_dataset );

	return grace_graph;
}

GRACE_UNITLESS *grace_unitless_calloc( void )
{
	GRACE_UNITLESS *grace_unitless;

	if ( ! ( grace_unitless =
			calloc( 1,
				sizeof ( GRACE_UNITLESS ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_unitless;
}

int grace_dataset_color_number(
			int grace_dataset_max_color_number,
			boolean cycle_colors_boolean )
{
	static int color_number = 0;

	if ( !cycle_colors_boolean ) return 1;

	if ( ++color_number > grace_dataset_max_color_number )
		color_number = 1;

	return color_number;
}

void grace_page_output(
		int width_pixels,
		int length_pixels,
		FILE *output_file )
{
	fprintf(output_file,
"# Grace project file\n"
"# --------------------\n"
"# Appaserver generated\n"
"# --------------------\n"
"#\n"
"@version 50103\n"
"@page size %d, %d\n"
"@page scroll 5%c\n"
"@page inout 5%c\n"
"@link page off\n"
"%s\n"
"%s\n"
"%s\n"
"%s\n"
"%s\n",
	width_pixels,
	length_pixels,
	'%', '%',
	/* ---------------------- */
	/* Returns program memory */
	/* ---------------------- */
	grace_page_map_font_string(),
	/* ---------------------- */
	/* Returns program memory */
	/* ---------------------- */
	grace_page_map_color_string(),
	/* --------------------- */
	/* Returns static memory */
	/* --------------------- */
	grace_page_default_string(),
	/* ---------------------- */
	/* Returns program memory */
	/* ---------------------- */
	grace_page_timestamp_string(),
	/* --------------------- */
	/* Returns static memory */
	/* --------------------- */
	grace_page_region_string() );
}

GRACE_TITLE *grace_title_new(
		char *title,
		char *sub_title )
{
	GRACE_TITLE *grace_title;

	if ( !title || !*title )
	{
		char message[ 128 ];

		sprintf(message, "title is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	grace_title = grace_title_calloc();

	grace_title->title = title;
	grace_title->sub_title = sub_title;

	grace_title->title_length =
		grace_title_length(
			title );

	grace_title->title_char_size = GRACE_TITLE_CHAR_SIZE;

	if ( grace_title->title_length > 50 )
		grace_title->title_char_size -= 0.2;

	grace_title->sub_title_char_size =
		GRACE_TITLE_SUB_TITLE_CHAR_SIZE;

	grace_title->sub_title_length =
		grace_title_sub_title_length(
			sub_title );

	if ( grace_title->sub_title_length > 130 )
		grace_title->sub_title_char_size -= 0.4;
	else
	if ( grace_title->sub_title_length > 90 )
		grace_title->sub_title_char_size -= 0.3;
	else
	if ( grace_title->sub_title_length > 60 )
		grace_title->sub_title_char_size -= 0.2;

	return grace_title;
}

GRACE_TITLE *grace_title_calloc( void )
{
	GRACE_TITLE *grace_title;

	if ( ! ( grace_title = calloc( 1, sizeof ( GRACE_TITLE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_title;
}

int grace_title_length(	char *title )
{
	return string_strlen( title );
}

int grace_title_sub_title_length(
		char *sub_title )
{
	return string_strlen( sub_title );
}

GRACE_PAGE *grace_page_new( int grace_graph_list_length )
{
	GRACE_PAGE *grace_page;

	if ( !grace_graph_list_length ) return (GRACE_PAGE *)0;

	grace_page = grace_page_calloc();

	if (	grace_graph_list_length <
		GRACE_PAGE_PORTRAIT_THRESHOLD )
	{
		grace_page->width_pixels =
			GRACE_PAGE_LANDSCAPE_WIDTH_PIXELS;

		grace_page->length_pixels =
			GRACE_PAGE_LANDSCAPE_LENGTH_PIXELS;

		grace_page->landscape_boolean = 1;
	}
	else
	{
		grace_page->width_pixels =
			GRACE_PAGE_PORTRAIT_WIDTH_PIXELS;

		grace_page->length_pixels =
			GRACE_PAGE_PORTRAIT_LENGTH_PIXELS;
	}

	return grace_page;
}

GRACE_PAGE *grace_page_calloc( void )
{
	GRACE_PAGE *grace_page;

	if ( ! ( grace_page = calloc( 1, sizeof ( GRACE_PAGE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_page;
}

void grace_graph_top_output(
		int graph_number,
		FILE *output_file )
{
	fprintf(output_file,
"@g%d on\n"
"@g%d hidden false\n"
"@g%d type XY\n"
"@g%d stacked false\n"
"@g%d bar hgap 0.000000\n"
"@g%d fixedpoint off\n"
"@g%d fixedpoint type 0\n"
"@g%d fixedpoint xy 0.000000, 0.000000\n"
"@g%d fixedpoint format general general\n"
"@g%d fixedpoint prec 6, 6\n"
"@with g%d\n",
		graph_number,
		graph_number,
		graph_number,
		graph_number,
		graph_number,
		graph_number,
		graph_number,
		graph_number,
		graph_number,
		graph_number,
		graph_number );
}

double grace_dataset_minimum_horizontal(
		LIST *grace_dataset_list )
{
	GRACE_DATASET *grace_dataset;
	double minimum_horizontal = FLOAT_MAXIMUM_DOUBLE;
	double point_minimum_horizontal;

	if ( !list_rewind( grace_dataset_list ) ) return minimum_horizontal;

	do {
		grace_dataset = list_get( grace_dataset_list );

		if (	grace_dataset->grace_dataset_enum ==
			grace_dataset_whisker )
		{
			if ( !list_length(
				grace_dataset->
					grace_whisker_point_list ) )
			{
				continue;
			}

			point_minimum_horizontal =
				grace_whisker_point_minimum_horizontal(
					grace_dataset->
						grace_whisker_point_list );
		}
		else
		{
			if ( !list_length(
				grace_dataset->
					grace_point_list ) )
			{
				continue;
			}

			point_minimum_horizontal =
				grace_point_minimum_horizontal(
					grace_dataset->
						grace_point_list );
		}

		if ( point_minimum_horizontal < minimum_horizontal )
			minimum_horizontal =
				point_minimum_horizontal;

	} while ( list_next( grace_dataset_list ) );

	return minimum_horizontal;
}

double grace_dataset_maximum_horizontal( LIST *grace_dataset_list )
{
	GRACE_DATASET *grace_dataset;
	double maximum_horizontal = FLOAT_MINIMUM_DOUBLE;
	double point_maximum_horizontal;

	if ( !list_rewind( grace_dataset_list ) )
		return maximum_horizontal;

	do {
		grace_dataset = list_get( grace_dataset_list );

		if (	grace_dataset->grace_dataset_enum ==
			grace_dataset_whisker )
		{
			if ( !list_length(
				grace_dataset->
					grace_whisker_point_list ) )
			{
				continue;
			}

			point_maximum_horizontal =
				grace_whisker_point_maximum_horizontal(
					grace_dataset->
						grace_whisker_point_list );
		}
		else
		{
			if ( !list_length(
				grace_dataset->
					grace_point_list ) )
			{
				continue;
			}

			point_maximum_horizontal =
				grace_point_maximum_horizontal(
					grace_dataset->
						grace_point_list );
		}

		if ( point_maximum_horizontal > maximum_horizontal )
			maximum_horizontal =
				point_maximum_horizontal;

	} while ( list_next( grace_dataset_list ) );

	return maximum_horizontal;
}

double grace_dataset_minimum_vertical( LIST *grace_dataset_list )
{
	GRACE_DATASET *grace_dataset;
	double minimum_vertical = FLOAT_MAXIMUM_DOUBLE;
	double point_minimum_vertical;

	if ( !list_rewind( grace_dataset_list ) )
		return minimum_vertical;

	do {
		grace_dataset = list_get( grace_dataset_list );

		if (	grace_dataset->grace_dataset_enum ==
			grace_dataset_whisker )
		{
			if ( !list_length(
				grace_dataset->
					grace_whisker_point_list ) )
			{
				continue;
			}

			point_minimum_vertical =
				grace_whisker_point_minimum_vertical(
					grace_dataset->
						grace_whisker_point_list );
		}
		else
		{
			if ( !list_length(
				grace_dataset->
					grace_point_list ) )
			{
				continue;
			}

			point_minimum_vertical =
				grace_point_minimum_vertical(
					grace_dataset->
						grace_point_list );
		}

		if ( point_minimum_vertical < minimum_vertical )
			minimum_vertical =
				point_minimum_vertical;

	} while ( list_next( grace_dataset_list ) );

	return minimum_vertical;
}

double grace_dataset_maximum_vertical( LIST *grace_dataset_list )
{
	GRACE_DATASET *grace_dataset;
	double maximum_vertical = FLOAT_MINIMUM_DOUBLE;
	double point_maximum_vertical;

	if ( !list_rewind( grace_dataset_list ) )
		return maximum_vertical;

	do {
		grace_dataset = list_get( grace_dataset_list );

		if (	grace_dataset->grace_dataset_enum ==
			grace_dataset_whisker )
		{
			if ( !list_length(
				grace_dataset->
					grace_whisker_point_list ) )
			{
				continue;
			}

			point_maximum_vertical =
				grace_whisker_point_maximum_vertical(
					grace_dataset->
						grace_whisker_point_list );
		}
		else
		{
			if ( !list_length(
				grace_dataset->
					grace_point_list ) )
			{
				continue;
			}

			point_maximum_vertical =
				grace_point_maximum_vertical(
					grace_dataset->
						grace_point_list );
		}

		if ( point_maximum_vertical > maximum_vertical )
			maximum_vertical =
				point_maximum_vertical;

	} while ( list_next( grace_dataset_list ) );

	return maximum_vertical;
}

double grace_graph_cell_horizontal_double(
		QUERY_CELL *date_query_cell,
		QUERY_CELL *time_query_cell )
{
	char date_string[ 16 ];
	char *date_pointer = {0};
	char time_string[ 16 ];
	char *time_pointer = {0};

	if ( !date_query_cell
	||   !date_query_cell->select_datum
	||   !*date_query_cell->select_datum )
	{
		return 0.0;
	}

	if ( time_query_cell )
	{
		time_pointer = time_query_cell->select_datum;
	}

	if ( date_query_cell->attribute_is_date )
	{
		date_pointer = date_query_cell->select_datum;
	}
	else
	if ( date_query_cell->attribute_is_date_time
	||   date_query_cell->attribute_is_current_date_time )
	{
		column( date_string, 0, date_query_cell->select_datum );
		date_pointer = date_string;

		column( time_string, 1, date_query_cell->select_datum );
		time_pointer = time_string;
	}

	return
	grace_graph_horizontal_double(
		date_pointer /* date_string */,
		time_pointer /* time_string */ );
}

double grace_graph_horizontal_double(
		char *date_string,
		char *time_string )
{
	JULIAN *julian;
	double julian_double;

	if ( !date_string ) return 0.0;

	if ( string_strcmp( time_string, "null" ) == 0 )
		time_string = (char *)0;

	if ( ! ( julian =
			julian_yyyy_mm_dd_hhmm_new(
				date_string,
				time_string ) ) )
	{
		return 0.0;
	}

	julian_double = julian->current;
	free( julian );

	return julian_double;
}

double grace_graph_vertical_double( char *select_datum )
{
	return string_atof( select_datum );
}

boolean grace_graph_null_boolean(
		char *select_datum )
{
	if ( !select_datum || !*select_datum )
		return 1;
	else
		return 0;
}

void grace_graph_list_query_fetch_row_list_set(
		LIST *grace_graph_list,
		char *date_attribute_name,
		char *time_attribute_name,
		LIST *number_attribute_name_list,
		LIST *query_fetch_row_list )
{
	QUERY_ROW *query_row;
	QUERY_CELL *date_query_cell;
	QUERY_CELL *time_query_cell = {0};
	double horizontal_double;
	char *number_attribute_name;
	GRACE_DATASET *grace_dataset;
	QUERY_CELL *number_query_cell;
	double vertical_double;

	if ( !list_length( grace_graph_list )
	||   !date_attribute_name
	||   !list_length( number_attribute_name_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( query_fetch_row_list ) ) return;

	do {
		query_row =
			list_get(
				query_fetch_row_list );

		if ( ! ( date_query_cell =
				query_cell_seek(
					date_attribute_name,
					query_row->cell_list ) ) )
		{
			char message[ 128 ];

			sprintf(message,
				"query_cell_seek(%s) returned empty.",
				date_attribute_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( time_attribute_name )
		{
			time_query_cell =
				query_cell_seek(
					time_attribute_name,
					query_row->cell_list );
		}

		horizontal_double =
			grace_graph_cell_horizontal_double(
				date_query_cell,
				time_query_cell );

		if ( !horizontal_double ) continue;

		list_rewind( number_attribute_name_list );

		do {
			number_attribute_name =
				list_get(
					number_attribute_name_list );

			if ( ! ( grace_dataset =
					grace_dataset_graph_list_seek(
						number_attribute_name
							/* datatype_name */,
						grace_graph_list ) ) )
			{
				char message[ 128 ];

				sprintf(message,
			"grace_dataset_graph_list_seek(%s) returned empty.",
					number_attribute_name );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( ! ( number_query_cell =
					query_cell_seek(
						number_attribute_name,
						query_row->cell_list ) ) )
			{
				char message[ 128 ];

				sprintf(message,
					"query_cell_seek(%s) returned empty.",
					number_attribute_name );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( !grace_graph_null_boolean(
				number_query_cell->select_datum ) )
			{
				vertical_double =
					grace_graph_vertical_double(
						number_query_cell->
							select_datum );

				if ( !grace_dataset->grace_point_list )
					grace_dataset->grace_point_list =
						list_new();

				list_set(
					grace_dataset->grace_point_list,
					grace_point_new(
						horizontal_double,
						vertical_double ) );
			}

		} while ( list_next( number_attribute_name_list ) );

	} while ( list_next( query_fetch_row_list ) );
}

GRACE_VIEW *grace_view_new(
		double grace_view_min_x,
		double grace_view_portrait_x_offset,
		double grace_view_landscape_x_offset,
		int grace_graph_list_length,
		int horizontal_range_integer,
		boolean landscape_boolean )
{
	GRACE_VIEW *grace_view;

	if ( !grace_graph_list_length ) return NULL;

	grace_view = grace_view_calloc();

	grace_view->min_x = grace_view_min_x;

	grace_view->max_x =
		grace_view_max_x(
			grace_view_min_x,
			grace_view_portrait_x_offset,
			grace_view_landscape_x_offset,
			landscape_boolean );

	grace_view->min_y =
		grace_view_min_y(
			grace_graph_list_length );

	grace_view->y_offset =
		grace_view_y_offset(
			grace_graph_list_length,
			landscape_boolean );

	grace_view->max_y =
		grace_view_max_y(
			grace_view->min_y,
			grace_view->y_offset );

	grace_view->y_increment =
		grace_view_y_increment(
			grace_graph_list_length,
			landscape_boolean );

	grace_view->x_tick_major =
		grace_view_x_tick_major(
			horizontal_range_integer );

	grace_view->x_tick_minor =
		grace_view_x_tick_minor(
			horizontal_range_integer );

	return grace_view;
}

GRACE_VIEW *grace_view_calloc( void )
{
	GRACE_VIEW *grace_view;

	if ( ! ( grace_view = calloc( 1, sizeof ( GRACE_VIEW ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_view;
}

double grace_view_max_x(
		double grace_view_min_x,
		double grace_view_portrait_x_offset,
		double grace_view_landscape_x_offset,
		boolean landscape_boolean )
{
	if ( landscape_boolean )
	{
		return
		grace_view_min_x +
		grace_view_landscape_x_offset;
	}
	else
	{
		return
		grace_view_min_x +
		grace_view_portrait_x_offset;
	}
}

double grace_view_min_y( int grace_graph_list_length )
{
	double min_y;

	if ( !grace_graph_list_length ) return 0.0;

	if ( grace_graph_list_length == 1 )
		min_y = 0.43;
	else
		min_y = 0.5;

	return min_y;
}

double grace_view_y_increment(
		int grace_graph_list_length,
		boolean landscape_boolean )
{
	double y_increment;

	if ( !grace_graph_list_length ) return 0.0;

	if ( landscape_boolean )
	{
		if ( grace_graph_list_length == 1 )
			y_increment = 0.0;
		else
		if ( grace_graph_list_length == 2 )
			y_increment = 0.44;
		else
		if ( grace_graph_list_length == 3 )
			y_increment = 0.3;
		else
		if ( grace_graph_list_length == 4 )
			y_increment = 0.23;
		else
		if ( grace_graph_list_length == 5 )
			y_increment = 0.175;
		else
			y_increment =
				0.75 / (double)grace_graph_list_length;
	}
	else
	{
		if ( grace_graph_list_length == 1 )
			y_increment = 0.0;
		else
		if ( grace_graph_list_length == 2 )
			y_increment = 0.53;
		else
		if ( grace_graph_list_length == 3 )
			y_increment = 0.41;
		else
		if ( grace_graph_list_length == 4 )
			y_increment = 0.3;
		else
		if ( grace_graph_list_length == 5 )
			y_increment = 0.24;
		else
		if ( grace_graph_list_length == 6 )
			y_increment = 0.2;
		else
		if ( grace_graph_list_length == 7 )
			y_increment = 0.17;
		else
		if ( grace_graph_list_length == 8 )
			y_increment = 0.15;
		else
		if ( grace_graph_list_length == 9 )
			y_increment = 0.13;
		else
		if ( grace_graph_list_length == 10 )
			y_increment = 0.12;
		else
		if ( grace_graph_list_length == 11 )
			y_increment = 0.07;
		else
		if ( grace_graph_list_length == 12 )
			y_increment = 0.05;
		else
			y_increment =
				0.6 / (double)grace_graph_list_length;
	}

	return y_increment;
}

double grace_view_y_offset(
		int grace_graph_list_length,
		boolean landscape_boolean )
{
	double y_offset;

	if ( !grace_graph_list_length ) return 0.0;

	if ( landscape_boolean )
	{
		if ( grace_graph_list_length == 1 )
			y_offset = 0.45;
		else
		if ( grace_graph_list_length == 2 )
			y_offset = 0.375;
		else
		if ( grace_graph_list_length == 3 )
			y_offset = 0.23;
		else
		if ( grace_graph_list_length == 4 )
			y_offset = 0.16;
		else
		if ( grace_graph_list_length == 5 )
			y_offset = 0.12;
		else
			y_offset =
				0.6 / (double)grace_graph_list_length;
	}
	else
	{
		if ( grace_graph_list_length == 1 )
			y_offset = 0.46;
		else
		if ( grace_graph_list_length == 2 )
			y_offset = 0.4;
		else
		if ( grace_graph_list_length == 3 )
			y_offset = 0.31;
		else
		if ( grace_graph_list_length == 4 )
			y_offset = 0.23;
		else
		if ( grace_graph_list_length == 5 )
			y_offset = 0.17;
		else
		if ( grace_graph_list_length == 6 )
			y_offset = 0.14;
		else
		if ( grace_graph_list_length == 7 )
			y_offset = 0.11;
		else
		if ( grace_graph_list_length == 8 )
			y_offset = 0.09;
		else
		if ( grace_graph_list_length == 9 )
			y_offset = 0.08;
		else
		if ( grace_graph_list_length == 10 )
			y_offset = 0.075;
		else
		if ( grace_graph_list_length == 11 )
			y_offset = 0.07;
		else
		if ( grace_graph_list_length == 12 )
			y_offset = 0.065;
		else
			y_offset =
				0.8 / (double)grace_graph_list_length;
	}

	return y_offset;
}

double grace_view_x_tick_major( int horizontal_range_integer )
{
	double x_tick_major;

	if ( horizontal_range_integer <= 1 )
		x_tick_major = 0.25;
	else
	if ( horizontal_range_integer == 2 )
		x_tick_major = 0.5;
	else
	if ( horizontal_range_integer <= 7 )
		x_tick_major = 1.0;
	else
	if ( horizontal_range_integer <= 14 )
		x_tick_major = 2.0;
	else
	if ( horizontal_range_integer <= 62 )
		x_tick_major = 7.0;
	else
	if ( horizontal_range_integer <= 100 )
		x_tick_major = 10.0;
	else
	if ( horizontal_range_integer <= 180 )
		x_tick_major = 15.0;
	else
		x_tick_major =
			(double)horizontal_range_integer / 25.0;

	return x_tick_major;
}

int grace_view_x_tick_minor( int horizontal_range_integer )
{
	int x_tick_minor;

	if ( horizontal_range_integer <= 1 )
		x_tick_minor = 5;
	else
	if ( horizontal_range_integer == 2 )
		x_tick_minor = 4;
	else
	if ( horizontal_range_integer <= 7 )
		x_tick_minor = 3;
	else
	if ( horizontal_range_integer <= 14 )
		x_tick_minor = 2;
	if ( horizontal_range_integer <= 62 )
		x_tick_minor = 1;
	else
		x_tick_minor = 0;
/*
	if ( horizontal_range_integer <= 100 )
		x_tick_minor = 1;
	else
	if ( horizontal_range_integer <= 180 )
		x_tick_minor = 3;
	else
	if ( horizontal_range_integer <= 366 )
		x_tick_minor = 3;
	else
	if ( horizontal_range_integer <= 366 * 2 )
		x_tick_minor = 4;
	else
	if ( horizontal_range_integer <= 366 * 3 )
		x_tick_minor = 11;
	else
	if ( horizontal_range_integer <= 366 * 10 )
		x_tick_minor = 3;
	else
	if ( horizontal_range_integer <= 366 * 15 )
		x_tick_minor = 1;
	else
		x_tick_minor = 3;
*/

	return x_tick_minor;
}

GRACE_VIEW *grace_view_increment( GRACE_VIEW *grace_view )
{
	GRACE_VIEW *grace_view_next;

	if ( !grace_view )
	{
		char message[ 128 ];

		sprintf(message, "grace_view is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	grace_view_next = grace_view_calloc();

	memcpy(	grace_view_next,
		grace_view,
		sizeof ( GRACE_VIEW ) );

	grace_view_next->min_y += grace_view_next->y_increment;

	grace_view_next->max_y =
		grace_view_max_y(
			grace_view_next->min_y,
			grace_view_next->y_offset );

	return grace_view_next;
}

GRACE_WORLD *grace_world_new( LIST *grace_dataset_list )
{
	GRACE_WORLD *grace_world;

	if ( !list_length( grace_dataset_list ) ) return NULL;

	grace_world = grace_world_calloc();

	grace_world->grace_dataset_minimum_horizontal =
		grace_dataset_minimum_horizontal(
			grace_dataset_list );

	grace_world->grace_dataset_maximum_horizontal =
		grace_dataset_maximum_horizontal(
			grace_dataset_list );

	grace_world->horizontal_range =
		grace_world_horizontal_range(
			grace_world->grace_dataset_minimum_horizontal,
			grace_world->grace_dataset_maximum_horizontal );

	grace_world->min_x =
		grace_world_min_x(
			grace_world->grace_dataset_minimum_horizontal );

	grace_world->max_x =
		grace_world_max_x(
			grace_world->grace_dataset_maximum_horizontal );

	grace_world->grace_dataset_minimum_vertical =
		grace_dataset_minimum_vertical(
			grace_dataset_list );

	grace_world->grace_dataset_maximum_vertical =
		grace_dataset_maximum_vertical(
			grace_dataset_list );

	grace_world->min_y =
		grace_world_min_y(
			grace_world->grace_dataset_minimum_vertical );

	grace_world->max_y =
		grace_world_max_y(
			grace_world->grace_dataset_maximum_vertical );

	grace_world->vertical_range =
		grace_world_vertical_range(
			grace_world->grace_dataset_minimum_vertical,
			grace_world->grace_dataset_maximum_vertical );

	grace_world->y_tick_major =
		grace_world_y_tick_major(
			grace_world->grace_dataset_maximum_vertical,
			grace_world->max_y );

	grace_world->y_tick_precision =
		grace_world_y_tick_precision(
			grace_world->y_tick_major );

	return grace_world;
}

GRACE_WORLD *grace_world_calloc( void )
{
	GRACE_WORLD *grace_world;

	if ( ! ( grace_world = calloc( 1, sizeof ( GRACE_WORLD ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_world;
}

double grace_world_horizontal_range(
		double grace_dataset_minimum_horizontal,
		double grace_dataset_maximum_horizontal )
{
	return
	grace_dataset_maximum_horizontal -
	grace_dataset_minimum_horizontal;
}

double grace_world_min_x(
		double grace_dataset_minimum_horizontal )
{
	return grace_dataset_minimum_horizontal - 5.0;
}

double grace_world_max_x(
		double grace_dataset_maximum_horizontal )
{
	return grace_dataset_maximum_horizontal + 5.0;
}

double grace_world_min_y(
		double grace_dataset_minimum_vertical )
{
	if ( grace_dataset_minimum_vertical > 0.0 )
		return 0.0;
	else
		return grace_dataset_minimum_vertical;
}

double grace_world_max_y(
		double grace_dataset_maximum_vertical )
{
	return grace_dataset_maximum_vertical;
}

double grace_world_vertical_range(
		double grace_world_min_y,
		double grace_dataset_maximum_vertical )
{
	return
	grace_dataset_maximum_vertical -
	grace_world_min_y;
}

char *grace_dataset_graph_type(
		enum grace_dataset_enum grace_dataset_enum )
{
	char *graph_type;

	if ( grace_dataset_enum == grace_dataset_bar )
		graph_type = "bar";
	else
		graph_type = "xy";

	return graph_type;
}

int grace_dataset_line_type(
		enum grace_dataset_enum grace_dataset_enum )
{
	int line_type;

	if ( grace_dataset_enum == grace_dataset_bar )
		line_type = 0;
	else
		line_type = 1;

	return line_type;
}

void grace_dataset_list_output(
			int graph_number,
			LIST *grace_dataset_list,
			FILE *output_file )
{
	GRACE_DATASET *grace_dataset;

	if ( !output_file )
	{
		char message[ 128 ];

		sprintf(message, "output_file is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( grace_dataset_list ) ) return;

	do {
		grace_dataset =
			list_get(
				grace_dataset_list );

		if ( grace_dataset->
			grace_dataset_enum ==
				grace_dataset_whisker
		&& !list_length(
			grace_dataset->
				grace_whisker_point_list ) )
		{
			continue;
		}
		else
		if ( !list_length(
			grace_dataset->
				grace_point_list ) )
		{
			continue;
		}

		grace_dataset_output(
			graph_number,
			grace_dataset->color_number,
			grace_dataset->legend,
			grace_dataset->dataset_number,
			grace_dataset->line_type,
			grace_dataset->graph_type,
			grace_dataset->symbol_size,
			output_file );

		if ( grace_dataset->
			grace_dataset_enum ==
				grace_dataset_whisker)
		{
			grace_whisker_point_list_output(
				grace_dataset->grace_whisker_point_list,
				output_file );
		}
		else
		{
			grace_point_list_output(
				grace_dataset->grace_point_list,
				output_file );
		}

	} while ( list_next( grace_dataset_list ) );
}

void grace_dataset_output(
		int graph_number,
		int color_number,
		char *legend,
		int dataset_number,
		int line_type,
		char *graph_type,
		double symbol_size,
		FILE *output_file )
{
	fprintf(output_file,
"@    s%d symbol 0\n"
"@    s%d symbol size %.2lf\n"
"@    s%d symbol color %d\n"
"@    s%d symbol pattern 1\n"
"@    s%d symbol fill color %d\n"
"@    s%d symbol fill pattern 1\n"
"@    s%d symbol linewidth 1.0\n"
"@    s%d symbol linestyle 1\n"
"@    s%d symbol char 65\n"
"@    s%d symbol char font 0\n"
"@    s%d symbol skip 0\n"
"@    s%d line type %d\n"
"@    s%d line linestyle 1\n"
"@    s%d line linewidth 1.0\n"
"@    s%d line color %d\n"
"@    s%d line pattern 1\n"
"@    s%d baseline type 0\n"
"@    s%d baseline off\n"
"@    s%d dropline off\n"
"@    s%d fill type 0\n"
"@    s%d fill rule 0\n"
"@    s%d fill color 1\n"
"@    s%d fill pattern 1\n"
"@    s%d avalue off\n"
"@    s%d avalue type 2\n"
"@    s%d avalue char size 1.000000\n"
"@    s%d avalue font 0\n"
"@    s%d avalue color 1\n"
"@    s%d avalue rot 0\n"
"@    s%d avalue format general\n"
"@    s%d avalue prec 3\n"
"@    s%d avalue prepend \"\"\n"
"@    s%d avalue append \"\"\n"
"@    s%d avalue offset 0.000000 , 0.000000\n"
"@    s%d errorbar on\n"
"@    s%d errorbar place normal\n"
"@    s%d errorbar color 1\n"
"@    s%d errorbar pattern 1\n"
"@    s%d errorbar size 1.000000\n"
"@    s%d errorbar linewidth 1.0\n"
"@    s%d errorbar linestyle 1\n"
"@    s%d errorbar riser linewidth 1.0\n"
"@    s%d errorbar riser linestyle 1\n"
"@    s%d errorbar riser clip off\n"
"@    s%d errorbar riser clip length 0.100000\n"
"@    s%d comment \"%s\"\n"
"@    s%d legend  \"%s\"\n"
"@target G%d.S%d\n"
"@type %s\n",
	dataset_number,
	dataset_number,
	symbol_size,
	dataset_number,
	color_number,
	dataset_number,
	dataset_number,
	color_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	line_type,
	dataset_number,
	dataset_number,
	dataset_number,
	color_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	legend,
	dataset_number,
	legend,
	graph_number,
	dataset_number,
	graph_type );
}

char *grace_filename_postscript_filename( APPASERVER_LINK *appaserver_link )
{
	char *link_filename;
	char *tail_half;
	char *postscript_filename;

	if ( !appaserver_link )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_link is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	link_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_filename(
			appaserver_link->filename_stem,
			appaserver_link->begin_date_string,
			appaserver_link->end_date_string,
			appaserver_link->process_id,
			appaserver_link->session_key,
			"ps" /* extension */ );

	tail_half =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_tail_half(
			appaserver_link->application_name,
			link_filename );

	postscript_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_output_filename(
			appaserver_link->appaserver_parameter_data_directory,
			tail_half );

	free( link_filename );
	free( tail_half );

	return postscript_filename;
}

char *grace_filename_pdf_prompt_filename( APPASERVER_LINK *appaserver_link )
{
	char *link_filename;
	char *tail_half;
	char *prompt_filename;

	if ( !appaserver_link
	||   !appaserver_link->appaserver_link_prompt )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_link is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	link_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_filename(
			appaserver_link->filename_stem,
			appaserver_link->begin_date_string,
			appaserver_link->end_date_string,
			appaserver_link->process_id,
			appaserver_link->session_key,
			"pdf" /* extension */ );

	tail_half =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_tail_half(
			appaserver_link->application_name,
			link_filename );

	prompt_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_prompt_filename(
			appaserver_link->appaserver_link_prompt->link_half,
			tail_half );

	free( link_filename );
	free( tail_half );

	return prompt_filename;
}

char *grace_filename_pdf_output_filename( APPASERVER_LINK *appaserver_link )
{
	char *link_filename;
	char *tail_half;
	char *output_filename;

	if ( !appaserver_link )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_link is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	link_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_filename(
			appaserver_link->filename_stem,
			appaserver_link->begin_date_string,
			appaserver_link->end_date_string,
			appaserver_link->process_id,
			appaserver_link->session_key,
			"pdf" /* extension */ );

	tail_half =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_tail_half(
			appaserver_link->application_name,
			link_filename );

	output_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_output_filename(
			appaserver_link->appaserver_parameter_data_directory,
			tail_half );

	free( link_filename );
	free( tail_half );

	return output_filename;
}

char *grace_filename_agr_prompt_filename( APPASERVER_LINK *appaserver_link )
{
	char *link_filename;
	char *tail_half;
	char *prompt_filename;

	if ( !appaserver_link
	||   !appaserver_link->appaserver_link_prompt )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_link is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	link_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_filename(
			appaserver_link->filename_stem,
			appaserver_link->begin_date_string,
			appaserver_link->end_date_string,
			appaserver_link->process_id,
			appaserver_link->session_key,
			"agr" /* extension */ );

	tail_half =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_tail_half(
			appaserver_link->application_name,
			link_filename );

	prompt_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_prompt_filename(
			appaserver_link->appaserver_link_prompt->link_half,
			tail_half );

	free( link_filename );
	free( tail_half );

	return prompt_filename;
}

char *grace_filename_agr_output_filename( APPASERVER_LINK *appaserver_link )
{
	char *link_filename;
	char *tail_half;
	char *output_filename;

	if ( !appaserver_link )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_link is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	link_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_filename(
			appaserver_link->filename_stem,
			appaserver_link->begin_date_string,
			appaserver_link->end_date_string,
			appaserver_link->process_id,
			appaserver_link->session_key,
			"agr" /* extension */ );

	tail_half =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_tail_half(
			appaserver_link->application_name,
			link_filename );

	output_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_output_filename(
			appaserver_link->appaserver_parameter_data_directory,
			tail_half );

	free( link_filename );
	free( tail_half );

	return output_filename;
}

GRACE_FILENAME *grace_filename_new(
		char *application_name,
		char *session_key,
		char *filename_key,
		char *appaserver_parameter_data_directory )
{
	GRACE_FILENAME *grace_filename;

	if ( !application_name
	||   !appaserver_parameter_data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	grace_filename = grace_filename_calloc();

	grace_filename->process_id =
		grace_filename_process_id(
			session_key );

	grace_filename->appaserver_link =
		appaserver_link_new(
			application_http_prefix(
				application_ssl_support_boolean(
					application_name ) ),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
			appaserver_parameter_data_directory,
			GRACE_FILENAME_STEM,
			application_name,
			grace_filename->process_id,
			session_key,
			filename_key /* begin_date_string */,
			(char *)0 /* end_date_string */,
			(char *)0 /* extension */ );

	grace_filename->agr_output_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		grace_filename_agr_output_filename(
			grace_filename->appaserver_link );

	grace_filename->agr_prompt_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		grace_filename_agr_prompt_filename(
			grace_filename->appaserver_link );

	grace_filename->pdf_output_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		grace_filename_pdf_output_filename(
			grace_filename->appaserver_link );

	grace_filename->pdf_prompt_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		grace_filename_pdf_prompt_filename(
			grace_filename->appaserver_link );

	grace_filename->postscript_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		grace_filename_postscript_filename(
			grace_filename->appaserver_link );

	grace_filename->grace_home_directory =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		application_grace_home_directory(
			application_name );

	grace_filename->grace_execution_directory =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		application_grace_execution_directory(
			application_name );

	return grace_filename;
}

pid_t grace_filename_process_id( char *session_key )
{
	if ( session_key && *session_key )
		return (pid_t)0;
	else
		return getpid();
}

GRACE_FILENAME *grace_filename_calloc( void )
{
	GRACE_FILENAME *grace_filename;

	if ( ! ( grace_filename = calloc( 1, sizeof ( GRACE_FILENAME ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_filename;
}

void grace_graph_output(
			GRACE_TITLE *grace_title,
			GRACE_GRAPH *grace_graph,
			FILE *output_file )
{
	if ( !grace_graph
	||   !output_file )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( grace_graph->grace_dataset_list ) ) return;

	grace_graph_top_output(
		grace_graph->graph_number,
		output_file );

	grace_world_output(
		grace_graph->grace_world,
		output_file );

	grace_view_output(
		grace_graph->grace_view,
		output_file );

	if ( grace_title )
	{
		grace_title_output(
			GRACE_TITLE_MAX_SUB_TITLE_LENGTH,
			grace_title,
			output_file );
	}

	if ( grace_graph->grace_horizontal_label )
	{
		grace_horizontal_label_output(
			grace_graph->grace_horizontal_label,
			output_file );
	}

	fprintf(output_file,
"@    xaxes scale Normal\n"
"@    yaxes scale Normal\n"
"@    xaxes invert off\n"
"@    yaxes invert off\n" );

	grace_graph_xaxis_output(
		grace_graph->xaxis_tick_on_off,
		grace_graph->grace_view->x_tick_major,
		grace_graph->grace_view->x_tick_minor,
		grace_graph->xaxis_ticklabel_format,
		grace_graph->xaxis_ticklabel_angle,
		output_file );

	grace_graph_yaxis_output(
		grace_graph->grace_world->y_tick_major,
		grace_graph->grace_world->y_tick_precision,
		grace_graph->yaxis_heading,
		output_file );

	grace_dataset_list_output(
		grace_graph->graph_number,
		grace_graph->grace_dataset_list,
		output_file );
}

void grace_world_output(
		GRACE_WORLD *grace_world,
		FILE *output_file )
{
	if ( !grace_world
	||   !output_file )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	fprintf(output_file,
"@    world xmin %lf\n"
"@    world xmax %lf\n"
"@    world ymin %lf\n"
"@    world ymax %lf\n"
"@    stack world 0, 0, 0, 0\n",
		grace_world->min_x,
		grace_world->max_x,
		grace_world->min_y,
		grace_world->max_y );
}

void grace_view_output(
		GRACE_VIEW *grace_view,
		FILE *output_file )
{
	if ( !grace_view )
	{
		char message[ 128 ];

		sprintf(message, "grace_view is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !output_file )
	{
		char message[ 128 ];

		sprintf(message, "output_file is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	fprintf(output_file,
"@    view xmin %.5lf\n"
"@    view xmax %.5lf\n"
"@    view ymin %.5lf\n"
"@    view ymax %.5lf\n",
		grace_view->min_x,
		grace_view->max_x,
		grace_view->min_y,
		grace_view->max_y );
}

void grace_title_output(
		int max_sub_title_length,
		GRACE_TITLE *grace_title,
		FILE *output_file )
{
	char *sub_title;

	if ( !grace_title
	||   !output_file )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !grace_title->sub_title
	||   (int)strlen( grace_title->sub_title ) > max_sub_title_length )
	{
		sub_title = "";
	}
	else
	{
		sub_title = grace_title->sub_title;
	}

	fprintf(output_file,
"@    title \"%s\"\n"
"@    title font 6\n"
"@    title size %.1lf\n"
"@    title color 1\n"
"@    subtitle \"%s\"\n"
"@    subtitle font 6\n"
"@    subtitle size %.1lf\n"
"@    subtitle color 1\n",
		grace_title->title,
		grace_title->title_char_size,
		sub_title,
		grace_title->sub_title_char_size );
}

void grace_graph_xaxis_output(
		char *xaxis_tick_on_off,
		double x_tick_major,
		int x_tick_minor,
		char *xaxis_ticklabel_format,
		int xaxis_ticklabel_angle,
		FILE *output_file )
{
	if ( !xaxis_tick_on_off
	||   !xaxis_ticklabel_format
	||   !output_file )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	fprintf(output_file,
"@    xaxis  on\n"
"@    xaxis  type zero false\n"
"@    xaxis  offset 0.000000 , 0.000000\n"
"@    xaxis  bar off\n"
"@    xaxis  bar color 1\n"
"@    xaxis  bar linestyle 1\n"
"@    xaxis  bar linewidth 0.8\n"
"@    xaxis  label \"\"\n"
"@    xaxis  label layout para\n"
"@    xaxis  label place auto\n"
"@    xaxis  label char size 0.650000\n"
"@    xaxis  label font 6\n"
"@    xaxis  label color 1\n"
"@    xaxis  label place normal\n"
"@    xaxis  tick %s\n"
"@    xaxis  tick major %.5lf\n"
"@    xaxis  tick minor ticks %d\n"
"@    xaxis  tick default 6\n"
"@    xaxis  tick place rounded false\n"
"@    xaxis  tick in\n"
"@    xaxis  tick major size 1\n"
"@    xaxis  tick major color 1\n"
"@    xaxis  tick major linewidth 1.0\n"
"@    xaxis  tick major linestyle 1\n"
"@    xaxis  tick major grid off\n"
"@    xaxis  tick minor color 1\n"
"@    xaxis  tick minor linewidth 1.0\n"
"@    xaxis  tick minor linestyle 1\n"
"@    xaxis  tick minor grid off\n"
"@    xaxis  tick minor size 0.500000\n"
"@    xaxis  ticklabel on\n"
"@    xaxis  ticklabel prec 5\n"
"@    xaxis  ticklabel format %s\n"
"@    xaxis  ticklabel append \"\"\n"
"@    xaxis  ticklabel prepend \"\"\n"
"@    xaxis  ticklabel angle %d\n"
"@    xaxis  ticklabel skip 0\n"
"@    xaxis  ticklabel stagger 1\n"
"@    xaxis  ticklabel place normal\n"
"@    xaxis  ticklabel offset auto\n"
"@    xaxis  ticklabel offset 0.000000 , 0.010000\n"
"@    xaxis  ticklabel sign normal\n"
"@    xaxis  ticklabel start type auto\n"
"@    xaxis  ticklabel start 0.000000\n"
"@    xaxis  ticklabel stop type auto\n"
"@    xaxis  ticklabel stop 0.000000\n"
"@    xaxis  ticklabel char size 0.5\n"
"@    xaxis  ticklabel font 4\n"
"@    xaxis  ticklabel color 1\n"
"@    xaxis  tick place normal\n"
"@    xaxis  ticklabel type auto\n"
"@    xaxis  tick type auto\n",
		xaxis_tick_on_off,
		x_tick_major,
		x_tick_minor,
		xaxis_ticklabel_format,
		xaxis_ticklabel_angle );
}

void grace_graph_yaxis_output(
		double y_tick_major,
		int y_tick_precision,
		char *yaxis_heading,
		FILE *output_file )
{
	fprintf(output_file,
"@    yaxis  on\n"
"@    yaxis  type zero false\n"
"@    yaxis  offset 0.000000 , 0.000000\n"
"@    yaxis  bar on\n"
"@    yaxis  bar color 1\n"
"@    yaxis  bar linestyle 1\n"
"@    yaxis  bar linewidth 0.8\n"
"@    yaxis  label \"%s\"\n"
"@    yaxis  label layout para\n"
"@    yaxis  label char size 0.700000\n"
"@    yaxis  label font 6\n"
"@    yaxis  label color 1\n"
"@    yaxis  label place normal\n"
"@    yaxis  tick on\n"
"@    yaxis  tick major %.5lf\n"
"@    yaxis  tick minor ticks 0\n"
"@    yaxis  tick default 6\n"
"@    yaxis  tick place rounded true\n"
"@    yaxis  tick in\n"
"@    yaxis  tick major size 1.000000\n"
"@    yaxis  tick major color 15\n"
"@    yaxis  tick major linewidth 1.0\n"
"@    yaxis  tick major linestyle 3\n"
"@    yaxis  tick major grid on\n"
"@    yaxis  tick minor color 15\n"
"@    yaxis  tick minor linewidth 1.0\n"
"@    yaxis  tick minor linestyle 2\n"
"@    yaxis  tick minor grid on\n"
"@    yaxis  tick minor size 0.500000\n"
"@    yaxis  ticklabel on\n"
"@    yaxis  ticklabel prec %d\n"
"@    yaxis  ticklabel format decimal\n"
"@    yaxis  ticklabel append \"\"\n"
"@    yaxis  ticklabel prepend \"\"\n"
"@    yaxis  ticklabel angle 0\n"
"@    yaxis  ticklabel skip 0\n"
"@    yaxis  ticklabel stagger 0\n"
"@    yaxis  ticklabel place normal\n"
"@    yaxis  ticklabel offset auto\n"
"@    yaxis  ticklabel offset 0.000000 , 0.010000\n"
"@    yaxis  ticklabel sign normal\n"
"@    yaxis  ticklabel start type auto\n"
"@    yaxis  ticklabel start 0.000000\n"
"@    yaxis  ticklabel stop type auto\n"
"@    yaxis  ticklabel stop 0.000000\n"
"@    yaxis  ticklabel char size 0.6\n"
"@    yaxis  ticklabel font 4\n"
"@    yaxis  ticklabel color 1\n"
"@    yaxis  tick place normal\n"
"@    yaxis  ticklabel type auto\n"
"@    yaxis  tick type auto\n"
"@    altxaxis  off\n"
"@    altyaxis  off\n"
"@    legend on\n"
"@    legend loctype view\n"
"@    legend x1 1.2\n"
"@    legend y1 0.89\n"
"@    legend box color 0\n"
"@    legend box pattern 0\n"
"@    legend box linewidth 0.0\n"
"@    legend box linestyle 0\n"
"@    legend box fill color 0\n"
"@    legend box fill pattern 0\n"
"@    legend font 4\n"
"@    legend char size 0.55\n"
"@    legend color 1\n"
"@    legend length 1\n"
"@    legend vgap 1\n"
"@    legend hgap 1\n"
"@    legend invert false\n"
"@    frame type 0\n"
"@    frame linestyle 1\n"
"@    frame linewidth 1.0\n"
"@    frame color 1\n"
"@    frame pattern 1\n"
"@    frame background color 0\n"
"@    frame background pattern 0\n",
		(yaxis_heading) ? yaxis_heading : "",
		y_tick_major,
		y_tick_precision );
}

char *grace_graph_xaxis_ticklabel_format( int horizontal_range )
{
	char *ticklabel_format;

/*
	if ( !horizontal_range )
	{
		ticklabel_format = "decimal";
	}
	else
*/
	if ( horizontal_range <= 2 )
	{
		ticklabel_format = "mmddhms";
	}
	else
	if ( horizontal_range <= 180 )
	{
		ticklabel_format = "daymonth";
	}
	else
	if ( horizontal_range <= 366 )
	{
		ticklabel_format = "mmdd";
	}
	else
	{
		ticklabel_format = "yymmdd";
	}

	return ticklabel_format;
}

int grace_graph_xaxis_ticklabel_angle( int horizontal_range )
{
	int ticklabel_angle;

	if ( horizontal_range <= 2 )
	{
		ticklabel_angle = 15;
	}
	else
	if ( horizontal_range <= 180 )
	{
		ticklabel_angle = 20;
	}
	else
	if ( horizontal_range <= 366 )
	{
		ticklabel_angle = 30;
	}
	else
	{
		ticklabel_angle = 50;
	}

	return ticklabel_angle;
}

void grace_point_list_output(
			LIST *grace_point_list,
			FILE *output_file )
{
	GRACE_POINT *grace_point;

	if ( !output_file )
	{
		char message[ 128 ];

		sprintf(message, "output_file is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( grace_point_list ) ) return;

	do {
		grace_point =
			list_get(
				grace_point_list );

		grace_point_output(
			grace_point,
			output_file );

	} while ( list_next( grace_point_list ) );

	fprintf( output_file, "&\n" );
}

void grace_point_output(
			GRACE_POINT *grace_point,
			FILE *output_file )
{
	if ( !grace_point )
	{
		char message[ 128 ];

		sprintf(message, "grace_point is empty." );

		if ( output_file ) fclose( output_file );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !output_file )
	{
		char message[ 128 ];

		sprintf(message, "output_file is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	fprintf(output_file,
		"%.6lf\t%.4lf\n",
		grace_point->horizontal_double,
		grace_point->vertical_double );
}

char *grace_pdf_system_string( GRACE_FILENAME *grace_filename )
{
	char system_string[ 1024 ];

	if ( !grace_filename
	||   !grace_filename->postscript_filename
	||   !grace_filename->pdf_output_filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"ps2pdf.sh %s %s",
		grace_filename->postscript_filename,
		grace_filename->pdf_output_filename );

	return strdup( system_string );
}

char *grace_ps_system_string(
		char *grace_executable_template,
		GRACE_FILENAME *grace_filename )
{
	char grace_executable_system_string[ 256 ];
	char system_string[ 1024 ];

	if ( !grace_executable_template
	||   !grace_filename
	||   !grace_filename->grace_home_directory
	||   !grace_filename->grace_execution_directory
	||   !grace_filename->agr_output_filename
	||   !grace_filename->postscript_filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(grace_executable_system_string,
		grace_executable_template,
		grace_filename->grace_home_directory,
		grace_filename->grace_execution_directory );

	sprintf(system_string,
		"tee %s |"
		"%s "
		"-noask "
		"-autoscale x "
		"-hdevice PostScript "
		"-pipe "
		"-hardcopy "
		"-printfile %s)",
		grace_filename->agr_output_filename,
		grace_executable_system_string, 
		grace_filename->postscript_filename );

	return strdup( system_string );
}

void grace_single_graph_pdf_create(
		char *grace_ps_system_string,
		GRACE_PAGE *grace_page,
		GRACE_TITLE *grace_title,
		GRACE_GRAPH *grace_graph,
		char *grace_pdf_system_string )
{
	FILE *output_pipe;

	if ( !grace_ps_system_string
	||   !grace_page
	||   !grace_title
	||   !grace_graph
	||   !grace_pdf_system_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	output_pipe = appaserver_output_pipe( grace_ps_system_string );

	grace_page_output(
		grace_page->width_pixels,
		grace_page->length_pixels,
		output_pipe );

	grace_graph_output(
		grace_title,
		grace_graph,
		output_pipe );

	pclose( output_pipe );

	if ( system( grace_pdf_system_string ) ){}
}

void grace_graph_list_pdf_create(
		char *grace_ps_system_string,
		GRACE_PAGE *grace_page,
		GRACE_TITLE *grace_title,
		LIST *grace_graph_list,
		char *grace_pdf_system_string )
{
	FILE *output_pipe;
	GRACE_GRAPH *grace_graph;

	if ( !grace_ps_system_string
	||   !grace_page
	||   !grace_title
	||   !list_rewind( grace_graph_list )
	||   !grace_pdf_system_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	output_pipe = appaserver_output_pipe( grace_ps_system_string );

	grace_page_output(
		grace_page->width_pixels,
		grace_page->length_pixels,
		output_pipe );

	do {
		grace_graph = list_get( grace_graph_list );

		if ( list_at_last( grace_graph_list ) )
		{
			grace_graph_output(
				grace_title,
				grace_graph,
				output_pipe );
		}
		else
		{
			grace_graph_output(
				(GRACE_TITLE *)0,
				grace_graph,
				output_pipe );
		}

	} while ( list_next( grace_graph_list ) );

	pclose( output_pipe );

	if ( system( grace_pdf_system_string ) ){}
}

GRACE_WINDOW *grace_window_new(
		GRACE_FILENAME *grace_filename,
		char *sub_title,
		char *filename_key,
		pid_t process_id )
{
	GRACE_WINDOW *grace_window;
	char *onload_open_tag;
	char *sub_title_display;

	if ( !process_id )
	{
		char message[ 128 ];

		sprintf(message, "process_id is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	grace_window = grace_window_calloc();

	if ( grace_filename )
	{
		char *pdf_target;

		pdf_target =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			grace_prompt_pdf_target(
				filename_key,
				process_id );

		grace_window->onload_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			chart_window_onload_string(
				grace_filename->pdf_prompt_filename,
				pdf_target );
	}

	onload_open_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_body_onload_open_tag(
			JAVASCRIPT_WAIT_OVER,
			grace_window->onload_string /* javascript_replace */,
			(char *)0 /* document_body_menu_onload_string */ );

	grace_window->date_time_now =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		date_time_now( date_utc_offset() );

	grace_window->screen_title =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		grace_window_screen_title(
			grace_window->date_time_now );

	sub_title_display =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		chart_window_sub_title_display(
			sub_title );

	grace_window->grace_home_url_html =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		grace_window_grace_home_url_html(
			GRACE_WINDOW_GRACE_URL );

	grace_window->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		chart_window_html(
			onload_open_tag,
			grace_window->screen_title,
			sub_title_display );

	free( onload_open_tag );
	free( sub_title_display );

	return grace_window;
}

GRACE_WINDOW *grace_window_calloc( void )
{
	GRACE_WINDOW *grace_window;

	if ( ! ( grace_window = calloc( 1, sizeof ( GRACE_WINDOW ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_window;
}

char *grace_prompt_pdf_target(
		char *filename_key,
		pid_t process_id )
{
	static char target[ 128 ];

	if ( filename_key )
	{
		snprintf(
			target,
			sizeof ( target ),
			"grace_pdf_%s_%d",
			filename_key,
			process_id );
	}
	else
	{
		snprintf(
			target,
			sizeof ( target ),
			"grace_pdf_%d",
			process_id );
	}

	return target;
}

char *grace_window_screen_title( char *date_time_now )
{
	static char screen_title[ 128 ];

	if ( !date_time_now )
	{
		char message[ 128 ];

		sprintf(message, "date_time_now is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(screen_title,
		"<h1>Grace Chart Viewer<br>%s</h1>",
		date_time_now );

	return screen_title;
}

char *grace_prompt_pdf_anchor_html(
		char *pdf_prompt_filename,
		char *filename_key,
		char *grace_prompt_pdf_target )
{
	static char html[ 256 ];
	char *ptr = html;

	if ( !pdf_prompt_filename
	||   !grace_prompt_pdf_target )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf( ptr,
"<a href=\"%s\" target=\"%s\">View ",
			pdf_prompt_filename,
			grace_prompt_pdf_target );

	if ( filename_key )
	{
		char buffer[ 128 ];

		ptr += sprintf( ptr,
			"%s ",
			string_initial_capital(
				buffer,
				filename_key ) );
	}

	sprintf( ptr, "chart</a>" );
	return html;
}

char *grace_prompt_agr_target(
		char *filename_key,
		pid_t process_id )
{
	static char target[ 128 ];

	if ( filename_key )
	{
		snprintf(
			target,
			sizeof ( target ),
			"grace_agr_%s_%d",
			filename_key,
			process_id );
	}
	else
	{
		snprintf(
			target,
			sizeof ( target ),
			"grace_agr_%d",
			process_id );
	}

	return target;
}

char *grace_prompt_agr_anchor_html(
		char *agr_prompt_filename,
		char *filename_key,
		char *grace_prompt_agr_target )
{
	static char html[ 256 ];
	char *ptr = html;

	if ( !agr_prompt_filename
	||   !grace_prompt_agr_target )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(ptr,
"<a href=\"%s\" target=\"%s\">View ",
			agr_prompt_filename,
			grace_prompt_agr_target );

	if ( filename_key )
	{
		char buffer[ 128 ];

		ptr += sprintf( ptr,
			"%s ",
			string_initial_capital(
				buffer,
				filename_key ) );
	}

	sprintf(ptr, "AGR file</a>" );

	return html;
}

char *grace_prompt_html(
		char *grace_prompt_pdf_anchor_html,
		char *grace_prompt_agr_anchor_html )
{
	char html[ 1024 ];

	if ( !grace_prompt_pdf_anchor_html
	||   !grace_prompt_agr_anchor_html )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		html,
		sizeof ( html ),
		"<tr><td>%s<td>%s",
		grace_prompt_pdf_anchor_html,
		grace_prompt_agr_anchor_html );

	return strdup( html );
}

char *grace_window_grace_home_url_html( char *grace_window_grace_url )
{
	static char url_html[ 256 ];

	if ( !grace_window_grace_url )
	{
		char message[ 128 ];

		sprintf(message, "grace_home_grace_url is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		url_html,
		sizeof ( url_html ),
"<br><p>For more information, visit the <a href=\"%s\" target=_new>Grace Home Page.</a>",
		grace_window_grace_url );

	return url_html;
}

double grace_view_max_y(
		double grace_view_min_y,
		double grace_view_y_offset )
{
	return grace_view_min_y + grace_view_y_offset;
}

GRACE_TICKLABEL *grace_ticklabel_new(
		double horizontal_double,
		char *ticklabel_string )
{
	GRACE_TICKLABEL *grace_ticklabel;

	if ( !ticklabel_string )
	{
		char message[ 128 ];

		sprintf(message, "ticklabel_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	grace_ticklabel = grace_ticklabel_calloc();

	grace_ticklabel->agr_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		grace_ticklabel_agr_string(
			grace_ticklabel_major_integer(),
			horizontal_double,
			ticklabel_string );

	return grace_ticklabel;
}

GRACE_TICKLABEL *grace_ticklabel_calloc( void )
{
	GRACE_TICKLABEL *grace_ticklabel;

	if ( ! ( grace_ticklabel = calloc( 1, sizeof ( GRACE_TICKLABEL ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_ticklabel;
}

int grace_ticklabel_major_integer( void )
{
	static int major_integer = 0;

	return ++major_integer;
}

char *grace_ticklabel_agr_string(
		int grace_ticklabel_major_integer,
		double horizontal_double,
		char *ticklabel_string )
{
	char agr_string[ 128 ];

	if ( !grace_ticklabel_major_integer
	||   !ticklabel_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ticklabel_string && *ticklabel_string )
	{
		sprintf(agr_string,
"@    xaxis  ticklabel %d, \"%s\"\n",
			grace_ticklabel_major_integer,
			ticklabel_string );
	}
	else
	{
		sprintf(agr_string,
"@    xaxis  tick major %d, %.6lf\n",
			grace_ticklabel_major_integer,
			horizontal_double );
	}

	return strdup( agr_string );
}

double grace_ticklabel_first_horizontal_double(
		LIST *grace_ticklabel_list )
{
	GRACE_TICKLABEL *grace_ticklabel;

	if ( ! ( grace_ticklabel =
			list_first(
				grace_ticklabel_list ) ) )
	{
		char message[ 128 ];

		sprintf(message, "grace_ticklabel_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_ticklabel->horizontal_double;
}

void grace_ticklabel_list_output(
		LIST *grace_ticklabel_list,
		FILE *output_file )
{
	GRACE_TICKLABEL *grace_ticklabel;

	if ( !output_file )
	{
		char message[ 128 ];

		sprintf(message, "output_file is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( grace_ticklabel_list ) ) return;

	do {
		grace_ticklabel = list_get( grace_ticklabel_list );

		fprintf(output_file,
			"%s\n",
			grace_ticklabel->agr_string );

	} while ( list_next( grace_ticklabel_list ) );
}

GRACE_HORIZONTAL_LABEL *grace_horizontal_label_new( void )
{
	GRACE_HORIZONTAL_LABEL *grace_horizontal_label;

	grace_horizontal_label = grace_horizontal_label_calloc();
	grace_horizontal_label->grace_ticklabel_list = list_new();

	return grace_horizontal_label;
}

GRACE_HORIZONTAL_LABEL *grace_horizontal_label_calloc( void )
{
	GRACE_HORIZONTAL_LABEL *grace_horizontal_label;

	if ( ! ( grace_horizontal_label =
			calloc( 1,
				sizeof ( GRACE_HORIZONTAL_LABEL ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_horizontal_label;
}

void grace_horizontal_label_output(
		GRACE_HORIZONTAL_LABEL *grace_horizontal_label,
		FILE *output_file )
{
	int grace_ticklabel_list_length;

	if ( !grace_horizontal_label
	||   !output_file )
	{
		return;
	}

	if ( ! ( grace_ticklabel_list_length =
			list_length(
				grace_horizontal_label->
					grace_ticklabel_list ) ) )
	{
		return;
	}

	fprintf(output_file,
		"%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		grace_horizontal_label_agr_string(
			grace_ticklabel_first_horizontal_double(
				grace_horizontal_label->
					grace_ticklabel_list ),
			grace_ticklabel_list_length ) );

	grace_ticklabel_list_output(
		grace_horizontal_label->grace_ticklabel_list,
		output_file );

	fprintf(output_file,
		"%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		grace_horizontal_label_tick_minor_string(
			grace_ticklabel_list_length ) );
}

char *grace_horizontal_label_agr_string(
		double grace_ticklabel_first_horizontal_double,
		int grace_ticklabel_list_length )
{
	static char agr_string[ 256 ];

	sprintf(agr_string,
"@    xaxis  tick spec type both\n"
"@    xaxis  tick spec %d\n"
"@    xaxis  tick minor 0, %lf",
		grace_ticklabel_list_length + 2,
		grace_ticklabel_first_horizontal_double - 1.0 );

	return agr_string;
}

char *grace_horizontal_label_tick_minor_string(
		int grace_ticklabel_list_length )
{
	static char tick_minor_string[ 128 ];

	sprintf(tick_minor_string,
"@    xaxis  tick minor %d, %d",
		 grace_ticklabel_list_length + 1,
		 grace_ticklabel_list_length );

	return tick_minor_string;
}

GRACE_SETUP *grace_setup_new(
		char *application_name,
		char *session_key,
		char *application_parameter_data_directory,
		char *title,
		char *sub_title,
		char *filename_key,
		int grace_graph_list_length )
{
	GRACE_SETUP *grace_setup;

	grace_setup = grace_setup_calloc();

	grace_setup->grace_filename =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_filename_new(
			application_name,
			session_key,
			filename_key,
			application_parameter_data_directory );

	grace_setup->grace_ps_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		grace_ps_system_string(
			GRACE_EXECUTABLE_TEMPLATE,
			grace_setup->grace_filename );

	grace_setup->grace_page =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_page_new(
			grace_graph_list_length );

	grace_setup->grace_pdf_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		grace_pdf_system_string(
			grace_setup->grace_filename );

	grace_setup->grace_title =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_title_new(
			title,
			sub_title );

	return grace_setup;
}

GRACE_SETUP *grace_setup_calloc( void )
{
	GRACE_SETUP *grace_setup;

	if ( ! ( grace_setup = calloc( 1, sizeof ( GRACE_SETUP ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_setup;
}

GRACE_GENERIC *grace_generic_new(
		char *application_name,
		char *data_directory,
		char *value_attribute_name,
		char *process_generic_grace_title,
		char *process_generic_grace_sub_title,
		LIST *process_generic_grace_point_list,
		int process_generic_grace_horizontal_range )
{
	GRACE_GENERIC *grace_generic;

	if ( !application_name
	||   !data_directory
	||   !value_attribute_name
	||   !process_generic_grace_title
	||   !process_generic_grace_sub_title )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	grace_generic = grace_generic_calloc();

	grace_generic->grace_setup =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_setup_new(
			application_name,
			(char *)0 /* session_key */,
			data_directory,
			process_generic_grace_title,
			process_generic_grace_sub_title,
			(char *)0 /* filename_key */,
			1 /* graph_list_length */ );

	grace_generic->grace_view =
		grace_view_new(
			GRACE_VIEW_MIN_X,
			GRACE_VIEW_PORTRAIT_X_OFFSET,
			GRACE_VIEW_LANDSCAPE_X_OFFSET,
			1 /* graph_list_length */,	
			process_generic_grace_horizontal_range,
			grace_generic->
				grace_setup->
				grace_page->
				landscape_boolean );

	if ( !grace_generic->grace_view )
	{
		char message[ 128 ];

		sprintf(message, "grace_view_new() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	grace_generic->grace_graph =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_generic_grace_graph(
			value_attribute_name /* dataset_name */,
			process_generic_grace_point_list,
			process_generic_grace_horizontal_range,
			grace_generic->grace_view );

	grace_generic->grace_point_vertical_range_double =
		grace_point_vertical_range_double(
			process_generic_grace_point_list );

	if ( ! ( grace_generic->grace_graph->grace_world =
			grace_world_new(
				grace_generic->grace_graph->
					grace_dataset_list ) ) )
	{
		char message[ 128 ];

		sprintf(message, "grace_world_new() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_generic;
}

GRACE_GENERIC *grace_generic_calloc( void )
{
	GRACE_GENERIC *grace_generic;

	if ( ! ( grace_generic = calloc( 1, sizeof ( GRACE_GENERIC ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_generic;
}

GRACE_GRAPH *grace_generic_grace_graph(
		char *dataset_name,
		LIST *process_generic_grace_point_list,
		int process_generic_grace_horizontal_range,
		GRACE_VIEW *grace_view )
{
	GRACE_GRAPH *grace_graph;
	GRACE_DATASET *grace_dataset;

	if ( !dataset_name
	||   !grace_view )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	grace_graph =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_graph_new(
			0 /* graph_number */,
			1 /* xaxis_tick_on_boolean */,
			process_generic_grace_horizontal_range,
			dataset_name /* yaxis_heading */,
			grace_view );

	list_set(
		grace_graph->grace_dataset_list,
		( grace_dataset =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			grace_dataset_new(
			     dataset_name /* datatype_name */,
			     process_generic_grace_point_list,
			     grace_dataset_xy,
			     grace_dataset_color_number(
				GRACE_DATASET_MAX_COLOR_NUMBER,
				0 /* not cycle_colors_boolean */ ),
				GRACE_DATASET_LINE_SYMBOL_SIZE ) ) );

	return grace_graph;
}

GRACE_POINT *grace_point_delimited_parse(
		char delimiter,
		int date_piece,
		int time_piece,
		int value_piece,
		char *delimited_row )
{
	char *date_string;
	char *time_string = {0};
	boolean value_null_boolean;
	double horizontal_double;
	double value_double;

	if ( !delimiter
	||   !delimited_row )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	date_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		grace_point_date_string(
			delimiter,
			date_piece,
			delimited_row );

	if ( time_piece > -1 )
	{
		time_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			grace_point_time_string(
				delimiter,
				time_piece,
				delimited_row );
	}

	horizontal_double =
		grace_graph_horizontal_double(
			date_string,
			time_string );

	if ( !horizontal_double ) return (GRACE_POINT *)0;

	value_null_boolean =
		grace_point_value_null_boolean(
			delimiter,
			value_piece,
			delimited_row );

	if ( value_null_boolean ) return (GRACE_POINT *)0;

	value_double =
		grace_point_value_double(
			delimiter,
			value_piece,
			delimited_row );

	return
	grace_point_new(
		horizontal_double,
		value_double /* vertical_double */ );
}

char *grace_point_date_string(
		char delimiter,
		int date_piece,
		char *delimited_row )
{
	static char date_string[ 32 ];

	if ( !delimiter
	||   !delimited_row )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	piece(	date_string,
		delimiter,
		delimited_row,
		date_piece );
}

char *grace_point_time_string(
		char delimiter,
		int time_piece,
		char *delimited_row )
{
	static char time_string[ 32 ];

	if ( !delimiter
	||   !delimited_row )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( time_piece == -1 )
	{
		*time_string = '\0';
		return time_string;
	}
	else
	{
		return
		piece(	time_string,
			delimiter,
			delimited_row,
			time_piece );
	}
}

boolean grace_point_value_null_boolean(
		char delimiter,
		int value_piece,
		char *delimited_row )
{
	char value_string[ 32 ];

	if ( !delimiter
	||   !delimited_row )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	piece(	value_string,
		delimiter,
		delimited_row,
		value_piece );

	if ( !*value_string )
		return 1;
	else
		return 0;
}

double grace_point_value_double(
		char delimiter,
		int value_piece,
		char *delimited_row )
{
	char value_string[ 32 ];

	if ( !delimiter
	||   !delimited_row )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	piece(	value_string,
		delimiter,
		delimited_row,
		value_piece );

	return atof( value_string );
}

GRACE_WHISKER_POINT *grace_whisker_point_new(
		double horizontal_double,
		double low_value,
		double average_value,
		double high_value )
{
	GRACE_WHISKER_POINT *grace_whisker_point =
		grace_whisker_point_calloc();

	grace_whisker_point->horizontal_double = horizontal_double;
	grace_whisker_point->low_value = low_value;
	grace_whisker_point->average_value = average_value;
	grace_whisker_point->high_value = high_value;

	return grace_whisker_point;
}

GRACE_WHISKER_POINT *grace_whisker_point_calloc( void )
{
	GRACE_WHISKER_POINT *grace_whisker_point;

	if ( ! ( grace_whisker_point =
			calloc( 1,
				sizeof ( GRACE_WHISKER_POINT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_whisker_point;
}

double grace_whisker_point_minimum_horizontal(
		LIST *grace_whisker_point_list )
{
	GRACE_WHISKER_POINT *grace_whisker_point;

	if ( ! ( grace_whisker_point =
			list_first( grace_whisker_point_list ) ) )
	{
		return 0.0;
	}

	return grace_whisker_point->horizontal_double;
}

double grace_whisker_point_maximum_horizontal(
		LIST *grace_whisker_point_list )
{
	GRACE_WHISKER_POINT *grace_whisker_point;

	if ( ! ( grace_whisker_point =
			list_last( grace_whisker_point_list ) ) )
	{
		return 0.0;
	}

	return grace_whisker_point->horizontal_double;
}

double grace_whisker_point_minimum_vertical(
		LIST *grace_whisker_point_list )
{
	GRACE_WHISKER_POINT *grace_whisker_point;
	double minimum_vertical = FLOAT_MAXIMUM_DOUBLE;

	if ( !list_rewind( grace_whisker_point_list ) ) return 0.0;

	do {
		grace_whisker_point =
			list_get(
				grace_whisker_point_list );

		if ( grace_whisker_point->low_value < minimum_vertical )
			minimum_vertical =
				grace_whisker_point->low_value;

	} while ( list_next( grace_whisker_point_list ) );

	return minimum_vertical;
}

double grace_whisker_point_maximum_vertical(
		LIST *grace_whisker_point_list )
{
	GRACE_WHISKER_POINT *grace_whisker_point;
	double maximum_vertical = 0.0;

	if ( !list_rewind( grace_whisker_point_list ) ) return 0.0;

	do {
		grace_whisker_point =
			list_get(
				grace_whisker_point_list );

		if ( grace_whisker_point->high_value > maximum_vertical )
			maximum_vertical =
				grace_whisker_point->high_value;

	} while ( list_next( grace_whisker_point_list ) );

	return maximum_vertical;
}

GRACE_WHISKER_POINT *grace_whisker_point_delimited_parse(
			char delimiter,
			int date_piece,
			int time_piece,
			int low_value_piece,
			int average_value_piece,
			int high_value_piece,
			char *delimited_row )
{
	char *date_string;
	char *time_string = {0};
	double horizontal_double;
	boolean value_null_boolean;
	double low_value;
	double average_value;
	double high_value;

	if ( !delimiter
	||   !delimited_row )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	date_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		grace_point_date_string(
			delimiter,
			date_piece,
			delimited_row );

	if ( time_piece > -1 )
	{
		time_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			grace_point_time_string(
				delimiter,
				time_piece,
				delimited_row );
	}

	horizontal_double =
		grace_graph_horizontal_double(
			date_string,
			time_string );

	if ( !horizontal_double ) return (GRACE_WHISKER_POINT *)0;

	value_null_boolean =
		grace_point_value_null_boolean(
			delimiter,
			low_value_piece,
			delimited_row );

	if ( value_null_boolean ) return (GRACE_WHISKER_POINT *)0;

	low_value =
		grace_point_value_double(
			delimiter,
			low_value_piece,
			delimited_row );

	average_value =
		grace_point_value_double(
			delimiter,
			average_value_piece,
			delimited_row );

	high_value =
		grace_point_value_double(
			delimiter,
			high_value_piece,
			delimited_row );

	return
	grace_whisker_point_new(
		horizontal_double,
		low_value,
		average_value,
		high_value );
}

void grace_whisker_point_list_output(
			LIST *grace_whisker_point_list,
			FILE *output_file )
{
	GRACE_WHISKER_POINT *grace_whisker_point;

	if ( !output_file )
	{
		char message[ 128 ];

		sprintf(message, "output_file is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( grace_whisker_point_list ) ) return;

	do {
		grace_whisker_point =
			list_get(
				grace_whisker_point_list );

		grace_whisker_point_output(
			grace_whisker_point,
			output_file );

	} while ( list_next( grace_whisker_point_list ) );

	fprintf( output_file, "&\n" );
}

void grace_whisker_point_output(
			GRACE_WHISKER_POINT *grace_whisker_point,
			FILE *output_file )
{
	if ( !grace_whisker_point )
	{
		char message[ 128 ];

		sprintf(message, "grace_whisker_point is empty." );

		if ( output_file ) fclose( output_file );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !output_file )
	{
		char message[ 128 ];

		sprintf(message, "output_file is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	fprintf(output_file,
		"%.6lf\t%.4lf\t%.4lf\t%.4lf\t%.4lf\n",
		grace_whisker_point->horizontal_double,
		grace_whisker_point->high_value,
		grace_whisker_point->low_value,
		grace_whisker_point->average_value,
		grace_whisker_point->average_value );
}

GRACE_WHISKER *grace_whisker_new(
		char *application_name,
		char *data_directory,
		char *value_attribute_name,
		char *process_generic_whisker_title,
		char *process_generic_whisker_sub_title,
		LIST *process_generic_whisker_point_list,
		int process_generic_whisker_horizontal_range )
{
	GRACE_WHISKER *grace_whisker;

	if ( !application_name
	||   !data_directory
	||   !value_attribute_name
	||   !process_generic_whisker_title
	||   !process_generic_whisker_sub_title
	||   !list_length( process_generic_whisker_point_list )
	||   !process_generic_whisker_horizontal_range )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	grace_whisker = grace_whisker_calloc();

	grace_whisker->grace_setup =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_setup_new(
			application_name,
			(char *)0 /* session_key */,
			data_directory,
			process_generic_whisker_title,
			process_generic_whisker_sub_title,
			(char *)0 /* filename_key */,
			1 /* graph_list_length */ );

	grace_whisker->grace_view =
		grace_view_new(
			GRACE_VIEW_MIN_X,
			GRACE_VIEW_PORTRAIT_X_OFFSET,
			GRACE_VIEW_LANDSCAPE_X_OFFSET,
			1 /* graph_list_length */,	
			process_generic_whisker_horizontal_range,
			grace_whisker->
				grace_setup->
				grace_page->
				landscape_boolean );

	if ( !grace_whisker->grace_view )
	{
		char message[ 128 ];

		sprintf(message, "grace_view_new() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	grace_whisker->grace_graph =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_whisker_grace_graph(
			value_attribute_name /* dataset_name */,
			process_generic_whisker_point_list,
			process_generic_whisker_horizontal_range,
			grace_whisker->grace_view );

	grace_whisker->grace_graph->grace_world =
		grace_world_new(
			grace_whisker->
				grace_graph->
				grace_dataset_list );

	if ( !grace_whisker->grace_graph->grace_world )
	{
		char message[ 128 ];

		sprintf(message, "grace_world_new() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_whisker;
}

GRACE_WHISKER *grace_whisker_calloc( void )
{
	GRACE_WHISKER *grace_whisker;

	if ( ! ( grace_whisker = calloc( 1, sizeof ( GRACE_WHISKER ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_whisker;
}

GRACE_GRAPH *grace_whisker_grace_graph(
		char *dataset_name,
		LIST *process_generic_whisker_point_list,
		int process_generic_whisker_horizontal_range,
		GRACE_VIEW *grace_view )
{
	GRACE_GRAPH *grace_graph;
	GRACE_DATASET *grace_dataset;

	if ( !dataset_name
	||   !list_length( process_generic_whisker_point_list )
	||   !process_generic_whisker_horizontal_range
	||   !grace_view )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	grace_graph =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_graph_new(
			0 /* graph_number */,
			1 /* xaxis_tick_on_boolean */,
			process_generic_whisker_horizontal_range,
			dataset_name /* yaxis_heading */,
			grace_view );

	grace_dataset =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_dataset_new(
			dataset_name /* datatype_name */,
			(LIST *)0 /* grace_point_list */,
			grace_dataset_whisker,
			grace_dataset_color_number(
				GRACE_DATASET_MAX_COLOR_NUMBER,
				0 /* not cycle_colors_boolean */ ),
			GRACE_DATASET_WHISKER_SYMBOL_SIZE ); 

	grace_dataset->grace_whisker_point_list =
		process_generic_whisker_point_list;

	list_set(
		grace_graph->grace_dataset_list,
		grace_dataset );

	return grace_graph;
}

char *grace_page_map_font_string( void )
{
	return
	"@map font 0 to \"Times-Roman\", \"Times-Roman\"\n"
	"@map font 1 to \"Times-Italic\", \"Times-Italic\"\n"
	"@map font 2 to \"Times-Bold\", \"Times-Bold\"\n"
	"@map font 3 to \"Times-BoldItalic\", \"Times-BoldItalic\"\n"
	"@map font 4 to \"Helvetica\", \"Helvetica\"\n"
	"@map font 5 to \"Helvetica-Oblique\", \"Helvetica-Oblique\"\n"
	"@map font 6 to \"Helvetica-Bold\", \"Helvetica-Bold\"\n"
	"@map font 7 to \"Helvetica-BoldOblique\", \"Helvetica-BoldOblique\"\n"
	"@map font 8 to \"Courier\", \"Courier\"\n"
	"@map font 9 to \"Courier-Oblique\", \"Courier-Oblique\"\n"
	"@map font 10 to \"Courier-Bold\", \"Courier-Bold\"\n"
	"@map font 11 to \"Courier-BoldOblique\", \"Courier-BoldOblique\"\n"
	"@map font 12 to \"Symbol\", \"Symbol\"\n"
	"@map font 13 to \"ZapfDingbats\", \"ZapfDingbats\"\n";
}

char *grace_page_map_color_string( void )
{
	return
	"@map color 0 to (255, 255, 255), \"white\"\n"
	"@map color 1 to (0, 0, 0), \"black\"\n"
	"@map color 2 to (255, 0, 0), \"red\"\n"
	"@map color 3 to (0, 0, 255), \"blue\"\n"
	"@map color 4 to (0, 255, 0), \"green\"\n"
	"@map color 5 to (255, 165, 0), \"orange\"\n"
	"@map color 6 to (188, 143, 143), \"brown\"\n"
	"@map color 7 to (148, 0, 211), \"violet\"\n"
	"@map color 8 to (0, 255, 255), \"cyan\"\n"
	"@map color 9 to (255, 0, 255), \"magenta\"\n"
	"@map color 10 to (103, 7, 72), \"maroon\"\n"
	"@map color 11 to (114, 33, 188), \"indigo\"\n"
	"@map color 12 to (0, 125, 255), \"gr_blue\"\n"
	"@map color 13 to (64, 224, 208), \"turquoise\"\n"
	"@map color 14 to (0, 139, 0), \"green4\"\n"
	"@map color 15 to (220, 220, 220), \"grey\"\n";
}

char *grace_page_default_string( void )
{
	static char default_string[ 512 ];

	sprintf(default_string,
		"@default linewidth 1.0\n"
		"@default linestyle 1\n"
		"@default color 1\n"
		"@default pattern 1\n"
		"@default font 6\n"
		"@default char size 1.000000\n"
		"@default symbol size 1.000000\n"
		"@default sformat \"%c16.8f\"\n"
		"@reference date 0\n"
		"@date wrap on\n"
		"@date wrap year 1900\n"
		"@background color 0\n"
		"@page background fill on\n",
		'%' );

	return default_string;
}

char *grace_page_timestamp_string( void )
{
	return
	"@timestamp off\n"
	"@timestamp 0.03, 0.03\n"
	"@timestamp color 1\n"
	"@timestamp rot 0\n"
	"@timestamp font 6\n"
	"@timestamp char size 1.000000\n";
}

char *grace_page_region_string( void )
{
	static char region_string[ 1024 ];
	char *ptr = region_string;
	int region;

	for ( region = 0; region < 5; region++ )
	{
		ptr += sprintf(
			ptr,
			"@r%d type above\n"
			"@r%d linestyle 1\n"
			"@r%d linewidth 1.0\n"
			"@r%d color 1\n"
			"@r%d line 0, 0, 0, 0\n",
			region,
			region,
			region,
			region,
			region );
	}

	return region_string;
}

GRACE_PROMPT *grace_prompt_new(
		GRACE_FILENAME *grace_filename,
		char *filename_key,
		pid_t process_id )
{
	GRACE_PROMPT *grace_prompt;

	if ( !grace_filename
	||   !process_id )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	grace_prompt = grace_prompt_calloc();

	grace_prompt->pdf_target =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		grace_prompt_pdf_target(
			filename_key,
			process_id );

	grace_prompt->pdf_anchor_html =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		grace_prompt_pdf_anchor_html(
			grace_filename->pdf_prompt_filename,
			filename_key,
			grace_prompt->pdf_target );

	grace_prompt->agr_target =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		grace_prompt_agr_target(
			filename_key,
			process_id );

	grace_prompt->agr_anchor_html =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		grace_prompt_agr_anchor_html(
			grace_filename->agr_prompt_filename,
			filename_key,
			grace_prompt->agr_target );

	grace_prompt->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		grace_prompt_html(
			grace_prompt->pdf_anchor_html,
			grace_prompt->agr_anchor_html );

	return grace_prompt;
}

GRACE_PROMPT *grace_prompt_calloc( void )
{
	GRACE_PROMPT *grace_prompt;

	if ( ! ( grace_prompt = calloc( 1, sizeof ( GRACE_PROMPT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return grace_prompt;
}

double grace_world_y_tick_major(
		double maximum_vertical,
		double world_max_y )
{
	double tick_major;

	if ( maximum_vertical < 0.0 )
	{
		maximum_vertical = float_abs( maximum_vertical );
	}

	if ( maximum_vertical < 1.0 )
		tick_major = 0.1;
	else
	if ( maximum_vertical < 2.0 )
		tick_major = 0.2;
	else
	if ( maximum_vertical < 5.0 )
		tick_major = 0.5;
	else
	if ( maximum_vertical < 10.0 )
		tick_major = 1.0;
	else
	if ( maximum_vertical < 20.0 )
		tick_major = 2.0;
	else
	if ( maximum_vertical < 30.0 )
		tick_major = 3.0;
	else
	if ( maximum_vertical < 50.0 )
		tick_major = 4.0;
	else
	if ( maximum_vertical < 100.0 )
		tick_major = 6.0;
	else
		tick_major = maximum_vertical / 15.0;

	if ( world_max_y > 0.0
	&&   tick_major >= world_max_y )
	{
		tick_major = world_max_y / 2.0;
	}

	return tick_major;
}

int grace_world_y_tick_precision( double grace_world_y_tick_major )
{
	return ( grace_world_y_tick_major < 1.0 );
}

double grace_point_horizontal_range_double( LIST *grace_point_list )
{
	GRACE_POINT *grace_point;
	double minimum_horizontal = FLOAT_MAXIMUM_DOUBLE;
	double maximum_horizontal = FLOAT_MINIMUM_DOUBLE;

	if ( !list_rewind( grace_point_list ) ) return 0.0;

	do {
		grace_point = list_get( grace_point_list );

		if ( grace_point->horizontal_double < minimum_horizontal )
		{
			minimum_horizontal = grace_point->horizontal_double;
		}
		else
		if ( grace_point->horizontal_double > maximum_horizontal )
		{
			maximum_horizontal = grace_point->horizontal_double;
		}

	} while ( list_next( grace_point_list ) );

	return maximum_horizontal - minimum_horizontal;
}

double grace_point_vertical_range_double( LIST *grace_point_list )
{
	GRACE_POINT *grace_point;
	double minimum_vertical = FLOAT_MAXIMUM_DOUBLE;
	double maximum_vertical = FLOAT_MINIMUM_DOUBLE;

	if ( !list_rewind( grace_point_list ) ) return 0.0;

	do {
		grace_point = list_get( grace_point_list );

		if ( grace_point->vertical_double < minimum_vertical )
		{
			minimum_vertical = grace_point->vertical_double;
		}
		else
		if ( grace_point->vertical_double > maximum_vertical )
		{
			maximum_vertical = grace_point->vertical_double;
		}

	} while ( list_next( grace_point_list ) );

	return maximum_vertical - minimum_vertical;
}

LIST *grace_dataset_point_list(
		char *date_attribute_name,
		char *time_attribute_name,
		LIST *query_fetch_row_list,
		char *grace_datatype_name )
{
	QUERY_ROW *query_row;
	QUERY_CELL *number_query_cell;
	QUERY_CELL *date_query_cell;
	QUERY_CELL *time_query_cell = {0};
	double vertical_double;
	double horizontal_double;
	LIST *grace_point_list;

	if ( !list_rewind( query_fetch_row_list ) ) return NULL;

	grace_point_list = list_new();

	do {
		query_row =
			list_get(
				query_fetch_row_list );

		if ( ! ( number_query_cell =
				query_cell_seek(
					grace_datatype_name,
					query_row->cell_list ) ) )
		{
			char message[ 128 ];

			sprintf(message,
				"query_cell_seek(%s) returned empty.",
				grace_datatype_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( grace_graph_null_boolean(
			number_query_cell->select_datum ) )
		{
			continue;
		}

		vertical_double =
			grace_graph_vertical_double(
				number_query_cell->
					select_datum );

		if ( ! ( date_query_cell =
				query_cell_seek(
					date_attribute_name,
					query_row->cell_list ) ) )
		{
			char message[ 128 ];

			sprintf(message,
				"query_cell_seek(%s) returned empty.",
				date_attribute_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( time_attribute_name )
		{
			time_query_cell =
				query_cell_seek(
					time_attribute_name,
					query_row->cell_list );
		}

		horizontal_double =
			grace_graph_cell_horizontal_double(
				date_query_cell,
				time_query_cell );

		if ( !horizontal_double ) continue;

		list_set(
			grace_point_list,
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			grace_point_new(
				horizontal_double,
				vertical_double ) );

	} while ( list_next( query_fetch_row_list ) );

	return grace_point_list;
}

