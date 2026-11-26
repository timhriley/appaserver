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

	recall_save->javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		recall_save_javascript(
			RECALL_NAME_DELIMITER /* Probably , */,
			RECALL_EQUAL_DELIMITER /* Probably == */,
			RECALL_BLOCK_DELIMITER /* Probably ~~ */,
			widget_container_list,
			application_ssl_support_boolean,
			recall_save->cookie_key );

	recall_save->cookie_multi_key =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		recall_save_cookie_multi_key(
			RECALL_MULTI_KEY_PREFIX,
			RECALL_PROCESS_COOKIE_KEY,
			folder_name,
			state );

	recall_save->multi_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		recall_save_multi_javascript(
			RECALL_NAME_DELIMITER,
			RECALL_EQUAL_DELIMITER,
			RECALL_BLOCK_DELIMITER,
			RECALL_MULTI_DELIMITER,
			RECALL_MULTI_INDEX_DELIMITER,
			widget_container_list,
			application_ssl_support_boolean,
			recall_save->cookie_multi_key );

	if ( recall_save->javascript
	||   recall_save->multi_javascript )
	{
		recall_save->apply_javascript =
			/* --------------------------- */
			/* Borrow RECALL_LOAD's	       */
			/* Returns heap memory or null */
			/* --------------------------- */
			recall_load_apply_javascript(
				(char *)0 /* button_reset_javascript */,
				recall_save->javascript
					/* recall_load_javascript */,
				recall_save->multi_javascript
					/* recall_load_multi_javascript */ );
	}

	return recall_save;
}

char *recall_save_cookie_key(
		const char *recall_process_cookie_key,
		char *folder_name,
		char *state )
{
	static char cookie_key[ 128 ];

	if ( folder_name && state )
	{
		snprintf(
			cookie_key,
			sizeof ( cookie_key ),
			"%s_%s",
			folder_name,
			state );
	}
	else
	{
		string_strcpy(
			cookie_key,
			(char *)recall_process_cookie_key,
			sizeof ( cookie_key ) );
	}

	return cookie_key;
}

char *recall_save_open(
		boolean application_ssl_support_boolean,
		char *recall_save_cookie_key )
{
	static char open[ 256 ];

	if ( !recall_save_cookie_key )
	{
		char message[ 128 ];

		sprintf(message, "recall_save_cookie_key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		open,
		sizeof ( open ),
		"var result = new recall_save('%s','%s','%s',",
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

char *recall_save_close(
		const char recall_name_delimiter,
		const char *recall_equal_delimiter,
		const char *recall_block_delimiter )
{
	static char close[ 128 ];

	snprintf(
		close,
		sizeof ( close ),
		",'%c','%s','%s');",
		recall_name_delimiter,
		recall_equal_delimiter,
		recall_block_delimiter );

	return close;
}

char *recall_save_javascript(
		const char recall_name_delimiter,
		const char *recall_equal_delimiter,
		const char *recall_block_delimiter,
		LIST *widget_container_list,
		boolean application_ssl_support_boolean,
		char *recall_save_cookie_key )
{
	char javascript[ STRING_64K ];
	char *ptr = javascript;
	WIDGET_CONTAINER *widget_container;
	boolean got_one = 0;
	char *name_string;

	if ( !recall_save_cookie_key )
	{
		char message[ 128 ];

		sprintf(message, "recall_save_cookie_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( widget_container_list ) ) return NULL;

	ptr += sprintf(
		ptr,
		"%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		recall_save_open(
			application_ssl_support_boolean,
			recall_save_cookie_key ) );

	/* Surround widget name list in quotes */
	/* ----------------------------------- */
	ptr += sprintf( ptr, "'" );

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

		name_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			recall_widget_name_string(
				recall_name_delimiter,
				widget_container,
				got_one );

		ptr += sprintf( ptr, "%s", name_string );
		got_one = 1;

	} while ( list_next( widget_container_list ) );

	if ( !got_one ) return NULL;

	/* Surround widget name list in quotes */
	/* ----------------------------------- */
	ptr += sprintf( ptr, "'" );

	ptr += sprintf(
		ptr,
		"%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		recall_save_close(
			recall_name_delimiter,
			recall_equal_delimiter,
			recall_block_delimiter ) );

	return strdup( javascript );
}

char *recall_save_multi_open(
		boolean application_ssl_support_boolean,
		char *recall_save_cookie_multi_key )
{
	static char open[ 128 ];

	if ( !recall_save_cookie_multi_key )
	{
		char message[ 128 ];

		sprintf(message, "recall_save_cookie_multi_key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		open,
		sizeof ( open ),
		"var result = new recall_multi_save('%s','%s','%s',",
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

char *recall_save_multi_close(
		const char recall_name_delimiter,
		const char *recall_equal_delimiter,
		const char *recall_block_delimiter,
		const char *recall_multi_delimiter,
		const char recall_multi_index_delimiter )
{
	static char close[ 128 ];

	snprintf(
		close,
		sizeof ( close ),
		",'%c','%s','%s','%s','%c');",
		recall_name_delimiter,
		recall_equal_delimiter,
		recall_block_delimiter,
		recall_multi_delimiter,
		recall_multi_index_delimiter );

	return close;
}

char *recall_save_multi_javascript(
		const char recall_name_delimiter,
		const char *recall_equal_delimiter,
		const char *recall_block_delimiter,
		const char *recall_multi_delimiter,
		const char recall_multi_index_delimiter,
		LIST *widget_container_list,
		boolean application_ssl_support_boolean,
		char *recall_save_cookie_multi_key )
{
	char javascript[ STRING_64K ];
	char *ptr = javascript;
	WIDGET_CONTAINER *widget_container;
	boolean got_one = 0;
	char *name_string;

	if ( !recall_save_cookie_multi_key )
	{
		char message[ 128 ];

		sprintf(message, "recall_save_cookie_multi_key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( widget_container_list ) ) return NULL;

	ptr += sprintf(
		ptr,
		"%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		recall_save_multi_open(
			application_ssl_support_boolean,
		 	recall_save_cookie_multi_key ) );

	/* Surround widget name list in quotes */
	/* ----------------------------------- */
	ptr += sprintf( ptr, "'" );

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

		name_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			recall_widget_name_string(
				recall_name_delimiter,
				widget_container,
				got_one );

		ptr += sprintf( ptr, "%s", name_string );
		got_one = 1;

	} while ( list_next( widget_container_list ) );

	if ( !got_one ) return NULL;

	/* Surround widget name list in quotes */
	/* ----------------------------------- */
	ptr += sprintf( ptr, "'" );

	ptr += sprintf(
		ptr,
		"%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		recall_save_multi_close(
			recall_name_delimiter,
			recall_equal_delimiter,
			recall_block_delimiter,
			recall_multi_delimiter,
			recall_multi_index_delimiter ) );

	return strdup( javascript );
}

char *recall_load_javascript(
		const char *recall_equal_delimiter,
		const char *recall_block_delimiter,
		char *form_name,
		char *recall_save_cookie_key )
{
	char javascript[ STRING_1K ];

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

	snprintf(
		javascript,
		sizeof ( javascript ),
		"var result = new recall_load(%s,'%s','%s','%s');",
		form_name,
		recall_save_cookie_key,
		recall_equal_delimiter,
		recall_block_delimiter );

	return strdup( javascript );
}

char *recall_load_multi_javascript(
		const char *recall_equal_delimiter,
		const char *recall_block_delimiter,
		const char *recall_multi_delimiter,
		char recall_multi_index_delimiter,
		char *form_name,
		char *recall_save_cookie_multi_key )
{
	char javascript[ STRING_1K ];

	if ( !form_name
	||   !recall_save_cookie_multi_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		javascript,
		sizeof ( javascript ),
		"var result = "
		"new recall_multi_load(%s,'%s','%s','%s','%s','%c');",
		form_name,
		recall_save_cookie_multi_key,
		recall_equal_delimiter,
		recall_block_delimiter,
		recall_multi_delimiter,
		recall_multi_index_delimiter );

	return strdup( javascript );
}

char *recall_load_apply_javascript(
		char *button_reset_javascript,
		char *recall_load_javascript,
		char *recall_load_multi_javascript )
{
	char apply_javascript[ STRING_65K ];
	char *ptr = apply_javascript;

	if ( !recall_load_javascript
	&&   !recall_load_multi_javascript )
	{
		return (char *)0;
	}

	if (	string_strlen( button_reset_javascript ) +
		string_strlen( recall_load_javascript ) +
		string_strlen( recall_load_multi_javascript ) + 2
		>= STRING_65K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_65K );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( button_reset_javascript )
	{
		ptr += sprintf( ptr,
			"%s",
			button_reset_javascript );
	}

	if ( recall_load_javascript )
	{
		ptr += sprintf( ptr,
			"%s",
			recall_load_javascript );
	}

	if ( recall_load_multi_javascript )
	{
		ptr += sprintf( ptr,
			"%s",
			recall_load_multi_javascript );
	}

	return strdup( apply_javascript );
}

char *recall_save_cookie_multi_key(
		const char *recall_multi_key_prefix,
		const char *recall_process_cookie_key,
		char *folder_name,
		char *state )
{
	static char multi_key[ 128 ];

	if ( folder_name && state )
	{
		snprintf(
			multi_key,
			sizeof ( multi_key ),
			"%s%s_%s",
			recall_multi_key_prefix,
			folder_name,
			state );
	}
	else
	{
		string_strcpy(
			multi_key,
			(char *)recall_process_cookie_key,
			sizeof ( multi_key ) );
	}

	return multi_key;
}

RECALL_LOAD *recall_load_new(
		char *state,
		char *form_name,
		char *folder_name,
		boolean multi_recall_boolean )
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

	recall_load->button_reset_javascript =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		button_reset_javascript(
			RECALL_MULTI_INDEX_DELIMITER,
			form_name );

	recall_load->javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		recall_load_javascript(
			RECALL_EQUAL_DELIMITER,
			RECALL_BLOCK_DELIMITER,
			form_name,
			recall_load->recall_save_cookie_key );

	if ( multi_recall_boolean )
	{
		recall_load->recall_save_cookie_multi_key =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			recall_save_cookie_multi_key(
				RECALL_MULTI_KEY_PREFIX,
				RECALL_PROCESS_COOKIE_KEY,
				folder_name,
				state );

		recall_load->multi_javascript =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			recall_load_multi_javascript(
				RECALL_EQUAL_DELIMITER,
				RECALL_BLOCK_DELIMITER,
				RECALL_MULTI_DELIMITER,
				RECALL_MULTI_INDEX_DELIMITER,
				form_name,
				recall_load->recall_save_cookie_multi_key );
	}

	recall_load->apply_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		recall_load_apply_javascript(
			recall_load->button_reset_javascript,
			recall_load->javascript,
			recall_load->multi_javascript );

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
	char display[ STRING_4K ];

	if ( !recall_save ) return "NULL";

	*display = '\0';

	snprintf(
		display,
		sizeof ( display ),
		"cookie_key=%s\n"
		"javascript=%s\n"
		"cookie_multi_key=%s\n"
		"multi_javascript=%s\n"
		"apply_javascript=%s\n",
		recall_save->cookie_key,
		recall_save->javascript,
		recall_save->cookie_multi_key,
		recall_save->multi_javascript,
		recall_save->apply_javascript );

	return strdup( display );
}

char *recall_load_display( RECALL_LOAD *recall_load )
{
	char display[ STRING_4K ];

	if ( !recall_load ) return "NULL";

	*display = '\0';

	snprintf(
		display,
		sizeof ( display ),
		"cookie_key=%s\n"
		"javascript=%s\n"
		"cookie_multi_key=%s\n"
		"multi_javascript=%s\n"
		"apply_javascript=%s\n",
		recall_load->recall_save_cookie_key,
		recall_load->javascript,
		recall_load->recall_save_cookie_multi_key,
		recall_load->multi_javascript,
		recall_load->apply_javascript );

	return strdup( display );
}

char *recall_widget_name( WIDGET_CONTAINER *widget_container )
{
	char *widget_name;

	if ( !widget_container )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( widget_container->widget_type == upload )
		widget_name = widget_container->upload->recall_widget_name;
	else
		widget_name = widget_container->widget_name;

	return widget_name;
}

char *recall_widget_name_string(
		const char recall_name_delimiter,
		WIDGET_CONTAINER *widget_container,
		boolean got_one )
{
	static char string[ 128 ];
	char *ptr = string;

	if ( got_one )
	{
		ptr += sprintf( ptr, "%c", recall_name_delimiter );
	}

	ptr += sprintf(
		ptr,
		"%s",
		/* ------------------------------ */
		/* Returns component of parameter */
		/* ------------------------------ */
		recall_widget_name(
			widget_container ) );

	return string;
}

