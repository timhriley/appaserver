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
#include "appaserver.h"
#include "security.h"
#include "environ.h"
#include "relation.h"
#include "frameset.h"
#include "dictionary_separate.h"
#include "choose_isa.h"
#include "prompt_insert.h"
#include "insert_table.h"
#include "prompt_edit.h"
#include "edit_table.h"
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

char *post_choose_folder_name(
			char *folder_name,
			char *drillthru_start_current_folder_name )
{
	if ( drillthru_start_current_folder_name
	&&   *drillthru_start_current_folder_name )
	{
		return drillthru_start_current_folder_name;
	}
	else
	{
		return folder_name;
	}
}

POST_CHOOSE_FOLDER *post_choose_folder_new(
			/* ----------------------------------- */
			/* See session_folder_integrity_exit() */
			/* ----------------------------------- */
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state )
{
	DICTIONARY *drillthru_dictionary;

	POST_CHOOSE_FOLDER *post_choose_folder =
		post_choose_folder_calloc();

	post_choose_folder->drillthru =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		drillthru_start(
			( drillthru_dictionary = dictionary_small() ) /* out */,
			folder_name );

	post_choose_folder->name =
		post_choose_folder_name(
			folder_name,
			drillthru_start_current_folder_name(
				post_choose_folder->
				     drillthru->
				     relation_mto1_drillthru_list ) );

	if ( !post_choose_folder->name )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: post_choose_folder_name() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (POST_CHOOSE_FOLDER *)0;
	}

	post_choose_folder->relation_isa_boolean =
		post_choose_folder_relation_isa_boolean(
			state,
			APPASERVER_INSERT_STATE );

	post_choose_folder->folder =
		folder_fetch(
			post_choose_folder_name(
				folder_name,
				drillthru_start_current_folder_name(
					post_choose_folder->
					     drillthru->
					     relation_mto1_drillthru_list ) ),
			(char *)0 /* role_name */,
			(LIST *)0 /* exclude_attribute_name_list */,
			0 /* not fetch_folder_attribute_list */,
			0 /* not fetch_relation_mto1_non_isa_list */,
			post_choose_folder->relation_isa_boolean,
			1 /* fetch_relation_one2m_list */,
			0 /* not fetch_relation_one2m_recursive_list */,
			0 /* not fetch_process */,
			0 /* not fetch_role_folder_list */,
			0 /* not fetch_row_level_restriction */,
			0 /* not fetch_role_operation_list */ );

	if ( !post_choose_folder->folder )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: folder_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			folder_name );
		return (POST_CHOOSE_FOLDER *)0;
	}

	post_choose_folder->folder->relation_pair_one2m_list =
		relation_pair_one2m_list(
			post_choose_folder->folder->relation_one2m_list );

	post_choose_folder->form_name =
		post_choose_folder_form_name(
			list_length(
				post_choose_folder->
					folder->
					relation_pair_one2m_list ),
			post_choose_folder->folder->folder_form );
	
	post_choose_folder->isa_drop_down =
		post_choose_folder_isa_drop_down(
			list_length(
				post_choose_folder->
					folder->
					relation_mto1_isa_list ) );

	post_choose_folder->prompt_insert =
		post_choose_folder_prompt_insert(
			post_choose_folder->form_name,
			state );

	post_choose_folder->insert_table =
		post_choose_folder_insert_table(
			post_choose_folder->form_name,
			state );

	post_choose_folder->prompt_edit =
		post_choose_folder_prompt_edit(
			post_choose_folder->form_name,
			state,
			post_choose_folder->
				drillthru->
				drillthru_participating );

	post_choose_folder->edit_table =
		post_choose_folder_edit_table(
			post_choose_folder->form_name,
			state );

	post_choose_folder->system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_choose_folder_system_string(
			post_choose_folder->isa_drop_down,
			post_choose_folder->prompt_insert,
			post_choose_folder->insert_table,
			post_choose_folder->prompt_edit,
			post_choose_folder->edit_table,
			application_name,
			session_key,
			login_name,
			role_name,
			post_choose_folder->folder->folder_name,
			state,
			/* ------------------------------------ */
			/* Returns frameset_prompt_frame or	*/
			/* frameset_edit_frame			*/
			/* ------------------------------------ */
			post_choose_folder_target_frame(
				post_choose_folder->insert_table,
				post_choose_folder->edit_table,
				post_choose_folder->
					drillthru->
					drillthru_participating,
				FRAMESET_PROMPT_FRAME,
				FRAMESET_EDIT_FRAME ),
			drillthru_dictionary,
			list_first(
				post_choose_folder->
					folder->
					relation_mto1_isa_list )
						/* first_one2m_is_relation */ );

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

boolean post_choose_folder_prompt_insert(
			char *folder_form_name,
			char *state )
{
	if ( string_strcmp( state, "insert" ) != 0 )
		return 0;

	return ( string_strcmp( folder_form_name, "prompt" ) == 0 );
}

boolean post_choose_folder_insert_table(
			char *folder_form_name,
			char *state )
{
	if ( string_strcmp( state, "insert" ) != 0 )
		return 0;

	return ( string_strcmp( folder_form_name, "table" ) == 0 );
}

boolean post_choose_folder_prompt_edit(
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

boolean post_choose_folder_edit_table(
			char *folder_form_name,
			char *state )
{
	if ( string_strcmp( state, "insert" ) == 0 )
		return 0;

	return ( string_strcmp( folder_form_name, "table" ) == 0 );
}

char *post_choose_folder_target_frame(
			boolean insert_table,
			boolean edit_table,
			boolean drillthru_participating,
			char *frameset_prompt_frame,
			char *frameset_edit_frame )
{
	return (	insert_table ||
	     		edit_table   ||
	     		drillthru_participating )
		? frameset_prompt_frame
		: frameset_edit_frame;
}

boolean post_choose_folder_isa_drop_down(
			int relation_mto1_isa_list_length )
{
	return (boolean)relation_mto1_isa_list_length;
}

char *post_choose_folder_system_string(
			boolean isa_drop_down,
			boolean prompt_insert,
			boolean insert_table,
			boolean prompt_edit,
			boolean edit_table,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state,
			char *target_frame,
			DICTIONARY *drillthru_dictionary,
			RELATION *first_one2m_isa_relation )
{
	char system_string[ 1024 ];

	if ( isa_drop_down )
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

		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		choose_isa_output_system_string(
			CHOOSE_ISA_OUTPUT_EXECUTABLE,
			login_name,
			session_key,
			folder_name,
			first_one2m_isa_relation->one_folder->folder_name,
			role_name,
		 	appaserver_error_filename( application_name ) );
	}
	else
	if ( prompt_insert )
	{
		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		prompt_insert_output_system_string(
			PROMPT_INSERT_OUTPUT_EXECUTABLE,
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
	if ( insert_table )
	{
		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		insert_table_output_system_string(
			INSERT_TABLE_OUTPUT_EXECUTABLE,
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
	if ( prompt_edit )
	{
		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		prompt_edit_output_system_string(
			PROMPT_EDIT_OUTPUT_EXECUTABLE,
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
	if ( edit_table )
	{
		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		edit_table_output_system_string(
			EDIT_TABLE_OUTPUT_EXECUTABLE,
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

char *post_choose_folder_action_string(
			char *post_choose_folder_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state,
			char *frameset_prompt_frame )
{
	char action_string[ 1024 ];

	if ( !post_choose_folder_executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !state
	||   !frameset_prompt_frame )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(action_string,
		"href=\"%s/%s?%s+%s+%s+%s+%s+%s\" target=%s",
			appaserver_library_http_prompt(
				appaserver_parameter_cgi_directory(),
				appaserver_library_server_address(),
				application_ssl_support_yn(
					application_name ),
				application_prepend_http_protocol_yn(
					application_name ) ),
		post_choose_folder_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		folder_name,
		state,
		frameset_prompt_frame );

	return strdup( action_string );
}

