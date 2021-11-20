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
#include "date_convert.h"
#include "folder.h"

/* Constants */
/* --------- */
#define QUERY_MAX_ROWS				500
#define QUERY_STARTING_LABEL			"qquery_"
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
	char *attribute_name;
	char *datatype_name;
	char *escaped_replaced_data;
} QUERY_DATA;

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
	char *widget_folder_name;
	LIST *exclude_attribute_name_list;
	LIST *query_drop_down_list;
	LIST *query_attribute_list;
	char *query_drop_down_list_where;
	char *query_attribute_list_where;
	char *where_clause;
} QUERY_WIDGET_WHERE;

typedef struct
{
	char *folder_name;
	char *attribute_name;
	boolean attribute_is_date;
	boolean attribute_is_date_time;
	QUERY_DATE_CONVERT *query_date_convert;
	char *query_select_string;
} QUERY_SELECT;

typedef struct
{
	/* Input */
	/* ----- */
	DICTIONARY *drillthru_dictionary;
	DICTIONARY *query_dictionary;
	LIST *where_attribute_data_list;
	char *security_entity_where;
	LIST *ignore_select_attribute_name_list;
	DICTIONARY *sort_dictionary;

	/* Process */
	/* ------- */
	QUERY_WIDGET_WHERE *query_widget_where;
	PROMPT_RECURSIVE *prompt_recursive;
	ROLE *role;
	FOLDER *folder;
	SECURITY_ENTITY *security_entity;
	ROLE_FOLDER *role_folder;
	int max_rows /* use zero for unlimited */;
	LIST *query_drop_down_list;
	LIST *query_prompt_recursive_drop_down_list;
	LIST *query_attribute_list;
	char *query_drop_down_where;
	char *query_drop_down_data_where;
	char *query_attribute_where;
	char *query_join_where;
	char *query_related_join;
	LIST *select_list;
	char *select_clause;
	char *where_clause;
	char *from_clause;
	char *order_clause;
	LIST *delimited_list;
	LIST *dictionary_list;
} QUERY;

/* QUERY_ISA_WIDGET operations */
/* --------------------------- */

/* Always succeeds */
/* --------------- */
QUERY *query_isa_widget_new(
			char *one2m_isa_folder_name,
			LIST *primary_key_list,
			LIST *folder_attribute_primary_list,
			char *login_name,
			char *security_entity_where );

/* QUERY_WIDGET operations */
/* ----------------------- */

/* Always succeeds */
/* --------------- */
QUERY *query_widget_new(
			char *widget_folder_name,
			char *login_name,
			LIST *primary_key_list,
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *security_entity_where,
			DICTIONARY *drillthru_dictionary );

/* QUERY_WIDGET_WHERE operations */
/* ----------------------------- */
QUERY_WIDGET_WHERE *query_widget_where_calloc(
			void );

/* Always succeeds */
/* --------------- */
QUERY_WIDGET_WHERE *query_widget_where_new(
			char *widget_folder_name,
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *security_entity_where,
			DICTIONARY *drillthru_dictionary );

/* Returns heap memory or null */
/* --------------------------- */
char *query_widget_where_clause(
			char *drop_down_where,
			char *attribute_list_where,
			char *security_entity_where );

/* QUERY_TABLE operations */
/* ---------------------- */
QUERY *query_table_new(
			char *security_sql_injection_escape_folder_name,
			char *security_sql_injection_escape_login_name,
			LIST *ignore_select_attribute_name_list,
			ROLE *role,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary );

/* QUERY where */
/* ----------- */

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

/* QUERY_SELECT operations */
/* ----------------------- */

/* Always succeeds */
/* --------------- */
QUERY_SELECT *query_select_new(
			char *folder_name,
			char *attribute_name,
			boolean attribute_is_date,
			boolean attribute_is_date_time,
			QUERY_DATE_CONVERT *query_date_convert );

LIST *query_select_list(
			LIST *folder_attribute_append_isa_list,
			LIST *ignore_select_attribute_name_list,
			LIST *exclude_lookup_attribute_name_list,
			int relation_mto1_isa_length,
			QUERY_DATE_CONVERT *query_date_convert );

LIST *query_primary_select_list(
			LIST *primary_key_list,
			LIST *folder_attribute_list,
			QUERY_DATE_CONVERT *query_date_convert );

/* Returns heap memory or null */
/* --------------------------- */
char *query_select_clause(
			LIST *select_list );

/* Safely returns heap memory */
/* -------------------------- */
char *query_select_string(
			char *folder_name,
			char *attribute_name,
			boolean attribute_is_date,
			boolean attribute_is_date_time,
			enum date_convert_format );

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

QUERY_DATA *query_data_escaped(
			DICTIONARY *dictionary,
			char *attribute_name,
			char *datatype_name,
			int dictionary_offset,
			char *starting_label,
			char *dictionary_prepend_folder_name );

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

boolean query_data_list_accounted_for(
			LIST *query_drop_down_row_list,
			LIST *data_list );

char *query_get_order_clause(
			DICTIONARY *sort_dictionary,
			char *folder_name,
			LIST *attribute_list );

char *query_get_non_multi_key_sort_attribute_with_prefix_and_index(
			DICTIONARY *sort_dictionary );

char *query_get_multi_key_sort_attribute_with_prefix_and_index(
			DICTIONARY *sort_dictionary );

char *query_with_sort_multi_attribute_get_order_clause(
			char *sort_multi_attribute_with_prefix_and_index,
			char *folder_name,
			LIST *attribute_list );

char *query_parse_multi_attribute_get_order_clause(
			char *sort_multi_attribute,
			char *folder_name,
			boolean descending,
			LIST *attribute_list );

LIST *query_prompt_recursive_drop_down_list(
			LIST *exclude_attribute_name_list,
			char *mto1_folder_name,
			LIST *prompt_recursive_folder_list,
			DICTIONARY *query_dictionary );

QUERY_DROP_DOWN *query_process_drop_down(
			LIST *exclude_attribute_name_list,
			char *folder_name,
			LIST *foreign_attribute_name_list,
			LIST *attribute_list,
			DICTIONARY *drilldown_dictionary );

char *query_drop_down_list_in_clause_display(
			char *attribute_name,
			LIST *data_list );

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
QUERY *query_detail_new(
			LIST *where_attribute_data_list,
			char *folder_name,
			char *role_name,
			char *login_name,
			LIST *ignore_select_attribute_name_list,
			char *attribute_not_null_join,
			char *attribute_not_null_folder_name );

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

/* QUERY_DATA operations */
/* --------------------- */
LIST *query_data_list(
			LIST *foreign_key_list,
			LIST *many_folder_attribute_list,
			LIST *data_string_list );

QUERY_DATA *query_data_new(
			char *attribute_name,
			char *datatype_name,
			char *escaped_replaced_data );

char *query_data_convert_date_international(
			char *datatype_name,
			char *escaped_replaced_data );

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

/* QUERY generic operations */
/* ------------------------ */
QUERY *query_calloc(	void );

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

/* Sample: ssort_button_descend_full_name^street_address_0 */
/* ------------------------------------------------------- */
/* Returns heap memory					   */
/* ------------------------------------------------------- */
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

#endif
