/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_prompt_lookup.c			*/
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
#include "post_prompt_lookup.h"
#include "form_prompt_attribute_relational.h"
#include "form_prompt_lookup.h"

FORM_PROMPT_LOOKUP *form_prompt_lookup_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			boolean omit_insert_button,
			boolean omit_delete_button,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			char *security_entity_where )
{
	FORM_PROMPT_LOOKUP *form_prompt_lookup;
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

	form_prompt_lookup = form_prompt_lookup_calloc();

	form_prompt_lookup->radio_pair_list =
		form_prompt_lookup_radio_pair_list(
			omit_insert_button,
			omit_delete_button,
			list_length( relation_mto1_non_isa_list ) );

	form_prompt_lookup->radio_list =
		radio_list_new(
			FORM_RADIO_LIST_NAME,
			form_prompt_lookup->radio_pair_list,
			RADIO_LOOKUP_LABEL /* initial_label */ );

	form_prompt_lookup->action_string =
		form_prompt_lookup_action_string(
			POST_PROMPT_LOOKUP_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name );

	form_prompt_lookup->form_tag_html =
		form_tag_html(
			FORM_PROMPT_LOOKUP_NAME /* form_name */,
			form_prompt_lookup->action_string,
			FRAMESET_TABLE_FRAME /* target_frame */ );

	form_prompt_lookup->form_prompt_lookup_element_list =
		form_prompt_lookup_element_list_new(
			folder_attribute_append_isa_list,
			relation_mto1_non_isa_list,
			relation_join_one2m_list,
			drillthru_dictionary,
			login_name,
			security_entity_where );

	if ( !form_prompt_lookup->
		form_prompt_lookup_element_list
	||   !form_prompt_lookup->
		form_prompt_lookup_element_list->
		appaserver_element_list_html )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: appaserver_element_list_html is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_prompt_lookup->form_multi_select_all_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		form_multi_select_all_javascript(
			form_prompt_lookup->
				form_prompt_lookup_element_list->
				element_list );

	if ( ! ( form_prompt_lookup->recall =
			recall_new(
				folder_name,
				APPASERVER_LOOKUP_STATE,
				FORM_PROMPT_LOOKUP_NAME /* form_name */,
				form_prompt_lookup->
					form_prompt_lookup_element_list->
					element_list ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: recall_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_prompt_lookup->button_list =
		form_prompt_lookup_button_list(
			form_prompt_lookup->
				form_multi_select_all_javascript,
			form_prompt_lookup->
				recall->
				save_javascript,
			form_prompt_lookup->
				recall->
				load_javascript );

	form_prompt_lookup->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_prompt_lookup_html(
			form_prompt_lookup->form_tag_html,
			form_prompt_lookup->radio_list->html,
			form_prompt_lookup->
				form_prompt_lookup_element_list->
				appaserver_element_list_html,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			( tmp = button_list_html(
				   form_prompt_lookup->button_list ) )
					     /* button_list_html */,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_html() );

	free( form_prompt_lookup->
		form_prompt_lookup_element_list->
		appaserver_element_list_html );

	/* Free button_list_html() */
	/* ----------------------- */
	if ( tmp ) free( tmp );

	button_list_free( form_prompt_lookup->button_list );

	return form_prompt_lookup;
}

LIST *form_prompt_lookup_radio_pair_list(
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

FORM_PROMPT_LOOKUP *form_prompt_lookup_calloc( void )
{
	FORM_PROMPT_LOOKUP *form_prompt_lookup;

	if ( ! ( form_prompt_lookup = calloc( 1, sizeof( FORM_PROMPT_LOOKUP ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_lookup;
}

FORM_PROMPT_LOOKUP_ATTRIBUTE *form_prompt_lookup_attribute_new(
			char *attribute_name,
			char *folder_attribute_prompt,
			char *datatype_name,
			int attribute_width,
			char *hint_message,
			LIST *form_prompt_lookup_relation_list )
{
	FORM_PROMPT_LOOKUP_ATTRIBUTE *form_prompt_lookup_attribute;
		form_prompt_lookup_attribute_calloc();

	if ( form_prompt_lookup_relation_attribute_name_exists(
		attribute_name,
		form_prompt_lookup_relation_list ) )
	{
		return (FORM_PROMPT_LOOKUP_ATTRIBUTE *)0;
	}

	form_prompt_lookup_attribute = form_prompt_lookup_attribute_calloc();

	form_prompt_lookup_attribute->element_list = list_new();

	form_prompt_lookup_attribute->no_display_name =
		form_prompt_lookup_attribute_no_display_name(
			FORM_NO_DISPLAY_PREFIX,
			attribute_name );

	list_set(
		form_prompt_lookup_attribute->element_list,
		( form_prompt_lookup_attribute->
			no_display_appaserver_element =
				appaserver_element_new(
					checkbox,
					form_prompt_lookup_attribute->
						no_display_name ) ) );

	form_prompt_lookup_attribute->
		no_display_appaserver_element->
		checkbox =
			element_checkbox_new(
				(char *)0 /* attribute_name */,
				form_prompt_lookup_attribute->
					no_display_appaserver_element->
					element_name,
				(char *)0 /* prompt_string */,
				(char *)0 /* on_click */,
				-1 /* tab_order */,
				(char *)0 /* image_source */,
				1 /* recall */ );

	list_set(
		form_prompt_lookup_attribute->element_list,
		appaserver_element_new( table_data, (char *)0 ) );

	list_set(
		form_prompt_lookup_attribute->element_list,
		( form_prompt_lookup_attribute->prompt_appaserver_element =
			appaserver_element_new(
				non_edit_text, (char *)0 ) ) );

	form_prompt_lookup_attribute->prompt_appaserver_element->non_edit_text =
		element_non_edit_text_new(
			(char *)0,
			folder_attribute_prompt );

	list_set(
		form_prompt_lookup_attribute->element_list,
		appaserver_element_new( table_data, (char *)0 ) );

	form_prompt_lookup_attribute->form_prompt_attribute_relational =
		form_prompt_attribute_relational_new(
			attribute_name,
			datatype_name,
			attribute_width,
			hint_message,
			(char *)0 /* post_change_javascript */ );

	if ( !form_prompt_lookup_attribute->
		form_prompt_attribute_relational )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: form_prompt_attribute_relational_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list_set_list(
		form_prompt_lookup_attribute->element_list,
		form_prompt_lookup_attribute->
			form_prompt_attribute_relational->
			element_list );

	return form_prompt_lookup_attribute;
}

char *form_prompt_lookup_attribute_no_display_name(
			char *form_prompt_lookup_no_display_prefix,
			char *attribute_name )
{
	char name[ 128 ];

	sprintf(name,
		"%s%s",
		form_prompt_lookup_no_display_prefix,
		attribute_name );

	return strdup( name );
}

FORM_PROMPT_LOOKUP_ATTRIBUTE *form_prompt_lookup_attribute_calloc( void )
{
	FORM_PROMPT_LOOKUP_ATTRIBUTE *form_prompt_lookup_attribute;

	if ( ! ( form_prompt_lookup_attribute =
			calloc( 1, sizeof( FORM_PROMPT_LOOKUP_ATTRIBUTE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_lookup_attribute;
}

FORM_PROMPT_LOOKUP_ELEMENT_LIST *
	form_prompt_lookup_element_list_new(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	FORM_PROMPT_LOOKUP_ELEMENT_LIST *form_prompt_lookup_element_list;

	if ( !list_rewind( folder_attribute_append_isa_list ) )
		return (FORM_PROMPT_LOOKUP_ELEMENT_LIST *)0;

	form_prompt_lookup_element_list =
		form_prompt_lookup_element_list_calloc();

	form_prompt_lookup_element_list->element_list = list_new();

	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		list_set(
			form_prompt_lookup_element_list->element_list,
			appaserver_element_new(
				table_row, (char *)0 ) );

		list_set(
			form_prompt_lookup_element_list->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

		if ( ( form_prompt_lookup_element_list->
			form_prompt_lookup_relation =
				form_prompt_lookup_relation_new(
					folder_attribute->attribute_name,
					relation_mto1_non_isa_list,
					drillthru_dictionary,
					login_name,
					security_entity_where,
					form_prompt_lookup_element_list->
					    form_prompt_lookup_relation_list ) ) )
		{
			if ( !form_prompt_lookup_element_list->
				form_prompt_lookup_relation_list )
			{
				form_prompt_lookup_element_list->
					form_prompt_lookup_relation_list =
						list_new();
			}

			list_set(
				form_prompt_lookup_element_list->
					form_prompt_lookup_relation_list,
				form_prompt_lookup_element_list->
					form_prompt_lookup_relation );

			list_set_list(
				form_prompt_lookup_element_list->
					element_list,
				form_prompt_lookup_element_list->
					form_prompt_lookup_relation->
						element_list );

			continue;
		}

		if ( ( form_prompt_lookup_element_list->
			form_prompt_lookup_attribute =
			   form_prompt_lookup_attribute_new(
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
				form_prompt_lookup_element_list->
				    form_prompt_lookup_relation_list ) ) )
		{
			list_set_list(
				form_prompt_lookup_element_list->
					element_list,
				form_prompt_lookup_element_list->
					form_prompt_lookup_attribute->
					element_list );
		}

	} while ( list_next( folder_attribute_append_isa_list ) );

	if ( list_length( relation_join_one2m_list ) )
	{
		form_prompt_lookup_element_list->
			join_element_list =
			   form_prompt_lookup_element_list_join_element_list(
				FORM_NO_DISPLAY_PREFIX,
				relation_join_one2m_list );
	}

	if ( ! ( form_prompt_lookup_element_list->appaserver_element_list_html =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			appaserver_element_list_html(
				form_prompt_lookup_element_list->
					element_list ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: appaserver_element_list_html() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_lookup_element_list;
}

LIST *form_prompt_lookup_element_list_join_element_list(
			char *form_prompt_lookup_no_display_prefix,
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
			form_prompt_lookup_no_display_prefix,
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
				FORM_NO_DISPLAY_PUSH_BUTTON_HEADING
					/* prompt_string */,
				(char *)0 /* on_click */,
				-1 /* tab_order */,
				(char *)0 /* image_source */,
				1 /* recall */ );

	} while ( list_next( relation_join_one2m_list ) );

	return element_list;
}

FORM_PROMPT_LOOKUP_ELEMENT_LIST *
	form_prompt_lookup_element_list_calloc(
			void )
{
	FORM_PROMPT_LOOKUP_ELEMENT_LIST *form_prompt_lookup_element_list;

	if ( ! ( form_prompt_lookup_element_list =
			calloc( 1, sizeof( FORM_PROMPT_LOOKUP_ELEMENT_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_lookup_element_list;
}

FORM_PROMPT_LOOKUP_RELATION *form_prompt_lookup_relation_new(
			char *attribute_name,
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where,
			LIST *form_prompt_lookup_relation_list )
{
	FORM_PROMPT_LOOKUP_RELATION *form_prompt_lookup_relation;
	RELATION *relation;

	if ( form_prompt_lookup_relation_attribute_name_exists(
		attribute_name,
		form_prompt_lookup_relation_list ) )
	{
		return (FORM_PROMPT_LOOKUP_RELATION *)0;
	}

	if ( ! ( relation =
			relation_consumes(
				attribute_name,
				relation_mto1_non_isa_list ) ) )
	{
		return (FORM_PROMPT_LOOKUP_RELATION *)0;
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

	form_prompt_lookup_relation = form_prompt_lookup_relation_calloc();
	form_prompt_lookup_relation->relation = relation;
	form_prompt_lookup_relation->element_list = list_new();

	form_prompt_lookup_relation->query_widget =
		query_widget_fetch(
			relation->one_folder->folder_name
				/* widget_folder_name */,
			login_name,
			relation->one_folder->folder_attribute_list,
			relation->one_folder->relation_mto1_non_isa_list,
			security_entity_where,
			drillthru_dictionary );

	if ( !form_prompt_lookup_relation->query_widget )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: query_widget_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_prompt_lookup_relation->no_display_name =
		form_prompt_lookup_relation_no_display_name(
			FORM_NO_DISPLAY_PREFIX,
			form_prompt_lookup_relation->relation->name );

	list_set(
		form_prompt_lookup_relation->element_list,
		( form_prompt_lookup_relation->no_display_appaserver_element =
			appaserver_element_new(
				checkbox,
				form_prompt_lookup_relation->
					no_display_name ) ) );

	form_prompt_lookup_relation->
		no_display_appaserver_element->
		checkbox =
			element_checkbox_new(
				(char *)0 /* attribute_name */,
				form_prompt_lookup_relation->
					no_display_appaserver_element->
					element_name,
				FORM_NO_DISPLAY_PUSH_BUTTON_HEADING
					/* prompt_string */,
				(char *)0 /* on_click */,
				-1 /* tab_order */,
				(char *)0 /* image_source */,
				1 /* recall */ );

	list_set(
		form_prompt_lookup_relation->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

	list_set(
		form_prompt_lookup_relation->element_list,
		( form_prompt_lookup_relation->prompt_appaserver_element =
			appaserver_element_new(
				non_edit_text, (char *)0 ) ) );

	form_prompt_lookup_relation->
		prompt_appaserver_element->
		non_edit_text =
			element_non_edit_text_new(
				(char *)0,
				form_prompt_lookup_relation->
					relation->
					prompt );

	list_set(
		form_prompt_lookup_relation->element_list,
		appaserver_element_new(
			table_data, (char *)0 ) );

	form_prompt_lookup_relation->element_name =
		form_prompt_lookup_relation_element_name(
			FORM_PROMPT_RELATION_PREFIX,
			form_prompt_lookup_relation->name );

	if ( relation->drop_down_multi_select )
	{
		form_prompt_lookup_relation->original_name =
			form_prompt_lookup_relation_original_name(
				FORM_PROMPT_ORIGINAL_PREFIX,
				form_prompt_lookup_relation->name );

		list_set(
			form_prompt_lookup_relation->element_list,
			( form_prompt_lookup_relation->
				drop_down_appaserver_element =
					appaserver_element_new(
						multi_drop_down,
						(char *)0 ) ) );

		form_prompt_lookup_relation->
			drop_down_appaserver_element->
			multi_drop_down =
				element_multi_drop_down_new(
					form_prompt_lookup_relation->
						original_name,
					form_prompt_lookup_relation->
						element_name,
					form_prompt_lookup_relation->
						query_widget->
						delimited_list,
					relation->
						one_folder->
						no_initial_capital );
	}
	else
	{
		list_set(
			form_prompt_lookup_relation->element_list,
			( form_prompt_lookup_relation->
				drop_down_appaserver_element =
					appaserver_element_new(
						prompt_drop_down,
						form_prompt_lookup_relation->
							element_name ) ) );

		form_prompt_lookup_relation->
			drop_down_appaserver_element->
			prompt_drop_down =
				element_prompt_drop_down_new(
					form_prompt_lookup_relation->
						drop_down_appaserver_element->
						element_name,
					form_prompt_lookup_relation->
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
			form_prompt_lookup_relation->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

		list_set(
			form_prompt_lookup_relation->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

		list_set(
			form_prompt_lookup_relation->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );
	}

	if ( relation->hint_message )
	{
		list_set(
			form_prompt_lookup_relation->element_list,
			( form_prompt_lookup_relation->
				hint_message_appaserver_element =
					appaserver_element_new(
						non_edit_text, (char *)0 ) ) );

		form_prompt_lookup_relation->
			hint_message_appaserver_element->
			non_edit_text =
				element_non_edit_text_new(
					(char *)0 /* prompt_name */,
					relation->hint_message );
	}

	return form_prompt_lookup_relation;
}

boolean form_prompt_lookup_relation_attribute_name_exists(
			char *attribute_name,
			LIST *form_prompt_lookup_relation_list )
{
	FORM_PROMPT_LOOKUP_RELATION *form_prompt_lookup_relation;

	if ( !list_rewind( form_prompt_lookup_relation_list ) ) return 0;

	do {
		form_prompt_lookup_relation =
			list_get(
				form_prompt_lookup_relation_list );

		if ( list_string_exists(
			attribute_name,
			form_prompt_lookup_relation->
				relation->
				foreign_key_list ) )
		{
			return 1;
		}

	} while ( list_next( form_prompt_lookup_relation_list ) );

	return 0;
}

char *form_prompt_lookup_relation_no_display_name(
			char *form_prompt_lookup_no_display_prefix,
			char *relation_name )
{
	char name[ 128 ];

	sprintf(name,
		"%s%s",
		form_prompt_lookup_no_display_prefix,
		relation_name );

	return strdup( name );
}

char *form_prompt_lookup_relation_original_name(
			char *form_prompt_lookup_original_prefix,
			char *relation_name )
{
	char name[ 128 ];

	sprintf(name,
		"%s%s",
		form_prompt_lookup_original_prefix,
		relation_name );

	return strdup( name );
}

char *form_prompt_lookup_relation_element_name(
			char *form_prompt_lookup_relation_prefix,
			char *relation_name )
{
	char name[ 128 ];

	sprintf(name,
		"%s%s",
		form_prompt_lookup_relation_prefix,
		relation_name );

	return strdup( name );
}

FORM_PROMPT_LOOKUP_RELATION *form_prompt_lookup_relation_calloc( void )
{
	FORM_PROMPT_LOOKUP_RELATION *form_prompt_lookup_relation;

	if ( ! ( form_prompt_lookup_relation =
			calloc( 1, sizeof( FORM_PROMPT_LOOKUP_RELATION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_lookup_relation;
}

char *form_prompt_lookup_html(
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

LIST *form_prompt_lookup_button_list(
			char *form_multi_select_all_javascript,
			char *recall_save_javascript,
			char *recall_load_javascript )
{
	LIST *button_list = list_new();

	list_set(
		button_list,
		button_submit(
			form_multi_select_all_javascript,
			recall_save_javascript,
			(char *)0 /* form_verify_notepad_widths_javascript */,
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
		button_recall( recall_load_javascript ) );

	return button_list;
}

char *form_prompt_lookup_action_string(
			char *post_prompt_lookup_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name )
{
	char action_string[ 1024 ];

	if ( !post_prompt_lookup_executable
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
		post_prompt_lookup_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		folder_name );

	return strdup( action_string );
}

