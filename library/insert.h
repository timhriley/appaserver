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
#include "process.h"
#include "relation.h"
#include "folder_attribute.h"

/* Structures */
/* ---------- */
typedef struct
{
	char *attribute_name;
	char *escaped_replaced_data;
} INSERT_DATA;

typedef struct
{
	LIST *insert_data_list;
	char *sql_statement;
	char *command_line;
} INSERT_ROW;

typedef struct
{
	/* Input */
	/* ----- */
	char *application_name;
	char *folder_name;
	LIST *primary_key_list;
	LIST *relation_mto1_non_isa_list;
	LIST *folder_attribute_name_list;
	LIST *folder_attribute_list;
	DICTIONARY *row_dictionary;
	char *post_change_command_line;

	/* Process */
	/* ------- */
	LIST *insert_row_list;
} INSERT;

/* INSERT operations */
/* ----------------- */
INSERT *insert_calloc( 	void );

INSERT *insert_new(
			char *application_name,
			char *folder_name,
			LIST *primary_key_list,
			LIST *relation_mto1_non_isa_list,
			LIST *folder_attribute_name_list,
			LIST *folder_attribute_list,
			DICTIONARY *row_dictionary,
			char *post_change_command_line );

LIST *insert_sql_statement_list(
			LIST *insert_row_list );

LIST *insert_post_change_command_line_list(
			LIST *insert_row_list );

char *insert_statement(
			char *folder_table_name,
			LIST *insert_data_name_list,
			LIST *insert_date_extract_list );

/* INSERT_ROW operations */
/* --------------------- */
INSERT_ROW *insert_row_calloc(
			void );

INSERT_ROW *insert_row_new(
			char *folder_table_name,
			LIST *primary_key_list,
			LIST *relation_mto1_non_isa_list,
			LIST *folder_attribute_name_list,
			LIST *folder_attribute_list,
			DICTIONARY *row_dictionary,
			char *post_change_command_line,
			int row );

boolean insert_row_primary_key_okay(
			LIST *insert_data_name_list,
			LIST *primary_key_list );

/* Returns heap memory or null */
/* --------------------------- */
char *insert_row_sql_statement(
			char *folder_table_name,
			LIST *data_name_list,
			LIST *data_extract_list );

/* Returns heap memory or null */
/* --------------------------- */
char *insert_row_command_line(
			char *post_change_command_line,
			LIST *insert_data_list );

/* INSERT_DATA operations */
/* ---------------------- */
INSERT_DATA *insert_data_calloc(
			void );

INSERT_DATA *insert_data_new(
			char *attribute_name,
			char *escaped_replaced_data );

LIST *insert_data_relation_extract_list(
			RELATION *relation_mto1,
			LIST *folder_attribute_list,
			DICTIONARY *row_dictionary,
			int row );

INSERT_DATA *insert_data_attribute_extract(
			char *attribute_list,
			LIST *folder_attribute_list,
			DICTIONARY *row_dictionary,
			int row );

LIST *insert_data_name_list(
			LIST *insert_data_list );

LIST *insert_data_extract_list(
			LIST *insert_data_list );

#endif
