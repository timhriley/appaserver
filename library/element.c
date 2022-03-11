/* $APPASERVER_HOME/library/element.c					*/
/* -------------------------------------------------------------------- */
/* This is the appaserver APPASERVER_ELEMENT ADT.			*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "String.h"
#include "timlib.h"
#include "session.h"
#include "attribute.h"
#include "form.h"
#include "relation.h"
#include "folder.h"
#include "date.h"
#include "list.h"
#include "appaserver_library.h"
#include "application.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "piece.h"
#include "basename.h"
#include "environ.h"
#include "javascript.h"
#include "appaserver.h"
#include "element.h"

APPASERVER_ELEMENT *appaserver_element_calloc( void )
{
	APPASERVER_ELEMENT *element;

	if ( ! ( element = calloc( 1, sizeof( APPASERVER_ELEMENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

	}
	return element;
}

APPASERVER_ELEMENT *appaserver_element_new(
			enum element_type element_type,
			char *element_name )
{
	APPASERVER_ELEMENT *element = appaserver_element_calloc();

	element->element_type = element_type;
	element->element_name = element_name;

	if ( element_type == table_open )
		element->table_open =
			element_table_open_calloc();
	else
	if ( element_type == table_heading )
		element->table_heading =
			element_table_heading_calloc();
	else
	if ( element_type == table_row )
		element->table_row =
			element_table_row_calloc();
	else
	if ( element_type == table_close )
		element->table_close =
			element_table_close_calloc();
	else
	if ( element_type == checkbox )
		element->checkbox =
			element_checkbox_calloc();
	else
	if ( element_type == prompt_drop_down )
		element->prompt_drop_down =
			element_prompt_drop_down_calloc();
	else
	if ( element_type == drop_down )
		element->drop_down =
			element_drop_down_calloc();
	else
	if ( element_type == button )
		element->button =
			element_button_calloc();
	else
	if ( element_type == text )
		element->text =
			element_text_calloc();
	else
	if ( element_type == non_edit_text )
		element->non_edit_text =
			element_non_edit_text_calloc();
	else
	if ( element_type == yes_no )
		element->yes_no =
			element_yes_no_calloc();
	else
	if ( element_type == hidden )
		element->hidden =
			element_hidden_calloc();
	else
	if ( element_type == line_break )
		element->line_break =
			element_line_break_calloc();
	else
	if ( element_type == table_data )
		element->table_data =
			element_table_data_calloc();
	else
	if ( element_type == multi_drop_down )
		element->multi_drop_down =
			element_multi_drop_down_calloc();
	else
	{
		fprintf( stderr, 
			 "Error: %s/%s() got invalid element type.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	return element;
}

char *element_drop_down_html(
			char *drop_down_name,
			char *value,
			LIST *delimited_list,
			LIST *display_list,
			boolean output_null_option,
			boolean output_not_null_option,
			boolean output_select_option,
			int display_size,
			boolean multi_select,
			int tab_order,
			char *javascript_replace,
			char *background_color )
{
	char html[ STRING_TWO_MEG ];
	char *ptr = html;
	char buffer[ 256 ];
	char *value_list_html;

	if ( !drop_down_name || !*drop_down_name )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: drop_down_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"<select name=\"%s\" id=\"%s\"",
		drop_down_name,
		drop_down_name );

	if ( display_size )
	{
		ptr += sprintf( ptr, " size=%d", display_size );
	}

	if ( multi_select ) ptr += sprintf( ptr, " multiple" );

	if ( tab_order > 0 )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_element_tab_order_html(
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
			appaserver_element_background_color_html(
				background_color ) );
	}

	if ( javascript_replace && *javascript_replace )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_element_javascript_html(
				javascript_replace ) );
	}

	ptr += sprintf( ptr, ">\n" );

	if ( value && *value )
	{
		char *first_option_value_html;

		first_option_value_html =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			element_drop_down_option_value_html(
				value,
				list_offset_seek(
					display_list,
					list_seek_offset(
						value,
						delimited_list ) )
							/* value_display */ );

		ptr += sprintf(
			ptr,
			"%s\n",
			first_option_value_html );

		output_select_option = 0;
	}

	if ( output_select_option )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			element_drop_down_option_value_html(
				ELEMENT_SELECT_OPERATOR,
				string_initial_capital(
					buffer,
					ELEMENT_SELECT_OPERATOR ) ) );

		output_select_option = 0;
	}

	value_list_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		element_drop_down_option_value_list_html(
			delimited_list,
			display_list,
			value );

	ptr += sprintf( ptr, "%s\n", value_list_html );

	free( value_list_html );

	if ( output_select_option )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			element_drop_down_option_value_html(
				ELEMENT_SELECT_OPERATOR,
				string_initial_capital(
					buffer,
					ELEMENT_SELECT_OPERATOR ) ) );
	}

	ptr += sprintf(
		ptr,
		"%s\n",
		/* ---------------------- */
		/* Returns static memory. */
		/* ---------------------- */
		element_drop_down_close_html(
			output_null_option,
			output_not_null_option ) );

	return strdup( html );
}

ELEMENT_TABLE_OPEN *element_table_open_calloc( void )
{
	ELEMENT_TABLE_OPEN *table_open;

	if ( ! ( table_open = calloc( 1, sizeof( ELEMENT_TABLE_OPEN ) ) ) )
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

char *element_table_open_html( void )
{
	return "<table border=0>";
}

ELEMENT_TABLE_HEADING *element_table_heading_calloc( void )
{
	ELEMENT_TABLE_HEADING *table_heading;

	if ( ! ( table_heading =
			calloc( 1, sizeof( ELEMENT_TABLE_HEADING ) ) ) )
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

ELEMENT_TABLE_HEADING *element_table_heading_new( char *heading_string )
{
	ELEMENT_TABLE_HEADING *table_heading = element_table_heading_calloc();

	table_heading->heading_string = heading_string;

	return table_heading;
}

char *element_table_heading_html( char *heading_string )
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

ELEMENT_TABLE_ROW *element_table_row_calloc( void )
{
	ELEMENT_TABLE_ROW *table_row;

	if ( ! ( table_row = calloc( 1, sizeof( ELEMENT_TABLE_ROW ) ) ) )
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

char *element_table_row_html( char *background_color )
{
	static char html[ 128 ];

	if ( !background_color )
	{
		strcpy( html, "<tr>" );
	}
	else
	{
		sprintf(html,
			"<tr bgcolor=%s",
			background_color );
	}

	return html;
}

ELEMENT_TABLE_CLOSE *element_table_close_calloc( void )
{
	ELEMENT_TABLE_CLOSE *table_close;

	if ( ! ( table_close = calloc( 1, sizeof( ELEMENT_TABLE_CLOSE ) ) ) )
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

char *element_table_close_html( void )
{
	return "</table>";
}

#ifdef NOT_DEFINED
APPASERVER_ELEMENT *element_yes_no(
			char *element_name,
			char *post_change_javascript,
			boolean with_is_null,
			boolean with_not_null )
{
	APPASERVER_ELEMENT *element;
	LIST *delimited_list = list_new();

	list_set(
		delimited_list,
		"yes" );

	list_set(
		delimited_list,
		"no" );

	if ( with_is_null )
	{
		list_set(
			delimited_list,
			ELEMENT_NULL_OPERATOR );
	}

	if ( with_not_null )
	{
		list_set(
			delimited_list,
			ELEMENT_NOT_NULL_OPERATOR );
	}

	element =
		appaserver_element_new(
			drop_down );

	element->drop_down->many_folder_name = element_name;

	if ( post_change_javascript && *post_change_javascript )
	{
		element->drop_down->post_change_javascript =
			post_change_javascript;
	}

	return element;
}
#endif

char *appaserver_element_javascript(
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
		post_change_javascript,
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

boolean element_exists_reference_number(
			LIST *element_list )
{
	APPASERVER_ELEMENT *element;

	if ( !list_rewind( element_list ) ) return 0;

	do {
		element = list_get( element_list );

		if ( element->element_type == reference_number ) return 1;

	} while( list_next( element_list ) );

	return 0;
}

boolean element_exists_upload_filename(
			LIST *element_list )
{
	APPASERVER_ELEMENT *element;

	if ( !list_rewind( element_list ) ) return 0;

	do {
		element = list_get( element_list );

		if ( element->element_type == upload ) return 1;

	} while( list_next( element_list ) );

	return 0;
}

char *element_date_format_string(
			enum date_convert_format date_convert_format )
{
	static char format_string[ 64 ];

	if ( date_convert_format == international )
	{
		sprintf( format_string,
			 "%cY-%cm-%cd",
		 	 '%','%','%' );
	}
	else
	if ( date_convert_format == american )
	{
		sprintf( format_string,
			 "%cm/%cd/%cY",
		 	 '%','%','%' );
	}
	else
	if ( date_convert_format == military )
	{
		sprintf( format_string,
			 "%cd-%cb-%cY",
		 	 '%','%','%' );
	}
	else
	{
		sprintf( format_string,
			 "%cY-%cm-%cd",
		 	 '%','%','%' );
	}

	return format_string;
}

char *appaserver_element_number_commas_string(
			char *attribute_name,
			char *value )
{
	if ( string_exists_substring( attribute_name, "year" ) )
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

#ifdef NOT_DEFINED
APPASERVER_ELEMENT *element_sort_order(
			FOLDER_ATTRIBUTE *folder_attribute )
{
	APPASERVER_ELEMENT *element;

	element =
		appaserver_element_new(
			text_item );

	element->text_item->attribute_width =
		(folder_attribute->primary_key_index)
			? 15 : 10;

	if ( folder_attribute->primary_key_index )
	{
		char heading[ 128 ];

		sprintf( heading, "*%s", folder_attribute->attribute_name );
		element->text_item->heading = strdup( heading );
	}
	else
	{
		element->text_item->heading = folder_attribute->attribute_name;
	}

	element->text_item->readonly = 1;
	return element;
}
#endif

ELEMENT_NON_EDIT_TEXT *element_non_edit_text_calloc( void )
{
	ELEMENT_NON_EDIT_TEXT *non_edit_text;

	if ( ! ( non_edit_text =
			calloc( 1, sizeof( ELEMENT_NON_EDIT_TEXT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return non_edit_text;
}

ELEMENT_NON_EDIT_TEXT *element_non_edit_text_new(
			char *attribute_name,
			char *message )
{
	ELEMENT_NON_EDIT_TEXT *element_non_edit_text =
		element_non_edit_text_calloc();

	element_non_edit_text->attribute_name = attribute_name;
	element_non_edit_text->message = message;

	return element_non_edit_text;
}

char *element_non_edit_text_initial_capital(
			char *value,
			char *message )
{
	char initial_capital[ 2048 ];

	if ( !value && !message ) return strdup( "" );

	if ( value )
	{
		return strdup(
			string_initial_capital(
				initial_capital,
				value ) );
	}
	else
	{
		return strdup(
			string_initial_capital(
				initial_capital,
				message ) );
	}
}

char *element_drop_down_name(
			LIST *attribute_name_list,
			int row_number )
{
	static char drop_down_name[ 1024 ];
	char *results;

	if ( !list_length( attribute_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	results =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_display_delimited(
			attribute_name_list,
			SQL_DELIMITER );

	sprintf(drop_down_name,
		"%s_%d",
		results,
		row_number );

	free( results );
	return drop_down_name;
}

int appaserver_element_tab_order( int tab_order )
{
	if ( tab_order == -1 ) return -1;

	return tab_order + 1;
}

int element_drop_down_display_size(
			int delimited_list_length )
{
	if ( delimited_list_length == 0 || delimited_list_length > 5 )
		return 1;
	else
		return delimited_list_length;
}

LIST *element_drop_down_display_list(
			LIST *delimited_list,
			boolean no_initial_capital )
{
	LIST *display_list;
	char *delimited_string;
	char display_buffer[ 1024 ];
	char data_buffer[ 1024 ];
	char buffer[ 1024 ];
	char delimiter_string[ 2 ];

	if ( no_initial_capital ) return list_copy( delimited_list );

	if ( !list_rewind( delimited_list ) ) return (LIST *)0;

	*delimiter_string = SQL_DELIMITER;
	*(delimiter_string + 1) = '\0';

	display_list = list_new();

	do {
		delimited_string = list_get( delimited_list );

		if ( string_character_exists(
			delimited_string,
			'|' ) )
		{
			piece( data_buffer, '|', delimited_string, 0 );
			piece( display_buffer, '|', delimited_string, 1 );

			free( delimited_string );

			list_set_current(
				delimited_list,
				strdup( data_buffer ) );

			list_set(
				display_list,
				strdup(
					string_search_replace(
					   string_initial_capital(
						buffer,
						display_buffer ),
					   delimiter_string,
					   ELEMENT_LONG_DASH_DELIMITER ) ) );
		}
		else
		if ( string_character_exists(
			delimited_string,
			'[' ) )
		{
			piece( data_buffer, '[', delimited_string, 0 );

			free( delimited_string );

			list_set_current(
				delimited_list,
				strdup( data_buffer ) );

			list_set(
				display_list,
				strdup(
					string_search_replace(
					   string_initial_capital(
						buffer,
						delimited_string ),
					   delimiter_string,
					   ELEMENT_LONG_DASH_DELIMITER ) ) );
		}
		else
		{
			list_set(
				display_list,
				strdup(
					string_search_replace(
					   string_initial_capital(
						buffer,
						data_buffer ),
					   delimiter_string,
					   ELEMENT_LONG_DASH_DELIMITER ) ) );
		}
	} while ( list_next( delimited_list ) );

	return display_list;
}

char *element_drop_down_option_value_html(
			char *value,
			char *display )
{
	static char html[ 256 ];

	if ( !value || !display )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( strcmp( value, display ) != 0 )
	{
		sprintf(html,
			"\t<option value=\"%s\">%s",
			value,
			display );
	}
	else
	{
		sprintf(html,
			"\t<option>%s",
			value );
	}

	return html;
}

char *element_drop_down_close_html(
			boolean output_null_option,
			boolean output_not_null_option )
{
	static char html[ 1024 ];
	char *ptr = html;
	char buffer[ 128 ];

	if ( output_null_option )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			element_drop_down_option_value_html(
				ELEMENT_NULL_OPERATOR,
				string_initial_capital(
					buffer,
					ELEMENT_NULL_OPERATOR ) ) );
	}

	if ( output_not_null_option )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			element_drop_down_option_value_html(
				ELEMENT_NOT_NULL_OPERATOR,
				string_initial_capital(
					buffer,
					ELEMENT_NOT_NULL_OPERATOR ) ) );
	}

	ptr += sprintf( ptr, "</select>" );

	return html;
}

char *element_checkbox_html(
			char *element_name,
			char *prompt_display,
			boolean checked,
			char *on_click,
			int tab_order,
			char *background_color,
			char *image_source )
{
	char html[ 1024 ];
	char *ptr = html;

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( image_source && *image_source )
	{
		ptr += sprintf(
			ptr,
			"<img src=\"%s\">",
			image_source );
	}

	if ( prompt_display && *prompt_display )
	{
		ptr += sprintf( ptr, "%s", prompt_display );
	}

	ptr += sprintf(
		ptr,
		"<input name=\"%s\" type=checkbox value=y",
		element_name );

	if ( on_click && *on_click )
	{
		ptr += sprintf(
			ptr,
			" onClick=\"%s\"",
			on_click );
	}

	if ( tab_order > 0 )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_element_tab_order_html(
				tab_order ) );
	}

	if ( checked )
	{
		ptr += sprintf(
			ptr,
			" checked" );
	}

	if ( background_color && *background_color )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_element_background_color_html(
				background_color ) );
	}

	ptr += sprintf(
		ptr,
		">" );

	return strdup( html );
}

int element_multi_display_size( void )
{
	return 10;
}

ELEMENT_MULTI_DROP_DOWN *element_multi_drop_down_calloc( void )
{
	ELEMENT_MULTI_DROP_DOWN *element_multi_drop_down;

	if ( ! ( element_multi_drop_down =
			calloc( 1, sizeof( ELEMENT_MULTI_DROP_DOWN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return element_multi_drop_down;
}

ELEMENT_PROMPT_DROP_DOWN *element_prompt_drop_down_new(
			char *element_name,
			LIST *delimited_list,
			boolean no_initial_capital,
			boolean output_null_option,
			boolean output_not_null_option,
			boolean output_select_option,
			int display_size,
			int tab_order,
			boolean multi_select,
			char *post_change_javascript,
			boolean recall )
{
	ELEMENT_PROMPT_DROP_DOWN *element_prompt_drop_down =
		element_prompt_drop_down_calloc();

	if ( !element_name || !*element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	element_prompt_drop_down->element_name = element_name;
	element_prompt_drop_down->delimited_list = delimited_list;
	element_prompt_drop_down->no_initial_capital = no_initial_capital;
	element_prompt_drop_down->output_null_option = output_null_option;

	element_prompt_drop_down->output_not_null_option =
		output_not_null_option;

	element_prompt_drop_down->output_select_option = output_select_option;
	element_prompt_drop_down->display_size = display_size;
	element_prompt_drop_down->tab_order = tab_order;
	element_prompt_drop_down->multi_select = multi_select;

	element_prompt_drop_down->post_change_javascript =
		post_change_javascript;

	element_prompt_drop_down->recall = recall;

	return element_prompt_drop_down;
}

ELEMENT_PROMPT_DROP_DOWN *element_empty_prompt_drop_down_new(
			char *element_name,
			int display_size,
			boolean multi_select,
			boolean recall )
{
	ELEMENT_PROMPT_DROP_DOWN *element_prompt_drop_down =
		element_prompt_drop_down_calloc();

	if ( !element_name || !*element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	element_prompt_drop_down->element_name = element_name;
	element_prompt_drop_down->display_size = display_size;
	element_prompt_drop_down->multi_select = multi_select;
	element_prompt_drop_down->recall = recall;

	return element_prompt_drop_down;
}

char *element_prompt_drop_down_html(
			char *element_name,
			LIST *delimited_list,
			LIST *display_list,
			boolean output_null_option,
			boolean output_not_null_option,
			boolean output_select_option,
			int display_size,
			boolean multi_select,
			int tab_order,
			char *javascript_replace )
{
	char html[ STRING_64K ];
	char *ptr = html;
	char buffer[ 256 ];
	char *value_list_html;

	if ( !element_name || !*element_name )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"<select name=\"%s\" id=\"%s\"",
		element_name,
		element_name );

	if ( display_size )
	{
		ptr += sprintf( ptr, " size=%d", display_size );
	}

	if ( multi_select ) ptr += sprintf( ptr, " multiple" );

	if ( tab_order > 0 )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_element_tab_order_html(
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
			appaserver_element_javascript_html(
				javascript_replace ) );
	}

	ptr += sprintf( ptr, ">\n" );

	if ( output_select_option )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			element_drop_down_option_value_html(
				ELEMENT_SELECT_OPERATOR,
				string_initial_capital(
					buffer,
					ELEMENT_SELECT_OPERATOR ) ) );
	}

	value_list_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		element_drop_down_option_value_list_html(
			delimited_list,
			display_list,
			(char *)0 /* value */ );

	ptr += sprintf( ptr, "%s\n", value_list_html );

	free( value_list_html );

	ptr += sprintf(
		ptr,
		"%s\n",
		/* ---------------------- */
		/* Returns static memory. */
		/* ---------------------- */
		element_drop_down_close_html(
			output_null_option,
			output_not_null_option ) );

	return strdup( html );
}

char *element_empty_prompt_drop_down_html(
			char *element_name,
			int display_size,
			boolean multi_select )
{
	char html[ 1024 ];
	char *ptr = html;

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"<td><select name=\"%s\" size=%d",
		element_name,
		(display_size) ? display_size : 1 );

	if ( multi_select ) ptr += sprintf( ptr, " multiple" );

	ptr += sprintf( ptr, ">\n" );

	return strdup( html );
}

ELEMENT_PROMPT_DROP_DOWN *element_prompt_drop_down_calloc( void )
{
	ELEMENT_PROMPT_DROP_DOWN *element_prompt_drop_down;

	if ( ! ( element_prompt_drop_down =
			calloc( 1, sizeof( ELEMENT_PROMPT_DROP_DOWN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return element_prompt_drop_down;
}

ELEMENT_DROP_DOWN *element_drop_down_new(
			char *name,
			LIST *attribute_name_list,
			LIST *delimited_list,
			LIST *display_list,
			boolean no_initial_capital,
			boolean output_null_option,
			boolean output_not_null_option,
			boolean output_select_option,
			int display_size,
			int tab_order,
			boolean multi_select,
			char *post_change_javascript,
			boolean readonly,
			boolean recall )
{
	ELEMENT_DROP_DOWN *element_drop_down = element_drop_down_calloc();

	element_drop_down->name = name;
	element_drop_down->attribute_name_list = attribute_name_list;
	element_drop_down->delimited_list = delimited_list;
	element_drop_down->display_list = display_list;
	element_drop_down->no_initial_capital = no_initial_capital;
	element_drop_down->output_null_option = output_null_option;
	element_drop_down->output_not_null_option = output_not_null_option;
	element_drop_down->output_select_option = output_select_option;
	element_drop_down->display_size = display_size;
	element_drop_down->tab_order = tab_order;
	element_drop_down->multi_select = multi_select;
	element_drop_down->post_change_javascript = post_change_javascript;
	element_drop_down->readonly = readonly;
	element_drop_down->recall = recall;

	return element_drop_down;
}
 
char *element_multi_drop_down_name(
			LIST *attribute_name_list )
{
	static char drop_down_name[ 1024 ];
	char *results;

	if ( !list_length( attribute_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: list_length() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	results =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_display_delimited(
			attribute_name_list,
			SQL_DELIMITER );

	strcpy(	drop_down_name, results );

	free( results );
	return drop_down_name;
}

char *element_multi_drop_down_original_name(
			LIST *attribute_name_list,
			char *element_name_prefix )
{
	static char drop_down_name[ 256 ];

	if ( !list_length( attribute_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !element_name_prefix )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name_prefix is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(drop_down_name,
		"%s%s",
		element_name_prefix,	
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_display_delimited(
			attribute_name_list,
			SQL_DELIMITER ) );

	return drop_down_name;
}

ELEMENT_MULTI_DROP_DOWN *element_multi_drop_down_new(
			char *original_name,
			char *element_name,
			LIST *delimited_list,
			boolean no_initial_capital )
{
	ELEMENT_MULTI_DROP_DOWN *element_multi_drop_down;

	if ( !original_name
	||   !*original_name
	||   !element_name
	||   !*element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	element_multi_drop_down = element_multi_drop_down_calloc();

	element_multi_drop_down->original_drop_down =
		element_prompt_drop_down_new(
			original_name,
			delimited_list,
			no_initial_capital,
			0 /* not output_null_option */,
			0 /* not output_not_null_option */,
			0 /* not output_select_option */,
			element_multi_display_size(),
			0 /* tab_order */,
			1 /* multi_select */,
			(char *)0 /* post_change_javascript */,
			1 /* recall */ );

	element_multi_drop_down->table_data =
		element_table_data_calloc();

	element_multi_drop_down->move_right_button =
		element_button_new(
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			element_multi_drop_down_move_right_label(),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			element_multi_drop_down_move_right_action_string(
				original_name,
				element_name,
				ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER ) );

	element_multi_drop_down->element_line_break =
		element_line_break_calloc();

	element_multi_drop_down->move_left_button =
		element_button_new(
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			element_multi_drop_down_move_left_label(),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			element_multi_drop_down_move_left_action_string(
				original_name,
				element_name,
				ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER ) );

	element_multi_drop_down->empty_drop_down =
		element_empty_prompt_drop_down_new(
			element_name,
			element_multi_display_size(),
			1 /* multi_select */,
			1 /* recall */ );

	return element_multi_drop_down;
}

ELEMENT_BUTTON *element_button_calloc( void )
{
	ELEMENT_BUTTON *element_button;

	if ( ! ( element_button = calloc( 1, sizeof( ELEMENT_BUTTON ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return element_button;
}

ELEMENT_BUTTON *element_button_new(
			char *label,
			char *action_string )
{
	ELEMENT_BUTTON *element_button = element_button_calloc();

	element_button->button =
		/* --------- */
		/* Sets html */
		/* --------- */
		button_new(
			label,
			action_string );

	return element_button;
}

char *element_button_html( char *button_html )
{
	if ( !button_html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: button_html is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return button_html;
}

char *element_multi_drop_down_move_right_label( void )
{
	return "Add ->";
}

char *element_multi_drop_down_move_left_label( void )
{
	return "<- Remove";
}

char *element_multi_drop_down_move_right_action_string(
			char *original_name,
			char *element_name,
			char element_multi_move_left_right_delimiter )
{
	char action_string[ 1024 ];

	if ( !original_name
	||   !*original_name
	||   !element_name
	||   !*element_name )
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
		element_name,
		element_multi_move_left_right_delimiter );

	return strdup( action_string );
}

char *element_multi_drop_down_move_left_action_string(
			char *original_name,
			char *element_name,
			char element_multi_move_left_right_delimiter )
{
	char action_string[ 1024 ];

	if ( !original_name
	||   !*original_name
	||   !element_name
	||   !*element_name )
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
		element_name,
		element_multi_move_left_right_delimiter );

	return strdup( action_string );
}

ELEMENT_LINE_BREAK *element_line_break_calloc( void )
{
	ELEMENT_LINE_BREAK *element_line_break;

	if ( ! ( element_line_break =
			calloc( 1, sizeof( ELEMENT_LINE_BREAK ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return element_line_break;
}

char *element_line_break_html( void )
{
	return "<br>";
}

char *element_table_data_html(
			boolean align_right,
			int column_span )
{
	char html[ 128 ];
	char *ptr = html;

	ptr += sprintf( ptr, "<td" );

	if ( align_right )
	{
		ptr += sprintf( ptr, " align=right" );
	}

	if ( column_span )
	{
		ptr += sprintf( ptr, " colspan=%d", column_span );
	}

	ptr += sprintf( ptr, ">" );

	return strdup( html );
}

char *element_list_html( LIST *appaserver_element_list )
{
	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	appaserver_element_list_html(
		appaserver_element_list,
		(char *)0 /* application_name */,
		(char *)0 /* background_color */,
		(char *)0 /* state */,
		0 /* row_number */,
		(DICTIONARY *)0 /* row_dictionary */ );
}

char *appaserver_element_list_html(
			LIST *appaserver_element_list /* in/out */,
			char *application_name,
			char *background_color,
			char *state,
			int row_number,
			DICTIONARY *row_dictionary )
{
	APPASERVER_ELEMENT *appaserver_element;
	char html[ STRING_FOUR_MEG ];
	char *ptr = html;
	char *element_html;

	if ( !list_rewind( appaserver_element_list ) )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: appaserver_element_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (char *)0;
	}

	do {
		appaserver_element = list_get( appaserver_element_list );

		if ( appaserver_element->hidden ) continue;

		if ( ! ( element_html =
				/* --------------------------- */
				/* Returns heap memory or null */
				/* --------------------------- */
				appaserver_element_html(
					appaserver_element /* in/out */,
					application_name,
					background_color,
					state,
					row_number,
					row_dictionary ) ) )
		{
			fprintf(stderr,
	"Warning in %s/%s()/%d: appaserver_element_html(%d) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				appaserver_element->element_type );

			continue;
		}

		ptr += sprintf(
			ptr,
			"%s", 
			element_html );

		if ( appaserver_element->element_type != table_data )
		{
			ptr += sprintf( ptr, "\n" );
		}

		free( element_html );

	} while ( list_next( appaserver_element_list ) );

	return strdup( html );
}

char *appaserver_element_hidden_list_html(
			LIST *appaserver_element_list /* in/out */,
			int row_number,
			DICTIONARY *row_dictionary )
{
	APPASERVER_ELEMENT *appaserver_element;
	char html[ STRING_64K ];
	char *ptr = html;
	char *element_html;

	if ( !list_rewind( appaserver_element_list ) )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: appaserver_element_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (char *)0;
	}

	do {
		appaserver_element = list_get( appaserver_element_list );

		if ( !appaserver_element->hidden ) continue;

		if ( ! ( element_html =
				/* --------------------------- */
				/* Returns heap memory or null */
				/* --------------------------- */
				appaserver_element_html(
					appaserver_element /* in/out */,
					(char *)0 /* application_name */,
					(char *)0 /* background_color */,
					(char *)0 /* state */,
					row_number,
					row_dictionary ) ) )
		{
			fprintf(stderr,
	"Warning in %s/%s()/%d: appaserver_element_html(%d) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				appaserver_element->element_type );
			return (char *)0;
		}

		ptr += sprintf(
			ptr,
			"%s\n", 
			element_html );

		free( element_html );

	} while ( list_next( appaserver_element_list ) );

	return strdup( html );
}

#ifdef NOT_DEFINED
char *appaserver_element_button_set_all_control_string(
			APPASERVER_ELEMENT *element,
			int form_number )
{
	static char button_control_string[ 256 ];

	if ( element->element_type == button )
	{
		if ( toggle_button_set_all_control_string )
		{
			char delete_warning_javascript[ 128 ];

			if ( timlib_begins_string( element->name, "delete" ) )
			{
				strcpy( delete_warning_javascript,
					"timlib_delete_button_warning(); " );
			}
			else
			{
				*delete_warning_javascript = '\0';
			}

			sprintf(button_control_string,
				"%sform_push_button_set_all('%s',%d);",
				delete_warning_javascript,
				element->button->label,
				form_number );

			return button_control_string;
		}
	}
	return (char *)0;
}
#endif

LIST *appaserver_element_heading_name_list(
			LIST *appaserver_element_list )
{
	LIST *heading_name_list;
	char *heading_name;
	APPASERVER_ELEMENT *appaserver_element;

	if ( !list_rewind( appaserver_element_list ) ) return (LIST *)0;

	heading_name_list = list_new();

	do {
		appaserver_element =
			list_get(
				appaserver_element_list );

		if ( ( heading_name =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			appaserver_element_heading_name(
				appaserver_element->element_name ) ) )
		{
			list_set( heading_name_list, heading_name );
		}
	} while ( list_next( appaserver_element_list ) );

	return heading_name_list;
}

char *appaserver_element_heading_name( char *element_name )
{
	char heading_name[ 128 ];
	char trimmed_name[ 128 ];

	if ( !element_name ) return (char *)0;

	trim_index( trimmed_name, element_name );
	string_initial_capital( heading_name, trimmed_name );

	return strdup( heading_name );
}

char *appaserver_element_html(
			APPASERVER_ELEMENT *appaserver_element /* in/out */,
			char *application_name,
			char *background_color,
			char *state,
			int row_number,
			DICTIONARY *row_dictionary )
{
	if ( !appaserver_element )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: appaserver_element is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (char *)0;
	}

	if ( appaserver_element->element_type == table_open )
	{
		return
		strdup(
			/* Returns program memory */
			/* ---------------------- */
			element_table_open_html() );
	}
	else
	if ( appaserver_element->element_type == table_heading )
	{
		if ( !appaserver_element->table_heading )
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
		element_table_heading_html(
			appaserver_element->
				table_heading->
				heading_string );
	}
	else
	if ( appaserver_element->element_type == table_row )
	{
		return
		strdup(
			/* Returns static memory */
			/* --------------------- */
			element_table_row_html( background_color ) );
	}
	else
	if ( appaserver_element->element_type == table_close )
	{
		return strdup(
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			element_table_close_html() );
	}
	else
	if ( appaserver_element->element_type == checkbox )
	{
		if ( !appaserver_element->checkbox )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: checkbox is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		appaserver_element_checkbox_html(
			appaserver_element->checkbox,
			background_color,
			state,
			row_number,
			row_dictionary );
	}
	else
	if ( appaserver_element->element_type == drop_down )
	{
		if ( !appaserver_element->drop_down )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: drop_down is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		appaserver_element_drop_down_html(
			appaserver_element->drop_down,
			background_color,
			state,
			row_number,
			row_dictionary );
	}
	else
	if ( appaserver_element->element_type == prompt_drop_down )
	{
		if ( !appaserver_element->prompt_drop_down )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: prompt_drop_down is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_element_prompt_drop_down_html(
			appaserver_element->prompt_drop_down,
			state );
	}
	else
	if ( appaserver_element->element_type == button )
	{
		if ( !appaserver_element->button )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: button is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		/* Returns heap memory */
		/* ------------------- */
		return element_button_html(
			appaserver_element->button->button->html );
	}
	else
	if ( appaserver_element->element_type == non_edit_text )
	{
		if ( !appaserver_element->non_edit_text )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: non_edit_text is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		element_non_edit_text_initial_capital(
			appaserver_element_value(
				appaserver_element->
					non_edit_text->
					attribute_name,
				row_dictionary ),
			appaserver_element->
				non_edit_text->
				message );
	}
	else
	if ( appaserver_element->element_type == hidden )
	{
		if ( !appaserver_element->hidden )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: hidden is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		/* Returns heap memory or null */
		/* --------------------------- */
		return appaserver_element_hidden_html(
			appaserver_element->hidden,
			row_number,
			row_dictionary );
	}
	else
	if ( appaserver_element->element_type == notepad )
	{
		if ( !appaserver_element->notepad )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: notepad is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		/* Returns heap memory or null */
		/* --------------------------- */
		return appaserver_element_notepad_html(
			appaserver_element->notepad,
			background_color,
			state,
			row_number,
			row_dictionary );
	}
	else
	if ( appaserver_element->element_type == line_break )
	{
		return
		strdup(
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			element_line_break_html() );
	}
	else
	if ( appaserver_element->element_type == table_data )
	{
		if ( !appaserver_element->table_data )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: table_data is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		/* Returns heap memory */
		/* ------------------- */
		return element_table_data_html(
			appaserver_element->table_data->align_right,
			appaserver_element->table_data->column_span );
	}
	else
	if ( appaserver_element->element_type == multi_drop_down )
	{
		if ( !appaserver_element->multi_drop_down )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: multi_drop_down is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		/* Returns heap memory */
		/* ------------------- */
		return element_multi_drop_down_html(
			appaserver_element->
				multi_drop_down->
				original_drop_down,
			appaserver_element->
				multi_drop_down->
				table_data,
			appaserver_element->
				multi_drop_down->
				move_right_button,
			appaserver_element->
				multi_drop_down->
				element_line_break,
			appaserver_element->
				multi_drop_down->
				move_left_button,
			appaserver_element->
				multi_drop_down->
				empty_drop_down );
	}
	else
	if ( appaserver_element->element_type == text )
	{
		if ( !appaserver_element->text )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: text is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !appaserver_element->text->datatype_name )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: datatype_name is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		/* Returns heap memory or null */
		/* --------------------------- */
		return appaserver_element_text_html(
			appaserver_element->text,
			background_color,
			state,
			row_number,
			row_dictionary );
	}
	else
	if ( appaserver_element->element_type == password )
	{
		if ( !appaserver_element->password )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: password is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		element_password_html(
			appaserver_element_name(
				appaserver_element->
					password->
					attribute_name,
				row_number ),
			element_password_compare_element_name(
				appaserver_element->
					password->
					attribute_name,
				row_number ),
			appaserver_element_value(
				appaserver_element->
					password->
					attribute_name,
				row_dictionary ),
			appaserver_element->
				password->
				attribute_width_max_length,
			appaserver_element->
				password->
				element_text_max_display_size,
			appaserver_element->
				password->
				tab_order );
	}
	else
	if ( appaserver_element->element_type == upload )
	{
		if ( !appaserver_element->upload )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: upload is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( appaserver_element->upload->html )
		{
			return appaserver_element->upload->html;
		}
		else
		if ( string_strcmp( state, APPASERVER_INSERT_STATE ) == 0 )
		{
			return
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			element_upload_insert_html(
				appaserver_element_name(
					appaserver_element->
						upload->
						attribute_name,
					row_number ),
					appaserver_element->
						upload->
						tab_order );
		}
		else
		{
			return
			appaserver_element_upload_update_html(
				appaserver_element->upload,
				background_color,
				row_dictionary,
				application_name );
		}
	}
	if ( appaserver_element->element_type == yes_no )
	{
		if ( !appaserver_element->yes_no )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: yes_no is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		/* Returns heap memory or null */
		/* --------------------------- */
		return appaserver_element_yes_no_html(
			appaserver_element->yes_no,
			background_color,
			state,
			row_number,
			row_dictionary );
	}
	else
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: invalid element_type = %d.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			appaserver_element->element_type );
		return (char *)0;
	}
}

char *appaserver_element_name(
			char *name,
			int row_number )
{
	static char element_name[ 128 ];

	if ( !name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( element_name, "%s_%d", name, row_number );

	return element_name;
}

char *element_drop_down_heading( LIST *attribute_name_list )
{
	char heading[ 1024 ];
	char formatted_heading[ 1024 ];

	sprintf(heading,
		"%s", 
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_display_delimited(
			attribute_name_list,
			SQL_DELIMITER ) );

	return
	strdup(
		string_initial_capital(
			formatted_heading,
			heading ) );
}

char *element_multi_drop_down_html(
			ELEMENT_PROMPT_DROP_DOWN *original_drop_down,
			ELEMENT_TABLE_DATA *table_data,
			ELEMENT_BUTTON *move_right_button,
			ELEMENT_LINE_BREAK *element_line_break,
			ELEMENT_BUTTON *move_left_button,
			ELEMENT_PROMPT_DROP_DOWN *empty_drop_down )
{
	char html[ STRING_128K ];
	char *ptr = html;
	char *element_html;

	/* ------------------------------- */
	/* Create the left multi drop-down */
	/* ------------------------------- */

	if ( original_drop_down || !original_drop_down->element_name )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: original_drop_down->element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	element_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		element_prompt_drop_down_html(
			original_drop_down->element_name,
			original_drop_down->delimited_list,
			element_drop_down_display_list(
				original_drop_down->delimited_list,
				original_drop_down->no_initial_capital ),
			original_drop_down->output_null_option,
			original_drop_down->output_not_null_option,
			original_drop_down->output_select_option,
			original_drop_down->display_size,
			1 /* multi_select */,
			original_drop_down->tab_order,
			(char *)0 /* javascript_replace */ );

	ptr += sprintf(
		ptr,
		"%s\n",
		element_html );

	free( element_html );

	/* ---------------------------- */
	/* Create the move-right button */
	/* ---------------------------- */

	if ( !table_data )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: table_data is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"%s\n",
		( element_html =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			element_table_data_html(
				table_data->align_right,
				table_data->column_span ) ) );

	free( element_html );

	if ( !move_right_button )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: move_right_button is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"%s\n",
		( element_html =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			element_button_html(
				move_right_button->button->html ) ) );

	free( element_html );

	/* ----------------- */
	/* Create line break */
	/* ----------------- */

	if ( !element_line_break )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_line_break is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		element_line_break_html() );

	/* --------------------------- */
	/* Create the move-left button */
	/* --------------------------- */

	if ( !move_left_button )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: move_left_button is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"%s\n",
		( element_html =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			element_button_html(
				move_left_button->button->html ) ) );

	free( element_html );

	/* -------------------------- */
	/* Create the empty drop-down */
	/* -------------------------- */

	if ( empty_drop_down )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: empty_drop_down is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	element_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		element_empty_prompt_drop_down_html(
			empty_drop_down->element_name,
			empty_drop_down->display_size,
			1 /* multi_select */ );

	if ( !element_html )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: element_empty_prompt_drop_down_html() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"%s\n",
		element_html );

	free( element_html );

	return strdup( html );
}

char *appaserver_element_javascript_html(
			char *appaserver_element_javascript )
{
	static char html[ 256 ];

	*html = '\0';

	if ( appaserver_element_javascript && *appaserver_element_javascript )
	{
		sprintf(html,
			" onChange=\"%s\"",
			appaserver_element_javascript );
	}

	return html;
}

char *appaserver_element_tab_order_html(
			int tab_order )
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

char *appaserver_element_background_color_html(
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

APPASERVER_ELEMENT *appaserver_element_name_seek(
			char *element_name,
			LIST *element_list )
{
	APPASERVER_ELEMENT *element;

	if ( !list_rewind( element_list ) ) return (APPASERVER_ELEMENT *)0;

	do {
		element = list_get( element_list );

		if ( element->element_name
		&&   string_strcmp(
			element_name,
			element->element_name ) == 0 )
		{
			return element;
		}

	} while ( list_next( element_list ) );

	return (APPASERVER_ELEMENT *)0;
}

APPASERVER_ELEMENT *appaserver_element_key_seek(
			char *element_key,
			LIST *element_list )
{
	APPASERVER_ELEMENT *element;

	if ( !list_rewind( element_list ) ) return (APPASERVER_ELEMENT *)0;

	do {
		element = list_get( element_list );

		if ( element->element_key
		&&   string_strcmp(
			element_key,
			element->element_key ) == 0 )
		{
			return element;
		}

	} while ( list_next( element_list ) );

	return (APPASERVER_ELEMENT *)0;
}

boolean element_checkbox_checked(
			char *attribute_name,
			DICTIONARY *row_dictionary )
{
	char *data;

	if ( !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	data = dictionary_get( attribute_name, row_dictionary );

	if ( data && *data == 'y' )
		return 1;
	else
		return 0;
}

char *element_text_html(
			char *element_name,
			char *value,
			int attribute_width_max_length,
			int element_text_max_display_size,
			char *on_change,
			char *on_focus,
			char *on_keyup,
			boolean autocomplete_off,
			int tab_order,
			char *background_color )
{
	char html[ STRING_64K ];
	char *ptr = html;
	int size;

	if ( !element_name || !*element_name ) return (char *)0;

	if ( attribute_width_max_length > element_text_max_display_size )
	{
		size = element_text_max_display_size;
	}
	else
	{
		size = attribute_width_max_length;
	}

	ptr += sprintf(
		ptr,
		"<input name=\"%s\" type=text size=%d maxlength=%d",
		element_name,
		size,
		attribute_width_max_length );

	if ( value && *value )
	{
		ptr += sprintf(
			ptr,
			" value=\"%s\"",
			value );
	}

	if ( autocomplete_off )
	{
		ptr += sprintf( ptr, " autocomplete=\"off\"" );
	}

	if ( attribute_width_max_length > size )
	{
		ptr += sprintf( ptr, " title=\"%s\"", value );
	}

	if ( tab_order >= 1 )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_element_tab_order_html(
				tab_order ) );
	}

	if ( on_change && *on_change )
	{
		ptr += sprintf(
			ptr,
			" onChange=\"%s\"",
			on_change );
	}

	if ( on_focus && *on_focus )
	{
		ptr += sprintf(
			ptr,
			" onFocus=\"%s\"",
			on_focus );
	}

	if ( on_keyup && *on_keyup )
	{
		ptr += sprintf(
			ptr,
			" onKeyup=\"%s\"",
			on_keyup );
	}

	if ( background_color && *background_color )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_element_background_color_html(
				background_color ) );
	}

	ptr += sprintf( ptr, ">" );

	return strdup( html );
}

ELEMENT_TEXT *element_text_calloc( void )
{
	ELEMENT_TEXT *element_text;

	if ( ! ( element_text = calloc( 1, sizeof( ELEMENT_TEXT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

	}
	return element_text;
}

ELEMENT_TEXT *element_text_new(
			char *attribute_name,
			char *datatype_name,
			int attribute_width_max_length,
			boolean null_to_slash,
			boolean prevent_carrot,
			char *on_change,
			char *on_focus,
			char *on_keyup,
			int tab_order,
			boolean recall )
{
	ELEMENT_TEXT *element_text = element_text_calloc();

	element_text->attribute_name = attribute_name;
	element_text->datatype_name = datatype_name;

	element_text->attribute_width_max_length =
		attribute_width_max_length;

	element_text->null_to_slash = null_to_slash;
	element_text->prevent_carrot = prevent_carrot;
	element_text->on_change = on_change;
	element_text->on_focus = on_focus;
	element_text->on_keyup = on_keyup;
	element_text->tab_order = tab_order;
	element_text->recall = recall;

	return element_text;
}

char *element_text_value(
			char *attribute_name,
			DICTIONARY *row_dictionary,
			boolean is_number )
{
	char *value;

	if ( !attribute_name || !row_dictionary ) return (char *)0;

	if ( ! ( value = dictionary_get( attribute_name, row_dictionary ) ) )
	{
		return (char *)0;
	}

	if ( is_number )
	{
		char *hold = value;

		value =
			/* ---------------------------- */
			/* Returns value or heap memory */
			/* ---------------------------- */
			appaserver_element_number_commas_string(
				attribute_name,
				value );

		if ( value != hold ) free( hold );
	}

	return value;
}

char *element_text_javascript_replace_on_change(
			char *on_change,
			int row_number,
			char *state,
			boolean null_to_slash )
{
	char text_on_change[ 1024 ];
	char *ptr = text_on_change;

	if ( ( !on_change || !*on_change ) && !null_to_slash )
	{
		return (char *)0;
	}

	if ( on_change && *on_change )
	{
		char *tmp;

		if ( ( tmp =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			javascript_replace(
				on_change,
				state,
				row_number ) ) )
		{
			ptr += sprintf( ptr, "%s", tmp );
			free( tmp );
		}
	}

	if ( null_to_slash )
	{
		if ( ptr != text_on_change )
		{
			ptr += sprintf( ptr, ";" );
		}

		ptr += sprintf(
			ptr,
		 	"null2slash(this)" );
	}

	return strdup( text_on_change );
}

char *element_text_javascript_replace_on_focus(
			char *on_focus,
			int row_number,
			char *state )
{
	if ( !on_focus || !*on_focus ) return (char *)0;

	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	javascript_replace(
		on_focus,
		state,
		row_number );
}

char *element_text_prevent_carrot_on_keyup(
			char *on_keyup,
			boolean prevent_carrot )
{
	char text_on_keyup[ 1024 ];
	char *ptr = text_on_keyup;

	if ( ( !on_keyup || !*on_keyup ) && !prevent_carrot )
	{
		return (char *)0;
	}

	if ( on_keyup && *on_keyup )
	{
		ptr += sprintf(
			ptr,
			"%s",
			on_keyup );
	}

	if ( prevent_carrot )
	{
		if ( ptr != text_on_keyup )
		{
			ptr += sprintf( ptr, ";" );
		}

		ptr += sprintf(
			ptr,
			"timlib_prevent_carrot(event,this)" );
	}

	return strdup( text_on_keyup );
}

boolean element_text_autocomplete_off( char *attribute_name )
{
	if ( string_instr( "password", attribute_name, 1 ) != -1
	||   string_instr( "login_name", attribute_name, 1 ) != -1 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

char *appaserver_element_value(
			char *attribute_name,
			DICTIONARY *row_dictionary )
{
	if ( !attribute_name || !row_dictionary ) return (char *)0;

	return
	dictionary_get(
		attribute_name,
		row_dictionary );
}

char *appaserver_element_prompt_drop_down_html(
			ELEMENT_PROMPT_DROP_DOWN *prompt_drop_down,
			char *state )
{
	if ( !prompt_drop_down )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: prompt_drop_down is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	/* Returns heap memory */
	/* ------------------- */
	return element_prompt_drop_down_html(
		prompt_drop_down->element_name,
		prompt_drop_down->delimited_list,
		element_drop_down_display_list(
			prompt_drop_down->delimited_list,
			prompt_drop_down->no_initial_capital ),
		prompt_drop_down->output_null_option,
		prompt_drop_down->output_not_null_option,
		prompt_drop_down->output_select_option,
		(prompt_drop_down->display_size)
			? prompt_drop_down->display_size
			: element_drop_down_display_size(
				list_length(
					prompt_drop_down->delimited_list ) ),
		prompt_drop_down->multi_select,
		prompt_drop_down->tab_order,
		javascript_replace(
			prompt_drop_down->post_change_javascript,
			state,
			0 /* row_number */ ) );
}

char *appaserver_element_drop_down_html(
			ELEMENT_DROP_DOWN *drop_down,
			char *background_color,
			char *state,
			int row_number,
			DICTIONARY *row_dictionary )
{
	char *html;

	if ( !drop_down )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: drop_down is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_length( drop_down->attribute_name_list )
	&&   dictionary_length( row_dictionary ) )
	{
		drop_down->value =
			appaserver_element_value(
				/* ----------------------------- */
				/* Returns static memory or null */
				/* ----------------------------- */
				appaserver_element_key_string(
					drop_down->attribute_name_list ),
				row_dictionary );
	}

	if ( !drop_down->value ) return (char *)0;

	/* Returns heap memory */
	/* ------------------- */
	return element_drop_down_html(
		(drop_down->name)
			? drop_down->name
			: /* --------------------- */
			  /* Returns static memory */
			  /* --------------------- */
			  element_drop_down_name(
				drop_down->attribute_name_list,
				row_number ),
		drop_down->value,
		drop_down->delimited_list,
		(drop_down->display_list)
			? drop_down->display_list
			: element_drop_down_display_list(
				drop_down->
					delimited_list,
				drop_down->
					no_initial_capital ),
		drop_down->output_null_option,
		drop_down->output_not_null_option,
		drop_down->output_select_option,
		(drop_down->display_size)
			? drop_down->display_size
			: element_drop_down_display_size(
				list_length(
					drop_down->delimited_list ) ),
		0 /* not multi_select */,
		drop_down->tab_order,
		/* ----------------------------- */
		/* Returns static memory or null */
		/* ----------------------------- */
		javascript_replace(
			drop_down->post_change_javascript,
			state,
			row_number ),
		background_color );

	return html;
}

ELEMENT_TABLE_DATA *element_table_data_calloc( void )
{
	ELEMENT_TABLE_DATA *element_table_data;

	if ( ! ( element_table_data =
			calloc( 1, sizeof( ELEMENT_TABLE_DATA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return element_table_data;
}

ELEMENT_TABLE_DATA *element_table_data_new(
			boolean align_right,
	 		int column_span )
{
	ELEMENT_TABLE_DATA *element_table_data = element_table_data_calloc();

	element_table_data->align_right = align_right;
	element_table_data->column_span = column_span;

	return element_table_data;
}

char *element_drop_down_option_value_list_html(
			LIST *delimited_list,
			LIST *display_list,
			char *value )
{
	char html[ STRING_TWO_MEG ];
	char *ptr = html;
	char *local_value;

	*ptr = '\0';

	if ( !list_rewind( delimited_list ) ) return strdup( html );

	if ( list_length( delimited_list ) != list_length( display_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: list length mismatch.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list_rewind( display_list );

	do {
		local_value = list_get( display_list );

		if ( value && strcmp( value, local_value ) == 0 )
		{
			list_next( display_list );
			continue;
		}

		if ( ptr != html ) ptr += sprintf( ptr, "\n" );

		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			element_drop_down_option_value_html(
				local_value,
				list_get( display_list ) ) );

		list_next( display_list );

	} while ( list_next( delimited_list ) );

	return strdup( html );
}

char *appaserver_element_text_html(
			ELEMENT_TEXT *text,
			char *background_color,
			char *state,
			int row_number,
			DICTIONARY *row_dictionary )
{
	char *html;

	text->value =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		element_text_value(
			text->attribute_name,
			row_dictionary,
			attribute_is_number(
			text->datatype_name ) );

	text->javascript_replace_on_change =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		element_text_javascript_replace_on_change(
			text->on_change,
			row_number,
			state,
			text->null_to_slash );

	text->javascript_replace_on_focus =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		element_text_javascript_replace_on_focus(
			text->on_focus,
			row_number,
			state );

	text->prevent_carrot_on_keyup =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		element_text_prevent_carrot_on_keyup(
			text->on_keyup,
			text->prevent_carrot );

	text->autocomplete_off =
		element_text_autocomplete_off(
			text->attribute_name );

	html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		element_text_html(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_element_name(
				text->
					attribute_name,
				row_number ),
			text->value,
			text->attribute_width_max_length,
			ELEMENT_TEXT_MAX_DISPLAY_SIZE,
			text->javascript_replace_on_change,
			text->javascript_replace_on_focus,
			text->prevent_carrot_on_keyup,
			text->autocomplete_off,
			text->tab_order,
			background_color );

	/* Reset value for next iteration */
	/* ------------------------------ */
	if ( text->value )
	{
		free( text->value );
		text->value = (char *)0;
	}

	if ( text->javascript_replace_on_change )
	{
		free( text->javascript_replace_on_change );
	}

	if ( text->javascript_replace_on_focus )
	{
		free( text->javascript_replace_on_focus );
	}

	if ( text->prevent_carrot_on_keyup )
	{
		free( text->prevent_carrot_on_keyup );
	}

	return html;
}

char *appaserver_element_key_string( LIST *attribute_name_list )
{
	static char key_string[ 1024 ];
	char *results;

	if ( !list_length( attribute_name_list ) ) return (char *)0;

	results =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_display_delimited(
			attribute_name_list,
			',' /* delimiter */ );

	strcpy( key_string, results );
	free( results );

	return key_string;
}

ELEMENT_HIDDEN *element_hidden_calloc( void )
{
	ELEMENT_HIDDEN *element_hidden;

	if ( ! ( element_hidden = calloc( 1, sizeof( ELEMENT_HIDDEN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return element_hidden;
}

ELEMENT_HIDDEN *element_hidden_new(
			char *attribute_name,
			LIST *attribute_name_list )
{
	ELEMENT_HIDDEN *element_hidden = element_hidden_calloc();

	element_hidden->attribute_name = attribute_name;
	element_hidden->attribute_name_list = attribute_name_list;

	return element_hidden;
}

/* Returns heap memory */
/* ------------------- */
char *element_hidden_html(
			char *element_name,
			char *value )
{
	char html[ 1024 ];

	if ( !element_name || !value )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"<input name=%s type=hidden value=\"%s\">",
		element_name,
		value );

	return strdup( html );
}

char *appaserver_element_hidden_html(
			ELEMENT_HIDDEN *hidden,
			int row_number,
			DICTIONARY *row_dictionary )
{
	char *html;

	if ( !hidden )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: hidden is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( hidden->key_string =
			element_hidden_key_string(
				hidden->attribute_name,
				hidden->attribute_name_list ) ) )
	{
		return (char *)0;
	}

	hidden->element_hidden_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		element_hidden_name(
			hidden->key_string,
			row_number );

	hidden->value =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		appaserver_element_value(
			hidden->key_string,
			row_dictionary );

	if ( !hidden->value ) return (char *)0;

	html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		element_hidden_html(
			hidden->element_hidden_name,
			hidden->value );

	free( hidden->value );

	return html;
}

char *element_hidden_name(
			char *key_string,
			int row_number )
{
	static char name[ 1024 ];

	if ( !key_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: key_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( name, "%s_%d", key_string, row_number );

	return name;
}

char *element_hidden_key_string(
			char *attribute_name,
			LIST *attribute_name_list )
{

	if ( attribute_name )
	{
		static char key_string[ 1024 ];

		strcpy( key_string, attribute_name );
		return key_string;
	}
	else
	if ( list_length( attribute_name_list ) )
	{
		return
		/* ----------------------------- */
		/* Returns static memory or null */
		/* ----------------------------- */
		appaserver_element_key_string(
			attribute_name_list );
	}
	else
	{
		return (char *)0;
	}
}

char *element_notepad_html(
			char *element_name,
			char *value,
			int attribute_size,
			int columns,
			int rows,
			boolean null_to_slash,
			char *javascript_replace,
			int tab_order,
			char *background_color )
{
	char html[ STRING_128K ];
	char *ptr = html;
	char *on_change;

	if ( !element_name ) return (char *)0;

	if ( attribute_size >= ELEMENT_LARGE_NOTEPAD_THRESHOLD )
	{
		columns = ELEMENT_LARGE_NOTEPAD_COLUMNS;
		rows = ELEMENT_LARGE_NOTEPAD_ROWS;
	}

	ptr += sprintf(
		ptr,
		"<textarea name=\"%s\" cols=%d rows=%d",
		element_name,
		columns,
		rows );

	if ( ( on_change =
		element_notepad_onchange(
			null_to_slash,
			javascript_replace ) ) )
	{
		ptr += sprintf( ptr, "%s", on_change );
	}

	ptr += sprintf(
		ptr,
		" onblur=\"timlib_check_notepad_size(this, %d);\"",
		attribute_size );

	ptr += sprintf(
		ptr,
		" onkeyup=\"timlib_prevent_carrot(event,this)\"" );

	if ( tab_order >= 1 )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_element_tab_order_html(
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
			appaserver_element_background_color_html(
				background_color ) );
	}

	ptr += sprintf(
		ptr,
		 " wrap=%s>\n",
		 ELEMENT_TEXTAREA_WRAP );

	if ( value && *value )
	{
		ptr += sprintf(
			ptr,
		 	"%s\n",
		 	value );
	}

	ptr += sprintf(
		ptr,
		 "</textarea>\n" );

	return strdup( html );
}

char *element_notepad_onchange(
			boolean null_to_slash,
			char *javascript_replace )
{
	char on_change[ 512 ];
	char *ptr = on_change;
	boolean got_one = 0;

	if ( !null_to_slash && !javascript_replace ) return (char *)0;

	ptr += sprintf( ptr, " onChange=\"" );

	if ( null_to_slash )
	{
		ptr += sprintf( ptr, "null2slash(this)" );
		got_one = 1;
	}

	if ( javascript_replace && *javascript_replace )
	{
		if ( got_one ) ptr += sprintf( ptr, " && " );
		ptr += sprintf( ptr, "%s", javascript_replace );
		got_one = 1;
	}

	if ( got_one )
	{
		ptr += sprintf( ptr, "\"" );
		return strdup( on_change );
	}
	else
	{
		return (char *)0;
	}
}

ELEMENT_NOTEPAD *element_notepad_calloc( void )
{
	ELEMENT_NOTEPAD *element_notepad;

	if ( ! ( element_notepad = calloc( 1, sizeof( ELEMENT_NOTEPAD ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return element_notepad;
}

ELEMENT_NOTEPAD *element_notepad_new(
			char *attribute_name,
			int attribute_size,
			int columns,
			int rows,
			boolean null_to_slash,
			char *post_change_javascript,
			int tab_order )
{
	ELEMENT_NOTEPAD *element_notepad = element_notepad_calloc();

	element_notepad->attribute_name = attribute_name;
	element_notepad->attribute_size = attribute_size;
	element_notepad->columns = columns;
	element_notepad->rows = rows;
	element_notepad->null_to_slash = null_to_slash;
	element_notepad->post_change_javascript = post_change_javascript;
	element_notepad->tab_order = tab_order;

	return element_notepad;
}

char *appaserver_element_notepad_html(
			ELEMENT_NOTEPAD *notepad,
			char *background_color,
			char *state,
			int row_number,
			DICTIONARY *row_dictionary )
{
	if ( !notepad )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: notepad is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	element_notepad_html(
		appaserver_element_name(
			notepad->attribute_name,
			row_number ),
		appaserver_element_value(
			notepad->attribute_name,
			row_dictionary ),
		notepad->attribute_size,
		notepad->columns,
		notepad->rows,
		notepad->null_to_slash,
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		javascript_replace(
			notepad->post_change_javascript,
			state,
			row_number ),
		notepad->tab_order,
		background_color );
}

ELEMENT_PASSWORD *element_password_calloc( void )
{
	ELEMENT_PASSWORD *element_password;

	if ( ! ( element_password = calloc( 1, sizeof( ELEMENT_PASSWORD ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

	}
	return element_password;
}

ELEMENT_PASSWORD *element_password_new(
			char *attribute_name,
			int attribute_width_max_length,
			int element_text_max_display_size,
			int tab_order )
{
	ELEMENT_PASSWORD *element_password = element_password_calloc();

	element_password->attribute_name = attribute_name;

	element_password->attribute_width_max_length =
		attribute_width_max_length;

	element_password->element_text_max_display_size =
		element_text_max_display_size;

	element_password->tab_order = tab_order;

	return element_password;
}

char *element_password_compare_element_name(
			char *attribute_name,
			int row_number )
{
	static char element_name[ 128 ];

	sprintf(element_name,
		"%s_compare_%d",
		attribute_name,
		row_number );

	return element_name;
}

char *element_password_html(
			char *element_name,
			char *compare_element_name,
			char *value,
			int attribute_width_max_length,
			int element_text_max_display_size,
			int tab_order )
{
	char html[ STRING_16K ];
	char *ptr = html;
	int size;

	if ( !element_name || !*element_name ) return (char *)0;
	if ( !compare_element_name || !*compare_element_name ) return (char *)0;

	if ( attribute_width_max_length > element_text_max_display_size )
	{
		size = element_text_max_display_size;
	}
	else
	{
		size = attribute_width_max_length;
	}

	ptr += sprintf(
		ptr,
		"%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		element_table_open_html() );

	/* Output the password field */
	/* ------------------------- */
	ptr += sprintf( ptr, "<tr><td>Type\n" );

	ptr += sprintf(
		ptr,
		"<td><input name=\"%s\" type=password size=%d maxlength=%d",
		element_name,
		size,
		attribute_width_max_length );

	if ( value && *value )
	{
		ptr += sprintf(
			ptr,
			" value=\"%s\"",
			value );
	}

	ptr += sprintf( ptr, " autocomplete=off" );

	ptr += sprintf( ptr, " onChange=\"null2slash(this)\"" );

	if ( tab_order >= 1 )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_element_tab_order_html(
				tab_order ) );
	}

	ptr += sprintf( ptr, ">\n" );

	/* Output the password compare field */
	/* --------------------------------- */
	ptr += sprintf( ptr, "<tr><td>Retype\n" );

	ptr += sprintf(
		ptr,
"<td><input name=\"%s\" type=password size=%d maxlength=%d",
		compare_element_name,
		size,
		attribute_width_max_length );

	ptr += sprintf(
		ptr,
		" onChange=\"password_compare('%s','%s')\"",
		element_name,
		compare_element_name );

	ptr += sprintf( ptr, " autocomplete=off" );

	if ( tab_order >= 1 )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_element_tab_order_html(
				tab_order ) );
	}

	ptr += sprintf( ptr, ">\n" );

	ptr += sprintf( ptr, "</table>\n" );

	return strdup( html );
}

ELEMENT_UPLOAD *element_upload_calloc( void )
{
	ELEMENT_UPLOAD *element_upload;

	if ( ! ( element_upload =
			calloc( 1, sizeof( ELEMENT_UPLOAD ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return element_upload;
}

ELEMENT_UPLOAD *element_upload_new(
			char *attribute_name,
			int tab_order,
			boolean recall )
{
	ELEMENT_UPLOAD *element_upload = element_upload_calloc();

	element_upload->attribute_name = attribute_name;
	element_upload->tab_order = tab_order;
	element_upload->recall = recall;

	return element_upload;
}

char *element_upload_insert_html(
			char *element_name,
			int tab_order )
{
	char html[ 128 ];
	char *ptr = html;

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"<input name=\"%s\" type=file accept=\"*\" value=\"\"",
		element_name );

	if ( tab_order >= 1 )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_element_tab_order_html(
				tab_order ) );
	}

	ptr += sprintf( ptr, ">" );

	return strdup( html );
}

char *element_upload_update_html(
			char *value,
			char *background_color,
			char *application_name )
{
	char html[ 1024 ];
	char *ptr = html;
	static int target_offset = 0;

	ptr += sprintf(
		ptr,
		"<a href=\"%s\" target=upload_%d %s>%s</a>",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		element_upload_update_hypertext_reference(
			application_name,
			value ),
		++target_offset,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_element_background_color_html(
			background_color ),
		value );

	return strdup( html );
}


char *element_upload_update_hypertext_reference(
			char *application_name,
			char *value )
{
	static char hypertext_reference[ 256 ];

	if ( !application_name
	||   !value
	||   !*value )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(hypertext_reference,
		"/appaserver/%s/data/%s",
		application_name,
		value );

	return hypertext_reference;
}

void element_upload_update_link(
			char *application_name,
			char *value,
			char *upload_directory,
			char *document_root )
{
	char system_string[ 1024 ];

	if ( !application_name
	||   !value
	||   !upload_directory
	||   !document_root )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"ln -s %s/%s/%s %s/appaserver/%s/data/%s",
		upload_directory,
		application_name,
		value,
		document_root,
		application_name,
		value );

	if ( system( system_string ) ){};
}

ELEMENT_CHECKBOX *element_checkbox_calloc( void )
{
	ELEMENT_CHECKBOX *element_checkbox;

	if ( ! ( element_checkbox = calloc( 1, sizeof( ELEMENT_CHECKBOX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return element_checkbox;
}

ELEMENT_CHECKBOX *element_checkbox_new(
			char *attribute_name,
			char *element_name,
			char *prompt_string,
			char *on_click,
			int tab_order,
			char *image_source,
			boolean recall )
{
	ELEMENT_CHECKBOX *element_checkbox = element_checkbox_calloc();

	element_checkbox->attribute_name = attribute_name;
	element_checkbox->element_name = element_name;
	element_checkbox->prompt_string = prompt_string;
	element_checkbox->on_click = on_click;
	element_checkbox->tab_order = tab_order;
	element_checkbox->image_source = image_source;
	element_checkbox->recall = recall;

	return element_checkbox;
}

char *element_checkbox_javascript_replace_on_click(
			char *on_click,
			int row_number,
			char *state )
{
	char replace_on_click[ 1024 ];
	char *ptr = replace_on_click;
	char *tmp;

	if ( ( !on_click || !*on_click ) )
	{
		return (char *)0;
	}

	if ( ( tmp =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		javascript_replace(
			on_click,
			state,
			row_number ) ) )
	{
		ptr += sprintf( ptr, "%s", tmp );
		free( tmp );
	}

	return strdup( replace_on_click );
}

char *appaserver_element_checkbox_html(
			ELEMENT_CHECKBOX *checkbox,
			char *background_color,
			char *state,
			int row_number,
			DICTIONARY *row_dictionary )
{
	char prompt_display[ 256 ];
	char *html;

	if ( !checkbox )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: checkbox is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	checkbox->checked =
		element_checkbox_checked(
			checkbox->attribute_name,
			row_dictionary );

	checkbox->javascript_replace_on_click =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		element_checkbox_javascript_replace_on_click(
			checkbox->on_click,
			row_number,
			state );

	html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		element_checkbox_html(
			(checkbox->element_name)
				? checkbox->element_name
				: /* --------------------- */
				  /* Returns static memory */
				  /* --------------------- */
				  appaserver_element_name(
					checkbox->attribute_name,
					row_number ),
			string_initial_capital(
				prompt_display,
				checkbox->prompt_string ),
			checkbox->checked,
			checkbox->javascript_replace_on_click,
			checkbox->tab_order,
			background_color,
			checkbox->image_source );

/*
	if ( checkbox->javascript_replace_on_click )
	{
		free( checkbox->javascript_replace_on_click );
	}
*/

	return html;
}

APPASERVER_ELEMENT *appaserver_element_operation_table_heading(
			char *process_name,
			char *delete_warning_javascript )
{
	APPASERVER_ELEMENT *element;
	char on_click[ 128 ];

	if ( !process_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: process_name is empty.\n",
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
			"%s;form_push_button_set_all('%s',0);",
			delete_warning_javascript,
			process_name );
	}
	else
	{
		sprintf(on_click, 
			"form_push_button_set_all('%s',0);",
			process_name );
	}

	element->checkbox->element_name = process_name;
	element->checkbox->on_click = strdup( on_click );

	return element;
}

APPASERVER_ELEMENT *appaserver_element_table_heading(
			char *heading_string )
{
	APPASERVER_ELEMENT *element;

	element =
		appaserver_element_new(
			table_heading,
			(char *)0 /* element_name */ );

	element->table_heading->heading_string = heading_string;

	return element;
}

char *element_checkbox_submit_javascript(
			char *on_click_javascript,
			char *form_name )
{
	char submit_javascript[ 1024 ];
	char *ptr = submit_javascript;

	if ( !form_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: form_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( on_click_javascript )
	{
		ptr += sprintf( ptr, "%s &&", on_click_javascript );
	}

	ptr += sprintf(
		ptr,
		"push_button_submit('%s')",
		form_name );

	return strdup( submit_javascript );
}

char *appaserver_element_upload_update_html(
			ELEMENT_UPLOAD *upload,
			char *background_color,
			DICTIONARY *row_dictionary,
			char *application_name )
{
	char *value =
		/* ------------------------------------ */
		/* Returns row_dictionary->		*/
		/*		hash_table->		*/
		/* 		other_data or null	*/
		/* ------------------------------------ */
		appaserver_element_value(
			upload->attribute_name,
			row_dictionary );

	if ( !value )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: appaserver_element_value(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			upload->attribute_name );
 		return (char *)0;
	}

	/* Set a link to $DOCUMENT_ROOT */
	/* ---------------------------- */
	element_upload_update_link(
		application_name,
		value,
		appaserver_parameter_upload_directory(),
		appaserver_parameter_document_root() );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	element_upload_update_html(
		value,
		background_color,
		application_name );
}

char *appaserver_element_yes_no_html(
			ELEMENT_YES_NO *yes_no,
			char *background_color,
			char *state,
			int row_number,
			DICTIONARY *row_dictionary )
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
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	element_yes_no_html(
		(yes_no->element_name)
			? yes_no->element_name
			: appaserver_element_name(
				yes_no->attribute_name,
				row_number ),
		element_yes_no_affirmative(
			yes_no->attribute_name,
			row_dictionary ),
		yes_no->output_null_option,
		yes_no->output_not_null_option,
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		javascript_replace(
			yes_no->post_change_javascript,
			state,
			row_number ),
		yes_no->tab_order,
		background_color );
}

ELEMENT_YES_NO *element_yes_no_new(
			char *attribute_name,
			char *element_name,
			boolean output_null_option,
			boolean output_not_null_option,
			char *post_change_javascript,
			int tab_order,
			boolean recall )
{
	ELEMENT_YES_NO *element_yes_no = element_yes_no_calloc();

	element_yes_no->attribute_name = attribute_name;
	element_yes_no->element_name = element_name;
	element_yes_no->output_null_option = output_null_option;
	element_yes_no->output_not_null_option = output_not_null_option;
	element_yes_no->post_change_javascript = post_change_javascript;
	element_yes_no->tab_order = tab_order;
	element_yes_no->recall = recall;

	return element_yes_no;
}

boolean element_yes_no_affirmative(
			char *attribute_name,
			DICTIONARY *row_dictionary )
{
	char *data;

	if ( !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	data = dictionary_get( attribute_name, row_dictionary );

	if ( data && *data == 'y' )
		return 1;
	else
		return 0;
}

char *element_yes_no_html(
			char *element_name,
			boolean yes_no_affirmative,
			boolean output_null_option,
			boolean output_not_null_option,
			char *javascript_replace,
			int tab_order,
			char *background_color )
{
	char html[ 1024 ];
	char *ptr = html;
	char buffer[ 256 ];
	LIST *delimited_list = list_new();
	LIST *display_list = list_new();
	char *value_list_html;

	if ( !element_name || !*element_name )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"<select name=\"%s\" id=\"%s\"",
		element_name,
		element_name );

	if ( tab_order > 0 )
	{
		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_element_tab_order_html(
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
			appaserver_element_javascript_html(
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
			appaserver_element_background_color_html(
				background_color ) );
	}

	ptr += sprintf( ptr, ">\n" );

	if ( !yes_no_affirmative )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			element_drop_down_option_value_html(
				ELEMENT_SELECT_OPERATOR,
				string_initial_capital(
					buffer,
					ELEMENT_SELECT_OPERATOR ) ) );
	}

	list_set( delimited_list, "y" );
	list_set( delimited_list, "n" );
	list_set( display_list, "Yes" );
	list_set( display_list, "No" );

	value_list_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		element_drop_down_option_value_list_html(
			delimited_list,
			display_list,
			(char *)0 /* value */ );

	ptr += sprintf( ptr, "%s\n", value_list_html );

	free( value_list_html );

	if ( yes_no_affirmative )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			element_drop_down_option_value_html(
				ELEMENT_SELECT_OPERATOR,
				string_initial_capital(
					buffer,
					ELEMENT_SELECT_OPERATOR ) ) );
	}

	ptr += sprintf(
		ptr,
		"%s\n",
		/* ---------------------- */
		/* Returns static memory. */
		/* ---------------------- */
		element_drop_down_close_html(
			output_null_option,
			output_not_null_option ) );

	return strdup( html );
}

ELEMENT_YES_NO *element_yes_no_calloc( void )
{
	ELEMENT_YES_NO *element_yes_no;

	if ( ! ( element_yes_no = calloc( 1, sizeof( ELEMENT_YES_NO ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return element_yes_no;
}

boolean appaserver_element_recall_boolean(
			APPASERVER_ELEMENT *element )
{
	if ( element->element_type == multi_drop_down )
		return 1;
	else
	if ( element->element_type == drop_down )
		return element->drop_down->recall;
	else
	if ( element->element_type == prompt_drop_down )
		return element->prompt_drop_down->recall;
	else
	if ( element->element_type == text )
		return element->text->recall;
	else
	if ( element->element_type == checkbox )
		return element->checkbox->recall;
	else
	if ( element->element_type == upload )
		return element->upload->recall;
	else
	if ( element->element_type == yes_no )
		return element->yes_no->recall;
	else
		return 0;
}

ELEMENT_DROP_DOWN *element_drop_down_calloc( void )
{
	ELEMENT_DROP_DOWN *element_drop_down;

	if ( ! ( element_drop_down =
			calloc( 1, sizeof( ELEMENT_DROP_DOWN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return element_drop_down;
}

