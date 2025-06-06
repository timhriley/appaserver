/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_table_edit.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
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
#include "appaserver_parameter.h"
#include "dictionary_separate.h"
#include "pair_one2m.h"
#include "widget.h"
#include "javascript.h"
#include "frameset.h"
#include "button.h"
#include "table.h"
#include "appaserver.h"
#include "folder_operation.h"
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
		char *target_frame,
		char *post_change_javascript,
		DICTIONARY *query_dictionary,
		DICTIONARY *drillthru_dictionary,
		DICTIONARY *no_display_dictionary,
		LIST *folder_operation_list,
		int table_edit_query_row_list_length,
		char *table_edit_post_action_string,
		LIST *table_edit_heading_label_list,
		LIST *table_edit_heading_name_list )
{
	FORM_TABLE_EDIT *form_table_edit;

	if ( !folder_name
	||   !target_frame
	||   !table_edit_post_action_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: drillthru_dictionary=[%s]\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	dictionary_display( drillthru_dictionary ) );
msg( (char *)0, message );
}
	form_table_edit = form_table_edit_calloc();

	form_table_edit->form_increment_number =
		form_increment_number();

	form_table_edit->form_name =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_name(
			FORM_TABLE_EDIT_LABEL,
			form_table_edit->form_increment_number );

	form_table_edit->top_button_widget_container_list =
		form_table_edit_button_widget_container_list(
			form_table_edit->form_name,
			post_change_javascript,
			0 /* not button_to_top_boolean */ );

	if ( table_edit_query_row_list_length > 10 )
	{
		form_table_edit->bottom_button_widget_container_list =
			form_table_edit_button_widget_container_list(
				form_table_edit->form_name,
				post_change_javascript,
				1 /* button_to_top_boolean */ );
	}

	form_table_edit->sort_buttons_boolean =
		form_table_edit_sort_buttons_boolean(
			FORM_TABLE_EDIT_MINIMUM_SORT,
			table_edit_query_row_list_length );

	if ( form_table_edit->sort_buttons_boolean )
	{
		form_table_edit->sort_container_list =
			form_table_edit_sort_container_list(
				form_table_edit->form_name,
				list_length( folder_operation_list )
					/* folder_operation_list_length */,
				table_edit_heading_name_list );
	}

	form_table_edit->heading_container_list =
		form_table_edit_heading_container_list(
			no_display_dictionary,
			folder_operation_list,
			table_edit_heading_label_list,
			form_table_edit->form_name );

	form_table_edit->query_dictionary_hidden_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		dictionary_separate_hidden_html(
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			query_dictionary );

	form_table_edit->drillthru_dictionary_hidden_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		dictionary_separate_hidden_html(
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			drillthru_dictionary );

	form_table_edit->no_display_dictionary_hidden_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		dictionary_separate_hidden_html(
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			no_display_dictionary );

	form_table_edit->form_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_tag(
			form_table_edit->form_name,
			table_edit_post_action_string,
			target_frame );

	form_table_edit->heading_container_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_table_edit_heading_container_string(
			form_table_edit->heading_container_list );

	form_table_edit->open_html =
		form_table_edit_open_html(
			form_table_edit->form_tag,
			form_table_edit->top_button_widget_container_list,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			widget_table_open_tag(
				1 /* border_boolean */,
				0 /* cell_spacing */,
				0 /* cell_padding */ ),
			form_table_edit->sort_container_list,
			form_table_edit->heading_container_string );

	form_table_edit->close_html =
		form_table_edit_close_html(
			form_table_edit->bottom_button_widget_container_list,
			form_table_edit->query_dictionary_hidden_html,
			form_table_edit->drillthru_dictionary_hidden_html,
			form_table_edit->no_display_dictionary_hidden_html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_tag() );

	if ( list_length( folder_operation_list ) )
	{
		form_table_edit->blank_folder_operation_list =
			form_table_edit_blank_folder_operation_list(
				list_length( folder_operation_list ) );

		form_table_edit->heading_container_list =
			form_table_edit_heading_container_list(
				no_display_dictionary,
				form_table_edit->blank_folder_operation_list,
				table_edit_heading_label_list,
				form_table_edit->form_name );

		free( form_table_edit->heading_container_string );

		form_table_edit->heading_container_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			form_table_edit_heading_container_string(
				form_table_edit->heading_container_list );
	}

	return form_table_edit;
}

LIST *form_table_edit_button_widget_container_list(
		char *form_name,
		char *post_change_javascript,
		boolean button_to_top_boolean )
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
				button,
				(char *)0 /* widget_name */ ) ) );

	widget_container->widget_button->button =
		/* --------------------------------- */
		/* Safely returns everything in heap */
		/* --------------------------------- */
		button_submit(
			form_name,
			(char *)0 /* multi_select_all_javascript */,
			(char *)0 /* recall_save_javascript */,
			(char *)0 /* verify_attribute_widths_javascript */ );

	list_set(
		widget_container_list,
		( widget_container =
			widget_container_new(
				button,
				(char *)0 /* widget_name */ ) ) );

	widget_container->widget_button->button =
		/* --------------------------------- */
		/* Safely returns everything in heap */
		/* --------------------------------- */
		button_reset(
			form_name,
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			javascript_replace(
				post_change_javascript,
				APPASERVER_UPDATE_STATE,
				0 /* row_number */ ) );

	list_set(
		widget_container_list,
		( widget_container =
			widget_container_new(
				button,
				(char *)0 /* widget_name */ ) ) );

	widget_container->widget_button->button = button_back();

	list_set(
		widget_container_list,
		( widget_container =
			widget_container_new(
				button,
				(char *)0 /* widget_name */ ) ) );

	widget_container->widget_button->button = button_forward();

	if ( button_to_top_boolean )
	{
		list_set(
			widget_container_list,
			( widget_container =
				widget_container_new(
					button,
					(char *)0 /* widgetname */ ) ) );

		widget_container->widget_button->button = button_to_top();
	}

	list_set(
		widget_container_list,
		widget_container_new(
			table_close, (char *)0 ) );

	return widget_container_list;
}

LIST *form_table_edit_sort_container_list(
		char *form_name,
		int folder_operation_list_length,
		LIST *table_edit_heading_name_list )
{
	WIDGET_CONTAINER *widget_container;
	LIST *widget_container_list;
	char widget_name[ 512 ];
	static char onclick[ 128 ];
	int i;

	if ( !form_name )
	{
		char message[ 128 ];

		sprintf(message, "form_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( table_edit_heading_name_list ) ) return( LIST *)0;

	widget_container_list = list_new();

	snprintf(
		onclick,
		sizeof ( onclick ),
		"%s && push_button_submit('%s');",
		JAVASCRIPT_WAIT_START,
		form_name );

	list_set(
		widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	/* Placeholders for the operations */
	/* ------------------------------- */
	for(	i = 0;
		i < folder_operation_list_length;
		i++ )
	{
		list_set(
			widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );
	}

	list_rewind( table_edit_heading_name_list );

	do {
		list_set(
			widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

		sprintf(widget_name,
			"%s%s",
			DICTIONARY_SEPARATE_SORT_PREFIX,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			form_table_edit_heading_name(
				SQL_DELIMITER,
				FORM_SORT_ASCEND_LABEL,
				(char *)list_get(
					table_edit_heading_name_list ) ) );

		list_set(
			widget_container_list,
			( widget_container =
				widget_container_new(
					checkbox,
					strdup( widget_name ) ) ) );

		widget_container->checkbox->prompt = "Sort";
		widget_container->checkbox->onclick = onclick;

	} while ( list_next( table_edit_heading_name_list ) );

	/* Create the descend checkboxes */
	/* ----------------------------- */
	list_set(
		widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	/* Placeholders for the operations */
	/* ------------------------------- */
	for(	i = 0;
		i < folder_operation_list_length;
		i++ )
	{
		list_set(
			widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );
	}

	list_rewind( table_edit_heading_name_list );

	do {
		list_set(
			widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

		sprintf(widget_name,
			"%s%s",
			DICTIONARY_SEPARATE_SORT_PREFIX,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			form_table_edit_heading_name(
				SQL_DELIMITER,
				FORM_SORT_DESCEND_LABEL,
				(char *)list_get(
					table_edit_heading_name_list ) ) );

		list_set(
			widget_container_list,
			( widget_container =
				widget_container_new(
					checkbox,
					strdup( widget_name ) ) ) );

		widget_container->checkbox->prompt = "Descend";
		widget_container->checkbox->onclick = onclick;

	} while ( list_next( table_edit_heading_name_list ) );

	return widget_container_list;
}

char *form_table_edit_open_html(
		char *form_tag,
		LIST *top_button_widget_container_list,
		char *table_tag,
		LIST *sort_widget_container_list,
		char *heading_container_string )
{
	char html[ STRING_64K ];
	char *ptr = html;
	char *tmp;

	if ( !form_tag
	||   !table_tag
	||   !heading_container_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf( ptr, "%s\n", form_tag );

	if ( list_length( top_button_widget_container_list ) )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			( tmp =
				/* --------------------------- */
				/* Returns heap memory or null */
				/* --------------------------- */
				widget_container_list_html(
					(char *)0 /* state */,
					-1 /* row_number */,
					(char *)0 /* background_color */,
					top_button_widget_container_list ) ) );

		if ( !tmp )
		{
			char message[ 128 ];

			sprintf(message,
			"widget_container_list_html() returned empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		free( tmp );
	}

	ptr += sprintf(
		ptr,
		"%s\n",
		table_tag );

	if ( list_length( sort_widget_container_list ) )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			( tmp =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				widget_container_list_html(
					(char *)0 /* state */,
					-1 /* row_number */,
					(char *)0 /* background_color */,
					sort_widget_container_list ) ) );

		free( tmp );
	}

	ptr += sprintf(
		ptr,
		"%s\n",
		heading_container_string );

	return strdup( html );
}

char *form_table_edit_heading_container_string(
		LIST *heading_container_list )
{
	if ( !list_length( heading_container_list ) )
	{
		char message[ 128 ];

		sprintf(message, "heading_container_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	widget_container_list_html(
		(char *)0 /* state */,
		-1 /* row_number */,
		(char *)0 /* background_color */,
		heading_container_list );
}

char *form_table_edit_close_html(
		LIST *bottom_button_widget_container_list,
		char *query_dictionary_hidden_html,
		char *drillthru_dictionary_hidden_html,
		char *no_display_dictionary_hidden_html,
		char *form_close_tag )
{
	char html[ STRING_64K ];
	char *ptr = html;
	char *tmp;

	if ( !form_close_tag )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: form_close_tag is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_length( bottom_button_widget_container_list ) )
	{
		 tmp =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			widget_container_list_html(
				(char *)0 /* state */,
				-1 /* row_number */,
				(char *)0 /* background_color */,
			      bottom_button_widget_container_list );

		if (	strlen( html ) +
			strlen( tmp ) + 1 >= STRING_64K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_64K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf( ptr, "%s\n", tmp );

		free( tmp );
	}

	if ( query_dictionary_hidden_html )
	{
		if (	strlen( html ) +
			strlen( query_dictionary_hidden_html ) +
			1 >= STRING_64K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_64K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"%s\n",
			query_dictionary_hidden_html );
	}

	if ( drillthru_dictionary_hidden_html )
	{
		if (	strlen( html ) +
			strlen( drillthru_dictionary_hidden_html ) +
			1 >= STRING_64K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_64K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"%s\n",
			drillthru_dictionary_hidden_html );
	}

	if ( no_display_dictionary_hidden_html )
	{
		if (	strlen( html ) +
			strlen( no_display_dictionary_hidden_html ) +
			1 >= STRING_64K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_64K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"%s\n",
			no_display_dictionary_hidden_html );
	}

	if (	strlen( html ) +
		strlen( form_close_tag ) +
		1 >= STRING_64K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_64K );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf( ptr, "%s", form_close_tag );

	return strdup( html );
}

LIST *form_table_edit_heading_container_list(
		DICTIONARY *no_display_dictionary,
		LIST *folder_operation_list,
		LIST *heading_label_list,
		char *form_name )
{
	FOLDER_OPERATION *folder_operation;
	char heading_string[ 128 ];
	LIST *widget_container_list = list_new();
	char *heading_label;

	list_set(
		widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	if ( list_rewind( folder_operation_list ) )
	do {
		folder_operation = list_get( folder_operation_list );

		if ( !folder_operation->operation
		||   !folder_operation->operation->widget_container )
		{
			char message[ 128 ];

			sprintf(message,
			"folder_operation->operation is empty or incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set(
			widget_container_list,
			widget_container_new(
				table_heading, (char *)0 ) );

		list_set(
			widget_container_list,
			form_table_edit_heading_checkbox_container(
				form_name,
				folder_operation->
					operation->
					operation_name,
				folder_operation->
					operation->
					widget_container,
				folder_operation->
					operation->
					delete_warning_javascript ) );

	} while ( list_next( folder_operation_list ) );

	if ( list_rewind( heading_label_list ) )
	do {
		heading_label = list_get( heading_label_list );

		if ( dictionary_key_boolean(
			no_display_dictionary,
			heading_label ) )
		{
			continue;
		}

		list_set(
			widget_container_list,
			widget_container_new(
				table_heading,
				strdup(
				     string_initial_capital(
					heading_string,
					heading_label ) ) ) );

	} while ( list_next( heading_label_list ) );

	return widget_container_list;
}

WIDGET_CONTAINER *form_table_edit_heading_checkbox_container(
		char *form_name,
		char *operation_name,
		WIDGET_CONTAINER *input_widget_container,
		char *delete_warning_javascript )
{
	WIDGET_CONTAINER *widget_container;
	char onclick[ 128 ];

	if ( !form_name
	||   !input_widget_container )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( input_widget_container->widget_type == blank )
	{
		return
		widget_container_new(
			blank,
			(char *)0 /* widget_name */ );
	}

	if ( !input_widget_container->checkbox )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"input_widget_container->checkbox is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	widget_container =
		widget_container_new(
			checkbox,
			operation_name
				/* widget_name */ );

	widget_container->
		checkbox->
		prompt =
			/* ------------------------ */
			/* Returns either parameter */
			/* ------------------------ */
			form_table_edit_heading_checkbox_string(
				operation_name,
				input_widget_container->
					checkbox->
					prompt );

	if ( delete_warning_javascript )
	{
		snprintf(
			onclick,
			sizeof ( onclick ),
			"%s && form_push_button_set_all('%s','%s');",
			delete_warning_javascript,
			form_name,
			operation_name );
	}
	else
	{
		snprintf(
			onclick,
			sizeof ( onclick ),
			"form_push_button_set_all('%s','%s');",
			form_name,
			operation_name );
	}

	widget_container->checkbox->onclick = strdup( onclick );

	return widget_container;
}

boolean form_table_edit_sort_buttons_boolean(
		int form_table_edit_minimum_sort,
		int table_edit_query_row_list_length )
{
	return
	( table_edit_query_row_list_length >= form_table_edit_minimum_sort );
}

char *form_table_edit_heading_name(
		char sql_delimiter,
		char *form_sort_label,
		char *heading_name )
{
	static char name[ 256 ];
	char *ptr = name;
	int p;
	char piece_buffer[ 64 ];
	char name_buffer[ 128 ];

	for (	p = 0;
		piece(
			piece_buffer,
			sql_delimiter,
			heading_name,
			p );
		p++ )
	{
		if ( ptr != name ) ptr += sprintf( ptr, "%c", sql_delimiter );

		sprintf(name_buffer,
			"%s%s",
			form_sort_label,
			piece_buffer );

		ptr += sprintf(
			ptr,
			"%s",
			name_buffer );
	}

	return name;
}

char *form_table_edit_heading_checkbox_string(
		char *operation_name,
		char *checkbox_prompt )
{
	if ( checkbox_prompt )
		return checkbox_prompt;
	else
		return operation_name;
}

LIST *form_table_edit_blank_folder_operation_list(
		int folder_operation_list_length )
{
	LIST *folder_operation_list;
	int i;
	FOLDER_OPERATION *folder_operation;

	if ( !folder_operation_list_length ) return NULL;

	folder_operation_list = list_new();

	for (	i = 0;
		i < folder_operation_list_length;
		i++ )
	{
		folder_operation = folder_operation_calloc();
		folder_operation->operation = operation_blank_new();
		list_set( folder_operation_list, folder_operation );
	}

	return folder_operation_list;
}

