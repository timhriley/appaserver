/* $APPASERVER_HOME/src_appaserver/output_prompt_insert.c	*/
/* ------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "session.h"
#include "post_dictionary.h"
#include "prompt_insert.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	char *target_frame;
	APPASERVER_PARAMETER *appaserver_parameter;
	boolean menu_horizontal;
	PROMPT_INSERT *prompt_insert;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf(stderr,
"Usage: %s session login_name role folder target_frame post_dictionary\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	folder_name = argv[ 4 ];
	target_frame = argv[ 5 ];

	session_environment_set( application_name );
	appaserver_parameter = appaserver_parameter_new();

	prompt_insert =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		prompt_insert_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			target_frame,
			state,
			( menu_horizontal =
				frameset_menu_horizontal(
					application_name,
					login_name ) )
					/* menu_horizontal_boolean */,
			appaserver_parameter->
				appaserver_data_directory,
			post_dictionary_string_new(
				argv[ 6 ] ) );

	if ( prompt_insert->forbid )
	{
		document_quick_output( application_name );

		printf(
	"<h3>An internal error occurred. Check the system log.</h3>\n" );

		printf( "%s\n", document_close_html() );
		exit( 1 );
	}

	if ( !menu_horizontal )
	{
		document_output_content_type();
	}

	printf( "%s\n", prompt_insert->document_form_html );

	return 0;
}

