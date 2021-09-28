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
#include "attribute.h"
#include "session.h"
#include "related_folder.h"
#include "folder.h"
#include "query.h"
#include "date.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "piece.h"
#include "timlib.h"
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
	if ( element_type == prompt )
		element->prompt =
			element_prompt_calloc();
	else
	if ( element_type == checkbox )
		element->checkbox =
			element_checkbox_calloc();
	else
	if ( element_type == drop_down )
		element->drop_down =
			element_drop_down_calloc();
#ifdef NOT_DEFINED
	else
	if ( element_type == non_edit_multi_select )
		i->non_edit_multi_select = element_non_edit_multi_select_new();
	else
	if ( element_type == toggle_button )
		i->toggle_button = element_toggle_button_new();
	else
	if ( element_type == radio_button )
		i->radio_button = element_radio_button_new();
	else
	if ( element_type == notepad )
		i->notepad = element_notepad_new();
	else
	if ( element_type == text_item
	||   element_type == element_date
	||   element_type == element_time
	||   element_type == element_current_date
	||   element_type == element_current_time
	||   element_type == element_current_date_time
	||   element_type == element_date_time )
	{
		i->text_item = element_text_item_new();
	}
	else
	if ( element_type == password )
		i->password = element_password_new();
	else
	if ( element_type == prompt_data
	||   element_type == prompt_data_plus_hidden )
	{
		i->prompt_data = element_prompt_data_new();
	}
	else
	if ( element_type == reference_number )
		i->reference_number = element_reference_number_new();
	else
	if ( element_type == hidden )
		i->hidden = element_hidden_new();
	else
	if ( element_type == upload_filename )
		i->upload_filename = element_upload_filename_new();
	else
	if ( element_type == empty_column )
		i->empty_column = element_empty_column_new();
	else
	if ( element_type == http_filename )
		i->http_filename = element_http_filename_new();
	else
	if ( element_type == anchor )
		i->anchor = element_anchor_new();
	else
	if ( element_type == blank )
	{
		/* do nothing */
	}
	else
	if ( element_type == table_opening )
	{
		/* do nothing */
	}
	else
	if ( element_type == table_row )
	{
		/* do nothing */
	}
	else
	if ( element_type == table_closing )
	{
		/* do nothing */
	}
	else
	if ( element_type == prompt )
	{
		/* do nothing */
	}
	else
	if ( element_type == prompt_heading )
	{
		/* do nothing */
	}
	else
	if ( element_type == javascript_filename )
	{
		/* do nothing */
	}
	else
	if ( element_type == non_edit_text )
	{
		i->non_edit_text = element_new_non_edit_text();
		i->non_edit_text->text = element_name;
	}
#endif
	else
	{
		fprintf( stderr, 
			 "Error: %s/%s() got invalid element type.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	return i;

}

char *element_password_html(
			char *element_name,
			char *data,
			int attribute_width,
			int row,
			int tab_index )
{
	char html[ 1024 ];
	char *ptr;
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

	if ( tab_index )
	{
		ptr += sprintf(
			ptr,
			" tabindex=%d",
			tab_index );
	}

	ptr += sprintf( ptrt, ">\n" );

	/* Output the password compare field */
	/* --------------------------------- */
	ptr += sprintf( ptr, "<tr><td>Retype\n" );

	ptr += sprintf( ptr,
"<td><input name=\"%s_compare_%d\" type=\"password\" size=\"%d\"",
		element_name, row, size );

	ptr += sprintf( ptre, " maxlength=\"%d\"", maxlength );

	ptr += sprintf( ptr,
		 " onChange=\"password_compare('%s_%d','%s_compare_%d')\"",
		 element_name,
		 row,
		 element_name,
		 row );

	ptr += sprintf( ptr, " Autocomplete=Off" );

	if ( tab_index )
	{
		ptr += sprintf(
			ptr,
			" tabindex=%d",
			tab_index + 1 );
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
			char *element_drop_down_name,
			char *initial_data,
			LIST *delimited_list,
			boolean no_initial_capital,
			LIST *display_list,
			boolean output_null_option,
			boolean output_not_null_option,
			boolean output_select_option,
			int column_span,
			int drop_down_size,
			char *post_change_javascript,
			char *background_color,
			int tab_index )
{
	char html[ STRING_TWO_MEG ];
	char *ptr = html;

	column_span = (column_span) ? column_span : 1;

	ptr += sprintf(
		ptr,
		"<td align=left colspan=%d>\n"
		"<select name=\"%s\" id=\"%s\"",
		column_span,
		element_drop_down_name,
		element_drop_down_name );

	fprintf(	output_file,
			" size=\"%d\"",
			( multi_select ) ? ELEMENT_MULTI_SELECT_ROW_COUNT : 1 );

	if ( multi_select ) fprintf( output_file, " multiple" );

	if ( tab_index )
	{
		fprintf( output_file,
			 " tabindex=%d",
			 tab_index );
	}

	if ( post_change_javascript
	&&   *post_change_javascript )
	{
		fprintf(output_file, " onchange=\"%s\"",
			element_replace_javascript_variables(
				buffer,
				post_change_javascript,
				row,
				state ) );
	}

	if ( onblur_javascript_function
	&&   *onblur_javascript_function )
	{
		fprintf(output_file,
			" onblur=\"%s\"",
			element_replace_javascript_variables(
				buffer,
				onblur_javascript_function,
				row,
				state ) );
	}

	fprintf( output_file, ">" );

	if ( !multi_select && initial_data && *initial_data )
	{
		if ( ( *(initial_data +
			strlen( initial_data ) - 1 ) ==
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER )
		||	strcmp( initial_data, " | " ) == 0 )
		{
			strcpy( initial_label, "Select" );
		}
		else
		{
			element_drop_down_initial_label(
				initial_label,
				initial_data,
				no_initial_capital,
				option_data_list,
				option_label_list );
		}
	}
	else
	{
		initial_data = "select";
		strcpy( initial_label, "Select" );
	}

	if ( strcmp( initial_label, "select" ) == 0 )
		*initial_label = toupper( *initial_label );

	if ( element_appaserver_name_boolean( element_name ) )
	{
		if ( strcmp( initial_label, "y" ) == 0 )
			strcpy( initial_label, "Yes" );
		if ( strcmp( initial_label, "n" ) == 0 )
			strcpy( initial_label, "No" );
	}

	if ( !multi_select )
	{
		char *local_initial_label;

		if ( ! ( seeked_initial_data =
				element_seek_initial_data(
					&seeked_initial_label,
					initial_data,
					option_data_list,
					option_label_list ) ) )
		{
			seeked_initial_data = initial_data;
			seeked_initial_label = initial_label;
		}

		local_initial_label =
			element_data2label(
				buffer,
			 	element_delimit_drop_down_data(
					delimited_label_buffer,
					seeked_initial_label,
					date_piece_offset ),
				no_initial_capital );

		/* Set the first option */
		/* -------------------- */
		fprintf( output_file, "<option " );

		fprintf( output_file,
			 "value=\"%s\">%s\n",
			 seeked_initial_data,
			 local_initial_label );
	}

	list_rewind( option_label_list );

	if ( list_rewind( option_data_list ) )
	{
		do {
			data = list_get( option_data_list );

			/* Skip the preselected item. */
			/* -------------------------- */
			if ( timlib_strcmp(
					data,
					seeked_initial_data ) == 0 )
			{
				list_next( option_label_list );
				continue;
			}

			if ( list_length( option_label_list )
			&&   !list_past_end( option_label_list ) )
			{
				label = list_get( option_label_list );
			}
			else
			{
				label = list_get( option_data_list );
			}

			if ( element_appaserver_name_boolean( element_name ) )
			{
				if ( strcmp(
					data,
					"y" ) == 0 )
				{
					label = "Yes";
				}
				if ( strcmp(
					data,
					"n" ) == 0 )
				{
					label = "No";
				}
			}

			fprintf( output_file,
			"\t<option value=\"%s\">%s\n", 
				data,
			 	element_data2label(
					buffer,
			 		element_delimit_drop_down_data(
						delimited_label_buffer,
						label,
						date_piece_offset ),
					no_initial_capital ) );

			if ( list_length( option_label_list ) )
				list_next( option_label_list );

		} while( list_next( option_data_list ) );

		if ( output_null_option )
		{
			fprintf(output_file,
				"\t<option value=\"%s\">%s\n",
				NULL_OPERATOR,
				format_initial_capital( buffer,
						        NULL_OPERATOR ) );

		}

		if ( output_not_null_option )
		{
			fprintf(output_file,
				"\t<option value=\"%s\">%s\n",
				NOT_NULL_OPERATOR,
				format_initial_capital( buffer,
						        NOT_NULL_OPERATOR ) );
		}

		if ( output_select_option
		&&   strcmp( initial_data, "select" ) != 0 )
		{
			fprintf(output_file,
				"\t<option value=\"\">Select\n" );
		}
	}

	/* fprintf( output_file, "</select></td>\n" ); */
	fprintf( output_file, "</select>\n" );

	/* ----------------------------------------------- */
	/* Create the multi_select second drop down 	   */
	/* ----------------------------------------------- */
	if ( multi_select )
	{
		fprintf(output_file,
			"<td align=left><table border=1><tr>\n" );

		fprintf(output_file,
			"<td>"
			"<input type=button name=\"add_%s\" value=\"%s\""
			" onclick="
		"\"post_change_multi_select_move_right('%s','%s','%c');",
			drop_down_element_name,
			ELEMENT_MULTI_SELECT_ADD_LABEL,
			drop_down_element_name,
			multi_select_drop_down_element_name,
			ELEMENT_MULTI_SELECT_MOVE_LEFT_RIGHT_INDEX_DELIMITER );

		if ( post_change_javascript
		&&   *post_change_javascript )
		{
			fprintf(output_file,
				"%s",
				element_replace_javascript_variables(
					buffer,
					post_change_javascript,
					row,
				(char *)0 /* state */ ) );
		}

		fprintf(output_file,
			"\"><br>\n" );

		fprintf(output_file,
			"<input type=button name=\"remove_%s\" value=\"%s\""
			" onclick="
			"\"post_change_multi_select_move_left('%s','%s','%c');",
			drop_down_element_name,
			ELEMENT_MULTI_SELECT_REMOVE_LABEL,
			drop_down_element_name,
			multi_select_drop_down_element_name,
			ELEMENT_MULTI_SELECT_MOVE_LEFT_RIGHT_INDEX_DELIMITER );

		if ( post_change_javascript
		&&   *post_change_javascript )
		{
			fprintf(output_file,
				"%s",
				element_replace_javascript_variables(
					buffer,
					post_change_javascript,
					row,
				(char *)0 /* state */ ) );
		}

		fprintf(output_file,
			"\">\n" );

		fprintf(output_file,
			"<td><select name=\"%s\""
			" multiple size=%d",
			multi_select_drop_down_element_name,
			ELEMENT_MULTI_SELECT_ROW_COUNT );

		if ( post_change_javascript
		&&   *post_change_javascript )
		{
			fprintf(output_file,
				" onchange=\"%s\"",
				element_replace_javascript_variables(
					buffer,
					post_change_javascript,
					row,
				(char *)0 /* state */ ) );
		}

		fprintf(output_file,
			"></table>\n" );
	}
}

char *element_table_row_html( void )
{
	return "<tr>" );
}

APPASERVER_ELEMENT *element_yes_no(
			char *element_name,
			char *post_change_javascript,
			boolean with_is_null,
			boolean with_not_null )
{
	APPASERVER_ELEMENT *element;
	LIST *option_data_list;

	option_data_list = list_new();

	list_set(
		option_data_list,
		"y" );

	list_set(
		option_data_list,
		"n" );

	if ( with_is_null )
	{
		list_set(
			option_data_list,
			NULL_OPERATOR );
	}

	if ( with_not_null )
	{
		list_set(
			option_data_list,
			NOT_NULL_OPERATOR );
	}

	element =
		appaserver_element_new(
			drop_down );

	element->drop_down->element_name = element_name;
	element->drop_down->option_data_list = option_data_list;

	if ( post_change_javascript
	&&   *post_change_javascript )
	{
		element->drop_down->post_change_javascript =
			post_change_javascript;
	}

	return element;
}

char *appaserver_element_post_change_javascript(
			char *source,
			int row,
			char *state )
{
	char post_change_javascript[ 1024 ];
	char row_string[ 8 ];

	if ( !source || !*source ) return strdup( "" );

	strcpy( post_change_javascript, source );
	sprintf( row_string, "%d", row );

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

char *element_carrot_replace(
			char *data )
{
	static char replace[ 2048 ];

	if ( !character_exists( data, '^' ) ) return data;

	strcpy( replace, data );

	return string_search_replace(
			replace,
			"^",
			ELEMENT_MULTI_ATTRIBUTE_DISPLAY_DELIMITER );
}

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

ELEMENT_PROMPT *element_prompt_calloc( void )
{
	ELEMENT_PROMPT *prompt;

	if ( ! ( prompt = calloc( 1, sizeof( ELEMENT_PROMPT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return prompt;
}

char *element_prompt_html( char *prompt_string )
{
	char html[ 128 ];

	if ( !prompt_string || !*prompt_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: prompt_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"<td align=left>%s",
		prompt_string );

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
			LIST *foreign_key_list,
			int row )
{
	char drop_down_name[ 1024 ];

	if ( !list_length( foreign_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(drop_down_name,
		%s_%d",
		list_display_delimited(
			foreign_key_list,
			ELEMENT_DROP_DOWN_DELIMITER ) );

	return strdup( drop_down_name );
}

int appaserver_element_tab_index( int tab_index )
{
	if ( tax_index == -1 ) return -1;

	return tab_index + 1;
}

