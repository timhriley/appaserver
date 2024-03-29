/* $APPASERVER_HOME/library/query.h					*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */


#ifndef QUERY_H
#define QUERY_H

#include "folder.h"
#include "related_folder.h"
#include "list.h"
#include "dictionary.h"
#include "boolean.h"
#include "piece.h"
#include "prompt_recursive.h"
#include "folder.h"

/* Constants */
/* --------- */
#define QUERY_WHERE_BUFFER			131072
#define QUERY_MAX_ROWS				500
#define QUERY_STARTING_LABEL			"qquery_"
#define QUERY_FROM_STARTING_LABEL		"from_"
#define QUERY_TO_STARTING_LABEL			"to_"
#define QUERY_DROP_DOWN_ORIGINAL_STARTING_LABEL	"original_"
#define QUERY_RELATION_OPERATOR_STARTING_LABEL	"relation_operator_"
#define QUERY_NULL_EXPRESSION			\
			 " (%s = 'null' or %s = '' or %s is null)"
#define QUERY_NOT_NULL_EXPRESSION		\
			 " (%s <> 'null' or %s <> '' or %s is not null)"

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
	LIST *attribute_name_list;
	LIST *data_list_list;
} QUERY_OR_SEQUENCE;

typedef struct
{
	LIST *attribute_name_list;
	LIST *data_list;
} QUERY_DROP_DOWN_ROW;

typedef struct
{
	char *folder_name;
	LIST *query_drop_down_row_list;
} QUERY_DROP_DOWN;

typedef struct
{
	char *attribute_name;
	char *folder_name;
	char *datatype;
	enum relational_operator relational_operator;
	char *from_data;
	char *to_data;
	int primary_key_index;
} QUERY_ATTRIBUTE;

typedef struct
{
	char *folder_name;
	LIST *foreign_attribute_name_list;
	LIST *query_drop_down_list;
	LIST *query_attribute_list;
	char *where_clause;
} QUERY_SUBQUERY;

typedef struct
{
	char *select_clause;
	char *from_clause;
	char *drop_down_where_clause;
	char *attribute_where_clause;
	char *non_joined_where_clause;
	char *subquery_where_clause;
	char *where_clause;
	char *order_clause;
	LIST *query_attribute_list;
	LIST *query_drop_down_list;
	LIST *row_dictionary_list;
	LIST *query_subquery_list;
} QUERY_OUTPUT;

typedef struct
{
	FOLDER *folder;
	LIST *one2m_subquery_related_folder_list;
	char *login_name;
	DICTIONARY *dictionary;
	DICTIONARY *sort_dictionary;
	LIST *mto1_join_folder_list;
	LIST *where_attribute_name_list;
	LIST *where_attribute_data_list;
	int max_rows /* use zero for unlimited */;
	PROMPT_RECURSIVE *prompt_recursive;
	QUERY_OUTPUT *query_output;
} QUERY;

/* Operations */
/* ---------- */
QUERY *query_calloc(		void );

QUERY_OUTPUT *query_output_calloc(
				void );

QUERY *query_insert_new(	char *application_name,
				char *login_name,
				char *folder_name,
				DICTIONARY *query_dictionary );

QUERY_DROP_DOWN *query_get_insert_drop_down(
				char *folder_name,
				LIST *foreign_attribute_name_list,
				LIST *attribute_list,
				DICTIONARY *query_dictionary );

QUERY *query_new(		char *application_name,
				char *login_name,
				char *folder_name,
				LIST *append_isa_attribute_list,
				DICTIONARY *dictionary,
				DICTIONARY *sort_dictionary,
				ROLE *role,
				LIST *where_attribute_name_list,
				LIST *where_attribute_data_list,
				int max_rows /* zero for unlimited */,
				boolean include_root_folder,
				LIST *one2m_subquery_folder_name_list,
				RELATED_FOLDER *root_related_folder );

QUERY *query_primary_data_new(	char *application_name,
				char *login_name,
				char *folder_name,
				DICTIONARY *dictionary,
				ROLE *role,
				int max_rows /* zero for unlimited */,
				boolean include_root_folder );

QUERY_OUTPUT *query_insert_output_new(
				FOLDER *folder,
				DICTIONARY *query_dictionary );

LIST *query_get_insert_drop_down_list(
				char *folder_name,
				LIST *mto1_related_folder_list,
				DICTIONARY *query_dictionary );

QUERY_OUTPUT *query_primary_data_output_new(
				QUERY *query,
				FOLDER *folder,
				boolean include_root_folder );

QUERY_OUTPUT *query_output_new(	QUERY *query,
				boolean include_root_folder,
				LIST *where_attribute_name_list,
				LIST *where_attribute_data_list,
				FOLDER *folder,
				LIST *subquery_one2m_related_folder_list,
				PROMPT_RECURSIVE *prompt_recursive,
				LIST *append_isa_attribute_list );

LIST *query_row_dictionary_list(
				char *application_name,
				char *select_clause,
				char *from_clause,
				char *where_clause,
				char *order_clause,
				int max_rows,
				LIST *append_isa_attribute_list,
				char *login_name );

LIST *query_get_record_list(	char *application_name,
				QUERY_OUTPUT *query_output,
				char *select_clause,
				char *order_clause );

char *query_get_select_clause(
			char *application_name,
			LIST *append_isa_attribute_list );

LIST *query_get_subquery_drop_down_list(
				LIST *exclude_attribute_name_list,
				FOLDER *root_folder,
				LIST *mto1_related_folder_list,
				LIST *mto1_append_isa_related_folder_list,
				DICTIONARY *dictionary );

LIST *query_primary_data_drop_down_list(
				LIST *exclude_attribute_name_list,
				FOLDER *root_folder,
				LIST *mto1_related_folder_list,
				LIST *mto1_append_isa_related_folder_list,
				DICTIONARY *dictionary,
				boolean include_root_folder );

LIST *query_drop_down_list(	LIST *exclude_attribute_name_list,
				FOLDER *root_folder,
				LIST *mto1_related_folder_list,
				LIST *mto1_append_isa_related_folder_list,
				DICTIONARY *dictionary,
				boolean include_root_folder );

QUERY_DROP_DOWN *query_get_subquery_drop_down(
				LIST *exclude_attribute_name_list,
				char *folder_name,
				LIST *foreign_attribute_name_list,
				LIST *attribute_list,
				DICTIONARY *dictionary,
				char *dictionary_prepend_folder_name );

QUERY_DROP_DOWN *query_primary_data_drop_down(
				LIST *exclude_attribute_name_list,
				char *root_folder_name,
				char *dictionary_prepend_folder_name,
				LIST *foreign_attribute_name_list,
				LIST *attribute_list,
				DICTIONARY *dictionary );

QUERY_DROP_DOWN *query_drop_down(
				LIST *exclude_attribute_name_list,
				char *root_folder_name,
				char *dictionary_prepend_folder_name,
				LIST *foreign_attribute_name_list,
				LIST *attribute_list,
				DICTIONARY *dictionary );

QUERY_DROP_DOWN *query_row_drop_down(
				LIST *exclude_attribute_name_list,
				QUERY_DROP_DOWN *query_drop_down,
				char *folder_name,
				LIST *foreign_attribute_name_list,
				LIST *attribute_list,
				DICTIONARY *dictionary,
				int index,
				char *dictionary_prepend_folder_name );

QUERY_DROP_DOWN_ROW *query_drop_down_row_new(
				LIST *attribute_name_list,
				LIST *attribute_list,
				LIST *data_list );

QUERY_DROP_DOWN *query_drop_down_new(
				char *folder_name );

boolean query_get_drop_down_dictionary_data(
				char **data,
				DICTIONARY *dictionary, 
				char *primary_attribute_name,
				int dictionary_offset );

char *query_get_operator_name(	char *attribute_name,
				DICTIONARY *dictionary,
				int dictionary_offset,
				char *starting_label,
				char *dictionary_prepend_folder_name );

void query_convert_date_international(
				char **data );

boolean query_get_dictionary_data(
				char **data,
				DICTIONARY *dictionary, 
				char *attribute_name,
				int dictionary_offset,
				char *starting_label,
				char *dictionary_prepend_folder_name );

char *query_get_data_escaped(
			DICTIONARY *dictionary,
			char *attribute_name,
			int dictionary_offset,
			char *starting_label,
			char *dictionary_prepend_folder_name );

LIST *query_get_attribute_list(
				LIST *append_isa_attribute_list,
				DICTIONARY *dictionary,
				LIST *exclude_attribute_name_list,
				char *dictionary_prepend_folder_name );

enum relational_operator query_get_relational_operator(
				char *operator_name );

QUERY_SUBQUERY *query_subquery_new(
				char *folder_name,
				LIST *foreign_attribute_name_list );

QUERY_ATTRIBUTE *query_attribute_new(
				char *attribute_name,
				char *folder_name,
				char *datatype,
				enum relational_operator,
				char *from_data,
				char *to_data,
				int primary_key_index );

boolean query_attribute_list_exists(
				LIST *query_attribute_list,
				char *attribute_name );

char *query_get_process_where_clause(
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

char *query_get_display_where_clause(
			char *where_clause,
			char *application_name,
			char *folder_name,
			boolean is_primary_application );

char *query_append_where_clause_related_join(
				char *application_name,
				char *source_where_clause,
				LIST *primary_attribute_name_list,
				LIST *related_attribute_name_list,
				char *folder_name,
				char *related_folder_name );

char *query_append_folder_name(
				char *folder_name,
				char *append_folder_name );

char *query_get_between_date_time_where(
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

char *query_get_sys_string( 	char *application_name,
				char *select_clause,
				char *from_clause,
				char *where_clause,
				char *subquery_where_clause,
				char *order_by_clause,
				int max_rows );

char *query_drop_down_list_display(
				char *folder_name,
				LIST *query_drop_down_list );

char *query_attribute_list_display(
				char *folder_name,
				LIST *query_attribute_list );

char *query_relational_operator_display(
				enum relational_operator );

boolean query_get_date_time_between_attributes(
				QUERY_ATTRIBUTE **date_between_attribute,
				QUERY_ATTRIBUTE **time_between_attribute,
				LIST *query_attribute_list,
				boolean combine_date_time );

boolean query_data_list_accounted_for(
				LIST *query_drop_down_row_list,
				LIST *data_list );

char *query_get_order_clause(	DICTIONARY *sort_dictionary,
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

char *query_get_drop_down_data_where(
				char *application_name,
				char *folder_name,
				char *attribute_name,
				char *data );

LIST *query_get_one2m_subquery_related_folder_list(
				char *application_name,
				LIST *one2m_subquery_folder_name_list,
				LIST *one2m_recursive_related_folder_list,
				RELATED_FOLDER *root_related_folder );

LIST *query_get_subquery_list(	DICTIONARY *dictionary,
				LIST *one2m_subquery_related_folder_list,
				int length_primary_attribute_name_list );

char *query_get_subquery_where_clause(
				char *application_name,
				char *folder_name,
				LIST *query_subquery_list,
				LIST *primary_attribute_name_list );

char *query_append_where_clause(char *source_where_clause,
				char *append_where_clause );

char *query_get_display(	QUERY_OUTPUT *query_output );

char *query_subquery_list_display(
				LIST *subquery_list );

char *query_get_drop_down_list_display(
				LIST *query_drop_down_list );

char *query_get_query_attribute_list_display(
				LIST *query_attribute_list );

LIST *query_append_prompt_recursive_folder_list(
				LIST *query_drop_down_list,
				LIST *exclude_attribute_name_list,
				char *root_folder_name,
				LIST *prompt_recursive_folder_list,
				DICTIONARY *query_dictionary );

QUERY *query_process_new(	char *application_name,
				char *login_name,
				char *folder_name,
				DICTIONARY *preprompt_dictionary );

QUERY_OUTPUT *query_process_output_new(
				FOLDER *folder,
				DICTIONARY *preprompt_dictionary );

QUERY_OUTPUT *query_process_output_new(
				FOLDER *folder,
				DICTIONARY *preprompt_dictionary );

LIST *query_get_process_drop_down_list(
				LIST *exclude_attribute_name_list,
				char *folder_name,
				LIST *mto1_recursive_related_folder_list,
				DICTIONARY *preprompt_dictionary );

QUERY_DROP_DOWN *query_get_process_drop_down(
				LIST *exclude_attribute_name_list,
				char *folder_name,
				LIST *foreign_attribute_name_list,
				LIST *attribute_list,
				DICTIONARY *preprompt_dictionary );

QUERY_DROP_DOWN_ROW *query_process_drop_down_row_new(
				char *attribute_name,
				LIST *attribute_list,
				LIST *data_list );

QUERY_DROP_DOWN_ROW *query_drop_down_row_calloc(
				void );

char *query_drop_down_list_in_clause_display(
				char *attribute_name,
				LIST *data_list );

char *query_get_process_drop_down_where_clause(
				LIST *query_drop_down_list,
				char *folder_name );

char *query_get_dictionary_where_clause(
				DICTIONARY *dictionary,
				LIST *primary_attribute_name_list,
				char *dictionary_indexed_prefix );

QUERY_OR_SEQUENCE *query_or_sequence_new(
				LIST *attribute_name_list );

int query_or_sequence_set_data_list_string(
				LIST *data_list_list,
				char *data_list_string );

int query_or_sequence_set_data_list(
				LIST *data_list_list,
				LIST *data_list );

char *query_or_sequence_where_clause(
				LIST *attribute_name_list,
				LIST *data_list_list,
				boolean with_and_prefix );

char *query_or_sequence_get_where_clause(
				LIST *attribute_name_list,
				LIST *data_list_list,
				boolean with_and_prefix );

QUERY *query_process_parameter_new(
				char *application_name,
				LIST *attribute_list,
				DICTIONARY *parameter_dictionary );

QUERY_OUTPUT *query_process_parameter_output_new(
				DICTIONARY *dictionary,
				char *application_name,
				LIST *attribute_list );

QUERY *query_folder_new(	char *application_name,
				char *login_name,
				char *folder_name,
				DICTIONARY *dictionary,
				ROLE *role,
				LIST *where_attribute_name_list,
				LIST *where_attribute_data_list );

QUERY *query_process_drop_down_new(
				char *application_name,
				char *folder_name,
				DICTIONARY *dictionary );

QUERY_OUTPUT *query_process_drop_down_output_new(
				FOLDER *folder,
				char *folder_name,
				DICTIONARY *query_dictionary );

LIST *query_process_drop_down_get_drop_down_list(
				LIST *mto1_related_folder_list,
				DICTIONARY *dictionary );

QUERY_DROP_DOWN *query_process_drop_down(
				LIST *foreign_attribute_name_list,
				LIST *attribute_list,
				DICTIONARY *dictionary );

char *query_primary_where_clause(
			LIST *primary_key_list,
			char *input_buffer,
			char delimiter );

QUERY_OUTPUT *query_folder_output_new(
				QUERY *query,
				FOLDER *folder,
				PROMPT_RECURSIVE *prompt_recursive,
				LIST *where_attribute_name_list,
				LIST *where_attribute_data_list,
				LIST *mto1_join_folder_list );

void query_output_set_row_level_non_owner_forbid_join(
				QUERY_OUTPUT *query_output,
				FOLDER *folder );

void query_set_row_level_non_owner_forbid_dictionary(
				QUERY *query,
				FOLDER *folder );

char *query_folder_drop_down_where(
				LIST *query_drop_down_list,
				char *application_name,
				char *folder_name );

LIST *query_process_generic_drop_down_list(
				char *value_folder_name,
				LIST *mto1_related_folder_list,
				DICTIONARY *dictionary );

QUERY_DROP_DOWN *query_process_multi_drop_down(
				char *value_folder_name,
				LIST *foreign_attribute_name_list,
				LIST *attribute_list,
				DICTIONARY *dictionary );

QUERY *query_folder_data_new(
			char *application_name,
			char *login_name,
			char *folder_name,
			DICTIONARY *dictionary,
			ROLE *role,
			int max_rows,
			boolean include_root_folder );

QUERY_DROP_DOWN *query_related_drop_down(
			LIST *exclude_attribute_name_list,
			char *root_folder_name,
			char *dictionary_prepend_folder_name,
			LIST *foreign_attribute_name_list,
			LIST *attribute_list,
			DICTIONARY *dictionary );

QUERY *query_related_folder_data_new(
			char *application_name,
			char *login_name,
			char *folder_name,
			DICTIONARY *dictionary,
			ROLE *role,
			int max_rows );

QUERY_OUTPUT *query_related_data_output_new(
			QUERY *query,
			FOLDER *folder );

LIST *query_related_data_drop_down_list(
			LIST *exclude_attribute_name_list,
			FOLDER *root_folder,
			LIST *mto1_related_folder_list,
			LIST *mto1_append_isa_related_folder_list,
			DICTIONARY *dictionary );

QUERY_DROP_DOWN *query_related_data_drop_down(
			LIST *exclude_attribute_name_list,
			char *root_folder_name,
			char *dictionary_prepend_folder_name,
			LIST *foreign_attribute_name_list,
			LIST *attribute_list,
			DICTIONARY *dictionary );

LIST *query_edit_table_dictionary_list(
			char *application_name,
			char *select_clause,
			char *from_clause,
			char *where_clause,
			char *order_clause,
			int max_rows,
			LIST *append_isa_attribute_list,
			char *login_name );

QUERY_OUTPUT *query_edit_table_output_new(
			QUERY *query,
			char *first_folder_name,
			FOLDER *folder,
			LIST *append_isa_attribute_list,
			PROMPT_RECURSIVE *prompt_recursive,
			char *query_select_folder_name,
			char *attribute_not_null_join,
			char *attribute_not_null_folder_name );

LIST *query_edit_table_drop_down_list(
			LIST *exclude_attribute_name_list,
			char *first_folder_name,
			LIST *mto1_related_folder_list,
			LIST *mto1_append_isa_related_folder_list,
			DICTIONARY *dictionary );

QUERY *query_edit_table_new(
			DICTIONARY *dictionary,
			char *application_name,
			char *login_name,
			char *query_select_folder_name,
			ROLE *role,
			LIST *append_isa_attribute_list,
			char *attribute_not_null_join,
			char *attribute_not_null_folder_name );

LIST *query_edit_table_drop_down_list_evaluate(
			LIST *exclude_attribute_name_list,
			char *root_folder_name,
			char *dictionary_prepend_folder_name,
			LIST *foreign_attribute_name_list,
			LIST *attribute_list,
			DICTIONARY *dictionary );

QUERY_DROP_DOWN *query_edit_table_row_drop_down(
			LIST *exclude_attribute_name_list,
			QUERY_DROP_DOWN *query_drop_down,
			char *root_folder_name,
			LIST *foreign_attribute_name_list,
			LIST *attribute_list,
			DICTIONARY *dictionary,
			int index,
			char *dictionary_prepend_folder_name );

QUERY_DROP_DOWN_ROW *query_drop_down_edit_table_new(
			LIST *attribute_name_list,
			LIST *attribute_list,
			LIST *data_list );

char *query_edit_table_drop_down_where(
			LIST *query_drop_down_list,
			char *application_name );

char *query_edit_table_where(
			char **drop_down_where_clause,
			char **attribute_where_clause,
			LIST *query_drop_down_list,
			LIST *query_attribute_list,
			char *application_name,
			boolean combine_date_time );

char *query_drop_down_where(
			LIST *query_drop_down_list,
			char *application_name,
			char *folder_name );

char *query_simple_where(
			char *folder_name,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			LIST *append_isa_attribute_list );

QUERY *query_detail_new(
			char *application_name,
			char *folder_name,
			ROLE *role,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			LIST *append_isa_attribute_list,
			char *attribute_not_null_join,
			char *attribute_not_null_folder_name );

QUERY_OUTPUT *query_detail_output_new(
			FOLDER *folder,
			LIST *mto1_isa_related_folder_list,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			LIST *append_isa_attribute_list,
			char *attribute_not_null_join,
			char *attribute_not_null_folder_name );

char *query_output_one2m_isa_where(
			char **from_clause,
			char *application_name,
			char *folder_name,
			LIST *one2m_isa_related_folder_list,
			LIST *primary_attribute_name_list,
			char *where_clause,
			boolean one_only );

char *query_output_mto1_isa_where(
			char *application_name,
			char *folder_name,
			LIST *mto1_isa_related_folder_list,
			LIST *primary_attribute_name_list,
			char *where_clause,
			boolean one_only );

char *query_folder_where_clause(
			FOLDER *folder,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			LIST *append_isa_attribute_list );

char *query_related_join_where(
			char *application_name,
			char *source_where_clause,
			LIST *primary_attribute_name_list,
			LIST *foreign_attribute_name_list,
			char *folder_name,
			char *related_folder_name );

LIST *query_get_drop_down_list(
			LIST *exclude_attribute_name_list,
			FOLDER *root_folder,
			LIST *mto1_related_folder_list,
			LIST *mto1_append_isa_related_folder_list,
			DICTIONARY *dictionary,
			boolean include_root_folder );

QUERY_DROP_DOWN *query_get_drop_down(
			LIST *exclude_attribute_name_list,
			char *root_folder_name,
			char *dictionary_prepend_folder_name,
			LIST *foreign_attribute_name_list,
			LIST *attribute_list,
			DICTIONARY *dictionary );

LIST *query_with_folder_name_get_mto1_join_folder_list(
			char *application_name,
			char *folder_name,
			ROLE *role );

char *query_login_name_where_clause(
			FOLDER *folder,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			char *login_name );

QUERY_DROP_DOWN *query_get_row_drop_down(
			LIST *exclude_attribute_name_list,
			QUERY_DROP_DOWN *query_drop_down,
			char *root_folder_name,
			LIST *foreign_attribute_name_list,
			LIST *attribute_list,
			DICTIONARY *dictionary,
			int index,
			char *dictionary_prepend_folder_name );

char *query_folder_get_where_clause(
			char **drop_down_where_clause,
			char **attribute_where_clause,
			LIST *query_drop_down_list,
			LIST *query_attribute_list,
			char *application_name,
			char *folder_name,
			boolean combine_date_time );

char *query_get_folder_drop_down_where_clause(
			LIST *query_drop_down_list,
			char *application_name,
			char *folder_name );

QUERY *query_prompt_folder_data_new(
			char *application_name,
			char *login_name,
			char *folder_name,
			DICTIONARY *dictionary,
			ROLE *role,
			int max_rows );

QUERY_OUTPUT *query_prompt_data_output_new(
			QUERY *query,
			FOLDER *folder );

/* Returns heap memory */
/* ------------------- */
char *query_join_where_clause(
			LIST *primary_attribute_name_list,
			LIST *related_attribute_name_list,
			char *folder_name,
			char *related_folder_name );

char *query_from_clause(
			char *folder_name,
			LIST *isa_attribute_list,
			char *attribute_not_null_folder_name );

char *query_combined_where_clause(
			char **drop_down_where_clause,
			char **attribute_where_clause,
			LIST *query_drop_down_list,
			LIST *query_attribute_list,
			char *application_name,
			char *folder_name,
			boolean combine_date_time );

char *query_key_list_where_clause(
			LIST *primary_key_list,
			char *input_buffer,
			char delimiter );

LIST *query_detail_dictionary_list(
			char *application_name,
			char *select_clause,
			char *from_clause,
			char *where_clause,
			char *order_clause,
			int max_rows,
			LIST *append_isa_attribute_list,
			char *login_name );

QUERY *query_simple_new(
			DICTIONARY *dictionary,
			char *application_name,
			char *login_name,
			char *folder_name );

QUERY *query_sort_order_new(
			DICTIONARY *dictionary,
			FOLDER *folder );

QUERY_OUTPUT *query_sort_order_output_new(
			QUERY *query );

#endif
