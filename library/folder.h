/* $APPASERVER_HOME/libary/folder.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FOLDER_H
#define FOLDER_H

#include "list.h"
#include "boolean.h"
#include "process.h"
#include "dictionary.h"
#include "security.h"

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
				"drilldown_yn,"			\
				"no_initial_capital_yn,"	\
				"index_directory,"		\
				"data_directory,"		\
				"create_view_statement,"	\
				"javascript_filename"

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
	char *folder_name;
	char *role_name;
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
	boolean drilldown;
	boolean no_initial_capital;
	char *index_directory;
	char *data_directory;
	char *create_view_statement;
	char *javascript_filename;

	/* Process */
	/* ------- */
	PROCESS *populate_drop_down_process;
	PROCESS *post_change_process;
	LIST *role_exclude_attribute_name_list;
	LIST *folder_attribute_list;
	LIST *folder_attribute_primary_list;
	LIST *primary_key_list;
	LIST *folder_attribute_append_isa_list;
	LIST *role_folder_list;
	long int folder_row_count;
	char *row_level_restriction_string;
	boolean non_owner_view_only;
	boolean non_owner_forbid;
	char *folder_table_name;
	LIST *relation_mto1_non_isa_list;
	LIST *relation_mto1_isa_list;
	LIST *relation_one2m_list;
	LIST *relation_one2m_recursive_list;
	LIST *relation_pair_one2m_list;
	LIST *relation_join_one2m_list;
	LIST *role_operation_list;
	LIST *delimited_list;
	LIST *dictionary_list;
} FOLDER;

/* FOLDER operations */
/* ----------------- */
FOLDER *folder_new(	char *folder_name );

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
			/* -------------------------- */
			/* Also sets primary_key_list */
			/* -------------------------- */
			boolean fetch_folder_attribute_list,
			boolean fetch_relation_mto1_non_isa_list,
			/* ------------------------------------------- */
			/* Maybe sets folder_attribute_append_isa_list */
			/* ------------------------------------------- */
			boolean fetch_relation_mto1_isa_list,
			boolean fetch_relation_one2m_list,
			boolean fetch_relation_one2m_recursive_list,
			boolean fetch_process,
			boolean fetch_role_folder_list,
			boolean fetch_row_level_restriction,
			boolean fetch_role_operation_list );

FOLDER *folder_parse(	char *input,
			char *sql_injection_escape_role_name,
			LIST *exclude_attribute_name_list,
			/* -------------------------- */
			/* Also sets primary_key_list */
			/* -------------------------- */
			boolean fetch_folder_attribute_list,
			boolean fetch_relation_mto1_non_isa_list,
			/* ------------------------------------------- */
			/* Maybe sets folder_attribute_append_isa_list */
			/* ------------------------------------------- */
			boolean fetch_relation_mto1_isa_list,
			boolean fetch_relation_one2m_list,
			boolean fetch_relation_one2m_recursive_list,
			boolean fetch_process,
			boolean fetch_role_folder_list,
			boolean fetch_row_level_restriction,
			boolean fetch_role_operation_list );

long int folder_row_count(
			char *folder_table_name );

/* Returns heap memory */
/* ------------------- */
char *folder_table_name(
			char *application_name,
			char *folder_name );

FOLDER *folder_fetch(	char *sql_injection_escape_folder_name,
			/* ---------------------------- */
			/* If fetching role_folder_list */
			/* ---------------------------- */
			char *sql_injection_escape_role_name,
			LIST *exclude_attribute_name_list,
			/* -------------------------- */
			/* Also sets primary_key_list */
			/* -------------------------- */
			boolean fetch_folder_attribute_list,
			boolean fetch_relation_mto1_non_isa_list,
			/* ------------------------------------------- */
			/* Maybe sets folder_attribute_append_isa_list */
			/* ------------------------------------------- */
			boolean fetch_relation_mto1_isa_list,
			boolean fetch_relation_one2m_list,
			boolean fetch_relation_one2m_recursive_list,
			boolean fetch_process,
			boolean fetch_role_folder_list,
			boolean fetch_row_level_restriction,
			boolean fetch_role_operation_list );

FOLDER *folder_quick_fetch(
			char *folder_name );

char *folder_row_level_restriction_string(
			char *folder_name );

LIST *folder_process_primary_delimited_list(
			PROCESS *populate_drop_down_process,
			DICTIONARY *preprompt_dictionary,
			char *login_name );

LIST *folder_query_primary_delimited_list(
			char *table_name,
			LIST *folder_attribute_list,
			LIST *primary_key_list,
			DICTIONARY *preprompt_dictionary,
			char *login_name );

LIST *folder_query_delimited_list(
			char *table_name,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_isa_list,
			DICTIONARY *query_dictionary );

char *folder_delimited(
			char *table_name,
			LIST *primary_key_list,
			char *primary_where_clause );

LIST *folder_delimited_list(
			char *table_name,
			LIST *attribute_name_list,
			char *where_clause );

boolean folder_non_owner_view_only(
			char *row_level_restriction_string );

boolean folder_non_owner_forbid(
			char *row_level_restriction_string );

LIST *folder_delimited_fetch(
			char *table_name,
			LIST *attribute_name_list,
			char *where );

LIST *folder_primary_delimited_list(
			char *folder_name,
			LIST *primary_key_list,
			LIST *folder_attribute_list,
			SECURITY_ENTITY *security_entity,
			DICTIONARY *drillthru_dictionary,
			char *login_name );

#endif
