/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/recall.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "widget.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "recall.h"

RECALL_SAVE *recall_save_calloc( void )
{
	RECALL_SAVE *recall_save;

	if ( ! ( recall_save = calloc( 1, sizeof ( RECALL_SAVE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return recall_save;
}

RECALL_LOAD *recall_load_calloc( void )
{
	RECALL_LOAD *recall_load;

	if ( ! ( recall_load = calloc( 1, sizeof ( RECALL_LOAD ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return recall_load;
}

RECALL_SAVE *recall_save_new(
		char *state,
		char *form_name,
		char *folder_name,
		LIST *widget_container_list,
		boolean application_ssl_support_boolean )
{
	RECALL_SAVE *recall_save;

	recall_save = recall_save_calloc();

	recall_save->cookie_key =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		recall_save_cookie_key(
			RECALL_PROCESS_COOKIE_KEY,
			folder_name,
			state );

	recall_save->keystrokes_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		recall_save_keystrokes_javascript(
			RECALL_DELIMITER,
			WIDGET_MULTI_LEFT_RIGHT_DELIMITER,
			form_name,
			widget_container_list,
			application_ssl_support_boolean,
			recall_save->cookie_key );

	recall_save->cookie_multi_key =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		recall_save_cookie_multi_key(
			folder_name,
			state );

	recall_save->keystrokes_multi_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		recall_save_keystrokes_multi_javascript(
			RECALL_DELIMITER,
			WIDGET_MULTI_LEFT_RIGHT_DELIMITER,
			form_name,
			widget_container_list,
			application_ssl_support_boolean,
			recall_save->cookie_multi_key );

	if ( recall_save->keystrokes_javascript
	||   recall_save->keystrokes_multi_javascript )
	{
		recall_save->javascript =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			recall_load_javascript(
				recall_save->keystrokes_javascript,
				recall_save->keystrokes_multi_javascript );
	}

	return recall_save;
}

char *recall_save_cookie_key(
		char *recall_process_cookie_key,
		char *folder_name,
		char *state )
{
	static char cookie_key[ 128 ];

	if ( folder_name && state )
	{
		sprintf(cookie_key,
			"%s_%s",
			folder_name,
			state );
	}
	else
	{
		strcpy( cookie_key, recall_process_cookie_key );
	}

	return cookie_key;
}

char *recall_save_multi_cookie_key(
		char *folder_name,
		char *state )
{
	static char multi_cookie_key[ 128 ];

	if ( folder_name && state )
	{
		sprintf(multi_cookie_key,
			"multi_%s_%s",
			folder_name,
			state );
	}
	else
	{
		strcpy( multi_cookie_key, "multi_process" );
	}

	return multi_cookie_key;
}

char *recall_save_keystrokes_open(
		char *form_name,
		boolean application_ssl_support_boolean,
		char *recall_save_cookie_key )
{
	static char open[ 256 ];

	if ( !form_name
	||   !recall_save_cookie_key )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(open,
		"keystrokes_save(%s,'%s','%s','%s','",
		form_name,
		recall_save_cookie_key,
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		recall_save_samesite_string(
			application_ssl_support_boolean ),
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		recall_save_secure_string(
			application_ssl_support_boolean ) );

	return open;
}

char *recall_save_keystrokes_close(
		char recall_delimiter,
		char widget_multi_left_right_delimiter )
{
	static char close[ 16 ];

	if ( !recall_delimiter
	||   !widget_multi_left_right_delimiter )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(close,
		"','%c','%c')",
		recall_delimiter,
		widget_multi_left_right_delimiter );

	return close;
}

char *recall_save_keystrokes_javascript(
		const char recall_delimiter,
		const char widget_multi_left_right_delimiter,
		char *form_name,
		LIST *widget_container_list,
		boolean application_ssl_support_boolean,
		char *recall_save_cookie_key )
{
	char javascript[ STRING_64K ];
	char *ptr = javascript;
	WIDGET_CONTAINER *widget_container;
	boolean got_one = 0;

	if ( !form_name
	||   !recall_save_cookie_key )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( widget_container_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		"%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		recall_save_keystrokes_open(
			form_name,
			application_ssl_support_boolean,
			recall_save_cookie_key ) );

	do {
		widget_container = list_get( widget_container_list );

		if ( recall_ignore_boolean(
			APPASERVER_EXECUTE_YN,
			widget_container->widget_name,
			widget_container->widget_type,
			widget_container->recall_boolean,
			0 /* not multi_drop_down_boolean */ ) )
		{
			continue;
		}

		if (	string_strlen( javascript ) +
			string_strlen( widget_container->widget_name ) + 1 >=
			STRING_64K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_64K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !got_one )
		{
			got_one = 1;
		}
		else
		{
			ptr += sprintf( ptr, "%c", recall_delimiter );
		}

		ptr += sprintf(
			ptr,
			"%s",
			widget_container->widget_name );

	} while ( list_next( widget_container_list ) );

	if ( !got_one ) return (char *)0;

	ptr += sprintf(
		ptr,
		"%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		recall_save_keystrokes_close(
			recall_delimiter,
			widget_multi_left_right_delimiter ) );

	return strdup( javascript );
}

char *recall_save_keystrokes_multi_open(
		char *form_name,
		boolean application_ssl_support_boolean,
		char *recall_save_cookie_multi_key )
{
	static char open[ 256 ];

	if ( !form_name
	||   !recall_save_cookie_multi_key )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(open,
		"keystrokes_multi_save(%s,'%s','%s','%s','",
		form_name,
		recall_save_cookie_multi_key,
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		recall_save_samesite_string(
			application_ssl_support_boolean ),
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		recall_save_secure_string(
			application_ssl_support_boolean ) );

	return open;
}

char *recall_save_keystrokes_multi_close(
		char recall_delimiter,
		char widget_multi_left_right_delimiter )
{
	static char close[ 16 ];

	if ( !recall_delimiter
	||   !widget_multi_left_right_delimiter )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(close,
		"','%c','%c','%c')",
		recall_delimiter,
		widget_multi_left_right_delimiter,
		recall_delimiter /* multi_select_remember_delimiter */ );

	return close;
}

char *recall_save_keystrokes_multi_javascript(
		const char recall_delimiter,
		const char widget_multi_left_right_delimiter,
		char *form_name,
		LIST *widget_container_list,
		boolean application_ssl_support_boolean,
		char *recall_save_cookie_multi_key )
{
	char javascript[ STRING_64K ];
	char *ptr = javascript;
	WIDGET_CONTAINER *widget_container;
	boolean got_one = 0;

	if ( !form_name
	||   !recall_save_cookie_multi_key )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( widget_container_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		"%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		recall_save_keystrokes_multi_open(
		 	form_name,
			application_ssl_support_boolean,
		 	recall_save_cookie_multi_key ) );

	do {
		widget_container = list_get( widget_container_list );

		if ( recall_ignore_boolean(
			APPASERVER_EXECUTE_YN,
			widget_container->widget_name,
			widget_container->widget_type,
			widget_container->recall_boolean,
			1 /* multi_drop_down_boolean */ ) )
		{
			continue;
		}

		if (	string_strlen( javascript ) +
			string_strlen( widget_container->widget_name ) + 1 >=
			STRING_64K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_64K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !got_one )
		{
			got_one = 1;
		}
		else
		{
			ptr += sprintf( ptr, "%c", recall_delimiter );
		}

		ptr += sprintf(
			ptr,
			"%s",
			widget_container->widget_name );

	} while ( list_next( widget_container_list ) );

	if ( !got_one ) return (char *)0;

	ptr += sprintf(
		ptr,
		"%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		recall_save_keystrokes_multi_close(
			recall_delimiter,
			widget_multi_left_right_delimiter ) );

	return strdup( javascript );
}

char *recall_load_keystrokes_open(
		char *form_name,
		char *recall_save_cookie_key )
{
	static char open[ 128 ];

	if ( !form_name
	||   !recall_save_cookie_key )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(open,
		"keystrokes_onload(%s,'%s','",
		form_name,
		recall_save_cookie_key );

	return open;
}

char *recall_load_keystrokes_close(
		char recall_delimiter,
		char widget_multi_left_right_delimiter )
{
	static char close[ 16 ];

	if ( !recall_delimiter
	||   !widget_multi_left_right_delimiter )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(close,
		"','%c','%c')",
		recall_delimiter,
		widget_multi_left_right_delimiter );

	return close;
}

char *recall_load_keystrokes_javascript(
		const char recall_delimiter,
		const char widget_multi_left_right_delimiter,
		char *form_name,
		LIST *widget_container_list,
		char *recall_save_cookie_key )
{
	char javascript[ STRING_64K ];
	char *ptr = javascript;
	WIDGET_CONTAINER *widget_container;
	boolean got_one = 0;

	if ( !form_name
	||   !recall_save_cookie_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( widget_container_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		"%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		recall_load_keystrokes_open(
			form_name,
			recall_save_cookie_key ) );

	do {
		widget_container = list_get( widget_container_list );

		if ( recall_ignore_boolean(
			APPASERVER_EXECUTE_YN,
			widget_container->widget_name,
			widget_container->widget_type,
			widget_container->recall_boolean,
			0 /* not multi_drop_down_boolean */ ) )
		{
			continue;
		}

		if (	string_strlen( javascript ) +
			string_strlen( widget_container->widget_name ) + 1 >=
			STRING_64K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_64K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !got_one )
		{
			got_one = 1;
		}
		else
		{
			ptr += sprintf( ptr, "%c", recall_delimiter );
		}

		ptr += sprintf(
			ptr,
			"%s",
			widget_container->widget_name );

	} while ( list_next( widget_container_list ) );

	if ( !got_one ) return (char *)0;

	ptr += sprintf(
		ptr,
		"%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		recall_load_keystrokes_close(
			recall_delimiter,
			widget_multi_left_right_delimiter ) );

	return strdup( javascript );
}

char *recall_load_keystrokes_multi_javascript(
		const char recall_delimiter,
		const char widget_multi_left_right_delimiter,
		char *form_name,
		LIST *widget_container_list,
		char *recall_save_cookie_multi_key )
{
	char javascript[ STRING_64K ];
	char *ptr = javascript;
	WIDGET_CONTAINER *widget_container;
	boolean got_one = 0;

	if ( !recall_delimiter
	||   !widget_multi_left_right_delimiter
	||   !form_name
	||   !recall_save_cookie_multi_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( widget_container_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		"%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		recall_load_keystrokes_multi_open(
			form_name,
			recall_save_cookie_multi_key ) );

	do {
		widget_container = list_get( widget_container_list );

		if ( recall_ignore_boolean(
			APPASERVER_EXECUTE_YN,
			widget_container->widget_name,
			widget_container->widget_type,
			widget_container->recall_boolean,
			1 /* multi_drop_down_boolean */ ) )
		{
			continue;
		}

		if (	string_strlen( javascript ) +
			string_strlen( widget_container->widget_name ) + 1 >=
			STRING_64K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_64K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !got_one )
		{
			got_one = 1;
		}
		else
		{
			ptr += sprintf( ptr, "%c", recall_delimiter );
		}

		ptr += sprintf(
			ptr,
			"%s",
			widget_container->widget_name );

	} while ( list_next( widget_container_list ) );

	if ( !got_one ) return (char *)0;

	ptr += sprintf(
		ptr,
		"%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		recall_load_keystrokes_multi_close(
			recall_delimiter,
			widget_multi_left_right_delimiter ) );

	return strdup( javascript );
}

char *recall_load_javascript(
		char *recall_load_keystrokes_javascript,
		char *recall_load_keystrokes_multi_javascript )
{
	char javascript[ STRING_128K ];

	*javascript = '\0';

	if ( !recall_load_keystrokes_javascript
	&&   !recall_load_keystrokes_multi_javascript )
	{
		return (char *)0;
	}

	if (	string_strlen( recall_load_keystrokes_javascript ) +
		string_strlen( recall_load_keystrokes_multi_javascript ) + 1
		>= STRING_128K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_128K );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( recall_load_keystrokes_javascript )
	{
		strcpy( javascript, recall_load_keystrokes_javascript );
	}

	if ( recall_load_keystrokes_multi_javascript )
	{
		if ( !*javascript )
		{
			strcpy(
				javascript,
				recall_load_keystrokes_multi_javascript );
		}
		else
		{
			sprintf(javascript + strlen( javascript ),
				" && %s",
				recall_load_keystrokes_multi_javascript );
		}
	}

	return strdup( javascript );
}

char *recall_save_cookie_multi_key(
		char *folder_name,
		char *state )
{
	static char multi_key[ 128 ];

	if ( folder_name && state )
	{
		sprintf(multi_key,
			"multi_%s_%s",
			folder_name,
			state );
	}
	else
	{
		strcpy( multi_key, "multi_process" );
	}

	return multi_key;
}

char *recall_load_keystrokes_multi_open(
		char *form_name,
		char *recall_save_cookie_multi_key )
{
	static char open[ 128 ];

	if ( !form_name
	||   !recall_save_cookie_multi_key )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(open,
		 "keystrokes_multi_onload(%s,'%s','",
		 form_name,
		 recall_save_cookie_multi_key );

	return open;
}

char *recall_load_keystrokes_multi_close(
		char recall_delimiter,
		char widget_multi_left_right_delimiter )
{
	static char close[ 16 ];

	if ( !recall_delimiter
	||   !widget_multi_left_right_delimiter )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(close,
		"','%c','%c','%c')",
		recall_delimiter,
		widget_multi_left_right_delimiter,
		recall_delimiter /* multi_select_remember_delimiter */ );

	return close;
}

RECALL_LOAD *recall_load_new(
		char *state,
		char *form_name,
		char *folder_name,
		LIST *widget_container_list )
{
	RECALL_LOAD *recall_load;

	recall_load = recall_load_calloc();

	recall_load->recall_save_cookie_key =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		recall_save_cookie_key(
			RECALL_PROCESS_COOKIE_KEY,
			folder_name,
			state );

	recall_load->keystrokes_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		recall_load_keystrokes_javascript(
			RECALL_DELIMITER,
			WIDGET_MULTI_LEFT_RIGHT_DELIMITER,
			form_name,
			widget_container_list,
			recall_load->recall_save_cookie_key );

	recall_load->recall_save_cookie_multi_key =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		recall_save_cookie_multi_key(
			folder_name,
			state );

	recall_load->keystrokes_multi_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		recall_load_keystrokes_multi_javascript(
			RECALL_DELIMITER,
			WIDGET_MULTI_LEFT_RIGHT_DELIMITER,
			form_name,
			widget_container_list,
			recall_load->recall_save_cookie_multi_key );

	if ( recall_load->keystrokes_javascript
	||   recall_load->keystrokes_multi_javascript )
	{
		recall_load->javascript =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			recall_load_javascript(
				recall_load->keystrokes_javascript,
				recall_load->keystrokes_multi_javascript );
	}

	return recall_load;
}

char *recall_save_samesite_string(
		boolean application_ssl_support_boolean )
{
	if ( application_ssl_support_boolean )
		return "strict";
	else
		return "";
}

char *recall_save_secure_string(
		boolean application_ssl_support_boolean )
{

	if ( application_ssl_support_boolean )
		return "strict";
	else
		return "";
}

boolean recall_ignore_boolean(
		const char *appaserver_execute_yn,
		char *widget_name,
		enum widget_type widget_type,
		boolean recall_boolean,
		boolean multi_drop_down_boolean )
{
	if ( !widget_name ) return 1;

	if ( !recall_boolean ) return 1;

	if ( string_strcmp(
		widget_name,
		(char *)appaserver_execute_yn ) == 0 )
	{
		return 1;
	}

	if (	multi_drop_down_boolean
	&&	widget_type != multi_drop_down )
	{
		return 1;
	}

	if (	!multi_drop_down_boolean
	&&	widget_type == multi_drop_down )
	{
		return 1;
	}

	return 0;
}

char *recall_save_display( RECALL_SAVE *recall_save )
{
	char display[ STRING_64K ];

	if ( !recall_save ) return "NULL";

	*display = '\0';

	snprintf(
		display,
		sizeof ( display ),
		"cookie_key=%s\n"
		"keystrokes_javascript=%s\n"
		"cookie_multi_key=%s\n"
		"keystrokes_multi_javascript=%s\n"
		"javascript=%s\n",
		recall_save->cookie_key,
		recall_save->keystrokes_javascript,
		recall_save->cookie_multi_key,
		recall_save->keystrokes_multi_javascript,
		recall_save->javascript );

	return strdup( display );
}

char *recall_load_display( RECALL_LOAD *recall_load )
{
	char display[ STRING_64K ];

	if ( !recall_load ) return "NULL";

	*display = '\0';

	snprintf(
		display,
		sizeof ( display ),
		"cookie_key=%s\n"
		"keystrokes_javascript=%s\n"
		"cookie_multi_key=%s\n"
		"keystrokes_multi_javascript=%s\n"
		"javascript=%s\n",
		recall_load->recall_save_cookie_key,
		recall_load->keystrokes_javascript,
		recall_load->recall_save_cookie_multi_key,
		recall_load->keystrokes_multi_javascript,
		recall_load->javascript );

	return strdup( display );
}

