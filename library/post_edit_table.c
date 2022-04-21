/* $APPASERVER_HOME/library/post_edit_table.c				*/
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
#include "edit_table.h"
#include "post_edit_table.h"

POST_EDIT_TABLE *post_edit_table_new(
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
	POST_EDIT_TABLE *post_edit_table = post_edit_table_calloc();

	post_edit_table->target_frame = target_frame;
	post_edit_table->detail_base_folder_name = detail_base_folder_name;

	post_edit_table->session_folder =
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

	if ( ! ( post_edit_table->role =
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
		return (POST_EDIT_TABLE *)0;
	}

	if ( ! ( post_edit_table->folder =
			folder_fetch(
				folder_name,
				/* ------------------------- */
				/* Fetching role_folder_list */
				/* ------------------------- */
				role_name,
				role_exclude_lookup_attribute_name_list(
					post_edit_table->
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
		return (POST_EDIT_TABLE *)0;
	}

	post_edit_table->post_dictionary =
		post_dictionary_stdin_new(
			/* ----------------------------- */
			/* Not expecting a spooled file. */
			/* ----------------------------- */
			(char *)0 /* appaserver_parameter_upload_directory */,
			(char *)0 /* session_key */ );

	post_edit_table->dictionary_separate =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		dictionary_separate_post_edit_table_new(
			post_edit_table->
				post_dictionary->
				original_post_dictionary,
			application_name,
			login_name,
			post_edit_table->
				folder->
				relation_mto1_non_isa_list,
			folder_attribute_name_list(
				post_edit_table->
					folder->
					folder_attribute_append_isa_list ),
			role_operation_name_list(
				post_edit_table->
					folder->
					role_operation_list ),
			folder_attribute_date_name_list(
				post_edit_table->
					folder->
					folder_attribute_append_isa_list ) );

	post_edit_table->edit_table_spool_filename =
		edit_table_spool_filename(
			appaserver_parameter_data_directory(),
			application_name,
			folder_name,
			session_key );

	post_edit_table->file_dictionary =
		dictionary_file_fetch(
			post_edit_table->edit_table_spool_filename,
			SQL_DELIMITER );

	if ( dictionary_length(
		post_edit_table->
			dictionary_separate->
			non_prefixed_dictionary )
	&&   dictionary_length( post_edit_table->file_dictionary ) )
	{
		post_edit_table->update =
			update_new(
				application_name,
				login_name,
				post_edit_table->
					dictionary_separate->
					non_prefixed_dictionary
						/* post_dictionary */,
				post_edit_table->file_dictionary,
				post_edit_table->role,
				post_edit_table->folder );
	}

	if ( dictionary_length(
		post_edit_table->
			dictionary_separate->
			operation_dictionary ) )
	{
		post_edit_table->operation_row_list =
			operation_row_list_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				post_edit_table->
					folder->
					role_operation_list,
				post_edit_table->
					folder->
					primary_key_list,
				post_edit_table->
					folder->
					folder_attribute_name_list,
				post_edit_table->
					dictionary_separate->
					operation_dictionary,
				post_edit_table->
					dictionary_separate->
					multi_row_dictionary );
	}

	return post_edit_table;
}

POST_EDIT_TABLE *post_edit_table_calloc( void )
{
	POST_EDIT_TABLE *post_edit_table;

	if ( ! ( post_edit_table = calloc( 1, sizeof( POST_EDIT_TABLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_edit_table;
}

char *post_edit_table_action_string(
			char *post_edit_table_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *target_frame,
			char *detail_base_folder_name )
{
	char action_string[ 1024 ];

	if ( !post_edit_table_executable
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
		post_edit_table_executable,
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

