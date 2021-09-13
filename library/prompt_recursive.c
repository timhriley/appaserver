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
#include "relation.h"
#include "prompt_recursive.h"

PROMPT_RECURSIVE *prompt_recursive_new(
			char *folder_name,
			LIST *relation_mto1_non_isa_list )
{
	PROMPT_RECURSIVE *recursive;

	if ( ! ( recursive =
			calloc( 1, sizeof( PROMPT_RECURSIVE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	recursive->folder_name = folder_name;

	recursive->prompt_recursive_folder_list =
		prompt_recursive_folder_list(
			relation_mto1_non_isa_list );

	return recursive;
}

LIST *prompt_recursive_folder_list(
			LIST *relation_mto1_non_isa_list )
{
	LIST *folder_list = {0};
	PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;
	RELATION *relation;

	if ( !list_rewind( relation_mto1_non_isa_list ) ) return (LIST *)0;

	do {
		relation = list_get( relation_mto1_non_isa_list );

		if ( !relation->prompt_mto1_recursive ) continue;

		if ( !relation->one_folder )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: one_folder is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !list_length( relation->one_folder->primary_key_list ) )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( ! ( prompt_recursive_folder =
				prompt_recursive_folder_new(
					relation->one_folder,
					/* ------------------------------ */
					/* Exclude drop_down_multi_select */
					/* ------------------------------ */
					relation->drop_down_multi_select ) ) )
		{
			continue;
		}

		if ( !folder_list ) folder_list = list_new();

		list_set(
			folder_list,
			prompt_recursive_folder );

	} while( list_next( relation_mto1_non_isa_list ) );

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

PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder_new(
			FOLDER *one_folder,
			boolean drop_down_multi_select )
{
	PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;
	RELATION *relation;

	if ( !one_folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: one_folder is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( one_folder->primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( drop_down_multi_select ) return (PROMPT_RECURSIVE_FOLDER *)0;

	prompt_recursive_folder = prompt_recursive_folder_calloc();

	prompt_recursive_folder->one_folder = one_folder;

	prompt_recursive_folder->relation_mto1_primary_key_subset_list =
		relation_mto1_primary_key_subset_list(
			one_folder->folder_name,
			one_folder->primary_key_list );

	if ( !list_rewind(
		prompt_recursive_folder->
			relation_mto1_primary_key_subset_list ) )
	{
		return (PROMPT_RECURSIVE_FOLDER *)0;
	}

	prompt_recursive_folder->prompt_recursive_mto1_folder_list =
		list_new();

	do {
		relation =
			list_get(
				prompt_recursive_folder->
					relation_mto1_primary_key_subset_list );

		if ( !relation->one_folder )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: one_folder is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			prompt_recursive_folder->
				prompt_recursive_mto1_folder_list,
			prompt_recursive_mto1_folder_new(
				relation->one_folder ) );

	} while ( list_next( 
		prompt_recursive_folder->
			relation_mto1_primary_key_subset_list ) );

	return prompt_recursive_folder;
}

PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder_calloc( void )
{
	PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder;

	if ( ! ( prompt_recursive_mto1_folder =
			calloc( 1, sizeof( PROMPT_RECURSIVE_MTO1_FOLDER ) ) ) )
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

PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder_new(
			FOLDER *one_folder )
{
	PROMPT_RECURSIVE_MTO1_FOLDER *recursive_mto1_folder =
		prompt_recursive_mto1_folder_calloc();

	recursive_mto1_folder->one_folder = one_folder;
	return recursive_mto1_folder;
}

