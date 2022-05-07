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
#include "table_insert.h"
#include "prompt_lookup.h"
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

	if ( post_choose_folder->drillthru->drillthru_participating )
	{
		post_choose_folder->drillthru_output_system_string =
			drillthru_output_system_string(
				DRILLTHRU_OUTPUT_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				state,
				dictionary_separate_send_string(
				    dictionary_separate_send_dictionary(
					(DICTIONARY *)0
						/* sort_dictionary */,
					DICTIONARY_SEPARATE_SORT_PREFIX,
					(DICTIONARY *)0
						/* query_dictionary */,
					DICTIONARY_SEPARATE_QUERY_PREFIX,
					drillthru_dictionary,
					DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
					(DICTIONARY *)0
						/* ignore_dictionary */,
					DICTIONARY_SEPARATE_IGNORE_PREFIX,
					(DICTIONARY *)0
						/* no_display_dictionary */,
					DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
					(DICTIONARY *)0
						/* pair_one2m_dictionary */,
					DICTIONARY_SEPARATE_PAIR_PREFIX,
					(DICTIONARY *)0
						/* non_prefixed_dictionary */
					) ),
		 		appaserver_error_filename(
					application_name ) );

		return post_choose_folder;
	}

	post_choose_folder->name =
		/* ---------------------------------------------------------- */
		/* Returns drillthru_start_current_folder_name or folder_name */
		/* ---------------------------------------------------------- */
		post_choose_folder_name(
			folder_name,
			/* -------------------------------------------------- */
			/* Returns list_tail( relation_mto1_drillthru_list )->*/
			/*			one_folder->		      */
			/*			folder->		      */
			/*			folder_name		      */
			/* or null.					      */
			/* -------------------------------------------------- */
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

	post_choose_folder->fetch_relation_mto1_isa =
		post_choose_folder_fetch_relation_mto1_isa(
			state,
			APPASERVER_INSERT_STATE );

	post_choose_folder->folder =
		folder_fetch(
			post_choose_folder->name,
			(char *)0 /* role_name */,
			(LIST *)0 /* exclude_attribute_name_list */,
			0 /* not fetch_folder_attribute_list */,
			0 /* not fetch_relation_mto1_non_isa_list */,
			post_choose_folder->fetch_relation_mto1_isa,
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

	post_choose_folder->table_insert =
		post_choose_folder_table_insert(
			post_choose_folder->form_name,
			state );

	post_choose_folder->prompt_lookup =
		post_choose_folder_prompt_lookup(
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
			post_choose_folder->table_insert,
			post_choose_folder->prompt_lookup,
			post_choose_folder->edit_table,
			application_name,
			session_key,
			login_name,
			role_name,
			post_choose_folder->folder->folder_name,
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

boolean post_choose_folder_prompt_lookup(
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

boolean post_choose_folder_isa_drop_down(
			int relation_mto1_isa_list_length )
{
	return (boolean)relation_mto1_isa_list_length;
}

char *post_choose_folder_system_string(
			boolean isa_drop_down,
			boolean prompt_insert,
			boolean table_insert,
			boolean prompt_lookup,
			boolean edit_table,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
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
			session_key,
			login_name,
			role_name,
			folder_name,
			first_one2m_isa_relation->one_folder->folder_name,
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
			session_key,
			login_name,
			role_name,
			folder_name,
			(char *)0 /* dictionary_separate_send_string */,
		 	appaserver_error_filename( application_name ) );
	}
	else
	if ( table_insert )
	{
		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		table_insert_output_system_string(
			TABLE_INSERT_OUTPUT_EXECUTABLE,
			session_key,
			login_name,
			role_name,
			folder_name,
			FRAMESET_PROMPT_FRAME /* target_frame */,
			(char *)0 /* dictionary_separate_send_string */,
		 	appaserver_error_filename( application_name ) );
	}
	else
	if ( prompt_lookup )
	{
		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		prompt_lookup_output_system_string(
			PROMPT_EDIT_OUTPUT_EXECUTABLE,
			session_key,
			login_name,
			role_name,
			folder_name,
			(char *)0 /* dictionary_separate_send_string */,
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
			session_key,
			login_name,
			role_name,
			folder_name,
			FRAMESET_PROMPT_FRAME /* target_frame */,
			(char *)0 /* subsub_title */,
			(char *)0 /* dictionary_separate_send_string */,
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

char *post_choose_folder_href_string(
			char *post_choose_folder_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state,
			char *frameset_prompt_frame )
{
	char href_string[ 1024 ];

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

	sprintf(href_string,
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

	return strdup( href_string );
}

boolean post_choose_folder_fetch_relation_mto1_isa(
			char *state,
			char *appaserver_insert_state )
{
	return
	( string_strcmp( state, appaserver_insert_state ) == 0 );
}
