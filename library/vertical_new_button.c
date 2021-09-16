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

char *vertical_new_button_many_element_name(
			char *vertical_new_button_many_label )
{
	return vertical_new_button_many_label;
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

