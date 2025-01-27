/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/execute_select.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef EXECUTE_SELECT_H
#define EXECUTE_SELECT_H

#include "boolean.h"
#include "list.h"
#include "select_statement.h"
#include "role.h"
#include "appaserver_link.h"

#define EXECUTE_BEGIN_STATE			0
#define EXECUTE_SELECT_STATE			1
#define EXECUTE_FROM_STATE			2

#define EXECUTE_SELECT_ATTRIBUTE_VIOLATION	\
		"You are trying to select an excluded column"

#define EXECUTE_SELECT_FOLDER_VIOLATION	\
		"You are trying to select from an excluded table"

typedef struct
{
	char *statement;
	SELECT_STATEMENT *select_statement;
	ROLE *role;
	char *error_message;
	LIST *column_name_list;
	LIST *folder_name_list;
	boolean first_column_asterisk_boolean;
	char *html_heading_string;
	char *html_system_string;
	APPASERVER_LINK *appaserver_link;
	char *prompt_filename;
	char *output_filename;
	char *spreadsheet_heading_string;
	char *spreadsheet_system_string;
	char *error_filename;
	char *input_system_string;
	char *appaserver_link_anchor_html;
} EXECUTE_SELECT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
EXECUTE_SELECT *execute_select_new(
		char *application_name,
		char *session_key,
		char *role_name,
		char *select_statement_title,
		char *login_name,
		char *statement,
		char *data_directory );

/* Process */
/* ------- */
EXECUTE_SELECT *execute_select_calloc(
		void );

boolean execute_select_first_column_asterisk_boolean(
		char *first_column_name );

/* Returns static memory */
/* --------------------- */
char *execute_select_error_filename(
		char *session_key );

/* Driver */
/* ------ */

/* Returns row_count */
/* ----------------- */
int execute_select_generate(
		char *html_system_string,
		char *output_filename,
		char *spreadsheet_heading_string,
		char *spreadsheet_system_string,
		char *error_filename,
		char *input_system_string );

/* Process */
/* ------- */

/* Safely returns */
/* -------------- */
FILE *execute_select_html_output_pipe(
		char *html_system_string );

/* Safely returns */
/* -------------- */
FILE *execute_select_spreadsheet_output_pipe(
		char *spreadsheet_system_string );

/* Safely returns */
/* -------------- */
FILE *execute_select_input_pipe(
		char *input_system_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *execute_select_html_system_string(
		char sql_delimiter,
		int html_table_queue_top_bottom,
		char *execute_select_html_heading_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *execute_select_spreadsheet_system_string(
		char sql_delimiter,
		char *output_filename );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *execute_select_input_system_string(
		const char sql_delimiter,
		char *statement,
		char *execute_select_error_filename );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *execute_select_html_heading_string(
		LIST *column_name_list );

/* Usage */
/* ----- */
LIST *execute_select_column_name_list(
		char **error_message,
		char *role_name,
		LIST *role_attribute_exclude_list,
		char *statement );

/* Usage */
/* ----- */
LIST *execute_select_asterisk_name_list(
		char *folder_name );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *execute_select_spreadsheet_heading_string(
		const char sql_delimiter,
		LIST *column_name_list );

/* Usage */
/* ----- */
LIST *execute_select_folder_name_list(
		char **error_message,
		char *role_name,
		char *statement );

/* Usage */
/* ----- */
boolean execute_select_exists_attribute_exclude(
		LIST *role_attribute_exclude_list,
		char *attribute_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *execute_select_exclude_violation(
		char *role_name,
		char *attribute_name );

/* Usage */
/* ----- */
boolean execute_select_exists_role_folder(
		const char *role_system,
		char *role_name,
		char *folder_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *execute_select_not_exists_role_violation(
		char *role_name,
		char *folder_name );

#endif
