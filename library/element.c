/* $APPASERVER_HOME/library/element.c					*/
/* -------------------------------------------------------------------- */
/* This is the appaserver element ADT.					*/
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
#include "query.h"
#include "date.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "piece.h"
#include "basename.h"
#include "javascript.h"
#include "element.h"

APPASERVER_ELEMENT *appaserver_element_calloc( void )
{
	APPASERVER_ELEMENT *element;

	if ( ! ( element =
			calloc( 1, sizeof( APPASERVER_ELEMENT ) ) ) )
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
			enum element_type element_type )
{
	APPASERVER_ELEMENT *element = appaserver_element_calloc();

	element->element_type = element_type;

	if ( element_type == table_row )
		element->table_row =
			element_table_row_calloc();
	else
	if ( element_type == non_edit_text )
		element->non_edit_text =
			element_non_edit_text_calloc();
	else
	if ( element_type == checkbox )
		element->checkbox =
			element_checkbox_calloc();
	else
	if ( element_type == drop_down )
		element->drop_down =
			element_drop_down_calloc();

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

char *element_password_html(
			char *element_name,
			char *data,
			int attribute_width,
			int row,
			int tab_order )
{
	char html[ 1024 ];
	char *ptr = html;
	int maxlength;
	int size;

	maxlength = attribute_width;

	if ( attribute_width > ELEMENT_MAX_TEXT_WIDTH )
		size = ELEMENT_MAX_TEXT_WIDTH;
	else
		size = attribute_width;

	if ( !data ) data = "";

	ptr += sprintf( ptr, "<td>\n" );

	ptr += sprintf( ptr, "<table border=0>\n" );

	ptr += sprintf( ptr, "<tr><td>Type\n" );

	ptr += sprintf( ptr,
	"<td><input name=\"%s_%d\" type=\"password\" size=\"%d\" value=\"%s\"",
		element_name, row, size, data );

	ptr += sprintf( ptr, " maxlength=\"%d\"", maxlength );

	ptr += sprintf( ptr,
		 " onChange=\"null2slash(this)\"" );

	ptr += sprintf( ptr, " Autocomplete=Off" );

	if ( tab_order > 0 )
	{
		ptr += sprintf(
			ptr,
			" taborder=%d",
			tab_order );
	}

	ptr += sprintf( ptr, ">\n" );

	/* Output the password compare field */
	/* --------------------------------- */
	ptr += sprintf( ptr, "<tr><td>Retype\n" );

	ptr += sprintf( ptr,
"<td><input name=\"%s_compare_%d\" type=\"password\" size=\"%d\"",
		element_name, row, size );

	ptr += sprintf( ptr, " maxlength=\"%d\"", maxlength );

	ptr += sprintf( ptr,
		 " onChange=\"password_compare('%s_%d','%s_compare_%d')\"",
		 element_name,
		 row,
		 element_name,
		 row );

	ptr += sprintf( ptr, " Autocomplete=Off" );

	if ( tab_order > 0 )
	{
		ptr += sprintf(
			ptr,
			" taborder=%d",
			tab_order +1 );
	}

	ptr += sprintf( ptr, ">\n" );

	ptr += sprintf( ptr, "</table>\n" );
	ptr += sprintf( ptr, "</td>\n" );

	return strdup( html );
}

void element_password_erase_data( char *data )
{
	while( *data ) *data++ = '*';
}

char *element_drop_down_html(
			char *drop_down_name,
			char *initial_data,
			LIST *delimited_list,
			LIST *display_list,
			boolean output_null_option,
			boolean output_not_null_option,
			boolean output_select_option,
			int column_span,
			int drop_down_size,
			boolean multi_select,
			char *post_change_javascript,
			char *background_color,
			int tab_order )
{
	char html[ STRING_TWO_MEG ];
	char *ptr = html;
	char *initial_data_html = {0};
	char *data;
	char *display = {0};
	char buffer[ 128 ];

	ptr += sprintf(
		ptr,
		"<td align=left colspan=%d>\n"
		"<select name=\"%s\" id=\"%s\" size=%d",
		(column_span) ? column_span : 1,
		drop_down_name,
		drop_down_name,
		(drop_down_size) ? drop_down_size : 1 );

	if ( multi_select ) ptr += sprintf( ptr, " multiple" );

	if ( tab_order > 0 )
	{
		ptr += sprintf(
			ptr,
			" taborder=%d",
			tab_order );
	}

	if ( background_color && *background_color )
	{
		ptr += sprintf(
			ptr,
			" style=\"{background:%s}\"",
			background_color );
	}

	if ( post_change_javascript && *post_change_javascript )
	{
		ptr += sprintf(
			ptr,
			" onChange=\"%s\"",
			post_change_javascript );
	}

	ptr += sprintf( ptr, ">\n" );

	if ( initial_data && *initial_data )
	{
		initial_data_html =
			/* ------------------------------ */
			/* Zaps row in delimited_list and */
			/* adds "Select" to bottom.	  */
			/* Returns heap memory or null.	  */
			/* ------------------------------ */
			element_drop_down_initial_data_html(
				delimited_list,
				display_list,
				initial_data );
	}

	if ( initial_data_html )
	{
		ptr += sprintf( ptr, "%s\n", initial_data_html );
	}
	else
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

	if ( !list_rewind( delimited_list ) )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			element_drop_down_close_html(
				output_null_option,
				output_not_null_option,
				output_select_option ) );
		
		return strdup( html );
	}

	if ( display_list ) list_rewind( display_list );

	do {
		data = list_get( delimited_list );

		/* Skip the preselected item. */
		/* -------------------------- */
		if ( !data )
		{
			if ( display_list ) list_next( display_list );
			continue;
		}

		if ( display_list )
		{
			display = list_get( display_list );
		}

		ptr += sprintf(
			ptr,
			"%s\n",
			/* ---------------------- */
			/* Returns static memory. */
			/* ---------------------- */
			element_drop_down_option_value_html(
				data,
				display ) );

		if ( display_list )
		{
			list_next( display_list );
		}

	} while( list_next( delimited_list ) );

	ptr += sprintf(
		ptr,
		"%s\n",
		/* Returns static memory. */
		/* ---------------------- */
		element_drop_down_close_html(
			output_null_option,
			output_not_null_option,
			output_select_option ) );

	return strdup( html );
}

char *element_table_row_html( void )
{
	return "<tr>";
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

char *appaserver_element_post_change_javascript(
			char *source,
			int row_number,
			char *state )
{
	char post_change_javascript[ 1024 ];
	char row_string[ 8 ];

	if ( !source || !*source ) return strdup( "" );

	strcpy( post_change_javascript, source );

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

	free( source );
	return strdup( post_change_javascript );
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

		if ( element->element_type == upload_filename ) return 1;

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

char *element_place_commas_in_number_string(
				char *element_name,
				char *data )
{
	if ( timlib_exists_string( element_name, "year" ) )
		return data;
	else
		return place_commas_in_number_string( data );
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

char *element_non_edit_text_html( char *message )
{
	char html[ 128 ];

	if ( !message || !*message )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: message is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( html, "<td align=left>%s", message );

	return strdup( html );
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

char *element_drop_down_name(
			LIST *element_name_list,
			int row_number )
{
	char drop_down_name[ 1024 ];

	if ( !list_length( element_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(drop_down_name,
		"%s_%d",
		list_display_delimited(
			element_name_list,
			SQL_DELIMITER ),
		row_number );

	return strdup( drop_down_name );
}

int appaserver_element_tab_order( int tab_order )
{
	if ( tab_order == -1 ) return -1;

	return tab_order + 1;
}

int element_drop_down_size(
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

char *element_drop_down_initial_data_html(
			LIST *delimited_list,
			LIST *display_list,
			char *initial_data )
{
	int offset;
	char *data;
	char *display = {0};

	if ( !initial_data || !*initial_data ) return (char *)0;

	if ( ( offset =
		list_seek_offset(
			initial_data,
			delimited_list ) == -1 ) )
	{
		return (char *)0;
	}

	list_offset_seek( delimited_list, offset );

	data = list_get( delimited_list );

	list_set_current( delimited_list, (char *)0 );
	list_set( delimited_list, "select" );

	if ( list_length( display_list ) )
	{
		list_offset_seek( display_list, offset );

		display = list_get( display_list );
		list_set( display_list, "Select" );
	}

	return
	strdup(
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		element_drop_down_option_value_html(
			data,
			display ) );
}

char *element_drop_down_option_value_html(
			char *data,
			char *display )
{
	static char html[ 256 ];

	if ( display && *display )
	{
		sprintf(html,
			"\t<option value=\"%s\">%s",
			data,
			display );
	}
	else
	{
		sprintf(html,
			"\t<option>%s",
			data );
	}

	return html;
}

char *element_drop_down_close_html(
			boolean output_null_option,
			boolean output_not_null_option,
			boolean output_select_option )
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

	ptr += sprintf( ptr, "</select>" );

	return html;
}

char *element_checkbox_html(
			char *element_name,
			char *prompt_display,
			boolean checked,
			char *action_string,
			int tab_order,
			char *value )
{
	char html[ 1024 ];
	char *ptr = html;

	if ( !element_name
	||   !prompt_display
	||   !value )
	{
		return (char *)0;
	}

	ptr += sprintf(
		ptr,
"<td>%s<input name=\"%s\" type=\"checkbox\" value=\"%s\"",
		prompt_display,
		element_name,
		value );

	if ( action_string && *action_string )
	{
		ptr += sprintf(
			ptr,
			" onClick=\"%s\"",
			action_string );
	}

	if ( tab_order > 0 )
	{
		ptr += sprintf(
			ptr,
			" taborder=%d",
			tab_order );
	}

	if ( checked )
	{
		ptr += sprintf(
			ptr,
			" checked" );
	}

	ptr += sprintf(
		ptr,
		">" );

	return strdup( html );
}

int element_multi_drop_down_size( void )
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

ELEMENT_DROP_DOWN *element_drop_down_new(
			char *drop_down_name,
			char *initial_data,
			LIST *delimited_list,
			boolean no_initial_capital,
			boolean output_null_option,
			boolean output_not_null_option,
			boolean output_select_option,
			int column_span,
			int drop_down_size,
			int tab_order,
			boolean multi_select,
			char *post_change_javascript )
{
	ELEMENT_DROP_DOWN *element_drop_down = element_drop_down_calloc();

	element_drop_down->drop_down_name = drop_down_name;
	element_drop_down->initial_data = initial_data;
	element_drop_down->delimited_list = delimited_list;
	element_drop_down->no_initial_capital = no_initial_capital;
	element_drop_down->output_null_option = output_null_option;
	element_drop_down->output_not_null_option = output_not_null_option;
	element_drop_down->output_select_option = output_select_option;
	element_drop_down->column_span = column_span;
	element_drop_down->drop_down_size = drop_down_size;
	element_drop_down->tab_order = tab_order;
	element_drop_down->multi_select = multi_select;
	element_drop_down->post_change_javascript = post_change_javascript;

	element_drop_down->html =
		element_drop_down_html(
			drop_down_name,
			initial_data,
			delimited_list,
			element_drop_down_display_list(
				delimited_list,
				no_initial_capital ),
			output_null_option,
			output_not_null_option,
			output_select_option,
			column_span,
			drop_down_size,
			multi_select,
			post_change_javascript,
			form_table_row_background_color(),
			tab_order );

	return element_drop_down;
}

char *element_multi_drop_down_name(
			LIST *attribute_name_list,
			char *element_name_prefix )
{
	char drop_down_name[ 256 ];

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
		strcpy(	drop_down_name,
			list_display_delimited(
				attribute_name_list,
				SQL_DELIMITER ) );
	}
	else
	{
		sprintf(drop_down_name,
			"%s%s",
			element_name_prefix,	
			list_display_delimited(
				attribute_name_list,
				SQL_DELIMITER ) );
	}

	return strdup( drop_down_name );
}

ELEMENT_MULTI_DROP_DOWN *element_multi_drop_down_new(
			LIST *attribute_name_list,
			LIST *delimited_list,
			boolean no_initial_capital,
			char *post_change_javascript )
{
	ELEMENT_MULTI_DROP_DOWN *element_multi_drop_down;

	if ( !list_length( attribute_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	element_multi_drop_down = element_multi_drop_down_calloc();

	element_multi_drop_down->original_drop_down =
		element_drop_down_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			element_multi_drop_down_name(
				attribute_name_list,
				ELEMENT_MULTI_DROP_DOWN_ORIGINAL_PREFIX ),
			(char *)0 /* initial_data */,
			delimited_list,
			no_initial_capital,
			0 /* not output_null_option */,
			0 /* not output_not_null_option */,
			0 /* not output_select_option */,
			1 /* column_span */,
			element_multi_drop_down_size(),
			0 /* tab_order */,
			1 /* multi_select */,
			post_change_javascript );

	element_multi_drop_down->move_right_button =
		element_button_new(
			element_multi_drop_down_move_right_label(),
			element_multi_drop_down_move_right_action_string(
				attribute_name_list ),
			1 /* with_table_data_tag */ );

	element_multi_drop_down->element_break_tag = element_break_tag_new();

	element_multi_drop_down->move_left_button =
		element_button_new(
			element_multi_drop_down_move_left_label(),
			element_multi_drop_down_move_left_action_string(
				attribute_name_list ),
			0 /* not with_table_data_tag */ );

	element_multi_drop_down->empty_html =
		element_drop_down_empty_html(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			element_multi_drop_down_name(
				attribute_name_list,
				(char *)0 /* element_name_prefix */ ),
			element_multi_drop_down_size(),
			1 /* multi_select */ );

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
			char *action_string,
			boolean with_table_data_tag )
{
	ELEMENT_BUTTON *element_button = element_button_calloc();

	element_button->label = label;
	element_button->action_string = action_string;

	element_button->html =
		element_button_html(
			label,
			action_string,
			with_table_data_tag );

	return element_button;
}

char *element_button_html(
			char *label,
			char *action_string,
			boolean with_table_data_tag )
{
	char html[ 1024 ];
	char *ptr = html;

	if ( !label || !action_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: input parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( with_table_data_tag )
	{
		ptr += sprintf( ptr, "<td>" );
	}

	ptr += sprintf(
		ptr,
		"<input type=button value=\"%s\" onclick=\"%s\">\n",
		label,
		action_string );

	return strdup( html );
}

char *element_drop_down_empty_html(
			char *drop_down_name,
			int drop_down_size,
			boolean multi_select )
{
	char html[ 1024 ];
	char *ptr = html;

	if ( !drop_down_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: drop_down_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"<td><select name=\"%s\" size=%d",
		drop_down_name,
		(drop_down_size) ? drop_down_size : 1 );

	if ( multi_select ) ptr += sprintf( ptr, " multiple" );

	ptr += sprintf( ptr, ">\n" );

	return strdup( html );
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
			LIST *attribute_name_list )
{
	char action_string[ 1024 ];

	if ( !list_length( attribute_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(action_string,
		"post_change_multi_select_move_right('%s','%s','%c');",
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		element_multi_drop_down_name(
			attribute_name_list,
			ELEMENT_MULTI_DROP_DOWN_ORIGINAL_PREFIX ),
		element_multi_drop_down_name(
			attribute_name_list,
			(char *)0 /* element_name_prefix */ ),
		ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER );

	return strdup( action_string );
}

char *element_multi_drop_down_move_left_action_string(
			LIST *attribute_name_list )
{
	char action_string[ 1024 ];

	if ( !list_length( attribute_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(action_string,
		"post_change_multi_select_move_left('%s','%s','%c');",
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		element_multi_drop_down_name(
			attribute_name_list,
			ELEMENT_MULTI_DROP_DOWN_ORIGINAL_PREFIX ),
		element_multi_drop_down_name(
			attribute_name_list,
			(char *)0 /* element_name_prefix */ ),
		ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER );

	return strdup( action_string );
}

ELEMENT_BREAK_TAG *element_break_tag_calloc( void )
{
	ELEMENT_BREAK_TAG *element_break_tag;

	if ( ! ( element_break_tag =
			calloc( 1, sizeof( ELEMENT_BREAK_TAG ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return element_break_tag;
}

ELEMENT_BREAK_TAG *element_break_tag_new( void )
{
	ELEMENT_BREAK_TAG *element_break_tag = element_break_tag_calloc();

	element_break_tag->html = element_break_tag_html();

	return element_break_tag;
}

char *element_break_tag_html( void )
{
	return "<br>";
}

char *appaserver_element_html(
			APPASERVER_ELEMENT *appaserver_element )
{
	if ( appaserver_element->element_type == table_row )
		return appaserver_element->table_row->html;
	else
	if ( appaserver_element->element_type == checkbox )
		return appaserver_element->checkbox->html;
	else
	if ( appaserver_element->element_type == drop_down )
		return appaserver_element->drop_down->html;
	else
	if ( appaserver_element->element_type == multi_drop_down )
	{
		if ( appaserver_element->multi_drop_down->empty_html )
			return appaserver_element->multi_drop_down->empty_html;
		else
			return appaserver_element->multi_drop_down->html;
	}
	else
	if ( appaserver_element->element_type == button )
		return appaserver_element->button->html;
	else
		return (char *)0;
}

char *appaserver_element_list_html(
			LIST *appaserver_element_list )
{
	APPASERVER_ELEMENT *appaserver_element;
	char html[ STRING_FOUR_MEG ];
	char *ptr = html;
	char *element_html;

	if ( !list_rewind( appaserver_element_list ) ) return (char *)0;

	do {
		appaserver_element = list_get( appaserver_element_list );

		/* ---------------------------------------- */
		/* Returns heap, static, or program memory, */
		/* or it returns null			    */
		/* ---------------------------------------- */
		if ( ( element_html =
				appaserver_element_html(
					appaserver_element ) ) )
		{
			ptr += sprintf(
				ptr,
				"%s\n", 
				element_html );
		}

	} while ( list_next( appaserver_element_list ) );

	return strdup( html );
}

