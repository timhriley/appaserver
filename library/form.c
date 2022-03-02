/* -------------------------------------------------------------------- */
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
	char html[ STRING_FOUR_MEG ];
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

FORM_PROMPT_EDIT *form_prompt_edit_new(
			char *action_string,
			boolean omit_insert_button,
			boolean omit_delete_button,
			boolean omit_new_button,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,.
			DICTIONARY *drillthru_dictionary,
			boolean drillthru_participating,
			boolean drillthru_skipped,
			boolean drillthru_finished )
{
	FORM_PROMPT_EDIT *form_prompt_edit = form_prompt_edit_calloc();
	char *tmp1;
	char *tmp2;

	form_prompt_edit->radio_pair_list =
		form_prompt_edit_radio_pair_list(
			prompt_edit_omit_insert_button,
			prompt_edit_omit_delete_button,
			prompt_edit_omit_new_button,
			list_length( relation_mto1_non_isa_list ) );

	form_prompt_edit->radio_list =
		radio_list_new(
			FORM_RADIO_LIST_NAME,
			form_prompt_edit->radio_pair_list,
			RADIO_LOOKUP_LABEL /* initial_label */ );

	form_prompt_edit->target_frame =
		form_prompt_edit_target_frame(
			drillthru_participating,
			drillthru_skipped,
			drillthru_finished,
			FRAMESET_PROMPT_FRAME,
			FRAMESET_EDIT_FRAME );

	form_prompt_edit->tag_html =
		form_tag_html(
			"prompt_edit" /* form_name */,
			action_string,
			form_prompt_edit->target_frame );

	form_prompt_edit->element_list =
		form_prompt_edit_element_list(
			folder_attribute_append_isa_list,
			relation_mto1_non_isa_list,
			relation_join_one2m_list,
			drillthru_dictionary );

	form_prompt_edit->keystrokes_save_string =
		form_prompt_edit_keystrokes_save_string(
			form_prompt_edit->element_list );

	form_prompt_edit->keystrokes_recall_string =
		form_prompt_edit_keystrokes_recall_string(
			form_prompt_edit->element_list );

	form_prompt_edit->button_element_list =
		form_prompt_edit_button_element_list(
			form_prompt_edit->keystrokes_save_string,
			form_prompt_edit->keystrokes_recall_string );

	form_prompt_edit->html =
		form_prompt_edit_html(
			form_prompt_edit->tag_html,
			form_prompt_edit->radio_list->html,
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			( tmp1 = element_list_html(
					form_prompt_edit->element_list )
						/* element_list_html */ ),
			( tmp2 = element_list_html(
					form_prompt_edit->button_element_list ),
						/* button_element_list_html */),
			form_close_html() );

	if ( tmp1 ) free( tmp1 );
	if ( tmp2 ) free( tmp2 );

	return form_prompt_edit;
}

char *form_prompt_edit_target_frame(
			boolean drillthru_participating,
			boolean drillthru_skipped,
			boolean drillthru_finished,
			char *frameset_prompt_frame,
			char *frameset_edit_frame )
{
	if ( !drillthru_participating )
		return frameset_edit_frame;
	else
	if ( drillthru_skipped )
		return frameset_edit_frame;
	else
	if ( drillthru_finished )
		return frameset_edit_frame;
	else
		return frameset_prompt_frame;
}

char *form_prompt_edit_keystrokes_save_string(
			LIST *form_prompt_edit_element_list )
{
	char string[ 65536 ];
	char *ptr = string;

	*ptr = '\0';

	return strdup( string );
}

char *form_prompt_edit_keystrokes_recall_string(
			LIST *form_prompt_edit_element_list )
{
	char string[ 65536 ];
	char *ptr = string;

	*ptr = '\0';

	return strdup( string );
}

char *form_prompt_edit_html(
			char *tag_html,
			char *radio_list_html,
			char *element_list_html,
			char *button_element_list_html,
			char *form_close_html )
{
	char html[ STRING_TWO_MEG ];
	char *ptr = html;

	*html = '\0';

	return strdup( html );
}

LIST *form_prompt_edit_radio_pair_list(
			boolean prompt_edit_omit_insert_button,
			boolean prompt_edit_omit_delete_button,
			boolean prompt_edit_omit_new_button,
			int relation_mto1_non_isa_list_length )
{
	LIST *radio_pair_list = list_new();

	list_set(
		radio_pair_list,
		radio_pair_new(
			RADIO_STATISTICS_NAME,
			RADIO_STATISTICS_LABEL ) );

	if ( relation_mto1_non_isa_list_length )
	{
		list_set(
			radio_pair_list,
			radio_pair_new(
				RADIO_GROUP_COUNT_NAME,
				RADIO_GROUP_COUNT_LABEL ) );
	}

	list_set(
		radio_pair_list,
		radio_pair_new(
			RADIO_SPREADSHEET_NAME,
			RADIO_SPREADSHEET_LABEL ) );

	if ( !prompt_edit_omit_insert_button )
	{
		list_set(
			radio_pair_list,
			radio_pair_new(
				RADIO_INSERT_NAME,
				RADIO_INSERT_LABEL ) );
	}

	if ( !prompt_edit_omit_delete_button )
	{
		list_set(
			radio_pair_list,
			radio_pair_new(
				RADIO_DELETE_NAME,
				RADIO_DELETE_LABEL ) );
	}

	list_set(
		radio_pair_list,
		radio_pair_new(
			RADIO_LOOKUP_NAME,
			RADIO_LOOKUP_LABEL ) );

	return radio_pair_list;
}

FORM_PROMPT_EDIT *form_prompt_edit_calloc( void )
{
	FORM_PROMPT_EDIT *form_prompt_edit;

	if ( ! ( form_prompt_edit = calloc( 1, sizeof( FORM_PROMPT_EDIT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_edit;
}

LIST *form_prompt_edit_element_list(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary )
{
	LIST *element_list = list_new();
	FOLDER_ATTRIBUTE *folder_attribute;
	RELATION *relation;
	APPASERVER_ELEMENT *element;

	if ( !list_length( folder_attribute_append_isa_list ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: folder_attribute_append_isa_list is empty\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

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

	do {
		folder_attribute = list_get( folder_attribute_append_isa_list );

		if ( ( relation =
			relation_consumes(
				folder_attribute->attribute_name,
				relation_mto1_non_isa_list ) ) )
		{

		relation->consumes_taken = 1;

		if ( !relation->one_folder )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: relation->one_folder is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !relation->one_folder->folder_name )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: one_folder->folder_name is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !list_length( relation->foreign_key_list ) )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		&&   !related_folder->omit_lookup_before_drop_down )
		{
			build_related_folder_element_list(
			   	ajax_fill_drop_down_related_folder,
			   	return_list,
			   	related_folder,
			   	done_folder_name_list,
			   	isa_folder_list,
			   	attribute,
			   	lookup_before_drop_down,
			   	application_name,
			   	session,
			   	role_name,
			   	login_name,
			   	preprompt_dictionary,
		  	   	no_display_push_button_prefix,
		  	   	no_display_push_button_heading,
			   	row_level_non_owner_forbid,
			   	folder_name
					/* one2m_folder_name_for_processes */,
			 	  attribute_exists_in_preprompt_dictionary,
			   	state,
			   	foreign_attribute_name_list );

				continue;
		}

		if ( strcmp( attribute->datatype, "hidden_text" ) == 0 )
		{
			element_list = (LIST *)0;
		}
		else
		{
			element_list =
			     attribute_prompt_element_list(
				attribute->attribute_name,
				(char *)0 /* prepend_folder_name */,
				attribute->datatype,
				attribute->post_change_javascript,
				attribute->width,
				attribute->hint_message,
				attribute->primary_key_index,
				omit_push_buttons );
		}

		if ( element_list )
		{
			list_append_list(
				return_list,
				element_list );

			list_append_pointer(
				exclude_attribute_name_list,
				attribute->attribute_name );
		}

	} while( list_next( attribute_list ) );

	if ( list_rewind( mto1_related_folder_list ) )
	{
		LIST *subtract_list;

		do {
			related_folder =
				list_get_pointer(
					mto1_related_folder_list );
	
			if ( related_folder->ignore_output ) continue;

			if ( isa_folder_list
			&&   list_length( isa_folder_list )
			&&   appaserver_isa_folder_accounted_for(
				isa_folder_list,
				related_folder->folder->folder_name,
				related_folder->related_attribute_name ) )
			{
				continue;
			}
	
			foreign_attribute_name_list =
			related_folder_foreign_attribute_name_list(
			   folder_get_primary_key_list(
				related_folder->folder->
					attribute_list ),
			   related_folder->related_attribute_name,
			   related_folder->folder_foreign_attribute_name_list );
	
			if ( list_length(
				list_subtract_string_list(
					foreign_attribute_name_list,
					exclude_attribute_name_list ) ) == 0 )
			{
				continue;
			}
	
			subtract_list =
				list_subtract(
					foreign_attribute_name_list,
					omit_update_attribute_name_list );

			if (	list_length( subtract_list ) !=
				list_length( foreign_attribute_name_list ) )
			{
				continue;
			}

			hint_message =
				related_folder_get_hint_message(
					attribute->hint_message,
					related_folder->hint_message,
					related_folder->folder->notepad );

			list_append_list(
				return_list,
				related_folder_prompt_element_list(
				   (RELATED_FOLDER **)0
				       /* ajax_fill_drop_down_related_folder */,
				   application_name,
				   session,
				   role_name,
				   login_name,
				   related_folder->folder->folder_name,
				   related_folder->folder->
					populate_drop_down_process,
				   related_folder->folder->
					attribute_list,
				   foreign_attribute_name_list,
				   0 /* dont omit_ignore_push_buttons */,
				   preprompt_dictionary,
		  		   no_display_push_button_heading,
		  		   no_display_push_button_prefix,
				   (char *)0 /* post_change_java... */,
				   hint_message,
				   0 /* max_drop_down_size */,
				   (LIST *)0 /* common_non_primary_a... */,
				   0 /* not is_primary_attribute */,
				   related_folder->folder->
					row_level_non_owner_view_only,
				   related_folder->folder->
					row_level_non_owner_forbid,
			   	   related_folder->
					related_attribute_name,
			   	   related_folder->
					drop_down_multi_select,
				   related_folder->
					folder->
					no_initial_capital,
				   (char *)0 /* state */,
				   (char *)0 /* one2m_folder_name...processes*/,
				   0 /* tab_index */,
				   0 /* not set_first_initial_data */,
				   1 /* output_null_option */,
				   1 /* output_not_null_option */,
				   1 /* output_select_option */,
				   (char *)0
				   /* appaserver_user_foreign_login_name */,
			           related_folder->omit_lookup_before_drop_down
				   ) );
	
		} while( list_next( mto1_related_folder_list ) );
	}

	if ( list_rewind( folder->join_1tom_related_folder_list ) )
	{
		RELATED_FOLDER *related_folder;

		do {
			related_folder =
				list_get(
					folder->join_1tom_related_folder_list );

			/* Make two line breaks */
			/* -------------------- */
			element = element_appaserver_new( linebreak, "" );

			list_set( return_list, element );
			list_set( return_list, element );

			/* Make the push button element. */
			/* ----------------------------- */
			sprintf( element_name,
		 		 "%s%s",
				 NO_DISPLAY_PUSH_BUTTON_PREFIX,
				 related_folder->
					one2m_folder->
					folder_name );

			element =
				element_appaserver_new(
					toggle_button, 
					strdup( element_name ) );

			element_toggle_button_set_heading(
				element->toggle_button,
				NO_DISPLAY_PUSH_BUTTON_HEADING );

			list_append_pointer(
					return_list, 
					element );

			/* Make the prompt. */
			/* ---------------- */
			element =
				element_appaserver_new(
					prompt,
				 	related_folder->
						one2m_folder->
						folder_name );

			list_append_pointer(
					return_list, 
					element );

		} while( list_next( folder->join_1tom_related_folder_list ) );
	}

	return return_list;
	return element_list;
}

FORM_PROMPT_EDIT_RELATIONAL *
	form_prompt_edit_relational_new(
			char *relational_name,
			char *from_name,
			char *to_name,
			char *attribute_name,
			char *datatype_name,
			int attribute_width )
{
	FORM_PROMPT_EDIT_RELATIONAL *form_prompt_edit_relational =
		form_prompt_edit_relational_calloc();

	form_prompt_edit_relational->element_list = list_new();

	form_prompt_edit_relational->operation_list =
		form_prompt_edit_relational_operation_list(
			datatype_name );

	list_set(
		form_prompt_edit_relational->element_list,
		( form_prompt_edit_relational->
			relational_operator_appaserver_element =
				appaserver_element_new(
					drop_down, (char *)0 ) ) );

	form_prompt_edit_relational->
		relational_operator_appaserver_element->
		drop_down =
			element_drop_down_new(
				relational_name,
				(LIST *)0 /* attribute_name_list */,
				form_prompt_edit_relational->operation_list,
				(LIST *)0 /* display_list */,
				0 /* not no_initial_capital */,
				1 /* output_null_option */,
				1 /* output_not_null_option */,
				1 /* output_select_option */,
				1 /* element_drop_down_display_size */,
				-1 /* tab_order */,
				0 /* not multi_select */,
				(char *)0 /* post_change_javascript */,
				0 /* not readonly */,
				1 /* recall */ );

	list_set(
		form_prompt_edit_relational->element_list,
		appaserver_element_new(
			table_data, (char *)0 ) );

	list_set(
		form_prompt_edit_relational->element_list,
		( form_prompt_edit_relational->
			text_from_appaserver_element =
				appaserver_element_new(
					text, (char *)0 ) ) );

	form_prompt_edit_relational->text_from_appaserver_element->text =
		element_text_new(
			from_name,
			datatype_name,
			FORM_PROMPT_EDIT_FROM_ATTRIBUTE_WIDTH,
			0 /* not null_to_slash */,
			1 /* prevent_carrot */,
			(char *)0 /* on_change */,
			(char *)0 /* on_focus */,
			(char *)0 /* on_keyup */,
			-1 /* tab_order */,
			1 /* recall */ );

	list_set(
		form_prompt_edit_relational->element_list,
		appaserver_element_new(
			table_data, (char *)0 ) );

	list_set(
		form_prompt_edit_relational->element_list,
		( form_prompt_edit_relational->
			and_appaserver_element =
				appaserver_element_new(
					non_edit_text, (char *)0 ) ) );

	form_prompt_edit_relational->and_appaserver_element->non_edit_text =
		element_non_edit_text_new(
			(char *)0
			"and" /* message */ );

	list_set(
		form_prompt_edit_relational->element_list,
		appaserver_element_new(
			table_data, (char *)0 ) );

	list_set(
		form_prompt_edit_relational->element_list,
		( form_prompt_edit_relational->
			text_to_appaserver_element =
				appaserver_element_new(
					text, (char *)0 ) ) );

	form_prompt_edit_relational->text_from_appaserver_element->text =
		element_text_new(
			to_name,
			datatype_name,
			attribute_width,
			0 /* not null_to_slash */,
			1 /* prevent_carrot */,
			(char *)0 /* on_change */,
			(char *)0 /* on_focus */,
			(char *)0 /* on_keyup */,
			-1 /* tab_order */,
			1 /* recall */ );

	return form_prompt_edit_relational;
}

LIST *form_prompt_edit_relational_operation_list(
			char *datatype_name )
{
	LIST *list;

	list = list_new();

	if ( attribute_is_date( datatype_name )
	||   attribute_is_time( datatype_name )
	||   attribute_is_date_time( datatype_name ) )
	{
		list_set( list, BEGINS_OPERATOR );
		list_set( list, EQUAL_OPERATOR );
		list_set( list, BETWEEN_OPERATOR );
		list_set( list, OR_OPERATOR );
		list_set( list, GREATER_THAN_OPERATOR );
		list_set( list, GREATER_THAN_EQUAL_TO_OPERATOR );
		list_set( list, LESS_THAN_OPERATOR );
		list_set( list, LESS_THAN_EQUAL_TO_OPERATOR );
		list_set( list, NOT_EQUAL_OPERATOR );
		list_set( list, NOT_EQUAL_OR_NULL_OPERATOR );
		list_set( list, NULL_OPERATOR );
		list_set( list, NOT_NULL_OPERATOR );
	}
	else
	if ( attribute_is_notepad( datatype_name ) )
	{
		list_set( list, CONTAINS_OPERATOR );
		list_set( list, NOT_CONTAINS_OPERATOR );
		list_set( list, NULL_OPERATOR );
		list_set( list, NOT_NULL_OPERATOR );
	}
	else
	if ( attribute_is_text( datatype_name )
	||   attribute_is_upload( datatype_name ) )
	{
		list_set( list, BEGINS_OPERATOR );
		list_set( list, EQUAL_OPERATOR );
		list_set( list, CONTAINS_OPERATOR );
		list_set( list, OR_OPERATOR );
		list_set( list, GREATER_THAN_EQUAL_TO_OPERATOR );
		list_set( list, NOT_CONTAINS_OPERATOR );
		list_set( list, NOT_EQUAL_OPERATOR );
		list_set( list, NOT_EQUAL_OR_NULL_OPERATOR );
		list_set( list, NULL_OPERATOR );
		list_set( list, NOT_NULL_OPERATOR );
	}
	else
	{
		list_set( list, EQUAL_OPERATOR );
		list_set( list, BETWEEN_OPERATOR );
		list_set( list, BEGINS_OPERATOR );
		list_set( list, CONTAINS_OPERATOR );
		list_set( list, NOT_CONTAINS_OPERATOR );
		list_set( list, OR_OPERATOR );
		list_set( list, NOT_EQUAL_OPERATOR );
		list_set( list, NOT_EQUAL_OR_NULL_OPERATOR );
		list_set( list, GREATER_THAN_OPERATOR );
		list_set( list, GREATER_THAN_EQUAL_TO_OPERATOR );
		list_set( list, LESS_THAN_OPERATOR );
		list_set( list, LESS_THAN_EQUAL_TO_OPERATOR );
		list_set( list, NULL_OPERATOR );
		list_set( list, NOT_NULL_OPERATOR );
	}

	return list;
}

FORM_PROMPT_EDIT_RELATIONAL *
	form_prompt_edit_relational_calloc(
			void )
{
	FORM_PROMPT_EDIT_RELATIONAL *form_prompt_edit_relational;

	if ( ! ( form_prompt_edit_relational =
			calloc( 1, sizeof( FORM_PROMPT_EDIT_RELATIONAL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_edit_relational;
}

FORM_PROMPT_EDIT_ATTRIBUTE *form_prompt_edit_attribute_new(
			char *attribute_name,
			int primary_key_index,
			char *datatype_name,
			int attribute_width,
			char *hint_message,
			LIST *form_prompt_edit_relation_list )
{
	FORM_PROMPT_EDIT_ATTRIBUTE *form_prompt_edit_attribute =
		form_prompt_edit_attribute_calloc();

	if ( form_prompt_edit_relation_attribute_name_exists(
		attribute_name,
		form_prompt_edit_relation_list ) )
	{
		return (FORM_PROMPT_EDIT_ATTRIBUTE *)0;
	}

	form_prompt_edit_attribute->element_list = list_new();

	form_prompt_edit_attribute->no_display_name =
		form_prompt_edit_attribute_no_display_name(
			FORM_PROMPT_EDIT_NO_DISPLAY_PREFIX,
			attribute_name );

	list_set(
		form_prompt_edit_attribute->element_list,
		( form_prompt_edit_attribute->
			no_display_appaserver_element =
				appaserver_element_new(
					checkbox, (char *)0 ) ) );

	form_prompt_edit_attribute->
		no_display_appaserver_element->
		checkbox =
			element_checkbox_new(
				form_prompt_edit_attribute->no_display_name,
				(char *)0 /* element_name */,
				(char *)0 /* prompt_string */,
				(char *)0 /* on_click */,
				-1 /* tab_order */,
				(char *)0 /* image_source */,
				1 /* recall */ );

	list_set(
		form_prompt_edit_attribute->element_list,
		appaserver_element_new( table_data, (char *)0 ) );

	form_prompt_edit_attribute->prompt =
		form_prompt_edit_attribute_prompt(
			attribute_name,
			primary_key_index );

	list_set(
		form_prompt_edit_attribute->element_list,
		( form_prompt_edit_attribute->prompt_appaserver_element =
			appaserver_element_new(
				non_edit_text, (char *)0 ) ) );

	form_prompt_edit_attribute->prompt_appaserver_element->non_edit_text =
		element_non_edit_text_new(
			(char *)0,
			form_prompt_edit_attribute->prompt );

	list_set(
		form_prompt_edit_attribute->element_list,
		appaserver_element_new( table_data, (char *)0 ) );

	form_prompt_edit_attribute->from_name =
		form_prompt_edit_attribute_from_name(
			FORM_PROMPT_EDIT_FROM_PREFIX,
			attribute_name );

	if ( attribute_is_yes_no( attribute_name ) )
	{
		list_set(
			form_prompt_edit_attribute->element_list,
			( form_prompt_edit_attribute->
				yes_no_appaserver_element =
					appaserver_element_new(
						yes_no, (char *)0 ) ) );

		form_prompt_edit_attribute->
			yes_no_appaserver_element =
			yes_no =
				element_yes_no_new(
					form_prompt_edit_attribute->from_name,
					(char *)0 /* on_click */,
					-1 /* tab_order */,
					1 /* recall */ );

		list_set(
			form_prompt_edit_attribute->element_list,
			appaserver_element_new( table_data, (char *)0 ) );

		list_set(
			form_prompt_edit_attribute->element_list,
			appaserver_element_new( table_data, (char *)0 ) );

		list_set(
			form_prompt_edit_attribute->element_list,
			appaserver_element_new( table_data, (char *)0 ) );
	}
	else
	{
		form_prompt_edit_attribute->relational_name =
			form_prompt_edit_attribute_relational_name(
				FORM_PROMPT_EDIT_RELATIONAL_PREFIX,
				attribute_name );

		form_prompt_edit_attribute->to_name =
			form_prompt_edit_attribute_to_name(
			FORM_PROMPT_EDIT_TO_PREFIX,
			attribute_name );

		form_prompt_edit_attribute->form_prompt_edit_relational =
			form_prompt_edit_relational_new(
				form_prompt_edit_attribute->relational_name,
				form_prompt_edit_attribute->from_name,
				form_prompt_edit_attribute->to_name,
				attribute_name,
				datatype_name,
				attribute_width );

		if ( !form_prompt_edit_attribute->form_prompt_edit_relational )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: form_prompt_edit_relational_new() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set_list(
			form_prompt_edit_attribute->element_list,
			form_prompt_edit_attribute->
				form_prompt_edit_relational->
					element_list );
	}

	if ( hint_message )
	{
		list_set(
			form_prompt_edit_attribute->element_list,
			appaserver_element_new( table_data, (char *)0 ) );

		list_set(
			form_prompt_edit_attribute->element_list,
			( form_prompt_edit_attribute->
				hint_message_appaserver_element =
					appaserver_element_new(
						non_edit_text, (char *)0 ) ) );

		form_prompt_edit_attribute->
			hint_message_appaserver_element =
			non_edit_text =
				element_non_edit_text_new(
					(char *)0,
					hint_message );
	}

	return form_prompt_edit_attribute;
}

char *form_prompt_edit_attribute_no_display_name(
			char *form_prompt_edit_no_display_prefix,
			char *attribute_name )
{
	char name[ 128 ];

	sprintf(name,
		"%s%s",
		form_prompt_edit_no_display_prefix,
		attribute_name );

	return strdup( name );
}

char *form_prompt_edit_attribute_prompt(
			char *attribute_name,
			int primary_key_index )
{
	char name[ 128 ];
	char *ptr = name;

	if ( primary_key_index ) ptr += sprintf( ptr, "* " );

	string_initial_capital( ptr, attribute_name );

	return strdup( name );
}

char *form_prompt_edit_attribute_from_name(
			char *form_prompt_edit_from_prefix,
			char *attribute_name )
{
	char name[ 128 ];

	sprintf(name,
		"%s%s",
		form_prompt_edit_from_prefix,
		attribute_name );

	return strdup( name );
}

char *form_prompt_edit_attribute_relational_name(
			char *form_prompt_edit_attribute_relational_prefix,
			char *attribute_name )
{
	char name[ 128 ];

	sprintf(name,
		"%s%s",
		form_prompt_edit_attribute_relational_prefix,
		attribute_name );

	return strdup( name );
}

char *form_prompt_edit_attribute_to_name(
			char *form_prompt_edit_to_prefix,
			char *attribute_name )
{
	char name[ 128 ];

	sprintf(name,
		"%s%s",
		form_prompt_edit_to_prefix,
		attribute_name );

	return strdup( name );
}

FORM_PROMPT_EDIT_ATTRIBUTE *form_prompt_edit_attribute_calloc( void )
{
	FORM_PROMPT_EDIT_ATTRIBUTE *form_prompt_edit_attribute;

	if ( ! ( form_prompt_edit_attribute =
			calloc( 1, sizeof( FORM_PROMPT_EDIT_ATTRIBUTE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_edit_attribute;
}

FORM_PROMPT_EDIT_ELEMENT_LIST *
	form_prompt_edit_element_list_new(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	FORM_PROMPT_EDIT_ELEMENT_LIST *form_prompt_edit_element_list;

	if ( !list_rewind( folder_attribute_append_isa_list ) )
		return (FORM_PROMPT_EDIT_ELEMENT_LIST *)0;

	form_prompt_edit_element_list =
		form_prompt_edit_element_list_calloc();

	form_prompt_edit_element_list->element_list = list_new();

	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		list_set(
			form_prompt_edit_element_list->element_list,
			appaserver_element_new(
				table_row, (char *)0 ) );

		list_set(
			form_prompt_edit_element_list->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

		if ( ( form_prompt_edit_element_list->
			form_prompt_edit_relation =
				form_prompt_edit_relation_new(
					folder_attribute->attribute_name,
					folder_attribute->primary_key_index,
					relation_mto1_non_isa_list,
					drillthru_dictionary,
					login_name,
					security_entity_where,
					form_prompt_edit_relation_list ) ) )
		{
			if ( !form_prompt_edit_element_list->
				form_prompt_edit_relation_list )
			{
				form_prompt_edit_element_list->
					form_prompt_edit_relation_list =
						list_new();
			}

			list_set(
				form_prompt_edit_element_list->
					form_prompt_edit_relation_list,
				form_prompt_edit_element_list->
					form_prompt_relation );

			list_set_list(
				form_prompt_edit_element_list->element_list,
				form_prompt_edit_element_list->
					form_prompt_edit_relation->
						element_list );

			continue;
		}

		if ( ( form_prompt_edit_element_list->
			form_prompt_edit_attribute =
				form_prompt_edit_attribute_new(
					folder_attribute->attribute_name,
					folder_attribute->primary_key_index,
					folder_attribute->
						attribute->
						datatype_name,
					folder_attribute->
						attribute->
						width,
					folder_attribute->
						attribute->
						hint_message,
					form_prompt_edit_element_list->
					    form_prompt_edit_relation_list ) ) )
		{
			list_set_list(
				form_prompt_edit_element_list->element_list,
				form_prompt_edit_element_list->
					form_prompt_edit_attribute->
					element_list );
		}

	} while ( list_next( folder_attribute_append_isa_list ) );

	if ( ! ( folder_prompt_edit_element_list->element_list_html =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			element_list_html(
				form_prompt_edit_element_list->
					element_list ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: element_list_html() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_edit_element_list;
}

FORM_PROMPT_EDIT_ELEMENT_LIST *
	form_prompt_edit_element_list_calloc(
			void )
{
	FORM_PROMPT_EDIT_ELEMENT_LIST *form_prompt_edit_element_list;

	if ( ! ( form_prompt_edit_element_list =
			calloc( 1, sizeof( FORM_PROMPT_EDIT_ELEMENT_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_edit_element_list;
}

FORM_PROMPT_EDIT_RELATION *form_prompt_edit_relation_new(
			char *attribute_name,
			int primary_key_index,
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where,
			LIST *form_prompt_edit_relation_list )
{
	FORM_PROMPT_EDIT_RELATION *form_prompt_edit_relation;
	RELATION *relation;

	if ( form_prompt_edit_relation_attribute_name_exists(
		attribute_name,
		form_prompt_edit_relation_list ) )
	{
		return (FORM_PROMPT_EDIT_RELATION *)0;
	}

	if ( ! ( relation =
			relation_consumes(
				attribute_name,
				relation_mto1_non_isa_list ) ) )
	{
		return (FORM_PROMPT_EDIT_RELATION *)0;
	}

	if ( !relation->one_folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: relation->one_folder is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !relation->one_folder->folder_name )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: relation->one_folder->folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	relation->consumes_taken = 1;
	form_prompt_edit_relation = form_prompt_edit_relation_calloc();
	form_prompt_edit_relation->relation = relation;
	form_prompt_edit_relation->element_list = list_new();

	form_prompt_edit_relation->query_widget =
		query_widget_new(
			relation->one_folder->folder_name
				/* widget_folder_name */,
			login_name,
			relation->
				one_folder->
				folder_attribute_list(),
			relation->
				one_folder->
				relation_mto1_non_isa_list,
			security_entity_where,
			drillthru_dictionary );

	if ( !form_prompt_edit_relation->query_widget )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: query_widget_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_prompt_edit_relation->name =
		form_prompt_edit_relation_name(
			relation->one_folder->folder_name
				/* one_folder_name */,
			char *relation->related_attribute_name,
			LIST *relation->foreign_key_list );

	form_prompt_edit_relation->no_display_name =
		form_prompt_edit_relation_no_display_name(
			FORM_PROMPT_EDIT_NO_DISPLAY_PREFIX,
			form_prompt_edit_relation->name );

	list_set(
		form_prompt_edit_relation->element_list,
		( form_prompt_edit_relation->no_display_appaserver_element =
			appaserver_element_new(
				checkbox, (char *)0 ) ) );

	form_prompt_edit_relation->
		no_display_appaserver_element->
		element_checkbox =
			element_checkbox_new(
				form_prompt_edit_relation->no_display_name,
				(char *)0 /* element_name */,
				(char *)0 /* prompt_string */,
				(char *)0 /* on_click */,
				-1 /* tab_order */,
				(char *)0 /* image_source */,
				1 /* recall */ );

	list_set(
		form_prompt_edit_relation->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

	form_prompt_edit_relation->prompt =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_prompt_edit_relation_prompt(
			relation->one_folder->folder_name
				/* one_folder_name */,
			relation->related_attribute_name,
			relation->foreign_key_list,
			primary_key_index );

	list_set(
		form_prompt_edit_relation->element_list,
		( form_prompt_edit_relation->prompt_appaserver_element =
			appaserver_element_new(
				non_edit_text, (char *)0 ) ) );

	form_prompt_edit_relation->
		prompt_appaserver_element->
		non_edit_text =
			element_non_edit_text_new(
				(char *)0,
				form_prompt_edit_relation->prompt );

	list_set(
		form_prompt_edit_relation->element_list,
		appaserver_element_new(
			table_data, (char *)0 ) );

	form_prompt_edit_relation->element_name =
		form_prompt_edit_relation_element_name(
			FORM_PROMPT_EDIT_RELATION_PREFIX,
			form_prompt_edit_relation->name );

	if ( relation->one_folder->multi_select )
	{
		list_set(
			element_list,
			( form_prompt_edit_relation->
				drop_down_appaserver_element =
					appaserver_element_new(
						multi_drop_down,
						(char *)0 ) ) );

		form_prompt_edit_relation->
			drop_down_appaserver_element->
			multi_drop_down =
				element_multi_drop_down_new(
					form_prompt_edit_relation->
						element_name,
					query_widget->delimited_list,
					relation->
						one_folder->
						no_initial_capital,
					(char *)0 /* post_change_java... */ );
	}
	else
	{
		list_set(
			element_list,
			( form_prompt_edit_relation->
				drop_down_appaserver_element =
					appaserver_element_new(
						drop_down, (char *)0 ) ) );

		form_prompt_edit_relation->
			drop_down_appaserver_element->
			drop_down =
				element_drop_down_new(
					form_prompt_edit_relation->element_name,
					(LIST *)0 /* attribute_name_list */,
					form_prompt_edit_relation->
						query_widget->
						delimited_list,
					(LIST *)0 /* display_list */,
					relation->
						one_folder->
						no_initial_capital,
					1 /* output_null_option */,
					1 /* output_not_null_option */,
					1 /* output_select_option */,
					element_drop_down_display_size(
						list_length(
						   form_prompt_edit_relation->
							query_widget->
							delimited_list ) ),
					-1 /* tab_order */,
					0 /* not multi_select */,
					(char *)0 /* post_change_javascript */,
					0 /* not readonly */,
					1 /* recall */ );

		list_set(
			form_prompt_edit_relation->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

		list_set(
			form_prompt_edit_relation->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );

		list_set(
			form_prompt_edit_relation->element_list,
			appaserver_element_new(
				table_data, (char *)0 ) );
	}

	if ( relation->hint_message )
	{
		list_set(
			form_prompt_edit_relation->element_list,
			( form_prompt_edit_relation->
				hint_message_appaserver_element =
					appaserver_element_new(
						non_edit_text, (char *)0 ) ) );

		form_prompt_edit_relation->
			hint_message_appaserver_element =
			non_edit_text =
				element_non_edit_text_new(
					(char *)0 /* prompt_name */,
					relation->hint_message );
	}

	return form_prompt_edit_relation;
}

boolean form_prompt_edit_relation_attribute_name_exists(
			char *attribute_name,
			LIST *form_prompt_edit_relation_list )
{
	FORM_PROMPT_EDIT_RELATION *form_prompt_edit_relation;

	if ( !list_rewind( form_prompt_edit_relation_list ) ) return 0;

	do {
		form_prompt_edit_relation =
			list_get(
				form_prompt_edit_relation_list );

		if ( list_string_exists(
			attribute_name,
			form_prompt_edit_relation->
				relation->
				foreign_key_list ) )
		{
			return 1;
		}

	} while ( list_next( form_prompt_edit_relation_list ) );

	return 0;
}

char *form_prompt_edit_relation_name(
			char *one_folder_name,
			char *related_attribute_name,
			LIST *foreign_key_list )
{
	char name[ 256 ];

	if ( strlen( foreign_key_list ) )
	{
		strcpy(	name,
			list_display_delimited(
				foreign_key_list,
				'^' ) );
	}
	else
	if ( related_attribute_name && *related_attribute_name )
	{
		sprintf(name,
			%s^%s",
			one_folder_name,
			related_attribute_name );
	}
	else
	{
		strcpy( name, one_folder_name );
	}

	return strdup( name );
}

char *form_prompt_edit_relation_no_display_name(
			char *form_prompt_edit_no_display_prefix,
			char *relation_name )
{
	char name[ 128 ];

	sprintf(name,
		"%s%s",
		form_prompt_edit_no_display_prefix,
		relation_name );

	return strdup( name );
}

char *form_prompt_edit_relation_original_name(
			char *form_prompt_edit_original_prefix,
			char *relation_name )
{
	char name[ 128 ];

	sprintf(name,
		"%s%s",
		form_prompt_edit_original_prefix,
		relation_name );

	return strdup( name );
}

char *form_prompt_edit_relation_element_name(
			char *form_prompt_edit_relation_prefix,
			char *relation_name )
{
	char name[ 128 ];

	sprintf(name,
		"%s%s",
		form_prompt_edit_relation_prefix,
		relation_name );

	return strdup( name );
}

char *form_prompt_edit_relation_prompt(
			char *one_folder_name,
			char *related_attribute_name,
			LIST *foreign_key_list,
			int primary_key_index )
{
	char prompt[ 128 ];
	char *ptr = prompt;
	char buffer1[ 64 ];
	char buffer2[ 64 ];

	if ( !one_folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: one_folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( primary_key_index ) ptr += sprintf( ptr, "* " );

	if ( list_length( foreign_key_list ) )
	{
		ptr += sprintf(
			ptr,
			"%s",
			list_display_delimited(
				foreign_key_list,
				',' ) );
	}
	else
	if ( related_attribute_name && *related_attribute_name )
	{
		ptr += sprintf(
			ptr,
			"%s (%s)",
			string_initial_capital(
				buffer1,
				one_folder_name ),
			string_initial_capital(
				buffer2,
				related_attribute_name ) );
	}
	else
	{
		ptr += sprintf(
			ptr,
			"%s",
			string_initial_capital(
				buffer1,
				one_folder_name ) );
	}

	return strdup( prompt );
}

char *form_prompt_edit_relation_element_name(
			char *form_prompt_edit_relation_prefix,
			char *relation_name )
{
	char name[ 128 ];

	sprintf(name,
		"%s%s",
		form_prompt_edit_relation_prefix,
		relation_name );

	return strdup( name );
}

FORM_PROMPT_EDIT_RELATION *form_prompt_edit_relation_calloc( void )
{
	FORM_PROMPT_EDIT_RELATION *form_prompt_edit_relation;

	if ( ! ( form_prompt_edit_relation =
			calloc( 1, sizeof( FORM_PROMPT_EDIT_RELATION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_prompt_edit_relation;
}

