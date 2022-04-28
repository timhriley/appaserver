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
#include "button.h"
#include "appaserver.h"
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

char *form_close_html( void )
{
	return "</form>";
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
			element_list /* in/out */ ) ) )
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

char *form_verify_notepad_widths_javascript(
			char *form_name,
			LIST *element_list )
{
	APPASERVER_ELEMENT *element;
	char javascript[ 1024 ];
	char *ptr = javascript;
	boolean got_one = 0;

	if ( !list_rewind( element_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		"verify_attribute_widths('" );

	do {
		element = list_get( element_list );

		if ( element->element_type != notepad ) continue;

		if ( !element->notepad->attribute_size ) continue;

		if ( !got_one )
			got_one = 1;
		else
			ptr += sprintf( ptr, "," );

		ptr += sprintf( ptr, "%s", element->notepad->attribute_name );

	} while ( list_next( element_list ) );

	if ( !got_one ) return (char *)0;

	ptr += sprintf( ptr, "','" );
	got_one = 0;

	list_rewind( element_list );

	do {
		element_list = list_get( element_list );

		if ( element->element_type != notepad ) continue;

		if ( !element->notepad->attribute_size ) continue;

		if ( !got_one )
			got_one = 1;
		else
			ptr += sprintf( ptr, "," );

		ptr += sprintf( ptr, "%d", element->notepad->attribute_size );

	} while( list_next( element_list ) );

	ptr += sprintf(
		ptr,
		"','%s')",
		form_name );

	return strdup( javascript );
}

char *form_cookie_key(	char *form_name,
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

char *form_cookie_multi_key(
			char *form_name,
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

char *form_keystrokes_recall_javascript(
			char *form_name,
			char *form_cookie_key,
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
		form_cookie_key );

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
				"%s",
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

char *form_keystrokes_multi_recall_javascript(
			char *form_name,
			char *form_cookie_multi_key,
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
		form_cookie_multi_key );

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
				"%s",
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

char *form_keystrokes_save_javascript(
			char *form_name,
			char *form_cookie_key,
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
		form_cookie_key );

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
				"%s",
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

char *form_keystrokes_multi_save_javascript(
			char *form_name,
			char *form_cookie_multi_key,
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
		form_cookie_multi_key );

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
				"%s",
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

char *form_multi_select_all_javascript(
			LIST *element_list )
{
	APPASERVER_ELEMENT *element;
	char javascript[ 1024 ];
	char *ptr = javascript;
	boolean got_one = 0;

	if ( !list_rewind( element_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		"post_change_multi_select_all('" );

	do {
		element = list_get( element_list );

		if ( element->element_type != multi_drop_down ) continue;

		if ( !element->element_name )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !got_one )
			got_one = 1;
		else
			ptr += sprintf( ptr, "," );

		ptr += sprintf( ptr, "%s", element->element_name );

	} while ( list_next( element_list ) );

	if ( !got_one )
		return (char *)0;
	else
	{
		ptr += sprintf( ptr, "')" );
		return strdup( javascript );
	}
}

char *form_ignore_name(	char *form_ignore_prefix,
			char *relation_name )
{
	char name[ 128 ];

	if ( !form_ignore_prefix
	||   !relation_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(name,
		"%s%s",
		form_ignore_prefix,
		relation_name );

	return strdup( name );
}

char *form_no_display_name(
			char *form_no_display_prefix,
			char *attribute_name )
{
	char name[ 128 ];

	if ( !form_no_display_prefix
	||   !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(name,
		"%s%s",
		form_no_display_prefix,
		attribute_name );

	return strdup( name );
}

char *form_background_color( void )
{
	static int cycle_count = 0;
	static char **background_color_array = {0};
	static int background_color_array_length = 0;
	char *background_color;

	if ( !background_color_array )
	{
		background_color_array =
			form_background_color_array(
				&background_color_array_length );
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

char **form_background_color_array(
			int *background_color_array_length )
{
	static char **background_color_array;
	char *application_constants_color;
	APPLICATION_CONSTANTS *application_constants;

	background_color_array =
		(char **)calloc(FORM_MAX_BACKGROUND_COLOR_ARRAY,
				sizeof( char * ) );

	application_constants = application_constants_new();

	application_constants->dictionary =
		application_constants_dictionary();

	if ( ( application_constants_color =
		application_constants_fetch(
			application_constants->dictionary,
			"color1" ) ) )
	{
		background_color_array[ 0 ] = application_constants_color;
		*background_color_array_length = 1;

		if ( ( application_constants_color =
			application_constants_fetch(
				application_constants->dictionary,
				"color2" ) ) )
		{
			background_color_array[ 1 ] =
				application_constants_color;
			*background_color_array_length = 2;
		}
		else
		{
			background_color_array[ 1 ] = "white";
		}

		if ( ( application_constants_color =
			application_constants_fetch(
				application_constants->dictionary,
				"color3" ) ) )
		{
			background_color_array[ 2 ] =
				application_constants_color;
			*background_color_array_length = 3;
		}
		else
		{
			background_color_array[ 2 ] = "white";
		}


		if ( ( application_constants_color =
			application_constants_fetch(
				application_constants->dictionary,
				"color4" ) ) )
		{
			background_color_array[ 3 ] =
				application_constants_color;
			*background_color_array_length = 4;
		}
		else
		{
			background_color_array[ 3 ] = "white";
		}

		if ( ( application_constants_color =
			application_constants_fetch(
				application_constants->dictionary,
				"color5" ) ) )
		{
			background_color_array[ 4 ] =
				application_constants_color;
			*background_color_array_length = 5;
		}
		else
		{
			background_color_array[ 4 ] = "white";
		}


		if ( ( application_constants_color =
			application_constants_fetch(
				application_constants->dictionary,
				"color6" ) ) )
		{
			background_color_array[ 5 ] =
				application_constants_color;
			*background_color_array_length = 6;
		}
		else
		{
			background_color_array[ 5 ] = "white";
		}


		if ( ( application_constants_color =
			application_constants_fetch(
				application_constants->dictionary,
				"color7" ) ) )
		{
			background_color_array[ 6 ] =
				application_constants_color;
			*background_color_array_length = 7;
		}
		else
		{
			background_color_array[ 6 ] = "white";
		}


		if ( ( application_constants_color =
			application_constants_fetch(
				application_constants->dictionary,
				"color8" ) ) )
		{
			background_color_array[ 7 ] =
				application_constants_color;
			*background_color_array_length = 8;
		}
		else
		{
			background_color_array[ 7 ] = "white";
		}


		if ( ( application_constants_color =
			application_constants_fetch(
				application_constants->dictionary,
				"color9" ) ) )
		{
			background_color_array[ 8 ] =
				application_constants_color;
			*background_color_array_length = 9;
		}
		else
		{
			background_color_array[ 8 ] = "white";
		}


		if ( ( application_constants_color =
			application_constants_fetch(
				application_constants->dictionary,
				"color10" ) ) )
		{
			background_color_array[ 9 ] =
				application_constants_color;
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

