/* APPASERVER_HOME/library/vertical_new_button.c			*/
/* -------------------------------------------------------------------- */
/* This controls the vertical new buttons on the prompt insert form.	*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include "timlib.h"
#include "String.h"
#include "element.h"
#include "application.h"
#include "appaserver.h"
#include "dictionary.h"
#include "frameset.h"
#include "folder_menu.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "appaserver_link.h"
#include "vertical_new_button.h"

VERTICAL_NEW_BUTTON *vertical_new_button_calloc( void )
{
	VERTICAL_NEW_BUTTON *vertical_new_button;

	if ( ! ( vertical_new_button =
			calloc( 1, sizeof ( VERTICAL_NEW_BUTTON ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return vertical_new_button;
}

char *vertical_new_button_dictionary_one_folder_name(
			char *vertical_new_button_one_prefix,
			DICTIONARY *non_prefixed_dictionary )
{
	LIST *key_list;
	char *key;
	char *data;
	char return_key[ 128 ];

	if ( !non_prefixed_dictionary ) return (char *)0;

	key_list =
		dictionary_extract_prefixed_key_list(
			non_prefixed_dictionary,
			vertical_new_button_one_prefix );

	if ( !list_rewind( key_list ) ) return (char *)0;

	do {
		key = list_get( key_list );

		data =
			dictionary_get(
				key,
				non_prefixed_dictionary );

		if ( string_strcmp( data, "yes" ) == 0 )
		{
			return strdup(
				trim_index(
				return_key,
				key +
				strlen( vertical_new_button_one_prefix ) ) );
		}
	} while( list_next( key_list ) );

	return (char *)0;
}

char *vertical_new_button_one_element_name(
			char *vertical_new_button_one_prefix,
			char *one_folder_name )
{
	char element_name[ 128 ];

	sprintf( element_name,
		 "%s%s",
		 vertical_new_button_one_prefix,
		 one_folder_name );

	return strdup( element_name );
}

/*
APPASERVER_ELEMENT *vertical_new_button_element(
			char *one_folder_name,
			LIST *role_folder_insert_list,
			char *form_name,
			char *vertical_new_button_one_prefix,
			char *vertical_new_button_display )
{
	APPASERVER_ELEMENT *element;

	if ( list_exists_string(
		one_folder_name,
		role_folder_insert_list ) )
	{
		element =
			element_appaserver_new(
				toggle_button, 
				vertical_new_button_one_element_name(
					vertical_new_button_one_prefix,
					one_folder_name ) );

		element_toggle_button_set_heading(
			element->toggle_button,
			vertical_new_button_display );

		element->toggle_button->form_name = form_name;
		element->toggle_button->onchange_submit_yn = 'y';
	
		return element;
	}
	else
	{
		return (APPASERVER_ELEMENT *)0;
	}
}
*/

char *vertical_new_button_dictionary_folder_name(
			char *hidden_label,
			DICTIONARY *non_prefixed_dictionary )
{
	return dictionary_get(
			hidden_label,
			non_prefixed_dictionary );
}

void vertical_new_button_dictionary_set(
			DICTIONARY *non_prefixed_dictionary,
			char *hidden_label,
			char *folder_name )
{
	dictionary_set_pointer(
		non_prefixed_dictionary,
		hidden_label,
		folder_name );
}

char *vertical_new_button_onload_control_string( char *prompt_filename )
{
	char onload_control_string[ 128 ];

	sprintf(onload_control_string,
		"window.open('%s','%s');",
		prompt_filename,
		PROMPT_FRAME );

	return strdup( onload_control_string );

}

char *vertical_new_button_blank_prompt_screen(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *document_root_directory,
			char *appaserver_data_directory )
{
	char *prompt_filename;
	char *output_filename;
	APPASERVER_LINK *appaserver_link;
	DOCUMENT *document;
	FILE *output_stream;

	appaserver_link =
		appaserver_link_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			"blank_screen" /* filename_stem */,
			application_name,
			0 /* process_id */,
			session_key,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"html" );

	output_filename = appaserver_link->output->filename;

	prompt_filename = appaserver_link->prompt->filename;

	if ( ! ( output_stream = fopen( output_filename, "w" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: fopen(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			output_filename );
		exit( 1 );
	}

	if ( appaserver_frameset_menu_horizontal(
		application_name,
		login_name ) )
	{
		FOLDER_MENU *folder_menu;

		if ( ! ( folder_menu =
				folder_menu_fetch(
					application_name,
					session_key,
					appaserver_data_directory,
					role_name ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_menu_fetch() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		document =
			/* --------------- */
			/* Always succeeds */
			/* --------------- */
			document_new(
				application_name,
				menu_fetch(
					application_name,
					login_name,
					session_key,
					role_name,
					FRAMESET_PROMPT_FRAME
						/* target_frame */,
					folder_menu->lookup_count_list ) );
	}
	else
	{
		document =
			/* --------------- */
			/* Always succeeds */
			/* --------------- */
			document_new(
				application_name,
				(MENU *)0 /* menu */ );
	}

	document_begin( output_stream, document );

	fprintf(output_stream,
		"%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_close_html() );

	fclose( output_stream );

	return prompt_filename;
}
