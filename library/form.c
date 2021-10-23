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
#include "form.h"

#ifdef NOT_DEFINED
LIST *form_hydrology_validation_element_list(
			HASH_TABLE *attribute_hash_table,
			LIST *attribute_name_list )
{
	LIST *return_list;
	ATTRIBUTE *attribute;
	char *attribute_name;
	APPASERVER_ELEMENT *element = {0};

	return_list = list_new();

	/* For each attribute */
	/* ------------------ */
	if ( list_reset( attribute_name_list ) )
		do {
			attribute_name = list_get( attribute_name_list );

			attribute = (ATTRIBUTE *)hash_table_get( 
							attribute_hash_table,
							attribute_name );

			if ( !attribute )
			{
				fprintf(stderr,
				"%s cannot find attribute = (%s)\n",
					__FUNCTION__,
					attribute_name );
				exit( 1 );
			}

			/* Ignore station and datatype */
			/* --------------------------- */
			if ( strcmp( 	attribute->attribute_name,
					"station" ) == 0 
			||   strcmp(	attribute->attribute_name,
					"datatype" ) == 0 )
			{
				continue;
			}
			else
			/* ----------------------------------------- */
			/* Measurement value get a text item element */
			/* ----------------------------------------- */
			if ( strcmp( 	attribute->attribute_name, 
					"measurement_value" ) == 0 )
			{
				element =
					appaserver_element_new(
						text_item,
						attribute->attribute_name);

				element_text_item_set_attribute_width(
						element->text_item, 
						attribute->width );

				element_text_item_set_heading(
						element->text_item,
						attribute->attribute_name );
			}
			else
			/* --------------------------------------------- */
			/* Measurement date and time get hidden elements */
			/* --------------------------------------------- */
			if ( strcmp( 	attribute->attribute_name,
					"measurement_date" ) == 0 
			||   strcmp(	attribute->attribute_name,
					"measurement_time" ) == 0 )
			{
				element =
					element_appaserver_new(
						prompt_data,
						attribute->attribute_name );

				element->prompt_data->heading = element->name;

				list_append( 	return_list, 
						element, 
						sizeof( APPASERVER_ELEMENT ) );

				element =
					element_appaserver_new(
						hidden,
						attribute->attribute_name);
			}

			list_append( 	return_list, 
					element, 
					sizeof( APPASERVER_ELEMENT ) );

		} while( list_next( attribute_name_list ) );
	return return_list;
}
#endif

FORM_BUTTON *form_button_back_to_drillthru( char *action_string )
{
	FORM_BUTTON *form_button = form_button_calloc();
	char html[ 1024 ];
	char *ptr = html;

	/* Close form[0] */
	/* ------------- */
	ptr += sprintf(
		ptr,
		"</form>\n" );

	/* Open form[1] */
	/* ------------ */
	ptr += sprintf(
		ptr,
"<form enctype=\"multipart/form-data\" method=post action=\"%s\" target=%s>\n"
"<input type=button value=\"Back to Drillthru\" onClick=\"document.forms[1].submit()\"\n",
		action_string,
		PROMPT_FRAME );

	form_button->html = strdup( html );
	return form_button;
}

FORM_BUTTON *form_button_back_to_top( void )
{
	FORM_BUTTON *form_button = form_button_calloc();
	char html[ 256 ];

	sprintf(html,
"<td><a onClick=\"%s\"><img src=\"/%s/top.png\"></a>",
		"window.scrollTo(0,0)",
		IMAGE_RELATIVE_DIRECTORY );

	form_button->html = strdup( html );
	return form_button;
}

FORM_BUTTON *form_button_remember_html(
			char *action_string )
{
	FORM_BUTTON *form_button = form_button_calloc();
	char html[ 1024 ];

	sprintf(html,
"<td><input type=\"button\" value=\"Recall\" onClick=\"%s\">",
		action_string );

	form_button->html = strdup( html );
	return form_button;
}

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

FORM_BUTTON *form_button_back_forward( void )
{
	FORM_BUTTON *form_button = form_button_calloc();
	char html[ 1024 ];

	sprintf(html,
"<td><input type=\"button\" value=\"Back\" onClick=\"history.back()\">\n"
"<td><input type=\"button\" value=\"Forward\" onClick=\"timlib_history_forward()\">" );

	form_button->html = strdup( html );
	return form_button;
}

FORM_BUTTON *form_button_reset(
			int form_number,
			char *post_change_javascript )
{
	FORM_BUTTON *form_button = form_button_calloc();
	char html[ 1024 ];
	char *ptr = html;

	ptr += sprintf(
		ptr,
"<td><input type=\"button\" value=\"Reset\" onClick=\"form_reset(document.forms[%d], '%c')",
		form_number,
		ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER );

	if ( post_change_javascript && *post_change_javascript )
	{
		ptr += sprintf(
			ptr,
			"&& %s",
			javascript_replace_row(
				post_change_javascript,
			"0" /* row_string */ ) );
	}

	ptr += sprintf( ptr, "\">" );

	form_button->html = strdup( html );
	return form_button;
}

FORM_BUTTON *form_button_drillthru_skip( void )
{
	FORM_BUTTON *form_button = form_button_calloc();
	char html[ 1024 ];

	sprintf(html,
"<td><input type=\"button\" value=\"Skip\" title=\"Skip this form if you don't know exactly what you're looking for.\" onClick=\"form_reset(document.forms[0], '%c'); document.forms[0].submit()\">",
		ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER );

	form_button->html = strdup( html );
	return form_button;
}

FORM_BUTTON *form_button_html_help(
			char *application_name,
			char *html_help_file_anchor )
{
	FORM_BUTTON *form_button = form_button_calloc();
	char full_pathname[ 512 ];
	char *appaserver_mount_point;
	char *relative_source_directory;
	char source_directory[ 128 ];
	char source_directory_filename[ 512 ];
	int index;
	char html[ 1024 ];

	if ( !html_help_file_anchor || !*html_help_file_anchor )
		return (FORM_BUTTON *)0;

	appaserver_mount_point =
		appaserver_parameter_file_mount_point();

	relative_source_directory =
		application_relative_source_directory(
			application_name );

	for(	index = 0;
		piece(	source_directory,
			PATH_DELIMITER,
			relative_source_directory,
			index );
		index++ )
	{
		sprintf(source_directory_filename, 
		 	"%s/%s/%s",
		 	appaserver_mount_point,
		 	source_directory,
		 	html_help_file_anchor );

		if ( timlib_file_exists( source_directory_filename ) ) break;
	}

	if ( !*source_directory )
	{
		fprintf( stderr,
"Warning in %s/%s()/%d: cannot html_help_file_anchor = [%s] in any of (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 html_help_file_anchor,
			 relative_source_directory );
		return (FORM_BUTTON *)0;
	}

	if ( *html_help_file_anchor == '/' )
	{
		strcpy( full_pathname, html_help_file_anchor );
	}
	else
	{
		sprintf(	full_pathname,
				"/appaserver/%s/%s",
				source_directory,
				html_help_file_anchor );
	}

	sprintf(html,
"<td><input type=button value=Help onclick='window.open(\"%s\",\"help_window\",\"width=600,height=350,resizable=yes,scrollbars=yes\")'>",
		full_pathname );

	form_button->html = strdup( html );
	return form_button;
}

LIST *form_button_insert_pair_one2m_submit_list(
			LIST *pair_one2m_folder_list )
{
	PAIR_ONE2M_FOLDER *pair_one2m_folder;
	LIST *button_list;
	FORM_BUTTON *form_button;

	if ( !list_rewind( pair_one2m_folder_list ) )
	{
		return (LIST *)0;
	}

	button_list = list_new();

	do {
		pair_one2m_folder =
			list_get( 
				pair_one2m_folder_list );

		form_button = form_button_calloc();

		if ( !pair_one2m_folder->folder_button_string )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_button_string is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		form_button->html =
			pair_one2m_folder->
				folder_button_string;

		list_set(
			button_list,
			form_button );

	} while( list_next( pair_one2m_folder_list ) );

	return button_list;
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
			/* -------------------------- */
			/* Safely returns heap memory */
			/* -------------------------- */
			element_drop_down_name(
				primary_key_list /* element_name_list */,
				0 ),
			(char *)0 /* initial_data */,
			delimited_list,
			no_initial_capital,
			0 /* not output_null_option */,
			0 /* not output_not_null_option */,
			1 /* output_select_option */,
			1 /* column_span */,
			element_drop_down_size(
				list_length( delimited_list ) ),
			appaserver_element_tab_order( -1 ),
			0 /* not multi_select */,
			(char *)0 /* post_change_javascript */ );

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
		form_button_submit(
			0 /* form_number */ ) );

	list_set(
		button_list,
		form_button_reset(
			0 /* form_number */,
			(char *)0 /* post_change_javascript */ ) );

	list_set(
		button_list,
		form_button_back_forward() );

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

	form_button_list_output(
		output_stream,
		button_list );

	/* Close form then table */
	/* --------------------- */
	fprintf( output_stream, "</form>\n" );
	fprintf( output_stream, "</table>\n" );
}

FORM_BUTTON *form_button_submit(
			int form_number )
{
	FORM_BUTTON *form_button = form_button_calloc();
	char html[ 1024 ];

	sprintf(html,
"<td><input type=button value=\"|    Submit    |\" onClick=\"document.forms[%d].submit();\">",
		form_number );

	form_button->html = strdup( html );
	return form_button;
}

FORM_BUTTON *form_button_calloc( void )
{
	FORM_BUTTON *form_button;

	if ( ! ( form_button = calloc( 1, sizeof( FORM_BUTTON ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_button;
}

void form_button_list_output(
			FILE *output_stream,
			LIST *button_list )
{
	FORM_BUTTON *form_button;

	if ( !list_rewind( button_list ) ) return;

	fprintf(output_stream,
		"<table border=0>\n<tr>\n" );

	do {
		form_button = list_get( button_list );

		form_button_output(
			output_stream,
			form_button->html );

		fprintf(output_stream,
			"%s\n",
			form_button->html );

	} while ( list_next( button_list ) );

	fprintf(output_stream,
		"</table\n" );
}

void form_button_output(
			FILE *output_stream,
			char *html )
{
	if ( !html || !*html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: html is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	fprintf( output_stream, "%s\n", html );
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

