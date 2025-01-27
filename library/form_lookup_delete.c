/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_lookup_delete.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "dictionary_separate.h"
#include "button.h"
#include "frameset.h"
#include "form.h"
#include "form_lookup_delete.h"

FORM_LOOKUP_DELETE *form_lookup_delete_new(
		char *lookup_delete_execute_label,
		DICTIONARY *query_dictionary,
		char *lookup_delete_action_string )
{
	FORM_LOOKUP_DELETE *form_lookup_delete;
	WIDGET_CONTAINER *widget_container;

	if ( !lookup_delete_execute_label
	||   !lookup_delete_action_string )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_lookup_delete = form_lookup_delete_calloc();

	form_lookup_delete->widget_container_list = list_new();

	list_set(
		form_lookup_delete->widget_container_list,
		( widget_container = widget_container_new(
			table_open, (char *)0 ) ) );

	widget_container->table_open->border_boolean = 1;

	list_set(
		form_lookup_delete->widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	list_set(
		form_lookup_delete->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	form_lookup_delete->widget_confirm_checkbox_container =
		widget_confirm_checkbox_container(
			lookup_delete_execute_label,
			"Execute" /* checkbox_prompt */ );

	list_set(
		form_lookup_delete->widget_container_list,
		form_lookup_delete->widget_confirm_checkbox_container );

	list_set(
		form_lookup_delete->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		form_lookup_delete->widget_container_list,
		widget_container_new(
			non_edit_text,
			FORM_LOOKUP_DELETE_OMIT_MESSAGE ) );

	list_set(
		form_lookup_delete->widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	list_set(
		form_lookup_delete->widget_container_list,
			widget_container_new(
			table_data, (char *)0 ) );

	form_lookup_delete->widget_submit_button_container =
		widget_submit_button_container();

	list_set(
		form_lookup_delete->widget_container_list,
		form_lookup_delete->widget_submit_button_container );

	list_set(
		form_lookup_delete->widget_container_list,
		widget_container_new(
			table_close, (char *)0 ) );

	form_lookup_delete->widget_container_list_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		widget_container_list_html(
			(char *)0 /* state */,
			-1 /* row_number */,
			(char *)0 /* background_color */,
			form_lookup_delete->widget_container_list );

	form_lookup_delete->form_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_tag(
			FORM_LOOKUP_DELETE_NAME,
			lookup_delete_action_string,
			FRAMESET_TABLE_FRAME /* target_frame */ );

	form_lookup_delete->query_dictionary_hidden_html =
		dictionary_separate_hidden_html(
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			query_dictionary );

	form_lookup_delete->form_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_html(
			form_lookup_delete->form_tag,
			form_lookup_delete->widget_container_list_html,
			form_lookup_delete->query_dictionary_hidden_html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_tag() );

	return form_lookup_delete;
}

FORM_LOOKUP_DELETE *form_lookup_delete_calloc( void )
{
	FORM_LOOKUP_DELETE *form_lookup_delete;

	if ( ! ( form_lookup_delete =
			calloc( 1,
				sizeof ( FORM_LOOKUP_DELETE ) ) ) )
	{
		char message[ 256 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return form_lookup_delete;
}

