/* ----------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_prompt_process.c	*/
/* ----------------------------------------------------------	*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ----------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "boolean.h"
#include "appaserver_parameter.h"
#include "application.h"
#include "post_prompt_process.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *process_or_set_name;
	boolean is_drillthru;
	APPASERVER_PARAMETER *appaserver_parameter;
	POST_PROMPT_PROCESS *post_prompt_process;

	if ( argc != 7 )
	{
		fprintf(stderr,
"Usage: %s application session login role process_or_set is_drillthru_yn\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	process_or_set_name = argv[ 5 ];
	is_drillthru = ( *argv[ 6 ] == 'y' );

	appaserver_parameter = appaserver_parameter_new();

	if ( ! ( post_prompt_process =
			post_prompt_process_new(
				argc,
				argv,
				application_name,
				session_key,
				login_name,
				role_name,
				process_or_set_name,
				is_drillthru,
				appaserver_parameter->document_root,
				application_relative_source_directory(
					application_name ) ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: post_prompt_process_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( post_prompt_process->prompt_process_output_system_string )
	{
		if ( system(
			post_prompt_process->
				prompt_process_output_system_string ) ){}
	}
	else
	if ( post_prompt_process->command_line )
	{
		if ( system( post_prompt_process->command_line ) ){}
	}

	return 0;
}

