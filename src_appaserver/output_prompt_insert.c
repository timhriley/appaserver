/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/output_prompt_insert.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "session.h"
#include "application.h"
#include "post_dictionary.h"
#include "prompt_insert.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	APPASERVER_PARAMETER *appaserver_parameter;
	POST_DICTIONARY *post_dictionary;
	PROMPT_INSERT *prompt_insert;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf(stderr,
			"Usage: %s session login_name role folder\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	folder_name = argv[ 4 ];

	appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	if ( !appaserver_parameter->data_directory )
	{
		char message[ 128 ];

		sprintf(message,
			"appaserver_parameter->data_directory is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

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

	prompt_insert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		prompt_insert_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			application_menu_horizontal_boolean(
				application_name ),
			appaserver_parameter->data_directory,
			post_dictionary->original_post_dictionary );

	printf( "%s\n", prompt_insert->document_form_html );

	return 0;
}
