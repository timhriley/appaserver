/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_choose_isa.c			 	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "sql.h"
#include "appaserver_error.h"
#include "button.h"
#include "form.h"
#include "form_choose_isa.h"

FORM_CHOOSE_ISA *form_choose_isa_calloc( void )
{
	FORM_CHOOSE_ISA *form_choose_isa;

	if ( ! ( form_choose_isa = calloc( 1, sizeof ( FORM_CHOOSE_ISA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_choose_isa;
}

LIST *form_choose_isa_button_widget_container_list( void )
{
	WIDGET_CONTAINER *widget_container;
	LIST *widget_container_list = list_new();

	list_set(
		widget_container_list,
		widget_container_new(
			table_open, (char *)0 ) );

	list_set(
		widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	/* Create <Submit> */
	/* --------------- */
	list_set(
		widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		widget_container_list,
		( widget_container =
			widget_container_new(
				button, (char *)0 ) ) );

	widget_container->widget_button->button =
		button_submit(
			(char *)0 /* multi_select_all_javascript */,
			(char *)0 /* recall_save_javascript */,
			(char *)0 /* verify_attribute_widths_javascript */,
			0 /* form_number */ );

	/* Create <Reset> */
	/* -------------- */
	list_set(
		widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		widget_container_list,
		( widget_container =
			widget_container_new(
				button, (char *)0 ) ) );

	widget_container->widget_button->button =
		button_reset(
			(char *)0 /* javascript_replace */,
			0 /* form_number */ );

	/* Create <Back> */
	/* ------------- */
	list_set(
		widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		widget_container_list,
		( widget_container =
			widget_container_new(
				button, (char *)0 ) ) );

	widget_container->widget_button->button = button_back();

	/* Create <Forward> */
	/* ---------------- */
	list_set(
		widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		widget_container_list,
		( widget_container =
			widget_container_new(
				button, (char *)0 ) ) );

	widget_container->widget_button->button = button_forward();

	list_set(
		widget_container_list,
		widget_container_new(
			table_close, (char *)0 ) );

	return widget_container_list;
}

FORM_CHOOSE_ISA *form_choose_isa_new(
		const char *widget_lookup_checkbox_name,
		char *prompt_message,
		LIST *primary_key_list,
		LIST *delimited_list,
		boolean no_initial_capital,
		char *choose_isa_action_string,
		char *target_frame )
{
	FORM_CHOOSE_ISA *form_choose_isa;
	WIDGET_CONTAINER *widget_container;

	if ( !prompt_message
	||   !list_length( primary_key_list )
	||   !choose_isa_action_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_choose_isa = form_choose_isa_calloc();

	form_choose_isa->widget_container_list = list_new();

	list_set(
		form_choose_isa->widget_container_list,
		( widget_container =
		 	widget_container_new(
				table_open, (char *)0 ) ) );

	widget_container->
		table_open->
		border_boolean = 1;

	list_set(
		form_choose_isa->widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	list_set(
		form_choose_isa->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		form_choose_isa->widget_container_list,
		widget_container_new(
			non_edit_text,
			prompt_message ) );

	list_set(
		form_choose_isa->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	form_choose_isa->drop_down_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		form_choose_isa_drop_down_name(
			ATTRIBUTE_MULTI_KEY_DELIMITER,
			primary_key_list );

	list_set(
		form_choose_isa->widget_container_list,
		( form_choose_isa->drop_down_widget_container =
			widget_container_new(
				drop_down,
				form_choose_isa->drop_down_name
					/* widget_name */ ) ) );

	form_choose_isa->
		drop_down_widget_container->
		drop_down->
		widget_drop_down_option_list =
			widget_drop_down_option_list(
				SQL_DELIMITER,
				WIDGET_DROP_DOWN_LABEL_DELIMITER,
				WIDGET_DROP_DOWN_EXTRA_DELIMITER,
				WIDGET_DROP_DOWN_DASH_DELIMITER,
				delimited_list,
				no_initial_capital );

	form_choose_isa->
		drop_down_widget_container->
		drop_down->
		top_select_boolean = 1;

	list_set(
		form_choose_isa->widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	list_set(
		form_choose_isa->widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	form_choose_isa->widget_lookup_checkbox_container =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		widget_lookup_checkbox_container(
			(char *)widget_lookup_checkbox_name );

	list_set(
		form_choose_isa->widget_container_list,
		form_choose_isa->widget_lookup_checkbox_container );

	list_set(
		form_choose_isa->widget_container_list,
		widget_container_new(
			table_close, (char *)0 ) );

	form_choose_isa->button_widget_container_list =
		form_choose_isa_button_widget_container_list();

	list_set_list(
		form_choose_isa->widget_container_list,
		form_choose_isa->button_widget_container_list );

	form_choose_isa->form_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_tag(
			FORM_CHOOSE_ISA_NAME,
			choose_isa_action_string,
			target_frame );

	form_choose_isa->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_choose_isa_html(
			form_choose_isa->form_tag,
			form_choose_isa->widget_container_list,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_tag() );

	return form_choose_isa;
}

char *form_choose_isa_html(
		char *form_tag,
		LIST *widget_container_list,
		char *form_close_tag )
{
	char *container_list_html;
	char *html;

	if ( !form_tag
	||   !list_length( widget_container_list )
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

	container_list_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		widget_container_list_html(
			(char *)0 /* state */,
			-1,
			(char *)0 /* background_color */,
			widget_container_list );

	html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_html(
			form_tag,
			container_list_html,
			(char *)0 /* dictionary_hidden_html */,
			form_close_tag );

	free( container_list_html );
	return html;
}

char *form_choose_isa_drop_down_name(
		const char attribute_multi_key_delimiter,
		LIST *primary_key_list )
{
	if ( !list_length( primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "primary_key_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* ------------------------- */
	/* Returns heap memory or "" */
	/* ------------------------- */
	list_display_delimited(
		primary_key_list,
		(char)attribute_multi_key_delimiter );
}

