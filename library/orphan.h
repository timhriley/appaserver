/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/orphan.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and Freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef ORPHAN_H
#define ORPHAN_H

#include "boolean.h"
#include "list.h"
#include "relation_mto1.h"

typedef struct
{
	char *role_name;
	LIST *folder_name_list;
} ORPHAN_ROLE;

/* Usage */
/* ----- */
LIST *orphan_role_list(
		char *folder_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ORPHAN_ROLE *orphan_role_new(
		char *role_name );

/* Process */
/* ------- */
ORPHAN_ROLE *orphan_role_calloc(
		void );

/* Usage */
/* ----- */
boolean orphan_role_folder_name_boolean(
		LIST *orphan_role_list,
		char *folder_name );

typedef struct
{
	RELATION_MTO1 *relation_mto1;
	char *many_table_name;
	char *one_table_name;
	char *string;
	char *system_string;
	LIST *orphan_data_list;
} ORPHAN_SUBQUERY;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ORPHAN_SUBQUERY *orphan_subquery_new(
		char *application_name,
		char *folder_name,
		RELATION_MTO1 *relation_mto1 );

/* Process */
/* ------- */
ORPHAN_SUBQUERY *orphan_subquery_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *orphan_subquery_string(
		RELATION_MTO1 *relation_mto1,
		char *many_table_name,
		char *one_table_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *orphan_subquery_system_string(
		char *orphan_subquery_string );

/* Usage */
/* ----- */
boolean orphan_subquery_clean_boolean(
		LIST *orphan_subquery_list );

/* Usage */
/* ----- */
void orphan_insert_list_display(
		boolean stdout_boolean,
		char *folder_name,
		LIST *orphan_insert_list );

/* Process */
/* ------- */
FILE *orphan_insert_display_pipe(
		boolean stdout_boolean,
		char *folder_name );

/* Driver */
/* ------ */
void orphan_subquery_string_stdout(
		LIST *orphan_subquery_list );

typedef struct
{
	char *one_table_name;
	char *orphan_data;
	char *statement;
} ORPHAN_INSERT_STATEMENT;

/* Usage */
/* ----- */
LIST *orphan_insert_statement_list(
		char *one_table_name,
		LIST *primary_key_list,
		LIST *orphan_data_list );

/* Usage */
/* ----- */
ORPHAN_INSERT_STATEMENT *orphan_insert_statement_new(
		char *one_table_name,
		LIST *primary_key_list,
		char *orphan_data );

/* Process */
/* ------- */
ORPHAN_INSERT_STATEMENT *orphan_insert_statement_calloc(
		void );

typedef struct
{
	LIST *orphan_insert_statement_list;
} ORPHAN_INSERT;

/* Usage */
/* ----- */
LIST *orphan_insert_list(
		LIST *orphan_subquery_list );

/* Usage */
/* ----- */
ORPHAN_INSERT *orphan_insert_new(
		ORPHAN_SUBQUERY *orphan_subquery );

/* Process */
/* ------- */
ORPHAN_INSERT *orphan_insert_calloc(
		void );

/* Usage */
/* ----- */
void orphan_insert_execute(
		LIST *orphan_insert_list );

typedef struct
{
	char *many_table_name;
	char *orphan_data;
	char *delete_key_list_data_string_sql;
} ORPHAN_DELETE_STATEMENT;

/* Usage */
/* ----- */
LIST *orphan_delete_statement_list(
		char *many_table_name,
		LIST *relation_foreign_key_list,
		LIST *orphan_data_list );

/* Usage */
/* ----- */
ORPHAN_DELETE_STATEMENT *orphan_delete_statement_new(
		const char sql_delimiter,
		char *many_table_name,
		LIST *relation_foreign_key_list,
		char *orphan_data );

/* Process */
/* ------- */
ORPHAN_DELETE_STATEMENT *orphan_delete_statement_calloc(
		void );

typedef struct
{
	char *many_table_name;
	char *one_table_name;
	LIST *relation_foreign_key_list;
	LIST *orphan_delete_statement_list;
} ORPHAN_DELETE;

/* Usage */
/* ----- */
LIST *orphan_delete_list(
		LIST *orphan_subquery_list );

/* Usage */
/* ----- */
ORPHAN_DELETE *orphan_delete_new(
		char *many_table_name,
		char *one_table_name,
		LIST *relation_foreign_key_list,
		LIST *orphan_data_list );

/* Process */
/* ------- */
ORPHAN_DELETE *orphan_delete_calloc(
		void );

/* Usage */
/* ----- */
void orphan_delete_list_display(
		boolean stdout_boolean,
		LIST *orphan_delete_list );

/* Process */
/* ------- */
FILE *orphan_delete_display_pipe(
		boolean stdout_boolean,
		char *one_table_name );

/* Usage */
/* ----- */
void orphan_delete_execute(
		LIST *orphan_delete_list );

typedef struct
{
	FOLDER *folder;
	LIST *relation_mto1_list;
	LIST *relation_mto1_isa_list;
} ORPHAN_FOLDER_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ORPHAN_FOLDER_INPUT *orphan_folder_input_new(
		char *role_name,
		char *folder_name );

/* Process */
/* ------- */
ORPHAN_FOLDER_INPUT *orphan_folder_input_calloc(
		void );

typedef struct
{
	char *folder_name;
	ORPHAN_FOLDER_INPUT *orphan_folder_input;
	LIST *orphan_subquery_list;
	LIST *orphan_insert_list;
	LIST *orphan_delete_list;
} ORPHAN_FOLDER;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ORPHAN_FOLDER *orphan_folder_new(
		char *application_name,
		char *role_name,
		char *folder_name,
		boolean delete_boolean );

/* Process */
/* ------- */
ORPHAN_FOLDER *orphan_folder_calloc(
		void );

/* Usage */
/* ----- */
boolean orphan_folder_clean_boolean(
		LIST *orphan_folder_list );

typedef struct
{
	char *folder_name;
	LIST *orphan_role_list;
	LIST *orphan_folder_list;
} ORPHAN;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ORPHAN *orphan_new(
		char *application_name,
		char *folder_name,
		boolean delete_boolean );

/* Process */
/* ------- */
ORPHAN *orphan_calloc(
		void );

/* Returns folder_name or null */
/* --------------------------- */
char *orphan_folder_name(
		char *folder_name );

/* Driver */
/* ------ */
void orphan_clean_output(
		boolean stdout_boolean,
		char *orphan_folder_name );

#endif

