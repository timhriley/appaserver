/* ---------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_prompt_insert.c		*/
/* ---------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* ---------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "String.h"
#include "frameset.h"
#include "table_edit.h"
#include "table_insert.h"
#include "prompt_insert.h"
#include "post_prompt_insert.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	POST_PROMPT_INSERT *post_prompt_insert;
	char *send_string;
	char *system_string;

	if ( argc != 6 )
	{
		fprintf(stderr,
		"Usage: %s application session login_name role folder\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	folder_name = argv[ 5 ];

	if ( ! ( post_prompt_insert =
			post_prompt_insert_new(
				argc,
				argv,
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: post_prompt_insert_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( post_prompt_insert->insert )
	{
		post_prompt_insert->error_message_list_string =
			/* ----------------------------------------- */
			/* Returns error_message_list_string or null */
			/* ----------------------------------------- */
			insert_sql_statement_list_execute(
				post_prompt_insert->
					insert->
					insert_sql_statement_list );
	}

	send_string =
		dictionary_separate_send_string(
			post_prompt_insert->
				dictionary_separate_send_dictionary );

	if ( !post_prompt_insert->pair_one2m_post_prompt_insert
	&& ( post_prompt_insert->insert
	||   post_prompt_insert->lookup_boolean ) )
	{
		system_string =
			table_edit_output_system_string(
				TABLE_EDIT_OUTPUT_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				folder_name,
				FRAMESET_TABLE_FRAME /* target_frame */,
				post_prompt_insert->
					error_message_list_string
						/*subsub_title */,
				send_string,
				appaserver_error_filename(
					application_name ) );
	}
	else
	{
		system_string =
			table_insert_output_system_string(
				TABLE_INSERT_OUTPUT_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				(post_prompt_insert->
					pair_one2m_post_prompt_insert)
					  ? post_prompt_insert->
					       pair_one2m_post_prompt_insert->
					       many_folder_name
					  : folder_name,
				FRAMESET_TABLE_FRAME /* target_frame */,
				send_string,
				appaserver_error_filename(
					application_name ) );
	}

	if ( system( system_string ) ){}

	return 0;
}

