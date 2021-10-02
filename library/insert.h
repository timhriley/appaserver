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
} INSERT_ATTRIBUTE_DATA;

typedef struct
{
	/* Input */
	/* ----- */
	char *application_name;
	char *session_key;
	char *folder_name;
	LIST *primary_key_list;
	LIST *folder_attribute_list;

	/* Process */
	/* ------- */
	LIST *data_list;
	DICTIONARY *row_dictionary;
	LIST *ignore_attribute_name_list;
	boolean insert_row_zero_only;
	boolean dont_remove_tmp_file;
	int rows_inserted;
	char *post_insert_process;
	char *login_name;
	char *tmp_file_name;
	FILE *tmp_file;
	LIST *attribute_list;
	LIST *attribute_data_list;
} INSERT;

/* INSERT operations */
/* ----------------- */
INSERT *insert_calloc( 	void );

INSERT *insert_new(
			char *application_name,
			char *session_key,
			char *folder_name,
			LIST *primary_key_list,
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *row_dictionary,
			DICTIONARY *ignore_dictionary,
			char *post_change_process_command_line );

/* INSERT_ATTRIBUTE_DATA operations */
/* -------------------------------- */
INSERT_ATTRIBUTE_DATA *insert_attribute_data_calloc(
			void );

INSERT_ATTRIBUTE_DATA *insert_attribute_data_new(
			char *attribute_name,
			char *escaped_replaced_data );

LIST *insert_attribute_data_list(
			DICTIONARY *row_dictionary,
			int row,

#endif
