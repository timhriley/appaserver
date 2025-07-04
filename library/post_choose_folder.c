/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/post_choose_folder.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "boolean.h"
#include "String.h"
#include "application.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "appaserver.h"
#include "security.h"
#include "environ.h"
#include "relation_mto1.h"
#include "relation_one2m.h"
#include "frameset.h"
#include "dictionary_separate.h"
#include "choose_isa.h"
#include "prompt_insert.h"
#include "table_insert.h"
#include "prompt_lookup.h"
#include "table_edit.h"
#include "execute_system_string.h"
#include "post_choose_folder.h"

POST_CHOOSE_FOLDER *post_choose_folder_calloc( void )
{
	POST_CHOOSE_FOLDER *post_choose_folder;

	if ( ! ( post_choose_folder =
			calloc( 1, sizeof ( POST_CHOOSE_FOLDER ) ) ) )
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

POST_CHOOSE_FOLDER *post_choose_folder_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *state,
		char *appaserver_data_directory )
{
	POST_CHOOSE_FOLDER *post_choose_folder;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !state
	||   !appaserver_data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	post_choose_folder = post_choose_folder_calloc();

	post_choose_folder->drillthru_start =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		drillthru_start_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			state,
			appaserver_data_directory );

	if ( post_choose_folder->drillthru_start->participating_boolean )
	{
		return post_choose_folder;
	}

	post_choose_folder->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			1 /* not fetch_folder_attribute_list */,
			0 /* not fetch_attribute */,
			1 /* cache_boolean */ );

	post_choose_folder->fetch_relation_mto1_isa =
		post_choose_folder_fetch_relation_mto1_isa(
			APPASERVER_INSERT_STATE,
			state );

	if ( post_choose_folder->fetch_relation_mto1_isa )
	{
		post_choose_folder->relation_mto1_isa_list =
			relation_mto1_isa_list(
				(LIST *)0 /* mto1_isa_list Pass in null */,
				folder_name,
				post_choose_folder->
					folder->
					folder_attribute_primary_key_list,
				0 /* not fetch_relation_one2m_list */,
				0 /* not fetch_relation_mto1_list */ );
	}

	post_choose_folder->relation_one2m_pair_list =
		relation_one2m_pair_list(
			folder_name,
			post_choose_folder->
				folder->
				folder_attribute_primary_key_list );

	post_choose_folder->form_name =
		/* ------------------------------------------------- */
		/* Returns either const parameter or appaserver_form */
		/* ------------------------------------------------- */
		post_choose_folder_form_name(
			FORM_APPASERVER_PROMPT,
			FORM_APPASERVER_TABLE,
			list_length(
				post_choose_folder->
					relation_one2m_pair_list )
				/* relation_one2m_pair_list_length */,
			post_choose_folder->folder->appaserver_form );
	
	post_choose_folder->isa_drop_down_boolean =
		post_choose_folder_isa_drop_down_boolean(
			list_length(
				post_choose_folder->
					relation_mto1_isa_list )
				/* relation_mto1_isa_list_length */ );

	post_choose_folder->prompt_insert_boolean =
		post_choose_folder_prompt_insert_boolean(
			APPASERVER_INSERT_STATE,
			FORM_APPASERVER_PROMPT,
			state,
			post_choose_folder->form_name );

	post_choose_folder->table_insert_boolean =
		post_choose_folder_table_insert_boolean(
			APPASERVER_INSERT_STATE,
			FORM_APPASERVER_TABLE,
			state,
			post_choose_folder->form_name );

	post_choose_folder->prompt_lookup_boolean =
		post_choose_folder_prompt_lookup_boolean(
			/* -------- */
			/* Not this */
			/* -------- */
			APPASERVER_INSERT_STATE,
			FORM_APPASERVER_PROMPT,
			state,
			post_choose_folder->form_name );

	post_choose_folder->table_edit_boolean =
		post_choose_folder_table_edit_boolean(
			/* -------- */
			/* Not this */
			/* -------- */
			APPASERVER_INSERT_STATE,
			FORM_APPASERVER_TABLE,
			state,
			post_choose_folder->form_name );

	post_choose_folder->appaserver_error_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_error_filename(
			application_name );

	post_choose_folder->system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_choose_folder_system_string(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			post_choose_folder->isa_drop_down_boolean,
			post_choose_folder->prompt_insert_boolean,
			post_choose_folder->table_insert_boolean,
			post_choose_folder->prompt_lookup_boolean,
			post_choose_folder->table_edit_boolean,
			post_choose_folder->appaserver_error_filename,
			list_first(
				post_choose_folder->
					relation_mto1_isa_list )
					/* relation_mto1_isa_first */ );

	return post_choose_folder;
}

char *post_choose_folder_form_name(
		const char *form_appaserver_prompt,
		const char *form_appaserver_table,
		int relation_pair_one2m_list_length,
		char *appaserver_form )
{
	if ( relation_pair_one2m_list_length )
		return (char *)form_appaserver_prompt;
	else
	if ( !appaserver_form || !*appaserver_form )
		return (char *)form_appaserver_table;
	else
		return appaserver_form;
}

boolean post_choose_folder_prompt_insert_boolean(
		const char *appaserver_insert_state,
		const char *form_appaserver_prompt,
		char *state,
		char *folder_form_name )
{
	if ( string_strcmp( state, (char *)appaserver_insert_state ) != 0 )
		return 0;

	return
	( string_strcmp(
		folder_form_name,
		(char *)form_appaserver_prompt ) == 0 );
}

boolean post_choose_folder_table_insert_boolean(
		const char *appaserver_insert_state,
		const char *form_appaserver_table,
		char *state,
		char *folder_form_name )
{
	if ( string_strcmp( state, (char *)appaserver_insert_state ) != 0 )
		return 0;

	return
	( string_strcmp(
		folder_form_name,
		(char *)form_appaserver_table ) == 0 );
}

boolean post_choose_folder_prompt_lookup_boolean(
		const char *appaserver_insert_state,
		const char *form_appaserver_prompt,
		char *state,
		char *folder_form_name )
{
	if ( string_strcmp( state, (char *)appaserver_insert_state ) == 0 )
		return 0;

	return
	( string_strcmp(
		folder_form_name,
		(char *)form_appaserver_prompt ) == 0 );
}

boolean post_choose_folder_table_edit_boolean(
		const char *appaserver_insert_state,
		const char *form_appaserver_table,
		char *state,
		char *folder_form_name )
{
	if ( string_strcmp( state, (char *)appaserver_insert_state ) == 0 )
		return 0;

	return
	( string_strcmp(
		folder_form_name,
		(char *)form_appaserver_table ) == 0 );
}

boolean post_choose_folder_isa_drop_down_boolean(
		int relation_mto1_isa_list_length )
{
	return (boolean)relation_mto1_isa_list_length;
}

char *post_choose_folder_system_string(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		boolean isa_drop_down_boolean,
		boolean prompt_insert_boolean,
		boolean table_insert_boolean,
		boolean prompt_lookup_boolean,
		boolean table_edit_boolean,
		char *appaserver_error_filename,
		RELATION_MTO1 *relation_mto1_isa_first )
{
	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !appaserver_error_filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( isa_drop_down_boolean )
	{
		if ( !relation_mto1_isa_first )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: relation_mto1_isa_first is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !relation_mto1_isa_first->one_folder_name )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: one_folder_name is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		execute_system_string_folder_isa(
			CHOOSE_ISA_EXECUTABLE,
			session_key,
			login_name,
			role_name,
			folder_name,
			relation_mto1_isa_first->one_folder_name,
		 	appaserver_error_filename );
	}
	else
	if ( prompt_insert_boolean )
	{
		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		execute_system_string_prompt_insert(
			PROMPT_INSERT_EXECUTABLE,
			session_key,
			login_name,
			role_name,
			folder_name,
			(char *)0 /* dictionary_separate_send_string */,
		 	appaserver_error_filename );
	}
	else
	if ( table_insert_boolean )
	{
		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		execute_system_string_table_insert(
			TABLE_INSERT_EXECUTABLE,
			session_key,
			login_name,
			role_name,
			folder_name,
			FRAMESET_PROMPT_FRAME /* target_frame */,
			(char *)0 /* results_string */,
			(char *)0 /* error_string */,
			(char *)0 /* dictionary_separate_send_string */,
		 	appaserver_error_filename );
	}
	else
	if ( prompt_lookup_boolean )
	{
		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		execute_system_string_prompt_lookup(
			PROMPT_LOOKUP_EXECUTABLE,
			session_key,
			login_name,
			role_name,
			folder_name,
			(char *)0 /* dictionary_separate_send_string */,
		 	appaserver_error_filename );
	}
	else
	if ( table_edit_boolean )
	{
		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		execute_system_string_table_edit(
			TABLE_EDIT_EXECUTABLE,
			session_key,
			login_name,
			role_name,
			folder_name,
			FRAMESET_PROMPT_FRAME /* target_frame */,
			(char *)0 /* results_string */,
			(char *)0 /* error_string */,
			(char *)0 /* dictionary_separate_send_string */,
		 	appaserver_error_filename );
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
}

boolean post_choose_folder_fetch_relation_mto1_isa(
		const char *appaserver_insert_state,
		char *state )
{
	return
	( string_strcmp( state, (char *)appaserver_insert_state ) == 0 );
}

char *post_choose_folder_action_string(
		char *post_choose_folder_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *state,
		char *folder_name )
{
	static char action_string[ 128 ];

	if ( !post_choose_folder_executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !state )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		action_string,
		sizeof ( action_string ),
		"%s/%s?%s+%s+%s+%s+%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_http_prompt(
			/* ------------------------------------------------ */
			/* Returns component of global_appaserver_parameter */
			/* ------------------------------------------------ */
			appaserver_parameter_cgi_directory(),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
			application_ssl_support_boolean( application_name ) ),
		post_choose_folder_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		state );

	if ( folder_name )
	{
		sprintf(action_string + strlen( action_string ),
			"+%s",
			folder_name );
	}

	return action_string;
}

