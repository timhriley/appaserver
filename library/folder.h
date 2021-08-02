/* $APPASERVER_HOME/libary/folder.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FOLDER_H
#define FOLDER_H

#include "list.h"
#include "process.h"
#include "boolean.h"

/* Constants */
/* --------- */
#define FOLDER_SELECT		"folder,"			\
				"form,"				\
				"insert_rows_number,"		\
				"lookup_email_output_yn,"	\
				"notepad,"			\
				"populate_drop_down_process,"	\
				"post_change_process,"		\
				"html_help_file_anchor,"	\
				"post_change_javascript,"	\
				"subschema,"			\
				"exclude_application_export_yn,"\
				"lookup_before_drop_down_yn,"	\
				"no_initial_capital_yn,"	\
				"index_directory,"		\
				"data_directory,"		\
				"create_view_statement,"	\
				"appaserver_yn"

#define FOLDER_TABLE				"folder"
#define FOLDER_ROW_LEVEL_RESTRICTION_TABLE	"folder_row_level_restrictions"

#define FOLDER_MAXROWS				10000
#define FOLDER_MIN_ROWS_SORT_BUTTONS		2

/* Data structures */
/* --------------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *sql_injection_escape_folder_name;
	char *sql_injection_escape_role_name;
	char *folder_form;
	int insert_rows_number;
	boolean lookup_email_output;
	char *notepad;
	char *populate_drop_down_process_name;
	char *post_change_process_name;
	char *html_help_file_anchor;
	char *post_change_javascript;
	char *subschema;
	boolean exclude_application_export;
	boolean lookup_before_drop_down;
	boolean no_initial_capital;
	char *index_directory;
	char *data_directory;
	char *create_view_statement;
	boolean appaserver;

	/* Process */
	/* ------- */
	PROCESS *populate_drop_down_process;
	PROCESS *post_change_process;
	LIST *role_exclude_attribute_name_list;
	LIST *folder_attribute_list;
	LIST *primary_attribute_name_list;
	LIST *role_folder_list;
	long int folder_row_count;
	char *folder_row_level_restriction_string;
	char *folder_table_name;
	LIST *folder_attribute_append_isa_list;
	LIST *mto1_isa_relation_list;
	LIST *folder_primary_delimited_list;
	LIST *folder_delimited_list;
	LIST *folder_dictionary_list;
} FOLDER;

/* Operations */
/* ---------- */
FOLDER *folder_new( 	char *sql_injection_escape_folder_name );

/* Returns static memory */
/* --------------------- */
char *folder_primary_where(
			char *sql_injection_escape_folder_name );

/* Returns heap memory */
/* ------------------- */
char *folder_system_string(
			char *where );

LIST *folder_system_list(
			char *system_string,
			char *sql_injection_escape_role_name,
			LIST *role_exclude_attribute_name_lst,
			boolean fetch_folder_attribute_list,
			boolean fetch_process,
			boolean fetch_role_folder_list,
			boolean fetch_row_level_restriction );

FOLDER *folder_parse(	char *input,
			char *sql_injection_escape_role_name,
			LIST *role_exclude_attribute_name_list,
			boolean fetch_folder_attribute_list,
			boolean fetch_process,
			boolean fetch_role_folder_list,
			boolean fetch_row_level_restriction );

long int folder_row_count(
			char *folder_table_name );

/* Returns heap memory */
/* ------------------- */
char *folder_table_name(
			char *application_name,
			char *folder_name );

FOLDER *folder_fetch(	char *sql_injection_escape_folder_name,
			char *sql_injection_escape_role_name,
			LIST *role_exclude_attribute_name_list,
			boolean fetch_folder_attribute_list,
			boolean fetch_process,
			boolean fetch_role_folder_list,
			boolean fetch_row_level_restriction );

char *folder_row_level_restriction_string(
			char *folder_name );

LIST *folder_process_primary_delimited_list(
			PROCESS *populate_drop_down_process,
			DICTIONARY *preprompt_dictionary.
			char *login_name );

LIST *folder_query_primary_delimited_list(
			char *table_name,
			LIST *folder_attribute_list,
			LIST *primary_attribute_name_list,
			DICTIONARY *preprompt_dictionary.
			char *login_name );

LIST *folder_delimited_list(
			char *table_name,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_isa_list,
			DICTIONARY *query_dictionary );

LIST *folder_dictionary_list(
			char *table_name,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_isa_list,
			DICTIONARY *query_dictionary );

#endif
