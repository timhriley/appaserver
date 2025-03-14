/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_prompt_insert.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "environ.h"
#include "dictionary.h"
#include "appaserver_error.h"
#include "operation.h"
#include "list.h"
#include "sql.h"
#include "application.h"
#include "appaserver_parameter.h"
#include "dictionary_separate.h"
#include "pair_one2m.h"
#include "widget.h"
#include "javascript.h"
#include "frameset.h"
#include "query.h"
#include "appaserver.h"
#include "role_folder.h"
#include "post_prompt_insert.h"
#include "form_prompt_insert.h"

FORM_PROMPT_INSERT_ATTRIBUTE *form_prompt_insert_attribute_new(
		char *application_name,
		char *login_name,
		char *post_change_javascript,
		char *attribute_name,
		char *folder_attribute_prompt,
		char *datatype_name,
		int attribute_width,
		char *hint_message,
		int form_prompt_insert_ignore_boolean,
		LIST *form_prompt_insert_relation_list,
		int tab_order )
{
	FORM_PROMPT_INSERT_ATTRIBUTE *form_prompt_insert_attribute;

	if ( form_prompt_insert_relation_attribute_exists(
		attribute_name,
		form_prompt_insert_relation_list ) )
	{
		return NULL;
	}

	form_prompt_insert_attribute = form_prompt_insert_attribute_calloc();

	form_prompt_insert_attribute->widget_container_list = list_new();

	list_set(
		form_prompt_insert_attribute->widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	if ( form_prompt_insert_ignore_boolean )
	{
		list_set(
			form_prompt_insert_attribute->widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

		list_set(
			form_prompt_insert_attribute->widget_container_list,
			( form_prompt_insert_attribute->
			    ignore_widget_container =
				widget_container_new(
					checkbox,
					/* ------------------- */
					/* Returns heap memory */
					/* ------------------- */
					form_prefix_widget_name(
					     DICTIONARY_SEPARATE_IGNORE_PREFIX,
					     attribute_name ) ) ) );

		form_prompt_insert_attribute->
			ignore_widget_container->
			checkbox->
			prompt = FORM_PROMPT_INSERT_IGNORE_PROMPT;

		form_prompt_insert_attribute->
			ignore_widget_container->
			recall_boolean = 1;
	}

	/* Empty cell for "New" checkbox */
	/* ----------------------------- */ 
	list_set(
		form_prompt_insert_attribute->widget_container_list,
		widget_container_new( table_data, (char *)0 ) );

	list_set(
		form_prompt_insert_attribute->widget_container_list,
		widget_container_new( table_data, (char *)0 ) );

	list_set(
		form_prompt_insert_attribute->widget_container_list,
		( form_prompt_insert_attribute->prompt_widget_container =
			widget_container_new(
				non_edit_text,
				folder_attribute_prompt
					/* widget_name */ ) ) );

	list_set(
		form_prompt_insert_attribute->widget_container_list,
		widget_container_new( table_data, (char *)0 ) );

	form_prompt_insert_attribute->attribute_is_notepad =
		attribute_is_notepad(
			ATTRIBUTE_DATATYPE_NOTEPAD,
			datatype_name );

	if ( form_prompt_insert_attribute->attribute_is_notepad )
	{
		list_set(
			form_prompt_insert_attribute->widget_container_list,
			( form_prompt_insert_attribute->widget_container =
				widget_container_new(
					notepad,
					attribute_name
						/* widget_name */ ) ) );

		form_prompt_insert_attribute->
			widget_container->
			notepad->
			attribute_size = attribute_width;

		form_prompt_insert_attribute->
			widget_container->
			notepad->
			tab_order = tab_order;

		form_prompt_insert_attribute->
			widget_container->
			notepad->
			post_change_javascript = post_change_javascript;
	}
	else
	if ( attribute_is_yes_no( attribute_name ) )
	{
		list_set(
			form_prompt_insert_attribute->widget_container_list,
			( form_prompt_insert_attribute->
				widget_container =
					widget_container_new(
						yes_no,
						attribute_name
							/* widget_name */ ) ) );

		form_prompt_insert_attribute->
			widget_container->
			yes_no->
			tab_order = tab_order;

		form_prompt_insert_attribute->
			widget_container->
			yes_no->
			post_change_javascript = post_change_javascript;
	}
	else
	if ( attribute_is_date( datatype_name )
	||   attribute_is_date_time( datatype_name )
	||   attribute_is_current_date_time( datatype_name ) )
	{
		list_set(
			form_prompt_insert_attribute->widget_container_list,
			( form_prompt_insert_attribute->
				widget_container =
					widget_container_new(
						widget_date,
						attribute_name
							/* widget_name */ ) ) );

		form_prompt_insert_attribute->
			widget_container->
			date->
			datatype_name = datatype_name;

		form_prompt_insert_attribute->
			widget_container->
			date->
			attribute_width_max_length = attribute_width;

		form_prompt_insert_attribute->
			widget_container->
			date->
			display_size = attribute_width;

		form_prompt_insert_attribute->
			widget_container->
			date->
			tab_order = tab_order;

		form_prompt_insert_attribute->
			widget_container->
			date->
			application_name = application_name;

		form_prompt_insert_attribute->
			widget_container->
			date->
			login_name = login_name;

		form_prompt_insert_attribute->
			widget_container->
			date->
			post_change_javascript = post_change_javascript;
	}
	else
	if ( attribute_is_time( datatype_name ) )
	{
		list_set(
			form_prompt_insert_attribute->widget_container_list,
			( form_prompt_insert_attribute->
				widget_container =
					widget_container_new(
						widget_time,
						attribute_name
							/* widget_name */ ) ) );

		form_prompt_insert_attribute->
			widget_container->
			time->
			attribute_width_max_length =
				attribute_width;

		form_prompt_insert_attribute->
			widget_container->
			time->
			widget_text_display_size =
				attribute_width;

		form_prompt_insert_attribute->
			widget_container->
			time->
			tab_order = tab_order;

		form_prompt_insert_attribute->
			widget_container->
			time->
			post_change_javascript = post_change_javascript;
	}
	else
	{
		list_set(
			form_prompt_insert_attribute->widget_container_list,
			( form_prompt_insert_attribute->
				widget_container =
					widget_container_new(
						character,
						attribute_name
							/* widget_name */ ) ) );

		form_prompt_insert_attribute->
			widget_container->
			text->
			datatype_name = datatype_name;

		form_prompt_insert_attribute->
			widget_container->
			text->
			attribute_width_max_length = attribute_width;

		form_prompt_insert_attribute->
			widget_container->
			text->
			widget_text_display_size =
				widget_text_display_size(
					WIDGET_TEXT_DEFAULT_MAX_LENGTH,
					attribute_width );

		form_prompt_insert_attribute->
			widget_container->
			text->
			tab_order = tab_order;

		form_prompt_insert_attribute->
			widget_container->
			text->
			post_change_javascript = post_change_javascript;
	}

	form_prompt_insert_attribute->attribute_is_password =
		attribute_is_password(
			datatype_name );

	if ( !form_prompt_insert_attribute->attribute_is_notepad
	&&   !form_prompt_insert_attribute->attribute_is_password )
	{
		form_prompt_insert_attribute->
			widget_container->
			recall_boolean = 1;
	}

	if ( hint_message )
	{
		list_set(
			form_prompt_insert_attribute->widget_container_list,
			widget_container_new( table_data, (char *)0 ) );

		list_set(
			form_prompt_insert_attribute->widget_container_list,
			( form_prompt_insert_attribute->
				hint_message_widget_container =
					widget_container_new(
						non_edit_text,
						hint_message
							/* widget_name */ ) ) );

		form_prompt_insert_attribute->
			hint_message_widget_container->
			non_edit_text->
			no_initial_capital_boolean = 1;
	}

	return form_prompt_insert_attribute;
}

FORM_PROMPT_INSERT_ATTRIBUTE *form_prompt_insert_attribute_calloc( void )
{
	FORM_PROMPT_INSERT_ATTRIBUTE *form_prompt_insert_attribute;

	if ( ! ( form_prompt_insert_attribute =
			calloc( 1,
				sizeof ( FORM_PROMPT_INSERT_ATTRIBUTE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_insert_attribute;
}

boolean form_prompt_insert_relation_attribute_exists(
		char *attribute_name,
		LIST *form_prompt_insert_relation_list )
{
	FORM_PROMPT_INSERT_RELATION *form_prompt_insert_relation;

	if ( !list_rewind( form_prompt_insert_relation_list ) ) return 0;

	do {
		form_prompt_insert_relation =
			list_get(
				form_prompt_insert_relation_list );

		if ( !form_prompt_insert_relation->relation_mto1 )
		{
			char message[ 128 ];

			sprintf(message,
		"form_prompt_insert_relation->relation_mto1 is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !list_length(
			form_prompt_insert_relation->
				relation_mto1->
				relation_foreign_key_list ) )
		{
			char message[ 128 ];

			sprintf(message,
			"relation_mto1->relation_foreign_key_list is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( list_string_exists(
			attribute_name,
			form_prompt_insert_relation->
				relation_mto1->
				relation_foreign_key_list ) )
		{
			return 1;
		}

	} while ( list_next( form_prompt_insert_relation_list ) );

	return 0;
}

FORM_PROMPT_INSERT_RELATION *form_prompt_insert_relation_calloc( void )
{
	FORM_PROMPT_INSERT_RELATION *form_prompt_insert_relation;

	if ( ! ( form_prompt_insert_relation =
			calloc( 1,
				sizeof ( FORM_PROMPT_INSERT_RELATION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_insert_relation;
}

FORM_PROMPT_INSERT_WIDGET_LIST *
	form_prompt_insert_widget_list_new(
		const char *prompt_insert_lookup_checkbox,
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *post_change_javascript,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		DICTIONARY *drillthru_dictionary,
		SECURITY_ENTITY *security_entity,
		LIST *folder_attribute_append_isa_list,
		boolean role_folder_lookup_boolean,
		boolean form_prompt_insert_ignore_boolean )
{
	int tab_order = 0;
	FORM_PROMPT_INSERT_WIDGET_LIST *form_prompt_insert_widget_list;
	FOLDER_ATTRIBUTE *folder_attribute;
	FORM_PROMPT_INSERT_RELATION *form_prompt_insert_relation;
	WIDGET_CONTAINER *widget_container;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	if ( !list_rewind( folder_attribute_append_isa_list ) )
	{
		char message[ 128 ];

		sprintf(message, "folder_attribute_append_isa_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_prompt_insert_widget_list =
		form_prompt_insert_widget_list_calloc();

	form_prompt_insert_widget_list->
		role_folder_insert_list =
			role_folder_insert_list(
				role_name );

	form_prompt_insert_widget_list->widget_container_list = list_new();

	list_set(
		form_prompt_insert_widget_list->widget_container_list,
		( widget_container =
			widget_container_new(
				table_open, (char *)0 ) ) );

	widget_container->table_open->border_boolean = 1;

	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		if (	folder_attribute->omit_insert
		||	folder_attribute->omit_insert_prompt )
		{
			continue;
		}

		++tab_order;

		if ( ( form_prompt_insert_relation =
			form_prompt_insert_relation_new(
				application_name,
				login_name,
				role_name,
				folder_name
					/* many_folder_name */,
				post_change_javascript,
				relation_mto1_list
					/* many_folder_relation_mto1_list */,
				relation_mto1_isa_list,
				drillthru_dictionary,
				security_entity,
				form_prompt_insert_widget_list->
					role_folder_insert_list,
				form_prompt_insert_widget_list->
				      form_prompt_insert_relation_list,
				folder_attribute->attribute_name,
				form_prompt_insert_ignore_boolean,
				tab_order ) ) )
		{
			if ( !form_prompt_insert_widget_list->
				form_prompt_insert_relation_list )
			{
				form_prompt_insert_widget_list->
					form_prompt_insert_relation_list =
						list_new();
			}

			list_set(
				form_prompt_insert_widget_list->
					form_prompt_insert_relation_list,
				form_prompt_insert_relation );

			list_set_list(
				form_prompt_insert_widget_list->
					widget_container_list,
				form_prompt_insert_relation->
					widget_container_list );

			continue;
		}

		form_prompt_insert_widget_list->
		    form_prompt_insert_attribute =
			form_prompt_insert_attribute_new(
				application_name,
				login_name,
				post_change_javascript,
				folder_attribute->attribute_name,
				folder_attribute->prompt
				     /* folder_attribute_prompt */,
				folder_attribute->
					attribute->
					datatype_name,
				folder_attribute->
					attribute->
					width,
				folder_attribute->
					attribute->
					hint_message,
				form_prompt_insert_ignore_boolean,
				form_prompt_insert_widget_list->
				      form_prompt_insert_relation_list,
				tab_order );

		if ( !form_prompt_insert_widget_list->
			form_prompt_insert_attribute )
		{
			continue;
		}

		list_set_list(
			form_prompt_insert_widget_list->
				widget_container_list,
			form_prompt_insert_widget_list->
				form_prompt_insert_attribute->
				widget_container_list );

	} while ( list_next( folder_attribute_append_isa_list ) );

	if ( role_folder_lookup_boolean )
	{
		list_set_list(
			form_prompt_insert_widget_list->widget_container_list,
			form_prompt_insert_widget_list_lookup_list(
				prompt_insert_lookup_checkbox ) );
	}

	list_set(
		form_prompt_insert_widget_list->widget_container_list,
		widget_container_new(
			table_close, (char *)0 ) );

	form_prompt_insert_widget_list->widget_hidden_container_list =
		widget_hidden_container_list(
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			drillthru_dictionary );

	list_set_list(
		form_prompt_insert_widget_list->widget_container_list,
		form_prompt_insert_widget_list->
			widget_hidden_container_list );

	return form_prompt_insert_widget_list;
}

FORM_PROMPT_INSERT_WIDGET_LIST *
	form_prompt_insert_widget_list_calloc(
		void )
{
	FORM_PROMPT_INSERT_WIDGET_LIST *form_prompt_insert_widget_list;

	if ( ! ( form_prompt_insert_widget_list =
			calloc( 1,
				sizeof ( FORM_PROMPT_INSERT_WIDGET_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_insert_widget_list;
}

FORM_PROMPT_INSERT *form_prompt_insert_new(
		const char *prompt_insert_lookup_checkbox,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *post_change_javascript,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *drillthru_dictionary,
		SECURITY_ENTITY *security_entity,
		boolean role_folder_lookup_boolean,
		LIST *relation_one2m_pair_list,
		char *post_choose_folder_action_string )
{
	FORM_PROMPT_INSERT *form_prompt_insert;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( folder_attribute_append_isa_list ) )
	{
		char message[ 128 ];

		sprintf(message, "folder_attribute_append_isa_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_prompt_insert = form_prompt_insert_calloc();

	form_prompt_insert->action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_prompt_insert_action_string(
			POST_PROMPT_INSERT_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name );

	form_prompt_insert->form_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_tag(
			FORM_PROMPT_INSERT_NAME,
			form_prompt_insert->action_string,
			FRAMESET_TABLE_FRAME );

	form_prompt_insert->relation_one2m_pair_list_length =
		list_length(
			relation_one2m_pair_list );

	form_prompt_insert->ignore_boolean =
		form_prompt_insert_ignore_boolean(
			form_prompt_insert->
				relation_one2m_pair_list_length );

	form_prompt_insert->form_prompt_insert_widget_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_prompt_insert_widget_list_new(
			prompt_insert_lookup_checkbox,
			application_name,
			login_name,
			role_name,
			folder_name,
			post_change_javascript,
			relation_mto1_list,
			relation_mto1_isa_list,
			drillthru_dictionary,
			security_entity,
			folder_attribute_append_isa_list,
			role_folder_lookup_boolean,
			form_prompt_insert->ignore_boolean );

	form_prompt_insert->form_multi_select_all_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		form_multi_select_all_javascript(
			form_prompt_insert->
				form_prompt_insert_widget_list->
				widget_container_list );

	form_prompt_insert->form_verify_notepad_widths_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		form_verify_notepad_widths_javascript(
			FORM_PROMPT_INSERT_NAME,
			form_prompt_insert->
				form_prompt_insert_widget_list->
				widget_container_list );

	form_prompt_insert->recall_save =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		recall_save_new(
			APPASERVER_INSERT_STATE,
			FORM_PROMPT_INSERT_NAME,
			folder_name,
			form_prompt_insert->
				form_prompt_insert_widget_list->
				widget_container_list,
			application_ssl_support_boolean(
				application_name ) );

	form_prompt_insert->recall_load =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		recall_load_new(
			APPASERVER_INSERT_STATE,
			FORM_PROMPT_INSERT_NAME,
			folder_name,
			form_prompt_insert->
				form_prompt_insert_widget_list->
				widget_container_list );

	if ( list_length(
		form_prompt_insert->
			form_prompt_insert_widget_list->
			form_prompt_insert_relation_list ) )
	{
		form_prompt_insert->widget_checkbox_submit_javascript =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			widget_checkbox_submit_javascript(
				FORM_PROMPT_INSERT_NAME,
				form_prompt_insert->recall_save->javascript );

		form_prompt_insert_relation_javascript_save_set(
			form_prompt_insert->
				form_prompt_insert_widget_list->
				form_prompt_insert_relation_list,
			form_prompt_insert->
				widget_checkbox_submit_javascript );
	}

	if ( form_prompt_insert->relation_one2m_pair_list_length )
	{
		form_prompt_insert->pair_one2m_prompt_insert =
			pair_one2m_prompt_insert_new(
				DICTIONARY_SEPARATE_PAIR_PREFIX,
				PAIR_ONE2M_ONE_FOLDER_KEY,
				PAIR_ONE2M_MANY_FOLDER_KEY,
				PAIR_ONE2M_UNFULFILLED_LIST_KEY,
				PAIR_ONE2M_FOLDER_DELIMITER,
				folder_name /* one_folder_name */,
				form_prompt_insert->recall_save->javascript,
				relation_one2m_pair_list );

		list_set(
			form_prompt_insert->
				form_prompt_insert_widget_list->
				widget_container_list,
			form_prompt_insert->
				pair_one2m_prompt_insert->
				hidden_widget );
	}

	form_prompt_insert->button_container_list =
		form_prompt_insert_button_container_list(
			post_choose_folder_action_string,
			form_prompt_insert->form_multi_select_all_javascript,
			form_prompt_insert->recall_save->javascript,
			form_prompt_insert->recall_load->javascript,
			form_prompt_insert->
				form_verify_notepad_widths_javascript,
			(form_prompt_insert->pair_one2m_prompt_insert)
				? form_prompt_insert->
					pair_one2m_prompt_insert->
					pair_one2m_button_list
				: (LIST *)0 );

	form_prompt_insert->button_list_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_prompt_insert_button_list_html(
			form_prompt_insert->button_container_list );

	form_prompt_insert->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_prompt_insert_html(
			session_key,
			login_name,
			role_name,
			folder_name,
			form_prompt_insert->
				form_prompt_insert_widget_list->
				form_prompt_insert_relation_list,
			form_prompt_insert->form_tag,
			form_prompt_insert->
				form_prompt_insert_widget_list->
				widget_container_list,
			form_prompt_insert->button_list_html,
			(form_prompt_insert->pair_one2m_prompt_insert)
				? form_prompt_insert->
					pair_one2m_prompt_insert->
					pair_one2m_dictionary
				: (DICTIONARY *)0,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_tag() );

	free( form_prompt_insert->button_list_html );

	return form_prompt_insert;
}

FORM_PROMPT_INSERT *form_prompt_insert_calloc( void )
{
	FORM_PROMPT_INSERT *form_prompt_insert;

	if ( ! ( form_prompt_insert =
			calloc( 1, sizeof ( FORM_PROMPT_INSERT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_insert;
}

char *form_prompt_insert_action_string(
		const char *post_prompt_insert_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name )
{
	char action_string[ 1024 ];

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(action_string,
		"%s/%s?%s+%s+%s+%s+%s",
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
		post_prompt_insert_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		folder_name );

	return strdup( action_string );
}

LIST *form_prompt_insert_button_container_list(
		char *post_choose_folder_action_string,
		char *form_multi_select_all_javascript,
		char *recall_save_javascript,
		char *recall_load_javascript,
		char *form_verify_notepad_widths_javascript,
		LIST *pair_one2m_button_list )
{
	LIST *button_container_list;
	WIDGET_CONTAINER *widget_container;
	PAIR_ONE2M_BUTTON *pair_one2m_button;

	if ( !recall_save_javascript
	||   !recall_load_javascript )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	button_container_list = list_new();

	list_set(
		button_container_list,
		widget_container_new(
			table_open, (char *)0 ) );

	list_set(
		button_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	list_set(
		button_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		button_container_list,
		( widget_container =
			widget_container_new(
				button, (char *)0 ) ) );

	widget_container->widget_button->button =
		button_submit(
			form_multi_select_all_javascript,
			recall_save_javascript,
			form_verify_notepad_widths_javascript,
			0 /* form_number */ );

	if ( list_rewind( pair_one2m_button_list ) )
	do {
		pair_one2m_button =
			list_get(
				pair_one2m_button_list );

		list_set(
			button_container_list,
			pair_one2m_button->widget_container );

	} while ( list_next( pair_one2m_button_list ) );

	list_set(
		button_container_list,
		( widget_container =
			widget_container_new(
				button, (char *)0 ) ) );

	widget_container->widget_button->button =
		button_reset(
			(char *)0 /* javascript_replace */,
			0 /* form_number */ );

	list_set(
		button_container_list,
		( widget_container =
			widget_container_new(
				button, (char *)0 ) ) );

	widget_container->widget_button->button = button_back();

	list_set(
		button_container_list,
		( widget_container =
			widget_container_new(
				button, (char *)0 ) ) );

	widget_container->widget_button->button = button_forward();

	widget_container =
		widget_container_new(
			button, (char *)0 );

	widget_container->widget_button->button =
		button_recall(
			recall_load_javascript );

	if ( widget_container->widget_button->button )
	{
		list_set(
			button_container_list,
			widget_container );
	}

	if ( post_choose_folder_action_string )
	{
		list_set(
			button_container_list,
			( widget_container =
			  	widget_container_new(
					button, (char *)0 ) ) );

		widget_container->widget_button->button =
			button_restart_drillthru(
				post_choose_folder_action_string );
	}

	list_set(
		button_container_list,
		( widget_container =
			widget_container_new(
				button, (char *)0 ) ) );

	widget_container->widget_button->button = button_to_top();

	list_set(
		button_container_list,
		widget_container_new(
			table_close, (char *)0 ) );

	return button_container_list;
}

char *form_prompt_insert_html(
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *form_prompt_insert_relation_list,
		char *form_tag,
		LIST *widget_container_list,
		char *button_container_list_html,
		DICTIONARY *pair_one2m_dictionary,
		char *form_close_tag )
{
	char html[ STRING_704K ];
	char *ptr = html;
	LIST *ajax_client_list;
	char *javascript;
	char *widget_html;
	char *capacity_message;

	if ( !session_key
	||   !login_name
	||   !role_name
	||   !form_tag
	||   !list_length( widget_container_list )
	||   !button_container_list_html
	||   !form_close_tag )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	*ptr = '\0';

	ajax_client_list =
		form_prompt_insert_relation_ajax_client_list(
			form_prompt_insert_relation_list );

	if ( list_length( ajax_client_list ) )
	{
		javascript =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			ajax_client_list_javascript(
				session_key,
				login_name,
				role_name,
				ajax_client_list );

		if ( javascript )
		{
			ptr += sprintf(
				ptr,
				"%s\n",
				javascript );

			free( javascript );
		}
	}

	ptr += sprintf(
		ptr,
		"%s\n",
		form_tag );

	widget_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_container_list_html(
			APPASERVER_INSERT_STATE,
			JAVASCRIPT_PROMPT_ROW_NUMBER
				/* probably 0 */,
			(char *)0 /* background_color */,
			widget_container_list );

	if (	strlen( html ) +
		strlen( widget_html ) + 1 >= STRING_704K )
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

	ptr += sprintf(
		ptr,
		"%s\n",
		widget_html );

	free( widget_html );

	widget_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		widget_container_list_hidden_html(
			JAVASCRIPT_PROMPT_ROW_NUMBER
				/* probably 0 */,
			widget_container_list );

	if ( widget_html )
	{
		if (	strlen( html ) +
			strlen( widget_html ) + 1 >= STRING_704K )
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

		ptr += sprintf(
			ptr,
			"%s\n",
			widget_html );

		free( widget_html );
	}

	if ( dictionary_length( pair_one2m_dictionary ) )
	{
		widget_html =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			dictionary_separate_hidden_html(
				DICTIONARY_SEPARATE_PAIR_PREFIX,
				pair_one2m_dictionary );

		if ( !widget_html )
		{
			char message[ 128 ];

			sprintf(message,
			"dictionary_separate_hidden_html() returned empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if (	strlen( html ) +
			strlen( widget_html ) +
			1 >= STRING_704K )
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

		ptr += sprintf(
			ptr,
			"%s\n",
			widget_html );

		free( widget_html );
	}

	if (	strlen( html ) +
		strlen( button_container_list_html ) +
		strlen( form_close_tag ) + 2 >= STRING_704K )
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

	ptr += sprintf(
		ptr,
		"%s\n%s",
		button_container_list_html,
		form_close_tag );

	capacity_message =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		form_prompt_insert_capacity_message(
			__FUNCTION__ /* function_name */,
			STRING_704K /* max_strlen */,
			APPASERVER_CAPACITY_THRESHOLD,
			folder_name,
			strlen( html ) /* strlen_html */ );

	if ( capacity_message )
	{
		appaserver_error_message_file(
			(char *)0 /* application_name */,
			(char *)0 /* login_name */,
			capacity_message );

		free( capacity_message );
	}

	return strdup( html );
}

char *form_prompt_insert_capacity_message(
		const char *function_name,
		const int max_strlen,
		const double appaserver_capacity_threshold,
		char *folder_name,
		int strlen_html )
{
	char capacity_message[ 256 ];
	double capacity;

	capacity =
		( (double)strlen_html /
	  	(double)max_strlen ) * 100.0;

	if ( capacity < appaserver_capacity_threshold ) return NULL;

	sprintf(capacity_message,
"%s(): For folder_name=%s, returning length=%d, which is capacity=%.0lf%c",
		function_name,
		folder_name,
		strlen_html,
		capacity,
		'%' );

	return strdup( capacity_message );
}

FORM_PROMPT_INSERT_RELATION *form_prompt_insert_relation_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *many_folder_name,
		char *post_change_javascript,
		LIST *many_folder_relation_mto1_list,
		LIST *relation_mto1_isa_list,
		DICTIONARY *drillthru_dictionary,
		SECURITY_ENTITY *security_entity,
		LIST *role_folder_insert_list,
		LIST *form_prompt_insert_relation_list,
		char *attribute_name,
		boolean form_prompt_insert_ignore_boolean,
		int tab_order )
{
	FORM_PROMPT_INSERT_RELATION *form_prompt_insert_relation;

	if ( form_prompt_insert_relation_attribute_exists(
		attribute_name,
		form_prompt_insert_relation_list ) )
	{
		return NULL;
	}

	form_prompt_insert_relation = form_prompt_insert_relation_calloc();

	form_prompt_insert_relation->relation_mto1 =
		relation_mto1_consumes(
			attribute_name,
			many_folder_relation_mto1_list );

	if ( !form_prompt_insert_relation->relation_mto1 )
	{
		form_prompt_insert_relation->relation_mto1 =
			relation_mto1_isa_consumes(
				attribute_name,
				relation_mto1_isa_list );

		if ( !form_prompt_insert_relation->relation_mto1 )
		{
			free( form_prompt_insert_relation );
			return NULL;
		}
	}

	if ( !form_prompt_insert_relation->relation_mto1->one_folder )
	{
		char message[ 128 ];

		sprintf(message, "relation_mto1->one_folder is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length(
		form_prompt_insert_relation->
			relation_mto1->
			one_folder->
			folder_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"one_folder->folder_attribute_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length(
		form_prompt_insert_relation->
			relation_mto1->
			relation_foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message,
		"for one_folder=%s, relation_foreign_key_list is empty.",
			form_prompt_insert_relation->
				relation_mto1->
				one_folder->
				folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_prompt_insert_relation->widget_container_list = list_new();

	list_set(
		form_prompt_insert_relation->
			widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	if ( form_prompt_insert_ignore_boolean )
	{
		list_set(
			form_prompt_insert_relation->
				widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

		list_set(
			form_prompt_insert_relation->
				widget_container_list,
				( form_prompt_insert_relation->
					ignore_widget_container =
					widget_container_new(
					   checkbox,
					   form_prefix_widget_name(
					      DICTIONARY_SEPARATE_IGNORE_PREFIX,
					      form_prompt_insert_relation->
							relation_mto1->
							relation_name ) ) ) );

		form_prompt_insert_relation->
			ignore_widget_container->
			checkbox->
			prompt = FORM_PROMPT_INSERT_IGNORE_PROMPT;

		form_prompt_insert_relation->
			ignore_widget_container->
			recall_boolean = 1;
	}

	list_set(
		form_prompt_insert_relation->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	form_prompt_insert_relation->role_folder_insert_boolean =
		role_folder_insert_boolean(
			ROLE_PERMISSION_INSERT,
			form_prompt_insert_relation->
				relation_mto1->
				one_folder_name,
			role_folder_insert_list );

	if ( form_prompt_insert_relation->role_folder_insert_boolean )
	{
		form_prompt_insert_relation->vertical_new_checkbox =
			vertical_new_checkbox_new(
				VERTICAL_NEW_CHECKBOX_PREFIX,
				VERTICAL_NEW_CHECKBOX_PROMPT,
				form_prompt_insert_relation->
					relation_mto1->
					one_folder_name );

		list_set(
			form_prompt_insert_relation->widget_container_list,
			form_prompt_insert_relation->
				vertical_new_checkbox->
				widget_container );
	}

	list_set(
		form_prompt_insert_relation->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		form_prompt_insert_relation->widget_container_list,
		( form_prompt_insert_relation->prompt_widget_container =
			widget_container_new(
				non_edit_text,
				form_prompt_insert_relation->
					relation_mto1->
					relation_prompt ) ) );

	list_set(
		form_prompt_insert_relation->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	form_prompt_insert_relation->relation_mto1_to_one_fetch_list =
		relation_mto1_to_one_fetch_list(
			role_name,
			form_prompt_insert_relation->
				relation_mto1->
				one_folder_name,
			form_prompt_insert_relation->
				relation_mto1->
				one_folder->
				folder_attribute_primary_key_list );

	if ( !form_prompt_insert_relation->
		relation_mto1->
		relation->
		omit_ajax_fill_drop_down )
	{
		form_prompt_insert_relation->ajax_client =
			/* --------------------------------- */
			/* Returns null if not participating */
			/* --------------------------------- */
			ajax_client_relation_mto1_new(
				form_prompt_insert_relation->
					relation_mto1,
				form_prompt_insert_relation->
					relation_mto1_to_one_fetch_list,
				1 /* top_select_boolean */ );
	}

	if ( form_prompt_insert_relation->ajax_client )
	{
		form_prompt_insert_relation->drop_down_widget_container =
			list_first(
				form_prompt_insert_relation->
					ajax_client->
					widget_container_list );

		if ( !form_prompt_insert_relation->drop_down_widget_container )
		{
			char message[ 128 ];

			sprintf(message,
	"list_first(ajax_client->widget_container_list) returned empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		form_prompt_insert_relation->
			drop_down_widget_container->
			drop_down->
			tab_order = tab_order;

		form_prompt_insert_relation->
			drop_down_widget_container->
			drop_down->
			post_change_javascript = post_change_javascript;

		form_prompt_insert_relation->
			drop_down_widget_container->
			recall_boolean = 1;

		list_set_list(
			form_prompt_insert_relation->
				widget_container_list,
			form_prompt_insert_relation->
				ajax_client->
				widget_container_list );

		goto form_prompt_insert_relation_hint_message;
	}

	list_set(
		form_prompt_insert_relation->widget_container_list,
		( form_prompt_insert_relation->drop_down_widget_container =
			widget_container_new(
				drop_down,
				form_prompt_insert_relation->
					relation_mto1->
					relation_name
					/* widget_name */ ) ) );

	form_prompt_insert_relation->query_drop_down =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_drop_down_new(
			application_name,
			login_name,
			many_folder_name,
			form_prompt_insert_relation->
				relation_mto1->
				one_folder_name,
			APPASERVER_INSERT_STATE,
			form_prompt_insert_relation->
				relation_mto1->
				one_folder->
				folder_attribute_list,
			form_prompt_insert_relation->
				relation_mto1->
				one_folder->
				populate_drop_down_process_name,
			form_prompt_insert_relation->
				relation_mto1_to_one_fetch_list,
			drillthru_dictionary
				/* drop_down_dictionary */,
			security_entity );

	form_prompt_insert_relation->
		drop_down_widget_container->
		drop_down->
		widget_drop_down_option_list =
			widget_drop_down_option_list(
				ATTRIBUTE_MULTI_KEY_DELIMITER,
				WIDGET_DROP_DOWN_LABEL_DELIMITER,
				WIDGET_DROP_DOWN_EXTRA_DELIMITER,
				WIDGET_DROP_DOWN_DASH_DELIMITER,
				form_prompt_insert_relation->
					query_drop_down->delimited_list,
				form_prompt_insert_relation->
					relation_mto1->
						one_folder->
						no_initial_capital );

	form_prompt_insert_relation->
		drop_down_widget_container->
		drop_down->
		null_boolean = 1;

	form_prompt_insert_relation->
		drop_down_widget_container->
		drop_down->
		top_select_boolean = 1;

	form_prompt_insert_relation->
		drop_down_widget_container->
		drop_down->
		display_size = 1;

	form_prompt_insert_relation->
		drop_down_widget_container->
		drop_down->
		tab_order = tab_order;

	form_prompt_insert_relation->
		drop_down_widget_container->
		drop_down->
		post_change_javascript = post_change_javascript;

	form_prompt_insert_relation->
		drop_down_widget_container->
		recall_boolean = 1;

form_prompt_insert_relation_hint_message:

	if ( form_prompt_insert_relation->
		relation_mto1->
		relation->
		hint_message )
	{
		list_set(
			form_prompt_insert_relation->widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

		list_set(
			form_prompt_insert_relation->widget_container_list,
			( form_prompt_insert_relation->
				hint_message_widget_container =
					widget_container_new(
						non_edit_text,
						form_prompt_insert_relation->
							relation_mto1->
							relation->
							hint_message ) ) );

		form_prompt_insert_relation->
			hint_message_widget_container->
			non_edit_text->
			no_initial_capital_boolean = 1;
	}

	return form_prompt_insert_relation;
}

LIST *form_prompt_insert_widget_list_lookup_list(
		const char *prompt_insert_lookup_checkbox )
{
	LIST *widget_container_list;
	WIDGET_CONTAINER *widget_container;

	widget_container_list = list_new();

	list_set(
		widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	list_set(
		widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		widget_container_list,
		( widget_container =
			widget_container_new(
				checkbox,
				(char *)prompt_insert_lookup_checkbox ) ) );

	widget_container->checkbox->prompt = "Lookup";

	return widget_container_list;
}

char *form_prompt_insert_button_list_html(
		LIST *button_container_list )
{
	char *html;

	html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_container_list_html(
			APPASERVER_INSERT_STATE,
			JAVASCRIPT_PROMPT_ROW_NUMBER
				/* probably 0 */,
			(char *)0 /* background_color */,
			button_container_list );

	return html;
}

void form_prompt_insert_relation_javascript_save_set(
		LIST *form_prompt_insert_relation_list,
		char *widget_checkbox_submit_javascript )
{
	FORM_PROMPT_INSERT_RELATION *form_prompt_insert_relation;

	if ( !list_rewind( form_prompt_insert_relation_list ) ) return;

	do {
		form_prompt_insert_relation =
			list_get(
				form_prompt_insert_relation_list );

		if ( form_prompt_insert_relation->vertical_new_checkbox )
		{
			form_prompt_insert_relation->
				vertical_new_checkbox->
				widget_container->
				checkbox->
				onclick =
					widget_checkbox_submit_javascript;
		}

	} while ( list_next( form_prompt_insert_relation_list ) );
}

boolean form_prompt_insert_ignore_boolean(
		int relation_one2m_pair_list_length )
{
	return (relation_one2m_pair_list_length == 0);
}

LIST *form_prompt_insert_relation_ajax_client_list(
		LIST *form_prompt_insert_relation_list )
{
	FORM_PROMPT_INSERT_RELATION *form_prompt_insert_relation;
	LIST *ajax_client_list = {0};

	if ( list_rewind( form_prompt_insert_relation_list ) )
	do {
		form_prompt_insert_relation =
			list_get(
				form_prompt_insert_relation_list );

		if ( form_prompt_insert_relation->ajax_client )
		{
			if ( !ajax_client_list ) ajax_client_list = list_new();

			list_set(
				ajax_client_list,
				form_prompt_insert_relation->ajax_client );
		}

	} while ( list_next( form_prompt_insert_relation_list ) );

	return ajax_client_list;
}

