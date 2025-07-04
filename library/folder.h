/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/libary/folder.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FOLDER_H
#define FOLDER_H

#include "list.h"
#include "boolean.h"
#include "process.h"

#define FOLDER_OMIT_ISOLATE_ROLE 1

#define FOLDER_SELECT		"table_name,"			\
				"appaserver_form,"		\
				"insert_rows_number,"		\
				"notepad,"			\
				"populate_drop_down_process,"	\
				"post_change_process,"		\
				"html_help_file_anchor,"	\
				"post_change_javascript,"	\
				"subschema,"			\
				"storage_engine,"		\
				"exclude_application_export_yn,"\
				"drillthru_yn,"			\
				"no_initial_capital_yn,"	\
				"index_directory,"		\
				"data_directory,"		\
				"create_view_statement,"	\
				"javascript_filename"

#define FOLDER_TABLE		"appaserver_table"
#define FOLDER_PRIMARY_KEY	"table_name"

typedef struct
{
	char *folder_name;
	char *role_name;
	char *appaserver_form;
	int insert_rows_number;
	char *notepad;
	char *populate_drop_down_process_name;
	char *post_change_process_name;
	char *html_help_file_anchor;
	char *post_change_javascript;
	char *subschema;
	char *storage_engine;
	boolean exclude_application_export;
	boolean drillthru;
	boolean no_initial_capital;
	char *index_directory;
	char *data_directory;
	char *create_view_statement;
	char *javascript_filename;
	LIST *folder_attribute_list;
	LIST *folder_attribute_primary_list;
	LIST *folder_attribute_primary_key_list;
	LIST *folder_attribute_name_list;
	/* -------------- */
	/* Set externally */
	/* -------------- */
	PROCESS *post_change_process;
} FOLDER;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FOLDER *folder_fetch(
		char *folder_name,
		LIST *role_attribute_exclude_name_list,
		/* -------------------------------------------- */
		/* Sets folder_attribute_primary_list		*/
		/* Sets folder_attribute_primary_key_list	*/
		/* Sets folder_attribute_name_list		*/
		/* -------------------------------------------- */
		boolean fetch_folder_attribute_list,
		boolean fetch_attribute,
		boolean cache_boolean );

/* Usage */
/* ----- */
FOLDER *folder_cache_fetch(
		char *folder_name,
		LIST *role_attribute_exclude_lookup_name_list,
		boolean fetch_folder_attribute_list,
		boolean fetch_attribute );

/* Process */
/* ------- */
FOLDER *folder_seek(
		char *folder_name,
		LIST *folder_list );

/* Usage */
/* ----- */
FOLDER *folder_where_fetch(
		char *folder_name,
		LIST *role_attribute_exclude_lookup_name_list,
		boolean fetch_folder_attribute_list,
		boolean fetch_attribute );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *folder_primary_where_string(
		const char *folder_primary_key,
		char *folder_name );

/* Usage */
/* ----- */
FOLDER *folder_parse(
		LIST *exclude_attribute_name_list,
		boolean fetch_folder_attribute_list,
		boolean fetch_attribute,
		boolean cache_boolean,
		char *input );

/* Usage */
/* ----- */
FOLDER *folder_new(
		char *folder_name );

/* Process */
/* ------- */
FOLDER *folder_calloc(
		void );

/* Usage */
/* ----- */
LIST *folder_list(
		LIST *exclude_attribute_name_list,
		boolean fetch_folder_attribute_list,
		boolean fetch_attribute );

/* Usage */
/* ----- */
LIST *folder_table_name_list(
		char *role_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *folder_role_where_string(
		const char *role_folder_table,
		const char *folder_primary_key,
		char *role_name_list_string );

/* Usage */
/* ----- */
unsigned long folder_row_count(
		char *folder_table_name );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *folder_appaserver_form(
		char *folder_name );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *folder_notepad(
		char *folder_name );

/* Usage */
/* ----- */
boolean folder_no_initial_capital(
		char *folder_name );

/* Usage */
/* ----- */
LIST *folder_name_list(
		LIST *folder_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *folder_table_name(
		char *application_name,
		char *folder_name );

/* Usage */
/* ----- */
LIST *folder_system_name_list(
		const char *role_system );

/* Usage */
/* ----- */
LIST *folder_fetch_name_list(
		const char *folder_primary_key,
		const char *folder_table );

/* Usage */
/* ----- */
boolean folder_form_prompt_boolean(
		const char *form_appaserver_prompt,
		char *appaserver_form );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *folder_column_fetch(
		const char *column_name,
		char *folder_name );

/* Usage */
/* ----- */
boolean folder_column_boolean(
		char *table_name,
		char *column_name );

#endif
