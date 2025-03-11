/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/widget.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "String.h"
#include "column.h"
#include "session.h"
#include "attribute.h"
#include "file.h"
#include "form.h"
#include "relation.h"
#include "folder.h"
#include "date.h"
#include "list.h"
#include "sql.h"
#include "appaserver.h"
#include "appaserver_user.h"
#include "application.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "upload_source.h"
#include "piece.h"
#include "basename.h"
#include "environ.h"
#include "javascript.h"
#include "appaserver.h"
#include "query.h"
#include "attribute.h"
#include "row_security.h"
#include "date_convert.h"
#include "widget.h"

WIDGET_CONTAINER *widget_container_calloc( void )
{
	WIDGET_CONTAINER *widget_container;

	if ( ! ( widget_container = calloc( 1, sizeof ( WIDGET_CONTAINER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

	}
	return widget_container;
}

WIDGET_CONTAINER *widget_container_new(
		enum widget_type widget_type,
		char *widget_name )
{
	WIDGET_CONTAINER *widget_container =
		widget_container_calloc();

	widget_container->widget_type = widget_type;
	widget_container->widget_name = widget_name;

	if ( widget_type == table_open )
		widget_container->table_open =
			widget_table_open_calloc();
	else
	if ( widget_type == table_heading )
		widget_container->table_heading =
			widget_table_heading_new(
				widget_name /* heading_string */ );
	else
	if ( widget_type == table_row )
		widget_container->table_row =
			widget_table_row_calloc();
	else
	if ( widget_type == table_close )
		widget_container->table_close =
			widget_table_close_calloc();
	else
	if ( widget_type == checkbox )
	{
		widget_container->checkbox =
			widget_checkbox_new(
				widget_name );
	}
	else
	if ( widget_type == drop_down )
	{
		widget_container->drop_down =
			widget_drop_down_new(
				widget_name );
	}
	else
	if ( widget_type == multi_drop_down )
	{
		widget_container->multi_drop_down =
			widget_multi_drop_down_new(
				widget_name );
	}
	else
	if ( widget_type == button )
		widget_container->widget_button =
			widget_button_new(
				widget_name /* label */ );
	else
	if ( widget_type == character )
	{
		widget_container->text =
			widget_text_new(
				widget_name );

		widget_container->text->attribute_width_max_length =
			WIDGET_TEXT_DEFAULT_MAX_LENGTH;

		widget_container->text->widget_text_display_size =
			WIDGET_TEXT_DEFAULT_DISPLAY_SIZE;
	}
	else
	if ( widget_type == widget_date )
	{
		widget_container->date =
			widget_date_new(
				widget_name );
	}
	else
	if ( widget_type == widget_time )
	{
		widget_container->time =
			widget_time_new(
				widget_name );
	}
	else
	if ( widget_type == non_edit_text )
	{
		widget_container->non_edit_text =
			widget_non_edit_text_new(
				widget_name );
	}
	else
	if ( widget_type == yes_no )
	{
		widget_container->yes_no =
			widget_yes_no_new(
				widget_name );
	}
	else
	if ( widget_type == hidden )
		widget_container->hidden =
			widget_hidden_new(
				widget_name );
	else
	if ( widget_type == line_break )
		widget_container->line_break =
			widget_line_break_new();
	else
	if ( widget_type == table_data )
		widget_container->table_data =
			widget_table_data_calloc();
	else
	if ( widget_type == notepad )
	{
		widget_container->notepad = widget_notepad_new(
			widget_name );
	}
	else
	if ( widget_type == encrypt )
	{
		widget_container->encrypt =
			widget_encrypt_new(
				widget_name );
	}
	else
	if ( widget_type == upload )
	{
		widget_container->upload =
			widget_upload_new(
				widget_name );
	}
	else
	if ( widget_type == blank )
	{
		/* do nothing */
	}
	else
	if ( widget_type == anchor )
	{
		widget_container->anchor = widget_anchor_calloc();
	}
	else
	if ( widget_type == unordered_list_open )
	{
		/* do nothing */
	}
	else
	if ( widget_type == unordered_list_close )
	{
		/* do nothing */
	}
	else
	if ( widget_type == list_item )
	{
		/* do nothing */
	}
	else
	if ( widget_type == widget_radio )
	{
		widget_container->radio =
			widget_radio_new(
				widget_name );
	}
	else
	if ( widget_type == widget_password )
	{
		widget_container->password =
			widget_password_new(
				widget_name
				/* widget_password_display_name */ );
	}
	else
	{
		char message[ 128 ];

		sprintf(message,
			"invalid widget type = %d.",
			widget_type );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return widget_container;
}

char *widget_drop_down_select_tag(
		int display_size,
		int tab_order,
		boolean multi_select_boolean,
		char *widget_container_key,
		char *widget_drop_down_row_number_id,
		char *javascript_replace,
		char *background_color,
		boolean top_select_boolean )
{
	char tag[ 1024 ];
	char *ptr = tag;

	if ( !widget_container_key )
	{
		char message[ 128 ];

		sprintf(message, "widget_container_key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		"<select name=\"%s\"",
		widget_container_key );

	if ( widget_drop_down_row_number_id )
	{
		ptr += sprintf(
			ptr,
			" id=\"%s\"",
			widget_drop_down_row_number_id );
	}

	ptr += sprintf( ptr, " size=%d", display_size );

	if ( multi_select_boolean ) ptr += sprintf( ptr, " multiple" );

	if ( tab_order > 0 )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_tab_order_html(
				tab_order ) );
	}

	if ( background_color )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_background_color_html(
				background_color ) );
	}

	if ( javascript_replace )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_javascript_html(
				javascript_replace ) );
	}

	ptr += sprintf( ptr, ">" );

	if ( top_select_boolean )
	{
		sprintf(ptr,
			"\n%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_drop_down_option_static_tag(
				WIDGET_SELECT_OPERATOR ) );
	}

	return strdup( tag );
}

WIDGET_TABLE_OPEN *widget_table_open_new(
		boolean border_boolean )
{
	WIDGET_TABLE_OPEN *table_open =
		widget_table_open_calloc();

	table_open->border_boolean = border_boolean;

	return table_open;
}

WIDGET_TABLE_OPEN *widget_table_open_calloc( void )
{
	WIDGET_TABLE_OPEN *table_open;

	if ( ! ( table_open = calloc( 1, sizeof ( WIDGET_TABLE_OPEN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return table_open;
}

char *widget_table_open_tag(
		boolean border_boolean,
		int cell_spacing,
		int cell_padding )
{
	char tag[ 128 ];
	char *ptr = tag;

	ptr += sprintf(
		ptr,
		"<table cellspacing=%d cellpadding=%d",
		cell_spacing,
		cell_padding );

	if ( border_boolean ) ptr += sprintf( ptr, " border" );

	sprintf( ptr, ">" );

	return strdup( tag );
}

WIDGET_TABLE_HEADING *widget_table_heading_calloc( void )
{
	WIDGET_TABLE_HEADING *table_heading;

	if ( ! ( table_heading =
			calloc( 1, sizeof ( WIDGET_TABLE_HEADING ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return table_heading;
}

WIDGET_TABLE_HEADING *widget_table_heading_new( char *heading_string )
{
	WIDGET_TABLE_HEADING *table_heading;

	table_heading = widget_table_heading_calloc();
	table_heading->heading_string = heading_string;

	return table_heading;
}

char *widget_table_heading_tag( char *heading_string )
{
	char heading_html[ 128 ];

	if ( heading_string )
	{
		sprintf( heading_html, "<th>%s", heading_string );
	}
	else
	{
		strcpy( heading_html, "<th>" );
	}

	return strdup( heading_html );
}

WIDGET_TABLE_ROW *widget_table_row_calloc( void )
{
	WIDGET_TABLE_ROW *table_row;

	if ( ! ( table_row = calloc( 1, sizeof ( WIDGET_TABLE_ROW ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return table_row;
}

char *widget_table_row_tag( char *background_color )
{
	static char html[ 128 ];

	if ( !background_color )
	{
		strcpy( html, "<tr>" );
	}
	else
	{
		sprintf(html,
			"<tr bgcolor=%s>",
			background_color );
	}

	return html;
}

WIDGET_TABLE_CLOSE *widget_table_close_calloc( void )
{
	WIDGET_TABLE_CLOSE *table_close;

	if ( ! ( table_close = calloc( 1, sizeof ( WIDGET_TABLE_CLOSE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return table_close;
}

char *widget_table_close_tag( void )
{
	return "</table>";
}

char *widget_container_javascript(
			char *javascript,
			char *state,
			int row_number )
{
	static char post_change_javascript[ 1024 ];
	char row_string[ 8 ];

	if ( !javascript || !*javascript ) return (char *)0;

	strcpy( post_change_javascript, javascript );

	sprintf( row_string, "%d", row_number );

	string_search_replace(
		post_change_javascript /* source_destination */,
		"$row",
		row_string );

	if ( state && *state )
	{
		string_search_replace(
			post_change_javascript,
			"$state",
			state );
	}

	return post_change_javascript;
}

boolean widget_exists_upload_filename( LIST *widget_list )
{
	WIDGET_CONTAINER *widget;

	if ( list_rewind( widget_list ) )
	do {
		widget = list_get( widget_list );

		if ( widget->widget_type == upload ) return 1;

	} while( list_next( widget_list ) );

	return 0;
}

char *widget_container_number_commas_string(
		char *widget_name,
		char *value )
{
	if ( string_exists_substring( widget_name, "year" ) )
	{
		return value;
	}
	else
	{
		return
		strdup(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			string_commas_number_string(
				value ) );
	}
}

WIDGET_NON_EDIT_TEXT *widget_non_edit_text_new( char *widget_name )
{
	WIDGET_NON_EDIT_TEXT *widget_non_edit_text =
		widget_non_edit_text_calloc();

	widget_non_edit_text->widget_name = widget_name;
	return widget_non_edit_text;
}

WIDGET_NON_EDIT_TEXT *widget_non_edit_text_calloc( void )
{
	WIDGET_NON_EDIT_TEXT *widget_non_edit_text;

	if ( ! ( widget_non_edit_text =
			calloc( 1, sizeof ( WIDGET_NON_EDIT_TEXT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return widget_non_edit_text;
}

char *widget_non_edit_text_html(
		LIST *query_row_cell_list,
		char *widget_name,
		boolean no_initial_capital_boolean )
{
	char *container_value = {0};
	char *html_string;

	if ( query_row_cell_list )
	{
		container_value =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			widget_container_value(
				ATTRIBUTE_MULTI_KEY_DELIMITER,
				query_row_cell_list,
				widget_name );

		if ( container_value
		&&   string_exists_character(
			widget_name,
			ATTRIBUTE_MULTI_KEY_DELIMITER ) )
		{
			char *tmp;

			tmp =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				widget_drop_down_option_display_string(
					WIDGET_DROP_DOWN_LABEL_DELIMITER,
					WIDGET_DROP_DOWN_DASH_DELIMITER,
					no_initial_capital_boolean,
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					sql_delimiter_string(
						ATTRIBUTE_MULTI_KEY_DELIMITER ),
					container_value );

			free( container_value );
			container_value = tmp;
		}
	}

	html_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_non_edit_text_html_string(
			widget_name,
			no_initial_capital_boolean,
			container_value );

	if ( container_value ) free( container_value );

	return html_string;
}

char *widget_non_edit_text_html_string(
		char *widget_name,
		boolean no_initial_capital_boolean,
		char *widget_container_value )
{
	char html_string[ STRING_64K ];

	if ( !widget_name
	&&   !widget_container_value )
	{
		char message[ 128 ];

		sprintf(message,
		"both widget_name and widget_container_value are empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( widget_container_value )
	{
		if ( strlen( widget_container_value ) >= STRING_64K )
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

		if ( no_initial_capital_boolean )
		{
			strcpy( html_string, widget_container_value );
		}
		else
		{
			string_initial_capital(
				html_string,
				widget_container_value );
		}
	}
	else
	if ( widget_name )
	{
		if ( no_initial_capital_boolean )
		{
			strcpy( html_string, widget_name );
		}
		else
		{
			string_initial_capital(
				html_string,
				widget_name );
		}
	}

	return strdup( html_string );
}

char *widget_drop_down_name(
		const char attribute_multi_key_delimiter,
		char *widget_name,
		LIST *attribute_name_list )
{
	static char drop_down_name[ 128 ];
	char *results;

	if ( ( !widget_name && !list_length( attribute_name_list ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( widget_name )
	{
		strcpy( drop_down_name, widget_name );
	}
	else
	{
		results =
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			list_display_delimited(
				attribute_name_list,
				(char)attribute_multi_key_delimiter );

		if ( !*results )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: list_display_delimited() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		strcpy(drop_down_name, results );
		free( results );
	}

	return drop_down_name;
}

int widget_container_tab_order( void )
{
	static int tab_order = 0;

	return ++tab_order;
}

char *widget_drop_down_option_tag(
		char *value_string,
		char *display_string )
{
	static char tag[ 256 ];

	if ( !value_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: value_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !display_string || !*display_string )
		display_string = value_string;

	sprintf(tag,
		"\t<option value=\"%s\">%s",
		value_string,
		display_string );

	return tag;
}

boolean widget_drop_down_null_boolean(
		boolean null_boolean,
		boolean top_select_boolean )
{
	if ( top_select_boolean )
		return 0;
	else
		return null_boolean;
}

char *widget_drop_down_select_close_tag(
		boolean null_boolean,
		boolean not_null_boolean,
		boolean bottom_select_boolean )
{
	static char tag[ 300 ];
	char *ptr = tag;

	if ( null_boolean )
	{
		ptr += sprintf( ptr,
			"%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_drop_down_option_static_tag(
				QUERY_IS_NULL ) );
	}

	if ( not_null_boolean )
	{
		ptr += sprintf( ptr,
			"%s\n",
			widget_drop_down_option_static_tag(
				QUERY_NOT_NULL ) );
	}

	if ( bottom_select_boolean )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			widget_drop_down_option_static_tag(
				WIDGET_SELECT_OPERATOR ) );
	}

	sprintf( ptr, "</select>" );
	return tag;
}

char *widget_checkbox_html(
		char *widget_name,
		char *prompt,
		char *onclick,
		int tab_order,
		char *image_source,
		char *state,
		int row_number,
		LIST *query_row_cell_list,
		char *background_color )
{
	char *container_key = {0};
	boolean checkbox_checked = 0;
	char *replace;

	if ( !widget_name
	&&   !prompt )
	{
		char message[ 128 ];

		sprintf(message, "both widget_name and prompt are empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( widget_name )
	{
		container_key =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_key(
				widget_name,
				row_number );
	}

	if ( widget_name && list_length( query_row_cell_list ) )
	{
		checkbox_checked =
			widget_checkbox_checked(
				query_row_cell_list,
				widget_name );
	}

	replace =
		/* ----------------------------- */
		/* Returns static memory or null */
		/* ----------------------------- */
		javascript_replace(
			onclick /* post_change_javascript */,
			state,
			row_number );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	widget_checkbox_html_string(
		prompt,
		tab_order,
		image_source,
		background_color,
		container_key,
		checkbox_checked,
		replace );
}

char *widget_checkbox_html_string(
		char *prompt,
		int tab_order,
		char *image_source,
		char *background_color,
		char *widget_container_key,
		boolean widget_checkbox_checked,
		char *javascript_replace )
{
	char html[ STRING_1K ];
	char *ptr = html;
	char display[ 128 ];

	if ( !prompt
	&&   !widget_container_key )
	{
		char message[ 128 ];

		sprintf(message,
			"both prompt and widget_container_key are empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( image_source && *image_source )
	{
		ptr += sprintf(
			ptr,
			"<img src=\"%s\">",
			image_source );
	}

	if ( prompt )
	{
		string_initial_capital( display, prompt );
	}
	else
	{
		string_initial_capital( display, widget_container_key );
	}

	ptr += sprintf( ptr, "%s", display ); 

	ptr += sprintf( ptr,
		"<input type=checkbox value=\"y\"" );

	if ( widget_container_key )
	{
		ptr += sprintf( ptr,
			" name=\"%s\" id=\"%s\"",
			widget_container_key,
			widget_container_key );
	}

	if ( tab_order > 0 )
	{
		ptr += sprintf( ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_tab_order_html(
				tab_order ) );
	}

	if ( widget_checkbox_checked )
	{
		ptr += sprintf( ptr, " checked" );
	}

	if ( background_color && *background_color )
	{
		ptr += sprintf( ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_background_color_html(
				background_color ) );
	}

	if ( javascript_replace && *javascript_replace )
	{
		if (	strlen( html ) +
			strlen( javascript_replace ) +
			11 >= STRING_1K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_1K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf( ptr,
			" onClick=\"%s\"",
			javascript_replace );
	}

	sprintf( ptr, ">" );

	return strdup( html );
}

WIDGET_MULTI_DROP_DOWN *widget_multi_drop_down_calloc( void )
{
	WIDGET_MULTI_DROP_DOWN *widget_multi_drop_down;

	if ( ! ( widget_multi_drop_down =
			calloc( 1, sizeof ( WIDGET_MULTI_DROP_DOWN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return widget_multi_drop_down;
}

char *widget_empty_prompt_drop_down_html(
		char *widget_name,
		int display_size,
		boolean multi_select )
{
	char html[ 1024 ];
	char *ptr = html;

	if ( !widget_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: widget_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"<td><select name=\"%s\" size=%d",
		widget_name,
		(display_size) ? display_size : 1 );

	if ( multi_select ) ptr += sprintf( ptr, " multiple" );

	ptr += sprintf( ptr, ">\n" );

	return strdup( html );
}

char *widget_multi_drop_down_original_name(
		char *widget_multi_original_prefix,
		char *widget_name )
{
	char drop_down_name[ 256 ];

	if ( !widget_multi_original_prefix
	||   !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(drop_down_name,
		"%s%s",
		widget_multi_original_prefix,	
		widget_name );

	return strdup( drop_down_name );
}

WIDGET_MULTI_DROP_DOWN *widget_multi_drop_down_new(
		char *widget_name )
{
	WIDGET_MULTI_DROP_DOWN *widget_multi_drop_down;

	if ( !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "widget_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_multi_drop_down = widget_multi_drop_down_calloc();

	widget_multi_drop_down->original_name =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_multi_drop_down_original_name(
			WIDGET_MULTI_ORIGINAL_PREFIX,
			widget_name );

	widget_multi_drop_down->original_drop_down =
		widget_drop_down_calloc();

	widget_multi_drop_down->
		original_drop_down->
		widget_name =
			widget_multi_drop_down->original_name;

	widget_multi_drop_down->
		original_drop_down->
		display_size = WIDGET_MULTI_DISPLAY_SIZE;

	widget_multi_drop_down->table_data =
		widget_table_data_calloc();

	widget_multi_drop_down->move_right_button =
		widget_button_new(
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			widget_multi_drop_down_right_label() );

	widget_multi_drop_down->move_right_button->button->action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_multi_drop_down_right_action_string(
			widget_multi_drop_down->
				original_drop_down->
				widget_name
				/* original_name */,
			widget_name,
			WIDGET_MULTI_LEFT_RIGHT_DELIMITER );

	widget_multi_drop_down->move_right_button->button->hover_message =
		WIDGET_MULTI_DROP_DOWN_RIGHT_MESSAGE;

	widget_multi_drop_down->move_right_button->button->html =
		widget_button_html(
			0 /* row_number */,
			widget_multi_drop_down->
				move_right_button );

	widget_multi_drop_down->widget_line_break =
		widget_line_break_new();

	widget_multi_drop_down->move_left_button =
		widget_button_new(
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			widget_multi_drop_down_left_label() );

	widget_multi_drop_down->move_left_button->button->action_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			widget_multi_drop_down_left_action_string(
				widget_multi_drop_down->
					original_drop_down->
					widget_name
					/* original_name */,
				widget_name,
				WIDGET_MULTI_LEFT_RIGHT_DELIMITER );

	widget_multi_drop_down->move_left_button->button->hover_message =
			WIDGET_MULTI_DROP_DOWN_LEFT_MESSAGE;

	widget_multi_drop_down->move_left_button->button->html =
		widget_button_html(
			0 /* row_number */,
			widget_multi_drop_down->
				move_left_button );

	widget_multi_drop_down->multi_empty_drop_down =
		widget_multi_empty_drop_down_new(
			widget_name );

	return widget_multi_drop_down;
}

WIDGET_BUTTON *widget_button_calloc( void )
{
	WIDGET_BUTTON *widget_button;

	if ( ! ( widget_button = calloc( 1, sizeof ( WIDGET_BUTTON ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return widget_button;
}

WIDGET_BUTTON *widget_button_new( char *label )
{
	WIDGET_BUTTON *widget_button;

	widget_button = widget_button_calloc();

	widget_button->button =
		button_new(
			label );

	return widget_button;
}

char *widget_button_html(
		int row_number,
		WIDGET_BUTTON *widget_button )
{
	if ( !widget_button
	||   !widget_button->button )
	{
		char message[ 128 ];

		sprintf(message, "widget_button is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( widget_button->button->html )
	{
		return widget_button->button->html;
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	button_html(
		widget_button->button->label,
		widget_button->button->action_string,
		widget_button->button->hover_message,
		row_number );
}

char *widget_multi_drop_down_right_label( void )
{
	return "Add ->";
}

char *widget_multi_drop_down_left_label( void )
{
	return "<- Remove";
}

char *widget_multi_drop_down_right_action_string(
			char *original_name,
			char *widget_name,
			char widget_multi_move_left_right_delimiter )
{
	char action_string[ 1024 ];

	if ( !original_name
	||   !*original_name
	||   !widget_name
	||   !*widget_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(action_string,
		"post_change_multi_select_move_right('%s','%s','%c');",
		original_name,
		widget_name,
		widget_multi_move_left_right_delimiter );

	return strdup( action_string );
}

char *widget_multi_drop_down_left_action_string(
			char *original_name,
			char *widget_name,
			char widget_multi_move_left_right_delimiter )
{
	char action_string[ 1024 ];

	if ( !original_name
	||   !*original_name
	||   !widget_name
	||   !*widget_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(action_string,
		"post_change_multi_select_move_left('%s','%s','%c');",
		original_name,
		widget_name,
		widget_multi_move_left_right_delimiter );

	return strdup( action_string );
}

WIDGET_LINE_BREAK *widget_line_break_new( void )
{
	WIDGET_LINE_BREAK *widget_line_break =
		widget_line_break_calloc();

	widget_line_break->tag = "<br>";
	return widget_line_break;
}

WIDGET_LINE_BREAK *widget_line_break_calloc( void )
{
	WIDGET_LINE_BREAK *widget_line_break;

	if ( ! ( widget_line_break =
			calloc( 1, sizeof ( WIDGET_LINE_BREAK ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return widget_line_break;
}

char *widget_table_data_tag(
			boolean align_right,
			int column_span,
			int width_em,
			char *style )
{
	static char tag[ 128 ];
	char *ptr = tag;

	ptr += sprintf( ptr, "<td" );

	if ( align_right )
	{
		ptr += sprintf( ptr, " align=right" );
	}

	if ( column_span > 1 )
	{
		ptr += sprintf( ptr, " colspan=%d", column_span );
	}

	if ( width_em || style )
	{
		ptr += sprintf( ptr,
			" style=\"" );

		if ( width_em && style ) ptr += sprintf( ptr, "{" );

		if ( width_em )
		{
			ptr += sprintf( ptr,
				"width: %dem;",
				width_em );
		}

		if ( style ) ptr += sprintf( ptr, "%s", style );

		if ( width_em && style ) ptr += sprintf( ptr, "}" );

		ptr += sprintf( ptr, "\"" );
	}

	ptr += sprintf( ptr, ">" );

	return tag;
}

LIST *widget_container_heading_name_list(
		LIST *widget_container_list )
{
	LIST *heading_name_list;
	WIDGET_CONTAINER *widget_container;

	if ( !list_rewind( widget_container_list ) ) return (LIST *)0;

	heading_name_list = list_new();

	do {
		widget_container =
			list_get(
				widget_container_list );

		if ( widget_container->heading_string )
		{
			list_set(
				heading_name_list,
				widget_container->widget_name );
		}

	} while ( list_next( widget_container_list ) );

	return heading_name_list;
}

LIST *widget_container_heading_label_list( LIST *widget_container_list )
{
	LIST *heading_label_list;
	WIDGET_CONTAINER *widget_container;

	if ( !list_rewind( widget_container_list ) ) return (LIST *)0;

	heading_label_list = list_new();

	do {
		widget_container =
			list_get(
				widget_container_list );

		if ( widget_container->heading_string )
		{
			list_set(
				heading_label_list,
				widget_container->heading_string );
		}

	} while ( list_next( widget_container_list ) );

	return heading_label_list;
}

char *widget_container_html(
		char *state,
		int row_number,
		LIST *query_row_cell_list,
		RELATION_JOIN_ROW *relation_join_row,
		char *background_color,
		WIDGET_CONTAINER *widget_container )
{
	if ( !widget_container )
	{
		char message[ 128 ];

		sprintf(message, "widget_container is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( widget_container->widget_type == table_open )
	{
		return
		strdup(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			widget_table_open_tag(
				widget_container->
					table_open->
					border_boolean,
				widget_container->
					table_open->
					cell_spacing,
				widget_container->
					table_open->
					cell_padding ) );
	}
	else
	if ( widget_container->widget_type == table_heading )
	{
		if ( !widget_container->table_heading )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: table_heading is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_table_heading_tag(
			widget_container->
				table_heading->
				heading_string );
	}
	else
	if ( widget_container->widget_type == table_row )
	{
		return
		strdup(
			/* Returns static memory */
			/* --------------------- */
			widget_table_row_tag(
				background_color ) );
	}
	else
	if ( widget_container->widget_type == table_close )
	{
		return
		strdup(
			/* Returns program memory */
			/* ---------------------- */
			widget_table_close_tag() );
	}
	else
	if ( widget_container->widget_type == checkbox )
	{
		if ( !widget_container->checkbox )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: checkbox is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_container_checkbox_html(
			state,
			row_number,
			query_row_cell_list,
			background_color,
			widget_container->checkbox );
	}
	else
	if ( widget_container->widget_type == drop_down )
	{
		char *html;

		if ( !widget_container->drop_down )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: drop_down is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		html =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			widget_container_drop_down_html(
				state,
				row_number,
				query_row_cell_list,
				relation_join_row,
				background_color,
				widget_container->drop_down );

		return html;
	}
	else
	if ( widget_container->widget_type == multi_drop_down )
	{
		if ( !widget_container->multi_drop_down )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: multi_drop_down is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_container_multi_drop_down_html(
			state,
			widget_container->multi_drop_down );
	}
	else
	if ( widget_container->widget_type == character )
	{
		if ( !widget_container->text )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: text is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_container_text_html(
			state,
			row_number,
			query_row_cell_list,
			background_color,
			widget_container->text );
	}
	else
	if ( widget_container->widget_type == hidden )
	{
		if ( !widget_container->hidden
		||   !widget_container->hidden->widget_name )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: hidden is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		widget_hidden_html(
			row_number,
			query_row_cell_list,
			widget_container->hidden );
	}
	else
	if ( widget_container->widget_type == notepad )
	{
		if ( !widget_container->notepad )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: notepad is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_container_notepad_html(
			state,
			row_number,
			query_row_cell_list,
			background_color,
			widget_container->notepad );
	}
	else
	if ( widget_container->widget_type == upload )
	{
		char *html;

		if ( !widget_container->upload )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: upload is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		html =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			widget_container_upload_html(
				query_row_cell_list,
				background_color,
				widget_container->upload );

		return html;
	}
	else
	if ( widget_container->widget_type == yes_no )
	{
		if ( !widget_container->yes_no )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: yes_no is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_container_yes_no_html(
			state,
			row_number,
			query_row_cell_list,
			background_color,
			widget_container->yes_no );
	}
	else
	if ( widget_container->widget_type == widget_date )
	{
		if ( !widget_container->date )
		{
			char message[ 128 ];

			sprintf(message,
				"widget_name=[%s], date is empty.",
				widget_container->widget_name );

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
		widget_container_date_html(
			state,
			row_number,
			query_row_cell_list,
			background_color,
			widget_container->date );
	}
	else
	if ( widget_container->widget_type == widget_time )
	{
		if ( !widget_container->time )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: time is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_container_time_html(
			state,
			row_number,
			query_row_cell_list,
			background_color,
			widget_container->time );
	}
	else
	if ( widget_container->widget_type == button )
	{
		if ( !widget_container->widget_button )
		{
			char message[ 128 ];

			sprintf(message,
				"widget_container->widget_button is empty." );

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
		widget_button_html(
			row_number,
			widget_container->widget_button );
	}
	else
	if ( widget_container->widget_type == non_edit_text )
	{
		if ( !widget_container->non_edit_text )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: non_edit_text is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_container_non_edit_text_html(
			query_row_cell_list,
			widget_container->non_edit_text );
	}
	else
	if ( widget_container->widget_type == line_break )
	{
		if ( !widget_container->line_break )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: table_data is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return
		strdup( widget_container->line_break->tag );
	}
	else
	if ( widget_container->widget_type == table_data )
	{
		if ( !widget_container->table_data )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: table_data is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return
		strdup(
			/* Returns static memory */
			/* --------------------- */
			widget_table_data_tag(
				widget_container->
					table_data->
					align_right,
				widget_container->
					table_data->
					column_span,
				widget_container->
					table_data->
					width_em,
				widget_container->
					table_data->
					style ) );
	}
	else
	if ( widget_container->widget_type == encrypt )
	{
		if ( !widget_container->encrypt )
		{
			char message[ 128 ];

			sprintf(message,
				"widget_container->encrypt is empty." );

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
		widget_container_encrypt_html(
			row_number,
			query_row_cell_list,
			widget_container->encrypt );
	}
	else
	if ( widget_container->widget_type == blank )
	{
		return strdup( "" );
	}
	else
	if ( widget_container->widget_type == anchor )
	{
		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_container_anchor_html(
			widget_container->anchor );
	}
	else
	if ( widget_container->widget_type == unordered_list_open )
	{
		return strdup( "<ul>" );
	}
	else
	if ( widget_container->widget_type == unordered_list_close )
	{
		return strdup( "</ul>" );
	}
	else
	if ( widget_container->widget_type == list_item )
	{
		return strdup( "<li>" );
	}
	else
	if ( widget_container->widget_type == widget_radio )
	{
		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_container_radio_html(
			widget_container->radio );
	}
	else
	if ( widget_container->widget_type == widget_ajax )
	{
		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_container_ajax_html(
			widget_container->
				ajax->
				ajax_client_widget_container_list,
			query_row_cell_list,
			row_number,
			background_color );
	}
	else
	if ( widget_container->widget_type == widget_password )
	{
		return
		widget_password_html(
			widget_container->
				password->
				widget_password_display_name,
			widget_container->
				password->
				attribute_name,
			widget_container->
				password->
				text
				/* widget_text */,
			state,
			row_number,
			query_row_cell_list,
			background_color );
	}
	else
	{
		char message[ 128 ];

		sprintf(message,
			"invalid widget_type = %d.",
			widget_container->widget_type );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* Stub */
	/* ---- */
	return (char *)0;
}

char *widget_container_name(
		char *attribute_name,
		int row_number )
{
	if ( !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* --------------------- */
	/* Returns static memory */
	/* --------------------- */
	widget_container_key(
		attribute_name /* widget_name */,
		row_number );
}

char *widget_multi_drop_down_html(
		char *state,
		WIDGET_DROP_DOWN *original_drop_down,
		WIDGET_TABLE_DATA *table_data,
		WIDGET_BUTTON *move_right_button,
		WIDGET_LINE_BREAK *widget_line_break,
		WIDGET_BUTTON *move_left_button,
		WIDGET_MULTI_EMPTY_DROP_DOWN *multi_empty_drop_down,
		boolean null_boolean,
		boolean not_null_boolean )
{
	char *drop_down_html;
	char *table_data_tag;
	char *html_string;

	if ( !original_drop_down
	||   !original_drop_down->widget_name
	||   !table_data
	||   !move_right_button
	||   !move_right_button->button
	||   !move_right_button->button->html
	||   !widget_line_break
	||   !move_left_button
	||   !move_left_button->button
	||   !move_left_button->button->html
	||   !multi_empty_drop_down
	||   !multi_empty_drop_down->html )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	drop_down_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_drop_down_html(
			original_drop_down->widget_name,
			(LIST *)0 /* attribute_name_list */,
			0 /* not top_select_boolean */,
			null_boolean,
			not_null_boolean,
			0 /* not bottom_select_boolean */,
			WIDGET_MULTI_DISPLAY_SIZE,
			0 /* tab_order */,	
			1 /* multi_select_boolean */,
			original_drop_down->post_change_javascript,
			state,
			-1 /* row_number to omit index suffix */,
			(LIST *)0 /* query_row_cell_list */,
			(RELATION_JOIN_ROW *)0,
			original_drop_down->widget_drop_down_option_list,
			(LIST *)0 /* foreign_key_list */,
			(char *)0 /* background_color */,
			(char *)0 /* widget_drop_down_id */ );

	table_data_tag =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		widget_table_data_tag(
			table_data->align_right,
			table_data->column_span,
			table_data->width_em,
			table_data->style );

	html_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_multi_drop_down_html_string(
			drop_down_html,
			table_data_tag,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			widget_table_open_tag(
				0 /* not border_boolean */,
				0 /* cell_spacing */,
				0 /* cell_padding */ ),
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_table_row_tag(
				(char *)0 /* background_color */ ),
			move_right_button->button->html,
			widget_line_break->tag,
			move_left_button->button->html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			widget_table_close_tag(),
			multi_empty_drop_down->html );

	free( drop_down_html );

	return html_string;
}

char *widget_multi_drop_down_html_string(
		char *widget_drop_down_html,
		char *widget_table_data_tag,
		char *widget_table_open_tag,
		char *widget_table_row_tag,
		char *move_right_html,
		char *widget_line_break_tag,
		char *move_left_html,
		char *widget_table_close_tag,
		char *widget_multi_empty_drop_down_html )
{
	char html_string[ STRING_128K ];

	if ( !widget_drop_down_html
	||   !widget_table_data_tag
	||   !widget_table_open_tag
	||   !widget_table_row_tag
	||   !move_right_html
	||   !widget_line_break_tag
	||   !move_left_html
	||   !widget_table_close_tag
	||   !widget_multi_empty_drop_down_html )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if (	strlen( widget_drop_down_html ) +
		strlen( widget_table_data_tag ) +
		strlen( widget_table_open_tag ) +
		strlen( widget_table_row_tag ) +
		strlen( widget_table_data_tag ) +
		strlen( move_right_html ) +
		strlen( widget_line_break_tag ) +
		strlen( move_left_html ) +
		strlen( widget_table_close_tag ) +
		strlen( widget_table_data_tag ) +
		strlen( widget_multi_empty_drop_down_html ) +
		11 >= STRING_128K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_128K );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(html_string,
		"%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s",
		widget_table_open_tag,
		widget_table_row_tag,
		widget_table_data_tag,
		widget_drop_down_html,
		widget_table_data_tag,
		move_right_html,
		widget_line_break_tag,
		move_left_html,
		widget_table_data_tag,
		widget_multi_empty_drop_down_html,
		widget_table_close_tag );

	return strdup( html_string );
}

char *widget_container_javascript_html(
		char *javascript_replace )
{
	static char html[ 2048 ];

	*html = '\0';

	if ( !javascript_replace )
	{
		char message[ 128 ];

		sprintf(message, "javascript_replace is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if (	strlen( javascript_replace ) +
		13 >= 2048 )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			2048 );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( javascript_replace && *javascript_replace )
	{
		sprintf(html,
			" onChange=\"%s\"",
			javascript_replace );
	}

	return html;
}

char *widget_container_tab_order_html( int tab_order )
{
	static char html[ 32 ];

	*html = '\0';

	if ( tab_order >= 1 )
	{
		sprintf(html,
			" tabindex=%d",
			tab_order );
	}

	return html;
}

char *widget_container_background_color_html(
		char *background_color )
{
	static char html[ 128 ];

	*html = '\0';

	if ( background_color && *background_color )
	{
		sprintf(html,
			" style=\"{background:%s}\"",
			background_color );
	}

	return html;
}

WIDGET_CONTAINER *widget_container_name_seek(
			char *widget_name,
			LIST *widget_list )
{
	WIDGET_CONTAINER *widget;

	if ( !list_rewind( widget_list ) ) return (WIDGET_CONTAINER *)0;

	do {
		widget = list_get( widget_list );

		if ( widget->widget_name
		&&   string_strcmp(
			widget_name,
			widget->widget_name ) == 0 )
		{
			return widget;
		}

	} while ( list_next( widget_list ) );

	return (WIDGET_CONTAINER *)0;
}

boolean widget_checkbox_checked(
		LIST *query_row_cell_list,
		char *widget_name )
{
	QUERY_CELL *query_cell;

	if ( !widget_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: widget_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( query_cell =
			query_cell_seek(
				widget_name /* attribute_name */,
				query_row_cell_list ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"query_cell_seek(%s) returned empty.",
			widget_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
				__LINE__,
			message );
	}

	if ( query_cell->select_datum && *query_cell->select_datum == 'y' )
		return 1;
	else
		return 0;
}

char *widget_text_html_string(
		int attribute_width_max_length,
		int widget_text_display_size,
		int tab_order,
		boolean viewonly,
		char *background_color,
		char *widget_container_name,
		char *widget_text_value_string,
		char *replace_on_change_javascript,
		boolean autocomplete_off,
		/* --------------------- */
		/* Expect program memory */
		/* --------------------- */
		char *prevent_carrot_javascript )
{
	char html[ 1024 ];
	char *ptr = html;

	if ( !widget_container_name )
	{
		char message[ 128 ];

		sprintf(message, "widget_container_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		"<input name=\"%s\" id=\"%s\" type=text size=%d maxlength=%d",
		widget_container_name,
		widget_container_name,
		widget_text_display_size,
		attribute_width_max_length );

	if ( viewonly )
	{
		ptr += sprintf(
			ptr,
			" readonly" );
	}

	if ( widget_text_value_string )
	{
		ptr += sprintf(
			ptr,
			" value=\"%s\"",
			widget_text_value_string );
	}

	if ( autocomplete_off )
	{
		ptr += sprintf( ptr, " autocomplete=\"off\"" );
	}

	if (	string_strlen( widget_text_value_string ) >
		widget_text_display_size )
	{
		ptr += sprintf(
			ptr,
			" title=\"%s\"",
			widget_text_value_string );
	}

	if ( tab_order > 0 )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_tab_order_html(
				tab_order ) );
	}

	if ( replace_on_change_javascript )
	{
		ptr += sprintf(
			ptr,
			" onChange=\"%s\"",
			replace_on_change_javascript );
	}

	ptr += sprintf(
		ptr,
		"%s",
		prevent_carrot_javascript );

	if ( background_color )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_background_color_html(
				background_color ) );
	}

	ptr += sprintf( ptr, ">" );

	return strdup( html );
}

WIDGET_TEXT *widget_text_new( char *widget_name )
{
	WIDGET_TEXT *widget_text;

	if ( !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "widget_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_text = widget_text_calloc();

	widget_text->widget_name = widget_name;

	return widget_text;
}

WIDGET_TEXT *widget_text_calloc( void )
{
	WIDGET_TEXT *widget_text;

	if ( ! ( widget_text = calloc( 1, sizeof ( WIDGET_TEXT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

	}

	return widget_text;
}

char *widget_text_value_string(
		LIST *query_cell_list,
		char *value_string,
		char *widget_name )
{
	if ( !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "widget_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( value_string ) return value_string;

	if ( list_length( query_cell_list ) )
	{
		QUERY_CELL *query_cell;

		if ( ! ( query_cell =
				query_cell_seek(
					widget_name /* attribute_name */,
					query_cell_list ) ) )
		{
			char message[ 65536 ];

			sprintf(message,
				"query_cell_seek(%s) returned empty.\n"
				"query_cell_list=[%s]",
				widget_name,
				query_cell_list_display( query_cell_list ) );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		value_string = query_cell->display_datum;
	}

	return value_string;
}

char *widget_text_replace_javascript(
		char *post_change_javascript,
		int row_number,
		char *state,
		boolean null_to_slash )
{
	char replace_javascript[ 1024 ];
	char *ptr = replace_javascript;

	if ( ( !post_change_javascript
	||     !*post_change_javascript )
	&&     !null_to_slash )
	{
		return (char *)0;
	}

	if ( null_to_slash )
	{
		ptr += sprintf(
			ptr,
		 	"null2slash(this)" );
	}

	if ( post_change_javascript )
	{
		char *tmp;

		if ( ptr != replace_javascript )
		{
			ptr += sprintf( ptr, ";" );
		}

		if ( ( tmp =
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			javascript_replace(
				post_change_javascript,
				state,
				row_number ) ) )
		{
			ptr += sprintf( ptr, "%s", tmp );
		}
	}

	return strdup( replace_javascript );
}

boolean widget_text_autocomplete_off( char *widget_name )
{
	if ( string_instr( "password", widget_name, 1 ) != -1
	||   string_instr( "login_name", widget_name, 1 ) != -1 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

char *widget_container_value(
		char attribute_multi_key_delimiter,
		LIST *query_row_cell_list,
		char *widget_name )
{
	if ( !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "widget_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( query_row_cell_list ) ) return (char *)0;

	if ( attribute_multi_key_delimiter )
	{
		return
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		widget_container_multi_key_value(
			attribute_multi_key_delimiter,
			query_row_cell_list,
			widget_name );
	}
	else
	{
		QUERY_CELL *query_cell;

		if ( ! ( query_cell =
				query_cell_seek(
					widget_name /* attribute_name */,
					query_row_cell_list ) ) )
		{
			return NULL;
		}

		return query_cell->display_datum;
	}
}

char *widget_container_multi_key_value(
		char attribute_multi_key_delimiter,
		LIST *query_row_cell_list,
		char *widget_name )
{
	QUERY_CELL *query_cell;
	char container_value[ STRING_64K ];
	char *ptr = container_value;
	LIST *list;

	if ( !attribute_multi_key_delimiter
	||   !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( query_row_cell_list ) ) return NULL;

	list =
		list_string_to_list(
			widget_name,
			attribute_multi_key_delimiter );

	if ( list_rewind( list ) )
	do {
		widget_name = list_get( list );

		if ( ! ( query_cell =
				query_cell_seek(
					widget_name /* attribute_name */,
					query_row_cell_list ) ) )
		{
			return NULL;
		}

		if ( !query_cell->display_datum )
			query_cell->display_datum = "";

		if (	strlen( container_value ) +
			strlen( query_cell->display_datum ) + 1 >= STRING_64K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				65536 );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( ptr != container_value )
		{
			ptr += sprintf(
				ptr,
				"%c",
				attribute_multi_key_delimiter );
		}

		ptr += sprintf( ptr, "%s", query_cell->display_datum );

	} while ( list_next( list ) );

	list_free_string_list( list );

	return strdup( container_value );
}

char *widget_container_key(
		char *widget_name,
		int row_number )
{
	static char key[ 256 ];

	if ( !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "widget_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		/* Stub */
		/* ---- */
		exit( 1 );
	}

	if ( row_number > 0 )
	{
		sprintf(key,
			"%s_%d",
			widget_name,
			row_number );
	}
	else
	{
		strcpy( key, widget_name );
	}

	return key;
}

char *widget_container_multi_drop_down_html(
		char *state,
		WIDGET_MULTI_DROP_DOWN *multi_drop_down )
{
	char *html;

	if ( !multi_drop_down
	||   !multi_drop_down->original_drop_down
	||   !multi_drop_down->table_data
	||   !multi_drop_down->move_right_button
	||   !multi_drop_down->widget_line_break
	||   !multi_drop_down->move_left_button
	||   !multi_drop_down->multi_empty_drop_down )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: multi_drop_down is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_multi_drop_down_html(
			state,
			multi_drop_down->original_drop_down,
			multi_drop_down->table_data,
			multi_drop_down->move_right_button,
			multi_drop_down->widget_line_break,
			multi_drop_down->move_left_button,
			multi_drop_down->multi_empty_drop_down,
			multi_drop_down->null_boolean,
			multi_drop_down->not_null_boolean );

	return html;
}

char *widget_container_drop_down_html(
		char *state,
		int row_number,
		LIST *query_row_cell_list,
		RELATION_JOIN_ROW *relation_join_row,
		char *background_color,
		WIDGET_DROP_DOWN *drop_down )
{
	char *widget_id;

	if ( !drop_down )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: drop_down is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	widget_id =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		widget_drop_down_id(
			drop_down->many_folder_name,
			drop_down->widget_name );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	widget_drop_down_html(
		drop_down->widget_name /* mutually exclusive */,
		drop_down->attribute_name_list /* mutually exclusive */,
		drop_down->top_select_boolean,
		drop_down->null_boolean,
		drop_down->not_null_boolean,
		drop_down->bottom_select_boolean,
		drop_down->display_size,
		drop_down->tab_order,
		drop_down->multi_select_boolean,
		drop_down->post_change_javascript,
		state,
		row_number,
		query_row_cell_list,
		relation_join_row,
		drop_down->widget_drop_down_option_list,
		drop_down->foreign_key_list,
		background_color,
		widget_id /* widget_drop_down_id */ );
}

WIDGET_TABLE_DATA *widget_table_data_calloc( void )
{
	WIDGET_TABLE_DATA *widget_table_data;

	if ( ! ( widget_table_data =
			calloc( 1, sizeof ( WIDGET_TABLE_DATA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return widget_table_data;
}

char *widget_container_non_edit_text_html(
		LIST *query_row_cell_list,
		WIDGET_NON_EDIT_TEXT *non_edit_text )
{
	if ( !non_edit_text )
	{
		char message[ 128 ];

		sprintf(message, "non_edit_text is empty." );

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
	widget_non_edit_text_html(
		query_row_cell_list,
		non_edit_text->widget_name,
		non_edit_text->no_initial_capital_boolean );
}

char *widget_container_text_html(
		char *state,
		int row_number,
		LIST *query_cell_list,
		char *background_color,
		WIDGET_TEXT *widget_text )
{
	if ( !widget_text )
	{
		char message[ 128 ];

		sprintf(message, "widget_text is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !widget_text->widget_name )
	{
		char message[ 128 ];

		sprintf(message, "widget_text->widget_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !widget_text->attribute_width_max_length )
	{
		char message[ 128 ];

		sprintf(message, "attribute_width_max_length is empty." );

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
	widget_text_html(
		widget_text->widget_name,
		widget_text->attribute_width_max_length,
		widget_text->widget_text_display_size,
		widget_text->null_to_slash,
		widget_text->prevent_carrot_boolean,
		widget_text->post_change_javascript,
		widget_text->tab_order,
		widget_text->value_string,
		widget_text->viewonly,
		state,
		row_number,
		query_cell_list,
		background_color );
}

char *widget_text_html(
		char *widget_name,
		int attribute_width_max_length,
		int widget_text_display_size,
		boolean null_to_slash,
		boolean prevent_carrot_boolean,
		char *post_change_javascript,
		int tab_order,
		char *value_string,
		boolean viewonly,
		char *state,
		int row_number,
		LIST *query_cell_list,
		char *background_color )
{
	char *container_name;
	char *text_value_string;
	char *replace_post_change_javascript = {0};
	boolean autocomplete_off;
	char *html;

	container_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		widget_container_name(
			widget_name,
			row_number );

	text_value_string =
		/* ---------------------------- */
		/* Returns value_string,	*/
		/* component of query_cell_list,*/
		/* heap memory,			*/
		/* or null			*/
		/* ---------------------------- */
		widget_text_value_string(
			query_cell_list,
			value_string,
			widget_name );

	if ( post_change_javascript || null_to_slash )
	{
		replace_post_change_javascript =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			widget_text_replace_javascript(
				post_change_javascript,
				row_number,
				state,
				null_to_slash );
	}

	autocomplete_off =
		widget_text_autocomplete_off(
			widget_name );

	if ( !widget_text_display_size )
		widget_text_display_size =
			WIDGET_TEXT_DEFAULT_DISPLAY_SIZE;

	html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_text_html_string(
			attribute_width_max_length,
			widget_text_display_size,
			tab_order,
			viewonly,
			background_color,
			container_name,
			text_value_string,
			replace_post_change_javascript,
			autocomplete_off,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			widget_text_prevent_carrot_javascript(
				prevent_carrot_boolean ) );

	if ( replace_post_change_javascript )
		free( replace_post_change_javascript );

	return html;
}

char *widget_container_date_html(
		char *state,
		int row_number,
		LIST *query_cell_list,
		char *background_color,
		WIDGET_DATE *date )
{
	if ( !date
	||   !date->widget_name
	||   !date->datatype_name
	||   !date->application_name
	||   !date->login_name )
	{
		char message[ 128 ];

		sprintf(message, "date is empty or incomplete." );

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
	widget_date_html(
		date->widget_name,
		date->datatype_name,
		date->attribute_width_max_length,
		date->display_size,
		date->null_to_slash,
		date->post_change_javascript,
		date->tab_order,
		date->application_name,
		date->login_name,
		state,
		row_number,
		query_cell_list,
		background_color );
}

char *widget_container_time_html(
		char *state,
		int row_number,
		LIST *query_cell_list,
		char *background_color,
		WIDGET_TIME *time )
{
	if ( !time
	||   !time->widget_name )
	{
		char message[ 128 ];

		sprintf(message, "time is empty or incomplete." );

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
	widget_time_html(
		time->widget_name,
		time->attribute_width_max_length,
		time->widget_text_display_size,
		time->null_to_slash,
		time->post_change_javascript,
		time->tab_order,
		state,
		row_number,
		query_cell_list,
		background_color );
}

WIDGET_HIDDEN *widget_hidden_new( char *widget_name )
{
	WIDGET_HIDDEN *widget_hidden = widget_hidden_calloc();

	widget_hidden->widget_name = widget_name;
	return widget_hidden;
}

WIDGET_HIDDEN *widget_hidden_calloc( void )
{
	WIDGET_HIDDEN *widget_hidden;

	if ( ! ( widget_hidden = calloc( 1, sizeof ( WIDGET_HIDDEN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return widget_hidden;
}

char *widget_hidden_html_string(
		char *widget_hidden_container_key,
		char *widget_hidden_value )
{
	char html[ 1024 ];

	if ( !widget_hidden_container_key )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: widget_hidden_container_key is is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"<input name=%s type=hidden value=\"%s\">",
		widget_hidden_container_key,
		(widget_hidden_value) ? widget_hidden_value : "" );

	return strdup( html );
}

char *widget_hidden_html(
		int row_number,
		LIST *query_row_cell_list,
		WIDGET_HIDDEN *widget_hidden )
{
	char *key;
	char *value;

	if ( !widget_hidden
	||   !widget_hidden->widget_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: widget_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	key =
		/* Returns widget_name or static memory */
		/* ------------------------------------ */
		widget_hidden_container_key(
			widget_hidden->widget_name,
			row_number );

	value =
		   /* --------------------------------- */
		   /* Returns value_string,		*/
		   /* heap memory,			*/
		   /* or null				*/
		   /* --------------------------------- */
		   widget_hidden_value(
			query_row_cell_list,
			widget_hidden->widget_name,
			widget_hidden->value_string );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	widget_hidden_html_string(
		key /* widget_hidden_container_key */,
		value /* widget_hidden_value */ );
}

char *widget_notepad_html(
		char *widget_name,
		int attribute_size,
		boolean null_to_slash,
		char *post_change_javascript,
		int tab_order,
		char *state,
		int row_number,
		LIST *query_cell_list,
		char *background_color )
{
	char *container_key = {0};
	char *container_value = {0};
	char *text_replace_on_change = {0};

	if ( !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "widget_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	container_key =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		widget_container_key(
			widget_name,
			row_number );
 
	if ( list_length( query_cell_list ) )
	{
		container_value =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			widget_container_value(
				(char)0 /* ATTRIBUTE_MULTI_KEY_DELIMITER */,
				query_cell_list,
				widget_name );
	}

	if ( post_change_javascript || null_to_slash )
	{
		text_replace_on_change =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			widget_text_replace_javascript(
				post_change_javascript,
				row_number,
				state,
				null_to_slash );
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	widget_notepad_html_string(
		tab_order,
		background_color,
		container_key,
		container_value,
		attribute_size,
		WIDGET_NOTEPAD_COLUMNS,
		WIDGET_NOTEPAD_ROWS,
		text_replace_on_change );
}

char *widget_notepad_html_string(
		int tab_order,
		char *background_color,
		char *widget_container_key,
		char *widget_container_value,
		int attribute_size,
		int widget_notepad_columns,
		int widget_notepad_rows,
		char *widget_text_replace_javascript )
{
	char html[ STRING_65K ];
	char *ptr = html;

	if ( !widget_container_key )
	{
		char message[ 128 ];

		sprintf(message, "widget_container_key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		"<textarea name=\"%s\" cols=%d rows=%d",
		widget_container_key,
		widget_notepad_columns,
		widget_notepad_rows );

	if ( widget_text_replace_javascript )
	{
		ptr += sprintf(
			ptr,
			" onchange=\"%s\"",
			widget_text_replace_javascript );
	}

	ptr += sprintf(
		ptr,
		" onblur=\"timlib_check_notepad_size(this, %d);\"",
		attribute_size );

	if ( tab_order > 0 )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_tab_order_html(
				tab_order ) );
	}

	if ( background_color && *background_color )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_background_color_html(
				background_color ) );
	}

	ptr += sprintf(
		ptr,
		 " wrap=%s>",
		 WIDGET_TEXTAREA_WRAP );

	if ( widget_container_value )
	{
		ptr += sprintf(
			ptr,
		 	"%s",
		 	widget_container_value );
	}

	ptr += sprintf(
		ptr,
		 "</textarea>\n" );

	return strdup( html );
}

WIDGET_NOTEPAD *widget_notepad_new( char *widget_name )
{
	WIDGET_NOTEPAD *widget_notepad;

	if ( !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_notepad = widget_notepad_calloc();

	widget_notepad->widget_name = widget_name;

	return widget_notepad;
}

WIDGET_NOTEPAD *widget_notepad_calloc( void )
{
	WIDGET_NOTEPAD *widget_notepad;

	if ( ! ( widget_notepad = calloc( 1, sizeof ( WIDGET_NOTEPAD ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return widget_notepad;
}

char *widget_container_notepad_html(
		char *state,
		int row_number,
		LIST *query_cell_list,
		char *background_color,
		WIDGET_NOTEPAD *notepad )
{
	if ( !notepad
	||   !notepad->widget_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: notepad is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	widget_notepad_html(
		notepad->widget_name,
		notepad->attribute_size,
		notepad->null_to_slash,
		notepad->post_change_javascript,
		notepad->tab_order,
		state,
		row_number,
		query_cell_list,
		background_color );
}

WIDGET_ENCRYPT *widget_encrypt_new( char *widget_name )
{
	WIDGET_ENCRYPT *widget_encrypt;

	if ( !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "widget_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_encrypt = widget_encrypt_calloc();
	widget_encrypt->widget_name = widget_name;

	return widget_encrypt;
}

WIDGET_ENCRYPT *widget_encrypt_calloc( void )
{
	WIDGET_ENCRYPT *widget_encrypt;

	if ( ! ( widget_encrypt = calloc( 1, sizeof ( WIDGET_ENCRYPT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

	}
	return widget_encrypt;
}

char *widget_encrypt_html(
		int widget_text_display_size,
		int row_number,
		LIST *query_cell_list,
		char *widget_name,
		int attribute_width_max_length,
		int tab_order )
{
	char *compare_name;
	char *container_key;
	char *container_value = {0};

	if ( !widget_name
	||   !widget_text_display_size )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	compare_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		widget_encrypt_compare_name(
			WIDGET_ENCRYPT_COMPARE_LABEL,
			widget_name,
			row_number );

	container_key =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		widget_container_key(
			widget_name,
			row_number );

	if ( query_cell_list )
	{
		container_value =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			widget_container_value(
				(char)0 /* ATTRIBUTE_MULTI_KEY_DELIMITER */,
				query_cell_list,
				widget_name );
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	widget_encrypt_html_string(
		attribute_width_max_length,
		widget_text_display_size,
		tab_order,
		compare_name,
		container_key,
		container_value );
}

char *widget_encrypt_html_string(
		int attribute_width_max_length,
		int widget_text_display_size,
		int tab_order,
		char *compare_name,
		char *container_key,
		char *container_value )
{
	char html[ STRING_16K ];
	char *ptr = html;
	int size;

	if ( !compare_name
	||   !container_key )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( attribute_width_max_length > widget_text_display_size )
	{
		size = widget_text_display_size;
	}
	else
	{
		size = attribute_width_max_length;
	}

	/* Put the widget set in a table */
	/* ----------------------------- */
	ptr += sprintf(
		ptr,
		"%s\n",
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_table_open_tag(
			0 /* not border_boolean */,
			0 /* cell_spacing */,
			0 /* cell_padding */ ) );

	/* Output the password field */
	/* ------------------------- */
	ptr += sprintf( ptr, "<tr><td>Type\n" );

	ptr += sprintf(
		ptr,
		"<td><input name=\"%s\" type=password size=%d maxlength=%d",
		container_key,
		size,
		attribute_width_max_length );

	if ( container_value )
	{
		ptr += sprintf(
			ptr,
			" value=\"%s\"",
			container_value );
	}

	ptr += sprintf( ptr, " autocomplete=off" );

	ptr += sprintf( ptr, " onChange=\"null2slash(this)\"" );

	if ( tab_order > 0 )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_tab_order_html(
				tab_order ) );
	}

	ptr += sprintf( ptr, ">\n" );

	/* Output the password compare field */
	/* --------------------------------- */
	ptr += sprintf( ptr, "<tr><td>Retype\n" );

	ptr += sprintf(
		ptr,
"<td><input name=\"%s\" type=password size=%d maxlength=%d",
		compare_name,
		size,
		attribute_width_max_length );

	ptr += sprintf(
		ptr,
		" onChange=\"timlib_password_compare('%s','%s')\"",
		container_key,
		compare_name );

	ptr += sprintf( ptr, " autocomplete=off" );

	if ( tab_order > 0 )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_tab_order_html(
				tab_order + 1 ) );
	}

	ptr += sprintf( ptr, ">\n</table>\n" );

	return strdup( html );
}

WIDGET_UPLOAD *widget_upload_new( char *widget_name )
{
	WIDGET_UPLOAD *widget_upload;

	if ( !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "widget_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_upload = widget_upload_calloc();
	widget_upload->widget_name = widget_name;

	return widget_upload;
}

WIDGET_UPLOAD *widget_upload_calloc( void )
{
	WIDGET_UPLOAD *widget_upload;

	if ( ! ( widget_upload = calloc( 1, sizeof ( WIDGET_UPLOAD ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return widget_upload;
}

char *widget_upload_prompt_frame_html(
		char *widget_name,
		int tab_order )
{
	if ( !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "widget_name is empty." );

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
	widget_upload_prompt_frame_html_string(
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		widget_container_key(
			widget_name,
			0 /* row_number */ ),
		tab_order );
}

char *widget_upload_prompt_frame_html_string(
		char *widget_container_key,
		int tab_order )
{
	char html[ 512 ];
	char *ptr = html;

	if ( !widget_container_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: container_key is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"<input name=\"%s\" type=file accept=\"*\" value=\"\"",
		widget_container_key );

	if ( tab_order > 0 )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_tab_order_html(
				tab_order ) );
	}

	ptr += sprintf( ptr, ">" );

	return strdup( html );
}

char *widget_upload_edit_frame_html(
		char *application_name,
		char *session_key,
		char *widget_name,
		LIST *query_cell_list,
		char *background_color )
{
	static char *upload_directory;
	static char *document_root;
	char *container_value;
	char *filespecification;
	char *filename_basename;
	char *filename_extension;
	char *hypertext;
	char *link_system_string;

	if ( !application_name
	||   !session_key
	||   !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !upload_directory )
	{
		upload_directory =
			appaserver_parameter_upload_directory();
	}

	if ( !document_root )
	{
		document_root =
			appaserver_parameter_document_root();
	}

	container_value =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		widget_container_value(
			(char)0 /* ATTRIBUTE_MULTI_KEY_DELIMITER */,
			query_cell_list,
			widget_name );

	if ( !container_value || !*container_value ) return strdup( "" );

	filespecification =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		widget_upload_source_filespecification(
			application_name,
			upload_directory,
			container_value );

	if ( !file_exists_boolean( filespecification ) )
	{
		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_non_edit_text_html_string(
			widget_name,
			1 /* no_initial_capital_boolean */,
			container_value );
	}

	filename_basename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		widget_upload_filename_basename(
			container_value );

	filename_extension =
		/* ----------------------------- */
		/* Returns static memory or null */
		/* ----------------------------- */
		widget_upload_filename_extension(
			container_value );

	hypertext =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		widget_upload_hypertext(
			application_name,
			session_key,
			filename_basename,
			filename_extension );

	link_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		widget_upload_link_system_string(
			document_root,
			filespecification,
			hypertext );

	if ( system( link_system_string ) ){}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	widget_upload_edit_frame_html_string(
		background_color,
		container_value,
		hypertext );
}

char *widget_upload_edit_frame_html_string(
		char *background_color,
		char *container_value,
		char *widget_upload_hypertext )
{
	char html[ 1024 ];
	static int target_offset;

	snprintf(
		html,
		sizeof ( html ),
		"<a href=\"%s\" target=upload_%d %s>%s</a>",
		(widget_upload_hypertext)
			? widget_upload_hypertext
			: "",
		++target_offset,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		widget_container_background_color_html(
			background_color ),
		(container_value)
			? container_value
			: "" );

	return strdup( html );
}

char *widget_upload_hypertext(
		char *application_name,
		char *session_key,
		char *filename_basename,
		char *filename_extension )
{
	static char hypertext[ 256 ];

	if ( !application_name
	||   !session_key
	||   !filename_basename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !filename_extension )
	{
		snprintf(
			hypertext,
			sizeof ( hypertext ),
			"/appaserver_data/%s/%s_%s",
			application_name,
			filename_basename,
			session_key );
	}
	else
	{
		snprintf(
			hypertext,
			sizeof ( hypertext ),
			"/appaserver_data/%s/%s_%s.%s",
			application_name,
			filename_basename,
			session_key,
			filename_extension );
	}

	return hypertext;
}

char *widget_upload_link_system_string(
		char *document_root,
		char *source_filespecification,
		char *hypertext )
{
	static char system_string[ 512 ];

	if ( !document_root
	||   !source_filespecification
	||   !hypertext )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"ln -s '%s' '%s%s' 2>/dev/null",
		source_filespecification,
		document_root,
		hypertext );

	return system_string;
}

WIDGET_CHECKBOX *widget_checkbox_new( char *widget_name )
{
	WIDGET_CHECKBOX *widget_checkbox;

	widget_checkbox = widget_checkbox_calloc();
	widget_checkbox->widget_name = widget_name;

	return widget_checkbox;
}

WIDGET_CHECKBOX *widget_checkbox_calloc( void )
{
	WIDGET_CHECKBOX *widget_checkbox;

	if ( ! ( widget_checkbox = calloc( 1, sizeof ( WIDGET_CHECKBOX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return widget_checkbox;
}

char *widget_container_checkbox_html(
		char *state,
		int row_number,
		LIST *query_cell_list,
		char *background_color,
		WIDGET_CHECKBOX *checkbox )
{
	if ( !checkbox )
	{
		char message[ 128 ];

		sprintf(message, "checkbox is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	widget_checkbox_html(
		checkbox->widget_name,
		checkbox->prompt,
		checkbox->onclick,
		checkbox->tab_order,
		checkbox->image_source,
		state,
		row_number,
		query_cell_list,
		background_color );
}

WIDGET_CONTAINER *widget_container_table_heading(
		char *heading_string )
{
	WIDGET_CONTAINER *widget;

	widget =
		widget_container_new(
			table_heading,
			(char *)0 /* widget_name */ );

	widget->table_heading->heading_string = heading_string;

	return widget;
}

char *widget_checkbox_submit_javascript(
		char *form_name,
		char *recall_save_javascript )
{
	char javascript[ STRING_64K ];
	char *ptr = javascript;

	if ( !form_name
	||   !recall_save_javascript )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( recall_save_javascript )
	{
		ptr += sprintf( ptr,
			"%s && ",
			recall_save_javascript );
	}

	sprintf(ptr,
		"push_button_submit( '%s' )",
		form_name );

	return strdup( javascript );
}

char *widget_container_upload_html(
		LIST *query_row_cell_list,
		char *background_color,
		WIDGET_UPLOAD *widget_upload )
{
	if ( !widget_upload
	||   !widget_upload->widget_name )
	{
		char message[ 128 ];

		sprintf(message, "widget_upload is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( query_row_cell_list ) )
	{
		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_upload_prompt_frame_html(
			widget_upload->widget_name,
			widget_upload->tab_order );
	}
	else
	{
		if ( !widget_upload->application_name )
		{
			char message[ 128 ];

			sprintf(message,
				"widget_upload->application_name is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !widget_upload->session_key )
		{
			char message[ 128 ];

			sprintf(message,
				"widget_upload->session_key is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !widget_upload->widget_name )
		{
			char message[ 128 ];

			sprintf(message,
				"widget_upload->widget_name is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		return
		/* ------------------------ */
		/* Returns heap memory null */
		/* ------------------------ */
		widget_upload_edit_frame_html(
			widget_upload->application_name,
			widget_upload->session_key,
			widget_upload->widget_name,
			query_row_cell_list,
			background_color );
	}
}

char *widget_container_yes_no_html(
			char *state,
			int row_number,
			LIST *query_cell_list,
			char *background_color,
			WIDGET_YES_NO *yes_no )
{
	if ( !yes_no )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: yes_no is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	widget_yes_no_html(
		yes_no->widget_name,
		yes_no->post_change_javascript,
		yes_no->tab_order,
		state,
		row_number,
		query_cell_list,
		background_color );
}

char *widget_yes_no_html(
			char *widget_name,
			char *post_change_javascript,
			int tab_order,
			char *state,
			int row_number,
			LIST *query_cell_list,
			char *background_color )
{
	char *container_key;
	char *container_value;
	char *replace = {0};

	container_key =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		widget_container_key(
			widget_name,
			row_number );

	container_value =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		widget_container_value(
			(char)0 /* ATTRIBUTE_MULTI_KEY_DELIMITER */,
			query_cell_list,
			widget_name );

	if ( post_change_javascript )
	{
		replace =
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			javascript_replace(
				post_change_javascript,
				state,
				row_number );
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	widget_yes_no_html_string(
		container_key,
		widget_yes_no_affirmative_boolean( container_value ),
		widget_yes_no_not_affirmative_boolean( container_value ),
		replace,
		tab_order,
		background_color );
}

char *widget_yes_no_html_string(
		char *widget_container_key,
		boolean affirmative_boolean,
		boolean not_affirmative_boolean,
		char *javascript_replace,
		int tab_order,
		char *background_color )
{
	char html_string[ 1024 ];
	char *ptr = html_string;
	LIST *option_list;
	WIDGET_YES_NO_OPTION *widget_yes_no_option;
	char *option_list_html;

	ptr += sprintf(
		ptr,
		"<select name=\"%s\" id=\"%s\"",
		widget_container_key,
		widget_container_key );

	if ( tab_order > 0 )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_tab_order_html(
				tab_order ) );
	}

	if ( javascript_replace && *javascript_replace )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_javascript_html(
				javascript_replace ) );
	}

	if ( background_color && *background_color )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_background_color_html(
				background_color ) );
	}

	ptr += sprintf( ptr, ">\n" );

	option_list = list_new();

	if ( affirmative_boolean )
	{
		widget_yes_no_option =
			widget_yes_no_option_new(
				1 /* yes_boolean */,
				0 /* not no_boolean */,
				0 /* not select_boolean */ );

		list_set(
			option_list,
			widget_yes_no_option->
				widget_drop_down_option );

		widget_yes_no_option =
			widget_yes_no_option_new(
				0 /* not yes_boolean */,
				1 /* no_boolean */,
				0 /* not select_boolean */ );

		list_set(
			option_list,
			widget_yes_no_option->
				widget_drop_down_option );
	}
	else
	if ( not_affirmative_boolean )
	{
		widget_yes_no_option =
			widget_yes_no_option_new(
				0 /* not yes_boolean */,
				1 /* no_boolean */,
				0 /* not select_boolean */ );

		list_set(
			option_list,
			widget_yes_no_option->
				widget_drop_down_option );

		widget_yes_no_option =
			widget_yes_no_option_new(
				1 /* yes_boolean */,
				0 /* not no_boolean */,
				0 /* not select_boolean */ );

		list_set(
			option_list,
			widget_yes_no_option->
				widget_drop_down_option );
	}
	else
	{
		widget_yes_no_option =
			widget_yes_no_option_new(
				0 /* not yes_boolean */,
				0 /* not no_boolean */,
				1 /* select_boolean */ );

		list_set(
			option_list,
			widget_yes_no_option->
				widget_drop_down_option );

		widget_yes_no_option =
			widget_yes_no_option_new(
				1 /* yes_boolean */,
				0 /* not no_boolean */,
				0 /* not select_boolean */ );

		list_set(
			option_list,
			widget_yes_no_option->
				widget_drop_down_option );

		widget_yes_no_option =
			widget_yes_no_option_new(
				0 /* not yes_boolean */,
				1 /* no_boolean */,
				0 /* not select_boolean */ );

		list_set(
			option_list,
			widget_yes_no_option->
				widget_drop_down_option );
	}

	option_list_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		widget_drop_down_option_list_html(
			option_list,
			(char *)0 /* widget_drop_down_value */ );

	if ( !option_list_html )
	{
		char message[ 128 ];

		sprintf(message,
			"widget_drop_down_option_list_html() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf( ptr, "%s\n", option_list_html );
	free( option_list_html );

	sprintf(ptr,
		"%s",
		/* ---------------------- */
		/* Returns static memory. */
		/* ---------------------- */
		widget_drop_down_select_close_tag(
			0 /* not null_boolean */,
			0 /* not not_null_boolean */,
			0 /* not bottom_select_boolean */ ) );

	return strdup( html_string );
}

WIDGET_YES_NO *widget_yes_no_calloc( void )
{
	WIDGET_YES_NO *widget_yes_no;

	if ( ! ( widget_yes_no = calloc( 1, sizeof ( WIDGET_YES_NO ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return widget_yes_no;
}

WIDGET_DROP_DOWN *widget_drop_down_new( char *widget_name )
{
	WIDGET_DROP_DOWN *widget_drop_down =
		widget_drop_down_calloc();

	widget_drop_down->widget_name = widget_name;

	return widget_drop_down;
}

WIDGET_DROP_DOWN *widget_drop_down_calloc( void )
{
	WIDGET_DROP_DOWN *widget_drop_down;

	if ( ! ( widget_drop_down =
			calloc( 1, sizeof ( WIDGET_DROP_DOWN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return widget_drop_down;
}

boolean widget_date_calendar_boolean(
		char *datatype_name,
		char *state,
		char *widget_container_value )
{
	if ( attribute_is_current_date_time( datatype_name )
	&&   string_strcmp( state, APPASERVER_INSERT_STATE ) == 0 )
	{
		return 0;
	}

	if ( attribute_is_date_time( datatype_name )
	&&   widget_container_value
	&&   *widget_container_value )
	{
		return 0;
	}

	return 1;
}

char *widget_date_html(
		char *widget_name,
		char *datatype_name,
		int attribute_width_max_length,
		int display_size,
		boolean null_to_slash,
		char *post_change_javascript,
		int tab_order,
		char *application_name,
		char *login_name,
		char *state,
		int row_number,
		LIST *query_cell_list,
		char *background_color )
{
	char *container_key;
	char *container_value = {0};
	char *javascript_replace_on_change = {0};
	static char *user_date_format_string = {0};
	static char *date_format_template = {0};
	char *calendar_image_tag = {0};

	if ( !widget_name
	||   !datatype_name
	||   !application_name
	||   !login_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	container_key =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		widget_container_key(
			widget_name,
			row_number );

	if (	( attribute_is_current_date_time( datatype_name )
	||	  attribute_is_current_date( datatype_name ) )
	&&	string_strcmp( state, APPASERVER_INSERT_STATE ) == 0 )
	{
		container_value =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			widget_date_login_name_now_date_string(
				attribute_width_max_length,
				application_name,
				login_name );
	}
	else
	{
		container_value =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			widget_container_value(
				(char)0 /* ATTRIBUTE_MULTI_KEY_DELIMITER */,
				query_cell_list,
				widget_name );
	}

	if ( post_change_javascript || null_to_slash )
	{
		javascript_replace_on_change =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			widget_text_replace_javascript(
				post_change_javascript,
				row_number,
				state,
				null_to_slash );
	}

	if ( widget_date_calendar_boolean(
		datatype_name,
		state,
		container_value ) )
	{
		if ( !user_date_format_string )
		{
			user_date_format_string =
				/* ------------------------------------- */
				/* Returns heap memory or program memory */
				/* ------------------------------------- */
				appaserver_user_date_format_string(
					application_name,
					login_name );

			date_format_template =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				widget_date_format_template(
					user_date_format_string );
		}

		calendar_image_tag =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_date_calendar_image_tag(
				container_key,
				date_format_template );
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	widget_date_html_string(
		attribute_width_max_length,
		display_size,
		container_key,
		container_value,
		javascript_replace_on_change,
		calendar_image_tag,
		tab_order,
		background_color );
}

char *widget_date_html_string(
		int attribute_width_max_length,
		int display_size,
		char *container_key,
		char *container_value,
		char *on_change,
		char *calendar_image_tag,
		int tab_order,
		char *background_color )
{
	char html[ 1024 ];
	char *ptr = html;

	if ( !container_key )
	{
		char message[ 128 ];

		sprintf(message, "container_key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		"<input name=\"%s\" id=\"%s\" type=text size=%d maxlength=%d",
		container_key,
		container_key,
		display_size,
		attribute_width_max_length );

	if ( container_value && *container_value )
	{
		ptr += sprintf(
			ptr,
			" value=\"%s\"",
			container_value );
	}

	if ( tab_order > 0 )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_tab_order_html(
				tab_order ) );
	}

	ptr += sprintf(
		ptr,
		" onChange=\"validate_date(this)" );

	if ( on_change && *on_change )
	{
		ptr += sprintf(
			ptr,
			" && %s",
			on_change );
	}

	ptr += sprintf( ptr, "\"" );

	if ( background_color && *background_color )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_background_color_html(
				background_color ) );
	}

	ptr += sprintf( ptr, ">" );

	if ( calendar_image_tag )
	{
		ptr += sprintf(
			ptr,
			"\n%s",
			calendar_image_tag );
	}

	return strdup( html );
}

WIDGET_DATE *widget_date_new( char *widget_name )
{
	WIDGET_DATE *widget_date;

	if ( !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "widget_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_date = widget_date_calloc();
	widget_date->widget_name = widget_name;

	return widget_date;
}

WIDGET_DATE *widget_date_calloc( void )
{
	WIDGET_DATE *widget_date;

	if ( ! ( widget_date = calloc( 1, sizeof ( WIDGET_DATE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return widget_date;
}

char *widget_date_format_template( char *user_date_format )
{
	static char date_format_template[ 16 ];

	if ( !user_date_format )
	{
		char message[ 128 ];

		sprintf(message, "user_date_format is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strcmp( user_date_format, "american" ) == 0 )
	{
		sprintf(date_format_template,
			"%cm/%cd/%cY",
			'%',
			'%',
			'%' );
	}
	else
	if ( strcmp( user_date_format, "military" ) == 0 )
	{
		sprintf(date_format_template,
			"%cd-%cb-%cY",
			'%',
			'%',
			'%' );
	}
	else
	{
		sprintf(date_format_template,
			"%cY-%cm-%cd",
			'%',
			'%',
			'%' );
	}

	return date_format_template;
}

char *widget_date_calendar_image_tag(
		char *widget_container_key,
		char *date_format_template )
{
	static char image_tag[ 512 ];
	static int image_number = 0;

	if ( !widget_container_key
	||   !date_format_template )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(image_tag,
"<img id=date_trigger_%d src=/appaserver_home/zimages/calendar.gif>\n"
"<script type=text/javascript>\n"
"Calendar.setup( {inputField: \"%s\",dateFormat: \"%s\",fdow: 0,trigger: \"date_trigger_%d\",align: \"M\",onSelect: function() {this.hide() }});\n"
"</script>\n",
		image_number,
		widget_container_key,
		date_format_template,
		image_number );

	image_number++;

	return image_tag;
}

WIDGET_MULTI_EMPTY_DROP_DOWN *widget_multi_empty_drop_down_new(
			char *widget_name )
{
	WIDGET_MULTI_EMPTY_DROP_DOWN *
		widget_multi_empty_drop_down;

	if ( !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "widget_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_multi_empty_drop_down = widget_multi_empty_drop_down_calloc();

	widget_multi_empty_drop_down->
		widget_drop_down_select_tag =
			widget_drop_down_select_tag(
				WIDGET_MULTI_DISPLAY_SIZE,
				0 /* tab_order */,
				1 /* multi_select_boolean */,
				widget_name /* widget_container_key */,
				(char *)0 /* widget_drop_down_row_number_id */,
				(char *)0 /* javascript_replace */,
				(char *)0 /* background_color */,
				0 /* not top_select_boolean */ );

	widget_multi_empty_drop_down->
		widget_drop_down_select_close_tag =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_drop_down_select_close_tag(
				0 /* not null_boolean */,
				0 /* not not_null_boolean */,
				0 /* not bottom_select_boolean */ );

	widget_multi_empty_drop_down->html =
		widget_multi_empty_drop_down_html(
			widget_multi_empty_drop_down->
				widget_drop_down_select_tag,
			widget_multi_empty_drop_down->
				widget_drop_down_select_close_tag );

	return widget_multi_empty_drop_down;
}

WIDGET_MULTI_EMPTY_DROP_DOWN *widget_multi_empty_drop_down_calloc( void )
{
	WIDGET_MULTI_EMPTY_DROP_DOWN *
		widget_multi_empty_drop_down;

	if ( ! ( widget_multi_empty_drop_down =
			calloc(	1,
				sizeof ( WIDGET_MULTI_EMPTY_DROP_DOWN ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return widget_multi_empty_drop_down;
}

char *widget_multi_empty_drop_down_html(
			char *select_tag,
			char *select_close_tag )
{
	char html[ 1024 ];

	if ( !select_tag
	||   !select_close_tag )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(html,
		"%s\n%s",
		select_tag,
		select_close_tag );

	return strdup( html );
}

LIST *widget_drop_down_option_list(
		const char attribute_multi_key_delimiter,
		const char widget_drop_down_label_delimiter /* '|' */,
		const char widget_drop_down_extra_delimiter /* '[' */,
		const char *widget_drop_down_dash_delimiter,
		LIST *delimited_list,
		boolean no_initial_capital )
{
	LIST *list;
	char *delimiter_string;
	char *delimited_string;

	list = list_new();

	delimiter_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		sql_delimiter_string(
			attribute_multi_key_delimiter );

	if ( list_rewind( delimited_list ) )
	do {
		delimited_string = list_get( delimited_list );

		if ( !*delimited_string ) continue;

		list_set(
			list,
			widget_drop_down_option_delimited_new(
				widget_drop_down_label_delimiter,
				widget_drop_down_extra_delimiter,
				widget_drop_down_dash_delimiter,
				no_initial_capital,
				delimiter_string,
				delimited_string ) );

	} while ( list_next( delimited_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

WIDGET_DROP_DOWN_OPTION *widget_drop_down_option_delimited_new(
		const char widget_drop_down_label_delimiter /* '|' */,
		const char widget_drop_down_extra_delimiter /* '[' */,
		const char *widget_drop_down_dash_delimiter,
		boolean no_initial_capital,
		char *sql_delimiter_string,
		char *delimited_string )
{
	WIDGET_DROP_DOWN_OPTION *widget_drop_down_option;

	if ( !delimited_string || !*delimited_string )
	{
		char message[ 128 ];

		sprintf(message, "delimited_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_drop_down_option = widget_drop_down_option_calloc();

	widget_drop_down_option->value_string =
		strdup(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_drop_down_option_value_string(
				widget_drop_down_label_delimiter,
				widget_drop_down_extra_delimiter,
				delimited_string ) );

	widget_drop_down_option->display_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_drop_down_option_display_string(
			widget_drop_down_label_delimiter,
			widget_drop_down_dash_delimiter,
			no_initial_capital,
			sql_delimiter_string,
			delimited_string );

	widget_drop_down_option->tag =
		strdup(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_drop_down_option_tag(
				widget_drop_down_option->value_string,
				widget_drop_down_option->display_string ) );

	return widget_drop_down_option;
}

WIDGET_DROP_DOWN_OPTION *widget_drop_down_option_calloc( void )
{
	WIDGET_DROP_DOWN_OPTION *widget_drop_down_option;

	if ( ! ( widget_drop_down_option =
			calloc( 1, sizeof ( WIDGET_DROP_DOWN_OPTION ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return widget_drop_down_option;
}

char *widget_drop_down_option_value_string(
		char widget_drop_down_label_delimiter /* '|' */,
		char widget_drop_down_extra_delimiter /* '[' */,
		char *delimited_string )
{
	static char value_string[ 256 ];

	if ( !delimited_string || !*delimited_string )
	{
		char message[ 128 ];

		sprintf(message, "delimited_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( string_character_exists(
			delimited_string,
			widget_drop_down_label_delimiter ) )
	{
		piece(	value_string,
			widget_drop_down_label_delimiter,
			delimited_string,
			0 );
	}
	else
	if ( string_character_exists(
			delimited_string,
			widget_drop_down_extra_delimiter ) )
	{
		piece(	value_string,
			widget_drop_down_extra_delimiter,
			delimited_string,
			0 );

		/* ----------------------------------------- */
		/* Returns buffer with end shortened (maybe) */
		/* ----------------------------------------- */
		string_trim_right_spaces( value_string /* buffer */ );
	}
	else
	{
		strcpy( value_string, delimited_string );
	}

	return value_string;
}

char *widget_drop_down_option_display_string(
		const char widget_drop_down_label_delimiter /* '|' */,
		const char *widget_drop_down_dash_delimiter,
		boolean no_initial_capital,
		char *sql_delimiter_string,
		char *delimited_string )
{
	char display_string[ 1024 ];

	if ( !delimited_string || !*delimited_string )
	{
		char message[ 128 ];

		sprintf(message, "delimited_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( string_character_exists(
		delimited_string,
	      	widget_drop_down_label_delimiter ) )
	{
		piece(	display_string,
			widget_drop_down_label_delimiter,
			delimited_string,
			1 );
	}
	else
	{
		strcpy( display_string, delimited_string );
	}

	string_search_replace(
		display_string
			/* source_destination */,
		sql_delimiter_string
			/* search_string */,
		(char *)widget_drop_down_dash_delimiter
			/* replace_string */ );

	if ( !no_initial_capital )
	{
		string_initial_capital( display_string, display_string );
	}

	return strdup( display_string );
}

char *widget_drop_down_html_string(
		char *select_tag,
		char *relation_join_html,
		char *option_list_html,
		char *select_close_tag )
{
	char string[ STRING_704K ];

	if ( !select_tag
	&&   !select_close_tag )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if (	strlen( select_tag ) +
		string_strlen( relation_join_html ) +
		string_strlen( option_list_html ) +
		strlen( select_close_tag ) +
		2 >= STRING_704K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_704K );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( relation_join_html )
	{
		sprintf(string,
			"%s\n%s\n%s",
			select_tag,
			relation_join_html,
			select_close_tag );
	}
	else
	if ( option_list_html )
	{
		sprintf(string,
			"%s\n%s\n%s",
			select_tag,
			option_list_html,
			select_close_tag );
	}
	else
	{
		sprintf(string,
			"%s\n%s",
			select_tag,
			select_close_tag );
	}

	return strdup( string );
}

char *widget_drop_down_html(
		char *widget_name /* mutually exclusive */,
		LIST *attribute_name_list /* mutually exclusive */,
		boolean top_select_boolean,
		boolean null_boolean,
		boolean not_null_boolean,
		boolean bottom_select_boolean,
		int display_size,
		int tab_order,
		boolean multi_select_boolean,
		char *post_change_javascript,
		char *state,
		int row_number,
		LIST *query_row_cell_list,
		RELATION_JOIN_ROW *relation_join_row,
		LIST *widget_drop_down_option_list,
		LIST *foreign_key_list,
		char *background_color,
		char *widget_drop_down_id )
{
	char *drop_down_name;
	int drop_down_display_size;
	char *container_key;
	char *widget_id;
	char *drop_down_value = {0};
	char *replace = {0};
	char *select_tag;
	char *relation_join_html = {0};
	char *option_list_html = {0};
	char *select_close_tag;
	char *html_string;

	if ( !widget_name
	&&   !list_length( attribute_name_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"both widget_name and attribute_name_list are empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( widget_name
	&&   list_length( attribute_name_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"both widget_name and attribute_name_list are set." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	drop_down_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		widget_drop_down_name(
			ATTRIBUTE_MULTI_KEY_DELIMITER,
			widget_name /* mutually exclusive */,
			attribute_name_list /* mutually exclusive */ );

	drop_down_display_size =
		widget_drop_down_display_size(
			WIDGET_MULTI_DISPLAY_SIZE,
			display_size,
			multi_select_boolean );

	container_key =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		widget_container_key(
			drop_down_name,
			row_number );
 
	widget_id =
		/* --------------------------------------------- */
		/* Returns static memory or widget_container_key */
		/* --------------------------------------------- */
		widget_drop_down_row_number_id(
			widget_drop_down_id,
			row_number,
			container_key );

	if ( list_length( query_row_cell_list ) )
	{
		drop_down_value =
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			widget_drop_down_value(
				ATTRIBUTE_MULTI_KEY_DELIMITER,
				widget_name,
				query_row_cell_list,
				relation_join_row,
				foreign_key_list );
	}

	if ( post_change_javascript )
	{
		replace =
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			javascript_replace(
				post_change_javascript,
				state,
				row_number );
	}

	top_select_boolean =
		widget_drop_down_top_select_boolean(
			top_select_boolean,
			drop_down_value );

	select_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_drop_down_select_tag(
			drop_down_display_size,
			tab_order,
			multi_select_boolean,
			container_key,
			widget_id /* widget_drop_down_row_number_id */,
			replace,
			background_color,
			top_select_boolean );

	if ( relation_join_row )
	{
		relation_join_html =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			widget_drop_down_relation_join_html(
				widget_name,
				relation_join_row );
	}

	if ( !relation_join_html )
	{
		option_list_html =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			widget_drop_down_option_list_html(
				widget_drop_down_option_list,
				drop_down_value );
	}

	bottom_select_boolean =
		widget_drop_down_bottom_select_boolean(
			bottom_select_boolean,
			top_select_boolean );

	select_close_tag =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		widget_drop_down_select_close_tag(
			null_boolean,
			not_null_boolean,
			bottom_select_boolean );

	html_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_drop_down_html_string(
			select_tag,
			relation_join_html,
			option_list_html,
			select_close_tag );

	free( select_tag );

	if ( relation_join_html ) free( relation_join_html );
	if ( option_list_html ) free( option_list_html );

	return html_string;
}

char *widget_drop_down_option_list_html(
		LIST *widget_drop_down_option_list,
		char *widget_drop_down_value )
{
	char html[ STRING_704K ];
	char *ptr = html;
	WIDGET_DROP_DOWN_OPTION *widget_drop_down_option;
	WIDGET_DROP_DOWN_OPTION *widget_drop_down_option_existing = {0};

	*ptr = '\0';

	if ( widget_drop_down_value
	&&   *widget_drop_down_value )
	{
		if ( ( widget_drop_down_option_existing =
				widget_drop_down_option_seek(
					widget_drop_down_value,
					widget_drop_down_option_list ) ) )
		{
			if ( !widget_drop_down_option_existing->tag )
			{
				char message[ 128 ];

				sprintf(message,
			"widget_drop_down_option_existsing->tag is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			ptr += sprintf(
				ptr,
				"%s",
				widget_drop_down_option_existing->tag );
		}
		else
		{
			char *display_string;
			char *option_tag;

			display_string =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				widget_drop_down_option_display_string(
					WIDGET_DROP_DOWN_LABEL_DELIMITER,
					WIDGET_DROP_DOWN_DASH_DELIMITER,
					0 /* not no_initial_capital_boolean */,
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					sql_delimiter_string( SQL_DELIMITER ),
					widget_drop_down_value );

			option_tag =
				widget_drop_down_option_tag(
					widget_drop_down_value,
					display_string );

			ptr += sprintf(
				ptr,
				"%s",
				option_tag );
		}
	}

	if ( list_rewind( widget_drop_down_option_list ) )
	do {
		widget_drop_down_option =
			list_get(
				widget_drop_down_option_list );

		if ( 	widget_drop_down_option_existing ==
			widget_drop_down_option  )
		{
			continue;
		}

		if ( !widget_drop_down_option->tag )
		{
			char message[ 256 ];

			sprintf(message,
"widget_drop_down_option = [%s], widget_drop_down_option->tag is empty.",
				widget_drop_down_option->value_string );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if (	strlen( html ) +
			strlen( widget_drop_down_option->tag ) +
			1 >= STRING_704K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_704K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}
	
		if ( ptr != html ) ptr += sprintf( ptr, "\n" );

		ptr += sprintf(
			ptr,
			"%s",
			widget_drop_down_option->tag );

	} while ( list_next( widget_drop_down_option_list ) );

	return strdup( html );
}

WIDGET_DROP_DOWN_OPTION *widget_drop_down_option_seek(
			char *widget_drop_down_value,
			LIST *widget_drop_down_option_list )
{
	WIDGET_DROP_DOWN_OPTION *widget_drop_down_option;

	if ( !widget_drop_down_value
	||   !list_rewind( widget_drop_down_option_list ) )
	{
		return (WIDGET_DROP_DOWN_OPTION *)0;
	}

	do {
		widget_drop_down_option =
			list_get(
				widget_drop_down_option_list );

		if ( strcasecmp(
			widget_drop_down_value,
			widget_drop_down_option->value_string ) == 0 )
		{
			return widget_drop_down_option;
		}

	} while ( list_next( widget_drop_down_option_list ) );

	return (WIDGET_DROP_DOWN_OPTION *)0;
}

char *widget_table_edit_row_html(
		char *table_edit_state,
		int row_number,
		LIST *query_row_cell_list,
		RELATION_JOIN_ROW *relation_join_row,
		LIST *apply_widget_container_list,
		char *form_background_color,
		boolean hidden_only )
{
	char html[ STRING_640K ];
	char *ptr = html;
	WIDGET_CONTAINER *widget_container;
	char *container_html;

	if ( !list_rewind( apply_widget_container_list ) )
		return strdup( "" );

	*ptr = '\0';

	do {
		widget_container =
			list_get(
				apply_widget_container_list );

		if ( hidden_only
		&&   widget_container->widget_type != hidden )
		{
			continue;
		}
		else
		if ( !hidden_only
		&& widget_container->widget_type == hidden )
		{
			continue;
		}

		container_html =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			widget_container_html(
				table_edit_state,
				row_number,
				query_row_cell_list,
				relation_join_row,
				form_background_color,
				widget_container );

		if (	strlen( html ) +
			strlen( container_html ) +
			1 >= STRING_640K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_640K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"%s\n",
			container_html );

		free( container_html );

	} while ( list_next( apply_widget_container_list ) );

	return strdup( html );
}

char *widget_container_list_html(
		char *state,
		int row_number /* -1 to omit index suffix */,
		char *background_color,
		LIST *widget_container_list )
{
	char html[ STRING_704K ];
	char *ptr = html;
	WIDGET_CONTAINER *widget_container;
	char *container_html = {0};

	*ptr = '\0';

	if ( list_rewind( widget_container_list ) )
	do {
		widget_container =
			list_get(
				widget_container_list );

		if ( widget_container->widget_type == hidden )
		{
			continue;
		}

		container_html =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			widget_container_html(
				state,
				row_number,
				(LIST *)0 /* query_cell_list */,
				(RELATION_JOIN_ROW *)0,
				background_color,
				widget_container );

		if (	strlen( html ) + strlen( container_html ) + 1 >=
			STRING_704K )
		{
			char message[ 128 ];

			fprintf( stderr,
				"%s/%s()/%d: container_html=[\n%s\n]\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				container_html );

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_704K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"%s\n",
			container_html );

		free( container_html );

	} while ( list_next( widget_container_list ) );

	return strdup( html );
}

char *widget_container_list_hidden_html(
		int row_number,
		LIST *widget_container_list )
{
	char html[ 65536 ];
	char *ptr = html;
	WIDGET_CONTAINER *widget_container;
	char *container_html;

	if ( !list_rewind( widget_container_list ) ) return (char *)0;

	do {
		widget_container =
			list_get(
				widget_container_list );

		if ( widget_container->widget_type != hidden )
		{
			continue;
		}

		container_html =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			widget_container_html(
				(char *)0 /* state */,
				row_number,
				(LIST *)0 /* query_cell_list */,
				(RELATION_JOIN_ROW *)0,
				(char *)0 /* background_color */,
				widget_container );

		ptr += sprintf(
			ptr,
			"%s\n",
			container_html );

		free( container_html );

	} while ( list_next( widget_container_list ) );

	return strdup( html );
}

char *widget_container_ajax_html(
		LIST *widget_container_list,
		LIST *query_row_cell_list,
		int row_number,
		char *background_color )
{
	if ( !list_length( widget_container_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

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
	widget_ajax_html(
		widget_container_list,
		query_row_cell_list,
		row_number,
		background_color );
}

char *widget_container_radio_html( WIDGET_RADIO *radio )
{
	if ( !radio )
	{
		char message[ 128 ];

		sprintf(message, "radio is empty." );

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
	widget_radio_html(
		radio->widget_name,
		radio->value,
		radio->prompt,
		radio->onclick );
}

char *widget_container_anchor_html( WIDGET_ANCHOR *anchor )
{
	if ( !anchor )
	{
		char message[ 128 ];

		sprintf(message, "anchor is empty." );

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
	widget_anchor_html(
		anchor->hypertext_reference,
		anchor->target_frame,
		anchor->prompt );
}

char *widget_container_encrypt_html(
		int row_number,
		LIST *query_cell_list,
		WIDGET_ENCRYPT *encrypt )
{
	if ( !encrypt )
	{
		char message[ 128 ];

		sprintf(message, "encrypt is empty." );

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
	widget_encrypt_html(
		WIDGET_TEXT_DEFAULT_DISPLAY_SIZE,
		row_number,
		query_cell_list,
		encrypt->widget_name,
		encrypt->attribute_width_max_length,
		encrypt->tab_order );
}

WIDGET_DROP_DOWN_OPTION *widget_drop_down_option_new( char *string )
{
	if ( !string || !*string )
	{
		char message[ 128 ];

		sprintf(message, "string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	widget_drop_down_option_delimited_new(
		0 /* widget_drop_down_label_delimiter */,
		0 /* widget_drop_down_extra_delimiter */,
		(char *)0 /* widget_drop_down_dash_delimiter */,
		0 /* not no_initial_capital */,
		(char *)0 /* sql_delimiter_string */,
		string );
}

int widget_drop_down_display_size(
		int widget_multi_display_size,
		int display_size,
		boolean multi_select_boolean )
{
	if ( display_size )
		return display_size;
	else
	if ( multi_select_boolean )
		return widget_multi_display_size;
	else
		return 1;
}

char *widget_hidden_value(
		LIST *query_row_cell_list,
		char *widget_name,
		char *value_string )
{
	char *value;

	if ( !widget_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: widget_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( value_string ) return value_string;

	value =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		widget_container_value(
			(char)0 /* ATTRIBUTE_MULTI_KEY_DELIMITER */,
			query_row_cell_list,
			widget_name );

	return value;
}

char *widget_hidden_container_key(
			char *widget_name,
			int row_number )
{
	if ( !widget_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: widget_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( row_number )
	{
		return
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		widget_container_key(
			widget_name,
			row_number );
	}
	else
	{
		return widget_name;
	}
}

char *widget_drop_down_value(
		const char attribute_multi_key_delimiter,
		char *widget_name,
		LIST *query_cell_list,
		RELATION_JOIN_ROW *relation_join_row,
		LIST *foreign_key_list )
{
	static char drop_down_value[ 256 ];
	char *ptr = drop_down_value;
	QUERY_CELL *query_cell;
	char *foreign_key;
	LIST *folder_name_list;

	folder_name_list =
		/* ----------------------------- */
		/* Returns static LIST * or null */
		/* ----------------------------- */
		relation_join_row_folder_name_list( relation_join_row );

	if ( list_string_exists(
		widget_name,
		folder_name_list ) )
	{
		return NULL;
	}

	if ( !list_rewind( foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "foreign_key_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	*drop_down_value = '\0';

	do {
		foreign_key = list_get( foreign_key_list );

		if ( ! ( query_cell =
				query_cell_seek(
					foreign_key /* attribute_name */,
					query_cell_list ) ) )
		{
			char message[ 2048 ];

			sprintf(message,
		"for widget_name=%s, query_cell_seek(%s,%s) returned empty.",
				widget_name,
				foreign_key,
				query_cell_list_display(
					query_cell_list ) );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !query_cell->select_datum )
		{
			char message[ 128 ];

			sprintf(message,
		"for attribute_name=[%s], query_cell->select_datum is empty.",
				query_cell->attribute_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if (	strlen( drop_down_value ) +
			strlen( query_cell->select_datum ) + 1 >= 256 )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				256 );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( ptr != drop_down_value )
		{
			ptr += sprintf(
				ptr,
				"%c",
				attribute_multi_key_delimiter );
		}

		ptr += sprintf(
			ptr,
			"%s",
			query_cell->select_datum );

	} while ( list_next( foreign_key_list ) );

	if ( !*drop_down_value )
		return NULL;
	else
		return drop_down_value;
}

LIST *widget_container_dictionary_hidden_list( DICTIONARY *dictionary )
{
	LIST *widget_container_list;
	LIST *key_list;
	char *key;
	char *data;
	WIDGET_CONTAINER *widget_container;

	if ( !dictionary_length( dictionary ) ) return (LIST *)0;

	widget_container_list = list_new();
	key_list = dictionary_key_list( dictionary );
	list_rewind( key_list );

	do {
		key = list_get( key_list );

		data = dictionary_get( key, dictionary );

		list_set(
			widget_container_list,
			( widget_container =
				widget_container_new(
					hidden,
					key ) ) );

		widget_container->hidden->value_string = data;

	} while ( list_next( key_list ) );

	return widget_container_list;
}

WIDGET_ANCHOR *widget_anchor_calloc( void )
{
	WIDGET_ANCHOR *widget_anchor;

	if ( ! ( widget_anchor = calloc( 1, sizeof ( WIDGET_ANCHOR ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return widget_anchor;
}

char *widget_anchor_html(
			char *hypertext_reference,
			char *target_frame,
			char *prompt )
{
	char html[ 1024 ];

	if ( !hypertext_reference
	||   !target_frame
	||   !prompt )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(html,
		"<a href=\"%s\" target=%s>%s</a>",
		hypertext_reference,
		target_frame,
		prompt );

	return strdup( html );
}

WIDGET_CONTAINER *widget_lookup_checkbox_container(
			char *checkbox_label )
{
	WIDGET_CONTAINER *widget_container;

	if ( !checkbox_label )
	{
		char message[ 128 ];

		sprintf(message, "checkbox_label is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_container =
		widget_container_new(
			checkbox,
			checkbox_label );

	widget_container->checkbox->prompt = "Lookup";

	return widget_container;
}

WIDGET_CONTAINER *widget_confirm_checkbox_container(
			char *checkbox_label,
			char *checkbox_prompt )
{
	WIDGET_CONTAINER *widget_container;

	if ( !checkbox_label
	||   !checkbox_prompt )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_container =
		widget_container_new(
			checkbox,
			checkbox_label );

	widget_container->checkbox->prompt = checkbox_prompt;

	return widget_container;
}

WIDGET_CONTAINER *widget_submit_button_container( void )
{
	WIDGET_CONTAINER *submit_widget_container =
		widget_container_new(
			button, (char *)0 );

	submit_widget_container->widget_button->button =
		button_submit(
			(char *)0 /* multi_select_all_javascript */,
			(char *)0 /* recall_save_javascript */,
			(char *)0 /* verify_attribute_widths_javascript */,
			0 /* form_number */ );

	return submit_widget_container;
}

boolean widget_checkbox_dictionary_checked(
		char *widget_name,
		DICTIONARY *dictionary )
{
	char *get;

	if ( !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "widget_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	get = dictionary_get( widget_name, dictionary );

	if ( !get )
		return 0;
	else
	if ( tolower( *get ) == 'y' )
		return 1;
	else
		return 0;
}

LIST *widget_button_submit_reset_container_list( void )
{
	LIST *widget_container_list = list_new();
	WIDGET_CONTAINER *widget_container;

	list_set(
		widget_container_list,
		( widget_container =
			widget_container_new(
				button, (char *)0 ) ) );

	widget_container->widget_button->button =
		button_submit(
			(char *)0 /* form_multi_select_all_javascript */,
			(char *)0 /* recall_save_javascript */,
			(char *)0 /* form_verify_notepad_widths */,
			0 /* form_number */ );

	list_set(
		widget_container_list,
		( widget_container =
			widget_container_new(
				button, (char *)0 ) ) );

	widget_container->widget_button->button =
		button_reset(
			(char *)0 /* javascript_replace */,
			0 /* form_number */ );

	list_set(
		widget_container_list,
		( widget_container =
			widget_container_new(
				button, (char *)0 ) ) );

	widget_container->widget_button->button = button_back();

	list_set(
		widget_container_list,
		( widget_container =
			widget_container_new(
				button, (char *)0 ) ) );

	widget_container->widget_button->button = button_forward();

	return widget_container_list;
}

WIDGET_RADIO *widget_radio_new( char *widget_name )
{
	WIDGET_RADIO *widget_radio;

	if ( !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "widget_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_radio = widget_radio_calloc();
	widget_radio->widget_name = widget_name;

	return widget_radio;
}

WIDGET_RADIO *widget_radio_calloc( void )
{
	WIDGET_RADIO *widget_radio;

	if ( ! ( widget_radio = calloc( 1, sizeof ( WIDGET_RADIO ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return widget_radio;
}

char *widget_radio_html(
		char *widget_name,
		char *value,
		char *prompt,
		char *onclick )
{
	char html[ 1024 ];
	char *ptr = html;

	if ( !widget_name
	||   !value
	||   !prompt )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		"<input name=%s type=radio value=\"%s\"",
		widget_name,
		value );

	if ( onclick )
	{
		ptr += sprintf(
			ptr,
			" onclick=\"%s\"",
			onclick );
	}

	ptr += sprintf(
		ptr,
		">%s",
		prompt );

	return strdup( html );
}

boolean widget_drop_down_top_select_boolean(
		boolean top_select_boolean,
		char *widget_drop_down_value )
{
	if ( !top_select_boolean ) return 0;

	return
	(widget_drop_down_value) ? 0 : 1;
}

boolean widget_drop_down_bottom_select_boolean(
		boolean bottom_select_boolean,
		boolean top_select_boolean )
{
	if ( !bottom_select_boolean )
		return 0;
	else
		return 1 - top_select_boolean;
}

char *widget_drop_down_option_static_tag( char *widget_operator )
{
	char buffer[ 16 ];

	if ( !widget_operator )
	{
		char message[ 128 ];

		sprintf(message, "widget_operator is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* --------------------- */
	/* Returns static memory */
	/* --------------------- */
	widget_drop_down_option_tag(
		widget_operator
			/* value_string */,
		string_initial_capital(
			buffer,
			widget_operator ) /* display_string */ );
}

LIST *widget_hidden_container_list(
		char *dictionary_separate_prefix,
		DICTIONARY *dictionary )
{
	LIST *widget_container_list;
	LIST *key_list;
	char *key;
	char *hidden_key;
	WIDGET_CONTAINER *widget_container;
	char *get;

	if ( !dictionary_separate_prefix )
	{
		char message[ 128 ];

		sprintf(message, "dictionary_separate_prefix is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !dictionary_length( dictionary ) ) return NULL;

	widget_container_list = list_new();

	key_list =
		dictionary_key_list(
			dictionary );

	list_rewind( key_list );

	do {
		key = list_get(  key_list );

		hidden_key =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			widget_hidden_key(
				dictionary_separate_prefix,
				key );

		widget_container =
			widget_container_new(
				hidden,
				hidden_key /* widget_name */ );

		get =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				key,
				dictionary );

		widget_container->hidden->value_string = get;

		list_set(
			widget_container_list,
			widget_container );

	} while ( list_next( key_list ) );

	return widget_container_list;
}

char *widget_hidden_key(
		char *dictionary_separate_prefix,
		char *key )
{
	char hidden_key[ 128 ];

	if ( !dictionary_separate_prefix
	||   !key )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(hidden_key,
		"%s%s",
		dictionary_separate_prefix,
		key );

	return strdup( hidden_key );
}

WIDGET_YES_NO_OPTION *widget_yes_no_option_new(
		boolean yes_boolean,
		boolean no_boolean,
		boolean select_boolean )
{
	WIDGET_YES_NO_OPTION *widget_yes_no_option;

	widget_yes_no_option = widget_yes_no_option_calloc();

	widget_yes_no_option->widget_drop_down_option =
		widget_drop_down_option_calloc();

	if ( yes_boolean )
	{
		widget_yes_no_option->
			widget_drop_down_option->
			value_string = "y";

		widget_yes_no_option->
			widget_drop_down_option->
			display_string = "Yes";
	}
	else
	if ( no_boolean )
	{
		widget_yes_no_option->
			widget_drop_down_option->
			value_string = "n";

		widget_yes_no_option->
			widget_drop_down_option->
			display_string = "No";
	}
	else
	if ( select_boolean )
	{
		char display[ 8 ];

		widget_yes_no_option->
			widget_drop_down_option->
			value_string = WIDGET_SELECT_OPERATOR;

		widget_yes_no_option->
			widget_drop_down_option->
			display_string =
				strdup(
					string_initial_capital(
						display,
						WIDGET_SELECT_OPERATOR ) );
	}
	else
	{
		char message[ 128 ];

		sprintf(message, "cannot determine option to set." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		/* Stub */
		/* ---- */
		exit( 1 );
	}

	widget_yes_no_option->widget_drop_down_option->tag =
		strdup(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_drop_down_option_tag(
				widget_yes_no_option->
					widget_drop_down_option->
					value_string,
				widget_yes_no_option->
					widget_drop_down_option->
					display_string ) );

	return widget_yes_no_option;
}

WIDGET_YES_NO_OPTION *widget_yes_no_option_calloc( void )
{
	WIDGET_YES_NO_OPTION *widget_yes_no_option;

	if ( ! ( widget_yes_no_option =
			calloc( 1,
				sizeof ( WIDGET_YES_NO_OPTION ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return widget_yes_no_option;
}

WIDGET_YES_NO *widget_yes_no_new( char *widget_name )
{
	WIDGET_YES_NO *widget_yes_no;

	if ( !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "widget_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_yes_no = widget_yes_no_calloc();
	widget_yes_no->widget_name = widget_name;

	return widget_yes_no;
}

LIST *widget_container_back_to_top_list( void )
{
	LIST *widget_container_list = list_new();
	WIDGET_CONTAINER *widget_container;

	list_set(
		widget_container_list,
		widget_container_new(
			table_open, (char *)0 ) );

	list_set(
		widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	list_set(
		widget_container_list,
		widget_container_new(
			table_data, (char *)0 ) );

	list_set(
		widget_container_list,
		( widget_container =
			widget_container_new(
				button, (char *)0 ) ) );

	widget_container->
		widget_button->
		button = button_to_top();

	list_set(
		widget_container_list,
		widget_container_new(
			table_close, (char *)0 ) );

	return widget_container_list;
}

int widget_text_display_size(
		int widget_text_default_max_length,
		int attribute_width )
{
	if ( attribute_width > widget_text_default_max_length )
		return widget_text_default_max_length;
	else
		return attribute_width;
}

LIST *widget_container_heading_list( LIST *heading_label_list )
{
	char heading_string[ 128 ];
	LIST *widget_container_list = list_new();

	list_set(
		widget_container_list,
		widget_container_new(
			table_row, (char *)0 ) );

	if ( list_rewind( heading_label_list ) )
	do {
		list_set(
			widget_container_list,
			widget_container_new(
				table_heading,
				strdup(
				     string_initial_capital(
					heading_string,
					list_get(
					    heading_label_list ) ) ) ) );

	} while ( list_next( heading_label_list ) );

	return widget_container_list;
}

char *widget_time_html(
		char *widget_name,
		int attribute_width_max_length,
		int widget_text_display_size,
		boolean null_to_slash,
		char *post_change_javascript,
		int tab_order,
		char *state,
		int row_number,
		LIST *query_cell_list,
		char *background_color )
{
	char *container_key;
	char *container_value;
	char *replace_javascript = {0};
	char *html_string;

	if ( !widget_name
	||   !attribute_width_max_length
	||   !widget_text_display_size )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	container_key =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		widget_container_key(
			widget_name,
			row_number );

	container_value =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		widget_container_value(
			(char)0 /* ATTRIBUTE_MULTI_KEY_DELIMITER */,
			query_cell_list,
			widget_name );

	if ( post_change_javascript || null_to_slash )
	{
		replace_javascript =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			widget_text_replace_javascript(
				post_change_javascript,
				row_number,
				state,
				null_to_slash );
	}

	html_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_time_html_string(
			attribute_width_max_length,
			widget_text_display_size,
			tab_order,
			background_color,
			container_key,
			container_value,
			replace_javascript );

	if ( replace_javascript ) free( replace_javascript );

	return html_string;
}

char *widget_time_html_string(
		int attribute_width_max_length,
		int widget_text_display_size,
		int tab_order,
		char *background_color,
		char *container_key,
		char *container_value,
		char *widget_text_replace_javascript )
{
	char html[ 1024 ];
	char *ptr = html;

	if ( !container_key )
	{
		char message[ 128 ];

		sprintf(message, "container_key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		"<input name=\"%s\" id=\"%s\" type=text size=%d maxlength=%d",
		container_key,
		container_key,
		widget_text_display_size,
		attribute_width_max_length );

	if ( container_value && *container_value )
	{
		ptr += sprintf(
			ptr,
			" value=\"%s\"",
			container_value );
	}

	if ( tab_order > 0 )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_tab_order_html(
				tab_order ) );
	}

	ptr += sprintf(
		ptr,
		" onChange=\"validate_time(this)" );

	if ( widget_text_replace_javascript
	&&   *widget_text_replace_javascript )
	{
		ptr += sprintf(
			ptr,
			" && %s",
			widget_text_replace_javascript );
	}

	ptr += sprintf( ptr, "\"" );

	if ( background_color && *background_color )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_container_background_color_html(
				background_color ) );
	}

	ptr += sprintf(
		ptr,
		">\n" );

	return strdup( html );
}

WIDGET_TIME *widget_time_new( char *widget_name )
{
	WIDGET_TIME *widget_time;

	if ( !widget_name )
	{
		char message[ 128 ];

		sprintf(message, "widget_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_time = widget_time_calloc();
	widget_time->widget_name = widget_name;

	return widget_time;
}

WIDGET_TIME *widget_time_calloc( void )
{
	WIDGET_TIME *widget_time;

	if ( ! ( widget_time = calloc( 1, sizeof ( WIDGET_TIME ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return widget_time;
}

char *widget_encrypt_compare_name(
		const char *widget_encrypt_compare_label,
		char *widget_name,
		int row_number )
{
	static char compare_name[ 256 ];

	if ( !widget_name
	||   !row_number )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(compare_name,
		"%s_%s_%d",
		(widget_name)
			? widget_name
			: "",
		widget_encrypt_compare_label,
		row_number );

	return compare_name;
}

char *widget_dictionary_hidden_html( DICTIONARY *dictionary )
{
	LIST *hidden_list;

	if ( !dictionary_length( dictionary ) ) return strdup( "" );

	hidden_list =
		widget_container_dictionary_hidden_list(
			dictionary );

	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	widget_container_list_hidden_html(
		-1 /* row_number */,
		hidden_list );
}

char *widget_drop_down_relation_join_html(
		char *widget_name,
		RELATION_JOIN_ROW *relation_join_row )
{
	char html[ STRING_64K ];
	char *ptr = html;
	RELATION_JOIN_FOLDER *relation_join_folder;
	char *delimited_string;
	char buffer[ 1024 ];
	char *tag;

	if ( !relation_join_row ) return NULL;

	relation_join_folder =
		relation_join_folder_seek(
			widget_name /* folder_name */,
			relation_join_row->relation_join_folder_list );

	if ( !relation_join_folder ) return NULL;

	if ( !list_rewind( relation_join_folder->delimited_list ) )
		return NULL;

	*ptr = '\0';

	do {
		delimited_string =
			list_get(
				relation_join_folder->
					delimited_list );


		tag =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			widget_drop_down_option_tag(
				delimited_string /* value_string */,
				string_initial_capital(
					buffer,
					delimited_string )
						/* display_string */ );

		if ( strlen( html ) +
		     strlen( tag ) +
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

		if ( ptr != html ) ptr += sprintf( ptr, "\n" );

		ptr += sprintf( ptr, "%s", tag );

	} while ( list_next( 
			relation_join_folder->
				delimited_list ) );

	return strdup( html );
}

char *widget_upload_source_filespecification(
		char *application_name,
		char *upload_directory,
		char *widget_container_value )
{
	static char filespecification[ 128 ];

	if ( !application_name
	||   !upload_directory
	||   !widget_container_value )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		filespecification,
		sizeof ( filespecification ),
		"%s/%s/%s",
		upload_directory,
		application_name,
		widget_container_value );

	return filespecification;
}

char *widget_upload_filename_basename( char *widget_container_value )
{
	static char basename[ 128 ];

	if ( !widget_container_value )
	{
		char message[ 128 ];

		sprintf(message, "widget_container_value is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* ------------------------------------- */
	/* Returns null if not enough delimiters */
	/* ------------------------------------- */
	piece( basename, '.', widget_container_value, 0 );
}

char *widget_upload_filename_extension( char *widget_container_value )
{
	static char extension[ 128 ];

	if ( !widget_container_value )
	{
		char message[ 128 ];

		sprintf(message, "widget_container_value is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* ------------------------------------- */
	/* Returns null if not enough delimiters */
	/* ------------------------------------- */
	piece( extension, '.', widget_container_value, 1 );
}

char *widget_date_login_name_now_date_string(
		int attribute_width_max_length,
		char *application_name,
		char *login_name )
{
	char *date_now_string = {0};
	char *portion_date;
	char *portion_time;
	char *source_international;
	static enum date_convert_format_enum
		login_name_enum =
			date_convert_unknown;

	if ( attribute_width_max_length == 10 )
	{
		date_now_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			date_now_yyyy_mm_dd(
				date_utc_offset() );
	}
	else
	if ( attribute_width_max_length == 16 )
	{
		date_now_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			date_now16(
				date_utc_offset() );
	}
	else
	if ( attribute_width_max_length == 19 )
	{
		date_now_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			date_now19(
				date_utc_offset() );
	}
	else
	{
		char message[ 128 ];

		sprintf(message,
			"invalid attribute_width_max_length=%d.",
			attribute_width_max_length );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( login_name_enum == date_convert_unknown )
	{
		login_name_enum =
			date_convert_login_name_enum(
				application_name,
				login_name );
	}

	if ( login_name_enum == date_convert_international )
	{
		return date_now_string;
	}

	portion_date =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_date_portion_date(
			date_now_string );

	portion_time =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_date_portion_time(
			date_now_string );

	source_international =
		/* ----------------------------------------------- */
		/* Returns heap memory, source_date_string or null */
		/* ----------------------------------------------- */
		date_convert_source_international(
			login_name_enum,
			portion_date
				/* source_date_string */ );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	widget_date_combine_date_string(
		source_international,
		portion_time );
}

char *widget_date_portion_date( char *date_now_string )
{
	char portion_date[ 128 ];

	column( portion_date, 0, date_now_string );
	return strdup( portion_date );
}

char *widget_date_portion_time( char *date_now_string )
{
	char portion_time[ 128 ];

	column( portion_time, 1, date_now_string );
	return strdup( portion_time );
}

char *widget_date_combine_date_string(
		char *date_convert_source_international,
		char *widget_date_portion_time )
{
	char combine_date_string[ 128 ];

	sprintf(combine_date_string,
		"%s %s",
		date_convert_source_international,
		widget_date_portion_time );

	return strdup( combine_date_string );
}

WIDGET_AJAX *widget_ajax_new(
		LIST *widget_container_list )
{
	WIDGET_AJAX *widget_ajax;

	if ( !list_length( widget_container_list ) )
	{
		char message[ 128 ];

		sprintf(message, "widget_container_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	widget_ajax = widget_ajax_calloc();

	widget_ajax->
		ajax_client_widget_container_list =
			widget_container_list;

	return widget_ajax;
}

WIDGET_AJAX *widget_ajax_calloc( void )
{
	WIDGET_AJAX *widget_ajax;

	if ( ! ( widget_ajax = calloc( 1, sizeof ( WIDGET_AJAX ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return widget_ajax;
}

char *widget_ajax_html(
		LIST *widget_container_list,
		LIST *query_row_cell_list,
		int row_number,
		char *background_color )
{
	char html[ 1024 ];
	char *ptr = html;
	WIDGET_CONTAINER *widget_container;
	char *container_html;

	if ( !list_rewind( widget_container_list ) )
	{
		char message[ 128 ];

		sprintf(message, "widget_container_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	*ptr = '\0';

	do {
		widget_container = list_get( widget_container_list );

		container_html =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			widget_container_html(
				(char *)0 /* state */,
				row_number,
				query_row_cell_list,
				(RELATION_JOIN_ROW *)0 /* relation_join_row */,
				background_color,
				widget_container );

		if ( strlen( html ) +
		     strlen( container_html ) +
		     1 >= 1024 )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				1024 );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"%s\n",
			container_html );

		free( container_html );

	} while ( list_next( widget_container_list ) );

	return strdup( html );
}

WIDGET_CONTAINER *widget_container_ajax_new(
		char *widget_name,
		LIST *widget_container_list )
{
	WIDGET_CONTAINER *widget_container;

	if ( !widget_name
	||   !list_length( widget_container_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_container = widget_container_calloc();

	widget_container->widget_name = widget_name;
	widget_container->widget_type = widget_ajax;

	widget_container->ajax =
		widget_ajax_new(
			widget_container_list );

	return widget_container;
}

char *widget_password_value_string(
		LIST *query_row_cell_list,
		char *attribute_name )
{
	char *value_string = (char *)0;

	if ( !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_length( query_row_cell_list ) )
	{
		QUERY_CELL *query_cell;

		if ( ! ( query_cell =
				query_cell_seek(
					attribute_name,
					query_row_cell_list ) ) )
		{
			char message[ 65536 ];

			sprintf(message,
				"query_cell_seek(%s) returned empty.\n"
				"query_cell_list=[%s]",
				attribute_name,
				query_cell_list_display(
					query_row_cell_list ) );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		value_string = query_cell->select_datum;

		if ( *value_string )
			value_string =
				ROW_SECURITY_ASTERISK;
	}

	return value_string;
}

WIDGET_PASSWORD *widget_password_new(
		char *widget_password_display_name )
{
	WIDGET_PASSWORD *widget_password;

	if ( !widget_password_display_name )
	{
		char message[ 128 ];

		sprintf(message, "widget_password_display_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_password = widget_password_calloc();

	widget_password->widget_password_display_name =
		widget_password_display_name;

	widget_password->text =
		widget_text_new(
			widget_password_display_name
			/* widget_name */ );

	return widget_password;
}

WIDGET_PASSWORD *widget_password_calloc( void )
{
	WIDGET_PASSWORD *widget_password;

	if ( ! ( widget_password = calloc( 1, sizeof ( WIDGET_PASSWORD ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return widget_password;
}

char *widget_password_display_name( char *attribute_name )
{
	char display_name[ 128 ];

	if ( !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		/* Stub */
		/* ---- */
		exit( 1 );
	}

	snprintf(
		display_name,
		sizeof ( display_name ),
		"%s_display",
		attribute_name );

	return strdup( display_name );
}

char *widget_password_html(
		char *widget_password_display_name,
		char *attribute_name,
		WIDGET_TEXT *widget_text,
		char *state,
		int row_number,
		LIST *query_row_cell_list,
		char *background_color )
{
	char *value_string;

	if ( !widget_password_display_name
	||   !attribute_name
	||   !widget_text )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	value_string =
		/* ------------------------------ */
		/* Returns program memory or null */
		/* ------------------------------ */
		widget_password_value_string(
			query_row_cell_list,
			attribute_name );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	widget_text_html(
		widget_password_display_name /* widget_name */,
		widget_text->attribute_width_max_length,
		widget_text->widget_text_display_size,
		widget_text->null_to_slash,
		widget_text->prevent_carrot_boolean,
		widget_text->post_change_javascript,
		widget_text->tab_order,
		value_string,
		widget_text->viewonly,
		state,
		row_number,
		(LIST *)0 /* query_row_cell_list */,
		background_color );
}

boolean widget_yes_no_affirmative_boolean(
		char *widget_container_value )
{
	if ( !widget_container_value ) return 0;

	return
	( *widget_container_value == 'y' );
}

boolean widget_yes_no_not_affirmative_boolean(
		char *widget_container_value )
{
	if ( !widget_container_value ) return 0;

	return
	( *widget_container_value == 'n' );
}

char *widget_drop_down_id(
		char *many_folder_name,
		char *widget_name )
{
	static char widget_id[ 128 ];

	*widget_id = '\0';

	if ( many_folder_name
	&&   widget_name )
	{
		snprintf(
			widget_id,
			sizeof ( widget_id ),
			"%s_%s",
			many_folder_name,
			widget_name );
	}

	if ( *widget_id )
		return widget_id;
	else
		return NULL;
}

char *widget_drop_down_row_number_id(
		char *widget_drop_down_id,
		int row_number,
		char *widget_container_key )
{
	static char row_number_id[ 128 ];

	if ( !widget_drop_down_id ) return widget_container_key;

	snprintf(
		row_number_id,
		sizeof ( row_number_id ),
		"%s_%d",
		widget_drop_down_id,
		row_number );

	return row_number_id;
}

char *widget_text_prevent_carrot_javascript( boolean prevent_carrot_boolean )
{
	if ( prevent_carrot_boolean )
	{
		return
		" onKeyup=\"timlib_prevent_carrot(event,this)\"\n";
	}
	else
	{
		return "";
	}
}

