/* $APPASERVER_HOME/library/prompt_insert.c				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "String.h"
#include "environ.h"
#include "appaserver_library.h"
#include "appaserver.h"
#include "application.h"
#include "relation.h"
#include "prompt_insert.h"

PROMPT_INSERT *prompt_insert_calloc( void )
{
	PROMPT_INSERT *prompt_insert;

	if ( ! ( prompt_insert = calloc( 1, sizeof( PROMPT_INSERT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return prompt_insert;
}

PROMPT_INSERT *prompt_insert_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			char *state,
			boolean menu_boolean,
			char *data_directory,
			POST_DICTIONARY *post_dictionary )
{
	PROMPT_INSERT *prompt_insert = prompt_insert_calloc();

	return prompt_insert;
}

char *prompt_insert_output_system_string(
			char *executable,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *dictionary_separate_send_string,
			char *appaserver_error_filename )
{
	char system_string[ 1024 ];

	if ( !executable
	||   !login_name
	||   !session_key
	||   !folder_name
	||   !role_name
	||   !dictionary_separate_send_string
	||   !appaserver_error_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"%s %s %s %s %s \"%s\" 2>>%s",
		executable,
		login_name,
		session_key,
		folder_name,
		role_name,
		dictionary_separate_send_string,
		appaserver_error_filename );

	return strdup( system_string );
}

