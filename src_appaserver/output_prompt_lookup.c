/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/output_prompt_lookup.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "environ.h"
#include "application.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "session.h"
#include "post_dictionary.h"
#include "prompt_lookup.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	APPASERVER_PARAMETER *appaserver_parameter;
	boolean menu_horizontal_boolean;
	POST_DICTIONARY *post_dictionary;
	PROMPT_LOOKUP *prompt_lookup;

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

	appaserver_parameter = appaserver_parameter_new();

	post_dictionary =
		post_dictionary_stdin_new(
			/* ---------------------------------- */
			/* Used when expecting a spooled file */
			/* ---------------------------------- */
			(char *)0 /* application_name */,
			(char *)0 /* upload_directory */,
			(LIST *)0 /* upload_filename_list */ );

	prompt_lookup =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		prompt_lookup_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			( menu_horizontal_boolean =
				application_menu_horizontal_boolean(
					application_name ) ),
			appaserver_parameter->
				data_directory,
			post_dictionary );

	printf( "%s\n", prompt_lookup->document_form_html );

	return 0;
}

