/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/table_insert.c				*/
/* -------------------------------------------------------------------- */
/* No warrancy and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "String.h"
#include "environ.h"
#include "javascript.h"
#include "application.h"
#include "folder_attribute.h"
#include "frameset.h"
#include "insert.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "dictionary_separate.h"
#include "vertical_new_prompt.h"
#include "table_insert.h"

TABLE_INSERT *table_insert_calloc( void )
{
	TABLE_INSERT *table_insert;

	if ( ! ( table_insert = calloc( 1, sizeof( TABLE_INSERT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return table_insert;
}

TABLE_INSERT *table_insert_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		char *results_string,
		char *error_string,
		DICTIONARY *original_post_dictionary )
{
	TABLE_INSERT *table_insert;
	char *form_html;

	if ( !application_name
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

	table_insert = table_insert_calloc();

	table_insert->table_insert_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		table_insert_input_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			target_frame,
			original_post_dictionary );

	if ( table_insert->table_insert_input->table_insert_forbid )
	{
		char message[ 128 ];

		sprintf(message,
			"table_insert_forbid(%s,%s,%s) returned true.",
			login_name,
			role_name,
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	table_insert->entire_primary_key_ignored =
		table_insert_entire_primary_key_ignored(
			table_insert->
				table_insert_input->
				folder->
				folder_attribute_primary_key_list,
			table_insert->
				table_insert_input->
				dictionary_separate->
				ignore_name_list );

	if ( table_insert->entire_primary_key_ignored )
		return table_insert;

	table_insert->vertical_new_table =
		vertical_new_table_new(
			VERTICAL_NEW_PROMPT_ONE_HIDDEN_LABEL,
			VERTICAL_NEW_PROMPT_MANY_HIDDEN_LABEL,
			FRAMESET_PROMPT_FRAME,
			application_name,
			session_key,
			login_name,
			role_name,
			table_insert->
				table_insert_input->
				appaserver_parameter_data_directory,
			table_insert->
				table_insert_input->
				dictionary_separate->
				non_prefixed_dictionary,
			table_insert->
				table_insert_input->
				menu_horizontal_boolean );

	if ( table_insert->
		table_insert_input->
		relation_mto1_automatic_preselection )
	{
		if ( !table_insert->
			table_insert_input->
			pair_one2m_table_insert )
		{
			char message[ 128 ];

			sprintf(message, "pair_one2m_table_insert is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		table_insert->query_dictionary =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_dictionary_fetch(
				application_name,
				table_insert->
					table_insert_input->
					dictionary_separate->
					prompt_dictionary,
				table_insert->
					table_insert_input->
					relation_mto1_automatic_preselection->
					many_folder_name,
				table_insert->
					table_insert_input->
					relation_mto1_automatic_preselection->
					relation_foreign_key_list
					/* select_attribute_name_list */,
				table_insert->
					table_insert_input->
					pair_one2m_table_insert->
					folder_attribute_primary_key_list
				       /* where_attribute_name_list */ );

		table_insert->form_table_insert_automatic =
			form_table_insert_automatic_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				target_frame,
				table_insert->
					table_insert_input->
					folder->
					post_change_javascript,
				table_insert->
					table_insert_input->
					relation_mto1_list
					/* many_folder_relation_mto1_list */,
				table_insert->
					table_insert_input->
					relation_mto1_isa_list,
				table_insert->
					table_insert_input->
					folder_attribute_append_isa_list,
				table_insert->
					table_insert_input->
					dictionary_separate->
					drillthru_dictionary,
				table_insert->
					table_insert_input->
					dictionary_separate->
					prompt_dictionary,
				table_insert->
					table_insert_input->
					dictionary_separate->
					pair_one2m_dictionary,
				table_insert->
					table_insert_input->
					dictionary_separate->
					prompt_name_list,
				table_insert->
					table_insert_input->
					security_entity,
				table_insert->
					table_insert_input->
					relation_mto1_automatic_preselection,
				table_insert->
					query_dictionary->
					delimited_list );
	}
	else
	{
		table_insert->rows_number =
			table_insert_rows_number(
				table_insert->
					table_insert_input->
					folder->
					insert_rows_number,
				table_insert->
					vertical_new_table );

		/* Create a hidden widget for the many_folder_name */
		/* ----------------------------------------------- */
		table_insert->prompt_dictionary =
/* ------------------------------------------------------------------- */
/* Returns dictionary_separate_prompt_dictionary or dictionary_small() */
/* ------------------------------------------------------------------- */
			table_insert_prompt_dictionary(
				VERTICAL_NEW_PROMPT_MANY_HIDDEN_LABEL,
				table_insert->
					table_insert_input->
					dictionary_separate->
					prompt_dictionary,
				table_insert->
					vertical_new_table );

		table_insert->form_table_insert =
			form_table_insert_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				target_frame,
				table_insert->rows_number,
				table_insert->
					table_insert_input->
					folder->
					post_change_javascript,
				table_insert->
					table_insert_input->
					folder_attribute_non_primary_name_list,
				table_insert->
					table_insert_input->
					relation_mto1_list,
				table_insert->
					table_insert_input->
					relation_mto1_isa_list,
				table_insert->
					table_insert_input->
					folder_attribute_append_isa_list,
				table_insert->
					table_insert_input->
					dictionary_separate->
					drillthru_dictionary,
				table_insert->prompt_dictionary,
				dictionary_prefix(
					table_insert->
						table_insert_input->
						dictionary_separate->
						pair_one2m_dictionary,
					DICTIONARY_SEPARATE_PAIR_PREFIX ),
				table_insert->
					table_insert_input->
					dictionary_separate->
					ignore_name_list,
				table_insert->
					table_insert_input->
					dictionary_separate->
					prompt_name_list,
				table_insert->
					table_insert_input->
					security_entity );
	}

	table_insert->document_head_calendar_setup_string =
		/* ------------------------------ */
		/* Returns program memory or null */
		/* ------------------------------ */
		document_head_calendar_setup_string(
			table_insert->
			    table_insert_input->
			    folder_attribute_calendar_date_name_list_length );

	if ( error_string && *error_string ) results_string = NULL;

	table_insert->onload_javascript_string =
		/* ----------------------------------------------- */
		/* Returns component of vertical_new_table or null */
		/* ----------------------------------------------- */
		table_insert_onload_javascript_string(
			table_insert->
				vertical_new_table );

	table_insert->javascript_filename_list =
		table_insert_javascript_filename_list(
			table_insert->
				table_insert_input->
				folder->
				javascript_filename );

	table_insert->javascript_include_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		javascript_include_string(
			application_name,
			table_insert->javascript_filename_list );

	table_insert->document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_new(
			application_name,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			application_title_string( application_name ),
			table_insert->
				table_insert_input->
				title_string,
			results_string /* sub_title_string */,
			error_string /* sub_sub_title_string */,
			table_insert->
				table_insert_input->
				folder->
				notepad,
			table_insert->onload_javascript_string,
			(table_insert->table_insert_input->menu)
				? table_insert->table_insert_input->menu->html
				: (char *)0,
			/* ------------------------------ */
			/* Returns program memory or null */
			/* ------------------------------ */
			document_head_menu_setup_string(
				(table_insert->table_insert_input->menu)
					? 1
					: 0 /* menu_boolean */ ),
			table_insert->document_head_calendar_setup_string,
			table_insert->javascript_include_string );

	form_html =
		/* --------------------- */
		/* Returns either's html */
		/* --------------------- */
		table_insert_form_html(
			table_insert->form_table_insert_automatic,
			table_insert->form_table_insert );

	table_insert->document_form_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_form_html(
			table_insert->document->html,
			table_insert->document->document_head->html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_head_close_tag(),
			table_insert->document->document_body->html,
			form_html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_body_close_tag(),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_close_tag() );

	free( form_html );

	return table_insert;
}

boolean table_insert_forbid(
		boolean role_folder_insert_boolean )
{
	return 1 - role_folder_insert_boolean;
}

char *table_insert_input_title_string(
		char *appaserver_insert_state,
		char *folder_name )
{
	static char string[ 128 ];

	if ( !appaserver_insert_state
	||   !folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(string,
		"%s %s",
		appaserver_insert_state,
		folder_name );

	return
	string_initial_capital(
		string,
		string );
}

char *table_insert_form_html(
		FORM_TABLE_INSERT_AUTOMATIC *
			form_table_insert_automatic,
		FORM_TABLE_INSERT *
			form_table_insert )
{
	if ( !form_table_insert_automatic
	&&   !form_table_insert )
	{
		char message[ 128 ];

		sprintf(message, "both parameters are empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( form_table_insert_automatic )
		return form_table_insert_automatic->html;
	else
		return form_table_insert->html;
}

TABLE_INSERT_INPUT *table_insert_input_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		DICTIONARY *original_post_dictionary )
{
	TABLE_INSERT_INPUT *table_insert_input;

	if ( !application_name
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

	table_insert_input = table_insert_input_calloc();

	table_insert_input->role_folder_list =
		role_folder_list(
			role_name,
			folder_name );

	table_insert_input->role_folder_insert_boolean =
		role_folder_insert_boolean(
			ROLE_PERMISSION_INSERT,
			folder_name,
			table_insert_input->role_folder_list );

	table_insert_input->table_insert_forbid =
		table_insert_forbid(
			table_insert_input->role_folder_insert_boolean );

	if ( table_insert_input->table_insert_forbid )
		return table_insert_input;

	table_insert_input->title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		table_insert_input_title_string(
			APPASERVER_INSERT_STATE,
			folder_name );

	table_insert_input->role =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		role_fetch(
			role_name,
			1 /* fetch_attribute_exclude_list */ );

	table_insert_input->role_attribute_exclude_insert_name_list =
		role_attribute_exclude_insert_name_list(
			ROLE_PERMISSION_INSERT,
			table_insert_input->role->role_attribute_exclude_list );

	table_insert_input->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			table_insert_input->
				role_attribute_exclude_insert_name_list,
			/* -------------------------------------- */
			/* Sets folder_attribute_primary_list	  */
			/* Sets folder_attribute_primary_key_list */
			/* Sets folder_attribute_name_list	  */
			/* -------------------------------------- */
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */ );

	table_insert_input->folder_attribute_non_primary_name_list =
		folder_attribute_non_primary_name_list(
			table_insert_input->folder->folder_attribute_list );

	table_insert_input->relation_mto1_list =
		relation_mto1_list(
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			folder_name
				/* many_folder_name */,
			table_insert_input->
				folder->
				folder_attribute_primary_key_list
				/* many_primary_key_list */ );

	table_insert_input->relation_mto1_automatic_preselection =
		relation_mto1_automatic_preselection(
			table_insert_input->relation_mto1_list );

	table_insert_input->relation_mto1_isa_list =
		relation_mto1_isa_list(
			(LIST *)0 /* mto1_isa_list */,
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			folder_name
				/* many_folder_name */,
			table_insert_input->
				folder->
				folder_attribute_primary_key_list
				/* many_primary_key_list */,
			0 /* not fetch_relation_one2m_list */,
			1 /* fetch_relation_mto1_list */ );

	table_insert_input->folder_attribute_append_isa_list =
		folder_attribute_append_isa_list(
			table_insert_input->
				folder->
				folder_attribute_list
				/* append_isa_list */,
			table_insert_input->relation_mto1_isa_list );

	table_insert_input->folder_attribute_date_name_list =
		folder_attribute_date_name_list(
			table_insert_input->folder_attribute_append_isa_list );

	table_insert_input->folder_attribute_calendar_date_name_list_length =
		folder_attribute_calendar_date_name_list_length(
			table_insert_input->folder_attribute_append_isa_list );

	table_insert_input->dictionary_separate =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_table_insert_new(
			original_post_dictionary,
			application_name,
			login_name,
			table_insert_input->relation_mto1_list,
			table_insert_input->folder_attribute_date_name_list,
			table_insert_input->folder_attribute_append_isa_list,
			table_insert_input->
				folder->
				folder_attribute_name_list );

	table_insert_input->pair_one2m_table_insert =
		pair_one2m_table_insert_new(
			PAIR_ONE2M_ONE_FOLDER_KEY,
			table_insert_input->
				dictionary_separate->
				pair_one2m_dictionary );

	table_insert_input->
		appaserver_parameter_data_directory =
			appaserver_parameter_data_directory();

	table_insert_input->application_menu_horizontal_boolean =
		application_menu_horizontal_boolean(
			application_name );

	table_insert_input->vertical_new_table_participating_boolean =
		vertical_new_table_participating_boolean(
			VERTICAL_NEW_PROMPT_ONE_HIDDEN_LABEL,
			table_insert_input->
				dictionary_separate->
				non_prefixed_dictionary );

	table_insert_input->menu_horizontal_boolean =
		table_insert_input_menu_horizontal_boolean(
			FRAMESET_PROMPT_FRAME,
			target_frame,
			table_insert_input->
				application_menu_horizontal_boolean,
			table_insert_input->
				vertical_new_table_participating_boolean );

	if ( !table_insert_input->
		vertical_new_table_participating_boolean
	&&   table_insert_input->menu_horizontal_boolean )
	{
		table_insert_input->folder_menu =
			folder_menu_fetch(
				application_name,
				session_key,
				role_name,
				table_insert_input->
					appaserver_parameter_data_directory,
				0 /* not folder_menu_remove_boolean */ );

		table_insert_input->menu =
			menu_new(
				application_name,
				session_key,
				login_name,
				role_name,
				1 /* application_menu_horizontal_boolean */,
				table_insert_input->folder_menu->count_list );
	}

	table_insert_input->folder_row_level_restriction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_row_level_restriction_fetch(
			folder_name );

	table_insert_input->security_entity =
		security_entity_new(
			login_name,
			table_insert_input->
				folder_row_level_restriction->
				non_owner_forbid,
			table_insert_input->
				role->
				override_row_restrictions );

	return table_insert_input;
}

TABLE_INSERT_INPUT *table_insert_input_calloc( void )
{
	TABLE_INSERT_INPUT *table_insert_input;

	if ( ! ( table_insert_input =
			calloc( 1,
				sizeof ( TABLE_INSERT_INPUT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return table_insert_input;
}

boolean table_insert_entire_primary_key_ignored(
		LIST *folder_attribute_primary_key_list,
		LIST *ignore_name_list )
{
	LIST *subtract;

	subtract =
		list_subtract(
			folder_attribute_primary_key_list,
			ignore_name_list );

	if ( !list_length( subtract ) )
		return 1;
	else
		return 0;
}

LIST *table_insert_javascript_filename_list(
		char *folder_javascript_filename )
{
	LIST *filename_list = list_new();

	list_set( filename_list, "post_change_attribute.js" );
/*
	list_set( filename_list, "post_change_folder_attribute.js" );
*/

	if ( folder_javascript_filename )
	{
		list_set( filename_list, folder_javascript_filename );
	}

	return filename_list;
}

char *table_insert_onload_javascript_string(
		VERTICAL_NEW_TABLE *vertical_new_table )
{
	if ( vertical_new_table )
		return
		vertical_new_table->
			onload_javascript_string;
	else
		return (char *)0;
}

int table_insert_rows_number(
		int folder_insert_rows_number,
		VERTICAL_NEW_TABLE *vertical_new_table )
{
	if ( vertical_new_table )
		return 1;
	else
		return folder_insert_rows_number;
}

DICTIONARY *table_insert_prompt_dictionary(
		const char *vertical_new_prompt_many_hidden_label,
		DICTIONARY *dictionary_separate_prompt_dictionary,
		VERTICAL_NEW_TABLE *vertical_new_table )
{
	if ( !vertical_new_table )
	{
		return dictionary_separate_prompt_dictionary;
	}

	if ( !dictionary_separate_prompt_dictionary )
	{
		dictionary_separate_prompt_dictionary = dictionary_small();
	}

	dictionary_set(
		dictionary_separate_prompt_dictionary,
		(char *)vertical_new_prompt_many_hidden_label /* key */,
		vertical_new_table->many_folder_name );

	return dictionary_separate_prompt_dictionary;
}

boolean table_insert_input_menu_horizontal_boolean(
		const char *frameset_prompt_frame,
		char *target_frame,
		boolean application_menu_horizontal_boolean,
		boolean vertical_new_table_participating_boolean )
{
	boolean horizontal_boolean;

	if ( vertical_new_table_participating_boolean )
	{
		horizontal_boolean = application_menu_horizontal_boolean;
	}
	else
	{
		horizontal_boolean =
			menu_horizontal_boolean(
				target_frame,
				(char *)frameset_prompt_frame,
				application_menu_horizontal_boolean );
	}

	return horizontal_boolean;
}
