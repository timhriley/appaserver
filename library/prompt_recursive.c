/* library/prompt_recursive.c						*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "query.h"
#include "prompt_recursive.h"

PROMPT_RECURSIVE *prompt_recursive_new(
				char *application_name,
				char *query_folder_name,
				LIST *mto1_related_folder_list )
{
	PROMPT_RECURSIVE *prompt_recursive;

	prompt_recursive =
		(PROMPT_RECURSIVE *)
			calloc( 1, sizeof( PROMPT_RECURSIVE ) );

	prompt_recursive->query_folder_name = query_folder_name;

	prompt_recursive->prompt_recursive_folder_list =
		prompt_recursive_get_prompt_recursive_folder_list(
				application_name,
				mto1_related_folder_list );

	return prompt_recursive;

} /* prompt_recursive_new() */

LIST *prompt_recursive_get_prompt_recursive_folder_list(
				char *application_name,
				LIST *mto1_related_folder_list )
{
	LIST *prompt_recursive_folder_list = {0};
	PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( mto1_related_folder_list ) ) return (LIST *)0;

	do {
		related_folder = list_get( mto1_related_folder_list );

		if ( !related_folder->prompt_mto1_recursive ) continue;

		prompt_recursive_folder =
			prompt_recursive_folder_new(
				application_name,
				related_folder->folder /* prompt_folder */,
				related_folder->drop_down_multi_select );

		if ( !prompt_recursive_folder_list )
			prompt_recursive_folder_list = list_new();

		list_append_pointer(
			prompt_recursive_folder_list,
			prompt_recursive_folder );

	} while( list_next( mto1_related_folder_list ) );

	return prompt_recursive_folder_list;

} /* prompt_recursive_get_prompt_recursive_folder_list() */

PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder_new(
				char *application_name,
				FOLDER *prompt_folder,
				boolean prompt_folder_drop_down_multi_select )
{
	PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;

	prompt_recursive_folder =
		(PROMPT_RECURSIVE_FOLDER *)
			calloc( 1, sizeof( PROMPT_RECURSIVE_FOLDER ) );

	prompt_recursive_folder->prompt_folder = prompt_folder;
	prompt_recursive_folder->prompt_folder_drop_down_multi_select =
		prompt_folder_drop_down_multi_select;

	prompt_recursive_folder->prompt_recursive_mto1_folder_list =
		prompt_recursive_get_mto1_folder_list(
				application_name,
				prompt_folder->folder_name,
				prompt_folder->primary_attribute_name_list );

	return prompt_recursive_folder;

} /* prompt_recursive_folder_new() */

LIST *prompt_recursive_get_mto1_folder_list(
				char *application_name,
				char *prompt_folder_name,
				LIST *primary_attribute_name_list )
{
	LIST *prompt_recursive_mto1_folder_list = {0};
	LIST *mto1_recursive_related_folder_list;
	RELATED_FOLDER *related_folder;
	PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder;

	mto1_recursive_related_folder_list =
		related_folder_get_mto1_related_folder_list(
			list_new_list(),
			application_name,
			BOGUS_SESSION,
			prompt_folder_name,
			(char *)0 /* role_name */,
			0 /* isa_flag */,
			related_folder_prompt_recursive_only,
			0 /* dont override_row_restrictions */,
			primary_attribute_name_list,
			0 /* recursive_level */ );

	if ( !list_rewind( mto1_recursive_related_folder_list ) )
		return (LIST *)0;

	do {
		related_folder = list_get( mto1_recursive_related_folder_list );

		prompt_recursive_mto1_folder =
			prompt_recursive_mto1_folder_new(
				related_folder->folder,
				related_folder->recursive_level );

		if ( !prompt_recursive_mto1_folder_list )
			prompt_recursive_mto1_folder_list = list_new();

		list_append_pointer(
			prompt_recursive_mto1_folder_list,
			prompt_recursive_mto1_folder );

	} while( list_next( mto1_recursive_related_folder_list ) );

	return prompt_recursive_mto1_folder_list;

} /* prompt_recursive_get_mto1_folder_list() */

PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder_new(
				FOLDER *folder,
				int recursive_level )
{
	PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder;

	prompt_recursive_mto1_folder =
		(PROMPT_RECURSIVE_MTO1_FOLDER *)
			calloc( 1, sizeof( PROMPT_RECURSIVE_MTO1_FOLDER ) );

	prompt_recursive_mto1_folder->folder = folder;
	prompt_recursive_mto1_folder->recursive_level = recursive_level;

	return prompt_recursive_mto1_folder;

} /* prompt_recursive_mto1_folder_new() */

char *prompt_recursive_display(	PROMPT_RECURSIVE *prompt_recursive )
{
	PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;
	PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder;
	LIST *prompt_recursive_mto1_folder_list;
	char buffer[ 65536 ];
	char *ptr = buffer;

	*ptr = '\0';

	ptr += sprintf( ptr,
			"%s(): got query_folder_name = (%s)",
			__FUNCTION__,
			prompt_recursive->
				query_folder_name );

	if ( !list_rewind( prompt_recursive->prompt_recursive_folder_list ) )
	{
		return strdup( buffer );
	}

	do {
		prompt_recursive_folder =
			list_get_pointer(
				prompt_recursive->
					prompt_recursive_folder_list );

		ptr += sprintf( ptr,
"; got prompt_folder = (%s), primary_attribute_name_list = (%s)",
				prompt_recursive_folder->
					prompt_folder->
					folder_name,
				list_display(
					prompt_recursive_folder->
						prompt_folder->
						primary_attribute_name_list ) );

		prompt_recursive_mto1_folder_list =
			prompt_recursive_folder->
				prompt_recursive_mto1_folder_list;

		if ( !list_rewind( prompt_recursive_mto1_folder_list ) )
		{
			ptr += sprintf(
				ptr,
				" empty prompt_recursive_mto1_folder_list.\n" );
			continue;
		}

		do {
			prompt_recursive_mto1_folder =
				list_get_pointer(
					prompt_recursive_mto1_folder_list );

			ptr += sprintf(	ptr,
"; child folder = %s, recursive_level = %d, primary_attribute_name_list = (%s)",
					prompt_recursive_mto1_folder->
						folder->
						folder_name,
					prompt_recursive_mto1_folder->
						recursive_level,
					list_display(
					prompt_recursive_mto1_folder->
						folder->
						primary_attribute_name_list ) );

		} while( list_next( prompt_recursive_mto1_folder_list ) );

		ptr += sprintf( ptr, "\n" );

	} while( list_next( prompt_recursive->
				prompt_recursive_folder_list ) );

	return strdup( buffer );

} /* prompt_recursive_display() */

char *prompt_recursive_get_gray_drop_downs_javascript(
				FOLDER *prompt_folder,
				boolean prompt_folder_drop_down_multi_select,
				LIST *prompt_mto1_recursive_folder_list )
{
	PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder;
	char post_change_javascript[ 1024 ];
	char *ptr = post_change_javascript;
	int element_index;

	element_index = prompt_folder_drop_down_multi_select;

	ptr +=
		sprintf( ptr,
			 "timlib_gray_mutually_exclusive_drop_downs('" );

	if ( prompt_folder )
	{
		ptr +=
		sprintf( ptr,
			 "%s_%d",
			 list_display_delimited(
				attribute_primary_attribute_name_list(
			 		prompt_folder->attribute_list ),
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ),
			 element_index );
	}

	if ( !list_rewind( prompt_mto1_recursive_folder_list ) )
	{
		sprintf( ptr, "');" );
		return strdup( post_change_javascript );
	}

	do {
		prompt_recursive_mto1_folder =
			list_get_pointer(
				prompt_mto1_recursive_folder_list );

		if ( prompt_folder
		||   !list_at_first( prompt_mto1_recursive_folder_list ) )
		{
			ptr += sprintf( ptr, "," );
		}

		ptr +=
			sprintf( ptr,
			 "%s_%d",
			 list_display_delimited(
				attribute_primary_attribute_name_list(
					prompt_recursive_mto1_folder->
			 			folder->
						attribute_list ),
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ),
			 0 /* element_index */ );

	} while( list_next( prompt_mto1_recursive_folder_list ) );

	sprintf( ptr, "');" );

	return strdup( post_change_javascript );

} /* prompt_recursive_get_gray_drop_downs_javascript() */

char *prompt_recursive_get_grandchild_query_folder_name(
					PROMPT_RECURSIVE *prompt_recursive,
					DICTIONARY *query_dictionary )
{
	PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;
	PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder;
	LIST *prompt_recursive_folder_list;
	LIST *prompt_recursive_mto1_folder_list;

	if ( !prompt_recursive ) return (char *)0;

	prompt_recursive_folder_list =
		prompt_recursive->prompt_recursive_folder_list;

	if ( !list_rewind( prompt_recursive_folder_list ) ) return (char *)0;

	do {
		prompt_recursive_folder =
			list_get( prompt_recursive_folder_list );

		prompt_recursive_mto1_folder_list =
			prompt_recursive_folder->
				prompt_recursive_mto1_folder_list;

		if ( !list_rewind( prompt_recursive_mto1_folder_list ) )
			continue;

		do {
			prompt_recursive_mto1_folder =
				list_get( prompt_recursive_mto1_folder_list );

			if ( prompt_recursive_folder_is_queried(
				prompt_recursive_mto1_folder->
					folder->
					primary_attribute_name_list,
				query_dictionary ) )
			{
				if ( prompt_recursive_mto1_folder->
					recursive_level >= 1 )
				{
					return prompt_recursive_mto1_folder->
						folder->
						folder_name;
				}
				else
				{
					return (char *)0;
				}
			}

		} while( list_next( prompt_recursive_mto1_folder_list ) );

	} while( list_next( prompt_recursive_folder_list ) );

	return (char *)0;

} /* prompt_recursive_get_grandchild_query_folder_name() */

boolean prompt_recursive_folder_is_queried(
				LIST *primary_attribute_name_list,
				DICTIONARY *query_dictionary )
{
	char *attribute_name;
	char key[ 256 ];

	if ( !list_rewind( primary_attribute_name_list ) ) return 0;

	do {
		attribute_name =
			list_get_pointer(
				primary_attribute_name_list );

		sprintf( key,
			 "%s%s",
			 QUERY_RELATION_OPERATOR_STARTING_LABEL,
			 attribute_name );

		if ( !dictionary_fetch( query_dictionary, key ) )
		{
			return 0;
		}

		if ( !dictionary_fetch( query_dictionary,
					attribute_name ) )
		{
			return 0;
		}

	} while( list_next( primary_attribute_name_list ) );

	return 1;

} /* prompt_recursive_folder_is_queried() */

