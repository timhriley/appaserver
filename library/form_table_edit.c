/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_table_edit.c				*/
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
#include "role_operation.h"
#include "form.h"
#include "form_table_edit.h"

FORM_TABLE_EDIT *form_table_edit_calloc( void )
{
	FORM_TABLE_EDIT *form_table_edit;

	if ( ! ( form_table_edit = calloc( 1, sizeof( FORM_TABLE_EDIT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_table_edit;
}

FORM_TABLE_EDIT *form_table_edit_new(
			char *folder_name,
			char *post_change_javascript,
			int dictionary_list_length,
			char *post_table_edit_action_string,
			LIST *role_operation_list,
			LIST *table_edit_heading_name_list,
			char *target_frame,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			DICTIONARY *drillthru_dictionary,
			DICTIONARY *no_display_dictionary )
{
	FORM_TABLE_EDIT *form_table_edit = form_table_edit_calloc();

	form_table_edit->tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_table_edit_tag(
			FORM_TABLE_EDIT_NAME,
			post_table_edit_action_string,
			target_frame );

	form_table_edit->top_button_element_list =
		form_table_edit_button_element_list(
			post_change_javascript,
			0 /* dictionary_list_length */ );

	if ( dictionary_list_length > 10 )
	{
		form_table_edit->top_button_element_list_html =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			appaserver_element_list_html(
				form_table_edit->
					top_button_element_list );
	}

	form_table_edit->bottom_button_element_list =
		form_table_edit_button_element_list(
			post_change_javascript,
			dictionary_list_length );

	form_table_edit->bottom_button_element_list_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		appaserver_element_list_html(
			form_table_edit->
				bottom_button_element_list );

	form_table_edit->sort_checkbox_element_list =
		form_table_edit_sort_checkbox_element_list(
			folder_name,
			list_length( role_operation_list ),
			table_edit_heading_name_list );

	form_table_edit->sort_checkbox_element_list_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		appaserver_element_list_html(
			form_table_edit->sort_checkbox_element_list );

	form_table_edit->heading_element_list =
		form_table_edit_heading_element_list(
			role_operation_list,
			table_edit_heading_name_list );

	form_table_edit->heading_element_list_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		appaserver_element_list_html(
			form_table_edit->heading_element_list );

	form_table_edit->query_dictionary_hidden_html =
		dictionary_separate_hidden_html(
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			query_dictionary );

	form_table_edit->sort_dictionary_hidden_html =
		dictionary_separate_hidden_html(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			sort_dictionary );

	form_table_edit->drillthru_dictionary_hidden_html =
		dictionary_separate_hidden_html(
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			drillthru_dictionary );

	form_table_edit->no_display_dictionary_hidden_html =
		dictionary_separate_hidden_html(
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			no_display_dictionary );

	form_table_edit->html =
		form_table_edit_html(
			form_table_edit->tag,
			form_table_edit->top_button_element_list_html,
			element_table_open_html(
				1 /* border_boolean */ ),
			form_table_edit->sort_checkbox_element_list_html,
			form_table_edit->heading_element_list_html );

	form_table_edit->trailer_html =
		form_table_edit_trailer_html(
			form_table_edit->bottom_button_element_list_html,
			form_table_edit->query_dictionary_hidden_html,
			form_table_edit->sort_dictionary_hidden_html,
			form_table_edit->drillthru_dictionary_hidden_html,
			form_table_edit->no_display_dictionary_hidden_html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_html() );

	return form_table_edit;
}

LIST *form_table_edit_button_element_list(
			char *post_change_javascript,
			int dictionary_list_length )
{
	APPASERVER_ELEMENT *element;
	LIST *element_list = list_new();

	list_set(
		element_list,
		appaserver_element_new(
			table_open, (char *)0 ) );

	list_set(
		element_list,
		appaserver_element_new(
			table_row, (char *)0 ) );

	/* Create <Submit> */
	/* --------------- */
	list_set(
		element_list,
		appaserver_element_new(
			table_data, (char *)0 ) );

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
		appaserver_element_new(
			table_data, (char *)0 ) );

	list_set(
		element_list,
		( element =
			appaserver_element_new(
				button,
				(char *)0 /* element_name */ ) ) );

	element->button->button =
		button_reset(
			post_change_javascript,
			0 /* form_number */ );

	/* Create <Back> */
	/* ------------- */
	list_set(
		element_list,
		appaserver_element_new(
			table_data, (char *)0 ) );

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
		appaserver_element_new(
			table_data, (char *)0 ) );

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
			appaserver_element_new(
				table_data, (char *)0 ) );

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
		appaserver_element_new(
			table_close, (char *)0 ) );

	return element_list;
}

LIST *form_table_edit_sort_checkbox_element_list(
			char *folder_name,
			int operation_list_length,
			LIST *table_edit_heading_name_list )
{
	APPASERVER_ELEMENT *element;
	LIST *element_list = list_new();
	char element_name[ 128 ];
	char on_click[ 128 ];
	int i;

	if ( !list_length( table_edit_heading_name_list ) ) return( LIST *)0;

	sprintf( on_click, "push_button_submit('%s')", folder_name );

	list_set(
		element_list,
		appaserver_element_new(
			table_row, (char *)0 ) );

	/* Placeholders for the operations */
	/* ------------------------------- */
	for(	i = 0;
		i < operation_list_length;
		i++ )
	{
		list_set(
			element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );
	}

	list_rewind( table_edit_heading_name_list );

	do {
		list_set(
			element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

		sprintf(element_name,
			"%s%s%s",
			DICTIONARY_SEPARATE_SORT_PREFIX,
			FORM_SORT_ASCEND_LABEL,
			(char *)list_get( table_edit_heading_name_list ) );

		list_set(
			element_list,
			( element =
				appaserver_element_new(
					checkbox,
					strdup( element_name ) ) ) );

		element->checkbox->element_name = element->element_name;
		element->checkbox->prompt_string = "Sort";
		element->checkbox->on_click = strdup( on_click );

	} while ( list_next( table_edit_heading_name_list ) );

	/* Create the descend checkboxes */
	/* ----------------------------- */
	list_set(
		element_list,
		appaserver_element_new(
			table_row, (char *)0 ) );

	/* Placeholders for the operations */
	/* ------------------------------- */
	for(	i = 0;
		i < operation_list_length;
		i++ )
	{
		list_set(
			element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );
	}

	list_rewind( table_edit_heading_name_list );

	do {
		list_set(
			element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

		sprintf(element_name,
			"%s%s%s",
			DICTIONARY_SEPARATE_SORT_PREFIX,
			FORM_SORT_DESCEND_LABEL,
			(char *)list_get( table_edit_heading_name_list ) );

		list_set(
			element_list,
			( element =
				appaserver_element_new(
					checkbox,
					strdup( element_name ) ) ) );

		element->checkbox->element_name = element->element_name;
		element->checkbox->prompt_string = "Descend";
		element->checkbox->on_click = strdup( on_click );

	} while ( list_next( table_edit_heading_name_list ) );

	return element_list;
}

char *form_table_edit_html(
			char *form_table_edit_tag,
			char *top_button_element_list_html,
			char *element_table_open_html,
			char *sort_checkbox_element_list_html,
			char *heading_element_list_html )
{
	char html[ STRING_64K ];
	char *ptr = html;

	if ( !form_table_edit_tag
	||   !element_table_open_html
	||   !heading_element_list_html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"%s",
		form_table_edit_tag );

	if ( top_button_element_list_html )
	{
		ptr += sprintf(
			ptr,
			"\n%s",
			top_button_element_list_html );
	}

	ptr += sprintf(
		ptr,
		"\n%s",
		element_table_open_html );

	if ( sort_checkbox_element_list_html )
	{
		ptr += sprintf(
			ptr,
			"\n%s",
			sort_checkbox_element_list_html );
	}

	if ( heading_element_list_html )
	{
		ptr += sprintf(
			ptr,
			"\n%s",
			heading_element_list_html );
	}

	return strdup( html );
}

char *form_table_edit_trailer_html(
			char *bottom_button_element_list_html,
			char *query_dictionary_hidden_html,
			char *sort_dictionary_hidden_html,
			char *drillthru_dictionary_hidden_html,
			char *no_display_dictionary_hidden_html,
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

	if ( no_display_dictionary_hidden_html )
	{
		if ( ptr != html ) ptr += sprintf( ptr, "\n" );

		ptr += sprintf(
			ptr,
			"%s",
			no_display_dictionary_hidden_html );
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

LIST *form_table_edit_heading_element_list(
			LIST *role_operation_list,
			LIST *heading_name_list )
{
	ROLE_OPERATION *role_operation;
	APPASERVER_ELEMENT *element;
	char heading_string[ 128 ];
	LIST *element_list = list_new();

	list_set(
		element_list,
		appaserver_element_new(
			table_row, (char *)0 ) );

	if ( list_rewind( role_operation_list ) )
	{
		do {
			role_operation = list_get( role_operation_list );

			if ( !role_operation->operation )
			{
				fprintf(stderr,
			"ERROR in %s/%s()/%d: operation is empty.\n",
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
						role_operation->
						    operation->
						    operation_name ) );

			list_set(
				element_list,
				form_table_edit_operation_checkbox_element(
					role_operation->
						operation->
						operation_name,
					role_operation->
						operation->
						delete_warning_javascript ) );

		} while ( list_next( role_operation_list ) );
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

char *form_table_edit_tag(
			char *form_table_edit_name,
			char *post_table_edit_action_string,
			char *target_frame )
{
	char tag[ 1024 ];
	char *ptr = tag;

	if ( !form_table_edit_name
	||   !post_table_edit_action_string
	||   !target_frame )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"<form enctype=\"multipart/form-data\" method=post" );

	ptr += sprintf(
		ptr,
		" name=\"%s\"",
		form_table_edit_name );

	ptr += sprintf(
		ptr,
		" action=\"%s\" target=\"%s\">",
		post_table_edit_action_string,
		target_frame );

	return strdup( tag );
}

APPASERVER_ELEMENT *form_table_edit_operation_checkbox_element(
			char *operation_name,
			char *delete_warning_javascript )
{
	APPASERVER_ELEMENT *element;
	char on_click[ 128 ];

	if ( !operation_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: operation_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	element =
		appaserver_element_new(
			checkbox,
			(char *)0 /* element_name */ );

	if ( delete_warning_javascript )
	{
		sprintf(on_click, 
			"%s && table_edit_push_button_set_all('%s',0);",
			delete_warning_javascript,
			operation_name );
	}
	else
	{
		sprintf(on_click, 
			"form_push_button_set_all('%s',0);",
			operation_name );
	}

	element->checkbox->element_name = operation_name;
	element->checkbox->on_click = strdup( on_click );

	return element;
}

LIST *form_table_edit_operation_element_list(
			LIST *role_operation_list,
			boolean viewonly,
			char *form_delete_warning_javascript )
{
	LIST *element_list = list_new();
	ROLE_OPERATION *role_operation;
	APPASERVER_ELEMENT *element;

	if ( !list_rewind( role_operation_list ) )
		return element_list;

	do {
		role_operation = 
			list_get(
				role_operation_list );

		list_set(
			element_list,
			appaserver_element_new(
				table_data,
				(char *)0 /* name */ ) );

		if ( viewonly
		&&   operation_delete_boolean(
			role_operation->operation->operation_name ) )
		{
			continue;
		}

		element =
			appaserver_element_new(
				checkbox,
				role_operation->operation->operation_name
					/* element_name */ );

		element->checkbox->element_name =
			role_operation->operation->operation_name;

		element->checkbox->prompt_string =
			role_operation->operation->operation_name;

		if ( operation_delete_boolean(
			role_operation->
				operation->
				operation_name ) )
		{
			element->checkbox->on_click =
				form_delete_warning_javascript;
		}

		list_set( element_list, element );

	} while( list_next( role_operation_list ) );

	return element_list;
}

