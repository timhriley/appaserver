/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_insert_table.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pair_one2m.h"
#include "table_insert.h"
#include "table_edit.h"
#include "frameset.h"
#include "appaserver_error.h"
#include "execute_system_string.h"
#include "document.h"
#include "post_table_insert.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	char *target_frame;
	POST_TABLE_INSERT *post_table_insert;
	char *insert_statement_error_string = {0};
	char *results_string;
	char *system_string;

	if ( argc != 7 )
	{
		fprintf( stderr,
	"Usage: %s application session login_name role folder target_frame\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	folder_name = argv[ 5 ];
	target_frame = argv[ 6 ];

	document_content_type_output();

	post_table_insert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_table_insert_new(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name );

	if ( post_table_insert->insert )
	{
		insert_statement_error_string =
			insert_statement_execute(
				application_name,
				post_table_insert->
					insert->
					insert_statement->
					insert_folder_statement_list,
				post_table_insert->
					insert->
					appaserver_error_filename );
	
		results_string =
			post_table_insert->
				insert->
				results_string;
	}
	else
	{
		results_string = "No Rows Inserted";
	}

	if ( post_table_insert->vertical_new_post )
	{
		post_table_insert->
		    vertical_new_post->
		    prompt_insert =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			prompt_insert_new(
				application_name,
				session_key,
				login_name,
				role_name,
				post_table_insert->
				    vertical_new_post->
				many_folder_name,
				post_table_insert->
					vertical_new_post->
					application_menu_horizontal_boolean,
				post_table_insert->
					vertical_new_post->
					data_directory,
				post_table_insert->
					vertical_new_post->
					original_post_dictionary );

		vertical_new_post_prompt_insert_create(
			post_table_insert->
				vertical_new_post->
				vertical_new_filename,
			post_table_insert->
				vertical_new_post->
				prompt_insert );
	}

	if (	post_table_insert->
			pair_one2m_post_table_insert
	&&	post_table_insert->
			pair_one2m_post_table_insert->
			next_folder_name )
	{
		system_string =
			execute_system_string_table_insert(
				TABLE_INSERT_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				post_table_insert->
					pair_one2m_post_table_insert->
					next_folder_name,
				target_frame,
				results_string,
				insert_statement_error_string,
				post_table_insert->
					dictionary_separate_send_string,
				appaserver_error_filename(
					application_name ) );

		if ( system( system_string ) ){}
	}
	else
	if (	post_table_insert->
			pair_one2m_post_table_insert
	&&	!post_table_insert->
			pair_one2m_post_table_insert->
			next_folder_name )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			POST_TABLE_INSERT_PROCESS_NAME );
	
		printf(	"%s\n",
			PAIR_ONE2M_COMPLETE_HTML );
	
		if ( results_string )
		{
			printf(	"<h2>%s</h2>\n",
				results_string );
		}

		if ( insert_statement_error_string )
		{
			printf(	"<h3>%s</h3>\n",
				insert_statement_error_string );
		}

		document_close();
	}
	else
	{
		system_string =
			execute_system_string_table_edit(
				TABLE_EDIT_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				folder_name,
				target_frame,
				results_string,
				insert_statement_error_string,
				post_table_insert->
					dictionary_separate_send_string,
				appaserver_error_filename(
					application_name ) );

		if ( system( system_string ) ){}
	}

	return 0;
}

