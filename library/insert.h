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
			DICTIONARY *dictionary_separate_row_zero,
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

INSERT_DATA *insert_data_seek(
			char *attribute_name,
			LIST *insert_data_list );

LIST *insert_data_key_data_list(
			LIST *primary_key_list,
			LIST *insert_data_list );

typedef struct
{
	LIST *insert_data_list;
	LIST *insert_data_name_list;
	boolean primary_key_okay;
	char *sql_statement;
	char *command_line;
} INSERT_ROW;

/* Usage */
/* ----- */
INSERT_ROW *insert_row_new(
			char *folder_table_name,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			LIST *primary_key_list,
			LIST *folder_attribute_name_list,
			DICTIONARY *dictionary_separate_row_zero,
			DICTIONARY *dictionary_separate_row,
			char *process_name,
			char *post_change_command_line );

/* Process */
/* ------- */
INSERT_ROW *insert_row_calloc(
			void );

boolean insert_row_primary_key_okay(
			LIST *insert_data_name_list,
			LIST *primary_key_list );

/* Returns heap memory */
/* ------------------- */
char *insert_row_sql_statement(
			char *folder_table_name,
			LIST *insert_data_name_list,
			LIST *insert_data_extract_list );

/* Returns heap memory */
/* ------------------- */
char *insert_row_command_line(
			char *post_change_command_line,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *appaserver_insert_state,
			char *process_name,
			LIST *primary_data_list,
			LIST *insert_data_list );

/* Public */
/* ------ */
LIST *insert_row_sql_statement_list(
			LIST *insert_row_list );

LIST *insert_row_post_change_command_line_list(
			LIST *insert_row_list );

typedef struct
{
	LIST *insert_row_list;
	char *folder_table_name;
	int dictionary_highest_row;
	int row_number;
	DICTIONARY *dictionary_separate_row;
	INSERT_ROW *insert_row;
} INSERT;

/* Usage */
/* ----- */
INSERT *insert_new(	char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			LIST *primary_key_list,
			LIST *folder_attribute_name_list,
			DICTIONARY *dictionary_separate_row_zero,
			DICTIONARY *dictionary_separate_multi_row,
			char *process_name,
			char *post_change_command_line );

/* Process */
/* ------- */
INSERT *insert_calloc( 	void );

#endif
