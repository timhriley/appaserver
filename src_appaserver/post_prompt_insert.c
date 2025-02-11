/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_prompt_insert.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "String.h"
#include "frameset.h"
#include "create_table.h"
#include "table_edit.h"
#include "table_insert.h"
#include "prompt_insert.h"
#include "execute_system_string.h"
#include "post_prompt_insert.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	boolean omit_content_type_boolean = 0;
	POST_PROMPT_INSERT *post_prompt_insert;
	char *insert_statement_error_string = {0};
	char *results_string = {0};
	char *system_string = {0};
	boolean fatal_duplicate_error = 0;

	if ( argc < 6 )
	{
		fprintf(stderr,
"Usage: %s application session login_name role folder [omit_content_type_yn]\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	folder_name = argv[ 5 ];

	if ( argc == 7 ) 
	{
		omit_content_type_boolean =
			( *argv[ 6 ] == 'y' );
	}

	if ( !omit_content_type_boolean ) document_content_type_output();

	post_prompt_insert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_prompt_insert_new(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name );

	if ( post_prompt_insert->vertical_new_prompt )
	{
		system_string =
			post_prompt_insert->
				vertical_new_prompt->
				execute_system_string_table_insert;

		goto execute_system_string;
	}

	/* If pair one to many but left out a primary key */
	/* ---------------------------------------------- */
	if ( post_prompt_insert->missing_display )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			"Insert Results" /* title_string */ );

		printf(	"%s\n",
			post_prompt_insert->missing_display );

		document_close();
		exit( 0 );
	}

	/* If pressed the lookup button */
	/* ---------------------------- */
	if ( post_prompt_insert->post_prompt_insert_input->lookup_boolean )
	{
		system_string =
			execute_system_string_table_edit(
				TABLE_EDIT_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				folder_name,
				FRAMESET_TABLE_FRAME /* target_frame */,
				(char *)0 /* results_string */,
				(char *)0 /* error_string */,
				post_prompt_insert->
					dictionary_separate_send_string,
				post_prompt_insert->
					post_prompt_insert_input->
					appaserver_error_filename );

		goto execute_system_string;
	}

	/* If included all the primary keys */
	/* -------------------------------- */
	if ( post_prompt_insert->insert )
	{
		if ( !post_prompt_insert->
			insert->
			insert_zero
		||   !post_prompt_insert->
			insert->
			insert_statement
		||   !post_prompt_insert->
			insert->
			insert_zero->
			insert_folder )
		{
			char message[ 128 ];

			sprintf(message,
				"post_prompt_insert->insert is incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		insert_statement_error_string =
			insert_statement_execute(
				application_name,
				post_prompt_insert->
					insert->
					insert_statement->
					insert_folder_statement_list,
				post_prompt_insert->
					insert->
					appaserver_error_filename );

		fatal_duplicate_error =
			post_prompt_insert_fatal_duplicate_error(
				CREATE_TABLE_UNIQUE_SUFFIX,
				CREATE_TABLE_ADDITIONAL_SUFFIX,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				appaserver_table_name(
					post_prompt_insert->
						post_prompt_insert_input->
						folder->
						folder_name ),
				post_prompt_insert->
					post_prompt_insert_input->
					folder->
					folder_attribute_primary_key_list,
				insert_statement_error_string );

		if ( fatal_duplicate_error )
		{
			document_process_output(
				application_name,
				(LIST *)0 /* javascript_filename_list */,
				"Duplicate Error" /* title_string */ );

			printf(	"<h3>%s</h3>\n",
				insert_statement_error_string );

			document_close();
			exit( 0 );
		}
		else
		{
			results_string =
				post_prompt_insert->
					insert->
					results_string;
		}
	}

	/* If pair one to many is finished */
	/* ------------------------------- */
	if ( post_prompt_insert->
		pair_one2m_post_prompt_insert->
		one_folder_name
	&&   post_prompt_insert->
		pair_one2m_post_prompt_insert->
		many_folder_name
	&&   !post_prompt_insert->
		pair_one2m_post_prompt_insert->
		next_folder )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			"Insert Complete" /* title_string */ );

		document_close();
		exit( 0 );
	}

	/* If pair one to many is skipped */
	/* ------------------------------ */
	if ( post_prompt_insert->
		pair_one2m_post_prompt_insert->
		one_folder_name
	&&   !post_prompt_insert->
		pair_one2m_post_prompt_insert->
		many_folder_name )
	{
		system_string =
			execute_system_string_table_edit(
				TABLE_EDIT_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				folder_name,
				FRAMESET_TABLE_FRAME /* target_frame */,
				results_string,
				insert_statement_error_string,
				post_prompt_insert->
					dictionary_separate_send_string,
				post_prompt_insert->
					post_prompt_insert_input->
					appaserver_error_filename );

		goto execute_system_string;
	}

	/* If pair one to many has a next */
	/* ------------------------------ */
	if ( post_prompt_insert->
		pair_one2m_post_prompt_insert->
		next_folder )
	{
		system_string =
			execute_system_string_table_insert(
				TABLE_INSERT_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				post_prompt_insert->
					pair_one2m_post_prompt_insert->
					next_folder,
				FRAMESET_TABLE_FRAME /* target_frame */,
				results_string,
				insert_statement_error_string,
				post_prompt_insert->
					dictionary_separate_send_string,
				post_prompt_insert->
					post_prompt_insert_input->
					appaserver_error_filename );

		goto execute_system_string;
	}

	/* If left out a primary key */
	/* ------------------------- */
	if ( !post_prompt_insert->insert )
	{
		system_string =
			execute_system_string_table_insert(
				TABLE_INSERT_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				folder_name,
				FRAMESET_TABLE_FRAME /* target_frame */,
				results_string,
				insert_statement_error_string,
				post_prompt_insert->
					dictionary_separate_send_string,
				post_prompt_insert->
					post_prompt_insert_input->
					appaserver_error_filename );

		goto execute_system_string;
	}

	system_string =
		execute_system_string_table_edit(
			TABLE_EDIT_EXECUTABLE,
			session_key,
			login_name,
			role_name,
			folder_name,
			FRAMESET_TABLE_FRAME /* target_frame */,
			results_string,
			insert_statement_error_string,
			post_prompt_insert->
				dictionary_separate_send_string,
			post_prompt_insert->
				post_prompt_insert_input->
				appaserver_error_filename );

execute_system_string:

	if ( system_string && system( system_string ) ){}

	return 0;
}
