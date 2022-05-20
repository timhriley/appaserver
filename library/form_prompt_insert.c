/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_prompt_insert.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
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
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_parameter.h"
#include "application_constants.h"
#include "dictionary_separate.h"
#include "pair_one2m.h"
#include "element.h"
#include "javascript.h"
#include "frameset.h"
#include "button.h"
#include "query.h"
#include "appaserver.h"
#include "prompt_insert.h"
#include "post_prompt_insert.h"
#include "form_prompt_insert.h"

FORM_PROMPT_INSERT_ATTRIBUTE *form_prompt_insert_attribute_new(
			char *attribute_name,
			char *folder_attribute_prompt,
			char *datatype_name,
			int attribute_width,
			char *hint_message,
			LIST *form_prompt_insert_relation_list,
			int tab_order )
{
	FORM_PROMPT_INSERT_ATTRIBUTE *form_prompt_insert_attribute;

	if ( form_prompt_insert_relation_attribute_name_exists(
		attribute_name,
		form_prompt_insert_relation_list ) )
	{
		return (FORM_PROMPT_INSERT_ATTRIBUTE *)0;
	}

	form_prompt_insert_attribute = form_prompt_insert_attribute_calloc();

	form_prompt_insert_attribute->element_list = list_new();

	form_prompt_insert_attribute->ignore_name =
		form_prompt_insert_attribute_ignore_name(
			FORM_IGNORE_PREFIX,
			attribute_name );

	list_set(
		form_prompt_insert_attribute->element_list,
		( form_prompt_insert_attribute->
			ignore_appaserver_element =
				appaserver_element_new(
					checkbox,
					form_prompt_insert_attribute->
						ignore_name ) ) );

	free( form_prompt_insert_attribute->
			ignore_appaserver_element->
			checkbox );

	form_prompt_insert_attribute->
		ignore_appaserver_element->
		checkbox =
			element_checkbox_new(
				(char *)0 /* attribute_name */,
				form_prompt_insert_attribute->
					ignore_appaserver_element->
					element_name,
				FORM_IGNORE_PUSH_BUTTON_HEADING
					/* prompt_string */,
				(char *)0 /* on_click */,
				-1 /* tab_order */,
				(char *)0 /* image_source */,
				1 /* recall */ );

	list_set(
		form_prompt_insert_attribute->element_list,
		appaserver_element_new( table_data, (char *)0 ) );

	list_set(
		form_prompt_insert_attribute->element_list,
		( form_prompt_insert_attribute->prompt_appaserver_element =
			appaserver_element_new(
				non_edit_text, (char *)0 ) ) );

	free( form_prompt_insert_attribute->
			prompt_appaserver_element->
			non_edit_text );

	form_prompt_insert_attribute->prompt_appaserver_element->non_edit_text =
		element_non_edit_text_new(
			(char *)0,
			folder_attribute_prompt );

	list_set(
		form_prompt_insert_attribute->element_list,
		appaserver_element_new( table_data, (char *)0 ) );

	if ( attribute_is_yes_no( attribute_name ) )
	{
		list_set(
			form_prompt_insert_attribute->element_list,
			( form_prompt_insert_attribute->
				yes_no_appaserver_element =
					appaserver_element_new(
						yes_no,
						attribute_name ) ) );

		free( form_prompt_insert_attribute->
				yes_no_appaserver_element->
				yes_no );

		form_prompt_insert_attribute->
			yes_no_appaserver_element->
			yes_no =
				element_yes_no_new(
					(char *)0 /* attribute_name */,
					form_prompt_insert_attribute->
						yes_no_appaserver_element->
						element_name,
					0 /* not output_null_option */,
					0 /* not output_not_null_option */,
					(char *)0 /* post_change_javascript */,
					tab_order,
					1 /* recall */ );
	}
	else
	{
		list_set(
			form_prompt_insert_attribute->element_list,
			( form_prompt_insert_attribute->
				text_appaserver_element =
					appaserver_element_new(
						text,
						attribute_name ) ) );

		free( form_prompt_insert_attribute->
			text_appaserver_element->
			text );

		form_prompt_insert_attribute->
			text_appaserver_element->
			text =
				element_text_new(
					attribute_name,
					datatype_name,
					attribute_width,
					0 /* not null_to_slash */,
					1 /* prevent carrot */,
					(char *)0 /* on_change */,
					(char *)0 /* on_focus */,
					(char *)0 /* on_keyup */,
					tab_order,
					1 /* recall */ );
	}

	if ( hint_message )
	{
		list_set(
			form_prompt_insert_attribute->element_list,
			appaserver_element_new( table_data, (char *)0 ) );

		list_set(
			form_prompt_insert_attribute->element_list,
			( form_prompt_insert_attribute->
				hint_message_appaserver_element =
					appaserver_element_new(
						non_edit_text, (char *)0 ) ) );

		free( form_prompt_insert_attribute->
				hint_message_appaserver_element->
				non_edit_text );

		form_prompt_insert_attribute->
			hint_message_appaserver_element->
			non_edit_text =
				element_non_edit_text_new(
					(char *)0,
					hint_message );
	}

	return form_prompt_insert_attribute;
}

FORM_PROMPT_INSERT_ATTRIBUTE *form_prompt_insert_attribute_calloc(
			void )
{
	FORM_PROMPT_INSERT_ATTRIBUTE *form_prompt_insert_attribute;

	if ( ! ( form_prompt_insert_attribute =
			calloc( 1,
				sizeof( FORM_PROMPT_INSERT_ATTRIBUTE ) ) ) )
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

char *form_prompt_insert_attribute_ignore_name(
			char *form_prompt_insert_ignore_prefix,
			char *attribute_name )
{
	char name[ 128 ];

	if ( !form_prompt_insert_ignore_prefix
	||   !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(name,
		"%s%s",
		form_prompt_insert_ignore_prefix,
		attribute_name );

	return strdup( name );
}

FORM_PROMPT_INSERT_RELATION *form_prompt_insert_relation_new(
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where,
			LIST *form_prompt_insert_relation_list,
			char *attribute_name,
			int tab_order )
{
	FORM_PROMPT_INSERT_RELATION *form_prompt_insert_relation;
	RELATION *relation;

	if ( form_prompt_insert_relation_attribute_name_exists(
		attribute_name,
		form_prompt_insert_relation_list ) )
	{
		return (FORM_PROMPT_INSERT_RELATION *)0;
	}

	relation =
		relation_consumes(
			attribute_name,
			relation_mto1_non_isa_list );

	if ( !relation )
	{
		return (FORM_PROMPT_INSERT_RELATION *)0;
	}

	if ( !relation->one_folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: one_folder is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( relation->one_folder->folder_attribute_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: one_folder->folder_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_prompt_insert_relation = form_prompt_insert_relation_calloc();

	form_prompt_insert_relation->relation = relation;
	form_prompt_insert_relation->element_list = list_new();

	form_prompt_insert_relation->query_widget =
		query_widget_fetch(
			relation->one_folder->folder_name
				/* widget_folder_name */,
			login_name,
			relation->
				one_folder->
				folder_attribute_list,
			relation->
				one_folder->
				relation_mto1_non_isa_list,
			security_entity_where,
		drillthru_dictionary );

	form_prompt_insert_relation->form_ignore_name =
		form_ignore_name(
			FORM_IGNORE_PREFIX,
			relation->name );

	list_set(
		form_prompt_insert_relation->element_list,
		( form_prompt_insert_relation->
			ignore_appaserver_element =
				appaserver_element_new(
					checkbox,
					form_prompt_insert_relation->
						form_ignore_name ) ) );

	free( form_prompt_insert_relation->
		ignore_appaserver_element->
		checkbox );

	form_prompt_insert_relation->
		ignore_appaserver_element->
		checkbox =
			element_checkbox_new(
				(char *)0 /* attribute_name */,
				form_prompt_insert_relation->
					ignore_appaserver_element->
					element_name,
				FORM_IGNORE_PUSH_BUTTON_HEADING
					/* prompt_string */,
				(char *)0 /* on_click */,
				-1 /* tab_order */,
				(char *)0 /* image_source */,
				1 /* recall */ );

	list_set(
		form_prompt_insert_relation->element_list,
		appaserver_element_new(
			table_data, (char *)0 ) );

	list_set(
		form_prompt_insert_relation->element_list,
		( form_prompt_insert_relation->
			prompt_appaserver_element =
				appaserver_element_new(
					non_edit_text, (char *)0 ) ) );

	free( form_prompt_insert_relation->
		prompt_appaserver_element->
		non_edit_text );

	form_prompt_insert_relation->
		prompt_appaserver_element->
		non_edit_text =
			element_non_edit_text_new(
				(char *)0,
				relation->prompt );

	list_set(
		form_prompt_insert_relation->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

	list_set(
		form_prompt_insert_relation->element_list,
		( form_prompt_insert_relation->
			drop_down_appaserver_element =
				appaserver_element_new(
					prompt_drop_down,
					relation->name ) ) );

	free( form_prompt_insert_relation->
		drop_down_appaserver_element->
		prompt_drop_down );

	form_prompt_insert_relation->
		drop_down_appaserver_element->
		prompt_drop_down =
			element_prompt_drop_down_new(
				form_prompt_insert_relation->
					drop_down_appaserver_element->
					element_name,
				form_prompt_insert_relation->
					query_widget->
					delimited_list,
				relation->one_folder->no_initial_capital,
				1 /* output_null_option */,
				1 /* output_not_null_option */,
				1 /* output_select_option */,
				1 /* element_drop_down_display_size() */,
				tab_order,
				0 /* not multi_select */,
				(char *)0 /* post_change_javascript */,
				1 /* recall */ );

	if ( relation->hint_message )
	{
		list_set(
			form_prompt_insert_relation->element_list,
				appaserver_element_new(
					table_data, (char *)0 ) );

		list_set(
			form_prompt_insert_relation->element_list,
			( form_prompt_insert_relation->
				hint_message_appaserver_element =
					appaserver_element_new(
						non_edit_text, (char *)0 ) ) );

		free( form_prompt_insert_relation->
			hint_message_appaserver_element->
			non_edit_text );

		form_prompt_insert_relation->
			hint_message_appaserver_element->
			non_edit_text =
				element_non_edit_text_new(
					(char *)0 /* prompt_name */,
					relation->hint_message );
	}

	return form_prompt_insert_relation;
}

boolean form_prompt_insert_relation_attribute_name_exists(
			char *attribute_name,
			LIST *form_prompt_insert_relation_list )
{
	FORM_PROMPT_INSERT_RELATION *form_prompt_insert_relation;

	if ( !list_rewind( form_prompt_insert_relation_list ) ) return 0;

	do {
		form_prompt_insert_relation =
			list_get(
				form_prompt_insert_relation_list );

		if ( list_string_exists(
			attribute_name,
			form_prompt_insert_relation->
				relation->
				foreign_key_list ) )
		{
			return 1;
		}

	} while ( list_next( form_prompt_insert_relation_list ) );

	return 0;
}

FORM_PROMPT_INSERT_RELATION *form_prompt_insert_relation_calloc(
			void )
{
	FORM_PROMPT_INSERT_RELATION *form_prompt_insert_relation;

	if ( ! ( form_prompt_insert_relation =
			calloc( 1,
				sizeof( FORM_PROMPT_INSERT_RELATION ) ) ) )
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

FORM_PROMPT_INSERT_ELEMENT_LIST *
	form_prompt_insert_element_list_new(
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where,
			LIST *folder_attribute_append_isa_list,
			boolean role_folder_lookup )
{
	int tab_order;
	FORM_PROMPT_INSERT_ELEMENT_LIST *form_prompt_insert_element_list;
	FOLDER_ATTRIBUTE *folder_attribute;

	form_prompt_insert_element_list =
		form_prompt_insert_element_list_calloc();

	form_prompt_insert_element_list->element_list = list_new();
	tab_order = 0;

	if ( !list_rewind( folder_attribute_append_isa_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: folder_attribute_append_isa_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		tab_order++;

		list_set(
			form_prompt_insert_element_list->element_list,
				appaserver_element_new(
					table_row, (char *)0 ) );

		list_set(
			form_prompt_insert_element_list->element_list,
				appaserver_element_new(
					table_data, (char *)0 ) );

		if ( list_length( relation_mto1_non_isa_list )
		&&   ( form_prompt_insert_element_list->
			form_prompt_insert_relation =
				form_prompt_insert_relation_new(
					relation_mto1_non_isa_list,
					drillthru_dictionary,
					login_name,
					security_entity_where,
					form_prompt_insert_element_list->
					      form_prompt_insert_relation_list,
					folder_attribute->attribute_name,
					tab_order ) ) )
		{
			if ( !form_prompt_insert_element_list->
				form_prompt_insert_relation_list )
			{
				form_prompt_insert_element_list->
					form_prompt_insert_relation_list =
						list_new();
			}

			list_set(
				form_prompt_insert_element_list->
					form_prompt_insert_relation_list,
				form_prompt_insert_element_list->
					form_prompt_insert_relation );

			list_set_list(
				form_prompt_insert_element_list->
					element_list,
				form_prompt_insert_element_list->
					form_prompt_insert_relation->
					element_list );

			continue;
		}

		form_prompt_insert_element_list->
			form_prompt_insert_attribute =
				form_prompt_insert_attribute_new(
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
					form_prompt_insert_element_list->
					      form_prompt_insert_relation_list,
					tab_order );

		if ( !form_prompt_insert_element_list->
			form_prompt_insert_attribute )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: form_prompt_insert_attribute_new() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set_list(
			form_prompt_insert_element_list->
				element_list,
			form_prompt_insert_element_list->
				form_prompt_insert_attribute->
				element_list );

	} while ( list_next( folder_attribute_append_isa_list ) );

	if ( role_folder_lookup )
	{
		list_set_list(
			form_prompt_insert_element_list->element_list,
			form_prompt_insert_element_list_lookup_list(
				PROMPT_INSERT_LOOKUP_NAME,
				PROMPT_INSERT_LOOKUP_LABEL ) );
	}

	form_prompt_insert_element_list->appaserver_element_list_html =
		appaserver_element_list_html(
			form_prompt_insert_element_list->element_list );

	return form_prompt_insert_element_list;
}

FORM_PROMPT_INSERT_ELEMENT_LIST *
	form_prompt_insert_element_list_calloc(
			void )
{
	FORM_PROMPT_INSERT_ELEMENT_LIST *form_prompt_insert_element_list;

	if ( ! ( form_prompt_insert_element_list =
			calloc( 1,
				sizeof( FORM_PROMPT_INSERT_ELEMENT_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_insert_element_list;
}

FORM_PROMPT_INSERT *form_prompt_insert_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			char *security_entity_where,
			boolean role_folder_lookup )
{
	FORM_PROMPT_INSERT *form_prompt_insert;
	char *tmp;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !list_length( folder_attribute_append_isa_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_prompt_insert = form_prompt_insert_calloc();

	form_prompt_insert->action_string =
		form_prompt_insert_action_string(
			POST_PROMPT_INSERT_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name );

	form_prompt_insert->form_tag_html =
		form_tag_html(
			FORM_PROMPT_INSERT_NAME /* form_name */,
			form_prompt_insert->action_string,
			FRAMESET_TABLE_FRAME );

	if ( ! ( form_prompt_insert->form_prompt_insert_element_list =
			form_prompt_insert_element_list_new(
				relation_mto1_non_isa_list,
				drillthru_dictionary,
				login_name,
				security_entity_where,
				folder_attribute_append_isa_list,
				role_folder_lookup ) ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: form_prompt_insert_element_list_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_prompt_insert->form_multi_select_all_javascript =
		form_multi_select_all_javascript(
			form_prompt_insert->
				form_prompt_insert_element_list->
				element_list );

	form_prompt_insert->form_cookie_key =
		form_cookie_key(
			FORM_PROMPT_INSERT_NAME /* form_name */,
			folder_name );

	form_prompt_insert->form_cookie_multi_key =
		form_cookie_multi_key(
			FORM_PROMPT_INSERT_NAME /* form_name */,
			folder_name );

	form_prompt_insert->form_keystrokes_save_javascript =
		form_keystrokes_save_javascript(
			FORM_PROMPT_INSERT_NAME /* form_name */,
			form_prompt_insert->form_cookie_key,
			form_prompt_insert->
				form_prompt_insert_element_list->
				element_list );

	form_prompt_insert->form_keystrokes_multi_save_javascript =
		form_keystrokes_multi_save_javascript(
			FORM_PROMPT_INSERT_NAME /* form_name */,
			form_prompt_insert->form_cookie_multi_key,
			form_prompt_insert->
				form_prompt_insert_element_list->
				element_list );

	form_prompt_insert->form_keystrokes_recall_javascript =
		form_keystrokes_recall_javascript(
			FORM_PROMPT_INSERT_NAME /* form_name */,
			form_prompt_insert->form_cookie_key,
			form_prompt_insert->
				form_prompt_insert_element_list->
				element_list );

	form_prompt_insert->form_keystrokes_multi_recall_javascript =
		form_keystrokes_multi_recall_javascript(
			FORM_PROMPT_INSERT_NAME /* form_name */,
			form_prompt_insert->form_cookie_multi_key,
			form_prompt_insert->
				form_prompt_insert_element_list->
				element_list );

	form_prompt_insert->form_verify_notepad_widths_javascript =
		form_verify_notepad_widths_javascript(
			FORM_PROMPT_INSERT_NAME /* form_name */,
			form_prompt_insert->
				form_prompt_insert_element_list->
				element_list );

	form_prompt_insert->button_list =
		form_prompt_insert_button_list(
			form_prompt_insert->
				form_multi_select_all_javascript,
			form_prompt_insert->
				form_keystrokes_save_javascript,
			form_prompt_insert->
				form_keystrokes_multi_save_javascript,
			form_prompt_insert->
				form_keystrokes_recall_javascript,
			form_prompt_insert->
				form_keystrokes_multi_recall_javascript,
			form_prompt_insert->
				form_verify_notepad_widths_javascript );

	form_prompt_insert->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_prompt_insert_html(
			form_prompt_insert->form_tag_html,
			form_prompt_insert->
				form_prompt_insert_element_list->
				appaserver_element_list_html,
			( tmp =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				button_list_html(
					form_prompt_insert->
					     button_list ) ),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_html() );

	free( form_prompt_insert->
			form_prompt_insert_element_list->
			appaserver_element_list_html );

	free( tmp );

	button_list_free(
		form_prompt_insert->
			button_list );

	return form_prompt_insert;
}

FORM_PROMPT_INSERT *form_prompt_insert_calloc( void )
{
	FORM_PROMPT_INSERT *form_prompt_insert;

	if ( ! ( form_prompt_insert =
			calloc( 1, sizeof( FORM_PROMPT_INSERT ) ) ) )
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
			char *post_prompt_insert_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name )
{
	char action_string[ 1024 ];

	if ( !post_prompt_insert_executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(action_string,
		"%s/%s?%s+%s+%s+%s+%s",
		appaserver_library_http_prompt(
			appaserver_parameter_cgi_directory(),
			appaserver_library_server_address(),
			application_ssl_support_yn(
				application_name ),
			application_prepend_http_protocol_yn(
				application_name ) ),
		post_prompt_insert_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		folder_name );

	return strdup( action_string );
}

LIST *form_prompt_insert_button_list(
			char *form_multi_select_all_javascript,
			char *form_keystrokes_save_javascript,
			char *form_keystrokes_multi_save_javascript,
			char *form_keystrokes_recall_javascript,
			char *form_keystrokes_multi_recall_javascript,
			char *form_verify_notepad_widths_javascript )
{
	LIST *button_list = list_new();

	list_set(
		button_list,
		button_submit(
			form_multi_select_all_javascript,
			form_keystrokes_save_javascript,
			form_keystrokes_multi_save_javascript,
			form_verify_notepad_widths_javascript,
			0 /* form_number */ ) );

	list_set(
		button_list,
		button_reset(
			(char *)0 /* javascript_replace */,
			0 /* form_number */ ) );

	list_set(
		button_list,
		button_back() );

	list_set(
		button_list,
		button_forward() );

	list_set(
		button_list,
		button_recall(
			form_keystrokes_recall_javascript,
			form_keystrokes_multi_recall_javascript ) );

	return button_list;
}

char *form_prompt_insert_html(
			char *form_tag_html,
			char *form_prompt_edit_element_list_html,
			char *button_list_html,
			char *form_close_html )
{
	char html[ STRING_ONE_MEG ];

	if ( !form_tag_html
	||   !form_prompt_edit_element_list_html
	||   !button_list_html
	||   !form_close_html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"%s\n%s\n%s\n%s",
		form_tag_html,
		form_prompt_edit_element_list_html,
		button_list_html,
		form_close_html );

	return strdup( html );
}

LIST *form_prompt_insert_element_list_lookup_list(
			char *prompt_insert_lookup_name,
			char *prompt_insert_lookup_label )
{
	LIST *element_list;
	APPASERVER_ELEMENT *checkbox_element;

	if ( !prompt_insert_lookup_name
	||   !prompt_insert_lookup_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	element_list = list_new();

	list_set(
		element_list,
			appaserver_element_new(
				table_row, (char *)0 ) );

	list_set(
		element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

	list_set(
		element_list,
		( checkbox_element =
			appaserver_element_new(
				checkbox,
				prompt_insert_lookup_name ) ) );

	free( checkbox_element->checkbox );

	checkbox_element->checkbox =
		element_checkbox_new(
			(char *)0 /* attribute_name */,
			checkbox_element->element_name,
			prompt_insert_lookup_label /* prompt_string */,
			(char *)0 /* on_click */,
			-1 /* tab_order */,
			(char *)0 /* image_source */,
			0 /* not recall */ );

	return element_list;
}

