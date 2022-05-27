/* $APPASERVER_HOME/library/insert_database.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef INSERT_H
#define INSERT_H

#include <stdio.h>
#include "list.h"
#include "dictionary.h"
#include "boolean.h"

typedef struct
{
	char *attribute_name;
	char *data;
} INSERT_DATA;

/* Usage */
/* ----- */
INSERT_DATA *insert_data_extract(
			DICTIONARY *row_zero_dictionary,
			DICTIONARY *dictionary_separate_row,
			char *attribute_name );

/* Usage */
/* ----- */
INSERT_DATA *insert_data_new(
			char *attribute_name,
			char *data );

/* Process */
/* ------- */
INSERT_DATA *insert_data_calloc(
			void );

/* Public */
/* ------ */
LIST *insert_data_name_list(
			LIST *insert_data_list );

LIST *insert_data_extract_list(
			LIST *insert_data_list );

LIST *insert_data_key_data_list(
			LIST *primary_key_list,
			LIST *insert_data_list );

typedef struct
{
	LIST *insert_data_list;
	LIST *insert_data_name_list;
	LIST *insert_data_extract_list;
	char *folder_table_name;
	char *sql_statement;
	LIST *insert_data_key_data_list;
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
			LIST *primary_key_list,
			LIST *folder_attribute_name_list,
			DICTIONARY *row_zero_dictionary,
			DICTIONARY *dictionary_separate_row,
			LIST *ignore_name_list,
			char *process_name,
			char *post_change_command_line,
			char *appaserver_error_filename );

/* Process */
/* ------- */
INSERT_FOLDER *insert_folder_calloc(
			void );

boolean insert_folder_primary_key_okay(
			LIST *insert_data_name_list,
			LIST *primary_key_list );

/* Returns heap memory */
/* ------------------- */
char *insert_folder_sql_statement(
			char *folder_table_name,
			LIST *insert_data_name_list,
			LIST *insert_data_extract_list );

/* Returns heap memory */
/* ------------------- */
char *insert_folder_command_line(
			char *post_change_command_line,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *process_name,
			char *appaserver_error_filename,
			char *appaserver_insert_state,
			LIST *insert_data_list,
			LIST *primary_data_list );

typedef struct
{
	LIST *insert_folder_list;
	LIST *folder_attribute_name_list;
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
			DICTIONARY *row_zero_dictionary,
			DICTIONARY *dictionary_separate_row,
			LIST *ignore_name_list,
			char *process_name,
			char *post_change_command_line,
			char *appaserver_error_filename );

/* Process */
/* ------- */
INSERT_ROW *insert_row_calloc(
			void );

typedef struct
{
	LIST *list;
	int dictionary_highest_row;
	LIST *dictionary_name_list;
	DICTIONARY *dictionary_separate_row;
} INSERT_ROW_LIST;

/* Process */
/* ------- */
INSERT_ROW_LIST *insert_row_list_calloc(
			void );

typedef struct
{
	LIST *sql_statement_list;
	LIST *command_line_list;
	INSERT_FOLDER *insert_folder;
} INSERT_FOLDER_SQL_STATEMENT;

/* Usage */
/* ----- */
INSERT_FOLDER_SQL_STATEMENT *
	insert_folder_sql_statement_new(
			LIST *insert_folder_list );

/* Process */
/* ------- */
INSERT_FOLDER_SQL_STATEMENT *
	insert_folder_sql_statement_calloc(
			void );

typedef struct
{
	LIST *list;
	INSERT_ROW *insert_row;
} INSERT_SQL_STATEMENT_LIST;

/* Usage */
/* ----- */
INSERT_SQL_STATEMENT_LIST *
	insert_sql_statement_list_new(
			INSERT_ROW_LIST *insert_row_list );

/* Process */
/* ------- */
INSERT_SQL_STATEMENT_LIST *
	insert_sql_statement_list_calloc(
			void );

/* Driver */
/* ------ */

/* Returns error_message_list_string */
/* --------------------------------- */
char *insert_sql_statement_list_execute(
			INSERT_SQL_STATEMENT_LIST *insert_sql_statement_list );

/* Process */
/* ------- */
LIST *insert_sql_statement_list_extract_list(
			INSERT_SQL_STATEMENT_LIST *insert_sql_statement_list );
			
LIST *insert_sql_statement_list_extract_command_line_list(
			INSERT_SQL_STATEMENT_LIST *insert_sql_statement_list );

typedef struct
{
	INSERT_ROW_LIST *insert_row_list;
	INSERT_SQL_STATEMENT_LIST *insert_sql_statement_list;
} INSERT;

/* Usage */
/* ----- */
INSERT *insert_new(	char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			LIST *primary_key_list,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_isa_list,
			DICTIONARY *row_zero_dictionary,
			DICTIONARY *multi_row_dictionary,
			LIST *ignore_name_list,
			char *process_name,
			char *post_change_command_line );

/* Process */
/* ------- */
INSERT *insert_calloc( 	void );

#endif
