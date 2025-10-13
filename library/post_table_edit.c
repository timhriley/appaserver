/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/post_table_edit.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "String.h"
#include "appaserver.h"
#include "appaserver_parameter.h"
#include "date.h"
#include "appaserver_error.h"
#include "application.h"
#include "folder_attribute.h"
#include "folder_operation.h"
#include "role_folder.h"
#include "dictionary.h"
#include "update.h"
#include "sql.h"
#include "table_edit.h"
#include "post_table_edit.h"

POST_TABLE_EDIT_INPUT *post_table_edit_input_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		pid_t process_id,
		LIST *drilldown_relation_foreign_key_list )
{
	POST_TABLE_EDIT_INPUT *post_table_edit_input;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !target_frame
	||   !process_id )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	post_table_edit_input = post_table_edit_input_calloc();

	post_table_edit_input->role_folder_list =
		role_folder_list(
			role_name,
			folder_name );

	post_table_edit_input->role_folder_lookup_boolean =
		role_folder_lookup_boolean(
			APPASERVER_LOOKUP_STATE,
			APPASERVER_UPDATE_STATE,
			folder_name,
			post_table_edit_input->role_folder_list );

	post_table_edit_input->post_table_edit_forbid =
		post_table_edit_forbid(
			post_table_edit_input->role_folder_lookup_boolean );

	if ( post_table_edit_input->post_table_edit_forbid )
		return post_table_edit_input;

	post_table_edit_input->role_folder_update_boolean =
		role_folder_update_boolean(
			APPASERVER_UPDATE_STATE,
			folder_name,
			post_table_edit_input->role_folder_list );

	if ( ! ( post_table_edit_input->table_edit_state =
			/* ------------------------------ */
			/* Returns program memory or null */
			/* ------------------------------ */
			table_edit_state(
				folder_name,
				0 /* not viewonly_boolean */,
				post_table_edit_input->
					role_folder_list ) ) )
	{
		char message[ 128 ];

		sprintf(message, "table_edit_state() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	post_table_edit_input->role =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		role_fetch(
			role_name,
			1 /* fetch_attribute_exclude_list */ );

	post_table_edit_input->exclude_attribute_name_list = list_new();

	list_set_list(
		post_table_edit_input->exclude_attribute_name_list,
		role_attribute_exclude_name_list(
			APPASERVER_UPDATE_STATE,
			post_table_edit_input->
				role->
				role_attribute_exclude_list ) );

	list_set_list(
		post_table_edit_input->exclude_attribute_name_list,
		role_attribute_exclude_name_list(
			APPASERVER_LOOKUP_STATE,
			post_table_edit_input->
				role->
				role_attribute_exclude_list ) );

	post_table_edit_input->appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	post_table_edit_input->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			post_table_edit_input->exclude_attribute_name_list,
			/* -------------------------------------- */
			/* Sets folder_attribute_primary_list	  */
			/* Sets folder_attribute_primary_key_list */
			/* Sets folder_attribute_name_list	  */
			/* -------------------------------------- */
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */,
			1 /* cache_boolean */ );

	post_table_edit_input->relation_mto1_isa_list =
		relation_mto1_isa_list(
			(LIST *)0 /* mto1_isa_list Pass in null */,
			folder_name
				/* many_folder_name */,
			post_table_edit_input->
				folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */,
			0 /* not fetch_relation_one2m_list */,
			0 /* not fetch_relation_mto1_list */ );

	if ( post_table_edit_input->role_folder_update_boolean )
	{
		if ( post_table_edit_input->folder->post_change_process_name )
		{
			post_table_edit_input->post_change_process =
				process_fetch(
					post_table_edit_input->
						folder->
						post_change_process_name,
					(char *)0 /* document_root */,
					(char *)0 /* relative_source */,
					1 /* check_executable */,
					post_table_edit_input->
						appaserver_parameter->
						mount_point );
		}

		post_table_edit_input->relation_mto1_list =
			relation_mto1_list(
				folder_name
					/* many_folder_name */,
				post_table_edit_input->
					folder->
					folder_attribute_primary_key_list
					/* many_folder_primary_key_list */ );

		post_table_edit_input->relation_one2m_recursive_list =
			relation_one2m_recursive_list(
				(LIST *)0 /* one2m_list Pass in null */,
				folder_name
					/* one_folder_name */,
				post_table_edit_input->
					folder->
					folder_attribute_primary_key_list
					/* one_folder_primary_key_list */ );

		post_table_edit_input->folder_row_level_restriction =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			folder_row_level_restriction_fetch(
				folder_name );

		post_table_edit_input->appaserver_update_filespecification =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_update_filespecification(
				application_name,
				folder_name,
				session_key,
				process_id,
				drilldown_relation_foreign_key_list,
				post_table_edit_input->
					appaserver_parameter->
					data_directory );

		post_table_edit_input->file_dictionary =
			dictionary_file_fetch(
				post_table_edit_input->
					appaserver_update_filespecification,
				SQL_DELIMITER );

		if ( !post_table_edit_input->file_dictionary ) return NULL;
	}

	post_table_edit_input->folder_operation_list =
		folder_operation_list(
			folder_name,
			role_name,
			1 /* fetch_operation */,
			1 /* fetch_process */ );

	post_table_edit_input->folder_operation_name_list =
		folder_operation_name_list(
			post_table_edit_input->
				folder_operation_list );


	post_table_edit_input->post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_stdin_new(
			/* ----------------------------- */
			/* Not expecting a spooled file. */
			/* ----------------------------- */
			(char *)0 /* appliction_name */,
			(char *)0 /* upload_directory */,
			(LIST *)0 /* upload_filename_list */ );

	post_table_edit_input->folder_attribute_append_isa_list =
		folder_attribute_append_isa_list(
			post_table_edit_input->
				folder->
				folder_attribute_list
					/* append_isa_list */,
			post_table_edit_input->relation_mto1_isa_list );

	post_table_edit_input->folder_attribute_name_list =
		folder_attribute_name_list(
			(char *)0 /* folder_name */,
			post_table_edit_input->
				folder_attribute_append_isa_list );

	post_table_edit_input->folder_attribute_date_name_list =
		folder_attribute_date_name_list(
			post_table_edit_input->
				folder_attribute_append_isa_list );

	post_table_edit_input->dictionary_separate =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_post_table_edit_new(
			post_table_edit_input->
				post_dictionary->
				original_post_dictionary,
			application_name,
			login_name,
			post_table_edit_input->
				relation_mto1_list,
			post_table_edit_input->
				folder_attribute_name_list,
			post_table_edit_input->
				folder_operation_name_list,
			post_table_edit_input->
				folder_attribute_date_name_list,
			post_table_edit_input->
				folder_attribute_append_isa_list );

	post_table_edit_input_dictionary_time_set(
		post_table_edit_input->
			dictionary_separate->
			multi_row_dictionary /* in/out */,
		post_table_edit_input->
			folder_attribute_append_isa_list );

	post_table_edit_input->appaserver_error_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_error_filename(
			application_name );

	return post_table_edit_input;
}

POST_TABLE_EDIT_INPUT *post_table_edit_input_calloc( void )
{
	POST_TABLE_EDIT_INPUT *post_table_edit_input;

	if ( ! ( post_table_edit_input =
			calloc( 1,
				sizeof ( POST_TABLE_EDIT_INPUT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return post_table_edit_input;
}

POST_TABLE_EDIT *post_table_edit_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		pid_t process_id,
		LIST *drilldown_relation_foreign_key_list )
{
	POST_TABLE_EDIT *post_table_edit;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !target_frame
	||   !process_id )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	post_table_edit = post_table_edit_calloc();

	post_table_edit->session_folder =
		/* --------------------------------------------- */
		/* Sets appaserver environment and outputs argv. */
		/* Each parameter is security inspected.	 */
		/* --------------------------------------------- */
		session_folder_integrity_exit(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			APPASERVER_LOOKUP_STATE,
			APPASERVER_UPDATE_STATE );

	post_table_edit->post_table_edit_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_table_edit_input_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			target_frame,
			process_id,
			drilldown_relation_foreign_key_list );

	if ( !post_table_edit->post_table_edit_input )
		return post_table_edit;

	if ( post_table_edit->post_table_edit_input->post_table_edit_forbid )
	{
		char message[ 128 ];

		sprintf(message, "post_table_edit_forbid is set." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( post_table_edit->
		post_table_edit_input->
		role_folder_update_boolean
	&&   dictionary_length(
		post_table_edit->
			post_table_edit_input->
			dictionary_separate->
			multi_row_dictionary ) )
	{
		post_table_edit->update =
			update_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				post_table_edit->
					post_table_edit_input->
					role->
					override_row_restrictions,
				post_table_edit->
					post_table_edit_input->
					dictionary_separate->
					multi_row_dictionary,
				post_table_edit->
					post_table_edit_input->
					file_dictionary,
				post_table_edit->
					post_table_edit_input->
					folder_row_level_restriction,
				post_table_edit->
					post_table_edit_input->
					relation_one2m_recursive_list
						/* relation_one2m_list */,
				post_table_edit->
					post_table_edit_input->
					relation_mto1_list,
				post_table_edit->
					post_table_edit_input->
					relation_mto1_isa_list,
				post_table_edit->
					post_table_edit_input->
					folder_attribute_append_isa_list,
				post_table_edit->
					post_table_edit_input->
					post_change_process );
	}

	if ( dictionary_length(
		post_table_edit->
			post_table_edit_input->
			dictionary_separate->
			operation_dictionary ) )
	{
		post_table_edit->operation_row_list =
			operation_row_list_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				target_frame,
				post_table_edit->
					post_table_edit_input->
					table_edit_state,
				post_table_edit->
					post_table_edit_input->
					folder_operation_list,
				post_table_edit->
					post_table_edit_input->
					folder->
					folder_attribute_primary_key_list,
				post_table_edit->
					post_table_edit_input->
					folder->
					folder_attribute_name_list,
				post_table_edit->
					post_table_edit_input->
					dictionary_separate->
					operation_dictionary,
				post_table_edit->
					post_table_edit_input->
					dictionary_separate->
					no_display_dictionary,
				post_table_edit->
					post_table_edit_input->
					dictionary_separate->
					multi_row_dictionary,
				post_table_edit->
					post_table_edit_input->
					dictionary_separate->
					drillthru_dictionary,
				(post_table_edit->update)
					? post_table_edit->
						update->
						results_string
					: (char *)0,
				(post_table_edit->update)
					? post_table_edit->
						update->
						error_string
					: (char *)0 );
	}

	post_table_edit->dictionary_separate_send_dictionary =
		dictionary_separate_send_dictionary(
			post_table_edit->
				post_table_edit_input->
				dictionary_separate->
				sort_dictionary,
			DICTIONARY_SEPARATE_SORT_PREFIX,
			post_table_edit->
				post_table_edit_input->
				dictionary_separate->
				query_dictionary,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			post_table_edit->
				post_table_edit_input->
				dictionary_separate->
				drillthru_dictionary,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
				(DICTIONARY *)0 /* ignore_dictionary */,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			post_table_edit->
				post_table_edit_input->
				dictionary_separate->
				no_display_dictionary,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			post_table_edit->
				post_table_edit_input->
				dictionary_separate->
				pair_dictionary,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			post_table_edit->
				post_table_edit_input->
				dictionary_separate->
				prompt_dictionary
					/* non_prefixed_dictionary */ );

	dictionary_set(
		post_table_edit->dictionary_separate_send_dictionary,
		POST_TABLE_EDIT_FOLDER_LABEL,
		folder_name );

	post_table_edit->dictionary_separate_send_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		dictionary_separate_send_string(
			post_table_edit->
				dictionary_separate_send_dictionary );

	return post_table_edit;
}

POST_TABLE_EDIT *post_table_edit_calloc( void )
{
	POST_TABLE_EDIT *post_table_edit;

	if ( ! ( post_table_edit = calloc( 1, sizeof ( POST_TABLE_EDIT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_table_edit;
}

boolean post_table_edit_forbid(
		boolean role_folder_lookup_boolean )
{
	return 1 - role_folder_lookup_boolean;
}

void post_table_edit_input_dictionary_time_set(
		DICTIONARY *multi_row_dictionary,
		LIST *folder_attribute_append_isa_list )
{
	LIST *key_list;
	char *key;
	char trim_index[ 128 ];
	char *get;
	FOLDER_ATTRIBUTE *folder_attribute;

	key_list = dictionary_key_list( multi_row_dictionary );

	if ( list_rewind( key_list ) )
	do {
		key = list_get( key_list );

		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		get = dictionary_get( key, multi_row_dictionary );

		if ( !get || *get == '/' ) continue;

		/* Returns destination */
		/* ------------------- */
		(void)string_trim_index( trim_index, key );

		if ( ( folder_attribute =
			folder_attribute_seek(
				trim_index /* attribute_name */,
				folder_attribute_append_isa_list ) ) )
		{
			if ( !folder_attribute->attribute )
			{
				char message[ 128 ];

				snprintf(
					message,
					sizeof ( message ),
				"folder_attribute->attribute is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( attribute_is_date_time(
				folder_attribute->
					attribute->
					datatype_name ) )
			{
				char *set_now_hhmmss;

				set_now_hhmmss =
					/* --------------------------- */
					/* Returns heap memory or null */
					/* --------------------------- */
					date_set_now_hhmmss(
						get /* date_string */,
						date_utc_offset() );

				if ( set_now_hhmmss )
				{
					dictionary_set(
						multi_row_dictionary,
						key,
						set_now_hhmmss );
				}
			}
		}

	} while ( list_next( key_list ) );

	list_free_container( key_list );
}
