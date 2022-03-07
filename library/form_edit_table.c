/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_edit_table.c				*/
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
#include "form_edit_table.h"

FORM_EDIT_TABLE *form_edit_table_calloc( void )
{
	FORM_EDIT_TABLE *form_edit_table;

	if ( ! ( form_edit_table = calloc( 1, sizeof( FORM_EDIT_TABLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_edit_table;
}

FORM_EDIT_TABLE *form_edit_table_new(
			char *folder_name,
			char *javascript_replace,
			int dictionary_list_length,
			char *edit_table_submit_action_string,
			LIST *operation_list,
			LIST *edit_table_heading_name_list,
			char *target_frame,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			DICTIONARY *drillthru_dictionary,
			DICTIONARY *ignore_dictionary )
{
	FORM_EDIT_TABLE *form_edit_table = form_edit_table_calloc();

	form_edit_table->tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_edit_table_tag(
			edit_table_submit_action_string,
			target_frame );

	form_edit_table->top_button_element_list =
		form_edit_table_button_element_list(
			javascript_replace,
			0 /* dictionary_list_length */ );

	form_edit_table->top_button_element_list_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		appaserver_element_list_html(
			form_edit_table->top_button_element_list,
			(char *)0 /* application_name */,
			(char *)0 /* background_color */,
			(char *)0 /* state */,
			0 /* row_number */,
			(DICTIONARY *)0 /* row_dictionary */ );

	form_edit_table->bottom_button_element_list =
		form_edit_table_button_element_list(
			javascript_replace,
			dictionary_list_length );

	form_edit_table->bottom_button_element_list_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		appaserver_element_list_html(
			form_edit_table->bottom_button_element_list,
			(char *)0 /* application_name */,
			(char *)0 /* background_color */,
			(char *)0 /* state */,
			0 /* row_number */,
			(DICTIONARY *)0 /* row_dictionary */ );

	form_edit_table->sort_checkbox_element_list =
		form_edit_table_sort_checkbox_element_list(
			folder_name,
			list_length( operation_list ),
			edit_table_heading_name_list );

	form_edit_table->sort_checkbox_element_list_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		appaserver_element_list_html(
			form_edit_table->sort_checkbox_element_list,
			(char *)0 /* application_name */,
			(char *)0 /* background_color */,
			(char *)0 /* state */,
			0 /* row_number */,
			(DICTIONARY *)0 /* row_dictionary */ );

	form_edit_table->heading_element_list =
		form_edit_table_heading_element_list(
			operation_list,
			edit_table_heading_name_list );

	form_edit_table->heading_element_list_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		appaserver_element_list_html(
			form_edit_table->heading_element_list,
			(char *)0 /* application_name */,
			(char *)0 /* background_color */,
			(char *)0 /* state */,
			0 /* row_number */,
			(DICTIONARY *)0 /* row_dictionary */ );

	form_edit_table->query_dictionary_hidden_html =
		dictionary_separate_hidden_html(
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			query_dictionary );

	form_edit_table->sort_dictionary_hidden_html =
		dictionary_separate_hidden_html(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			sort_dictionary );

	form_edit_table->drillthru_dictionary_hidden_html =
		dictionary_separate_hidden_html(
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			drillthru_dictionary );

	form_edit_table->ignore_dictionary_hidden_html =
		dictionary_separate_hidden_html(
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			ignore_dictionary );

	form_edit_table->html =
		form_edit_table_html(
			form_edit_table->tag,
			form_edit_table->top_button_element_list_html,
			form_edit_table->sort_checkbox_element_list_html,
			form_edit_table->heading_element_list_html );

	form_edit_table->trailer_html =
		form_edit_table_trailer_html(
			form_edit_table->bottom_button_element_list_html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			element_table_close_html(),
			form_edit_table->query_dictionary_hidden_html,
			form_edit_table->sort_dictionary_hidden_html,
			form_edit_table->drillthru_dictionary_hidden_html,
			form_edit_table->ignore_dictionary_hidden_html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_html() );

	return form_edit_table;
}

LIST *form_edit_table_button_element_list(
			char *javascript_replace,
			int dictionary_list_length )
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
			(char *)0 /* keystrokes_save_javascript */,
			(char *)0 /* keystrokes_multi_save_javascript */,
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
			javascript_replace,
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

	/* Create <Top> */
	/* ------------ */
	if ( dictionary_list_length > 10 )
	{
		list_set(
			element_list,
			(element =
				appaserver_element_new(
					table_data,
					(char *)0 /* element_name */ ) ) );

		list_set(
			element_list,
			( element =
				appaserver_element_new(
					button,
					(char *)0 /* element_name */ ) ) );

		element->button->button = button_back_to_top();
	}

	list_set(
		element_list,
		( element = appaserver_element_new(
				table_close,
				(char *)0 /* element_name */ ) ) );

	return element_list;
}

LIST *form_edit_table_sort_checkbox_element_list(
			char *folder_name,
			int operation_list_length,
			LIST *edit_table_heading_name_list )
{
	APPASERVER_ELEMENT *element;
	LIST *element_list = list_new();
	char element_name[ 128 ];
	char action_string[ 128 ];
	int i;

	if ( !list_length( edit_table_heading_name_list ) ) return( LIST *)0;

	sprintf( action_string, "push_button_submit('%s')", folder_name );

	/* Create the assend checkboxes */
	/* ---------------------------- */
	list_set(
		element_list,
		( element = appaserver_element_new(
				table_row,
				(char *)0 /* element_name */ ) ) );

	for(	i = 0;
		i < operation_list_length;
		i++ )
	{
		list_set(
			element_list,
			( element =
				appaserver_element_new(
					table_data,
					(char *)0 /* element_name */ ) ) );
	}

	list_rewind( edit_table_heading_name_list );

	do {
		sprintf(element_name,
			"%s%s%s",
			DICTIONARY_SEPARATE_SORT_PREFIX,
			FORM_SORT_ASCEND_LABEL,
			(char *)list_get( edit_table_heading_name_list ) );

		list_set(
			element_list,
			( element =
				appaserver_element_new(
					checkbox,
					strdup( element_name ) ) ) );

		element->checkbox->element_name = element->element_name;
		element->checkbox->prompt_string = "Sort";
		element->checkbox->on_click = strdup( action_string );

	} while ( list_next( edit_table_heading_name_list ) );

	/* Create the descend checkboxes */
	/* ----------------------------- */
	list_set(
		element_list,
		( element = appaserver_element_new(
				table_row,
				(char *)0 /* element_name */ ) ) );

	for(	i = 0;
		i < operation_list_length;
		i++ )
	{
		list_set(
			element_list,
			( element =
				appaserver_element_new(
					table_data,
					(char *)0 /* element_name */ ) ) );
	}

	list_rewind( edit_table_heading_name_list );

	do {
		sprintf(element_name,
			"%s%s%s",
			DICTIONARY_SEPARATE_SORT_PREFIX,
			FORM_SORT_DESCEND_LABEL,
			(char *)list_get( edit_table_heading_name_list ) );

		list_set(
			element_list,
			( element =
				appaserver_element_new(
					checkbox,
					strdup( element_name ) ) ) );

		element->checkbox->element_name = element->element_name;
		element->checkbox->prompt_string = "Descend";
		element->checkbox->on_click = strdup( action_string );

	} while ( list_next( edit_table_heading_name_list ) );

	return element_list;
}

char *form_edit_table_html(
			char *form_edit_table_tag,
			char *top_button_element_list_html,
			char *sort_checkbox_element_list_html,
			char *heading_element_list_html )
{
	char html[ STRING_64K ];
	char *ptr = html;

	if ( form_edit_table_tag )
	{
		ptr += sprintf(
			ptr,
			"%s",
			form_edit_table_tag );
	}

	if ( top_button_element_list_html )
	{
		if ( ptr != html ) ptr += sprintf( ptr, "\n" );

		ptr += sprintf(
			ptr,
			"%s",
			top_button_element_list_html );
	}

	if ( sort_checkbox_element_list_html )
	{
		if ( ptr != html ) ptr += sprintf( ptr, "\n" );

		ptr += sprintf(
			ptr,
			"%s",
			sort_checkbox_element_list_html );
	}

	if ( heading_element_list_html )
	{
		if ( ptr != html ) ptr += sprintf( ptr, "\n" );

		ptr += sprintf(
			ptr,
			"%s",
			heading_element_list_html );
	}

	return strdup( html );
}

char *form_edit_table_trailer_html(
			char *bottom_button_element_list_html,
			char *element_table_close_html,
			char *query_dictionary_hidden_html,
			char *sort_dictionary_hidden_html,
			char *drillthru_dictionary_hidden_html,
			char *ignore_dictionary_hidden_html,
			char *form_close_html )
{
	char html[ STRING_ONE_MEG ];
	char *ptr = html;

	if ( bottom_button_element_list_html )
	{
		ptr += sprintf(
			ptr,
			"%s",
			bottom_button_element_list_html );
	}

	if ( element_table_close_html )
	{
		if ( ptr != html ) ptr += sprintf( ptr, "\n" );

		ptr += sprintf(
			ptr,
			"%s",
			element_table_close_html );
	}

	if ( query_dictionary_hidden_html )
	{
		if ( ptr != html ) ptr += sprintf( ptr, "\n" );

		ptr += sprintf(
			ptr,
			"%s",
			query_dictionary_hidden_html );
	}

	if ( sort_dictionary_hidden_html )
	{
		if ( ptr != html ) ptr += sprintf( ptr, "\n" );

		ptr += sprintf(
			ptr,
			"%s",
			sort_dictionary_hidden_html );
	}

	if ( drillthru_dictionary_hidden_html )
	{
		if ( ptr != html ) ptr += sprintf( ptr, "\n" );

		ptr += sprintf(
			ptr,
			"%s",
			drillthru_dictionary_hidden_html );
	}

	if ( ignore_dictionary_hidden_html )
	{
		if ( ptr != html ) ptr += sprintf( ptr, "\n" );

		ptr += sprintf(
			ptr,
			"%s",
			ignore_dictionary_hidden_html );
	}

	if ( form_close_html )
	{
		if ( ptr != html ) ptr += sprintf( ptr, "\n" );

		ptr += sprintf(
			ptr,
			"%s",
			form_close_html );
	}

	return strdup( html );
}

LIST *form_edit_table_heading_element_list(
			LIST *operation_list,
			LIST *heading_name_list )
{
	OPERATION *operation;
	char heading_string[ 128 ];
	APPASERVER_ELEMENT *element;
	LIST *element_list = list_new();

	list_set(
		element_list,
		appaserver_element_new(
			table_row,
			(char *)0 /* element_name */ ) );

	if ( list_rewind( operation_list ) )
	{
		do {
			operation = list_get( operation_list );

			if ( !operation->process )
			{
				fprintf(stderr,
				"ERROR in %s/%s()/%d: process is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}


			list_set(
				element_list,
				( element =
					appaserver_element_new(
					    table_heading,
					    (char *)0 /* element_name */ ) ) );

			element->table_heading->heading_string =
				strdup(
					string_initial_capital(
						heading_string,
						operation->
						    process->
						    process_name ) );

			list_set(
				element_list,
				appaserver_element_operation_table_heading(
					operation->process->process_name,
					operation->
						delete_warning_javascript ) );

		} while ( list_next( operation_list ) );
	}

	if ( list_rewind( heading_name_list ) )
	{
		do {
			list_set(
				element_list,
				appaserver_element_table_heading(
					list_get( heading_name_list ) ) );

		} while ( list_next( heading_name_list ) );
	}

	return element_list;
}

