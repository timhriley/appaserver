/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/form_drillthru.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "appaserver_error.h"
#include "widget.h"
#include "frameset.h"
#include "button.h"
#include "appaserver.h"
#include "application.h"
#include "post_choose_folder.h"
#include "post_drillthru.h"
#include "form_prompt_lookup.h"
#include "form_drillthru.h"

FORM_DRILLTHRU *form_drillthru_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *state,
		char *base_folder_name,
		char *folder_name,
		boolean first_time_boolean,
		LIST *folder_attribute_list,
		LIST *relation_mto1_list,
		SECURITY_ENTITY *security_entity,
		DICTIONARY *drillthru_dictionary )
{
	FORM_DRILLTHRU *form_drillthru;
	WIDGET_CONTAINER *widget_container;
	char *tmp;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !state
	||   !folder_name
	||   !list_length( folder_attribute_list )
	||   !dictionary_length( drillthru_dictionary ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	form_drillthru = form_drillthru_calloc();

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

	form_drillthru->form_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_tag(
			FORM_PROMPT_LOOKUP_NAME,
			form_drillthru->action_string,
			FRAMESET_PROMPT_FRAME /* target_frame */ );

	form_drillthru->form_prompt_lookup_widget_list =
		form_prompt_lookup_widget_list_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name /* many_folder_name */,
			(char *)0 /* post_change_javascript */,
			relation_mto1_list,
			(LIST *)0 /* relation_mto1_isa_list */,
			(LIST *)0 /* relation_one2m_join_list */,
			folder_attribute_list,
			drillthru_dictionary,
			1 /*drillthru_status_active_boolean */,
			security_entity,
			0 /* not no_display_boolean */,
			0 /* not drop_down_extra_options_boolean */ );

	if ( !form_drillthru->
		form_prompt_lookup_widget_list )
	{
		char message[ 128 ];

		sprintf(message,
			"form_drillthru is incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list_set_first(
		form_drillthru->
			form_prompt_lookup_widget_list->
			widget_container_list,
			( widget_container =
				widget_container_new(
				table_open, (char *)0 ) ) );

	widget_container->table_open->border_boolean = 1;

	list_set(
		form_drillthru->
			form_prompt_lookup_widget_list->
			widget_container_list,
		widget_container_new(
			table_close, (char *)0 ) );

	form_drillthru->
		widget_hidden_container_list =
			widget_hidden_container_list(
				DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
				drillthru_dictionary );

	list_set_list(
		form_drillthru->
			form_prompt_lookup_widget_list->
			widget_container_list,
		form_drillthru->
			widget_hidden_container_list );

	list_set_list(
		form_drillthru->
			form_prompt_lookup_widget_list->
			widget_container_list,
		widget_container_back_to_top_list() );

	if ( !first_time_boolean )
	{
		form_drillthru->post_choose_folder_action_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			post_choose_folder_action_string(
				POST_CHOOSE_FOLDER_EXECUTABLE,
				application_name,
				session_key,
				login_name,
				role_name,
				APPASERVER_LOOKUP_STATE,
				base_folder_name );
	}

	form_drillthru->form_multi_select_all_javascript =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		form_multi_select_all_javascript(
			form_drillthru->
				form_prompt_lookup_widget_list->
				widget_container_list );

	form_drillthru->recall_save =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		recall_save_new(
			APPASERVER_LOOKUP_STATE,
			FORM_PROMPT_LOOKUP_NAME,
			folder_name,
			form_drillthru->
				form_prompt_lookup_widget_list->
				widget_container_list,
			application_ssl_support_boolean(
				application_name ) );

	form_drillthru->recall_load =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		recall_load_new(
			APPASERVER_LOOKUP_STATE,
			FORM_PROMPT_LOOKUP_NAME,
			folder_name,
			form_drillthru->
				form_prompt_lookup_widget_list->
				widget_container_list );

	form_drillthru->form_prompt_lookup_button_list =
		form_prompt_lookup_button_list(
			FORM_PROMPT_LOOKUP_NAME,
			form_drillthru_skip_button_boolean(
				first_time_boolean,
				state ),
			form_drillthru->post_choose_folder_action_string,
			form_drillthru->form_multi_select_all_javascript,
			form_drillthru->recall_save->javascript,
			form_drillthru->recall_load->javascript );

	form_drillthru->form_prompt_lookup_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		form_prompt_lookup_html(
			form_drillthru->form_tag,
			(LIST *)0 /* form_prompt_lookup_no_display_all_list */,
			(char *)0 /* radio_list_html */,
			(char *)0 /* form_prompt_lookp_ajax_javascript */,
			form_drillthru->
				form_prompt_lookup_widget_list->
				widget_container_list,
			( tmp =
				button_list_html(
					form_drillthru->
					     form_prompt_lookup_button_list ) ),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			form_close_tag() );

	free( tmp );

	button_list_free( form_drillthru->form_prompt_lookup_button_list );

	return form_drillthru;
}

FORM_DRILLTHRU *form_drillthru_calloc( void )
{
	FORM_DRILLTHRU *form_drillthru;

	if ( ! ( form_drillthru = calloc( 1, sizeof ( FORM_DRILLTHRU ) ) ) )
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
	char action_string[ STRING_1K ];

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
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_http_prompt(
			appaserver_parameter_cgi_directory(),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
			application_ssl_support_boolean(
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

boolean form_drillthru_skip_button_boolean(
		boolean first_time_boolean,
		char *state )
{
	if ( string_strcmp(
		state,
		APPASERVER_INSERT_STATE ) == 0 )
	{
		return 0;
	}
	else
	{
		return first_time_boolean;
	}
}
