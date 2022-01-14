/* $APPASERVER_HOME/library/query.h					*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef QUERY_H
#define QUERY_H

#include "string.h"
#include "folder.h"
#include "relation.h"
#include "list.h"
#include "dictionary.h"
#include "boolean.h"
#include "piece.h"
#include "prompt_recursive.h"
#include "role.h"
#include "role_folder.h"
#include "security.h"
#include "row_security.h"
#include "date_convert.h"
#include "folder.h"

/* Constants */
/* --------- */
#define QUERY_EDIT_TABLE_MAX_ROWS		500
#define QUERY_FROM_STARTING_LABEL		"from_"
#define QUERY_TO_STARTING_LABEL			"to_"
#define QUERY_DROP_DOWN_ORIGINAL_STARTING_LABEL	"original_"
#define QUERY_RELATION_OPERATOR_STARTING_LABEL	"relation_operator_"
#define QUERY_LOGIN_NAME_ATTRIBUTE_NAME		"login_name"

/* Enumerated types */
/* ---------------- */
enum relational_operator {	equals,
				not_equal,
				not_equal_or_null,
				less_than,
				less_than_equal_to,
				greater_than,
				greater_than_equal_to,
				between,
				begins,
				contains,
				not_contains,
				query_or,
				is_null,
				not_null,
				relational_operator_empty,
				relational_operator_unknown };

/* Structures */
/* ---------- */
typedef struct
{
	char *login_name;
	enum date_convert_format date_convert_format;
} QUERY_DATE_CONVERT;

typedef struct
{
	/* Input */
	/* ----- */
	char *folder_name;
	char *attribute_name;
	char *datatype_name;
	char *data;

	/* Process */
	/* ------- */
	char *escaped_replaced_data;
} QUERY_DATA;

/* QUERY_DATA operations */
/* --------------------- */
QUERY_DATA *query_data_calloc(
			void );

LIST *query_data_list(
			LIST *foreign_key_list,
			LIST *folder_attribute_list,
			LIST *query_drop_down_row_data_string_list );

QUERY_DATA *query_data_new(
			char *folder_name,
			char *attribute_name,
			char *datatype_name,
			char *data );

/* Returns heap memory or data */
/* --------------------------- */
char *query_data_date_international(
			char *data,
			char *datatype_name );

QUERY_DATA *query_data_escaped(
			DICTIONARY *dictionary,
			char *attribute_name,
			char *datatype_name,
			int dictionary_offset,
			char *starting_label,
			char *dictionary_prepend_folder_name );

typedef struct 
{
	LIST *attribute_name_list;
	LIST *data_list_list;
} QUERY_OR_SEQUENCE;

typedef struct
{
	/* Attributes */
	/* ---------- */
	LIST *one_primary_key_list;
	LIST *foreign_key_list;
	LIST *many_folder_attribute_list;
	DICTIONARY *drillthru_dictionary;
	int index;

	/* Process */
	/* ------- */
	char *key;
	char *data_list_string;
	LIST *data_string_list;
	LIST *query_data_list;
} QUERY_DROP_DOWN_ROW;

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *many_folder_name;
	char *one_folder_name;
	LIST *one_primary_key_list;
	LIST *foreign_key_list;
	DICTIONARY *drillthru_dictionary;
	int highest_index;

	/* Process */
	/* ------- */
	enum relational_operator relational_operator;
	LIST *row_list;
} QUERY_DROP_DOWN;

typedef struct
{
	/* Process */
	/* ------- */
	char *key;
	char *data_list_string;
	LIST *data_string_list;
	LIST *query_data_list;
} QUERY_EDIT_TABLE_DROP_DOWN_ROW;

/* QUERY_EDIT_TABLE_DROP_DOWN_ROW operations */
/* ----------------------------------------- */
QUERY_EDIT_TABLE_DROP_DOWN_ROW *query_edit_table_drop_down_row_calloc(
			void );

LIST *query_edit_table_drop_down_row_list(
			LIST *foreign_key_list,
			LIST *folder_attribute_append_isa_list,
			DICTIONARY *query_dictionary,
			int dictionary_string_list_highest_index );

QUERY_EDIT_TABLE_DROP_DOWN_ROW *query_edit_table_drop_down_row_new(
			LIST *foreign_key_list,
			LIST *folder_attribute_append_isa_list,
			DICTIONARY *query_dictionary,
			int index );

/* Returns static memory */
/* --------------------- */
char *query_edit_table_drop_down_row_key(
			LIST *foreign_key_list,
			int index );

/* Returns query_dictionary->hash_table->other_data */
/* ------------------------------------------------ */
char *query_edit_table_drop_down_row_data_list_string(
			char *query_edit_table_drop_down_row_key,
			DICTIONARY *query_dictionary );

LIST *query_edit_table_drop_down_row_data_string_list(
			char *query_edit_table_drop_down_row_data_list_string );

LIST *query_edit_table_drop_down_row_data_list(
			LIST *foreign_key_list,
			LIST *folder_attribute_append_isa_list,
			LIST *query_edit_table_drop_down_row_data_string_list );

typedef struct
{
	/* Process */
	/* ------- */
	int highest_index;
	LIST *row_list;
} QUERY_EDIT_TABLE_DROP_DOWN;

/* QUERY_EDIT_TABLE_DROP_DOWN operations */
/* ------------------------------------- */
QUERY_EDIT_TABLE_DROP_DOWN *query_edit_table_drop_down_calloc(
			void );

LIST *query_edit_table_drop_down_list(
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *query_dictionary );

QUERY_EDIT_TABLE_DROP_DOWN *query_edit_table_drop_down_new(
			LIST *foreign_key_list,
			DICTIONARY *query_dictionary );

typedef struct
{
	/* Process */
	/* ------- */
	int relation_mto1_isa_list_length;
	LIST *query_edit_table_drop_down_list;
	char *query_edit_table_drop_down_list_where;
	LIST *prompt_recursive_drop_down_list;
	char *prompt_recursive_drop_down_list_where;
	LIST *query_attribute_list;
	char *query_attribute_list_where;
	char *query_join_where;
	char *query_edit_table_where;
} QUERY_EDIT_TABLE_WHERE;

/* QUERY_EDIT_TABLE_WHERE operations */
/* --------------------------------- */
QUERY_EDIT_TABLE_WHERE *query_edit_table_where_calloc(
			void );

QUERY_EDIT_TABLE_WHERE *query_edit_table_where_new(
			char *folder_name,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_mto1_isa_list,
			PROMPT_RECURSIVE *prompt_recursive,
			ROW_SECURITY_ROLE *row_security_role,
			char *security_entity_where,
			DICTIONARY *query_dictionary );

/* Returns heap memory or null */
/* --------------------------- */
char *query_edit_table_where(
			char *query_edit_table_drop_down_list_where,
			char *prompt_recursive_drop_down_list_where,
			char *query_attribute_list_where,
			char *query_join_where,
			char *security_entity_where );

typedef struct
{
	char *attribute_name;
	char *folder_name;
	char *datatype_name;
	enum relational_operator relational_operator;
	QUERY_DATA *from_data;
	QUERY_DATA *to_data;
	int primary_key_index;
} QUERY_ATTRIBUTE;

typedef struct
{
	/* Process */
	/* ------- */
	char *column_string;
} QUERY_SELECT;

/* QUERY_SELECT operations */
/* ----------------------- */
QUERY_SELECT *query_select_calloc(
			void );

LIST *query_edit_table_select_list(
			LIST *folder_attribute_append_isa_list,
			LIST *ignore_select_attribute_name_list,
			LIST *exclude_lookup_attribute_name_list,
			int relation_mto1_isa_length,
			QUERY_DATE_CONVERT *query_date_convert,
			ROW_SECURITY_ROLE *row_security_role );

LIST *query_widget_select_list(
			LIST *primary_key_list,
			LIST *folder_attribute_list,
			QUERY_DATE_CONVERT *query_date_convert );

/* Always succeeds */
/* --------------- */
QUERY_SELECT *query_select_new(
			char *folder_name,
			char *attribute_name,
			boolean attribute_is_date,
			boolean attribute_is_date_time,
			QUERY_DATE_CONVERT *query_date_convert );

/* Safely returns heap memory */
/* -------------------------- */
char *query_select_column_string(
			char *folder_name,
			char *attribute_name,
			boolean attribute_is_date,
			boolean attribute_is_date_time,
			enum date_convert_format query_date_convert_format );

/* Returns heap memory or null */
/* --------------------------- */
char *query_select_list_string(
			LIST *select_list );

/* Safely returns heap memory */
/* -------------------------- */
char *query_select_clause(
			char *query_select_list_string );

typedef struct
{
	/* Process */
	/* ------- */
	LIST *query_widget_select_list;
	char *query_select_list_string;
	char *query_select_clause;
	char *query_from_clause;
	char *query_where_clause;
	char *query_order_clause;
	LIST *delimited_list;
} QUERY_ISA_WIDGET;

/* QUERY_ISA_WIDGET operations */
/* --------------------------- */
QUERY_ISA_WIDGET *query_isa_widget_calloc(
			void );

/* Always succeeds */
/* --------------- */
QUERY_ISA_WIDGET *query_isa_widget_new(
			char *one2m_isa_folder_name,
			LIST *primary_key_list,
			LIST *folder_attribute_primary_list,
			char *login_name,
			char *security_entity_where );

/* Returns heap memory */
/* ------------------- */
char *query_join_where(
			char *folder_name,
			LIST *primary_key_list,
			LIST *mto1_isa_related_folder_list );

/* Returns static memory */
/* --------------------- */
char *query_isa_related_join(
			char *folder_name,
			LIST *primary_key_list,
			char *isa_folder_name,
			LIST *isa_foreign_attribute_name_list );

/* Returns heap memory */
/* ------------------- */
char *query_where_clause(
			char *query_output_drop_down_where,
			char *query_output_attribute_where,
			char *query_output_join_where,
			char *attribute_not_null_join );

boolean query_attribute_date_time_between(
			QUERY_ATTRIBUTE **date_between_attribute,
			QUERY_ATTRIBUTE **time_between_attribute,
			LIST *query_attribute_list );

/* Returns static memory */
/* --------------------- */
char *query_attribute_between_date_time_where(
			char *date_attribute_name,
			char *time_attribute_name,
			QUERY_DATA *date_from_data,
			QUERY_DATA *time_from_data,
			QUERY_DATA *date_to_data,
			QUERY_DATA *time_to_data,
			char *folder_table_name );

char *query_primary_key_where_clause(
			LIST *primary_key_list,
			char *input_buffer,
			char delimiter );


/* Returns heap memory or null */
/* --------------------------- */
char *query_drop_down_list_where(
			LIST *drop_down_list,
			int relation_mto1_isa_list_length );

/* Returns heap memory or null */
/* --------------------------- */
char *query_attribute_list_where(
			LIST *query_attribute_list );

/* QUERY_DROP_DOWN operations */
/* -------------------------- */
LIST *query_drop_down_list(
			LIST *exclude_attribute_name_list /* out */,
			char *many_folder_name,
			LIST *many_folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary );

QUERY_DROP_DOWN *query_drop_down_calloc(
			void );

/* Always succeeds */
/* --------------- */
QUERY_DROP_DOWN *query_drop_down_new(
			char *many_folder_name,
			LIST *many_folder_attribute_list,
			char *one_folder_name,
			LIST *one_primary_key_list,
			LIST *foreign_key_list,
			DICTIONARY *drillthru_dictionary,
			int highest_index );

char *query_drop_down_operator_key(
			LIST *foreign_key_list );

/* QUERY_ROW operations */
/* -------------------- */
LIST *query_row_dictionary_list(
			char *application_name,
			char *select_clause,
			char *from_clause,
			char *where_clause,
			char *order_clause,
			int max_rows,
			LIST *append_isa_attribute_list,
			char *login_name );

boolean query_get_drop_down_dictionary_data(
			char **data,
			DICTIONARY *dictionary, 
			char *primary_key,
			int dictionary_offset );

char *query_dictionary_operator_name(
			char *key,
			DICTIONARY *dictionary );

/* QUERY_ATTRIBUTE operations */
/* -------------------------- */
LIST *query_attribute_list(
			LIST *folder_attribute_list,
			DICTIONARY *dictionary,
			LIST *exclude_attribute_name_list );

/* Returns heap memory */
/* ------------------- */
char *query_attribute_list_where(
			LIST *query_attribute_list );

/* Returns static memory */
/* --------------------- */
char *query_attribute_operator_key(
			char *operator_name );

LIST *query_attribute_list(
			LIST *append_isa_attribute_list,
			DICTIONARY *dictionary,
			LIST *exclude_attribute_name_list );

enum relational_operator query_relational_operator(
			char *operator_name,
			char *datatype_name );

QUERY_ATTRIBUTE *query_attribute_new(
			char *attribute_name,
			char *folder_name,
			char *datatype_name,
			enum relational_operator,
			QUERY_DATA *from_data,
			QUERY_DATA *to_data,
			int primary_key_index );

boolean query_attribute_exists(
			LIST *query_attribute_list,
			char *attribute_name );

char *query_process_where_clause(
			char **drop_down_where_clause,
			char **attribute_where_clause,
			LIST *query_drop_down_list,
			LIST *query_attribute_list,
			char *application_name,
			char *folder_name );

char *query_get_drop_down_where_clause(
			LIST *query_drop_down_list,
			char *application_name,
			char *folder_name );

char *query_get_attribute_where_clause(
			LIST *query_attribute_list,
			char *application_name,
			boolean combine_date_time );

char *query_get_simple_where_clause(
			FOLDER *folder,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			char *login_name );

char *query_append_where_clause_related_join(
			char *application_name,
			char *source_where_clause,
			LIST *primary_key_list,
			LIST *related_attribute_name_list,
			char *folder_name,
			char *related_folder_name );

char *query_append_folder_name(
			char *folder_name,
			char *append_folder_name );

char *query_between_date_time_where(
			char *date_column_name,
			char *time_column_name,
			char *begin_date,
			char *begin_time,
			char *end_date,
			char *end_time,
			char *application_name,
			char *folder_name );

LIST *query_get_drop_down_data_list(
			LIST *attribute_name_list,
			DICTIONARY *dictionary,
			int index );

char *query_system_string(
			char *select_clause,
			char *from_clause,
			char *where_clause,
			char *order_clause,
			int max_rows );

char *query_drop_down_list_display(
			char *folder_name,
			LIST *query_drop_down_list );

char *query_attribute_list_display(
			char *folder_name,
			LIST *query_attribute_list );

char *query_relational_operator_display(
			enum relational_operator );

boolean query_date_time_between_attributes(
			QUERY_ATTRIBUTE **date_between_attribute,
			QUERY_ATTRIBUTE **time_between_attribute,
			LIST *query_attribute_list,
			boolean combine_date_time );

/* QUERY_OR_SEQUENCE operations */
/* ---------------------------- */
QUERY_OR_SEQUENCE *query_or_sequence_new(
			LIST *attribute_name_list );

boolean query_or_sequence_set_data_list_string(
			LIST *data_list_list,
			char *data_list_string );

boolean query_or_sequence_set_data_list(
			LIST *data_list_list,
			LIST *data_list );

char *query_or_sequence_where_clause(
			LIST *attribute_name_list,
			LIST *data_list_list,
			boolean with_and_prefix );

/* QUERY detail operations */
/* ----------------------- */
/*
QUERY *query_detail_new(
			LIST *where_attribute_data_list,
			char *folder_name,
			char *role_name,
			char *login_name,
			LIST *ignore_select_attribute_name_list,
			char *attribute_not_null_join,
			char *attribute_not_null_folder_name );
*/

char *query_folder_where_clause(
			FOLDER *folder,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			LIST *append_isa_attribute_list );

char *query_related_join_where(
			char *application_name,
			char *source_where_clause,
			LIST *primary_key_list,
			LIST *foreign_attribute_name_list,
			char *folder_name,
			char *related_folder_name );

/* Returns heap memory */
/* ------------------- */
char *query_join_where_clause(
			LIST *primary_key_list,
			LIST *related_attribute_name_list,
			char *folder_name,
			char *related_folder_name );

/* QUERY_DATE_CONVERT operations */
/* ----------------------------- */
QUERY_DATE_CONVERT *query_date_convert_calloc(
			void );

/* Always succeeds */
/* --------------- */
QUERY_DATE_CONVERT *query_date_convert_new(
			char *login_name );

/* Returns static memory */
/* --------------------- */
char *query_date_convert_select_string(
			char *attribute_name,
			boolean attribute_is_date,
			boolean attribute_is_date_time,
			enum date_convert_format date_convert_format );

char *query_drop_down_data_where(
			char *folder_name,
			char *attribute_name,
			char *data );

/* QUERY_DROP_DOWN_ROW operations */
/* ------------------------------ */
LIST *query_drop_down_row_list(
			LIST *one_primary_key_list,
			LIST *foreign_key_list,
			LIST *many_folder_attribute_list,
			DICTIONARY *drillthru_dictionary,
			int highest_index );

QUERY_DROP_DOWN_ROW *query_drop_down_row_new(
			LIST *one_primary_key_list,
			LIST *foreign_key_list,
			LIST *many_folder_attribute_list,
			DICTIONARY *drillthru_dictionary,
			int index );

QUERY_DROP_DOWN_ROW *query_drop_down_row_calloc(
			void );

/* Returns static memory */
/* --------------------- */
char *query_drop_down_row_key(
			LIST *foreign_key_list,
			int index );

char *query_drop_down_row_data_list_string(
			char *key,
			DICTIONARY *query_dictionary );

LIST *query_drop_down_row_data_string_list(
			char *data_list_string );

/* QUERY public operations */
/* ----------------------- */
char *query_yes_no_operator_name(
			char *attribute_name );

char *query_operator_name(
			char *operator_key,
			DICTIONARY *dictionary );

enum relational_operator
	query_relational_operator(
			char *operator_name,
			char *datatype_name );

/* Returns program memory */
/* ---------------------- */
char *query_operator_character_string(
			char *operator_string );


LIST *query_select_name_list(
			LIST *append_isa_attribute_list,
			LIST *ignore_select_attribute_name_list,
			LIST *lookup_attribute_exclude_name_list );

/* Returns heap memory */
/* ------------------- */
char *query_from(
			char *query_folder_name,
			LIST *mto1_isa_related_folder_list,
			char *attribute_not_null_folder_name );

/* Returns heap memory */
/* ------------------- */
char *query_order(
			char *query_folder_name,
			LIST *primary_key_list,
			LIST *append_isa_attribute_list,
			DICTIONARY *sort_dictionary );

/* Sample: ssort_button_assend|descend_full_name^street_address */
/* ------------------------------------------------------------ */
/* Returns heap memory						*/
/* ------------------------------------------------------------ */
char *query_sort_prefix_direction_attribute_index(
			DICTIONARY *sort_dictionary );

/* Returns heap memory */
/* ------------------- */
char *query_attribute_name_list_order(
			char *folder_name,
			LIST *attribute_name_list,
			char *descending_label );

char *query_isa_join_where(
			char *folder_name,
			LIST *primary_key_list,
			LIST *relation_mto1_isa_list );

char *query_output_order(
			char *mto1_folder_name,
			LIST *primary_key_list,
			DICTIONARY *sort_dictionary );

/* Returns heap memory */
/* ------------------- */
char *query_data_where(
			char *folder_name,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			LIST *folder_attribute_list );

/* QUERY (Primary) operations */
/* -------------------------- */
LIST *query_primary_delimited_fetch_list(
			char *folder_name,
			LIST *primary_key_list,
			LIST *folder_attribute_primary_list,
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where );

/* QUERY (Output) operations */
/* ------------------------- */
LIST *query_dictionary_list(
			char *select_clause,
			LIST *select_list,
			char *from_clause,
			char *where_clause,
			char *order_clause,
			int max_rows );

LIST *query_delimited_list(
			char *select_clause,
			char *from_clause,
			char *where_clause,
			char *order_clause,
			int max_rows );

LIST *query_system_dictionary_list(
			char *system_string,
			LIST *select_list );

LIST *query_primary_delimited_list(
			char *folder_table_name,
			LIST *primary_key_list,
			LIST *foreign_key_list,
			LIST *foreign_data_list );

typedef struct
{
	/* Process */
	/* ------- */
	LIST *exclude_attribute_name_list;
	LIST *query_drop_down_list;
	LIST *query_attribute_list;
	char *query_drop_down_list_where;
	char *query_drop_down_data_where;
	char *query_attribute_list_where;
	char *query_widget_where_evaluate;
	LIST *query_prompt_recursive_drop_down_list;
	LIST *query_search_replace_where_drop_down_list;
	char *query_join_where;
	char *query_related_join;
	char *query_primary_key_where_clause;
} QUERY_EDIT_TABLE_WHERE;

/* QUERY_EDIT_TABLE_WHERE operations */
/* --------------------------------- */
QUERY_EDIT_TABLE_WHERE *query_edit_table_where_calloc(
			void );

QUERY_EDIT_TABLE_WHERE *query_edit_table_where_new(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_isa_list,
			char *security_entity_where,
			DICTIONARY *query_dictionary,
			ROW_SECURITY_ROLE *row_security_role );

typedef struct
{
	/* Process */
	/* ------- */
	LIST *select_list;
	char *select_list_string;
	char *select_clause;
	char *from_clause;
	QUERY_EDIT_TABLE_WHERE *query_edit_table_where;
	char *order_clause;
	LIST *dictionary_list;
} QUERY_EDIT_TABLE;

/* QUERY_EDIT_TABLE operations */
/* --------------------------- */
QUERY_EDIT_TABLE *query_edit_table_calloc(
			void );

QUERY_EDIT_TABLE *query_edit_table_new(
			char *folder_name,
			char *login_name,
			char *security_entity_where,
			LIST *relation_join_one2m_list,
			LIST *ignore_select_attribute_name_list,
			LIST *exclude_lookup_attribute_name_list,
			LIST *folder_attribute_append_isa_list,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			ROW_SECURITY_ROLE *row_security_role );

char *query_edit_table_from_clause(
			char *folder_name,
			LIST *relation_mto1_isa_list,
			ROW_SECURITY_ROLE *row_security_role );

typedef struct
{
	/* Process */
	/* ------- */
	LIST *exclude_attribute_name_list;
	LIST *query_drop_down_list;
	char *query_drop_down_list_where;
	LIST *query_attribute_list;
	char *query_attribute_list_where;
	char *query_widget_where;
	char *clause;
} QUERY_WIDGET_WHERE;

/* QUERY_WIDGET_WHERE operations */
/* ----------------------------- */
QUERY_WIDGET_WHERE *query_widget_where_calloc(
			void );

QUERY_WIDGET_WHERE *query_widget_where_new(
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *security_entity_where,
			DICTIONARY *drillthru_dictionary );

/* Safely returns heap memory */
/* -------------------------- */
char *query_widget_where(
			char *query_drop_down_where,
			char *query_attribute_list_where,
			char *security_entity_where );

/* Safely returns heap memory */
/* -------------------------- */
char *query_widget_where_clause(
			char *query_widget_where );

char *query_widget_where_string(
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *security_entity_where,
			DICTIONARY *drillthru_dictionary );

typedef struct
{
	/* Process */
	/* ------- */
	LIST *select_list;
	char *query_select_list_string;
	char *query_select_clause;
	char *query_from_clause;
	QUERY_WIDGET_WHERE *where;
	char *query_order_clause;
	LIST *delimited_list;
} QUERY_WIDGET;

/* QUERY_WIDGET operations */
/* ----------------------- */
QUERY_WIDGET *query_widget_calloc(
			void );

QUERY_WIDGET *query_widget_new(
			char *widget_folder_name,
			char *login_name,
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *security_entity_where,
			DICTIONARY *drillthru_dictionary );

typedef struct
{
	/* stub */
} QUERY_FROM;

/* QUERY_FROM operations */
/* --------------------- */


/* Safely returns heap memory */
/* -------------------------- */
char *query_from_clause(
			char *folder_name );

/* Safely returns heap memory */
/* -------------------------- */
char *query_edit_table_from_clause(
			char *folder_name,
			LIST *relation_mto1_isa_list,
			ROW_SECURITY_ROLE *row_security_role );

typedef struct
{
	/* stub */
} QUERY_ORDER;

/* QUERY_ORDER operations */
/* ---------------------- */

/* Returns heap memory */
/* ------------------- */
char *query_order_clause(
			char *query_select_list_string,
			DICTIONARY *sort_dictionary );

char *query_order_key_list_clause(
			LIST *key_list );

typedef struct
{
	/* stub */
} QUERY_WHERE;

/* QUERY_WHERE operations */
/* ---------------------- */

/* Returns heap memory */
/* ------------------- */
char *query_where_clause(
			char *where_string );

#endif
