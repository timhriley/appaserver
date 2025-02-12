/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/drilldown.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "String.h"
#include "sql.h"
#include "appaserver.h"
#include "appaserver_user.h"
#include "attribute.h"
#include "query.h"
#include "menu.h"
#include "frameset.h"
#include "folder_operation.h"
#include "application.h"
#include "appaserver_error.h"
#include "update.h"
#include "process.h"
#include "post_table_edit.h"
#include "drilldown.h"

DRILLDOWN_MANY_TO_ONE *drilldown_many_to_one_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *drilldown_base_folder_name,
		char *target_frame,
		char *drilldown_primary_data_list_string,
		char *data_directory,
		pid_t process_id,
		char *one_folder_name,
		LIST *relation_translate_list,
		LIST *one_folder_primary_key_list,
		DICTIONARY *query_fetch_dictionary )
{
	DRILLDOWN_MANY_TO_ONE *drilldown_many_to_one;
	DICTIONARY *original_post_dictionary;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !drilldown_base_folder_name
	||   !target_frame
	||   !drilldown_primary_data_list_string
	||   !data_directory
	||   !process_id
	||   !one_folder_name
	||   !list_length( one_folder_primary_key_list )
	||   !dictionary_length( query_fetch_dictionary ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	drilldown_many_to_one = drilldown_many_to_one_calloc();
	drilldown_many_to_one->one_folder_name = one_folder_name;

	drilldown_many_to_one->attribute_data_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		drilldown_many_to_one_attribute_data_list(
			relation_translate_list,
			one_folder_primary_key_list,
			query_fetch_dictionary );

	/* Drop down column isn't set in table. */
	/* ------------------------------------ */
	if ( !list_length( drilldown_many_to_one->attribute_data_list ) )
	{
		free( drilldown_many_to_one );
		return NULL;
	}

	drilldown_many_to_one->drilldown_query_dictionary =
		/* ---------------------------------------------- */
		/* Returns query_dictionary or dictionary_small() */
		/* ---------------------------------------------- */
		drilldown_query_dictionary(
			QUERY_RELATION_OPERATOR_PREFIX,
			QUERY_EQUAL,
			(DICTIONARY *)0 /* query_dictionary */,
			one_folder_primary_key_list,
			drilldown_many_to_one->attribute_data_list );

	original_post_dictionary = dictionary_small();

	dictionary_prefixed_dictionary_set(
		original_post_dictionary,
		DICTIONARY_SEPARATE_QUERY_PREFIX,
		drilldown_many_to_one->drilldown_query_dictionary );

	drilldown_many_to_one->table_edit =
		table_edit_new(
			application_name,
			session_key,
			login_name,
			role_name,
			one_folder_name,
			target_frame,
			process_id,
			(char *)0 /* results_string */,
			(char *)0 /* error_string */,
			0 /* not application_menu_horizontal_boolean */,
			original_post_dictionary,
			data_directory,
			drilldown_base_folder_name,
			drilldown_primary_data_list_string,
			0 /* not viewonly_boolean */,
			1 /* omit_delete_boolean */ );

	/* If lookup permission isn't set. */
	/* ------------------------------- */
	if ( !drilldown_many_to_one->table_edit )
	{
		free( drilldown_many_to_one );
		return NULL;
	}

	return drilldown_many_to_one;
}

DRILLDOWN_MANY_TO_ONE *drilldown_many_to_one_calloc( void )
{
	DRILLDOWN_MANY_TO_ONE *drilldown_many_to_one;

	if ( ! ( drilldown_many_to_one =
			calloc( 1,
				sizeof ( DRILLDOWN_MANY_TO_ONE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return drilldown_many_to_one;
}

DRILLDOWN_ONE_TO_MANY *drilldown_one_to_many_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *drilldown_base_folder_name,
		char *target_frame,
		char *drilldown_primary_data_list_string,
		char *data_directory,
		DICTIONARY *sort_dictionary,
		char *post_table_edit_folder_name,
		LIST *primary_attribute_data_list,
		pid_t process_id,
		char *relation_many_folder_name,
		LIST *relation_foreign_key_list )
{
	DRILLDOWN_ONE_TO_MANY *drilldown_one_to_many;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !drilldown_base_folder_name
	||   !target_frame
	||   !drilldown_primary_data_list_string
	||   !data_directory
	||   !list_length( primary_attribute_data_list )
	||   !process_id
	||   !relation_many_folder_name
	||   !list_length( relation_foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	drilldown_one_to_many = drilldown_one_to_many_calloc();

	drilldown_one_to_many->relation_many_folder_name =
		relation_many_folder_name;

	drilldown_one_to_many->drilldown_query_dictionary =
		/* ---------------------------------------------- */
		/* Returns query_dictionary or dictionary_small() */
		/* ---------------------------------------------- */
		drilldown_query_dictionary(
			QUERY_RELATION_OPERATOR_PREFIX,
			QUERY_EQUAL,
			(DICTIONARY *)0 /* query_dictionary */,
			relation_foreign_key_list,
			primary_attribute_data_list );

	drilldown_one_to_many->original_post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		drilldown_one_to_many_original_post_dictionary(
			sort_dictionary,
			post_table_edit_folder_name,
			relation_many_folder_name,
			drilldown_one_to_many->drilldown_query_dictionary );

	drilldown_one_to_many->table_edit =
		table_edit_new(
			application_name,
			session_key,
			login_name,
			role_name,
			relation_many_folder_name,
			target_frame,
			process_id,
			(char *)0 /* results_string */,
			(char *)0 /* error_string */,
			0 /* not application_menu_horizontal_boolean */,
			drilldown_one_to_many->original_post_dictionary,
			data_directory,
			drilldown_base_folder_name,
			drilldown_primary_data_list_string,
			0 /* not viewonly_boolean */,
			0 /* not omit_delete_boolean */ );

	if ( !drilldown_one_to_many->table_edit
	||   !drilldown_one_to_many->table_edit->query_row_list_length )
	{
		free( drilldown_one_to_many );
		drilldown_one_to_many = NULL;
	}

	return drilldown_one_to_many;
}

DRILLDOWN_ONE_TO_MANY *drilldown_one_to_many_calloc( void )
{
	DRILLDOWN_ONE_TO_MANY *drilldown_one_to_many;

	if ( ! ( drilldown_one_to_many =
			calloc( 1,
				sizeof ( DRILLDOWN_ONE_TO_MANY ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return drilldown_one_to_many;
}

DRILLDOWN_PRIMARY *drilldown_primary_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *drilldown_base_folder_name,
		char *target_frame,
		char *drilldown_primary_data_list_string,
		char *data_directory,
		LIST *folder_attribute_primary_key_list,
		DICTIONARY *no_display_dictionary,
		DICTIONARY *query_dictionary,
		LIST *primary_attribute_data_list,
		pid_t process_id )
{
	DRILLDOWN_PRIMARY *drilldown_primary;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !drilldown_base_folder_name
	||   !target_frame
	||   !drilldown_primary_data_list_string
	||   !data_directory
	||   !list_length( folder_attribute_primary_key_list )
	||   !list_length( primary_attribute_data_list )
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

	drilldown_primary = drilldown_primary_calloc();

	drilldown_primary->drilldown_query_dictionary =
		/* ---------------------------------------------- */
		/* Returns query_dictionary or dictionary_small() */
		/* ---------------------------------------------- */
		drilldown_query_dictionary(
			QUERY_RELATION_OPERATOR_PREFIX,
			QUERY_EQUAL,
			query_dictionary,
			folder_attribute_primary_key_list,
			primary_attribute_data_list );

	drilldown_primary->original_post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		drilldown_primary_original_post_dictionary(
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			no_display_dictionary,
			drilldown_primary->drilldown_query_dictionary );

	drilldown_primary->table_edit =
		table_edit_new(
			application_name,
			session_key,
			login_name,
			role_name,
			drilldown_base_folder_name,
			target_frame,
			process_id,
			(char *)0 /* results_string */,
			(char *)0 /* error_string */,
			0 /* not application_menu_horizontal_boolean */,
			drilldown_primary->original_post_dictionary,
			data_directory,
			drilldown_base_folder_name,
			drilldown_primary_data_list_string,
			0 /* not viewonly_boolean */,
			0 /* not omit_delete_boolean */ );

	if ( !drilldown_primary->table_edit )
	{
		char message[ 128 ];

		sprintf(message,
			"table_edit_new(%s) returned empty.",
			drilldown_base_folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return drilldown_primary;
}

DRILLDOWN_PRIMARY *drilldown_primary_calloc( void )
{
	DRILLDOWN_PRIMARY *drilldown_primary;

	if ( ! ( drilldown_primary =
			calloc( 1,
				sizeof ( DRILLDOWN_PRIMARY ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return drilldown_primary;
}

DRILLDOWN *drilldown_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *drilldown_base_folder_name,
		char *target_frame,
		char *drilldown_primary_data_list_string,
		char *update_results_string,
		char *update_error_string,
		DICTIONARY *original_post_dictionary,
		char *data_directory )
{
	DRILLDOWN *drilldown;

	if ( !argc
	||   !argv
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !drilldown_base_folder_name
	||   !target_frame
	||   !drilldown_primary_data_list_string
	||   !dictionary_length( original_post_dictionary )
	||   !data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	drilldown = drilldown_calloc();

	drilldown->session_folder =
		/* -------------------------------------------- */
		/* Sets appaserver environment and outputs argv */
		/* Each parameter is security inspected		*/
		/* -------------------------------------------- */
		session_folder_integrity_exit(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			drilldown_base_folder_name,
			APPASERVER_LOOKUP_STATE );

	drilldown->drilldown_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		drilldown_input_new(
			application_name,
			session_key,
			login_name,
			role_name,
			drilldown_base_folder_name,
			target_frame,
			drilldown_primary_data_list_string,
			data_directory,
			original_post_dictionary );

	drilldown->drilldown_primary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		drilldown_primary_new(
			application_name,
			session_key,
			login_name,
			role_name,
			drilldown_base_folder_name,
			target_frame,
			drilldown_primary_data_list_string,
			data_directory,
			drilldown->
				drilldown_input->
				folder->
				folder_attribute_primary_key_list,
			drilldown->
				drilldown_input->
				dictionary_separate->
				no_display_dictionary,
			drilldown->
				drilldown_input->
				dictionary_separate->
				query_dictionary,
			drilldown->
				drilldown_input->
				primary_attribute_data_list,
			drilldown->
				drilldown_input->
				process_id );

	if ( list_rewind( drilldown->drilldown_input->relation_one2m_list ) )
	{
		RELATION_ONE2M *relation_one2m;
		DRILLDOWN_ONE_TO_MANY *drilldown_one_to_many;

		drilldown->drilldown_one_to_many_list = list_new();

		do {
			relation_one2m =
				list_get(
					drilldown->
						drilldown_input->
						relation_one2m_list );

			if ( !relation_one2m->relation )
			{
				char message[ 128 ];

				sprintf(message,
					"relation_one2m->relation is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( relation_one2m->relation->omit_drilldown )
				continue;

			drilldown_one_to_many =
				drilldown_one_to_many_new(
					application_name,
					session_key,
					login_name,
					role_name,
					drilldown_base_folder_name,
					target_frame,
					drilldown_primary_data_list_string,
					data_directory,
					drilldown->
						drilldown_input->
						dictionary_separate->
						sort_dictionary,
					drilldown->
						drilldown_input->
						post_table_edit_folder_name,
					drilldown->
						drilldown_input->
						primary_attribute_data_list,
					drilldown->
						drilldown_input->
						process_id,
					relation_one2m->many_folder_name,
					relation_one2m->
						relation_foreign_key_list );

			if ( drilldown_one_to_many )
			{
				list_set(
					drilldown->drilldown_one_to_many_list,
					drilldown_one_to_many );
			}

		} while ( list_next(
				drilldown->
					drilldown_input->
					relation_one2m_list ) );
	}

	drilldown->relation_mto1_list =
		drilldown_relation_mto1_list(
			drilldown->drilldown_input->relation_mto1_list,
			drilldown->drilldown_input->relation_mto1_isa_list );

	if ( list_rewind( drilldown->relation_mto1_list ) )
	{
		DICTIONARY *fetch_dictionary;
		RELATION_MTO1 *relation_mto1;
		DRILLDOWN_MANY_TO_ONE *drilldown_many_to_one;

		/* Fetch the foreign key; it may not be primary */
		/* -------------------------------------------- */
		fetch_dictionary =
			query_fetch_dictionary(
				drilldown->
					drilldown_input->
					folder->
					folder_attribute_name_list
						/* select_name_list */,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				folder_table_name(
					application_name,
					drilldown_base_folder_name ),
				drilldown->
					drilldown_input->
					folder->
					folder_attribute_primary_key_list,
				drilldown->
					drilldown_input->
					primary_attribute_data_list );

		/* A primary attribute was changed */
		/* ------------------------------- */
		if ( !fetch_dictionary ) goto drilldown_document;

		drilldown->drilldown_many_to_one_list = list_new();

		do {
			relation_mto1 =
				list_get(
					drilldown->
						relation_mto1_list );

			if ( !list_length(
				relation_mto1->
					relation_foreign_key_list )
			||   !relation_mto1->one_folder
			||   !list_length(
				relation_mto1->
					one_folder->
					folder_attribute_primary_key_list ) )
			{
				char message[ 128 ];

				sprintf(message,
			"relation_mto1->one_folder is empty or incomplete." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			drilldown_many_to_one =
				drilldown_many_to_one_new(
					application_name,
					session_key,
					login_name,
					role_name,
					drilldown_base_folder_name,
					target_frame,
					drilldown_primary_data_list_string,
					data_directory,
					drilldown->
						drilldown_input->
						process_id,
					relation_mto1->one_folder_name,
					relation_mto1->relation_translate_list,
					relation_mto1->
					    one_folder->
					    folder_attribute_primary_key_list,
					fetch_dictionary );

			if ( drilldown_many_to_one )
			{
				list_set(
					drilldown->drilldown_many_to_one_list,
					drilldown_many_to_one );
			}

		} while ( list_next(
				drilldown->
					relation_mto1_list ) );
	}

drilldown_document:

	drilldown->drilldown_document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		drilldown_document_new(
			application_name,
			session_key,
			login_name,
			role_name,
			drilldown_base_folder_name,
			update_results_string,
			update_error_string,
			drilldown->
				drilldown_input->
				menu,
			drilldown->
				drilldown_input->
				folder->
				notepad,
			drilldown->
				drilldown_input->
				folder->
				javascript_filename,
			drilldown->
				drilldown_input->
				folder->
				post_change_javascript,
			drilldown->
				drilldown_input->
				relation_mto1_list,
			drilldown->
				drilldown_input->
				relation_one2m_list,
			drilldown->drilldown_primary,
			drilldown->drilldown_one_to_many_list );

	return drilldown;
}

DRILLDOWN *drilldown_calloc( void )
{
	DRILLDOWN *drilldown;

	if ( ! ( drilldown = calloc( 1, sizeof ( DRILLDOWN ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return drilldown;
}

void drilldown_table_edit_output(
		const char *appaserver_user_primary_key,
		char *login_name,
		TABLE_EDIT *table_edit )
{
	if ( !login_name
	||   !table_edit
	||   !table_edit->
		query_table_edit
	||   !table_edit->
		form_table_edit
	||   !table_edit->
		table_edit_input
	||   !table_edit->
		table_edit_input->
		folder_row_level_restriction )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	table_edit_spool_file_output(
		table_edit->appaserver_spool_filename,
		table_edit->
			query_table_edit->
			query_fetch->
			row_list /* query_fetch_row_list */ );

	table_edit_output(
		appaserver_user_primary_key,
		login_name,
		table_edit->
			table_edit_input->
			folder_row_level_restriction->
			non_owner_viewonly,
		(char *)0 /* document_html */,
		table_edit->
			form_table_edit->
			open_html,
		table_edit->
			form_table_edit->
			heading_container_string,
		table_edit->
			table_edit_input->
			folder_operation_list,
		table_edit->
			query_table_edit->
			query_fetch->
			row_list /* query_fetch_row_list */,
		table_edit->
			relation_join,
		table_edit->
			row_security->
			regular_widget_list,
		table_edit->
			row_security->
			viewonly_widget_list,
		table_edit->
			table_edit_input->
			row_security_role_update_list,
		table_edit->
			table_edit_input->
			table_edit_state,
		table_edit->
			form_table_edit->
			close_html,
		(char *)0 /* document_trailer_html */ );
}

LIST *drilldown_input_primary_attribute_data_list(
		char *drilldown_primary_data_list_string )
{
	char unescaped[ 1024 ];

	if ( !drilldown_primary_data_list_string )
	{
		char message[ 128 ];

		sprintf(message,
			"drilldown_primary_data_list_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	return
	attribute_data_list(
		SQL_DELIMITER,
		string_unescape_character(
			unescaped,
			drilldown_primary_data_list_string,
			SQL_DELIMITER ) );
}

LIST *drilldown_document_javascript_filename_list(
		char *javascript_filename,
		LIST *relation_one2m_list,
		LIST *relation_mto1_list )
{
	LIST *list = list_new();

	list_set( list, "table_edit.js" );

	if ( javascript_filename )
	{
		list_set(
			list,
			javascript_filename );
	}

	if ( list_rewind( relation_one2m_list ) )
	{
		RELATION_ONE2M *relation_one2m;

		do {
			relation_one2m = list_get( relation_one2m_list );

			if ( !relation_one2m->many_folder )
			{
				char message[ 128 ];

				sprintf(message,
				"relation_one2m->many_folder is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( relation_one2m->
				many_folder->
				javascript_filename )
			{
				list_set(
					list,
					relation_one2m->
						many_folder->
						javascript_filename );
			}

		} while ( list_next( relation_one2m_list ) );
	}

	if ( list_rewind( relation_mto1_list ) )
	{
		RELATION_MTO1 *relation_mto1;

		do {
			relation_mto1 = list_get( relation_mto1_list );

			if ( !relation_mto1->one_folder )
			{
				char message[ 128 ];

				sprintf(message,
					"relation_mto1->one_folder is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( relation_mto1->
				one_folder->
				javascript_filename )
			{
				list_set(
					list,
					relation_mto1->
						one_folder->
						javascript_filename );
			}

		} while ( list_next( relation_mto1_list ) );
	}

	return list;
}

char *drilldown_document_onload_string(
		char *post_change_javascript,
		LIST *relation_one2m_list,
		LIST *relation_mto1_list )
{
	char onload_string[ STRING_64K ];
	char *ptr = onload_string;

	if ( post_change_javascript )
	{
		ptr += sprintf( ptr,
			"%s",
			post_change_javascript );
	}

	if ( list_rewind( relation_one2m_list ) )
	{
		RELATION_ONE2M *relation_one2m;

		do {
			relation_one2m = list_get( relation_one2m_list );

			if ( !relation_one2m->many_folder )
			{
				char message[ 128 ];

				sprintf(message,
				"relation_one2m->many_folder is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( relation_one2m->
				many_folder->
				post_change_javascript )
			{
				if ( ptr != onload_string )
					ptr += sprintf( ptr, " && " );

				ptr += sprintf( ptr,
					"%s",
					relation_one2m->
						many_folder->
						post_change_javascript );
			}

		} while ( list_next( relation_one2m_list ) );
	}

	if ( list_rewind( relation_mto1_list ) )
	{
		RELATION_MTO1 *relation_mto1;

		do {
			relation_mto1 = list_get( relation_mto1_list );

			if ( !relation_mto1->one_folder )
			{
				char message[ 128 ];

				sprintf(message,
					"relation_mto1->one_folder is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( relation_mto1->
				one_folder->
				post_change_javascript )
			{
				if ( ptr != onload_string )
					ptr += sprintf( ptr, " && " );

				ptr += sprintf( ptr,
					"%s",
					relation_mto1->
						one_folder->
						post_change_javascript );
			}

		} while ( list_next( relation_mto1_list ) );
	}

	if ( ptr == onload_string )
		return (char *)0;
	else
		return strdup( onload_string );
}

char *drilldown_document_title_string( char *drilldown_base_folder_name )
{
	static char title_string[ 128 ];
	char buffer[ 64 ];

	if ( !drilldown_base_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "drilldown_base_folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(title_string,
		"Drilldown %s",
		string_initial_capital(
			buffer,
			drilldown_base_folder_name ) );

	return title_string;
}

DRILLDOWN_DOCUMENT *drilldown_document_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *drilldown_base_folder_name,
		char *update_results_string,
		char *update_error_string,
		MENU *menu,
		char *folder_menu,
		char *folder_javascript_filename,
		char *folder_post_change_javascript,
		LIST *relation_mto1_list,
		LIST *relation_one2m_list,
		DRILLDOWN_PRIMARY *drilldown_primary,
		LIST *drilldown_one_to_many_list )
{
	DRILLDOWN_DOCUMENT *drilldown_document;

	if ( !application_name
	||   !drilldown_base_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	drilldown_document = drilldown_document_calloc();

	drilldown_document->title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		drilldown_document_title_string(
			drilldown_base_folder_name );

	drilldown_document->document_head_menu_setup_string =
		/* ------------------------------ */
		/* Returns program memory or null */
		/* ------------------------------ */
		document_head_menu_setup_string(
			(menu) ? 1 : 0 /* menu_boolean */ ),

	drilldown_document->document_head_calendar_setup_string =
		/* ------------------------------ */
		/* Returns program memory or null */
		/* ------------------------------ */
		document_head_calendar_setup_string(
			1 /* folder_attribute_date_name_list_length */ );

	drilldown_document->javascript_filename_list =
		drilldown_document_javascript_filename_list(
			folder_javascript_filename,
			relation_one2m_list,
			relation_mto1_list );

	drilldown_document->javascript_include_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		javascript_include_string(
			application_name,
			drilldown_document->javascript_filename_list );

	drilldown_document->onload_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		drilldown_document_onload_string(
			folder_post_change_javascript,
			relation_one2m_list,
			relation_mto1_list );

	update_results_string =
		/* ------------------------------------- */
		/* Returns update_results_string or null */
		/* ------------------------------------- */
		drilldown_document_update_results_string(
			update_results_string );

	if ( update_error_string )
	{
		update_results_string = (char *)0;
	}

	drilldown_document->document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_new(
			application_name,
			application_title_string( application_name ),
			drilldown_document->title_string,
			update_results_string /* sub_title_string */,
			update_error_string /* sub_sub_title_string */,
			folder_menu,
			drilldown_document->onload_string
				/* onload_javascript_string */,
			(menu) ? menu->html : (char *)0,
			drilldown_document->document_head_menu_setup_string,
			drilldown_document->document_head_calendar_setup_string,
			drilldown_document->javascript_include_string );

	drilldown_document->ajax_client_list =
		drilldown_document_ajax_client_list(
			drilldown_primary,
			drilldown_one_to_many_list );

	if ( list_length(
		drilldown_document->
			ajax_client_list ) )
	{
		drilldown_document->ajax_client_list_javascript =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			ajax_client_list_javascript(
				session_key,
				login_name,
				role_name,
				drilldown_document->ajax_client_list );
	}
	else
	{
		drilldown_document->ajax_client_list_javascript = "";
	}

	return drilldown_document;
}

DRILLDOWN_DOCUMENT *drilldown_document_calloc( void )
{
	DRILLDOWN_DOCUMENT *drilldown_document;

	if ( ! ( drilldown_document =
			calloc( 1,
				sizeof ( DRILLDOWN_DOCUMENT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return drilldown_document;
}

DRILLDOWN_INPUT *drilldown_input_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *drilldown_base_folder_name,
		char *target_frame,
		char *drilldown_primary_data_list_string,
		char *data_directory,
		DICTIONARY *original_post_dictionary )
{
	DRILLDOWN_INPUT *drilldown_input;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !drilldown_base_folder_name
	||   !target_frame
	||   !drilldown_primary_data_list_string
	||   !data_directory
	||   !dictionary_length( original_post_dictionary ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	drilldown_input = drilldown_input_calloc();

	drilldown_input->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			drilldown_base_folder_name,
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			/* -------------------------------------------- */
			/* Sets folder_attribute_primary_list		*/
			/* Sets folder_attribute_primary_key_list	*/
			/* Sets primary_key_list			*/
			/* -------------------------------------------- */
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */ );

	drilldown_input->relation_mto1_list =
		relation_mto1_list(
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			drilldown_base_folder_name
				/* many_folder_name */,
			drilldown_input->
				folder->
				folder_attribute_primary_key_list
				/* many_primary_key_list */ );

	drilldown_input->relation_mto1_isa_list =
		relation_mto1_isa_list(
			(LIST *)0 /* mto1_isa_list */,
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			drilldown_base_folder_name
				/* many_folder_name */,
			drilldown_input->
				folder->
				folder_attribute_primary_key_list
				/* many_primary_key_list */,
			0 /* not fetch_relation_one2m_list */,
			0 /* not fetch_relation_mto1_list */ );

	drilldown_input->relation_one2m_list =
		relation_one2m_list(
			/* ---------------------------------------- */
			/* Set to cache folders for the entire role */
			/* ---------------------------------------- */
			role_name,
			drilldown_base_folder_name
				/* one_folder_name */,
			drilldown_input->
				folder->
				folder_attribute_primary_key_list
				/* one_primary_key_list */,
			1 /* include_isa_boolean */ );

	drilldown_input->folder_attribute_append_isa_list =
		folder_attribute_append_isa_list(
			drilldown_input->folder->folder_attribute_list
				/* append_isa_list */,
			drilldown_input->relation_mto1_isa_list );

	drilldown_input->application_menu_horizontal_boolean =
		application_menu_horizontal_boolean(
			application_name );

	drilldown_input->menu_horizontal_boolean =
		menu_horizontal_boolean(
			target_frame /* current_frame */,
			FRAMESET_PROMPT_FRAME,
			drilldown_input->application_menu_horizontal_boolean );

	if ( drilldown_input->menu_horizontal_boolean )
	{
		drilldown_input->folder_menu =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			folder_menu_fetch(
				application_name,
				session_key,
				role_name,
				data_directory,
				0 /* not folder_menu_remove_boolean */ );

		drilldown_input->menu =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			menu_new(
				application_name,
				session_key,
				login_name,
				role_name,
				1 /* application_menu_horizontal_boolean */,
				drilldown_input->folder_menu->count_list );
	}

	drilldown_input->dictionary_separate =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_table_edit_new(
			original_post_dictionary,
			application_name,
			login_name,
			(LIST *)0 /* folder_attribute_date_name_list */,
			drilldown_input->folder_attribute_append_isa_list );

	drilldown_input->post_table_edit_folder_name =
		/* ---------------------------------------------------- */
		/* Returns component of non_prefixed_dictionary or null */
		/* ---------------------------------------------------- */
		drilldown_input_post_table_edit_folder_name(
			POST_TABLE_EDIT_FOLDER_LABEL,
			drilldown_input->
				dictionary_separate->
				non_prefixed_dictionary );

	drilldown_input->primary_attribute_data_list =
		drilldown_input_primary_attribute_data_list(
			drilldown_primary_data_list_string );

	drilldown_input->process_id = getpid();

	return drilldown_input;
}

DRILLDOWN_INPUT *drilldown_input_calloc( void )
{
	DRILLDOWN_INPUT *drilldown_input;

	if ( ! ( drilldown_input = calloc( 1, sizeof ( DRILLDOWN_INPUT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return drilldown_input;
}

char *drilldown_document_update_results_string( char *update_results_string )
{
	if ( !update_results_string
	||   !*update_results_string
	||   strcmp(
		update_results_string,
		PROCESS_UPDATE_RESULTS_PLACEHOLDER ) == 0 )
	{
		return (char *)0;
	}
	else
	{
		return update_results_string;
	}
}

LIST *drilldown_many_to_one_attribute_data_list(
		LIST *relation_translate_list,
		LIST *one_folder_primary_key_list,
		DICTIONARY *query_fetch_dictionary )
{
	LIST *attribute_data_list;
	char *primary_key;
	char *foreign_key;
	char *get;

	if ( !list_rewind( one_folder_primary_key_list )
	||   !dictionary_length( query_fetch_dictionary ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	attribute_data_list = list_new();

	do {
		primary_key = list_get( one_folder_primary_key_list );

		foreign_key =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			relation_translate_foreign_key(
				primary_key,
				relation_translate_list );

		if ( ! ( get =
				/* --------------------------------------- */
				/* Returns component of dictionary or null */
				/* --------------------------------------- */
				dictionary_get(
					foreign_key,
					query_fetch_dictionary ) ) )
		{
			/* Drop down column isn't set in table. */
			/* ------------------------------------ */
			return (LIST *)0;
		}

		list_set( attribute_data_list, get );

	} while ( list_next( one_folder_primary_key_list ) );

	return attribute_data_list;
}

LIST *drilldown_one_to_many_attribute_data_list(
		LIST *one_folder_primary_key_list,
		DICTIONARY *dictionary_separate_query_dictionary )
{
	LIST *attribute_data_list;
	char *primary_key;
	char *get;

	if ( !dictionary_length( dictionary_separate_query_dictionary )
	||   !list_rewind( one_folder_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	attribute_data_list = list_new();

	do {
		primary_key = list_get( one_folder_primary_key_list );

		get =
			dictionary_get(
				primary_key,
				dictionary_separate_query_dictionary );

		if ( !get )
		{
			char message[ 128 ];

			sprintf(message,
				"dictionary_get(%s) returned empty.\n",
				primary_key );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set( attribute_data_list, get );

	} while ( list_next( one_folder_primary_key_list ) );

	return attribute_data_list;
}

DICTIONARY *drilldown_query_dictionary(
		const char *query_relation_operator_prefix,
		const char *query_equal,
		DICTIONARY *dictionary_separate_query_dictionary,
		LIST *key_list,
		LIST *attribute_data_list )
{
	if ( !list_length( key_list )
	||   !list_length( attribute_data_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_length( key_list ) != list_length( attribute_data_list ) )
	{
		char message[ 1024 ];

		sprintf(message,
		"cannot map: key_list=[%s] with attribute_data_list=[%s]",
			list_display( key_list ),
			list_display( attribute_data_list ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !dictionary_separate_query_dictionary )
	{
		dictionary_separate_query_dictionary =
			dictionary_small();
	}

	return
	dictionary_set_dictionary(
		dictionary_separate_query_dictionary /* source_destination */,
		/* -------------------------- */
		/* Returns dictionary_small() */
		/* -------------------------- */
		attribute_query_dictionary(
			query_relation_operator_prefix,
			query_equal,
			key_list,
			attribute_data_list ) );
}

LIST *drilldown_document_ajax_client_list(
		DRILLDOWN_PRIMARY *drilldown_primary,
		LIST *drilldown_one_to_many_list )
{
	LIST *ajax_client_list = list_new();
	LIST *client_list;
	DRILLDOWN_ONE_TO_MANY *drilldown_one_to_many;

	if ( !drilldown_primary
	||   !drilldown_primary->table_edit
	||   !drilldown_primary->table_edit->row_security )
	{
		char message[ 128 ];

		sprintf(message, "drilldown_primary is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	client_list =
		row_security_ajax_client_list(
		drilldown_primary->
			table_edit->
			row_security );

	list_set_list(
		ajax_client_list,
		client_list );

	if ( list_rewind( drilldown_one_to_many_list ) )
	do {
		drilldown_one_to_many =
			list_get(
				drilldown_one_to_many_list );

		if ( !drilldown_one_to_many->table_edit
		||   !drilldown_one_to_many->table_edit->row_security )
		{
			char message[ 128 ];

			sprintf(message,
		"drilldown_one_to_many->table_edit is empty or incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		client_list =
			row_security_ajax_client_list(
			drilldown_one_to_many->
				table_edit->
				row_security );

		list_set_list(
			ajax_client_list,
			client_list );

	} while ( list_next( drilldown_one_to_many_list ) );

	if ( !list_length( ajax_client_list ) )
	{
		list_free( ajax_client_list );
		ajax_client_list = (LIST *)0;
	}

	return ajax_client_list;
}

LIST *drilldown_relation_mto1_list(
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list )
{
	LIST *mto1_list;

	if ( !list_length( relation_mto1_list )
	&&   !list_length( relation_mto1_isa_list ) )
	{
		return NULL;
	}

	if ( !list_length( relation_mto1_list ) )
		return relation_mto1_isa_list;

	if ( !list_length( relation_mto1_isa_list ) )
		return relation_mto1_list;

	mto1_list = list_copy( relation_mto1_isa_list );

	list_set_list(
		mto1_list,
		relation_mto1_list );

	return mto1_list;
}

char *drilldown_input_post_table_edit_folder_name(
		const char *post_table_edit_folder_label,
		DICTIONARY *non_prefixed_dictionary )
{
	return
	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	dictionary_get(
		(char *)post_table_edit_folder_label /* key */,
		non_prefixed_dictionary );
}

DICTIONARY *drilldown_primary_original_post_dictionary(
		const char *dictionary_separate_no_display_prefix,
		const char *dictionary_separate_query_prefix,
		DICTIONARY *no_display_dictionary,
		DICTIONARY *drilldown_query_dictionary )
{
	DICTIONARY *original_post_dictionary = dictionary_medium();

	dictionary_prefixed_dictionary_set(
		original_post_dictionary /* out */,
		(char *)dictionary_separate_no_display_prefix,
		no_display_dictionary );

	dictionary_prefixed_dictionary_set(
		original_post_dictionary /* out */,
		(char *)dictionary_separate_query_prefix,
		drilldown_query_dictionary );

	return original_post_dictionary;
}

DICTIONARY *drilldown_one_to_many_original_post_dictionary(
		DICTIONARY *sort_dictionary,
		char *post_table_edit_folder_name,
		char *relation_many_folder_name,
		DICTIONARY *drilldown_query_dictionary )
{
	DICTIONARY *original_post_dictionary;

	if ( !relation_many_folder_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"relation_many_folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	original_post_dictionary = dictionary_small();

	dictionary_prefixed_dictionary_set(
		original_post_dictionary /* out */,
		DICTIONARY_SEPARATE_QUERY_PREFIX,
		drilldown_query_dictionary );

	if ( string_strcmp(
		post_table_edit_folder_name,
		relation_many_folder_name ) == 0 )
	{
		dictionary_prefixed_dictionary_set(
			original_post_dictionary /* out */,
			DICTIONARY_SEPARATE_SORT_PREFIX,
			sort_dictionary );
	}

	return original_post_dictionary;
}

