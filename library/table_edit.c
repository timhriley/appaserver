/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/table_edit.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "String.h"
#include "environ.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "row_security.h"
#include "application.h"
#include "appaserver.h"
#include "role.h"
#include "folder_operation.h"
#include "role_folder.h"
#include "relation_mto1.h"
#include "relation_one2m.h"
#include "relation_join.h"
#include "form.h"
#include "query.h"
#include "sql.h"
#include "javascript.h"
#include "frameset.h"
#include "post_table_edit.h"
#include "table_edit.h"

TABLE_EDIT *table_edit_calloc( void )
{
	TABLE_EDIT *table_edit;

	if ( ! ( table_edit =
			calloc( 1, sizeof ( TABLE_EDIT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return table_edit;
}

TABLE_EDIT_INPUT *table_edit_input_calloc( void )
{
	TABLE_EDIT_INPUT *table_edit_input;

	if ( ! ( table_edit_input =
			calloc( 1, sizeof ( TABLE_EDIT_INPUT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return table_edit_input;
}

TABLE_EDIT_INPUT *table_edit_input_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		boolean application_menu_horizontal_boolean,
		DICTIONARY *original_post_dictionary,
		char *data_directory,
		char *drilldown_base_folder_name,
		boolean viewonly_boolean,
		boolean omit_delete_boolean )
{
	TABLE_EDIT_INPUT *table_edit_input;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !target_frame
	||   !data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	table_edit_input = table_edit_input_calloc();

	table_edit_input->role_folder_list =
		role_folder_list(
			role_name,
			folder_name );

	table_edit_input->role =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		role_fetch(
			role_name,
			1 /* fetch_attribute_exclude_list */ );

	table_edit_input->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			/* ---------------------------------------- */
			/* Set to cache folders for the entire role */
			/* ---------------------------------------- */
			role_name,
			table_edit_input->exclude_lookup_attribute_name_list,
			/* -------------------------------------------- */
			/* Sets folder_attribute_primary_list		*/
			/* Sets folder_attribute_primary_key_list	*/
			/* Sets folder_attribute_name_list		*/
			/* -------------------------------------------- */
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */ );

	if ( !list_length( table_edit_input->folder->folder_attribute_list )
	||   !list_length(
		table_edit_input->
			folder->
			folder_attribute_primary_key_list )
	||   !list_length(
		table_edit_input->
			folder->
			folder_attribute_name_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"folder_fetch(%s) returned without any attributes.",
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	table_edit_input->folder_attribute_non_primary_name_list =
		folder_attribute_non_primary_name_list(
			table_edit_input->folder->folder_attribute_list );

	table_edit_input->exclude_lookup_attribute_name_list =
		role_attribute_exclude_name_list(
			ROLE_PERMISSION_LOOKUP,
			table_edit_input->role->role_attribute_exclude_list );

	table_edit_input->relation_mto1_list =
		relation_mto1_list(
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			folder_name
				/* many_folder_name */,
			table_edit_input->
				folder->
				folder_attribute_primary_key_list
				/* many_primary_key_list */ );

	table_edit_input->relation_mto1_list =
		relation_mto1_without_omit_drillthru_list(
			table_edit_input->relation_mto1_list );

	table_edit_input->relation_mto1_list =
		relation_mto1_without_omit_update_list(
			table_edit_input->relation_mto1_list );

	table_edit_input->relation_mto1_isa_list =
		relation_mto1_isa_list(
			(LIST *)0 /* mto1_isa_list */,
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			folder_name
				/* many_folder_name */,
			table_edit_input->
				folder->
				folder_attribute_primary_key_list
				/* many_primary_key_list */,
			0 /* not fetch_relation_one2m_list */,
			1 /* fetch_relation_mto1_list */ );

	table_edit_input->folder_attribute_append_isa_list =
		folder_attribute_append_isa_list(
			table_edit_input->
				folder->
				folder_attribute_list,
			table_edit_input->relation_mto1_isa_list );

	table_edit_input->folder_operation_list =
		folder_operation_list(
			folder_name,
			role_name,
			1 /* fetch_operation */,
			1 /* fetch_process */ );

	if ( omit_delete_boolean )
	{
		table_edit_input->folder_operation_list =
			table_edit_input_trim_delete_operation_list(
				table_edit_input->folder_operation_list );
	}

	if ( list_length( table_edit_input->relation_mto1_isa_list ) )
	{
		table_edit_input_isa_delete_prompt_set(
			folder_name,
			table_edit_input->folder_operation_list /* in/out */ );
	}

	if ( ! ( table_edit_input->table_edit_state =
			/* ------------------------------ */
			/* Returns program memory or null */
			/* ------------------------------ */
			table_edit_state(
				folder_name,
				viewonly_boolean,
				table_edit_input->role_folder_list ) ) )
	{
		return table_edit_input;
	}

	table_edit_input->folder_row_level_restriction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_row_level_restriction_fetch(
			folder_name );

	table_edit_input->security_entity =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		security_entity_new(
			login_name,
			table_edit_input->
				folder_row_level_restriction->
				non_owner_forbid,
			table_edit_input->role->override_row_restrictions );

	table_edit_input->folder_attribute_date_name_list =
		folder_attribute_date_name_list(
			table_edit_input->folder_attribute_append_isa_list );

	table_edit_input->dictionary_separate =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_table_edit_new(
			original_post_dictionary,
			application_name,
			login_name,
			table_edit_input->folder_attribute_date_name_list,
			table_edit_input->folder_attribute_append_isa_list );

	table_edit_input->onload_javascript_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		table_edit_input_onload_javascript_string(
			TABLE_EDIT_ONLOAD_JAVASCRIPT_KEY,
			table_edit_input->
				dictionary_separate->
				non_prefixed_dictionary );

	table_edit_input->relation_one2m_join_list =
		relation_one2m_join_list(
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			folder_name,
			table_edit_input->
				folder->
				folder_attribute_primary_key_list,
			table_edit_input->
				dictionary_separate->
				no_display_dictionary );

	if ( !drilldown_base_folder_name )
	{
		table_edit_input->menu_horizontal_boolean =
			menu_horizontal_boolean(
				target_frame /* current_frame */,
				FRAMESET_PROMPT_FRAME,
				application_menu_horizontal_boolean );
	}

	if ( table_edit_input->menu_horizontal_boolean )
	{
		table_edit_input->folder_menu =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			folder_menu_fetch(
				application_name,
				session_key,
				role_name,
				data_directory,
				0 /* not folder_menu_remove_boolean */ );

		table_edit_input->menu =
			menu_new(
				application_name,
				session_key,
				login_name,
				role_name,
				1 /* application_menu_horizontal_boolean */,
				table_edit_input->folder_menu->count_list );
	}

	if ( strcmp(	table_edit_input->table_edit_state,
			APPASERVER_VIEWONLY_STATE ) != 0 )
	{
		table_edit_input->row_security_role_update_list =
			/* --------------------------------- */
			/* Returns null if not participating */
			/* --------------------------------- */
			row_security_role_update_list_fetch(
				application_name,
				role_name,
				folder_name,
				table_edit_input->
					role->
					override_row_restrictions );
	}

	table_edit_input->drillthru_status =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		drillthru_status_new(
			table_edit_input->
				dictionary_separate->
				drillthru_dictionary );

	if ( !viewonly_boolean )
	{
		table_edit_input->max_foreign_key_list_length =
			table_edit_input_max_foreign_key_list_length(
				table_edit_input->
					drillthru_status->
					skipped_boolean,
				drilldown_base_folder_name );

		table_edit_input->viewonly_attribute_name_list =
			table_edit_input_viewonly_attribute_name_list(
				table_edit_input->
					role->
					role_attribute_exclude_list,
				table_edit_input->
					relation_mto1_list,
				table_edit_input->
					folder_attribute_append_isa_list,
				table_edit_input->
					max_foreign_key_list_length,
				table_edit_input->
					drillthru_status->
					skipped_boolean );
	}

	table_edit_input->date_convert_login_name_enum =
		date_convert_login_name_enum(
			application_name,
			login_name );

	return table_edit_input;
}

TABLE_EDIT *table_edit_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		pid_t process_id,
		char *results_string,
		char *error_string,
		boolean application_menu_horizontal_boolean,
		DICTIONARY *original_post_dictionary,
		char *data_directory,
		char *drilldown_base_folder_name,
		char *drilldown_primary_data_list_string,
		LIST *drilldown_relation_foreign_key_list,
		boolean viewonly_boolean,
		boolean omit_delete_boolean )
{
	TABLE_EDIT *table_edit;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !target_frame
	||   !data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( drilldown_base_folder_name
	&&   !*drilldown_base_folder_name )
	{
		drilldown_base_folder_name = (char *)0;
	}

	table_edit = table_edit_calloc();

	table_edit->table_edit_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		table_edit_input_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			target_frame,
			application_menu_horizontal_boolean,
			original_post_dictionary,
			data_directory,
			drilldown_base_folder_name,
			viewonly_boolean,
			omit_delete_boolean );

	if ( !table_edit->table_edit_input->table_edit_state )
	{
		free( table_edit );
		return NULL;
	}

	table_edit->query_table_edit =
		/* ---------------------------- */
		/* Safely returns.		*/
		/* Note: frees each where.	*/
		/* ---------------------------- */
		query_table_edit_new(
			application_name,
			login_name,
			folder_name,
			table_edit->
				table_edit_input->
				security_entity->
				where,
			table_edit->
				table_edit_input->
				dictionary_separate->
				no_display_name_list,
			table_edit->
				table_edit_input->
				exclude_lookup_attribute_name_list,
			table_edit->
				table_edit_input->
				relation_mto1_list,
			table_edit->
				table_edit_input->
				relation_mto1_isa_list,
			table_edit->
				table_edit_input->
				folder_attribute_append_isa_list,
			table_edit->
				table_edit_input->
				folder->
				folder_attribute_primary_key_list,
			table_edit->
				table_edit_input->
				folder->
				folder_attribute_name_list,
			table_edit->
				table_edit_input->
				dictionary_separate->
				query_dictionary,
			table_edit->
				table_edit_input->
				dictionary_separate->
				sort_dictionary,
			/* ------------------------- */
			/* Null if not participating */
			/* ------------------------- */
			table_edit->
				table_edit_input->
				row_security_role_update_list,
			TABLE_EDIT_QUERY_MAX_ROWS );

	table_edit->query_row_list_length =
		table_edit_query_row_list_length(
			table_edit->
				query_table_edit->
				query_fetch->
				row_list );

	table_edit->row_security =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		row_security_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			table_edit->
				table_edit_input->
				folder_attribute_non_primary_name_list,
			table_edit->table_edit_input->relation_mto1_list,
			table_edit->table_edit_input->relation_mto1_isa_list,
			table_edit->table_edit_input->relation_one2m_join_list,
			table_edit->
				table_edit_input->
				folder_attribute_append_isa_list,
			table_edit->
				table_edit_input->
				folder->
				post_change_javascript,
			table_edit->
				table_edit_input->
				folder_row_level_restriction->
				non_owner_viewonly,
			table_edit->
				table_edit_input->
				dictionary_separate->
				drillthru_dictionary
				/* drop_down_dictionary */,
			table_edit->
				table_edit_input->
				dictionary_separate->
				no_display_name_list,
			table_edit->
				query_table_edit->
				query_select_name_list,
			table_edit->
				table_edit_input->
				viewonly_attribute_name_list,
			table_edit->
				table_edit_input->
				table_edit_state,
			table_edit->
				table_edit_input->
				row_security_role_update_list,
			table_edit->query_row_list_length );

	table_edit->post_action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		table_edit_post_action_string(
			POST_TABLE_EDIT_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			target_frame,
			process_id,
			drilldown_base_folder_name,
			drilldown_primary_data_list_string,
			drilldown_relation_foreign_key_list );

	table_edit->heading_label_list =
		table_edit_heading_label_list(
			table_edit->row_security->regular_widget_list,
			table_edit->row_security->viewonly_widget_list );

	table_edit->heading_name_list =
		table_edit_heading_name_list(
			table_edit->row_security->regular_widget_list,
			table_edit->row_security->viewonly_widget_list );

	table_edit->form_table_edit =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_table_edit_new(
			folder_name,
			target_frame,
			table_edit->
				table_edit_input->
				folder->
				post_change_javascript,
			table_edit->
				table_edit_input->
				dictionary_separate->
				query_dictionary,
			table_edit->
				table_edit_input->
				dictionary_separate->
				drillthru_dictionary,
			table_edit->
				table_edit_input->
				dictionary_separate->
				no_display_dictionary,
			table_edit->
				table_edit_input->
				folder_operation_list,
			table_edit->query_row_list_length,
			table_edit->post_action_string,
			table_edit->heading_label_list,
			table_edit->heading_name_list );

	table_edit->appaserver_update_filespecification =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_update_filespecification(
			application_name,
			folder_name,
			session_key,
			process_id,
			drilldown_relation_foreign_key_list,
			data_directory );

	if ( !drilldown_base_folder_name )
	{
		table_edit->document_notepad =
			/* ------------------------------- */
			/* Returns folder->notepad or null */
			/* ------------------------------- */
			table_edit_document_notepad(
				FRAMESET_PROMPT_FRAME,
				target_frame,
				table_edit->
					table_edit_input->
					folder->
					notepad );

		table_edit->table_edit_document =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			table_edit_document_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				table_edit->
					table_edit_input->
					table_edit_state,
				table_edit->
					table_edit_input->
					folder_attribute_append_isa_list,
				table_edit->
					table_edit_input->
					onload_javascript_string,
				results_string,
				error_string,
				table_edit->
					table_edit_input->
					folder->
					javascript_filename,
				table_edit->
					form_table_edit->
					sort_buttons_boolean,
				table_edit->
					table_edit_input->
					folder->
					post_change_javascript,
				table_edit->
					table_edit_input->
					menu,
				table_edit->row_security,
				table_edit->document_notepad );
	}

	if ( list_length(
		table_edit->
			table_edit_input->
			relation_one2m_join_list ) )
	{
		table_edit->relation_join =
			relation_join_new(
				table_edit->
					table_edit_input->
					folder->
					folder_attribute_primary_key_list
					/* one_folder_primary_key_list */,
				table_edit->
					table_edit_input->
					relation_one2m_join_list,
				table_edit->
					query_table_edit->
					query_fetch->
					row_list );
	}

	return table_edit;
}

char *table_edit_state(
		char *folder_name,
		boolean viewonly_boolean,
		LIST *role_folder_list )
{
	if ( viewonly_boolean ) return APPASERVER_VIEWONLY_STATE;

	if ( role_folder_update_boolean(
		ROLE_PERMISSION_UPDATE,
		folder_name,
		role_folder_list ) )
	{
		return APPASERVER_UPDATE_STATE;
	}

	if ( role_folder_lookup_boolean(
		ROLE_PERMISSION_LOOKUP,
		ROLE_PERMISSION_UPDATE,
		folder_name,
		role_folder_list ) )
	{
		return APPASERVER_VIEWONLY_STATE;
	}

	return (char *)0;
}

char *table_edit_document_html(
		char *document_html,
		char *document_head_html,
		char *document_head_close_tag,
		char *document_body_html,
		char *ajax_client_list_javascript )
{
	char html[ STRING_128K ];

	if ( !document_html
	||   !document_head_html
	||   !document_head_close_tag
	||   !document_body_html )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if (	strlen( document_html ) +
		strlen( document_head_html ) +
		strlen( document_head_close_tag ) +
		strlen( document_body_html ) +
		string_strlen( ajax_client_list_javascript ) +
		4 >= STRING_128K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_128K );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(html,
		"%s\n%s\n%s\n%s\n%s",
		document_html,
		document_head_html,
		document_head_close_tag,
		document_body_html,
		(ajax_client_list_javascript)
			? ajax_client_list_javascript
			: "" );

	return strdup( html );
}

char *table_edit_document_trailer_html(
		char *document_body_close_tag,
		char *document_close_tag )
{
	char trailer_html[ 1024 ];

	if ( !document_body_close_tag
	||   !document_close_tag )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if (	strlen( document_body_close_tag ) +
		strlen( document_close_tag ) + 1 >= 1024 )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			1024 );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(trailer_html,
		"%s\n%s",
		document_body_close_tag,
		document_close_tag );

	return strdup( trailer_html );
}

LIST *table_edit_apply_widget_container_list(
		ROW_SECURITY_REGULAR_WIDGET_LIST *
			regular_widget_list,
		ROW_SECURITY_VIEWONLY_WIDGET_LIST *
			viewonly_widget_list,
		boolean table_edit_row_viewonly )
{
	LIST *apply_widget_container_list;

	if ( !regular_widget_list && !viewonly_widget_list )
	{
		char message[ 128 ];

		sprintf(message,
	"both regular_widget_list and viewonly_widget_list are empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( table_edit_row_viewonly )
	{
		if ( !viewonly_widget_list )
		{
			char message[ 128 ];

			sprintf(message, "viewonly_widget_list is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		apply_widget_container_list =
			viewonly_widget_list->widget_container_list;
	}
	else
	if ( regular_widget_list )
	{
		apply_widget_container_list =
			regular_widget_list->widget_container_list;
	}
	else
	{
		apply_widget_container_list =
			viewonly_widget_list->widget_container_list;
	}

	return apply_widget_container_list;
}

char *table_edit_row_html(
		LIST *folder_operation_list,
		char *table_edit_state,
		int row_number,
		LIST *query_row_cell_list,
		RELATION_JOIN_ROW *relation_join_row,
		LIST *apply_widget_container_list,
		boolean table_edit_row_viewonly_boolean,
		char *form_background_color )
{
	FOLDER_OPERATION *folder_operation;
	char row_html[ STRING_704K ];
	char *ptr = row_html;
	char *html;

	if ( list_rewind( folder_operation_list ) )
	do {
		folder_operation =
			list_get(
				folder_operation_list );

		if ( !folder_operation->operation
		||   !folder_operation->operation->widget_container
		||   !folder_operation->
			operation->
			widget_container->
			checkbox )
		{
			char message[ 128 ];

			sprintf(message,
				"folder_operation is incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		html =
		    /* ------------------- */
		    /* Returns heap memory */
		    /* ------------------- */
		    operation_html(
			folder_operation->
				operation->
				widget_container,
			table_edit_state,
			row_number,
			form_background_color,
			table_edit_delete_mask_boolean(
				folder_operation->
					operation->
					delete_boolean,
			table_edit_row_viewonly_boolean ) );

		ptr += sprintf(
			ptr,
			"%s%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_table_data_tag(
				0 /* not align_right */,
				0 /* column_span */,
				0 /* width */,
				(char *)0 /* style */ ),
			html );

		free( html );

	} while ( list_next( folder_operation_list ) );

	html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_table_edit_row_html(
			table_edit_state,
			row_number,
			query_row_cell_list,
			relation_join_row,
			apply_widget_container_list,
			form_background_color,
			0 /* not hidden_only */ );

	if ( strlen( row_html ) + strlen( html ) >= STRING_704K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_704K );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( html ) ptr += sprintf( ptr, "%s", html );

	free( html );

	return strdup( row_html );
}

void table_edit_output(	
		const char *appaserver_user_login_name,
		char *login_name,
		boolean non_owner_viewonly,
		char *document_html,
		char *form_table_edit_open_html,
		char *form_table_edit_heading_container_string,
		LIST *folder_operation_list,
		LIST *query_fetch_row_list,
		RELATION_JOIN *relation_join,
		ROW_SECURITY_REGULAR_WIDGET_LIST *
			regular_widget_list,
		ROW_SECURITY_VIEWONLY_WIDGET_LIST *
			viewonly_widget_list,
		ROW_SECURITY_ROLE_UPDATE_LIST *
			row_security_role_update_list,
		char *table_edit_state,
		char *form_table_edit_close_html,
		char *document_trailer_html )
{
	if ( !login_name
	||   !form_table_edit_open_html
	||   !form_table_edit_heading_container_string
	||   !table_edit_state
	||   !form_table_edit_close_html )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( document_html )
	{
		printf( "%s\n",
			document_html );
	}

	if ( list_length( query_fetch_row_list ) > TABLE_EDIT_QUERY_MAX_ROWS )
	{
		printf(	"<h3>Warning: Max rows of %d exceeded.</h3>\n",
			TABLE_EDIT_QUERY_MAX_ROWS );
	}

	printf( "%s\n",
		form_table_edit_open_html );

	table_edit_output_row_list(
		appaserver_user_login_name,
		login_name,
		non_owner_viewonly,
		form_table_edit_heading_container_string,
		folder_operation_list,
		query_fetch_row_list,
		relation_join,
		regular_widget_list,
		viewonly_widget_list,
		row_security_role_update_list,
		table_edit_state );

	printf(	"%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		widget_table_close_tag() );

	table_edit_output_row_list_hidden(
		appaserver_user_login_name,
		login_name,
		non_owner_viewonly,
		query_fetch_row_list,
		regular_widget_list,
		viewonly_widget_list,
		row_security_role_update_list );

	printf(	"%s\n",
		form_table_edit_close_html );

	if ( document_trailer_html )
	{
		printf(	"%s\n",
			document_trailer_html );
	}
}

void table_edit_output_row_list(
		const char *appaserver_user_login_name,
		char *login_name,
		boolean non_owner_viewonly,
		char *form_table_edit_heading_container_string,
		LIST *folder_operation_list,
		LIST *query_fetch_row_list,
		RELATION_JOIN *relation_join,
		ROW_SECURITY_REGULAR_WIDGET_LIST *
			regular_widget_list,
		ROW_SECURITY_VIEWONLY_WIDGET_LIST *
			viewonly_widget_list,
		/* ------------------------- */
		/* Null if not participating */
		/* ------------------------- */
		ROW_SECURITY_ROLE_UPDATE_LIST *
			row_security_role_update_list,
		char *table_edit_state )
{
	QUERY_ROW *query_row;
	LIST *apply_widget_container_list;
	char *html;
	char *background_color;
	int row_number = 0;
	boolean viewonly_boolean;
	RELATION_JOIN_ROW *relation_join_row = {0};

	if ( !form_table_edit_heading_container_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( query_fetch_row_list ) ) return;

	if ( relation_join )
	{
		list_rewind( relation_join->relation_join_row_list );
	}

	do {
		if ( row_number == TABLE_EDIT_QUERY_MAX_ROWS )
		{
			break;
		}

		query_row = list_get( query_fetch_row_list );

		viewonly_boolean =
			table_edit_row_viewonly_boolean(
				appaserver_user_login_name,
				login_name,
				non_owner_viewonly,
				query_row->cell_list,
				query_row->viewonly_boolean,
				row_security_role_update_list );

		apply_widget_container_list =
			table_edit_apply_widget_container_list(
				regular_widget_list,
				viewonly_widget_list,
				viewonly_boolean );

		if ( !apply_widget_container_list )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: table_edit_apply_widget_container_list() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( relation_join )
		{
			relation_join_row =
				list_get(
					relation_join->
						relation_join_row_list );

			list_next( relation_join->relation_join_row_list );
		}

		/* Returns static memory */
		/* --------------------- */
		background_color = form_background_color();

		if ( row_number && row_number % 10 == 0 )
		{
			printf(	"%s\n",
				form_table_edit_heading_container_string );
		}

		html =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			table_edit_row_html(
				folder_operation_list,
				table_edit_state,
				++row_number,
				query_row->cell_list,
				relation_join_row,
				apply_widget_container_list,
				viewonly_boolean,
				background_color );

		printf(	"%s\n%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_table_row_tag( background_color ),
			html );

		free( html );

	} while ( list_next( query_fetch_row_list ) );
}

void table_edit_output_row_list_hidden(
		const char *appaserver_user_login_name,
		char *login_name,
		boolean non_owner_viewonly,
		LIST *query_fetch_row_list,
		ROW_SECURITY_REGULAR_WIDGET_LIST *
			regular_widget_list,
		ROW_SECURITY_VIEWONLY_WIDGET_LIST *
			viewonly_widget_list,
		ROW_SECURITY_ROLE_UPDATE_LIST *
			row_security_role_update_list )
{
	QUERY_ROW *query_row;
	char *html;
	int row_number = 0;
	boolean viewonly_boolean;
	LIST *apply_widget_container_list;

	if ( list_rewind( query_fetch_row_list ) )
	do {
		query_row = list_get( query_fetch_row_list );

		viewonly_boolean =
			table_edit_row_viewonly_boolean(
				appaserver_user_login_name,
				login_name,
				non_owner_viewonly,
				query_row->cell_list,
				query_row->viewonly_boolean,
				row_security_role_update_list );

		apply_widget_container_list =
			table_edit_apply_widget_container_list(
				regular_widget_list,
				viewonly_widget_list,
				viewonly_boolean );

		if ( !apply_widget_container_list )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: table_edit_apply_widget_container_list() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		html =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			widget_table_edit_row_html(
				(char *)0 /* table_edit_state */,
				++row_number,
				query_row->cell_list,
				(RELATION_JOIN_ROW *)0,
				apply_widget_container_list,
				(char *)0 /* form_background_color */,
				1 /* hidden_only */ );

		if ( *html ) printf( "%s\n", html );

		free( html );

	} while ( list_next( query_fetch_row_list ) );
}

char *table_edit_document_title_string(
		char *folder_name,
		char *state )
{
	static char string[ 128 ];

	if ( !folder_name
	||   !state )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(string,
		"%s %s",
		state,
		folder_name );

	return
	string_initial_capital(
		string,
		string );
}

boolean table_edit_row_viewonly_boolean(
		const char *appaserver_user_login_name,
		char *login_name,
		boolean non_owner_viewonly,
		LIST *query_row_cell_list,
		boolean query_row_viewonly_boolean,
		ROW_SECURITY_ROLE_UPDATE_LIST *
			row_security_role_update_list )
{
	QUERY_CELL *query_cell;

	if ( query_row_viewonly_boolean ) return 1;

	if ( non_owner_viewonly )
	{
		query_cell =
			query_cell_seek(
				(char *)appaserver_user_login_name
					/* attribute_name */,
				query_row_cell_list );

		if ( query_cell )
		{
			if ( string_strcmp(
				query_cell->select_datum,
				login_name ) != 0 )
			{
				return 1;
			}
		}
	}

	if ( row_security_role_update_list )
	{
		query_cell =
			query_cell_seek(
				row_security_role_update_list->
					attribute_not_null
						/* attribute_name */,
				query_row_cell_list );

		if (	query_cell
		&&	query_cell->select_datum
		&&	*query_cell->select_datum )
		{
			return 1;
		}
	}

	return 0;
}

boolean table_edit_delete_mask_boolean(
		boolean operation_delete_boolean,
		boolean table_edit_row_viewonly_boolean )
{
	return
	( operation_delete_boolean && table_edit_row_viewonly_boolean );
}

LIST *table_edit_heading_label_list(
		ROW_SECURITY_REGULAR_WIDGET_LIST *
			regular_widget_list,
		ROW_SECURITY_VIEWONLY_WIDGET_LIST *
			viewonly_widget_list )
{
	if ( !regular_widget_list
	&&   !viewonly_widget_list )
	{
		char message[ 128 ];

		sprintf(message,
	"both regular_widget_list and viewonly_widget_list are empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( regular_widget_list )
	{
		return
		widget_container_heading_label_list(
			regular_widget_list->
				widget_container_list );
	}
	else
	{
		return
		widget_container_heading_label_list(
			viewonly_widget_list->
				widget_container_list );
	}
}

LIST *table_edit_heading_name_list(
		ROW_SECURITY_REGULAR_WIDGET_LIST *
			regular_widget_list,
		ROW_SECURITY_VIEWONLY_WIDGET_LIST *
			viewonly_widget_list )
{
	if ( !regular_widget_list
	&&   !viewonly_widget_list )
	{
		char message[ 128 ];

		sprintf(message,
	"both regular_widget_list and viewonly_widget_list are empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( regular_widget_list )
	{
		return
		widget_container_heading_name_list(
			regular_widget_list->
				widget_container_list );
	}
	else
	{
		return
		widget_container_heading_name_list(
			viewonly_widget_list->
				widget_container_list );
	}
}

FILE *table_edit_spool_file_open( char *spool_filename )
{
	if ( !spool_filename )
	{
		char message[ 128 ];

		sprintf(message, "spool_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return fopen( spool_filename, "w" );
}

void table_edit_spool_file_output(
		char *spool_filename,
		LIST *query_fetch_row_list )
{
	FILE *file_open;
	QUERY_ROW *query_row;
	int row_number;

	if ( !list_rewind( query_fetch_row_list ) ) return;

	if ( ! ( file_open = table_edit_spool_file_open( spool_filename ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: table_edit_spool_file_open(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			spool_filename );
		exit( 1 );
	}

	row_number = 0;

	do {
		query_row = list_get( query_fetch_row_list );

		table_edit_spool_file_cell_list_output(
			file_open,
			++row_number,
			query_row->cell_list );

	} while ( list_next( query_fetch_row_list ) );

	fclose( file_open );
}

void table_edit_spool_file_cell_list_output(
		FILE *file_open,
		int row_number,
		LIST *cell_list )
{
	QUERY_CELL *query_cell;
	char destination[ STRING_64K ];

	if ( list_rewind( cell_list ) )
	do {
		query_cell = list_get( cell_list );

		fprintf(file_open,
			"%s_%d%c%s\n",
			query_cell->attribute_name,
			row_number,
			SQL_DELIMITER,
			string_escape_character(
				destination,
				query_cell->select_datum,
				SQL_DELIMITER ) );

	} while ( list_next( cell_list ) );
}

char *table_edit_post_action_string(
		char *post_table_edit_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		pid_t process_id,
		char *drilldown_base_folder_name,
		char *drilldown_primary_data_list_string,
		LIST *drilldown_relation_foreign_key_list )
{
	char action_string[ 1024 ];

	if ( !post_table_edit_executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !target_frame
	||   !process_id )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		action_string,
		sizeof ( action_string ),
		"%s/%s?%s+%s+%s+%s+%s+%s+%d+%s+%s+%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_http_prompt(
			appaserver_parameter_cgi_directory(),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
			application_ssl_support_boolean(
				application_name ) ),
		post_table_edit_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		folder_name,
		target_frame,
		process_id,
		(drilldown_base_folder_name)
			? drilldown_base_folder_name
			: "",
		(drilldown_primary_data_list_string)
			? drilldown_primary_data_list_string
			: "",
		(drilldown_relation_foreign_key_list)
			? list_display_delimited(
				drilldown_relation_foreign_key_list,
				ATTRIBUTE_MULTI_KEY_DELIMITER )
			: "" );

	return strdup( action_string );
}

LIST *table_edit_document_javascript_filename_list(
		char *folder_javascript_filename,
		boolean sort_buttons_boolean )
{
	LIST *filename_list = list_new();

	list_set( filename_list, "table_edit.js" );
	list_set( filename_list, "post_change_attribute.js" );

	if ( sort_buttons_boolean )
	{
		list_set( filename_list, "push_button_submit.js" );
	}

	if ( folder_javascript_filename )
	{
		list_set( filename_list, folder_javascript_filename );
	}

	return filename_list;
}

int table_edit_query_row_list_length( LIST *query_fetch_row_list )
{
	return list_length( query_fetch_row_list );
}

char *table_edit_document_javascript_replace(
		const char *appaserver_update_state,
		char *onload_javascript_string,
		char *post_change_javascript )
{
	char document_javascript_replace[ 1024 ];
	char *ptr = document_javascript_replace;

	*document_javascript_replace = '\0';

	if ( post_change_javascript )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			javascript_replace(
				post_change_javascript,
				(char *)appaserver_update_state,
				JAVASCRIPT_ONLOAD_ROW_NUMBER
					/* probably -1 */ ) );
	}

	if ( onload_javascript_string )
	{
		if ( ptr != document_javascript_replace )
			ptr += sprintf( ptr, " && " );

		ptr += sprintf(
			ptr,
			"%s",
			onload_javascript_string );
	}

	if ( *document_javascript_replace )
		return strdup( document_javascript_replace );
	else
		return (char *)0;
}

LIST *table_edit_input_trim_delete_operation_list(
		LIST *folder_operation_list )
{
	LIST *list = list_new();
	FOLDER_OPERATION *folder_operation;

	if ( list_rewind( folder_operation_list ) )
	do {
		folder_operation = list_get( folder_operation_list );

		if ( string_strncmp(
			folder_operation->operation_name /* large_string */,
			"delete" /* small_string */ ) == 0 )
		{
			continue;
		}

		list_set( list, folder_operation );

	} while ( list_next( folder_operation_list ) );

	if ( !list )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

LIST *table_edit_input_viewonly_attribute_name_list(
		LIST *role_attribute_exclude_list,
		LIST *relation_mto1_list,
		LIST *folder_attribute_append_isa_list,
		int max_foreign_key_list_length,
		boolean drillthru_status_skipped_boolean )
{
	LIST *viewonly_attribute_name_list;
	LIST *foreign_key_less_equal_list;
	RELATION_MTO1 *relation_mto1;

	viewonly_attribute_name_list =
		role_attribute_exclude_name_list(
			ROLE_PERMISSION_UPDATE,
			role_attribute_exclude_list );

	if ( !viewonly_attribute_name_list )
	{
		viewonly_attribute_name_list = list_new();
	}

	if ( list_length( relation_mto1_list )
	&&   max_foreign_key_list_length )
	{
		foreign_key_less_equal_list =
			relation_mto1_foreign_key_less_equal_list(
				relation_mto1_list,
				max_foreign_key_list_length );

		list_rewind( relation_mto1_list );

		do {
			relation_mto1 =
				list_get(
					relation_mto1_list );

			if ( table_edit_input_relation_not_used(
				relation_mto1,
				foreign_key_less_equal_list ) )
			{
				if ( !list_length(
					relation_mto1->
						relation_foreign_key_list ) )
				{
					char message[ 128 ];

					sprintf(message,
				"relation_foreign_key_list is empty." );

					appaserver_error_stderr_exit(
						__FILE__,
						__FUNCTION__,
						__LINE__,
						message );
				}

				list_set_list(
					viewonly_attribute_name_list,
					relation_mto1->
						relation_foreign_key_list );
			}
			
		} while ( list_next( relation_mto1_list ) );
	}

	if ( drillthru_status_skipped_boolean
	&&   list_rewind( relation_mto1_list ) )
	{
		do {
			relation_mto1 =
				list_get(
					relation_mto1_list );

			if ( !relation_mto1->one_folder
			||   !list_length(
				relation_mto1->
					relation_foreign_key_list ) )
			{
				char message[ 128 ];

				sprintf(message,
				"relation_mto1 is incomplete." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( relation_mto1->one_folder->drillthru )
			{
				list_set_list(
					viewonly_attribute_name_list,
					relation_mto1->
						relation_foreign_key_list );
			}

		} while ( list_next( relation_mto1_list ) );
	}

	list_set_list(
		viewonly_attribute_name_list,
		folder_attribute_viewonly_attribute_name_list(
			folder_attribute_append_isa_list ) );

	if ( !list_length( viewonly_attribute_name_list ) )
	{
		list_free( viewonly_attribute_name_list );
		viewonly_attribute_name_list = (LIST *)0;
	}

	return viewonly_attribute_name_list;
}

int table_edit_input_max_foreign_key_list_length(
		boolean drillthru_status_skipped_boolean,
		char *drilldown_base_folder_name )
{
	int max_foreign_key_list_length = 0;

	if ( drillthru_status_skipped_boolean )
		max_foreign_key_list_length = 1;
	else
	if ( drilldown_base_folder_name )
		max_foreign_key_list_length = 2;

	return max_foreign_key_list_length;
}

boolean table_edit_input_relation_not_used(
		RELATION_MTO1 *relation_mto1,
		LIST *relation_mto1_foreign_key_less_equal_list )
{
	RELATION_MTO1 *relation_mto1_foreign;

	if ( list_rewind( relation_mto1_foreign_key_less_equal_list ) )
	do {
		relation_mto1_foreign =
			list_get(
				relation_mto1_foreign_key_less_equal_list );

		if ( relation_mto1_foreign == relation_mto1 )
		{
			return 0;
		}

	} while ( list_next( relation_mto1_foreign_key_less_equal_list ) );

	return 1;
}

TABLE_EDIT_DOCUMENT *table_edit_document_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *table_edit_state,
		LIST *folder_attribute_append_isa_list,
		char *table_edit_input_onload_javascript_string,
		char *results_string,
		char *error_string,
		char *javascript_filename,
		boolean sort_buttons_boolean,
		char *post_change_javascript,
		MENU *menu,
		ROW_SECURITY *row_security,
		char *table_edit_document_notepad )
{
	TABLE_EDIT_DOCUMENT *table_edit_document;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !table_edit_state
	||   !list_length( folder_attribute_append_isa_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	table_edit_document = table_edit_document_calloc();

	table_edit_document->title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		table_edit_document_title_string(
			folder_name,
			table_edit_state );

	table_edit_document->folder_attribute_calendar_date_name_list_length =
		folder_attribute_calendar_date_name_list_length(
			folder_attribute_append_isa_list );

	table_edit_document->document_head_calendar_setup_string =
		/* ------------------------------ */
		/* Returns program memory or null */
		/* ------------------------------ */
		document_head_calendar_setup_string(
		   table_edit_document->
		      folder_attribute_calendar_date_name_list_length );

	if ( error_string && *error_string ) results_string = NULL;

	table_edit_document->javascript_filename_list =
		table_edit_document_javascript_filename_list(
			javascript_filename,
			sort_buttons_boolean );

	table_edit_document->javascript_include_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		javascript_include_string(
			application_name,
			table_edit_document->javascript_filename_list );

	table_edit_document->javascript_replace =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		table_edit_document_javascript_replace(
			APPASERVER_UPDATE_STATE,
			table_edit_input_onload_javascript_string,
			post_change_javascript );

	table_edit_document->document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_new(
			application_name,
			application_title_string( application_name ),
			table_edit_document->title_string,
			results_string /* sub_title_string */,
			error_string /* sub_sub_title_string */,
			table_edit_document_notepad,
			table_edit_document->javascript_replace
				/* onload_javascript_string */,
			(menu) ? menu->html : (char *)0,
			/* ------------------------------ */
			/* Returns program memory or null */
			/* ------------------------------ */
			document_head_menu_setup_string(
				(menu)
					? 1
					: 0 /* menu_boolean */ ),
			table_edit_document->
				document_head_calendar_setup_string,
			table_edit_document->javascript_include_string );

	table_edit_document->row_security_ajax_client_list =
		row_security_ajax_client_list(
			row_security );

	if ( list_length( table_edit_document->row_security_ajax_client_list ) )
	{
		table_edit_document->ajax_client_list_javascript =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			ajax_client_list_javascript(
				session_key,
				login_name,
				role_name,
				table_edit_document->
					row_security_ajax_client_list );
	}

	table_edit_document->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		table_edit_document_html(
			table_edit_document->document->html,
			table_edit_document->document->document_head->html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_head_close_tag(),
			table_edit_document->document->document_body->html,
			table_edit_document->ajax_client_list_javascript );

	table_edit_document->trailer_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		table_edit_document_trailer_html(
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_body_close_tag(),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_close_tag() );

	return table_edit_document;
}

TABLE_EDIT_DOCUMENT *table_edit_document_calloc( void )
{
	TABLE_EDIT_DOCUMENT *table_edit_document;

	if ( ! ( table_edit_document =
			calloc( 1,
				sizeof ( TABLE_EDIT_DOCUMENT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return table_edit_document;
}

char *table_edit_input_onload_javascript_string(
		const char *onload_javascript_key,
		DICTIONARY *non_prefixed_dictionary )
{
	char *get;

	get =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		dictionary_get(
			(char *)onload_javascript_key,
			non_prefixed_dictionary );

	if ( get )
	{
		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		security_sql_injection_unescape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			get /* datum */ );
	}
	else
	{
		return (char *)0;
	}
}

void table_edit_input_isa_delete_prompt_set(
		char *folder_name,
		LIST *folder_operation_list )
{
	FOLDER_OPERATION *folder_operation;
	char *isa_delete_prompt;

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	isa_delete_prompt =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		table_edit_input_isa_delete_prompt(
			folder_name );

	if ( list_rewind( folder_operation_list ) )
	do {
		folder_operation =
			list_get(
				folder_operation_list );

		if ( !folder_operation->operation
		||   !folder_operation->operation->widget_container
		||   !folder_operation->operation->widget_container->checkbox )
		{
			char message[ 128 ];

			sprintf(message,
			"folder_operation->operation is empty or incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( string_strcmp(
			folder_operation->operation_name,
			"delete_isa_only" ) == 0 )
		{
			folder_operation->
				operation->
				widget_container->
				checkbox->
				prompt = isa_delete_prompt;
		}

	} while ( list_next( folder_operation_list ) );
}

char *table_edit_input_isa_delete_prompt( char *folder_name )
{
	char isa_delete_prompt[ 128 ];

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(isa_delete_prompt,
		"delete %s only",
		folder_name );

	return
	strdup(
		string_initial_capital(
			isa_delete_prompt,
			isa_delete_prompt ) );
}

char *table_edit_document_notepad(
		const char *frameset_prompt_frame,
		char *target_frame,
		char *folder_notepad )
{
	if ( string_strcmp(
		(char *)frameset_prompt_frame,
		target_frame ) == 0 )
	{
		return folder_notepad;
	}

	return NULL;
}

