/* $APPASERVER_HOME/library/row_security.h		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef ROW_SECURITY_H
#define ROW_SECURITY_H

#include "list.h"
#include "folder.h"
#include "attribute.h"
#include "element.h"
#include "related_folder.h"
#include "role.h"
#include "operation_list.h"
#include "lookup_before_drop_down.h"

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *one_folder_name;
	char *attribute_not_null;

	/* Process */
	/* ------- */
	LIST *relation_one2m_recursive_list;
	boolean participating;
	char *join;
} ROW_SECURITY_ROLE_UPDATE_FOLDER;

/* ROW_SECURITY_ROLE_UPDATE_FOLDER operations */
/* ------------------------------------------ */
ROW_SECURITY_ROLE_UPDATE_FOLDER *row_security_role_update_folder_calloc(
			void );

ROW_SECURITY_ROLE_UPDATE_FOLDER *row_security_role_update_folder_fetch(
			char *folder_name,
			LIST *primary_key_list );

ROW_SECURITY_ROLE_UPDATE_FOLDER *row_security_role_update_folder_parse(
			char *input );

boolean row_security_role_update_folder_participating(
			char *folder_name,
			char *one_folder_name,
			LIST *relation_one2m_recursive_list );

/* Returns heap memory */
/* ------------------- */
char *row_security_role_update_folder_join(
			char *folder_name,
			char *one_folder_name,
			LIST *primary_key_list );

typedef struct
{
	/* Input */
	/* ----- */
	char *folder_name;
	LIST *primary_key_list;
	boolean role_override_restrictions;

	/* Process */
	/* ------- */
	ROW_SECURITY_ROLE_UPDATE_FOLDER *row_security_role_update_folder;
	boolean viewonly;
} ROW_SECURITY_ROLE_UPDATE;

/* ROW_SECURITY_ROLE_UPDATE operations */
/* ----------------------------------- */
ROW_SECURITY_ROLE_UPDATE *row_security_role_update_calloc(
			void );

ROW_SECURITY_ROLE_UPDATE *row_security_role_update_new(
			char *folder_name,
			LIST *primary_key_list,
			boolean role_override_row_restrictions );

boolean row_security_role_update_viewonly(
			boolean role_override_restrictions );

/* Returns static memory */
/* --------------------- */
char *row_security_role_update_system_string(
			char *folder_name );

typedef struct
{
	FOLDER *folder;
	ROLE *role;
	char *login_name;
	LIST *role_update_list;
	char *state;
	DICTIONARY *preprompt_dictionary;
	DICTIONARY *query_dictionary;
	DICTIONARY *sort_dictionary;
	LIST *ignore_attribute_name_list;
	FOLDER *foreign_login_name_folder;
	char *attribute_not_null_join;
	FOLDER *attribute_not_null_folder;
	char *attribute_not_null_string;
	boolean row_security_is_participating;
	LIST *regular_element_list;
	LIST *viewonly_element_list;
	LIST *apply_element_list;
	char query_folder_name[ 256 ];
	char *isa_where_join;
	boolean add_attribute_not_null_to_list;
	LIST *row_dictionary_list;
	RELATION *relation_ajax_fill_drop_down;
} ROW_SECURITY;

/* ROW_SECURITY operations */
/* ----------------------- */
ROW_SECURITY *row_security_calloc(
			void );

ROW_SECURITY *row_security_folder_new(
			char *application_name,
			FOLDER *folder,
			ROLE *role,
			char *login_name,
			char *state,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			LIST *ignore_attribute_name_list );

ROW_SECURITY_ELEMENT_LIST_STRUCTURE *
	row_security_detail_structure_new(
			char *application_name,
			enum row_security_state row_security_state,
			LIST *primary_attribute_data_list,
			FOLDER *folder,
			LIST *ignore_attribute_name_list,
			char *role_name,
			char *login_name,
			char *state,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			char *attribute_not_null_join,
			char *attribute_not_null_folder_name,
			FOLDER *foreign_login_name_folder,
			boolean make_primary_keys_non_edit,
			enum omit_delete_operation 
				regular_omit_delete_operation,
			enum omit_delete_operation 
				viewonly_omit_delete_operation,
			boolean omit_operation_buttons,
			boolean row_security_is_participating );

LIST *row_security_role_update_list(
			char *application_name );

boolean row_security_participating_related_folder(
			FOLDER *row_update_folder,
			FOLDER *participating_folder );

ATTRIBUTE *row_security_get_root_attribute_not_null(
			FOLDER *row_update_folder,
			char *attribute_not_null_string );

void row_security_set_additional_select_attribute(
			LIST *attribute_list,
			ATTRIBUTE *attribute_not_null );

LIST *row_security_detail_dictionary_list(
			char *application_name,
			DICTIONARY *sort_dictionary,
			FOLDER *folder,
			ROLE *role,
			char *folder_name,
			LIST *where_clause_attribute_name_list,
			LIST *where_clause_data_list,
			char *attribute_not_null_join,
			char *attribute_not_null_folder_name,
			LIST *join_1tom_related_folder_list );

void row_security_append_join_1tom_related_folder_list(
			LIST *row_dictionary_list,
			LIST *join_1tom_related_folder_list,
			char *application_name,
			LIST *primary_key_list );

void row_security_set_dictionary_related_folder(
			DICTIONARY *row_dictionary,
			RELATED_FOLDER *related_folder,
			char *application_name,
			LIST *primary_key_list );

enum row_security_state row_security_get_row_security_state(
			char **attribute_not_null_join,
			FOLDER **attribute_not_null_folder,
			char **attribute_not_null_string,
			LIST *role_update_list,
			char *folder_name,
			boolean override_role_restrictions,
			char *folder_state );

boolean row_security_supervisor_logged_in(
			enum row_security_state row_security_state );

char *row_security_role_update_list_display(
			LIST *role_update_list );

LIST *row_security_edit_table_dictionary_list(
			DICTIONARY *query_dictionary,
			char *application_name,
			DICTIONARY *sort_dictionary,
			FOLDER *folder,
			ROLE *role,
			char *login_name,
			char *query_select_folder_list_string,
			char *attribute_not_null_join,
			char *attribute_not_null_folder_name );

ROW_SECURITY_ELEMENT_LIST_STRUCTURE *
	row_security_edit_table_structure_new(
			DICTIONARY *query_dictionary,
			char *application_name,
			enum row_security_state row_security_state,
			FOLDER *folder,
			ROLE *role,
			char *login_name,
			char *state,
			DICTIONARY *sort_dictionary,
			LIST *ignore_attribute_name_list,
			char *attribute_not_null_join,
			FOLDER *attribute_not_null_folder,
			FOLDER *foreign_login_name_folder,
			boolean make_primary_keys_non_edit,
			enum omit_delete_operation omit_delete_operation,
			boolean omit_operation_buttons,
			boolean ajax_fill_drop_down_omit,
			boolean row_security_is_participating );

LIST *row_security_edit_table_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char *application_name,
			FOLDER *folder,
			ROLE *role,
			char *login_name,
			LIST *ignore_attribute_name_list,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			int row_dictionary_list_length,
			char *state,
			LIST *non_edit_folder_name_list,
			char update_yn,
			enum omit_delete_operation omit_delete_operation,
			boolean omit_operation_buttons,
			boolean make_primary_keys_non_edit,
			boolean prompt_data_separate_folder );

LIST *row_security_edit_table_update_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			DICTIONARY *query_dictionary,
			char *application_name,
			char *session,
			FOLDER *folder,
			ROLE *role,
			char *login_name,
			LIST *include_attribute_name_list,
			DICTIONARY *preprompt_dictionary,
			LIST *operation_list_list,
			int row_dictionary_list_length,
			LIST *ignore_attribute_name_list,
			char update_yn,
			char *state,
			LIST *non_edit_folder_name_list,
			boolean override_row_restrictions,
			char *folder_post_change_javascript,
			int max_query_rows_for_drop_downs,
			char *one2m_folder_name_for_processes,
			boolean make_primary_keys_non_edit,
			boolean prompt_data_separate_folder );

LIST *row_security_regular_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			DICTIONARY *query_dictionary,
			char *application_name,
			FOLDER *folder,
			ROLE *role,
			char *login_name,
			LIST *ignore_attribute_name_list,
			DICTIONARY *preprompt_dictionary,
			int row_dictionary_list_length,
			char *state,
			enum omit_delete_operation omit_delete_operation,
			boolean omit_operation_buttons,
			boolean make_primary_keys_non_edit,
			boolean prompt_data_separate_folder );

LIST *row_security_regular_evaluate_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char *application_name,
			char *session,
			FOLDER *folder,
			ROLE *role,
			char *login_name,
			LIST *include_attribute_name_list,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			LIST *operation_list_list,
			int row_dictionary_list_length,
			LIST *ignore_attribute_name_list,
			char *state,
			boolean override_row_restrictions,
			char *folder_post_change_javascript,
			int max_query_rows_for_drop_downs,
			char *one2m_folder_name_for_processes,
			boolean make_primary_keys_non_edit,
			boolean prompt_data_separate_folder,
			boolean row_level_non_owner_forbid );

LIST *row_security_viewonly_element_list(
			char *application_name,
			FOLDER *folder,
			ROLE *role,
			LIST *ignore_attribute_name_list,
			enum omit_delete_operation omit_delete_operation,
			boolean omit_operation_buttons );

LIST *row_security_viewonly_evaluate_element_list(
			char *application_name,
			FOLDER *folder,
			LIST *include_attribute_name_list,
			LIST *operation_list_list,
			LIST *ignore_attribute_name_list,
			char *folder_post_change_javascript );

ROW_SECURITY_ELEMENT_LIST_STRUCTURE *
		row_security_sort_order_structure_new(
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			FOLDER *folder );

LIST *row_security_sort_order_dictionary_list(
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			FOLDER *folder );

LIST *row_security_sort_order_element_list(
			LIST *folder_attribute_list );

#endif
