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
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "security.h"
#include "environ.h"
#include "relation.h"
#include "dictionary_separate.h"
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

	post_choose_folder->drillthru =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		drillthru_start( folder_name );

	folder_name =
		drillthru_participating_folder_name(
			folder_name,
			post_choose_folder->drillthru );

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
			0 /* not fetch_row_level_restriction */,
			0 /* not fetch_role_operation_list */ );

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
			post_choose_folder->folder->relation_one2m_list );

	post_choose_folder->form_name =
		post_choose_folder_form_name(
			list_length(
				post_choose_folder->
					relation_pair_one2m_list ),
			post_choose_folder->folder->folder_form );
	
	post_choose_folder->choose_isa_drop_down =
		post_choose_folder_choose_isa_drop_down(
			list_length(
				post_choose_folder->
					folder->
					relation_mto1_isa_list ) );

	post_choose_folder->prompt_insert_form =
		post_choose_folder_prompt_insert_form(
			post_choose_folder->form_name,
			state );

	post_choose_folder->insert_table_form =
		post_choose_folder_insert_table_form(
			post_choose_folder->form_name,
			state );

	post_choose_folder->prompt_edit_form =
		post_choose_folder_prompt_edit_form(
			post_choose_folder->form_name,
			state,
			post_choose_folder->
				drillthru->
				drillthru_participating );

	post_choose_folder->edit_table_form =
		post_choose_folder_edit_table_form(
			post_choose_folder->form_name,
			state );

	post_choose_folder->system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_choose_folder_system_string(
			post_choose_folder->choose_isa_drop_down,
			post_choose_folder->prompt_insert_form,
			post_choose_folder->insert_table_form,
			post_choose_folder->prompt_edit_form,
			post_choose_folder->edit_table_form,
			application_name,
			login_name,
			session_key,
			folder_name,
			role_name,
			post_choose_folder_target_frame(
				post_choose_folder->insert_table_form,
				post_choose_folder->edit_table_form,
				post_choose_folder->
					drillthru->
					drillthru_participating ),
			state,
			post_choose_folder->
				drillthru->
				drillthru_dictionary,
			list_first(
				post_choose_folder->
					folder->
					relation_mto1_isa_list ) );

	if ( !post_choose_folder->system_string )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: post_choose_folder_system_string() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_choose_folder;
}

char *post_choose_folder_form_name(
			int relation_pair_one2m_list_length,
			char *folder_form )
{
	if ( relation_pair_one2m_list_length )
		return "prompt";
	else
	if ( !folder_form || !*folder_form )
		return "table";
	else
		return folder_form;
}

boolean post_choose_folder_fetch_relation_mto1_isa_list(
			char *state )
{
	return ( string_strcmp( state, "insert" ) == 0 );
}

boolean post_choose_folder_prompt_insert_form(
			char *folder_form_name,
			char *state )
{
	if ( string_strcmp( state, "insert" ) != 0 )
		return 0;

	return ( string_strcmp( folder_form_name, "prompt" ) == 0 );
}

boolean post_choose_folder_insert_table_form(
			char *folder_form_name,
			char *state )
{
	if ( string_strcmp( state, "insert" ) != 0 )
		return 0;

	return ( string_strcmp( folder_form_name, "table" ) == 0 );
}

boolean post_choose_folder_prompt_edit_form(
			char *folder_form_name,
			char *state,
			boolean drillthru_participating )
{

	if ( drillthru_participating )
		return 1;

	if ( string_strcmp( state, "insert" ) == 0 )
		return 0;

	return ( string_strcmp( folder_form_name, "prompt" ) == 0 );
}

boolean post_choose_folder_edit_table_form(
			char *folder_form_name,
			char *state )
{
	if ( string_strcmp( state, "insert" ) == 0 )
		return 0;

	return ( string_strcmp( folder_form_name, "table" ) == 0 );
}

char *post_choose_folder_system_string(
			boolean choose_isa_drop_down,
			boolean prompt_insert_form,
			boolean insert_table_form,
			boolean prompt_edit_form,
			boolean edit_table_form,
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *state,
			char *target_frame,
			DICTIONARY *drillthru_dictionary,
			RELATION *first_one2m_isa_relation )
{
	char system_string[ 1024 ];

	if ( choose_isa_drop_down )
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
			folder_name,
			first_one2m_isa_relation->one_folder->folder_name,
			role_name,
		 	appaserver_error_filename( application_name ) );
	}
	else
	if ( prompt_insert_form )
	{
		sprintf(system_string,
			"output_prompt_insert_form %s %s %s %s \"%s\" 2>>%s",
			login_name,
			session_key,
			folder_name,
			role_name,
			dictionary_separate_send_string(
				dictionary_separate_send_dictionary(
					(DICTIONARY *)0
						/* sort_dictionary */,
					(DICTIONARY *)0
						/* query_dictionary */,
					drillthru_dictionary,
					(DICTIONARY *)0
						/* ignore_dictionary */,
					(DICTIONARY *)0
						/* pair_one2m_dictionary */,
					(DICTIONARY *)0
						/* non_prefixed_dictionary */
					) ),
		 	appaserver_error_filename( application_name ) );
	}
	else
	if ( insert_table_form )
	{
		sprintf(system_string,
			"output_insert_table_form %s %s %s %s \"%s\" 2>>%s",
			login_name,
			session_key,
			folder_name,
			role_name,
			dictionary_separate_send_string(
				dictionary_separate_send_dictionary(
					(DICTIONARY *)0
						/* sort_dictionary */,
					(DICTIONARY *)0
						/* query_dictionary */,
					drillthru_dictionary,
					(DICTIONARY *)0
						/* ignore_dictionary */,
					(DICTIONARY *)0
						/* pair_one2m_dictionary */,
					(DICTIONARY *)0
						/* non_prefixed_dictionary */
					) ),
		 	appaserver_error_filename( application_name ) );
	}
	else
	if ( prompt_edit_form )
	{
		sprintf(system_string,
		"output_prompt_edit_form %s %s %s %s %s %s \"%s\" 2>>%s",
			login_name,
			session_key,
			folder_name,
			role_name,
			target_frame,
			state,
			dictionary_separate_send_string(
				dictionary_separate_send_dictionary(
					(DICTIONARY *)0
						/* sort_dictionary */,
					(DICTIONARY *)0
						/* query_dictionary */,
					drillthru_dictionary,
					(DICTIONARY *)0
						/* ignore_dictionary */,
					(DICTIONARY *)0
						/* pair_one2m_dictionary */,
					(DICTIONARY *)0
						/* non_prefixed_dictionary */
					) ),
		 	appaserver_error_filename( application_name ) );
	}
	if ( edit_table_form )
	{
		sprintf(system_string,
			"output_edit_table_form %s %s %s %s %s \"%s\" 2>>%s",
			login_name,
			session_key,
			folder_name,
			role_name,
			target_frame,
			dictionary_separate_send_string(
				dictionary_separate_send_dictionary(
					(DICTIONARY *)0
						/* sort_dictionary */,
					(DICTIONARY *)0
						/* query_dictionary */,
					drillthru_dictionary,
					(DICTIONARY *)0
						/* ignore_dictionary */,
					(DICTIONARY *)0
						/* pair_one2m_dictionary */,
					(DICTIONARY *)0
						/* non_prefixed_dictionary */
					) ),
		 	appaserver_error_filename( application_name ) );
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

char *post_choose_folder_target_frame(
			boolean insert_table_form,
			boolean edit_table_form,
			boolean drillthru_participating )
{
	return (	insert_table_form ||
	     		edit_table_form   ||
	     		drillthru_participating )
		? PROMPT_FRAME
		: EDIT_FRAME;
}

