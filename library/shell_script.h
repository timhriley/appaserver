/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/shell_script.h				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#ifndef SHELL_SCRIPT_H
#define SHELL_SCRIPT_H

#include "list.h"

#define SHELL_SCRIPT_HERE_LABEL		"shell_all_done"

typedef struct
{
} SHELL_SCRIPT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SHELL_SCRIPT *shell_script_new(
		char *application_name,
		char *shell_filename,
		LIST *sql_statement_list,
		boolean tee_appaserver_boolean,
		boolean html_paragraph_wrapper_boolean );

/* Process */
/* ------- */
SHELL_SCRIPT *shell_script_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *shell_script_open_string(
		char *application_name );

/* Returns static memory */
/* --------------------- */
char *shell_script_here_open_string(
		const char *shell_script_here_label );

/* Returns static memory */
/* --------------------- */
char *shell_script_here_close_string(
		const char *shell_script_here_label,
		boolean tee_appaserver_boolean,
		boolean html_paragraph_wrapper_boolean );

/* Returns program memory */
/* ---------------------- */
char *shell_script_close_string(
		void );

/* Public */
/* ------ */
void shell_script_spool_string_list(
		char *application_name,
		char *shell_filename,
		LIST *sql_statement_list );

#endif
