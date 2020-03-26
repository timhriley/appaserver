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
#include "attribute.h"
#include "session.h"
#include "element.h"
#include "related_folder.h"
#include "folder.h"
#include "query.h"
#include "date.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "piece.h"
#include "timlib.h"
#include "process_parameter_list.h"
#include "basename.h"

ELEMENT_NON_EDIT_TEXT *element_new_non_edit_text( void )
{
	return (ELEMENT_NON_EDIT_TEXT *)
			calloc( 1, sizeof( ELEMENT_NON_EDIT_TEXT ) );
}

ELEMENT *element_new_element( void )
{
	return (ELEMENT *)calloc( 1, sizeof( ELEMENT ) );
}

ELEMENT *element_new(	enum element_type element_type,
			char *name )
{
	ELEMENT *i;

	i = element_new_element();

	i->element_type = element_type;
	i->name = name;
	i->remember_keystrokes_ok = 1;

	if ( element_type == drop_down )
		i->drop_down = element_drop_down_new();
	else
	if ( element_type == non_edit_multi_select )
		i->non_edit_multi_select = element_non_edit_multi_select_new();
	else
	if ( element_type == toggle_button )
		i->toggle_button = element_toggle_button_new();
	else
	if ( element_type == push_button )
		i->push_button = element_push_button_new();
	else
	if ( element_type == radio_button )
		i->radio_button = element_radio_button_new();
	else
	if ( element_type == notepad )
		i->notepad = element_notepad_new();
	else
	if ( element_type == linebreak )
		i->linebreak = element_linebreak_new();
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
		i->non_edit_text->text = name;
	}
	else
	{
		fprintf( stderr, 
			 "Error: %s/%s() got invalid element type.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	return i;

} /* element_new() */

void element_simple_output(	ELEMENT *element,
				int row )
{
	element_output(
			(DICTIONARY *)0 /* hidden_name_dictionary */,
			element,
			row,
			1,
			stdout,
			(char *)0 /* background_color */,
			(char *)0 /* application_name */,
			(char *)0 /* login_name */ );
}

void element_output_as_dictionary(
			FILE *output_file,
			ELEMENT *element, 
			int row )
{
	if ( element->element_type == notepad )
	{
		element_notepad_output_as_dictionary(
				output_file,
				element->name,
				element->notepad->data,
				row );
	}
	else
	if ( element->element_type == text_item )
	{
		element_text_item_output_as_dictionary(
			output_file,
			element->name,
			element->text_item->data,
			row );
	}
	else
	if ( element->element_type == password )
	{
		element_password_output_as_dictionary(
				output_file,
				element->name,
				element->password->data,
				row );
	}
	else
	if ( element->element_type == drop_down )
	{
		element_drop_down_output_as_dictionary(
				output_file,
				element->name,
				row,
				element->drop_down->initial_data,
				element->drop_down->folder_name,
				list_length(
					element->drop_down->option_data_list ),
				element->drop_down->max_drop_down_size
				);
	}
	else
	if ( element->element_type == reference_number )
	{
		element_reference_number_output_as_dictionary(
					output_file,
					element->name,
					element->reference_number->data,
					row );
	}
	else
	if ( element->element_type == hidden )
	{
		element_hidden_output(	output_file,
					element->name,
					element->hidden->data,
					row );
	}
	else
	if ( element->element_type == prompt_data
	||   element->element_type == prompt_data_plus_hidden )
	{
		element_hidden_output(	output_file,
					element->name,
					element->prompt_data->data,
					row );
	}
	if ( element->element_type == http_filename
	&&   element->http_filename->data )
	{
		element_text_item_output_as_dictionary(
			output_file,
			element->name,
			element->http_filename->data,
			row );
	}
	else
	if ( element->text_item )
	{
		element_text_item_output_as_dictionary(
			output_file,
			element->name,
			element->text_item->data,
			row );
	}
} /* element_output_as_dictionary() */

void element_output( 	DICTIONARY *hidden_name_dictionary,
			ELEMENT *element, 
			int row,
			int with_toggle_buttons,
			FILE *output_file,
			char *background_color,
			char *application_name,
			char *login_name )
{
	if ( element->element_type == linebreak )
	{
		element_linebreak_output( output_file );
	}
	else
	if ( element->element_type == notepad )
	{
		element_notepad_output(
			 	output_file,
				element->notepad->attribute_width,
				element->name,
				element->notepad->data,
				row,
				element->notepad->number_rows,
				element->notepad->onchange_null2slash_yn,
				element->tab_index );
	}
	else
	if ( element->element_type == blank )
	{
		element_blank_data_output( output_file );
	}
	else
	if ( element->element_type == text_item )
	{
		element_text_item_output(
			output_file,
			element->name,
			element->text_item->data,
			element->text_item->attribute_width,
			row,
			element->text_item->onchange_null2slash_yn,
			element->text_item->post_change_javascript,
			element->text_item->on_focus_javascript_function,
			element->text_item->widget_size,
			background_color,
			element->tab_index,
			0 /* not without_td_tags */,
			element->text_item->readonly,
			element->text_item->state,
			element->text_item->is_numeric );
	}
	else
	if ( element->element_type == element_date )
	{
		element_date_output(
			output_file,
			element->name,
			element->text_item->data,
			element->text_item->attribute_width,
			row,
			element->text_item->onchange_null2slash_yn,
			element->text_item->post_change_javascript,
			element->text_item->on_focus_javascript_function,
			element->text_item->widget_size,
			background_color,
			application_name,
			login_name,
			1 /* with_calendar_popup */,
			element->text_item->readonly,
			element->tab_index );
	}
	else
	if ( element->element_type == element_current_date_time
	||   element->element_type == element_current_date )
	{
		boolean with_calendar_popup = 1;

		/* If okay to create the date and if no existing date. */
		/* --------------------------------------------------- */
		if ( !element->text_item->dont_create_current_date
		&&   ( !element->text_item->data
		||     !*element->text_item->data ) )
		{
			DATE_CONVERT *date_convert;
			char *time_string;
			char data[ 64 ];

			date_convert =
				date_convert_new_user_format_date_convert(
				application_name,
				login_name,
				date_get_now_yyyy_mm_dd(
					date_get_utc_offset() ) );

			if ( element->element_type == element_current_date )
			{
				strcpy( data, date_convert->return_date );
			}
			else
			/* --------------------------------- */
			/* Must be element_current_date_time */
			/* --------------------------------- */
			{
				if ( element->text_item->attribute_width == 19 )
					time_string =
					    date_get_now_hh_colon_mm_colon_ss(
						date_get_utc_offset() );
				else
					time_string =
					    date_get_now_hh_colon_mm(
						date_get_utc_offset() );

				sprintf( data,
				 	"%s %s",
				 	date_convert->return_date,
				 	time_string );
			}

			element->text_item->data = strdup( data );
			with_calendar_popup = 0;
		}
		else
		/* -------------------------------------------------- */
		/* The calendar doesn't work with a time in the date. */
		/* -------------------------------------------------- */
		if ( element->element_type == element_current_date_time
		&&   element->text_item->data
		&&   *element->text_item->data )
		{
			with_calendar_popup = 0;
		}

		element_date_output(
			output_file,
			element->name,
			element->text_item->data,
			element->text_item->attribute_width,
			row,
			element->text_item->onchange_null2slash_yn,
			element->text_item->post_change_javascript,
			element->text_item->on_focus_javascript_function,
			element->text_item->widget_size,
			background_color,
			application_name,
			login_name,
			with_calendar_popup,
			element->text_item->readonly,
			element->tab_index );
	}
	else
	if ( element->element_type == element_current_date )
	{
		if ( !element->text_item->data )
		{
			DATE_CONVERT *date_convert;

			date_convert =
				date_convert_new_user_format_date_convert(
				application_name,
				login_name,
				date_get_now_yyyy_mm_dd(
					date_get_utc_offset() ) );

			element->text_item->data = date_convert->return_date;
		}

		element_date_output(
			output_file,
			element->name,
			element->text_item->data,
			element->text_item->attribute_width,
			row,
			element->text_item->onchange_null2slash_yn,
			element->text_item->post_change_javascript,
			element->text_item->on_focus_javascript_function,
			element->text_item->widget_size,
			background_color,
			application_name,
			login_name,
			1 /* with_calendar_popup */,
			element->text_item->readonly,
			element->tab_index );
	}
	else
	if ( element->element_type == element_date_time )
	{
		boolean with_calendar_popup = 1;

		/* The calendar doesn't work with a time in the date. */
		/* -------------------------------------------------- */
		if ( element->text_item->data )
		{
			with_calendar_popup = 0;
		}

		element_date_output(
			output_file,
			element->name,
			element->text_item->data,
			element->text_item->attribute_width,
			row,
			element->text_item->onchange_null2slash_yn,
			element->text_item->post_change_javascript,
			element->text_item->on_focus_javascript_function,
			element->text_item->widget_size,
			background_color,
			application_name,
			login_name,
			with_calendar_popup,
			element->text_item->readonly,
			element->tab_index );
	}
	else
	if ( element->element_type == element_current_time )
	{
		if ( !element->text_item->data )
		{
			if ( element->text_item->attribute_width >= 7 )
				element->text_item->data =
					date_get_now_time_hhmm_colon_ss(
						date_get_utc_offset() );
			else
				element->text_item->data =
					date_get_now_hhmm(
						date_get_utc_offset() );
		}

		element_text_item_output(
			output_file,
			element->name,
			element->text_item->data,
			element->text_item->attribute_width,
			row,
			element->text_item->onchange_null2slash_yn,
			element->text_item->post_change_javascript,
			element->text_item->on_focus_javascript_function,
			element->text_item->widget_size,
			background_color,
			element->tab_index,
			0 /* not without_td_tags */,
			element->text_item->readonly,
			element->text_item->state,
			element->text_item->is_numeric );
	}
	else
	if ( element->element_type == element_time )
	{
		element_text_item_output(
			output_file,
			element->name,
			element->text_item->data,
			element->text_item->attribute_width,
			row,
			element->text_item->onchange_null2slash_yn,
			element->text_item->post_change_javascript,
			element->text_item->on_focus_javascript_function,
			element->text_item->widget_size,
			background_color,
			element->tab_index,
			0 /* not without_td_tags */,
			element->text_item->readonly,
			element->text_item->state,
			element->text_item->is_numeric );
	}
	else
	if ( element->element_type == password )
	{
		element_password_output( 
				output_file,
				element->name,
				element->password->data,
				element->password->attribute_width,
				row,
				element->tab_index );
	}
	else
	if ( element->element_type == toggle_button )
	{
		if ( with_toggle_buttons )
		{
			element_toggle_button_output(
				output_file,
				element->name,
				element->toggle_button->heading,
				element->toggle_button->checked,
				row,
				element->toggle_button->onchange_submit_yn,
				element->toggle_button->form_name,
				element->toggle_button->image_source,
				element->toggle_button->
					onclick_keystrokes_save_string,
				element->toggle_button->onclick_function );
		}
		else
			element_output_non_element( "", output_file );
	}
	else
	if ( element->element_type == push_button )
	{
		element_push_button_output(
			output_file,
			element->push_button->label,
			row,
			element->push_button->onclick_function );
	}
	else
	if ( element->element_type == radio_button )
	{
		element_radio_button_output(
				output_file,
				element->radio_button->onchange_submit_yn,
				element->radio_button->form_name,
				element->radio_button->image_source,
				element->radio_button->value,
				element->radio_button->checked,
				element->radio_button->heading,
				element->name,
				element->radio_button->onclick,
				row,
				element->radio_button->state,
				element->radio_button->post_change_javascript );
	}
	else
	if ( element->element_type == drop_down )
	{
		if ( !list_length( element->drop_down->option_label_list ) )
		{
			element_drop_down_set_option_data_option_label_list(
				&element->drop_down->option_data_list,
				&element->drop_down->option_label_list,
				element->drop_down->option_data_list
					/* source_list */ );
		}

		element_drop_down_output(
			output_file,
			element->name,
			element->drop_down->option_data_list,
			element->drop_down->option_label_list,
			element->drop_down->number_columns,
			element->drop_down->multi_select,
			row,
			element->drop_down->initial_data,
			element->drop_down->output_null_option,
			element->drop_down->output_not_null_option,
			element->drop_down->output_select_option,
			element->drop_down->folder_name,
			element->drop_down->post_change_javascript,
			element->drop_down->max_drop_down_size,
			element->drop_down->multi_select_element_name,
			element->drop_down->onblur_javascript_function,
			background_color,
			element->drop_down->date_piece_offset,
			element->drop_down->no_initial_capital,
			element->drop_down->readonly,
			element->tab_index,
			element->drop_down->state,
			element->drop_down->attribute_width );
	}
	else
	if ( element->element_type == non_edit_multi_select )
	{
		element_non_edit_multi_select_output(
			output_file,
			element->name,
			element->non_edit_multi_select->option_label_list );
	}
	else
	if ( element->element_type == prompt )
	{
		element_prompt_output(	output_file,
					element->name,
					0 /* not with_heading_format */ );
	}
	else
	if ( element->element_type == prompt_heading )
	{
		element_prompt_output(	output_file,
					element->name,
					1 /* with_heading_format */ );
	}
	else
	if ( element->element_type == prompt_data
	||   element->element_type == prompt_data_plus_hidden )
	{
		element_prompt_data_output(
				output_file,
				element->name,
				element->prompt_data->align,
				element->prompt_data->data,
				element->prompt_data->format_initial_capital );

		if ( element->element_type == prompt_data_plus_hidden )
		{
			element_hidden_name_dictionary_output(
					output_file,
					hidden_name_dictionary,
					row,
					element->name,
					element->prompt_data->data );
		}
	}
	else
	if ( element->element_type == reference_number )
	{
		element_reference_number_output( 
				output_file,
				element->name,
				element->reference_number->data,
				row,
				element->reference_number->attribute_width,
				element->reference_number->omit_update );
	}
	else
	if ( element->element_type == hidden )
	{
		element_hidden_name_dictionary_output(
					output_file,
					hidden_name_dictionary,
					row,
					element->name,
					element->hidden->data );
	}
	else
	if ( element->element_type == upload_filename )
	{
		element_upload_filename_output(
				output_file,
				element->upload_filename->attribute_width,
				row,
				element->name );
	}
	else
	if ( element->element_type == javascript_filename )
	{
		element_javascript_filename_output(
				output_file,
				element->name );
	}
	else
	if ( element->element_type == http_filename )
	{
		element_http_filename_output(	
						output_file,
						element->http_filename,
						row,
						background_color,
						element->name,
						application_name );
	}
	else
	if ( element->element_type == anchor )
	{
		element_anchor_output(	output_file,
					element->anchor->prompt,
					element->anchor->href );
	}
	else
	if ( element->element_type == table_opening )
	{
		element_table_opening_output( output_file );
	}
	else
	if ( element->element_type == table_row )
	{
		element_table_row_output( output_file );
	}
	else
	if ( element->element_type == table_closing )
	{
		element_table_closing_output( output_file );
	}
	else
	if ( element->element_type == empty_column )
	{
		element_empty_column_output( output_file );
	}
	else
	if ( element->element_type == non_edit_text )
	{
		element_non_edit_text_output(
					output_file,
					element->non_edit_text->text,
					element->non_edit_text->column_span,
					element->non_edit_text->padding_em );
	}
	else
	{
		char msg[ 1024 ];
		sprintf( msg, 
			 "%s/%s(%s)/%d: unrecognized element type = %d",
			 __FILE__,
			 __FUNCTION__,
			 element->name,
			 __LINE__,
			 element->element_type );
		appaserver_output_error_message(
			application_name, msg, (char *)0 );
	}
} /* element_output() */

void element_output_non_element( char *s, FILE *output_file )
{
	fprintf( output_file, "<td>%s", s );
	fprintf( output_file, "</td>\n" );
}

void element_set_data( ELEMENT *e, char *s )
{
	if ( e->element_type == element_date
	||   e->element_type == element_current_date
	||   e->element_type == element_current_date_time )
	{
		element_text_item_set_data( e->text_item, s );
	}
	else
	if ( e->element_type == text_item )
	{
		element_text_item_set_data( e->text_item, s );
	}
	else
	if ( e->element_type == password )
	{
		element_password_set_data( e->password, s );
	}
	else
	if ( e->element_type == notepad )
	{
		element_notepad_set_data( e->notepad, s );
	}
	else
	if ( e->element_type == drop_down )
	{
		e->drop_down->initial_data = s;
	}
	else
	if ( e->element_type == prompt_data
	||   e->element_type == prompt_data_plus_hidden )
	{
		element_prompt_data_set_data( e->prompt_data, s );
	}
	else
	if ( e->element_type == reference_number )
	{
		element_reference_number_set_data( e->reference_number, s );
	}
	else
	if ( e->element_type == hidden )
	{
		element_hidden_set_data( e->hidden, s );
	}
	else
	if ( e->element_type == http_filename )
	{
		e->http_filename->data = s;
	}
	else
	if ( e->element_type == non_edit_multi_select )
	{
		e->non_edit_multi_select->option_label_list =
			list_string2list(
				s,
				MULTI_ATTRIBUTE_DATA_LABEL_DELIMITER );
	}
} /* element_set_data() */

int element_get_attribute_width( ELEMENT *e )
{
	if ( e->element_type == text_item )
		return e->text_item->attribute_width;
	else
	if ( e->element_type == password )
		return e->password->attribute_width;
	else
	if ( e->element_type == toggle_button )
		return 0;
	else
	if ( e->element_type == radio_button )
		return 0;
	else
	if ( e->element_type == notepad )
		return e->notepad->attribute_width;
	else
	if ( e->element_type == drop_down )
		return 0;
	else
	if ( e->element_type == prompt_data )
		return 0;
	else
	if ( e->element_type == reference_number )
		return e->reference_number->attribute_width;
	else
	if ( e->element_type == hidden )
		return 0;
	else
	if ( e->element_type == upload_filename )
		return e->upload_filename->attribute_width;
	else
	if ( e->element_type == http_filename )
		return 0;
	else
	if ( e->element_type == linebreak )
		return 0;
	else
		return 0;
} /* element_get_attribute_width() */

char *element_get_heading(
			char **toggle_button_set_all_control_string,
			ELEMENT *e,
			int form_number )
{
	if ( e->element_type == text_item
	||   e->element_type == element_date
	||   e->element_type == element_current_date
	||   e->element_type == element_date_time )
	{
		return element_text_item_get_heading(
						e->name,
						e->text_item->heading );
	}
	else
	if ( e->element_type == password )
	{
		return element_password_get_heading(
						e->name,
						e->password->heading );
	}
	else
	if ( e->element_type == toggle_button )
	{
		if ( toggle_button_set_all_control_string )
		{
			static char local_toggle_button_control_string[ 256 ];
			char delete_warning_javascript[ 128 ];

			if ( timlib_begins_string( e->name, "delete" ) )
			{
				strcpy( delete_warning_javascript,
					"timlib_delete_button_warning(); " );
			}
			else
			{
				*delete_warning_javascript = '\0';
			}

			sprintf(local_toggle_button_control_string,
				"%sform_push_button_set_all('%s',%d);",
				delete_warning_javascript,
				e->name,
				form_number );

			*toggle_button_set_all_control_string =
				local_toggle_button_control_string;
		}
		return element_toggle_button_get_heading(
					e->name,
					e->toggle_button->heading );
	}
	else
	if ( e->element_type == radio_button )
	{
		return element_radio_button_get_heading(
					e->radio_button->heading,
					e->name );
	}
	else
	if ( e->element_type == notepad )
		return element_notepad_get_heading( e->notepad );
	else
	if ( e->element_type == drop_down )
		return element_drop_down_get_heading(	e->name,
							e->drop_down->heading );
	else
	if ( e->element_type == non_edit_multi_select )
		return element_non_edit_multi_select_get_heading(
					e->name );
	else
	if ( e->element_type == prompt_data )
		return element_prompt_data_get_heading(
						e->name,
						e->prompt_data->heading );
	else
	if ( e->element_type == reference_number )
		return
		element_reference_number_get_heading(
						e->name,
						e->reference_number->heading );
	else
	if ( e->element_type == hidden )
		return "";
	else
	if ( e->element_type == upload_filename )
		return "";
	else
	if ( e->element_type == javascript_filename )
		return "";
	else
	if ( e->element_type == http_filename )
		return e->name;
	else
	if ( e->element_type == linebreak )
		return "";
	else
		return (char *)0;

} /* element_get_heading() */

/* ELEMENT_NOTEPAD Operations */
/* -------------------------- */
ELEMENT_NOTEPAD *element_notepad_new( void )
{
	ELEMENT_NOTEPAD *e;
	e = (ELEMENT_NOTEPAD *)calloc( 1, sizeof( ELEMENT_NOTEPAD ) );
	e->number_rows = ELEMENT_NOTEPAD_DEFAULT_NUMBER_ROWS;
	return e;
}

void element_notepad_set_data(			ELEMENT_NOTEPAD *e,
						char *s )
{
	e->data = strdup( s );
}

void element_notepad_set_attribute_width(	ELEMENT_NOTEPAD *e,
						int w )
{
	e->attribute_width = w;
}

char *element_drop_down_get_heading(	char *element_name,
					char *heading )
{
	if ( !heading )
		heading = element_name;
	else
		heading = heading;
	return heading;
}

char *element_non_edit_multi_select_get_heading(
					char *element_name )
{
	return element_name;
}

char *element_notepad_get_heading( ELEMENT_NOTEPAD *e )
{
	return e->heading;
}

void element_notepad_output_as_dictionary(
				FILE *output_file,
				char *element_name,
				char *data,
				int row )
{

	if ( !data ) data = "";

	fprintf(output_file,
		"%s_%d%c%s\n",
		element_name,
		row,
		ELEMENT_DICTIONARY_DELIMITER,
		data );

} /* element_notepad_output_as_dictionary() */

void element_notepad_output( 	FILE *output_file,
				int attribute_width,
				char *element_name,
				char *data,
				int row,
				int number_rows,
				char onchange_null2slash_yn,
				int tab_index )
{
	int display_width;

	if ( attribute_width > ELEMENT_MAX_TEXT_WIDTH )
		display_width = ELEMENT_MAX_TEXT_WIDTH;
	else
		display_width = attribute_width;

	if ( !data ) data = "";

	/* If just one row then query screen displays a text item */
	/* ------------------------------------------------------ */
	if ( number_rows == 1 )
	{
		fprintf( output_file,
	"<td><input name=\"%s_%d\" type=\"text\" size=\"%d\" value=\"%s\"",
		element_name, row, display_width, data );

		fprintf( output_file,
			 " maxlength=\"%d\"",
			 attribute_width );

		fprintf( output_file,
			 " onkeyup=\"timlib_prevent_carrot(event,this)\"" );

		if ( onchange_null2slash_yn == 'y' )
		{
			fprintf( output_file,
				 " onChange=\"null2slash(this)\"" );
		}

		if ( tab_index )
		{
			fprintf( output_file,
				 " tabindex=%d",
				 tab_index );
		}

		fprintf( output_file, ">\n" );
	}
	else
	{
		if ( attribute_width >= ELEMENT_LARGE_NOTEPAD_THRESHOLD )
		{
			display_width = ELEMENT_LARGE_NOTEPAD_WIDTH;
			number_rows = ELEMENT_LARGE_NOTEPAD_NUMBER_ROWS;
		}

		fprintf( output_file,
	"<td><textarea name=\"%s_%d\" cols=%d rows=%d",
		element_name, row,
		display_width,
		number_rows );

		if ( onchange_null2slash_yn == 'y' )
		{
			fprintf( output_file,
				 " onChange=\"null2slash(this)\"" );
		}

		fprintf( output_file,
			 " onblur=\"timlib_check_notepad_size(this, %d);\"",
			 attribute_width );

		fprintf( output_file,
		 	 " onkeyup=\"timlib_prevent_carrot(event,this)\"" );

		if ( tab_index )
		{
			fprintf( output_file,
				 " tabindex=%d",
				 tab_index );
		}

		fprintf( output_file,
			 " wrap=%s>%s</textarea>\n",
			 ELEMENT_TEXTAREA_WRAP,
			 data );
	}

	fprintf( output_file, "</td>\n" );

} /* element_notepad_output() */

void element_notepad_set_onchange_null2slash( ELEMENT_NOTEPAD *e )
{
	e->onchange_null2slash_yn = 'y';
}

/* ELEMENT_PUSH_BUTTONS Operations */
/* ------------------------------- */
ELEMENT_PUSH_BUTTON *element_push_button_new( void )
{
	ELEMENT_PUSH_BUTTON *e =
		(ELEMENT_PUSH_BUTTON *)
			calloc( 1,
				sizeof( ELEMENT_PUSH_BUTTON ) );

	if ( !e )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return e;
}

void element_push_button_output( 	FILE *output_file,
					char *element_label,
					int row,
					char *onclick_function )
{
	fprintf( output_file, "<input type=\"button\"" );

	if ( element_label && *element_label )
	{
		fprintf( output_file,
" value=\"%s\"",
	   	element_label );
	}

	if ( onclick_function )
	{
		if ( timlib_exists_string( onclick_function, "$row" ) )
		{
			char buffer[ 128 ];
			char row_string[ 16 ];

			strcpy( buffer, onclick_function );
			sprintf( row_string, "%d", row );
			search_replace_string( buffer, "$row", row_string );

			onclick_function = strdup( buffer );
		}

		fprintf( output_file,
	 	 	" onclick=\"%s\"",
		 	onclick_function );
	}

	fprintf( output_file, ">\n" );

} /* element_push_button_output() */

/* ELEMENT_TOGGLE_BUTTONS Operations */
/* --------------------------------- */
void element_toggle_button_set_heading(		ELEMENT_TOGGLE_BUTTON *e,
						char *heading )
{
	e->heading = strdup( heading );
}

ELEMENT_TOGGLE_BUTTON *element_toggle_button_new( void )
{
	ELEMENT_TOGGLE_BUTTON *e =
		(ELEMENT_TOGGLE_BUTTON *)
			calloc( 1,
				sizeof( ELEMENT_TOGGLE_BUTTON ) );

	if ( !e )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return e;
}

char *element_toggle_button_get_heading( char *element_name, char *heading )
{
	if ( !heading )
		heading = element_name;

	return heading;
}

void element_toggle_button_set_checked( ELEMENT_TOGGLE_BUTTON *e )
{
	e->checked = 1;
}

void element_toggle_button_output( 	FILE *output_file,
					char *element_name,
					char *heading,
					boolean checked,
					int row,
					char onchange_submit_yn,
					char *form_name,
					char *image_source,
					char *onclick_keystrokes_save_string,
					char *onclick_function )
{
	char buffer[ 256 ];

	fprintf( output_file, "<td>" );

	if ( image_source && *image_source )
		fprintf( output_file, "<img src=\"%s\">", image_source );

	fprintf(output_file,
		"%s",
	   	format_initial_capital(
			buffer, 
			element_toggle_button_get_heading(
				element_name,
				heading ) ) );

	fprintf( output_file,
"<input" );

	if ( element_name && *element_name )
	{
		fprintf( output_file,
" name=\"%s_%d\"",
	   	element_name,
	   	row );
	}

	fprintf( output_file,
" type=\"checkbox\" value=\"yes\"" );

	if ( checked ) fprintf( output_file, " checked" );

	if ( onchange_submit_yn == 'y' )
	{
		if ( !form_name )
		{
			fprintf( stderr,
	"ERROR in %s/%s(): onchange_submit_yn = y but form_name is null\n",
				 __FILE__,
				 __FUNCTION__ );
			exit( 1 );
		}

		if ( onclick_keystrokes_save_string
		&&   *onclick_keystrokes_save_string )
		{
			/* Note the string has a trailing "&&" */
			/* ----------------------------------- */
			fprintf( output_file,
			 " onclick=\"%s push_button_submit('%s')\"",
			 onclick_keystrokes_save_string,
			 form_name );
		}
		else
		{
			fprintf( output_file,
			 " onclick=\"push_button_submit('%s')\"",
			 form_name );
		}
	}
	else
	if ( onclick_function )
	{
		fprintf( output_file,
		 	 " onclick=\"%s\"",
			 onclick_function );
	}

	fprintf( output_file, ">" );

	fprintf( output_file, "</td>\n" );

} /* element_toggle_button_output() */

/* ELEMENT_RADIO_BUTTONS Operations */
/* -------------------------------- */
void element_radio_button_set_heading(		ELEMENT_RADIO_BUTTON *e,
						char *heading )
{
	e->heading = strdup( heading );
}

ELEMENT_RADIO_BUTTON *element_radio_button_new( void )
{
	ELEMENT_RADIO_BUTTON *e =
			(ELEMENT_RADIO_BUTTON *)
				calloc( 1,
					sizeof( ELEMENT_RADIO_BUTTON ) );
	return e;
}

char *element_radio_button_get_heading( char *heading, char *name )
{
	if ( heading )
		return heading;
	else
		return name;
}

void element_radio_button_output( 	FILE *output_file,
					char onchange_submit_yn,
					char *form_name,
					char *image_source,
					char *value,
					int checked,
					char *heading,
					char *name,
					char *onclick,
					int row,
					char *state,
					char *post_change_javascript )
{
	char buffer[ 512 ];

	fprintf( output_file, "<td>" );

	if ( image_source && *image_source )
	{
		fprintf( output_file, "<img src=\"%s\">", image_source );
	}

	fprintf( output_file,
		 "<input name=\"%s\" type=\"radio\"",
	   	 name );

	if ( value )
	{
		fprintf( output_file,
			 " value=\"%s\"",
	   		 value );
	}
	else
	{
		fprintf( output_file,
			 " value=\"%s_%d\"",
			 name,
			 row );
	}

	fprintf( output_file,
		 " class=\"%s\"",
		 name );

	if ( checked ) fprintf( output_file, " checked" );

	if ( onchange_submit_yn == 'y' )
	{
		if ( !form_name )
		{
			fprintf( stderr,
	"ERROR in %s/%s(): onchange_submit_yn = y but form_name is null.\n",
				 __FILE__,
				 __FUNCTION__ );
			exit( 1 );
		}

		fprintf( output_file,
			 " onclick=\"radio_button_submit('%s')\"",
			 form_name );
	}
	else
	if ( onclick && *onclick )
	{
		fprintf( output_file,
			 " onclick=\"%s",
			 onclick );

		if ( post_change_javascript
		&&   *post_change_javascript )
		{
			fprintf(output_file,
				" ;%s",
				element_replace_javascript_variables(
					buffer,
					post_change_javascript,
					row,
					state ) );
		}

		fprintf(output_file, "\"" );
	}

	fprintf( output_file,
		 ">%s",
	   	 format_initial_capital(
			buffer, 
			element_radio_button_get_heading( 
				heading,
				name ) ) );

	fprintf( output_file, "</td>\n" );

} /* element_radio_button_output() */

/* ELEMENT_TEXT_ITEM Operations */
/* ---------------------------- */
ELEMENT_TEXT_ITEM *element_text_item_new( void )
{
	ELEMENT_TEXT_ITEM *e = (ELEMENT_TEXT_ITEM *)
					calloc( 1,
						sizeof( ELEMENT_TEXT_ITEM ) );
	return e;
}

void element_text_item_set_default(	ELEMENT_TEXT_ITEM *e,
					char *s )
{
	element_text_item_set_data( e, s );
}

void element_text_item_set_data(		ELEMENT_TEXT_ITEM *e,
						char *s )
{
	e->data = strdup( s );
}

void element_text_item_set_attribute_width(	ELEMENT_TEXT_ITEM *e,
						int w )
{
	e->attribute_width = w;
}

void element_text_item_set_heading(	ELEMENT_TEXT_ITEM *e,
					char *s )
{
	e->heading = s;
}

char *element_text_item_get_heading( char *element_name, char *heading )
{
	if ( heading && *heading )
		return heading;
	else
		return element_name;
}

void element_text_item_output_as_dictionary(
				FILE *output_file,
				char *element_name,
				char *element_data,
				int row )
{
	char *data;

	if ( element_data )
		data = element_data;
	else
		data = "";

	fprintf( output_file,
		 "%s_%d%c%s\n",
		 element_name,
		 row,
		 ELEMENT_DICTIONARY_DELIMITER,
		 dictionary_trim_double_bracked_string( data ) );

} /* element_text_item_output_as_dictionary() */

void element_date_output( 	FILE *output_file,
				char *element_name,
				char *data,
				int attribute_width,
				int row, 
				char onchange_null2slash_yn,
				char *post_change_javascript,
				char *on_focus_javascript_function,
				int widget_size,
				char *background_color,
				char *application_name,
				char *login_name,
				boolean with_calendar_popup,
				boolean readonly,
				int tab_index )
{
	int maxlength;
	char buffer[ 512 ];
	int did_onchange = 0;
	enum date_convert_format date_convert_format;
	char *format_string;
	static int date_trigger_offset = 0;

	date_convert_format =
		date_convert_get_user_date_format(
				application_name,
				login_name );

	maxlength = attribute_width;

	if ( !widget_size ) widget_size = attribute_width;

	if ( widget_size > ELEMENT_MAX_TEXT_WIDTH )
		widget_size = ELEMENT_MAX_TEXT_WIDTH;

	if ( !data ) data = "";

	fprintf( output_file, "<td>" );

	fprintf( output_file,
	"<input name=\"%s_%d\" type=\"text\" size=\"%d\" value=\"%s\"",
		element_name, row, widget_size, data );

	fprintf( output_file, " maxlength=\"%d\"", maxlength );

	fprintf( output_file,
		 " onkeyup=\"timlib_prevent_carrot(event,this)\"" );

	if ( tab_index )
	{
		fprintf( output_file,
			 " tabindex=%d",
			 tab_index );
	}

	if ( background_color && *background_color )
	{
		fprintf(	output_file,
				" style=\"{background:%s}\"",
				background_color );
	}

	if ( readonly )
	{
		fprintf( output_file, " readonly" );
	}

	if ( onchange_null2slash_yn == 'y'
	|| ( post_change_javascript
	&&   *post_change_javascript ) )
	{
		fprintf( output_file, " onChange=\"" );
		did_onchange = 1;
	}

	if ( onchange_null2slash_yn == 'y' )
		fprintf( output_file, "null2slash(this);" );

	if ( post_change_javascript
	&&   *post_change_javascript )
	{
		fprintf(output_file,
			"%s;",
			element_replace_javascript_variables(
				buffer,
				post_change_javascript,
				row,
				(char *)0 /* state */ ) );
	}

	if ( did_onchange ) fprintf( output_file, "\"" );

	if ( on_focus_javascript_function
	&&   *on_focus_javascript_function )
	{
		fprintf(output_file,
			" onFocus=\"%s\"",
			element_replace_javascript_variables(
				buffer,
				on_focus_javascript_function,
				row,
				(char *)0 /* state */ ) );
	}

	if ( !readonly && with_calendar_popup )
	{
		fprintf( output_file, " id=\"%s_%d\">\n", element_name, row );

		fprintf(output_file,
		"<img id=\"trigger_%d\" src=/%s/calendar.gif>\n",
			date_trigger_offset,
			IMAGE_RELATIVE_DIRECTORY );
	
		format_string =
			element_get_date_format_string(
				date_convert_format );

		fprintf( output_file, "<script type=\"text/javascript\">\n" );

		fprintf( output_file,
			 "Calendar.setup( {inputField: \"%s_%d\","
			 "dateFormat: \"%s\","
			 "fdow: 0,"
			 "trigger: \"trigger_%d\","
			 "align: \"M\","
			 "onSelect: function() {"
			 "this.hide() }\n"
			 "});\n"
			 "</script>\n",
			 element_name,
			 row,
			 format_string,
			 date_trigger_offset );

		date_trigger_offset++;
	}
	else
	{
		fprintf( output_file, ">" );
	}

	fprintf( output_file, "</td>\n" );

} /* element_date_output() */

void element_text_item_output( 	FILE *output_file,
				char *element_name,
				char *data,
				int attribute_width,
				int row, 
				char onchange_null2slash_yn,
				char *post_change_javascript,
				char *on_focus_javascript_function,
				int widget_size,
				char *background_color,
				int tab_index,
				boolean without_td_tags,
				boolean readonly,
				char *state,
				boolean is_numeric )
{
	int maxlength;
	char buffer[ 512 ];
	int did_onchange = 0;
	char *type = "text";

	maxlength = attribute_width;

	if ( !widget_size ) widget_size = attribute_width;

	if ( widget_size > ELEMENT_MAX_TEXT_WIDTH )
		widget_size = ELEMENT_MAX_TEXT_WIDTH;

	if ( !data ) data = "";

	if ( is_numeric )
	{
		data = element_place_commas_in_number_string(
				element_name,
				data );
	}

	if ( !without_td_tags ) fprintf( output_file, "<td>" );

	fprintf( output_file,
	"<input name=\"%s_%d\" type=\"%s\" size=\"%d\" value=\"%s\"",
		element_name, row, type, widget_size, data );

	fprintf( output_file, " maxlength=\"%d\"", maxlength );

	if ( maxlength > widget_size )
	{
		fprintf( output_file, " title=\"%s\"", data );
	}

	if ( tab_index )
	{
		fprintf( output_file,
			 " tabindex=%d",
			 tab_index );
	}

	if ( readonly )
	{
		fprintf( output_file, " readonly" );
	}

	fprintf( output_file,
		 " onkeyup=\"timlib_prevent_carrot(event,this)\"" );

	if ( background_color && *background_color )
	{
		fprintf(	output_file,
				" style=\"{background:%s}\"",
				background_color );
	}

	if ( onchange_null2slash_yn == 'y'
	|| ( post_change_javascript
	&&   *post_change_javascript ) )
	{
		fprintf( output_file, " onChange=\"" );
		did_onchange = 1;
	}

	if ( onchange_null2slash_yn == 'y' )
		fprintf( output_file, "null2slash(this);" );

	if ( post_change_javascript
	&&   *post_change_javascript )
	{
		fprintf(output_file,
			"%s;",
			element_replace_javascript_variables(
				buffer,
				post_change_javascript,
				row,
				state ) );
	}

	if ( did_onchange ) fprintf( output_file, "\"" );

	if ( on_focus_javascript_function
	&&   *on_focus_javascript_function )
	{
		fprintf(output_file, " onFocus=\"%s\"",
			element_replace_javascript_variables(
				buffer,
				on_focus_javascript_function,
				row,
				state ) );
	}

	fprintf( output_file, ">" );

	if ( !without_td_tags ) fprintf( output_file, "</td>\n" );

} /* element_text_item_output() */

/* ELEMENT_PASSWORD Operations */
/* ---------------------------- */
ELEMENT_PASSWORD *element_password_new( void )
{
	ELEMENT_PASSWORD *e = (ELEMENT_PASSWORD *)
					calloc( 1, sizeof( ELEMENT_PASSWORD ) );
	return e;
}

void element_password_set_default(	ELEMENT_PASSWORD *e,
					char *s )
{
	element_password_set_data( e, s );
}

void element_password_set_data(		ELEMENT_PASSWORD *e,
					char *s )
{
	e->data = strdup( s );
}

void element_password_set_attribute_width(	ELEMENT_PASSWORD *e,
						int w )
{
	e->attribute_width = w;
}

void element_password_set_heading(	ELEMENT_PASSWORD *e,
					char *s )
{
	e->heading = strdup( s );
}

char *element_password_get_heading( char *element_name, char *heading )
{
	if ( heading && *heading )
		return heading;
	else
		return element_name;
}

void element_password_output_as_dictionary(
				FILE *output_file,
				char *element_name,
				char *data,
				int row )
{
	if ( !data ) data = "";

	fprintf( output_file,
		 "%s_%d%c%s\n",
		 element_name,
		 row,
		 ELEMENT_DICTIONARY_DELIMITER,
		 dictionary_trim_double_bracked_string( data ) );

} /* element_password_output_as_dictionary() */

void element_password_output( 	FILE *output_file,
				char *element_name,
				char *data,
				int attribute_width,
				int row,
				int tab_index )
{
	int maxlength, size;

	maxlength = attribute_width;

	if ( attribute_width > ELEMENT_MAX_TEXT_WIDTH )
		size = ELEMENT_MAX_TEXT_WIDTH;
	else
		size = attribute_width;

	if ( !data ) data = "";

	fprintf( output_file, "<td>\n" );

	fprintf( output_file, "<table border=0>\n" );

	/* Output the password field */
	/* ------------------------- */
	fprintf( output_file, "<tr><td>Type\n" );

	fprintf( output_file,
	"<td><input name=\"%s_%d\" type=\"password\" size=\"%d\" value=\"%s\"",
		element_name, row, size, data );

	fprintf( output_file, " maxlength=\"%d\"", maxlength );

	fprintf( output_file,
		 " onChange=\"null2slash(this)\"" );

	fprintf( output_file, " Autocomplete=Off" );

	if ( tab_index )
	{
		fprintf( output_file,
			 " tabindex=%d",
			 tab_index );
	}

	fprintf( output_file, ">\n" );

	/* Output the password compare field */
	/* --------------------------------- */
	fprintf( output_file, "<tr><td>Retype\n" );

	fprintf( output_file,
"<td><input name=\"%s_compare_%d\" type=\"password\" size=\"%d\"",
		element_name, row, size );

	fprintf( output_file, " maxlength=\"%d\"", maxlength );

	fprintf( output_file,
		 " onChange=\"password_compare('%s_%d','%s_compare_%d')\"",
		 element_name,
		 row,
		 element_name,
		 row );

	fprintf( output_file, " Autocomplete=Off" );

	if ( tab_index )
	{
		fprintf( output_file,
			 " tabindex=%d",
			 tab_index + 1 );
	}

	fprintf( output_file, ">\n" );

	fprintf( output_file, "</table>\n" );
	fprintf( output_file, "</td>\n" );

} /* element_password_output() */

void element_password_erase_data( char *data )
{
	while( *data ) *data++ = '*';
}

/* ELEMENT_REFERENCE_NUMBER Operations */
/* ----------------------------------- */
ELEMENT_REFERENCE_NUMBER *element_reference_number_new( void )
{
	ELEMENT_REFERENCE_NUMBER *e = (ELEMENT_REFERENCE_NUMBER *)
				calloc( 1, sizeof( ELEMENT_REFERENCE_NUMBER ) );
	return e;
}

void element_reference_number_output( 	FILE *output_file,
					char *element_name,
					char *data,
					int row,
					int attribute_width,
					boolean omit_update )
{
	int maxlength, size;

	if ( !data ) data = "";

	if ( omit_update )
	{
		fprintf( output_file, "<td>%s</td>\n", data );
		fprintf( output_file,
			 "<input name=\"%s_%d\"",
			 element_name,
			 row );
		fprintf( output_file,
		" type=\"hidden\" value=\"%s\">\n", data );
	}
	else
	{
		maxlength = attribute_width;
	
		if ( attribute_width > ELEMENT_MAX_TEXT_WIDTH )
			size = ELEMENT_MAX_TEXT_WIDTH;
		else
			size = attribute_width;
	
		fprintf( output_file, "<td>" );
	
		fprintf( output_file,
		"<input name=\"%s_%d\" type=\"text\" size=\"%d\" value=\"%s\"",
			element_name, row, size, data );
	
		fprintf( output_file, " maxlength=\"%d\">", maxlength );
	}

} /* element_reference_number_output() */

void element_reference_number_output_as_dictionary(
					FILE *output_file,
					char *element_name,
					char *data,
					int row )
{
	if ( !data ) data = "";

	fprintf( output_file,
		 "%s_%d%c%s\n",
		 element_name,
		 row,
		 ELEMENT_DICTIONARY_DELIMITER,
		 dictionary_trim_double_bracked_string( data ) );

} /* element_reference_number_output_as_dictionary() */


void element_reference_number_set_data(		ELEMENT_REFERENCE_NUMBER *e,
						char *s )
{
	e->data = strdup( s );
}

void element_reference_number_set_attribute_width(
						ELEMENT_REFERENCE_NUMBER *e,
						int w )
{
	e->attribute_width = w;
}

void element_reference_number_set_heading(	
					ELEMENT_REFERENCE_NUMBER *e,
					char *s )
{
	e->heading = strdup( s );
}

char *element_reference_number_get_heading( char *element_name, char *heading )
{
	if ( heading && *heading )
		return heading;
	else
		return element_name;
}

ELEMENT_DROP_DOWN *element_drop_down_new( void )
{
	ELEMENT_DROP_DOWN *e = (ELEMENT_DROP_DOWN *)
					calloc( 1,
						sizeof( ELEMENT_DROP_DOWN ) );
	if ( !e )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	e->date_piece_offset = -1;
	return e;
}

void element_drop_down_set_option_data_option_label_list(
					LIST **option_data_list,
					LIST **option_label_list,
					LIST *source_list )
{
	LIST *local_option_data_list;
	LIST *local_option_label_list;
	char option_data[ 1024 ];
	char option_label[ 1024 ];
	char *record;

	*option_data_list = local_option_data_list = list_new_list();
	*option_label_list = local_option_label_list = list_new_list();

	if ( !list_rewind( source_list ) ) return;

	do {
		record = list_get_pointer( source_list );

		piece(	option_data,
			MULTI_ATTRIBUTE_DATA_LABEL_DELIMITER,
			record,
			0 );
		list_append_pointer(	local_option_data_list,
					strdup( option_data ) );

		if ( piece(	option_label,
				MULTI_ATTRIBUTE_DATA_LABEL_DELIMITER,
				record,
				1 )
		&&   *option_label )
		{
			list_append_pointer(	local_option_label_list,
						strdup( option_label ) );
		}
		else
		{
			list_append_pointer(	local_option_label_list,
						strdup( option_data ) );
		}

	} while( list_next( source_list ) );

} /* element_drop_down_set_option_data_option_label_list() */

ELEMENT_NON_EDIT_MULTI_SELECT *element_non_edit_multi_select_new( void )
{
	ELEMENT_NON_EDIT_MULTI_SELECT *e =
		(ELEMENT_NON_EDIT_MULTI_SELECT *)
			calloc( 1,
				sizeof( ELEMENT_NON_EDIT_MULTI_SELECT ) );
	if ( !e )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return e;

} /* element_non_edit_multi_select_new() */

void element_non_edit_multi_select_set_option_label_list(
					ELEMENT_NON_EDIT_MULTI_SELECT *e,
					LIST *l )
{
	e->option_label_list = l;
}

void element_prompt_output(	FILE *output_file,
				char *element_name,
				boolean with_heading_format )
{
	char buffer[ 512 ];
	char search_string[ 2 ];

	*search_string = MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER;
	*(search_string + 1) = '\0';

	strcpy( buffer, element_name );
	search_replace_string(
			buffer,
			search_string, 
			"-" );

	fprintf(output_file,
		"<td align=left>" );

	if ( with_heading_format )
		fprintf( output_file, "<h1>" );

	fprintf(output_file,
		"%s",
		format_initial_capital( buffer, buffer ) );

	if ( with_heading_format )
		fprintf( output_file, "</h1>" );

	fprintf(output_file, "\n" );

} /* element_prompt_output() */

void element_drop_down_output_as_dictionary(
				FILE *output_file,
				char *element_name,
				int row,
				char *initial_data,
				char *folder_name,
				int length_option_data_list,
				int max_drop_down_size )
{
	if ( 	max_drop_down_size
	&&      ( length_option_data_list > 
		  max_drop_down_size ) )
	{
		element_hidden_output(
			output_file,
			element_name,
		 	dictionary_trim_double_bracked_string( initial_data ),
			row );
		return;
	}

	if ( initial_data && *initial_data )
	{
		if ( *(initial_data +
			strlen( initial_data ) - 1 ) ==
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER )
		{
			initial_data = "select";
		}

	}
	else
	{
		initial_data = "select";
	}

	if ( folder_name && *folder_name )
	{
		fprintf( output_file,
		 	 "%s.%s_%d%c%s\n",
			 folder_name,
			 element_name,
			 row,
			 ELEMENT_DICTIONARY_DELIMITER,
		 	 dictionary_trim_double_bracked_string( initial_data ));
	}
	else
	{
		fprintf( output_file,
		 	 "%s_%d%c%s\n",
			 element_name,
			 row,
			 ELEMENT_DICTIONARY_DELIMITER,
		 	 dictionary_trim_double_bracked_string( initial_data ));
	}
		 
} /* element_drop_down_output_as_dictionary() */

void element_drop_down_output( 	
				FILE *output_file,
				char *element_name,
				LIST *option_data_list,
				LIST *option_label_list,
				int number_columns,
				boolean multi_select,
				int row,
				char *initial_data,
				boolean output_null_option,
				boolean output_not_null_option,
				boolean output_select_option,
				char *folder_name,
				char *post_change_javascript,
				int max_drop_down_size,
				char *multi_select_element_name,
				char *onblur_javascript_function,
				char *background_color,
				int date_piece_offset,
				boolean no_initial_capital,
				boolean readonly,
				int tab_index,
				char *state,
				int attribute_width )
{
	char buffer[ 1024 ];
	char delimited_label_buffer[ 512 ];
	char initial_label[ 512 ];
	char drop_down_element_name[ 512 ];
	char multi_select_drop_down_element_name[ 512 ];
	char *data;
	char *label;
	char *seeked_initial_data = {0};
	char *seeked_initial_label;

	if ( !option_data_list )
	{
		fprintf( stderr, "ERROR in %s/%s(): null option_data_list.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	if ( multi_select && !multi_select_element_name )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty multi_select_element_name\n",
		 	 __FILE__,
		 	 __FUNCTION__,
		 	 __LINE__ );
		exit( 1 );
	}

	if ( readonly )
	{
		ELEMENT *element;

		element = element_new( text_item, element_name );

		if ( attribute_width )
		{
			element_text_item_set_attribute_width(
				element->text_item, 
				attribute_width );
		}
		else
		{
			element_text_item_set_attribute_width(
				element->text_item, 
				ELEMENT_MAX_TEXT_WIDTH );
		}

		element->text_item->data = initial_data;
		element->text_item->readonly = 1;

		element_text_item_output(
			output_file,
			element->name,
			element->text_item->data,
			element->text_item->attribute_width,
			row,
			element->text_item->onchange_null2slash_yn,
			element->text_item->post_change_javascript,
			element->text_item->on_focus_javascript_function,
			element->text_item->widget_size,
			background_color,
			element->tab_index,
			0 /* not without_td_tags */,
			element->text_item->readonly,
			element->text_item->state,
			element->text_item->is_numeric );

		return;

	} /* if readonly */

	if ( 	max_drop_down_size
	&&      ( list_length( option_data_list ) > 
		  max_drop_down_size ) )
	{
		ELEMENT *element;

		if ( initial_data && *initial_data )
		{
			element_data_list_to_label(
					initial_label,
					initial_data,
					option_data_list,
					option_label_list );
		}
		else
		{
			*initial_label = '\0';
			*initial_data = '\0';
		}

		element = element_non_edit_text_new_element(
					element_name,
					initial_label,
					1 /* column_span */,
					0 /* padding_em */ );

		element_output(
			(DICTIONARY *)0 /* hidden_name_dictionary */,
			element,
			row,
			0,
			output_file,
			background_color,
			(char *)0 /* application_name */,
			(char *)0 /* login_name */ );

		element = element_hidden_new_element(
						element_name,
						initial_data );
		element_output(
			(DICTIONARY *)0 /* hidden_name_dictionary */,
			element,
			row,
			0,
			output_file,
			background_color,
			(char *)0 /* application_name */,
			(char *)0 /* login_name */ );
		return;

	} /* if ( list_length( option_data_list ) > max_drop_down_size ) ) */

	number_columns = (number_columns) ? number_columns : 1;

	/* Have row numbers on prompt screen multi-selects start with 1 */
	/* ------------------------------------------------------------ */
	if ( multi_select && !row ) row = 1;

	if ( folder_name && *folder_name )
	{
		if ( row == -1 )
		{
			sprintf(drop_down_element_name,
				"%s.%s",
				folder_name,
				element_name );
		}
		else
		{
			sprintf(drop_down_element_name,
				"%s.%s_%d",
				folder_name,
				element_name,
				row );
		}

		if ( multi_select && multi_select_element_name )
		{
			sprintf(multi_select_drop_down_element_name,
				"%s.%s_%d",
				folder_name,
				multi_select_element_name,
				row );
		}
	}
	else
	{
		if ( row == -1 )
		{
			strcpy(drop_down_element_name, element_name );
		}
		else
		{
			sprintf(drop_down_element_name,
				"%s_%d",
				element_name,
				row );
		}

		if ( multi_select && multi_select_element_name )
		{
			sprintf(multi_select_drop_down_element_name,
				"%s_%d",
				multi_select_element_name,
				row );
		}
	}

	fprintf(	output_file,
			"<td align=left colspan=%d>\n"
			"<select name=\"%s\" id=\"%s\"",
			number_columns,
			drop_down_element_name,
			drop_down_element_name );

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
			element_drop_down_get_initial_label(
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

	if ( process_parameter_list_element_name_boolean( element_name ) )
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
			data = list_get_string( option_data_list );

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
				label = list_get_pointer( option_label_list );
			}
			else
			{
				label = list_get_pointer( option_data_list );
			}

			if ( process_parameter_list_element_name_boolean(
								element_name ) )
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

} /* element_drop_down_output() */

/* ELEMENT_NON_EDIT_MULTI_SELECT */
/* ----------------------------- */
void element_non_edit_multi_select_output(
				FILE *output_file,
				char *element_name,
				LIST *option_label_list )
{
	char *data;
	char buffer[ 1024 ];
	char delimited_label_buffer[ 512 ];

	fprintf(	output_file,
			"<td align=left colspan=1>\n"
			"<select name=\"%s\"",
			element_name );

	fprintf(	output_file,
			" size=\"%d\"",
			ELEMENT_NON_EDIT_MULTI_SELECT_ROW_COUNT );

	fprintf( output_file, " multiple" );

	fprintf( output_file, " disabled" );

	fprintf( output_file, ">" );

	if ( list_rewind( option_label_list ) )
	{
		do {
			data = list_get_pointer( option_label_list );

			fprintf( output_file,
			"\t<option value=\"%s\">%s\n", 
				data,
			 	element_data2label(
					buffer,
			 		element_delimit_drop_down_data(
						delimited_label_buffer,
						data,
						-1 /* date_piece_offset */ ),
					0 /* no_initial_capital */ ) );

		} while( list_next( option_label_list ) );

	}

	fprintf( output_file, "</select></td>\n" );


} /* element_non_edit_multi_select_output() */

/* ELEMENT_LINEBREAK Operations */
/* ---------------------------- */
ELEMENT_LINEBREAK *element_linebreak_new( void )
{
	ELEMENT_LINEBREAK *e = 
			(ELEMENT_LINEBREAK *)
				calloc( 1, sizeof( ELEMENT_LINEBREAK ) );
	return e;
}

void element_linebreak_output(	FILE *output_file )
{
	fprintf( output_file, "<tr>\n" );
}

/* ELEMENT_PROMPT_DATA Operations */
/* ------------------------------ */
ELEMENT_PROMPT_DATA *element_prompt_data_new( void )
{
	ELEMENT_PROMPT_DATA *e = (ELEMENT_PROMPT_DATA *)
			calloc( 1, sizeof( ELEMENT_PROMPT_DATA ) );

	return e;
} /* element_prompt_data_new() */

void element_prompt_data_set_heading( 	ELEMENT_PROMPT_DATA *e,
					char *heading )
{
	e->heading = strdup( heading );
}

char *element_prompt_data_get_heading( char *element_name, char *heading )
{
	if ( heading )
		return heading;
	else
		return element_name;
}

void element_prompt_data_set_data(		ELEMENT_PROMPT_DATA *e,
						char *s )
{
	e->data = s;
}

void element_prompt_data_output(
				FILE *output_file,
				char *element_name,
				char *align,
				char *data,
				boolean format_initial_capital_variable )
{
	char buffer[ 65536 ];
	char align_string[ 512 ];

	if ( !data ) data = "";

	if ( align )
	{
		sprintf( align_string, "align=%s", align );

		if ( strcmp( align, "right" ) == 0 )

		data = element_place_commas_in_number_string(
				element_name,
				data );
	}
	else
	{
		*align_string = '\0';
	}

	if ( format_initial_capital_variable )
		format_initial_capital( buffer, data );
	else
		strcpy( buffer, data );

	fprintf(output_file,
		"<td %s>%s", 
		align_string,
		element_data_delimiter2label_delimiter(
			search_replace_string(
				buffer,
				"%0D%0A",
				"<br>" ) ) );

	fprintf( output_file, "</td>\n" );

} /* element_prompt_data_output() */

/* ELEMENT_EMPTY_COLUMN Operations */
/* ------------------------------- */
ELEMENT_EMPTY_COLUMN *element_empty_column_new( void )
{
	ELEMENT_EMPTY_COLUMN *i = (ELEMENT_EMPTY_COLUMN *)
		calloc( 1, sizeof( ELEMENT_EMPTY_COLUMN ) );
	return i;
}

void element_empty_column_output( FILE *output_file )
{
	fprintf( output_file, "<td></td>\n" );
}

/* ELEMENT_HIDDEN Operations */
/* ------------------------- */
ELEMENT_HIDDEN *element_hidden_new( void )
{
	ELEMENT_HIDDEN *i = (ELEMENT_HIDDEN *)
		calloc( 1, sizeof( ELEMENT_HIDDEN ) );

	return i;

} /* element_hidden_new() */

void element_hidden_set_data( ELEMENT_HIDDEN *e, char *s )
{
	if ( *s ) e->data = strdup( s );
}

void element_hidden_output(
				FILE *output_file,
				char *name,
				char *data,
				int row )
{
	if ( !data ) data = "";

	if ( row == -1 )
	{
		fprintf( output_file,
			 "%s",
			 name );
	}
	else
	{
		fprintf( output_file,
			 "%s_%d",
			 name,
			 row );
	}

	fprintf( output_file,
		 "%c%s\n",
		 ELEMENT_DICTIONARY_DELIMITER,
		 dictionary_trim_double_bracked_string( data ) );

} /* element_hidden_output() */

void element_hidden_name_dictionary_output(
				FILE *output_file,
				DICTIONARY *hidden_name_dictionary,
				int row,
				char *name,
				char *data )
{
	char key_name[ 512 ];

	/* ------------------------------------------------------------ */
	/* Sometimes an element will be outputted as a dictionary	*/
	/* multiple times. However, this causes post2dictionary		*/
	/* to repeat the dictionary index to account for the		*/
	/* multiple select drop down posts.				*/
	/* ------------------------------------------------------------ */

	if ( dictionary_exists_suffix( name ) )
	{
		strcpy( key_name, name );
	}
	else
	if ( row == -1 )
	{
		strcpy( key_name, name );
	}
	else
	{
		sprintf( key_name, "%s_%d", name, row );
	}

	if ( hidden_name_dictionary )
	{
		if ( dictionary_exists_key( hidden_name_dictionary, key_name ) )
		{
			return;
		}

		dictionary_set_pointer(	hidden_name_dictionary,
					strdup( key_name ),
					"" );
	}

	if ( !data ) data = "";

	fprintf( output_file, "<input name=\"%s\"", key_name );

	fprintf( output_file,
	" type=\"hidden\" value=\"%s\">\n", data );

} /* element_hidden_name_dictionary_output() */

void element_upload_filename_output(	FILE *output_file,
					int attribute_width,
					int row,
					char *name )
{
	int size;
	int maxlength = attribute_width;

	if ( maxlength > ELEMENT_MAX_TEXT_WIDTH )
		size = ELEMENT_MAX_TEXT_WIDTH;
	else
		size = maxlength;

	if ( row == -1 )
	{
		fprintf( output_file,
		"<td><input name=\"%s\"", name );
	}
	else
	{
		fprintf( output_file,
		"<td><input name=\"%s_%d\"", name, row );
	}

	fprintf( output_file,
	" type=\"file\" size=\"%d\"", size );

	fprintf( output_file,
	" accept=\"*\"" );

	fprintf( output_file, " maxlength=\"%d\"", maxlength );
	fprintf( output_file, "></td>\n" );

} /* element_upload_filename_output() */

void element_javascript_filename_output(FILE *output_file,
					char *filename )
{
	fprintf( output_file,
		 "<script type=\"text/javascript\" src=\"%s\"> </script>\n",
		 filename );

} /* element_javascript_filename_output() */

void element_http_filename_output(	FILE *output_file,
					ELEMENT_HTTP_FILENAME *http_filename,
					int row,
					char *background_color,
					char *element_name,
					char *application_name )
{
	char *filename;
	char filename_link[ 512 ];
	static int target_offset = 0;

	if ( !http_filename->data ) http_filename->data = "";

	filename = basename_get_filename( http_filename->data );

	if ( timlib_strncmp( http_filename->data, "http" ) != 0
	&&   *http_filename->data != '/' )
	{
		sprintf(	filename_link,
				"/appaserver/%s/%s",
				application_name,
				http_filename->data );
	}
	else
	{
		strcpy( filename_link, http_filename->data );
		filename = http_filename->data;
	}

	fprintf(output_file,
		"<td><a href=\"%s\" target=%s_%d>%s</a>", 
		filename_link,
		element_name,
		++target_offset,
		filename );
/*
		(*filename) ? filename : http_filename->data );
*/

	if ( http_filename->update_text_item )
	{
		ELEMENT_TEXT_ITEM *element_text_item;

		element_text_item = http_filename->update_text_item;

		fprintf( output_file, "<br>" );

		element_text_item_output(
			output_file,
			element_name,
			http_filename->data,
			element_text_item->attribute_width,
			row,
			element_text_item->onchange_null2slash_yn,
			element_text_item->post_change_javascript,
			element_text_item->on_focus_javascript_function,
			element_text_item->widget_size,
			background_color,
			0 /* tab_index */,
			1 /* without_td_tags */,
			element_text_item->readonly,
			element_text_item->state,
			element_text_item->is_numeric );
	}

	fprintf(output_file, "</td>\n" );

	if ( !http_filename->update_text_item )
	{
		element_hidden_name_dictionary_output(
			output_file,
			(DICTIONARY *)0 /* hidden_name_dictionary */,
			row,
			element_name,
			http_filename->data );
	}

} /* element_http_filename_output() */

void element_anchor_output(		FILE *output_file,
					char *prompt,
					char *href )
{
	char output_string[ 1024 ];
	static int offset = 1;

	format_initial_capital( output_string, prompt );

	fprintf(output_file,
		"<td><a href=\"%s\" target=anchor_%d>%s</a></td>\n", 
		href,
		offset++,
		output_string );
} /* element_anchor_output() */

void element_table_opening_output( FILE *output_file )
{
/*
	fprintf(output_file, "<td></td><td colspan=99><table border=1><tr>\n" );
*/
	fprintf(output_file, "<td colspan=99><table border=1><tr>\n" );
}

void element_table_row_output( FILE *output_file )
{
	fprintf(output_file, "<tr>\n" );
}

void element_table_closing_output( FILE *output_file )
{
	fprintf(output_file, "</table></td>\n" );
}

void element_blank_data_output(	FILE *output_file )
{
	fprintf( output_file, "<td>" );
	fprintf( output_file, "</td>\n" );
}

void element_text_item_set_onchange_null2slash( ELEMENT_TEXT_ITEM *e )
{
	e->onchange_null2slash_yn = 'y';
}

void element_drop_down_set_output_null_option( ELEMENT_DROP_DOWN *e )
{
	e->output_null_option = 1;
}

void element_drop_down_set_output_not_null_option( ELEMENT_DROP_DOWN *e )
{
	e->output_not_null_option = 1;
}

ELEMENT_UPLOAD_FILENAME *element_upload_filename_new( void )
{
	ELEMENT_UPLOAD_FILENAME *i = (ELEMENT_UPLOAD_FILENAME *)
		calloc( 1, sizeof( ELEMENT_UPLOAD_FILENAME ) );

	return i;

} /* element_upload_filename_new() */

ELEMENT_HTTP_FILENAME *element_http_filename_new( void )
{
	ELEMENT_HTTP_FILENAME *i = (ELEMENT_HTTP_FILENAME *)
		calloc( 1, sizeof( ELEMENT_HTTP_FILENAME ) );

	return i;

} /* element_http_filename_new() */

ELEMENT_ANCHOR *element_anchor_new( void )
{
	ELEMENT_ANCHOR *i = (ELEMENT_ANCHOR *)
		calloc( 1, sizeof( ELEMENT_ANCHOR ) );

	return i;

} /* element_anchor_new() */

ELEMENT *element_non_edit_text_new_element(	char *name,
						char *text,
						int column_span,
						int padding_em )
{
	ELEMENT *element;

	element = element_new_element();
	element->element_type = non_edit_text;
	element->name = name;
	element->non_edit_text = element_new_non_edit_text();
	element->non_edit_text->text = text;
	element->non_edit_text->column_span = column_span;
	element->non_edit_text->padding_em = padding_em;
	return element;
} /* element_non_edit_text_new_new_element() */

ELEMENT *element_hidden_new_element(	char *name,
					char *data )
{
	ELEMENT *element;

	element = element_new_element();
	element->element_type = hidden;
	element->name = name;
	element->hidden = element_hidden_new();
	element->hidden->data = data;
	return element;
} /* element_hidden_new_new_element() */

void element_non_edit_text_output(	FILE *output_file,
					char *text,
					int column_span,
					int padding_em )
{
	int local_column_span;
	char output_buffer[ 65536 ];

	strcpy( output_buffer, text );
	search_replace_string(
			output_buffer,
			"%0D%0A",
			"<br>" );

	local_column_span = (column_span) ? column_span : 1;

	fprintf( output_file,
		 "<td colspan=%d",
		 local_column_span );

	if ( padding_em )
	{
		fprintf(	output_file,
				" style=\"padding: 0 %dem 0 %dem;\"",
				padding_em,
				padding_em );
	}

	fprintf( output_file,
		 ">%s</td>\n",
		 element_data_delimiter2label_delimiter( output_buffer ) );

} /* element_non_edit_text_output() */

int element_type_count( LIST *element_list, enum element_type element_type )
{
	ELEMENT *element;
	int count = 0;

	if ( list_rewind( element_list ) )
	{
		do {
			element =
				(ELEMENT *)
					list_get_pointer(
						element_list );
			if ( element->element_type == element_type )
				count++;
		} while( list_next( element_list ) );
	}
	return count;
} /* element_type_count() */

ELEMENT *element_get_yes_no_element(	char *attribute_name,
					char *prepend_folder_name,
					char *post_change_javascript,
					boolean with_is_null,
					boolean with_not_null )
{
	ELEMENT *element;
	char element_name[ 128 ];
	LIST *option_data_list;

	option_data_list = list_new();

	list_append_pointer(	option_data_list,
				"y" );

	list_append_pointer(	option_data_list,
				"n" );

	if ( with_is_null )
	{
		list_append_pointer(	option_data_list,
					NULL_OPERATOR );
	}

	if ( with_not_null )
	{
		list_append_pointer(	option_data_list,
					NOT_NULL_OPERATOR );
	}

	if ( prepend_folder_name && *prepend_folder_name )
	{
		sprintf( element_name,
			 "%s.%s",
			 prepend_folder_name,
			 attribute_name );
	}
	else
	{
		strcpy( element_name, attribute_name );
	}

	element =
		element_new(
			drop_down,
			strdup( element_name ) );

	element->drop_down->option_data_list = option_data_list;

	if ( post_change_javascript
	&&   *post_change_javascript )
	{
		element->drop_down->post_change_javascript =
			post_change_javascript;
	}

	return element;

} /* element_get_yes_no_element() */

char *element_data_delimiter2label_delimiter( char *source_destination )
{
	char multi_attribute_drop_down_delimiter_string[ 2 ];

	sprintf(	multi_attribute_drop_down_delimiter_string,
			"%c",
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );

	search_replace_string(
			source_destination,
			multi_attribute_drop_down_delimiter_string, 
			ELEMENT_MULTI_ATTRIBUTE_DISPLAY_DELIMITER );

	return source_destination;

} /* element_data_delimiter2label_delimiter() */

char *element_data2label(	char *destination,
				char *data,
				boolean no_initial_capital )
{
	if ( !data )
	{
		*destination = '\0';
	}
	else
	{
		if ( no_initial_capital )
		{
			strcpy( destination, data );
			/* search_replace_character( destination, '_', ' ' ); */
		}
		else
		{
			format_initial_capital( destination, data );
		}
	}

	return destination;
} /* element_data2label() */

char *element_shift_first_label( char *data, char delimiter )
{
	char piece_data[ 1024 ];
	char shifted_data[ 1024 ];
	char data_buffer[ 1024 ];

	strcpy( shifted_data, data );
	piece( piece_data, delimiter, data, 0 );
	piece_delete( shifted_data, delimiter, 0 );
	sprintf(	shifted_data + strlen( shifted_data ),
			"%c%s",
			delimiter,
			piece_data );

	format_initial_capital( data_buffer, shifted_data );
	return strdup( data_buffer );

} /* element_shift_first_label() */

int element_exists_delimiter( char *option_data, char delimiter )
{
	while( *option_data )
	{
		if ( *option_data == delimiter ) return 1;
		option_data++;
	}
	return 0;
} /* element_exists_delimiter() */

char *element_replace_javascript_variables(
				char *destination,
				char *source,
				int row,
				char *state )
{
	char row_string[ 8 ];

	sprintf( row_string, "%d", row );
	strcpy( destination, source );
	search_replace_string(
			destination,
			"$row",
			row_string );

	if ( state && *state )
	{
		search_replace_string(
				destination,
				"$state",
				state );
	}

	return destination;
} /* element_replace_javascript_variables() */

char *element_delimit_drop_down_data(	char *destination,
					char *source,
					int date_piece_offset )
{
	char *anchor = destination;
	int piece_count;

	piece_count = 0;
	while( *source )
	{
		if ( *source == MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER )
		{
			destination +=
				sprintf(
				destination,
				"%s",
				ELEMENT_MULTI_ATTRIBUTE_DISPLAY_DELIMITER );

			source++;
			piece_count++;

date_piece_offset = 0; /* stub */

		}
		else
		{
			*destination++ = *source++;
		}
	}
	*destination = '\0';
	return anchor;
} /* element_delimit_drop_down_data() */

ELEMENT *element_get_text_item_variant_element(
				char *attribute_name,
				char *datatype,
				int width,
				char *post_change_javascript,
				char *on_focus_javascript_function )
{
	ELEMENT *element;

	if ( strcmp( datatype, "current_date" ) == 0 )
	{
		element = element_new(
				element_current_date,
				attribute_name );
	}
	else
	if ( strcmp( datatype, "current_time" ) == 0 )
	{
		element = element_new(
				element_current_time,
				attribute_name );
	}
	else
	if ( strcmp( datatype, "current_date_time" ) == 0 )
	{
		element = element_new(
				element_current_date_time,
				attribute_name );
	}
	else
	if ( strcmp( datatype, "date" ) == 0 )
	{
		element = element_new(
				element_date,
				attribute_name );
	}
	else
	if ( strcmp( datatype, "time" ) == 0 )
	{
		element = element_new(
				element_time,
				attribute_name );
	}
	else
	if ( strcmp( datatype, "date_time" ) == 0 )
	{
		element = element_new(
				element_date_time,
				attribute_name );
	}
	else
	{
		element = element_new(
				text_item,
				attribute_name );
	}

	element_text_item_set_attribute_width(
			element->text_item, 
			width );

	if ( element->text_item
	&&   post_change_javascript
	&&   *post_change_javascript )
	{
		element->text_item->post_change_javascript =
			attribute_append_post_change_javascript(
				element->text_item->post_change_javascript,
				post_change_javascript,
				0 /* not place_first */ );
	}

	element->text_item->on_focus_javascript_function =
		on_focus_javascript_function;

	return element;

} /* element_get_text_item_variant_element() */

char *element_list_display( LIST *element_list )
{
	ELEMENT *element;
	char buffer[ 65536 ];
	char *ptr = buffer;

	ptr += sprintf( ptr, "\n" );
	if ( list_rewind( element_list ) )
	{
		do {
			element = list_get_pointer( element_list );
			ptr += sprintf(	ptr,
					"%s\n",
					element_display( element ) );

		} while( list_next( element_list ) );
	}

	return strdup( buffer );

} /* element_list_display() */

char *element_display( ELEMENT *element )
{
	static char buffer[ 1024 ];

	sprintf(	buffer,
		"element name = %s, type = %s, omit_heading_sort_button = %d",
			element->name,
			element_get_type_string(
				element->element_type ),
			element->omit_heading_sort_button );

	if ( element->element_type == prompt_data_plus_hidden )
	{
		sprintf( buffer + strlen( buffer ),
			 ", heading = %s",
			 element->prompt_data->heading );
	}

	if ( element->text_item )
	{
		sprintf( buffer + strlen( buffer ),
			 ", heading = %s, attribute_width = %d",
			 element->text_item->heading,
			 element->text_item->attribute_width );

		if ( element->text_item->readonly )
		{
			sprintf( buffer + strlen( buffer ),
				 ", readonly" );
		}
	}

	if ( element->text_item
	&&   element->text_item->post_change_javascript
	&&   *element->text_item->post_change_javascript )
	{
		sprintf( buffer + strlen( buffer ),
			 ", post_change_javascript = %s",
			 element->text_item->post_change_javascript );
	}

	if ( element->element_type == hidden )
	{
		sprintf( buffer + strlen( buffer ),
			 ", data = %s",
			 (element->hidden->data)
			 	? element->hidden->data : "null" );
	}
	else
	if ( element->element_type == prompt_data )
	{
		sprintf(buffer + strlen( buffer ),
			", data = %s, heading = %s",
			(element->prompt_data->data)
			 	? element->prompt_data->data : "null",
			(element->prompt_data->heading)
				? element->prompt_data->heading : "null" );
	}
	else
	if ( element->element_type == drop_down )
	{
		sprintf(buffer + strlen( buffer ),
			", no_initial_capital = %d, heading = %s",
			element->drop_down->no_initial_capital,
			element->drop_down->heading );

		if ( element->drop_down->readonly )
		{
			sprintf( buffer + strlen( buffer ),
				 ", readonly" );
		}
	}
	else
	if ( element->element_type == push_button )
	{
		sprintf(buffer + strlen( buffer ),
			", label = %s, onclick_function = %s",
			element->push_button->label,
			element->push_button->onclick_function );
	}

	return buffer;

} /* element_display() */

char *element_get_type_string( enum element_type element_type )
{
	return element_get_element_type_string( element_type );
}

char *element_get_destination_multi_select_element_name( LIST *element_list )
{
	ELEMENT *element;

	if ( list_at_end( element_list ) ) return (char *)0;

	do {
		element = list_get_pointer( element_list );

		if ( element->element_type == drop_down
		&&   element->drop_down->multi_select )
		{
			static char name_with_suffix[ 512 ];

			sprintf(name_with_suffix,
				"%s_1",
				element->drop_down->multi_select_element_name );
			list_next( element_list );
			return name_with_suffix;
		}
	} while( list_next( element_list ) );

	return (char *)0;

} /* element_get_destination_multi_select_element_name() */

boolean element_exists_reference_number(LIST *element_list )
{
	ELEMENT *element;

	if ( !list_rewind( element_list ) ) return 0;

	do {
		element = list_get_pointer( element_list );
		if ( element->element_type == reference_number ) return 1;
	} while( list_next( element_list ) );
	return 0;
} /* element_exists_reference_number() */

boolean element_exists_upload_filename( LIST *element_list )
{
	ELEMENT *element;

	if ( !list_rewind( element_list ) ) return 0;

	do {
		element = list_get_pointer( element_list );
		if ( element->element_type == upload_filename ) return 1;
	} while( list_next( element_list ) );
	return 0;
} /* element_exists_upload_filename() */

LIST *element_list2remember_keystrokes_non_multi_element_name_list(
			LIST *element_list )
{
	ELEMENT *element;
	LIST *remember_keystrokes_element_name_list = list_new();
	char element_name[ 512 ];

	if ( !list_rewind( element_list ) )
		return remember_keystrokes_element_name_list;

	do {
		element = list_get_pointer( element_list );

		/* ---------------------------------------------------- */
		/* Remember: can't include upload filenames because some*/
		/* browsers considers them protected elements.		*/
		/* ---------------------------------------------------- */
		if ( element->remember_keystrokes_ok )
		{
			if ( element->element_type == text_item
			||   element->element_type == element_date
			||   element->element_type == element_current_date
			||   element->element_type == element_time
			||   element->element_type == element_current_time
			||   element->element_type == element_date_time
			||   element->element_type == element_current_date_time
			||   ( element->element_type == drop_down
			&&     !element->drop_down->multi_select ) )
			{
				sprintf( element_name, "%s_0", element->name );
	
				list_append_string(
					remember_keystrokes_element_name_list,
					element_name );
			}
		}

	} while( list_next( element_list ) );
	return remember_keystrokes_element_name_list;
} /* element_list2remember_keystrokes_non_multi_element_name_list() */

LIST *element_list2remember_keystrokes_multi_element_name_list(
			LIST *element_list )
{
	ELEMENT *element;
	LIST *multi_element_name_list = list_new();
	char element_name[ 512 ];

	if ( !list_rewind( element_list ) )
		return multi_element_name_list;

	do {
		element = list_get_pointer( element_list );

		/* ---------------------------------------------------- */
		/* Remember: can't include upload filenames because some*/
		/* browsers considers them protected elements.		*/
		/* ---------------------------------------------------- */
		if ( element->remember_keystrokes_ok )
		{
			if ( element->element_type == drop_down
			&&   element->drop_down->multi_select )
			{
				sprintf( element_name, "%s_1", element->name );
	
				list_append_string(
					multi_element_name_list,
					element_name );
			}
		}

	} while( list_next( element_list ) );
	return multi_element_name_list;
} /* element_list2remember_keystrokes_multi_element_name_list() */

char *element_get_element_type_string( enum element_type element_type )
{
	if ( element_type == drop_down )
		return "drop_down";
	else
	if ( element_type == toggle_button )
		return "toggle_button";
	else
	if ( element_type == radio_button )
		return "radio_button";
	else
	if ( element_type == notepad )
		return "notepad";
	else
	if ( element_type == password )
		return "password";
	else
	if ( element_type == non_edit_text )
		return "non_edit_text";
	else
	if ( element_type == non_edit_multi_select )
		return "non_edit_multi_select";
	else
	if ( element_type == linebreak )
		return "linebreak";
	else
	if ( element_type == text_item )
		return "text_item";
	else
	if ( element_type == upload_filename )
		return "upload_filename";
	else
	if ( element_type == prompt_data )
		return "prompt_data";
	else
	if ( element_type == prompt_data_plus_hidden )
		return "prompt_data_plus_hidden";
	else
	if ( element_type == reference_number )
		return "reference_number";
	else
	if ( element_type == hidden )
		return "hidden";
	else
	if ( element_type == empty_column )
		return "empty_column";
	else
	if ( element_type == blank )
		return "blank";
	else
	if ( element_type == element_date_time )
		return "date_time";
	else
	if ( element_type == element_current_date_time )
		return "current_date_time";
	else
	if ( element_type == element_current_date )
		return "current_date";
	else
	if ( element_type == element_current_time )
		return "current_time";
	else
	if ( element_type == element_time )
		return "time";
	else
	if ( element_type == http_filename )
		return "http_filename";
	else
	if ( element_type == timestamp )
		return "timestamp";
	else
	if ( element_type == table_opening )
		return "table_opening";
	else
	if ( element_type == table_row )
		return "table_row";
	else
	if ( element_type == table_closing )
		return "table_closing";
	else
	if ( element_type == prompt )
		return "prompt";
	else
	if ( element_type == prompt_heading )
		return "prompt_heading";
	else
	if ( element_type == element_date )
		return "date";
	else
	if ( element_type == push_button )
		return "push_button";
	else
	{
		char buffer[ 128 ];
		sprintf( buffer, "unknown:%d", element_type );
		return strdup( buffer );
	}
} /* element_get_element_type_string() */

boolean element_combined_option_data_list(
				LIST *option_data_list )
{
	char *option_data;

	if ( !list_length( option_data_list ) ) return 0;
	option_data = list_get_first_pointer( option_data_list );

	if ( exists_character(
			option_data,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ) )
	{
		return ( exists_character(
				option_data,
				MULTI_ATTRIBUTE_DATA_LABEL_DELIMITER ) );
	}
	else
	{
		return 0;
	}

} /* element_combined_option_data_list() */

LIST *element_get_combined_option_label_list(
				LIST *option_data_list )
{
	char option_label[ 128 ];
	char option_data[ 128 ];
	char *option_data_label;
	LIST *option_label_list = list_new();

	if ( !list_rewind( option_data_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty option_data_list\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		option_data_label = list_get_pointer( option_data_list );

		piece(	option_label,
			MULTI_ATTRIBUTE_DATA_LABEL_DELIMITER,
			option_data_label,
			0 );

		if ( !piece(	option_data,
				MULTI_ATTRIBUTE_DATA_LABEL_DELIMITER,
				option_data_label,
				1 ) )
		{
			strcpy( option_data, option_data_label );
		}

		/* free( option_data_label ); */

		list_set_current_pointer(
			option_data_list,
			strdup( option_data ) );

		list_append_pointer(
			option_label_list,
			strdup( option_label ) );

	} while( list_next( option_data_list ) );
	return option_label_list;
} /* element_get_combined_option_label_list() */

void element_drop_down_get_initial_label(
					char *initial_label,
					char *initial_data,
					boolean no_initial_capital,
					LIST *option_data_list,
					LIST *option_label_list )
{
	element_data_list_to_label(	initial_label,
					initial_data,
					option_data_list,
					option_label_list );

	element_data2label(		initial_label,
					initial_label,
					no_initial_capital );

} /* element_drop_down_get_initial_label() */

void element_data_list_to_label(	char *label,
					char *data,
					LIST *option_data_list,
					LIST *option_label_list )
{
	char *option_data;
	char *option_label;

	if ( !list_length( option_data_list )
	||   	list_length( option_data_list ) !=
		list_length( option_label_list ) )
	{
		strcpy( label, data );
		return;
	}
	else
	{
		list_rewind( option_data_list );
		list_rewind( option_label_list );

		do {
			option_data = list_get_pointer( option_data_list );

			if ( timlib_delimiter_independent_strcmp(
					option_data,
					data ) == 0 )
			{
				option_label =
					list_get_pointer( option_label_list );

				strcpy(	label, option_label );
				return;
			}

			list_next( option_label_list );

		} while( list_next( option_data_list ) );
	}

	strcpy( label, data );

} /* element_data_list_to_label() */

void element_list_set_omit_heading_sort_button(
					LIST *element_list,
					LIST *join_1tom_related_folder_list )
{
	RELATED_FOLDER *related_folder;
	ELEMENT *element;

	if ( !list_rewind( element_list ) ) return;
	if ( !list_length( join_1tom_related_folder_list ) ) return;

	do {
		element = list_get_pointer( element_list );

		list_rewind( join_1tom_related_folder_list );

		do {
			related_folder =
				list_get_pointer(
					join_1tom_related_folder_list );

			if ( strcmp(	related_folder->
						one2m_related_folder->
						folder_name,
					element->name ) == 0 )
			{
				element->omit_heading_sort_button = 1;
			}

		} while( list_next( join_1tom_related_folder_list ) );

	} while( list_next( element_list ) );

} /* element_list_set_omit_heading_sort_button() */

char *element_get_date_format_string(
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

} /* element_get_date_format_string() */

void element_list_set_readonly(
			LIST *element_list )
{
	ELEMENT *element;

	if ( !list_rewind( element_list ) ) return;

	do {
		element = list_get_pointer( element_list );

		if ( element->text_item )
		{
			element->text_item->readonly = 1;
		}
		else
		if ( element->drop_down )
		{
			element->drop_down->readonly = 1;
		}

	} while( list_next( element_list ) );

} /* element_list_set_readonly() */

char *element_seek_initial_data(	char **initial_label,
					char *initial_data,
					LIST *option_data_list,
					LIST *option_label_list )
{
	char piece_buffer[ 512 ];
	char *option_data;

	if ( !list_rewind( option_data_list ) ) return (char *)0;

	list_rewind( option_label_list );

	do {
		option_data = list_get_pointer( option_data_list );

		piece( piece_buffer, '[', option_data, 0 );

		if ( timlib_strcmp( piece_buffer, initial_data ) == 0 )
		{
			*initial_label = list_get_pointer( option_label_list );

			if ( strcmp( *initial_label, "y" ) == 0 )
			{
				*initial_label = "Yes";
			}
			else
			if ( strcmp( *initial_label, "n" ) == 0 )
			{
				*initial_label = "No";
			}

			return option_data;
		}

		list_next( option_label_list );

	} while( list_next( option_data_list ) );

	return (char *)0;

} /* element_seek_initial_data() */

char *element_place_commas_in_number_string(
				char *element_name,
				char *data )
{
	if ( timlib_exists_string( element_name, "year" ) )
		return data;
	else
		return place_commas_in_number_string( data );

} /* element_place_commas_in_number_string() */
