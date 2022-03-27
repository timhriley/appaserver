/* $APPASERVER_HOME/library/post_edit_table.c				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "String.h"
#include "appaserver_library.h"
#include "appaserver_parameter.h"
#include "application.h"
#include "post_edit_table.h"

POST_EDIT_TABLE *post_edit_table_new(
			int argc,
			char **argv,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state,
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
			state );

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

