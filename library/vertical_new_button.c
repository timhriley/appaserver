/* APPASERVER_HOME/library/vertical_new_button.c			*/
/* -------------------------------------------------------------------- */
/* This controls the vertical new buttons on the prompt insert form.	*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include "timlib.h"
#include "String.h"
#include "element.h"
#include "dictionary.h"
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
			dictionary_get_data(
				non_prefixed_dictionary,
				key );

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

char *vertical_new_button_dictionary_folder_name(
			char *hidden_label,
			DICTIONARY *non_prefixed_dictionary )
{
	return dictionary_get_data(
			non_prefixed_dictionary,
			hidden_label );
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
			char *document_root_directory )
{
	char sys_string[ 1024 ];
	char *prompt_filename;
	char *output_filename;
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			"blank_screen" /* filename_stem */,
			application_name,
			0 /* process_id */,
			session,
			"html" );

	output_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	prompt_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	if ( appaserver_frameset_menu_horizontal(
		application_name,
		login_name ) )
	{
		sprintf(sys_string,
"output_choose_role_folder_process_form '%s' '%s' '%s' '%s' '' n n > %s 2>>%s",
				application_name,
				session,
				login_name,
				role->role_name,
				output_filename,
				appaserver_error_filename(
					application_name ) );
		}
		else
		{
			sprintf(sys_string,
		 		"output_blank_screen.sh '%s' '' n > %s 2>>%s",
		 		application_background_color(
					application_name ),
				output_filename,
		 		appaserver_error_get_filename(
					application_name ) );
		}

		if ( system( sys_string ) ) {};

	return prompt_filename;
}
