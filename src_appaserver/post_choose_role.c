/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_choose_role.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "document.h"
#include "appaserver_error.h"
#include "choose_role.h"
#include "post_choose_role.h"

int main( int argc, char **argv )
{
	POST_CHOOSE_ROLE *post_choose_role;

	document_content_type_output();

	if ( ! ( post_choose_role =
			post_choose_role_new(
				argc,
				argv,
				CHOOSE_ROLE_DROP_DOWN_NAME ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: post_choose_role_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	appaserver_error_login_name_append_file(
		argc,
		argv,
		post_choose_role->sql_injection_escape_application_name,
		post_choose_role->sql_injection_escape_login_name );

	return system( post_choose_role->execute_system_string_menu );
}

