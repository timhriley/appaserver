/* $APPASERVER_HOME/library/prompt_recursive.c				*/
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
#include "environ.h"
#include "related_folder.h"
#include "prompt_recursive.h"

PROMPT_RECURSIVE *prompt_recursive(
			char *folder_name,
			LIST *mto1_related_folder_list )
{
	PROMPT_RECURSIVE *recursive;

	recursive =
		(PROMPT_RECURSIVE *)
			calloc( 1, sizeof( PROMPT_RECURSIVE ) );

	recursive->folder_name = folder_name;

	recursive->prompt_recursive_folder_list =
		prompt_recursive_folder_list(
				mto1_related_folder_list );

	return recursive;
}

LIST *prompt_recursive_folder_list(
			LIST *mto1_related_folder_list )
{
	LIST *folder_list = {0};
	PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( mto1_related_folder_list ) ) return (LIST *)0;

	do {
		related_folder = list_get( mto1_related_folder_list );

		if ( !related_folder->prompt_mto1_recursive ) continue;

		prompt_recursive_folder =
			prompt_recursive_folder(
				related_folder->folder );

		if ( !folder_list ) folder_list = list_new();

		list_set(
			folder_list,
			prompt_recursive_folder );

	} while( list_next( mto1_related_folder_list ) );

	return folder_list;
}

PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder_calloc( void )
{
	PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;

	prompt_recursive_folder =
		(PROMPT_RECURSIVE_FOLDER *)
			calloc( 1, sizeof( PROMPT_RECURSIVE_FOLDER ) );

	if ( !prompt_recursive_folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return prompt_recursive_folder;
}

PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder(
			FOLDER *folder )
{
	PROMPT_RECURSIVE_FOLDER *recursive_folder =
		prompt_recursive_folder_calloc();

	recursive_folder->folder = folder;

	recursive_folder->prompt_recursive_mto1_folder_list =
		prompt_recursive_mto1_folder_list(
			folder->folder_name,
			folder->primary_key_list );

	return recursive_folder;
}

LIST *prompt_recursive_mto1_folder_list(
			char *folder_name,
			LIST *primary_key_list )
{
	LIST *recursive_mto1_folder_list = {0};
	LIST *mto1_recursive_related_folder_list;
	RELATED_FOLDER *related_folder;
	PROMPT_RECURSIVE_MTO1_FOLDER *recursive_mto1_folder;

	mto1_recursive_related_folder_list =
		related_folder_mto1_related_folder_list(
			list_new_list(),
			environment_application_name(),
			BOGUS_SESSION,
			folder_name,
			(char *)0 /* role_name */,
			0 /* isa_flag */,
			related_folder_prompt_recursive_only,
			0 /* dont override_row_restrictions */,
			primary_key_list,
			0 /* recursive_level */ );

	if ( !list_rewind( mto1_recursive_related_folder_list ) )
		return (LIST *)0;

	do {
		related_folder = list_get( mto1_recursive_related_folder_list );

		recursive_mto1_folder =
			prompt_recursive_mto1_folder(
				related_folder->folder,
				related_folder->recursive_level );

		if ( !recursive_mto1_folder_list )
			recursive_mto1_folder_list =
				list_new();

		list_set(
			recursive_mto1_folder_list,
			recursive_mto1_folder );

	} while( list_next( mto1_recursive_related_folder_list ) );

	return recursive_mto1_folder_list;
}

PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder_calloc( void )
{
	PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder;

	prompt_recursive_mto1_folder =
		(PROMPT_RECURSIVE_MTO1_FOLDER *)
			calloc( 1, sizeof( PROMPT_RECURSIVE_MTO1_FOLDER ) );

	if ( !prompt_recursive_mto1_folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return prompt_recursive_mto1_folder;
}

PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder(
			FOLDER *folder,
			int recursive_level )
{
	PROMPT_RECURSIVE_MTO1_FOLDER *recursive_mto1_folder;

	recursive_mto1_folder =
		prompt_recursive_mto1_folder_calloc();

	recursive_mto1_folder->folder = folder;
	recursive_mto1_folder->recursive_level = recursive_level;

	return recursive_mto1_folder;
}

char *prompt_recursive_display(	PROMPT_RECURSIVE *prompt_recursive )
{
	PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;
	PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder;
	LIST *prompt_recursive_mto1_folder_list;
	char buffer[ 65536 ];
	char *ptr = buffer;

	*ptr = '\0';

	ptr += sprintf( ptr,
			"%s(): got folder_name = (%s)",
			__FUNCTION__,
			prompt_recursive->
				folder_name );

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
"; got prompt_folder = (%s), primary_key_list = (%s)",
				prompt_recursive_folder->
					prompt_folder->
					folder_name,
				list_display(
					prompt_recursive_folder->
						prompt_folder->
						primary_key_list ) );

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
"; child folder = %s, recursive_level = %d, primary_key_list = (%s)",
					prompt_recursive_mto1_folder->
						folder->
						folder_name,
					prompt_recursive_mto1_folder->
						recursive_level,
					list_display(
					prompt_recursive_mto1_folder->
						folder->
						primary_key_list ) );

		} while( list_next( prompt_recursive_mto1_folder_list ) );

		ptr += sprintf( ptr, "\n" );

	} while( list_next( prompt_recursive->
				prompt_recursive_folder_list ) );

	return strdup( buffer );
}

