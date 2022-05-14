/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_drillthru.c				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "environ.h"
#include "dictionary.h"
#include "appaserver_error.h"
#include "list.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_parameter.h"
#include "element.h"
#include "javascript.h"
#include "frameset.h"
#include "button.h"
#include "appaserver.h"
#include "post_drillthru.h"
#include "form_prompt_lookup.h"
#include "form_drillthru.h"

FORM_DRILLTHRU *form_drillthru_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *state,
			char *folder_name,
			LIST *folder_attribute_list,
			LIST *relation_mto1_non_isa_list,
			char *security_entity_where,
			DICTIONARY *drillthru_dictionary )
{
	FORM_DRILLTHRU *form_drillthru = form_drillthru_calloc();
	char *tmp;

	form_drillthru->action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_drillthru_action_string(
			POST_DRILLTHRU_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			state,
			folder_name );

	form_drillthru->form_tag_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_tag_html(
			FORM_PROMPT_LOOKUP_NAME /* form_name */,
			form_drillthru->action_string,
			FRAMESET_PROMPT_FRAME /* target_frame */ );

	if ( ! ( form_drillthru->form_prompt_lookup_element_list =
			form_prompt_lookup_element_list_new(
				folder_attribute_list,
				relation_mto1_non_isa_list,
				(LIST *)0 /* relation_join_one2m_list */,
				drillthru_dictionary,
				login_name,
				security_entity_where ) ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: form_prompt_lookup_element_list_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	form_drillthru->form_multi_select_all_javascript =
		form_multi_select_all_javascript(
			form_drillthru->
				form_prompt_lookup_element_list->
				element_list );

	form_drillthru->form_cookie_key =
		form_cookie_key(
			FORM_PROMPT_LOOKUP_NAME /* form_name */,
			folder_name );

	form_drillthru->form_cookie_multi_key =
		form_cookie_multi_key(
			FORM_PROMPT_LOOKUP_NAME /* form_name */,
			folder_name );

	form_drillthru->form_keystrokes_save_javascript =
		form_keystrokes_save_javascript(
			FORM_PROMPT_LOOKUP_NAME /* form_name */,
			form_drillthru->form_cookie_key,
			form_drillthru->
				form_prompt_lookup_element_list->
				element_list );

	form_drillthru->form_keystrokes_multi_save_javascript =
		form_keystrokes_multi_save_javascript(
			FORM_PROMPT_LOOKUP_NAME /* form_name */,
			form_drillthru->form_cookie_multi_key,
			form_drillthru->
				form_prompt_lookup_element_list->
				element_list );

	form_drillthru->form_keystrokes_recall_javascript =
		form_keystrokes_recall_javascript(
			FORM_PROMPT_LOOKUP_NAME /* form_name */,
			form_drillthru->form_cookie_key,
			form_drillthru->
				form_prompt_lookup_element_list->
				element_list );

	form_drillthru->form_keystrokes_multi_recall_javascript =
		form_keystrokes_multi_recall_javascript(
			FORM_PROMPT_LOOKUP_NAME /* form_name */,
			form_drillthru->form_cookie_multi_key,
			form_drillthru->
				form_prompt_lookup_element_list->
				element_list );

	form_drillthru->form_prompt_lookup_button_list =
		form_prompt_lookup_button_list(
			form_drillthru->form_multi_select_all_javascript,
			form_drillthru->form_keystrokes_save_javascript,
			form_drillthru->form_keystrokes_multi_save_javascript,
			form_drillthru->form_keystrokes_recall_javascript,
			form_drillthru->form_keystrokes_multi_recall_javascript,
			(char *)0
				/* form_verify_notepad_widths_javascript */ );

	form_drillthru->html =
		form_prompt_lookup_html(
			form_drillthru->form_tag_html,
			(char *)0 /* radio_list_html */,
			form_drillthru->form_prompt_lookup_element_list->
				appaserver_element_list_html,
			( tmp =
				button_list_html(
					form_drillthru->
					     form_prompt_lookup_button_list ) ),
			form_close_html() );

	free( form_drillthru->
		form_prompt_lookup_element_list->
		appaserver_element_list_html );

	free( tmp );

	button_list_free( form_drillthru->form_prompt_lookup_button_list );

	return form_drillthru;
}

FORM_DRILLTHRU *form_drillthru_calloc( void )
{
	FORM_DRILLTHRU *form_drillthru;

	if ( ! ( form_drillthru = calloc( 1, sizeof( FORM_DRILLTHRU ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: () returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return form_drillthru;
}

char *form_drillthru_action_string(
			char *post_drillthru_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *state,
			char *folder_name )
{
	char action_string[ 1024 ];

	if ( !post_drillthru_executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !state
	||   !folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(action_string,
		"%s/%s?%s+%s+%s+%s+%s+%s",
		appaserver_library_http_prompt(
			appaserver_parameter_cgi_directory(),
			appaserver_library_server_address(),
			application_ssl_support_yn(
				application_name ),
			application_prepend_http_protocol_yn(
				application_name ) ),
		post_drillthru_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		state,
		folder_name );

	return strdup( action_string );
}

