/* $APPASERVER_HOME/library/post_table_insert.c			*/
/* ------------------------------------------------------------ */
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "String.h"
#include "appaserver.h"
#include "appaserver_library.h"
#include "appaserver_parameter.h"
#include "application.h"
#include "folder_attribute.h"
#include "role_operation.h"
#include "dictionary.h"
#include "update.h"
#include "table_insert.h"
#include "post_table_insert.h"

POST_TABLE_INSERT *post_table_insert_new(
			int argc,
			char **argv,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *target_frame )
{
	POST_TABLE_INSERT *post_table_insert;

	if ( !argc
	||   !argv
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

	post_table_insert = post_table_insert_calloc();

	return post_table_insert;
}

POST_TABLE_INSERT *post_table_insert_calloc( void )
{
	POST_TABLE_INSERT *post_table_insert;

	if ( ! ( post_table_insert =
			calloc( 1, sizeof( POST_TABLE_INSERT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_table_insert;
}
