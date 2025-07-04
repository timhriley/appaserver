/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/drillthru.c		 			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "String.h"
#include "appaserver.h"
#include "application.h"
#include "dictionary.h"
#include "appaserver_error.h"
#include "folder_attribute.h"
#include "relation_mto1.h"
#include "drillthru.h"

DRILLTHRU_STATUS *drillthru_status_new(
		DICTIONARY *drillthru_dictionary )
{
	DRILLTHRU_STATUS *drillthru_status;

	drillthru_status = drillthru_status_calloc();

	drillthru_status->base_folder_name =
		/* ------------------------------------------------- */
		/* Returns component of drillthru_dictionary or null */
		/* ------------------------------------------------- */
		drillthru_status_base_folder_name(
			DRILLTHRU_BASE_KEY,
			drillthru_dictionary );

	drillthru_status->participating_boolean =
		drillthru_status_participating_boolean(
			drillthru_status->base_folder_name );

	drillthru_status->first_time_boolean =
		drillthru_status_first_time_boolean(
			DRILLTHRU_FULFILLED_KEY,
			drillthru_dictionary );

	drillthru_status->skipped_boolean =
		drillthru_status_skipped_boolean(
			DRILLTHRU_SKIPPED_KEY,
			drillthru_dictionary,
			drillthru_status->first_time_boolean );

	drillthru_status->active_boolean =
		drillthru_status_active_boolean(
			drillthru_status->skipped_boolean );

	return drillthru_status;
}

DRILLTHRU_STATUS *drillthru_status_calloc( void )
{
	DRILLTHRU_STATUS *drillthru_status;

	if ( ! ( drillthru_status = calloc( 1, sizeof ( DRILLTHRU_STATUS ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return drillthru_status;
}

DRILLTHRU_START *drillthru_start_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *state,
		char *appaserver_data_directory )
{
	DRILLTHRU_START *drillthru_start;

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

	drillthru_start = drillthru_start_calloc();

	drillthru_start->folder_attribute_fetch_primary_key_list =
		folder_attribute_fetch_primary_key_list(
			folder_name );

	drillthru_start->relation_mto1_drillthru_list =
		relation_mto1_drillthru_list(
			(LIST *)0 /* relation_mto1_list Pass in null */,
			folder_name
				/* base_folder_name */,
			drillthru_start->
				folder_attribute_fetch_primary_key_list
				/* base_folder_primary_key_list */,
			(LIST *)0 /* fulfilled_folder_name_list */ );

	if ( ! ( drillthru_start->participating_boolean =
		     drillthru_start_participating_boolean(
			list_length(
				drillthru_start->
					relation_mto1_drillthru_list ) ) ) )
	{
		return drillthru_start;
	}

	drillthru_start->drillthru_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		drillthru_start_dictionary(
			DRILLTHRU_BASE_KEY,
			folder_name );

	drillthru_start->drillthru_current_folder_name =
		/* ---------------------------------------------------- */
		/* Returns list_tail( relation_mto1_drillthru_list )->	*/
		/*			one_folder->			*/
		/*			folder->			*/
		/*			folder_name			*/
		/* or null.						*/
		/* ---------------------------------------------------- */
		drillthru_current_folder_name(
			drillthru_start->relation_mto1_drillthru_list );

	if ( !drillthru_start->drillthru_current_folder_name )
	{
		char message[ 128 ];

		sprintf(message,
			"drillthru_current_folder_name() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	drillthru_start->drillthru_document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		drillthru_document_new(
			application_name,
			session_key,
			login_name,
			role_name,
			state,
			drillthru_start->drillthru_dictionary,
			appaserver_data_directory,
			folder_name /* base_folder_name */,
			drillthru_start->drillthru_current_folder_name,
			1 /* first_time_boolean */ );

	return drillthru_start;
}

boolean drillthru_start_participating_boolean(
		int relation_mto1_drillthru_list_length )
{
	if ( relation_mto1_drillthru_list_length )
		return 1;
	else
		return 0;
}

char *drillthru_current_folder_name( LIST *relation_mto1_drillthru_list )
{
	RELATION_MTO1 *relation_mto1;

	relation_mto1 = list_last( relation_mto1_drillthru_list );

	if ( relation_mto1 )
		return relation_mto1->one_folder_name;
	else
		return (char *)0;
}

DRILLTHRU_DOCUMENT *drillthru_document_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *state,
		DICTIONARY *drillthru_dictionary,
		char *appaserver_data_directory,
		char *base_folder_name,
		char *drillthru_current_folder_name,
		boolean first_time_boolean )
{
	DRILLTHRU_DOCUMENT *drillthru_document;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !state
	||   !dictionary_length( drillthru_dictionary )
	||   !appaserver_data_directory
	||   !base_folder_name
	||   !drillthru_current_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	drillthru_document = drillthru_document_calloc();

	drillthru_document->role =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		role_fetch(
			role_name,
			0 /* not fetch_attribute_exclude_list */ );

	drillthru_document->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			drillthru_current_folder_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */,
			1 /* cache_boolean */ );

	drillthru_document->relation_mto1_list =
		relation_mto1_list(
			drillthru_current_folder_name,
			drillthru_document->
				folder->
				folder_attribute_primary_key_list );

	drillthru_document->folder_row_level_restriction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_row_level_restriction_fetch(
			drillthru_current_folder_name );

	drillthru_document->security_entity =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		security_entity_new(
			login_name,
			drillthru_document->
				folder_row_level_restriction->
				non_owner_forbid,
			drillthru_document->
				role->
				override_row_restrictions );

	drillthru_document->application_menu_horizontal_boolean =
		application_menu_horizontal_boolean(
			application_name );

	if ( drillthru_document->application_menu_horizontal_boolean )
	{
		drillthru_document->folder_menu =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			folder_menu_fetch(
				application_name,
				session_key,
				role_name,
				appaserver_data_directory,
				0 /* not folder_menu_remove_boolean */ );

		drillthru_document->menu =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			menu_new(
				application_name,
				session_key,
				login_name,
				role_name,
				1 /* application_menu_horizontal_boolean */,
				drillthru_document->folder_menu->count_list );
	}

	drillthru_document->form_drillthru =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_drillthru_new(
			application_name,
			session_key,
			login_name,
			role_name,
			state,
			base_folder_name,
			drillthru_current_folder_name,
			first_time_boolean,
			drillthru_document->folder->folder_attribute_list,
			drillthru_document->relation_mto1_list,
			drillthru_document->security_entity,
			drillthru_dictionary );

	drillthru_document->folder_attribute_calendar_date_name_list_length =
		folder_attribute_calendar_date_name_list_length(
			drillthru_document->folder->folder_attribute_list );

	drillthru_document->title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		drillthru_document_title_string(
			state,
			base_folder_name );

	drillthru_document->sub_title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		drillthru_document_sub_title_string(
			drillthru_current_folder_name );

	drillthru_document->document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_new(
			application_name,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			application_title_string( application_name ),
			drillthru_document->title_string,
			drillthru_document->sub_title_string,
			(char *)0 /* sub_sub_title_string */,
			drillthru_document->folder->notepad,
			(char *)0 /* onload_javascript_string */,
			(drillthru_document->menu)
				? drillthru_document->menu->html
				: (char *)0,
			document_head_menu_setup_string(
				(drillthru_document->menu)
				? 1
				: 0 /* menu_boolean */ ),
			document_head_calendar_setup_string(
			   drillthru_document->
			     folder_attribute_calendar_date_name_list_length ),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_head_javascript_string()
				/* javascript_include_string */ );

	drillthru_document->document_form_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_form_html(
			drillthru_document->document->html,
			drillthru_document->document->document_head->html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_head_close_tag(),
			drillthru_document->document->document_body->html,
			drillthru_document->
				form_drillthru->
				form_prompt_lookup_html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_body_close_tag(),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_close_tag() );

	free( drillthru_document->form_drillthru->form_prompt_lookup_html );

	return drillthru_document;
}

DRILLTHRU_START *drillthru_start_calloc( void )
{
	DRILLTHRU_START *drillthru_start;

	if ( ! ( drillthru_start =
			calloc( 1,
				sizeof ( DRILLTHRU_START ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return drillthru_start;
}

DRILLTHRU_DOCUMENT *drillthru_document_calloc( void )
{
	DRILLTHRU_DOCUMENT *drillthru_document;

	if ( ! ( drillthru_document =
			calloc( 1,
				sizeof ( DRILLTHRU_DOCUMENT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return drillthru_document;
}

char *drillthru_document_title_string(
		char *state,
		char *base_folder_name )
{
	static char string[ 64 ];

	if ( !state
	||   !base_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(string,
		"%s %s",
		state,
		base_folder_name );

	return
	string_initial_capital(
		string,
		string );
}

char *drillthru_document_sub_title_string( char *current_folder_name )
{
	static char string[ 64 ];

	if ( !current_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "current_folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(string,
		"Drillthru %s",
		current_folder_name );

	return
	string_initial_capital(
		string,
		string );
}

DRILLTHRU_CONTINUE *drillthru_continue_new(
		DICTIONARY *drillthru_dictionary /* in/out */,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *current_folder_name,
		char *state,
		char *appaserver_data_directory )
{
	DRILLTHRU_CONTINUE *drillthru_continue;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !current_folder_name
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

	drillthru_continue = drillthru_continue_calloc();

	drillthru_continue->drillthru_status =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		drillthru_status_new(
			drillthru_dictionary );

	if ( !drillthru_continue->drillthru_status->participating_boolean )
	{
		return drillthru_continue;
	}

	if ( drillthru_continue->drillthru_status->skipped_boolean )
	{
		if ( strcmp(
			state,
			APPASERVER_INSERT_STATE ) == 0 )
		{
			drillthru_continue->drillthru_document =
				drillthru_document_new(
					application_name,
					session_key,
					login_name,
					role_name,
					state,
					drillthru_dictionary,
					appaserver_data_directory,
					drillthru_continue->
						drillthru_status->
						base_folder_name,
					current_folder_name,
					1 /* first_time_boolean */ );
		}

		return drillthru_continue;
	}

	drillthru_continue->fulfilled_folder_name_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		drillthru_continue_fulfilled_folder_name_list(
			DRILLTHRU_FULFILLED_KEY,
			DRILLTHRU_KEY_DELIMITER,
			drillthru_dictionary );

	list_set(
		drillthru_continue->fulfilled_folder_name_list,
		current_folder_name );

	drillthru_continue->folder_attribute_fetch_primary_key_list =
		folder_attribute_fetch_primary_key_list(
			drillthru_continue->
				drillthru_status->
				base_folder_name );

	drillthru_continue->relation_mto1_drillthru_list =
		relation_mto1_drillthru_list(
			(LIST *)0 /* relation_mto1_list Pass in null */,
			drillthru_continue->drillthru_status->base_folder_name,
			drillthru_continue->
				folder_attribute_fetch_primary_key_list,
			drillthru_continue->fulfilled_folder_name_list );

	drillthru_continue->completed_boolean =
		drillthru_continue_completed_boolean(
			list_length(
			    drillthru_continue->
				relation_mto1_drillthru_list
				/* relation_mto1_drillthru_list_length */ ) );

	if ( drillthru_continue->completed_boolean )
		return drillthru_continue;

	drillthru_continue->drillthru_current_folder_name =
		/* ---------------------------------------------------- */
		/* Returns list_tail( relation_mto1_drillthru_list )->	*/
		/*			one_folder->			*/
		/*			folder->			*/
		/*			folder_name			*/
		/* or null.						*/
		/* ---------------------------------------------------- */
		drillthru_current_folder_name(
			drillthru_continue->
				relation_mto1_drillthru_list );

	drillthru_continue->data_list_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		drillthru_continue_data_list_string(
			DRILLTHRU_KEY_DELIMITER,
			drillthru_continue->fulfilled_folder_name_list );

	dictionary_set(
		drillthru_dictionary,
		DRILLTHRU_FULFILLED_KEY,
		drillthru_continue->data_list_string );

	drillthru_continue->drillthru_document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		drillthru_document_new(
			application_name,
			session_key,
			login_name,
			role_name,
			state,
			drillthru_dictionary,
			appaserver_data_directory,
			drillthru_continue->drillthru_status->base_folder_name,
			drillthru_continue->drillthru_current_folder_name,
			0 /* not first_time_boolean */ );

	return drillthru_continue;
}

boolean drillthru_status_participating_boolean( char *base_folder_name )
{
	if ( base_folder_name )
		return 1;
	else
		return 0;
}

boolean drillthru_status_first_time_boolean(
		const char *drillthru_fulfilled_key,
		DICTIONARY *drillthru_dictionary )
{
	return
	!dictionary_key_exists(
		drillthru_dictionary,
		(char *)drillthru_fulfilled_key );
}

boolean drillthru_status_skipped_boolean(
		const char *drillthru_skipped_key,
		DICTIONARY *drillthru_dictionary,
		boolean first_time_boolean )
{
	int length;

	length = dictionary_length( drillthru_dictionary );

	if ( !length ) return 0;

	if ( dictionary_key_exists(
		drillthru_dictionary,
		(char *)drillthru_skipped_key ) )
	{
		return 1;
	}

	if ( !first_time_boolean )
		return 0;
	else
	if ( length > 1 )
		return 0;
	else
		return 1;
}

boolean drillthru_status_active_boolean(
	boolean skipped_boolean )
{
	return 1 - skipped_boolean;
}

DRILLTHRU_CONTINUE *drillthru_continue_calloc( void )
{
	DRILLTHRU_CONTINUE *drillthru_continue;

	if ( ! ( drillthru_continue =
			calloc( 1,
				sizeof ( DRILLTHRU_CONTINUE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return drillthru_continue;
}

char *drillthru_status_base_folder_name(
		const char *drillthru_base_key,
		DICTIONARY *drillthru_dictionary )
{
	return
	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	dictionary_get(
		(char *)drillthru_base_key,
		drillthru_dictionary );
}

LIST *drillthru_continue_fulfilled_folder_name_list(
		const char *drillthru_fulfilled_key,
		const char drillthru_key_delimiter,
		DICTIONARY *drillthru_dictionary )
{
	LIST *name_list;

	if ( !dictionary_length( drillthru_dictionary ) )
	{
		char message[ 128 ];

		sprintf(message, "drillthru_dictionary is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	name_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		list_string_list(
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				(char *)drillthru_fulfilled_key,
				drillthru_dictionary ),
			(char)drillthru_key_delimiter );

	return name_list;
}

boolean drillthru_continue_completed_boolean(
		int relation_mto1_drillthru_list_length )
{
	if ( !relation_mto1_drillthru_list_length )
		return 1;
	else
		return 0;
}

char *drillthru_continue_data_list_string(
		const char drillthru_key_delimiter,
		LIST *fulfilled_folder_name_list )
{
	if ( !list_length( fulfilled_folder_name_list ) )
	{
		char message[ 128 ];

		sprintf(message, "fulfilled_folder_name_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* ------------------------- */
	/* Returns heap memory or "" */
	/* ------------------------- */
	list_delimited(
		fulfilled_folder_name_list,
		(char)drillthru_key_delimiter );
}

DICTIONARY *drillthru_start_dictionary(
		const char *drillthru_base_key,
		char *folder_name )
{
	DICTIONARY *dictionary;

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	dictionary = dictionary_small();

	dictionary_set(
		dictionary,
		(char *)drillthru_base_key,
		folder_name );

	return dictionary;
}
