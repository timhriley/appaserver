/* --------------------------------------------- */
/* $APPASERVER_HOME/library/cookie.c		 */
/* --------------------------------------------- */
/*						 */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "element.h"
#include "String.h"
#include "cookie.h"

char *cookie_key(	char *form_name,
			char *folder_name )
{
	static char key[ 128 ];

	if ( !form_name || !folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(key,
		"<%s_%s>",
		form_name,
		folder_name );

	return key;
}

char *cookie_multi_key(	char *form_name,
			char *folder_name )
{
	static char key[ 128 ];

	if ( !form_name || !folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(key,
		"<multi_%s_%s>",
		form_name,
		folder_name );

	return key;
}

char *cookie_keystrokes_recall_string(
			char *form_name,
			char *cookie_key,
			LIST *element_list )
{
	APPASERVER_ELEMENT *element;
	char string[ STRING_64K ];
	char *ptr = string;
	boolean found_one = 0;

	if ( !list_rewind( element_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		"keystrokes_onload(document.%s,'%s','",
		form_name,
		cookie_key );

	do {
		element = list_get( element_list );

		if ( element->element_type == multi_drop_down )
			continue;

		if ( appaserver_element_recall_boolean( element ) )
		{
			if ( !element->element_name )
			{
				fprintf(stderr,
		"ERROR in %s/%s()/%d: for type = %d, element_name is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					element->element_type );
				exit( 1 );
			}

			if ( !found_one )
				found_one = 1;
			else
				ptr += sprintf(
					ptr,
					"%c",
					ELEMENT_KEYSTROKES_NAME_DELIMITER );

			ptr += sprintf(
				ptr,
				"%s_0",
				element->element_name );
		}

	} while( list_next( element_list ) );

	if ( !found_one )
	{
		return (char *)0;
	}
	else
	{
		ptr += sprintf(
			ptr,
			",'%c','%c')",
		 	ELEMENT_KEYSTROKES_NAME_DELIMITER,
		 	ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER );

		return strdup( string );
	}
}

char *cookie_keystrokes_multi_recall_string(
			char *form_name,
			char *cookie_key,
			LIST *element_list )
{
	APPASERVER_ELEMENT *element;
	char string[ STRING_64K ];
	char *ptr = string;
	boolean found_one = 0;

	if ( !list_rewind( element_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		"keystrokes_multi_onload(document.%s,'%s','",
		form_name,
		cookie_key );

	do {
		element = list_get( element_list );

		if ( element->element_type != multi_drop_down )
			continue;

		if ( appaserver_element_recall_boolean( element ) )
		{
			if ( !element->element_name )
			{
				fprintf(stderr,
		"ERROR in %s/%s()/%d: for type = %d, element_name is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					element->element_type );
				exit( 1 );
			}

			if ( !found_one )
				found_one = 1;
			else
				ptr += sprintf(
					ptr,
					"%c",
					ELEMENT_KEYSTROKES_NAME_DELIMITER );

			ptr += sprintf(
				ptr,
				"%s_1",
				element->element_name );
		}

	} while( list_next( element_list ) );

	if ( !found_one )
	{
		return (char *)0;
	}
	else
	{
		ptr += sprintf(
			ptr,
			",'%c','%c','%c')",
		 	ELEMENT_KEYSTROKES_NAME_DELIMITER,
		 	ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER,
		 	ELEMENT_MULTI_SELECT_REMEMBER_DELIMITER );

		return strdup( string );
	}
}

char *cookie_keystrokes_save_string(
			char *form_name,
			char *cookie_key,
			LIST *element_list )
{
	APPASERVER_ELEMENT *element;
	char string[ STRING_64K ];
	char *ptr = string;
	boolean found_one = 0;

	if ( !list_rewind( element_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		"keystrokes_save(document.%s,'%s','",
		form_name,
		cookie_key );

	do {
		element = list_get( element_list );

		if ( element->element_type == multi_drop_down )
			continue;

		if ( appaserver_element_recall_boolean( element ) )
		{
			if ( !element->element_name )
			{
				fprintf(stderr,
		"ERROR in %s/%s()/%d: for type = %d, element_name is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					element->element_type );
				exit( 1 );
			}

			if ( !found_one )
				found_one = 1;
			else
				ptr += sprintf(
					ptr,
					"%c",
					ELEMENT_KEYSTROKES_NAME_DELIMITER );

			ptr += sprintf(
				ptr,
				"%s_0",
				element->element_name );
		}

	} while( list_next( element_list ) );

	if ( !found_one )
	{
		return (char *)0;
	}
	else
	{
		ptr += sprintf(
			ptr,
			",'%c','%c')",
		 	ELEMENT_KEYSTROKES_NAME_DELIMITER,
		 	ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER );

		return strdup( string );
	}
}

char *cookie_keystrokes_multi_save_string(
			char *form_name,
			char *cookie_key,
			LIST *element_list )
{
	APPASERVER_ELEMENT *element;
	char string[ STRING_64K ];
	char *ptr = string;
	boolean found_one = 0;

	if ( !list_rewind( element_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		"keystrokes_multi_save(document.%s,'%s','",
		form_name,
		cookie_key );

	do {
		element = list_get( element_list );

		if ( element->element_type != multi_drop_down )
			continue;

		if ( appaserver_element_recall_boolean( element ) )
		{
			if ( !element->element_name )
			{
				fprintf(stderr,
		"ERROR in %s/%s()/%d: for type = %d, element_name is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					element->element_type );
				exit( 1 );
			}

			if ( !found_one )
				found_one = 1;
			else
				ptr += sprintf(
					ptr,
					"%c",
					ELEMENT_KEYSTROKES_NAME_DELIMITER );

			ptr += sprintf(
				ptr,
				"%s_1",
				element->element_name );
		}

	} while( list_next( element_list ) );

	if ( !found_one )
	{
		return (char *)0;
	}
	else
	{
		ptr += sprintf(
			ptr,
			",'%c','%c','%c')",
		 	ELEMENT_KEYSTROKES_NAME_DELIMITER,
		 	ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER,
		 	ELEMENT_MULTI_SELECT_REMEMBER_DELIMITER );

		return strdup( string );
	}
}

