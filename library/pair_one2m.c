/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/pair_one2m.c		   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "appaserver_error.h"
#include "relation.h"
#include "dictionary_separate.h"
#include "pair_one2m.h"

PAIR_ONE2M_BUTTON *pair_one2m_button_new(
		const char *dictionary_separate_pair_prefix,
		const char *pair_one2m_many_folder_key,
		char *recall_save_javascript,
		char *many_folder_name )
{
	PAIR_ONE2M_BUTTON *pair_one2m_button;

	if ( !recall_save_javascript
	||   !many_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	pair_one2m_button = pair_one2m_button_calloc();

	pair_one2m_button->widget_container =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		widget_container_new(
			button,
			many_folder_name );

	free( pair_one2m_button->widget_container->widget_button );

	pair_one2m_button->widget_container->widget_button =
		widget_button_new(
			many_folder_name /* label */ );

	pair_one2m_button->action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		pair_one2m_button_action_string(
			dictionary_separate_pair_prefix,
			pair_one2m_many_folder_key,
			recall_save_javascript,
			many_folder_name );

	pair_one2m_button->
		widget_container->
		widget_button->
		button->
		action_string = pair_one2m_button->action_string;

	pair_one2m_button->
		widget_container->
		widget_button->
		button->
		html =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			widget_button_html(
				0 /* row_number */,
				pair_one2m_button->
					widget_container->
					widget_button );

	return pair_one2m_button;
}

PAIR_ONE2M_BUTTON *pair_one2m_button_calloc( void )
{
	PAIR_ONE2M_BUTTON *pair_one2m_button;

	if ( ! ( pair_one2m_button =
			calloc( 1, sizeof ( PAIR_ONE2M_BUTTON ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return pair_one2m_button;
}

char *pair_one2m_button_action_string(
		const char *dictionary_separate_pair_prefix,
		const char *pair_one2m_many_folder_key,
		char *recall_save_javascript,
		char *many_folder_name )
{
	char action_string[ 2048 ];

	if ( !recall_save_javascript
	||   !many_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if (	strlen( recall_save_javascript ) +
		strlen( dictionary_separate_pair_prefix ) +
		strlen( pair_one2m_many_folder_key ) +
		strlen( many_folder_name ) +
		70 >= 2048 )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			2048 );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(action_string,
		"%s && "
		"timlib_element_value_set ('%s%s', '%s') && "
		"document.forms[0].submit();",
		recall_save_javascript,
		dictionary_separate_pair_prefix,
		pair_one2m_many_folder_key,
		many_folder_name );

	return strdup( action_string );
}

char *pair_one2m_prompt_insert_unfulfilled_list_string(
		const char pair_one2m_folder_delimiter,
		LIST *relation_one2m_pair_list )
{
	RELATION_ONE2M *relation_one2m;
	char list_string[ 1024 ];
	char *ptr = list_string;

	if ( list_rewind( relation_one2m_pair_list ) )
	do {
		relation_one2m =
			list_get(
				relation_one2m_pair_list );

		if ( !relation_one2m->many_folder_name )
		{
			char message[ 128 ];

			sprintf(message,
				"relation_one2m->many_folder_name is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( ptr != list_string )
			ptr += sprintf(
				ptr,
				"%c",
				pair_one2m_folder_delimiter );

		ptr += sprintf( ptr, "%s", relation_one2m->many_folder_name );

	} while ( list_next( relation_one2m_pair_list ) );

	if ( ptr == list_string )
		return NULL;
	else
		return strdup( list_string );
}

PAIR_ONE2M_PROMPT_INSERT *pair_one2m_prompt_insert_new(
		const char *dictionary_separate_pair_prefix,
		const char *pair_one2m_one_folder_key,
		const char *pair_one2m_many_folder_key,
		const char *pair_one2m_unfulfilled_list_key,
		const char pair_one2m_folder_delimiter,
		char *one_folder_name,
		char *recall_save_javascript,
		LIST *relation_one2m_pair_list )
{
	PAIR_ONE2M_PROMPT_INSERT *pair_one2m_prompt_insert;

	if ( !pair_one2m_one_folder_key
	||   !pair_one2m_unfulfilled_list_key
	||   !pair_one2m_folder_delimiter
	||   !one_folder_name
	||   !recall_save_javascript )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( relation_one2m_pair_list ) ) return NULL;

	pair_one2m_prompt_insert = pair_one2m_prompt_insert_calloc();

	pair_one2m_prompt_insert->pair_dictionary = dictionary_small();

	dictionary_set(
		pair_one2m_prompt_insert->pair_dictionary,
		(char *)pair_one2m_one_folder_key,
		one_folder_name );

	pair_one2m_prompt_insert->unfulfilled_list_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		pair_one2m_prompt_insert_unfulfilled_list_string(
			pair_one2m_folder_delimiter,
			relation_one2m_pair_list );

	if ( pair_one2m_prompt_insert->unfulfilled_list_string )
	{
		dictionary_set(
			pair_one2m_prompt_insert->pair_dictionary,
			(char *)pair_one2m_unfulfilled_list_key,
			pair_one2m_prompt_insert->unfulfilled_list_string );
	}

	pair_one2m_prompt_insert->pair_one2m_button_list =
		pair_one2m_button_list(
			dictionary_separate_pair_prefix,
			pair_one2m_many_folder_key,
			recall_save_javascript,
			relation_one2m_pair_list );

	pair_one2m_prompt_insert->hidden_widget =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		pair_one2m_prompt_insert_hidden_widget(
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			PAIR_ONE2M_MANY_FOLDER_KEY );

	return pair_one2m_prompt_insert;
}

LIST *pair_one2m_button_list(
		const char *dictionary_separate_pair_prefix,
		const char *pair_one2m_many_folder_key,
		char *recall_save_javascript,
		LIST *relation_one2m_pair_list )
{
	RELATION_ONE2M *relation_one2m;
	LIST *list = list_new();

	if ( !dictionary_separate_pair_prefix
	||   !pair_one2m_many_folder_key
	||   !recall_save_javascript )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( relation_one2m_pair_list ) )
	do {
		relation_one2m =
			list_get(
				relation_one2m_pair_list );

		list_set(
			list,
			pair_one2m_button_new(
				dictionary_separate_pair_prefix,
				pair_one2m_many_folder_key,
				recall_save_javascript,
				relation_one2m->many_folder_name ) );

	} while ( list_next( relation_one2m_pair_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

PAIR_ONE2M_PROMPT_INSERT *pair_one2m_prompt_insert_calloc( void )
{
	PAIR_ONE2M_PROMPT_INSERT *pair_one2m_prompt_insert;

	if ( ! ( pair_one2m_prompt_insert =
			calloc( 1, sizeof ( PAIR_ONE2M_PROMPT_INSERT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return pair_one2m_prompt_insert;
}

PAIR_ONE2M_POST_TABLE_INSERT *pair_one2m_post_table_insert_new(
		DICTIONARY *pair_dictionary /* in/out */,
		const char *pair_one2m_unfulfilled_list_key,
		const char *pair_one2m_fulfilled_list_key,
		const char pair_one2m_folder_delimiter,
		char *many_folder_name )
{
	PAIR_ONE2M_POST_TABLE_INSERT *pair_one2m_post_table_insert;
	char *unfulfilled_list_string;

	if ( !many_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !dictionary_length( pair_dictionary ) ) return NULL;

	pair_one2m_post_table_insert = pair_one2m_post_table_insert_calloc();

	pair_one2m_post_table_insert->fulfilled_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		pair_one2m_post_table_insert_fulfilled_list(
			pair_dictionary,
			(char *)pair_one2m_fulfilled_list_key,
			pair_one2m_folder_delimiter );

	if ( !list_string_exists(
		many_folder_name,
		pair_one2m_post_table_insert->fulfilled_list ) )
	{
		char *fulfilled_list_string;

		list_set(
			pair_one2m_post_table_insert->fulfilled_list,
			many_folder_name );

		fulfilled_list_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			list_delimited_string(
				pair_one2m_post_table_insert->fulfilled_list,
				pair_one2m_folder_delimiter );

		dictionary_set(
			pair_dictionary,
			(char *)pair_one2m_fulfilled_list_key,
			fulfilled_list_string );
	}

	pair_one2m_post_table_insert->unfulfilled_list =
		pair_one2m_post_table_insert_unfulfilled_list(
			pair_dictionary,
			(char *)pair_one2m_unfulfilled_list_key,
			pair_one2m_folder_delimiter );

	list_subtract_string(
		pair_one2m_post_table_insert->unfulfilled_list,
		many_folder_name );

	unfulfilled_list_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		list_delimited_string(
			pair_one2m_post_table_insert->unfulfilled_list,
			pair_one2m_folder_delimiter );

	dictionary_set(
		pair_dictionary,
		(char *)pair_one2m_unfulfilled_list_key,
		unfulfilled_list_string );

	pair_one2m_post_table_insert->next_folder_name =
		pair_one2m_post_table_insert_next_folder_name(
			pair_one2m_post_table_insert->unfulfilled_list );

	return pair_one2m_post_table_insert;
}

PAIR_ONE2M_POST_TABLE_INSERT *pair_one2m_post_table_insert_calloc( void )
{
	PAIR_ONE2M_POST_TABLE_INSERT *pair_one2m_post_table_insert;

	if ( ! ( pair_one2m_post_table_insert =
		   calloc( 1,
			   sizeof ( PAIR_ONE2M_POST_TABLE_INSERT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return pair_one2m_post_table_insert;
}


LIST *pair_one2m_post_table_insert_fulfilled_list(
		DICTIONARY *pair_dictionary,
		char *pair_one2m_fulfilled_list_key,
		char pair_one2m_folder_delimiter )
{
	LIST *fulfilled_list;
	char *fulfilled_list_string;

	fulfilled_list_string =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		dictionary_get(
			pair_one2m_fulfilled_list_key,
			pair_dictionary );

	if ( !fulfilled_list_string )
	{
		fulfilled_list = list_new();
	}
	else
	{
		fulfilled_list =
			list_string_list(
				fulfilled_list_string,
				pair_one2m_folder_delimiter );
	}

	return fulfilled_list;
}

LIST *pair_one2m_post_table_insert_unfulfilled_list(
		DICTIONARY *pair_dictionary,
		char *pair_one2m_unfulfilled_list_key,
		char pair_one2m_folder_delimiter )
{
	LIST *unfulfilled_list = {0};
	char *unfulfilled_list_string;

	if ( ( unfulfilled_list_string =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		dictionary_get(
			pair_one2m_unfulfilled_list_key,
			pair_dictionary ) )
	&&     *unfulfilled_list_string )
	{
		unfulfilled_list =
			list_string_list(
				unfulfilled_list_string,
				pair_one2m_folder_delimiter );
	}

	return unfulfilled_list;
}

char *pair_one2m_post_table_insert_next_folder_name(
			LIST *unfulfilled_list )
{
	return list_first( unfulfilled_list );
}

PAIR_ONE2M_POST_PROMPT_INSERT *pair_one2m_post_prompt_insert_new(
		const char *pair_one2m_one_folder_key,
		const char *pair_one2m_many_folder_key,
		const char *pair_one2m_unfulfilled_list_key,
		const char pair_one2m_folder_delimiter,
		DICTIONARY *pair_dictionary )
{
	PAIR_ONE2M_POST_PROMPT_INSERT *pair_one2m_post_prompt_insert;

	pair_one2m_post_prompt_insert =
		pair_one2m_post_prompt_insert_calloc();

	if ( ! ( pair_one2m_post_prompt_insert->one_folder_name =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				(char *)pair_one2m_one_folder_key,
				pair_dictionary ) ) )
	{
		return pair_one2m_post_prompt_insert;
	}

	if ( ! ( pair_one2m_post_prompt_insert->many_folder_name =
			dictionary_get(
				(char *)pair_one2m_many_folder_key,
				pair_dictionary ) ) )
	{
		return pair_one2m_post_prompt_insert;
	}

	pair_one2m_post_prompt_insert->unfulfilled_list_string =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		dictionary_get(
			(char *)pair_one2m_unfulfilled_list_key,
			pair_dictionary );

	pair_one2m_post_prompt_insert->unfulfilled_list =
		list_delimiter_extract(
			pair_one2m_post_prompt_insert->unfulfilled_list_string,
			pair_one2m_folder_delimiter );

	pair_one2m_post_prompt_insert->next_folder_name =
/* --------------------------------------------------------------------- */
/* Returns many_folder_name, a component of unfulfilled_list, "" or null */
/* --------------------------------------------------------------------- */
		pair_one2m_post_prompt_insert_next_folder_name(
			pair_one2m_post_prompt_insert->many_folder_name,
			pair_one2m_post_prompt_insert->unfulfilled_list );

	return pair_one2m_post_prompt_insert;
}

char *pair_one2m_post_prompt_insert_next_folder_name(
		char *many_folder_name,
		LIST *unfulfilled_list )
{
	char *next_folder_name;

	/* If has pairs available, but press [Submit] */
	/* ------------------------------------------ */
	if ( many_folder_name
	&&   !*many_folder_name )
	{
		next_folder_name = "";
	}
	else
	if ( many_folder_name )
	{
		next_folder_name = many_folder_name;
	}
	else
	{
		next_folder_name =
			/* --------------------------------- */
			/* Returns the first element or null */
			/* --------------------------------- */
			list_first( unfulfilled_list );
	}

	return next_folder_name;
}

PAIR_ONE2M_POST_PROMPT_INSERT *pair_one2m_post_prompt_insert_calloc( void )
{
	PAIR_ONE2M_POST_PROMPT_INSERT *pair_one2m_post_prompt_insert;

	if ( ! ( pair_one2m_post_prompt_insert =
		   calloc( 1,
			   sizeof ( PAIR_ONE2M_POST_PROMPT_INSERT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return pair_one2m_post_prompt_insert;
}

WIDGET_CONTAINER *pair_one2m_prompt_insert_hidden_widget(
		char *dictionary_separate_pair_prefix,
		char *pair_one2m_many_folder_key )
{
	WIDGET_CONTAINER *hidden_widget;
	char widget_name[ 128 ];

	sprintf(widget_name,
		"%s%s",
		dictionary_separate_pair_prefix,
		pair_one2m_many_folder_key );

	hidden_widget =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		widget_container_new(
			hidden,
			strdup( widget_name ) );

	return hidden_widget;
}

PAIR_ONE2M_TABLE_INSERT *pair_one2m_table_insert_new(
		const char *pair_one2m_one_folder_key,
		DICTIONARY *pair_dictionary )
{
	PAIR_ONE2M_TABLE_INSERT *pair_one2m_table_insert;

	if ( !dictionary_length( pair_dictionary ) ) return NULL;

	pair_one2m_table_insert = pair_one2m_table_insert_calloc();

	pair_one2m_table_insert->one_folder_name =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		dictionary_get(
			(char *)pair_one2m_one_folder_key,
			pair_dictionary );

	if ( !pair_one2m_table_insert->one_folder_name )
	{
		free( pair_one2m_table_insert );
		return NULL;
	}

	pair_one2m_table_insert->folder_attribute_primary_key_list =
		folder_attribute_fetch_primary_key_list(
			pair_one2m_table_insert->one_folder_name );

	return pair_one2m_table_insert;
}

PAIR_ONE2M_TABLE_INSERT *pair_one2m_table_insert_calloc( void )
{
	PAIR_ONE2M_TABLE_INSERT *pair_one2m_table_insert;

	if ( ! ( pair_one2m_table_insert =
			calloc( 1,
				sizeof ( PAIR_ONE2M_TABLE_INSERT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return pair_one2m_table_insert;
}

