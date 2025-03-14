/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/export_table.h	 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef EXPORT_TABLE_H
#define EXPORT_TABLE_H

#include "list.h"
#include "boolean.h"
#include "appaserver_link.h"

typedef struct
{
	char *application_table_name;
	LIST *folder_attribute_name_list;
	char *appaserver_select;
	char *appaserver_system_string;
	char *date_now_yyyy_mm_dd;
	char *shell_filespecification;
	char *execute_bit_system_string;
	char *appaserver_spreadsheet_heading_string;
	char *html_title;
	char *html_table_system_string;
	char *insert_folder_statement_system_string;
	APPASERVER_LINK *appaserver_link;
	char *appaserver_link_anchor_html;
	char *spreadsheet_filespecification;
	char *appaserver_spreadsheet_output_system_string;
} EXPORT_TABLE;

/* Usage */
/* ----- */
EXPORT_TABLE *export_table_new(
		char *application_name,
		char *folder_name,
		char *data_directory,
		char *where );

/* Process */
/* ------- */
EXPORT_TABLE *export_table_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *export_table_shell_filespecification(
		char *aplication_name,
		char *folder_name,
		char *data_directory,
		char *date_now_yyyy_mm_dd );

/* Returns heap memory */
/* ------------------- */
char *export_table_execute_bit_system_string(
		char *export_table_shell_filespecification );

/* Returns parameter */
/* ----------------- */
char *export_table_spreadsheet_filespecification(
		char *appaserver_link_output_filename );

/* Returns heap memory */
/* ------------------- */
char *export_table_html_title(
		char *folder_name );

/* Usage */
/* ----- */
LIST *export_table_spreadsheet_filename_list(
		LIST *export_table_list );

/* Usage */
/* ----- */
LIST *export_table_shell_filename_list(
		LIST *export_table_list );

/* Driver */
/* ------ */
void export_table_shell_output(
		char *application_name,
		char *application_table_name,
		char *appaserver_select,
		char *appaserver_system_string,
		char *shell_filespecification,
		char *execute_bit_system_string,
		boolean output_prompt_boolean );

/* Driver */
/* ------ */
void export_table_spreadsheet_output(
		char *appaserver_system_string,
		char *spreadsheet_filespecification,
		char *appaserver_spreadsheet_heading_string,
		char *appaserver_spreadsheet_output_system_string,
		char *appaserver_link_anchor_html );

/* Driver */
/* ------ */
void export_table_html_output(
		char *appaserver_system_string,
		char *html_table_system_string );

/* Driver */
/* ------ */
void export_table_insert_statement_output(
		char *appaserver_system_string,
		char *insert_folder_statement_system_string,
		char *output_pipe_string );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *export_table_output_system_string(
		char *insert_folder_statement_system_string,
		char *output_pipe_string );

#endif
