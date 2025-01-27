/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/grace.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef GRACE_H
#define GRACE_H

#include <sys/types.h>
#include "boolean.h"
#include "list.h"
#include "query.h"
#include "appaserver_link.h"
#include "histogram.h"

#define GRACE_FILENAME_STEM			"grace"
#define GRACE_VIEW_MIN_X			0.1
#define GRACE_VIEW_PORTRAIT_X_OFFSET		0.87
#define GRACE_VIEW_LANDSCAPE_X_OFFSET		1.1

typedef struct
{
	double min_x;
	double max_x;
	double min_y;
	double max_y;
	double y_increment;
	double y_offset;
	double x_tick_major;
	int x_tick_minor;
} GRACE_VIEW;

/* Usage */
/* ----- */
GRACE_VIEW *grace_view_new(
		double grace_view_min_x,
		double grace_view_portrait_x_offset,
		double grace_view_landscape_x_offset,
		int grace_graph_list_length,
		int horizontal_range_integer,
		boolean landscape_boolean );

/* Process */
/* ------- */
GRACE_VIEW *grace_view_calloc(
		void );

double grace_view_max_x(
		double grace_view_min_x,
		double grace_view_portrait_x_offset,
		double grace_view_landscape_x_offset,
		boolean landscape_boolean );

double grace_view_max_y(
		double grace_view_min_y,
		double grace_view_y_offset );

/* Usage */
/* ----- */
double grace_view_min_y(
		int grace_graph_list_length );

/* Usage */
/* ----- */
double grace_view_y_increment(
		int grace_graph_list_length,
		boolean landscape_boolean );

/* Usage */
/* ----- */
double grace_view_y_offset(
		int grace_graph_list_length,
		boolean landscape_boolean );

/* Usage */
/* ----- */
double grace_view_x_tick_major(
		int horizontal_range_integer );

/* Usage */
/* ----- */
int grace_view_x_tick_minor(
		int horizontal_range_integer );

/* Usage */
/* ----- */
GRACE_VIEW *grace_view_increment(
		GRACE_VIEW *grace_view );

/* Driver */
/* ------ */
void grace_view_output(
		GRACE_VIEW *grace_view,
		FILE *output_file );

typedef struct
{
	double horizontal_double;
	double vertical_double;
} GRACE_POINT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GRACE_POINT *grace_point_new(
		double horizontal_double,
		double vertical_double );

/* Process */
/* ------- */
GRACE_POINT *grace_point_calloc(
		void );

/* Usage */
/* ----- */
double grace_point_minimum_horizontal(
		LIST *grace_point_list );

/* Usage */
/* ----- */
double grace_point_maximum_horizontal(
		LIST *grace_point_list );

/* Usage */
/* ----- */
double grace_point_minimum_vertical(
		LIST *grace_point_list );

/* Usage */
/* ----- */
double grace_point_maximum_vertical(
		LIST *grace_point_list );

/* Usage */
/* ----- */
GRACE_POINT *grace_point_delimited_parse(
		char delimiter,
		int date_piece,
		int time_piece,
		int value_piece,
		char *delimited_row );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *grace_point_date_string(
		char delimiter,
		int date_piece,
		char *delimited_row );

/* Returns static memory */
/* --------------------- */
char *grace_point_time_string(
		char delimiter,
		int time_piece,
		char *delimited_row );

boolean grace_point_value_null_boolean(
		char delimiter,
		int value_piece,
		char *delimited_row );

double grace_point_value_double(
		char delimiter,
		int value_piece,
		char *delimited_row );

/* Driver */
/* ------ */
void grace_point_list_output(
		LIST *grace_point_list,
		FILE *output_file );

/* Process */
/* ------- */
void grace_point_output(
		GRACE_POINT *grace_point,
		FILE *output_file );

/* Usage */
/* ----- */
double grace_point_horizontal_range_double(
		LIST *grace_point_list );

/* Usage */
/* ----- */
double grace_point_vertical_range_double(
		LIST *grace_point_list );

typedef struct
{
	double horizontal_double;
	double low_value;
	double average_value;
	double high_value;
} GRACE_WHISKER_POINT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GRACE_WHISKER_POINT *grace_whisker_point_new(
		double horizontal_double,
		double low_value,
		double average_value,
		double high_value );

/* Process */
/* ------- */
GRACE_WHISKER_POINT *grace_whisker_point_calloc(
		void );

/* Usage */
/* ----- */
double grace_whisker_point_minimum_horizontal(
		LIST *grace_whisker_point_list );

/* Usage */
/* ----- */
double grace_whisker_point_maximum_horizontal(
		LIST *grace_whisker_point_list );

/* Usage */
/* ----- */
double grace_whisker_point_minimum_vertical(
		LIST *grace_whisker_point_list );

/* Usage */
/* ----- */
double grace_whisker_point_maximum_vertical(
		LIST *grace_whisker_point_list );

/* Usage */
/* ----- */
GRACE_WHISKER_POINT *grace_whisker_point_delimited_parse(
		char delimiter,
		int date_piece,
		int time_piece,
		int low_value_piece,
		int average_value_piece,
		int high_value_piece,
		char *delimited_row );

/* Driver */
/* ------ */
void grace_whisker_point_list_output(
		LIST *grace_whisker_point_list,
		FILE *output_file );

/* Process */
/* ------- */
void grace_whisker_point_output(
		GRACE_WHISKER_POINT *grace_whisker_point,
		FILE *output_file );

enum grace_dataset_enum	{
		grace_dataset_xy,
		grace_dataset_bar,
		grace_dataset_whisker };

#define GRACE_DATASET_LINE_SYMBOL_SIZE		0.05
#define GRACE_DATASET_WHISKER_SYMBOL_SIZE	0.3
#define GRACE_DATASET_MAX_COLOR_NUMBER		15

typedef struct
{
	char *datatype_name;
	enum grace_dataset_enum grace_dataset_enum;
	int color_number;
	LIST *grace_point_list;
	LIST *grace_whisker_point_list;
	char *legend;
	int dataset_number;
	int line_type;
	char *graph_type;
	double symbol_size;
} GRACE_DATASET;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GRACE_DATASET *grace_dataset_new(
		char *datatype_name,
		LIST *grace_point_list,
		enum grace_dataset_enum grace_dataset_enum,
		int grace_dataset_color_number,
		double symbol_size );

/* Process */
/* ------- */
GRACE_DATASET *grace_dataset_calloc(
		void );

/* Returns heap memory or "" */
/* ------------------------- */
char *grace_dataset_legend(
		char *datatype_name );

int grace_dataset_number(
		void );

int grace_dataset_line_type(
		enum grace_dataset_enum grace_dataset_enum );

/* Returns program memory */
/* ---------------------- */
char *grace_dataset_graph_type(
		enum grace_dataset_enum grace_dataset_enum );

/* Usage */
/* ----- */
GRACE_DATASET *grace_dataset_graph_list_seek(
		char *datatype_name,
		LIST *grace_graph_list );

/* Usage */
/* ----- */
GRACE_DATASET *grace_dataset_seek(
		char *datatype_name,
		LIST *grace_dataset_list );

/* Usage */
/* ----- */
double grace_dataset_minimum_horizontal(
		LIST *grace_dataset_list );

/* Usage */
/* ----- */
double grace_dataset_maximum_horizontal(
		LIST *grace_dataset_list );

/* Usage */
/* ----- */
double grace_dataset_minimum_vertical(
		LIST *grace_dataset_list );

/* Usage */
/* ----- */
double grace_dataset_maximum_vertical(
		LIST *grace_dataset_list );

/* Usage */
/* ----- */
int grace_dataset_color_number(
		int grace_dataset_max_color_number,
		boolean cycle_colors_boolean );

/* Usage */
/* ----- */
LIST *grace_dataset_point_list(
		char *date_attribute_name,
		char *time_attribute_name,
		LIST *query_fetch_row_list,
		char *grace_datatype_name );

/* Usage */
/* ----- */
void grace_dataset_list_output(
		int graph_number,
		LIST *grace_dataset_list,
		FILE *output_file );

/* Process */
/* ------- */
void grace_dataset_output(
		int graph_number,
		int color_number,
		char *legend,
		int dataset_number,
		int line_type,
		char *graph_type,
		double symbol_size,
		FILE *output_file );

typedef struct
{
	double grace_dataset_minimum_horizontal;
	double grace_dataset_maximum_horizontal;
	double horizontal_range;
	double min_x;
	double max_x;
	double grace_dataset_minimum_vertical;
	double grace_dataset_maximum_vertical;
	double vertical_range;
	double min_y;
	double max_y;
	double y_tick_major;
	int y_tick_precision;
} GRACE_WORLD;

/* Usage */
/* ----- */
GRACE_WORLD *grace_world_new(
		LIST *grace_dataset_list );

/* Process */
/* ------- */
GRACE_WORLD *grace_world_calloc(
		void );

double grace_world_horizontal_range(
		double grace_dataset_minimum_horizontal,
		double grace_dataset_maximum_horizontal );

double grace_world_min_x(
		double grace_dataset_minimum_horizontal );

double grace_world_max_x(
		double grace_dataset_maximum_horizontal );

double grace_world_max_y(
		double grace_dataset_maximum_vertical );

double grace_world_vertical_range(
		double grace_world_min_y,
		double grace_dataset_maximum_vertical );

double grace_world_y_tick_major(
		double grace_dataset_maximum_vertical,
		double grace_world_max_y );

/* Usage */
/* ----- */
double grace_world_min_y(
		double grace_dataset_minimum_vertical );

/* Usage */
/* ----- */
int grace_world_y_tick_precision(
		double grace_world_y_tick_major );

/* Usage */
/* ----- */
void grace_world_output(
		GRACE_WORLD *grace_world,
		FILE *output_file );

#define GRACE_TITLE_CHAR_SIZE			1.0
#define GRACE_TITLE_SUB_TITLE_CHAR_SIZE		0.8
#define GRACE_TITLE_MAX_SUB_TITLE_LENGTH	100

typedef struct
{
	char *title;
	char *sub_title;
	int title_length;
	int sub_title_length;
	double title_char_size;
	double sub_title_char_size;
} GRACE_TITLE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GRACE_TITLE *grace_title_new(
		char *title,
		char *sub_title );

/* Process */
/* ------- */
GRACE_TITLE *grace_title_calloc(
		void );

int grace_title_length(
		char *title );

int grace_title_sub_title_length(
		char *sub_title );

/* Driver */
/* ------ */
void grace_title_output(
		int grace_title_max_subtitle_length,
		GRACE_TITLE *grace_title,
		FILE *output_file );

typedef struct
{
	double horizontal_double;
	char *ticklabel_string;
	char *agr_string;
} GRACE_TICKLABEL;

/* Usage */
/* ----- */
GRACE_TICKLABEL *grace_ticklabel_new(
		double horizontal_double,
		char *ticklabel_string );

/* Process */
/* ------- */
GRACE_TICKLABEL *grace_ticklabel_calloc(
		void );

int grace_ticklabel_major_integer(
		void );

/* Returns heap memory */
/* ------------------- */
char *grace_ticklabel_agr_string(
		int grace_ticklabel_major_integer,
		double horizontal_double,
		char *ticklabel_string );

/* Usage */
/* ----- */
double grace_ticklabel_first_horizontal_double(
		LIST *grace_ticklabel_list );

/* Usage */
/* ----- */
void grace_ticklabel_list_output(
		LIST *grace_ticklabel_list,
		FILE *output_file );

typedef struct
{
	LIST *grace_ticklabel_list;
} GRACE_HORIZONTAL_LABEL;

/* Usage */
/* ----- */
GRACE_HORIZONTAL_LABEL *grace_horizontal_label_new(
		void );

/* Process */
/* ------- */
GRACE_HORIZONTAL_LABEL *grace_horizontal_label_calloc(
		void );

/* Usage */
/* ----- */
void grace_horizontal_label_output(
		GRACE_HORIZONTAL_LABEL *grace_horizontal_label,
		FILE *output_file );

/* Process */
/* ------- */
double grace_ticklabel_first_horizontal_double(
		LIST *grace_ticklabel_list );

/* Returns static memory */
/* --------------------- */
char *grace_horizontal_label_agr_string(
		double grace_ticklabel_first_horizontal_double,
		int grace_ticklabel_list_length );

/* Returns static memory */
/* --------------------- */
char *grace_horizontal_label_tick_minor_string(
		int grace_ticklabel_list_length );

typedef struct
{
	int graph_number;
	char *yaxis_heading;
	GRACE_VIEW *grace_view;
	LIST *grace_dataset_list /* initialized only */;
	char *xaxis_tick_on_off;
	char *xaxis_ticklabel_format;
	int xaxis_ticklabel_angle;

	/* Set externally */
	/* -------------- */
	GRACE_WORLD *grace_world;
	GRACE_HORIZONTAL_LABEL *grace_horizontal_label;
} GRACE_GRAPH;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GRACE_GRAPH *grace_graph_new(
		int graph_number,
		boolean xaxis_tick_on_boolean,
		int horizontal_range,
		char *yaxis_heading,
		GRACE_VIEW *grace_view );

/* Process */
/* ------- */
GRACE_GRAPH *grace_graph_calloc(
		void );

/* Returns program memory */
/* ---------------------- */
char *grace_graph_xaxis_tick_on_off(
		boolean xaxis_tick_on_boolean );

/* Returns program memory */
/* ---------------------- */
char *grace_graph_xaxis_ticklabel_format(
		int horizontal_range );

int grace_graph_xaxis_ticklabel_angle(
		int horizontal_range );

/* Usage */
/* ----- */
void grace_graph_list_query_fetch_row_list_set(
		LIST *grace_graph_list,
		char *date_attribute_name,
		char *time_attribute_name,
		LIST *number_attribute_name_list,
		LIST *query_fetch_row_list );

/* Process */
/* ------- */
boolean grace_graph_null_boolean(
		char *query_cell_select_datum );

double grace_graph_vertical_double(
		char *query_cell_select_datum );

/* Usage */
/* ----- */
void grace_graph_row_list_set(
		GRACE_GRAPH *grace_graph,
		char *date_attribute_name,
		char *time_attribute_name,
		LIST *number_attribute_name_list,
		LIST *query_fetch_row_list );

/* Usage */
/* ----- */
void grace_graph_delimited_list_set(
		GRACE_GRAPH *grace_graph,
		char delimiter,
		int date_piece_offset,
		int time_piece_offset,
		int value_piece_offset,
		LIST *delimited_list );

/* Usage */
/* ----- */
double grace_graph_cell_horizontal_double(
		QUERY_CELL *date_query_cell,
		QUERY_CELL *time_query_cell );

/* Process */
/* ------- */
double grace_graph_horizontal_double(
		char *date_string,
		char *time_string );

/* Driver */
/* ------ */
void grace_graph_output(
		GRACE_TITLE *grace_title,
		GRACE_GRAPH *grace_graph,
		FILE *output_file );

/* Process */
/* ------- */
void grace_graph_top_output(
		int graph_number,
		FILE *output_file );

void grace_graph_xaxis_output(
		char *xaxis_tick_on_off,
		double x_tick_major,
		int x_tick_minor,
		char *xaxis_ticklabel_format,
		int xaxis_ticklabel_angle,
		FILE *output_file );

void grace_graph_yaxis_output(
		double y_tick_major,
		int y_tick_precision,
		char *yaxis_heading,
		FILE *output_file );

typedef struct
{
	pid_t process_id;
	APPASERVER_LINK *appaserver_link;
	char *agr_output_filename;
	char *agr_prompt_filename;
	char *pdf_output_filename;
	char *pdf_prompt_filename;
	char *postscript_filename;
	char *grace_home_directory;
	char *grace_execution_directory;
} GRACE_FILENAME;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GRACE_FILENAME *grace_filename_new(
		char *application_name,
		char *session_key,
		char *filename_key,
		char *appaserver_parameter_data_directory );

/* Process */
/* ------- */
GRACE_FILENAME *grace_filename_calloc(
		void );

pid_t grace_filename_process_id(
		char *session_key );

/* Returns heap memory */
/* ------------------- */
char *grace_filename_agr_output_filename(
		APPASERVER_LINK *appaserver_link );

/* Returns heap memory */
/* ------------------- */
char *grace_filename_agr_prompt_filename(
		APPASERVER_LINK *appaserver_link );

/* Returns heap memory */
/* ------------------- */
char *grace_filename_pdf_output_filename(
		APPASERVER_LINK *appaserver_link );

/* Returns heap memory */
/* ------------------- */
char *grace_filename_pdf_prompt_filename(
		APPASERVER_LINK *appaserver_link );

/* Returns heap memory */
/* ------------------- */
char *grace_filename_postscript_filename(
		APPASERVER_LINK *appaserver_link );

#define GRACE_WINDOW_GRACE_URL "http://plasma-gate.weizmann.ac.il/Grace/"

typedef struct
{
	char *pdf_target;
	char *pdf_anchor_html;
	char *agr_target;
	char *agr_anchor_html;
	char *html;
} GRACE_PROMPT;

/* Usage */
/* ----- */
GRACE_PROMPT *grace_prompt_new(
		GRACE_FILENAME *grace_filename,
		char *filename_key,
		pid_t process_id );

/* Process */
/* ------- */
GRACE_PROMPT *grace_prompt_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *grace_prompt_pdf_anchor_html(
		char *pdf_prompt_filename,
		char *filename_key,
		char *grace_prompt_pdf_target );

/* Returns static memory */
/* --------------------- */
char *grace_prompt_agr_anchor_html(
		char *agr_prompt_filename,
		char *filename_key,
		char *grace_prompt_agr_target );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *grace_prompt_pdf_target(
		char *filename_key,
		pid_t process_id );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *grace_prompt_agr_target(
		char *filename_key,
		pid_t process_id );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *grace_prompt_html(
		char *grace_prompt_pdf_anchor_html,
		char *grace_prompt_agr_anchor_html );

typedef struct
{
	char *onload_string;
	char *date_time_now;
	char *screen_title;
	char *grace_home_url_html;
	char *html;
} GRACE_WINDOW;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GRACE_WINDOW *grace_window_new(
		GRACE_FILENAME *grace_filename,
		char *sub_title,
		char *filename_key,
		pid_t process_id );

/* Process */
/* ------- */
GRACE_WINDOW *grace_window_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *grace_window_onload_string(
		char *pdf_prompt_filename,
		char *grace_prompt_pdf_target );

/* Returns static memory */
/* --------------------- */
char *grace_window_screen_title(
		char *date_time_now );

/* Returns heap memory */
/* ------------------- */
char *grace_window_sub_title_display(
		char *sub_title );

/* Returns static memory */
/* --------------------- */
char *grace_window_grace_home_url_html(
		char *grace_window_grace_url );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *grace_window_html(
		char *document_body_onload_open_tag,
		char *screen_title,
		char *sub_title_display );

#define GRACE_PAGE_PORTRAIT_THRESHOLD 		4
#define GRACE_PAGE_PORTRAIT_WIDTH_PIXELS	612
#define GRACE_PAGE_PORTRAIT_LENGTH_PIXELS	792
#define GRACE_PAGE_LANDSCAPE_WIDTH_PIXELS	796
#define GRACE_PAGE_LANDSCAPE_LENGTH_PIXELS	612

typedef struct
{
	int width_pixels;
	int length_pixels;
	boolean landscape_boolean;
} GRACE_PAGE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GRACE_PAGE *grace_page_new(
		int grace_graph_list_length );

/* Process */
/* ------- */
GRACE_PAGE *grace_page_calloc(
		void );

/* Usage */
/* ----- */
void grace_page_output(
		int grace_page_width_pixels,
		int grace_page_length_pixels,
		FILE *output_file );

/* Process */
/* ------- */

/* Returns program memory */
/* ---------------------- */
char *grace_page_map_font_string(
		void );

/* Returns program memory */
/* ---------------------- */
char *grace_page_map_color_string(
		void );

/* Returns static memory */
/* --------------------- */
char *grace_page_default_string(
		void );

/* Returns program memory */
/* ---------------------- */
char *grace_page_timestamp_string(
		void );

/* Returns static memory */
/* --------------------- */
char *grace_page_region_string(
		void );

typedef struct
{
	GRACE_FILENAME *grace_filename;
	char *grace_ps_system_string;
	GRACE_PAGE *grace_page;
	char *grace_pdf_system_string;
	GRACE_TITLE *grace_title;
} GRACE_SETUP;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GRACE_SETUP *grace_setup_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		char *title,
		char *sub_title,
		char *filename_key,
		int grace_graph_list_length );

/* Process */
/* ------- */
GRACE_SETUP *grace_setup_calloc(
		void );

typedef struct
{
	GRACE_SETUP *grace_setup;
	GRACE_VIEW *grace_view;
	GRACE_DATASET *grace_dataset;
	GRACE_GRAPH *grace_graph;
} GRACE_UNITLESS;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
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
		char *filename_key );

/* Process */
/* ------- */
GRACE_UNITLESS *grace_unitless_calloc(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GRACE_GRAPH *grace_unitless_grace_graph(
		int horizontal_range,
		GRACE_VIEW *grace_view,
		GRACE_DATASET *grace_dataset );

typedef struct
{
	GRACE_SETUP *grace_setup;
	GRACE_VIEW *grace_view;
	GRACE_GRAPH *grace_graph;
} GRACE_HISTOGRAM;

/* Usage */
/* ----- */
GRACE_HISTOGRAM *grace_histogram_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		char *title,
		char *sub_title,
		HISTOGRAM *histogram );

/* Process */
/* ------- */
GRACE_HISTOGRAM *grace_histogram_calloc(
		void );

#define GRACE_EXECUTABLE_TEMPLATE "(GRACE_HOME=%s; export GRACE_HOME; LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib; export LD_LIBRARY_PATH; %s/gracebat"
/*								          ^ */
/*								          | */
/* Don't close the param because parameters follow.			    */
/* ------------------------------------------------------------------------ */

typedef struct
{
	/* stub */
} GRACE;

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *grace_ps_system_string(
		char *grace_executable_template,
		GRACE_FILENAME *grace_filename );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *grace_pdf_system_string(
		GRACE_FILENAME *grace_filename );

/* Driver */
/* ------ */
void grace_single_graph_pdf_create(
		char *grace_ps_system_string,
		GRACE_PAGE *grace_page,
		GRACE_TITLE *grace_title,
		GRACE_GRAPH *grace_graph,
		char *grace_pdf_system_string );

/* Driver */
/* ------ */
void grace_graph_list_pdf_create(
		char *grace_ps_system_string,
		GRACE_PAGE *grace_page,
		GRACE_TITLE *grace_title,
		LIST *grace_graph_list,
		char *grace_pdf_system_string );

typedef struct
{
	GRACE_SETUP *grace_setup;
	GRACE_VIEW *grace_view;
	GRACE_GRAPH *grace_graph;
	double grace_point_vertical_range_double;
} GRACE_GENERIC;

/* Usage */
/* ----- */

/* ------------------------------------- */
/* Safely returns.			 */
/* Interface with PROCESS_GENERIC_GRACE. */
/* ------------------------------------- */
GRACE_GENERIC *grace_generic_new(
		char *application_name,
		char *data_directory,
		char *value_attribute_name,
		char *process_generic_grace_title,
		char *process_generic_grace_sub_title,
		LIST *process_generic_grace_point_list,
		int process_generic_grace_horizontal_range );

/* Process */
/* ------- */
GRACE_GENERIC *grace_generic_calloc(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GRACE_GRAPH *grace_generic_grace_graph(
		char *dataset_name,
		LIST *process_generic_grace_point_list,
		int process_generic_grace_horizontal_range,
		GRACE_VIEW *grace_view );

typedef struct
{
	GRACE_SETUP *grace_setup;
	GRACE_VIEW *grace_view;
	GRACE_GRAPH *grace_graph;
	double grace_point_vertical_range_double;
} GRACE_WHISKER;

/* -------------------------------------------- */
/* Usage					*/
/* -------------------------------------------- */
/* Safely returns.				*/
/* Interface with PROCESS_GENERIC_WHISKER.	*/
/* -------------------------------------------- */
GRACE_WHISKER *grace_whisker_new(
		char *application_name,
		char *data_directory,
		char *value_attribute_name,
		char *process_generic_whisker_title,
		char *process_generic_whisker_sub_title,
		LIST *process_generic_whisker_point_list,
		int process_generic_whisker_horizontal_range );

/* Process */
/* ------- */
GRACE_WHISKER *grace_whisker_calloc(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GRACE_GRAPH *grace_whisker_grace_graph(
		char *dataset_name,
		LIST *process_generic_whisker_point_list,
		int process_generic_whisker_horizontal_range,
		GRACE_VIEW *grace_view );

#endif
