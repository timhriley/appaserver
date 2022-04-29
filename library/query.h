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
#include "role.h"
#include "role_folder.h"
#include "security.h"
#include "date_convert.h"
#include "folder.h"

/* Constants */
/* --------- */

/* Enumerated types */
/* ---------------- */
enum query_relation {		equal,
				not_equal,
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
				not_null};

/* Structures */
/* ---------- */
typedef struct
{
	/* Process */
	/* ------- */
	char *key;
	char *yes_no_key;
	char *operator_fetch;
	enum query_relation query_relation_enum;
	char *character_string;
} QUERY_RELATION;

/* QUERY_RELATION operators */
/* ------------------------ */
QUERY_RELATION *query_relation_calloc(
			void );

QUERY_RELATION *query_relation_new(
			char *attribute_name,
			char *datatype_name,
			DICTIONARY *dictionary );

/* Returns static memory */
/* --------------------- */
char *query_relation_key(
			char *attribute_name,
			char *query_relation_starting_label );

/* Returns program memory */
/* ---------------------- */
char *query_relation_yes_no_key(
			char *attribute_name );

/* Returns dictionary->hash_table->other_data or null */
/* -------------------------------------------------- */
char *query_relation_operator_fetch(
			char *query_relation_key,
			char *query_relation_yes_no_key,
			DICTIONARY *dictionary );

enum query_relation query_relation_enum(
			char *query_relation_operator_fetch,
			char *datatype_name );

/* Returns program memory */
/* ---------------------- */
char *query_relation_character_string(
			enum query_relation query_relation_enum );

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *folder_name;
	char *attribute_name;
	char *datatype_name;

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

QUERY_DATA *query_data_fetch(
			char *folder_name,
			char *attribute_name,
			char *datatype_name,
			DICTIONARY *dictionary );

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

/* Returns static memory */
/* --------------------- */
char *query_data_is_null_where(
			char *attribute_full_attribute_name );

/* Returns static memory */
/* --------------------- */
char *query_data_not_null_where(
			char *attribute_full_attribute_name );

/* Returns static memory */
/* --------------------- */
char *query_data_or_where(
			char *attribute_full_attribute_name,
			boolean attribute_is_number,
			char *escaped_replaced_data );

/* Returns static memory */
/* --------------------- */
char *query_data_begins_where(
			char *attribute_full_attribute_name,
			char *escaped_replaced_data );

/* Returns static memory */
/* --------------------- */
char *query_data_contains_where(
			char *attribute_full_attribute_name,
			char *escaped_replaced_data );

/* Returns static memory */
/* --------------------- */
char *query_data_not_contains_where(
			char *attribute_full_attribute_name,
			char *escaped_replaced_data );

/* Returns static memory */
/* --------------------- */
char *query_data_not_equal_where(
			char *full_attribute_name,
			boolean attribute_is_number,
			char *escaped_replaced_data );

/* Returns static memory */
/* --------------------- */
char *query_data_number_where(
			char *full_attribute_name,
			char *character_string,
			char *escaped_replaced_data );

/* Returns static memory */
/* --------------------- */
char *query_data_string_where(
			char *full_attribute_name,
			char *character_string,
			char *escaped_replaced_data );

typedef struct
{
	/* Attribute */
	/* --------- */
	char *folder_name;

	/* Process */
	/* ------- */
	char *date_attribute_base;
	char *time_attribute_name;
	QUERY_DATA *from_date;
	QUERY_DATA *from_time;
	QUERY_DATA *to_date;
	QUERY_DATA *to_time;
	QUERY_DATA *from;
	QUERY_DATA *to;
	char *date_time_where;
	char *non_date_time_where;
	char *where_string;
} QUERY_BETWEEN_DATA;

/* QUERY_BETWEEN_DATA operations */
/* ----------------------------- */
QUERY_BETWEEN_DATA *query_between_data_calloc(
			void );

QUERY_BETWEEN_DATA *query_between_data_new(
			char *attribute_name,
			char *datatype_name,
			DICTIONARY *dictionary,
			char *application_name,
			char *folder_name,
			int relation_mto1_isa_list_length );

/* Returns heap memory or null */
/* --------------------------- */
char *query_between_data_date_attribute_base(
			char *attribute_name,
			boolean is_date_or_time );

/* Returns heap memory or null */
/* --------------------------- */
char *query_between_data_time_attribute_name(
			char *query_between_data_attribute_base );


QUERY_DATA *query_between_data_from_date(
			char *attribute_name,
			char *datatype_name,
			DICTIONARY *dictionary );

QUERY_DATA *query_between_data_from_time(
			char *query_between_data_time_attribute_name,
			DICTIONARY *dictionary );

QUERY_DATA *query_between_data_to_date(
			char *attribute_name,
			char *datatype_name,
			DICTIONARY *dictionary );

QUERY_DATA *query_between_data_to_time(
			char *query_between_data_time_attribute_name,
			DICTIONARY *dictionary );

QUERY_DATA *query_between_data_from(
			char *attribute_name,
			char *datatype_name,
			DICTIONARY *dictionary );

QUERY_DATA *query_between_data_to(
			char *attribute_name,
			char *datatype_name,
			DICTIONARY *dictionary );

/* Returns static memory or null */
/* ----------------------------- */
char *query_between_data_date_time_where(
			QUERY_DATA *from_date,
			QUERY_DATA *from_time,
			QUERY_DATA *to_date,
			QUERY_DATA *to_time,
			char *folder_table_name,
			int relation_mto1_isa_list_length );

/* Returns static memory or null */
/* ----------------------------- */
char *query_between_data_non_date_time_where(
			QUERY_DATA *data_from,
			QUERY_DATA *data_to,
			char *datatype_name,
			char *folder_table_name,
			int relation_mto1_isa_list_length );

/* Returns heap memory or null */
/* --------------------------- */
char *query_between_data_where_string(
			char *query_between_data_date_time_where,
			char *query_between_data_where );

typedef struct
{
	char *login_name;
	enum date_convert_format date_convert_format;
} QUERY_DATE_CONVERT;

typedef struct 
{
	LIST *attribute_name_list;
	LIST *data_list_list;
} QUERY_OR_SEQUENCE;

typedef struct
{
	/* Process */
	/* ------- */
	char *key;
	char *data_list_string;
	LIST *data_string_list;
	LIST *query_data_list;
} QUERY_DROP_DOWN_ROW;

typedef struct
{
	/* Input */
	char *many_folder_name;

	/* Process */
	/* ------- */
	int highest_index;
	LIST *row_list;
} QUERY_DROP_DOWN;

/* QUERY_DROP_DOWN operations */
/* -------------------------- */
LIST *query_drop_down_list(
			char *many_folder_name,
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *dictionary );

QUERY_DROP_DOWN *query_drop_down_calloc(
			void );

/* Always succeeds */
/* --------------- */
QUERY_DROP_DOWN *query_drop_down_new(
			char *many_folder_name,
			LIST *folder_attribute_list,
			LIST *foreign_key_list,
			DICTIONARY *dictionary,
			int highest_index );

/* Public */
/* ------ */
char *query_drop_down_list_where(
			char *application_name,
			LIST *query_drop_down_list,
			int relation_mto1_isa_list_length );

typedef struct
{
	/* Process */
	/* ------- */
	int relation_mto1_isa_list_length;
	LIST *query_drop_down_list;
	char *query_drop_down_list_where;
	LIST *prompt_recursive_drop_down_list;
	char *prompt_recursive_drop_down_list_where;
	LIST *query_attribute_list;
	char *query_attribute_list_where;
	char *query_join_where;
	char *string;
} QUERY_EDIT_TABLE_WHERE;

/* QUERY_EDIT_TABLE_WHERE operations */
/* --------------------------------- */
QUERY_EDIT_TABLE_WHERE *query_edit_table_where_calloc(
			void );

/* Always succeeds */
/* --------------- */
QUERY_EDIT_TABLE_WHERE *query_edit_table_where_new(
			char *application_name,
			char *folder_name,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_mto1_isa_list,
			char *security_entity_where,
			DICTIONARY *query_dictionary,
			RELATION *row_security_role_relation );

/* Returns heap memory or null */
/* --------------------------- */
char *query_edit_table_where_string(
			char *query_drop_down_list_where,
			char *query_attribute_list_where,
			char *query_join_where,
			char *security_entity_where );

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *application_name;
	char *folder_name;
	int relation_mto1_isa_list_length;

	/* Process */
	/* ------- */
	QUERY_RELATION *query_relation;
	QUERY_BETWEEN_DATA *query_between_data;
	QUERY_DATA *query_data;
} QUERY_ATTRIBUTE;

/* QUERY_ATTRIBUTE operations */
/* -------------------------- */
QUERY_ATTRIBUTE *query_attribute_calloc(
			void );

LIST *query_attribute_list(
			char *application_name,
			LIST *folder_attribute_append_isa_list,
			DICTIONARY *dictionary,
			int relation_mto1_isa_list_length );

QUERY_ATTRIBUTE *query_attribute_new(
			char *application_name,
			char *folder_name,
			char *attribute_name,
			char *datatype_name,
			DICTIONARY *dictionary,
			int relation_mto1_isa_list_length );

/* Returns heap memory */
/* ------------------- */
char *query_attribute_list_where(
			LIST *query_attribute_list );

/* Returns static memory, structure memory, or null */
/* ------------------------------------------------ */
char *query_attribute_where(
			char *folder_table_name,
			int relation_mto1_isa_list_length,
			QUERY_RELATION *query_relation,
			QUERY_BETWEEN_DATA *query_between_data,
			QUERY_DATA *query_data );

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
			LIST *no_display_name_list,
			LIST *exclude_lookup_attribute_name_list,
			int relation_mto1_isa_length,
			QUERY_DATE_CONVERT *query_date_convert,
			char *row_security_role_attribute_not_null );

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

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *query_select_column_string(
			char *folder_name,
			char *attribute_name,
			boolean attribute_is_date,
			boolean attribute_is_date_time,
			enum date_convert_format date_convert_format );

/* Public */
/* ------ */

LIST *query_select_name_list(
			LIST *select_list );

/* Returns heap memory or null */
/* --------------------------- */
char *query_select_list_string(
			LIST *select_list );

typedef struct
{
	/* Process */
	/* ------- */
	LIST *query_drop_down_list;
	char *query_drop_down_list_where;
	LIST *query_attribute_list;
	char *query_attribute_list_where;
	char *query_widget_where;
	char *string;
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

/* Returns heap memory */
/* ------------------- */
char *query_widget_where_string(
			char *query_drop_down_where,
			char *query_attribute_list_where,
			char *security_entity_where );

typedef struct
{
	/* Process */
	/* ------- */
	LIST *query_widget_select_list;
	char *query_select_list_string;
	QUERY_WIDGET_WHERE *where;
	char *query_order_string;
	LIST *delimited_list;
} QUERY_ISA_WIDGET;

/* QUERY_ISA_WIDGET operations */
/* --------------------------- */
QUERY_ISA_WIDGET *query_isa_widget_calloc(
			void );

/* Always succeeds */
/* --------------- */
QUERY_ISA_WIDGET *query_isa_widget_new(
			char *one_isa_folder_name,
			LIST *primary_key_list,
			LIST *folder_attribute_primary_list,
			char *login_name,
			char *security_entity_where );

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

char *query_or_sequence_where(
			LIST *attribute_name_list,
			LIST *data_list_list );

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

/* QUERY_DROP_DOWN_ROW operations */
/* ------------------------------ */
LIST *query_drop_down_row_list(
			LIST *foreign_key_list,
			LIST *folder_attribute_list,
			DICTIONARY *dictionary,
			int highest_index );

QUERY_DROP_DOWN_ROW *query_drop_down_row_new(
			LIST *foreign_key_list,
			LIST *folder_attribute_list,
			DICTIONARY *dictionary,
			int index );

QUERY_DROP_DOWN_ROW *query_drop_down_row_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *query_drop_down_row_key(
			LIST *foreign_key_list,
			int index );

/* Returns dictionary->hash_table->other_data */
/* ------------------------------------------ */
char *query_drop_down_row_data_list_string(
			char *key,
			DICTIONARY *dictionary );

LIST *query_drop_down_row_data_string_list(
			char sql_delimiter,
			char *data_list_string );

/* Public */
/* ------ */
char *query_drop_down_row_list_where(
			char *folder_table_name,
			LIST *query_drop_down_row_list,
			int relation_mto1_isa_list_length );

/* Private */
/* ------- */

/* Returns heap memory or null */
/* --------------------------- */
char *query_drop_down_row_where(
			char *folder_table_name,
			LIST *query_data_list,
			int relation_mto1_isa_list_length );

/* Returns static memory */
/* --------------------- */
char *query_drop_down_row_data_where(
			char *folder_name,
			char *attribute_name,
			char *data );

/* Returns program memory */
/* ---------------------- */
char *query_operator_character_string(
			char *operator_string );


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

typedef struct
{
	/* Process */
	/* ------- */
	LIST *select_list;
	char *query_select_list_string;
	LIST *query_select_name_list;
	char *from_string;
	QUERY_EDIT_TABLE_WHERE *where;
	char *query_order_string;
	char *query_system_string;
	LIST *row_dictionary_list;
} QUERY_EDIT_TABLE;

/* QUERY_EDIT_TABLE operations */
/* --------------------------- */
QUERY_EDIT_TABLE *query_edit_table_calloc(
			void );

QUERY_EDIT_TABLE *query_edit_table_new(
			char *application_name,
			char *login_name,
			char *folder_name,
			char *security_entity_where,
			LIST *relation_join_one2m_list,
			LIST *no_display_name_list,
			LIST *exclude_lookup_attribute_name_list,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_mto1_isa_list,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			char *row_security_role_folder_name,
			RELATION *row_security_role_relation,
			char *row_security_role_attribute_not_null );

char *query_edit_table_from_string(
			char *folder_name,
			LIST *relation_mto1_isa_list,
			char *row_security_role_folder_name );

LIST *query_edit_table_row_dictionary_list(
			char *query_system_string,
			LIST *query_select_name_list,
			LIST *one_folder_primary_key_list,
			char *application_name,
			LIST *relation_join_one2m_list,
			char *one_folder_name );

typedef struct
{
	/* Process */
	/* ------- */
	LIST *select_list;
	char *query_select_list_string;
	QUERY_WIDGET_WHERE *where;
	char *query_order_string;
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
} QUERY;

/* QUERY operations */
/* ---------------- */
char *query_order_string(
			char *query_select_list_string,
			DICTIONARY *sort_dictionary );

LIST *query_delimited_list(
			char *select_string,
			char *from_string,
			char *where_string,
			char *order_string );

/* Returns heap memory */
/* ------------------- */
char *query_order_key_list_string(
			LIST *key_list );

/* Returns heap memory */
/* ------------------- */
char *query_system_string(
			char *select_string,
			char *from_string,
			char *where_string,
			char *order_string,
			int max_rows );

/* Returns heap memory or null */
/* --------------------------- */
char *query_join_where(
			char *application_name,
			char *folder_name,
			LIST *primary_key_list,
			LIST *relation_mto1_isa_list,
			RELATION *row_security_role_relation );

LIST *query_primary_delimited_list(
			char *folder_table_name,
			LIST *primary_key_list,
			LIST *foreign_key_list,
			LIST *foreign_data_list );

/* Private */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *query_join_relation_where(
			char *application_name,
			char *primary_folder_table_name,
			LIST *primary_key_list,
			LIST *relation_mto1_isa_list );

/* Returns static memory */
/* --------------------- */
char *query_string_join_where(
			char *primary_folder_table_name,
			char *relation_folder_table_name,
			LIST *primary_key_list,
			LIST *foreign_key_list );

/* Returns heap memory */
/* ------------------- */
char *query_data_where(
			char *folder_name,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			LIST *folder_attribute_list );

#endif
