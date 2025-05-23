/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/create_table.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef CREATE_TABLE_H
#define CREATE_TABLE_H

#include "list.h"
#include "boolean.h"
#include "folder.h"
#include "shell_script.h"

#define CREATE_TABLE_EXECUTABLE		"create_table"
#define CREATE_TABLE_MYISAM_ENGINE	"MyISAM"
#define CREATE_TABLE_UNIQUE_SUFFIX	"unique"
#define CREATE_TABLE_ADDITIONAL_SUFFIX	"additional_unique"

typedef struct
{
	FOLDER *folder;
	char *appaserver_table_name;
	char *drop_statement;
	char *statement;
	char *unique_index_statement;
	LIST *additional_unique_index_list;
	LIST *additional_index_list;
	char *shell_script_filespecification;
	LIST *sql_statement_list;
	SHELL_SCRIPT *shell_script;
} CREATE_TABLE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CREATE_TABLE *create_table_new(
		char *application_name,
		char *folder_name,
		boolean execute_boolean,
		char *data_directory );

/* Process */
/* ------- */
CREATE_TABLE *create_table_calloc(
		void );

LIST *create_table_sql_statement_list(
		char *create_table_drop_statement,
		char *create_table_statement,
		char *create_table_unique_index_statement,
		LIST *create_table_additional_unique_list,
		LIST *create_table_additional_index_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *create_table_drop_statement(
		char *create_view_statement,
		char *appaserver_table_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *create_table_statement(
		char *create_view_statement,
		LIST *folder_attribute_list,
		char *data_directory,
		char *index_directory,
		char *storage_engine,
		char *appaserver_table_name );

/* Process */
/* ------- */

/* Returns either parameter */
/* ------------------------ */
char *create_table_storage_engine(
		const char *create_table_myisam_engine,
		char *storage_engine );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *create_table_unique_index_statement(
		const char *create_table_unique_suffix,
		LIST *folder_attribute_primary_key_list,
		char *appaserver_table_name );

/* Usage */
/* ----- */
LIST *create_table_additional_unique_index_list(
		const char *create_table_additional_suffix,
		LIST *folder_attribute_list,
		char *appaserver_table_name );

/* Process */
/* ------- */
LIST *create_table_additional_unique_name_list(
		LIST *folder_attribute_list );

/* Returns static memory */
/* --------------------- */
char *create_table_additional_unique_name(
		const char *create_table_additional_suffix,
		char *attribute_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *create_table_unique_index_name(
		char *create_table_unique_suffix,
		char *appaserver_table_name );

/* Usage */
/* ------ */
LIST *create_table_additional_index_list(
		LIST *folder_attribute_list,
		char *appaserver_table_name );

/* Process */
/* ------- */
LIST *create_table_additional_index_name_list(
		LIST *folder_attribute_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SHELL_SCRIPT *create_table_shell_script(
		char *application_name,
		boolean execute_boolean,
		char *shell_script_filespecification,
		LIST *create_table_sql_statement_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *create_table_primary_index_system_string(
		const char *create_table_unique_suffix,
		LIST *primary_key_list,
		char *appaserver_table_name );

#endif
