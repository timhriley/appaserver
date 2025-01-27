/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/export_process_set.h			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#ifndef EXPORT_PROCESS_SET_H
#define EXPORT_PROCESS_SET_H

#include "list.h"
#include "boolean.h"
#include "post_dictionary.h"
#include "dictionary_separate.h"

typedef struct
{
	DICTIONARY_SEPARATE *dictionary_separate;
	LIST *name_list;
	boolean exclude_roles_boolean;
	char *filename;
	char *shell_script_open_string;
	char *shell_script_here_open_string;
	char *shell_script_here_close_string;
	char *shell_script_close_string;
	char *execute_bit_system_string;
} EXPORT_PROCESS_SET;

/* Usage */
/* ----- */
EXPORT_PROCESS_SET *export_process_set_new(
		char *application_name,
		char *export_process_set_name,
		boolean exclude_roles_boolean,
		char *data_directory );

/* Process */
/* ------- */
EXPORT_PROCESS_SET *export_process_set_calloc(
		void );

LIST *export_process_set_name_list(
		char *export_process_set_name );

/* Returns static memory */
/* --------------------- */
char *export_process_set_filename(
		char *application_name,
		char *data_directory,
		char *date_now_yyyy_mm_dd );

/* Safely returns */
/* -------------- */
FILE *export_process_set_file(
		char *export_process_set_filename );

/* Returns heap memory */
/* ------------------- */
char *export_process_set_execute_bit_system_string(
		char *export_process_set_filename );

/* Usage */
/* ----- */
void export_process_set_output_process_set(
		FILE *export_process_set_file,
		char *process_set_name );

/* Usage */
/* ----- */
void export_process_set_output_standard(
		const char *appaserver_table_name,
		FILE *export_process_set_file,
		char *process_set_name );

/* Process */
/* ------- */
FILE *export_process_set_input_pipe(
		char *appaserver_system_string );

/* Usage */
/* ----- */
void export_process_set_output_prompt(
		FILE *export_process_set_file,
		char *process_set_name );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *export_process_set_prompt_where(
		char *process_set_name );

/* Usage */
/* ----- */
void export_process_set_output_drop_down_prompt(
		FILE *export_process_set_file,
		char *process_set_name );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *export_process_set_drop_down_prompt_where(
		char *process_set_name );

/* Usage */
/* ----- */
void export_process_set_output_drop_down_prompt_data(
		FILE *export_process_set_file,
		char *process_set_name );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *export_process_set_drop_down_prompt_data_where(
		char *process_set_name );

/* Usage */
/* ----- */
void export_process_set_output_process_group(
		FILE *export_process_set_file,
		char *process_set_name );

/* Usage */
/* ----- */

/* Returns heap memory or "" */
/* ------------------------- */
char *export_process_set_select(
		char *folder_name );

#endif
