/* $APPASERVER_HOME/library/form.c					*/
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
#include "form.h"

char *form_next_reference_number(
			int *form_current_reference_number )
{
	int current_reference_number;
	static char return_reference_number[ 128 ];

	current_reference_number = *form_current_reference_number;

	if ( !current_reference_number ) current_reference_number = 1;
	*form_current_reference_number = current_reference_number + 1;

	sprintf( return_reference_number, 
		 "%d", 
		 current_reference_number );

	return return_reference_number;
}

char **form_table_row_background_color_array(
			int *background_color_array_length,
			char *application_name )
{
	char **background_color_array;
	char *color_from_application_constants;
	APPLICATION_CONSTANTS *application_constants;

	background_color_array =
		(char **)calloc(FORM_MAX_BACKGROUND_COLOR_ARRAY,
				sizeof( char * ) );

	application_constants = application_constants_new();
	application_constants->dictionary =
		application_constants_get_dictionary(
			application_name );

	if ( ( color_from_application_constants =
		application_constants_fetch(
			application_constants->dictionary,
			"color1" ) ) )
	{
		background_color_array[ 0 ] = color_from_application_constants;
		*background_color_array_length = 1;

		if ( ( color_from_application_constants =
			application_constants_fetch(
				application_constants->dictionary,
				"color2" ) ) )
		{
			background_color_array[ 1 ] =
				color_from_application_constants;
			*background_color_array_length = 2;
		}
		else
		{
			background_color_array[ 1 ] = "white";
		}

		if ( ( color_from_application_constants =
			application_constants_fetch(
				application_constants->dictionary,
				"color3" ) ) )
		{
			background_color_array[ 2 ] =
				color_from_application_constants;
			*background_color_array_length = 3;
		}
		else
		{
			background_color_array[ 2 ] = "white";
		}


		if ( ( color_from_application_constants =
			application_constants_fetch(
				application_constants->dictionary,
				"color4" ) ) )
		{
			background_color_array[ 3 ] =
				color_from_application_constants;
			*background_color_array_length = 4;
		}
		else
		{
			background_color_array[ 3 ] = "white";
		}


		if ( ( color_from_application_constants =
			application_constants_fetch(
				application_constants->dictionary,
				"color5" ) ) )
		{
			background_color_array[ 4 ] =
				color_from_application_constants;
			*background_color_array_length = 5;
		}
		else
		{
			background_color_array[ 4 ] = "white";
		}


		if ( ( color_from_application_constants =
			application_constants_fetch(
				application_constants->dictionary,
				"color6" ) ) )
		{
			background_color_array[ 5 ] =
				color_from_application_constants;
			*background_color_array_length = 6;
		}
		else
		{
			background_color_array[ 5 ] = "white";
		}


		if ( ( color_from_application_constants =
			application_constants_fetch(
				application_constants->dictionary,
				"color7" ) ) )
		{
			background_color_array[ 6 ] =
				color_from_application_constants;
			*background_color_array_length = 7;
		}
		else
		{
			background_color_array[ 6 ] = "white";
		}


		if ( ( color_from_application_constants =
			application_constants_fetch(
				application_constants->dictionary,
				"color8" ) ) )
		{
			background_color_array[ 7 ] =
				color_from_application_constants;
			*background_color_array_length = 8;
		}
		else
		{
			background_color_array[ 7 ] = "white";
		}


		if ( ( color_from_application_constants =
			application_constants_fetch(
				application_constants->dictionary,
				"color9" ) ) )
		{
			background_color_array[ 8 ] =
				color_from_application_constants;
			*background_color_array_length = 9;
		}
		else
		{
			background_color_array[ 8 ] = "white";
		}


		if ( ( color_from_application_constants =
			application_constants_fetch(
				application_constants->dictionary,
				"color10" ) ) )
		{
			background_color_array[ 9 ] =
				color_from_application_constants;
			*background_color_array_length = 10;
		}
		else
		{
			background_color_array[ 9 ] = "white";
		}
	}
	else
	{
		background_color_array[ 0 ] = FORM_COLOR1;
		background_color_array[ 1 ] = FORM_COLOR2;
		background_color_array[ 2 ] = FORM_COLOR3;
		background_color_array[ 3 ] = FORM_COLOR4;
		background_color_array[ 4 ] = FORM_COLOR5;
		*background_color_array_length = 5;
	}
	return background_color_array;
}

FORM_PROMPT *form_prompt_calloc( void )
{
	FORM_PROMPT *form_prompt;

	if ( ! ( form_prompt = calloc( 1, sizeof( FORM_PROMPT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt;
}

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
		( element =
			appaserver_element_new(
				table_row,
				(char *)0 /* element_name */ ) ) );

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
			(LIST *)0 /* display_list */,
			no_initial_capital,
			0 /* not output_null_option */,
			0 /* not output_not_null_option */,
			1 /* output_select_option */,
			element_drop_down_display_size(
				list_length(
					delimited_list ) ),
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

FORM_RADIO *form_radio_calloc( void )
{
	FORM_RADIO *form_radio;

	if ( ! ( form_radio = calloc( 1, sizeof( FORM_RADIO ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_radio;
}

FORM_RADIO *form_radio_new(
			char *radio_name,
			char *initial_value,
			LIST *value_string_list,
			char *set_all_push_buttons_html )
{
	FORM_RADIO *form_radio = form_radio_calloc();

	form_radio->value_list =
		form_radio_value_list(
			radio_name,
			initial_value,
			value_string_list );

	form_radio->html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		form_radio_html(
			form_radio->value_list,
			set_all_push_buttons_html );

	if ( !form_radio->html )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: form_radio_html() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	return form_radio;
}

char *form_radio_html(
			LIST *value_list,
			char *set_all_push_buttons_html )
{
	char html[ STRING_INPUT_BUFFER ];
	char *ptr = html;
	FORM_RADIO_VALUE *form_radio_value;

	if ( !list_rewind( value_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		"<table cellspacing=0 cellpadding=0 border>\n<tr>\n" );

	if ( set_all_push_buttons_html && *set_all_push_buttons_html )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			set_all_push_buttons_html );
	}

	do {
		form_radio_value =
			list_get(
				value_list );

		ptr += sprintf(
			ptr,
			"%s\n",
			form_radio_value->html );

	} while ( list_next( value_list ) );

	ptr += sprintf(
		ptr,
		"</table>\n" );

	return strdup( html );
}

FORM_RADIO_VALUE *form_radio_value_calloc( void )
{
	FORM_RADIO_VALUE *form_radio_value;

	if ( ! ( form_radio_value = calloc( 1, sizeof( FORM_RADIO_VALUE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_radio_value;
}

LIST *form_radio_value_list(
			char *radio_value,
			char *initial_value,
			LIST *value_string_list )
{
	LIST *value_list;

	if ( !list_rewind( value_string_list ) ) return (LIST *)0;

	value_list = list_new();

	do {
		list_set(
			value_list,
			form_radio_value_new(
				radio_value,
				initial_value,
				list_get( value_string_list ) ) );

	} while ( list_next( value_string_list ) );

	return value_list;
}

FORM_RADIO_VALUE *form_radio_value_new(
			char *radio_name,
			char *initial_value,
			char *value_string )
{
	FORM_RADIO_VALUE *form_radio_value = form_radio_value_calloc();

	form_radio_value->html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		form_radio_value_html(
			radio_name,
			initial_value,
			value_string );

	if ( !form_radio_value->html ) return (FORM_RADIO_VALUE *)0;

	return form_radio_value;
}

char *form_radio_value_html(
			char *radio_name,
			char *initial_value,
			char *value_string )
{
	char html[ 1024 ];
	char *ptr = html;
	char buffer[ 128 ];

	if ( !radio_name || !*radio_name || !value_string || !*value_string )
		return (char *)0;

	ptr += sprintf(
		ptr,
		"<td><input name=\"%s\" type=radio value=\"%s\""
		" class=lookup_option_radio_button",
		radio_name,
		value_string );

	if ( string_strcmp( value_string, initial_value ) == 0 )
	{
		ptr += sprintf( ptr, " checked" );
	}

	ptr += sprintf(
		ptr,
		">%s\n",
		string_initial_capital(
			buffer,
			value_string ) );

	return strdup( html );
}


char *form_title_html( char *title )
{
	char title_html[ 256 ];

	if ( !title )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: title is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(title_html,
		"<h1>%s</h1>\n",
		title );

	return strdup( title_html );
}

char *form_tag_html(	char *form_name,
			char *action_string,
			char *target_frame )
{
	char tag_html[ 1024 ];
	char *ptr = tag_html;

	if ( !action_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: action_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !target_frame )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: target_frame is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"<form enctype=\"multipart/form-data\" method=post" );

	if ( form_name && *form_name )
	{
		ptr += sprintf(
			ptr,
			" name=\"%s\"",
			form_name );
	}

	ptr += sprintf(
		ptr,
		" action=\"%s\" target=\"%s\">",
		action_string,
		target_frame );

	return strdup( tag_html );
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

char *form_table_row_background_color( void )
{
	static int cycle_count = 0;
	static char **background_color_array = {0};
	static int background_color_array_length = 0;
	char *background_color;

	if ( !background_color_array )
	{
		background_color_array =
			form_table_row_background_color_array(
				&background_color_array_length,
				environment_application_name() );
	}

	if ( !background_color_array_length )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty background_color_array.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	background_color = background_color_array[ cycle_count ];
	if ( ++cycle_count == background_color_array_length )
		cycle_count = 0;

	return background_color;
}

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
				element_list,
				(char *)0 /* application_name */,
				(char *)0 /* background_color */,
				(char *)0 /* state */,
				0 /* row_number */,
				(DICTIONARY *)0 /* row_dictionary */ ) ),
		( tmp2 = appaserver_element_list_html(
				button_element_list,
				(char *)0 /* application_name */,
				(char *)0 /* background_color */,
				(char *)0 /* state */,
				0 /* row_number */,
				(DICTIONARY *)0 /* row_dictionary */ ) ),
		form_close_html );

		free( tmp1 );
		free( tmp2 );

	return strdup( html );
}

FORM_CHOOSE_ROLE *form_choose_role_calloc( void )
{
	FORM_CHOOSE_ROLE *form_choose_role;

	if ( ! ( form_choose_role = calloc( 1, sizeof( FORM_CHOOSE_ROLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_choose_role;
}

FORM_CHOOSE_ROLE *form_choose_role_new(
			LIST *role_name_list,
			char *post_action_string,
			char *target_frame,
			char *form_name,
			char *drop_down_element_name )
{
	FORM_CHOOSE_ROLE *form_choose_role;

	if ( !list_length( role_name_list ) )
	{
		return (FORM_CHOOSE_ROLE *)0;
	}

	form_choose_role = form_choose_role_calloc();

	form_choose_role->tag_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_tag_html(
			form_name,
			post_action_string,
			target_frame );

	form_choose_role->drop_down_onchange_javascript =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		form_choose_role_drop_down_onchange_javascript(
			form_name );

	form_choose_role->element_list =
		form_choose_role_element_list(
			role_name_list,
			form_choose_role->drop_down_onchange_javascript,
			drop_down_element_name );

	if ( !list_length( form_choose_role->element_list ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: form_choose_role_element_list() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_choose_role->html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		form_choose_role_html(
			form_choose_role->tag_html,
			form_choose_role->element_list,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_html() );

	if ( !form_choose_role->html )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: form_choose_role_html() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_choose_role;
}

char *form_choose_role_drop_down_onchange_javascript(
			char *form_name )
{
	static char onchange_javascript[ 128 ];

	if ( !form_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: form_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(onchange_javascript,
		"%s.submit()",
		form_name );

	return onchange_javascript;
}

LIST *form_choose_role_element_list(
			LIST *role_name_list,
			char *drop_down_onchange_javascript,
			char *drop_down_element_name )
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
		( element =
			appaserver_element_new(
				table_row,
				(char *)0 /* element_name */ ) ) );

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
			drop_down_element_name,
			(LIST *)0 /* attribute_name_list */,
			role_name_list /* delimited_list */,
			(LIST *)0 /* display_list */,
			0 /* not no_initial_capital */,
			0 /* not output_null_option */,
			0 /* not output_not_null_option */,
			0 /* not output_select_option */,
			element_drop_down_display_size(
				list_length(
					role_name_list ) ),
			-1 /* tab order */,
			0 /* not multi_select */,
			drop_down_onchange_javascript
				/* post_change_javascript */,
			0 /* not readonly */,
			0 /* not recall */ );

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

char *form_choose_role_html(
			char *tag_html,
			LIST *element_list,
			char *form_close_html )
{
	char html[ 65536 ];
	char *element_list_html;

	if ( !tag_html
	||   !list_length( element_list )
	||   !form_close_html )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (char *)0;
	}

	if ( ! ( element_list_html =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			appaserver_element_list_html(
				element_list,
				(char *)0 /* application_name */,
				(char *)0 /* background_color */,
				(char *)0 /* state */,
				0 /* row_number */,
				(DICTIONARY *)0 /* row_dictionary */ ) ) )
	{
		fprintf(stderr,
"Warning in %s/%s()/%d: appaserver_element_list_html() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (char *)0;
	}

	sprintf(html,
		"%s\n%s\n%s",
		tag_html,
		element_list_html,
		form_close_html );

	free( element_list_html );

	return strdup( html );
}

char *form_close_html( void )
{
	return "</form>";
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

char *form_element_list_html(
			char *tag_html,
			LIST *element_list,
			char *form_close_html )
{
	char html[ STRING_64K ];
	char *tmp;

	if (	!tag_html
	||	!list_length( element_list )
	||	!form_close_html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ( tmp =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		appaserver_element_list_html(
			element_list /* in/out */,
			(char *)0 /* application_name */,
			(char *)0 /* background_color */,
			(char *)0 /* state */,
			0 /* row_number */,
			(DICTIONARY *)0 /* row_dictionary */ ) ) )
	{ 
		sprintf(html,
			"%s\n%s\n%s",
			tag_html,
			tmp,
			form_close_html );

		free( tmp );
	}
	else
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: appaserver_element_list_html() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return strdup( html );
}

