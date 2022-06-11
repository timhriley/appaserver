/* $APPASERVER_HOME/src_appaserver/output_prompt_process.c		*/
/* --------------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "timlib.h"
#include "environ.h"
#include "list.h"
#include "application.h"
#include "appaserver_error.h"
#include "menu.h"
#include "frameset.h"
#include "appaserver_parameter.h"
#include "post_dictionary.h"
#include "prompt_process.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *process_or_set_name;
	boolean has_drillthru;
	boolean is_drillthru;
	POST_DICTIONARY *post_dictionary;
	APPASERVER_PARAMETER *appaserver_parameter;
	PROMPT_PROCESS *prompt_process;

	application_name = environment_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf(stderr,
"Usage: %s session login role process_or_set dictionary has_drillthru_yn is_drillthru_yn\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	process_or_set_name = argv[ 4 ];

	post_dictionary =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		post_dictionary_string_new(
			argv[ 5 ] );

	has_drillthru = ( *argv[ 6 ] == 'y' );
	is_drillthru = ( *argv[ 7 ] == 'y' );

	appaserver_parameter = appaserver_parameter_new();

	prompt_process =
		prompt_process_new(
			application_name,
			session_key,
			login_name,
			role_name,
			process_or_set_name,
			has_drillthru,
			is_drillthru,
			post_dictionary,
			menu_horizontal_boolean(
				FRAMESET_PROMPT_FRAME /* current_frame */,
				FRAMESET_PROMPT_FRAME,
				frameset_menu_horizontal(
					application_name,
					login_name ) ),
			appaserver_parameter->data_directory,
			appaserver_parameter->document_root,
			application_relative_source_directory(
				application_name ) );

	return 0;
}

