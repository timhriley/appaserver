/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/query.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef QUERY_H
#define QUERY_H

#include "list.h"
#include "dictionary.h"
#include "boolean.h"
#include "piece.h"
#include "security_entity.h"
#include "date_convert.h"
#include "query_isa.h"
#include "process.h"
#include "relation_mto1.h"
#include "dictionary.h"
#include "row_security_role_update.h"
#include "folder_attribute.h"

#define QUERY_EQUAL			"equals"
#define QUERY_NOT_EQUAL			"not_equal"
#define QUERY_LESS_THAN			"less_than"
#define QUERY_LESS_THAN_EQUAL_TO	"less_than_equal_to"
#define QUERY_GREATER_THAN		"greater_than"
#define QUERY_GREATER_THAN_EQUAL_TO	"greater_than_equal_to"
#define QUERY_BETWEEN			"between"
#define QUERY_BEGINS			"begins"
#define QUERY_CONTAINS			"contains"
#define QUERY_NOT_CONTAINS		"not_contains"
#define QUERY_OR			"or_,"
#define QUERY_IS_NULL			"is_empty"
#define QUERY_NOT_NULL			"not_empty"

#define QUERY_QUICK_YES_NO		"yes"
#define QUERY_RELATION_OPERATOR_PREFIX	"relation_operator_"
#define QUERY_DROP_DOWN_FETCH_MAX_ROWS	10000

typedef struct
{
	char *attribute_name;
	char *select_datum;
	int primary_key_index;
	boolean attribute_is_date;
	boolean attribute_is_date_time;
	boolean attribute_is_current_date_time;
	boolean attribute_is_number;
	char *display_datum;
} QUERY_CELL;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_CELL *query_cell_parse(
		LIST *folder_attribute_list,
		enum date_convert_format_enum destination_enum,
		char *attribute_name,
		char *select_datum );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_CELL *query_cell_new(
		char *attribute_name,
		char *select_datum,
		int primary_key_index,
		boolean attribute_is_date,
		boolean attribute_is_date_time,
		boolean attribute_is_current_date_time,
		boolean attribute_is_number,
		char *display_datum );

/* Process */
/* ------- */
QUERY_CELL *query_cell_calloc(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_CELL *query_cell_simple_new(
		char *attribute_name,
		char *select_datum );

/* Usage */
/* ----- */

/* Returns heap memory or select_datum */
/* ----------------------------------- */
char *query_cell_display_datum(
		enum date_convert_format_enum destination_enum,
		char *select_datum,
		boolean attribute_is_date,
		boolean attribute_is_date_time,
		boolean attribute_is_current_date_time,
		boolean attribute_is_number );

/* Usage */
/* ----- */
LIST *query_cell_primary_data_list(
		LIST *query_row_cell_list );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *query_cell_list_delimited_string(
		char sql_delimiter,
		LIST *query_row_cell_list );

/* Usage */
/* ----- */
LIST *query_cell_attribute_data_list(
		LIST *attribute_name_list,
		LIST *query_row_cell_list );

/* Usage */
/* ----- */
LIST *query_cell_data_list(
		LIST *query_row_cell_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *query_cell_quote_comma_string(
		QUERY_CELL *query_cell,
		boolean first_cell_boolean );

/* Usage */
/* ----- */
void query_cell_free(
		QUERY_CELL *query_cell );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *query_cell_where_string(
		LIST *query_row_cell_list );

/* Usage */
/* ----- */
void query_cell_command_line_replace(
		char *command_line,
		LIST *query_cell_list );

/* Usage */
/* ----- */
QUERY_CELL *query_cell_seek(
		char *attribute_name,
		LIST *cell_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *query_cell_list_display(
		LIST *cell_list );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *query_cell_display(
		QUERY_CELL *query_cell );

/* Usage */
/* ----- */
LIST *query_cell_attribute_build_list(
		LIST *key_list,
		LIST *data_list );

/* Usage */
/* ----- */

/* Returns query_cell_list or null */
/* ------------------------------- */
LIST *query_cell_attribute_list(
		LIST *attribute_name_list,
		LIST *query_row_cell_list );

/* Usage */
/* ----- */
boolean query_cell_list_set_attribute_name(
		LIST *attribute_name_list,
		LIST *query_cell_list /* in/out */ );

typedef struct
{
	char *input;
	LIST *cell_list;

	/* Set externally */
	/* -------------- */
	boolean viewonly_boolean;
} QUERY_ROW;

/* Usage */
/* ----- */
QUERY_ROW *query_row_parse(
		const char sql_delimiter,
		LIST *folder_attribute_append_isa_list,
		enum date_convert_format_enum destination_enum,
		LIST *query_select_name_list,
		boolean input_save_boolean,
		char *input /* stack memory */ );

/* Usage */
/* ----- */
QUERY_ROW *query_row_new(
		boolean input_save_boolean,
		char *input /* stack memory or null */,
		LIST *cell_list );

/* Process */
/* ------- */
QUERY_ROW *query_row_calloc(
		void );

/* Usage */
/* ----- */
QUERY_ROW *query_row_primary(
		QUERY_ROW *query_row );

/* Usage */
/* ----- */
LIST *query_row_delimited_list(
		char sql_delimiter,
		LIST *query_fetch_row_list );

/* Usage */
/* ----- */
void query_row_spreadsheet_output(
		FILE *output_file,
		QUERY_ROW *query_row );

/* Usage */
/* ----- */
void query_row_free(
		QUERY_ROW *query_row );

/* Usage */
/* ----- */

/* Returns -1 if not available */
/* --------------------------- */
int query_row_list_days_range(
		char *date_attribute_name,
		LIST *row_list );

/* Usage */
/* ----- */
QUERY_ROW *query_row_seek(
		LIST *row_list,
		char *date_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *query_row_display(
		QUERY_ROW *query_row );

/* Usage */
/* ----- */
boolean query_row_list_set_viewonly_boolean(
		LIST *query_fetch_row_list /* in/out */,
		LIST *relation_one2m_list );

/* Usage */
/* ----- */
boolean query_row_set_viewonly_boolean(
		QUERY_ROW *query_row /* in/out */,
		LIST *relation_one2m_list );

typedef struct
{
	char *folder_name;
	LIST *query_select_name_list;
	LIST *row_list;
} QUERY_FETCH;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_FETCH *query_fetch_new(
		char *folder_name,
		LIST *folder_attribute_append_isa_list,
		enum date_convert_format_enum destination_enum,
		LIST *query_select_name_list,
		char *query_system_string,
		boolean input_save_boolean );

/* Process */
/* ------- */
QUERY_FETCH *query_fetch_calloc(
		void );

/* Usage */
/* ----- */
void query_fetch_html_display(
		char *folder_name,
		LIST *query_select_name_list,
		LIST *query_fetch_row_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *query_fetch_display_system_string(
		char *folder_name,
		LIST *query_select_name_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FILE *query_fetch_input_pipe(
		char *query_system_string );

enum query_relation {		query_unknown,
				query_equal,
				query_not_equal,
				query_less_than,
				query_less_than_equal_to,
				query_greater_than,
				query_greater_than_equal_to,
				query_between,
				query_begins,
				query_contains,
				query_not_contains,
				query_or,
				query_is_null,
				query_not_null };

typedef struct
{
	enum query_relation query_relation_enum;
	char *character_string;
} QUERY_RELATION;

/* Usage */
/* ----- */
QUERY_RELATION *query_relation_new(
		DICTIONARY *dictioanry,
		char *attribute_name,
		char *datatype_name );

/* Process */
/* ------- */
QUERY_RELATION *query_relation_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *query_relation_key(
		char *query_relation_operator_prefix,
		char *attribute_name );

/* Returns program memory or null */
/* ------------------------------ */
char *query_relation_yes_no_key(
		char *attribute_name );

/* Returns dictionary->hash_table->cell_array->other_data or null */
/* -------------------------------------------------------------- */
char *query_relation_operator_extract(
		DICTIONARY *dictionary,
		char *query_relation_key,
		char *query_relation_yes_no_key );

enum query_relation query_relation_enum(
		char *query_equal,
		char *query_not_equal,
		char *query_less_than,
		char *query_less_than_equal_to,
		char *query_greater_than,
		char *query_greater_than_equal_to,
		char *query_between,
		char *query_begins,
		char *query_contains,
		char *query_not_contains,
		char *query_or,
		char *query_null,
		char *query_not_null,
		char *datatype_name,
		char *query_relation_operator_extract );

/* Returns program memory */
/* ---------------------- */
char *query_relation_character_string(
		enum query_relation query_relation_enum );

typedef struct
{
	char *date_attribute_name;
	char *date_from_data;
	char *date_to_data;
	char *time_attribute_name;
	char *time_from_data;
	char *time_to_data;
	char *where;
} QUERY_DATE_TIME_BETWEEN;

/* Usage */
/* ----- */
LIST *query_date_time_between_list(
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *dictionary,
		char *table_name );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *query_date_time_between_date_attribute_name(
		LIST *attribute_trimmed_list,
		char *time_attribute_name );

/* Usage */
/* ----- */
QUERY_DATE_TIME_BETWEEN *query_date_time_between_new(
		DICTIONARY *dictionary,
		char *table_name,
		char *date_attribute_name,
		char *time_attribute_name );

/* Process */
/* ------- */
QUERY_DATE_TIME_BETWEEN *query_date_time_between_calloc(
		void );

/* Returns component of DICTIONARY */
/* ------------------------------- */
char *query_date_time_between_date_from_data(
		DICTIONARY *dictionary,
		char *date_attribute_name,
		char *prompt_lookup_from_prefix );

/* Returns component of DICTIONARY */
/* ------------------------------- */
char *query_date_time_between_date_to_data(
		DICTIONARY *dictionary,
		char *date_attribute_name,
		char *prompt_lookup_to_prefix );

/* Returns component of DICTIONARY */
/* ------------------------------- */
char *query_date_time_between_time_from_data(
		DICTIONARY *dictionary,
		char *time_attribute_name,
		char *prompt_lookup_from_prefix );

/* Returns component of DICTIONARY */
/* ------------------------------- */
char *query_date_time_between_time_to_data(
		DICTIONARY *dictionary,
		char *time_attribute_name,
		char *prompt_lookup_to_prefix );

/* Returns heap memory */
/* ------------------- */
char *query_date_time_between_where(
		char *table_name,
		char *date_attribute_name,
		char *time_attribute_name,
		char *date_from_data,
		char *date_to_data,
		char *time_from_data,
		char *time_to_data );

/* Usage */
/* ----- */
boolean query_date_time_between_exists(
		char *attribute_name,
		LIST *query_date_time_between_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LIST *query_date_time_between_attribute_name_list(
		LIST *query_date_time_between_list );

typedef struct
{
	char *where;
} QUERY_DROP_DOWN_DATUM;

/* Usage */
/* ----- */
QUERY_DROP_DOWN_DATUM *
	query_drop_down_datum_new(
		char *many_table_name,
		char *foreign_key,
		char *datum );

/* Process */
/* ------- */
QUERY_DROP_DOWN_DATUM *
	query_drop_down_datum_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *query_drop_down_datum_where(
		char *many_table_name,
		char *foreign_key,
		char *datum );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *query_drop_down_datum_list_where(
		LIST *query_drop_down_datum_list,
		char *conjunction );

typedef struct
{
	char *many_folder_name;
	char *one_folder_name;
	char *foreign_key;
	LIST *query_drop_down_datum_list;
} QUERY_MULTI_DROP_DOWN_COLUMN;

/* Usage */
/* ----- */

/* Returns null if exists in list already */
/* -------------------------------------- */
QUERY_MULTI_DROP_DOWN_COLUMN *query_multi_drop_down_column_new(
		char *many_folder_name,
		char *one_folder_name,
		char *many_table_name,
		LIST *query_multi_drop_down_column_list,
		char *foreign_key,
		char *datum );

/* Process */
/* ------- */
QUERY_MULTI_DROP_DOWN_COLUMN *query_multi_drop_down_column_calloc(
		void );

/* Usage */
/* ----- */
QUERY_MULTI_DROP_DOWN_COLUMN *query_multi_drop_down_column_seek(
		char *one_folder_name,
		char *foreign_key,
		LIST *query_multi_drop_down_column_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *query_multi_drop_down_column_where(
		LIST *query_multi_drop_down_column_list );

typedef struct
{
	char *many_folder_name;
	char *one_folder_name;
	LIST *foreign_key_list;
	LIST *query_multi_drop_down_column_list;
	char *query_multi_drop_down_column_where;
} QUERY_MULTI_SELECT_DROP_DOWN;

/* Usage */
/* ----- */
QUERY_MULTI_SELECT_DROP_DOWN *query_multi_select_drop_down_new(
		char *many_folder_name,
		char *one_folder_name,
		char *many_table_name /* optional */,
		DICTIONARY *dictionary,
		LIST *foreign_key_list,
		int highest_index );

/* Process */
/* ------- */
QUERY_MULTI_SELECT_DROP_DOWN *query_multi_select_drop_down_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *query_multi_select_drop_down_key(
		char *foreign_key,
		int index );

typedef struct
{
	char *data_list_string;
	LIST *query_drop_down_datum_list;
	char *query_drop_down_datum_list_where;
} QUERY_DROP_DOWN_ROW;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_DROP_DOWN_ROW *query_drop_down_row_new(
		const char multi_attribute_key_delimiter,
		char *many_table_name,
		LIST *foreign_key_list,
		char *data_list_string );

/* Process */
/* ------- */
QUERY_DROP_DOWN_ROW *query_drop_down_row_calloc(
		void );

typedef struct
{
	LIST *foreign_key_list;
	char *data_list_string;
	QUERY_DROP_DOWN_ROW *query_drop_down_row;
	char *where;
} QUERY_SINGLE_SELECT_DROP_DOWN;

/* Usage */
/* ----- */
QUERY_SINGLE_SELECT_DROP_DOWN *query_single_select_drop_down_new(
		char *many_table_name,
		DICTIONARY *dictionary,
		LIST *foreign_key_list );

/* Process */
/* ------- */
QUERY_SINGLE_SELECT_DROP_DOWN *query_single_select_drop_down_calloc(
		void );

/* Usage */
/* ----- */

/* Returns component of query_single_select_drop_down */
/* -------------------------------------------------- */
char *query_single_select_drop_down_where(
		QUERY_SINGLE_SELECT_DROP_DOWN *
			query_single_select_drop_down );

/* Usage */
/* ----- */
boolean query_single_select_drop_down_boolean(
		DICTIONARY *dictionary,
		LIST *foreign_key_list );

typedef struct
{
	LIST *foreign_key_list;
	char *where;
} QUERY_IS_NULL_DROP_DOWN;

/* Usage */
/* ----- */
QUERY_IS_NULL_DROP_DOWN *query_is_null_drop_down_new(
		char *many_table_name,
		LIST *foreign_key_list );

/* Process */
/* ------- */
QUERY_IS_NULL_DROP_DOWN *query_is_null_drop_down_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *query_is_null_drop_down_where(
		char *many_table_name,
		LIST *foreign_key_list );

/* Usage */
/* ----- */
boolean query_is_null_drop_down_boolean(
		DICTIONARY *dictionary,
		LIST *foreign_key_list,
		int highest_index );

typedef struct
{
	LIST *foreign_key_list;
	char *where;
} QUERY_NOT_NULL_DROP_DOWN;

/* Usage */
/* ----- */
QUERY_NOT_NULL_DROP_DOWN *query_not_null_drop_down_new(
		char *many_table_name,
		LIST *foreign_key_list );

/* Process */
/* ------- */
QUERY_NOT_NULL_DROP_DOWN *query_not_null_drop_down_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *query_not_null_drop_down_where(
		char *many_table_name,
		LIST *foreign_key_list );

/* Usage */
/* ----- */
boolean query_not_null_drop_down_boolean(
		DICTIONARY *dictionary,
		LIST *foreign_key_list,
		int highest_index );

typedef struct
{
	LIST *query_date_time_between_list;
	LIST *list;
	char *where;
} QUERY_ATTRIBUTE_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_ATTRIBUTE_LIST *query_attribute_list_new(
		char *application_name,
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *dictionary,
		int relation_mto1_isa_list_length,
		char *row_security_role_update_list_where );

/* Process */
/* ------- */
QUERY_ATTRIBUTE_LIST *query_attribute_list_calloc(
		void );

/* Returns static memory or null */
/* ----------------------------- */
char *query_attribute_list_table_name(
		char *application_name,
		int relation_mto1_isa_list_length,
		char *row_security_role_update_list_where,
		char *folder_name );

/* Returns heap memory or null.			*/
/* Note: it frees char *where in each list.	*/
/* -------------------------------------------- */
char *query_attribute_list_where(
		LIST *query_date_time_between_list,
		LIST *query_attribute_list );

/* Public */
/* ------ */
LIST *query_attribute_list_name_list(
		LIST *query_date_time_between_list,
		LIST *query_attribute_list );

typedef struct
{
	char *many_folder_name;
	char *one_folder_name;
	int dictionary_key_list_highest_index;
	QUERY_IS_NULL_DROP_DOWN *query_is_null_drop_down;
	QUERY_NOT_NULL_DROP_DOWN *query_not_null_drop_down;
	QUERY_MULTI_SELECT_DROP_DOWN *query_multi_select_drop_down;
	QUERY_SINGLE_SELECT_DROP_DOWN *query_single_select_drop_down;
} QUERY_DROP_DOWN_RELATION;

/* Usage */
/* ----- */
QUERY_DROP_DOWN_RELATION *query_drop_down_relation_new(
		DICTIONARY *dictionary,
		char *many_table_name /* optional */,
		int dictionary_highest_index,
		RELATION_MTO1 *relation_mto1 );

/* Process */
/* ------- */
QUERY_DROP_DOWN_RELATION *query_drop_down_relation_calloc(
		void );

/* Usage */
/* ----- */

/* Note: it frees char *where */
/* -------------------------- */
char *query_drop_down_relation_where(
		LIST *query_drop_down_relation_list );

/* Usage */
/* ----- */
LIST *query_drop_down_relation_attribute_name_list(
		LIST *query_drop_down_relation_list );

typedef struct
{
	int dictionary_highest_index;
	LIST *list;
	char *where;
} QUERY_ISA_DROP_DOWN_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_ISA_DROP_DOWN_LIST *query_isa_drop_down_list_new(
		LIST *relation_mto1_isa_list,
		DICTIONARY *dictionary );

/* Process */
/* ------- */
QUERY_ISA_DROP_DOWN_LIST *query_isa_drop_down_list_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *query_isa_drop_down_list_where(
		LIST *query_drop_down_list_list );

typedef struct
{
	int dictionary_highest_index;
	LIST *query_drop_down_relation_list;
	char *query_drop_down_relation_where;
	LIST *query_drop_down_relation_attribute_name_list;
} QUERY_DROP_DOWN_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_DROP_DOWN_LIST *query_drop_down_list_new(
		LIST *relation_mto1_to_one_list,
		DICTIONARY *dictionary,
		char *many_table_name /* optional */ );

/* Process */
/* ------- */
QUERY_DROP_DOWN_LIST *query_drop_down_list_calloc(
		void );

typedef struct
{
	int relation_mto1_isa_list_length;
	char *query_table_name;
	QUERY_DROP_DOWN_LIST *query_drop_down_list;
	QUERY_ISA_DROP_DOWN_LIST *query_isa_drop_down_list;
	char *drop_down_where;
	QUERY_ATTRIBUTE_LIST *query_attribute_list;
	QUERY_ISA *query_isa;
	char *string;
} QUERY_TABLE_EDIT_WHERE;

/* Usage */
/* ----- */

/* ---------------------------- */
/* Safely returns.		*/
/* Note: frees each where.	*/
/* ---------------------------- */
QUERY_TABLE_EDIT_WHERE *query_table_edit_where_new(
		char *application_name,
		char *folder_name,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		char *security_entity_where,
		DICTIONARY *query_dictionary,
		char *row_security_role_update_list_where );

/* Process */
/* ------- */
QUERY_TABLE_EDIT_WHERE *query_table_edit_where_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *query_table_edit_where_drop_down_where(
		char *query_drop_down_relation_where,
		char *query_isa_drop_down_list_where );

/* Returns heap memory or null. 	*/
/* Note: frees each non-static where.	*/
/* ------------------------------------ */
char *query_table_edit_where_string(
		/* --------------------- */
		/* Static memory or null */
		/* --------------------- */
		char *security_entity_where,
		char *query_drop_down_relation_where,
		char *query_attribute_list_where,
		char *query_isa_join_where,
		char *row_security_role_update_list_where );

/* Usage */
/* ----- */
void query_table_edit_where_free(
		QUERY_TABLE_EDIT_WHERE *query_table_edit_where );

typedef struct
{
	char *attribute_name;
	QUERY_RELATION *query_relation;
	boolean attribute_is_yes_no;
	boolean attribute_is_number;
	boolean attribute_is_date_time;
	boolean attribute_is_current_date_time;
	char *where;
} QUERY_ATTRIBUTE;

/* Usage */
/* ----- */
QUERY_ATTRIBUTE *query_attribute_new(
		DICTIONARY *dictionary,
		char *table_name,
		char *attribute_name,
		char *datatype_name );

/* Process */
/* ------- */
QUERY_ATTRIBUTE *query_attribute_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *query_attribute_is_null_where(
		char *attribute_full_attribute_name );

/* Returns heap memory */
/* ------------------- */
char *query_attribute_not_null_where(
		char *attribute_full_attribute_name );

/* Returns heap memory */
/* ------------------- */
char *query_attribute_begins_where(
		DICTIONARY *dictionary,
		char *table_name,
		char *attribute_name );

/* Returns heap memory */
/* ------------------- */
char *query_attribute_contains_where(
		DICTIONARY *dictionary,
		char *table_name,
		char *attribute_name );

/* Returns heap memory */
/* ------------------- */
char *query_attribute_not_contains_where(
		DICTIONARY *dictionary,
		char *table_name,
		char *attribute_name );

/* Returns heap memory */
/* ------------------- */
char *query_attribute_equal_where(
		DICTIONARY *dictionary,
		char *table_name,
		char *attribute_name,
		boolean attribute_is_number );

/* Returns heap memory */
/* ------------------- */
char *query_attribute_not_equal_where(
		DICTIONARY *dictionary,
		char *table_name,
		char *attribute_name,
		boolean attribute_is_number );

/* Returns heap memory */
/* ------------------- */
char *query_attribute_generic_where(
		DICTIONARY *dictionary,
		char *table_name,
		char *attribute_name,
		char *query_relation_character_string,
		boolean attribute_is_number );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *query_attribute_yes_no_where(
		DICTIONARY *dictionary,
		char *table_name,
		char *attribute_name );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *query_attribute_or_where(
		DICTIONARY *dictionary,
		char *table_name,
		char *attribute_name,
		boolean attribute_is_number );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *query_attribute_between_where(
		DICTIONARY *dictionary,
		char *table_name,
		char *attribute_name,
		boolean attribute_is_number,
		boolean attribute_is_date_time,
		boolean attribute_is_current_date_time );

/* Usage */
/* ----- */

/* Returns component of dictionary or null */
/* --------------------------------------- */
char *query_attribute_from_data(
		DICTIONARY *dictionary,
		char *attribute_name,
		boolean attribute_is_number );

/* Usage */
/* ----- */

/* Returns component of dictionary, heap memory, or null */
/* ----------------------------------------------------- */
char *query_attribute_to_data(
		DICTIONARY *dictionary,
		char *attribute_name,
		boolean attribute_is_number,
		boolean attribute_is_date_time,
		boolean attribute_is_current_date_time );

typedef struct
{
	char *attribute_name;
	char *full_attribute_name;
} QUERY_SELECT;

/* Usage */
/* ----- */
LIST *query_select_table_edit_list(
		char *application_name,
		char *folder_name,
		LIST *folder_attribute_append_isa_list,
		LIST *no_display_name_list,
		LIST *exclude_lookup_attribute_name_list,
		int relation_mto1_isa_list_length,
		char *attribute_not_null );

/* Usage */
/* ----- */
LIST *query_select_primary_list(
		char *application_name,
		char *folder_name,
		LIST *folder_attribute_append_isa_list,
		char *additional_attribute_name,
		int relation_mto1_isa_list_length );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_SELECT *query_select_new(
		char *application_name,
		int relation_mto1_isa_list_length,
		char *attribute_not_null,
		char *folder_name,
		char *attribute_name );

/* Process */
/* ------- */
QUERY_SELECT *query_select_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory or attribute_name */
/* ------------------------------------- */
char *query_select_full_attribute_name(
		char *application_name,
		int relation_mto1_isa_list_length,
		char *attribute_not_null,
		char *folder_name,
		char *attribute_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LIST *query_select_list(
		LIST *folder_attribute_list );

/* Usage */
/* ----- */
boolean query_select_skip_boolean(
		char *folder_name,
		char *folder_attribute_folder_name,
		char *attribute_name,
		LIST *no_display_name_list,
		LIST *exclude_lookup_attribute_name_list,
		int primary_key_index,
		char *attribute_not_null );

/* Usage */
/* ----- */
void query_select_need_but_not_get_boolean(
		boolean *need_but_not_get_boolean,
		char *attribute_not_null,
		char *attribute_name );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *query_select_list_string(
		LIST *query_select_list,
		LIST *common_name_list );

/* Usage */
/* ----- */
LIST *query_select_name_list(
		LIST *query_select_list );

typedef struct
{
	int isa_list_length;
	LIST *query_select_primary_list;
	char *query_select_list_string;
	char *query_from_string;
	QUERY_TABLE_EDIT_WHERE *query_table_edit_where;
	char *query_system_string;
	LIST *query_select_name_list;
	QUERY_FETCH *query_fetch;
} QUERY_PRIMARY_KEY;

/* Usage */
/* ----- */

/* ----------------------- */
/* Safely returns.	   */
/* Note: frees each where. */
/* ----------------------- */
QUERY_PRIMARY_KEY *query_primary_key_fetch(
		char *application_name,
		char *login_name,
		char *folder_name,
		char *security_entity_where,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *query_dictionary,
		char *additional_attribute_name );

/* Process */
/* ------- */
QUERY_PRIMARY_KEY *query_primary_key_calloc(
		void );

int query_primary_key_isa_list_length(
		LIST *relation_mto1_isa_list );

typedef struct
{
	LIST *query_select_table_edit_list;
	char *query_select_list_string;
	LIST *query_select_name_list;
	char *query_from_string;
	QUERY_TABLE_EDIT_WHERE *query_table_edit_where;
	char *query_order_string;
	char *query_system_string;
	enum date_convert_format_enum destination_enum;
	QUERY_FETCH *query_fetch;
} QUERY_TABLE_EDIT;

/* Usage */
/* ----- */

/* ---------------------------- */
/* Safely returns.		*/
/* Note: frees each where.	*/
/* ---------------------------- */
QUERY_TABLE_EDIT *query_table_edit_new(
		char *application_name,
		char *login_name,
		char *folder_name,
		char *security_entity_where,
		LIST *no_display_name_list,
		LIST *exclude_lookup_attribute_name_list,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		LIST *folder_attribute_primary_key_list,
		LIST *folder_attribute_name_list,
		DICTIONARY *query_dictionary,
		DICTIONARY *sort_dictionary,
		/* ------------------------- */
		/* Null if not participating */
		/* ------------------------- */
		ROW_SECURITY_ROLE_UPDATE_LIST *
			row_security_role_update_list,
		int table_edit_query_max_rows );

/* Process */
/* ------- */
QUERY_TABLE_EDIT *query_table_edit_calloc(
		void );

typedef struct
{
	char *query_table_name;
	QUERY_DROP_DOWN_LIST *query_drop_down_list;
	QUERY_ATTRIBUTE_LIST *query_attribute_list;
	char *string;
} QUERY_DROP_DOWN_WHERE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_DROP_DOWN_WHERE *query_drop_down_where_new(
		char *application_name,
		char *one_folder_name,
		LIST *one_folder_attribute_list,
		LIST *relation_mto1_to_one_list,
		char *security_entity_where,
		DICTIONARY *dictionary );

/* Process */
/* ------- */
QUERY_DROP_DOWN_WHERE *query_drop_down_where_calloc(
		void );

/* Returns heap memory or null */
/* --------------------------- */
char *query_drop_down_where_string(
		char *security_entity_where,
		char *query_drop_down_relation_where,
		char *query_attribute_list_where );

typedef struct
{
	LIST *query_select_list;
	char *query_select_list_string;
	LIST *query_select_name_list;
	QUERY_DROP_DOWN_WHERE *query_drop_down_where;
	char *query_order_string;
	char *query_system_string;
	enum date_convert_format_enum destination_enum;
	QUERY_FETCH *query_fetch;
	LIST *query_row_delimited_list;
} QUERY_DROP_DOWN_FETCH;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_DROP_DOWN_FETCH *query_drop_down_fetch_new(
		char *application_name,
		char *login_name,
		char *one_folder_name,
		LIST *one_folder_attribute_list,
		LIST *relation_mto1_list,
		DICTIONARY *drop_down_dictionary,
		SECURITY_ENTITY *security_entity,
		LIST *common_name_list );

/* Process */
/* ------- */
QUERY_DROP_DOWN_FETCH *query_drop_down_fetch_calloc(
		void );

typedef struct
{
	QUERY_DROP_DOWN_WHERE *query_drop_down_where;
	PROCESS *process;
	char *appaserver_error_filename;
	char *command_line;
	LIST *delimited_list;
} QUERY_DROP_DOWN_PROCESS;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_DROP_DOWN_PROCESS *query_drop_down_process_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *state,
		char *many_folder_name,
		char *one_folder_name,
		char *related_column,
		LIST *one_folder_attribute_list,
		char *populate_drop_down_process_name,
		LIST *relation_mto1_to_one_list,
		DICTIONARY *dictionary,
		SECURITY_ENTITY *security_entity );

/* Process */
/* ------- */
QUERY_DROP_DOWN_PROCESS *query_drop_down_process_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *query_drop_down_process_command_line(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *state,
		char *many_folder_name,
		char *related_column,
		char *populate_drop_down_process_name,
		DICTIONARY *dictionary,
		char *where_string,
		char *process_command_line,
		char *appaserver_error_filename );

typedef struct
{
	QUERY_DROP_DOWN_PROCESS *query_drop_down_process;
	QUERY_DROP_DOWN_FETCH *query_drop_down_fetch;
	LIST *delimited_list;
} QUERY_DROP_DOWN;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_DROP_DOWN *query_drop_down_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *state,
		char *many_folder_name,
		char *one_folder_name,
		char *related_column,
		LIST *one_folder_attribute_list,
		char *populate_drop_down_process_name,
		LIST *relation_mto1_to_one_list,
		DICTIONARY *drop_down_dictionary,
		SECURITY_ENTITY *security_entity,
		LIST *common_name_list );

/* Process */
/* ------- */
QUERY_DROP_DOWN *query_drop_down_calloc(
		void );

typedef struct
{
	/* Stub */
} QUERY;

/* Usage */
/* ----- */

/* Returns component of dictionary or null */
/* --------------------------------------- */
char *query_dictionary_extract(
		DICTIONARY *dictionary,
		char *attribute_name,
		/* ------------------ */
		/* Mutually exclusive */
		/* ------------------ */
		char *prompt_lookup_from_prefix,
		/* ------------------ */
		/* Mutually exclusive */
		/* ------------------ */
		char *prompt_lookup_to_prefix,
		boolean attribute_is_number );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *query_extract_key(
		char *attribute_name,
		char *prompt_lookup_from_prefix,
		char *prompt_lookup_to_prefix );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *query_table_name(
		char *application_name,
		char *folder_name,
		int relation_mto1_isa_list_length,
		char *attribute_not_null );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *query_order_string(
		LIST *folder_attribute_primary_key_list,
		LIST *folder_attribute_name_list,
		DICTIONARY *sort_dictionary );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *query_key_list_order_string(
		const char attribute_multi_key_delimiter,
		const char *form_sort_ascend_label,
		const char *form_sort_descend_label,
		LIST *folder_attribute_name_list,
		LIST *sort_dictionary_key_list );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *query_key_order_string(
		const char attribute_multi_key_delimiter,
		LIST *folder_attribute_name_list,
		char *attribute_name_list_string,
		boolean descend_boolean );

/* Usage */
/* ----- */
DICTIONARY *query_fetch_dictionary(
		LIST *select_name_list,
		char *folder_table_name,
		LIST *primary_key_list,
		LIST *primary_data_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *query_drop_down_key(
		LIST *foreign_key_list,
		int index,
		char multi_attribute_key_delimiter );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *query_system_string(
		char *application_name,
		char *select_string,
		char *from_string,
		char *where_string,
		char *group_by_string,
		char *order_string,
		int max_rows );

/* Usage */
/* ----- */

/* Returns heap memory.				*/
/* Note: folder_table_name() is not used here.	*/
/* -------------------------------------------- */
char *query_from_string(
		char *folder_name,
		LIST *relation_mto1_isa_list,
		char *row_security_role_update_list_from );

/* Usage */
/* ----- */

LIST *query_primary_delimited_list(
		char *folder_table_name,
		LIST *primary_key_list,
		LIST *foreign_key_list,
		LIST *foreign_data_list );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *query_data_where(
		char *folder_table_name,
		LIST *where_attribute_name_list,
		LIST *where_attribute_data_list,
		LIST *folder_attribute_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *query_where_display(
		char *where_string,
		int max_length );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *query_prompt_target_frame(
		char *datatype_name,
		pid_t process_id );

/* Usage */
/* ----- */
unsigned long query_row_count(
		char *folder_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *query_row_count_system_string(
		char *appaserver_table_name );

typedef struct
{
	LIST *query_select_list;
	char *query_select_list_string;
	LIST *query_select_name_list;
	char *query_order_string;
	char *query_system_string;
	enum date_convert_format_enum destination_enum;
	QUERY_FETCH *query_fetch;
	LIST *query_row_delimited_list;
} QUERY_CHOOSE_ISA;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_CHOOSE_ISA *query_choose_isa_new(
		char *application_name,
		char *login_name,
		char *one_isa_folder_name,
		LIST *folder_attribute_primary_list,
		char *security_entity_where );

/* Process */
/* ------- */
QUERY_CHOOSE_ISA *query_choose_isa_calloc(
		void );

#define QUERY_SPREADSHEET_MAX_ROWS	1000000

typedef struct
{
	LIST *query_select_table_edit_list;
	char *query_select_list_string;
	LIST *query_select_name_list;
	char *query_from_string;
	QUERY_TABLE_EDIT_WHERE *query_table_edit_where;
	char *query_order_string;
	char *query_system_string;
	enum date_convert_format_enum destination_enum;
} QUERY_SPREADSHEET;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_SPREADSHEET *query_spreadsheet_new(
		char *application_name,
		char *login_name,
		char *folder_name,
		char *security_entity_where,
		DICTIONARY *query_dictionary,
		LIST *no_display_name_list,
		LIST *role_attribute_exlcude_name_list,
		LIST *folder_attribute_append_isa_list,
		LIST *folder_attribute_primary_key_list,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list );

/* Process */
/* ------- */
QUERY_SPREADSHEET *query_spreadsheet_calloc(
		void );

/* Driver */
/* ------ */

/* Returns row_count */
/* ----------------- */
unsigned long query_spreadsheet_output(
		LIST *folder_attribute_append_isa_list,
		enum date_convert_format_enum destination_enum,
		LIST *query_select_name_list,
		char *query_system_string,
		char *output_filename );

/* Process */
/* ------- */

/* Safely returns */
/* -------------- */
FILE *query_spreadsheet_output_file(
		char *output_filename );

#define QUERY_CHART_MAX_ROWS 	1000000

typedef struct
{
	char *date_attribute_name;
	char *time_attribute_name;
	LIST *number_attribute_name_list;
	LIST *select_list;
	char *query_select_list_string;
	LIST *query_select_name_list;
	char *query_from_string;
	QUERY_TABLE_EDIT_WHERE *query_table_edit_where;
	LIST *order_name_list;
	char *query_order_string;
	char *query_system_string;
	QUERY_FETCH *query_fetch;
} QUERY_CHART;

/* Usage */
/* ----- */
QUERY_CHART *query_chart_new(
		char *application_name,
		char *login_name,
		char *folder_name,
		char *security_entity_where,
		DICTIONARY *query_dictionary,
		LIST *no_display_name_list,
		LIST *folder_attribute_append_isa_list,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list );

/* Process */
/* ------- */
QUERY_CHART *query_chart_calloc(
		void );

/* Returns component of folder_attribute_append_isa_list or null */
/* ------------------------------------------------------------- */
char *query_chart_date_attribute_name(
		LIST *folder_attribute_append_isa_list );

/* Returns component of folder_attribute_append_isa_list or null */
/* ------------------------------------------------------------- */
char *query_chart_time_attribute_name(
		LIST *folder_attribute_append_isa_list );

/* Returns list of components of folder_attribute_append_isa_list or null */
/* ---------------------------------------------------------------------- */
LIST *query_chart_number_attribute_name_list(
		LIST *no_display_name_list,
		LIST *folder_attribute_append_isa_list );

/* Safely returns */
/* -------------- */
LIST *query_chart_order_name_list(
		char *query_chart_date_attribute_name,
		char *query_chart_time_attribute_name );

/* Usage */
/* ----- */
LIST *query_chart_select_list(
		char *application_name,
		char *folder_name,
		int relation_mto1_isa_list_length,
		char *query_chart_date_attribute_name,
		char *query_chart_time_attribute_name,
		LIST *query_chart_number_attribute_name_list );

/* Usage */
/* ----- */
void query_chart_free(
		QUERY_CHART *query_chart );

typedef struct
{
	LIST *select_list;
	char *query_select_list_string;
	LIST *query_select_name_list;
	char *query_from_string;
	char *query_group_by_string;
	char *order_string;
	char *query_system_string;
} QUERY_GROUP;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_GROUP *query_group_new(
		char *application_name,
		char *folder_name,
		char *query_table_edit_where_string,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_float_name_list,
		unsigned long group_count_row_count,
		LIST *relation_foreign_key_list );

/* Process */
/* ------- */
QUERY_GROUP *query_group_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *query_group_by_string(
		LIST *foreign_key_list );

/* Usage */
/* ----- */
LIST *query_group_select_list(
		char *application_name,
		char *folder_name,
		LIST *folder_attribute_float_name_list,
		unsigned long group_count_row_count,
		LIST *foreign_key_list,
		int relation_mto1_isa_list_length );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *query_group_order_string(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *query_group_percent_expression(
		unsigned long group_count_row_count );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *query_group_average_expression(
		char *float_attribute_name );

typedef struct
{
	char *select_string;
	LIST *query_drop_down_datum_list;
	char *query_drop_down_datum_list_where;
	char *system_string;
	LIST *delimited_list;
} QUERY_DICTIONARY;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_DICTIONARY *query_dictionary_fetch(
		char *application_name,
		DICTIONARY *dictionary,
		char *folder_name,
		LIST *select_attribute_name_list,
		LIST *where_attribute_name_list );

/* Process */
/* ------- */
QUERY_DICTIONARY *query_dictionary_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *query_dictionary_select_string(
		LIST *select_attribute_name_list );

/* Returns heap memory */
/* ------------------- */
char *query_dictionary_system_string(
		char *query_dictionary_select_string,
		char *folder_table_name,
		char *query_drop_down_datum_list_where );

#endif
