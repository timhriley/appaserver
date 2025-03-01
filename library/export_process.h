/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/export_process.h				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef EXPORT_PROCESS_H
#define EXPORT_PROCESS_H

#include "list.h"
#include "boolean.h"

typedef struct
{
	LIST *name_list;
	LIST *process_parameter_list;
	char *filespecification;
	char *shell_script_open_string;
	char *shell_script_here_open_string;
	char *shell_script_here_close_string;
	char *shell_script_close_string;
	char *shell_script_execute_bit_system_string;
} EXPORT_PROCESS_SHELL;

/* Usage */
/* ----- */
EXPORT_PROCESS_SHELL *export_process_shell_new(
		char *application_name,
		char *export_process_name,
		boolean exclude_roles_boolean,
		char *data_directory );

/* Process */
/* ------- */
EXPORT_PROCESS_SHELL *export_process_shell_calloc(
		void );

LIST *export_process_shell_name_list(
		char *export_process_name,
		const char delimiter );

/* Returns static memory */
/* --------------------- */
char *export_process_shell_filespecification(
		char *application_name,
		char *data_directory,
		char *date_now_yyyy_mm_dd );

typedef struct
{
	LIST *process_parameter_list;
} EXPORT_PROCESS_FILE;

/* Usage */
/* ----- */
EXPORT_PROCESS_FILE *export_process_file_new(
		char *filename,
		LIST *process_name_list );

/* Process */
/* ------- */
EXPORT_PROCESS_FILE *
	export_process_file_calloc(
		void );

typedef struct
{
	/* Stub */
} EXPORT_PROCESS;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FILE *export_process_stream(
		char *filename,
		const char *mode );

/* Usage */
/* ----- */
void export_process_output_process(
		FILE *export_process_stream,
		char *process_name );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *export_process_escape(
		char *string_pipe_input );

/* Usage */
/* ----- */
void export_process_output_standard(
		char *table_name,
		FILE *export_process_stream,
		char *process_name );

/* Process */
/* ------- */
FILE *export_process_input_pipe(
		char *appaserver_system_string );

/* Usage */
/* ----- */
void export_process_output_prompt(
		FILE *export_process_stream,
		LIST *process_parameter_list );

/* Usage */
/* ----- */
void export_process_output_drop_down_prompt(
		FILE *export_process_stream,
		LIST *process_parameter_list );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *export_process_drop_down_prompt_where(
		LIST *process_parameter_list );

/* Usage */
/* ----- */
void export_process_output_drop_down_prompt_data(
		FILE *export_process_stream,
		LIST *process_parameter_list );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *export_process_drop_down_prompt_data_where(
		LIST *process_parameter_list );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *export_process_prompt_where(
		LIST *process_parameter_list );

/* Usage */
/* ----- */
void export_process_output_process_group(
		FILE *export_process_file,
		char *process_name );

/* Usage */
/* ----- */

/* Returns heap memory or "" */
/* ------------------------- */
char *export_process_select(
		char *table_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *export_process_delete_sql_statement(
		char *table_name,
		char *where_string );

/* Usage */
/* ----- */
void export_process_output_populate_name(
		FILE *export_process_stream,
		LIST *process_parameter_list,
		boolean helper_name_boolean );

#endif
