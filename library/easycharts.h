/* library/easycharts.h */
/* -------------------- */

#ifndef EASYCHARTS_H
#define EASYCHARTS_H

#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "hash_table.h"
#include "dictionary.h"
#include "boolean.h"

/* Constants */
/* --------- */
#define EASYCHARTS_EXTENDED_JAR_FILE	"chart.ext.jar"
#define EASYCHARTS_JAR_FILE		"chart.jar"

#define EASYCHARTS_APPLET_LIBRARY_LINE_CHART	 		"com.objectplanet.chart.LineChartApplet"
/* #define EASYCHARTS_APPLET_LIBRARY_LINE_CHART	 		"com.objectplanet.chart.ChartApplet" */

#define EASYCHARTS_APPLET_LIBRARY_BAR_CHART	 		"com.objectplanet.chart.BarChartApplet"

#define EASYCHARTS_APPLET_LIBRARY_TIMELINE	 		"com.objectplanet.chart.ext.TimeLineChartApplet"

#define EASYCHARTS_COLOR_LIST_STRING 		 "aqua,red,green,gray,blue,olive,maroon,navy,lime,purple,olive,fuchsia,green,yellow,maroon"

#define EASYCHARTS_MAX_VALUE		9999999
/*
#define EASYCHARTS_WIDTH		980
#define EASYCHARTS_HEIGHT		500
#define EASYCHARTS_WIDTH		750
#define EASYCHARTS_HEIGHT		450
#define EASYCHARTS_WIDTH		800
#define EASYCHARTS_HEIGHT		475
*/
#define EASYCHARTS_WIDTH		980
#define EASYCHARTS_HEIGHT		500
#define POINT_HIGHLIGHT_SIZE		5;
#define EASYCHARTS_FILENAME_STEM	"easycharts"
/*
#define EASYCHARTS_CHART_TEMPLATE	"%s/%s/chart_%d.html"
#define EASYCHARTS_PROMPT_TEMPLATE	"/%s/chart_%d.html"
#define EASYCHARTS_HTTP_PROMPT_TEMPLATE	"%s://%s/%s/chart_%d.html"
*/

/* Structures */
/* ---------- */
typedef struct
{
	double x_value;
	double y_value;
	char *x_label;
	boolean display_value;
} EASYCHARTS_POINT;

typedef struct
{
	char *date_time;
	double value;
	boolean null_value;
} EASYCHARTS_INPUT_VALUE;

typedef struct
{
	LIST *datatype_list;
	boolean bar_labels_on;
	boolean bar_chart;
	char *applet_library_code;
	char *x_axis_label;
	char *y_axis_label;
	DICTIONARY *date_time_dictionary;
	boolean double_range_adjusters;
} EASYCHARTS_INPUT_CHART;

typedef struct
{
	char *datatype_name;
	HASH_TABLE *value_hash_table;
	char *units;
} EASYCHARTS_INPUT_DATATYPE;

typedef struct
{
	char *datatype_name;
	char *units;
	LIST *point_list;
} EASYCHARTS_OUTPUT_DATATYPE;

typedef struct
{
	LIST *output_datatype_list;
	boolean bar_labels_on;
	boolean bar_chart;
	char *applet_library_code;
	char *x_axis_label;
	char *y_axis_label;
	boolean double_range_adjusters;
} EASYCHARTS_OUTPUT_CHART;

typedef struct
{
	LIST *output_chart_list;
	LIST *input_chart_list;
	EASYCHARTS_INPUT_DATATYPE *x_axis_datatype;
	char *applet_library_archive;
	int width;
	int height;
	char *title;
	boolean value_labels_on;
	boolean sample_scroller_on;
	boolean range_scroller_on;
	int xaxis_decimal_count;
	int yaxis_decimal_count;
	boolean range_labels_off;
	boolean value_lines_off;
	int range_step;
	int sample_label_angle;
	boolean legend_on;
	boolean bold_labels;
	boolean bold_legends;
	boolean set_y_lower_range;
	int font_size;
	char *label_parameter_name;
	boolean highlight_on;
	char *highlight_style;
	boolean series_line_off;
	int point_highlight_size;
	char *series_labels;
	char *chart_filename;
	char *prompt_filename;
} EASYCHARTS;

/* Prototypes */
/* ---------- */
EASYCHARTS *easycharts_new_easycharts(
				int width,
				int height );
EASYCHARTS *easycharts_new_timeline_easycharts(
				int width,
				int heigth );
EASYCHARTS *easycharts_new_scatter_easycharts(
				int width,
				int height );
EASYCHARTS_POINT *easycharts_new_point(
				double x_value,
				double y_value,
				char *x_label );
int easycharts_output_chart(
				FILE *destination,
				LIST *output_datatype_list,
				int highlight_on,
				char *highlight_style,
				int point_highlight_size,
				char *series_labels,
				int series_line_off,
				char *applet_library_code,
				char *applet_library_archive,
				int width,
				int height,
				char *title,
				char *x_axis_label,
				char *y_axis_label,
				boolean set_y_lower_range,
				boolean legend_on,
				boolean value_labels_on,
				boolean sample_scroller_on,
				boolean range_scroller_on,
				int xaxis_decimal_count,
				int yaxis_decimal_count,
				boolean range_labels_off,
				boolean value_lines_off,
				int range_step,
				int sample_label_angle,
				boolean bold_labels,
				boolean bold_legends,
				int font_size,
				boolean bar_labels_on,
				char *label_parameter_name,
				boolean include_sample_values_output,
				boolean bar_chart,
				boolean double_range_adjusters );

void easycharts_output_point_list(
				FILE *destination,
				LIST *point_list,
				boolean output_x_axis );
void easycharts_output_point_list(
				FILE *destination,
				LIST *point_list,
				boolean output_x_axis );

void easycharts_get_chart_filename(
				char **chart_filename,
				char **prompt_filename,
				char *application_name,
				char *document_root_directory,
				int pid );

EASYCHARTS_INPUT_VALUE *easycharts_new_input_value(
				char *date_time,
				double value,
				boolean null_value );
int easycharts_point_compare(	EASYCHARTS_POINT *point1,
				EASYCHARTS_POINT *point2 );
int easycharts_datatype_compare(EASYCHARTS_INPUT_DATATYPE *datatype,
				char *compare_datatype_name );
int easycharts_get_y_minimum_integer(
				LIST *datatype_list );
int easycharts_get_rounded_max_y( LIST *point_list );
int easycharts_get_rounded_min_y( LIST *datatype_list );
/*
EASYCHARTS_OUTPUT_CHART *easycharts_xy_get_output_chart(
				LIST *y_axis_datatype_list,
				LIST *timeline_key_list );
*/
LIST *easycharts_timeline_get_output_chart_list(
				LIST *input_chart_list );
EASYCHARTS_OUTPUT_CHART *easycharts_timeline_get_output_chart(
				LIST *datatype_list,
				LIST *date_time_key_list );
LIST *easycharts_timeline_get_x_axis_point_list(
				DICTIONARY *label_key_dictionary );
EASYCHARTS_OUTPUT_CHART *easycharts_new_output_chart(
				void );
void easycharts_output_y_minimum_integer(
				FILE *destination,
				int y_minimum_integer );
LIST *easycharts_get_series_labels_list(
				LIST *output_datatype_list );
int easycharts_get_yaxis_decimal_count(
				LIST *output_chart_list );
int easycharts_get_range_step(
				LIST *output_chart_list );
double easycharts_get_y_point_average(
				LIST *output_chart_list );
double easycharts_get_y_point_range(
				LIST *output_chart_list );
int easycharts_get_point_count(
				LIST *datatype_list );
void easycharts_trim_label_year_to_two(
				LIST *output_chart_list );
LIST *easycharts_scatter_get_point_list(
				EASYCHARTS_INPUT_DATATYPE *datatype_1,
				EASYCHARTS_INPUT_DATATYPE *datatype_2,
				LIST *date_time_key_list );
void easycharts_output_html(	FILE *chart_file );
void easycharts_set_printer_sized(
				int *height,
				int *width,
				int *font_size,
				boolean *bold_labels,
				boolean *bold_legends );
void easycharts_set_half_height(
				int *height,
				char printer_sized_yn );
/*
char *easycharts_xy_input_dataset_list_display(
				LIST *input_dataset_list,
				DICTIONARY *label_key_dictionary );
DICTIONARY *easycharts_remove_key_if_any_missing_key_dictionary(
				LIST *input_dataset_list,
				DICTIONARY *label_key_dictionary );
EASYCHARTS_TIMELINE_INFO *easycharts_new_timeline_info(
				char *time_format_input,
				char *x_axis_ticklabel_format,
				char *lowest_date_string,
				char *highest_date_string );
LIST *easycharts_xy_get_output_chart_list(
				LIST *input_dataset_list,
				DICTIONARY *label_key_dictionary );
*/
DICTIONARY *easycharts_remove_key_if_any_missing_key_dictionary(
				LIST *input_datatype_list,
				DICTIONARY *date_time_dictionary );
LIST *easycharts_scatter_get_output_chart_list(
				LIST *input_chart_list );
EASYCHARTS_OUTPUT_CHART *easycharts_scatter_get_output_chart(
				LIST *datatype_list,
				LIST *date_time_key_list,
				char *input_chart_y_axis_label );
boolean easycharts_output_all_charts(
				FILE *destination,
				LIST *output_chart_list,
				int highlight_on,
				char *highlight_style,
				int point_highlight_size,
				char *series_labels,
				int series_line_off,
				char *applet_library_archive,
				int width,
				int height,
				char *title,
				boolean set_y_lower_range,
				boolean legend_on,
				boolean value_labels_on,
				boolean sample_scroller_on,
				boolean range_scroller_on,
				int xaxis_decimal_count,
				int yaxis_decimal_count,
				boolean range_labels_off,
				boolean value_lines_off,
				int range_step,
				int sample_label_angle,
				boolean bold_labels,
				boolean bold_legends,
				int font_size,
				char *label_parameter_name,
				boolean include_sample_values_output );

/*
char *easycharts_scatter_get_y_axis_label(
				LIST *output_chart_list );
void easycharts_display_point_list(
				FILE *output_stream,
				LIST *point_list );
*/
LIST *easycharts_get_regression_point_list(
				double *slope,
				double *y_intercept,
				double *standard_error_estimate,
				LIST *point_list );
void easycharts_output_home_link(
				void );
void easycharts_output_graph_window(
				char *application_name,
				char *appaserver_mount_point,
				boolean with_document_output,
				char *window_name,
				char *prompt_filename,
				char *where_clause );
boolean easycharts_set_input_value(
				LIST *input_chart_list,
				char *datatype_name,
				char *date_time,
				double value,
				boolean null_value );
LIST *easycharts_new_datatype_list(
				LIST *datatype_name_list,
				LIST *datatype_units_list );
EASYCHARTS_INPUT_DATATYPE *easycharts_new_input_datatype(
				char *datatype_name,
				char *units );
boolean easycharts_set_all_input_values(
				LIST *input_chart_list,
				char *input_sys_string,
				int datatype_piece,
				int date_time_piece,
				int value_piece,
				char delimiter );
EASYCHARTS_OUTPUT_DATATYPE *easycharts_new_output_datatype(
				char *datatype_name,
				char *units );
EASYCHARTS_INPUT_CHART *easycharts_new_input_chart(
				void );
char *easycharts_output_chart_list_display(
				LIST *output_chart_list );
boolean easycharts_set_point(	LIST *point_list,
				double x_value,
				double y_value,
				char *x_label,
				boolean display_value );

char *easycharts_series_label_convert_comma(
				char *series_label );

#endif
