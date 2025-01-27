/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/post_prompt_lookup.c			*/
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
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "dictionary.h"
#include "frameset.h"
#include "lookup_statistic.h"
#include "group_count.h"
#include "lookup_spreadsheet.h"
#include "lookup_delete.h"
#include "lookup_grace.h"
#include "lookup_google.h"
#include "lookup_histogram.h"
#include "table_edit.h"
#include "prompt_lookup.h"
#include "lookup_sort.h"
#include "post_prompt_insert.h"
#include "execute_system_string.h"
#include "post_prompt_lookup.h"

POST_PROMPT_LOOKUP *post_prompt_lookup_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary )
{
	POST_PROMPT_LOOKUP *post_prompt_lookup;

	if ( !argc
	||   !argv
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	post_prompt_lookup = post_prompt_lookup_calloc();

	post_prompt_lookup->session_folder =
		/* ---------------------------------------------*/
		/* Sets appaserver environment and outputs argv.*/
		/* Each parameter is security inspected.	*/
		/* ---------------------------------------------*/
		session_folder_integrity_exit(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			APPASERVER_LOOKUP_STATE );

	post_prompt_lookup->post_prompt_lookup_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_prompt_lookup_input_new(
			FORM_RADIO_LIST_NAME,
			application_name,
			login_name,
			role_name,
			folder_name,
			original_post_dictionary );

	if ( string_strcmp(
		post_prompt_lookup->post_prompt_lookup_input->selection,
		RADIO_LOOKUP_VALUE ) == 0 )
	{
		post_prompt_lookup->query_drop_down_list =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_drop_down_list_new(
				post_prompt_lookup->
					post_prompt_lookup_input->
					relation_mto1_list,
				post_prompt_lookup->
					post_prompt_lookup_input->
					dictionary_separate->
					prompt_dictionary /* dictionary */,
				(char *)0 /* many_table_name */ );

		post_prompt_lookup->query_attribute_list =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_attribute_list_new(
				application_name,
				post_prompt_lookup->
					post_prompt_lookup_input->
					folder_attribute_append_isa_list,
				post_prompt_lookup->
					post_prompt_lookup_input->
					dictionary_separate->
					non_prefixed_dictionary
						/* dictionary */,
				list_length(
					post_prompt_lookup->
						post_prompt_lookup_input->
						relation_mto1_isa_list )
						/* relation_mto1...length */,
				(char *)0
				/* row_security_role_update_list_where */ );

		post_prompt_lookup->include_name_list =
			post_prompt_lookup_include_name_list(
				post_prompt_lookup->query_drop_down_list,
				post_prompt_lookup->query_attribute_list );

		post_prompt_lookup->missing_required_list =
			post_prompt_lookup_missing_required_list(
			     post_prompt_lookup->
				post_prompt_lookup_input->
				folder_attribute_lookup_required_name_list,
			     post_prompt_lookup->include_name_list );

		if ( list_length(
			post_prompt_lookup->missing_required_list ) )
		{
			post_prompt_lookup->missing_html =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				post_prompt_lookup_missing_html(
					POST_PROMPT_LOOKUP_MISSING_MESSAGE,
					folder_name,
					post_prompt_lookup->
						missing_required_list );
		}
	}
	else
	if ( string_strcmp(
		post_prompt_lookup->post_prompt_lookup_input->selection,
		RADIO_INSERT_VALUE ) == 0 )
	{
		post_prompt_lookup->relation_one2m_pair_list =
			relation_one2m_pair_list(
				/* ----------------------------------- */
				/* Set to cache all folders for a role */
				/* ----------------------------------- */
				role_name,
				folder_name
					/* one_folder_name */,
				post_prompt_lookup->
					post_prompt_lookup_input->
					folder->
					folder_attribute_primary_key_list
					/* one_primary_key_list */ );
	}

	post_prompt_lookup->dictionary_separate_send_dictionary =
		dictionary_separate_send_dictionary(
			(DICTIONARY *)0 /* sort_dictionary */,
			DICTIONARY_SEPARATE_SORT_PREFIX,
			(DICTIONARY *)0 /* query_dictionary */,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			post_prompt_lookup->
				post_prompt_lookup_input->
				dictionary_separate->
				drillthru_dictionary,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			post_prompt_lookup->
				post_prompt_lookup_input->
				dictionary_separate->
				no_display_dictionary,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			post_prompt_lookup->
				post_prompt_lookup_input->
				dictionary_separate->
				no_display_dictionary,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			(DICTIONARY *)0 /* pair_one2m_dictionary */,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			post_prompt_lookup->
				post_prompt_lookup_input->
				dictionary_separate->
				non_prefixed_dictionary );

	post_prompt_lookup->dictionary_separate_send_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		dictionary_separate_send_string(
			post_prompt_lookup->
				dictionary_separate_send_dictionary );

	post_prompt_lookup->execute_system_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		post_prompt_lookup_execute_system_string(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			post_prompt_lookup->
				post_prompt_lookup_input->
				dictionary_separate->
				non_prefixed_dictionary,
			post_prompt_lookup->
				post_prompt_lookup_input->
				appaserver_error_filename,
			post_prompt_lookup->
				post_prompt_lookup_input->
				selection,
			post_prompt_lookup->
				dictionary_separate_send_string );

	if ( !post_prompt_lookup->execute_system_string )
	{
		char message[ 128 ];

		sprintf(message,
	"post_prompt_lookup_execute_system_string(%s) returned empty.",
			post_prompt_lookup->
				post_prompt_lookup_input->
				selection );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return post_prompt_lookup;
}

POST_PROMPT_LOOKUP *post_prompt_lookup_calloc( void )
{
	POST_PROMPT_LOOKUP *post_prompt_lookup;

	if ( ! ( post_prompt_lookup =
			calloc( 1,
				sizeof ( POST_PROMPT_LOOKUP ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return post_prompt_lookup;
}

LIST *post_prompt_lookup_missing_required_list(
			LIST *lookup_required_name_list,
			LIST *lookup_include_name_list )
{
	return
	list_subtract(
		lookup_required_name_list /* list */,
		lookup_include_name_list /* subtract_this */ );
}

LIST *post_prompt_lookup_include_name_list(
		QUERY_DROP_DOWN_LIST *query_drop_down_list,
		QUERY_ATTRIBUTE_LIST *query_attribute_list )
{
	LIST *include_name_list;

	if ( !query_drop_down_list
	||   !query_attribute_list )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	include_name_list = list_new();

	list_set_list(
		include_name_list,
		query_drop_down_list->
			query_drop_down_relation_attribute_name_list );

	list_set_list(
		include_name_list,
		query_attribute_list_name_list(
			query_attribute_list->
				query_date_time_between_list,
			query_attribute_list->list
				/* query_attribute_list */ ) );

	return include_name_list;
}

char *post_prompt_lookup_missing_html(
			char *post_prompt_lookup_missing_message,
			char *folder_name,
			LIST *missing_required_list )
{
	char missing_html[ 1024 ];
	char *title_string;

	if ( !post_prompt_lookup_missing_message
	||   !folder_name
	||   !list_length( missing_required_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		prompt_lookup_title_string(
			APPASERVER_LOOKUP_STATE,
			folder_name );

	sprintf(missing_html,
		"<h1>%s</h1>\n<h3>%s %s</h3>",
		title_string,
		post_prompt_lookup_missing_message,
		list_delimited(
			missing_required_list,
			',' ) );

	return strdup( missing_html );
}

char *post_prompt_lookup_execute_system_string(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *non_prefixed_dictionary,
		char *appaserver_error_filename,
		char *selection,
		char *dictionary_separate_send_string )
{
	char *execute_system_string = {0};

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !dictionary_length( non_prefixed_dictionary )
	||   !appaserver_error_filename
	||   !selection )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strcmp(
		selection,
		RADIO_STATISTICS_VALUE ) == 0 )
	{
		execute_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			execute_system_string_lookup_statistic(
				LOOKUP_STATISTIC_EXECUTABLE,
				login_name,
				role_name,
				folder_name,
				dictionary_separate_send_string,
				appaserver_error_filename );
	}
	else
	if ( strcmp(
		selection,
		RADIO_GROUP_COUNT_VALUE ) == 0 )
	{
		execute_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			execute_system_string_group_count(
				GROUP_COUNT_EXECUTABLE,
				login_name,
				role_name,
				folder_name,
				dictionary_separate_send_string,
				appaserver_error_filename );
	}
	else
	if ( strcmp(
		selection,
		RADIO_SPREADSHEET_VALUE ) == 0 )
	{
		execute_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			execute_system_string_lookup_spreadsheet(
				LOOKUP_SPREADSHEET_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				folder_name,
				dictionary_separate_send_string,
				appaserver_error_filename );
	}
	else
	if ( strcmp(
		selection,
		RADIO_INSERT_VALUE ) == 0 )
	{
		execute_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			execute_system_string_post_prompt_insert(
				POST_PROMPT_INSERT_EXECUTABLE,
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				dictionary_separate_send_string,
				appaserver_error_filename );
	}
	else
	if ( strcmp(
		selection,
		RADIO_DELETE_VALUE ) == 0 )
	{
		execute_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			execute_system_string_lookup_delete(
				LOOKUP_DELETE_EXECUTABLE,
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				dictionary_separate_send_string,
				appaserver_error_filename,
				"one" /* one_two_state */ );
	}
	else
	if ( strcmp(
		selection,
		RADIO_GRACE_VALUE ) == 0 )
	{
		execute_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			execute_system_string_lookup_grace(
				LOOKUP_GRACE_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				folder_name,
				dictionary_separate_send_string,
				appaserver_error_filename );
	}
	else
	if ( strcmp(
		selection,
		RADIO_GOOGLE_VALUE ) == 0 )
	{
		execute_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			execute_system_string_lookup_google(
				LOOKUP_GOOGLE_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				folder_name,
				dictionary_separate_send_string,
				appaserver_error_filename );
	}
	else
	if ( strcmp(
		selection,
		RADIO_HISTOGRAM_VALUE ) == 0 )
	{
		execute_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			execute_system_string_lookup_histogram(
				LOOKUP_HISTOGRAM_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				folder_name,
				dictionary_separate_send_string,
				appaserver_error_filename );
	}
	else
	if ( strcmp(
		selection,
		RADIO_SORT_VALUE ) == 0 )
	{
		execute_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			execute_system_string_lookup_sort(
				LOOKUP_SORT_EXECUTABLE,
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				dictionary_separate_send_string,
				appaserver_error_filename );
	}
	else
	if ( strcmp(
		selection,
		RADIO_LOOKUP_VALUE ) == 0 )
	{
		execute_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			execute_system_string_table_edit(
				TABLE_EDIT_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				folder_name,
				FRAMESET_TABLE_FRAME /* target_frame */,
				(char *)0 /* results_string */,
				(char *)0 /* error_string */,
				dictionary_separate_send_string,
				appaserver_error_filename );
	}

	return execute_system_string;
}

char *post_prompt_lookup_input_selection(
			char *form_radio_list_name,
			DICTIONARY *non_prefixed_dictionary )
{
	char *selection;

	if ( !form_radio_list_name
	||   !dictionary_length( non_prefixed_dictionary ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( selection =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				form_radio_list_name,
				non_prefixed_dictionary ) ) )
	{
		char message[ 65536 ];

		sprintf(message,
	"dictionary_get(%s) returned empty using non_prefixed_dictionary=[%s].",
			form_radio_list_name,
			dictionary_display( non_prefixed_dictionary ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return selection;
}

POST_PROMPT_LOOKUP_INPUT *post_prompt_lookup_input_new(
		char *form_radio_list_name,
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary )
{
	POST_PROMPT_LOOKUP_INPUT *post_prompt_lookup_input;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	post_prompt_lookup_input = post_prompt_lookup_input_calloc();

	post_prompt_lookup_input->role =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		role_fetch(
			role_name,
			1 /* fetch_role_attribute_exclude_list */ );

	post_prompt_lookup_input->role_attribute_exclude_lookup_name_list =
		role_attribute_exclude_lookup_name_list(
			ROLE_PERMISSION_LOOKUP,
			ROLE_PERMISSION_UPDATE,
			post_prompt_lookup_input->
				role->
				role_attribute_exclude_list );

	post_prompt_lookup_input->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			post_prompt_lookup_input->
				role_attribute_exclude_lookup_name_list,
			/* -------------------------------------- */
			/* Sets folder_attribute_primary_list	  */
			/* Sets folder_attribute_primary_key_list */
			/* Sets folder_attribute_name_list	  */
			/* -------------------------------------- */
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */ );

	if ( !list_length(
		post_prompt_lookup_input->folder->folder_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message, "folder->folder_attribute_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	post_prompt_lookup_input->relation_mto1_list =
		relation_mto1_list(
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			folder_name
				/* many_folder_name */,
			post_prompt_lookup_input->
				folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */ );

	post_prompt_lookup_input->relation_mto1_isa_list =
		relation_mto1_isa_list(
			(LIST *)0 /* mto1_isa_list */,
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			folder_name
				/* many_folder_name */,
			post_prompt_lookup_input->
				folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */,
			0 /* not fetch_relation_one2m_list */,
			0 /* not fetch_relation_mto1_list */ );

	post_prompt_lookup_input->folder_attribute_append_isa_list =
		folder_attribute_append_isa_list(
			post_prompt_lookup_input->
				folder->
				folder_attribute_list
				/* append_isa_list */,
			post_prompt_lookup_input->
				relation_mto1_isa_list );

	post_prompt_lookup_input->
		folder_attribute_name_list =
			folder_attribute_name_list(
				(char *)0 /* folder_name */,
				post_prompt_lookup_input->
					folder_attribute_append_isa_list );

	post_prompt_lookup_input->folder_attribute_date_name_list =
		folder_attribute_date_name_list(
			post_prompt_lookup_input->
				folder_attribute_append_isa_list );

	post_prompt_lookup_input->folder_attribute_lookup_required_name_list =
		folder_attribute_lookup_required_name_list(
			post_prompt_lookup_input->
				folder_attribute_append_isa_list );

	post_prompt_lookup_input->dictionary_separate =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_post_prompt_lookup_new(
			original_post_dictionary,
			application_name,
			login_name,
			post_prompt_lookup_input->
				relation_mto1_list,
			post_prompt_lookup_input->
				folder_attribute_name_list,
			post_prompt_lookup_input->
				folder_attribute_date_name_list,
			post_prompt_lookup_input->
				folder_attribute_append_isa_list );

	post_prompt_lookup_input->drillthru_status =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		drillthru_status_new(
			post_prompt_lookup_input->
				dictionary_separate->
				drillthru_dictionary );

	post_prompt_lookup_input->selection =
		/* -------------------------------------------- */
		/* Returns component of non_prefixed_dictionary */
		/* -------------------------------------------- */
		post_prompt_lookup_input_selection(
			form_radio_list_name,
			post_prompt_lookup_input->
				dictionary_separate->
				non_prefixed_dictionary );

	post_prompt_lookup_input->appaserver_error_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_error_filename(
			application_name );

	return post_prompt_lookup_input;
}

POST_PROMPT_LOOKUP_INPUT *post_prompt_lookup_input_calloc( void )
{
	POST_PROMPT_LOOKUP_INPUT *post_prompt_lookup_input;

	if ( ! ( post_prompt_lookup_input =
			calloc( 1,
				sizeof ( POST_PROMPT_LOOKUP_INPUT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return post_prompt_lookup_input;
}

