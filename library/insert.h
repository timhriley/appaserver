/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/insert.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef INSERT_H
#define INSERT_H

#include <stdio.h>
#include "list.h"
#include "boolean.h"
#include "dictionary.h"
#include "relation_mto1.h"
#include "process.h"

typedef struct
{
	char *attribute_name;
	char *datum;
	int primary_key_index;
	boolean attribute_is_number;
} INSERT_DATUM;

/* Usage */
/* ----- */
INSERT_DATUM *insert_datum_extract(
		DICTIONARY *prompt_dictionary,
		DICTIONARY *multi_row_dictionary,
		int row_number,
		char *attribute_name,
		int primary_key_index,
		boolean attribute_is_number );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INSERT_DATUM *insert_datum_new(
		char *attribute_name,
		char *datum,
		int primary_key_index,
		boolean attribute_is_number );

/* Process */
/* ------- */
INSERT_DATUM *insert_datum_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *insert_datum_sql_statement(
		char *folder_table_name,
		LIST *insert_datum_list );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *insert_datum_attribute_name_list_string(
		LIST *insert_datum_list );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *insert_datum_value_list_string(
		LIST *insert_datum_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *insert_datum_value_string(
		char *datum,
		boolean attribute_is_number );

/* Usage */
/* ----- */
LIST *insert_datum_key_datum_list(
		LIST *insert_datum_list );

typedef struct
{
	char *application_table_name;
	LIST *insert_datum_list;
	char *insert_datum_sql_statement;
	LIST *insert_datum_key_datum_list;
	char *command_line;
} INSERT_FOLDER;

/* Usage */
/* ----- */
INSERT_FOLDER *insert_folder_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *root_folder_attribute_primary_key_list,
		LIST *folder_attribute_primary_key_list,
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *prompt_dictionary,
		DICTIONARY *multi_row_dictionary,
		LIST *ignore_name_list,
		PROCESS *post_change_process,
		char *appaserver_error_filename,
		int row_number,
		LIST *folder_attribute_name_list );

/* Process */
/* ------- */
INSERT_FOLDER *insert_folder_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *insert_folder_primary_data_list_string(
		const char attribute_multi_key_delimiter,
		LIST *insert_data_key_data_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *insert_folder_string_sql_statement(
		const char sql_delimiter,
		char *table_name,
		char *attribute_name_list_string,
		char *delimited_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *insert_folder_value_list_string(
		const char sql_delimiter,
		char *delimited_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *insert_folder_sql_statement_string(
		char *folder_table_name,
		char *attribute_name_list_string,
		char *value_list_string );

typedef struct
{
	INSERT_FOLDER *insert_folder;
	LIST *insert_folder_isa_list;
} INSERT_ROW;

/* Usage */
/* ----- */
INSERT_ROW *insert_row_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *primary_key_list,
		LIST *folder_attribute_append_isa_list,
		LIST *relation_mto1_isa_list,
		DICTIONARY *prompt_dictionary,
		DICTIONARY *dictionary_separate_row,
		LIST *ignore_name_list,
		PROCESS *post_change_process,
		char *appaserver_error_filename,
		int row_number );

/* Process */
/* ------- */
INSERT_ROW *insert_row_calloc(
		void );

typedef struct
{
	LIST *sql_statement_list;
	LIST *command_line_list;
} INSERT_FOLDER_STATEMENT;

/* Usage */
/* ----- */
INSERT_FOLDER_STATEMENT *
	insert_folder_statement_new(
		INSERT_FOLDER *insert_folder,
		LIST *insert_folder_isa_list );

/* Process */
/* ------- */
INSERT_FOLDER_STATEMENT *insert_folder_statement_calloc(
		void );

/* Usage */
/* ----- */
int insert_folder_statement_sql_list_length(
		LIST *insert_folder_statement_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *insert_folder_statement_system_string(
		const char sql_delimiter,
		char *application_table_name,
		char *field_list_string );

typedef struct
{
	int dictionary_highest_index;
	LIST *insert_row_list;
} INSERT_MULTI;

/* Usage */
/* ----- */
INSERT_MULTI *insert_multi_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *folder_attribute_primary_key_list,
		LIST *folder_attribute_append_isa_list,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		DICTIONARY *prompt_dictionary,
		DICTIONARY *multi_row_dictionary /* in/out */,
		LIST *ignore_name_list,
		PROCESS *post_change_process,
		char *appaserver_error_filename );

/* Process */
/* ------- */
INSERT_MULTI *insert_multi_calloc(
		void );

boolean insert_multi_all_primary_null_boolean(
		const char *appaserver_null_string,
		LIST *folder_attribute_primary_key_list,
		DICTIONARY *multi_row_dictionary,
		int row_number );

/* Usage */
/* ----- */
void insert_multi_attribute_default_set(
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *multi_row_dictionary /* in/out */,
		int row_number );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *insert_multi_key(
		char *attribute_name,
		int row_number );

typedef struct
{
	INSERT_FOLDER *insert_folder;
	LIST *insert_folder_isa_list;
} INSERT_ZERO;

/* Usage */
/* ----- */
INSERT_ZERO *insert_zero_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *folder_attribute_primary_key_list,
		LIST *folder_attribute_append_isa_list,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		DICTIONARY *prompt_dictionary,
		LIST *ignore_name_list,
		PROCESS *post_change_process,
		char *appaserver_error_filename );

/* Process */
/* ------- */
INSERT_ZERO *insert_zero_calloc(
		void );

void insert_zero_attribute_default_set(
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *prompt_dictionary /* in/out */,
		LIST *ignore_name_list );

typedef struct
{
	LIST *insert_folder_statement_list;
} INSERT_STATEMENT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INSERT_STATEMENT *insert_statement_new(
		INSERT_ZERO *insert_zero,
		INSERT_MULTI *insert_multi );

/* Process */
/* ------- */
INSERT_STATEMENT *insert_statement_calloc(
		void );

/* Usage */
/* ----- */

/* Returns insert_folder_statement_list */
/* ------------------------------------ */
LIST *insert_statement_multi_list(
		LIST *insert_row_list );

/* Usage */
/* ----- */

/* Returns insert_statement_error_string or null */
/* --------------------------------------------- */
char *insert_statement_execute(
		char *application_name,
		LIST *insert_folder_statement_list,
		char *appaserver_error_filename );

/* Process */
/* ------- */
LIST *insert_statement_extract_sql_list(
		LIST *insert_folder_statement_list );

LIST *insert_statement_extract_command_list(
		LIST *insert_folder_statement_list );

void insert_statement_command_execute(
		LIST *insert_statement_extract_command_list );

typedef struct
{
	char *appaserver_error_filename;
	INSERT_ZERO *insert_zero;
	INSERT_MULTI *insert_multi;
	INSERT_STATEMENT *insert_statement;
	int insert_folder_statement_sql_list_length;
	char *results_string;
} INSERT;

/* Usage */
/* ----- */
INSERT *insert_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *folder_attribute_primary_key_list,
		LIST *folder_attribute_append_isa_list,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		DICTIONARY *prompt_dictionary,
		DICTIONARY *multi_row_dictionary,
		LIST *ignore_name_list,
		PROCESS *post_change_process );

/* Process */
/* ------- */
INSERT *insert_calloc(
		void );

/* Usage */
/* ----- */
INSERT *insert_automatic_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *folder_attribute_primary_key_list,
		LIST *folder_attribute_append_isa_list,
		LIST *relation_mto1_isa_list,
		DICTIONARY *prompt_dictionary,
		DICTIONARY *multi_row_dictionary,
		PROCESS *post_change_process );

/* Usage */
/* ----- */
DICTIONARY *insert_automatic_multi_row_dictionary(
		LIST *folder_attribute_non_primary_name_list,
		DICTIONARY *multi_row_dictionary );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *insert_results_string(
		char *folder_name,
		LIST *relation_mto1_isa_list,
		int insert_folder_statement_sql_list_length );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *insert_results_folder_display(
		char *folder_name,
		LIST *relation_mto1_folder_name_list );

/* Usage */
/* ----- */

/* Returns login_name or null */
/* -------------------------- */
char *insert_login_name(
		char *login_name,
		LIST *role_attribute_exclude_name_list,
		LIST *folder_attribute_append_isa_list );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *insert_folder_command_line(
		char *post_change_process_command_line,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *post_change_process_name,
		char *appaserver_error_filename,
		char *appaserver_insert_state,
		LIST *insert_data_list,
		char *insert_folder_primary_data_list_string );

/* Usage */
/* ----- */
void insert_copy_common_set(
		DICTIONARY *dictionary /* in/out */,
		LIST *folder_attribute_list,
		LIST *relation_mto1_list );

#endif
