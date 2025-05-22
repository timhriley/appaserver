/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/rename_table.h	 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef RENAME_TABLE_H
#define RENAME_TABLE_H

typedef struct
{
	char *old_folder_table_name;
	char *new_folder_table_name;
	char *execute_system_string;
	char *drop_index_name;
	char *drop_index_system_string;
	char *create_table_primary_index_system_string;
	LIST *update_statement_list;
	char *shell_script;
	char *process_filespecification;
} RENAME_TABLE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RENAME_TABLE *rename_table_new(
		char *application_name,
		char *old_folder_name,
		char *new_folder_name,
		char *data_directory );

/* Process */
/* ------- */
RENAME_TABLE *rename_table_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *rename_table_drop_index_system_string(
		char *new_folder_table_name,
		char *rename_table_drop_index_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *rename_table_execute_system_string(
		char *old_folder_table_name,
		char *new_folder_table_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *rename_table_process_filespecification(
		char *application_name,
		char *old_folder_name,
		char *data_directory );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *rename_table_shell_script(
		char *application_name,
		char *rename_table_execute_system_string,
		char *rename_table_drop_index_system_string,
		char *create_table_primary_index_system_string,
		LIST *rename_table_update_statement_list );

/* Usage */
/* ----- */
LIST *rename_table_update_statement_list(
		const char *folder_table,
		const char *role_folder_table,
		const char *relation_table,
		const char *folder_attribute_table,
		const char *foreign_attribute_table,
		const char *folder_row_level_restriction_table,
		const char *process_parameter_table,
		const char *process_set_parameter_table,
		const char *folder_operation_table,
		const char *process_generic_table,
		const char *process_generic_value_table,
		char *old_folder_name,
		char *new_folder_name );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *rename_table_update_statement(
		const char *table,
		char *old_folder_name,
		char *new_folder_name,
		const char *folder_attribute_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *rename_table_system_string(
		char *statement );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *rename_table_drop_index_name(
		const char sql_delimiter,
		char *old_folder_table_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *rename_table_show_index_system_string(
		const char sql_delimiter,
		char *old_folder_table_name );

#endif
