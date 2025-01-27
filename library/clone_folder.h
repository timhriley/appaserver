/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/clone_folder.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef CLONE_FOLDER_H
#define CLONE_FOLDER_H

#include <stdio.h>
#include "list.h"
#include "boolean.h"

typedef struct
{
	LIST *sql_statement_list;
	char *folder_table_name;
	char *delete_statement;
	LIST *folder_attribute_list;
	LIST *folder_attribute_name_list;
	int piece_offset;
	char *select;
	char *where;
	char *appaserver_system_string;
	FILE *input_pipe;
	char *filename;
} CLONE_FOLDER;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CLONE_FOLDER *clone_folder_new(
		char *application_name,
		char *folder_name,
		char *attribute_name,
		char *old_data,
		char *new_data,
		boolean delete_boolean,
		char *data_directory );

/* Process */
/* ------- */
CLONE_FOLDER *clone_folder_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *clone_folder_delete_statement(
		char *folder_table_name,
		char *clone_folder_where );

/* Safely returns */
/* -------------- */
FILE *clone_folder_input_pipe(
		char *appaserver_system_string );

/* Returns static memory */
/* --------------------- */
char *clone_folder_filename(
		char *folder_name,
		char *data_directory,
		char *date_yyyy_mm_dd );

/* Usage */
/* ----- */
int clone_folder_piece_offset(
		char *attribute_name,
		LIST *folder_attribute_name_list );

/* Usage */
/* ----- */

/* Returns heap memory or "" */
/* ------------------------- */
char *clone_folder_select(
		LIST *folder_attribute_name_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *clone_folder_where(
		char *attribute_name,
		char *old_data );

/* Driver */
/* ------ */
void clone_folder_display(
		LIST *sql_statement_list );

/* Driver */
/* ------ */
void clone_folder_execute(
		LIST *sql_statement_list );

/* Process */
/* ------- */

/* Returns program memory */
/* ---------------------- */
char *clone_folder_execute_system_string(
		void );

/* Safely returns */
/* -------------- */
FILE *clone_folder_execute_pipe(
		char *clone_folder_execute_system_string );

#endif
