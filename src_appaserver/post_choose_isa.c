/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_choose_isa.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "session.h"
#include "application.h"
#include "appaserver.h"
#include "frameset.h"
#include "appaserver_error.h"
#include "security.h"
#include "table_edit.h"
#include "execute_system_string.h"
#include "post_choose_isa.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *insert_folder_name;
	char *one_folder_name;
	SESSION_FOLDER *session_folder;
	POST_CHOOSE_ISA *post_choose_isa;

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s application session login_name role insert_folder one_folder\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	insert_folder_name = argv[ 5 ];
	one_folder_name = argv[ 6 ];

	document_content_type_output();

	session_folder =
		session_folder_integrity_exit(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			insert_folder_name,
			APPASERVER_INSERT_STATE );

	post_choose_isa =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_choose_isa_new(
			session_folder->
				application_name,
			session_folder->
				session_key,
			session_folder->
				login_name,
			session_folder->
				role_name,
			session_folder->
				folder_name,
			/* ---------------------------- */
			/* Returns heap memory or datum */
			/* ---------------------------- */
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				one_folder_name /* datum */ ) );


	if ( post_choose_isa->insert )
	{
		char *insert_statement_error_string;

		insert_statement_error_string =
			insert_statement_execute(
				application_name,
				post_choose_isa->
					insert->
					insert_statement->
					insert_folder_statement_list,
				post_choose_isa->
					insert->
					appaserver_error_filename );

		post_choose_isa->execute_system_string_table_edit =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			execute_system_string_table_edit(
				TABLE_EDIT_EXECUTABLE,
				session_folder->session_key,
				session_folder->login_name,
				session_folder->role_name,
				session_folder->folder_name,
				FRAMESET_PROMPT_FRAME /* target_frame */,
				post_choose_isa->
					insert->
					result_string,
				insert_statement_error_string,
				post_choose_isa->
					dictionary_separate_send_string,
				post_choose_isa->
					post_choose_isa_input->
					appaserver_error_filename );
	}

	if ( post_choose_isa->skip_system_string )
	{
		if ( system(
			post_choose_isa->
				skip_system_string ) ){}
	}
	else
	if ( post_choose_isa->execute_system_string_table_edit )
	{
		if ( system(
			post_choose_isa->
				execute_system_string_table_edit ) ){}
	}
	else
	{
		char message[ 128 ];

		sprintf(message, "malformed configuration." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return 0;
}

