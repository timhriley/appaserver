/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_appaserver/post_delete_folder_block.c		*/
/* -------------------------------------------------------------------- */
/* This process is triggered if you select the delete radio		*/
/* button on a lookup form.						*/
/*									*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "String.h"
#include "appaserver_error.h"
#include "document.h"
#include "environ.h"
#include "session.h"
#include "appaserver.h"
#include "post_dictionary.h"
#include "lookup_delete.h"

void post_delete_folder_block_state_two(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary );

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	char *state;
	POST_DICTIONARY *post_dictionary;
	SESSION_FOLDER *session_folder;

	if ( argc != 7 )
	{
		fprintf(stderr, 
		"Usage: %s application session login_name role folder state\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	folder_name = argv[ 5 ];
	state = argv[ 6 ];

	post_dictionary =
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

	session_folder =
		/* --------------------------------------------- */
		/* Sets appaserver environment and outputs argv. */
		/* Each parameter is security inspected.	 */
		/* Any error will exit(1).			 */
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

	if ( strcmp( state, "one" ) == 0 )
	{
		/* ---------------------------------------------------- */
		/* Called from post_prompt_lookup.  			*/
		/* document_content_type_output() is already executed.  */
		/* ---------------------------------------------------- */
		lookup_delete_state_one_form_output(
			LOOKUP_DELETE_EXECUTABLE,
			session_folder->application_name,
			session_folder->session_key,
			session_folder->login_name,
			session_folder->role_name,
			session_folder->folder_name,
			post_dictionary->original_post_dictionary );
	}
	else
	if ( strcmp( state, "two" ) == 0 )
	{
		document_content_type_output();

		post_delete_folder_block_state_two(
			session_folder->application_name,
			session_folder->session_key,
			session_folder->login_name,
			session_folder->role_name,
			session_folder->folder_name,
			post_dictionary->original_post_dictionary );
	}
	else
	{
		char message[ 128 ];

		document_content_type_output();

		sprintf(message, "invalid state = [%s].", state );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return 0;
}

void post_delete_folder_block_state_two(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary )
{
	LOOKUP_DELETE *lookup_delete;

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

	lookup_delete =
		lookup_delete_new(
			application_name,
			login_name,
			role_name,
			folder_name,
			original_post_dictionary );

	if ( !lookup_delete )
	{
		char message[ 128 ];

		sprintf(message,
			"lookup_delete_new(%s,%s) returned empty.",
			login_name,
			role_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_delete_state_two_execute(
		application_name,
		login_name,
		role_name,
		folder_name,
		lookup_delete->lookup_delete_input->execute_boolean,
		lookup_delete->
			query_primary_key->
			query_fetch->
			row_list,
		lookup_delete->title_string );
}

