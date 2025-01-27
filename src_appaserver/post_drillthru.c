/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_drillthru.c			*/
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
#include "post_dictionary.h"
#include "post_drillthru.h"

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
	POST_DICTIONARY *post_dictionary;
	POST_DRILLTHRU *post_drillthru;

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

	post_drillthru =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_drillthru_new(
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
				/* appaserver_data_directory */,
			post_dictionary->
				original_post_dictionary );

	if ( post_drillthru->drillthru_continue->drillthru_document )
	{
		printf( "%s\n",
			post_drillthru->
				drillthru_continue->
				drillthru_document->
				document_form_html );
	}
	else
	{
		if ( !post_drillthru->system_string )
		{
			char message[ 128 ];

			sprintf(message,
				"post_drillthru->system_string is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		return
		system( post_drillthru->system_string );
	}
}

