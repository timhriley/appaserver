/* google_chart_dvr.c					*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "google_chart.h"
#include "piece.h"
#include "appaserver_library.h"
#include "appaserver_error.h"

void test_bar(		void );
void test_timeline(	void );

int main( void )
{
	/* test_bar(); */
	test_timeline();
	return 0;
}

void test_bar( void )
{
	GOOGLE_CHART *google_chart;

	google_chart =
		google_chart_new(
			google_column_chart,
			"Month",
			265 /* left */,
			50 /* top */,
			800 /* width */,
			500 /* height */,
			"#effdff" /* background_color */,
			0 /* not legend_position_bottom */,
			GOOGLE_CORECHART /* google_package_name */ );

	list_append_pointer(	google_chart->google_datatype_name_list,
				"periodofrecord" );
	list_append_pointer(	google_chart->google_datatype_name_list,
				"current" );

	google_chart_append_xaxis(
			google_chart->xaxis_list,
			"jan",
			list_length(
				google_chart->google_datatype_name_list ) );

	google_chart_append_xaxis(
			google_chart->xaxis_list,
			"feb",
			list_length(
				google_chart->google_datatype_name_list ) );

	google_chart_set_point(google_chart->xaxis_list,
				google_chart->google_datatype_name_list,
				"jan",
				(char *)0 /* hhmm */,
				"periodofrecord",
				1.3 );

	google_chart_set_point(google_chart->xaxis_list,
				google_chart->google_datatype_name_list,
				"jan",
				(char *)0 /* hhmm */,
				"current",
				1.4 );

	google_chart_set_point(google_chart->xaxis_list,
				google_chart->google_datatype_name_list,
				"feb",
				(char *)0 /* hhmm */,
				"periodofrecord",
				2.3 );

	google_chart_set_point(google_chart->xaxis_list,
				google_chart->google_datatype_name_list,
				"feb",
				(char *)0 /* hhmm */,
				"current",
				2.4 );

/*
	google_chart_display(	google_chart->xaxis_datatype_name,
				google_chart->xaxis_list,
				google_chart->google_datatype_name_list );
*/

	printf( "<html><body>\n" );
	google_chart_output_non_annotated( stdout );
	google_chart_output_draw_visualization_function(
				stdout,
				google_chart->google_chart_type,
				google_chart->xaxis_datatype_name,
				google_chart->xaxis_list,
				google_chart->google_datatype_name_list,
				"1AAA POR",
				"Rain/Inches (Sum)",
				google_chart->left,
				google_chart->top,
		 		google_chart->width,
		 		google_chart->height,
		 		google_chart->background_color,
				google_chart->legend_position_bottom,
				1 /* chart_type_bar */,
				google_chart->google_package_name,
				0 /* not dont_display_range_selector */,
				daily /* aggregate_level */ );

	printf( "</body></html>\n" );
} /* test_bar() */

void test_timeline( void )
{
	GOOGLE_CHART *google_chart;

	google_chart =
		google_chart_new(
			google_time_line,
			"Date",
			265 /* left */,
			50 /* top */,
			800 /* width */,
			500 /* height */,
			"#effdff" /* background_color */,
			0 /* not legend_position_bottom */,
			GOOGLE_ANNOTATED_TIMELINE /* google_package_name */);

	list_append_pointer(	google_chart->google_datatype_name_list,
				"stage" );

	google_chart_set_point_string(
				google_chart->xaxis_list,
				google_chart->google_datatype_name_list,
				"2012-05-01,stage,1.3",
				',' );

	google_chart_set_point_string(
				google_chart->xaxis_list,
				google_chart->google_datatype_name_list,
				"2012-06-01,stage,2.3",
				',' );

	google_chart_set_point_string(
				google_chart->xaxis_list,
				google_chart->google_datatype_name_list,
				"2012-07-01,stage,1.8",
				',' );

	google_chart_set_point_string(
				google_chart->xaxis_list,
				google_chart->google_datatype_name_list,
				"2012-08-01,stage,2.1",
				',' );

	google_chart_set_point_string(
				google_chart->xaxis_list,
				google_chart->google_datatype_name_list,
				"2012-09-01,stage,1.3",
				',' );

	google_chart_set_point_string(
				google_chart->xaxis_list,
				google_chart->google_datatype_name_list,
				"2012-10-01,stage,1.8",
				',' );

	google_chart_set_point_string(
				google_chart->xaxis_list,
				google_chart->google_datatype_name_list,
				"2012-11-01,stage,1.9",
				',' );

/*
	google_chart_display(	google_chart->xaxis_datatype_name,
				google_chart->xaxis_list,
				google_chart->google_datatype_name_list );
*/

	printf( "<html><body>\n" );
	google_chart_include( stdout );
	google_chart_output_draw_visualization_function(
				stdout,
				google_chart->google_chart_type,
				google_chart->xaxis_datatype_name,
				google_chart->xaxis_list,
				google_chart->google_datatype_name_list,
				"Stage 1AAA",
				(char *)0,
				google_chart->left,
				google_chart->top,
		 		google_chart->width,
		 		google_chart->height,
		 		google_chart->background_color,
				google_chart->legend_position_bottom,
				0 /* not chart_type_bar */,
				google_chart->google_package_name,
				0 /* not dont_display_range_selector */,
				daily /* aggregate_level */ );

	printf( "</body></html>\n" );
} /* test_timeline() */

