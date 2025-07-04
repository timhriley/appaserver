/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/folder_attribute.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FOLDER_ATTRIBUTE_H
#define FOLDER_ATTRIBUTE_H

#include "list.h"
#include "boolean.h"
#include "relation.h"
#include "attribute.h"

#define FOLDER_ATTRIBUTE_TABLE			"table_column"
#define SORT_ORDER_ATTRIBUTE_NAME		"sort_order"
#define DISPLAY_ORDER_ATTRIBUTE_NAME		"display_order"
#define SEQUENCE_NUMBER_ATTRIBUTE_NAME		"sequence_number"

#define FOLDER_ATTRIBUTE_SELECT	"table_name,"			\
				"column_name,"			\
				"primary_key_index,"		\
				"display_order,"		\
				"omit_insert_yn,"		\
				"omit_insert_prompt_yn,"	\
				"omit_update_yn,"		\
				"additional_unique_index_yn,"	\
				"additional_index_yn,"		\
				"insert_required_yn,"		\
				"lookup_required_yn,"		\
				"default_value"

typedef struct
{
	char *folder_name;
	char *attribute_name;
	int primary_key_index;
	int display_order;
	boolean omit_insert;
	boolean omit_insert_prompt;
	boolean omit_update;
	boolean additional_unique_index;
	boolean additional_index;
	boolean insert_required;
	boolean lookup_required;
	char *default_value;
	char *prompt;
	ATTRIBUTE *attribute;
} FOLDER_ATTRIBUTE;

/* Usage */
/* ----- */
LIST *folder_attribute_list(
		char *folder_name,
		LIST *role_attribute_exclude_name_list,
		boolean fetch_attribute,
		boolean cache_boolean );

/* Usage */
/* ----- */
LIST *folder_attribute_cache_list(
		char *folder_name,
		LIST *role_attribute_exclude_lookup_list,
		boolean fetch_attribute );

/* Usage */
/* ----- */
LIST *folder_attribute_get_list(
		boolean fetch_attribute );

/* Usage */
/* ----- */
LIST *folder_attribute_fetch_list(
		char *folder_name,
		LIST *role_attribute_exclude_name_list,
		boolean fetch_attribute );

/* Usage */
/* ----- */
FOLDER_ATTRIBUTE *folder_attribute_parse(
		char *input,
		boolean fetch_attribute );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *folder_attribute_prompt(
		char *attribute_name,
		int primary_key_index );

/* Usage */
/* ----- */
FOLDER_ATTRIBUTE *folder_attribute_new(
		char *folder_name,
		char *attribute_name );

/* Process */
/* ------- */
FOLDER_ATTRIBUTE *folder_attribute_calloc(
		void );

/* Usage */
/* ----- */
boolean folder_attribute_exists(
		char *folder_name,
		char *attribute_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *folder_attribute_exists_system_string(
		const char *folder_attribute_table,
		char *folder_attribute_primary_where );

boolean folder_attribute_exists_boolean(
		char *folder_attribute_exists_system_string );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FOLDER_ATTRIBUTE *folder_attribute_fetch(
		char *folder_name,
		char *attribute_name,
		boolean fetch_attribute );

/* Usage */
/* ----- */
LIST *folder_attribute_system_list(
		char *appaserver_system_string,
		boolean fetch_attribute );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *folder_attribute_system_string(
		char *folder_attribute_select,
		char *folder_attribute_table,
		char *where_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *folder_attribute_insert_statement(
		char *folder_attribute_select,
		char *folder_attribute_table,
		FOLDER_ATTRIBUTE *folder_attribute );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *folder_attribute_insert_statement_string(
		char *folder_attribute_select,
		char *folder_attribute_table,
		char *folder_name,
		char *attribute_name,
		char *primary_string,
		char *display_order_string,
		char *omit_insert_string,
		char *omit_insert_prompt_string,
		char *omit_update_string,
		char *additional_unique_index_string,
		char *additional_index_string,
		char *insert_required_string,
		char *lookup_required_string,
		char *default_value_string );

/* Usage */
/* ----- */

/* ------------------------- */
/* Returns heap memory or "" */
/* ------------------------- */
char *folder_attribute_list_string(
		char *folder_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *folder_attribute_where_string(
		LIST *folder_attribute_list,
		LIST *primary_key_list,
		LIST *primary_data_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *folder_attribute_fetch_where_string(
		const char *folder_primary_key,
		char *folder_name,
		LIST *role_attribute_exclude_name_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *folder_attribute_primary_where_string(
		const char *folder_primary_key,
		const char *attribute_primary_key,
		char *folder_name,
		char *attribute_name );

/* Usage */
/* ----- */

/* Return static or program memory */
/* ------------------------------- */
char *folder_attribute_exclude_where_string(
		const char *attribute_primary_key,
		LIST *role_attribute_exclude_name_list );

/* Usage */
/* ----- */
LIST *folder_attribute_append_isa_list(
		LIST *folder_attribute_list,
		LIST *relation_mto1_isa_list );

/* Usage */
/* ----- */
LIST *folder_attribute_seek_list(
		char *folder_name,
		LIST *role_attribute_exclude_name_list,
		LIST *folder_attribute_list );

/* Usage */
/* ----- */
FOLDER_ATTRIBUTE *folder_attribute_seek(
		char *attribute_name,
		LIST *folder_attribute_list );

/* Usage */
/* ----- */
LIST *folder_attribute_viewonly_attribute_name_list(
		LIST *folder_attribute_append_isa_list );

/* Public */
/* ------ */
LIST *folder_attribute_name_list(
		char *folder_name,
		LIST *folder_attribute_append_isa_list );

LIST *folder_attribute_folder_prefixed_name_list(
		LIST *folder_attribute_list );

LIST *folder_attribute_primary_list(
		char *folder_name,
		LIST *folder_attribute_list );

LIST *folder_attribute_non_primary_list(
		LIST *folder_attribute_list );

LIST *folder_attribute_primary_key_list(
		char *folder_name,
		LIST *folder_attribute_list );

LIST *folder_attribute_non_primary_name_list(
		LIST *folder_attribute_list );

LIST *folder_attribute_number_list(
		LIST *folder_attribute_list );

LIST *folder_attribute_date_name_list(
		LIST *folder_attribute_list );

int folder_attribute_calendar_date_name_list_length(
		LIST *folder_attribute_list );

LIST *folder_attribute_time_name_list(
		LIST *folder_attribute_list );

LIST *folder_attribute_float_name_list(
		LIST *folder_attribute_list );

LIST *folder_attribute_integer_name_list(
		LIST *folder_attribute_list );

LIST *folder_attribute_number_name_list(
		LIST *folder_attribute_list );

/* Returns heap memory */
/* ------------------- */
char *folder_attribute_sort_attribute_name(
		LIST *folder_attribute_list );

/* Returns heap memory */
/* ------------------- */
char *folder_attribute_list_display(
		LIST *folder_attribute_list );

LIST *folder_attribute_fetch_primary_key_list(
		char *folder_name );

LIST *folder_attribute_name_list_attribute_list(
		LIST *folder_name_list );

LIST *folder_attribute_lookup_required_name_list(
		LIST *folder_attribute_list );

LIST *folder_attribute_insert_required_name_list(
		LIST *folder_attribute_list );

LIST *folder_attribute_upload_filename_list(
		LIST *folder_attribute_list );

#endif
