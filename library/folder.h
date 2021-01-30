/* libary/folder.h							*/
/* -------------------------------------------------------------------- */
/* This is the appaserver FOLDER ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FOLDER_H
#define FOLDER_H

#include "list.h"
#include "role.h"
#include "process.h"
#include "appaserver_library.h"
#include "attribute.h"
#include "dictionary.h"
#include "boolean.h"
#include "appaserver_library.h"

/* Constants */
/* --------- */
#define FOLDER_SELECT_COLUMNS		"folder,"			\
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

#define FOLDER_TABLE_NAME				"folder"
#define FOLDER_MAXROWS					10000
#define FOLDER_FOLDER_NAME_PIECE			0
#define FOLDER_INSERT_ROWS_NUMBER_PIECE			1
#define FOLDER_LOOKUP_EMAIL_OUTPUT_PIECE		2
#define FOLDER_POPULATE_DROP_DOWN_PROCESS_PIECE		3
#define FOLDER_POST_CHANGE_PROCESS_PIECE		4
#define FOLDER_FORM_PIECE				5
#define FOLDER_NOTEPAD_PIECE				6
#define FOLDER_HTML_HELP_FILE_ANCHOR_PIECE		7
#define FOLDER_POST_CHANGE_JAVASCRIPT_PIECE		8

/* row_access_count has been retired */
/* --------------------------------- */
#define FOLDER_ROW_ACCESS_COUNT_PIECE			9

#define FOLDER_LOOKUP_BEFORE_DROP_DOWN_PIECE		10
#define FOLDER_NO_INITIAL_CAPITAL_PIECE			11
#define FOLDER_SUBSCHEMA_PIECE				12
#define FOLDER_DATA_DIRECTORY_PIECE			13
#define FOLDER_INDEX_DIRECTORY_PIECE			14
#define FOLDER_CREATE_VIEW_PIECE			15

#define FOLDER_MIN_ROWS_SORT_BUTTONS			2

/* Data structures */
/* --------------- */
typedef struct
{
	char *application_name;
	char *session;
	char *folder_name;
	LIST *attribute_list;
	LIST *attribute_float_list;
	LIST *non_primary_attribute_list;
	char *folder_form;
	int insert_rows_number;
	PROCESS *populate_drop_down_process;
	PROCESS *post_change_process;
	boolean lookup_email_output;
	boolean exclude_application_export;
	boolean lookup_transmit_output;
	boolean row_level_non_owner_forbid;
	boolean row_level_non_owner_view_only;
	long int row_count;
	boolean insert_permission;
	boolean update_permission;
	boolean lookup_permission;
	char *notepad;
	char *html_help_file_anchor;
	char *post_change_javascript;
	boolean lookup_before_drop_down;
	boolean no_initial_capital;
	LIST *one2m_related_folder_list;
	LIST *one2m_isa_related_folder_list;
	LIST *one2m_recursive_related_folder_list;
	LIST *pair_one2m_related_folder_list;
	LIST *mto1_related_folder_list;
	LIST *mto1_append_isa_related_folder_list;
	LIST *mto1_recursive_related_folder_list;
	LIST *mto1_isa_recursive_relation_list;
	LIST *mto1_lookup_before_drop_down_related_folder_list;
	LIST *primary_attribute_name_list;
	LIST *primary_data_list;
	DICTIONARY *primary_data_dictionary;
	char *subschema_name;
	char *data_directory;
	char *index_directory;
	LIST *attribute_name_list;
	LIST *join_1tom_related_folder_list;
	char *create_view_statement;
	LIST *mto1_isa_recursive_related_folder_list;
	LIST *mto1_isa_related_folder_list;
	LIST *append_isa_attribute_list;
	LIST *append_isa_attribute_name_list;
	boolean appaserver;
	LIST *one2m_relation_list;
	LIST *one2m_recursive_relation_list;
	LIST *mto1_relation_list;
	LIST *mto1_isa_relation_list;
} FOLDER;

/* Operations */
/* ---------- */
FOLDER *folder_calloc(			void );

FOLDER *folder_with_load_new(		char *application_name,
					char *session,
					char *folder_name,
					ROLE *role );

char *folder_get_folder_row_level_restrictions_record(
					char *application_name,
					char *folder_name );

void folder_load_row_level_restrictions(
					boolean *row_level_non_owner_forbid,
					boolean *row_level_non_owner_view_only,
					char *application_name,
					char *folder_name,
					LIST *mto1_related_folder_list );

FOLDER *folder_calloc( 			void );

FOLDER *folder_new( 			char *application_name,
					char *session,
					char *folder_name );

FOLDER *folder_new_folder( 		char *application_name,
					char *session,
					char *folder_name );

FOLDER *folder_folder_new( 		char *application_name,
					char *session,
					char *folder_name );

LIST *folder_get_attribute_name_list( 
				LIST *attribute_list );

LIST *folder_attribute_name_list( 
				LIST *attribute_list );

LIST *folder_get_primary_attribute_name_list(
				LIST *attribute_list );

LIST *folder_primary_attribute_name_list(
			LIST *attribute_list );

void folder_set_list_delimiter(
			FOLDER *folder,
			char delimiter );

LIST *folder_get_process_primary_data_list(
			char *application_name,
			char *folder_name,
			char *login_name,
			DICTIONARY *preprompt_dictionary,
			char delimiter,
			PROCESS *populate_drop_down_process,
			LIST *attribute_list,
			LIST *common_non_primary_attribute_name_list,
			boolean row_level_non_owner_forbid,
			LIST *exclude_attribute_name_list,
			char *role_name,
			char *state,
			char *one2m_folder_name_for_processes,
			char *process_name,
			char *prompt );

LIST *folder_get_primary_data_list(
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			DICTIONARY *parameter_dictionary,
			DICTIONARY *where_clause_dictionary,
			char delimiter,
			PROCESS *populate_drop_down_process,
			LIST *attribute_list,
			LIST *common_non_primary_attribute_name_list,
			boolean filter_only_login_name,
			LIST *exclude_attribute_name_list,
			char *role_name,
			char *state,
			char *one2m_folder_name_for_processes,
			char *appaserver_user_foreign_login_name,
			boolean include_root_folder );

LIST *folder_primary_data_list(
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			DICTIONARY *parameter_dictionary,
			DICTIONARY *where_clause_dictionary,
			char delimiter,
			PROCESS *populate_drop_down_process,
			LIST *attribute_list,
			LIST *common_non_primary_attribute_name_list,
			boolean filter_only_login_name,
			LIST *exclude_attribute_name_list,
			char *role_name,
			char *state,
			char *one2m_folder_name_for_processes,
			char *appaserver_user_foreign_login_name,
			boolean include_root_folder );

boolean folder_load(	int *insert_rows_number,
			boolean *lookup_email_output,
			boolean *row_level_non_owner_forbid,
			boolean *row_level_non_owner_view_only,
			PROCESS **populate_drop_down_process,
			PROCESS **post_change_process,
			char **folder_form,
			char **notepad,
			char **html_help_file_anchor,
			char **post_change_javascript,
			boolean *lookup_before_drop_down,
			char **data_directory,
			char **index_directory,
			boolean *no_initial_capital,
			char **subschema_name,
			char **create_view_statement,
			char *application_name,
			char *session,
			char *folder_name,
			boolean override_row_restrictions,
			char *role_name,
			LIST *mto1_related_folder_list );

int folder_get_insert_rows_number(	char *session,
					char *entity,
					FOLDER *folder );

LIST *folder_append_isa_attribute_name_list(
				LIST *append_isa_attribute_list );

LIST *folder_get_attribute_list(	char *application_name,
					char *folder_name );

LIST *folder_get_non_primary_attribute_name_list(
					LIST *attribute_list );

LIST *folder_drop_down_process_list(
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			PROCESS *populate_drop_down_process,
			char *role_name,
			DICTIONARY *parameter_dictionary,
			DICTIONARY *where_clause_dictionary,
			char *state,
			char *one2m_folder_name_for_processes,
			LIST *attribute_list,
			char piece_multi_attribute_data_label_delimiter,
			char *process_name,
			char *prompt );

char *folder_get_populate_drop_down_process( 
					FOLDER *folder );

DICTIONARY *folder_get_primary_data_dictionary(
					char *application,
					char *folder_name,
					LIST *primary_attribute_name_list );

FOLDER *folder_seek_folder(		LIST *folder_list,
					char *folder_name );

boolean folder_exists_folder(		char *application_name,
					char *folder_name );

boolean folder_exists_attribute(	char *application_name,
					char *folder_name,
					char *attribute_name );

LIST *folder_data_list(
			char *application_name,
			char *folder_name,
			LIST *attribute_name_list,
			char *where_clause,
			char delimiter,
			LIST *common_non_primary_attribute_name_list,
			char *login_name,
			LIST *date_position_list,
			int maxrows );

LIST *folder_get_non_primary_attribute_list(
					LIST *attribute_list );

char *folder_get_folder_record(
					char *application_name,
					char *folder_name );

LIST *folder_get_primary_attribute_list(LIST *attribute_list );

LIST *folder_get_folder_name_list(	char *application_name );

LIST *folder_get_folder_list(
				char *application_name,
				boolean override_row_restrictions,
				char *role_name );

boolean folder_get_pair_one2m_related_folder_boolean(
				char *folder_name,
				LIST *one2m_related_folder_list );

LIST *folder_get_insert_folder_name_list(
				char *application_name,
				char *login_name,
				char *role_name );

LIST *folder_get_zealot_folder_name_list(
				char *application_name );

LIST *folder_get_select_folder_name_list(
				char *application_name,
				char *login_name,
				char *role_name );

LIST *folder_get_single_primary_key_folder_name_list(
				char *application_name,
				char *login_name,
				char *role_name );

LIST *folder_get_primary_text_element_list(
				char *application_name,
				char *session,
				char *role_name,
				FOLDER *folder );

FOLDER *folder_get_unfulfilled_lookup_before_drop_down_folder(
				LIST *mto1_related_folder_list,
				DICTIONARY *post_dictionary );

char *folder_get_unfulfilled_dictionary_key(
				char *folder_name );

void folder_convert_date_attributes_to_database_format(
				DICTIONARY *dictionary,
				char *application_name,
				LIST *attribute_list );

LIST *folder_append_isa_mto1_related_folder_list(
				char *application_name,
				char *session,
				char *role_name,
				boolean override_row_restrictions,
				LIST *folder_mto1_isa_related_folder_list );

void folder_append_one2m_related_folder_list(
				LIST *mto1_related_folder_list,
				char *application_name );

void folder_append_mto1_related_folder_list(
				LIST *mto1_related_folder_list,
				char *application_name );

char *folder_get_foreign_join_where_clause(
			LIST *primary_attribute_name_list,
			char *one_folder_name,
			char *many_folder_name );

LIST *folder_get_role_folder_name_list(
			char *application_name,
			char *role_name );

char *folder_display(		FOLDER *folder );

LIST *folder_get_process_dictionary_list(
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			PROCESS *populate_drop_down_process,
			char *role_name,
			DICTIONARY *parameter_dictionary,
			char *state,
			char *one2m_folder_name_for_processes,
			LIST *attribute_list,
			char piece_multi_attribute_data_label_delimiter,
			char *process_name,
			char *prompt,
			LIST *primary_attribute_name_list );

LIST *folder_get_table_name_list(
				char *application_name );

boolean folder_table_exists(	char *table_name );

LIST *folder_fetch_table_name_list(
				void );

LIST *folder_append_isa_attribute_list(
				char *application_name,
				char *folder_name,
				LIST *folder_mto1_isa_related_folder_list,
				char *role_name );

LIST *folder_mto1_isa_related_folder_list(
			LIST *existing_related_folder_list,
			char *application_name,
			char *folder_name,
			char *role_name,
			int recursive_level );

LIST *folder_attribute_list(
			char *application_name,
			char *folder_name,
			char *role_name );

LIST *folder_primary_data_process_list(
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			DICTIONARY *parameter_dictionary,
			DICTIONARY *where_clause_dictionary,
			PROCESS *populate_drop_down_process,
			LIST *attribute_list,
			char *role_name,
			char *state,
			char *one2m_folder_name_for_processes );

LIST *folder_primary_data_table_list(
			char *application_name,
			char *folder_name,
			char *login_name,
			DICTIONARY *where_clause_dictionary,
			char delimiter,
			LIST *attribute_list,
			LIST *common_non_primary_attribute_name_list,
			LIST *exclude_attribute_name_list,
			char *role_name,
			boolean include_root_folder );

LIST *folder_related_primary_data_list(
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			DICTIONARY *parameter_dictionary,
			DICTIONARY *where_clause_dictionary,
			char delimiter,
			PROCESS *populate_drop_down_process,
			LIST *attribute_list,
			LIST *common_non_primary_attribute_name_list,
			boolean filter_only_login_name,
			char *role_name,
			char *state,
			char *one2m_folder_name_for_processes,
			char *appaserver_user_foreign_login_name );

LIST *folder_related_primary_data_table_list(
			char *application_name,
			char *folder_name,
			char *login_name,
			DICTIONARY *where_clause_dictionary,
			char delimiter,
			LIST *attribute_list,
			LIST *common_non_primary_attribute_name_list,
			LIST *exclude_attribute_name_list,
			char *role_name );

LIST *folder_get_primary_data_list(
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			DICTIONARY *parameter_dictionary,
			DICTIONARY *where_clause_dictionary,
			char delimiter,
			PROCESS *populate_drop_down_process,
			LIST *attribute_list,
			LIST *common_non_primary_attribute_name_list,
			boolean filter_only_login_name,
			LIST *exclude_attribute_name_list,
			char *role_name,
			char *state,
			char *one2m_folder_name_for_processes,
			char *appaserver_user_foreign_login_name,
			boolean include_root_folder );

LIST *folder_prompt_primary_data_list(
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			DICTIONARY *preprompt_dictionary,
			char delimiter,
			PROCESS *populate_drop_down_process,
			LIST *attribute_list,
			LIST *common_non_primary_attribute_name_list,
			boolean filter_only_login_name,
			char *role_name,
			char *state,
			char *one2m_folder_name_for_processes,
			char *appaserver_user_foreign_login_name );

LIST *folder_prompt_primary_data_table_list(
			char *application_name,
			char *folder_name,
			char *login_name,
			DICTIONARY *preprompt_dictionary,
			char delimiter,
			LIST *attribute_list,
			LIST *common_non_primary_attribute_name_list,
			LIST *exclude_attribute_name_list,
			char *role_name );

LIST *folder_lookup_update_folder_name_list(
			char *application_name,
			char *role_name );

LIST *folder_prompt_primary_data_process_list(
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			DICTIONARY *preprompt_dictionary,
			PROCESS *populate_drop_down_process,
			LIST *attribute_list,
			char *role_name,
			char *state,
			char *one2m_folder_name_for_processes );

LIST *folder_prompt_drop_down_process_list(
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			PROCESS *populate_drop_down_process,
			char *role_name,
			DICTIONARY *preprompt_dictionary,
			char *state,
			char *one2m_folder_name_for_processes,
			LIST *attribute_list,
			char piece_multi_attribute_data_label_delimiter );

LIST *folder_edit_table_drop_down_data_list(
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			DICTIONARY *parameter_dictionary,
			DICTIONARY *where_clause_dictionary,
			char delimiter,
			PROCESS *populate_drop_down_process,
			LIST *attribute_list,
			LIST *common_non_primary_attribute_name_list,
			LIST *exclude_attribute_name_list,
			char *role_name,
			char *state,
			char *one2m_folder_name_for_processes,
			boolean include_root_folder );

LIST *folder_prompt_insert_primary_data_list(
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			DICTIONARY *preprompt_dictionary,
			char delimiter,
			PROCESS *populate_drop_down_process,
			LIST *attribute_list,
			LIST *common_non_primary_attribute_name_list,
			char *role_name,
			char *state,
			char *one2m_folder_name_for_processes );

LIST *folder_prompt_insert_primary_data_table_list(
			char *application_name,
			char *folder_name,
			char *login_name,
			DICTIONARY *preprompt_dictionary,
			char delimiter,
			LIST *attribute_list,
			LIST *common_non_primary_attribute_name_list,
			LIST *exclude_attribute_name_list,
			char *role_name );

/* Returns static memory */
/* --------------------- */
char *folder_primary_where(
			char *folder_name );

FOLDER *folder_fetch(	char *folder_name,
			boolean fetch_attribute_list,
			boolean fetch_one2m_recursive_relation_list,
			boolean fetch_mto1_isa_recursive_relation_list,
			boolean fetch_mto1_relation_list );

FOLDER *folder_parse(	char *input,
			boolean fetch_attribute_list,
			boolean fetch_one2m_recursive_relation_list,
			boolean fetch_mto1_isa_recursive_relation_list,
			boolean fetch_mto1_relation_list );

char *folder_sys_string(
			char *where );

#endif
