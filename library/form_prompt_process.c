/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_prompt_process.c			*/
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
#include "post_prompt_process.h"
#include "form_prompt_attribute_relational.h"
#include "form_prompt_process.h"

FORM_PROMPT_PROCESS_ATTRIBUTE *
	form_prompt_process_attribute_new(
			char *attribute_name,
			char *datatype_name,
			int attribute_width,
			char *hint_message )
{
	FORM_PROMPT_PROCESS_ATTRIBUTE *form_prompt_process_attribute;

	if ( !attribute_name,
	||   !datatype_name,
	||   !attribute_width )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_prompt_process_attribute = form_prompt_process_attribute_calloc();

	form_prompt_process_attribute->element_list = list_new();

	list_set(
		form_prompt_process_attribute->element_list,
		( form_prompt_process_attribute->
			prompt_appaserver_element =
				appaserver_element_new(
					non_edit_text, (char *)0 ) ) );

	free( form_prompt_process_attribute->
			prompt_appaserver_element->
			non_edit_text );

	form_prompt_process_attribute->
		prompt_appaserver_element->
		non_edit_text =
			element_non_edit_text_new(
				(char *)0,
				attribute_name );

	list_set(
		form_prompt_process_attribute->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

	form_prompt_process_attribute->
		form_prompt_attribute_relational =
			form_prompt_attribute_relational_new(
				attribute_name,
				datatype_name,
				attribute_width,
				hint_message );

	if ( !form_prompt_process_attribute->form_prompt_attribute_relational )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: form_prompt_attribute_relational_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list_set_list(
		form_prompt_process_attribute->element_list,
		form_prompt_attribute_relational->element_list );

	return form_prompt_process_attribute;
}

FORM_PROMPT_PROCESS_ATTRIBUTE *form_prompt_process_attribute_calloc( void )
{
	FORM_PROMPT_PROCESS_ATTRIBUTE *form_prompt_process_attribute;

	if ( ! ( form_prompt_process_attribute =
			calloc(	1,
				sizeof( FORM_PROMPT_PROCESS_ATTRIBUTE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_process_attribute;
}

FORM_PROMPT_LOOKUP_ELEMENT_LIST *
	form_prompt_lookup_element_list_new(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where );

/* Process */
/* ------- */
FORM_PROMPT_LOOKUP_ELEMENT_LIST *
	form_prompt_lookup_element_list_calloc(
			void );

LIST *form_prompt_lookup_element_list_join_element_list(
			char *form_prompt_lookup_no_display_prefix,
			LIST *relation_join_one2m_list );

typedef struct
{
	LIST *radio_pair_list;
	RADIO_LIST *radio_list;
	char *target_frame;
	char *action_string;
	char *form_tag_html;
	FORM_PROMPT_LOOKUP_ELEMENT_LIST *form_prompt_lookup_element_list;
	char *form_multi_select_all_javascript;
	char *form_cookie_key;
	char *form_cookie_multi_key;
	char *form_keystrokes_save_javascript;
	char *form_keystrokes_multi_save_javascript;
	char *form_keystrokes_recall_javascript;
	char *form_keystrokes_multi_recall_javascript;
	char *form_verify_notepad_widths_javascript;
	LIST *button_list;
	char *html;
} FORM_PROMPT_LOOKUP;

/* FORM_PROMPT_LOOKUP operations */
/* --------------------------- */

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
FORM_PROMPT_LOOKUP *form_prompt_lookup_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			boolean prompt_lookup_omit_insert_button,
			boolean prompt_lookup_omit_delete_button,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			char *security_entity_where );

/* Process */
/* ------- */
FORM_PROMPT_LOOKUP *form_prompt_lookup_calloc(
			void );

LIST *form_prompt_lookup_radio_pair_list(
			boolean prompt_lookup_omit_insert_button,
			boolean prompt_lookup_omit_delete_button,
			int relation_mto1_non_isa_list_length );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_lookup_action_string(
			char *post_prompt_lookup_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name );

LIST *form_prompt_lookup_button_list(
			char *form_multi_select_all_javascript,
			char *form_keystrokes_save_javascript,
			char *form_keystrokes_multi_save_javascript,
			char *form_keystrokes_recall_javascript,
			char *form_keystrokes_multi_recall_javascript,
			char *form_verify_notepad_widths_javascript );

/* Returns heap memory */
/* ------------------- */
char *form_prompt_lookup_html(
			char *form_tag_html,
			char *radio_list_html,
			char *form_prompt_lookup_element_list_html,
			char *button_list_html,
			char *form_close_html );

#endif
