/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/merge_purge.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include "sql.h"
#include "String.h"
#include "piece.h"
#include "document.h"
#include "dictionary.h"
#include "folder.h"
#include "relation_one2m.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "role.h"
#include "role_folder.h"
#include "sql.h"
#include "role_folder.h"
#include "frameset.h"
#include "security.h"
#include "application.h"
#include "attribute.h"
#include "widget.h"
#include "appaserver.h"
#include "environ.h"
#include "javascript.h"
#include "update.h"
#include "delete_primary.h"
#include "dictionary_separate.h"
#include "merge_purge.h"

char *merge_purge_hypertext_reference(
		char *executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		char *folder_name )
{
	char hypertext_reference[ 1024 ];

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name
	||   !folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(hypertext_reference,
		"%s/%s?%s+%s+%s+%s+%s+%s",
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
			application_ssl_support_boolean(
				application_name ) ),
		executable,
		application_name,
		session_key,
		login_name,
		role_name,
		process_name,
		folder_name );

	return strdup( hypertext_reference );
}

MERGE_PURGE_CHOOSE *merge_purge_choose_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name )
{
	MERGE_PURGE_CHOOSE *merge_purge_choose;

	if ( !argc
	||   !argv
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	merge_purge_choose = merge_purge_choose_calloc();

	merge_purge_choose->session_process =
		/* --------------------------------------------- */
		/* Sets appaserver environment and outputs argv. */
		/* Each parameter is security inspected.	 */
		/* Any error will exit( 1 ).			 */
		/* --------------------------------------------- */
		session_process_integrity_exit(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			process_name );

	merge_purge_choose->title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		merge_purge_choose_title_string(
			process_name );

	merge_purge_choose->sub_sub_title_string =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		merge_purge_choose_sub_sub_title_string();

	merge_purge_choose->process =
		process_fetch(
			process_name,
			(char *)0 /* document_root */,
			(char *)0 /* relative_source_directory */,
			0 /* not check_executable */,
			(char *)0 /* mount_point */ );

	merge_purge_choose->document_body_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_body_html(
			(char *)0 /* menu_html */,
			merge_purge_choose->title_string,
			(char *)0 /* sub_title_string */,
			merge_purge_choose->sub_sub_title_string,
			(char *)0 /* body_onload_open_tag */,
			(char *)0 /* document_body_hide_preload_html */,
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			document_body_notepad_html(
				merge_purge_choose->process->notepad ) );

	merge_purge_choose->role_folder_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		merge_purge_choose_role_folder_where(
			ROLE_FOLDER_TABLE,
			RELATION_TABLE,
			ROLE_PERMISSION_UPDATE,
			role_name );

	merge_purge_choose->role_folder_name_list =
		merge_purge_choose_role_folder_name_list(
			ROLE_FOLDER_TABLE,
			merge_purge_choose->role_folder_where );

	merge_purge_choose->widget_container_list =
		merge_purge_choose_widget_container_list(
			application_name,
			session_key,
			login_name,
			role_name,
			process_name,
			merge_purge_choose->role_folder_name_list );

	merge_purge_choose->widget_container_list_html =
		widget_container_list_html(
			(char *)0 /* state */,
			-1 /* row_number */,
			(char *)0 /* background_color */,
			merge_purge_choose->widget_container_list );

	merge_purge_choose->document_form_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_form_html(
			(char *)0 /* document_html */,
			(char *)0 /* document_head_html */,
			(char *)0 /* document_head_close_tag */,
			merge_purge_choose->document_body_html,
			merge_purge_choose->widget_container_list_html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_body_close_tag(),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_close_tag() );

	free( merge_purge_choose->widget_container_list_html );

	return merge_purge_choose;
}

MERGE_PURGE_CHOOSE *merge_purge_choose_calloc( void )
{
	MERGE_PURGE_CHOOSE *merge_purge_choose;

	if ( ! ( merge_purge_choose =
			calloc( 1, sizeof ( MERGE_PURGE_CHOOSE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return merge_purge_choose;
}

char *merge_purge_choose_title_string( char *process_name )
{
	static char title_string[ 128 ];
	char buffer[ 64 ];

	sprintf(title_string,
		"%s Choose Table",
		string_initial_capital(
			buffer,
			process_name ) );

	return title_string;
}

LIST *merge_purge_choose_widget_container_list(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		LIST *role_folder_name_list )
{
	LIST *widget_container_list;
	char *folder_name;
	int counter;
	WIDGET_CONTAINER *widget_container;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( role_folder_name_list ) ) return (LIST *)0;

	widget_container_list = list_new();
	counter = 2;

	list_set(
		widget_container_list,
		( widget_container =
			widget_container_new(
				table_open, (char *)0 /* widget_name */ ) ) );

		widget_container->table_open->border_boolean = 1;
		widget_container->table_open->cell_spacing = 3;
		widget_container->table_open->cell_padding = 3;

	do {
		folder_name = list_get( role_folder_name_list );

		if ( ++counter == 3 )
		{
			list_set(
				widget_container_list,
				widget_container_new(
					table_row, (char *)0 ) );

			counter = 0;
		}

		list_set(
			widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

		list_set(
			widget_container_list,
			merge_purge_choose_widget_container(
				application_name,
				session_key,
				login_name,
				role_name,
				process_name,
				folder_name ) );

	} while ( list_next( role_folder_name_list ) );

	list_set(
		widget_container_list,
		widget_container_new(
			table_close, (char *)0 /* widget_name */ ) );

	return widget_container_list;
}

WIDGET_CONTAINER *merge_purge_choose_widget_container(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		char *folder_name )
{
	WIDGET_CONTAINER *widget_container;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name
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

	widget_container =
		widget_container_new(
			anchor, (char *)0 /* widget_name */ );

	widget_container->anchor->hypertext_reference =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		merge_purge_hypertext_reference(
			MERGE_PURGE_CHOOSE_POST_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			process_name,
			folder_name );

	widget_container->anchor->target_frame = FRAMESET_PROMPT_FRAME;

	widget_container->anchor->prompt =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		merge_purge_choose_prompt(
			folder_name );

	return widget_container;
}

char *merge_purge_choose_prompt( char *folder_name )
{
	char prompt[ 128 ];

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

	return
	strdup(
		string_initial_capital(
			prompt,
			folder_name ) );
}

MERGE_PURGE_DELETE *merge_purge_delete_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *delete_delimited_string,
		LIST *folder_attribute_primary_key_list )
{
	MERGE_PURGE_DELETE *merge_purge_delete;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !delete_delimited_string
	||   !list_length( folder_attribute_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	merge_purge_delete = merge_purge_delete_calloc();

	merge_purge_delete->delete_primary_query_cell_list =
		delete_primary_query_cell_list(
			SQL_DELIMITER,
			delete_delimited_string,
			folder_attribute_primary_key_list );

	merge_purge_delete->delete =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		delete_new(
			application_name,
			login_name,
			role_name,
			folder_name,
			merge_purge_delete->delete_primary_query_cell_list,
			1 /* isa_boolean */,
			0 /* not update_null_boolean */ );

	if ( !merge_purge_delete->
		delete->
		delete_input->
		folder_operation_delete_boolean )
	{
		return NULL;
	}

	return merge_purge_delete;
}

MERGE_PURGE_DELETE *merge_purge_delete_calloc( void )
{
	MERGE_PURGE_DELETE *merge_purge_delete;

	if ( ! ( merge_purge_delete =
			calloc( 1,
				sizeof ( MERGE_PURGE_DELETE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return merge_purge_delete;
}

char *merge_purge_delete_statement(
		LIST *delete_string_list,
		char *folder_table_name,
		LIST *folder_attribute_list,
		LIST *folder_attribute_key_list )
{
	char delete_statement[ 1024 ];
	char *ptr = delete_statement;
	char *where;

	if ( !list_length( delete_string_list )
	||   !folder_table_name
	||   !list_length( folder_attribute_list )
	||   !list_length( folder_attribute_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_attribute_where_string(
			folder_attribute_list,
			folder_attribute_key_list,
			delete_string_list /* data_list */ );

	ptr +=
		sprintf(
			ptr,
			"delete from %s where %s;",
			folder_table_name,
			where );

	return strdup( delete_statement );
}

MERGE_PURGE_UPDATE *merge_purge_update_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *keep_string_list,
		LIST *delete_string_list,
		LIST *relation_one2m_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_primary_list )
{
	MERGE_PURGE_UPDATE *merge_purge_update;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !list_length( keep_string_list )
	||   !list_length( delete_string_list )
	||   !list_length( relation_one2m_list )
	||   !list_length( folder_attribute_primary_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	merge_purge_update = merge_purge_update_calloc();

	merge_purge_update->multi_row_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		merge_purge_update_dictionary(
			keep_string_list,
			folder_attribute_primary_list );

	merge_purge_update->file_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		merge_purge_update_dictionary(
			delete_string_list,
			folder_attribute_primary_list );

	merge_purge_update->update =
		update_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			0 /* not override_row_restrictions */,
			merge_purge_update->multi_row_dictionary,
			merge_purge_update->file_dictionary,
			(FOLDER_ROW_LEVEL_RESTRICTION *)0,
			relation_one2m_list,
			relation_mto1_isa_list,
			folder_attribute_primary_list
				/* folder_attribute_append_isa_list */,
			(PROCESS *)0 /* post_change_process */ );

	if ( !merge_purge_update->update
	||   !merge_purge_update->update->update_row_list )
	{
		char message[ 128 ];

		sprintf(message,
			"update_new(%s) returned empty or incomplete.",
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	update_row_list_zap_root(
		merge_purge_update->
			update->
			update_row_list );

	return merge_purge_update;
}

MERGE_PURGE_UPDATE *merge_purge_update_calloc( void )
{
	MERGE_PURGE_UPDATE *merge_purge_update;

	if ( ! ( merge_purge_update =
			calloc( 1,
				sizeof ( MERGE_PURGE_UPDATE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return merge_purge_update;
}

#ifdef NOT_DEFINED
char *merge_purge_update_statement(
		LIST *keep_string_list,
		LIST *delete_string_list,
		LIST *primary_key_list,
		LIST *many_folder_attribute_list,
		char *folder_table_name )
{
	if ( !list_length( keep_string_list )
	||   !list_length( delete_string_list )
	||   !list_length( primary_key_list )
	||   !list_length( many_folder_attribute_list )
	||   !folder_table_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_length( keep_string_list ) !=
	     list_length( primary_key_list ) )
	{
		char message[ 1024 ];

		sprintf(message,
			"cannot align: (%s) <> (%s)",
			list_display_delimited(
				primary_key_list,
				',' ),
			list_display_delimited(
				keep_string_list,
				',' ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_length( delete_string_list ) !=
	     list_length( primary_key_list ) )
	{
		char message[ 1024 ];

		sprintf(message,
			"cannot align: (%s) <> (%s)",
			list_display_delimited(
				primary_key_list,
				',' ),
			list_display_delimited(
				delete_string_list,
				',' ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	update_sql_statement_string(
		keep_string_list /* new_string_list */,
		primary_key_list,
		delete_string_list /* primary_data_list */,
		many_folder_attribute_list,
		folder_table_name );
}
#endif

MERGE_PURGE_PROCESS *merge_purge_process_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		char *folder_name )
{
	MERGE_PURGE_PROCESS *merge_purge_process;

	if ( !argc
	||   !argv
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name
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

	merge_purge_process = merge_purge_process_calloc();

	/* --------------------------------------------- */
	/* Sets appaserver environment and outputs argv. */
	/* Each parameter is security inspected.	 */
	/* Any error will exit( 1 ).			 */
	/* --------------------------------------------- */
	merge_purge_process->session_process =
		session_process_integrity_exit(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			process_name );

	merge_purge_process->folder_name =
		/* ---------------------------- */
		/* Returns heap memory or datum */
		/* ---------------------------- */
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			folder_name );

	merge_purge_process->role_folder_list =
		role_folder_list(
			role_name,
			merge_purge_process->folder_name );

	if ( !session_folder_valid(
		APPASERVER_UPDATE_STATE,
		merge_purge_process->folder_name,
		merge_purge_process->role_folder_list ) )
	{
		session_access_failed_message_exit(
			application_name,
			login_name,
			session_current_ip_address(
				environment_remote_ip_address(
					ENVIRONMENT_REMOTE_KEY ) ) );
	}

	merge_purge_process->post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_stdin_new(
			/* ---------------------------------- */
			/* Used when expecting a spooled file */
			/* ---------------------------------- */
			(char *)0 /* application_name */,
			(char *)0 /* upload_directory */,
			(LIST *)0 /* upload_filename_list */ );

	merge_purge_process->non_prefixed_dictionary =
		dictionary_separate_non_prefixed(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			merge_purge_process->
				post_dictionary->
				original_post_dictionary );

	if ( !merge_purge_process->non_prefixed_dictionary )
	{
		char message[ 128 ];

		sprintf(message,
			"dictionary_separate_non_prefixed() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( merge_purge_process->keep_delimited_string =
			dictionary_get(
				MERGE_PURGE_KEEP_LABEL,
				merge_purge_process->
					non_prefixed_dictionary ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"dictionary_get(%s) returned empty.",
			MERGE_PURGE_KEEP_LABEL );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( merge_purge_process->delete_delimited_string =
			dictionary_get(
				MERGE_PURGE_DELETE_LABEL,
				merge_purge_process->
					non_prefixed_dictionary ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"dictionary_get(%s) returned empty.",
			MERGE_PURGE_DELETE_LABEL );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	merge_purge_process->same_boolean =
		merge_purge_process_same_boolean(
			merge_purge_process->keep_delimited_string,
			merge_purge_process->delete_delimited_string );

	if ( merge_purge_process->same_boolean )
		return merge_purge_process;

	merge_purge_process->keep_string_list =
		merge_purge_process_keep_string_list(
			SQL_DELIMITER,
			merge_purge_process->keep_delimited_string );

	merge_purge_process->delete_string_list =
		merge_purge_process_delete_string_list(
			SQL_DELIMITER,
			merge_purge_process->delete_delimited_string );

	merge_purge_process->folder_attribute_list =
		folder_attribute_list(
			role_name,
			merge_purge_process->folder_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			1 /* fetch_attribute */ );

	merge_purge_process->folder_attribute_primary_list =
		folder_attribute_primary_list(
			merge_purge_process->folder_name,
			merge_purge_process->folder_attribute_list );

	merge_purge_process->folder_attribute_primary_key_list =
		folder_attribute_primary_key_list(
			merge_purge_process->folder_name,
			merge_purge_process->folder_attribute_primary_list );

	merge_purge_process->relation_one2m_list =
		relation_one2m_list(
			role_name,
			merge_purge_process->folder_name
				/* one_folder_name */,
			merge_purge_process->
				folder_attribute_primary_key_list
				/* one_folder_primary_key_list */,
			0 /* not include_isa_boolean */ );

	if ( !list_length(
		merge_purge_process->
			relation_one2m_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"For %s: no relations exist. Just delete the row.",
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	merge_purge_process->relation_mto1_isa_list =
		relation_mto1_isa_list(
			(LIST *)0 /* mto1_isa_list */,
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			folder_name
				/* many_folder_name */,
			merge_purge_process->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */,
			0 /* not fetch_relation_one2m_list */,
			0 /* not fetch_relation_mto1_list */ );

	merge_purge_process->execute_boolean =
		merge_purge_process_execute_boolean(
			APPASERVER_EXECUTE_YN,
			merge_purge_process->non_prefixed_dictionary );

	merge_purge_process->merge_purge_update =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		merge_purge_update_new(
			application_name,
			session_key,
			login_name,
			role_name,
			merge_purge_process->folder_name,
			merge_purge_process->keep_string_list,
			merge_purge_process->delete_string_list,
			merge_purge_process->relation_one2m_list,
			merge_purge_process->relation_mto1_isa_list,
			merge_purge_process->folder_attribute_primary_list );

	merge_purge_process->merge_purge_delete =
		merge_purge_delete_new(
			application_name,
			login_name,
			role_name,
			merge_purge_process->folder_name,
			merge_purge_process->delete_delimited_string,
			merge_purge_process->
				folder_attribute_primary_key_list );

	if ( merge_purge_process->merge_purge_delete )
	{
		char *exists_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			merge_purge_process_exists_system_string(
				folder_name,
				merge_purge_process->
					merge_purge_delete->
					delete->
					delete_root->
					delete_where );

		merge_purge_process->row_exists_boolean =
			merge_purge_process_row_exists_boolean(
				exists_system_string );

		free( exists_system_string );
	}

	return merge_purge_process;
}

MERGE_PURGE_PROCESS *merge_purge_process_calloc( void )
{
	MERGE_PURGE_PROCESS *merge_purge_process;

	if ( ! ( merge_purge_process =
			calloc( 1,
			sizeof ( MERGE_PURGE_PROCESS ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return merge_purge_process;
}

boolean merge_purge_process_execute_boolean(
		const char *appaserver_execute_yn,
		DICTIONARY *non_prefixed_dictionary )
{
	char *execute_yn;

	if ( !dictionary_length( non_prefixed_dictionary ) )
	{
		char message[ 128 ];

		sprintf(message, "non_prefixed_dictionary is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	execute_yn =
		dictionary_get(
			(char *)appaserver_execute_yn,
			non_prefixed_dictionary );

	return ( execute_yn && *execute_yn == 'y' );
}

boolean merge_purge_process_same_boolean(
		char *keep_delimited_string,
		char *delete_delimited_string )
{
	if ( string_strcmp(
		keep_delimited_string,
		delete_delimited_string ) == 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

LIST *merge_purge_process_keep_string_list(
		char sql_delimiter,
		char *keep_delimited_string )
{
	return
	list_string_list(
		keep_delimited_string,
		sql_delimiter );
}

LIST *merge_purge_process_delete_string_list(
		char sql_delimiter,
		char *delete_delimited_string )
{
	return
	list_string_list(
		delete_delimited_string,
		sql_delimiter );
}

char *merge_purge_choose_role_folder_where(
		const char *role_folder_table,
		const char *relation_table,
		const char *role_permission_update,
		char *role_name )
{
	static char where[ 256 ];

	if ( !role_name )
	{
		char message[ 128 ];

		sprintf(message, "role_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(where,
		"role = '%s' and "
		"permission = '%s' and "
		"exists ( "
		"	select 1 "
		"	from %s "
		"	where %s.table_name = %s.related_table )",
		role_name,
		role_permission_update,
		relation_table,
		role_folder_table,
		relation_table );

	return where;
}

LIST *merge_purge_choose_role_folder_name_list(
		char *role_folder_table,
		char *role_folder_where )
{
	char system_string[ 1024 ];

	if ( !role_folder_table
	||   !role_folder_where )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
"echo \"select table_name from %s where %s order by table_name;\" | sql.e",
		role_folder_table,
		role_folder_where );

	return list_pipe_fetch( system_string );
}

void merge_purge_process_execute(
		char *application_name,
		MERGE_PURGE_UPDATE *merge_purge_update,
		MERGE_PURGE_DELETE *merge_purge_delete )
{
	if ( !application_name
	||   !merge_purge_update
	||   !merge_purge_update->update
	||   !merge_purge_update->update->update_row_list
	||   !merge_purge_delete
	||   !merge_purge_delete->delete )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	update_row_list_execute(
		application_name,
		merge_purge_update->
			update->
			update_row_list,
		merge_purge_update->
			update->
			appaserver_error_filename );

	update_row_list_command_line_execute(
		merge_purge_update->
			update->
			update_row_list );

	(void)delete_execute(
		merge_purge_delete->
			delete->
			delete_input->
			appaserver_error_filename,
		merge_purge_delete->
			delete->
			sql_statement_list,
		merge_purge_delete->
			delete->
			pre_command_list,
		merge_purge_delete->
			delete->
			command_list );
}

MERGE_PURGE_FOLDER *merge_purge_folder_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		char *folder_name )
{
	MERGE_PURGE_FOLDER *merge_purge_folder;

	if ( !argc
	||   !argv
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name
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

	merge_purge_folder = merge_purge_folder_calloc();

	merge_purge_folder->session_process =
		/* --------------------------------------------- */
		/* Sets appaserver environment and outputs argv. */
		/* Each parameter is security inspected.	 */
		/* Any error will exit( 1 ).			 */
		/* --------------------------------------------- */
		session_process_integrity_exit(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			process_name );

	merge_purge_folder->folder_name =
		/* ---------------------------- */
		/* Returns heap memory or datum */
		/* ---------------------------- */
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			folder_name );

	merge_purge_folder->role_folder_list =
		role_folder_list(
			role_name,
			merge_purge_folder->folder_name );

	if ( !session_folder_valid(
		APPASERVER_UPDATE_STATE,
		merge_purge_folder->folder_name,
		merge_purge_folder->role_folder_list ) )
	{
		session_access_failed_message_exit(
			application_name,
			login_name,
			session_current_ip_address(
				environment_remote_ip_address(
					ENVIRONMENT_REMOTE_KEY ) ) );
	}

	merge_purge_folder->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			merge_purge_folder->folder_name,
			/* Leave blank b/c not caching */
			(char *)0 /* role_name */,
			(LIST *)0 /* exclude_attribute_name_list */,
			/* Sets folder_attribute_primary_list */
			/* Sets folder_attribute_primary_key_list */
			/* Sets folder_attribute_name_list */
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */ );

	merge_purge_folder->application_menu_horizontal_boolean =
		application_menu_horizontal_boolean(
			application_name );

	merge_purge_folder->appaserver_parameter =
		appaserver_parameter_new();

	if ( merge_purge_folder->application_menu_horizontal_boolean )
	{
		merge_purge_folder->folder_menu =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			folder_menu_fetch(
				application_name,
				session_key,
				role_name,
				merge_purge_folder->
					appaserver_parameter->
					data_directory,
				0 /* not folder_menu_remove_boolean */ );

		merge_purge_folder->menu =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			menu_new(
				application_name,
				session_key,
				login_name,
				role_name,
				1 /* application_menu_horizontal_boolean */,
				merge_purge_folder->folder_menu->count_list );
	}

	merge_purge_folder->merge_purge_hypertext_reference =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		merge_purge_hypertext_reference(
			MERGE_PURGE_FOLDER_POST_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			process_name,
			merge_purge_folder->folder_name );

	merge_purge_folder->keep_prompt =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		merge_purge_folder_keep_prompt(
			merge_purge_folder->folder_name );

	merge_purge_folder->delete_prompt =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		merge_purge_folder_delete_prompt(
			merge_purge_folder->folder_name );

	merge_purge_folder->form_merge_purge =
		form_merge_purge_new(
			MERGE_PURGE_KEEP_LABEL,
			MERGE_PURGE_DELETE_LABEL,
			application_name,
			login_name,
			merge_purge_folder->folder_name,
			merge_purge_folder->folder->no_initial_capital,
			merge_purge_folder->
				folder->
				folder_attribute_primary_list,
			merge_purge_folder->keep_prompt,
			merge_purge_folder->delete_prompt,
			merge_purge_folder->merge_purge_hypertext_reference );

	merge_purge_folder->application_title_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		application_title_string(
			application_name );

	merge_purge_folder->title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		merge_purge_folder_title_string(
			process_name,
			merge_purge_folder->folder_name );

	merge_purge_folder->document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_new(
			application_name,
			merge_purge_folder->application_title_string,
			merge_purge_folder->title_string,
			(char *)0 /* sub_title_string */,
			(char *)0 /* sub_sub_title_string */,
			merge_purge_folder->folder->notepad,
			(char *)0 /* onload_javascript_string */,
			(merge_purge_folder->menu)
				? merge_purge_folder->menu->html
				: (char *)0,
			/* ------------------------------ */
			/* Returns program memory or null */
			/* ------------------------------ */
			document_head_menu_setup_string(
				(merge_purge_folder->menu)
					? 1
					: 0 /* menu_boolean */ ),
			(char *)0 /* calendar_setup_string */,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			javascript_include_string(
				application_name,
				(LIST *)0 /* javascript_filename_list */ ) );

	merge_purge_folder->document_form_html =
		document_form_html(
			merge_purge_folder->document->html,
			merge_purge_folder->document->document_head->html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_head_close_tag(),
			merge_purge_folder->document->document_body->html,
			merge_purge_folder->
				form_merge_purge->
				form_html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_body_close_tag(),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_close_tag() );

	form_merge_purge_free(
		merge_purge_folder->
			form_merge_purge );

	return merge_purge_folder;
}

MERGE_PURGE_FOLDER *merge_purge_folder_calloc( void )
{
	MERGE_PURGE_FOLDER *merge_purge_folder;

	if ( ! ( merge_purge_folder =
			calloc( 1,
				sizeof ( MERGE_PURGE_FOLDER ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return merge_purge_folder;
}

char *merge_purge_folder_keep_prompt( char *folder_name )
{
	static char keep_prompt[ 128 ];
	char buffer[ 64 ];

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(keep_prompt,
		"Keep %s",
		string_initial_capital(
			buffer,
			folder_name ) );

	return keep_prompt;
}

char *merge_purge_folder_delete_prompt( char *folder_name )
{
	static char delete_prompt[ 128 ];
	char buffer[ 64 ];

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(delete_prompt,
		"Purge %s",
		string_initial_capital(
			buffer,
			folder_name ) );

	return delete_prompt;
}

char *merge_purge_folder_title_string(
		char *process_name,
		char *folder_name )
{
	static char title_string[ 128 ];
	char buffer[ 64 ];

	if ( !process_name
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

	sprintf(title_string,
		"%s %s",
		process_name,
		folder_name );

	return
	strdup(
		string_initial_capital(
			buffer,
			title_string ) );
}

char *merge_purge_choose_sub_sub_title_string( void )
{
	return "Choose a table to merge purge";
}

DICTIONARY *merge_purge_update_dictionary(
		LIST *string_list,
		LIST *folder_attribute_primary_list )
{
	DICTIONARY *dictionary;
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( !list_rewind( string_list )
	||   !list_rewind( folder_attribute_primary_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	dictionary = dictionary_small();

	do {
		folder_attribute =
			list_get(
				folder_attribute_primary_list );

		dictionary_set(
			dictionary,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			merge_purge_update_key(
				folder_attribute->attribute_name ),
			(char *)list_get( string_list ) );

		list_next( string_list );

	} while ( list_next( folder_attribute_primary_list ) );

	return dictionary;
}

char *merge_purge_update_key( char *attribute_name )
{
	char key[ 256 ];

	if ( !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		/* Stub */
		/* ---- */
		exit( 1 );
	}

	sprintf(key,
		"%s_1",
		attribute_name );

	return strdup( key );
}

void merge_purge_html_display(
		char *delete_delimited_string,
		DELETE *delete )
{
	LIST *query_fetch_list;
	QUERY_FETCH *query_fetch;

	if ( !delete )
	{
		char message[ 128 ];

		sprintf(message, "delete is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	printf( "%s\n",
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		merge_purge_heading_html(
			WIDGET_DROP_DOWN_DASH_DELIMITER,
			SQL_DELIMITER,
			delete_delimited_string ) );

	fflush( stdout );

	query_fetch_list =
		delete_one2m_query_fetch_list(
			delete->delete_one2m_list->list
				/* delete_one2m_list */ );

	if ( list_rewind( query_fetch_list ) )
	do {
		query_fetch =
			list_get(
				query_fetch_list );

		query_fetch_html_display(
			query_fetch->folder_name,
			query_fetch->query_select_name_list,
			query_fetch->row_list );

	} while ( list_next( query_fetch_list ) );
}

char *merge_purge_heading_html(
		const char *widget_drop_down_dash_delimiter,
		const char sql_delimiter,
		char *delete_delimited_string )
{
	char sql_delimiter_string[ 2 ] = {0};
	char html[ 1024 ];
	char source_destination[ 1000 ];

	if ( !delete_delimited_string )
	{
		char message[ 128 ];

		sprintf(message, "delete_delimited_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	*sql_delimiter_string = sql_delimiter;

	string_strcpy( source_destination, delete_delimited_string, 1000 );

	snprintf(
		html,
		sizeof ( html ),
		"<h3>Purge: %s</h3>",
		string_with_space_search_replace(
			source_destination /* in/out */,
			sql_delimiter_string /* search_string */,
			(char *)widget_drop_down_dash_delimiter
				/* replace_string */ ) );

	return strdup( html );
}

char *merge_purge_process_exists_system_string(
		char *folder_name,
		char *delete_where )
{
	char *table_name;
	char system_string[ 1024 ];

	if ( !folder_name
	||   !delete_where )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* Returns static memory */
	/* --------------------- */
	table_name = appaserver_table_name( folder_name );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh \"count(1)\" %s \"%s\"",
		table_name,
		delete_where );

	return strdup( system_string );
}

boolean merge_purge_process_row_exists_boolean( char *exists_system_string )
{
	if ( !exists_system_string )
	{
		char message[ 128 ];

		sprintf(message, "exists_system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	(boolean)(atoi(
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe( exists_system_string ) ) );
}

