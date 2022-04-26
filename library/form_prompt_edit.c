/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_prompt_edit.c				*/
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
#include "appaserver.h"
#include "form_prompt_edit.h"

FORM_PROMPT_EDIT *form_prompt_edit_new(
			char *folder_name,
			boolean omit_insert_button,
			boolean omit_delete_button,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where,
			boolean drillthru_participating,
			boolean drillthru_skipped,
			boolean drillthru_finished )
{
	FORM_PROMPT_EDIT *form_prompt_edit = form_prompt_edit_calloc();
	char *tmp;

	form_prompt_edit->radio_pair_list =
		form_prompt_edit_radio_pair_list(
			omit_insert_button,
			omit_delete_button,
			list_length( relation_mto1_non_isa_list ) );

	form_prompt_edit->radio_list =
		radio_list_new(
			FORM_RADIO_LIST_NAME,
			form_prompt_edit->radio_pair_list,
			RADIO_LOOKUP_LABEL /* initial_label */ );

	form_prompt_edit->target_frame =
		form_prompt_edit_target_frame(
			drillthru_participating,
			drillthru_skipped,
			drillthru_finished,
			FRAMESET_PROMPT_FRAME,
			FRAMESET_EDIT_FRAME );

	form_prompt_edit->action_string =
		form_prompt_edit_action_string(
			POST_PROMPT_EDIT_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			state );

	form_prompt_edit->form_tag_html =
		form_tag_html(
			FORM_PROMPT_EDIT_NAME /* form_name */,
			form_prompt_edit->action_string,
			form_prompt_edit->target_frame );

	form_prompt_edit->form_prompt_edit_element_list =
		form_prompt_edit_element_list_new(
			folder_attribute_append_isa_list,
			relation_mto1_non_isa_list,
			relation_join_one2m_list,
			drillthru_dictionary,
			login_name,
			security_entity_where );

	if ( !form_prompt_edit->
		form_prompt_edit_element_list->
		appaserver_element_list_html )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: appaserver_element_list_html is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_prompt_edit->form_multi_select_all_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		form_multi_select_all_javascript(
			form_prompt_edit->
				form_prompt_edit_element_list->
				element_list );

	form_prompt_edit->form_cookie_key =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		form_cookie_key(
			FORM_PROMPT_EDIT_NAME /* form_name */,
			folder_name );

	form_prompt_edit->form_cookie_multi_key =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		form_cookie_multi_key(
			FORM_PROMPT_EDIT_NAME /* form_name */,
			folder_name );

	form_prompt_edit->form_keystrokes_save_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		form_keystrokes_save_javascript(
			FORM_PROMPT_EDIT_NAME /* form_name */,
			form_prompt_edit->form_cookie_key,
			form_prompt_edit->
				form_prompt_edit_element_list->
				element_list );

	form_prompt_edit->form_keystrokes_multi_save_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		form_keystrokes_multi_save_javascript(
			FORM_PROMPT_EDIT_NAME /* form_name */,
			form_prompt_edit->form_cookie_multi_key,
			form_prompt_edit->
				form_prompt_edit_element_list->
				element_list );

	form_prompt_edit->form_keystrokes_recall_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		form_keystrokes_recall_javascript(
			FORM_PROMPT_EDIT_NAME /* form_name */,
			form_prompt_edit->form_cookie_key,
			form_prompt_edit->
				form_prompt_edit_element_list->
				element_list );

	form_prompt_edit->form_keystrokes_multi_recall_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		form_keystrokes_multi_recall_javascript(
			FORM_PROMPT_EDIT_NAME /* form_name */,
			form_prompt_edit->form_cookie_multi_key,
			form_prompt_edit->
				form_prompt_edit_element_list->
				element_list );

	form_prompt_edit->form_verify_notepad_widths_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		form_verify_notepad_widths_javascript(
			FORM_PROMPT_EDIT_NAME /* form_name */,
			form_prompt_edit->
				form_prompt_edit_element_list->
				element_list );

	form_prompt_edit->button_list =
		form_prompt_edit_button_list(
			form_prompt_edit->
				form_multi_select_all_javascript,
			form_prompt_edit->
				form_keystrokes_save_javascript,
			form_prompt_edit->
				form_keystrokes_multi_save_javascript,
			form_prompt_edit->
				form_keystrokes_recall_javascript,
			form_prompt_edit->
				form_keystrokes_multi_recall_javascript,
			form_prompt_edit->
				form_verify_notepad_widths_javascript );

	form_prompt_edit->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_prompt_edit_html(
			form_prompt_edit->form_tag_html,
			form_prompt_edit->radio_list->html,
			form_prompt_edit->
				form_prompt_edit_element_list->
				appaserver_element_list_html,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			( tmp = button_list_html(
				   form_prompt_edit->button_list ) )
					     /* button_list_html */,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_html() );

	free( form_prompt_edit->
		form_prompt_edit_element_list->
		appaserver_element_list_html );

	/* Free button_list_html() */
	/* ----------------------- */
	if ( tmp ) free( tmp );

	button_list_free( form_prompt_edit->button_list );

	return form_prompt_edit;
}

char *form_prompt_edit_target_frame(
			boolean drillthru_participating,
			boolean drillthru_skipped,
			boolean drillthru_finished,
			char *frameset_prompt_frame,
			char *frameset_edit_frame )
{
	if ( !drillthru_participating )
		return frameset_edit_frame;
	else
	if ( drillthru_skipped )
		return frameset_edit_frame;
	else
	if ( drillthru_finished )
		return frameset_edit_frame;
	else
		return frameset_prompt_frame;
}

LIST *form_prompt_edit_radio_pair_list(
			boolean omit_insert_button,
			boolean omit_delete_button,
			int relation_mto1_non_isa_list_length )
{
	LIST *radio_pair_list = list_new();

	list_set(
		radio_pair_list,
		radio_pair_new(
			RADIO_STATISTICS_NAME,
			RADIO_STATISTICS_LABEL ) );

	if ( relation_mto1_non_isa_list_length )
	{
		list_set(
			radio_pair_list,
			radio_pair_new(
				RADIO_GROUP_COUNT_NAME,
				RADIO_GROUP_COUNT_LABEL ) );
	}

	list_set(
		radio_pair_list,
		radio_pair_new(
			RADIO_SPREADSHEET_NAME,
			RADIO_SPREADSHEET_LABEL ) );

	if ( !omit_insert_button )
	{
		list_set(
			radio_pair_list,
			radio_pair_new(
				RADIO_INSERT_NAME,
				RADIO_INSERT_LABEL ) );
	}

	if ( !omit_delete_button )
	{
		list_set(
			radio_pair_list,
			radio_pair_new(
				RADIO_DELETE_NAME,
				RADIO_DELETE_LABEL ) );
	}

	list_set(
		radio_pair_list,
		radio_pair_new(
			RADIO_LOOKUP_NAME,
			RADIO_LOOKUP_LABEL ) );

	return radio_pair_list;
}

FORM_PROMPT_EDIT *form_prompt_edit_calloc( void )
{
	FORM_PROMPT_EDIT *form_prompt_edit;

	if ( ! ( form_prompt_edit = calloc( 1, sizeof( FORM_PROMPT_EDIT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_edit;
}

FORM_PROMPT_EDIT_RELATIONAL *
	form_prompt_edit_relational_new(
			char *relational_name,
			char *from_name,
			char *to_name,
			char *datatype_name,
			int attribute_width )
{
	FORM_PROMPT_EDIT_RELATIONAL *form_prompt_edit_relational =
		form_prompt_edit_relational_calloc();

	form_prompt_edit_relational->element_list = list_new();

	form_prompt_edit_relational->operation_list =
		form_prompt_edit_relational_operation_list(
			datatype_name );

	list_set(
		form_prompt_edit_relational->element_list,
		( form_prompt_edit_relational->
			relation_operator_appaserver_element =
				appaserver_element_new(
					prompt_drop_down, relational_name ) ) );

	form_prompt_edit_relational->
		relation_operator_appaserver_element->
		prompt_drop_down =
			element_prompt_drop_down_new(
				form_prompt_edit_relational->
					relation_operator_appaserver_element->
					element_name,
				form_prompt_edit_relational->operation_list,
				0 /* not no_initial_capital */,
				1 /* output_null_option */,
				1 /* output_not_null_option */,
				1 /* output_select_option */,
				1 /* display_size */,
				-1 /* tab_order */,
				0 /* not multi_select */,
				(char *)0 /* post_change_javascript */,
				1 /* recall */ );

	list_set(
		form_prompt_edit_relational->element_list,
		appaserver_element_new(
			table_data, (char *)0 ) );

	list_set(
		form_prompt_edit_relational->element_list,
		( form_prompt_edit_relational->
			text_from_appaserver_element =
				appaserver_element_new(
					text, from_name ) ) );

	form_prompt_edit_relational->text_from_appaserver_element->text =
		element_text_new(
			form_prompt_edit_relational->
				text_from_appaserver_element->
				element_name,
			datatype_name,
			FORM_PROMPT_EDIT_FROM_ATTRIBUTE_WIDTH,
			0 /* not null_to_slash */,
			1 /* prevent_carrot */,
			(char *)0 /* on_change */,
			(char *)0 /* on_focus */,
			(char *)0 /* on_keyup */,
			-1 /* tab_order */,
			1 /* recall */ );

	list_set(
		form_prompt_edit_relational->element_list,
		appaserver_element_new(
			table_data, (char *)0 ) );

	list_set(
		form_prompt_edit_relational->element_list,
		( form_prompt_edit_relational->
			and_appaserver_element =
				appaserver_element_new(
					non_edit_text, (char *)0 ) ) );

	form_prompt_edit_relational->and_appaserver_element->non_edit_text =
		element_non_edit_text_new(
			(char *)0,
			"and" /* message */ );

	list_set(
		form_prompt_edit_relational->element_list,
		appaserver_element_new(
			table_data, (char *)0 ) );

	list_set(
		form_prompt_edit_relational->element_list,
		( form_prompt_edit_relational->
			text_to_appaserver_element =
				appaserver_element_new(
					text, to_name ) ) );

	form_prompt_edit_relational->text_from_appaserver_element->text =
		element_text_new(
			to_name,
			form_prompt_edit_relational->
				text_to_appaserver_element->
				element_name,
			attribute_width,
			0 /* not null_to_slash */,
			1 /* prevent_carrot */,
			(char *)0 /* on_change */,
			(char *)0 /* on_focus */,
			(char *)0 /* on_keyup */,
			-1 /* tab_order */,
			1 /* recall */ );

	return form_prompt_edit_relational;
}

LIST *form_prompt_edit_relational_operation_list(
			char *datatype_name )
{
	LIST *list;

	list = list_new();

	if ( attribute_is_date( datatype_name )
	||   attribute_is_time( datatype_name )
	||   attribute_is_date_time( datatype_name ) )
	{
		list_set( list, APPASERVER_BEGINS );
		list_set( list, APPASERVER_EQUAL );
		list_set( list, APPASERVER_BETWEEN );
		list_set( list, APPASERVER_OR );
		list_set( list, APPASERVER_GREATER_THAN );
		list_set( list, APPASERVER_GREATER_THAN_EQUAL_TO );
		list_set( list, APPASERVER_LESS_THAN );
		list_set( list, APPASERVER_LESS_THAN_EQUAL_TO );
		list_set( list, APPASERVER_NOT_EQUAL );
		list_set( list, APPASERVER_NULL );
		list_set( list, APPASERVER_NOT_NULL );
	}
	else
	if ( attribute_is_notepad( datatype_name ) )
	{
		list_set( list, APPASERVER_CONTAINS );
		list_set( list, APPASERVER_NOT_CONTAINS );
		list_set( list, APPASERVER_NULL );
		list_set( list, APPASERVER_NOT_NULL );
	}
	else
	if ( attribute_is_text( datatype_name )
	||   attribute_is_upload( datatype_name ) )
	{
		list_set( list, APPASERVER_BEGINS );
		list_set( list, APPASERVER_EQUAL );
		list_set( list, APPASERVER_CONTAINS );
		list_set( list, APPASERVER_NOT_CONTAINS );
		list_set( list, APPASERVER_OR );
		list_set( list, APPASERVER_GREATER_THAN_EQUAL_TO );
		list_set( list, APPASERVER_NOT_EQUAL );
		list_set( list, APPASERVER_NULL );
		list_set( list, APPASERVER_NOT_NULL );
	}
	else
	{
		list_set( list, APPASERVER_EQUAL );
		list_set( list, APPASERVER_BETWEEN );
		list_set( list, APPASERVER_BEGINS );
		list_set( list, APPASERVER_CONTAINS );
		list_set( list, APPASERVER_NOT_CONTAINS );
		list_set( list, APPASERVER_OR );
		list_set( list, APPASERVER_NOT_EQUAL );
		list_set( list, APPASERVER_GREATER_THAN );
		list_set( list, APPASERVER_GREATER_THAN_EQUAL_TO );
		list_set( list, APPASERVER_LESS_THAN );
		list_set( list, APPASERVER_LESS_THAN_EQUAL_TO );
		list_set( list, APPASERVER_NULL );
		list_set( list, APPASERVER_NOT_NULL );
	}

	return list;
}

FORM_PROMPT_EDIT_RELATIONAL *
	form_prompt_edit_relational_calloc(
			void )
{
	FORM_PROMPT_EDIT_RELATIONAL *form_prompt_edit_relational;

	if ( ! ( form_prompt_edit_relational =
			calloc( 1, sizeof( FORM_PROMPT_EDIT_RELATIONAL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_edit_relational;
}

FORM_PROMPT_EDIT_ATTRIBUTE *form_prompt_edit_attribute_new(
			char *attribute_name,
			char *folder_attribute_prompt,
			char *datatype_name,
			int attribute_width,
			char *hint_message,
			LIST *form_prompt_edit_relation_list )
{
	FORM_PROMPT_EDIT_ATTRIBUTE *form_prompt_edit_attribute =
		form_prompt_edit_attribute_calloc();

	if ( form_prompt_edit_relation_attribute_name_exists(
		attribute_name,
		form_prompt_edit_relation_list ) )
	{
		return (FORM_PROMPT_EDIT_ATTRIBUTE *)0;
	}

	form_prompt_edit_attribute->element_list = list_new();

	form_prompt_edit_attribute->no_display_name =
		form_prompt_edit_attribute_no_display_name(
			FORM_PROMPT_EDIT_NO_DISPLAY_PREFIX,
			attribute_name );

	list_set(
		form_prompt_edit_attribute->element_list,
		( form_prompt_edit_attribute->
			no_display_appaserver_element =
				appaserver_element_new(
					checkbox,
					form_prompt_edit_attribute->
						no_display_name ) ) );

	form_prompt_edit_attribute->
		no_display_appaserver_element->
		checkbox =
			element_checkbox_new(
				form_prompt_edit_attribute->
					no_display_appaserver_element->
					element_name,
				(char *)0 /* element_name */,
				(char *)0 /* prompt_string */,
				(char *)0 /* on_click */,
				-1 /* tab_order */,
				(char *)0 /* image_source */,
				1 /* recall */ );

	list_set(
		form_prompt_edit_attribute->element_list,
		appaserver_element_new( table_data, (char *)0 ) );

	list_set(
		form_prompt_edit_attribute->element_list,
		( form_prompt_edit_attribute->prompt_appaserver_element =
			appaserver_element_new(
				non_edit_text, (char *)0 ) ) );

	form_prompt_edit_attribute->prompt_appaserver_element->non_edit_text =
		element_non_edit_text_new(
			(char *)0,
			folder_attribute_prompt );

	list_set(
		form_prompt_edit_attribute->element_list,
		appaserver_element_new( table_data, (char *)0 ) );

	form_prompt_edit_attribute->from_name =
		form_prompt_edit_attribute_from_name(
			DICTIONARY_SEPARATE_FROM_PREFIX,
			attribute_name );

	if ( attribute_is_yes_no( attribute_name ) )
	{
		list_set(
			form_prompt_edit_attribute->element_list,
			( form_prompt_edit_attribute->
				yes_no_appaserver_element =
					appaserver_element_new(
						yes_no,
						form_prompt_edit_attribute->
							from_name ) ) );

		form_prompt_edit_attribute->
			yes_no_appaserver_element->
			yes_no =
				element_yes_no_new(
					(char *)0 /* attribute_name */,
					form_prompt_edit_attribute->
						yes_no_appaserver_element->
						element_name,
					1 /* output_null_option */,
					1 /* output_not_null_option */,
					(char *)0 /* post_change_javascript */,
					-1 /* tab_order */,
					1 /* recall */ );

		list_set(
			form_prompt_edit_attribute->element_list,
			appaserver_element_new( table_data, (char *)0 ) );

		list_set(
			form_prompt_edit_attribute->element_list,
			appaserver_element_new( table_data, (char *)0 ) );

		list_set(
			form_prompt_edit_attribute->element_list,
			appaserver_element_new( table_data, (char *)0 ) );
	}
	else
	{
		form_prompt_edit_attribute->relational_name =
			form_prompt_edit_attribute_relational_name(
				FORM_PROMPT_EDIT_RELATIONAL_PREFIX,
				attribute_name );

		form_prompt_edit_attribute->to_name =
			form_prompt_edit_attribute_to_name(
			DICTIONARY_SEPARATE_TO_PREFIX,
			attribute_name );

		form_prompt_edit_attribute->form_prompt_edit_relational =
			form_prompt_edit_relational_new(
				form_prompt_edit_attribute->relational_name,
				form_prompt_edit_attribute->from_name,
				form_prompt_edit_attribute->to_name,
				datatype_name,
				attribute_width );

		if ( !form_prompt_edit_attribute->form_prompt_edit_relational )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: form_prompt_edit_relational_new() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set_list(
			form_prompt_edit_attribute->element_list,
			form_prompt_edit_attribute->
				form_prompt_edit_relational->
					element_list );
	}

	if ( hint_message )
	{
		list_set(
			form_prompt_edit_attribute->element_list,
			appaserver_element_new( table_data, (char *)0 ) );

		list_set(
			form_prompt_edit_attribute->element_list,
			( form_prompt_edit_attribute->
				hint_message_appaserver_element =
					appaserver_element_new(
						non_edit_text, (char *)0 ) ) );

		form_prompt_edit_attribute->
			hint_message_appaserver_element->
			non_edit_text =
				element_non_edit_text_new(
					(char *)0,
					hint_message );
	}

	return form_prompt_edit_attribute;
}

char *form_prompt_edit_attribute_no_display_name(
			char *form_prompt_edit_no_display_prefix,
			char *attribute_name )
{
	char name[ 128 ];

	sprintf(name,
		"%s%s",
		form_prompt_edit_no_display_prefix,
		attribute_name );

	return strdup( name );
}

char *form_prompt_edit_attribute_from_name(
			char *form_prompt_edit_from_prefix,
			char *attribute_name )
{
	char name[ 128 ];

	sprintf(name,
		"%s%s",
		form_prompt_edit_from_prefix,
		attribute_name );

	return strdup( name );
}

char *form_prompt_edit_attribute_relational_name(
			char *form_prompt_edit_attribute_relational_prefix,
			char *attribute_name )
{
	char name[ 128 ];

	sprintf(name,
		"%s%s",
		form_prompt_edit_attribute_relational_prefix,
		attribute_name );

	return strdup( name );
}

char *form_prompt_edit_attribute_to_name(
			char *form_prompt_edit_to_prefix,
			char *attribute_name )
{
	char name[ 128 ];

	sprintf(name,
		"%s%s",
		form_prompt_edit_to_prefix,
		attribute_name );

	return strdup( name );
}

FORM_PROMPT_EDIT_ATTRIBUTE *form_prompt_edit_attribute_calloc( void )
{
	FORM_PROMPT_EDIT_ATTRIBUTE *form_prompt_edit_attribute;

	if ( ! ( form_prompt_edit_attribute =
			calloc( 1, sizeof( FORM_PROMPT_EDIT_ATTRIBUTE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_edit_attribute;
}

FORM_PROMPT_EDIT_ELEMENT_LIST *
	form_prompt_edit_element_list_new(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	FORM_PROMPT_EDIT_ELEMENT_LIST *form_prompt_edit_element_list;

	if ( !list_rewind( folder_attribute_append_isa_list ) )
		return (FORM_PROMPT_EDIT_ELEMENT_LIST *)0;

	form_prompt_edit_element_list =
		form_prompt_edit_element_list_calloc();

	form_prompt_edit_element_list->element_list = list_new();

	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		list_set(
			form_prompt_edit_element_list->element_list,
			appaserver_element_new(
				table_row, (char *)0 ) );

		list_set(
			form_prompt_edit_element_list->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

		if ( ( form_prompt_edit_element_list->
			form_prompt_edit_relation =
				form_prompt_edit_relation_new(
					folder_attribute->attribute_name,
					relation_mto1_non_isa_list,
					drillthru_dictionary,
					login_name,
					security_entity_where,
					form_prompt_edit_element_list->
					    form_prompt_edit_relation_list ) ) )
		{
			if ( !form_prompt_edit_element_list->
				form_prompt_edit_relation_list )
			{
				form_prompt_edit_element_list->
					form_prompt_edit_relation_list =
						list_new();
			}

			list_set(
				form_prompt_edit_element_list->
					form_prompt_edit_relation_list,
				form_prompt_edit_element_list->
					form_prompt_edit_relation );

			list_set_list(
				form_prompt_edit_element_list->element_list,
				form_prompt_edit_element_list->
					form_prompt_edit_relation->
						element_list );

			continue;
		}

		if ( ( form_prompt_edit_element_list->
			form_prompt_edit_attribute =
				form_prompt_edit_attribute_new(
					folder_attribute->attribute_name,
					folder_attribute->prompt,
					folder_attribute->
						attribute->
						datatype_name,
					folder_attribute->
						attribute->
						width,
					folder_attribute->
						attribute->
						hint_message,
					form_prompt_edit_element_list->
					    form_prompt_edit_relation_list ) ) )
		{
			list_set_list(
				form_prompt_edit_element_list->element_list,
				form_prompt_edit_element_list->
					form_prompt_edit_attribute->
					element_list );
		}

	} while ( list_next( folder_attribute_append_isa_list ) );

	if ( list_length( relation_join_one2m_list ) )
	{
		form_prompt_edit_element_list->
			join_element_list =
				form_prompt_edit_element_list_join_element_list(
					FORM_PROMPT_EDIT_NO_DISPLAY_PREFIX,
					relation_join_one2m_list );
	}

	if ( ! ( form_prompt_edit_element_list->appaserver_element_list_html =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			appaserver_element_list_html(
				form_prompt_edit_element_list->
					element_list ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: appaserver_element_list_html() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_edit_element_list;
}

LIST *form_prompt_edit_element_list_join_element_list(
			char *form_prompt_edit_no_display_prefix,
			LIST *relation_join_one2m_list )
{
	LIST *element_list;
	APPASERVER_ELEMENT *element;
	RELATION *relation;
	char element_name[ 128 ];

	if ( !list_rewind( relation_join_one2m_list ) ) return (LIST *)0;

	element_list = list_new();

	do {
		relation =
			list_get(
				relation_join_one2m_list );

		if ( !relation->many_folder )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: many_folder is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		sprintf(element_name,
			"%s%s",
			form_prompt_edit_no_display_prefix,
			relation->many_folder->folder_name );

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
			( element =
				appaserver_element_new(
					checkbox,
					strdup( element_name ) ) ) );

		element->checkbox =
			element_checkbox_new(
				(char *)0 /* attribute_name */,
				element->element_name,
				NO_DISPLAY_PUSH_BUTTON_HEADING
					/* prompt_string */,
				(char *)0 /* on_click */,
				-1 /* tab_order */,
				(char *)0 /* image_source */,
				1 /* recall */ );

	} while ( list_next( relation_join_one2m_list ) );

	return element_list;
}

FORM_PROMPT_EDIT_ELEMENT_LIST *
	form_prompt_edit_element_list_calloc(
			void )
{
	FORM_PROMPT_EDIT_ELEMENT_LIST *form_prompt_edit_element_list;

	if ( ! ( form_prompt_edit_element_list =
			calloc( 1, sizeof( FORM_PROMPT_EDIT_ELEMENT_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_edit_element_list;
}

FORM_PROMPT_EDIT_RELATION *form_prompt_edit_relation_new(
			char *attribute_name,
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where,
			LIST *form_prompt_edit_relation_list )
{
	FORM_PROMPT_EDIT_RELATION *form_prompt_edit_relation;
	RELATION *relation;

	if ( form_prompt_edit_relation_attribute_name_exists(
		attribute_name,
		form_prompt_edit_relation_list ) )
	{
		return (FORM_PROMPT_EDIT_RELATION *)0;
	}

	if ( ! ( relation =
			relation_consumes(
				attribute_name,
				relation_mto1_non_isa_list ) ) )
	{
		return (FORM_PROMPT_EDIT_RELATION *)0;
	}

	if ( !relation->one_folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: relation->one_folder is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !relation->one_folder->folder_name )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: relation->one_folder->folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	relation->consumes_taken = 1;

	form_prompt_edit_relation = form_prompt_edit_relation_calloc();
	form_prompt_edit_relation->relation = relation;
	form_prompt_edit_relation->element_list = list_new();

	form_prompt_edit_relation->query_widget =
		query_widget_new(
			relation->one_folder->folder_name
				/* widget_folder_name */,
			login_name,
			relation->one_folder->folder_attribute_list,
			relation->one_folder->relation_mto1_non_isa_list,
			security_entity_where,
			drillthru_dictionary );

	if ( !form_prompt_edit_relation->query_widget )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: query_widget_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_prompt_edit_relation->no_display_name =
		form_prompt_edit_relation_no_display_name(
			FORM_PROMPT_EDIT_NO_DISPLAY_PREFIX,
			form_prompt_edit_relation->relation->name );

	list_set(
		form_prompt_edit_relation->element_list,
		( form_prompt_edit_relation->no_display_appaserver_element =
			appaserver_element_new(
				checkbox,
				form_prompt_edit_relation->
					no_display_name ) ) );

	form_prompt_edit_relation->
		no_display_appaserver_element->
		checkbox =
			element_checkbox_new(
				(char *)0 /* attribute_name */,
				form_prompt_edit_relation->
					no_display_appaserver_element->
					element_name,
				NO_DISPLAY_PUSH_BUTTON_HEADING
					/* prompt_string */,
				(char *)0 /* on_click */,
				-1 /* tab_order */,
				(char *)0 /* image_source */,
				1 /* recall */ );

	list_set(
		form_prompt_edit_relation->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

	list_set(
		form_prompt_edit_relation->element_list,
		( form_prompt_edit_relation->prompt_appaserver_element =
			appaserver_element_new(
				non_edit_text, (char *)0 ) ) );

	form_prompt_edit_relation->
		prompt_appaserver_element->
		non_edit_text =
			element_non_edit_text_new(
				(char *)0,
				form_prompt_edit_relation->
					relation->
					prompt );

	list_set(
		form_prompt_edit_relation->element_list,
		appaserver_element_new(
			table_data, (char *)0 ) );

	form_prompt_edit_relation->element_name =
		form_prompt_edit_relation_element_name(
			FORM_PROMPT_EDIT_RELATION_PREFIX,
			form_prompt_edit_relation->name );

	if ( relation->drop_down_multi_select )
	{
		form_prompt_edit_relation->original_name =
			form_prompt_edit_relation_original_name(
				FORM_PROMPT_EDIT_ORIGINAL_PREFIX,
				form_prompt_edit_relation->name );

		list_set(
			form_prompt_edit_relation->element_list,
			( form_prompt_edit_relation->
				drop_down_appaserver_element =
					appaserver_element_new(
						multi_drop_down,
						(char *)0 ) ) );

		form_prompt_edit_relation->
			drop_down_appaserver_element->
			multi_drop_down =
				element_multi_drop_down_new(
					form_prompt_edit_relation->
						original_name,
					form_prompt_edit_relation->
						element_name,
					form_prompt_edit_relation->
						query_widget->
						delimited_list,
					relation->
						one_folder->
						no_initial_capital );
	}
	else
	{
		list_set(
			form_prompt_edit_relation->element_list,
			( form_prompt_edit_relation->
				drop_down_appaserver_element =
					appaserver_element_new(
						prompt_drop_down,
						form_prompt_edit_relation->
							element_name ) ) );

		form_prompt_edit_relation->
			drop_down_appaserver_element->
			prompt_drop_down =
				element_prompt_drop_down_new(
					form_prompt_edit_relation->
						drop_down_appaserver_element->
						element_name,
					form_prompt_edit_relation->
						query_widget->
						delimited_list,
					relation->
						one_folder->
						no_initial_capital,
					1 /* output_null_option */,
					1 /* output_not_null_option */,
					1 /* output_select_option */,
					1 /* display_size */,
					-1 /* tab_order */,
					0 /* not multi_select */,
					(char *)0 /* post_change_javascript */,
					1 /* recall */ );

		list_set(
			form_prompt_edit_relation->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

		list_set(
			form_prompt_edit_relation->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

		list_set(
			form_prompt_edit_relation->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );
	}

	if ( relation->hint_message )
	{
		list_set(
			form_prompt_edit_relation->element_list,
			( form_prompt_edit_relation->
				hint_message_appaserver_element =
					appaserver_element_new(
						non_edit_text, (char *)0 ) ) );

		form_prompt_edit_relation->
			hint_message_appaserver_element->
			non_edit_text =
				element_non_edit_text_new(
					(char *)0 /* prompt_name */,
					relation->hint_message );
	}

	return form_prompt_edit_relation;
}

boolean form_prompt_edit_relation_attribute_name_exists(
			char *attribute_name,
			LIST *form_prompt_edit_relation_list )
{
	FORM_PROMPT_EDIT_RELATION *form_prompt_edit_relation;

	if ( !list_rewind( form_prompt_edit_relation_list ) ) return 0;

	do {
		form_prompt_edit_relation =
			list_get(
				form_prompt_edit_relation_list );

		if ( list_string_exists(
			attribute_name,
			form_prompt_edit_relation->
				relation->
				foreign_key_list ) )
		{
			return 1;
		}

	} while ( list_next( form_prompt_edit_relation_list ) );

	return 0;
}

char *form_prompt_edit_relation_no_display_name(
			char *form_prompt_edit_no_display_prefix,
			char *relation_name )
{
	char name[ 128 ];

	sprintf(name,
		"%s%s",
		form_prompt_edit_no_display_prefix,
		relation_name );

	return strdup( name );
}

char *form_prompt_edit_relation_original_name(
			char *form_prompt_edit_original_prefix,
			char *relation_name )
{
	char name[ 128 ];

	sprintf(name,
		"%s%s",
		form_prompt_edit_original_prefix,
		relation_name );

	return strdup( name );
}

char *form_prompt_edit_relation_element_name(
			char *form_prompt_edit_relation_prefix,
			char *relation_name )
{
	char name[ 128 ];

	sprintf(name,
		"%s%s",
		form_prompt_edit_relation_prefix,
		relation_name );

	return strdup( name );
}

FORM_PROMPT_EDIT_RELATION *form_prompt_edit_relation_calloc( void )
{
	FORM_PROMPT_EDIT_RELATION *form_prompt_edit_relation;

	if ( ! ( form_prompt_edit_relation =
			calloc( 1, sizeof( FORM_PROMPT_EDIT_RELATION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_edit_relation;
}

char *form_prompt_edit_html(
			char *tag_html,
			char *radio_list_html,
			char *appaserver_element_list_html,
			char *button_list_html,
			char *form_close_html )
{
	char html[ STRING_ONE_MEG ];

	if ( !tag_html
	||   !radio_list_html
	||   !appaserver_element_list_html
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
		"%s\n%s\n%s\n%s\n%s",
		tag_html,
		radio_list_html,
		appaserver_element_list_html,
		button_list_html,
		form_close_html );

	return strdup( html );
}

LIST *form_prompt_edit_button_list(
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

char *form_prompt_edit_action_string(
			char *post_prompt_edit_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state )
{
	char action_string[ 1024 ];

	if ( !post_prompt_edit_executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !state )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(action_string,
		"%s/%s?%s+%s+%s+%s+%s+%s",
		appaserver_library_http_prompt(
			appaserver_parameter_cgi_directory(),
			appaserver_library_server_address(),
			application_ssl_support_yn(
				application_name ),
			application_prepend_http_protocol_yn(
				application_name ) ),
		post_prompt_edit_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		folder_name,
		state );

	return strdup( action_string );
}

