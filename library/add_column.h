/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/add_column.h	 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef ADD_COLUMN_H
#define ADD_COLUMN_H

#include "folder_attribute.h"

typedef struct
{
	FOLDER_ATTRIBUTE *folder_attribute;
	char *folder_table_name;
	char *attribute_database_datatype;
	char *execute_system_string;
	char *shell_script;
	char *process_filename;
} ADD_COLUMN;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ADD_COLUMN *add_column_new(
		char *application_name,
		char *folder_name,
		char *attribute_name,
		char *data_directory );

/* Process */
/* ------- */
ADD_COLUMN *add_column_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *add_column_execute_system_string(
		char *attribute_name,
		char *folder_table_name,
		char *attribute_database_datatype );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *add_column_process_filename(
		char *folder_name,
		char *attribute_name,
		char *data_directory );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *add_column_shell_script(
		char *application_name,
		FOLDER_ATTRIBUTE *folder_attribute,
		char *folder_table_name,
		char *add_column_execute_system_string );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *add_column_shell_script_system_string(
		char *statement );

/* Usage */
/* ----- */

/* Returns static memory or null */
/* ----------------------------- */
char *add_column_relation_insert_system_string(
		char *relation_table,
		char *relation_primary_key,
		char *folder_name,
		char *attribute_name );

#endif
