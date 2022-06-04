/* $APPASERVER_HOME/library/pair_one2m.c		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "relation.h"
#include "dictionary.h"
#include "dictionary_separate.h"
#include "timlib.h"
#include "pair_one2m.h"

LIST *pair_one2m_fulfilled_folder_name_list(
			char *pair_one2m_fulfilled_list_label,
			DICTIONARY *pair_one2m_dictionary )
{
	char *data;

	if ( ! ( data =
			dictionary_get(
				pair_one2m_fulfilled_list_label
					/* key */,
				pair_one2m_dictionary ) ) )
	{
		return list_new();
	}

	return
	list_delimited_string_to_list( data /* delimited_string */ );
}

char *pair_one2m_next_folder_name(
			LIST *pair_one2m_fulfilled_folder_name_list,
			LIST *relation_one2m_pair_list )
{
	RELATION *relation;

	if ( !list_go_tail( relation_one2m_pair_list ) )
		return (char *)0;

	do {
		relation =
			list_get(
				relation_one2m_pair_list );

		if ( !list_exists_string(
			relation->many_folder_name,
			pair_one2m_fulfilled_folder_name_list ) )
		{
			return relation->many_folder_name;
		}
	} while ( list_previous( relation_one2m_pair_list ) );

	return (char *)0;
}

boolean pair_one2m_participating(
			DICTIONARY *pair_one2m_dictionary )
{
	return ( dictionary_length( pair_one2m_dictionary ) );
}

PAIR_ONE2M_BUTTON *pair_one2m_button_new(
			char *recall_save_javascript,
			char *pair_one2m_prompt_insert_hidden_name,
			char *many_folder_name )
{
	PAIR_ONE2M_BUTTON *pair_one2m_button;

	if ( !recall_save_javascript
	||   !pair_one2m_prompt_insert_hidden_name
	||   !many_folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	pair_one2m_button = pair_one2m_button_calloc();

	pair_one2m_button->value_set_javascript =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		pair_one2m_button_value_set_javascript(
			pair_one2m_prompt_insert_hidden_name,
			many_folder_name );

	pair_one2m_button->label =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		pair_one2m_button_label(
			many_folder_name );

	pair_one2m_button->javascript =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		pair_one2m_button_javascript(
			recall_save_javascript,
			pair_one2m_button->value_set_javascript );

	pair_one2m_button->button =
		button_new(
			pair_one2m_button->label,
			pair_one2m_button->javascript );

	return pair_one2m_button;
}

PAIR_ONE2M_BUTTON *pair_one2m_button_calloc( void )
{
	PAIR_ONE2M_BUTTON *pair_one2m_button;

	if ( ! ( pair_one2m_button =
			calloc( 1, sizeof( PAIR_ONE2M_BUTTON ) ) ) )
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

char *pair_one2m_button_value_set_javascript(
			char *pair_one2m_prompt_insert_hidden_name,
			char *many_folder_name )
{
	static char javascript[ 256 ];

	sprintf(javascript,
		"timlib_element_value_set( '%s', '%s' )",
		pair_one2m_prompt_insert_hidden_name,
		many_folder_name );

	return javascript;
}

char *pair_one2m_button_javascript(
			char *recall_save_javascript,
			char *pair_one2m_button_value_set_javascript )
{
	char javascript[ 4096 ];

	if ( !recall_save_javascript
	||   !pair_one2m_button_value_set_javascript )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(javascript,
		"%s && %s && document.forms[0].submit()",
		recall_save_javascript,
		pair_one2m_button_value_set_javascript );

	return strdup( javascript );
}

char *pair_one2m_button_label( char *many_folder_name )
{
	char label[ 256 ];
	char *ptr = label;
	char piece_buffer[ 128 ];
	int p;
	char buffer[ 128 ];

	if ( !many_folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: many_folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	for(	p = 0;
		piece( piece_buffer, '_', many_folder_name, p );
		p++ )
	{
		if ( !p ) ptr += sprintf( ptr, "<br>" );

		ptr += sprintf(
			ptr,
			"%s",
			string_initial_capital(
				buffer,
				piece_buffer ) );
	}

	return strdup( label );
}

PAIR_ONE2M_PROMPT_INSERT *pair_one2m_prompt_insert_new(
			char *recall_save_javascript,
			LIST *relation_pair_one2m_list )
{
	PAIR_ONE2M_PROMPT_INSERT *pair_one2m_prompt_insert;

	if ( !recall_save_javascript )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( relation_pair_one2m_list ) )
	{
		return (PAIR_ONE2M_PROMPT_INSERT *)0;
	}

	pair_one2m_prompt_insert = pair_one2m_prompt_insert_calloc();

	pair_one2m_prompt_insert->pair_one2m_button_list = list_new();

	pair_one2m_prompt_insert->hidden_name =
		/* ------------------------------------ */
		/* Returns pair_one2m_many_folder_label */
		/* ------------------------------------ */
		pair_one2m_prompt_insert_hidden_name(
			PAIR_ONE2M_MANY_FOLDER_LABEL );

	pair_one2m_prompt_insert->transmit_hidden_element =
		appaserver_element_new(
			hidden,
			pair_one2m_prompt_insert->hidden_name );

	free( pair_one2m_prompt_insert->transmit_hidden_element->hidden );

	pair_one2m_prompt_insert->
		transmit_hidden_element->
		hidden =
			element_hidden_new(
				pair_one2m_prompt_insert->
					transmit_hidden_element->
					element_name,
				(char *)0 /* value */ );
	do {
		pair_one2m_prompt_insert->relation =
			list_get(
				relation_pair_one2m_list );

		list_set(
			pair_one2m_prompt_insert->pair_one2m_button_list,
			pair_one2m_button_new(
				recall_save_javascript,
				pair_one2m_prompt_insert->hidden_name,
				pair_one2m_prompt_insert->
					relation->
					many_folder_name ) );

	} while ( list_next( relation_pair_one2m_list ) );

	return pair_one2m_prompt_insert;
}

PAIR_ONE2M_PROMPT_INSERT *pair_one2m_prompt_insert_calloc( void )
{
	PAIR_ONE2M_PROMPT_INSERT *pair_one2m_prompt_insert;

	if ( ! ( pair_one2m_prompt_insert =
			calloc( 1, sizeof( PAIR_ONE2M_PROMPT_INSERT ) ) ) )
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

char *pair_one2m_prompt_insert_hidden_name(
			char *pair_one2m_many_folder_label )
{
	return pair_one2m_many_folder_label;
}

PAIR_ONE2M_POST_PROMPT_INSERT *
	pair_one2m_post_prompt_insert_new(
			char *one_folder_name,
			DICTIONARY *non_prefixed_dictionary )
{
	PAIR_ONE2M_POST_PROMPT_INSERT *
		pair_one2m_post_prompt_insert =
			pair_one2m_post_prompt_insert_calloc();

	if ( ! ( pair_one2m_post_prompt_insert->many_folder_name =
			pair_one2m_post_prompt_insert_many_folder_name(
				PAIR_ONE2M_MANY_FOLDER_LABEL,
				non_prefixed_dictionary ) ) )
	{
		free( pair_one2m_post_prompt_insert );
		return (PAIR_ONE2M_POST_PROMPT_INSERT *)0;
	}

	pair_one2m_post_prompt_insert->pair_one2m_dictionary =
		pair_one2m_post_prompt_insert_dictionary(
			PAIR_ONE2M_ONE_FOLDER_LABEL,
			one_folder_name );

	return pair_one2m_post_prompt_insert;
}

PAIR_ONE2M_POST_PROMPT_INSERT *
	pair_one2m_post_prompt_insert_calloc(
			void )
{
	PAIR_ONE2M_POST_PROMPT_INSERT *pair_one2m_post_prompt_insert;

	if ( ! ( pair_one2m_post_prompt_insert =
			calloc( 1, sizeof( PAIR_ONE2M_POST_PROMPT_INSERT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return pair_one2m_post_prompt_insert;
}

char *pair_one2m_post_prompt_insert_many_folder_name(
			char *pair_one2m_many_folder_label,
			DICTIONARY *non_prefixed_dictionary )
{
	return
	dictionary_get(
		pair_one2m_many_folder_label,
		non_prefixed_dictionary );
}

DICTIONARY *pair_one2m_post_prompt_insert_dictionary(
			char *pair_one2m_one_folder_label,
			char *one_folder_name )
{
	DICTIONARY *dictionary = dictionary_small();

	dictionary_set(
		dictionary,
		pair_one2m_one_folder_label,
		one_folder_name );

	return dictionary;
}

char *pair_one2m_fulfilled_list_key(
			char *pair_one2m_fulfilled_list_label )
{
	return pair_one2m_fulfilled_list_label;
}

char *pair_one2m_fulfilled_list_data(
			LIST *pair_one2m_fulfilled_folder_name_list,
			char *pair_one2m_next_folder_name,
			char sql_delimiter )
{
	char list_data[ 1024 ];

	if ( !list_length( pair_one2m_fulfilled_folder_name_list ) )
	{
		strcpy( list_data, pair_one2m_next_folder_name );
	}
	else
	{
		char *ptr = list_data;

		char *delimited_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			list_delimited_string(
				pair_one2m_fulfilled_folder_name_list,
				sql_delimiter );

		if ( !delimited_string )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: list_delimited_string() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		ptr += sprintf(
			ptr,
			"%s",
			delimited_string );

		ptr += sprintf(
			ptr,
			"%c%s",
			sql_delimiter,
			pair_one2m_next_folder_name );
	}

	return strdup( list_data );
}

