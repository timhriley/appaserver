/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/export_subschema.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef EXPORT_SUBSCHEMA_H
#define EXPORT_SUBSCHEMA_H

#include "list.h"
#include "boolean.h"
#include "post_dictionary.h"
#include "dictionary_separate.h"

typedef struct
{
	LIST *folder_name_list;
	char *filespecification;
	char *shell_script_open_string;
	char *shell_script_here_open_string;
	char *shell_script_here_close_string;
	char *shell_script_close_string;
	char *shell_script_execute_bit_system_string;
} EXPORT_SUBSCHEMA;

/* Usage */
/* ----- */
EXPORT_SUBSCHEMA *export_subschema_new(
		char *application_name,
		char *folder_name,
		boolean exclude_roles_boolean,
		char *data_directory );

/* Process */
/* ------- */
EXPORT_SUBSCHEMA *export_subschema_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *export_subschema_filespecification(
		char *application_name,
		char *data_directory,
		char *date_now_yyyy_mm_dd );

/* Safely returns */
/* -------------- */
FILE *export_subschema_process_file(
		char *export_subschema_filespecification );

/* Usage */
/* ----- */
LIST *export_subschema_folder_name_list(
		const char sql_delimiter,
		char *folder_name );

/* Usage */
/* ----- */
void export_subschema_output_folder(
		FILE *export_subschema_process_file,
		char *folder_name );

/* Usage */
/* ----- */
void export_subschema_output_relation(
		FILE *export_subschema_process_file,
		char *folder_name );

/* Process */
/* ------- */
FILE *export_subschema_input_pipe(
		char *appaserver_system_string );

/* Usage */
/* ----- */
void export_subschema_output_attribute(
		FILE *export_subschema_process_file,
		char *folder_name );

/* Usage */
/* ----- */
void export_subschema_output_subschema(
		FILE *export_subschema_process_file,
		char *folder_name );

/* Usage */
/* ----- */
void export_subschema_output_operation(
		FILE *export_subschema_process_file,
		char *folder_name );

/* Process */
/* ------- */

/* Returns static memory or null */
/* ----------------------------- */
char *export_subschema_operation_string(
		const char sql_delimiter,
		char *operation_name,
		boolean output_boolean );

/* Usage */
/* ----- */
void export_subschema_output_standard(
		const char *appaserver_folder_name,
		FILE *export_subschema_process_file,
		char *folder_name );

/* Usage */
/* ----- */
void export_subschema_output_process(
		FILE *export_subschema_process_file,
		const char *process_column_name,
		char *folder_name );

/* Process */
/* ------- */

/* Returns static memory or null */
/* ----------------------------- */
char *export_subschema_process_string(
		const char sql_delimiter,
		char *process_name,
		char *command_line );

/* Usage */
/* ----- */

/* Returns heap memory or "" */
/* ------------------------- */
char *export_subschema_select(
		char *appaserver_folder_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *export_subschema_attribute_where(
		const char *attribute_primary_key,
		char *folder_name );

#endif
