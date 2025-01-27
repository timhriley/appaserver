/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/rename_column.h	 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef RENAME_COLUMN_H
#define RENAME_COLUMN_H

#include "folder_attribute.h"

typedef struct
{
	FOLDER_ATTRIBUTE *folder_attribute;
	char *folder_table_name;
	char *attribute_database_datatype;
	char *execute_system_string;
	LIST *update_statement_list;
	char *shell_script;
	char *process_filename;
} RENAME_COLUMN;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RENAME_COLUMN *rename_column_new(
		char *application_name,
		char *folder_name,
		char *old_attribute_name,
		char *new_attribute_name,
		char *data_directory );

/* Process */
/* ------- */
RENAME_COLUMN *rename_column_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *rename_column_execute_system_string(
		char *old_attribute_name,
		char *new_attribute_name,
		char *folder_table_name,
		char *attribute_database_datatype );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *rename_column_process_filename(
		char *folder_name,
		char *new_attribute_name,
		char *data_directory );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *rename_column_shell_script(
		char *application_name,
		char *rename_column_execute_system_string,
		LIST *rename_column_update_statement_list );

/* Usage */
/* ----- */
LIST *rename_column_update_statement_list(
		const char *attribute_table,
		const char *folder_attribute_table,
		const char *relation_table,
		const char *foreign_attribute_table,
		const char *row_security_role_update_table,
		const char *role_attribute_exclude_table,
		const char *process_parameter_table,
		const char *process_set_parameter_table,
		const char *process_generic_value_table,
		char *old_attribute_name,
		char *new_attribute_name );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *rename_column_update_statement(
		const char *table,
		char *old_attribute_name,
		char *new_attribute_name,
		const char *folder_attribute_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *rename_column_system_string(
		char *statement );

#endif
