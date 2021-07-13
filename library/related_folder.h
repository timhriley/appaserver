/* $APPASERVER_HOME/library/related_folder.h				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver related_folder ADT.				*/
/* It is used as an interface to the RELATION folder.			*/ 
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef RELATED_FOLDER_H
#define RELATED_FOLDER_H

#include "list.h"
#include "folder.h"
#include "boolean.h"
#include "process.h"
#include "element.h"

/* Enumerated types */
/* ---------------- */
enum related_folder_recursive_request_type {
			related_folder_no_recursive,
			related_folder_prompt_recursive_only,
			related_folder_recursive_all };

enum relation_type {	mto1,
			one2m };

enum related_folder_list_usage {
			prompt_screen,
			edit_screen,
			detail,
			update,
			other,
			related_folder_unknown };

/* Constants */
/* --------- */
#define RELATED_FOLDER_AJAX_FILL_LABEL				"Fill"
#define RELATED_FOLDER_FOLDER_PIECE				0
#define RELATED_FOLDER_RELATED_FOLDER_PIECE			1
#define RELATED_FOLDER_RELATED_ATTRIBUTE_PIECE			2
#define RELATED_FOLDER_PAIR_1TOM_PIECE				3
#define RELATED_FOLDER_OMIT_1TOM_DETAIL_PIECE			4
#define RELATED_FOLDER_MTO1_RECURSIVE_PIECE			5
#define RELATED_FOLDER_RELATION_TYPE_ISA_PIECE			6
#define RELATED_FOLDER_COPY_COMMON_ATTRIBUTES_PIECE		7
#define RELATED_FOLDER_AUTOMATIC_PRESELECTION_PIECE		8
#define RELATED_FOLDER_DROP_DOWN_MULTI_SELECT_PIECE		9
#define RELATED_FOLDER_JOIN_1TOM_EACH_ROW_PIECE			10
#define RELATED_FOLDER_OMIT_LOOKUP_BEFORE_DROP_DOWN_PIECE	11
#define RELATED_FOLDER_AJAX_FILL_DROP_DOWN_PIECE		12
#define RELATED_FOLDER_HINT_MESSAGE_PIECE			13

#define RELATED_FOLDER_DELIMITER				'^'

/* Objects */
/* ------- */
typedef struct
{
	FOLDER *folder;
	FOLDER *one2m_folder;
	char *related_attribute_name;
	boolean relation_type_isa;
	int pair_1tom_order;
	boolean omit_1tom_detail;
	LIST *primary_data_list;
	boolean prompt_mto1_recursive;
	boolean copy_common_attributes;
	boolean ignore_output;
	LIST *common_non_primary_attribute_name_list;
	boolean automatic_preselection;
	boolean drop_down_multi_select;
	boolean join_1tom_each_row;
	boolean omit_lookup_before_drop_down;
	boolean ajax_fill_drop_down;
	LIST *automatic_preselection_dictionary_list;
	LIST *foreign_attribute_name_list;
	LIST *folder_foreign_attribute_name_list;
	char *hint_message;
	LIST *parent_primary_attribute_name_list;
	char *join_where_clause;
	int recursive_level /* zero based */;
	boolean is_primary_key_subset;
} RELATED_FOLDER;

/* Operations */
/* ---------- */
RELATED_FOLDER *related_folder_new(
			char *application_name,
			char *session,
			char *folder_name,
			char *related_attribute_name );

LIST *related_folder_foreign_attribute_name_list(
			LIST *primary_attribute_name_list,
			char *related_attribute_name,
			LIST *folder_foreign_attribute_name_list );

LIST *related_folder_get_primary_data_list(
			char *application_name,
			char *related_folder_name,
			LIST *select_attribute_name_list,
			LIST *where_attribute_name_list,
			LIST *primary_data_list );

RELATED_FOLDER *related_folder_attribute_consumes_related_folder(
			LIST **foreign_attribute_name_list,
			LIST *done_attribute_name_list,
			LIST *omit_update_attribute_name_list,
			LIST *mto1_related_folder_list,
			char *attribute_name,
			LIST *include_attribute_name_list );

LIST *related_folder_drop_down_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char *application_name,
			char *session,
			char *role_name,
			char *login_name,
			char *folder_name,
			PROCESS *populate_drop_down_process,
			LIST *attribute_list,
			LIST *foreign_attribute_name_list,
			boolean omit_ignore_push_buttons,
			DICTIONARY *preprompt_dictionary,
			char *ignore_or_no_display_push_button_prefix,
			char *ignore_or_no_display_push_button_heading,
			char *post_change_javascript,
			char *hint_message,
			int max_drop_down_size,
			LIST *common_non_primary_attribute_name_list,
			boolean is_primary_attribute,
			boolean row_level_non_owner_view_only,
			boolean row_level_non_owner_forbid,
			char *related_attribute_name,
			boolean drop_down_multi_select,
			boolean no_initial_capital,
			char *state,
			char *one2m_folder_name_for_processes,
			int tab_index,
			boolean set_first_initial_data,
			boolean output_null_option,
			boolean output_not_null_option,
			boolean output_select_option,
			char *appaserver_user_foreign_login_name,
			boolean prepend_folder_name,
			boolean omit_lookup_before_drop_down );

void related_folder_set_ignore_output_for_duplicate(
			LIST *related_folder_list );

LIST *related_folder_subtract_related_attribute_name_list(
			LIST *foreign_attribute_name_list,
			LIST *mto1_related_folder_list );

char *related_folder_list_display(
			LIST *related_folder_list,
			enum relation_type,
			char delimiter );

char *related_folder_display(
			RELATED_FOLDER *related_folder,
			enum relation_type relation_type );

LIST *related_folder_remove_duplicate_mto1_related_folder_list(
			LIST *mto1_related_folder_list );

LIST *related_folder_insert_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			/* --------------------------- */
			/* sets related_folder->folder */
			/* --------------------------- */
			RELATED_FOLDER *related_folder,
			char *application_name,
			char *session,
			char *login_name,
			LIST *foreign_attribute_name_list,
			int row_dictionary_list_length,
			DICTIONARY *parameter_dictionary,
			DICTIONARY *where_clause_dictionary,
			int prompt_data_element_only,
			char *post_change_javascript,
			int max_drop_down_size,
			boolean filter_login_name,
			boolean is_primary_attribute,
			char *role_name,
			int max_query_rows_for_drop_downs,
			boolean drop_down_multi_select,
			boolean no_initial_capital,
			char *one2m_folder_name_for_processes );

LIST *related_folder_update_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char *application_name,
			char *session,
			char *login_name,
			RELATED_FOLDER *related_folder,
			LIST *foreign_attribute_name_list,
			char update_yn,
			int row_dictionary_list_length,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			char *state,
			int prompt_data_element_only,
			char *post_change_javascript,
			int max_drop_down_size,
			boolean filter_login_name,
			boolean override_row_restrictions,
			boolean is_primary_attribute,
			char *role_name,
			int max_query_rows_for_drop_downs,
			boolean drop_down_multi_select,
			boolean no_initial_capital,
			char *one2m_folder_name_for_processes,
			boolean omit_lookup_before_drop_down );

LIST *related_folder_get_edit_insert_element_list(
			int row_dictionary_list_length,
			LIST *foreign_attribute_name_list,
			RELATED_FOLDER *related_folder,
			char *application_name,
			char *session,
			char *login_name,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			int prompt_data_element_only,
			char *post_change_javascript,
			int max_drop_down_size,
			boolean filter_login_name,
			boolean override_row_restrictions,
			boolean automatic_preselection,
			boolean drop_down_multi_select,
			boolean is_primary_attribute,
			char *role_name,
			int max_query_rows_for_drop_downs,
			boolean no_initial_capital,
			char *one2m_related_folder_name_for_processes );

LIST *related_folder_get_edit_lookup_element_list(
			LIST *foreign_attribute_name_list );

LIST *related_folder_1tom_related_folder_list(
			char *application_name,
			char *session,
			char *folder_name,
			char *role_name,
			enum related_folder_list_usage,
			LIST *primary_data_list,
			LIST *related_folder_list,
			boolean omit_isa_relations,
			enum related_folder_recursive_request_type,
			LIST *parent_primary_attribute_name_list,
			LIST *original_primary_attribute_name_list,
			char *prior_related_attribute_name );

LIST *related_folder_related_folder_list(
			char *application_name,
			char *session,
			char *folder_name,
			enum relation_type,
			LIST *existing_related_folder_list );

LIST *related_folder_get_mto1_related_folder_list(
			LIST *related_folder_list,
			char *application_name,
			char *session,
			char *folder_name,
			char *role_name,
			boolean isa_flag,
			enum related_folder_recursive_request_type,
			boolean override_row_restrictions,
			LIST *root_primary_attribute_name_list,
			int recursive_level );

LIST *related_folder_subtract_duplicate_related_folder_list(
			LIST *new_related_folder_list,
			LIST *related_folder_list );

LIST *related_folder_get_related_folder_name_list(
			LIST *related_folder_list );

LIST *related_folder_get_mto1_related_folder_name_list(
			LIST *mto1_related_folder_list );

LIST *related_folder_get_mto1_multi_key_name_list(
			LIST *mto1_related_folder_list );

LIST *related_folder_get_one2m_related_folder_name_list(
			LIST *one2m_related_folder_list );

LIST *related_folder_get_isa_related_folder_list(
			char *application_name,
			char *session,
			char *folder_name,
			char *role_name,
			boolean override_row_restrictions,
			enum related_folder_recursive_request_type );

boolean related_folder_exists_1tom_relations(
			char *application_name,
			char *session,
			char *folder_name,
			LIST *original_primary_attribute_name_list,
			char *related_attribute_name );

LIST *related_folder_global_related_folder_list(
			char *application_name,
			char *session,
			char delimiter );

boolean related_folder_exists_one2m_related_folder_list(
			char *related_folder_name,
			char *related_attribute_name,
			LIST *existing_related_folder_list );

boolean related_folder_exists_related_folder_list(
			char *related_folder_name,
			char *related_attribute_name,
			LIST *existing_related_folder_list,
			enum relation_type );

void related_folder_reset_ignore_output(
			LIST *related_folder_list );

LIST *related_folder_get_mto1_folder_name_list(
			LIST *mto1_isa_related_folder_list );

RELATED_FOLDER *related_folder_get_preprompt_related_folder(
			char *application_name,
			char *session,
			char *folder_name,
			char *role_name,
			boolean override_row_restrictions );

void related_folder_populate_common_non_primary_attribute_name_list(
			LIST *common_non_primary_attribute_name_list,
			char *application_name,
			char *related_folder_name,
			char *one2m_related_folder_name );

/* Note: this only applies when state=insert. */
/* ------------------------------------------ */
LIST *related_folder_get_common_non_primary_attribute_name_list(
			char *application_name,
			char *folder_name,
			LIST *mto1_related_folder_list );

LIST *related_folder_get_mto1_common_non_primary_related_folder_list(
			char *application_name,
			char *session,
			char *folder_name,
			boolean override_row_restrictions );

LIST *related_folder_list_get_preselection_dictionary_list(
			char *application_name,
			char *session,
			char *login_name,
			char *folder_name,
			DICTIONARY *query_dictionary,
			LIST *mto1_related_folder_list );

boolean related_folder_exists_prompt_mto1_recursive(
			LIST *mto1_related_folder_list );

boolean related_folder_exists_automatic_preselection(
			LIST *mto1_related_folder_list );

LIST *related_folder_get_preselection_dictionary_list(
			char *application_name,
			char *session,
			char *folder_name,
			char *related_folder_name,
			DICTIONARY *query_dictionary,
			char *login_name,
			char *role_name );

boolean related_folder_exists_all_key_data_list(
			DICTIONARY *dictionary,
			LIST *key_data_list,
			char delimiter );

LIST *related_folder_subtract_preselection_existing_dictionary_list(
			LIST *related_folder_dictionary_list,
			char *application_name,
			char *session,
			char *folder_name,
			DICTIONARY *query_dictionary,
			char *login_name,
			LIST *related_primary_attribute_name_list );

boolean related_folder_exists_pair_1tom(
			LIST *related_folder_list );

char *related_folder_get_hint_message(
			char *attribute_hint_message,
			char *related_folder_hint_message,
			char *folder_notepad );

LIST *related_folder_get_pair_one2m_related_folder_list(
			char *application_name,
			char *folder_name,
			char *role_name );

void related_folder_populate_primary_data_dictionary(
			LIST *mto1_related_folder_list,
			char *application_name );

boolean related_folder_exists_mto1_folder_name_list(
			LIST *mto1_related_folder_list,
			LIST *folder_name_list );

boolean related_folder_exists_mto1_folder_name(
			LIST *mto1_related_folder_list,
			char *folder_name );

char *related_folder_append_where_clause_related_join(
			FOLDER *folder,
			char *application_name,
			char *source_where_clause,
			RELATED_FOLDER *related_folder );

void related_folder_set_join_where_clause(
			LIST *one2m_related_folder_list,
			FOLDER *folder,
			char *application_name );

LIST *related_folder_lookup_before_drop_down_related_folder_list(
			LIST *related_folder_list,
			char *application_name,
			char *folder_name,
			LIST *base_folder_attribute_list,
			int recursive_level );

void related_folder_set_no_ignore_output(
			LIST *mto1_related_folder_list );

RELATED_FOLDER *related_folder_mto1_seek(
			LIST *mto1_related_folder_list,
			char *folder_name );

RELATED_FOLDER *related_folder_one2m_seek(
			LIST *one2m_related_folder_list,
			char *folder_name );

LIST *related_folder_get_join_1tom_related_folder_list(
			LIST *one2m_related_folder_list );

LIST *related_folder_join_1tom_related_folder_list(
			LIST *one2m_related_folder_list );

LIST *related_folder_get_non_edit_multi_element_list(
			char *folder_name );

boolean related_folder_mto1_exists_drop_down_multi_select(
			LIST *mto1_related_folder_list );

LIST *related_folder_fetch_non_primary_foreign_data_list(
			char *application_name,
			LIST *select_list,
			char *folder_name,
			LIST *primary_attribute_name_list,
			LIST *primary_data_list );

RELATED_FOLDER *related_folder_calloc(
			void );

void related_folder_one2m_append_unique(
			LIST *one2m_related_folder_list,
			RELATED_FOLDER *related_folder );

void related_folder_mto1_append_unique(
			LIST *mto1_related_folder_list1,
			LIST *mto1_related_folder_list2 );

char *related_folder_get_appaserver_user_foreign_login_name(
			LIST *mto1_related_folder_list );

RELATED_FOLDER *related_folder_get_view_only_related_folder(
			LIST *mto1_related_folder_list );

void related_folder_populate_one2m_foreign_attribute_dictionary(
			DICTIONARY *foreign_attribute_dictionary,
			char *last_primary_attribute_name,
			char *related_attribute_name );

void related_folder_list_populate_one2m_foreign_attribute_dictionary(
			DICTIONARY *foreign_attribute_dictionary,
			char *last_primary_attribute_name,
			LIST *one2m_recursive_related_folder_list );

void related_folder_list_populate_mto1_isa_foreign_attribute_dictionary(
			DICTIONARY *foreign_attribute_dictionary,
			char *last_primary_attribute_name,
			LIST *mto1_isa_related_folder_list,
			char *application_name );

int related_folder_pair_match_function(
			RELATED_FOLDER *related_folder_from_list,
			RELATED_FOLDER *related_folder_compare );

boolean related_folder_is_one2one_firewall(
			LIST *foreign_attribute_name_list,
			LIST *attribute_list );

LIST *related_folder_fetch_folder_foreign_attribute_name_list(
			char *application_name,
			char *folder_name,
			char *related_folder_name );

LIST *related_folder_fetch_folder_foreign_attribute_record_list(
			char *application_name );

void related_folder_mark_ignore_multi_attribute_primary_keys(
			LIST *mto1_related_folder_list );

void related_populate_folder_foreign_attribute_dictionary(
			DICTIONARY *foreign_attribute_dictionary,
			LIST *folder_foreign_attribute_name_list,
			LIST *primary_attribute_name_list );

RELATED_FOLDER *related_folder_get_ajax_fill_drop_down_related_folder(
			LIST *mto1_related_folder_list );

char *related_folder_ajax_onclick_function(
			LIST *attribute_name_list );

LIST *related_folder_mto1_isa_related_folder_list(
			LIST *existing_related_folder_list,
			char *application_name,
			char *folder_name,
			char *role_name,
			int recursive_level );

char *related_folder_mto1_list_display(
			LIST *related_folder_list );

LIST *related_folder_one2m_isa_related_folder_list(
			LIST *one2m_related_folder_list );

LIST *related_folder_prompt_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char *application_name,
			char *session,
			char *role_name,
			char *login_name,
			char *folder_name,
			PROCESS *populate_drop_down_process,
			LIST *attribute_list,
			LIST *foreign_attribute_name_list,
			boolean omit_ignore_push_buttons,
			DICTIONARY *preprompt_dictionary,
			char *ignore_or_no_display_push_button_prefix,
			char *ignore_or_no_display_push_button_heading,
			char *post_change_javascript,
			char *hint_message,
			int max_drop_down_size,
			LIST *common_non_primary_attribute_name_list,
			boolean is_primary_attribute,
			boolean row_level_non_owner_view_only,
			boolean row_level_non_owner_forbid,
			char *related_attribute_name,
			boolean drop_down_multi_select,
			boolean no_initial_capital,
			char *state,
			char *one2m_folder_name_for_processes,
			int tab_index,
			boolean set_first_initial_data,
			boolean output_null_option,
			boolean output_not_null_option,
			boolean output_select_option,
			char *appaserver_user_foreign_login_name,
			boolean omit_lookup_before_drop_down );

LIST *related_folder_edit_table_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char *application_name,
			char *session,
			char *login_name,
			RELATED_FOLDER *related_folder,
			LIST *foreign_attribute_name_list,
			char update_yn,
			int row_dictionary_list_length,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			char *state,
			boolean prompt_data_element_only,
			char *post_change_javascript,
			int max_drop_down_size,
			boolean override_row_restrictions,
			boolean is_primary_attribute,
			char *role_name,
			int max_query_rows_for_drop_downs,
			boolean drop_down_multi_select,
			boolean no_initial_capital,
			char *one2m_folder_name_for_processes,
			boolean omit_lookup_before_drop_down );

LIST *related_folder_prompt_insert_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char *application_name,
			char *session,
			char *role_name,
			char *login_name,
			char *folder_name,
			boolean relation_type_isa,
			PROCESS *populate_drop_down_process,
			LIST *attribute_list,
			LIST *foreign_attribute_name_list,
			boolean omit_drop_down_new_push_button,
			boolean omit_ignore_push_buttons,
			DICTIONARY *preprompt_dictionary,
			char *ignore_or_no_display_push_button_prefix,
			char *ignore_or_no_display_push_button_heading,
			char *post_change_javascript,
			char *hint_message,
			LIST *role_folder_insert_list,
			char *form_name,
			int max_drop_down_size,
			LIST *common_non_primary_attribute_name_list,
			boolean is_primary_attribute,
			char *related_attribute_name,
			boolean drop_down_multi_select,
			boolean no_initial_capital,
			char *state,
			char *one2m_folder_name_for_processes,
			int tab_index,
			boolean set_first_initial_data,
			boolean output_null_option,
			boolean output_not_null_option,
			boolean output_select_option,
			boolean omit_lookup_before_drop_down );

RELATED_FOLDER *related_folder_insert_table_consumes_related_folder(
			LIST **foreign_attribute_name_list,
			LIST *done_attribute_name_list,
			LIST *omit_insert_attribute_name_list,
			LIST *mto1_related_folder_list,
			char *attribute_name );

LIST *related_folder_prompt_insert_mto1_related_folder_list(
			LIST *related_folder_list,
			char *application_name,
			char *session,
			char *folder_name,
			char *role_name,
			boolean override_row_restrictions );

boolean related_folder_mto1_relation_type_isa(
			char *folder_name );

LIST *related_folder_regular_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char *application_name,
			char *session,
			char *login_name,
			RELATED_FOLDER *related_folder,
			LIST *foreign_attribute_name_list,
			int row_dictionary_list_length,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			char *state,
			boolean prompt_data_element_only,
			char *post_change_javascript,
			int max_drop_down_size,
			boolean row_level_non_owner_forbid,
			boolean override_row_restrictions,
			boolean is_primary_attribute,
			char *role_name,
			int max_query_rows_for_drop_downs,
			boolean drop_down_multi_select,
			boolean no_initial_capital,
			char *one2m_folder_name_for_processes,
			boolean omit_lookup_before_drop_down );

LIST *related_folder_viewonly_element_list(
			RELATED_FOLDER *related_folder,
			LIST *foreign_attribute_name_list,
			boolean is_primary_attribute );

#endif
