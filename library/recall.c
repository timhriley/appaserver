/* $APPASERVER_HOME/library/recall.c				*/
/* ------------------------------------------------------------ */
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "element.h"
#include "recall.h"

RECALL *recall_calloc( void )
{
	RECALL *recall;

	if ( ! ( recall = calloc( 1, sizeof( RECALL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return recall;
}

RECALL *recall_new(	char *folder_name,
			char *state,
			char *form_name,
			LIST *appaserver_element_list )
{
	RECALL *recall = recall_calloc();

	recall->cookie_key =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		recall_cookie_key(
			folder_name,
			state );

	recall->cookie_multi_key =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		recall_cookie_multi_key(
			folder_name,
			state );

	recall->keystrokes_save_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		recall_keystrokes_save_javascript(
			form_name,
			recall->cookie_key,
			appaserver_element_list,
			RECALL_DELIMITER,
			ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER );

	recall->keystrokes_multi_save_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		recall_keystrokes_multi_save_javascript(
			form_name,
			recall->cookie_multi_key,
			appaserver_element_list,
			RECALL_DELIMITER,
			ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER );

	recall->keystrokes_load_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		recall_keystrokes_load_javascript(
			form_name,
			recall->cookie_key,
			appaserver_element_list,
			RECALL_DELIMITER,
			ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER );

	recall->keystrokes_multi_load_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		recall_keystrokes_multi_load_javascript(
			form_name,
			recall->cookie_multi_key,
			appaserver_element_list,
			RECALL_DELIMITER,
			ELEMENT_MULTI_MOVE_LEFT_RIGHT_DELIMITER );

	if ( recall->keystrokes_save_javascript
	||   recall->keystrokes_multi_save_javascript )
	{
		recall->save_javascript =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			recall_save_javascript(
				recall->keystrokes_save_javascript,
				recall->keystrokes_multi_save_javascript );
	}

	if ( recall->keystrokes_load_javascript
	||   recall->keystrokes_multi_load_javascript )
	{
		recall->load_javascript =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			recall_load_javascript(
				recall->keystrokes_load_javascript,
				recall->keystrokes_multi_load_javascript );
	}

	if ( recall->keystrokes_save_javascript )
	{
		free( recall->keystrokes_save_javascript );
	}

	if( recall->keystrokes_multi_save_javascript )
	{
		free( recall->keystrokes_multi_save_javascript );
	}

	if( recall->keystrokes_load_javascript )
	{
		free( recall->keystrokes_load_javascript );
	}

	if( recall->keystrokes_multi_load_javascript )
	{
		free( recall->keystrokes_multi_load_javascript );
	}

	return recall;
}

char *recall_cookie_key(
			char *folder_name,
			char *state )
{
	static char cookie_key[ 128 ];

	if ( folder_name && state )
	{
		sprintf(cookie_key,
			"<%s_%s>",
			folder_name,
			state );
	}
	else
	{
		strcpy( cookie_key, "<process>" );
	}

	return cookie_key;
}

char *recall_multi_cookie_key(
			char *folder_name,
			char *state )
{
	static char multi_cookie_key[ 128 ];

	if ( folder_name && state )
	{
		sprintf(multi_cookie_key,
			"<multi_%s_%s>",
			folder_name,
			state );
	}
	else
	{
		strcpy( multi_cookie_key, "<multi_process>" );
	}

	return multi_cookie_key;
}

char *recall_keystrokes_save_javascript(
			char *form_name,
			char *recall_cookie_key,
			LIST *appaserver_element_list,
			char recall_delimiter,
			char element_multi_move_left_right_delimiter )
{
	char javascript[ STRING_64K ];
	char *ptr = javascript;
	APPASERVER_ELEMENT *appaserver_element;
	register boolean first_time = 1;

	if ( !form_name
	||   !recall_cookie_key
	||   !recall_delimiter
	||   !element_multi_move_left_right_delimiter )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( appaserver_element_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		 "keystrokes_save(document.%s,'%s','",
		 form_name,
		 recall_cookie_key );

	do {
		appaserver_element =
			list_get(
				appaserver_element_list );

		if ( !appaserver_element_recall_boolean( appaserver_element )
		||    appaserver_element->element_type == multi_drop_down )
		{
			continue;
		}


		if ( first_time )
		{
			first_time = 0;
		}
		else
		{
			ptr += sprintf( ptr, "%c", recall_delimiter );
		}

		if ( !appaserver_element->element_name )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: for type = %d, element_name is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				appaserver_element->element_type );
			exit( 1 );
		}

		ptr += sprintf(
			ptr,
			"%s",
			appaserver_element->element_name );

	} while ( list_next( appaserver_element_list ) );

	if ( first_time ) return (char *)0;

	ptr += sprintf(
		ptr,
		"','%c','%c'",
		recall_delimiter,
		element_multi_move_left_right_delimiter );

	return strdup( javascript );
}

char *recall_keystrokes_multi_save_javascript(
			char *form_name,
			char *recall_cookie_multi_key,
			LIST *appaserver_element_list,
			char recall_delimiter,
			char element_multi_move_left_right_delimiter )
{
	char javascript[ STRING_64K ];
	char *ptr = javascript;
	APPASERVER_ELEMENT *appaserver_element;
	register boolean first_time = 1;

	if ( !form_name
	||   !recall_cookie_multi_key
	||   !recall_delimiter
	||   !element_multi_move_left_right_delimiter )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( appaserver_element_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		 "keystrokes_multi_save(document.%s,'%s','",
		 form_name,
		 recall_cookie_multi_key );

	do {
		appaserver_element =
			list_get(
				appaserver_element_list );

		if ( !appaserver_element_recall_boolean( appaserver_element )
		||    appaserver_element->element_type != multi_drop_down )
		{
			continue;
		}

		if ( first_time )
		{
			first_time = 0;
		}
		else
		{
			ptr += sprintf( ptr, "%c", recall_delimiter );
		}

		if ( !appaserver_element->element_name )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: for type = %d, element_name is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				appaserver_element->element_type );
			exit( 1 );
		}

		ptr += sprintf(
			ptr,
			"%s",
			appaserver_element->element_name );

	} while ( list_next( appaserver_element_list ) );

	if ( first_time ) return (char *)0;

	ptr += sprintf(
		ptr,
		"','%c','%c'",
		recall_delimiter,
		element_multi_move_left_right_delimiter );

	return strdup( javascript );
}

char *recall_keystrokes_load_javascript(
			char *form_name,
			char *recall_cookie_key,
			LIST *appaserver_element_list,
			char recall_delimiter,
			char element_multi_move_left_right_delimiter )
{
	char javascript[ STRING_64K ];
	char *ptr = javascript;
	APPASERVER_ELEMENT *appaserver_element;
	register boolean first_time = 1;

	if ( !form_name
	||   !recall_cookie_key
	||   !recall_delimiter
	||   !element_multi_move_left_right_delimiter )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( appaserver_element_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		 "keystrokes_onload(document.%s,'%s','",
		 form_name,
		 recall_cookie_key );

	do {
		appaserver_element =
			list_get(
				appaserver_element_list );

		if ( !appaserver_element_recall_boolean( appaserver_element )
		||    appaserver_element->element_type == multi_drop_down )
		{
			continue;
		}


		if ( first_time )
		{
			first_time = 0;
		}
		else
		{
			ptr += sprintf( ptr, "%c", recall_delimiter );
		}

		if ( !appaserver_element->element_name )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: for type = %d, element_name is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				appaserver_element->element_type );
			exit( 1 );
		}

		ptr += sprintf(
			ptr,
			"%s",
			appaserver_element->element_name );

	} while ( list_next( appaserver_element_list ) );

	if ( first_time ) return (char *)0;

	ptr += sprintf(
		ptr,
		"','%c','%c'",
		recall_delimiter,
		element_multi_move_left_right_delimiter );

	return strdup( javascript );
}

char *recall_keystrokes_multi_load_javascript(
			char *form_name,
			char *recall_cookie_multi_key,
			LIST *appaserver_element_list,
			char recall_delimiter,
			char element_multi_move_left_right_delimiter )
{
	char javascript[ STRING_64K ];
	char *ptr = javascript;
	APPASERVER_ELEMENT *appaserver_element;
	register boolean first_time = 1;

	if ( !form_name
	||   !recall_cookie_multi_key
	||   !recall_delimiter
	||   !element_multi_move_left_right_delimiter )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( appaserver_element_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		 "keystrokes_multi_onload(document.%s,'%s','",
		 form_name,
		 recall_cookie_multi_key );

	do {
		appaserver_element =
			list_get(
				appaserver_element_list );

		if ( !appaserver_element_recall_boolean( appaserver_element )
		||    appaserver_element->element_type != multi_drop_down )
		{
			continue;
		}

		if ( first_time )
		{
			first_time = 0;
		}
		else
		{
			ptr += sprintf( ptr, "%c", recall_delimiter );
		}

		if ( !appaserver_element->element_name )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: for type = %d, element_name is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				appaserver_element->element_type );
			exit( 1 );
		}

		ptr += sprintf(
			ptr,
			"%s",
			appaserver_element->element_name );

	} while ( list_next( appaserver_element_list ) );

	if ( first_time ) return (char *)0;

	ptr += sprintf(
		ptr,
		"','%c','%c','%c'",
		recall_delimiter,
		element_multi_move_left_right_delimiter,
		recall_delimiter /* multi_select_remember_delimiter */ );

	return strdup( javascript );
}

char *recall_save_javascript(
			char *keystrokes_save_javascript,
			char *keystrokes_multi_save_javascript )
{
	char javascript[ STRING_128K ];

	*javascript = '\0';

	if ( !keystrokes_save_javascript
	&&   !keystrokes_multi_save_javascript )
	{
		return (char *)0;
	}

	if ( keystrokes_save_javascript )
	{
		strcpy( javascript, keystrokes_save_javascript );
	}

	if ( keystrokes_multi_save_javascript )
	{
		if ( !*javascript )
		{
			strcpy( javascript, keystrokes_multi_save_javascript );
		}
		else
		{
			sprintf(javascript + strlen( javascript ),
				" && %s",
				keystrokes_multi_save_javascript );
		}
	}

	return strdup( javascript );
}

char *recall_load_javascript(
			char *keystrokes_load_javascript,
			char *keystrokes_multi_load_javascript )
{
	char javascript[ STRING_128K ];

	*javascript = '\0';

	if ( !keystrokes_load_javascript
	&&   !keystrokes_multi_load_javascript )
	{
		return (char *)0;
	}

	if ( keystrokes_load_javascript )
	{
		strcpy( javascript, keystrokes_load_javascript );
	}

	if ( keystrokes_multi_load_javascript )
	{
		if ( !*javascript )
		{
			strcpy( javascript, keystrokes_multi_load_javascript );
		}
		else
		{
			sprintf(javascript + strlen( javascript ),
				" && %s",
				keystrokes_multi_load_javascript );
		}
	}

	return strdup( javascript );
}

