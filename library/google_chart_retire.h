/* $APPASERVER_HOME/library/google_chart_retire.h	*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef GOOGLE_CHART_RETIRE_H
#define GOOGLE_CHART_RETIRE_H

#include <stdio.h>
#include "list.h"
#include "aggregate_level.h"

/* Constants */
/* --------- */
#define GOOGLE_CHART_POSITION_TOP		10
#define GOOGLE_CHART_POSITION_LEFT		10
#define GOOGLE_CHART_WIDTH			900
#define GOOGLE_CHART_HEIGHT			600
#define GOOGLE_CHART_BACKGROUND_COLOR		"#effdff"
#define GOOGLE_CHART_CONVERT_MONTH_EXPRESSION	atoi( month ) - 1
#define GOOGLE_ANNOTATED_TIMELINE		"annotatedtimeline"
/* #define GOOGLE_CORECHART			"corechart" */
#define GOOGLE_CORECHART			"BarChart"

/* Enumerations */
/* ------------ */
enum google_chart_type{	google_column_chart,
			google_time_line,
			google_cat_whiskers };

/* Structures */
/* ---------- */
typedef struct
{
	char *v_axis_data;
	double **point_array;
} GOOGLE_DATA;

typedef struct
{
	char *column_heading;
	char *series_type;
	int min;
	int max;
	char *gridlines_color;
} GOOGLE_VAXIS_SERIES;

typedef struct
{
	char *title;
	LIST *v_axis_series_list;
	boolean two_sided;
} GOOGLE_VAXIS;

typedef struct
{
	char *title;
} GOOGLE_HAXIS;

typedef struct
{
	int chart_number;
	char *title;
	GOOGLE_HAXIS *h_axis;
	GOOGLE_VAXIS *v_axis;
	LIST *data_list;
} GOOGLE_COMBO_CHART;

typedef struct
{
	char *stratum_name;
	double **point_array;
} GOOGLE_BARCHART;

typedef struct
{
	char *date_string;
	char *time_hhmm;
	double **point_array;
} GOOGLE_TIMELINE; 

typedef struct
{
	int chart_number;
	enum google_chart_type google_chart_type;
	int left;
	int top;
	int width;
	int height;
	char *background_color;
	boolean legend_position_bottom;
	LIST *barchart_list;
	LIST *timeline_list;
	LIST *datatype_name_list;
	char *google_package_name;
	char *yaxis_label;
} GOOGLE_OUTPUT_CHART;

typedef struct
{
	char *date_time;
	double value;
	boolean null_value;
} GOOGLE_INPUT_VALUE;

typedef struct
{
	char *datatype_name;
	HASH_TABLE *value_hash_table;
} GOOGLE_UNIT_DATATYPE;

typedef struct
{
	char *unit;
	LIST *unit_datatype_list;
	DICTIONARY *date_time_dictionary;
	char *xaxis_label;
	char *yaxis_label;
	boolean bar_chart;
} GOOGLE_UNIT_CHART;

typedef struct
{
	char *datatype_name;
	LIST *input_value_list;
	char *xaxis_label;
	char *yaxis_label;
	boolean bar_chart;
} GOOGLE_DATATYPE_CHART;

typedef struct
{
	LIST *datatype_chart_list;
	LIST *unit_chart_list;
	LIST *output_chart_list;
	LIST *combo_chart_list;
	char *title;
	char *sub_title;
	char *stylesheet;
} GOOGLE_CHART;

/* Prototypes */
/* ---------- */
GOOGLE_CHART *google_chart_new(		void );

GOOGLE_INPUT_VALUE *google_chart_input_value_new(
					char *date_time );

GOOGLE_UNIT_CHART *google_unit_chart_new(
					char *unit );

GOOGLE_DATATYPE_CHART *google_datatype_chart_new(
					char *datatype_name );

GOOGLE_UNIT_DATATYPE *google_unit_datatype_new(
					char *datatype_name );

GOOGLE_OUTPUT_CHART *google_output_chart_new(
					int left,
					int top,
					int width,
					int height );

GOOGLE_BARCHART *google_barchart_new(	char *stratum_name,
					int length_datatype_name_list );

GOOGLE_TIMELINE *google_timeline_new(	char *date_string,
					char *time_hhmm,
					int length_datatype_name_list );

GOOGLE_BARCHART *google_barchart_append(
					LIST *barchart_list,
					char *stratum_name,
					int length_datatype_name_list );

GOOGLE_TIMELINE *google_timeline_append(
					LIST *timeline_list,
					char *date_string,
					char *hhmm,
					int length_datatype_name_list );

void google_barchart_set_point(		LIST *barchart_list,
					char *stratum_name,
					LIST *datatype_name_list,
					char *datatype_name,
					double point );

void google_timeline_set_point(		LIST *timeline_list,
					LIST *datatype_name_list,
					char *date_string,
					char *time_hhmm,
					char *datatype_name,
					double point );

void google_barchart_set_point_string(	LIST *barchart_list,
					LIST *datatype_name_list,
					char *delimited_string,
					char delimiter );

void google_timeline_set_point_string(	LIST *timeline_list,
					LIST *datatype_name_list,
					char *delimited_string,
					char delimiter );

GOOGLE_DATATYPE_CHART *google_datatype_chart_get_or_set(
					LIST *datatype_chart_list,
					char *datatype_name );

GOOGLE_BARCHART *google_barchart_get_or_set(
					LIST *barchart_list,
					char *stratum_name,
					int length_datatype_name_list );

GOOGLE_TIMELINE *google_timeline_get_or_set(
					LIST *timeline_list,
					char *date_string,
					char *time_hhmm,
					int length_datatype_name_list );

int google_chart_get_datatype_offset(
					LIST *datatype_name_list,
					char *datatype_name );

void google_timeline_display(		LIST *timeline_list,
					LIST *datatype_name_list );

void google_barchart_display(		LIST *barchar_list,
					LIST *datatype_name_list );

void google_chart_include(		FILE *output_file );

void google_chart_output_visualization_annotated(
				FILE *output_file,
				enum google_chart_type google_chart_type,
				LIST *timeline_list,
				LIST *barchart_list,
				LIST *datatype_name_list,
				char *google_package_name,
				enum aggregate_level,
				int chart_number,
				char *chart_title,
				char *yaxis_label );

void google_chart_output_visualization_non_annotated(
				FILE *output_file,
				enum google_chart_type google_chart_type,
				LIST *timeline_list,
				LIST *barchart_list,
				LIST *datatype_name_list,
				char *title,
				char *yaxis_label,
		 		int width,
		 		int height,
		 		char *background_color,
				boolean legend_position_bottom,
				boolean chart_type_bar,
				char *google_package_name,
				boolean dont_display_range_selector,
				enum aggregate_level,
				int chart_number );

void google_chart_anchor_chart(	FILE *output_file,
				char *title,
				char *google_package_name,
				int left,
				int top,
				int width,
				int height,
				int chart_number );

void google_barchart_append_delimited_string(
				LIST *barchart_list,
				char *comma_delimited_string,
				int length_datatype_name_list );

void google_timeline_append_delimited_string(
				LIST *timeline_list,
				char *comma_delimited_string,
				int length_datatype_name_list );

char *google_chart_convert_date(char *destination,
				char *yyyy_mm_dd,
				char *hhmm );

void google_chart_output_prompt(
				char *application_name,
				char *prompt_filename,
				char *target_window,
				char *where_clause );

void google_chart_output_options(
				FILE *output_file,
				char *title,
				enum google_chart_type google_chart_type,
				char *chart_type_string,
				boolean dont_display_range_selector,
				char *yaxis_label,
				int width,
				int height,
				char *background_color,
				char *legend_position_bottom_string );

void google_chart_output_datatype_column_heading(
				FILE *output_file,
				enum google_chart_type,
				char *first_column_datatype,
				LIST *datatype_name_list );

void google_chart_output_timeline_list(
				FILE *output_file,
				LIST *timeline_list,
				int length_datatype_name_list );

void google_chart_output_barchart_list(
				FILE *output_file,
				LIST *barchart_list,
				int length_datatype_name_list );

void google_barchart_append_datatype_name_string(
				LIST *barchart_list,
				char *datatype_name_list_string,
				int length_datatype_name_list,
				char delimiter );

void google_chart_float_chart(	FILE *output_file,
				char *title,
				int width,
				int height,
				int chart_number );

char *google_chart_get_visualization_function_name(
				int chart_number );

void google_chart_output_chart_instantiation(
				FILE *output_file,
				int chart_number );

boolean google_datatype_input_value_list_set(
				LIST *input_value_list,
				char *sys_string,
				int date_piece,
				int time_piece,
				int value_piece,
				char delimiter );

boolean google_chart_value_hash_table_set(
				HASH_TABLE *value_hash_table,
				DICTIONARY *date_time_dictionary,
				char *sys_string,
				int date_piece,
				int time_piece,
				int value_piece,
				char delimiter );

char *google_chart_get_date_time_key(
				char *date_string,
				char *time_string );

boolean google_chart_set_input_value(
				HASH_TABLE *value_hash_table,
				DICTIONARY *date_time_dictionary,
				char *date_string,
				char *time_string,
				double value,
				boolean null_value );

LIST *google_chart_datatype_get_output_chart_list(
				LIST *datatype_chart_list,
				int width,
				int height );

LIST *google_chart_unit_get_output_chart_list(
				LIST *unit_chart_list,
				int width,
				int height );

GOOGLE_OUTPUT_CHART *google_chart_unit_get_output_chart(
				LIST *datatype_list,
				LIST *date_time_key_list,
				int width,
				int height );

LIST *google_chart_get_unit_datatype_name_list(
				LIST *unit_datatype_list );

void google_chart_output_all_charts(
				FILE *output_file,
				LIST *output_chart_list,
				char *title,
				char *sub_title,
				char *stylesheet );

void google_chart_output_graph_window(
				char *application_name,
				char *appaserver_mount_point,
				boolean with_document_output,
				char *window_name,
				char *prompt_filename,
				char *where_clause );

GOOGLE_DATATYPE_CHART *google_datatype_append(
				LIST *datatype_chart_list,
				char *datatype_name );

boolean google_datatype_chart_input_value_list_set(
				LIST *input_value_list,
				char *sys_string,
				int date_piece,
				int time_piece,
				int value_piece,
				char delimiter );

char *google_datatype_chart_list_display(
				LIST *datatype_chart_list );

char *google_datatype_chart_display(
				GOOGLE_DATATYPE_CHART *datatype_chart );

char *google_chart_input_value_list_display(
				LIST *input_value_list );

double *google_point_double_calloc(
				void );

double **google_point_array_double_calloc(
				int array_length );

LIST *google_datatype_chart_get_datatype_name_list(
				LIST *datatype_chart_list );

GOOGLE_COMBO_CHART *google_combo_chart_new(
				void );

GOOGLE_HAXIS *google_haxis_new( void );

GOOGLE_VAXIS *google_vaxis_new( void );

GOOGLE_DATA *google_data_new( void );

GOOGLE_VAXIS_SERIES *google_vaxis_series_new(
				void );

#endif
