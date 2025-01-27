/* library/grace_retire.h				*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef GRACE_RETIRE_H
#define GRACE_RETIRE_H

#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "julian.h"
#include "boolean.h"

/* Enumerated types */
/* ---------------- */
enum grace_cycle_colors {
			do_cycle_colors,
			no_cycle_colors,
			no_cycle_colors_if_multiple_datatypes };

enum grace_graph_type {	unit_graph,
			datatype_overlay_graph,
			xy_graph,
			date_time_graph,
			quantum_graph };

/* Constants */
/* --------- */
/*
#define GRACE_MAX_GRAPH_LIST		10
*/
#define GRACE_LINE_COLOR_EXPRESSION	\
	(line_color % 16) ? line_color % 16 : line_color % 16 + 1
#define GRACE_DISTILL_LANDSCAPE_FLAG "-pagesize 11 8.5 in"
#define GRACE_YAXIS_MAJOR_LINESTYLE	3
#define GRACE_YAXIS_MINOR_LINESTYLE	2
#define GRACE_STARTING_LINE_COLOR	1
#define GRACE_XYHILO_SYMBOL_SIZE	0.30
#define GRACE_BAR_SYMBOL_SIZE		0.05
#define GRACE_MAX_BREAKS_NEW_DATASET	200
#define GRACE_FILENAME_STEM		"gracechart"
/*
#define GRACE_OUTPUT_FILE_TEMPLATE	"%s/%s/chart_%s.%s"
#define GRACE_FTP_OUTPUT_FILE_TEMPLATE	"%s://%s/%s/chart_%s.%s"
#define GRACE_WITHOUT_PREPEND_FTP_OUTPUT_FILE_TEMPLATE	\
					"/%s/chart_%s.%s"
#define GRACE_AGR_FILE_TEMPLATE		"%s/%s/chart_%s.agr"
#define GRACE_FTP_AGR_FILE_TEMPLATE	"%s://%s/%s/chart_%s.agr"
#define GRACE_WITHOUT_PREPEND_FTP_AGR_FILE_TEMPLATE	\
					"/%s/chart_%s.agr"
#define GRACE_POSTSCRIPT_FILE_TEMPLATE	"%s/%s/grace_%s.ps"
*/
/* #define GRACE_DEFAULT_XAXIS_TICKLABEL_ANGLE		90 */
#define GRACE_DEFAULT_XAXIS_TICKLABEL_ANGLE		0
#define GRACE_DEFAULT_YAXIS_TICKLABEL_PRECISION	 	1

#define GRACE_EXECUTABLE	"(GRACE_HOME=%s; export GRACE_HOME; LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib; export LD_LIBRARY_PATH; %s/gracebat"
/*								        ^ */
/*								        | */
/* Don't close the param because of parameters that follow.		  */
/* ---------------------------------------------------------------------- */

#define GRAPHS_UNTIL_PORTRAIT			4
#define STARTING_VIEW_MIN_X			0.100000
#define STARTING_VIEW_MIN_Y			0.050000
#define STARTING_SINGLE_VIEW_MIN_Y		0.730000
#define PORTRAIT_VIEW_X_RANGE			0.77
#define LANDSCAPE_VIEW_X_RANGE			1.10
#define DEFAULT_X_LABEL_SIZE			0.65
#define DEFAULT_Y_LABEL_SIZE			0.60
#define PORTRAIT_PAGE_WIDTH_PIXELS		612
#define PORTRAIT_PAGE_LENGTH_PIXELS		792
/* #define LANDSCAPE_PAGE_WIDTH_PIXELS		792 */
#define LANDSCAPE_PAGE_WIDTH_PIXELS		796
#define LANDSCAPE_PAGE_LENGTH_PIXELS		612
#define GRACE_DEFAULT_LEGEND_CHAR_SIZE		0.55
#define DEFAULT_TITLE_CHAR_SIZE			1.0
#define DEFAULT_SUBTITLE_CHAR_SIZE		0.80
#define DEFAULT_XAXIS_TICKLABEL_PRECISION	5

#define DEFAULT_YAXIS_TICK_PLACE	"normal"
#define DEFAULT_YAXIS_LABEL_PLACE	"normal"
#define DEFAULT_YAXIS_TICKLABEL_PLACE	"normal"
#define OPPOSITE_YAXIS_LABEL_PLACE	"opposite"
#define OPPOSITE_YAXIS_TICKLABEL_PLACE	"opposite"

#define DEFAULT_Y_INVERT_ON_OFF		"off"

/* Structures */
/* ---------- */
typedef struct
{
	double x;
	char *optional_label;
} GRACE_OPTIONAL_LABEL;

typedef struct
{
	double x;
	char *y_string;
	char *high_string;
	char *low_string;
	char *optional_label;
} GRACE_POINT;

typedef struct
{
	char *datatype_entity;
	char *datatype_name;
	LIST *dataset_list;
	char *datatype_type_bar_xy_xyhilo;
	char *legend;
	double symbol_size;
	boolean nodisplay_legend;
	int datatype_number;
	char *annotated_value_on_off;
	int line_linestyle;
	boolean inside_null;
} GRACE_DATATYPE;

typedef struct
{
	int dataset_number;
	int line_color;
	LIST *point_list;
} GRACE_DATASET;

typedef struct
{
	char *units;
	double x_tick_major;
	int x_tick_minor;
	double y_tick_major;
	int y_tick_minor;
	double view_min_x;
	double view_max_x;
	double view_min_y;
	double view_max_y;
	double world_min_y;
	double world_max_y;
	LIST *datatype_list;
	double y_label_size;
	double legend_char_size;
	char *yaxis_label_place_string;
	char *yaxis_ticklabel_place_string;
	int scale_to_zero;
	char *xaxis_ticklabel_on_off;
	char *xaxis_tickmarks_on_off;
	double legend_view_x;
	double legend_view_y;
	char *y_invert_on_off;
	char *yaxis_tick_place_string;
	char *yaxis_ticklabel_on_off;
	char *yaxis_tickmarks_on_off;
	char *xaxis_label;
	boolean xaxis_ticklabel_stagger;
	int xaxis_ticklabel_angle;
	int yaxis_ticklabel_precision;
	boolean yaxis_grid_lines;
	double horizontal_line_at_point;
} GRACE_GRAPH;

typedef struct
{
	char *datatype_entity;
	char *datatype;
	char *units;
	boolean bar_graph;
	boolean scale_graph_zero;
} GRACE_DATATYPE_OVERLAY_INPUT;

typedef struct
{
	GRACE_DATATYPE_OVERLAY_INPUT *anchor_datatype_overlay_input;
	LIST *compare_datatype_overlay_input_list;
} GRACE_DATATYPE_OVERLAY_INPUT_GROUP;

typedef struct
{
	enum grace_graph_type grace_graph_type;
	GRACE_DATATYPE_OVERLAY_INPUT_GROUP *grace_datatype_overlay_input_group;
	JULIAN *begin_date_julian, *end_date_julian;
	char *title;
	char *sub_title;
	char *xaxis_ticklabel_format;
	double x_label_size;
	LIST *anchor_graph_list;
	LIST *graph_list;
	char *agr_filename;
	char *ftp_agr_filename;
	char *postscript_filename;
	char *output_filename;
	char *ftp_output_filename;
	int number_of_days;
	double world_min_x;
	double world_max_x;
	int xaxis_ticklabel_precision;
	boolean datatype_type_xyhilo;
	boolean symbols;
	char *grace_output;
	int datatype_entity_piece;
	int datatype_piece;
	int date_piece;
	int time_piece;
	int value_piece;
	int page_width_pixels;
	int page_length_pixels;
	char *distill_landscape_flag;
	boolean dataset_no_cycle_color;
	boolean landscape_mode;
} GRACE;

/* Prototypes */
/* ---------- */
void grace_date_time_set_data(	LIST *graph_list,
				char *date_data,
				char *time_data,
				char *value_data,
				boolean dataset_no_cycle_color );

void grace_date_time_set_attribute_name(
				LIST *graph_list,
				char *float_integer_attribute_name );

GRACE *grace_new_date_time_grace(
				char *application_name,
				char *role_name,
				char *title,
				char *sub_title );

GRACE_POINT *grace_point_new(	double x,
				char *y_string,
				char *low_string,
				char *high_string,
				char *optional_label );

GRACE *grace_new_unit_graph_grace(
				char *application_name,
				char *role_name,
				char *infrastructure_process,
				char *data_process,
				char *argv_0,
				int datatype_entity_piece,
				int datatype_piece,
				int date_piece,
				int time_piece,
				int value_piece,
				char *title,
				char *sub_title,
				boolean datatype_type_xyhilo,
				enum grace_cycle_colors cycle_colors );

GRACE_GRAPH *grace_new_grace_graph( void );

void grace_set_point(		boolean *inside_null,
				LIST *dataset_list,
				double x,
				char *y_string,
				char *optional_label,
				int datatype_number,
				boolean dataset_no_cycle_color );

void grace_set_view_minimum( 	GRACE_GRAPH *graph, double x, double y );
void grace_set_view_maximum( 	GRACE_GRAPH *graph, double x, double y );
void grace_set_title( 		GRACE *grace, char *title );
void grace_graph_set_title(	GRACE_GRAPH *graph, char *title );
void grace_set_x_tick_major(	GRACE_GRAPH *graph, double x_tick_major );
void grace_set_x_tick_minor(	GRACE_GRAPH *graph, int x_tick_minor );
void grace_set_y_tick_major(	GRACE_GRAPH *graph, double y_tick_major );
void grace_set_y_tick_minor(	GRACE_GRAPH *graph, int y_tick_minor );

boolean grace_convert_postscript_to_pdf( 
				char *pdf_filename, 
				char *postscript_filename,
				char *distill_directory,
				char *distill_landscape_flag,
				char *ghost_script_directory );

FILE *grace_open_output_pipe(
				char *output_filename,
				char *postscript_filename,
				char *agr_filename,
				char *grace_home_directory,
				char *grace_execution_directory,
				boolean grace_free_option,
				char *grace_output );

boolean grace_output_point_list(
				FILE *output_pipe,
				int datatype_number,
				LIST *point_list,
				int graph_number,
				char *datatype_type_bar_xy_xyhilo );

int grace_set_structures(	int *page_width_pixels,
				int *page_length_pixels,
				char **distill_landscape_flag,
				boolean *landscape_mode,
				GRACE *grace,
				LIST *graph_list,
				LIST *anchor_graph_list,
				JULIAN *begin_date_julian,
				JULIAN *end_date_julian,
				int number_of_days,
				enum grace_graph_type grace_graph_type,
				boolean force_landscape_mode );

void grace_graph_set_scale_to_zero(
				GRACE_GRAPH *grace_graph,
			    	int scale_to_zero );
double grace_get_maximum_y_double(
				LIST *datatype_list );
double grace_get_minimum_y_double(
				LIST *datatype_list );
double grace_get_lowest_first_x_double(
				LIST *datatype_list );
double grace_get_highest_last_x_double(	
				LIST *datatype_list );
void grace_output_full_dataset_heading(
				FILE *output_pipe,
			   	int line_color,
			   	int dataset_number,
				char *legend,
				int line_linestyle,
				char *annotated_value_on_off,
				boolean symbols,
				char *datatype_type_bar_xy_hilo,
				double symbol_size );
void grace_output_abbreviated_dataset_heading(	
				FILE *output_pipe,
			   	int dataset_number,
			   	int line_color,
			   	int line_linestyle,
				boolean symbols,
				double symbol_size,
				char *datatype_type_bar_xy_xyhilo,
				char *legend );

void grace_output_top_heading(	FILE *output_pipe,
				int page_width_pixels,
				int page_length_pixels );

void grace_output_graph_heading(FILE *output_pipe,
				GRACE_GRAPH *graph,
				int graph_number,
				char *title,
				char *sub_title,
				char *xaxis_ticklabel_format,
				double x_label_size,
				double legend_char_size,
				char *units,
				double y_label_size,
				char *yaxis_label_place_string,
				char *yaxis_ticklabel_place_string,
				char *xaxis_ticklabel_on_off,
				char *xaxis_tickmarks_on_off,
				double legend_view_x,
				double legend_view_y,
				char *y_invert_on_off,
				char *yaxis_tick_place,
				double world_min_x,
				double world_max_x,
				double title_char_size,
				double subtitle_char_size,
				int xaxis_ticklabel_precision,
				char *xaxis_label,
				boolean yaxis_grid_lines,
				LIST *optional_label_list,
				boolean xaxis_ticklabel_stagger,
				double world_min_y,
				double world_max_y,
				int xaxis_ticklabel_angle,
				int yaxis_ticklabel_precision,
				char *yaxis_ticklabel_on_off,
				char *yaxis_tickmarks_on_off );

void grace_get_world_minimum_x(
				double *world_min_x,
				JULIAN *begin_date_julian,
				int number_of_days,
				int decrement_x_axis_for_bar );

void grace_get_world_minimum_y(
				double *world_min_y,
				double y );

void grace_get_world_maximum_x(
				double *world_max_x,
				JULIAN *end_date_julian );

void grace_get_world_maximum_y(
				double *world_max_y,
				JULIAN *end_date_julian,
				double y );

void grace_append_graph(	LIST *graph_list, GRACE_GRAPH *graph );

void grace_get_x_tick_major_minor(
				double *x_tick_major,
				int *x_tick_minor,
				int number_of_days );

void grace_get_y_tick_major_minor(		
				double *y_tick_major,
				int *y_tick_minor,
				int *yaxis_ticklabel_precision,
				double range,
				int graph_list_length,
				double world_max_y );

int grace_remove_empty_graphs(	LIST *graph_list,
				LIST *anchor_graph_list,
				enum grace_graph_type );

boolean grace_output_to_file(
				char *output_filename,
				char *postscript_filename,
				char *agr_filename,
				char *title,
				char *sub_title,
				char *xaxis_ticklabel_format,
				enum grace_graph_type grace_graph_type,
				double x_label_size,
				int page_width_pixels,
				int page_length_pixels,
				char *grace_home_directory,
				char *grace_execution_directory,
				boolean grace_free_option,
				char *grace_output,
				LIST *optional_label_list,
				boolean symbols,
				LIST *graph_list,
				LIST *anchor_graph_list,
				int xaxis_ticklabel_precision,
				double world_min_x,
				double world_max_x );

GRACE_DATATYPE *grace_new_grace_datatype( 
				char *datatype_entity,
				char *datatype_name );

double grace_get_number_x_ticks_between_labels(
				LIST *datatype_list,
				int label_count );

double get_average_difference_double( 
				LIST *double_list );

void grace_output_datatype(
				FILE *output_pipe,
				GRACE_DATATYPE *datatype,
				int graph_number,
				char *legend,
				boolean symbols );

int grace_populate_unit_graph_list(
				LIST *graph_list,
				char *argv_0,
				char *infrastructure_process,
				boolean datatype_type_xyhilo );

GRACE_DATATYPE *grace_get_grace_datatype(
				GRACE_GRAPH **return_graph,
				LIST *graph_list,
				char *datatype_entity,
				char *datatype_name,
				enum grace_graph_type );

void grace_set_aggregate_variables(
				GRACE *grace,
				JULIAN *begin_date_julian,
				JULIAN *end_date_julian,
				LIST *graph_list,
				int number_of_days,
				enum grace_graph_type grace_graph_type,
				double view_min_y,
				double view_y_offset,
				double view_y_increment,
				double view_x_range,
				int landscape_mode,
				boolean is_datatype_overlay_anchor );

void grace_get_portrait_view_y_minimum_offset_and_increment(
				double *view_min_y,
				double *view_y_offset,
				double *view_y_increment,
				double starting_single_view_min_y,
				double starting_view_min_y,
				int graph_list_length );

void grace_get_landscape_view_y_minimum_offset_and_increment(
				double *view_min_y,
				double *view_y_offset,
				double *view_y_increment,
				double starting_single_view_min_y,
				double starting_view_min_y,
				int graph_list_length );

void grace_graph_set_graph_type(GRACE_GRAPH *graph,
				char *graph_type );

void grace_populate_point_list( LIST *graph_list, 
				char *data_process,
				int datatype_entity_piece,
				int datatype_piece,
				int date_piece,
				int time_piece,
				int value_piece,
				enum grace_graph_type grace_graph_type,
				boolean datatype_type_xyhilo,
				boolean dataset_no_cycle_color );

boolean grace_parse_date_hh_colon_mm(
				int *hour,
				int *minute,
				char *date_time_string );

boolean grace_parse_time_hh_colon_mm(
				int *hour,
				int *minute,
				char *time );

boolean grace_parse_time_hhmm( 	int *hour,
				int *minute,
				char *time );

boolean grace_parse_date_hhmm(
				int *hour,
				int *minute,
				char *date_time_string );

boolean grace_parse_date_yyyy_mm_dd(int *year,
		    		int *month,
		    		int *day,
		    		char *date );

boolean grace_set_string_to_point_list(
				LIST *graph_list, 
				int datatype_entity_piece,
				int datatype_piece,
				int date_piece,
				int time_piece,
				int value_piece,
				char *delimited_string,
				enum grace_graph_type,
				boolean datatype_type_xyhilo,
				boolean dataset_no_cycle_color,
				char *optional_label );

void grace_set_omit_output_datatype_entity(
				GRACE *grace );

int grace_set_begin_end_date(	GRACE *grace,
				char *begin_date_string,
				char *end_date_string );

double grace_get_legend_char_size(
				LIST *datatype_list,
				enum grace_graph_type grace_graph_type );

GRACE *grace_new_datatype_overlay_grace(
				char *application_name,
				char *role_name );

void grace_output_data(		FILE *output_pipe,
				LIST *graph_list, 
				char *title,
				char *sub_title,
				char *xaxis_ticklabel_format,
				double x_label_size,
				enum grace_graph_type grace_graph_type,
				int xaxis_ticklabel_precision,
				LIST *optional_label_list,
				boolean symbols,
				double world_min_x,
				double world_max_x,
				boolean is_datatype_overlay_anchor_graph );

void grace_set_compare_datatype_overlay_input(
				LIST *compare_datatype_overlay_input_list,
				char *datatype_entity,
				char *datatype,
				char *units,
				boolean bar_graph,
				boolean scale_graph_zero );

GRACE_DATATYPE_OVERLAY_INPUT *grace_new_grace_datatype_overlay_input(
				char *datatype_entity,
				char *datatype,
				char *units,
				boolean bar_graph,
				boolean scale_graph_zero );

GRACE_DATATYPE_OVERLAY_INPUT_GROUP *
			grace_new_grace_datatype_overlay_input_group(
				char *anchor_datatype_entity,
				char *anchor_datatype,
				char *units,
				boolean bar_graph,
				boolean scale_graph_zero );

GRACE_DATATYPE *grace_graph_get_grace_datatype(
				LIST *datatype_list,
				char *datatype_entity,
				char *datatype_name,
				enum grace_graph_type grace_graph_type );

void grace_populate_datatype_overlay_graph_list(
				LIST *graph_list,
				LIST *anchor_graph_list,
				GRACE_DATATYPE_OVERLAY_INPUT *
					anchor_datatype_overlay_input,
				LIST *compare_datatype_overlay_input_list );

char *grace_datatype_list_get_datatype_type_bar_xy_xyhilo(
				LIST *datatype_list );

double grace_get_x_label_size( 	int number_of_days,
				int length_graph_list );

void grace_make_same_units_have_same_y_tick_marks(
				LIST *graph_list,
				LIST *anchor_graph_list );

void grace_get_widest_world_y(	double *new_world_min_y,
				double *new_world_max_y,
				double graph_world_min_y,
				double graph_world_max_y,
				double anchor_graph_world_min_y,
				double anchor_graph_world_max_y );

void grace_set_landscape(	int *page_width_pixels,
				int *page_length_pixels,
				char **distill_landscape_flag,
				double *starting_single_view_min_y,
				double *view_x_range );

boolean grace_graph_datatype_populated(
				LIST *datatype_list );

void grace_overlay_graph_remove_same_datatype(
				LIST *graph_list,
				LIST *anchor_graph_list );

GRACE *grace_new_xy_grace(	char *application_name,
				char *role_name,
				char *title,
				char *sub_title,
				char *units,
				char *legend );

GRACE *grace_new_quantum_grace(	char *application_name,
				char *role_name,
				char *title,
				char *sub_title );

void grace_set_xy_to_point_list(
				LIST *graph_list, 
				double x,
				char *y_string,
				char *optional_label,
				boolean dataset_no_cycle_color );

GRACE *grace_new_grace(		char *application_name,
	       			char *role_name );

double grace_get_x_range(	double world_min_x,
			  	double world_max_x );

int grace_graph_list_empty(	LIST *graph_list );

void grace_adjust_for_negative_y_max(
				double *world_min_y,
				double *world_max_y );

boolean grace_output_charts(	char *output_filename, 
				char *postscript_filename,
				char *agr_filename,
				char *title,
				char *sub_title,
				char *xaxis_ticklabel_format,
				enum grace_graph_type grace_graph_type,
				double x_label_size,
				int page_width_pixels,
				int page_length_pixels,
				char *grace_home_directory,
				char *grace_execution_directory,
				boolean grace_free_option,
				char *grace_output,
				char *distill_directory,
				char *distill_landscape_flag,
				char *ghost_script_directory,
				LIST *optional_label_list,
				boolean symbols,
				double world_min_x,
				double world_max_x,
				int xaxis_ticklabel_precision,
				LIST *graph_list,
				LIST *anchor_graph_list );

void grace_graph_set_global_world_max_y(
				LIST *graph_list,
				double ymax );

void grace_graph_set_global_world_min_y(
				LIST *graph_list,
				double world_min_y );

void grace_graph_set_global_y_tick_minor(
				LIST *graph_list,
				int y_tick_minor );

void grace_graph_set_global_y_tick_major(
				LIST *graph_list,
				double y_tick_major );

char *grace_get_agr_prompt(	void );

void grace_output_grace_home_link(
				void );

void grace_output_optional_label_list(
				FILE *output_pipe,
				LIST *optional_label_list );

LIST *grace_get_optional_label_list(
				LIST *graph_list );

void grace_email_graph(		char *application_name,
				char *email_address,
				char *chart_email_command_line,
				char *pdf_jpg_filename,
				boolean with_document_output,
				char *where_clause );

void grace_output_graph_window(	char *application_name,
				char *pdf_output_filename,
				char *ftp_agr_filename,
				boolean with_document_output,
				char *where_clause );

void grace_error_exit(			char *function_name,
					int line_number );

void grace_set_world_min_y_based_on_grace_point_low(
					LIST *graph_list );

void grace_set_world_max_y_based_on_grace_point_high(
					LIST *graph_list );

void grace_set_world_max_x_based_on_grace_point_high(
					double *world_max_x,
					LIST *graph_list );

void grace_set_world_min_x_based_on_grace_point_low(
					double *world_min_x,
					LIST *graph_list );

double grace_get_minimum_x_from_point_low(
					LIST *datatype_list );

double grace_get_minimum_y_from_point_low(
					LIST *datatype_list );

double grace_get_maximum_y_from_point_high(
					LIST *datatype_list );

double grace_get_maximum_x_from_point_high(
					LIST *datatype_list );

void grace_get_filenames(		char **agr_filename,
					char **ftp_agr_filename,
					char **postscript_filename,
					char **output_filename,
					char **pdf_output_filename,
					char *application_name,
					char *document_root_directory,
					char *graph_identifier,
					char *grace_output );

boolean grace_get_is_null(		char *value_string );

void grace_set_bar_graph(		LIST *graph_list );
void grace_set_scale_to_zero(		LIST *graph_list );

void grace_set_xaxis_ticklabel_angle(	LIST *graph_list,
					int xaxis_ticklabel_angle );

void grace_set_yaxis_ticklabel_precision(
					LIST *graph_list,
					int yaxis_ticklabel_precision );

void grace_set_symbol_size(		LIST *graph_list,
					double symbol_size );

void grace_set_x_minor_tickmarks_to_zero(
					LIST *graph_list );

void grace_set_y_minor_tickmarks_to_zero(
					LIST *graph_list );

GRACE_OPTIONAL_LABEL *grace_new_optional_label(
					double grace_point_x,
					char *optional_label );

void grace_set_xaxis_tickmarks_off(	LIST *graph_list );

boolean grace_no_legend_set(		LIST *grace_graph );

void grace_lower_legend(		LIST *graph_list,
					double pixels );
void grace_graph_make_yaxis_opposite(	GRACE_GRAPH *grace_graph );

void grace_set_first_graph_top_of_second(
					LIST *graph_list );

void grace_move_legend_bottom_left(	GRACE_GRAPH *grace_graph,
					boolean landscape_mode );

void grace_move_legend_bottom_right(	GRACE_GRAPH *grace_graph,
					boolean landscape_mode );

void grace_graph_increment_line_color(	LIST *datatype_list );

void grace_graph_set_no_yaxis_grid_lines_offset(
					LIST *graph_list,
					int graph_offset );

boolean grace_all_datatype_nodisplay_legend(
					LIST *datatype_list );

void grace_set_yaxis_tickmajor_integer_ceiling(
					LIST *graph_list );

void grace_graph_set_no_yaxis_grid_lines(
					LIST *graph_list );

void grace_increase_legend_char_size(
					GRACE_GRAPH *grace_graph,
					double increase_amount );

void grace_set_view_maximum_x(		GRACE_GRAPH *graph, double x );

GRACE_POINT *grace_new_grace_point(	double x,
					char *y_string,
					char *optional_label );

GRACE_GRAPH *grace_graph_new(		void );

boolean grace_is_null(			char *y_string );

boolean grace_contains_multiple_datatypes(
					LIST *graph_list );
GRACE_DATASET *grace_new_grace_dataset(
					int datatype_number,
					boolean dataset_no_cycle_color );

double grace_get_symbol_size(		double symbol_size,
					boolean symbols,
					char *datatype_type_bar_xy_xyhilo );
void grace_set_to_point_list(
					GRACE_DATATYPE *grace_datatype, 
					double x,
					char *y_string,
					char *optional_label,
					boolean dataset_no_cycle_color );

void grace_move_legend_bottom_center(
					double *legend_view_x,
					double *legend_view_y,
					double view_min_x,
					double view_min_y,
					boolean landscape_mode );

void grace_set_line_linestyle( 		LIST *datatype_list,
					int line_linestyle );

void grace_output_screen_title(		void );

void grace_output_horizontal_line_at_point(
					FILE *output_pipe,
					double horizontal_line_at_point );

void grace_datatype_set_legend(		LIST *datatype_list,
					int datatype_number,
					char *legend );

GRACE_DATATYPE *grace_datatype_seek(	LIST *datatype_list,
					int datatype_number );

GRACE_DATATYPE *grace_get_or_set_datatype(
					LIST *datatype_list,
					int datatype_number );

void grace_set_datatype_number_point(
				GRACE_GRAPH *grace_graph,
				double x,
				char *y_string,
				int datatype_number );

void grace_move_legend_up(	double *legend_view_y,
				double up_increment );

void grace_datatype_list_set_datatype_type_bar_xy_xyhilo(
				LIST *graph_list,
				char *datatype_type_bar_xy_xyhilo );

#endif
