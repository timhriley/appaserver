/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_lookup_sort.c			 	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "widget.h"
#include "frameset.h"
#include "query.h"
#include "form.h"
#include "form_lookup_sort.h"

FORM_LOOKUP_SORT_TABLE *form_lookup_sort_table_new(
		LIST *folder_attribute_primary_key_list,
		char *lookup_sort_input_attribute_name,
		LIST *query_fetch_row_list )
{
	FORM_LOOKUP_SORT_TABLE *form_lookup_sort_table;
	WIDGET_CONTAINER *widget_container;
	int row_number;
	QUERY_ROW *query_row;
	char *primary_key;
	QUERY_CELL *query_cell;

	if ( !list_length( folder_attribute_primary_key_list )
	||   !lookup_sort_input_attribute_name
	||   !list_rewind( query_fetch_row_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_lookup_sort_table = form_lookup_sort_table_calloc();

	form_lookup_sort_table->widget_container_list = list_new();

	list_set(
		form_lookup_sort_table->widget_container_list,
		( widget_container =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			widget_container_new(
				table_open, (char *)0 ) ) );

	widget_container->table_open->border_boolean = 1;

	form_lookup_sort_table->heading_container_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_lookup_sort_table_heading_container_list(
			folder_attribute_primary_key_list,
			lookup_sort_input_attribute_name );

	list_set_list(
		form_lookup_sort_table->widget_container_list,
		form_lookup_sort_table->heading_container_list );

	form_lookup_sort_table->onclick =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		form_lookup_sort_table_onclick(
			folder_attribute_primary_key_list );

	row_number = 1;

	do {
		query_row = list_get( query_fetch_row_list );

		list_set(
			form_lookup_sort_table->widget_container_list,
			widget_container_new(
				table_row, (char *)0 ) );

		list_set(
			form_lookup_sort_table->widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

		list_set(
			form_lookup_sort_table->widget_container_list,
			( widget_container =
				widget_container_new(
					widget_radio, "move" ) ) );

		widget_container->radio->value =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			form_lookup_sort_table_label(
				row_number,
				"move" /* prefix */ );

		widget_container->radio->prompt = "Move";

		list_set(
			form_lookup_sort_table->widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

		list_set(
			form_lookup_sort_table->widget_container_list,
			( widget_container =
				widget_container_new(
					widget_radio, "here" ) ) );

		widget_container->radio->value =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			form_lookup_sort_table_label(
				row_number,
				"here" /* prefix */ );

		widget_container->radio->prompt = "Here";

		widget_container->radio->onclick =
			form_lookup_sort_table->onclick;

		list_rewind( query_row->cell_list );
		list_rewind( folder_attribute_primary_key_list );

		do {
			primary_key =
				list_get(
					folder_attribute_primary_key_list );

			list_set(
				form_lookup_sort_table->widget_container_list,
				widget_container_new(
					table_data, (char *)0 ) );

			list_set(
				form_lookup_sort_table->widget_container_list,
				( widget_container =
					widget_container_new(
						character,
						/* ------------------- */
						/* Returns heap memory */
						/* ------------------- */
						form_lookup_sort_table_label(
							row_number,
							primary_key ) ) ) );

			widget_container->
				text->
				attribute_width_max_length =
					FORM_LOOKUP_SORT_TEXT_WIDTH;

			widget_container->
				text->
				widget_text_display_size =
					FORM_LOOKUP_SORT_TEXT_DISPLAY_SIZE;

			query_cell = list_get( query_row->cell_list );

			widget_container->text->value_string =
				query_cell->select_datum;

			widget_container->text->viewonly = 1;

			list_next( query_row->cell_list );

		} while ( list_next( folder_attribute_primary_key_list ) );

		list_set(
			form_lookup_sort_table->widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

		list_set(
			form_lookup_sort_table->widget_container_list,
			( widget_container =
			    widget_container_new(
				character,
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				form_lookup_sort_table_label(
				     row_number,
				     lookup_sort_input_attribute_name ) ) ) );

		widget_container->
			text->
			attribute_width_max_length =
				FORM_LOOKUP_SORT_TEXT_DISPLAY_SIZE;

		widget_container->
			text->
			widget_text_display_size =
				FORM_LOOKUP_SORT_ORDER_DISPLAY_SIZE;

		query_cell = list_get( query_row->cell_list );

		widget_container->
			text->
			value_string =
				query_cell->select_datum;

		widget_container->text->viewonly = 1;
		row_number++;

	} while ( list_next( query_fetch_row_list ) );

	list_set(
		form_lookup_sort_table->widget_container_list,
		widget_container_new(
			table_close, (char *)0 ) );

	return form_lookup_sort_table;
}

FORM_LOOKUP_SORT_TABLE *form_lookup_sort_table_calloc( void )
{
	FORM_LOOKUP_SORT_TABLE *form_lookup_sort_table;

	if ( ! ( form_lookup_sort_table =
			calloc( 1,
				sizeof ( FORM_LOOKUP_SORT_TABLE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return form_lookup_sort_table;
}

char *form_lookup_sort_table_label(
		int row_number,
		char *prefix )
{
	char label[ 128 ];

	if ( !row_number
	||   !prefix )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(label,
		"%s_%d",
		prefix,
		row_number );

	return strdup( label );
}

LIST *form_lookup_sort_table_heading_container_list(
		LIST *folder_attribute_primary_key_list,
		char *lookup_sort_input_attribute_name )
{
	char *primary_key;
	LIST *widget_container_list;

	if ( !list_rewind( folder_attribute_primary_key_list )
	||   !lookup_sort_input_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_container_list = list_new();

	list_set(
		widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	list_set(
		widget_container_list,
		widget_container_new(
			table_heading, "Move" ) );

	list_set(
		widget_container_list,
		widget_container_new(
			table_heading, "Here" ) );

	do {
		primary_key = list_get( folder_attribute_primary_key_list );

		list_set(
			widget_container_list,
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			widget_container_new(
				table_heading,
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				form_lookup_sort_table_primary_heading(
					primary_key ) ) );

	} while ( list_next( folder_attribute_primary_key_list ) );

	list_set(
		widget_container_list,
		widget_container_new(
			table_heading,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			form_lookup_sort_table_attribute_heading(
				lookup_sort_input_attribute_name ) ) );

	return widget_container_list;
}

char *form_lookup_sort_table_primary_heading( char *primary_key )
{
	char heading[ 128 ];
	char destination[ 128 ];

	if ( !primary_key )
	{
		char message[ 128 ];

		sprintf(message, "primary_key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(heading,
		"*%s",
		string_initial_capital(
			destination,
			primary_key ) );

	return strdup( heading );
}

char *form_lookup_sort_table_attribute_heading(
		char *lookup_sort_input_attribute_name )
{
	static char heading[ 128 ];

	if ( !lookup_sort_input_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "lookup_sort_input_attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	string_initial_capital(
		heading,
		lookup_sort_input_attribute_name );

	return heading;
}

char *form_lookup_sort_table_onclick( LIST *folder_attribute_primary_key_list )
{
	char *primary_key_list_string;
	static char onclick[ 256 ];

	if ( !list_length( folder_attribute_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"folder_attribute_primary_key_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	primary_key_list_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_delimited(
			folder_attribute_primary_key_list,
			',' );

	snprintf(
		onclick,
		sizeof ( onclick ),
		"sort_order_move( '%s' )",
		primary_key_list_string );

	return onclick;
}

FORM_LOOKUP_SORT *form_lookup_sort_new(
		LIST *folder_attribute_primary_key_list,
		char *lookup_sort_input_attribute_name,
		LIST *query_fetch_row_list,
		char *lookup_sort_form_action_string )
{
	FORM_LOOKUP_SORT *form_lookup_sort;

	if ( !list_length( folder_attribute_primary_key_list )
	||   !lookup_sort_input_attribute_name
	||   !list_length( query_fetch_row_list )
	||   !lookup_sort_form_action_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_lookup_sort = form_lookup_sort_calloc();

	form_lookup_sort->widget_container_list = list_new();

	form_lookup_sort->renumber_onclick =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		form_lookup_sort_renumber_onclick(
			lookup_sort_input_attribute_name );

	form_lookup_sort->button_container_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_lookup_sort_button_container_list(
			form_lookup_sort->renumber_onclick );

	list_set_list(
		form_lookup_sort->widget_container_list,
		form_lookup_sort->button_container_list );

	form_lookup_sort->form_lookup_sort_table =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_lookup_sort_table_new(
			folder_attribute_primary_key_list,
			lookup_sort_input_attribute_name,
			query_fetch_row_list );

	list_set_list(
		form_lookup_sort->widget_container_list,
		form_lookup_sort->
			form_lookup_sort_table->
			widget_container_list );

	form_lookup_sort->widget_container_list_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		widget_container_list_html(
			(char *)0 /* state */,
			-1 /* row_number */,
			(char *)0 /* background_color */,
			form_lookup_sort->widget_container_list );

	form_lookup_sort->form_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_tag(
			FORM_LOOKUP_SORT_NAME,
			lookup_sort_form_action_string,
			FRAMESET_TABLE_FRAME /* target_frame */ );

	form_lookup_sort->form_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_html(
			form_lookup_sort->form_tag,
			form_lookup_sort->widget_container_list_html,
			(char *)0 /* hidden_html */,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_tag() );

	return form_lookup_sort;
}

FORM_LOOKUP_SORT *form_lookup_sort_calloc( void )
{
	FORM_LOOKUP_SORT *form_lookup_sort;

	if ( ! ( form_lookup_sort =
			calloc( 1,
				sizeof ( FORM_LOOKUP_SORT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return form_lookup_sort;
}

LIST *form_lookup_sort_button_container_list(
		char *form_lookup_sort_renumber_onclick )
{
	LIST *widget_container_list;
	WIDGET_CONTAINER *widget_container;

	if ( !form_lookup_sort_renumber_onclick )
	{
		char message[ 128 ];

		sprintf(message,
			"form_lookup_sort_renumber_onclick is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_container_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		widget_button_submit_reset_container_list();

	list_set(
		widget_container_list,
		( widget_container =
			widget_container_new(
				button, "Renumber" ) ) );

	widget_container->widget_button->button->action_string =
		form_lookup_sort_renumber_onclick;

	return widget_container_list;
}

char *form_lookup_sort_renumber_onclick(
		char *lookup_sort_input_attribute_name )
{
	static char onclick[ 128 ];

	if ( !lookup_sort_input_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "lookup_sort_input_attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		onclick,
		sizeof ( onclick ),
		"sort_order_renumber( '%s' )",
		lookup_sort_input_attribute_name );

	return onclick;
}

