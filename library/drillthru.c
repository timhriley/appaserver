/* $APPASERVER_HOME/library/drillthru.c		 */
/* --------------------------------------------- */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "String.h"
#include "drillthru.h"

DRILLTHRU *drillthru_calloc( void )
{
	DRILLTHRU *drillthru;

	if ( ! ( drillthru = calloc( 1, sizeof( DRILLTHRU ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return drillthru;
}

DRILLTHRU *drillthru_start(
			char *base_folder_name )
{
	DRILLTHRU *drillthru = drillthru_calloc();

	drillthru->relation_mto1_drillthru_list =
		relation_mto1_drillthru_list(
			(LIST *)0 /* relation_list */,
			base_folder_name,
			(LIST *)0 /* fulfilled_folder_name_list */ );

	if ( !list_length( drillthru->relation_mto1_drillthru_list ) )
	{
		return drillthru;
	}

	drillthru->drillthru_dictionary = dictionary_small();

	drillthru_base_folder_name_set(
		drillthru->drillthru_dictionary,
		DRILLTHRU_BASE_FOLDER_KEY,
		base_folder_name );

	return drillthru;
}

DRILLTHRU *drillthru_participating(
			char *base_folder_name,
			DICTIONARY *drillthru_dictionary )
{
	DRILLTHRU *drillthru;

	if ( !base_folder_name || !*base_folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: base_folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !drillthru_dictionary )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: drillthru_dictionary is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	drillthru = drillthru_calloc();
	drillthru->base_folder_name = base_folder_name;
	drillthru->drillthru_dictionary = drillthru_dictionary;

	drillthru->fulfilled_folder_name_list =
		drillthru_fulfilled_folder_name_list(
			DRILLTHRU_FOLDER_LIST_KEY,
			drillthru->drillthru_dictionary );

	drillthru->relation_mto1_drillthru_list =
		relation_mto1_drillthru_list(
			(LIST *)0 /* relation_list */,
			base_folder_name,
			drillthru->fulfilled_folder_name_list );

	drillthru->current_folder_name =
		drillthru_current_folder_name(
			drillthru->relation_mto1_drillthru_list,
			drillthru->base_folder_name );

	return drillthru;
}

LIST *drillthru_fulfilled_folder_name_list(
			char *drillthru_folder_list_key,
			DICTIONARY *drillthru_dictionary )
{
	return list_string_extract(
			dictionary_get(
				drillthru_folder_list_key,
				drillthru_dictionary ),
			',' );
}

char *drillthru_current_folder_name(
			LIST *relation_mto1_drillthru_list,
			char *base_folder_name )
{
	RELATION *relation;

	if ( !list_tail( relation_mto1_drillthru_list ) )
		return base_folder_name;

	relation = list_get( relation_mto1_drillthru_list );

	if ( !relation->one_folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: one_folder is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return relation->one_folder->folder_name;
}

char *drillthru_base_folder_name(
			DICTIONARY *drillthru_dictionary,
			char *drillthru_base_folder_key )
{
	return
	dictionary_get(
		drillthru_base_folder_key,
		drillthru_dictionary );
}

void drillthru_base_folder_name_set(
			DICTIONARY *drillthru_dictionary,
			char *drillthru_base_folder_key,
			char *base_folder_name )
{
	dictionary_set(
		drillthru_dictionary,
		drillthru_base_folder_key,
		base_folder_name );
}

void drillthru_fulfilled_folder_name_list_set(
			DICTIONARY *drillthru_dictionary,
			char *drillthru_folder_list_key,
			LIST *fulfilled_folder_name_list )
{
	dictionary_set(
		drillthru_dictionary,
		drillthru_folder_list_key,
		list_display_delimited(
			fulfilled_folder_name_list,
			',' ) );
}

void drillthru_fulfilled_folder_name_set(
			DICTIONARY *drillthru_dictionary,
			LIST *fulfilled_folder_name_list,
			char *current_folder_name )
{
	list_set(
		fulfilled_folder_name_list,
		current_folder_name );

	drillthru_fulfilled_folder_name_list_set(
		drillthru_dictionary,
		DRILLTHRU_FOLDER_LIST_KEY,
		fulfilled_folder_name_list );
}

boolean drillthru_middle(
			char *folder_name,
			char *current_folder_name )
{
	return ( string_strcmp( folder_name, current_folder_name ) != 0 );
}

boolean drillthru_finished(
			char *folder_name,
			char *current_folder_name )
{
	return ( string_strcmp( folder_name, current_folder_name ) == 0 );
}
