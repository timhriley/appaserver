/* $APPASERVER_HOME/library/process_generic_output.h 			*/
/* -------------------------------------------------------------------- */
/* This is the appaserver process_generic_output ADT.			*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef PROCESS_GENERIC_OUTPUT_H
#define PROCESS_GENERIC_OUTPUT_H

/* Includes */
/* -------- */
#include "list.h"
#include "hashtbl.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"

/* Constants */
/* --------- */
#define HTML_TABLE_QUEUE_TOP_BOTTOM		500
#define PROCESS_GENERIC_OUTPUT_DELIMITER	'|'
#define PROCESS_GENERIC_ENTITY_DELIMITER	'/'

/* Enumerated types */
/* ---------------- */
enum process_generic_output_medium {
			text_file,
			output_medium_stdout,
			table,
			spreadsheet };

/* Structures */
/* ---------- */
typedef struct
{
	char *process_set;
	char *output_medium_string;
	enum process_generic_output_medium output_medium;
	char delimiter;
	char *begin_date;
	char *end_date;
	enum aggregate_level aggregate_level;
	enum aggregate_statistic aggregate_statistic;
	boolean accumulate;
	char *email_address;
	char *units_converted;
	int days_to_average;
	char *end_date_suffix;
	pid_t process_id;
	char *process_name;
} PROCESS_GENERIC_PARAMETER;

typedef struct
{
	char *date;
	char *time;
	char *unit;
	double value;
	boolean is_null;
	double accumulate;
} PROCESS_GENERIC_VALUE;

typedef struct
{
	char *datatype_folder_name;
	char *datatype_attribute_name;
	boolean exists_aggregation_sum;
	boolean exists_bar_graph;
	boolean exists_scale_graph_zero;
	boolean datatype_exists_units;
	DICTIONARY *post_dictionary;

	/* Process */
	/* ------- */
	char *datatype_name;
	boolean aggregation_sum;
	boolean bar_graph;
	boolean scale_graph_zero;
	char *unit;
	char *process_generic_datatype_select;
	char *process_generic_datatype_where;
	char *process_generic_datatype_system_string;
	HASH_TABLE *values_hash_table;

} PROCESS_GENERIC_DATATYPE;

typedef struct
{
	char *foreign_folder_name;
	DICTIONARY *post_dictionary;
	FOLDER *foreign_folder;
	LIST *foreign_attribute_name_list;
	LIST *foreign_attribute_data_list;
	int foreign_attribute_name_list_length;
} PROCESS_GENERIC_FOREIGN_FOLDER;

typedef struct
{
	/* Input */
	/* ----- */
	char *value_folder_name;
	DICTIONARY *post_dictionary;

	/* Process */
	/* ------- */
	char *datatype_folder_name;
	char *foreign_folder_name;
	char *datatype_attribute_name;
	char *date_attribute_name;
	char *time_attribute_name;
	char *value_attribute_name;
	boolean exists_aggregation_sum;
	boolean exists_bar_graph;
	boolean exists_scale_graph_zero;
	boolean datatype_exists_unit;
	boolean foreign_exists_unit;

	LIST *primary_attribute_name_list;
	PROCESS_GENERIC_DATATYPE *datatype;
	LIST *compare_datatype_list;
} PROCESS_GENERIC_VALUE_FOLDER;

typedef struct
{
	/* Input */
	/* ----- */
	char *process_set_name;
	char *output_medium_string;
	DICTIONARY *post_dictionary;

	/* Process */
	/* ------- */
	char *process_name;
	char *value_folder_name;
	PROCESS_GENERIC_VALUE_FOLDER *value_folder;
	PROCESS_GENERIC_PARAMETER *parameter;
	PROCESS_GENERIC_FOREIGN_FOLDER *foreign_folder;
	char *process_generic_output_date_where;
	char *process_generic_output_where;
	char *process_generic_output_system_string;
	char *process_generic_output_heading;
	char *process_generic_output_subtitle;

	/* To retire */
	/* --------- */
	boolean accumulate;
} PROCESS_GENERIC_OUTPUT;

/* Prototypes */
/* ---------- */

PROCESS_GENERIC_VALUE_FOLDER *process_generic_value_folder_new(
				char *application_name,
				char *process_name,
				char *process_set_name );

void process_generic_value_folder_load(
					char **value_folder_name,
					char **date_attribute_name,
					char **time_attribute_name,
					char **value_attribute_name,
					char **datatype_folder_name,
					char **foreign_folder_name,
					char *application_name,
					char *process_name,
					char *process_set_name );

void process_generic_datatype_folder_load(
			LIST **primary_attribute_name_list,
			boolean *datatype_exists_aggregation_sum,
			boolean *datatype_exists_bar_graph,
			boolean *datatype_exists_scale_graph_zero,
			char *application_name,
			char *datatype_folder_name );

void process_generic_datatype_load(
			boolean *datatype_aggregation_sum,
			boolean *datatype_bar_graph,
			boolean *datatype_scale_graph_zero,
			char **datatype_units,
			boolean datatype_exists_aggregation_sum,
			boolean datatype_exists_bar_graph,
			boolean datatype_exists_scale_graph_zero,
			char *application_name,
			char *datatype_folder_name,
			LIST *primary_attribute_name_list,
			LIST *primary_attribute_data_list,
			char *units_folder_name );

/* PROCESS_GENERIC_FOREIGN_FOLDER */
/* ------------------------------ */
PROCESS_GENERIC_FOREIGN_FOLDER *process_generic_foreign_folder_new(
			char *application_name,
			char *foreign_folder_name );

/* PROCESS_GENERIC_OUTPUT */
/* ---------------------- */
PROCESS_GENERIC_OUTPUT *process_generic_output_new(
		 			char *application_name,
					char *process_name,
					char *process_set_name,
					boolean accumulate );

char *process_generic_output_text_file_sys_string(
			char **begin_date,
			char **end_date,
			char **where_clause,
			char **units_label,
			int *datatype_entity_piece,
			int *datatype_piece,
			int *date_piece,
			int *time_piece,
			int *value_piece,
			int *length_select_list,
			char *application_name,
			PROCESS_GENERIC_OUTPUT *process_generic_output,
			DICTIONARY *post_dictionary,
			char delimiter,
			enum aggregate_level,
			enum aggregate_statistic,
			boolean append_low_high,
			boolean concat_datatype_entity,
			boolean concat_datatype,
			boolean accumulate_flag );

char *process_generic_output_begin_end_date_where(
			char **begin_date,
			char **end_date,
			DICTIONARY *dictionary,
			char *application_name,
			char *value_folder_name,
			char *date_attribute_name,
			char *where_clause,
			PROCESS_GENERIC_OUTPUT *process_generic_output );

LIST *process_generic_output_append_select_list(
			LIST *select_list,
			LIST *attribute_list,
			DICTIONARY *dictionary );

char *process_generic_output_get_process_name(
			char *process_set_name,
			DICTIONARY *post_dictionary );

char *process_generic_output_process_name(
			char *process_set_name,
			DICTIONARY *post_dictionary );

char *process_generic_output_get_units(
				char *application_name,
				char *datatype_folder_name,
				LIST *datatype_primary_attribute_name_list,
				DICTIONARY *post_dictionary );

char *process_generic_output_get_units_label(
					char *units,
					char *units_converted,
					enum aggregate_statistic );

char *process_generic_output_get_row_exceedance_stdout_sys_string(
				char **begin_date,
				char **end_date,
				int *percent_below_piece,
				char **where_clause,
				char *application_name,
				PROCESS_GENERIC_OUTPUT *
					process_generic_output,
				DICTIONARY *post_dictionary,
				char *login_name,
				int row );

char *process_generic_output_get_exceedance_stdout_sys_string(
				char **begin_date,
				char **end_date,
				int *percent_below_piece,
				char **where_clause,
				char *application_name,
				PROCESS_GENERIC_OUTPUT *
					process_generic_output,
				DICTIONARY *post_dictionary,
				boolean with_html_table,
				char *login_name );

char *process_generic_output_get_exceedance_html_table_justify_string(
				int list_length,
				enum aggregate_level );

LIST *process_generic_output_get_exceedance_heading_list(
				LIST *select_list,
				char *value_attribute_name,
				char *units_label,
				enum aggregate_level );

char *process_generic_output_get_units_folder(
				char *application_name,
				char *datatype_folder_name,
				char *datatype_attribute,
				DICTIONARY *post_dictionary );

void process_generic_output_get_period_of_record_date(
				char **date_string,
				char *application_name,
				char *value_folder_name,
				char *function,
				char *date_attribute,
				char *where_clause );

char *process_generic_output_get_datatype_entity_data(
				DICTIONARY *post_dictionary,
				char *datatype_entity_attribute );

enum aggregate_statistic process_generic_output_get_aggregate_statistic(
				boolean datatype_aggregation_sum );

LIST *process_generic_output_get_compare_datatype_name_list(
				LIST *compare_datatype_list );

char *process_generic_output_compare_datatype_name_list_display(
				LIST *compare_datatype_name_list );

char *process_generic_output_get_row_dictionary_where_clause(
			char **begin_date,
			char **end_date,
			char *application_name,
			PROCESS_GENERIC_OUTPUT *process_generic_output,
			DICTIONARY *dictionary,
			boolean with_set_dates,
			int row );

char *process_generic_output_get_dictionary_where_clause(
			char **begin_date,
			char **end_date,
			char *application_name,
			PROCESS_GENERIC_OUTPUT *process_generic_output,
			DICTIONARY *dictionary,
			boolean with_set_dates,
			char *value_folder_name );

LIST *process_generic_get_compare_datatype_list(
			char *application_name,
			LIST *foreign_attribute_name_list,
			char *datatype_folder_name,
			LIST *datatype_primary_attribute_name_list,
			boolean exists_aggregation_sum,
			boolean exists_bar_graph,
			boolean exists_scale_graph_zero,
			char *units_folder_name,
			DICTIONARY *post_dictionary,
			char *compare_datatype_prefix,
			boolean with_select_data,
			enum aggregate_level aggregate_level,
			char *begin_date_string,
			char *end_date_string,
			char *value_folder_name,
			char *date_attribute_name,
			char *time_attribute_name,
			char *value_attribute_name,
			boolean accumulate );

PROCESS_GENERIC_DATATYPE *process_generic_datatype_new(
			char *application_name,
			LIST *foreign_attribute_name_list,
			char *datatype_folder_name,
			LIST *primary_attribute_name_list,
			boolean exists_aggregation_sum,
			boolean exists_bar_graph,
			boolean exists_scale_graph_zero,
			char *units_folder_name,
			DICTIONARY *post_dictionary,
			int dictionary_index );

char *process_generic_compare_datatype_list_display(
		LIST *compare_datatype_list );

char *process_generic_datatype_display(
		PROCESS_GENERIC_DATATYPE *process_generic_datatype );

char *process_generic_output_get_foreign_where_clause(
			char *begin_date_string,
			char *end_date_string,
			LIST *foreign_attribute_name_list,
			LIST *foreign_attribute_data_list );

char *process_generic_output_get_foreign_folder_where_clause(
			char **begin_date,
			char **end_date,
			LIST *foreign_attribute_name_list,
			LIST *foreign_attribute_data_list,
			DICTIONARY *dictionary,
	       		char *application_name,
			char *value_folder_name,
			char *date_attribute_name );

HASH_TABLE *process_generic_values_hash_table(
			char *application_name,
			char *values_select_string,
			enum aggregate_level aggregate_level,
			LIST *foreign_attribute_name_list,
			LIST *foreign_attribute_data_list,
			char *begin_date_string,
			char *end_date_string,
			char *value_folder_name,
			char *date_attribute_name,
			boolean accumulate );

PROCESS_GENERIC_VALUE *process_generic_value_new(
			void );

char *process_generic_get_datatype_entity_name(
			LIST *foreign_attribute_data_list );

char *process_generic_output_get_heading_string(
			PROCESS_GENERIC_VALUE_FOLDER *value_folder,
			char heading_delimiter,
			enum aggregate_level aggregate_level );

char *process_generic_output_get_datatype_heading_string(
			PROCESS_GENERIC_FOREIGN_FOLDER *foreign_folder,
			PROCESS_GENERIC_DATATYPE *datatype,
			char heading_delimiter );

LIST *process_generic_output_get_select_list(
			int *datatype_entity_piece,
			int *datatype_piece,
			int *date_piece,
			int *time_piece,
			int *value_piece,
			int *length_select_list,
			PROCESS_GENERIC_VALUE_FOLDER *value_folder,
			boolean concat_datatype_entity,
			boolean concat_datatype );

char *process_generic_get_datatype_where_clause(
			LIST *primary_attribute_name_list,
			LIST *primary_attribute_data_list );

LIST *process_generic_get_datatype_primary_attribute_data_list(
			DICTIONARY *post_dictionary,
			LIST *datatype_primary_attribute_name_list,
			char *compare_datatype_prefix,
			int dictionary_index );

char *process_generic_get_datatype_name(
			LIST *datatype_primary_attribute_data_list,
			char delimiter );

LIST *process_generic_get_prefixed_primary_attribute_data_list(
			LIST *full_primary_attribute_data_list,
			int length_datatype_primary_attribute_name_list );

char *process_generic_get_datatype_entity(
			LIST *foreign_attribute_data_list,
			LIST *datatype_primary_attribute_name_list,
			char delimiter );

LIST *process_generic_output_get_concat_heading_list(
			PROCESS_GENERIC_VALUE_FOLDER *value_folder,
			enum aggregate_level );

LIST *process_generic_output_get_compare_entity_name_list(
			LIST *compare_datatype_list,
			LIST *primary_attribute_name_list );

LIST *process_generic_output_get_compare_datatype_units_list(
			LIST *compare_datatype_list );

enum aggregate_statistic
		process_generic_output_get_database_aggregate_statistic(
			char *application_name,
			char *appaserver_mount_point,
			LIST *primary_attribute_data_list,
			boolean exists_aggregation_sum );

char *process_generic_output_get_drop_down_where_clause(
			char *application_name,
			char *value_folder_name,
			DICTIONARY *dictionary );

boolean process_generic_output_validate_begin_end_date(
			char **begin_date_string /* in/out */,
			char **end_date_string /* in/out */,
			char *application_name,
			char *value_folder_name,
			char *date_attribute_name,
			DICTIONARY *query_removed_post_dictionary );

/* Returns heap memory */
/* ------------------- */
char *process_generic_where(
			LIST *foreign_attribute_name_list,
			LIST *foreign_attribute_data_list,
			char *date_where );

/* Returns static memory */
/* --------------------- */
char *process_generic_date_where(
			char *date_attribute_name,
			char *begin_date,
			char *end_date );

char *process_generic_output_drop_down_where(
			char *application_name,
			char *value_folder_name,
			DICTIONARY *query_removed_post_dictionary );

char *process_generic_output_where_clause(
			char *application_name,
			char *value_folder_name,
			char *date_attribute_name,
			char *begin_date_string,
			char *end_date_string,
			DICTIONARY *query_removed_post_dictionary );

QUERY_OUTPUT *process_generic_query_output(
			char *application_name,
			char *value_folder_name,
			LIST *mto1_related_folder_list,
			DICTIONARY *query_removed_post_dictionary );

/* PROCESS_GENERIC_OUTPUT */
/* ---------------------- */
PROCESS_GENERIC_OUTPUT *process_generic_output_calloc(
			void );

char *process_generic_output_value_folder_name(
			char *process_name,
			char *process_set_name );

/* Returns static memory */
/* --------------------- */
char *process_generic_output_date_where(
			char *date_attribute_name,
			char *begin_date,
			char *end_date );

/* Returns heap memory or null */
/* --------------------------- */
char *process_generic_output_where(
			LIST *foreign_attribute_name_list,
			LIST *foreign_attribute_data_list,
			char *process_generic_output_date_where );

char *process_generic_output_select(
			LIST *foreign_attribute_name_list,
			int foreign_attribute_name_list_length );

/* Returns heap memory or null */
/* --------------------------- */
char *process_generic_output_system_string(
			char *select,
			char *value_folder_name,
			char *where,
			enum aggregate_level,
			enum aggregate_statistic,
			char *end_date,
			boolean accumulate,
			int foreign_attribute_name_list_length );

/* Returns static memory */
/* --------------------- */
char *process_generic_output_heading(
			LIST *foreign_attribute_name_list,
			char *datatype_unit,
			char *foreign_folder_unit,
			enum aggregate_level aggregate_level,
			char *time_attribute_name,
			boolean accumulate );

/* Returns static memory */
/* --------------------- */
char *process_generic_output_subtitle(
			char *value_folder_name,
			char *begin_date,
			char *end_date,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic );

/* PROCESS_GENERIC_VALUE_FOLDER */
/* ---------------------------- */
PROCESS_GENERIC_VALUE_FOLDER *process_generic_value_folder_calloc(
			void );

PROCESS_GENERIC_VALUE_FOLDER *process_generic_value_folder_fetch(
			char *value_folder_name,
			DICTIONARY *post_dictionary );

/* PROCESS_GENERIC_DATATYPE */
/* ------------------------ */
PROCESS_GENERIC_DATATYPE *process_generic_datatype_calloc(
			void );

PROCESS_GENERIC_DATATYPE *process_generic_datatype_fetch(
			char *datatype_folder_name,
			char *datatype_attribute_name,
			boolean exists_aggregation_sum,
			boolean exists_bar_graph,
			boolean exists_scale_graph_zero,
			boolean datatype_exists_units,
			DICTIONARY *post_dictionary );

char *process_generic_datatype_select(
			char *datatype_attribute_name,
			boolean exists_aggregation_sum,
			boolean exists_bar_graph,
			boolean exists_scale_graph_zero,
			boolean unit_datatype_folder );

char *process_generic_datatype_where(
			char *datatype_attribute_name,
			char *datatype_name );

char *process_generic_datatype_system_string(
			char *select,
			char *datatype_folder_name,
			char *where );

PROCESS_GENERIC_DATATYPE *process_generic_datatype_parse(
			char *input_buffer );

/* PROCESS_GENERIC_VALUE */
/* --------------------- */
PROCESS_GENERIC_VALUE *process_generic_value_calloc(
			void );

PROCESS_GENERIC_VALUE *process_generic_value_parse(
			double *accumulate_value,
			char *input,
			char *time_attribute,
			boolean unit_value_folder,
			boolean accumulate,
			enum aggregate_level aggregate_level );

/* PROCESS_GENERIC_PARAMETER */
/* ------------------------- */
PROCESS_GENERIC_PARAMETER *process_generic_parameter_calloc(
			void );

/* Never fails */
/* ----------- */
PROCESS_GENERIC_PARAMETER *process_generic_parameter_parse(
			char *output_medium_string,
			DICTIONARY *post_dictionary,
			boolean aggregation_sum,
			char *argv_0 );

/* PROCESS_GENERIC_FOREIGN_FOLDER */
/* ------------------------------ */
PROCESS_GENERIC_FOREIGN_FOLDER *process_generic_foreign_folder_fetch(
			char *foreign_folder_name,
			DICTIONARY *post_dictionary );

#endif
