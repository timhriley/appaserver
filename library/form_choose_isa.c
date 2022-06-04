/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_choose_isa.c				*/
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
#include "form.h"
#include "form_choose_isa.h"

FORM_CHOOSE_ISA *form_choose_isa_calloc( void )
{
	FORM_CHOOSE_ISA *form_choose_isa;

	if ( ! ( form_choose_isa = calloc( 1, sizeof( FORM_CHOOSE_ISA ) ) ) )
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

LIST *form_choose_isa_element_list(
			char *choose_isa_prompt_message,
			LIST *primary_key_list,
			LIST *delimited_list,
			boolean no_initial_capital )
{
	LIST *element_list = list_new();
	APPASERVER_ELEMENT *element;

	/* Create a table */
	/* -------------- */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_open,
				(char *)0 /* element_name */ ) ) );

	/* Create a table row */
	/* ------------------ */
	list_set(
		element_list,
		appaserver_element_new(
			table_row, (char *)0 ) );

	/* Create the choose is prompt */
	/* --------------------------- */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				non_edit_text,
				(char *)0 /* element_name */ ) ) );

	free( element->non_edit_text );

	element->non_edit_text =
		element_non_edit_text_new(
			(char *)0 /* attribute_name */,
			choose_isa_prompt_message );

	/* Create a drop-down */
	/* ------------------ */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				drop_down,
				(char *)0 /* element_name */ ) ) );

	free( element->drop_down );

	element->drop_down =
		element_drop_down_new(
			(char *)0 /* name */,
			primary_key_list /* attribute_name_list */,
			delimited_list,
			no_initial_capital,
			0 /* not output_null_option */,
			0 /* not output_not_null_option */,
			1 /* output_select_option */,
			1 /* display_size */,
			-1 /* tab order */,
			0 /* not multi_select */,
			(char *)0 /* post_change_javascript */,
			0 /* not readonly */,
			0 /* not recall */ );

	/* Create a table row */
	/* ------------------ */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_row,
				(char *)0 /* element_name */ ) ) );

	/* Create a lookup state checkbox */
	/* ------------------------------ */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				checkbox,
				(char *)0 /* element_name */ ) ) );

	free( element->checkbox );

	element->checkbox =
		element_checkbox_new(
			(char *)0 /* attribute_name */,
			ELEMENT_NAME_LOOKUP_STATE /* element_name */,
			"Lookup" /* prompt_string */,
			(char *)0 /* on_click */,
			-1 /* tab_order */,
			(char *)0 /* image_source */,
			0 /* not remember */ );

	/* Close the table */
	/* --------------- */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_close,
				(char *)0 /* element_name */ ) ) );

	return element_list;
}

LIST *form_choose_isa_button_element_list( void )
{
	APPASERVER_ELEMENT *element;
	LIST *element_list = list_new();

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_open,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_row,
				(char *)0 /* element_name */ ) ) );

	/* Create <Submit> */
	/* --------------- */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_data,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				button,
				(char *)0 /* element_name */ ) ) );

	element->button->button =
		button_submit(
			(char *)0 /* multi_select_all_javascript */,
			(char *)0 /* recall_save_javascript */,
			(char *)0 /* verify_attribute_widths_javascript */,
			0 /* form_number */ );

	/* Create <Reset> */
	/* -------------- */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_data,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				button,
				(char *)0 /* element_name */ ) ) );

	element->button->button =
		button_reset(
			(char *)0 /* javascript_replace */,
			0 /* form_number */ );

	/* Create <Back> */
	/* ------------- */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_data,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				button,
				(char *)0 /* element_name */ ) ) );

	element->button->button = button_back();

	/* Create <Forward> */
	/* ---------------- */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_data,
				(char *)0 /* element_name */ ) ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				button,
				(char *)0 /* element_name */ ) ) );

	element->button->button = button_forward();

	list_set(
		element_list,
		( element = appaserver_element_new(
				table_close,
				(char *)0 /* element_name */ ) ) );

	return element_list;
}

FORM_CHOOSE_ISA *form_choose_isa_new(
			char *prompt_message,
			LIST *primary_key_list,
			LIST *delimited_list,
			boolean no_initial_capital,
			char *post_action_string )
{
	FORM_CHOOSE_ISA *form_choose_isa = form_choose_isa_calloc();

	form_choose_isa->tag_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_tag_html(
			"choose_isa" /* form_name */,
			post_action_string,
			FRAMESET_PROMPT_FRAME /* target_frame */ );

	form_choose_isa->element_list =
		form_choose_isa_element_list(
			prompt_message,
			primary_key_list,
			delimited_list,
			no_initial_capital );

	form_choose_isa->button_element_list =
		form_choose_isa_button_element_list();

	form_choose_isa->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_choose_isa_html(
			form_choose_isa->tag_html,
			form_choose_isa->element_list,
			form_choose_isa->button_element_list,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_html() );

	return form_choose_isa;
}

char *form_choose_isa_html(
			char *tag_html,
			LIST *element_list,
			LIST *button_element_list,
			char *form_close_html )
{
	char html[ STRING_ONE_MEG ];
	char *tmp1;
	char *tmp2;

	if ( !tag_html
	||   !list_length( element_list )
	||   !list_length( button_element_list )
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
		tag_html,
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		( tmp1 = appaserver_element_list_html(
				element_list ) ),
		( tmp2 = appaserver_element_list_html(
				button_element_list ) ),
		form_close_html );

		free( tmp1 );
		free( tmp2 );

	return strdup( html );
}

