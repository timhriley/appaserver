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
			post_choose_folder->folder->folder_form );
	
	post_choose_folder->output_choose_isa_drop_down =
		post_choose_folder_output_choose_isa_drop_down(
			list_length(
				post_choose_folder->
					folder->
					post_choose_folder->
					relation_mto1_isa_list )
					/* relation_mto1_isa_list_length */ );

	post_choose_folder->output_prompt_insert_form =
		post_choose_folder_output_prompt_insert_form(
			post_choose_folder->form_name,
			state );

	post_choose_folder->output_insert_table_form =
		post_choose_folder_output_insert_table_form(
			post_choose_folder->form_name,
			state );

	post_choose_folder->output_prompt_edit_form =
		post_choose_folder_output_prompt_edit_form(
			post_choose_folder->form_name,
			state );

	post_choose_folder->output_edit_table_form =
		post_choose_folder_output_edit_table_form(
			post_choose_folder->form_name,
			state );

	post_choose_folder->system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_choose_folder_system_string(
			output_choose_isa_drop_down,
			output_prompt_insert_form,
			output_insert_table_form,
			output_prompt_edit_form
			output_edit_table_form,
			application_name,
			login_name,
			session_key,
			current_folder_name,
			role_name,
			state,
			drilldown->drilldown_dictionary,
			list_first(
				post_choose_folder->
					folder->
					post_choose_folder->
					relation_mto1_isa_list ),
					/* first_one2m_isa_relation */ );

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

boolean post_choose_folder_output_prompt_insert_form(
			char *folder_form_name,
			char *state )
{
	if ( string_strcmp( state, "insert" ) != 0 )
		return 0;

	return ( string_strcmp( folder_form_name, "prompt" ) == 0 );
}

boolean post_choose_folder_output_insert_table_form(
			char *folder_form_name),
			char *state )
{
	if ( string_strcmp( state, "insert" ) != 0 )
		return 0;

	return ( string_strcmp( folder_form_name, "table" ) == 0 );
}

boolean post_choose_folder_output_prompt_edit_form(
			char *folder_form_name,
			char *state )
{
	if ( string_strcmp( state, "insert" ) == 0 )
		return 0;

	return ( string_strcmp( folder_form_name, "prompt" ) == 0 );
}

boolean post_choose_folder_output_edit_table_form(
			char *folder_form_name,
			char *state )
{
	if ( string_strcmp( state, "insert" ) == 0 )
		return 0;

	return ( string_strcmp( folder_form_name, "table" ) == 0 );
}

char *post_choose_folder_system_string(
			boolean output_choose_isa_drop_down,
			boolean output_prompt_insert_form,
			boolean output_insert_table_form,
			boolean output_prompt_edit_form
			boolean output_edit_table_form,
			char *application_name,
			char *login_name,
			char *session_key,
			char *current_folder_name,
			char *role_name,
			char *state,
			DICTIONARY *drilldown_dictionary,
			RELATION *first_one2m_isa_relation )
{
	char system_string[ 1024 ];

	if ( output_choose_isa_drop_down )
	{
		if ( !first_one2m_isa_relation )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: first_one2m_isa_relation is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !first_one2m_isa_relation->one_folder )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: one_folder is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		sprintf(system_string,
			"output_choose_isa_drop_down %s %s %s %s %s 2>>%s",
			login_name,
			session_key,
			current_folder_name,
			first_one2m_isa_relation->one_folder->folder_name,
			role_name,
			state,
		 	appaserver_error_filename( application_name ) );
	}
	else
	if (output_prompt_insert_form )
	{
	}
	else
	if (output_insert_table_form )
	{
	}
	else
	if (output_prompt_edit_form )
	{
	}
	if (output_edit_table_form )
	{
	}
	else
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: can't determine system_string\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return strdup( system_string );
}

