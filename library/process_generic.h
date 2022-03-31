/* $APPASERVER_HOME/library/process_generic.h 				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver process_generic ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef PROCESS_GENERIC_H
#define PROCESS_GENERIC_H

/* Includes */
/* -------- */
#include "list.h"
#include "hash_table.h"
#include "folder.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"

/* Constants */
/* --------- */
#define HTML_TABLE_QUEUE_TOP_BOTTOM		500

/* Enumerated types */
/* ---------------- */
enum process_generic_medium {
			text_file,
			output_medium_stdout,
			table,
			spreadsheet,
			gracechart,
			output_medium_unknown };

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *value_folder_name;
	LIST *where_attribute_name_list;
	LIST *where_attribute_data_list;
	char *date_where;

	/* Process */
	/* ------- */
	char *data_where;
	char *where_clause;

} PROCESS_GENERIC_WHERE;

typedef struct
{
	char *process_set;
	char *process_name;
	char *output_medium_string;
	enum process_generic_medium output_medium;
	char *begin_date;
	char *end_date;
	enum aggregate_level aggregate_level;
	enum aggregate_statistic aggregate_statistic;
	boolean accumulate;
	char *email_address;
	char *units_converted;
	int days_to_average;
	pid_t process_id;
	int date_piece;
	int time_piece;
	int value_piece;
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
	boolean datatype_exists_unit;
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
	/* Input */
	/* ----- */
	char *foreign_folder_name;
	DICTIONARY *post_dictionary;
	boolean foreign_exists_unit;

	/* Process */
	/* ------- */
	FOLDER *folder;
	LIST *foreign_key_list;
	LIST *foreign_attribute_data_list;
	char *unit;
} PROCESS_GENERIC_FOREIGN_FOLDER;

typedef struct
{
	/* Input */
	/* ----- */
	char *value_folder_name;
	DICTIONARY *post_dictionary;

	/* Attributes */
	/* ---------- */
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

	LIST *folder_attribute_list;
	LIST *folder_attribute_primary_key_list;
	int primary_key_list_length;
	PROCESS_GENERIC_DATATYPE *datatype;
	PROCESS_GENERIC_FOREIGN_FOLDER *foreign_folder;
} PROCESS_GENERIC_VALUE_FOLDER;

typedef struct
{
	/* Input */
	/* ----- */
	char *process_set_name;
	char *login_name;
	char *output_medium_string;
	DICTIONARY *post_dictionary;

	/* Process */
	/* ------- */
	char *process_name;
	char *value_folder_name;
	PROCESS_GENERIC_VALUE_FOLDER *value_folder;
	PROCESS_GENERIC_PARAMETER *parameter;
	PROCESS_GENERIC_FOREIGN_FOLDER *foreign_folder;
	PROCESS_GENERIC_WHERE *process_generic_where;
	char *process_generic_date_where;
	char *process_generic_system_string;
	char *process_generic_heading;
	char *process_generic_subtitle;
} PROCESS_GENERIC;

/* PROCESS_GENERIC operations */
/* -------------------------- */
PROCESS_GENERIC *process_generic_fetch(
			char *process_set_name,
			char *process_name,
			char *login_name,
			char *output_medium_string,
			DICTIONARY *post_dictionary );

char *process_generic_process_name(
			char *process_set_name,
			DICTIONARY *post_dictionary );

char *process_generic_units_label(
			char *units,
			char *units_converted,
			enum aggregate_statistic );

/* Returns static memory */
/* --------------------- */
char *process_generic_date_where(
			char *date_attribute_name,
			char *begin_date,
			char *end_date );

PROCESS_GENERIC *process_generic_calloc(
			void );

char *process_generic_value_folder_name(
			char *process_name,
			char *process_set_name );

/* Returns static memory */
/* --------------------- */
char *process_generic_date_where(
			char *date_attribute_name,
			char *begin_date,
			char *end_date );

/* Returns heap memory or null */
/* --------------------------- */
char *process_generic_where(
			LIST *foreign_attribute_name_list,
			LIST *foreign_attribute_data_list,
			char *process_generic_date_where );

char *process_generic_select(
			LIST *value_attribute_name_list,
			char *value_attribute_name );

/* Returns heap memory or null */
/* --------------------------- */
char *process_generic_system_string(
			char *select,
			char *value_folder_name,
			char *where,
			enum aggregate_level,
			enum aggregate_statistic,
			boolean accumulate,
			int date_piece,
			int time_piece,
			int value_piece,
			char *end_date,
			char *datatype_unit,
			char *foreign_folder_unit,
			char *units_converted );

/* Returns static memory */
/* --------------------- */
char *process_generic_heading(
			LIST *primary_key_list,
			char *value_attribute_name,
			char *datatype_unit,
			char *foreign_folder_unit,
			char *units_converted,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			boolean accumulate );

/* Returns static memory */
/* --------------------- */
char *process_generic_subtitle(
			char *value_folder_name,
			char *begin_date,
			char *end_date,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *additional_message );

/* PROCESS_GENERIC_VALUE_FOLDER operations */
/* --------------------------------------- */
PROCESS_GENERIC_VALUE_FOLDER *process_generic_value_folder_calloc(
			void );

PROCESS_GENERIC_VALUE_FOLDER *process_generic_value_folder_fetch(
			char *value_folder_name,
			DICTIONARY *post_dictionary );

PROCESS_GENERIC_VALUE_FOLDER *process_generic_value_folder_parse(
			char *input );

/* PROCESS_GENERIC_DATATYPE operations */
/* ----------------------------------- */
PROCESS_GENERIC_DATATYPE *process_generic_datatype_calloc(
			void );

PROCESS_GENERIC_DATATYPE *process_generic_datatype_fetch(
			char *datatype_folder_name,
			char *datatype_attribute_name,
			boolean exists_aggregation_sum,
			boolean exists_bar_graph,
			boolean exists_scale_graph_zero,
			boolean datatype_exists_unit,
			DICTIONARY *post_dictionary );

char *process_generic_datatype_select(
			char *datatype_attribute_name,
			boolean exists_aggregation_sum,
			boolean exists_bar_graph,
			boolean exists_scale_graph_zero,
			boolean unit_datatype_folder );

/* Safely returns heap memory */
/* -------------------------- */
char *process_generic_datatype_where(
			char *datatype_attribute_name,
			char *datatype_name );

char *process_generic_datatype_system_string(
			char *select,
			char *datatype_folder_name,
			char *where );

PROCESS_GENERIC_DATATYPE *process_generic_datatype_parse(
			char *input_buffer );

/* PROCESS_GENERIC_VALUE operations */
/* -------------------------------- */
PROCESS_GENERIC_VALUE *process_generic_value_calloc(
			void );

PROCESS_GENERIC_VALUE *process_generic_value_parse(
			double *accumulate_value,
			char *input,
			char *time_attribute,
			boolean unit_value_folder,
			boolean accumulate,
			enum aggregate_level aggregate_level );

/* PROCESS_GENERIC_PARAMETER operations */
/* ------------------------------------ */
PROCESS_GENERIC_PARAMETER *process_generic_parameter_calloc(
			void );

PROCESS_GENERIC_PARAMETER *process_generic_parameter_parse(
			char *output_medium_string,
			DICTIONARY *post_dictionary,
			boolean aggregation_sum,
			LIST *value_folder_primary_key_list,
			char *value_folder_date_attribute_name,
			char *value_folder_time_attribute_name );

/* PROCESS_GENERIC_FOREIGN_FOLDER operations */
/* ----------------------------------------- */
PROCESS_GENERIC_FOREIGN_FOLDER *process_generic_foreign_folder_fetch(
			char *foreign_folder_name,
			DICTIONARY *post_dictionary,
			boolean foreign_exists_unit );

/* PROCESS_GENERIC_WHERE operations */
/* -------------------------------- */
PROCESS_GENERIC_WHERE *process_generic_where_calloc(
			void );

PROCESS_GENERIC_WHERE *process_generic_where_new(
			char *value_folder_name,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			LIST *folder_attribute_primary_list,
			char *date_where );

/* Returns heap memory or null */
/* --------------------------- */
char *process_generic_where_clause(
			char *data_where,
			char *date_where );

#endif
