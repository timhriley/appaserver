/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_choose_folder.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "session.h"
#include "document.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "post_choose_folder.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *state;
	char *folder_name;
	SESSION_FOLDER *session_folder;
	APPASERVER_PARAMETER *appaserver_parameter;
	POST_CHOOSE_FOLDER *post_choose_folder;

	if ( argc != 7 )
	{
		fprintf(stderr,
		"Usage: %s application session login_name role state folder\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	state = argv[ 5 ];
	folder_name = argv[ 6 ];

	document_content_type_output();

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
			state );

	appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	post_choose_folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_choose_folder_new(
			/* ----------------------------------- */
			/* See session_folder_integrity_exit() */
			/* ----------------------------------- */
			session_folder->session->application_name,
			session_folder->session->session_key,
			session_folder->session->login_name,
			session_folder->role_name,
			session_folder->folder_name,
			session_folder->state,
			appaserver_parameter->data_directory
				/* appaserver_data_directory */ );

	if ( !post_choose_folder->drillthru_start )
	{
		char message[ 128 ];

		sprintf(message,
			"post_choose_folder_new(%s,%s) returned incomplete.",
			role_name,
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( post_choose_folder->drillthru_start->participating_boolean )
	{
		if ( !post_choose_folder->drillthru_start->drillthru_document
		||   !post_choose_folder->
			drillthru_start->
			drillthru_document->
			document_form_html )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
			"post_choose_folder->drillthru_start is incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		document_content_type_output();

		printf( "%s\n",
			post_choose_folder->
				drillthru_start->
				drillthru_document->
				document_form_html );
	}
	else
	{
		if ( !post_choose_folder->system_string )
		{
			char message[ 128 ];

			sprintf(message,
				"post_choose_folder->system_string is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		return
		system( post_choose_folder->system_string );
	}
}

