/* $APPASERVER_HOME/library/pair_one2m.c		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "relation.h"
#include "dictionary_separate.h"
#include "timlib.h"
#include "pair_one2m.h"

PAIR_ONE2M *pair_one2m_calloc( void )
{
	PAIR_ONE2M *pair_one2m;

	if ( ! ( pair_one2m =
			(PAIR_ONE2M *)calloc( 1, sizeof( PAIR_ONE2M ) ) ) )
	{
		fprintf( stderr, 
			 "%s/%s(): calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}
	return pair_one2m;
}

char *pair_folder_button_string(
			char *many_folder_name,
			char *keystrokes_save_function,
			char *pair_folder_onclick_function )
{
	char folder_button_string[ 2048 ];
	char buffer[ 128 ];

	/* -------------------------------------------- */
	/* The keystrokes_save_function is assumed to	*/
	/* have "&&" appended to it.			*/
	/* -------------------------------------------- */
	sprintf(folder_button_string,
"<button onClick=\"%s %s document.forms[0].submit();\">"
"%s</button>\n",
		keystrokes_save_function,
		pair_folder_onclick_function,
		format_initial_capital_cr(
			buffer,
			many_folder_name ) );

	return strdup( folder_button_string );
}

char *pair_folder_element_copy_function(
			char *prompt_many_element_name,
			char *many_folder_name )
{
	char copy_function[ 1024 ];

	sprintf( copy_function,
		 "timlib_element_value_set( '%s_0', '%s' ) && ",
		 prompt_many_element_name,
		 many_folder_name );

	return strdup( copy_function );
}

PAIR_ONE2M_FOLDER *pair_one2m_folder_new(
			char *many_folder_name )
{
	PAIR_ONE2M_FOLDER *pair_one2m_folder;

	if ( ! ( pair_one2m_folder =
			(PAIR_ONE2M_FOLDER *)
				calloc( 1, sizeof( PAIR_ONE2M_FOLDER ) ) ) )
	{
		fprintf( stderr, 
			 "%s/%s(): calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}
	pair_one2m_folder->many_folder_name = many_folder_name;

	return pair_one2m_folder;
}

PAIR_ONE2M *pair_one2m_prompt_form_new(
			char *one_folder_name )
{
	PAIR_ONE2M *pair_one2m = pair_one2m_calloc();

	pair_one2m->one_folder_name = one_folder_name;

	return pair_one2m;
}

PAIR_ONE2M *pair_one2m_post_new(
			DICTIONARY *pair_one2m_dictionary )
{
	PAIR_ONE2M *pair_one2m = pair_one2m_calloc();

	pair_one2m->pair_one2m_dictionary = pair_one2m_dictionary;
	pair_one2m->participating = 1;
	return pair_one2m;
}

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

	return list_delimited_string_to_list(
			data /* delimited_string */ );
}

DICTIONARY *pair_one2m_fulfilled_dictionary(
			/* -------------------------------------- */
			/* Sets and returns pair_one2m_dictionary */
			/* -------------------------------------- */
			DICTIONARY *pair_one2m_dictionary,
			char *pair_one2m_fulfilled_list_label,
			LIST *pair_one2m_fulfilled_folder_name_list )
{
	dictionary_set_pointer(
		pair_one2m_dictionary,
		pair_one2m_fulfilled_list_label,
		list_display_delimited(
			pair_one2m_fulfilled_folder_name_list,
			',' ) );
	return pair_one2m_dictionary;
}

char *pair_one2m_next_folder_name(
			LIST *pair_one2m_fulfilled_folder_name_list,
			LIST *relation_one2m_pair_relation_list )
{
	RELATION *relation;

	if ( !list_rewind( relation_one2m_pair_relation_list ) )
		return (char *)0;

	do {
		relation =
			list_get(
				relation_one2m_pair_relation_list );

		if ( !list_exists_string(
			relation->many_folder_name,
			pair_one2m_fulfilled_folder_name_list ) )
		{
			return relation->many_folder_name;
		}
	} while ( list_next( relation_one2m_pair_relation_list ) );

	return (char *)0;
}

char *pair_one2m_folder_name(
			char *folder_label,
			DICTIONARY *pair_one2m_dictionary )
{
	char key[ 128 ];
	char *data;

	sprintf( key, "%s_0", folder_label );

	if ( ! ( data =
			dictionary_get(
				key,
				pair_one2m_dictionary ) ) )
	{
		return (char *)0;
	}

	return data;
}

char *pair_one2m_prompt_element_name(
			char *pair_one2m_prefix,
			char *folder_label )
{
	char element_name[ 128 ];

	sprintf(element_name,
		"%s%s",
		pair_one2m_prefix,
		folder_label );

	return strdup( element_name );
}

boolean pair_one2m_participating(
			DICTIONARY *pair_one2m_dictionary )
{
	return ( dictionary_length( pair_one2m_dictionary ) );
}

boolean pair_one2m_duplicate(
			char *message_key,
			char *message )
{
	return ( instr( message_key, message, 1 ) != -1 );
}

void pair_one2m_duplicate_set(
			DICTIONARY *pair_one2m_dictionary,
			char *pair_one2m_duplicate_key )
{
	dictionary_set_pointer(
		pair_one2m_dictionary,
		pair_one2m_duplicate_key,
		"yes" );
}

void pair_one2m_duplicate_unset(
			DICTIONARY *pair_one2m_dictionary,
			char *pair_one2m_duplicate_key )
{
	dictionary_set_pointer(
		pair_one2m_dictionary,
		pair_one2m_duplicate_key,
		"no" );
}

boolean pair_one2m_inserted_duplicate(
			char *pair_one2m_duplicate_key,
			DICTIONARY *pair_one2m_dictionary )
{
	char *data;

	data =
		dictionary_get(
			pair_one2m_duplicate_key,
			pair_one2m_dictionary );

	if ( string_strcmp( data, "yes" ) == 0 )
		return 1;
	else
		return 0;
}

void pair_one2m_folder_set(
			DICTIONARY *pair_one2m_dictionary,
			char *folder_label,
			char *folder_name )
{
	char key[ 128 ];

	sprintf( key, "%s_0", folder_label );
	dictionary_set_pointer( pair_one2m_dictionary, key, folder_name );
}

LIST *pair_one2m_prompt_form_folder_list(
			LIST *one2m_pair_relation_list )
{
	RELATION *relation;
	PAIR_ONE2M_FOLDER *pair_one2m_folder;
	LIST *one2m_folder_list;

	if ( !list_rewind( one2m_pair_relation_list ) )
	{
		return (LIST *)0;
	}

	one2m_folder_list = list_new();

	do {
		relation =
			list_get( 
				one2m_pair_relation_list );

		pair_one2m_folder =
			pair_one2m_folder_new(
				relation->many_folder->folder_name );

		pair_one2m_folder->copy_function =
			pair_folder_element_copy_function(
				pair_one2m_prompt_element_name(
					DICTIONARY_SEPARATE_PAIR_PREFIX,
					PAIR_ONE2M_MANY_FOLDER_LABEL ),
				pair_one2m_folder->many_folder_name );

		list_set(
			one2m_folder_list,
			pair_one2m_folder );
			
	} while( list_next( one2m_pair_relation_list ) );

	return one2m_folder_list;
}

void pair_one2m_set_folder_button_string(
			LIST *prompt_form_folder_list,
			char *keystrokes_save_function )
{
	PAIR_ONE2M_FOLDER *pair_one2m_folder;

	if ( !keystrokes_save_function )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: keystrokes_save_function is empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );	
	}

	if ( !list_rewind( prompt_form_folder_list ) )
	{
		return;
	}

	do {
		pair_one2m_folder =
			list_get( 
				prompt_form_folder_list );

		pair_one2m_folder->folder_button_string =
			pair_folder_button_string(
				pair_one2m_folder->many_folder_name,
				keystrokes_save_function,
				pair_one2m_folder->
					copy_function );

	} while( list_next( prompt_form_folder_list ) );
}

void pair_one2m_set_fulfilled_name_list(
			LIST *fulfilled_name_list,
			char *many_folder_name,
			LIST *one2m_pair_relation_list )
{
	RELATION *relation;

	if ( !list_rewind( one2m_pair_relation_list ) )
	{
		return;
	}

	do {
		relation =
			list_get( 
				one2m_pair_relation_list );

		list_set_unique(
			fulfilled_name_list,
			relation->many_folder_name );

		if ( strcmp(	relation->many_folder_name,
				many_folder_name ) == 0 )
		{
			return;
		}
	} while( list_next( one2m_pair_relation_list ) );
}
