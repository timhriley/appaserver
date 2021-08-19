/* $APPASERVER_HOME/library/drilldown.c		 */
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
#include "drilldown.h"

DRILLDOWN *drilldown_calloc( void )
{
	DRILLDOWN *drilldown;

	if ( ! ( drilldown = calloc( 1, sizeof( DRILLDOWN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return drilldown;
}

DRILLDOWN *drilldown_fetch(
			char *base_folder_name,
			DICTIONARY *drilldown_dictionary )
{
	DRILLDOWN *drilldown;

	if ( !base_folder_name || !*base_folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: base_folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 )
	}

	if ( !drilldown_dictionary )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: drilldown_dictionary is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 )
	}

	drilldown = drilldown_calloc();
	drilldown->base_folder_name = base_folder_name;
	drilldown->drilldown_dictionary = drilldown_dictionary;

	drilldown->fulfilled_folder_name_list =
		drilldown_fulfilled_folder_name_list(
			DRILLDOWN_FOLDER_LIST_KEY,
			drilldown->drilldown_dictionary );

	drilldown->relation_mto1_drilldown_list =
		relation_mto1_drilldown_list(
			(LIST *)0 /* relation_list */,
			base_folder_name,
			drilldown->fulfilled_folder_name_list );

	if ( !dictionary_key_exists(
		drilldown->drilldown_dictionary,
		DRILLDOWN_BASE_FOLDER_KEY ) )
	{
		drilldown_base_folder_name_set(
			drilldown->drilldown_dictionary,
			DRILLDOWN_BASE_FOLDER_KEY,
			base_folder_name );
	}

	drilldown->current_folder_name =
		drilldown_current_folder_name(
			drilldown->relation_mto1_drilldown_list,
			drilldown->base_folder_name );

	return drilldown;
}

LIST *drilldown_fulfilled_folder_name_list(
			char *drilldown_folder_list_key,
			DICTIONARY *drilldown_dictionary )
{
	return list_string_extract(
			dictionary_get(
				drilldown_dictionary,
				drilldown_folder_list_key ),
			',' );
}

char *drilldown_current_folder_name(
			LIST *relation_mto1_drilldown_list,
			char *base_folder_name )
{
	RELATION *relation;

	if ( !list_tail( relation_mto1_drilldown_list ) )
		return base_folder_name;

	relation = list_get( relation_mto1_drilldown_list );

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

void drilldown_base_folder_name_set(
			DICTIONARY *drilldown_dictionary,
			char *drilldown_folder_list_key,
			char *base_folder_name )
{
	dictionary_set(
		drilldown_dictionary,
		drilldown_folder_list_key,
		base_folder_name );
}

void drilldown_fulfilled_folder_name_list_set(
			DICTIONARY *drilldown_dictionary,
			char *drilldown_folder_list_key,
			LIST *fulfilled_folder_name_list )
{
	dictionary_set(
		drilldown_dictionary,
		drilldown_folder_list_key,
		list_display_delimited(
			fulfilled_folder_name_list,
			',' ) );
}

void drilldown_fulfilled_folder_name_set(
			DICTIONARY *drilldown_dictionary,
			LIST *fulfilled_folder_name_list,
			char *current_folder_name )
{
	list_set(
		fulfilled_folder_name_list,
		current_folder_name );

	drilldown_fulfilled_folder_name_list_set(
		drilldown_dictionary,
		DRILLDOWN_FOLDER_LIST_KEY,
		fulfilled_folder_name_list );
}

