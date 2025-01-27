/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/process_generic.h 				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef PROCESS_GENERIC_H
#define PROCESS_GENERIC_H

#include "list.h"
#include "boolean.h"
#include "hash_table.h"
#include "folder.h"
#include "date.h"
#include "appaserver_link.h"
#include "aggregate_statistic.h"
#include "statistic.h"
#include "html.h"
#include "output_medium.h"

#define PROCESS_GENERIC_GRACE_MEDIUM_STRING		"gracechart"
#define PROCESS_GENERIC_GOOGLE_MEDIUM_STRING		"googlechart"
#define PROCESS_GENERIC_WHISKER_YN			"whisker_yn"

#define PROCESS_GENERIC_VALUE_FOLDER_PRIMARY_KEY	"value_folder"
#define PROCESS_GENERIC_VALUE_ATTRIBUTE_PRIMARY_KEY	"value_attribute"

typedef struct
{
	char *query_data_where;
	char *string;
} PROCESS_GENERIC_WHERE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC_WHERE *process_generic_where_new(
		LIST *where_attribute_name_list,
		LIST *where_attribute_data_list,
		LIST *folder_attribute_primary_list,
		char *process_generic_date_where );

/* Process */
/* ------- */
PROCESS_GENERIC_WHERE *process_generic_where_calloc(
		void );

/* Returns heap memory or null */
/* --------------------------- */
char *process_generic_where_string(
		char *query_data_where,
		char *process_generic_date_where );

typedef struct
{
	APPASERVER_LINK *appaserver_link;
	APPASERVER_LINK *agr_appaserver_link;
} PROCESS_GENERIC_LINK;

/* Usage */
/* ----- */

/* Returns program memory or null */
/* ------------------------------ */
char *process_generic_link_extension(
		enum output_medium output_medium );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC_LINK *process_generic_link_new(
		char *application_name,
		char *data_directory,
		char *filename_stem,
		pid_t process_id,
		char *process_generic_link_extension );

/* Process */
/* ------- */
PROCESS_GENERIC_LINK *process_generic_link_calloc(
		void );

typedef struct
{
	enum output_medium output_medium;
	char *process_generic_link_extension;
	PROCESS_GENERIC_LINK *process_generic_link;
	char *appaserver_user_date_format_string;
	char *begin_date_string;
	char *end_date_string;
	boolean date_is_date_time;
	enum aggregate_level aggregate_level;
	enum aggregate_statistic aggregate_statistic;
	boolean accumulate_boolean;
	boolean whisker_boolean;
	char *units_converted_name;
	int days_to_average;
	int date_piece;
	int time_piece;
	int value_piece;
	int accumulate_piece;
} PROCESS_GENERIC_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC_INPUT *process_generic_input_new(
		char *application_name,
		char *login_name,
		char *output_medium_string,
		char *data_directory,
		DICTIONARY *post_dictionary,
		char *process_generic_process_name,
		boolean aggregation_sum_boolean,
		LIST *folder_attribute_list,
		LIST *folder_attribute_primary_key_list,
		int primary_key_list_length,
		char *date_attribute_name,
		char *time_attribute_name );

/* Process */
/* ------- */
PROCESS_GENERIC_INPUT *process_generic_input_calloc(
		void );

/* Returns component of post_dictionary, heap memory, or null */
/* ---------------------------------------------------------- */
char *process_generic_input_begin_date_string(
		DICTIONARY *post_dictionary );

/* Returns component of post_dictionary, heap memory, or null */
/* ---------------------------------------------------------- */
char *process_generic_input_end_date_string(
		DICTIONARY *post_dictionary );

boolean process_generic_input_date_is_date_time(
		LIST *folder_attribute_list,
		char *date_attribute_name );

boolean process_generic_input_whisker_boolean(
		char *process_generic_whisker_yn,
		DICTIONARY *post_dictionary );

/* Returns component of post_dictionary or null */
/* -------------------------------------------- */
char *process_generic_input_units_converted(
		DICTIONARY *post_dictionary );

/* ----------------------- */
/* Returns -1 if not found */
/* ----------------------- */
int process_generic_input_date_piece(
		LIST *folder_attribute_primary_key_list,
		char *date_attribute_name );

/* ----------------------- */
/* Returns -1 if not found */
/* ----------------------- */
int process_generic_input_time_piece(
		LIST *folder_attribute_primary_key_list,
		char *time_attribute_name );

/* Returns primary_key_list_length */
/* ------------------------------- */
int process_generic_input_value_piece(
		int primary_key_list_length );

/* Returns -1 or value_piece */
/* ------------------------- */
int process_generic_input_accumulate_piece(
		boolean accumulate_boolean,
		int value_piece );

typedef struct
{
	char *primary_data_list_string;
	char *date_string;
	char *time_string;
	boolean value_null_boolean;
	char *value_string;
	double value_double;
	double accumulate_double;
} PROCESS_GENERIC_POINT;

/* Usage */
/* ----- */
LIST *process_generic_point_list(
		char delimiter,
		int primary_key_list_length,
		int date_piece,
		int time_piece,
		int value_piece,
		int accumulate_piece,
		char *system_string );

/* Process */
/* ------- */

/* Usage */
/* ----- */
PROCESS_GENERIC_POINT *process_generic_point_parse(
		char delimiter,
		int primary_key_list_length,
		int date_piece,
		int time_piece,
		int value_piece,
		int accumulate_piece,
		char *input );

/* Process */
/* ------- */
PROCESS_GENERIC_POINT *process_generic_point_calloc(
		void );


/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *process_generic_point_primary_data_list_string(
		char delimiter,
		int primary_key_list_length,
		char *input );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *process_generic_point_date_string(
		char delimiter,
		int date_piece,
		char *input );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *process_generic_point_time_string(
		char delimiter,
		int time_piece,
		char *input );

/* Usage */
/* ----- */
boolean process_generic_point_value_null_boolean(
		char delimiter,
		int value_piece,
		char *input );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *process_generic_point_value_string(
		char delimiter,
		int value_piece,
		char *input );

/* Usage */
/* ----- */
double process_generic_point_value_double(
		char *process_generic_point_value_string );

/* Usage */
/* ----- */
double process_generic_point_accumulate_double(
		char delimiter,
		int accumulate_piece,
		char *input );

/* Usage */
/* ----- */
void process_generic_point_free(
		PROCESS_GENERIC_POINT *process_generic_point );

typedef struct
{
	char *datatype_name;
	boolean aggregation_sum_boolean;
	boolean bar_graph;
	boolean scale_graph_zero;
	char *units_name;
} PROCESS_GENERIC_DATATYPE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC_DATATYPE *process_generic_datatype_fetch(
		DICTIONARY *post_dictionary,
		char *datatype_folder_name,
		char *datatype_attribute_name,
		boolean exists_aggregation_sum,
		boolean exists_bar_graph,
		boolean exists_scale_graph_zero,
		boolean datatype_exists_units );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *process_generic_datatype_select(
		char *datatype_attribute_name,
		boolean exists_aggregation_sum,
		boolean exists_bar_graph,
		boolean exists_scale_graph_zero,
		boolean unit_datatype_folder );

/* Returns static memory */
/* --------------------- */
char *process_generic_datatype_where(
		char *datatype_attribute_name,
		char *datatype_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC_DATATYPE *process_generic_datatype_parse(
		char *input );

/* Process */
/* ------- */
PROCESS_GENERIC_DATATYPE *process_generic_datatype_calloc(
		void );

/* Usage */
/* ----- */

/* Returns component of dictionary */
/* ------------------------------- */
char *process_generic_datatype_name(
		char *datatype_attribute_name,
		DICTIONARY *post_dictionary );

typedef struct
{
	FOLDER *folder;
	LIST *data_list;
	char *data_list_name;
	char *units_name;
} PROCESS_GENERIC_FOREIGN_FOLDER;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC_FOREIGN_FOLDER *process_generic_foreign_folder_fetch(
		DICTIONARY *post_dictionary,
		char *foreign_folder_name,
		boolean foreign_exists_units );

/* Process */
/* ------- */
PROCESS_GENERIC_FOREIGN_FOLDER *process_generic_foreign_folder_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *process_generic_foreign_folder_data_list_name(
		LIST *process_generic_foreign_folder_data_list,
		char delimiter );

/* Usage */
/* ----- */
LIST *process_generic_foreign_folder_data_list(
		DICTIONARY *post_dictionary,
		LIST *foreign_key_list );

/* Usage */
/* ----- */
char *process_generic_foreign_folder_units_name(
		boolean foreign_exists_units,
		char *last_data );

#define PROCESS_GENERIC_VALUE_TABLE	"proces_generic_value"

#define PROCESS_GENERIC_VALUE_SELECT	"datatype_folder,"		\
					"foreign_folder,"		\
					"datatype_attribute,"		\
					"date_attribute,"		\
					"time_attribute,"		\
					"datatype_aggregation_sum_yn,"	\
					"datatype_bar_graph_yn,"	\
					"datatype_scale_graph_zero_yn,"	\
					"datatype_units_yn,"		\
					"foreign_units_yn"
typedef struct
{
	char *value_folder_name;
	char *value_attribute_name;
	char *datatype_folder_name;
	char *foreign_folder_name;
	char *datatype_attribute_name;
	char *date_attribute_name;
	char *time_attribute_name;
	boolean exists_aggregation_sum;
	boolean exists_bar_graph;
	boolean exists_scale_graph_zero;
	boolean datatype_exists_units;
	boolean foreign_exists_units;
	LIST *folder_attribute_list;
	LIST *folder_attribute_primary_key_list;
	int primary_key_list_length;
	PROCESS_GENERIC_DATATYPE *process_generic_datatype;
	PROCESS_GENERIC_FOREIGN_FOLDER *process_generic_foreign_folder;
	char *units_name;
} PROCESS_GENERIC_VALUE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC_VALUE *process_generic_value_fetch(
		char *process_generic_value_table,
		DICTIONARY *post_dictionary,
		char *process_generic_value_folder_name,
		char *process_generic_value_attribute_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *process_generic_value_primary_where(
		char *process_generic_value_folder_name,
		char *process_generic_value_attribute_name );

/* Returns either parameter or null */
/* -------------------------------- */
char *process_generic_value_units_name(
		char *datatype_units_name,
		char *foreign_folder_units_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC_VALUE *process_generic_value_parse(
		char *value_folder_name,
		char *value_attribute_name,
		char *input );

/* Process */
/* ------- */
PROCESS_GENERIC_VALUE *process_generic_value_calloc(
		void );

#define PROCESS_GENERIC_TABLE		"process_generic"
#define PROCESS_GENERIC_SELECT		"value_folder,value_attribute"

typedef struct
{
	char *process_set_name;
	char *process_name;
	char *value_folder_name;
	char *value_attribute_name;
	PROCESS_GENERIC_VALUE *process_generic_value;
	PROCESS_GENERIC_INPUT *process_generic_input;
	char *select;
	char *date_where;
	PROCESS_GENERIC_WHERE *process_generic_where;
	char *system_string;
	char *heading;
	char *title;
	char *sub_title;
} PROCESS_GENERIC;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC *process_generic_new(
		char *application_name,
		char *login_name,
		char *process_generic_process_set_name,
		char *process_generic_process_name,
		char *output_medium_string,
		char *data_directory,
		DICTIONARY *post_dictionary );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *process_generic_select(
		LIST *folder_attribute_primary_key_list,
		char *value_attribute_name );

/* Returns static memory */
/* --------------------- */
char *process_generic_date_where(
		char *date_attribute_name,
		char *begin_date_string,
		char *end_date_string,
		boolean date_is_date_time );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *process_generic_heading(
		LIST *folder_attribute_primary_key_list,
		char *value_attribute_name,
		char *process_generic_value_units_name,
		char *units_converted_name,
		enum aggregate_level aggregate_level,
		enum aggregate_statistic aggregate_statistic,
		boolean accumulate_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *process_generic_system_string(
		char *application_name,
		char *process_generic_select,
		char *value_folder_name,
		char *where_string,
		enum aggregate_level aggregate_level,
		enum aggregate_statistic aggregate_statistic,
		int accumulate_piece,
		int date_piece,
		int time_piece,
		int value_piece,
		char *end_date_string,
		char *process_generic_value_units_name,
		char *process_generic_input_units_converted_name,
		boolean process_generic_input_whisker_boolean );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *process_generic_title(
		char *process_generic_process_set_name,
		char *process_generic_process_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *process_generic_sub_title(
		char *process_generic_value_folder_name,
		char *appaserver_user_date_format_string,
		char *begin_date_string,
		char *end_date_string,
		enum aggregate_level aggregate_level,
		enum aggregate_statistic aggregate_statistic );

/* Usage */
/* ----- */

/* Returns process_set_name or "null" */
/* ---------------------------------- */
char *process_generic_process_set_name(
		char *process_set_name );

/* Usage */
/* ----- */

/* Returns process_name, component of post_dictionary, or "null" */
/* ------------------------------------------------------------- */
char *process_generic_process_name(
		char *process_name,
		DICTIONARY *post_dictionary,
		char *process_generic_process_set_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC *process_generic_fetch(
		char *process_generic_table,
		char *process_generic_process_name,
		char *process_generic_process_set_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *process_generic_primary_where(
		char *process_generic_process_name,
		char *process_generic_process_set_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC *process_generic_parse(
		char *process_generic_process_name,
		char *process_generic_process_set_name,
		char *input );

/* Process */
/* ------- */
PROCESS_GENERIC *process_generic_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *process_generic_units_label(
		char *units,
		char *units_converted,
		enum aggregate_statistic aggregate_statistic );

#endif
