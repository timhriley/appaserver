/* $APPASERVER_HOME/library/post_table_edit.c				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "String.h"
#include "appaserver.h"
#include "appaserver_library.h"
#include "appaserver_parameter.h"
#include "application.h"
#include "folder_attribute.h"
#include "role_operation.h"
#include "dictionary.h"
#include "update.h"
#include "table_edit.h"
#include "post_table_edit.h"

POST_TABLE_EDIT *post_table_edit_new(
			int argc,
			char **argv,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *target_frame,
			char *detail_base_folder_name )
{
	POST_TABLE_EDIT *post_table_edit = post_table_edit_calloc();

	post_table_edit->target_frame = target_frame;
	post_table_edit->detail_base_folder_name = detail_base_folder_name;

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
			APPASERVER_LOOKUP_STATE );

	if ( ! ( post_table_edit->role =
			role_fetch(
				role_name,
				1 /* fetch_attribute_exclude_list */ ) ) )
 	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: role_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			role_name );
		return (POST_TABLE_EDIT *)0;
	}

	if ( ! ( post_table_edit->folder =
			folder_fetch(
				folder_name,
				/* ------------------------- */
				/* Fetching role_folder_list */
				/* ------------------------- */
				role_name,
				role_exclude_lookup_attribute_name_list(
					post_table_edit->
						role->
						attribute_exclude_list ),
				/* -------------------------- */
				/* Also sets primary_key_list */
				/* -------------------------- */
				1 /* fetch_folder_attribute_list */,
				1 /* fetch_relation_mto1_non_isa_list */,
				/* ------------------------------------------ */
				/* Also sets folder_attribute_append_isa_list */
				/* ------------------------------------------ */
				1 /* fetch_relation_mto1_isa_list */,
				0 /* not fetch_relation_one2m_list */,
				1 /* fetch_relation_one2m_recursive_list */,
				0 /* not fetch_process */,
				1 /* fetch_role_folder_list */,
				1 /* fetch_row_level_restriction */,
				1 /* fetch_role_operation_list */ ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: folder_fetch(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				folder_name );
		return (POST_TABLE_EDIT *)0;
	}

	post_table_edit->post_dictionary =
		post_dictionary_stdin_new(
			/* ----------------------------- */
			/* Not expecting a spooled file. */
			/* ----------------------------- */
			(char *)0 /* appaserver_parameter_upload_directory */,
			(char *)0 /* session_key */ );

	post_table_edit->dictionary_separate =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		dictionary_separate_post_table_edit_new(
			post_table_edit->
				post_dictionary->
				original_post_dictionary,
			application_name,
			login_name,
			post_table_edit->
				folder->
				folder_attribute_name_list,
			role_operation_name_list(
				post_table_edit->
					folder->
					role_operation_list ),
			folder_attribute_date_name_list(
				post_table_edit->
					folder->
					folder_attribute_append_isa_list ),
			post_table_edit->
				folder->
				folder_attribute_append_isa_list );

	post_table_edit->table_edit_spool_filename =
		table_edit_spool_filename(
			appaserver_parameter_data_directory(),
			application_name,
			folder_name,
			session_key );

	post_table_edit->file_dictionary =
		dictionary_file_fetch(
			post_table_edit->table_edit_spool_filename,
			SQL_DELIMITER );

	if ( dictionary_length(
		post_table_edit->
			dictionary_separate->
			multi_row_dictionary )
	&&   dictionary_length( post_table_edit->file_dictionary ) )
	{
		post_table_edit->update =
			update_new(
				application_name,
				login_name,
				post_table_edit->
					dictionary_separate->
					multi_row_dictionary,
				post_table_edit->file_dictionary,
				post_table_edit->role,
				post_table_edit->folder );
	}

	if ( dictionary_length(
		post_table_edit->
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
				post_table_edit->
					folder->
					role_operation_list,
				post_table_edit->
					folder->
					primary_key_list,
				post_table_edit->
					folder->
					folder_attribute_name_list,
				post_table_edit->
					dictionary_separate->
					operation_dictionary,
				post_table_edit->
					dictionary_separate->
					multi_row_dictionary );
	}

	return post_table_edit;
}

POST_TABLE_EDIT *post_table_edit_calloc( void )
{
	POST_TABLE_EDIT *post_table_edit;

	if ( ! ( post_table_edit = calloc( 1, sizeof( POST_TABLE_EDIT ) ) ) )
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

char *post_table_edit_action_string(
			char *post_table_edit_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *target_frame,
			char *detail_base_folder_name )
{
	char action_string[ 1024 ];

	if ( !post_table_edit_executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !target_frame )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(action_string,
		"%s/%s?%s+%s+%s+%s+%s+%s+%s",
		appaserver_library_http_prompt(
			appaserver_parameter_cgi_directory(),
			appaserver_library_server_address(),
			application_ssl_support_yn(
				application_name ),
			application_prepend_http_protocol_yn(
				application_name ) ),
		post_table_edit_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		folder_name,
		target_frame,
		(detail_base_folder_name)
			? detail_base_folder_name
			: "" );

	return strdup( action_string );
}

