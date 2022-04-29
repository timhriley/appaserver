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
#include "relation.h"
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
			DICTIONARY *drillthru_dictionary,
			char *folder_name )
{
	DRILLTHRU *drillthru = drillthru_calloc();

	drillthru->relation_mto1_drillthru_list =
		relation_mto1_drillthru_list(
			(LIST *)0 /* relation_list */,
			folder_name,
			(LIST *)0 /* fulfilled_folder_name_list */ );

	if ( ! ( drillthru->drillthru_participating =
		     drillthru_start_participating(
			list_length(
				drillthru->
					relation_mto1_drillthru_list ) ) ) )
	{
		return drillthru;
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

fprintf( stderr, "3)\n" );

	drillthru_base_set(
		drillthru_dictionary,
		DRILLTHRU_BASE_KEY,
		folder_name );

fprintf( stderr, "4)\n" );

	return drillthru;
}

DRILLTHRU *drillthru_continue(
			DICTIONARY *drillthru_dictionary,
			char *folder_name )
{
	DRILLTHRU *drillthru = drillthru_calloc();

	if ( !dictionary_length( drillthru_dictionary ) ) return drillthru;

	drillthru->base_folder_name =
		drillthru_base_folder_name(
			DRILLTHRU_BASE_KEY,
			drillthru_dictionary );

	if ( ! ( drillthru->drillthru_participating =
			drillthru_continue_participating(
				drillthru->base_folder_name ) ) )
	{
		return drillthru;
	}

	if ( ( drillthru->skipped =
		drillthru_skipped(
			dictionary_length( drillthru_dictionary ) ) ) )
	{
		return drillthru;
	}

	drillthru->fulfilled_folder_name_list =
		drillthru_fulfilled_folder_name_list(
			DRILLTHRU_FULFILLED_KEY,
			drillthru_dictionary );

	drillthru->relation_mto1_drillthru_list =
		relation_mto1_drillthru_list(
			(LIST *)0 /* relation_list */,
			drillthru->base_folder_name,
			drillthru->fulfilled_folder_name_list );

	drillthru->current_folder_name =
		drillthru_current_folder_name(
			drillthru->relation_mto1_drillthru_list,
			drillthru->base_folder_name );

	if ( ( drillthru->finished =
		drillthru_finished(
			folder_name,
			drillthru->current_folder_name ) ) )
	{
		return drillthru;
	}

	drillthru->current_fulfilled_folder_name_list =
		drillthru_current_fulfilled_folder_name_list(
			drillthru->fulfilled_folder_name_list /* out */,
			drillthru->current_folder_name );

	drillthru_dictionary_fulfilled_set(
		drillthru_dictionary /* out */,
		DRILLTHRU_FULFILLED_KEY,
		drillthru->current_fulfilled_folder_name_list );

	return drillthru;
}

LIST *drillthru_fulfilled_folder_name_list(
			char *drillthru_fulfilled_key,
			DICTIONARY *drillthru_dictionary )
{
	return
	list_string_extract(
		dictionary_get(
			drillthru_fulfilled_key,
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
			char *drillthru_base_key,
			DICTIONARY *drillthru_dictionary )
{
	return
	dictionary_get(
		drillthru_base_key,
		drillthru_dictionary );
}

void drillthru_base_set(
			DICTIONARY *drillthru_dictionary,
			char *drillthru_base_key,
			char *base_folder_name )
{
	dictionary_set(
		drillthru_dictionary,
		drillthru_base_key,
		base_folder_name );
}

void drillthru_dictionary_fulfilled_set(
			DICTIONARY *drillthru_dictionary,
			char *drillthru_fulfilled_key,
			LIST *fulfilled_folder_name_list )
{
	dictionary_set(
		drillthru_dictionary,
		drillthru_fulfilled_key,
		list_display_delimited(
			fulfilled_folder_name_list,
			',' ) );
}

boolean drillthru_skipped(
			int drillthru_dictionary_length )
{
	return ( drillthru_dictionary_length == 1 );
}

boolean drillthru_finished(
			char *folder_name,
			char *current_folder_name )
{
	return ( string_strcmp( folder_name, current_folder_name ) == 0 );
}

boolean drillthru_continue_participating(
			char *base_folder_name )
{
	if ( base_folder_name )
		return 1;
	else
		return 0;
}

boolean drillthru_start_participating(
			int relation_mto1_drillthru_list_length )
{
	if ( relation_mto1_drillthru_list_length )
		return 1;
	else
		return 0;
}

LIST *drillthru_current_fulfilled_folder_name_list(
			LIST *fulfilled_folder_name_list,
			char *current_folder_name )
{
	list_set(
		fulfilled_folder_name_list,
		current_folder_name );

	return fulfilled_folder_name_list;
}

char *drillthru_start_current_folder_name(
			LIST *relation_mto1_drillthru_list )
{
	RELATION *relation;

	if ( !list_tail( relation_mto1_drillthru_list ) ) return (char *)0;

	relation = list_get( relation_mto1_drillthru_list );

	if ( !relation->one_folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: relation->one_folder is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return relation->one_folder->folder_name;
}

char *drillthru_output_system_string(
			char *drillthru_output_executable,
			char *session_key,
			char *login_name,
			char *role_name,
			char *state,
			char *dictionary_separate_send_string,
			char *appaserver_error_filename )
{
	char system_string[ 1024 ];

	if ( !drillthru_output_executable
	||   !session_key
	||   !login_name
	||   !role_name
	||   !state
	||   !dictionary_separate_send_string
	||   !appaserver_error_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"%s %s %s %s %s \"%s\" 2>>%s",
		drillthru_output_executable,
		session_key,
		login_name,
		role_name,
		state,
		dictionary_separate_send_string,
		appaserver_error_filename );

	return strdup( system_string );
}
