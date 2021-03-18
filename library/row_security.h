/* library/row_security.h				   */
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

enum row_security_state {	security_supervisor,
				security_user,
				regular_supervisor,
				regular_user };

typedef struct
{
	FOLDER *folder;
	char *attribute_not_null_string;
} ROW_SECURITY_ROLE_UPDATE;

typedef struct
{
	LIST *regular_element_list;
	LIST *viewonly_element_list;
	LIST *apply_element_list;
	char query_folder_name[ 256 ];
	char *isa_where_join;
	boolean add_attribute_not_null_to_list;
	LIST *row_dictionary_list;
	RELATED_FOLDER *ajax_fill_drop_down_related_folder;
} ROW_SECURITY_ELEMENT_LIST_STRUCTURE;

typedef struct
{
	enum row_security_state row_security_state;
	FOLDER *select_folder;
	ROLE *login_role;
	LIST *role_update_list;
	char *login_name;
	char *state;
	DICTIONARY *preprompt_dictionary;
	DICTIONARY *query_dictionary;
	DICTIONARY *sort_dictionary;
	LIST *no_display_pressed_attribute_name_list;
	FOLDER *attribute_not_null_folder;
	FOLDER *foreign_login_name_folder;
	char *attribute_not_null_string;
	ROW_SECURITY_ELEMENT_LIST_STRUCTURE *
		row_security_element_list_structure;
} ROW_SECURITY;

ROW_SECURITY_ROLE_UPDATE *row_security_role_update_new(
				char *application_name,
				char *folder_name,
				char *attribute_not_null_string );

ROW_SECURITY *row_security_new(
			char *application_name,
			ROLE *login_role,
			char *select_folder_name,
			char *login_name,
			char *state,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			LIST *no_display_pressed_attribute_name_list );

ROW_SECURITY_ELEMENT_LIST_STRUCTURE *
	row_security_detail_element_list_structure(
			char *application_name,
			enum row_security_state row_security_state,
			char *login_name,
			char *state,
			ROLE *login_role,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			LIST *no_display_pressed_attribute_name_list,
			FOLDER *select_folder,
			FOLDER *attribute_not_null_folder,
			FOLDER *foreign_login_name_folder,
			LIST *where_clause_attribute_name_list,
			LIST *where_clause_data_list,
			LIST *non_edit_folder_name_list,
			boolean make_primary_keys_non_edit,
			enum omit_delete_operation omit_delete_operation,
			boolean omit_operation_buttons,
			char update_yn,
			boolean ajax_fill_drop_down_omit,
			LIST *append_isa_attribute_list );

LIST *row_security_get_role_update_list(
			char *application_name );

LIST *row_security_get_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char *application_name,
			FOLDER *select_folder,
			LIST *mto1_append_isa_related_folder_list,
			ROLE *login_role,
			LIST *no_display_pressed_attribute_name_list,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			int row_dictionary_list_length,
			char *state,
			LIST *non_edit_folder_name_list,
			char *login_name,
			char role_folder_update_yn,
			enum omit_delete_operation,
			boolean omit_operation_buttons,
			LIST *join_1tom_related_folder_list,
			boolean make_primary_keys_non_edit,
			boolean prompt_data_row_separate );

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
			LIST *append_isa_attribute_list,
			char *application_name,
			DICTIONARY *sort_dictionary,
			ROLE *login_role,
			char *folder_name,
			LIST *where_clause_attribute_name_list,
			LIST *where_clause_data_list,
			LIST *join_1tom_related_folder_list );

void row_security_append_join_1tom_related_folder_list(
			LIST *row_dictionary_list,
			LIST *join_1tom_related_folder_list,
			char *application_name,
			LIST *primary_attribute_name_list );

void row_security_set_dictionary_related_folder(
			DICTIONARY *row_dictionary,
			RELATED_FOLDER *related_folder,
			char *application_name,
			LIST *primary_attribute_name_list );

LIST *row_security_get_update_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char *login_name,
			char *application_name,
			char *session,
			char *folder_name,
			char *role_name,
			LIST *attribute_list,
			LIST *append_isa_attribute_list,
			LIST *include_attribute_name_list,
			LIST *mto1_append_isa_related_folder_list,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			LIST *operation_list_list,
			int row_dictionary_list_length,
			LIST *no_display_pressed_attribute_name_list,
			char update_yn,
			char *state,
			LIST *non_edit_folder_name_list,
			boolean override_row_restrictions,
			char *folder_post_change_javascript,
			int max_query_rows_for_drop_downs,
			char *one2m_folder_name_for_processes,
			LIST *join_1tom_related_folder_list,
			boolean make_primary_keys_non_edit,
			boolean prompt_data_separate_folder,
			boolean row_level_non_owner_forbid,
			char *folder_notepad );

enum row_security_state row_security_get_row_security_state(
			FOLDER **attribute_not_null_folder,
			char **attribute_not_null_string,
			LIST *role_update_list,
			char *select_folder_name,
			boolean override_role_restrictions );

boolean row_security_supervisor_logged_in(
			enum row_security_state row_security_state );

char *row_security_role_update_list_display(
			LIST *role_update_list );

ROW_SECURITY_ROLE_UPDATE *row_security_role_update_fetch(
			LIST *role_update_list,
			char *folder_name );

ROW_SECURITY_ELEMENT_LIST_STRUCTURE *row_security_element_list_structure_calloc(
			void );

LIST *row_security_edit_table_dictionary_list(
			char *application_name,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			ROLE *login_role,
			char *login_name,
			char *select_folder_name,
			LIST *join_1tom_related_folder_list );

ROW_SECURITY_ELEMENT_LIST_STRUCTURE *row_security_edit_table_structure_new(
			char *application_name,
			enum row_security_state row_security_state,
			char *login_name,
			char *state,
			ROLE *login_role,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			LIST *no_display_pressed_attribute_name_list,
			FOLDER *select_folder,
			FOLDER *attribute_not_null_folder,
			FOLDER *foreign_login_name_folder,
			LIST *non_edit_folder_name_list,
			boolean make_primary_keys_non_edit,
			enum omit_delete_operation omit_delete_operation,
			boolean omit_operation_buttons,
			char update_yn,
			boolean ajax_fill_drop_down_omit,
			LIST *append_isa_attribute_list );

LIST *row_security_edit_table_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char *application_name,
			FOLDER *select_folder,
			LIST *mto1_append_isa_related_folder_list,
			ROLE *login_role,
			LIST *no_display_pressed_attribute_name_list,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			int row_dictionary_list_length,
			char *state,
			LIST *non_edit_folder_name_list,
			char *login_name,
			char update_yn,
			enum omit_delete_operation omit_delete_operation,
			boolean omit_operation_buttons,
			LIST *join_1tom_related_folder_list,
			boolean make_primary_keys_non_edit,
			boolean prompt_data_separate_folder );

LIST *row_security_edit_table_update_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char *login_name,
			char *application_name,
			char *session,
			char *folder_name,
			char *role_name,
			LIST *attribute_list,
			LIST *append_isa_attribute_list,
			LIST *include_attribute_name_list,
			LIST *mto1_append_isa_related_folder_list,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			LIST *operation_list_list,
			int row_dictionary_list_length,
			LIST *no_display_pressed_attribute_name_list,
			char update_yn,
			char *state,
			LIST *non_edit_folder_name_list,
			boolean override_row_restrictions,
			char *folder_post_change_javascript,
			int max_query_rows_for_drop_downs,
			char *one2m_folder_name_for_processes,
			LIST *join_1tom_related_folder_list,
			boolean make_primary_keys_non_edit,
			boolean prompt_data_separate_folder );

#endif
