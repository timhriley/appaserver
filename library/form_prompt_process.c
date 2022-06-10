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
#include "form_prompt_lookup.h"
#include "form_prompt_attribute_relational.h"
#include "form_prompt_process.h"

FORM_PROMPT_PROCESS_ATTRIBUTE *
	form_prompt_process_attribute_new(
			char *post_change_javascript,
			char *attribute_name,
			char *datatype_name,
			int attribute_width,
			char *hint_message )
{
	FORM_PROMPT_PROCESS_ATTRIBUTE *form_prompt_process_attribute;

	if ( !attribute_name
	||   !datatype_name
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
				hint_message,
				post_change_javascript );

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
		form_prompt_process_attribute->
			form_prompt_attribute_relational->
			element_list );

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

FORM_PROMPT_PROCESS_DROP_DOWN *
	form_prompt_process_drop_down_new(
			char *post_change_javascript,
			PROCESS_PARAMETER_DROP_DOWN *
				process_parameter_drop_down )
{
	FORM_PROMPT_PROCESS_DROP_DOWN *form_prompt_process_drop_down;

	if ( !process_parameter_drop_down )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: process_parameter_drop_down is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_prompt_process_drop_down = form_prompt_process_drop_down_calloc();

	form_prompt_process_drop_down->element_list = list_new();

	list_set(
		form_prompt_process_drop_down->element_list,
		( form_prompt_process_drop_down->
			prompt_appaserver_element =
				appaserver_element_new(
					non_edit_text, (char *)0 ) ) );

	free(	form_prompt_process_drop_down->
			prompt_appaserver_element->
			non_edit_text );

	form_prompt_process_drop_down->
		prompt_appaserver_element->
		non_edit_text =
			element_non_edit_text_new(
				(char *)0,
				process_parameter_drop_down->
					drop_down_name );

	list_set(
		form_prompt_process_drop_down->element_list,
		appaserver_element_new(
			table_data, (char *)0 ) );

	form_prompt_process_drop_down->
		drop_down_appaserver_element =
			appaserver_element_new(
				prompt_drop_down,
				process_parameter_drop_down->drop_down_name );

	form_prompt_process_drop_down->
		drop_down_appaserver_element->
		prompt_drop_down =
			element_prompt_drop_down_new(
				form_prompt_process_drop_down->
				drop_down_appaserver_element->
					element_name,
				process_parameter_drop_down->
					delimited_list,
				0 /* not no_initial_capital */,
				0 /* not output_null_option */,
				0 /* not output_not_null_option */,
				1 /* output_select_option */,
				1 /* display_size */,
				-1 /* appaserver_element_tab_order() */,
				process_parameter_drop_down->
					drop_down_multi_select,
				post_change_javascript,
				1 /* recall */ );

	list_set(
		form_prompt_process_drop_down->element_list,
		form_prompt_process_drop_down->drop_down_appaserver_element );

	return form_prompt_process_drop_down;
}

FORM_PROMPT_PROCESS_DROP_DOWN *form_prompt_process_drop_down_calloc( void )
{
	FORM_PROMPT_PROCESS_DROP_DOWN *form_prompt_process_drop_down;

	if ( ! ( form_prompt_process_drop_down =
			calloc(	1,
				sizeof( FORM_PROMPT_PROCESS_DROP_DOWN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_process_drop_down;
}

FORM_PROMPT_PROCESS_ELEMENT_LIST *
	form_prompt_process_element_list_new(
			LIST *process_parameter_list,
			char *post_change_javascript )
{
	FORM_PROMPT_PROCESS_ELEMENT_LIST *form_prompt_process_element_list;
	PROCESS_PARAMETER *process_parameter;

	if ( !list_rewind( process_parameter_list ) )
	{
		return (FORM_PROMPT_PROCESS_ELEMENT_LIST *)0;
	}

	form_prompt_process_element_list =
		form_prompt_process_element_list_calloc();

	form_prompt_process_element_list->element_list = list_new();

	do {
		process_parameter =
			list_get(
				process_parameter_list );

		list_set(
			form_prompt_process_element_list->element_list,
			appaserver_element_new(
				table_row, (char *)0 ) );

		list_set(
			form_prompt_process_element_list->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

		if ( process_parameter->attribute )
		{
			form_prompt_process_element_list->
				form_prompt_process_attribute =
					form_prompt_process_attribute_new(
						post_change_javascript,
						process_parameter->
							attribute->
							attribute_name,
						process_parameter->
							attribute->
							datatype_name,
						process_parameter->
							attribute->
							width,
						process_parameter->
							attribute->
							hint_message );

			if ( !form_prompt_process_element_list->
				form_prompt_process_attribute )
			{
				fprintf(stderr,
"ERROR in %s/%s()/%d: form_prompt_process_attribute_new() returned empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			list_set_list(
				form_prompt_process_element_list->element_list,
				form_prompt_process_element_list->
					form_prompt_process_attribute->
					element_list );
		}
		else
		if ( process_parameter->process_parameter_drop_down )
		{
			form_prompt_process_element_list->
				form_prompt_process_drop_down =
				    form_prompt_process_drop_down_new(
					post_change_javascript,
					process_parameter->
						process_parameter_drop_down );

			if ( !form_prompt_process_element_list->
				form_prompt_process_drop_down )
			{
				fprintf(stderr,
"ERROR in %s/%s()/%d: form_prompt_process_drop_down_new() returned empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			list_set_list(
				form_prompt_process_element_list->
					element_list,
				form_prompt_process_element_list->
					form_prompt_process_drop_down->
						element_list );
		}

	} while ( list_next( process_parameter_list ) );

	form_prompt_process_element_list->appaserver_element_list_html =
		appaserver_element_list_html(
			form_prompt_process_element_list->element_list );

	return form_prompt_process_element_list;
}

FORM_PROMPT_PROCESS_ELEMENT_LIST *
	form_prompt_process_element_list_calloc(
			void )
{
	FORM_PROMPT_PROCESS_ELEMENT_LIST *form_prompt_process_element_list;

	if ( ! ( form_prompt_process_element_list =
			calloc(	1,
				sizeof( FORM_PROMPT_PROCESS_ELEMENT_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_process_element_list;
}

FORM_PROMPT_PROCESS *form_prompt_process_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name,
			LIST *process_parameter_list,
			char *post_change_javascript )
{
	FORM_PROMPT_PROCESS *form_prompt_process;
	char *tmp;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_or_set_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_prompt_process = form_prompt_process_calloc();

	form_prompt_process->action_string =
		form_prompt_process_action_string(
			POST_PROMPT_PROCESS_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			process_or_set_name );

	form_prompt_process->form_tag_html =
		form_tag_html(
			FORM_PROMPT_PROCESS_NAME /* form_name */,
			form_prompt_process->action_string,
			FRAMESET_TABLE_FRAME /* target_frame */ );

	form_prompt_process->form_prompt_process_element_list =
		form_prompt_process_element_list_new(
			process_parameter_list,
			post_change_javascript );

	form_prompt_process->form_multi_select_all_javascript =
		form_multi_select_all_javascript(
			form_prompt_process->
				form_prompt_process_element_list->
				element_list );

	if ( ! ( form_prompt_process->recall =
			recall_new(
				(char *)0 /* folder_name */,
				(char *)0 /* state */,
				FORM_PROMPT_PROCESS_NAME /* form_name */,
				form_prompt_process->
					form_prompt_process_element_list->
					element_list ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: recall_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_prompt_process->form_prompt_lookup_button_list =
		form_prompt_lookup_button_list(
			form_prompt_process->form_multi_select_all_javascript,
			form_prompt_process->recall->save_javascript,
			form_prompt_process->recall->load_javascript );

	form_prompt_process->html =
		form_prompt_lookup_html(
			form_prompt_process->form_tag_html,
			(char *)0 /* radio_list_html */,
			form_prompt_process->
				form_prompt_process_element_list->
				appaserver_element_list_html,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			( tmp = button_list_html(
				    form_prompt_process->
					form_prompt_lookup_button_list ) ),
			form_close_html() );

	free( form_prompt_process->
		form_prompt_process_element_list->
			appaserver_element_list_html );

	free( tmp );

	button_list_free(
		form_prompt_process->
			form_prompt_lookup_button_list );

	return form_prompt_process;
}

char *form_prompt_process_action_string(
			char *post_prompt_process_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name )
{
	char action_string[ 1024 ];

	if ( !post_prompt_process_executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_or_set_name )
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
		post_prompt_process_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		process_or_set_name );

	return strdup( action_string );
}

FORM_PROMPT_PROCESS *form_prompt_process_calloc( void )
{
	FORM_PROMPT_PROCESS *form_prompt_process;

	if ( ! ( form_prompt_process =
			calloc( 1, sizeof( FORM_PROMPT_PROCESS ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_process;
}

FORM_PROMPT_PROCESS_PROMPT *form_prompt_process_prompt_new(
			char *post_change_javascript,
			char *prompt_name,
			int input_width,
			char *hint_message,
			boolean upload_filename_boolean,
			boolean date_boolean )
{
	FORM_PROMPT_PROCESS_PROMPT *form_prompt_process_prompt;

	if ( !prompt_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: prompt_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_prompt_process_prompt = form_prompt_process_prompt_calloc();
	form_prompt_process_prompt->element_list = list_new();

	list_set(
		form_prompt_process_prompt->element_list,
		appaserver_element_new(
			non_edit_text, prompt_name ) );

	list_set(
		form_prompt_process_prompt->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

	if ( upload_filename_boolean )
	{
		form_prompt_process_prompt->text_appaserver_element =
			form_prompt_process_prompt_upload_filename_element(
				prompt_name );

		if ( !form_prompt_process_prompt->text_appaserver_element )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: form_prompt_process_prompt_upload_filename_element() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}
	}
	else
	if ( date_boolean )
	{
		form_prompt_process_prompt->text_appaserver_element =
			form_prompt_process_prompt_date_element(
				post_change_javascript,
				prompt_name );

		if ( !form_prompt_process_prompt->text_appaserver_element )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: form_prompt_process_prompt_date_element() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}
	}
	else
	{
		form_prompt_process_prompt->text_appaserver_element =
			form_prompt_process_prompt_text_element(
				post_change_javascript,
				prompt_name,
				input_width );

		if ( !form_prompt_process_prompt->text_appaserver_element )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: form_prompt_process_prompt_text_element() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}
	}

	list_set(
		form_prompt_process_prompt->element_list,
		form_prompt_process_prompt->text_appaserver_element );

	if ( hint_message )
	{
		list_set(
			form_prompt_process_prompt->element_list,
			appaserver_element_new( table_data, (char *)0 ) );

		list_set(
			form_prompt_process_prompt->element_list,
			( form_prompt_process_prompt->
				hint_message_appaserver_element =
					appaserver_element_new(
						non_edit_text, (char *)0 ) ) );

		free( form_prompt_process_prompt->
			hint_message_appaserver_element->
			non_edit_text );

		form_prompt_process_prompt->
			hint_message_appaserver_element->
			non_edit_text =
				element_non_edit_text_new(
					(char *)0 /* prompt_name */,
					hint_message );
	}

	return form_prompt_process_prompt;
}

FORM_PROMPT_PROCESS_PROMPT *form_prompt_process_prompt_calloc( void )
{
	FORM_PROMPT_PROCESS_PROMPT *form_prompt_process_prompt;

	if ( ! ( form_prompt_process_prompt =
			calloc( 1, sizeof( FORM_PROMPT_PROCESS_PROMPT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_process_prompt;
}

APPASERVER_ELEMENT *form_prompt_process_prompt_upload_filename_element(
			char *prompt_name )
{
	APPASERVER_ELEMENT *element;

	if ( !prompt_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: prompt_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	element =
		appaserver_element_new(
			upload,
			prompt_name );

	free( element->upload );

	element->upload =
		element_upload_new(
			element->element_name,
			-1 /* tab_order */,
			1 /* recall */ );

	return element;
}

APPASERVER_ELEMENT *form_prompt_process_prompt_date_element(
			char *post_change_javascript,
			char *prompt_name )
{
	APPASERVER_ELEMENT *element;

	if ( !prompt_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: prompt_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	element =
		appaserver_element_new(
			element_date,
			prompt_name );

	free( element->date );

	element->date =
		element_date_new(
			element->element_name,
			0 /* not null_to_slash */,
			post_change_javascript /* on_change */,
			-1 /* tab_order */,
			1 /* recall */ );

	return element;
}

APPASERVER_ELEMENT *form_prompt_process_prompt_text_element(
			char *post_change_javascript,
			char *prompt_name,
			int input_width )
{
	APPASERVER_ELEMENT *element;

	if ( !prompt_name
	||   !input_width )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	element =
		appaserver_element_new(
			text,
			prompt_name );

	free( element->text );

	element->text =
		element_text_new(
			element->element_name /* attribute_name */,
			(char *)0 /* datatype_name */,
			input_width /* attribute_width_max_length */,
			0 /* not null_to_slash */,
			1 /* prevent_carrot */,
			post_change_javascript /* on_change */,
			(char *)0 /* on_focus */,
			(char *)0 /* on_keyup */,
			-1 /* tab_order */,
			1 /* recall */ );

	return element;
}

