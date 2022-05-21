/* $APPASERVER_HOME/library/remember.c					*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "remember.h"

void form_output_remember_keystrokes_button( char *control_string )
{
	printf(
"<td><input type=\"button\" value=\"Recall\" onClick=\"%s\">\n",
		control_string );
}

char *form_get_remember_keystrokes_onload_control_string(
			char *form_name,
			LIST *non_multi_element_name_list,
			LIST *multi_element_name_list,
			char *post_change_javascript,
			char *cookie_key_prefix,
			char *cookie_key )
{
	char buffer[ 4096 ];
	char *local_form_name;
	char cookie_key_buffer[ 128 ];

	form_build_cookie_key_buffer(
		cookie_key_buffer,
		cookie_key_prefix,
		cookie_key );

	local_form_name =
		(form_name && *form_name) 
			? form_name : "unknown";

	sprintf( buffer,
		 "keystrokes_onload(document.%s,'<%s>','%s','%c','%c');",
		 local_form_name,
		 cookie_key_buffer,
		 list_display_delimited(
				non_multi_element_name_list,
				FORM_KEYSTROKES_ELEMENT_NAME_DELIMITER ),
		 FORM_KEYSTROKES_ELEMENT_NAME_DELIMITER,
		 ELEMENT_MULTI_SELECT_MOVE_LEFT_RIGHT_INDEX_DELIMITER );

	if ( multi_element_name_list && list_length( multi_element_name_list ) )
	{
		sprintf( buffer + strlen( buffer ),
"keystrokes_multi_onload(document.%s,'<multi_%s>','%s','%c','%c','%c');",
		 	local_form_name,
		 	cookie_key_buffer,
		 	list_display_delimited(
				multi_element_name_list,
				FORM_KEYSTROKES_ELEMENT_NAME_DELIMITER ),
		 	FORM_KEYSTROKES_ELEMENT_NAME_DELIMITER,
			ELEMENT_MULTI_SELECT_MOVE_LEFT_RIGHT_INDEX_DELIMITER,
		 	ELEMENT_MULTI_SELECT_REMEMBER_DELIMITER );
	}

	if ( post_change_javascript
	&&   *post_change_javascript )
	{
		sprintf( buffer + strlen( buffer ),
			 "%s;",
			 form_set_post_change_javascript_row_zero(
				post_change_javascript ) );
	}

	return strdup( buffer );
}

/* Appends to form->submit_control_string */
/* -------------------------------------- */
void form_append_remember_keystrokes_submit_control_string(
				char **onclick_keystrokes_save_string,
				FORM *form,
				LIST *non_multi_element_name_list,
				LIST *multi_element_name_list,
				char *cookie_key_prefix,
				char *cookie_key )
{
	char buffer[ 4096 ];
	char *form_name;
	char cookie_key_buffer[ 128 ];

	form_build_cookie_key_buffer(	cookie_key_buffer,
					cookie_key_prefix,
					cookie_key );

	form_name = (form->form_name && *form->form_name) 
				? form->form_name : "unknown";

	sprintf( buffer,
		 "keystrokes_save(document.%s,'<%s>','%s','%c','%c') && ",
		 form_name,
		 cookie_key_buffer,
		 list_display_delimited(
				non_multi_element_name_list,
				FORM_KEYSTROKES_ELEMENT_NAME_DELIMITER ),
		 FORM_KEYSTROKES_ELEMENT_NAME_DELIMITER,
		 ELEMENT_MULTI_SELECT_MOVE_LEFT_RIGHT_INDEX_DELIMITER );

	if ( multi_element_name_list && list_length( multi_element_name_list ) )
	{
		sprintf( buffer + strlen( buffer ),
"keystrokes_multi_save(document.%s,'<multi_%s>','%s','%c','%c','%c') && ",
		 	form_name,
		 	cookie_key_buffer,
		 	list_display_delimited(
				multi_element_name_list,
				FORM_KEYSTROKES_ELEMENT_NAME_DELIMITER ),
		 	FORM_KEYSTROKES_ELEMENT_NAME_DELIMITER,
		 	ELEMENT_MULTI_SELECT_MOVE_LEFT_RIGHT_INDEX_DELIMITER,
		 	ELEMENT_MULTI_SELECT_REMEMBER_DELIMITER );
	}

	*onclick_keystrokes_save_string = strdup( buffer );

	form_append_submit_control_string(
		form->submit_control_string, buffer );

}
void form_build_cookie_key_buffer(	char *cookie_key_buffer,
					char *cookie_key_prefix,
					char *cookie_key )
{
	if ( cookie_key_prefix && cookie_key )
	{
		sprintf(	cookie_key_buffer,
				"%s_%s",
				cookie_key_prefix,
				cookie_key );
	}
	else
	if ( cookie_key )
	{
		strcpy( cookie_key_buffer, cookie_key );
	}
	else
	if ( cookie_key_prefix )
	{
		strcpy( cookie_key_buffer, cookie_key_prefix );
	}
	else
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty parameters.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
}
void form_set_new_button_onclick_keystrokes_save_string(
			LIST *element_list,
			char *onclick_keystrokes_save_string )
{
	ELEMENT_APPASERVER *element;

	if ( !list_rewind( element_list ) ) return;

	do {
		element = list_get_pointer( element_list );

		if ( element->element_type == toggle_button
		&&   timlib_strncmp(element->name,
				    VERTICAL_NEW_BUTTON_ONE_PREFIX ) == 0 )
		{
			element->toggle_button->onclick_keystrokes_save_string =
				onclick_keystrokes_save_string;
		}
	} while( list_next( element_list ) );
}
