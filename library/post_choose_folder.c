/* $APPASERVER_HOME/library/post_choose_folder.c	*/
/* ---------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "boolean.h"
#include "String.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_parameter_file.h"
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "security.h"
#include "environ.h"
#include "relation.h"
#include "post_choose_folder.h"

POST_CHOOSE_FOLDER *post_choose_folder_calloc( void )
{
	POST_CHOOSE_FOLDER *post_choose_folder;

	if ( ! ( post_choose_folder =
			calloc( 1, sizeof( POST_CHOOSE_FOLDER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return post_choose_folder;
}

POST_CHOOSE_FOLDER *post_choose_folder_fetch(
			/* ----------------------------------- */
			/* See session_folder_integrity_exit() */
			/* ----------------------------------- */
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *folder_name,
			char *state )
{
	POST_CHOOSE_FOLDER *post_choose_folder =
		post_choose_folder_calloc();

	post_choose_folder->fetch_relation_mto1_isa_list =
		post_choose_folder_fetch_relation_mto1_isa_list(
			state );

	post_choose_folder->folder =
		folder_fetch(
			folder_name,
			(char *)0 /* role_name */,
			(LIST *)0 /* exclude_attribute_name_list */,
			0 /* not fetch_folder_attribute_list */,
			0 /* not fetch_relation_mto1_non_isa_list */,
			post_choose_folder->fetch_relation_mto1_isa_list,
			1 /* fetch_relation_one2m_list */,
			0 /* not fetch_relation_one2m_recursive_list */,
			0 /* not fetch_process */,
			0 /* not fetch_role_folder_list */,
			0 /* not fetch_row_level_restriction */ );

	if ( !post_choose_folder->folder )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			folder_name );
		exit( 1 );
	}

	post_choose_folder->relation_pair_one2m_list =
		relation_pair_one2m_list(
			folder->relation_one2m_list );

	if ( ! ( post_choose_folder->drilldown =
			drilldown_fetch(
				folder_name /* base_folder_name */,
				dictionary_small_new() ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: drilldown_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !post_choose_folder->drilldown->current_folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: current_folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	post_choose_folder->form_name =
		post_choose_folder_form_name(
			list_length(
				post_choose_folder->
					relation_pair_one2m_list )
				/* relation_pair_one2m_list_length */,
			folder->folder_form );
	

	return post_choose_folder;
}

char *post_choose_folder_form_name(
			int relation_pair_one2m_list_length,
			char *folder_form )
{
	if ( relation_pair_one2m_list_length )
		return "prompt";
	else
	if ( !folder_form )
		return "table";
	else
		return folder_form;
}

boolean post_choose_folder_fetch_relation_mto1_isa_list(
			char *state )
{
	return ( string_strcmp( state, "insert" ) == 0 );
}
