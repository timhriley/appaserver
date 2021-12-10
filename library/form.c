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
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "vertical_new_button.h"
#include "dictionary_separate.h"
#include "pair_one2m.h"
#include "element.h"
#include "javascript.h"
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

LIST *form_prompt_isa_element_list(
			char *one2m_isa_folder_name,
			LIST *primary_key_list,
			LIST *delimited_list,
			boolean no_initial_capital )
{
	LIST *element_list = list_new();
	APPASERVER_ELEMENT *element;
	char buffer[ 512 ];

	/* Create a table row */
	/* ------------------ */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_row ) ) );

	/* Returns program memory */
	/* ---------------------- */
	element->table_row->html = element_table_row_html();

	/* Create a prompt */
	/* --------------- */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				non_edit_text ) ) );

	element->non_edit_text->html =
		/* --------------------------- */
		/* Safely returns heap memory. */
		/* --------------------------- */
		element_non_edit_text_html(
			string_initial_capital(
				buffer /* prompt_string */,
				one2m_isa_folder_name /* source */ ) );

	/* Create a drop-down */
	/* ------------------ */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				drop_down ) ) );

	free( element->drop_down );

	element->drop_down =
		element_drop_down_new(
			primary_key_list /* attribute_name_list */,
			(char *)0 /* initial_data */,
			delimited_list,
			no_initial_capital,
			0 /* not output_null_option */,
			0 /* not output_not_null_option */,
			1 /* output_select_option */,
			1 /* column_span */,
			element_drop_down_size(
				list_length(
					delimited_list ) ),
			-1 /* tab order */,
			0 /* not multi_select */,
			(char *)0 /* post_change_javascript */,
			(char *)0 /* state */ );

	/* Create a table row */
	/* ------------------ */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				table_row ) ) );

	/* Returns program memory */
	/* ---------------------- */
	element->table_row->html = element_table_row_html();

	/* Create a lookup state checkbox */
	/* ------------------------------ */
	list_set(
		element_list,
		( element =
			appaserver_element_new(
				checkbox ) ) );

	element->checkbox->html =
		/* ---------------------------- */
		/* Returns heap memory or null. */
		/* ---------------------------- */
		element_checkbox_html(
			ELEMENT_NAME_LOOKUP_STATE,
			"Lookup",
			0 /* not checked */,
			(char *)0 /* action_string */,
			-1 /* tab_order */,
			"yes" /* value */ );

	if ( !element->checkbox->html )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: element_checkbox_html() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

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

char *form_message_html( char *prompt_message )
{
	char message_html[ 256 ];

	if ( !prompt_message )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: prompt_message is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(message_html,
		"<h1>%s</h1>\n",
		prompt_message );

	return strdup( message_html );
}

char *form_tag_html(	char *action_string,
			char *target_frame )
{
	char tag_html[ STRING_INPUT_BUFFER ];

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

	sprintf(tag_html,
		"<form enctype=\"multipart/form-data\"\n"
		"\tmethod=post name=\"prompt\"\n"
		"\taction=\"%s\"\n"
		"\ttarget=\"%s\">\n",
		action_string,
		target_frame );

	return strdup( tag_html );
}

FORM_PROMPT_ISA *form_prompt_isa_calloc( void )
{
	FORM_PROMPT_ISA *form_prompt_isa;

	if ( ! ( form_prompt_isa =
			calloc( 1, sizeof( FORM_PROMPT_ISA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_isa;
}

FORM_PROMPT_ISA *form_prompt_isa_new(
			char *title,
			char *prompt_message,
			char *one2m_folder_name,
			LIST *primary_key_list,
			LIST *delimited_list,
			boolean no_initial_capital,
			char *action_string )
{
	FORM_PROMPT_ISA *form_prompt_isa = form_prompt_isa_calloc();

	form_prompt_isa->title_html =
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		form_title_html( title );

	form_prompt_isa->message_html =
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		form_message_html( prompt_message );

	form_prompt_isa->tag_html =
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		form_tag_html(
			action_string,
			PROMPT_FRAME );

	form_prompt_isa->element_list =
		form_prompt_isa_element_list(
			one2m_folder_name,
			primary_key_list,
			delimited_list,
			no_initial_capital );

	form_prompt_isa->button_list =
		form_prompt_isa_button_list();

	return form_prompt_isa;
}

LIST *form_prompt_isa_button_list( void )
{
	LIST *button_list = list_new();

	list_set(
		button_list,
		button_submit(
			(char *)0,
			(char *)0,
			(char *)0,
			(char *)0,
			0 /* form_number */ ) );

	list_set(
		button_list,
		button_reset(
			(char *)0 /* post_change_javascript */,
			(char *)0 /* state */,
			0 /* form_number */ );

	list_set(
		button_list,
		button_back() );

	list_set(
		button_list,
		button_forward() );

	return button_list;
}

void form_prompt_isa_output(
			FILE *output_stream,
			char *title_html,
			char *message_html,
			char *form_tag_html,
			LIST *element_list,
			LIST *button_list )
{
	fprintf( output_stream, "%s\n", title_html );
	fprintf( output_stream, "%s\n", message_html );

	/* Open table then form */
	/* -------------------- */
	fprintf( output_stream, "<table border=0>\n" );
	fprintf( output_stream, "%s\n", form_tag_html );

	appaserver_element_list_output(
		output_stream,
		element_list );

	fprintf(output_stream,
		"%s\n",
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		button_list_html(
			button_list ) );

	/* Close form then table */
	/* --------------------- */
	fprintf( output_stream, "</form>\n" );
	fprintf( output_stream, "</table>\n" );
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
			int dictionary_list_length,
			char *submit_action_string,
			LIST *heading_list,
			LIST *operation_list )
{
	FORM_EDIT_TABLE *form_edit_table = form_edit_table_calloc();

	form_edit_table->dictionary_list_length = dictionary_list_length;

	return form_edit_table;
}

LIST *form_edit_table_button_list(
			int dictionary_list_length );

